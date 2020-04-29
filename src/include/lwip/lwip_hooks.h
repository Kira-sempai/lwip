/*
 * lwip_hooks.h
 *
 *  Created on: 14 ���. 2017 �.
 *      Author: dmitry
 */

#ifndef SRC_SHARED_LWIP_SUBMODULE_SRC_INCLUDE_LWIP_LWIP_HOOKS_H_
#define SRC_SHARED_LWIP_SUBMODULE_SRC_INCLUDE_LWIP_LWIP_HOOKS_H_


#include <lwip/altcp.h>

u16_t httpGetMaxWriteLen(struct altcp_pcb *pcb);
void httpPrioriyConnClear(struct altcp_pcb *pcb);

#ifdef __cplusplus
extern "C" {
#endif

char * getETagHeader(fs_file_extension * const pextension);
void setCookieSessionID(fs_file_extension * const pextension, const uint32_t session_id);

#ifdef __cplusplus
}
#endif

#endif /* SRC_SHARED_LWIP_SUBMODULE_SRC_INCLUDE_LWIP_LWIP_HOOKS_H_ */
