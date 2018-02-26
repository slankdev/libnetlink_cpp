
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


namespace netlink {

class Rtattr : ::rtattr {
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

  uint8_t* data() const
  {
    uint8_t* ret = reinterpret_cast<uint8_t*>(RTA_DATA(this));
    return ret;
  }

  size_t payload() const { return RTA_PAYLOAD(this); }

  uint32_t type() const { return rta_type; }
  size_t   len() const { return rta_len; }

  Rtattr* next(size_t len) const
  {
    const ::rtattr* attr = reinterpret_cast<const ::rtattr*>(this);
    ::rtattr* next = RTA_NEXT(attr, len);
    return reinterpret_cast<Rtattr*>(next);
  }

}; /* class Rtattr */

class Rtmsg : ::rtmsg {
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

  uint8_t  family  () const { return rtm_family  ; }
  uint8_t  dst_len () const { return rtm_dst_len ; }
  uint8_t  src_len () const { return rtm_src_len ; }
  uint8_t  tos     () const { return rtm_tos     ; }
  uint8_t  table   () const { return rtm_table   ; }
  uint8_t  protocol() const { return rtm_protocol; }
  uint8_t  scope   () const { return rtm_scope   ; }
  uint8_t  type    () const { return rtm_type    ; }
  uint32_t flags   () const { return rtm_flags   ; }

}; /* class Rtmsg */

class Msghdr : ::nlmsghdr {
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

  void summary() const
  {
    uint32_t type = msg_type();
    if (type == NLMSG_DONE) {
      printf("type=NLMSG_DONE\n");
      return;
    }
    if (type == NLMSG_ERROR) {
      printf("type=NLMSG_ERROR\n");
      return;
    }

    printf("[%s] len=%zd ", RTM_2STR(type), msg_len());
    switch (type) {
      case RTM_NEWROUTE:
      case RTM_DELROUTE:
      {
        Rtmsg* msg = reinterpret_cast<Rtmsg*>(this->data());
        printf("(%s)", RTN_2STR(msg->type()));
        printf("(%s)", RTPROT_2STR(msg->protocol()));
        printf("\n");
        slankdev::hexdump(stdout, this, msg_len());
#if 0
        int len = this->payload();
        size_t attr_cnt = 0;
        for (Rtattr* attr = reinterpret_cast<Rtattr*>(msg->rtm_rta());
            attr->ok(len); attr=attr->next(len) ) {
          attr_cnt ++;
          printf("{");
          printf("%s:", IFLA_2STR(attr->type()));
          size_t data_len = attr->len();
          uint8_t* data_ptr = attr->data();
          switch (attr->type()) {
            case IFLA_MTU:
              printf("%d", *(int*)data_ptr);
              break;
            case IFLA_ADDRESS:
            case IFLA_BROADCAST:
            {
              uint8_t* a = (uint8_t*)data_ptr;
              if (attr->payload() == 6) {
                printf("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                  a[0], a[1], a[2], a[3], a[4], a[5]);
              } else if (attr->payload() == 4) {
                printf("%d.%d.%d.%d",
                  a[0], a[1], a[2], a[3]);
              } else {
                printf("unknown");
              }
              break;
            }
            default:
            {
              printf("0x");
              for (size_t i=0; i<data_len; i++)
                printf("%02x", data_ptr[i]);
              break;
            }
          }
          printf("},");
        }
#endif
        printf("\n");
        break;
      }
      default:
        printf("type isn't supported");
    }
  }

}; /* class msghdr */



inline void handle_msg_route(Msghdr *hdr)
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
    default: printf("unknown(%d)\n", msghdr->msg_type()); break;
  }
}

class netlink {
 public:

 private:

  uint8_t buf[NETLINK_BUFFER_SIZE]; /* for only recv_msg() */
  Msghdr* recv_nxt = nullptr;       /* for only recv_msg() */
  size_t  len;                      /* for only recv_msg() */
 public:

  Msghdr* recv_msg()
  {
    if (recv_nxt == nullptr) {
      uint8_t* start = buf;
      len = sock_.recv(start, sizeof(buf)-len, 0);
      recv_nxt = reinterpret_cast<Msghdr*>(buf);
    }
    Msghdr* ret = recv_nxt;
    recv_nxt = recv_nxt->next(len);
    if (!recv_nxt->ok(len)) recv_nxt = nullptr;
    return ret;
  }

  void dump_fib()
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
    sock_.send(&req, req.hdr.nlmsg_len, 0);

    while (Msghdr* msghdr = recv_msg()) {
      if (msghdr->msg_type() == NLMSG_DONE) {
        printf("done\n");
        break;
      }
#if 1
      msghdr->summary();
#else
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
#endif
    }
  }

  netlink() { sock_.socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE); }

 private:
  slankdev::socketfd sock_;
}; /* class netlink */

} /* namespace netlink */


