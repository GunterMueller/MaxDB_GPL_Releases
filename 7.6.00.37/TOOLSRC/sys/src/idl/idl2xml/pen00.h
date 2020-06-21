/*


    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#define Gen00Def    1

#define INC_SYS "/INC/"                 /* system include directory */

/*----------------------------------------------------------------------------*/
/* Operating Systems                                                          */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* VAX/VMS                                                                    */
/*----------------------------------------------------------------------------*/

#ifdef OS_VMS

#define ARR_0 []                /* vax/vms cc likes this                */

#define SWAP_CODE 1

#define NRM_RFI  0              /* kein RFI-Byteswap gegenueber VMS */

#define VARARG   1              /* variable Zahl von Argumenten erlaubt */

#include "vms00.h"              /* nocheck */ /* wegen Portabilitaet */

#define stderr_Exists

#endif

/*----------------------------------------------------------------------------*/
/* APO  OS                                                                    */
/*----------------------------------------------------------------------------*/

#ifdef APO

#define VARARG   0              /* keine variable Zahl von Argumenten erlaubt */

#define ARR_0 [1]               /* ULTRIX cc likes this         */

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#endif

/*----------------------------------------------------------------------------*/
/* MS_DOS                                                                     */
/*----------------------------------------------------------------------------*/

#ifdef MS_DOS

#define VARARG   0              /* keine variable Zahl von Argumenten erlaubt */

#define ARR_0 []                /* C++        */

#define NRM_RFI  0              /* RFI-Byteswap gegenueber VMS */

#include <memory.h>             /* wegen Portabilitaet */

#include <malloc.h>             /* wegen Portabilitaet */

#include <stdio.h>              /* wegen Portabilitaet */

#include <stdlib.h>             /* wegen Portabilitaet */

#include <string.h>             /* wegen Portabilitaet */
#include <windows.h>
#include <time.h>

#define SWAP_CODE 1

typedef _int64         d_int,     *d_int_p;

#else
#ifdef BIT64
typedef long           d_int,     *d_int_p;
#else
typedef long long      d_int,     *d_int_p;
#endif 

#endif 

/*----------------------------------------------------------------------------*/
/* FreeBSD (Intel)                                                              */
/*----------------------------------------------------------------------------*/

#ifdef FREEBSD 

#include <stdlib.h>
#include <string.h>

#define ARR_0 []                /* gnu-c++  likes this         */

#define SWAP_CODE 0

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#define VARARG   1              /* keine variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif

/*----------------------------------------------------------------------------*/
/* Linux (Intel)                                                              */
/*----------------------------------------------------------------------------*/

#ifdef LINUX

#include <stdlib.h>
#include <string.h>

#define ARR_0 []                /* gnu-c++  likes this         */

#define SWAP_CODE 0

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#define VARARG   1              /* keine variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif

/*----------------------------------------------------------------------------*/
/* OSF1  OS / Alpha                                                           */
/*----------------------------------------------------------------------------*/

#ifdef HPUX

#include <stdlib.h>
#include <string.h>

#define ARR_0 []                /* cc  likes this         */

#define SWAP_CODE 0

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#define VARARG   1              /* keine variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif

/*----------------------------------------------------------------------------*/
/* OSF1  OS / Alpha                                                           */
/*----------------------------------------------------------------------------*/

#ifdef ALPHA

#include <stdlib.h>
#include <string.h>

#define ARR_0 []                /* cc  likes this         */

#define SWAP_CODE 1

#define NRM_RFI  0              /* RFI-Byteswap gegenueber VMS */

#define VARARG   1              /* keine variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif

/*----------------------------------------------------------------------------*/
/* AIX  OS                                                                    */
/*----------------------------------------------------------------------------*/

#ifdef AIX

#include <stdlib.h>
#include <string.h>

#define ARR_0 []               /* xlC likes this         */

#define SWAP_CODE 0

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#define VARARG   0              /* keine variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif


/*----------------------------------------------------------------------------*/
/* SUN  OS / SOLARIS                                                         */
/*----------------------------------------------------------------------------*/

#if defined SUN || defined SOLARIS

#include <stdlib.h>
#include <string.h>

#define ARR_0 []               /* CC  likes this         */

#define SWAP_CODE 0

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#define VARARG   0              /* keine variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif
/*----------------------------------------------------------------------------*/
/* SINIX                                                                      */
/*----------------------------------------------------------------------------*/

#ifdef SINIX

#define ARR_0 []               /* ULTRIX cc likes this         */

#define SWAP_CODE 1

#define NRM_RFI  0              /* RFI-Byteswap gegenueber VMS */

#define VARARG   0              /* keine variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif
/*----------------------------------------------------------------------------*/
/* OS Ultrix                                                                  */
/*----------------------------------------------------------------------------*/

#ifdef UTX

#define ARR_0 [1]               /* ULTRIX cc likes this         */

#define SWAP_CODE 1

#define NRM_RFI  0              /* kein RFI-Byteswap gegenueber VMS */

#define VARARG   1              /* variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif

/*----------------------------------------------------------------------------*/
/* IBM/VM                                                                     */
/*-------------------------------------------------------------------------*/

#ifdef OSIBMVM

#define ARR_0 [1]               /* ibm cc likes this            */

#define SWAP_CODE 0

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#define VARARG   0              /* keine variable Zahl von Argumenten erlaubt */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdefs.h>

#pragma linkage(pwfinit,OS)
#pragma linkage(pwfopen,OS)
#pragma linkage(pwfread,OS)
#pragma linkage(pwfwrite,OS)
#pragma linkage(pwfclose,OS)

#endif
 
/*----------------------------------------------------------------------------*/
/* FujitsuSiemensComputer FSC (SNI) Reliant UNIX                              */
/*----------------------------------------------------------------------------*/

#ifdef NMP

#include <stdlib.h>
#include <string.h>

#define ARR_0 []                /* Reliant cc likes this */

#define SWAP_CODE 0

#define NRM_RFI  1              /* RFI-Byteswap gegenueber VMS */

#define VARARG   1              /* variable Zahl von Argumenten erlaubt */

#define stderr_Exists

#endif

/*----------------------------------------------------------------------------*/
/*- Daten, die dem Benutzer offengelegt werden                               -*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Standard type definitions                                                  */
/*----------------------------------------------------------------------------*/


typedef char           *char_p,   *strng_p, **char_p_p;
typedef long int       l_int,     *l_int_p;
typedef short int      s_int,     *s_int_p;
typedef char           b_int,     *b_int_p;
typedef float          s_real,    *s_real_p;
typedef double         l_real,    *l_real_p;
typedef unsigned long  q_uint[2];
typedef q_uint                    *q_uint_p;
typedef unsigned long  l_uint,    *l_uint_p;
typedef unsigned short s_uint,    *s_uint_p;
typedef unsigned char  b_uint,    *b_uint_p, **b_uint_pp;
typedef unsigned long  bit_t;
typedef unsigned long  a_uint,    *a_uint_p;
typedef char           *buffer_p;

#ifndef rc_t_Declared
#define rc_t_Declared
typedef unsigned short rc_t;
#endif
typedef rc_t     *rc_t_p;
  

typedef char           strng ARR_0;
typedef unsigned long  l_uint_a ARR_0;
typedef unsigned short s_uint_a ARR_0;
typedef unsigned char  b_uint_a ARR_0;
typedef char           buffer ARR_0;

typedef int	Boolean;
#define TRUE            1
#define FALSE           0
#define NULLPTR         (void *) 0

#define AND             &&
#define OR              ||
#define loop            for (;;)

typedef enum
{
  bio_read, bio_write
} io_op_code_t;

/* Element Descriptor */

typedef struct
{
  s_uint        lg;             /* length of element */
  s_uint        ty;             /* data type code */
  b_uint_p      pt;             /* Pointer to element */
} *st_ed_p, st_ed;

typedef unsigned short   rtc_t;

#define RTC__OK 1
