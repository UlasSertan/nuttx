/****************************************************************************
 * arch/arm/src/am67/am67_pwm.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <assert.h>
#include <debug.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include <nuttx/timers/pwm.h>

#include "arm_internal.h"
#include "am67_pinmux.h"
#include "am67_pwm.h"
#include "am67_pwm_hw.h"

#ifdef CONFIG_AM67_EPWM0

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* MAIN_CTRL_MMR partition 1 lock (kick) registers */

#define AM67_MAIN_CTRL_MMR_BASE           0x00100000
#define AM67_CTRL_MMR_LOCK1_KICK0         0x5008      /* offset from base */
#define AM67_CTRL_MMR_LOCK1_KICK1         0x500c
#define AM67_CTRL_MMR_KICK0_UNLOCK_KEY    0x68ef3490
#define AM67_CTRL_MMR_KICK1_UNLOCK_KEY    0xd172bc5a
#define AM67_CTRL_MMR_KICK0_UNLOCKED      (1u << 0)   /* status: reads 1 = unlocked */

/* EPWM time-base clock gate (partition 1) */

#define AM67_CTRL_MMR_EPWM_TB_CLKEN       0x4130      /* offset from base */
#define AM67_EPWM_TB_CLKEN_EPWM0_EN       (1u << 0)
#define AM67_EPWM_TB_CLKEN_EPWM1_EN       (1u << 1)
#define AM67_EPWM_TB_CLKEN_EPWM2_EN       (1u << 2)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: am67_epwm_getreg
 *
 * Description:
 *   Get a 32-bit register value by offset
 *
 ****************************************************************************/

static inline uint32_t am67_epwm_getreg(uint32_t base, uint32_t offset)
{
  return getreg32(base + offset);
}

/****************************************************************************
 * Name: am67_epwm_putreg
 *
 * Description:
 *  Put a 32-bit register value by offset
 *
 ****************************************************************************/

static inline void am67_epwm_putreg(uint32_t base, uint32_t offset,
                                     uint32_t value)
{
  putreg32(value, base + offset);
}

/****************************************************************************
 * Name: am67_epwm_enable_register_write
 *
 * Description:
 *   Unlock partition 1 of the MAIN_CTRL_MMR (kick lock) so that the EPWM
 *   time-base clock gate register can be written.  The KICK0 status bit is
 *   read back to confirm that the partition is unlocked.
 *
 * Returned Value:
 *   Zero (OK) on success; -EIO if the partition is still locked after
 *   writing the unlock keys.
 *
 ****************************************************************************/

static int am67_epwm_enable_register_write(void)
{
  uint32_t regval = am67_epwm_getreg(AM67_MAIN_CTRL_MMR_BASE,
                                    AM67_CTRL_MMR_LOCK1_KICK0);

  if ((regval & AM67_CTRL_MMR_KICK0_UNLOCKED) == 0u)
    {
      am67_epwm_putreg(AM67_MAIN_CTRL_MMR_BASE, AM67_CTRL_MMR_LOCK1_KICK0,
                      AM67_CTRL_MMR_KICK0_UNLOCK_KEY);
      am67_epwm_putreg(AM67_MAIN_CTRL_MMR_BASE, AM67_CTRL_MMR_LOCK1_KICK1,
                      AM67_CTRL_MMR_KICK1_UNLOCK_KEY);
    }

  regval = am67_epwm_getreg(AM67_MAIN_CTRL_MMR_BASE,
                           AM67_CTRL_MMR_LOCK1_KICK0);

  if ((regval & AM67_CTRL_MMR_KICK0_UNLOCKED) == 0u)
    {
      pwmerr("ERROR: Could not unlock CTRL_MMR partition 1\n");
      return -EIO;
    }

  return OK;
}

/****************************************************************************
 * Name: am67_epwm_enable_clock
 *
 * Description:
 *   Enable the EPWM0 time-base clock in the EPWM_TB_CLKEN register.
 *   Read-modify-write to preserve the gates of the other EPWM instances
 *   (EPWM2 drives the board cooling fan).  The register is read back to
 *   verify that the gate bit stuck.
 *
 * Returned Value:
 *   Zero (OK) on success; -EIO if the clock gate did not enable.
 *
 ****************************************************************************/

static int am67_epwm_enable_clock(void)
{
  /* Read register first to preserve already existing epwm configurations */

  uint32_t regval = am67_epwm_getreg(AM67_MAIN_CTRL_MMR_BASE,
                                    AM67_CTRL_MMR_EPWM_TB_CLKEN);

  regval |= AM67_EPWM_TB_CLKEN_EPWM0_EN;

  am67_epwm_putreg(AM67_MAIN_CTRL_MMR_BASE, AM67_CTRL_MMR_EPWM_TB_CLKEN,
                  regval);

  /* Re-read the register to check for possible errors during the write */

  regval = am67_epwm_getreg(AM67_MAIN_CTRL_MMR_BASE,
                           AM67_CTRL_MMR_EPWM_TB_CLKEN);

  if ((regval & AM67_EPWM_TB_CLKEN_EPWM0_EN) == 0u)
    {
      pwmerr("ERROR: Could not enable EPWM0 clock: TB_CLKEN: 0x%08" PRIx32
             "\n", regval);
      return -EIO;
    }

  return OK;
}

/****************************************************************************
 * Name: am67_epwm_check_pid
 *
 * Description:
 *   Read the EPWM peripheral ID register and compare it against the
 *   expected value.  Verifies that the module is powered and that the
 *   base address is correct before any configuration is attempted.
 *
 * Returned Value:
 *   Zero (OK) on success; -EIO if the PID does not match.
 *
 ****************************************************************************/

static int am67_epwm_check_pid(void)
{
  uint32_t regval = am67_epwm_getreg(AM67_EPWM0_BASE,
                                    AM67_EPWM_PID_OFFSET);

  if (regval != AM67_EPWM_PID_EXPECTED)
    {
      pwmerr("ERROR: Unexpected EPWM PID: 0x%08" PRIx32
             " (expected 0x%08" PRIx32 ")\n",
             regval, (uint32_t)AM67_EPWM_PID_EXPECTED);
      return -EIO;
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: am67_epwm_init
 *
 * Description:
 *   Bring up the EPWM0 module: unlock the CTRL_MMR partition, enable the
 *   time-base clock, and verify the module is reachable by reading its
 *   peripheral ID.  Must be called before any EPWM register access.
 *
 * Assumptions:
 *   The EPWM0 device power domain must already be enabled via TISCI.
 *   NuttX has no TISCI client yet, so for now Linux must grant it before
 *   the R5F starts, e.g.:
 *     echo on > /sys/devices/platform/bus@f0000/23000000.pwm/power/control
 *   Otherwise the PID read bus-faults.  TODO: replace with a minimal
 *   NuttX TISCI client so the R5F owns the PWM power (safety).
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value from the first failing
 *   step otherwise.
 *
 ****************************************************************************/

int am67_epwm_init(void)
{
  int ret;

  ret = am67_epwm_enable_register_write();
  if (ret < 0)
    {
      return ret;
    }

  ret = am67_epwm_enable_clock();
  if (ret < 0)
    {
      return ret;
    }

  ret = am67_epwm_check_pid();
  if (ret < 0)
    {
      return ret;
    }

  am67_epwm_pinmux_init();

  return OK;
}

#endif /* CONFIG_AM67_EPWM0 */
