Import('*')


targetEnv.RegisterModule('lwip', depend=['kernel'], init=True, reset=True, run=True)

sources = Split("""
	
	src/api/err.c
	
	src/apps/http/fs.c
	src/apps/http/fs_custom_files.cc
	src/apps/http/httpd.c
	src/apps/http/httpd_memory_management.c
	
	src/core/def.c
	src/core/dns.c
	src/core/inet_chksum.c
	src/core/init.c
	src/core/ip.c
	src/core/mem.c
	src/core/memp.c
	src/core/netif.c
	src/core/pbuf.c
	src/core/stats.c
	src/core/timeouts.c
	src/core/udp.c
	src/core/raw.c
	src/core/tcp.c
	src/core/tcp_in.c
	src/core/tcp_out.c
	
	src/core/ipv4/autoip.c
	src/core/ipv4/dhcp.c
	src/core/ipv4/etharp.c
	src/core/ipv4/icmp.c
	src/core/ipv4/igmp.c
	src/core/ipv4/ip4_frag.c
	src/core/ipv4/ip4_addr.c
	src/core/ipv4/ip4.c

	src/netif/ethernet.c

	lwipinit.c

	""")

# fuer NO_SYS=0 notwendig
sources.extend([
	'src/api/tcpip.c',
	'src/core/sys.c',
	])

if 'CFG_ENABLE_FREERTOS' in targetEnv:
	sources.extend([
		'contrib/ports/freertos/sys_arch.c',
	])
else:
	sources.extend([
		'sys_arch.c',
	])
	
	

program_headers.append(File('src/include/lwip/lwip_hooks.h'))

program_sources.extend(File(sources))
program_objects.extend(targetEnv.StaticObject(sources))

targetEnv['CPPPATH'].extend( [
	 Dir('src/include'),
	 Dir('contrib/ports'),
	 
	 ] )
