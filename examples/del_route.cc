
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netlink/netlink.h>
#include <slankdev/net/addr.h>
#include <slankdev/net/if.h>
using namespace netlink;

void route_del(netlink::Netlink& nl,
    uint32_t dst, uint8_t pref)
{
  struct nlrtmsg {
    struct nlmsghdr hdr;
    struct rtmsg msg;
    uint8_t buf[1024];
  } req;
  memset (&req, 0, sizeof (req));
  req.hdr.nlmsg_type = 25; // del route
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
  req.hdr.nlmsg_len = NLMSG_SPACE (sizeof (req.msg));
  req.msg.rtm_family = AF_INET;
  req.msg.rtm_dst_len = pref;
  req.msg.rtm_table = 0xfe;
  req.msg.rtm_protocol = 0x00;
  req.msg.rtm_scope = RT_SCOPE_NOWHERE;
  req.msg.rtm_type = 0x00;
  netlink::addattr(&req.hdr, sizeof(req), 0x0001, &dst, sizeof(dst));

  nl.send_msg(&req, req.hdr.nlmsg_len);
}

int main(int argc, char** argv)
{
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <route> <pref>\n", argv[0]);
    return 1;
  }

  netlink::Netlink netlink;
  uint32_t dst  = inet_addr(argv[1]);
  uint8_t  pref = atoi(argv[2]);
  printf("del route %s/%u\n", slankdev::inaddr2str(dst).c_str(), pref);
  route_del(netlink, dst, pref);
}


