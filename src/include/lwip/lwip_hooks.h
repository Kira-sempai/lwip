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
char * getETagHeader(const void * const pextension);



#endif /* SRC_SHARED_LWIP_SUBMODULE_SRC_INCLUDE_LWIP_LWIP_HOOKS_H_ */
