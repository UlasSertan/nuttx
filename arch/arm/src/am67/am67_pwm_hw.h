/****************************************************************************
 * arch/arm/src/am67/am67_pwm_hw.h
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

#ifndef __ARCH_ARM_SRC_AM67_AM67_PWM_HW_H
#define __ARCH_ARM_SRC_AM67_AM67_PWM_HW_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/* Register offsets *********************************************************/

/* TB module */

#define AM67_PWM_EPWM_TBCTL       0x000
#define AM67_PWM_EPWM_TBSTS       0x002
#define AM67_PWM_EPWM_TBPHS       0x006
#define AM67_PWM_EPWM_TBCNT       0x008
#define AM67_PWM_EPWM_TBPRD       0x00a

/* CC module */

#define AM67_PWM_EPWM_CMPCTL      0x00e
#define AM67_PWM_EPWM_CMPA        0x012
#define AM67_PWM_EPWM_CMPB        0x014

/* AQ module */

#define AM67_PWM_EPWM_AQCTLA      0x016
#define AM67_PWM_EPWM_AQCTLB      0x018
#define AM67_PWM_EPWM_AQSFRC      0x01a
#define AM67_PWM_EPWM_AQCSFRC     0x01c

/* Register bit field definitions *******************************************/

/* Time-Base Control Register (TBCTL) */

#define AM67_PWM_EPWM_TBCTL_CTRMODE_SHIFT                   (0)
#define AM67_PWM_EPWM_TBCTL_CTRMODE_MASK                    (3u << 0)
#define AM67_PWM_EPWM_TBCTL_PHSEN_SHIFT                     (2)
#define AM67_PWM_EPWM_TBCTL_PHSEN_MASK                      (1u << 2)
#define AM67_PWM_EPWM_TBCTL_PRDLD_IMMEDIATE                 (1u << 3)
#define AM67_PWM_EPWM_TBCTL_SYNCOSEL_SHIFT                  (4)
#define AM67_PWM_EPWM_TBCTL_SYNCOSEL_MASK                   (3u << 4)
#define AM67_PWM_EPWM_TBCTL_SWFSYNC_SHIFT                   (6)
#define AM67_PWM_EPWM_TBCTL_SWFSYNC_MASK                    (1u << 6)
#define AM67_PWM_EPWM_TBCTL_HSPCLKDIV_SHIFT                 (7)
#define AM67_PWM_EPWM_TBCTL_HSPCLKDIV_MASK                  (7u << 7)
#define AM67_PWM_EPWM_TBCTL_CLKDIV_SHIFT                    (10)
#define AM67_PWM_EPWM_TBCTL_CLKDIV_MASK                     (7u << 10)
#define AM67_PWM_EPWM_TBCTL_PHSDIR_SHIFT                    (13)
#define AM67_PWM_EPWM_TBCTL_PHSDIR_MASK                     (1u << 13)
#define AM67_PWM_EPWM_TBCTL_FREE_SOFT_SHIFT                 (14)
#define AM67_PWM_EPWM_TBCTL_FREE_SOFT_MASK                  (3u << 14)

/* Counter-Compare Control Register (CMPCTL) */

#define AM67_PWM_EPWM_CMPCTL_LOADAMODE_SHIFT                (0)
#define AM67_PWM_EPWM_CMPCTL_LOADBMODE_SHIFT                (2)
#define AM67_PWM_EPWM_CMPCTL_LOADAMODE_MASK                 (3u << 0)
#define AM67_PWM_EPWM_CMPCTL_LOADBMODE_MASK                 (3u << 2)
#define AM67_PWM_EPWM_CMPCTL_SHDWAMODE_IMMEDIATE_SHIFT      (4)
#define AM67_PWM_EPWM_CMPCTL_SHDWBMODE_IMMEDIATE_SHIFT      (6)
#define AM67_PWM_EPWM_CMPCTL_SHDWAMODE_IMMEDIATE            (1u << 4)
#define AM67_PWM_EPWM_CMPCTL_SHDWBMODE_IMMEDIATE            (1u << 6)

#endif /* __ARCH_ARM_SRC_AM67_AM67_PWM_HW_H */
