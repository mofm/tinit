#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <termios.h>
#include "ifnet.h"


static const char *MC_ENVS[] = {
        "PATH=/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin",
        "MC_INIT",
        "MC_HOSTNAME",
        "MC_TTY",
        "MC_DEBUG",
        "MC_INTERFACE",
        "MC_IPADDR",
        "MC_NETMASK",
        "MC_BROADCAST",
        "MC_GATEWAY",
        "MC_DNS",
        NULL,
};

static const char *INTERFACES = "eth0";
static const char *BIN_SH = "/bin/sh";

static void pr_debug(const char *fmt, ...) {
    if (strcmp(getenv("MC_DEBUG"), "1") != 0) {
        return;
    }

    printf("init: ");

    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);

    printf("\n");
}

static void cleanup_env() {
    const char **env_var = (const char **) MC_ENVS;
    while (*env_var != NULL) {
        unsetenv(*env_var);
        env_var++;
    }
}

void mount_check(
        const char *source, const char *target,
        const char *filesystemtype, unsigned long mountflags,
        const void *data
) {
    struct stat info;

    if (stat(target, &info) == -1 && errno == ENOENT) {
        pr_debug("Creating %s\n", target);
        if (mkdir(target, 0755) < 0) {
            perror("Creating directory failed");
            exit(1);
        }
    }

    pr_debug("Mounting %s\n", target);
    if (mount(source, target, filesystemtype, mountflags, data) < 0) {
        perror("Mount failed");
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    mount_check("none", "/proc", "proc", 0, "");
    mount_check("none", "/dev/pts", "devpts", 0, "");
    mount_check("none", "/dev/mqueue", "mqueue", 0, "");
    mount_check("none", "/dev/shm", "tmpfs", 0, "");
    mount_check("none", "/sys", "sysfs", 0, "");
    mount_check("none", "/sys/fs/cgroup", "cgroup", 0, "");

    char *hostname = getenv("MC_HOSTNAME");
    if (hostname) {
        pr_debug("sethostname: %s", hostname);
        if (sethostname(hostname, strlen(hostname)) < 0) {
            perror("sethostname failed");
            return 1;
        }
    }

    char *init = getenv("MC_INIT");
    if (!init) {
        init = (char *)BIN_SH;
    }
    argv[0] = init;
    pr_debug("execvp: argc=%d argv0=%s", argc, argv[0]);

    if (strcmp(getenv("MC_TTY"), "1") == 0) {
        setsid();

        int fd = open("/dev/hvc0", O_RDWR);
        if (fd < 0) {
            perror("open: /dev/hvc0");
            return 1;
        }

        if (!isatty(fd)) {
            perror("isatty: /dev/hvc0");
            return 1;
        }

        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);

        while (fd > 2) {
            close(fd--);
        }
        ioctl(0, TIOCSCTTY, 1);
    } else {
        struct termios term;
        tcgetattr(0, &term);
        term.c_lflag &= ~ECHO;
        tcsetattr(0, 0, &term);
        printf("Welcome to MicroVM!\n");
    }

    // Set up networking
    char *loopback = "lo";
//    char *interface = "eth0";
    char *interface = getenv("MC_INTERFACE");
    if (!interface) {
        interface = (char *)INTERFACES;
    }
    int if_fd = create_socket();
    // first, up loopback interface
    pr_debug("ifup: %s", loopback);
    if (ifup(if_fd, loopback) < 0) {
        perror("if_up");
        return 1;
    }
    char *ipaddr = getenv("MC_IPADDR");
    char *netmask = getenv("MC_NETMASK");
    char *gateway = getenv("MC_GATEWAY");
    char *dns = getenv("MC_DNS");

    pr_debug("Check if there is a network interface");
    if (ifup(if_fd, interface) == 0) {
        pr_debug("Network interface will be configured");
        if (ipaddr && netmask) {
            pr_debug("Configuring network: ipaddr=%s netmask=%s", ipaddr, netmask);
            if (ifconfig_main(if_fd, interface, ipaddr, netmask) != 0) {
                perror("interface configuration failed");
                return 1;
            }
        }
        if (gateway) {
            pr_debug("Configuring gateway: %s", gateway);
            if (set_def_rt(if_fd, gateway) != 0) {
                perror("gateway configuration failed");
                return 1;
            }
        }
        if (dns) {
            pr_debug("Configuring DNS: %s", dns);
            if (add_dns(dns) != 0) {
                perror("DNS configuration failed");
                return 1;
            }
        }
    } else {
        pr_debug("No network interface found");
    }

    close_socket(if_fd);

    cleanup_env();
    return execv(argv[0], argv);
}