/****************************************************************************
 * arch/arm/src/am67/am67_pwm.h
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

#ifndef __ARCH_ARM_SRC_AM67_AM67_PWM_H
#define __ARCH_ARM_SRC_AM67_AM67_PWM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifdef CONFIG_AM67_EPWM0

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
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

int am67_epwm_init(void);

#endif /* CONFIG_AM67_EPWM0 */
#endif /* __ARCH_ARM_SRC_AM67_AM67_PWM_H */
