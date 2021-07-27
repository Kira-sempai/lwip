/**
 * lwIP options file
 */

#include <config.h>
#include <ptrcompat.h>
#include <kernel/assert.h>
#include <osAlloc.h>
#include <stdlib.h>
#include <hardware/mcu.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memt_calloc (size_t, size_t);
void* memt_malloc (size_t);
void  memt_free (void*);

#ifdef __cplusplus
}
#endif
// --- Memory ---


#define mem_clib_free memt_free
#define mem_clib_malloc memt_malloc
#define mem_clib_calloc memt_calloc

/*
  ------------------------------------
  -------------- NO SYS --------------
  ------------------------------------
*/
#ifdef CFG_THREADED_LWIP
#define NO_SYS 0
#else
#define NO_SYS 1
#endif

#define SYS_LIGHTWEIGHT_PROT (!NO_SYS)


/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/

#define MEM_LIBC_MALLOC 1
#define MEMP_MEM_MALLOC 1
//#define mem_free osFree
//#define mem_malloc osAlloc
//#define mem_calloc osCAlloc

/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

#define MEM_SIZE                        16384

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/

#define MEMP_NUM_RAW_PCB                30

#define MEMP_NUM_TCP_PCB                1
#define MEMP_NUM_TCP_PCB_LISTEN         1
#define MEMP_NUM_TCP_SEG                8

#define PBUF_POOL_SIZE                  8 // with NO_SYS=0, might need to increase this to 32 to avoid deadlocks


/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/

#define LWIP_DHCP 1

/**
 * LWIP_DHCP_DOES_ACD_CHECK==1: Perform address conflict detection on the dhcp address.
 */
#define LWIP_DHCP_DOES_ACD_CHECK        0

#define LWIP_DHCP_BOOTP_FILE 0

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
#define LWIP_DNS 1

/** DNS maximum number of entries to maintain locally. */
#define DNS_TABLE_SIZE 2

/** DNS maximum host name length supported in the name table. */
#define DNS_MAX_NAME_LENGTH             128

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/

#define LWIP_UDP 1


/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
#ifdef CFG_MODULE_NETWORK
#define LWIP_TCP 1
#else
#define LWIP_TCP 0
#endif

#define TCP_MSS                         1460


#if defined(MCU_GROUP_IS_STM32F1)
#define TCP_SND_BUF						(6*TCP_MSS)
#elif defined(MCU_GROUP_IS_STM32F2)
#define TCP_SND_BUF						(6*TCP_MSS)
#elif defined(MCU_GROUP_IS_STM32F7)
#define TCP_SND_BUF						(6*TCP_MSS)
#endif

//#define TCP_SND_QUEUELEN                20

#define TCP_LISTEN_BACKLOG              1

#ifdef CFG_WIFI_API
#define LWIP_ALTCP 1
#endif

#define TCP_OVERSIZE                    0

#define TCP_TMR_INTERVAL                5
#define TCP_SLOW_INTERVAL               500

/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/

#define LWIP_NETIF_STATUS_CALLBACK 1

#define LWIP_NETIF_LINK_CALLBACK 1

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
#define LWIP_NETCONN         (!NO_SYS)

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
#define LWIP_SOCKET          0

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
#define LWIP_STATS              1
#define LWIP_STATS_DISPLAY      0

#if LWIP_STATS
#define LINK_STATS              1
#define IP_STATS                1
#define ICMP_STATS              1
#define IGMP_STATS              0
#define IPFRAG_STATS            0
#define UDP_STATS               0
#define TCP_STATS               1
#define MEM_STATS               0
#define MEMP_STATS              0
#define PBUF_STATS              0
#define SYS_STATS               0
#endif /* LWIP_STATS */

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0
#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0


/*
   ---------------------------------------
   ---------- Hook options ---------------
   ---------------------------------------
*/

#define LWIP_HOOK_FILENAME <lwip/lwip_hooks.h>

/*
   ---------------------------------------
   ---------- HTTP options ---------------
   ---------------------------------------
*/

#if defined(MCU_GROUP_IS_STM32F1)
// buf size divided by 2
#define HTTP_MEMUSE_LEVEL_1 (8*1024)
// buf size divided by 4
#define HTTP_MEMUSE_LEVEL_2 (12*1024)

#define HTTP_MEM_RESERVE (4*1024)

#elif defined(MCU_GROUP_IS_STM32F2)
// buf size divided by 2
#define HTTP_MEMUSE_LEVEL_1 (12*1024)
// buf size divided by 4
#define HTTP_MEMUSE_LEVEL_2 (16*1024)

#define HTTP_MEM_RESERVE (8*1024)

#elif defined(MCU_GROUP_IS_STM32F7)
// buf size divided by 2
#define HTTP_MEMUSE_LEVEL_1 (12*1024)
// buf size divided by 4
#define HTTP_MEMUSE_LEVEL_2 (16*1024)

#define HTTP_MEM_RESERVE (6*1024)

#else
#error "MCU is not defined"
#endif


/*------------------- FS OPTIONS -------------------*/

/** Set this to 1 and provide the functions:
 * - "int fs_open_custom(struct fs_file *file, const char *name)"
 *    Called first for every opened file to allow opening files
 *    that are not included in fsdata(_custom).c
 * - "void fs_close_custom(struct fs_file *file)"
 *    Called to free resources allocated by fs_open_custom().
 */
#define LWIP_HTTPD_CUSTOM_FILES       1

/** LWIP_HTTPD_FS_ASYNC_READ==1: support asynchronous read operations
 * (fs_read_async returns FS_READ_DELAYED and calls a callback when finished).
 */
#define LWIP_HTTPD_FS_ASYNC_READ       1

/** Set this to 1 if you want to include code that creates HTTP headers
 * at runtime. Default is off: HTTP headers are then created statically
 * by the makefsdata tool. Static headers mean smaller code size, but
 * the (readonly) fsdata will grow a bit as every file includes the HTTP
 * header. */
#define LWIP_HTTPD_DYNAMIC_HEADERS 1

/** Maximum retries before the connection is aborted/closed.
 * - number of times pcb->poll is called -> default is 4*500ms = 2s;
 * - reset when pcb->sent is called
 */
#define HTTPD_MAX_RETRIES                   10

/** The poll delay is X*500ms */
#define HTTPD_POLL_INTERVAL                 4

/** Priority for tcp pcbs created by HTTPD (very low by default).
 *  Lower priorities get killed first when running out of memory.
 */
#define HTTPD_TCP_PRIO                      TCP_PRIO_MAX

/** Set this to 1 on platforms where strnstr is not available */
#define LWIP_HTTPD_STRNSTR_PRIVATE          1

/** Set this to 0 to drop support for HTTP/0.9 clients (to save some bytes) */
#define LWIP_HTTPD_SUPPORT_V09              0

/** Set this to 1 to enable HTTP/1.1 persistent connections.
 * ATTENTION: If the generated file system includes HTTP headers, these must
 * include the "Connection: keep-alive" header (pass argument "-11" to makefsdata).
 */
#define LWIP_HTTPD_SUPPORT_11_KEEPALIVE     0

/** Set this to 1 to support HTTP request coming in in multiple packets/pbufs */
#define LWIP_HTTPD_SUPPORT_REQUESTLIST      1

#if LWIP_HTTPD_SUPPORT_REQUESTLIST
/** Number of rx pbufs to enqueue to parse an incoming request (up to the first
    newline) */
#define LWIP_HTTPD_REQ_QUEUELEN             20
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */

/** Set this to 0 to not send the SSI tag (default is on, so the tag will
 * be sent in the HTML page */
#define LWIP_HTTPD_SSI_INCLUDE_TAG           0

/* By default, the httpd is limited to send 2*pcb->mss to keep resource usage low
   when http is not an important protocol in the device. */
#define HTTPD_LIMIT_SENDING_TO_2MSS 0

#define HTTP_IS_DATA_VOLATILE(hs) TCP_WRITE_FLAG_COPY

#define HTTP_IS_HDR_VOLATILE(hs, ptr) TCP_WRITE_FLAG_COPY

/* Define this to a function that returns the maximum amount of data to enqueue.*/
#define HTTPD_MAX_WRITE_LEN(pcb) httpGetMaxWriteLen(pcb)

#define LWIP_HTTPD_FILE_STATE         1
#define LWIP_HTTPD_FILE_EXTENSION     0
#define FS_FILE_EXTENSION_T_DEFINED
typedef struct fs_pextension fs_file_extension;


#define HTTPD_ADDITIONAL_CONTENT_TYPES {"gz", HTTP_HDR_HTMLZ}

/** Set this to 1 to support fs_read() to dynamically read file data.
 * Without this (default=off), only one-block files are supported,
 * and the contents must be ready after fs_open().
 */
#define LWIP_HTTPD_DYNAMIC_FILE_READ  1


#ifdef SIMULATION
#include "lwipopts_sim.h"
#else
#include <hardware/mcu.h>


/** Set this to 1 to support SSI (Server-Side-Includes) */
#define LWIP_HTTPD_SSI            0

/** Set this to 1 to support CGI */
#define LWIP_HTTPD_CGI            0
/** Set this to 1 to support HTTP POST */
#define LWIP_HTTPD_SUPPORT_POST   1

// this block is only used if NO_SYS==0
#if NO_SYS == 0
//#define DEFAULT_THREAD_STACKSIZE 0x40
//#define DEFAULT_THREAD_PRIO  30
#endif

/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/
//#define LWIP_DEBUG

#ifdef LWIP_DEBUG
#define LWIP_DBG_MIN_LEVEL         LWIP_DBG_LEVEL_ALL

//#define TCP_RST_DEBUG LWIP_DBG_ON
//#define UDP_DEBUG LWIP_DBG_ON
//#define MEMP_DEBUG LWIP_DBG_ON
//#define PBUF_DEBUG LWIP_DBG_ON
//#define SYS_DEBUG LWIP_DBG_ON
//#define TCP_INPUT_DEBUG LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG LWIP_DBG_ON
//#define TCP_DEBUG LWIP_DBG_ON
//#define TIMERS_DEBUG LWIP_DBG_OFF
//#define DHCP_DEBUG LWIP_DBG_ON
//#define TCP_QLEN_DEBUG      LWIP_DBG_ON
#define HTTPD_DEBUG         LWIP_DBG_ON
#endif

#include <stdio.h>

#define LWIP_RAND _rand


#include <platform.h>
#define SYS_ARCH_DECL_PROTECT(x) CRITICAL_STORE
#define SYS_ARCH_PROTECT(x)      CRITICAL_START()
#define SYS_ARCH_UNPROTECT(x)    CRITICAL_END()

#endif //SIMULATION
