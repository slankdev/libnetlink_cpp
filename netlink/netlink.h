
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <slankdev/socketfd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>


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
#define NETLINK_BUFFER_SIZE 10000

using slankdev::depth_fprintf_pusher;
using slankdev::depth_fprintf;

struct nlrtmsg {
  struct nlmsghdr hdr;
  struct rtmsg msg;
  uint8_t buf[1024];
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


namespace netlink {

class Rtattr : public ::rtattr {
 public:

  void dump(FILE* fp) const
  {
    const ::rtattr* attr = reinterpret_cast<const ::rtattr*>(this);
    print_rtattr(fp, attr);
  }

  bool ok(size_t len) const
  {
    const ::rtattr* attr = reinterpret_cast<const ::rtattr*>(this);
    return RTA_OK(attr, len);
  }

  Rtattr* next(size_t len) const
  {
    const ::rtattr* attr = reinterpret_cast<const ::rtattr*>(this);
    ::rtattr* next = RTA_NEXT(attr, len);
    return reinterpret_cast<Rtattr*>(next);
  }

}; /* class Rtattr */


class Rtmsg : public ::rtmsg {
 public:

  void dump(FILE* fp) const
  {
    const ::rtmsg* msg = reinterpret_cast<const ::rtmsg*>(this);
    print_rtmsg(fp, msg);
  }

  Rtattr* rtm_rta() const
  {
    const ::rtmsg* msg = reinterpret_cast<const ::rtmsg*>(this);
    ::rtattr* attr = RTM_RTA(msg);
    return reinterpret_cast<Rtattr*>(attr);
  }

}; /* class Rtmsg */


class Msghdr : public ::nlmsghdr {
 public:
  size_t msg_len() const { return this->nlmsg_len; }
  uint32_t msg_type() const { return this->nlmsg_type; }

  bool ok(size_t len) const
  {
    const nlmsghdr* curr = reinterpret_cast<const nlmsghdr*>(this);
    bool ret = NLMSG_OK(curr, len);
    return ret;
  }

  Msghdr* next(size_t len) const
  {
    const nlmsghdr* curr = reinterpret_cast<const nlmsghdr*>(this);
    nlmsghdr* next = NLMSG_NEXT(curr, len);
    return reinterpret_cast<Msghdr*>(next);
  }

  void* data() const
  {
    const nlmsghdr* curr = reinterpret_cast<const nlmsghdr*>(this);
    uint8_t* ptr = reinterpret_cast<uint8_t*>(NLMSG_DATA(curr));
    return ptr;
  }

  size_t payload() const
  {
    const nlmsghdr* curr = reinterpret_cast<const nlmsghdr*>(this);
    size_t ret = RTM_PAYLOAD(curr);
    return ret;
  }

  void dump(FILE* fp) const
  {
    const nlmsghdr* hdr = reinterpret_cast<const nlmsghdr*>(this);
    print_nlmsghdr(fp, hdr);
  }

}; /* class msghdr */

void handle_msg_route(Msghdr *hdr)
{
  Rtmsg* msg = reinterpret_cast<Rtmsg*>(hdr->data());
  msg->dump(stdout);

  int attr_len = hdr->payload();
  for (Rtattr* attr = reinterpret_cast<Rtattr*>(msg->rtm_rta());
      attr->ok(attr_len); attr=attr->next(attr_len)) {

    depth_fprintf(stdout, "attr: {\n");
    {
      depth_fprintf_pusher p;
      attr->dump(stdout);
    }
    depth_fprintf(stdout, "}\n");
  }
}

inline void handle_msg(Msghdr *msghdr)
{
  switch (msghdr->msg_type()) {

    case RTM_NEWROUTE:
    case RTM_DELROUTE:
      depth_fprintf(stdout, "raw: {\n");
      {
        depth_fprintf_pusher p;
        handle_msg_route(msghdr);
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

class netlink {
 public:

  void read_until_done() // TODO: delete
  {
    int nlsock = sock_.get_fd();
    char buf[NETLINK_BUFFER_SIZE];
    memset (buf, 0, sizeof (buf));
    char* start = buf;
    int len = 0;

    while (true) {
      size_t recvlen = recv(nlsock, start, sizeof(buf) - len, 0);
      len += recvlen;

      Msghdr* msghdr = reinterpret_cast<Msghdr*>(buf);
      for (; msghdr->ok(len); msghdr = msghdr->next(len)) {
        handle_msg(msghdr);
      }

      if (len == 0) break;
      if (len > 0) {
        printf ("netlink: shifting the data left: %d bytes\n", len);
        memmove(buf, msghdr, len);
        start = buf + len;
      } else start = buf;
    }
  }

  void dump_route()
  {
    int nlsock = sock_.get_fd();
    netlink_request_route_dump(nlsock);
    read_until_done();
  }

  void dump_fib()
  {
    int nlsock = sock_.get_fd();
    netlink_request_route_dump(nlsock);

    uint8_t buf[NETLINK_BUFFER_SIZE];
    uint8_t* start = buf;

    size_t len = sock_.recv(start, sizeof(buf)-len, 0);
    for ( Msghdr* msghdr = reinterpret_cast<Msghdr*>(buf);
          msghdr->ok(len); msghdr = msghdr->next(len)) {

      depth_fprintf(stdout, "msg : {\n");
      {
        depth_fprintf_pusher p;
        depth_fprintf(stdout, "hdr: { \n");
        {
          depth_fprintf_pusher p;
          msghdr->dump(stdout);
        }
        depth_fprintf(stdout, "}\n");
        depth_fprintf(stdout, "len   : %zd\n", len);
        depth_fprintf(stdout, "data : {\n");
        {
          depth_fprintf_pusher p;
          handle_msg(msghdr);
        }
        depth_fprintf(stdout, "}\n");
      }
      depth_fprintf(stdout, "}\n");
      depth_fprintf(stdout, "\n");

    }
  }

  netlink() { sock_.socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE); }

 private:
  slankdev::socketfd sock_;
}; /* class netlink */

} /* namespace netlink */


