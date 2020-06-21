/*
 * Copyright 2002 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#define JNIEXPORT
#define JNIIMPORT extern
#define JNICALL

#ifdef _HPUX
typedef long jint;
#else
#ifdef _LP64 /* 64-bit Solaris */
typedef int jint;
#else
typedef long jint;
#endif
#endif /* _HPUX */

typedef long long jlong;
typedef signed char jbyte;

#endif /* !_JAVASOFT_JNI_MD_H_ */

