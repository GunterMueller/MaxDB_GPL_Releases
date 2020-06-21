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
#include "idl/idl2xml/pen00.h"
#include "idl/idl2xml/mam.h"
#include "idl/idl2xml/rte.h" 
#include "idl/idl2xml/pms00.h" 
#include "idl/idl2xml/pmexec.h"
#include "idl/idl2xml/pmmadm.h"
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmdesc.h" 
#include "idl/idl2xml/pmstck.h"

/*--------------------------------------------------------------------------*/ 
/* initialize pattern match                                                 */
/*--------------------------------------------------------------------------*/ 

bool PmtPmvinit
(
 st_pmv_p   *pmv_p,
 us_int      mxnalt,            /* maximum no. of alternations    */
 us_int      mxnass,            /* maximum no. of assignments     */
 us_int      mxnincarn,         /* maximum no. of incarnations    */
 us_int      mxnprec,           /* maximum no. of precursors      */
 us_int      mxnarb             /* maximum no. of iterations      */
)
{
  ub_int_p   pt;
  ul_int     alt_lg;
  ul_int     incarn_lg;
  ul_int     prec_lg;
  ul_int     preb_lg;
  ul_int     ass_lg; 
  ul_int     arb_lg; 
  ul_int     str_lg; 
  ul_int     vt_size;
  us_int     pmv_lg;
  us_int     ix;
  st_pmv_p   pmv;
  ub_int     lower[26];
  ub_int     upper[26];
  

  pmv_lg    = sizeof (st_pmv);
  pmv       = (st_pmv_p) RteStdMalloc (pmv_lg);

  if (pmv == 0) return FALSE;

  memset (pmv, 0, sizeof (st_pmv));

  pmv->errline_lg   = pmc_mxerrline;
  pmv->cctstr_d.ml  = 1024;
  pmv->cctstr_d.pt  = RteStdRealloc (pmv->cctstr_d.pt, pmv->cctstr_d.ml);

  memcpy (lower, "abcdefghijklmnopqrstuvwxyz", 26);
  memcpy (upper, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26);
  
  for (ix = 0; ix < 26; ix++)
    {
     pmv->ultab[lower[ix]] = upper[ix];
     pmv->ultab[upper[ix]] = lower[ix];
    }
  
  
  if (0 == mxnalt) mxnalt = pmc_mxnalt;
  alt_lg    = sizeof (pmt_matchpos) * mxnalt;
  
  if (0 == mxnincarn) mxnincarn = pmc_mxnincarn;
  incarn_lg = sizeof (pmt_matchpos) * mxnincarn;
  
  if (0 == mxnass) mxnass = pmc_mxnass;  
  ass_lg    = sizeof (pmt_assdef)   * mxnass; 
  
  if (0 == mxnprec) mxnprec = pmc_mxnprec;
  prec_lg   = sizeof (ul_int)       * mxnprec;
  str_lg    = sizeof (ul_int)       * mxnprec;
  preb_lg   = sizeof (us_int)       * mxnprec;

  if (0 == mxnarb) mxnarb = pmc_mxnarb;  
  arb_lg    = sizeof (pmt_arbdef)   * mxnarb; 

  pt = 0;
  pmv->stk.alt = RteStdRealloc (pt, alt_lg);
  if (0 == pmv->stk.alt) return FALSE;
  pt = 0;
  pmv->stk.ret = RteStdRealloc (pt, incarn_lg);
  if (0 == pmv->stk.ret) return FALSE;
  pt = 0;
  pmv->stk.prec = RteStdRealloc (pt, prec_lg);
  if (0 == pmv->stk.prec) return FALSE;
  pt = 0;
  pmv->stk.precstr = RteStdRealloc (pt, str_lg);
  if (0 == pmv->stk.precstr) return FALSE;
  pt = 0;
  pmv->stk.preb = RteStdRealloc (pt, preb_lg);
  if (0 == pmv->stk.preb) return FALSE;
  pt = 0;
  pmv->stk.ass =  RteStdRealloc (pt, ass_lg);
  if (0 == pmv->stk.ass) return FALSE;
  pt = 0;
  pmv->stk.arb =  RteStdRealloc (pt, arb_lg);
  if (0 == pmv->stk.arb) return FALSE;

  pmv->stk.mxalt    = mxnalt;
  pmv->stk.mxincarn = mxnincarn;
  pmv->stk.mxprec   = mxnprec;
  pmv->stk.mxass    = mxnass;
  pmv->stk.mxarb    = mxnarb;

  vt_size   = sizeof (st_vartab);
  pmv->vt_p = (st_vartab_p) RteStdMalloc (vt_size);

  if (0 == pmv->vt_p) return FALSE;

  memset (pmv->vt_p, 0, sizeof (st_vartab));
 
  pmv->vt_p->mc = pcc_mxnvar;

  *pmv_p = pmv;

  return TRUE;
}
