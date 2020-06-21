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
#ifndef _PMS00_h
#define _PMS00_h


#define pce_err_mxlpat    101  /* pattern has to many nodes    */
#define pce_err_mxnpat    102  /* to many patterns for table   */
#define pce_err_mxnrpl    103  /* to many replacement-def's    */
#define pce_err_invrop    104  /* invalid RPN-Operation        */
#define pce_err_mxnrarg   105  /* to many RPN-Args on Stack    */
#define pce_err_invsyn    106  /* invalid pattern-syntax       */
#define pce_err_pntfnd    107  /* pattern not in table         */
#define pce_err_uarest    108  /* RPN-Stack not exhausted      */
#define pce_err_mxnvar    109  /* to many variables for table  */
#define pce_err_mxnmlg    110  /* variable name too long       */
#define pce_err_mxvllg    111  /* value too long for entry     */
#define pce_err_mxvaex    112  /* variable exists already      */
#define pce_err_slfref    113  /* pattern referenes itself     */


#define  pmc_mxerrline   1024  /* max. length of error line    */
#define  pmc_mxline      8192  /* max. length of input line    */
#define  pmc_mxnincarn   3000  /* max. recurs. incarnations    */
#define  pmc_mxnalt      6000  /* max. alternations in stack   */
#define  pmc_mxnass      1500  /* max. assignments             */
#define  pmc_mxnarb      1500  /* max. iterations              */
#define  pmc_mxnprec      500  /* max. precursors              */
#define  pcc_mxlpat       500  /* max. nodes per pattern       */
#define  pcc_mxnpat        30  /* Max. pattern / table         */
#define  pcc_mxnrarg       10  /* max. RPN variables in stack  */
#define  pcc_mxnrpl        50  /* max. replacement definitions */



typedef unsigned char  ub_int, *ub_int_p;
typedef unsigned short us_int, *us_int_p;
typedef unsigned long  ul_int, *ul_int_p;
#ifndef Gen00Def    
typedef short          s_int,  *s_int_p;
typedef long           l_int,  *l_int_p;
typedef char           *char_p;
#endif

#ifndef _PMS_H
typedef int        bool,   *bool_p;

typedef struct                       /* string descriptor             */
{
 us_int         lg;                  /* length  of string             */
 us_int         ml;                  /* maximum length of string      */
 ub_int_p       pt;                  /* pointer to string             */
} st_sdc, *st_sdc_p;

typedef struct                       /* memory descriptor             */
{
 ul_int         llg;                 /* length of memory area         */
 ul_int         lml;                 /* maximum length                */
 ub_int_p       pt;                  /* pointer to memory             */
} st_mdc, *st_mdc_p;

#endif

#define  stc_len_sshort 64
#define  stc_len_snorm  256
#define  stc_len_slong  512
#define  stc_mxnteps    50      /* max. entries in table */
#define  max_uint       65535

#define  rpn_ea         2
#define  rpn_eo         3
#define  rpn_eu         4
#define  rpn_ee         5
#define  rpn_ef         6
#define  rpn_eg         7
#define  rpn_eh         8

typedef char   stt_sshort[ stc_len_sshort + 1];
typedef char   stt_snorm [ stc_len_snorm + 1];
typedef char   stt_slong [ stc_len_slong + 1];

typedef stt_slong   * stt_strptr;

typedef struct st_pmv  *PmvEnv;

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

#define SWAP_CODE 1

#endif 

#define  TRUE           1
#define  FALSE          0

#endif 
