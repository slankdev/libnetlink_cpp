
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netlink/netlink.h>

int main(int argc, char** argv)
{
  netlink::netlink netlink;
  netlink.dump_fib();
}


