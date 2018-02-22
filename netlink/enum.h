
#pragma once
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <slankdev/util.h>
#include <slankdev/hexdump.h>

inline const char *
rta_type2str(unsigned short rta_type)
{
  switch (rta_type) {
  case IFLA_UNSPEC         : return "UNSPEC"         ;
  case IFLA_ADDRESS        : return "ADDRESS"        ;
  case IFLA_BROADCAST      : return "BROADCAST"      ;
  case IFLA_IFNAME         : return "IFNAME"         ;
  case IFLA_MTU            : return "MTU"            ;
  case IFLA_LINK           : return "LINK"           ;
  case IFLA_QDISC          : return "QDISC"          ;
  case IFLA_STATS          : return "STATS"          ;
  case IFLA_COST           : return "COST"           ;
  case IFLA_PRIORITY       : return "PRIORITY"       ;
  case IFLA_MASTER         : return "MASTER"         ;
  case IFLA_WIRELESS       : return "WIRELESS"       ;
  case IFLA_PROTINFO       : return "PROTINFO"       ;
  case IFLA_TXQLEN         : return "TXQLEN"         ;
  case IFLA_MAP            : return "MAP"            ;
  case IFLA_WEIGHT         : return "WEIGHT"         ;
  case IFLA_OPERSTATE      : return "OPERSTATE"      ;
  case IFLA_LINKMODE       : return "LINKMODE"       ;
  case IFLA_LINKINFO       : return "LINKINFO"       ;
  case IFLA_NET_NS_PID     : return "NET_NS_PID"     ;
  case IFLA_IFALIAS        : return "IFALIAS"        ;
  case IFLA_NUM_VF         : return "NUM_VF"         ;
  case IFLA_VFINFO_LIST    : return "VFINFO_LIST"    ;
  case IFLA_STATS64        : return "STATS64"        ;
  case IFLA_VF_PORTS       : return "VF_PORTS"       ;
  case IFLA_PORT_SELF      : return "PORT_SELF"      ;
  case IFLA_AF_SPEC        : return "AF_SPEC"        ;
  case IFLA_GROUP          : return "GROUP"          ;
  case IFLA_NET_NS_FD      : return "NET_NS_FD"      ;
  case IFLA_EXT_MASK       : return "EXT_MASK "      ;
  case IFLA_PROMISCUITY    : return "PROMISCUITY"    ;
  case IFLA_NUM_TX_QUEUES  : return "NUM_TX_QUEUES"  ;
  case IFLA_NUM_RX_QUEUES  : return "NUM_RX_QUEUES"  ;
  case IFLA_CARRIER        : return "CARRIER"        ;
  case IFLA_PHYS_PORT_ID   : return "PHYS_PORT_ID"   ;
  case IFLA_CARRIER_CHANGES: return "CARRIER_CHANGES";
  case IFLA_PHYS_SWITCH_ID : return "PHYS_SWITCH_ID" ;
  case IFLA_LINK_NETNSID   : return "LINK_NETNSID"   ;
  case IFLA_PHYS_PORT_NAME : return "PHYS_PORT_NAME" ;
  case IFLA_PROTO_DOWN     : return "PROTO_DOWN"     ;
  default: return "unknown";
  }
}

inline const char* rtm_type2str(uint16_t rtm_type)
{
  switch (rtm_type) {
  case RTM_NEWLINK     : return "NEWLINK"     ;
  case RTM_DELLINK     : return "DELLINK"     ;
  case RTM_GETLINK     : return "GETLINK"     ;
  case RTM_SETLINK     : return "SETLINK"     ;
  case RTM_NEWADDR     : return "NEWADDR"     ;
  case RTM_DELADDR     : return "DELADDR"     ;
  case RTM_GETADDR     : return "GETADDR"     ;
  case RTM_NEWROUTE    : return "NEWROUTE"    ;
  case RTM_DELROUTE    : return "DELROUTE"    ;
  case RTM_GETROUTE    : return "GETROUTE"    ;
  case RTM_NEWNEIGH    : return "NEWNEIGH"    ;
  case RTM_DELNEIGH    : return "DELNEIGH"    ;
  case RTM_GETNEIGH    : return "GETNEIGH"    ;
  case RTM_NEWRULE     : return "NEWRULE"     ;
  case RTM_DELRULE     : return "DELRULE"     ;
  case RTM_GETRULE     : return "GETRULE"     ;
  case RTM_NEWQDISC    : return "NEWQDISC"    ;
  case RTM_DELQDISC    : return "DELQDISC"    ;
  case RTM_GETQDISC    : return "GETQDISC"    ;
  case RTM_NEWTCLASS   : return "NEWTCLASS"   ;
  case RTM_DELTCLASS   : return "DELTCLASS"   ;
  case RTM_GETTCLASS   : return "GETTCLASS"   ;
  case RTM_NEWTFILTER  : return "NEWTFILTER"  ;
  case RTM_DELTFILTER  : return "DELTFILTER"  ;
  case RTM_GETTFILTER  : return "GETTFILTER"  ;
  case RTM_NEWACTION   : return "NEWACTION"   ;
  case RTM_DELACTION   : return "DELACTION"   ;
  case RTM_GETACTION   : return "GETACTION"   ;
  case RTM_NEWPREFIX   : return "NEWPREFIX"   ;
  case RTM_GETMULTICAST: return "GETMULTICAST";
  case RTM_GETANYCAST  : return "GETANYCAST"  ;
  case RTM_NEWNEIGHTBL : return "NEWNEIGHTBL" ;
  case RTM_GETNEIGHTBL : return "GETNEIGHTBL" ;
  case RTM_SETNEIGHTBL : return "SETNEIGHTBL" ;
  case RTM_NEWNDUSEROPT: return "NEWNDUSEROPT";
  case RTM_NEWADDRLABEL: return "NEWADDRLABEL";
  case RTM_DELADDRLABEL: return "DELADDRLABEL";
  case RTM_GETADDRLABEL: return "GETADDRLABEL";
  case RTM_GETDCB      : return "GETDCB"      ;
  case RTM_SETDCB      : return "SETDCB"      ;
  case RTM_NEWNETCONF  : return "NEWNETCONF"  ;
  case RTM_GETNETCONF  : return "GETNETCONF"  ;
  case RTM_NEWMDB      : return "NEWMDB"      ;
  case RTM_DELMDB      : return "DELMDB"      ;
  case RTM_GETMDB      : return "GETMDB"      ;
  case RTM_NEWNSID     : return "NEWNSID"     ;
  case RTM_DELNSID     : return "DELNSID"     ;
  case RTM_GETNSID     : return "GETNSID"     ;
  default: return "UNKNOWN";
  }
}


inline const char* rtn_type2str(uint8_t rtn_type)
{
  switch (rtn_type) {
    case RTN_UNSPEC     : return "UNSPEC"     ;
    case RTN_UNICAST    : return "UNICAST"    ;
    case RTN_LOCAL      : return "LOCAL"      ;
    case RTN_BROADCAST  : return "BROADCAST"  ;
    case RTN_ANYCAST    : return "ANYCAST"    ;
    case RTN_MULTICAST  : return "MULTICAST"  ;
    case RTN_BLACKHOLE  : return "BLACKHOLE"  ;
    case RTN_UNREACHABLE: return "UNREACHABLE";
    case RTN_PROHIBIT   : return "PROHIBIT"   ;
    case RTN_THROW      : return "THROW"      ;
    case RTN_NAT        : return "NAT"        ;
    case RTN_XRESOLVE   : return "XRESOLVE"   ;
    default: return "unknown";
  }
}

inline const char* RT_SCOPE_2STR(uint8_t s)
{
  switch (s) {
    case RT_SCOPE_UNIVERSE: return "UNIVERSE";
    case RT_SCOPE_SITE    : return "SITE"    ;
    case RT_SCOPE_LINK    : return "LINK"    ;
    case RT_SCOPE_HOST    : return "HOST"    ;
    case RT_SCOPE_NOWHERE : return "NOWHERE" ;
    default: return "user-defined";
  }
}

inline const char* RTPROTO_2STR(size_t val)
{
  switch (val) {
    case RTPROT_UNSPEC    : return "UNSPEC"  ;
    case RTPROT_REDIRECT  : return "REDIRECT";
    case RTPROT_KERNEL    : return "KERNEL"  ;
    case RTPROT_BOOT      : return "BOOT"    ;
    case RTPROT_STATIC    : return "STATIC"  ;
    case RTPROT_GATED     : return "GATED"   ;
    case RTPROT_RA        : return "RA"      ;
    case RTPROT_MRT       : return "MRT"     ;
    case RTPROT_ZEBRA     : return "ZEBRA"   ;
    case RTPROT_BIRD      : return "BIRD"    ;
    case RTPROT_DNROUTED  : return "DNROUTED";
    case RTPROT_XORP      : return "XORP"    ;
    case RTPROT_NTK       : return "NTK"     ;
    case RTPROT_DHCP      : return "DHCP"    ;
    case RTPROT_MROUTED   : return "MROUTED" ;
    case RTPROT_BABEL     : return "BABEL"   ;
    default: return "user-defined";
  }
}

inline const char* AF_2STR(size_t val)
{
  switch (val) {
    case AF_UNSPEC    : return "UNSPEC"    ;
    case AF_LOCAL     : return "LOCAL"     ;
    case AF_INET      : return "INET"      ;
    case AF_AX25      : return "AX25"      ;
    case AF_IPX       : return "IPX"       ;
    case AF_APPLETALK : return "APPLETALK ";
    case AF_NETROM    : return "NETROM"    ;
    case AF_BRIDGE    : return "BRIDGE"    ;
    case AF_ATMPVC    : return "ATMPVC"    ;
    case AF_X25       : return "X25"       ;
    case AF_INET6     : return "INET6"     ;
    case AF_ROSE      : return "ROSE"      ;
    case AF_DECnet    : return "DECnet"    ;
    case AF_NETBEUI   : return "NETBEUI"   ;
    case AF_SECURITY  : return "SECURITY"  ;
    case AF_KEY       : return "KEY"       ;
    case AF_NETLINK   : return "NETLINK"   ;
    case AF_PACKET    : return "PACKET"    ;
    case AF_ASH       : return "ASH"       ;
    case AF_ECONET    : return "ECONET"    ;
    case AF_ATMSVC    : return "ATMSVC"    ;
    case AF_RDS       : return "RDS"       ;
    case AF_SNA       : return "SNA"       ;
    case AF_IRDA      : return "IRDA"      ;
    case AF_PPPOX     : return "PPPOX"     ;
    case AF_WANPIPE   : return "WANPIPE"   ;
    case AF_LLC       : return "LLC"       ;
    case AF_IB        : return "IB"        ;
    case AF_MPLS      : return "MPLS"      ;
    case AF_CAN       : return "CAN"       ;
    case AF_TIPC      : return "TIPC"      ;
    case AF_BLUETOOTH : return "BLUETOOTH" ;
    case AF_IUCV      : return "IUCV"      ;
    case AF_RXRPC     : return "RXRPC"     ;
    case AF_ISDN      : return "ISDN"      ;
    case AF_PHONET    : return "PHONET"    ;
    case AF_IEEE802154: return "IEEE802154";
    case AF_CAIF      : return "CAIF"      ;
    case AF_ALG       : return "ALG"       ;
    case AF_NFC       : return "NFC"       ;
    case AF_VSOCK     : return "VSOCK"     ;
    case AF_MAX       : return "MAX"       ;
  }
}

