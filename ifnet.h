#ifndef TINIT_IFNET_H
#define TINIT_IFNET_H

#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/route.h>
#include <netinet/in.h>


int create_socket(void)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("failed to create socket");
        return -1;
    }
    return fd;
}

void close_socket(int fd)
{
    if (fd >= 0) {
        close(fd);
    }
}

static inline void init_sockaddr_in(struct sockaddr_in *sin, const char *ip)
{
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    sin->sin_addr.s_addr = inet_addr(ip);
}

static void init_ifreq(struct ifreq *ifr, const char *ifname)
{
    memset(ifr, 0, sizeof(struct ifreq));
    strncpy(ifr->ifr_name, ifname, IFNAMSIZ);
}

int get_ifflags(int fd, const char *ifname)
{
    struct ifreq ifr;
    init_ifreq(&ifr, ifname);
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
        perror("failed to get interface flags");
        return -1;
    }
    return ifr.ifr_flags;
}

int set_ifflags(int fd, const char *ifname, int flags)
{
    struct ifreq ifr;
    init_ifreq(&ifr, ifname);
    ifr.ifr_flags = flags;
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
        perror("failed to set interface flags");
        return -1;
    }
    return 0;
}

int set_ipaddr(int fd, const char *ifname, const char *ipaddr)
{
    struct ifreq ifr;
    init_ifreq(&ifr, ifname);
    init_sockaddr_in((struct sockaddr_in *) &ifr.ifr_addr, ipaddr);
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        perror("failed to set ip address");
        return -1;
    }
    return 0;
}

int set_netmask(int fd, const char *ifname, const char *netmask)
{
    struct ifreq ifr;
    init_ifreq(&ifr, ifname);
    init_sockaddr_in((struct sockaddr_in *) &ifr.ifr_addr, netmask);
    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0) {
        perror("failed to set netmask");
        return -1;
    }
    return 0;
}

int set_broadcast(int fd, const char *ifname, const char *broadcast)
{
    struct ifreq ifr;
    init_ifreq(&ifr, ifname);
    init_sockaddr_in((struct sockaddr_in *) &ifr.ifr_addr, broadcast);
    if (ioctl(fd, SIOCSIFBRDADDR, &ifr) < 0) {
        perror("failed to set broadcast address");
        return -1;
    }
    return 0;
}

int set_mtu(int fd, const char *ifname, int mtu)
{
    struct ifreq ifr;
    init_ifreq(&ifr, ifname);
    ifr.ifr_mtu = mtu;
    if (ioctl(fd, SIOCSIFMTU, &ifr) < 0) {
        perror("failed to set mtu");
        return -1;
    }
    return 0;
}

int set_def_rt(int fd, const char *gateway)
{
    struct rtentry rt;
    memset(&rt, 0, sizeof(rt));
    rt.rt_dst.sa_family = AF_INET;
    rt.rt_flags = RTF_UP | RTF_GATEWAY;
    init_sockaddr_in((struct sockaddr_in *) &rt.rt_gateway, gateway);
    init_sockaddr_in((struct sockaddr_in *) &rt.rt_genmask, "0.0.0.0");
    if (ioctl(fd, SIOCADDRT, &rt) < 0) {
        perror("failed to set default route");
        return -1;
    }
    return 0;
}

int ifup(int fd, const char *ifname)
{
    int flags = get_ifflags(fd, ifname);
    if (flags < 0) {
        return -1;
    }
    flags |= IFF_UP;
    return set_ifflags(fd, ifname, flags);
}

int ifconfig_main(int fd, char *ifname, char *ipaddr, char *netmask)
{
    if (ifup(fd, ifname) < 0) {
        close_socket(fd);
        return -1;
    }

    if (set_ipaddr(fd, ifname, ipaddr) < 0) {
        close_socket(fd);
        return -1;
    }

    if (set_netmask(fd, ifname, netmask) < 0) {
        close_socket(fd);
        return -1;
    }

    return 0;
}

int add_dns(const char *dns)
{
    FILE *fp = fopen("/etc/resolv.conf", "w");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, "nameserver %s", dns);
    fclose(fp);
    return 0;
}


#endif //TINIT_IFNET_H
