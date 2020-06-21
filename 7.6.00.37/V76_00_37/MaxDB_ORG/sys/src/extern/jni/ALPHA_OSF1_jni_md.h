/* Modifications Copyright 1998 Digital Equipment Corporation */
/*
 * @(#)jni_md.h	1.15 00/05/04
 *
 * Copyright 2000 Sun Microsystems, Inc. All rights reserved.
 * Copyright 2000 Sun Microsystems, Inc. Tous droits réservés.
 *
 * This software is the proprietary information of Sun Microsystems, Inc.
 * Use is subject to license terms.
 */

#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#define JNIEXPORT 
#define JNIIMPORT
#define JNICALL

/* #ifdef _LP64 */ /* 64-bit Solaris */  /* #### BP comment out #ifdef */
typedef int jint;
/* #else  */  /* #### BP comment out #else */
#ifdef __VMS
typedef __int64 jlong;
#else
typedef long jlong;   /* #### BP was jint */
#endif
/* #endif */  /* #### BP comment out #endif */
/* typedef long long jlong; */ /* #### BP comment out typedef of long long */
typedef signed char jbyte;

#endif /* !_JAVASOFT_JNI_MD_H_ */
