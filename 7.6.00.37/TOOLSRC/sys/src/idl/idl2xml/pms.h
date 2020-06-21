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
#ifndef _PMS_H
#define _PMS_H


#define PmsVersion    190

typedef int  bool, *bool_p;

typedef struct                      /* string descriptor              */
{
 unsigned        short lg;          /* length  of string              */
 unsigned        short ml;          /* maximum length of string       */
 unsigned        char *pt;          /* pointer to string              */
} st_sdc, *st_sdc_p;

typedef struct                       /* memory descriptor             */
{
 unsigned        long  llg;          /* length of memory area         */
 unsigned        long  lml;          /* maximum length                */
 unsigned        char *pt;           /* pointer to memory             */
} st_mdc, *st_mdc_p;

typedef struct st_pmv  *PmsEnv;     /* PMS environment                */

typedef struct                      /* communication area             */
{
 PmsEnv          pmv;               /* pointer to PMS environment     */
 unsigned long   cursor;            /* cursor                         */
 unsigned short  errcode;           /* error code (not in use)        */
 char           *errline;           /* pointer to error line          */
} st_pms, *st_pms_p;

bool PmExecInit                     /* PMS initialization routine     */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *pmf_p,             /* PMS file pointer               */
 char           *trc_p              /* trace file pointer             */
);

bool PmExecInitSet
(
 st_pms_p        pms,
 char           *pmf_p,
 char           *trc_p,
 unsigned short  mxnalt,            /* maximum no. of alternations    */
 unsigned short  mxnass,            /* maximum no. of assignments     */
 unsigned short  mxnincarn,         /* maximum no. of incarnations    */
 unsigned short  mxnprec,           /* maximum no. of precursors      */
 unsigned short  mxnsteps,          /* maximum no. of execution steps */
 unsigned short  mxnarb             /* maximum no. of iterations      */
);

bool PmExecMatch                    /* pattern execution              */
(
 st_pms_p        pms,               /* pointer to communication area  */
 st_sdc_p        pmv_matobj_dp,     /* pattern object                 */
 st_sdc_p        pmv_patnam         /* pattern name                   */
);

bool PmExecLongMatch                /* pattern execution              */
(
 st_pms_p        pms,               /* pointer to communication area  */
 st_mdc_p        pmv_matobj_dp,     /* pattern object                 */
 st_sdc_p        pmv_patnam         /* pattern name                   */
);

bool PmExecPattern                  /* pattern execution              */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *patnam,            /* pattern name                   */
 unsigned char  *matobj,            /* pointer to pattern object      */
 unsigned long  *lg,                /* length before and after exec.  */
 unsigned long   ml                 /* maximum length of pattern obj. */
);

bool PmExecGetVar                   /* get value of PMS variable      */
(
 st_pms_p        pms,               /* pointer to communication area  */
 st_sdc_p        r_id_dp,           /* descriptor of variable name    */
 st_sdc_p        w_var_dp           /* descriptor of variable value   */
);

bool PmExecFetch                    /* get value of PMS variable      */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *varnam,            /* variable name                  */ 
 unsigned char  *pt,                /* pointer to variable buffer     */
 unsigned short *lg,                /* returned length of value       */
 unsigned short  ml                 /* maximum length of value        */
);

bool PmExecAssign                   /* assign value to PMS variable   */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *varnam,            /* variable name                  */ 
 unsigned char  *pt,                /* pointer to variable buffer     */
 unsigned short lg                  /* length of value                */
);

bool PmExecTerm                     /* PMS termination routine        */
(
 st_pms_p        pms                /* pointer to communication area  */
);

bool PmdDInit                       /* initilization of a descriptor  */
(
 st_sdc_p        dp,                /* descriptor to be initiated     */
 unsigned char  *pt,                /* pointer to buffer              */
 unsigned short  ml,                /* maximum length of buffer       */
 char           *cp                 /* initiating string              */
);

bool PmBufferCreate                 /* get value of PMS variable      */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *filnam,            /* file name                      */ 
 char           *suffix             /* optional suffix string         */ 
);

bool PmBufferWrite                  /* get value of PMS variable      */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *filnam,            /* file name                      */ 
 char           *suffix             /* removal of suffix string       */ 
);

bool PmFileObject                   /* build pattern object from file */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *filnam,            /* file name                      */ 
 st_sdc_p        pmv_matobj_dp      /* pattern object                 */
);

bool PmBufferMatch                  /* pattern execution              */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *patnam             /* pattern name                   */
);


#endif


