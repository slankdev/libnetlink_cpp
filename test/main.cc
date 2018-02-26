
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netlink/netlink.h>
#include <slankdev/net/addr.h>
#include <slankdev/net/if.h>
using namespace netlink;

inline void handle_msg_route(Msghdr *hdr)
{
  const size_t len  = hdr->msg_len();
  Rtmsg* msg = reinterpret_cast<Rtmsg*>(hdr->data());

  uint8_t prefix = msg->dst_len();
  uint32_t addr;
  uint32_t psrc;
  uint32_t ifindex;
  bool is_default = false;
  int attr_len = hdr->payload();
  for (Rtattr* attr = reinterpret_cast<Rtattr*>(msg->rtm_rta());
      attr->ok(attr_len); attr=attr->next(attr_len)) {
    if      (attr->type() == 0x0001) addr    = attr->read_data<uint32_t>();
    else if (attr->type() == 0x0007) psrc    = attr->read_data<uint32_t>();
    else if (attr->type() == 0x0004) ifindex = attr->read_data<uint32_t>();
    else if (attr->type() == 0x0005) {
      psrc = attr->read_data<uint32_t>();
      is_default = true;
    }
  }

  if (msg->type() == RTN_UNICAST) {
    if (is_default) {
      printf("default via %s dev %s \n",
          slankdev::inaddr2str(psrc).c_str(),
          slankdev::if_index2name(ifindex).c_str());
    } else {
      printf("%s/%u via %s dev %s \n",
          slankdev::inaddr2str(addr).c_str(), prefix,
          slankdev::inaddr2str(psrc).c_str(),
          slankdev::if_index2name(ifindex).c_str());
    }
  }
}

inline void handle_msg(Msghdr *hdr)
{
  const uint32_t type = hdr->msg_type();
  const size_t   len  = hdr->msg_len();
  if (type==NLMSG_DONE || type==NLMSG_ERROR) {
    printf("done or error\n");
    return;
  }

  switch (type) {
    case RTM_NEWROUTE:
    case RTM_DELROUTE:
    {
      handle_msg_route(hdr);
      break;
    }
    default:
      printf("type isn't supported ");
      printf("unknown(%d)\n", hdr->msg_type());
      break;
  }
}

void dump_fib(netlink::Netlink& nl)
{
  struct nlrtmsg {
    struct nlmsghdr hdr;
    struct rtmsg msg;
    uint8_t buf[1024];
  } req;
  memset (&req, 0, sizeof (req));
  req.hdr.nlmsg_type = RTM_GETROUTE;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.hdr.nlmsg_len = NLMSG_SPACE (sizeof (req.msg));
  req.msg.rtm_family = AF_INET;
  req.msg.rtm_table = RT_TABLE_MAIN;
  req.msg.rtm_protocol = RTPROT_UNSPEC;
  nl.send_msg(&req, req.hdr.nlmsg_len);

  while (Msghdr* msghdr = nl.recv_msg()) {
    if (msghdr->msg_type() == NLMSG_DONE) {
      break;
    }
    handle_msg(msghdr);
  }
}

int main(int argc, char** argv)
{
  netlink::Netlink netlink;
  dump_fib(netlink);
}


