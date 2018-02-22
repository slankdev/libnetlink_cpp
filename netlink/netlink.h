
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <slankdev/socketfd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

namespace netlink {

class netlink {
 public:

#if 0
  void add_route();
  void del_route();
#endif

  void dump_route();
  netlink() { sock_.socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE); }

 private:
  slankdev::socketfd sock_;
}; /* class netlink */

} /* namespace netlink */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <net/if.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <slankdev/socketfd.h>
#include <slankdev/util.h>
#include <netlink/debug.h>
#define NETLINK_BUFFER_SIZE 4096

using slankdev::depth_fprintf_pusher;
using slankdev::depth_fprintf;

struct nlrtmsg {
  struct nlmsghdr hdr;
  struct rtmsg msg;
  uint8_t buf[1024];

  // void dump(FILE* fp) const
  // {
  //   using slankdev::depth_fprintf;
  //   using slankdev::depth_fprintf_pusher;
  //   depth_fprintf(fp, "hdr: {\n");
  //   {
  //     depth_fprintf_pusher p;
  //     print_nlmsghdr(fp, &hdr);
  //   }
  //   depth_fprintf(fp, "}\n");
  //   depth_fprintf(fp, "msg: {\n");
  //   {
  //     depth_fprintf_pusher p;
  //     print_rtmsg(fp, &msg);
  //   }
  //   depth_fprintf(fp, "}\n");
  // }
};


void netlink_request_route_dump(int nlsock)
{
  struct nlrtmsg req;

  /* create request message */
  memset (&req, 0, sizeof (req));
  req.hdr.nlmsg_type = RTM_GETROUTE;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.hdr.nlmsg_len = NLMSG_SPACE (sizeof (req.msg));
  req.msg.rtm_family = AF_INET;
  req.msg.rtm_table = RT_TABLE_MAIN;
  req.msg.rtm_protocol = RTPROT_UNSPEC;

  int ret = send (nlsock, &req, req.hdr.nlmsg_len, 0);
  if (ret < 0) {
    printf("fail\n");
    exit(1);
  }
}


void netlink_handle_msg_route(struct nlmsghdr *hdr)
{
  struct rtmsg* msg = (struct rtmsg *) NLMSG_DATA (hdr);
  print_rtmsg(stdout, msg);

  int attr_len = RTM_PAYLOAD(hdr);
  size_t i=0;
  for (struct rtattr* attr = (struct rtattr *)RTM_RTA(msg);
      RTA_OK(attr, attr_len); attr = RTA_NEXT(attr, attr_len)) {

    depth_fprintf(stdout, "attr[%zd]: {\n", i);
    {
      depth_fprintf_pusher p;
      print_rtattr(stdout, attr);
    }
    depth_fprintf(stdout, "}\n");
    i++;
  }
}

inline void netlink_msghdr_dump(FILE* fp,
          const struct nlmsghdr* msghdr)
{
  fprintf(fp, "netlink: message: type: %hu len: %lu, "
          "flags: %#x seq: %lu pid: %lu\n",
          (unsigned short) msghdr->nlmsg_type,
          (unsigned long) msghdr->nlmsg_len,
          (unsigned short) msghdr->nlmsg_flags,
          (unsigned long) msghdr->nlmsg_seq,
          (unsigned long) msghdr->nlmsg_pid);
}

inline void netlink_handle_msg (struct nlmsghdr *msghdr)
{
  /*
   * switch based on nlmsg_type of the message we just received
   * man 7 rtnetlink
   */
  switch (msghdr->nlmsg_type) {

    case RTM_NEWROUTE:
    case RTM_DELROUTE:
      depth_fprintf(stdout, "msg: {\n");
      {
        depth_fprintf_pusher p;
        netlink_handle_msg_route(msghdr);
      }
      depth_fprintf(stdout, "}\n");
      break;

    case RTM_NEWADDR:
    case RTM_DELADDR:
    case RTM_NEWNEIGH:
    case RTM_DELNEIGH:
    case RTM_NEWLINK:
    case RTM_DELLINK:
      break; // no support

    case NLMSG_DONE : printf("NLMSG_DONE \n"); break;
    case NLMSG_ERROR: printf("NLMSG_ERROR\n"); break;
    default: printf("unknown(%d)\n", msghdr->nlmsg_type); break;
  }
}


void netlink_read_until_done(int nlsock)
{
  char buf[NETLINK_BUFFER_SIZE];
  memset (buf, 0, sizeof (buf));
  char* start = buf;
  int len = 0;

  while (true) {
    size_t recvlen = recv(nlsock, start, sizeof(buf) - len, 0);
    len += recvlen;

    struct nlmsghdr* msghdr = (struct nlmsghdr *) buf;
    for (; NLMSG_OK (msghdr, len); msghdr = NLMSG_NEXT (msghdr, len)) {
      netlink_handle_msg (msghdr);
    }

    if (len == 0) break;
    if (len > 0) {
      printf ("netlink: shifting the data left: %d bytes\n", len);
      memmove(buf, msghdr, len);
      start = buf + len;
    } else start = buf;
  }
}

namespace netlink {

void netlink::dump_route()
{
  int nlsock = sock_.get_fd();
  netlink_request_route_dump(nlsock);
  netlink_read_until_done(nlsock);
}

} /* namespace netlink */

