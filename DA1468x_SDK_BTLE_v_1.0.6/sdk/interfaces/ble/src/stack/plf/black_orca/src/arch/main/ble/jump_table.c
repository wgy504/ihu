#include "rwip_config.h"     // RW SW configuration

#include <string.h>          // for mem* functions
#include "co_version.h"
#include "rwip.h"            // RW definitions
#include "arch.h"            // Platform architecture definition
#include "em_map_ble.h"
#include "em_map.h"

#if (BLE_EMB_PRESENT)
#include "rwble.h"           // rwble definitions
#endif //BLE_EMB_PRESENT


#if (BLE_HOST_PRESENT)
#include "rwble_hl.h"        // BLE HL definitions
#include "gapc.h"
#include "smpc.h"
#include "gattc.h"
#include "attc.h"
#include "atts.h"
#include "l2cc.h"
#endif //BLE_HOST_PRESENT

#if (DEEP_SLEEP)
#if (BT_EMB_PRESENT)
#include "ld_sleep.h"        // definitions for sleep mode
#endif //BT_EMB_PRESENT
#if (BLE_EMB_PRESENT)
#include "lld_sleep.h"       // definitions for sleep mode
#endif //BLE_EMB_PRESENT
#endif //DEEP_SLEEP

#if (BLE_EMB_PRESENT)
#include "llc.h"
#endif //BLE_EMB_PRESENT
#if (DISPLAY_SUPPORT)
#include "display.h"         // display definitions
#endif //DISPLAY_SUPPORT

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#include "plf.h"             // platform definition
#include "rf.h"              // RF definitions
#endif //BT_EMB_PRESENT || BLE_EMB_PRESENT

#if (GTL_ITF)
#include "gtl.h"
#endif //GTL_ITF

#if (HCIC_ITF)
#if (BT_EMB_PRESENT)
#include "hci.h"             // HCI definition
#elif (BLE_EMB_PRESENT)
#include "hcic.h"            // HCI definition
#endif //BT_EMB_PRESENT / BLE_EMB_PRESENT
#endif //HCIC_ITF

#if (KE_SUPPORT)
#include "ke.h"              // kernel definition
#include "ke_event.h"        // kernel event
#include "ke_timer.h"        // definitions for timer
#include "ke_mem.h"          // kernel memory manager
#endif //KE_SUPPORT

#include "dbg.h"             // debug definition

#if (BT_EMB_PRESENT)
#include "reg_btcore.h"      // bt core registers
#endif //BT_EMB_PRESENT


#if ((BLE_APP_PRESENT) || ((BLE_HOST_PRESENT && (!GTL_ITF))))
#include "app.h"
#endif //BLE_APP_PRESENT

#include "nvds.h"

#include "em_map_ble_user.h"
#include "gapm_task.h"
#include "rwip.h"
#include "l2cc_pdu.h"
#include "ea.h"


// In case of 8 links the DB heap is put in sysram.
#define RWIP_HEAP_NON_RET_SIZE_JT               RWIP_HEAP_NON_RET_SIZE_USER     // IN BYTES
#define RWIP_HEAP_ENV_SIZE_JT                   RWIP_HEAP_ENV_SIZE_USER         // IN BYTES
#define RWIP_HEAP_DB_SIZE_JT                    dg_configBLE_STACK_DB_HEAP_SIZE // IN BYTES
#define RWIP_HEAP_MSG_SIZE_JT                   RWIP_HEAP_MSG_SIZE_USER         // IN BYTES

uint32_t rwip_heap_non_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_NON_RET_SIZE_JT)];

#if (BLE_CONNECTION_MAX_USER <= 4) 
uint32_t rwip_heap_env_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_ENV_SIZE_JT)] __attribute__((section("ble_env_heap")));
uint32_t rwip_heap_msg_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_MSG_SIZE_JT)] __attribute__((section("ble_msg_heap")));
uint32_t rwip_heap_db_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_DB_SIZE_JT)]   __attribute__((section("ble_db_heap")));
#else
uint32_t rwip_heap_env_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_ENV_SIZE_JT)] __attribute__((section("ble_env_heap")));
uint32_t rwip_heap_msg_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_MSG_SIZE_JT)] __attribute__((section("ble_msg_heap")));
uint32_t rwip_heap_db_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_DB_SIZE_JT)]   __attribute__((section("ble_db_heap")));
#endif

void rf_init_func(struct rwip_rf_api *api);
void rf_reinit_func(void);
uint8_t smpc_check_param_func(struct l2cc_pdu *pdu);
void smpc_pdu_recv_func(uint8_t conidx, struct l2cc_pdu *pdu);
void lld_sleep_compensate_func(void);
void lld_sleep_compensate_func_rcx(void);

void lld_sleep_compensate_func_any(void);

void lld_sleep_init_func(void);
uint32_t lld_sleep_us_2_lpcycles_func(uint32_t us);
uint32_t lld_sleep_lpcycles_2_us_func(uint32_t lpcycles);
void uart_finish_transfers_func(void);
void UART_Handler_func(void);
void uart_init_func(uint8_t baudr, uint8_t mode );
void uart_flow_on_func(void);
void gtl_init_func(const struct rwip_eif_api* eif);
void gtl_eif_init_func(void);
void gtl_eif_read_start_func(void);
void gtl_eif_read_hdr_func(uint8_t len);
void gtl_eif_read_payl_func(uint16_t len, uint8_t* p_buf);
void gtl_eif_tx_done_func(uint8_t status);
void gtl_eif_rx_done_func(uint8_t status);
void h4tl_init_func(const struct rwip_eif_api* eif);
void h4tl_read_start_func(void);
void h4tl_read_hdr_func(uint8_t len);
void h4tl_read_payl_func(uint16_t len);
void h4tl_read_next_out_of_sync_func(void);
void h4tl_out_of_sync_func(void);
void h4tl_tx_done_func(uint8_t status);
void h4tl_rx_done_func(uint8_t status);
void ke_task_init_func(void);
void ke_timer_init_func(void);
void llm_encryption_done_func(void);
uint8_t nvds_get_func(uint8_t tag, nvds_tag_len_t * lengthPtr, uint8_t *buf);
const struct rwip_eif_api* rwip_eif_get_func(uint8_t type);
void platform_reset_func(uint32_t error);
void lld_test_stop_func(struct ea_elt_tag *elt);
struct ea_elt_tag *lld_test_mode_tx_func(struct co_buf_tx_node *txdesc, uint8_t tx_freq);
struct ea_elt_tag *lld_test_mode_rx_func(uint8_t rx_freq);
uint8_t nvds_get_func(uint8_t tag, nvds_tag_len_t * lengthPtr, uint8_t *buf);
uint8_t nvds_del_func(uint8_t tag);
uint8_t nvds_put_func(uint8_t tag, nvds_tag_len_t length, uint8_t *buf);
void prf_init_func(bool reset);
uint8_t prf_add_profile_func(struct gapm_profile_task_add_cmd * params,
                ke_task_id_t* prf_task);

void prf_create_func(uint8_t conidx);
void prf_cleanup_func(uint8_t conidx, uint8_t reason);
uint8_t nvds_init_func(uint8_t *base, uint32_t len);
void SetSystemVars_func (void);
void dbg_init_func(void);
void dbg_platform_reset_complete_func(uint32_t error);
int hci_rd_local_supp_feats_cmd_handler_func(ke_msg_id_t const msgid, void const *param,
                ke_task_id_t const dest_id, ke_task_id_t const src_id);

ke_task_id_t prf_get_id_from_task_func(ke_msg_id_t task);
ke_task_id_t prf_get_task_from_id_func(ke_msg_id_t id);

uint8_t l2cc_pdu_pack_func(struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_length,
                uint8_t *p_buffer, uint8_t conidx, uint8_t *llid);

uint8_t l2cc_pdu_unpack_func(struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_rem_len,
                const uint8_t *p_buffer, uint16_t pkt_length, uint8_t conidx, uint8_t llid);

void l2c_send_lecb_message_func(uint8_t conidx, uint16_t cid, uint16_t size);
uint16_t l2c_process_sdu_func(uint8_t conidx, struct l2cc_pdu *pdu);

int l2cc_pdu_recv_ind_handler_func(ke_msg_id_t const msgid, struct l2cc_pdu_recv_ind *ind,
                ke_task_id_t const dest_id, ke_task_id_t const src_id);

int gapc_lecb_connect_cfm_handler_func(ke_msg_id_t const msgid,
                struct gapc_lecb_connect_cfm *param, ke_task_id_t const dest_id,
                ke_task_id_t const src_id);

int atts_l2cc_pdu_recv_handler_func(uint8_t conidx, struct l2cc_pdu_recv_ind *param);

int attc_l2cc_pdu_recv_handler_func(uint8_t conidx, struct l2cc_pdu_recv_ind *param);

#if RWBLE_SW_VERSION_MAJOR >= 8
#include "lld.h"

void crypto_init_func(void);
void llm_le_adv_report_ind_func(struct co_buf_rx_desc *rxdesc);
int PK_PointMult_func(const ec_point *A, const uint8_t k[ECDH_KEY_LEN], ec_point *dst);
void llm_p256_start_func(struct ke_msg * msg);
uint8_t llm_create_p256_key_func(uint8_t state, uint8_t *A, uint8_t *priv);
int llm_p256_req_handler_func(ke_msg_id_t const msgid,
                              struct llm_p256_req const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id);
bool llc_le_length_effective_func(uint16_t conhdl);
void llc_le_length_conn_init_func(uint16_t conhdl);
void lld_data_tx_prog_func(struct lld_evt_tag *evt);
void lld_data_tx_check_func(struct lld_evt_tag *evt, struct lld_data_ind *msg);
void llc_pdu_send_func(uint16_t conhdl, uint8_t length);
#endif

void ble_stack_io_read(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
void ble_stack_io_write(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
void ble_stack_io_flow_off(void);

const uint32_t rom_func_addr_table_var[] __attribute__((section("jump_table_mem_area"))) = {
        (uint32_t) rf_init_func,                        //  0. rf_init_func_pos
        (uint32_t) rf_reinit_func,                      //  1. rf_reinit_func_pos
        (uint32_t) uart_init_func,                      //  2. uart_init_func_pos
        (uint32_t) uart_flow_on_func,                   //  3. uart_flow_on_func_pos
        (uint32_t) ble_stack_io_flow_off,               //  4. BLE stack hook for flow off
        (uint32_t) uart_finish_transfers_func,          //  5. uart_finish_transfers_func_pos
        (uint32_t) ble_stack_io_read,                   //  6. BLE stack hook for read
        (uint32_t) ble_stack_io_write,                  //  7. BLE stack hook for write
        (uint32_t) UART_Handler_func,                   //  8. UART_Handler_func_pos
        (uint32_t) gtl_init_func,                       //  9. gtl_init_func_pos
        (uint32_t) gtl_eif_init_func,                   // 10. gtl_eif_init_func_pos
        (uint32_t) gtl_eif_read_start_func,             // 11. gtl_eif_read_start_func_pos
        (uint32_t) gtl_eif_read_hdr_func,               // 12. gtl_eif_read_hdr_func_pos
        (uint32_t) gtl_eif_read_payl_func,              // 13. gtl_eif_read_payl_func_pos
        (uint32_t) gtl_eif_tx_done_func,                // 14. gtl_eif_tx_done_func_pos
        (uint32_t) gtl_eif_rx_done_func,                // 15. gtl_eif_rx_done_func_pos
        (uint32_t) h4tl_init_func,                      // 16. h4tl_init_func_pos
        (uint32_t) h4tl_read_start_func,                // 17. h4tl_read_start_func_pos
        (uint32_t) h4tl_read_hdr_func,                  // 18. h4tl_read_hdr_func_pos
        (uint32_t) h4tl_read_payl_func,                 // 19. h4tl_read_payl_func_pos
        (uint32_t) h4tl_read_next_out_of_sync_func,     // 20. h4tl_read_next_out_of_sync_func_pos
        (uint32_t) h4tl_out_of_sync_func,               // 21. h4tl_out_of_sync_func_pos
        (uint32_t) h4tl_tx_done_func,                   // 22. h4tl_tx_done_func_pos
        (uint32_t) h4tl_rx_done_func,                   // 23. h4tl_rx_done_func_pos
        (uint32_t) ke_task_init_func,                   // 24. ke_task_init_func_pos
        (uint32_t) ke_timer_init_func,                  // 25. ke_timer_init_func_pos
        (uint32_t) llm_encryption_done_func,            // 26. llm_encryption_done_func_pos

        (uint32_t) nvds_get_func,                       // 27. nvds_get_func_pos
        (uint32_t) nvds_put_func,                       // 28. nvds_put_func_pos
        (uint32_t) nvds_del_func,                       // 29. nvds_del_func_pos
        (uint32_t) nvds_init_func,                      // 30. nvds_init_func_pos
        (uint32_t) rwip_eif_get_func,                   // 31. rwip_eif_get_func_pos

        (uint32_t) platform_reset_func,                 // 32. platform_reset_func_pos
                                                        // 33. lld_sleep_compensate_func_pos
        ((dg_configUSE_LP_CLK == LP_CLK_32000) || (dg_configUSE_LP_CLK == LP_CLK_32768)) ?
                (uint32_t) lld_sleep_compensate_func :
                        ( (dg_configUSE_LP_CLK == LP_CLK_RCX) ?
                                (uint32_t) lld_sleep_compensate_func_rcx :
                                (uint32_t) lld_sleep_compensate_func_any ),
        (uint32_t) lld_sleep_init_func,                 // 34. lld_sleep_init_func_pos
        (uint32_t) lld_sleep_us_2_lpcycles_func,        // 35. lld_sleep_us_2_lpcycles_func_pos
        (uint32_t) lld_sleep_lpcycles_2_us_func,        // 36. lld_sleep_lpcycles_2_us_func_pos

        (uint32_t) lld_test_stop_func,                  // 37. lld_test_stop_func_pos
        (uint32_t) lld_test_mode_tx_func,               // 38. lld_test_mode_tx_func_pos
        (uint32_t) lld_test_mode_rx_func,               // 39. lld_test_mode_rx_func_pos
        (uint32_t) smpc_check_param_func,               // 40. smpc_check_param_func_pos
        (uint32_t) smpc_pdu_recv_func,                  // 41. smpc_pdu_recv_func_pos

        (uint32_t) prf_init_func,                       // 42. prf_init_func_pos
        (uint32_t) prf_add_profile_func,                // 43. prf_add_profile_func_pos
        (uint32_t) prf_create_func,                     // 44. prf_create_func_pos
        (uint32_t) prf_cleanup_func,                    // 45. prf_cleanup_func_pos

        (uint32_t) prf_get_id_from_task_func,           // 46. prf_get_id_from_task_func_pos
        (uint32_t) prf_get_task_from_id_func,           // 47. prf_get_task_from_id_func_pos

        (uint32_t) SetSystemVars_func,                  // 48. SetSystemVars_func_pos

        (uint32_t) dbg_init_func,                       // 49. dbg_init_func_pos
        (uint32_t) dbg_platform_reset_complete_func,    // 50. dbg_platform_reset_complete_func_pos
        (uint32_t) hci_rd_local_supp_feats_cmd_handler_func, // 51. hci_rd_local_supp_feats_cmd_handler_func_pos

        (uint32_t) l2cc_pdu_pack_func,                  // 52. l2cc_pdu_pack_func_pos
        (uint32_t) l2cc_pdu_unpack_func,                // 53. l2cc_pdu_unpack_func_pos
        (uint32_t) l2c_send_lecb_message_func,          // 54. l2c_send_lecb_message_func_pos
        (uint32_t) l2c_process_sdu_func,                // 55. l2c_process_sdu_func_pos

        (uint32_t) l2cc_pdu_recv_ind_handler_func,      // 56. l2cc_pdu_recv_ind_handler_func_pos
        (uint32_t) gapc_lecb_connect_cfm_handler_func,  // 57. gapc_lecb_connect_cfm_handler_func_pos
        (uint32_t) atts_l2cc_pdu_recv_handler_func,     // 58. atts_l2cc_pdu_recv_handler_func_pos
        (uint32_t) attc_l2cc_pdu_recv_handler_func,     // 59. attc_l2cc_pdu_recv_handler_func_pos

#if RWBLE_SW_VERSION_MAJOR >= 8
        (uint32_t) crypto_init_func,                    // 60. crypto_init_func_pos
        (uint32_t) llm_le_adv_report_ind_func,          // 61. llm_le_adv_report_ind_func_pos
        (uint32_t) PK_PointMult_func,                   // 62. PK_PointMult_func_pos
        (uint32_t) llm_p256_start_func,                 // 63. llm_p256_start_func_pos
        (uint32_t) llm_create_p256_key_func,            // 64. llm_create_p256_key_func_pos
        (uint32_t) llm_p256_req_handler_func,           // 65. llm_p256_req_handler_func_pos
        (uint32_t) llc_le_length_effective_func,        // 66. llc_le_length_effective_func_pos
        (uint32_t) llc_le_length_conn_init_func,        // 67. llc_le_length_conn_init_func_pos
        (uint32_t) lld_data_tx_prog_func,               // 68. lld_data_tx_prog_func_pos
        (uint32_t) lld_data_tx_check_func,              // 69. lld_data_tx_check_func_pos
        (uint32_t) llc_pdu_send_func,                   // 70. llc_pdu_send_func_pos
#endif
                                                        // [RWBLE_SW_VERSION_MAJOR >= 8|RWBLE_SW_VERSION_MAJOR < 8]. ...
        (uint32_t) NULL,                                // [71|60]. custom_preinit_func_pos
        (uint32_t) NULL,                                // [72|61]. custom_postinit_func_pos
        (uint32_t) NULL,                                // [73|62]. custom_appinit_func_pos
        (uint32_t) NULL,                                // [74|63]. custom_preloop_func_pos - unused
        (uint32_t) NULL,                                // [75|64]. custom_preschedule_func_pos - unused
        (uint32_t) NULL,                                // [76|65]. custom_postschedule_func_pos - unused
        (uint32_t) NULL,                                // [77|66]. custom_postschedule_async_func_pos - unused
        (uint32_t) NULL,                                // [78|67]. custom_presleepcheck_func_pos
        (uint32_t) NULL,                                // [79|68]. custom_appsleepset_func_pos - unused
        (uint32_t) NULL,                                // [80|69]. custom_postsleepcheck_func_pos - unused
        (uint32_t) NULL,                                // [81|70]. custom_presleepenter_func_pos - unused
        (uint32_t) NULL,                                // [82|71]. custom_postsleepexit_func_pos - unused
        (uint32_t) NULL,                                // [83|72]. custom_prewakeup_func_pos
        (uint32_t) NULL,                                // [84|73]. custom_postwakeup_func_pos
        (uint32_t) NULL,                                // [85|74]. custom_preidlecheck_func_pos - unused
#ifdef dg_configBLE_HOOK_PTI_MODIFY
        (uint32_t) dg_configBLE_HOOK_PTI_MODIFY,        // [86|75]. custom_pti_set_func_pos
#else
        (uint32_t) NULL,                                // [86|75]. custom_pti_set_func_pos
#endif

        (uint32_t) NULL,                                // [87|76]. GPADC_init_func_pos
        (uint32_t) NULL,                                // [88|77]. meas_precharge_freq_func_pos
        (uint32_t) NULL,                                // [89|78]. LUT_CFG_pos
        (uint32_t) NULL,                                // [90|79]. check_pll_lock_func_pos
        (uint32_t) NULL,                                // [91|80]. update_calcap_min_channel_func_pos
        (uint32_t) NULL,                                // [92|81]. update_calcap_max_channel_func_pos
        (uint32_t) NULL,                                // [93|82]. write_one_SW_LUT_entry_func_pos
        (uint32_t) NULL,                                // [94|83]. write_HW_LUT_func_pos
        (uint32_t) NULL,                                // [95|84]. clear_HW_LUT_func_pos
        (uint32_t) NULL,                                // [96|85]. update_LUT_func_pos
        (uint32_t) NULL,                                // [97|86]. save_configure_restore_func_pos
        (uint32_t) NULL,                                // [98|87]. update_calcap_ranges_func_pos
        (uint32_t) NULL,                                // [99|88]. find_initial_calcap_ranges_func_pos
        (uint32_t) NULL,                                // [100|89]. pll_vcocal_LUT_InitUpdate_func_pos
        (uint32_t) NULL,                                // [101|90]. set_rf_cal_cap_func_pos
        (uint32_t) NULL,                                // [102|91]. enable_rf_diag_irq_func_pos
        (uint32_t) NULL,                                // [103|92]. modulationGainCalibration_func_pos
        (uint32_t) NULL,                                // [104|93]. DCoffsetCalibration_func_pos
        (uint32_t) NULL,                                // [105|94]. IffCalibration_func_pos
        (uint32_t) NULL,                                // [106|95]. rf_calibration_func_pos
        (uint32_t) NULL,                                // [107|96]. get_rc16m_count_func_pos
        (uint32_t) NULL,                                // [108|97]. set_gauss_modgain_func_pos
};


const uint32_t rom_cfg_table_var[46] __attribute__((section("jump_table_mem_area"))) = {
        (uint32)TASK_GTL,                       //  0. GTL task
        (uint32)&rwip_heap_non_ret[0],          //  1. rwip_heap_non_ret_pos
        (uint32)RWIP_HEAP_NON_RET_SIZE_JT,      //  2. rwip_heap_non_ret_size
        (uint32)&rwip_heap_env_ret[0],          //  3. rwip_heap_env_pos
        (uint32)RWIP_HEAP_ENV_SIZE_JT,          //  4. rwip_heap_env_size
        (uint32)&rwip_heap_db_ret[0],           //  5. rwip_heap_db_pos
        (uint32)RWIP_HEAP_DB_SIZE_JT,           //  6. rwip_heap_db_size
        (uint32)&rwip_heap_msg_ret[0],          //  7. rwip_heap_msg_pos
        (uint32)RWIP_HEAP_MSG_SIZE_JT,          //  8. rwip_heap_msg_size
        MAX_SLEEP_DURATION_PERIODIC_WAKEUP_DEF, //  9. max_sleep_duration_periodic_wakeup_pos
        MAX_SLEEP_DURATION_EXTERNAL_WAKEUP_DEF,	// 10. max_sleep_duration_external_wakeup_pos
#if BLE_APP_PRESENT
        TASK_APP,                               // 11. app_main_task_pos
#else // BLE_HOST_PRESENT
        TASK_GTL,                               // 11.
#endif
        0x00D2,                                 // 12. man_id_pos (Dialog Semi ManId)
        50,                                     // 13. max_tl_pending_packets_adv_pos
        60,                                     // 14. max_tl_pending_packets_pos
        1,                                      // 15. ea_timer_prog_delay_pos
# ifdef RELEASE_BUILD
        1,                                      // 16. ea_clock_corr_lat_pos
# else
        4,
# endif
        2,                                      // 17. ea_be_used_dft_pos
        2,                                      // 18. start_margin_pos
        4,                                      // 19. test_mode_margin_pos
        3,                                      // 20. bw_used_slave_dft_pos
        6,                                      // 21. bw_used_adv_dft_pos
        (dg_configUSE_LP_CLK != LP_CLK_32768),  // 22. hz32000_pos (0: 32768, !0: 32000)
        2,                                      // 23. rwble_prog_latency_dft_pos
        2,                                      // 24. rwble_asap_latency_pos
        0,                                      // 25. rwble_priority_adv_ldc_pos
        0,                                      // 26. rwble_priority_scan_pos
        4,                                      // 27. rwble_priority_mconnect_pos
        4,                                      // 28. rwble_priority_sconnect_pos
        3,                                      // 29. rwble_priority_adv_hdc_pos
        5,                                      // 30. rwble_priority_init_pos
        6,                                      // 31. rwble_priority_max_pos
        10,                                     // 32. gap_lecb_cnx_max
        10,                                     // 33. gapm_scan_filter_size
        200,                                    // 34. smpc_rep_attempts_timer_def_val
        3000,                                   // 35. smpc_rep_attempts_timer_max_val
        2,                                      // 36. smpc_rep_attempts_timer_mult
        3000,                                   // 37. smpc_timeout_timer_duration
        0x0BB8,                                 // 38. att_trans_rtx
        0x10,                                   // 39. att_sec_enc_key_size
        468,                                    // 40. wakeup_boundary_var
        485,                                    // 41. lld_evt_abort_cnt_duration
        624,                                    // 42. ea_check_halfslot_boundary
        106,                                    // 43. ea_check_slot_boundary
#ifdef BLE_PROD_TEST
        (BLE_RX_BUFFER_CNT/8),                  // 44. RX interrupt threshold
#else
        (BLE_RX_BUFFER_CNT/2),                  // 44. RX interrupt threshold
#endif
        BLE_CONNECTION_MAX_USER,                // 45. nb_links_user
};

