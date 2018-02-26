
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netlink/netlink.h>
#include <slankdev/net/addr.h>
#include <slankdev/net/if.h>
using namespace netlink;

void route_add(netlink::Netlink& nl,
    uint32_t dst, uint8_t pref, uint32_t nhop)
{
  struct nlrtmsg {
    struct nlmsghdr hdr;
    struct rtmsg msg;
    uint8_t buf[1024];
  } req;
  memset (&req, 0, sizeof (req));
  req.hdr.nlmsg_type = 24; // add route
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_MATCH | NLM_F_ATOMIC;
  req.hdr.nlmsg_len = NLMSG_SPACE (sizeof (req.msg));
  req.msg.rtm_family = AF_INET;
  req.msg.rtm_dst_len = pref;
  req.msg.rtm_table = 0xfe;
  req.msg.rtm_protocol = RTPROT_BOOT;
  req.msg.rtm_scope = RT_SCOPE_UNIVERSE;
  req.msg.rtm_type = RTN_UNICAST;
  netlink::addattr(&req.hdr, sizeof(req), 0x0001, &dst, sizeof(dst));
  netlink::addattr(&req.hdr, sizeof(req), 0x0005, &nhop, sizeof(nhop));

  nl.send_msg(&req, req.hdr.nlmsg_len);
}

int main(int argc, char** argv)
{
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <route> <pref> <nexthop>\n", argv[0]);
    return 1;
  }

  netlink::Netlink netlink;
  uint32_t dst  = inet_addr(argv[1]);
  uint8_t  pref = atoi(argv[2]);
  uint32_t nhop = inet_addr(argv[3]);
  printf("add route %s/%u via %s\n",
      slankdev::inaddr2str(dst).c_str(), pref,
      slankdev::inaddr2str(nhop).c_str());
  route_add(netlink, dst, pref, nhop);
}


