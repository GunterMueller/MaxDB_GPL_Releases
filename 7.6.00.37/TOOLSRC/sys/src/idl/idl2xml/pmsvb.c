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
#include "idl/idl2xml/ "pms.h"
#include "idl/idl2xml/ "pmsvb.h"



short int WINAPI PmVBExecInit       /* PMS initialization routine     */
(
 long           *pms,               /* pointer to communication area  */
 char           *pmf_p,             /* PMS file pointer               */
 char           *trc_p              /* trace file pointer             */
)
{
  bool       brc;
  
  brc = PmExecInit ((st_pms_p) pms, pmf_p, trc_p);
  
  if (brc) 
    return -1;
  else
    return 0;
}

short int WINAPI PmVBExecPattern    /* pattern execution              */
(
 long           *pms,               /* pointer to communication area  */
 char           *patnam,            /* pattern name                   */
 unsigned char  *matobj,            /* pointer to pattern object      */
 long           *lg,                /* length before and after exec.  */
 long            ml                 /* maximum length of pattern obj. */
)
{
  bool       brc;
  
  brc = PmExecPattern ((st_pms_p) pms, patnam, matobj, lg, ml);

  if (brc) 
    return -1;
  else
    return 0;
}

short int WINAPI PmVBExecFetch      /* get value of PMS variable      */
(
 long           *pms,               /* pointer to communication area  */
 char           *varnam,            /* variable name                  */ 
 unsigned char  *pt,                /* pointer to variable buffer     */
 short          *lg,                /* returned length of value       */
 short           ml                 /* maximum length of value        */
)
{
  bool       brc;

  brc = PmExecFetch ((st_pms_p) pms, varnam, pt, lg, ml);
  
  if (brc) 
    return -1;
  else
    return 0;
             
}

short int WINAPI PmVBExecAssign     /* assign value to PMS variable   */
(
 long           *pms,               /* pointer to communication area  */
 char           *varnam,            /* variable name                  */ 
 unsigned char  *pt,                /* pointer to variable buffer     */
 short           lg                 /* length of value                */
)
{
  bool       brc;
  
  brc = PmExecAssign ((st_pms_p) pms, varnam, pt, lg);
  
  if (brc) 
    return -1;
  else
    return 0;
}

short int WINAPI PmVBExecTerm       /* PMS termination routine        */
(
 long           *pms                /* pointer to communication area  */
)
{
  bool       brc;
  
  brc = PmExecTerm ((st_pms_p) pms); 
 
  
  if (brc) 
    return -1;
  else
    return 0;
}



