#ifndef __ARCH_ARM_SRC_AM67_TISCI_H
#define __ARCH_ARM_SRC_AM67_TISCI_H


#include <nuttx/config.h>
#include <stdint.h>
#include <nuttx/compiler.h>



#define TISCI_MSG_PAYLOAD_MAX           44u


#define SEC_PROXY_TARGET_BASE           0x4d000000u
#define SEC_PROXY_RT_BASE               0x4a600000u
#define SEC_PROXY_THREAD_SIZE           0x00001000u


#define TISCI_MSG_SET_DEVICE            0x0200u
#define TISCI_MSG_GET_DEVICE            0x0201u
#define TISCI_MSG_SET_DEVICE_RESETS     0x0202u
#define TISCI_MSG_GET_DEVICE_MULTIPLE   0x0204u


/* TISCI Device Flags */

#define TISCI_MSG_FLAG_DEVICE_WAKE_ENABLED   (1u << 8)  /* Configure as wake source */
#define TISCI_MSG_FLAG_DEVICE_RESET_ISO      (1u << 9)  /* Enable reset isolation */
#define TISCI_MSG_FLAG_DEVICE_EXCLUSIVE      (1u << 10) /* Claim device exclusively */

/* TISCI Device Software States (for TISCI_MSG_SET_DEVICE) */

#define TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF  0u     /* Turn device off when possible */
#define TISCI_MSG_VALUE_DEVICE_SW_STATE_RETENTION 1u     /* Disable device, keep retention */
#define TISCI_MSG_VALUE_DEVICE_SW_STATE_ON        2u     /* Turn device on for usage */

/* TISCI Device Hardware States (from TISCI_MSG_GET_DEVICE response) */

#define TISCI_MSG_VALUE_DEVICE_HW_STATE_OFF       0u     /* Device is hardware off */
#define TISCI_MSG_VALUE_DEVICE_HW_STATE_ON        1u     /* Device is hardware on */
#define TISCI_MSG_VALUE_DEVICE_HW_STATE_TRANS     2u     /* Device is in state transition */

/* TISCI Host IDs */

#define TISCI_HOST_ID_MAIN_0_R5_0       37u
#define TISCI_HOST_ID_MAIN_0_R5_1       38u

/* Secure Proxy Thread Allocation: MAIN_0_R5_0 */

#define MAIN_0_R5_0_RESPONSE_THRD_ID    4u
#define MAIN_0_R5_0_WRITE_THRD_ID       5u
#define MAIN_0_R5_0_RESPONSE_MAX_MSG    6u
#define MAIN_0_R5_0_WRITE_MAX_MSG       5u
#define MAIN_0_R5_0_INTR                64u

/* Secure Proxy Thread Allocation: MAIN_0_R5_1 */

#define MAIN_0_R5_1_RESPONSE_THRD_ID    6u
#define MAIN_0_R5_1_WRITE_THRD_ID       7u
#define MAIN_0_R5_1_RESPONSE_MAX_MSG    6u
#define MAIN_0_R5_1_WRITE_MAX_MSG       5u
#define MAIN_0_R5_1_INTR                65u


#define MAIN_0_R5_0_TX_ADDR             (SEC_PROXY_TARGET_BASE + \
                                         (MAIN_0_R5_0_WRITE_THRD_ID * \
                                          SEC_PROXY_THREAD_SIZE))

#define MAIN_0_R5_0_RX_ADDR             (SEC_PROXY_TARGET_BASE + \
                                         (MAIN_0_R5_0_RESPONSE_THRD_ID * \
                                          SEC_PROXY_THREAD_SIZE))

#define MAIN_0_R5_1_TX_ADDR             (SEC_PROXY_TARGET_BASE + \
                                         (MAIN_0_R5_1_WRITE_THRD_ID * \
                                          SEC_PROXY_THREAD_SIZE))

#define MAIN_0_R5_1_RX_ADDR             (SEC_PROXY_TARGET_BASE + \
                                         (MAIN_0_R5_1_RESPONSE_THRD_ID * \
                                          SEC_PROXY_THREAD_SIZE))


/* TISCI Device IDs */

#define J722S_DEV_MAIN_GPIOMUX_INTROUTER0          3u
#define J722S_DEV_WKUP_MCU_GPIOMUX_INTROUTER0      5u
#define J722S_DEV_ECAP0                            51u
#define J722S_DEV_ECAP1                            52u
#define J722S_DEV_ECAP2                            53u
#define J722S_DEV_GPIO0                            77u
#define J722S_DEV_GPIO1                            78u
#define J722S_DEV_MCU_GPIO0                        79u
#define J722S_DEV_EPWM0                            86u
#define J722S_DEV_EPWM1                            87u
#define J722S_DEV_MCU_I2C0                         106u
#define J722S_DEV_WKUP_I2C0                        107u
#define J722S_DEV_WKUP_UART0                       114u
#define J722S_DEV_MCU_MCSPI0                       147u
#define J722S_DEV_UART1                            152u
#define J722S_DEV_MCU_MCAN0                        188u
#define J722S_DEV_MCASP0                           190u
#define J722S_DEV_OLDI_TX_CORE0                    234u
#define J722S_DEV_OLDI_TX_CORE1                    235u

/* Clock IDs for MAIN Peripherals */

#define J722S_DEV_UART1_FCLK_CLK                     0u
#define J722S_DEV_UART1_VBUSP_CLK                    5u

#define J722S_DEV_MCASP0_AUX_CLK                     0u
#define J722S_DEV_MCASP0_AHCLKX_CLK                  1u

#define J722S_DEV_EPWM0_VBUSP_CLK                    0u
#define J722S_DEV_EPWM1_VBUSP_CLK                    0u

#define J722S_DEV_ECAP0_VBUSP_CLK                    0u
#define J722S_DEV_ECAP1_VBUSP_CLK                    0u
#define J722S_DEV_ECAP2_VBUSP_CLK                    0u

#define J722S_DEV_GPIO0_MMR_CLK                      0u
#define J722S_DEV_GPIO1_MMR_CLK                      0u

/* Clock IDs for MCU / WKUP Domain Peripherals */

#define J722S_DEV_MCU_MCSPI0_CLKSPIREF_CLK           0u
#define J722S_DEV_MCU_MCSPI0_VBUSP_CLK               5u

#define J722S_DEV_MCU_MCAN0_MCANSS_CCLK_CLK          1u
#define J722S_DEV_MCU_MCAN0_MCANSS_HCLK_CLK          6u

#define J722S_DEV_WKUP_UART0_FCLK_CLK                0u
#define J722S_DEV_WKUP_UART0_VBUSP_CLK               3u

#define J722S_DEV_MCU_GPIO0_MMR_CLK                  0u

#define J722S_DEV_MCU_I2C0_CLK                       0u
#define J722S_DEV_WKUP_I2C0_CLK                      0u



#define TISCI_MSG_SET_CLOCK                         0x0100u
#define TISCI_MSG_GET_CLOCK                         0x0101u
#define TISCI_MSG_SET_CLOCK_PARENT                  0x0102u
#define TISCI_MSG_GET_CLOCK_PARENT                  0x0103u
#define TISCI_MSG_GET_NUM_CLOCK_PARENTS             0x0104u
#define TISCI_MSG_SET_FREQ                          0x010cu
#define TISCI_MSG_QUERY_FREQ                        0x010du
#define TISCI_MSG_GET_FREQ                          0x010eu


/* TISCI Clock Software States */
#define TISCI_MSG_VALUE_CLOCK_SW_STATE_UNREQ        0u
#define TISCI_MSG_VALUE_CLOCK_SW_STATE_AUTO         1u
#define TISCI_MSG_VALUE_CLOCK_SW_STATE_REQ          2u

/* TISCI Clock Hardware States */
#define TISCI_MSG_VALUE_CLOCK_HW_STATE_NOT_READY    0u
#define TISCI_MSG_VALUE_CLOCK_HW_STATE_READY        1u

/* TISCI Clock Flags */
#define TISCI_MSG_FLAG_CLOCK_ALLOW_SSC              (1u << 8)
#define TISCI_MSG_FLAG_CLOCK_ALLOW_FREQ_CHANGE      (1u << 9)
#define TISCI_MSG_FLAG_CLOCK_INPUT_TERM             (1u << 10)
#define TISCI_MSG_FLAG_CLOCK_SSC_ACTIVE             (1u << 11)


/* Response flags */

#define TISCI_MSG_FLAG_RESERVED0                    (1u << 0)
#define TISCI_MSG_FLAG_AOP                          (1u << 1)    /* Must request ACK before sending most of the time with this flag */
#define TISCI_MSG_FLAG_REQ_NOTFWD2DM                (1u << 3)

/* Request flags */

#define TISCI_MSG_FLAG_ACK                          (1u << 1)    /* If this flag is not set it is interpreted as NAK */


begin_packed struct tisci_header_s
{
  uint16_t type;
  uint8_t  host;   /* Must contain valid SoC Host ID */
  uint8_t  seq;
  uint32_t flags;
} end_packed_struct;

begin_packed struct tisci_sec_header_s
{
  uint16_t integ_check;
  uint16_t rsvd;  /* Init to zero always, reserved */
} end_packed_struct;

/* Used only in secured transport */
begin_packed struct tisci_sec_msg_s
{
  struct tisci_sec_header_s   sec_header;
  struct tisci_header_s       header;
  uint8_t                     payload[TISCI_MSG_PAYLOAD_MAX];
} end_packed_struct;

begin_packed struct tisci_msg_s
{
  struct tisci_header_s       header;
  uint8_t                     payload[TISCI_MSG_PAYLOAD_MAX];
} end_packed_struct;

begin_packed struct tisci_msg_set_device_req_s
{
  struct tisci_header_s       hdr;
  uint32_t                    id;
  uint32_t                    reserved;
  uint8_t                     state;
} end_packed_struct;

begin_packed struct tisci_msg_set_device_resp_s
{
  struct tisci_header_s       hdr;
} end_packed_struct;

begin_packed struct tisci_msg_get_device_req_s
{
  struct tisci_header_s       hdr;
  uint32_t                    id;
} end_packed_struct;

begin_packed struct tisci_msg_get_device_resp_s
{
  struct tisci_header_s       hdr;
  uint32_t                    context_lost_cnt;
  uint32_t                    resets;
  uint8_t                     programmed_state;
  uint8_t                     current_state;
} end_packed_struct;


begin_packed struct tisci_msg_get_device_multiple_req_s
{
  struct tisci_header_s       hdr;
  uint16_t                    start_device_id;
} end_packed_struct;

begin_packed struct tisci_msg_get_device_multiple_resp_s
{
  struct tisci_header_s       header;
  uint16_t                    count;
  uint16_t                    remaining;
  uint32_t                    device_state_bitmap;
} end_packed_struct;

begin_packed struct tisci_msg_set_device_resets_req_s
{
  struct tisci_header_s header;
  uint32_t              id;
  uint32_t              resets;
} end_packed_struct;

begin_packed struct tisci_msg_set_device_resets_resp_s
{
  struct tisci_header_s header;
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_SET_CLOCK (0x0100)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_set_clock_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint8_t               clk;
  uint8_t               state;
  uint32_t              clk32;
} end_packed_struct;

begin_packed struct tisci_msg_set_clock_resp_s
{
  struct tisci_header_s hdr;
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_GET_CLOCK (0x0101)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_get_clock_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint8_t               clk;
  uint32_t              clk32;
} end_packed_struct;

begin_packed struct tisci_msg_get_clock_resp_s
{
  struct tisci_header_s hdr;
  uint8_t               programmed_state;
  uint8_t               current_state;
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_SET_CLOCK_PARENT (0x0102)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_set_clock_parent_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint8_t               clk;
  uint32_t              clk32;
  uint8_t               parent;
  uint32_t              parent32;
} end_packed_struct;

begin_packed struct tisci_msg_set_clock_parent_resp_s
{
  struct tisci_header_s hdr;
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_GET_CLOCK_PARENT (0x0103)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_get_clock_parent_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint8_t               clk;
  uint32_t              clk32;
} end_packed_struct;

begin_packed struct tisci_msg_get_clock_parent_resp_s
{
  struct tisci_header_s hdr;
  uint8_t               parent;
  uint32_t              parent32;
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_GET_NUM_CLOCK_PARENTS (0x0104)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_get_num_clock_parents_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint8_t               clk;
  uint32_t              clk32;
} end_packed_struct;

begin_packed struct tisci_msg_get_num_clock_parents_resp_s
{
  struct tisci_header_s hdr;
  uint8_t               num_parents;
  uint32_t              num_parents32;
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_SET_FREQ (0x010c)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_set_freq_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint64_t              min_freq_hz;
  uint64_t              target_freq_hz;
  uint64_t              max_freq_hz;
  uint8_t               clk;
  uint32_t              clk32;
} end_packed_struct;

begin_packed struct tisci_msg_set_freq_resp_s
{
  struct tisci_header_s hdr;
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_QUERY_FREQ (0x010d)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_query_freq_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint64_t              min_freq_hz;
  uint64_t              target_freq_hz;
  uint64_t              max_freq_hz;
  uint8_t               clk;
  uint32_t              clk32;
} end_packed_struct;

begin_packed struct tisci_msg_query_freq_resp_s
{
  struct tisci_header_s hdr;
  uint64_t              freq_hz;  /* Added! (This was cut off in your last paste) */
} end_packed_struct;

/* -------------------------------------------------------------------------
 * TISCI_MSG_GET_FREQ (0x010e)
 * ------------------------------------------------------------------------- */
begin_packed struct tisci_msg_get_freq_req_s
{
  struct tisci_header_s hdr;
  uint32_t              device;
  uint8_t               clk;
  uint32_t              clk32;
} end_packed_struct;

begin_packed struct tisci_msg_get_freq_resp_s
{
  struct tisci_header_s hdr;
  uint64_t              freq_hz;
} end_packed_struct;

#endif /* __ARCH_ARM_SRC_AM67_TISCI_H */
