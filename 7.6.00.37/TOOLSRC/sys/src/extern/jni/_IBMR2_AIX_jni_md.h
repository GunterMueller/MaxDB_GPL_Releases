/*
 * @(#)src/contract/jvm/pfm/jni_md.h, xa130, xa130, 20000908 1.4.1.2
 * ===========================================================================
 * Licensed Materials - Property of IBM
 * IBM Java(tm)2 SDK, Standard Edition, v 1.2
 *
 * (C) Copyright IBM Corp. 1999 All Rights Reserved.
 * Copyright 1996-1998 by Sun Microsystems Inc.,
 * ===========================================================================
 * Change activity:
 *
 * Reason  Date   Origin  Description
 * ------  ----   ------  ----------------------------------------------------
 * 009653  080800 hdngmr  Added XJNICALL definition.
 *
 * ===========================================================================
 */

#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#define JNIEXPORT_PROTOTYPE                                           /*ibm.54*/
#define JNIEXPORT
#define JNIIMPORT
#define JNICALL
#define XJNICALL                                                   /*ibm@9653*/

#ifdef _LP64 /* 64-bit Solaris */
typedef int jint;
#else
typedef long jint;
#endif
typedef long long jlong;
typedef signed char jbyte;
typedef jint jsize;                                                 /*ibm@9237*/
#define JNI_JSIZE_ALREADY_DEFINED_IN_JNI_MD_H 1

#endif /* !_JAVASOFT_JNI_MD_H_ */
