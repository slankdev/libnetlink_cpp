
#pragma once
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <slankdev/util.h>
#include <slankdev/hexdump.h>
#include <netlink/enum.h>

inline void print_rtattr(FILE* fp, const struct rtattr* attr)
{
  using slankdev::depth_fprintf;
  uint8_t* data_ptr = (uint8_t*)(attr);
  size_t   data_len = attr->rta_len;
  depth_fprintf(fp, "len : %u\n", attr->rta_len);
  depth_fprintf(fp, "type: %u (%s)\n",
      attr->rta_type, IFLA_2STR(attr->rta_type));
  depth_fprintf(fp, "attr-raw: ");
  for (size_t i=0; i<data_len; i++) {
    fprintf(fp, "%02x ", data_ptr[i]);
  }
  fprintf(fp, "\n");
}

inline void print_nlmsghdr(FILE* fp, const struct nlmsghdr* hdr)
{
  using slankdev::depth_fprintf;
  depth_fprintf(fp, "len  (32bit): %u \n", hdr->nlmsg_len  );
  depth_fprintf(fp, "type (16bit): %s(%u)\n", RTM_2STR(hdr->nlmsg_type), hdr->nlmsg_type );
  depth_fprintf(fp, "flags(16bit): %u \n", hdr->nlmsg_flags);
  depth_fprintf(fp, "seq  (32bit): %u \n", hdr->nlmsg_seq  );
  depth_fprintf(fp, "pid  (32bit): %u \n", hdr->nlmsg_pid  );
}

inline void print_rtmsg(FILE* fp, const struct rtmsg* msg)
{
  using slankdev::depth_fprintf;
	depth_fprintf(fp, "family  (8bit): %u (%s)\n",
      msg->rtm_family, AF_2STR(msg->rtm_family));
	depth_fprintf(fp, "dst_len (8bit): %u \n", msg->rtm_dst_len );
	depth_fprintf(fp, "src_len (8bit): %u \n", msg->rtm_src_len );
	depth_fprintf(fp, "tos     (8bit): %u \n", msg->rtm_tos     );
	depth_fprintf(fp, "table   (8bit): %u \n", msg->rtm_table   );
	depth_fprintf(fp, "protocol(8bit): %u (%s)\n",
      msg->rtm_protocol, RTPROT_2STR(msg->rtm_protocol));
	depth_fprintf(fp, "scope   (8bit): %u (%s) \n",
      msg->rtm_scope, RT_SCOPE_2STR(msg->rtm_scope));
	depth_fprintf(fp, "type    (8bit): %u (%s) \n",
      msg->rtm_type, RTN_2STR(msg->rtm_type));
	depth_fprintf(fp, "flags  (32bit): %u \n", msg->rtm_flags   );
}


