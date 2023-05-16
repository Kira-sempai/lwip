#include <lwip/init.h>
#include <lwip/tcpip.h>
#include <lwip/opt.h>
#include <lwip/timeouts.h>

#if !NO_SYS
static sys_sem_t init_sem;
#endif

#if LWIP_NETIF_LINK_CALLBACK
static void
link_callback(struct netif *state_netif)
{
  if (netif_is_link_up(state_netif)) {
    printf("link_callback==UP\r\n");
  } else {
    printf("link_callback==DOWN\r\n");
  }
}
#endif /* LWIP_NETIF_LINK_CALLBACK */

/* This function initializes all network interfaces */
static void
test_netif_init(void)
{

}

/* This function initializes applications */
static void
apps_init(void)
{
#if LWIP_DNS_APP && LWIP_DNS
  /* wait until the netif is up (for dhcp, autoip or ppp) */
  sys_timeout(5000, dns_dorequest, NULL);
#endif /* LWIP_DNS_APP && LWIP_DNS */

#if LWIP_CHARGEN_APP && LWIP_SOCKET
  chargen_init();
#endif /* LWIP_CHARGEN_APP && LWIP_SOCKET */

#if LWIP_PING_APP && LWIP_RAW && LWIP_ICMP
  ping_init(&netif_default->gw);
#endif /* LWIP_PING_APP && LWIP_RAW && LWIP_ICMP */

#if LWIP_NETBIOS_APP && LWIP_UDP
  netbiosns_init();
#ifndef NETBIOS_LWIP_NAME
#if LWIP_NETIF_HOSTNAME
  netbiosns_set_name(netif_default->hostname);
#else
  netbiosns_set_name("NETBIOSLWIPDEV");
#endif
#endif
#endif /* LWIP_NETBIOS_APP && LWIP_UDP */

#if LWIP_HTTPD_APP && LWIP_TCP
#ifdef LWIP_HTTPD_APP_NETCONN
  http_server_netconn_init();
#else /* LWIP_HTTPD_APP_NETCONN */
#if defined(LWIP_HTTPD_EXAMPLE_CUSTOMFILES) && LWIP_HTTPD_EXAMPLE_CUSTOMFILES && defined(LWIP_HTTPD_EXAMPLE_CUSTOMFILES_ROOTDIR)
  fs_ex_init(LWIP_HTTPD_EXAMPLE_CUSTOMFILES_ROOTDIR);
#endif
  httpd_init();
#if defined(LWIP_HTTPD_EXAMPLE_SSI_SIMPLE) && LWIP_HTTPD_EXAMPLE_SSI_SIMPLE
  ssi_ex_init();
#endif
#if defined(LWIP_HTTPD_EXAMPLE_CGI_SIMPLE) && LWIP_HTTPD_EXAMPLE_CGI_SIMPLE
  cgi_ex_init();
#endif
#if defined(LWIP_HTTPD_EXAMPLE_HTTPS) && LWIP_HTTPD_EXAMPLE_HTTPS
  https_ex_init();
#endif
#endif /* LWIP_HTTPD_APP_NETCONN */
#endif /* LWIP_HTTPD_APP && LWIP_TCP */

#if LWIP_NETIO_APP && LWIP_TCP
  netio_init();
#endif /* LWIP_NETIO_APP && LWIP_TCP */

#if LWIP_RTP_APP && LWIP_SOCKET && LWIP_IGMP
  rtp_init();
#endif /* LWIP_RTP_APP && LWIP_SOCKET && LWIP_IGMP */

#if LWIP_SHELL_APP && LWIP_NETCONN
  shell_init();
#endif /* LWIP_SHELL_APP && LWIP_NETCONN */
#if LWIP_TCPECHO_APP
#if LWIP_NETCONN && defined(LWIP_TCPECHO_APP_NETCONN)
  tcpecho_init();
#else /* LWIP_NETCONN && defined(LWIP_TCPECHO_APP_NETCONN) */
  tcpecho_raw_init();
#endif
#endif /* LWIP_TCPECHO_APP && LWIP_NETCONN */
#if LWIP_UDPECHO_APP && LWIP_NETCONN
  udpecho_init();
#endif /* LWIP_UDPECHO_APP && LWIP_NETCONN */
#if LWIP_SOCKET_EXAMPLES_APP && LWIP_SOCKET
  socket_examples_init();
#endif /* LWIP_SOCKET_EXAMPLES_APP && LWIP_SOCKET */
#if LWIP_MDNS_APP
  mdns_example_init();
#endif
#if LWIP_SNMP_APP
  snmp_example_init();
#endif
#if LWIP_SNTP_APP
  sntp_example_init();
#endif
#if LWIP_TFTP_APP
  tftp_example_init_server();
#endif
#if LWIP_TFTP_CLIENT_APP
  tftp_example_init_client();
#endif
#if LWIP_LWIPERF_APP
  lwiperf_example_init();
#endif
#if LWIP_MQTT_APP
  mqtt_example_init();
#endif

#ifdef LWIP_APP_INIT
  LWIP_APP_INIT();
#endif
}

/* This function initializes this lwIP test. When NO_SYS=1, this is done in
 * the main_loop context (there is no other one), when NO_SYS=0, this is done
 * in the tcpip_thread context */
static void
test_init(void * arg)
{ /* remove compiler warning */
#if NO_SYS
  LWIP_UNUSED_ARG(arg);
#else /* NO_SYS */
  sys_sem_t *init_sem;
  LWIP_ASSERT("arg != NULL", arg != NULL);
  init_sem = (sys_sem_t*)arg;
#endif /* NO_SYS */

  /* init randomizer again (seed per thread) */
//  srand((unsigned int)time(NULL));

  /* init network interfaces */
  test_netif_init();

  /* init apps */
  apps_init();

#if !NO_SYS
  sys_sem_signal(init_sem);
#endif /* !NO_SYS */
}

void lwipInit(void)
{
	// initialize TCP/IP
#if NO_SYS
	// single-threaded version
	lwip_init();
	test_init(NULL);
#else
	// multi-threaded version
	err_t err;
	err = sys_sem_new(&init_sem, 0);
	LWIP_ASSERT("failed to create init_sem", err == ERR_OK);
	LWIP_UNUSED_ARG(err);
	tcpip_init(test_init, &init_sem);
	/* we have to wait for initialization to finish before
	 * calling update_adapter()! */
	sys_sem_wait(&init_sem);
	sys_sem_free(&init_sem);
#endif

#if (LWIP_SOCKET || LWIP_NETCONN) && LWIP_NETCONN_SEM_PER_THREAD
	netconn_thread_init();
#endif
}

void lwipReset(void)
{
#if NO_SYS
	sys_restart_timeouts();
#endif
}

void lwipRun(void)
{
#if NO_SYS
	sys_check_timeouts();
#endif
}
