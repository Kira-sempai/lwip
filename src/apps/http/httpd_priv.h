#ifndef LWIP_HTTPD_PRIV_H_
#define LWIP_HTTPD_PRIV_H_

#include "lwip/pbuf.h"
#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"
#include "lwip/apps/fs.h"
#include "lwip/apps/httpd_opts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_HTTPD_DYNAMIC_HEADERS
/* The number of individual strings that comprise the headers sent before each
 * requested file.
 */
#define NUM_FILE_HDR_STRINGS 7
#define HDR_STRINGS_IDX_HTTP_STATUS           0 /* e.g. "HTTP/1.0 200 OK\r\n" */
#define HDR_STRINGS_IDX_SERVER_NAME           1 /* e.g. "Server: "HTTPD_SERVER_AGENT"\r\n" */
#define HDR_STRINGS_IDX_CONTENT_LEN_KEEPALIVE 2 /* e.g. "Content-Length: xy\r\n" and/or "Connection: keep-alive\r\n" */
#define HDR_STRINGS_IDX_CONTENT_LEN_NR        3 /* the byte count, when content-length is used */
#define HDR_STRINGS_IDX_ETAG                  4 /* e.g. "ETag: "2073041041"" */
#define HDR_STRINGS_IDX_CACHE_CONTROL_MAX_AGE 5 /* e.g. "Cache-Control: max-age=31536000\r\nExpires: Tue, 01 Jan 2030 00:00:00 GMT\r\n" */
#define HDR_STRINGS_IDX_CONTENT_TYPE          6 /* the content type (or default answer content type including default document) */

/* The dynamically generated Content-Length buffer needs space for CRLF + NULL */
#define LWIP_HTTPD_MAX_CONTENT_LEN_OFFSET 3
#ifndef LWIP_HTTPD_MAX_CONTENT_LEN_SIZE
/* The dynamically generated Content-Length buffer shall be able to work with
   ~953 MB (9 digits) */
#define LWIP_HTTPD_MAX_CONTENT_LEN_SIZE   (9 + LWIP_HTTPD_MAX_CONTENT_LEN_OFFSET)
#endif
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */

#if LWIP_HTTPD_SSI
#define HTTPD_LAST_TAG_PART 0xFFFF

enum tag_check_state {
  TAG_NONE,       /* Not processing an SSI tag */
  TAG_LEADIN,     /* Tag lead in "<!--#" being processed */
  TAG_FOUND,      /* Tag name being read, looking for lead-out start */
  TAG_LEADOUT,    /* Tag lead out "-->" being processed */
  TAG_SENDING     /* Sending tag replacement string */
};

struct http_ssi_state {
  const char *parsed;     /* Pointer to the first unparsed byte in buf. */
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
  const char *tag_started;/* Pointer to the first opening '<' of the tag. */
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG */
  const char *tag_end;    /* Pointer to char after the closing '>' of the tag. */
  u32_t parse_left; /* Number of unparsed bytes in buf. */
  u16_t tag_index;   /* Counter used by tag parsing state machine */
  u16_t tag_insert_len; /* Length of insert in string tag_insert */
#if LWIP_HTTPD_SSI_MULTIPART
  u16_t tag_part; /* Counter passed to and changed by tag insertion function to insert multiple times */
#endif /* LWIP_HTTPD_SSI_MULTIPART */
  u8_t tag_type; /* index into http_ssi_tag_desc array */
  u8_t tag_name_len; /* Length of the tag name in string tag_name */
  char tag_name[LWIP_HTTPD_MAX_TAG_NAME_LEN + 1]; /* Last tag name extracted */
  char tag_insert[LWIP_HTTPD_MAX_TAG_INSERT_LEN + 1]; /* Insert string for tag_name */
  enum tag_check_state tag_state; /* State of the tag processor */
};

struct http_ssi_tag_description {
  const char *lead_in;
  const char *lead_out;
};

#endif /* LWIP_HTTPD_SSI */

struct http_state {
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
  struct http_state *next;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  struct fs_file file_handle;
  struct fs_file *handle;
  const char *file;       /* Pointer to first unsent byte in buf. */

  struct altcp_pcb *pcb;
#if LWIP_HTTPD_SUPPORT_REQUESTLIST
  struct pbuf *req;
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */

#if LWIP_HTTPD_DYNAMIC_FILE_READ
  char *buf;        /* File read buffer. */
  int buf_len;      /* Size of file read buffer, buf. */
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
  u32_t left;       /* Number of unsent bytes in buf. */
  u8_t retries;
#if LWIP_HTTPD_SUPPORT_11_KEEPALIVE
  u8_t keepalive;
#endif /* LWIP_HTTPD_SUPPORT_11_KEEPALIVE */
#if LWIP_HTTPD_SSI
  struct http_ssi_state *ssi;
#endif /* LWIP_HTTPD_SSI */
#if LWIP_HTTPD_CGI
  char *params[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Params extracted from the request URI */
  char *param_vals[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Values for each extracted param */
#endif /* LWIP_HTTPD_CGI */
#if LWIP_HTTPD_DYNAMIC_HEADERS
  const char *hdrs[NUM_FILE_HDR_STRINGS]; /* HTTP headers to be sent. */
  char hdr_content_len[LWIP_HTTPD_MAX_CONTENT_LEN_SIZE];
  u16_t hdr_pos;     /* The position of the first unsent header byte in the
                        current string */
  u16_t hdr_index;   /* The index of the hdr string currently being sent. */
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */
#if LWIP_HTTPD_TIMING
  u32_t time_started;
#endif /* LWIP_HTTPD_TIMING */
#if LWIP_HTTPD_SUPPORT_POST
  u32_t post_content_len_left;
#if LWIP_HTTPD_POST_MANUAL_WND
  u32_t unrecved_bytes;
  u8_t no_auto_wnd;
  u8_t post_finished;
#endif /* LWIP_HTTPD_POST_MANUAL_WND */
#endif /* LWIP_HTTPD_SUPPORT_POST*/
  
  uint32_t session_id;
};

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HTTPD_PRIV_H_ */
