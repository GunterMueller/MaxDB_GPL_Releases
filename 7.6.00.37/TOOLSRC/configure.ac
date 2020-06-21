# Process this file with autoconf to produce a configure script.
AC_INIT(sapdb-build-tools, 0.9, sapdb.general@listserv.sap.com)
AC_CONFIG_SRCDIR([basescripts.mk.in])
# use one devconfig.h.in instead of configure'ing each independantly
# but: copy to each subproject for easier -I options
AC_CONFIG_HEADER([sys/src/base/inc/devconfig.h:devconfig.h.in])
AC_CONFIG_HEADER([sys/src/base/vmake/devconfig.h:devconfig.h.in])
AC_CONFIG_HEADER([sys/src/base/ptoc/include/devconfig.h:devconfig.h.in])

# Checks for system which cannot use gcc.
AC_CANONICAL_HOST
AC_MSG_CHECKING([checking for usable gcc compiler])
case "${host}" in
*-*-solaris*) eval CC=cc ; eval export CC ; eval CXX=CC ; eval export CXX
                      AC_MSG_RESULT([not usable]) ;;
*alpha*-dec*)         eval CC=cc ; eval export CC ; eval CXX=CC ; eval export CXX
                      AC_MSG_RESULT([not usable]) ;;
*)                    AC_MSG_RESULT([theoretically usable]) ;;
esac

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lc':
AC_CHECK_LIB([c], [main])
# FIXME: Replace `main' with a function in `-list':
AC_CHECK_LIB([ist], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([unistd.h dirent.h sys/select.h prototypes.h ar.h sys/wait.h wait.h alloca.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_STRUCT_TM
AC_CHECK_MEMBERS(struct fl_hdr.fl_fstmoff,,,[#include <ar.h>])
AC_CHECK_SIZEOF(long int *)
AC_CHECK_SIZEOF(void *, 4)
AC_C_BIGENDIAN

# Checks for library functions.
AC_CHECK_FUNCS([access link chmod])
AC_FUNC_SETPGRP
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_SETPGRP
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([btowc bzero getcwd gethostbyaddr gethostbyname getpass memset mkdir putenv re_comp regcomp rmdir socket sqrt strcasecmp strchr strcspn strdup strerror strncasecmp strrchr strstr strtol tzset utime])
SOCKET_LIBS=""
AC_CHECK_LIB(nsl, gethostname, SOCKET_LIBS="$SOCKET_LIBS -lnsl")
AC_CHECK_LIB(socket, accept, SOCKET_LIBS="$SOCKET_LIBS -lsocket")
AC_SUBST(netlibs, $SOCKET_LIBS)

AC_LANG(C++)
AC_MSG_CHECKING([size of C++ data type 'bool'])
# compile will fail for sizeof (bool) == 1, this failure is the ok result
AC_TRY_COMPILE([],[int  testv [sizeof (bool)] = {1, 2};],msg="not usable",msg="ok")
AC_MSG_RESULT([sizeof (bool) == 1: $msg])
if test ".$msg" != .ok; then
  echo "It is required that the C++ data type 'bool' has got a"
  echo "size of one byte. The current C++ compiler can not be used."
  exit 1
fi
AC_LANG(C)

# detect platform identifier
AC_SUBST(sapdb_mach, AUTO_MACH)
AC_SUBST(sapdb_os, UNIX)
AC_SUBST(sapdb_osspec, AUTO_OSSPEC)
AC_SUBST(sapdb_fpic, "")

case "${host_cpu}" in
i[[34567]]86*) sapdb_mach=I386  ;;
sparc*)        sapdb_mach=SUN   ;;
SPARC*)        sapdb_mach=SPARC ;; #in the future? [JnZ SUN]
hppa2*)        sapdb_mach=PA20W ;;
alpha*)        sapdb_mach=ALPHA ;;
powerpc*)      sapdb_mach=_IBMR2;;
mips*)         sapdb_mach=NMP   ;;
ia64*)         sapdb_mach=IA64  ;;
s390x*)        sapdb_mach=S390X ;;
s390*)         sapdb_mach=S390  ;;
x86_64*)       sapdb_mach=X86_64 ;;
*)
         echo ""
         echo "ATTENTION: this cpu is not supported yet"
         echo ""
         ;;
esac

case "${host_os}" in
linux*)       sapdb_osspec=LINUX ;;
solaris*)     case ${host_cpu} in
              sparc*) sapdb_osspec=SVR4   ;;
              *)      sapdb_osspec=SOLARIS;;
              esac
              ;;
hpux*)        sapdb_osspec=HPUX  ;;
osf*)         sapdb_osspec=OSF1  ;;
aix*)         sapdb_osspec=AIX   ;;
syv4*)        sapdb_osspec=SVR4  ;;
freebsd*)     sapdb_osspec=FREEBSD;;
openbsd*)     sapdb_osspec=OPENBSD;;
esac

case "${host_os}" in
linux*)       sapdb_fpic="-fPIC" ;;
*)            sapdb_fpic=""      ;;
esac

AC_SUBST(tool, `pwd`)
AC_SUBST(buildroot, `pwd`)

AC_CONFIG_FILES([sys/src/base/Makefile
                 sys/src/base/Makedefs
                 sys/src/base/concom/Makefile
                 sys/src/base/mf/Makefile
                 sys/src/base/mfcline/Makefile
                 sys/src/base/ptoc/Makefile
                 sys/src/base/ptoc/Makedefs
                 sys/src/base/ptoc/libpc/Makefile
                 sys/src/base/ptoc/pcomp/Makefile
                 sys/src/base/ptoc/ptoc/Makefile
                 sys/src/base/ptoc/sqlsrc/Makefile
                 sys/src/base/ptoc/tools/Makefile
                 sys/src/base/ptoc/tools/eyacc/Makefile
                 sys/src/base/ptoc/tools/makedepend/Makefile
                 sys/src/base/ptoc/tools/mkvers/Makefile
                 sys/src/base/ptoc/tools/tws/Makefile
                 sys/src/base/vmake/Makefile
                 Makefile
                 makedirs.mk
                 basescripts.mk])
AC_OUTPUT

