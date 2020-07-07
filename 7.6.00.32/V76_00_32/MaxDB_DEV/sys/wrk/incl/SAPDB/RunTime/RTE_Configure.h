/*
 * RTE_Config.h
 * this file is generated by:
 * imf RTE_Configure.pl
 */

#ifndef RTE_CONFIGURE_H
#define RTE_CONFIGURE_H

/*
 * true if byte order is big endian 1 = [0001]
 * not defined if byte order is little endian 1 = [1000]
 */
/* #undef SDB_BYTE_ORDER_MOST_SIGNIFICANT_BYTE_FIRST */

/*
 * true if 64bit address space is supported
 * not defined if only 32bit address space is supported
 */
/* #undef SDB_BIT64 */

/*
 * int type for 8 bye integer
 */
#define SDB_INT8_TYPE long long

/*
 * true if old style Linux threads are used
 */
/* #undef SDB_THREAD_CREATES_PROCESS_ID */

/*
 * true if security/pam_appl.h can be included
 */
#define HAS_PAM_SUPPORT 1

/*
 * parameter msg of PAM_conversation is const
 */
#define HAS_CONST_PAM_MESSAGE_PARAMETER 1

/*
 * true if sys/atomic.h can be included
 */
/* #undef SDB_HAS_SYS_ATOMIC */

#endif
