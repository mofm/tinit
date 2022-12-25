AC_PREREQ([2.71])
AC_INIT([tinit],[0.0.1],[emre.eryilmaz@piesso.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_CANONICAL_HOST

dnl Initialize maintainer mode
AM_MAINTAINER_MODE([enable])

AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h sys/ioctl.h sys/mount.h sys/socket.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([dup2 memset mkdir sethostname socket])

# AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([
           Makefile
           src/Makefile
])
AC_OUTPUT