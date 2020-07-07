/*
 * @(#)src/contract/jvm/pfm/jni_md.h, core, xx131, 20020223 1.7.1.1
 * ===========================================================================
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * IBM Java(tm)2 SDK, Standard Edition, v 1.3.1
 * (C) Copyright IBM Corp. 1998, 2001. All Rights Reserved
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 * ===========================================================================
 */

/*
 *
 * ===========================================================================
 *
 * Copyright 1996-2000 Sun Microsystems, Inc. All Rights Reserved.
 * ===========================================================================
 * Change activity:
 *
 * Reason  Date   Origin  Description
 * ------  ----   ------  ----------------------------------------------------
 * 009653  080800 hdngmr  Added XJNICALL definition.
 *
 * ===========================================================================
 */

/*
 * Note: this is from a 32bit java on X86_64 linux.
 *       It is unknown whether this is suitable
 *       for a 64bit Java.  DanielD
 */
#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#define JNIEXPORT_PROTOTYPE                                           /*ibm.54*/
#define JNIEXPORT
#define JNIIMPORT
#define JNICALL
#define XJNICALL                                                     /*ibm@9653*/

#include <inttypes.h>                                                /*ibm@23359*/

typedef int32_t jint;                                                /*ibm@23359*/
typedef int64_t jlong;                                               /*ibm@23359*/
typedef signed char jbyte;
#ifdef _LP64
typedef jlong jsize;                                                 /*ibm@23359*/
#else
typedef jint jsize;                                                  /*ibm@9237*/
#endif
#define JNI_JSIZE_ALREADY_DEFINED_IN_JNI_MD_H 1

#endif /* !_JAVASOFT_JNI_MD_H_ */
