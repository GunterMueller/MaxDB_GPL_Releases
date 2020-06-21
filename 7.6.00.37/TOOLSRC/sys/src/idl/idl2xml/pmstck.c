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
#include "idl/idl2xml/pmradm.h" 
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmdesc.h" 
#include "idl/idl2xml/pmstck.h"
#include "idl/idl2xml/pmsprm.h" 
#include "idl/idl2xml/pmsmsg.h"
#include "idl/idl2xml/pmprim.h" 

#define M__NAME "PMSST"

void PmeAssign
(   
 st_mam_p     v_mam_p,
 st_pmv_p     pmv,
 st_sdc_p     matobj_dp,
 us_int       prec,
 us_int       postc,
 st_sdc_p     str_dp,
 us_int       substno
);

/*--------------------------------------------------------------------------*/ 
/* pop alternative from stack                                               */
/*--------------------------------------------------------------------------*/ 

void PmfPopAlt 
(
 st_pmv_p     pmv,
 ul_int_p     altern
)
{
  pmt_matchpos_p stk_alt;
  pmt_arbdef_p   stk_arb;
  
  if (pmv->stk.ctalt == 0)
    {
     _NOMRAL (pmv->errline, s_error);
    }
  else
    {
     stk_alt = pmv->stk.alt + pmv->stk.ctalt - 1;
     
     *altern           = stk_alt->node;
     pmv->curcall      = stk_alt->callpat;
     pmv->curpat       = stk_alt->pattern;
     pmv->cursor       = stk_alt->precurs;
     pmv->buf_ix       = stk_alt->prebuf_ix;
     pmv->curincarn    = stk_alt->incarn;
     pmv->stk.ctincarn = stk_alt->mxincarn; /* here free incarnations */
     pmv->stk.ctarb    = stk_alt->ctarb;
     pmv->stk.ctalt--;
	 if (pmv->stk.ctarb > 0)
	   {
		 stk_arb         = pmv->stk.arb + pmv->stk.ctarb - 1;
       stk_arb->for_ct = stk_alt->for_ct;
	   }
    }
}    
    

/*--------------------------------------------------------------------------*/ 
/* Return from pattern call                                                 */
/*--------------------------------------------------------------------------*/ 

void PmfPopRet 
(
 st_pmv_p pmv
) 
{
  pmt_matchpos_p stk_ret;
  pmt_arbdef_p   stk_arb;

  while ((pmv->curnode == pmn__nil) && (pmv->curincarn > 0)) /* ...for multiples return !!! */
    {
     stk_ret = pmv->stk.ret + pmv->curincarn - 1;

     pmv->curincarn = stk_ret->incarn;
     pmv->curcall   = stk_ret->callpat;
     pmv->curpat    = stk_ret->pattern;
     pmv->curnode   = stk_ret->node;  
     pmv->stk.ctarb = stk_ret->ctarb;
 	 if (pmv->stk.ctarb > 0)
	   {
		stk_arb         = pmv->stk.arb + pmv->stk.ctarb - 1;
        stk_arb->for_ct = stk_ret->for_ct;
	   }
   }
  /* no, don't release it, may have alternates, see pmf_pop_alt */
}

/*--------------------------------------------------------------------------*/ 
/* push alternative & set successor                                         */
/*--------------------------------------------------------------------------*/ 

bool PmfAltSucc 
(
 st_pmv_p   pmv,
 ul_int     altern,
 ul_int     succ  
)
{
  pmt_matchpos_p stk_alt;
  pmt_arbdef_p   stk_arb;

  if ((altern != pmn__nil) || (pmv->stk.ctalt == 0)) /* fuer ende-marker */ 
    {
     if (pmv->stk.ctalt == pmv->stk.mxalt)
       {
        _MXNALT (pmv->errline, s_error, pmv->stk.ctalt + 1);
        return FALSE;
	    }
     else 
       {
        stk_alt = pmv->stk.alt + pmv->stk.ctalt;
        pmv->stk.ctalt++;

        stk_alt->callpat   = pmv->curcall;
        stk_alt->pattern   = pmv->curpat;
        stk_alt->node      = altern;
        stk_alt->precurs   = pmv->cursor;
        stk_alt->prebuf_ix = pmv->buf_ix;
        stk_alt->mxincarn  = pmv->stk.ctincarn;
        stk_alt->incarn    = pmv->curincarn;
        stk_alt->ctarb     = pmv->stk.ctarb;
 	     if (pmv->stk.ctarb > 0)
	       {
	        stk_arb         = pmv->stk.arb + pmv->stk.ctarb - 1;
            stk_alt->for_ct = stk_arb->for_ct;
	       }
       }
    }
  pmv->succ = succ;
  return TRUE;
}

bool PmfPreCursor 
(
 st_pmv_p   pmv
)
{
  pmt_matchpos_p stk_alt;

  stk_alt = pmv->stk.alt + (pmv->stk.ctalt - 1);
  stk_alt->precurs   = pmv->cursor;
  stk_alt->prebuf_ix = pmv->buf_ix;

  return TRUE;
}
                                 
/*--------------------------------------------------------------------------*/ 
/* iterative break                                                          */
/*--------------------------------------------------------------------------*/ 

bool PmfBreakx 
(
 st_pmv_p   pmv,
 st_sdc_p   str_dp,
 us_int_p   cursor_p,
 st_sdc_p   val_dp
)
{
  ub_int         set[256];
  st_sdc         set_d;
  pmt_matchpos_p stk_alt;
  pmt_arbdef_p   stk_arb;

  set_d.lg = 256;
  set_d.pt = set;
  
  if (!PmpIniSet (val_dp, &set_d))
    {
     pmv->errcode = 0;
     _INTERR (pmv->errline, s_error, "PmIniSet"); 
     return FALSE;
    }
   
  if (PmsBreak (str_dp, cursor_p, &set_d))
    {
     if (pmv->stk.ctalt == pmv->stk.mxalt)
       {
        _MXNALT (pmv->errline, s_error, pmv->stk.ctalt + 1);
        return FALSE;
       } 
     else 
       {
        stk_alt = pmv->stk.alt + pmv->stk.ctalt;
        pmv->stk.ctalt++;

        stk_alt->callpat   = pmv->curcall;
        stk_alt->pattern   = pmv->curpat;
        stk_alt->node      = pmv->curnode;
        stk_alt->precurs   = pmv->cursor + 1;  /* what about buf_ix */
        stk_alt->mxincarn  = pmv->stk.ctincarn;
        stk_alt->incarn    = pmv->curincarn;
        stk_alt->ctarb     = pmv->stk.ctarb;
 	    if (pmv->stk.ctarb > 0)
	      {
	       stk_arb         = pmv->stk.arb + pmv->stk.ctarb - 1;
           stk_alt->for_ct = stk_arb->for_ct;
	      }
        return TRUE;
       }    
    }    
  else
    {
     return FALSE;
    }
}   

/*--------------------------------------------------------------------------*/ 
/* return cursor position                                                   */
/*--------------------------------------------------------------------------*/ 

bool PmfCursor 
(
 st_sdc_p   dum_dp,
 us_int_p   cursor_p,
 us_int_p   val_p
)
{
  *val_p = *cursor_p;
  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* dynamic pattern call                                                     */
/*--------------------------------------------------------------------------*/ 

bool PmfCallPat 
(
 st_pmv_p    pmv,
 us_int      newpat
)
{
  pmt_matchpos_p stk_ret;
  pmt_arbdef_p   stk_arb;

  if (pmv->stk.ctincarn == pmv->stk.mxincarn)
    {
     _MXNINC (pmv->errline, s_error, pmv->stk.ctincarn + 1);
     return FALSE;
    } 
  else
    {
     stk_ret = pmv->stk.ret + pmv->stk.ctincarn;
     pmv->stk.ctincarn++;
     stk_ret->incarn    = pmv->curincarn;
     stk_ret->ctarb     = pmv->stk.ctarb;
     stk_ret->callpat   = pmv->curcall;
     stk_ret->pattern   = pmv->curpat;
     stk_ret->node      = pmv->succ;
 	 if (pmv->stk.ctarb > 0)
	   {
	    stk_arb         = pmv->stk.arb + pmv->stk.ctarb - 1;
       stk_ret->for_ct = stk_arb->for_ct;
	   }
     /* set new node */
     pmv->curincarn = pmv->stk.ctincarn;
     pmv->curcall   = newpat;
     pmv->curpat    = newpat;
     pmv->succ      = pmn__root;

     return TRUE;
    }
}

/*--------------------------------------------------------------------------*/ 
/* push precursor for assignment                                            */
/*--------------------------------------------------------------------------*/ 
                
bool PmfPshPrec 
(
 st_pmv_p    pmv,
 ul_int      cursor,
 us_int      buf_ix
)
{
  if (pmv->stk.ctprec == pmv->stk.mxprec) 
    {
     _MXNPCP (pmv->errline, s_error, pmv->stk.ctprec + 1);
     return FALSE;
    }
  else
    {
     *(pmv->stk.prec + pmv->stk.ctprec) = cursor;
     *(pmv->stk.preb + pmv->stk.ctprec) = buf_ix;
     pmv->stk.ctprec++;
     return TRUE;
    }
}    

/*--------------------------------------------------------------------------*/ 
/* pop precursor for assignment                                             */
/*--------------------------------------------------------------------------*/ 
                
bool PmfPopPrec 
(
 st_pmv_p    pmv
)
{
  if (pmv->stk.ctprec < 1) 
    {
     _NOMRPC (pmv->errline, s_error);
     return FALSE;
    }
  else
    {
     pmv->stk.ctprec--;
     pmv->precurs   = *(pmv->stk.prec + pmv->stk.ctprec); 
     pmv->prebuf_ix = *(pmv->stk.preb + pmv->stk.ctprec); 
     return FALSE;
    }
}



/*--------------------------------------------------------------------------*/ 
/* push precursor of string stack                                           */
/*--------------------------------------------------------------------------*/ 
                
bool PmfPshPrecStr 
(
 st_pmv_p    pmv,
 ul_int      cursorstr
)
{
  if (pmv->stk.ctprecstr == pmv->stk.mxprec) 
    {
     _MXNPCP (pmv->errline, s_error, pmv->stk.ctprecstr + 1);
     return FALSE;
    }
  else
    {
     *(pmv->stk.precstr + pmv->stk.ctprecstr) = cursorstr;
     pmv->stk.ctprecstr++;
     return TRUE;
    }
}    

/*--------------------------------------------------------------------------*/ 
/* pop precursor of string stack                                            */
/*--------------------------------------------------------------------------*/ 
                
bool PmfPopPrecStr 
(
 st_pmv_p    pmv
)
{
  if (pmv->stk.ctprecstr < 1) 
    {
     _NOMRPC (pmv->errline, s_error);
     return FALSE;
    }
  else
    {
     pmv->stk.ctprecstr--;
     pmv->precursstr  = *(pmv->stk.precstr + pmv->stk.ctprecstr); 
     return FALSE;
    }
}



/*--------------------------------------------------------------------------*/ 
/* pop precursor for assignment                                             */
/*--------------------------------------------------------------------------*/ 
                
bool PmfPopPrecImmed 
(
 st_pmv_p    pmv
)
{
  if (pmv->stk.ctprec < 1) 
    {
     _NOMRPC (pmv->errline, s_error);
     return FALSE;
    }
  else
    {
     pmv->stk.ctprec--;
     pmv->precurs   = *(pmv->stk.prec + pmv->stk.ctprec); 
     pmv->prebuf_ix = *(pmv->stk.preb + pmv->stk.ctprec); 
     return TRUE;
    }
}

/*--------------------------------------------------------------------------*/ 
/* get precursor for assignment                                             */
/*--------------------------------------------------------------------------*/ 
                
bool PmfGetPrec 
(
 st_pmv_p    pmv,
 ul_int_p    cursor_p
)
{
  if (pmv->stk.ctprec < 1) 
    {
     _NOMRPC (pmv->errline, s_error);
     return FALSE;
    }
  else
    {
     *cursor_p = *(pmv->stk.prec + pmv->stk.ctprec - 1);
     return TRUE;
    }
}


/*--------------------------------------------------------------------------*/ 
/* push assignment                                                          */
/*--------------------------------------------------------------------------*/ 

bool PmfPshAss 
(
 st_pmv_p   pmv,
 char       oper,
 us_int     subnr,
 st_sdc_p   to_str_dp
)
{
  bool         dum4;
  pmt_assdef_p stk_ass;
  
  if (pmv->stk.ctass == pmv->stk.mxass)
    {
     _MXNASS (pmv->errline, s_error, pmv->stk.mxass);
     return FALSE;
    }
  else 
    {
     stk_ass = pmv->stk.ass + pmv->stk.ctass;
     pmv->stk.ctass++;
 
     stk_ass->incarn   = pmv->curincarn;
     stk_ass->asstyp   = oper;
     stk_ass->subno    = subnr;
     stk_ass->arg_ty   = (ub_int) to_str_dp->ml;
     stk_ass->level    = pmv->stk.ctprec;

	  dum4 = PmfPopPrec ( pmv);
     
     stk_ass->precurs    = pmv->precurs;
     stk_ass->prebuf_ix  = pmv->prebuf_ix;
     stk_ass->postcurs   = pmv->cursor;
     stk_ass->postbuf_ix = pmv->buf_ix;

     if (0 == pmv->prebuf_ix)
       stk_ass->precursor = pmv->precurs;
     else
       stk_ass->precursor = (pmv->b_p + pmv->prebuf_ix - 1)->acc_lg + pmv->precurs;

     if (0 == pmv->buf_ix)
       stk_ass->postcursor = pmv->cursor;
     else
       stk_ass->postcursor = (pmv->b_p + pmv->buf_ix - 1)->acc_lg + pmv->cursor;

     return TRUE;
    }
}
    
/*--------------------------------------------------------------------------*/ 
/* pop assignment                                                           */
/*--------------------------------------------------------------------------*/ 

bool PmfPopAss 
(   
 st_pmv_p    pmv
)
{
  bool brc;
  brc = PmfPshPrec (pmv, 
                    pmv->stk.ass[pmv->stk.ctass - 1].precurs,
                    pmv->stk.ass[pmv->stk.ctass - 1].prebuf_ix );

  if (pmv->stk.ctass < 1) 
    _NOMRAS (pmv->errline, s_error);
  else 
    pmv->stk.ctass--;
  return FALSE;
}

/*--------------------------------------------------------------------------*/ 
/* push string assignment                                                   */
/*--------------------------------------------------------------------------*/ 

bool PmfPshStr 
(
 st_pmv_p   pmv,
 char       oper,
 us_int     subnr,
 st_sdc_p   to_str_dp
)
{
  rc_t         rc;
  bool         dum4;
  pmt_assdef_p stk_ass;
  
  if (pmv->stk.ctass == pmv->stk.mxass)
    {
     _MXNASS (pmv->errline, s_error, pmv->stk.mxass);
     return FALSE;
    }
  else 
    {
     stk_ass = pmv->stk.ass + pmv->stk.ctass;
     pmv->stk.ctass++;
 
     stk_ass->incarn   = pmv->curincarn;
     stk_ass->asstyp   = oper;
     stk_ass->subno    = subnr;
     stk_ass->arg_ty   = (ub_int) to_str_dp->ml;
     stk_ass->level    = pmv->stk.ctprecstr;

	  dum4 = PmfPopPrecStr (pmv);

     pmv->precursstr = pmv->strstack_d.llg;

     rc = PmrStrAdd (&pmv->strstack_d, &pmv->callstr_d);

     pmv->postcursstr = pmv->strstack_d.llg;
     
     stk_ass->precurs    = pmv->precursstr;
     stk_ass->postcurs   = pmv->postcursstr;


     return TRUE;
    }
}
    
/*--------------------------------------------------------------------------*/ 
/* pop string assignment                                                    */
/*--------------------------------------------------------------------------*/ 

bool PmfPopStr 
(   
 st_pmv_p    pmv
)
{
  bool brc;
  brc = PmfPshPrecStr (pmv, 
                       pmv->stk.ass[pmv->stk.ctass - 1].precurs);

  pmv->strstack_d.llg = pmv->stk.ass[pmv->stk.ctass - 1].precurs;
  
  if (pmv->stk.ctass < 1) 
    _NOMRAS (pmv->errline, s_error);
  else
    { 
     pmv->stk.ctass--;
    }
  return FALSE;
}

bool PmfPshArb 
(   
 st_pmv_p    pmv,
 us_int      min_ct,
 us_int      max_ct
)
{
  pmt_arbdef_p stk_arb;

  /*return TRUE; */
  
  if (pmv->stk.ctarb == pmv->stk.mxarb)
    {
     _MXNARB (pmv->errline, s_error);
     return FALSE;
    }
  else 
    {
     stk_arb = pmv->stk.arb + pmv->stk.ctarb;
     pmv->stk.ctarb++;
 
     stk_arb->status   = pcc_first;
     stk_arb->for_ct   = 0;
     stk_arb->min_ct   = min_ct;
     stk_arb->max_ct   = max_ct;
     stk_arb->alt_ct   = pmv->stk.ctalt + 1;
     return TRUE;
    }
}

bool PmfResetArb 
(   
 st_pmv_p    pmv,
 us_int      altern
)
{
  bool           brc;
  us_int         ix;
  pmt_matchpos_p stk_alt;
  pmt_arbdef_p   stk_arb;

  
  stk_arb = pmv->stk.arb + pmv->stk.ctarb - 1;
  if (stk_arb->status == pcc_first)
    {
     brc = TRUE;
	  stk_arb->status = pcc_next;
    }
  else
    {
	  if ((stk_arb->max_ct > 0) || (stk_arb->min_ct > 0))
	    {
        stk_arb->for_ct++;

	 	  if ((stk_arb->min_ct > 0) && 
            (stk_arb->min_ct == stk_arb->for_ct))
		    {
			 if (stk_arb->min_ct == stk_arb->max_ct)
			 {
              stk_arb->status = pcc_max;
		      return FALSE;
			 }
			 else
			 {
              stk_arb->status = pcc_min;
		      return TRUE;
			 }
		    }

	 	  if ((stk_arb->max_ct > 0) && 
            (stk_arb->max_ct == stk_arb->for_ct))
		    {
           stk_arb->status = pcc_max;
		     return FALSE;
		    }
	    }

     stk_alt = pmv->stk.alt + pmv->stk.ctalt - 1;
	  stk_alt--;
     for (ix = pmv->stk.ctalt - 1; ix > 1; ix--, stk_alt--)
	   {
		if ((stk_alt->pattern == pmv->curpat) &&
			(stk_alt->node == altern))
		  {
		   if (stk_alt->precurs < pmv->cursor)
		     {
	          brc = TRUE;
		     }
	       else
	         brc = FALSE;
		   break;
		  }
	   }
    }
  
  return brc;
}

bool PmfPopArb 
(   
 st_pmv_p    pmv,
 bool        exec
)
{
  pmt_arbdef_p   stk_arb;


  if (pmv->stk.ctarb < 1) 
    {
     _NOMRAR (pmv->errline, s_error);
     return FALSE;
	 }
  else
    {
     /*pmv->stk.ctarb--;*/
     stk_arb = pmv->stk.arb + pmv->stk.ctarb - 1;
     if (exec)
       {
        if (stk_arb->alt_ct < pmv->stk.ctalt)
          return FALSE;
       }
	  if (stk_arb->min_ct > 0)
	    {
	     if ((stk_arb->status == pcc_min) ||
            (stk_arb->status == pcc_max))
           return TRUE;
		  if (stk_arb->min_ct > stk_arb->for_ct) 
          {
           return FALSE;
          }
	    }
     return TRUE;
	 }
}
                       
