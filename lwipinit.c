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
#if LWIP_IPV4 && USE_ETHERNET
  ip4_addr_t ipaddr, netmask, gw;
#endif /* LWIP_IPV4 && USE_ETHERNET */
#if USE_SLIPIF
  u8_t num_slip1 = 0;
#if LWIP_IPV4
  ip4_addr_t ipaddr_slip1, netmask_slip1, gw_slip1;
#endif
#if USE_SLIPIF > 1
  u8_t num_slip2 = 1;
#if LWIP_IPV4
  ip4_addr_t ipaddr_slip2, netmask_slip2, gw_slip2;
#endif
#endif /* USE_SLIPIF > 1 */
#endif /* USE_SLIPIF */
#if USE_DHCP || USE_AUTOIP
  err_t err;
#endif

#if USE_PPP
  const char *username = NULL, *password = NULL;
#ifdef PPP_USERNAME
  username = PPP_USERNAME;
#endif
#ifdef PPP_PASSWORD
  password = PPP_PASSWORD;
#endif
  printf("ppp_connect: COM%d\n", (int)sio_idx);
#if PPPOS_SUPPORT
  ppp_sio = sio_open(sio_idx);
  if (ppp_sio == NULL) {
    printf("sio_open error\n");
  } else {
    ppp = pppos_create(&ppp_netif, ppp_output_cb, pppLinkStatusCallback, NULL);
    if (ppp == NULL) {
      printf("pppos_create error\n");
    } else {
      ppp_set_auth(ppp, PPPAUTHTYPE_ANY, username, password);
      ppp_connect(ppp, 0);
    }
  }
#endif /* PPPOS_SUPPORT */
#endif  /* USE_PPP */

#if USE_ETHERNET
#if LWIP_IPV4
  ip4_addr_set_zero(&gw);
  ip4_addr_set_zero(&ipaddr);
  ip4_addr_set_zero(&netmask);
#if USE_ETHERNET_TCPIP
#if USE_DHCP
  printf("Starting lwIP, local interface IP is dhcp-enabled\n");
#elif USE_AUTOIP
  printf("Starting lwIP, local interface IP is autoip-enabled\n");
#else /* USE_DHCP */
  LWIP_PORT_INIT_GW(&gw);
  LWIP_PORT_INIT_IPADDR(&ipaddr);
  LWIP_PORT_INIT_NETMASK(&netmask);
  printf("Starting lwIP, local interface IP is %s\n", ip4addr_ntoa(&ipaddr));
#endif /* USE_DHCP */
#endif /* USE_ETHERNET_TCPIP */
#else /* LWIP_IPV4 */
  printf("Starting lwIP, IPv4 disable\n");
#endif /* LWIP_IPV4 */

#if LWIP_IPV4
  init_default_netif(&ipaddr, &netmask, &gw);
#else
  init_default_netif();
#endif
#if LWIP_IPV6
  netif_create_ip6_linklocal_address(netif_default, 1);
  printf("ip6 linklocal address: %s\n", ip6addr_ntoa(netif_ip6_addr(netif_default, 0)));
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_STATUS_CALLBACK
  netif_set_status_callback(netif_default, status_callback);
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(netif_default, link_callback);
#endif /* LWIP_NETIF_LINK_CALLBACK */

#if USE_ETHERNET_TCPIP
#if LWIP_AUTOIP
  autoip_set_struct(netif_default, &netif_autoip);
#endif /* LWIP_AUTOIP */
#if LWIP_DHCP
  dhcp_set_struct(netif_default, &netif_dhcp);
#endif /* LWIP_DHCP */
  netif_set_up(netif_default);
#if USE_DHCP
  err = dhcp_start(netif_default);
  LWIP_ASSERT("dhcp_start failed", err == ERR_OK);
#elif USE_AUTOIP
  err = autoip_start(netif_default);
  LWIP_ASSERT("autoip_start failed", err == ERR_OK);
#endif /* USE_DHCP */
#else /* USE_ETHERNET_TCPIP */
  /* Use ethernet for PPPoE only */
  netif.flags &= ~(NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP); /* no ARP */
  netif.flags |= NETIF_FLAG_ETHERNET; /* but pure ethernet */
#endif /* USE_ETHERNET_TCPIP */

#if USE_PPP && PPPOE_SUPPORT
  /* start PPPoE after ethernet netif is added! */
  ppp = pppoe_create(&ppp_netif, netif_default, NULL, NULL, pppLinkStatusCallback, NULL);
  if (ppp == NULL) {
    printf("pppoe_create error\n");
  } else {
    ppp_set_auth(ppp, PPPAUTHTYPE_ANY, username, password);
    ppp_connect(ppp, 0);
  }
#endif /* USE_PPP && PPPOE_SUPPORT */

#endif /* USE_ETHERNET */
#if USE_SLIPIF
#if LWIP_IPV4
#define SLIP1_ADDRS &ipaddr_slip1, &netmask_slip1, &gw_slip1,
  LWIP_PORT_INIT_SLIP1_IPADDR(&ipaddr_slip1);
  LWIP_PORT_INIT_SLIP1_GW(&gw_slip1);
  LWIP_PORT_INIT_SLIP1_NETMASK(&netmask_slip1);
  printf("Starting lwIP slipif, local interface IP is %s\n", ip4addr_ntoa(&ipaddr_slip1));
#else
#define SLIP1_ADDRS
  printf("Starting lwIP slipif\n");
#endif
#if defined(SIO_USE_COMPORT) && SIO_USE_COMPORT
  num_slip1++; /* COM ports cannot be 0-based */
#endif
  netif_add(&slipif1, SLIP1_ADDRS &num_slip1, slipif_init, ip_input);
#if !USE_ETHERNET
  netif_set_default(&slipif1);
#endif /* !USE_ETHERNET */
#if LWIP_IPV6
  netif_create_ip6_linklocal_address(&slipif1, 1);
  printf("SLIP ip6 linklocal address: %s\n", ip6addr_ntoa(netif_ip6_addr(&slipif1, 0)));
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_STATUS_CALLBACK
  netif_set_status_callback(&slipif1, status_callback);
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(&slipif1, link_callback);
#endif /* LWIP_NETIF_LINK_CALLBACK */
  netif_set_up(&slipif1);

#if USE_SLIPIF > 1
#if LWIP_IPV4
#define SLIP2_ADDRS &ipaddr_slip2, &netmask_slip2, &gw_slip2,
  LWIP_PORT_INIT_SLIP2_IPADDR(&ipaddr_slip2);
  LWIP_PORT_INIT_SLIP2_GW(&gw_slip2);
  LWIP_PORT_INIT_SLIP2_NETMASK(&netmask_slip2);
  printf("Starting lwIP SLIP if #2, local interface IP is %s\n", ip4addr_ntoa(&ipaddr_slip2));
#else
#define SLIP2_ADDRS
  printf("Starting lwIP SLIP if #2\n");
#endif
#if defined(SIO_USE_COMPORT) && SIO_USE_COMPORT
  num_slip2++; /* COM ports cannot be 0-based */
#endif
  netif_add(&slipif2, SLIP2_ADDRS &num_slip2, slipif_init, ip_input);
#if LWIP_IPV6
  netif_create_ip6_linklocal_address(&slipif1, 1);
  printf("SLIP2 ip6 linklocal address: ");
  ip6_addr_debug_print(0xFFFFFFFF & ~LWIP_DBG_HALT, netif_ip6_addr(&slipif2, 0));
  printf("\n");
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_STATUS_CALLBACK
  netif_set_status_callback(&slipif2, status_callback);
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(&slipif2, link_callback);
#endif /* LWIP_NETIF_LINK_CALLBACK */
  netif_set_up(&slipif2);
#endif /* USE_SLIPIF > 1*/
#endif /* USE_SLIPIF */
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
