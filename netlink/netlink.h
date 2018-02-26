
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

  template <class T>
  T read_data() const
  {
    T* ptr = reinterpret_cast<T*>(data());
    return *ptr;
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

}; /* class msghdr */


class Netlink {
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

  void send_msg(const void* buf, size_t len)
  { sock_.send(buf, len, 0); }

  Netlink() { sock_.socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE); }

 private:
  slankdev::socketfd sock_;
}; /* class netlink */

void addattr(struct nlmsghdr *n, size_t maxlen, int type, void *data, size_t alen)
{
    int len = RTA_LENGTH(alen);
    struct rtattr *rta;
    if (NLMSG_ALIGN(n->nlmsg_len) + len > maxlen) exit(1);

    rta = (struct rtattr*)(((char*)n) + NLMSG_ALIGN(n->nlmsg_len));
    rta->rta_type = type;
    rta->rta_len = len;
    memcpy(RTA_DATA(rta), data, alen);
    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + len;
}

} /* namespace netlink */


