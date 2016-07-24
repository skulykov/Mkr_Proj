#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>  //inet_addr
#include <netdb.h> 

#include <linux/netlink.h>
#include <linux/rtnetlink.h>


#include "Dbg.h"


//http://www.linuxquestions.org/questions/linux-networking-3/how-to-add-a-gateway-address-using-ioctl-in-c-in-linux-512213/
//http://publib.boulder.ibm.com/html/as400/v4r5/ic2962/info/apis/sioctl.htm#Header_25
//http://www.linuxjournal.com/article/7356
//http://www.linuxjournal.com/article/7356?page=0,4
int setDefGateway(const char * deviceName, const char * defGateway)
{
	int sockfd;
	struct rtentry rm; //http://www.freebsd.org/cgi/man.cgi?query=rtentry&sektion=9&apropos=0&manpath=FreeBSD+6.1-RELEASE
	struct sockaddr_in ic_gateway;// Gateway IP address
	int err;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		printf("socket is -1\n");
		return -1;
	}

	memset(&rm, 0, sizeof(rm));


	ic_gateway.sin_family = AF_INET;
	ic_gateway.sin_addr.s_addr = inet_addr(defGateway);
	ic_gateway.sin_port = 0;

	((struct sockaddr_in*)&rm.rt_dst)->sin_family = AF_INET;
	((struct sockaddr_in*)&rm.rt_dst)->sin_addr.s_addr = 0;
	((struct sockaddr_in*)&rm.rt_dst)->sin_port = 0;

	((struct sockaddr_in*)&rm.rt_genmask)->sin_family = AF_INET;
	((struct sockaddr_in*)&rm.rt_genmask)->sin_addr.s_addr = 0;
	((struct sockaddr_in*)&rm.rt_genmask)->sin_port = 0;

	memcpy((void *)&rm.rt_gateway, &ic_gateway, sizeof(ic_gateway));
	rm.rt_flags = RTF_UP | RTF_GATEWAY;
	if ((err = ioctl(sockfd, SIOCADDRT, &rm)) < 0)
	{
		printf("SIOCADDRT failed , ret->%d\n", err);
		return -1;
	}
	return 0;
}


//===============================================================================
//http://stackoverflow.com/questions/11788326/extract-current-route-from-netlink-message-code-attached

#define ERR_RET(x) do { perror(x); return EXIT_FAILURE; } while (0);
#define BUFFER_SIZE 4095

int  loop(int sock, struct sockaddr_nl *addr)
{
	int     received_bytes = 0;
	struct  nlmsghdr *nlh;
	char    destination_address[32];
	char    gateway_address[32];
	struct  rtmsg *route_entry;  /* This struct represent a route entry \
								 in the routing table */
	struct  rtattr *route_attribute; /* This struct contain route \
									 attributes (route type) */
	int     route_attribute_len = 0;
	char    buffer[BUFFER_SIZE];

	bzero(destination_address, sizeof(destination_address));
	bzero(gateway_address, sizeof(gateway_address));
	bzero(buffer, sizeof(buffer));

	sk_log_to_file("main_route()..enter loop\r\n");

	/* Receiving netlink socket data */
	while (1)
	{
		received_bytes = recv(sock, buffer, sizeof(buffer), 0);
		if (received_bytes < 0)
			ERR_RET("recv");
		/* cast the received buffer */
		nlh = (struct nlmsghdr *) buffer;
		/* If we received all data ---> break */
		if (nlh->nlmsg_type == NLMSG_DONE)
			break;
		/* We are just intrested in Routing information */
		if (addr->nl_groups == RTMGRP_IPV4_ROUTE)
			break;
	}
	sk_log_to_file("main_route()..enter loop_1\r\n");
	/* Reading netlink socket data */
	/* Loop through all entries */
	/* For more informations on some functions :
	* http://www.kernel.org/doc/man-pages/online/pages/man3/netlink.3.html
	* http://www.kernel.org/doc/man-pages/online/pages/man7/rtnetlink.7.html
	*/

	for (; NLMSG_OK(nlh, received_bytes); \
		nlh = NLMSG_NEXT(nlh, received_bytes))
	{
		/* Get the route data */
		route_entry = (struct rtmsg *) NLMSG_DATA(nlh);

		/* We are just intrested in main routing table */
		if (route_entry->rtm_table != RT_TABLE_MAIN)
			continue;

		/* Get attributes of route_entry */
		route_attribute = (struct rtattr *) RTM_RTA(route_entry);

		/* Get the route atttibutes len */
		route_attribute_len = RTM_PAYLOAD(nlh);
		/* Loop through all attributes */
		for (; RTA_OK(route_attribute, route_attribute_len); \
			route_attribute = RTA_NEXT(route_attribute, route_attribute_len))
		{
			/* Get the destination address */
			if (route_attribute->rta_type == RTA_DST)
			{
				inet_ntop(AF_INET, RTA_DATA(route_attribute), \
					destination_address, sizeof(destination_address));
			}
			/* Get the gateway (Next hop) */
			if (route_attribute->rta_type == RTA_GATEWAY)
			{
				inet_ntop(AF_INET, RTA_DATA(route_attribute), \
					gateway_address, sizeof(gateway_address));
			}
		}

		/* Now we can dump the routing attributes */
		if (nlh->nlmsg_type == RTM_DELROUTE)
			sk_log_to_file( "Deleting route to destination --> %s and gateway %s\n", \
				destination_address, gateway_address);
		if (nlh->nlmsg_type == RTM_NEWROUTE)
			sk_log_to_file("Adding route to destination --> %s and gateway %s\n", \
				destination_address, gateway_address);
	}

	return 0;
}

int main_route()
{
	int sock = -1;
	struct sockaddr_nl addr;

	/* Zeroing addr */
	bzero(&addr, sizeof(addr));
	sk_log_to_file("main_route()..\r\n");
	if ((sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
		ERR_RET("socket");

	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_IPV4_ROUTE;

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		ERR_RET("bind");

	while (1)
	{
		loop(sock, &addr);
		sleep(10);
	}

	/* Close socket */
	close(sock);

	return 0;
}

//=============================================================
//https://oroboro.com/linux-routing-tables-in-c/
bool addNullRoute(long host)
{
	// create the control socket.
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	struct rtentry route;
	memset(&route, 0, sizeof(route));

	// set the gateway to 0.
	struct sockaddr_in *addr = (struct sockaddr_in *)&route.rt_gateway;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = 0;

	// set the host we are rejecting. 
	addr = (struct sockaddr_in*) &route.rt_dst;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(host);

	// Set the mask. In this case we are using 255.255.255.255, to block a single
	// IP. But you could use a less restrictive mask to block a range of IPs. 
	// To block and entire C block you would use 255.255.255.0, or 0x00FFFFFFF
	addr = (struct sockaddr_in*) &route.rt_genmask;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = 0xFFFFFFFF;

	// These flags mean: this route is created "up", or active
	// The blocked entity is a "host" as opposed to a "gateway"
	// The packets should be rejected. On BSD there is a flag RTF_BLACKHOLE
	// that causes packets to be dropped silently. We would use that if Linux
	// had it. RTF_REJECT will cause the network interface to signal that the 
	// packets are being actively rejected.
	route.rt_flags = RTF_UP | RTF_HOST | RTF_REJECT;
	route.rt_metric = 0;

	// this is where the magic happens..
	if (ioctl(fd, SIOCADDRT, &route))
	{
		close(fd);
		return false;
	}

	// remember to close the socket lest you leak handles.
	close(fd);
	return true;
}

bool delNullRoute(long host)
{
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	struct rtentry route;
	memset(&route, 0, sizeof(route));

	struct sockaddr_in *addr = (struct sockaddr_in *)&route.rt_gateway;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = 0;

	addr = (struct sockaddr_in*) &route.rt_dst;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(host);

	addr = (struct sockaddr_in*) &route.rt_genmask;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = 0xFFFFFFFF;

	route.rt_flags = RTF_UP | RTF_HOST | RTF_REJECT;
	route.rt_metric = 0;

	// this time we are deleting the route:
	if (ioctl(fd, SIOCDELRT, &route))
	{
		close(fd);
		return false;
	}

	close(fd);
	return true;
}
#if 0
bool syncNullRoutes(const KxVector<long>& hostList)
{
	// hostlist contains the complete list of remote IPs we want to ban.
	// IPs on this list that are not already banned will get banned.
	// IPs that are banned that are not on this list will get unbanned.

	// read the route table from procfs. 
	KxTokBuf routeTable;
	KxfPath path("/proc/net/route");
	if (!path.readFile(routeTable))
	{
		return false;
	}

	KxVector<long> hl = hostList;
	KxVector<long> ex;
	hl.sort();

	// parse the route table to see which routes already exist.
	const char* line;
	KxTokBuf lineBuf;
	while ((line = routeTable.getToken("\n", "\r\t ")))
	{
		// consider only rows that affect all interfaces, since our ban 
		// routes all work like that.
		if (*line != '*') continue;
		lineBuf.tokenize(line + 1);

		u32 vals[10];
		u32 idx = 0;
		const char* tok;
		while ((tok = lineBuf.getToken(" \t", " \t")))
		{
			vals[idx++] = strtol(tok, NULL, 16);
			if (idx >= 10) break;
		}

		// at this point, each column in the row has been parsed into vals.
		// offset 2, is the flags field. Offset 0 is the remote IP. 
		if (vals[2] == (RTF_UP | RTF_HOST | RTF_REJECT))
		{
			long ip = htonl(vals[0]);
			if (hl.contains(ip))
			{
				// route exists in hostList, and in route table. Add to ex
				ex.insert(ip);
			}
			else {
				// route does not exist in hostList, remove from route table.
				delNullRoute(ip);
			}
		}
	}

	// add in all routes that don't exist in route table.
	ex.sort();
	for (u32 i = 0; i < hl.size(); i++)
	{
		long ip = hl[i];
		if (ex.contains(ip))
			addNullRoute(ip);
	}
	return true;
}
#endif



//******************************************************************************
//******************************************************************************
#define IF_NAMESIZE 32

#pragma pack(2)
// Structure for sending the request
typedef struct
{
	struct nlmsghdr nlMsgHdr;
	struct rtmsg rtMsg;
	char buf[1024];
}route_request;

// Structure for storing routes
struct RouteInfo
{
	unsigned long dstAddr;
	unsigned long mask;
	unsigned long gateWay;
	unsigned long flags;
	unsigned long srcAddr;
	unsigned char proto;
	unsigned long metrics;  // svk added
	char ifName[IF_NAMESIZE];
};

// Function for accessing interface name
int ifname(int, char *);
//http://linux-spawn.blogspot.ca/2009/12/reading-routing-table-using-c-program.html
int main_get_route_info(/*int argc, char *argv[]*/)
{
	int route_sock, i, j;
	route_request *request = (route_request *)malloc(sizeof(route_request));
	int retValue = -1, nbytes = 0, reply_len = 0;
	char reply_ptr[1024];
	ssize_t counter = 1024;
	int count = 0;
	struct rtmsg *rtp;
	struct rtattr *rtap;
	struct nlmsghdr *nlp;
	int rtl;
	struct RouteInfo route[24];
	char* buf = reply_ptr;
	unsigned long bufsize;
	/*
	if (argc > 1)
	{
		if (getuid() && geteuid())
		{
			sk_log_to_file("Oops!\n Need SuperUser rights!\n");
		}
		exit(1);
	}
	*/
	sk_log_to_file("enter main_get_route_info..\r\n");
	route_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

	bzero(request, sizeof(route_request));

	// Fill in the NETLINK header
	request->nlMsgHdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
	request->nlMsgHdr.nlmsg_type = RTM_GETROUTE;
	request->nlMsgHdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;

	// set the routing message header
	request->rtMsg.rtm_family = AF_INET;
	request->rtMsg.rtm_table = 254;

	// Send routing request
	if ((retValue = send(route_sock, request, sizeof(route_request), 0)) < 0)
	{
		sk_log_to_file("enter main_get_route_info..Error send\r\n");
		exit(1);
	}

	for (;;)
	{
		if (counter < sizeof(struct nlmsghdr))
		{
			sk_log_to_file("Routing table is bigger than 1024\n");
			exit(1);
		}

		nbytes = recv(route_sock, &reply_ptr[reply_len], counter, 0);

		if (nbytes < 0)
		{
			sk_log_to_file("Error in recv\n");
			break;
		}

		if (nbytes == 0)
			sk_log_to_file("EOF in netlink\n");

		nlp = (struct nlmsghdr*)(&reply_ptr[reply_len]);

		if (nlp->nlmsg_type == NLMSG_DONE)
		{
			// All data has been received.
			// Truncate the reply to exclude this message,
			// i.e. do not increase reply_len.
			break;
		}

		if (nlp->nlmsg_type == NLMSG_ERROR)
		{
			sk_log_to_file("Error in msg\n");
			exit(1);
		}

		reply_len += nbytes;
		counter -= nbytes;
	}

	/*======================================================*/
	bufsize = reply_len;
	// string to hold content of the route
	// table (i.e. one entry)
	unsigned int flags;

	// outer loop: loops thru all the NETLINK
	// headers that also include the route entry
	// header
	nlp = (struct nlmsghdr *) buf;

	for (i = -1; NLMSG_OK(nlp, bufsize); nlp = NLMSG_NEXT(nlp, bufsize))
	{
		// get route entry header
		rtp = (struct rtmsg *) NLMSG_DATA(nlp);
		// we are only concerned about the
		// tableId route table
		if (rtp->rtm_table != 254)
			continue;
		i++;
		// init all the strings
		bzero(&route[i], sizeof(struct RouteInfo));
		flags = rtp->rtm_flags;
		route[i].proto = rtp->rtm_protocol;

		// inner loop: loop thru all the attributes of
		// one route entry
		rtap = (struct rtattr *) RTM_RTA(rtp);
		rtl = RTM_PAYLOAD(nlp);
		for (; RTA_OK(rtap, rtl); rtap = RTA_NEXT(rtap, rtl))
		{
			switch (rtap->rta_type)
			{
				// destination IPv4 address
			case RTA_DST:
				count = 32 - rtp->rtm_dst_len;

				route[i].dstAddr = *(unsigned long *)RTA_DATA(rtap);

				route[i].mask = 0xffffffff;
				for (; count != 0; count--)
					route[i].mask = route[i].mask << 1;

				//sk_log_to_file("dst:%s \tmask:0x%x \t",inet_ntoa(route[i].dstAddr), route[i].mask);
				break;
			case RTA_GATEWAY:
				route[i].gateWay = *(unsigned long *)RTA_DATA(rtap);
				//sk_log_to_file("gw:%s\t",inet_ntoa(route[i].gateWay));
				break;
			case RTA_PREFSRC:
				route[i].srcAddr = *(unsigned long *)RTA_DATA(rtap);
				//sk_log_to_file("src:%s\t", inet_ntoa(route[i].srcAddr));
				break;
				// unique ID associated with the network
				// interface
			case RTA_OIF:
				ifname(*((int *)RTA_DATA(rtap)), route[i].ifName);
				//sk_log_to_file( "ifname %s\n", route[i].ifName);
				break;
			case RTA_METRICS:
				route[i].metrics = *(unsigned long *)RTA_DATA(rtap);
				break;
			default:
				break;
			}

		}
		//set Flags

		//[TODO]: UP hardcoded?!
		route[i].flags |= RTF_UP;
		if (route[i].gateWay != 0)
			route[i].flags |= RTF_GATEWAY;
		if (route[i].mask == 0xFFFFFFFF)
			route[i].flags |= RTF_HOST;
	}

	// Print the route records
	sk_log_to_file("Destination\tGateway \tNetmask \tflags \tMetrics \tIfname \n");
	sk_log_to_file("-----------\t------- \t--------\t------\t------\t------ \n");
	for (j = 0; j <= i; j++)
	{
		struct in_addr addr;
		addr.s_addr = route[j].dstAddr;
		sk_log_to_file("%s \t0x%08x\t0x%08x \t%d \t%d \t%s\n",
			inet_ntoa(addr),
			ntohl(route[j].gateWay),
			route[j].mask,
			route[j].flags,
			route[j].metrics,
			route[j].ifName);
	}
	return 0;
}

/*--------------------------------------------------------------
* To get the name of the interface provided the interface index
*--------------------------------------------------------------*/
int ifname(int if_index, char *ifName)
{
	int fd, retVal = -1;
	struct sockaddr_in *sin;
	struct ifreq ifr;


	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1)
	{
		sk_log_to_file("ifname(..) socket..\r\n");
		exit(1);
	}

	ifr.ifr_ifindex = if_index;

	if (ioctl(fd, SIOCGIFNAME, &ifr, sizeof(ifr)))
	{
		sk_log_to_file("ifname(..) ioctl..\r\n");
		exit(1);
	}

	strcpy(ifName, ifr.ifr_name);
	return 1;
}