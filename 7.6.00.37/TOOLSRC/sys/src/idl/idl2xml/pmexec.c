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
#include "idl/idl2xml/pms.h" 
#include "idl/idl2xml/pms00.h" 
#include "idl/idl2xml/pmexec.h"
#include "idl/idl2xml/pmmadm.h"
#include "idl/idl2xml/pmradm.h"
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmdesc.h" 
#include "idl/idl2xml/pmstck.h"
#include "idl/idl2xml/pmprim.h" 
#include "idl/idl2xml/pmlprim.h" 
#include "idl/idl2xml/pmsmsg.h"


#define M__NAME "PMSEX"

bool PmApproxPrep
(
 st_pmv_p    pmv,
 st_exelst_p exe_p
);

bool PmApproxExec
(
 st_pmv_p   pmv
);

bool PmAssoPrep
(
 st_pmv_p    pmv,
 st_exelst_p r_exe_p
);

bool PmCallExec
(
 st_pmv_p   pmv
);
bool PmCallPrep
(
 st_pmv_p    pmv,
 st_exelst_p r_exe_p
);

bool PmAssoExec
(
 st_pmv_p   pmv
);

bool PmExec
(
 st_pms_p   pms,
 ul_int     ml,
 ul_int_p   rl,
 st_sdc_p   pmv_patnam
);

bool PmtPmvinit
(
 st_pmv_p   *pmv_p,
 us_int      mxnalt,            /* maximum no. of alternations    */
 us_int      mxnass,            /* maximum no. of assignments     */
 us_int      mxnincarn,         /* maximum no. of incarnations    */
 us_int      mxnprec,           /* maximum no. of precursors      */
 us_int      mxnarb             /* maximum no. of iterations      */
);

st_fltlst_p PmFlatList 
(
 st_mam_p  mam_p
);

st_fxelst_p PmFlatPat
(
 st_mam_p     mam_p,
 us_int       pat_id
);

/*--------------------------------------------------------------------------*/ 
/* Get variable                                                             */
/*--------------------------------------------------------------------------*/ 

bool PmExecGetVar
(
 st_pms_p    pms,
 st_sdc_p    r_id_dp,
 st_sdc_p    w_var_dp
)
{
  st_varlst_p var_p;
  bool        brc;
  st_pmv_p    pmv;

 pmv = (st_pmv_p) pms->pmv;

 brc = PmmVarFind (&pmv->mam.varcb, &pmv->mam.rescb[PmrNamCb], r_id_dp, &var_p);

 if (brc)
   {
    w_var_dp->pt = var_p->val_d.pt;
    w_var_dp->lg = var_p->val_d.lg;
   }

 return brc;
}

bool PmExecFetch                    /* get value of PMS variable      */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *varnam,            /* variable name                  */ 
 unsigned char  *pt,                /* pointer to variable buffer     */
 unsigned short *lg,                /* returned length of value       */
 unsigned short  ml                 /* maximum length of value        */
)
{
  st_varlst_p var_p;
  bool        brc;
  st_pmv_p    pmv;
  st_sdc      id_d;
  us_int      l; 
   
 pmv             = (st_pmv_p) pms->pmv;
 pmv->errline[0] = 0;
 id_d.lg         = strlen (varnam);
 id_d.ml         = id_d.lg;
 id_d.pt         = (ub_int_p) varnam;
 
 brc = PmmVarFind (&pmv->mam.varcb, &pmv->mam.rescb[PmrNamCb], &id_d, &var_p);

 if (brc)
   {
    if (var_p->val_d.lg > ml) l = ml;
    else                      l = var_p->val_d.lg;
    memcpy (pt, var_p->val_d.pt, l);
    *lg = var_p->val_d.lg;

    if (l < ml) 
      *(pt + l) = 0;
   }

 pms->errline = pmv->errline;
 if (pmv->errline[0] == 0)
   pms->errcode = FALSE;
 else
   pms->errcode = TRUE;

 return brc;
}

bool PmExecAssign                   /* assign value to PMS variable   */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *varnam,            /* variable name                  */ 
 unsigned char  *pt,                /* pointer to variable buffer     */
 unsigned short lg                  /* returned length of value       */
)
{
  st_varlst_p var_p;
  rc_t        rc;
  bool        brc;
  st_pmv_p    pmv;
  st_sdc      id_d;
  st_dsc      val_d;
   
 pmv     = (st_pmv_p) pms->pmv;
 id_d.lg = strlen (varnam);
 id_d.ml = id_d.lg;
 id_d.pt = (ub_int_p) varnam;
 
 brc = PmmVarFind (&pmv->mam.varcb, &pmv->mam.rescb[PmrNamCb], &id_d, &var_p);

 if (brc)
   {
	if (var_p->nocon)
      {
       val_d.lg = lg;
	   val_d.ty = 0;
	   val_d.pt = pt;

       rc = PmmVarReplace (&pmv->mam.varcb, var_p, &val_d);
       return RcRcdOk == rc;
	  }
	else
	  {
	   return 0;
	  }
   }

 return brc;
}
/*--------------------------------------------------------------------------*/ 
/* Terminate PMS environment                                                */
/*--------------------------------------------------------------------------*/ 

bool PmExecTerm
(
 st_pms_p    pms
)
{
/* rc_t           rc; */
 us_int         ix;
 st_pmv_p       pmv;
 st_fltlst_p    flt_p;

  pmv = (st_pmv_p) pms->pmv;

  if (0 == pmv->mam.rescb->lst_d.pt);
 /*   rc = PmmFilClose (&pmv->mam, FilOptNoSave); */
  else
  	RteStdFree (pmv->mam.rescb->lst_d.pt);

  if (0 != pmv->mam.fltcb.lst_d.pt)
    {
	  flt_p = (st_fltlst_p) pmv->mam.fltcb.lst_d.pt;
	  for (ix = 0; ix < pmv->mam.fltcb.ct; ix++, flt_p++)
	    {
        if (0 != flt_p->val_d.pt)
 	       RteStdFree (flt_p->val_d.pt);
	    }
 	  RteStdFree (pmv->mam.fltcb.lst_d.pt);
    }
 

  RteStdFree ((void*) pmv->vt_p);
  RteStdFree ((void*) pmv->stk.alt);
  RteStdFree ((void*) pmv->stk.ret);
  RteStdFree ((void*) pmv->stk.prec);
  RteStdFree ((void*) pmv->stk.precstr);
  RteStdFree ((void*) pmv->stk.preb);
  RteStdFree ((void*) pmv->stk.ass);
  RteStdFree ((void*) pmv->stk.arb);
  RteStdFree ((void*) pmv->cctstr_d.pt);
  RteStdFree ((void*) pmv->strstack_d.pt);

  if (pmv->mam.PatternListMoved) 
	  RteStdFree ((void*) pmv->mam.patcb.lst_d.pt);
  
  RteStdFree ((void*) pmv);

  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* Initialize PMS environment                                               */
/*--------------------------------------------------------------------------*/ 

bool PmExecInitSet
(
 st_pms_p        pms,
 char_p          pmf_p,
 char_p          trc_p,
 unsigned short  mxnalt,            /* maximum no. of alternations    */
 unsigned short  mxnass,            /* maximum no. of assignments     */
 unsigned short  mxnincarn,         /* maximum no. of incarnations    */
 unsigned short  mxnprec,           /* maximum no. of precursors      */
 unsigned short  mxnsteps,          /* maximum no. of execution steps */
 unsigned short  mxnarb             /* maximum no. of iterations      */
)
{
 unsigned short rc;
 st_pmv_p       pmv;
 
 memset (pms, 0, sizeof(st_pms));
 
 PmtPmvinit (&pmv, mxnalt, mxnass, mxnincarn, mxnprec, mxnarb);

 pms->pmv = (PmsEnv) pmv;

 pmv->mxnsteps = mxnsteps;

 memset (&pmv->mam, 0, sizeof (st_mam));

 if (0 == strlen (pmf_p))
   rc = RcOk;
 else
   rc = PmrFilOpen (&pmv->mam, pmf_p);
 
 if (rc != RcFilOk)
   {
    _PMINIT (pmv->errline, s_error, rc, "PmrFilOpen" , pmf_p);
    return FALSE;
   }
 else
   {
    memset (&pmv->mam.varcb, 0, sizeof (st_lstcb));
    rc = PmmTmpCreateNeu (&pmv->mam, 32768, &pmv->mam.rescb[PmrVarCb]);
       
	  if (rc != RcFilOk)
      {
       _PMINIT (pmv->errline, s_error, rc, "PmmDstCreate" , "VAR.TMP");
       return FALSE;
      }
   }

 rc = PmrPatOpen (&pmv->mam);

 pmv->con1_p  = (st_conlst_p) pmv->mam.concb.lst_d.pt;
 pmv->var1_p  = (st_varlst_p) pmv->mam.varcb.lst_d.pt;
 pmv->cct1_p  = (st_cctlst_p) pmv->mam.cctcb.lst_d.pt;
 pmv->set1_p  = (st_setlst_p) pmv->mam.setcb.lst_d.pt;
 pmv->pat1_p  = (st_patlst_p) pmv->mam.patcb.lst_d.pt;
 pmv->flt1_p  = (st_fltlst_p) pmv->mam.fltcb.lst_d.pt;

 
 if (rc != RcOk)
   {
    _PMINIT(pmv->errline, s_error, rc, "PmrPatOpen","No memory available");
    return FALSE;
   }

 if (0 == strlen (trc_p))
   {
    pmv->trcp = 0;
   }
 else
   {
    rc  = RteFilOpen (fil_put, trc_p, &pmv->trcp);
    if ((rc % 2) != 1)
      {
       _PMINIT (pmv->errline, s_error, rc, "Tracefile Open" , trc_p);
       return FALSE;
      }
   }

 return TRUE;
}

bool PmExecInit
(
 st_pms_p    pms,
 char_p      pmf_p,
 char_p      trc_p
)
{
 return PmExecInitSet (pms, pmf_p, trc_p, 0, 0, 0, 0, 0, 0);
}

bool PmExecInitHSet
(
 st_pms_p        pms,
 st_lstcb_p      CbList_p,
 char_p          trc_p,
 unsigned short  mxnalt,            /* maximum no. of alternations    */
 unsigned short  mxnass,            /* maximum no. of assignments     */
 unsigned short  mxnincarn,         /* maximum no. of incarnations    */
 unsigned short  mxnprec,           /* maximum no. of precursors      */
 unsigned short  mxnsteps,          /* maximum no. of execution steps */
 unsigned short  mxnarb             /* maximum no. of iterations      */
)
{
 unsigned short rc;
 unsigned short ix;
 st_pmv_p       pmv;
 st_lstcb_p     Cbp;

 memset (pms, 0, sizeof(st_pms));
 
 PmtPmvinit (&pmv, mxnalt, mxnass, mxnincarn, mxnprec, mxnarb);

 pms->pmv = (PmsEnv) pmv;

 pmv->mxnsteps = mxnsteps;

 memset (&pmv->mam, 0, sizeof (st_mam));

 Cbp = CbList_p;

 for (ix = 1; ix < PmrPermCb; ix++, Cbp++)
   pmv->mam.rescb[ix] = *Cbp;

 Cbp = CbList_p;
 pmv->mam.fcncb = *Cbp;
 pmv->mam.patcb = *(Cbp + 1);
 pmv->mam.concb = *(Cbp + 2);
 pmv->mam.cctcb = *(Cbp + 4);
 pmv->mam.namcb = *(Cbp + 5);
 pmv->mam.setcb = *(Cbp + 6);
 pmv->mam.pdfcb = *(Cbp + 7);

 /* rc = PmrDescAll (&pmv->mam);  */
 
 
 
 rc = PmmTmpCreateNeu (&pmv->mam, 32768, &pmv->mam.rescb[PmrVarCb]);

 if (rc != RcFilOk)
   {
    _PMINIT (pmv->errline, s_error, rc, "PmmDstCreate" , "VAR.TMP");
    return FALSE;
   }

 if (0 == trc_p)
   {
    pmv->trcp = 0;
   }
 else
   {
    rc  = RteFilOpen (fil_put, trc_p, &pmv->trcp);
    if ((rc % 2) != 1)
      {
       _PMINIT (pmv->errline, s_error, rc, "Tracefile Open" , trc_p);
       return FALSE;
      }
   }

 rc = PmrPatOpen (&pmv->mam);

 pmv->con1_p  = (st_conlst_p) pmv->mam.concb.lst_d.pt;
 pmv->var1_p  = (st_varlst_p) pmv->mam.varcb.lst_d.pt;
 pmv->cct1_p  = (st_cctlst_p) pmv->mam.cctcb.lst_d.pt;
 pmv->set1_p  = (st_setlst_p) pmv->mam.setcb.lst_d.pt;
 pmv->pat1_p  = (st_patlst_p) pmv->mam.patcb.lst_d.pt;
 pmv->flt1_p  = (st_fltlst_p) pmv->mam.fltcb.lst_d.pt;

 return TRUE;
}

bool PmExecInitH
(
 st_pms_p    pms,
 st_lstcb_p  CbList_p,
 char_p      trc_p
)
{
 return PmExecInitHSet (pms, CbList_p, trc_p, 0, 0, 0, 0, 0, 0);
}



/*--------------------------------------------------------------------------*/ 
/* update assigment stack after a replacement                               */
/*--------------------------------------------------------------------------*/ 
void PmeUpdAssStack
(
 pmt_assdef_p stk1_ass,
 us_int       iass,
 us_int       start,
 us_int       end,
 l_int        dif_lg,
 ul_int       postc,
 us_int_p     ix_p 
)
{
 us_int       ix;
 us_int       level_alt;
 pmt_assdef_p s_a; 
 bool         nul_upd;
 
 s_a = stk1_ass + *ix_p;
 nul_upd = (s_a->precurs == s_a->postcurs) && ((iass - 1) == *ix_p);
 
 level_alt = end + 1;
 for (ix = 0; ix <= (end - start);)
   {
    s_a = stk1_ass + *ix_p;
    if (s_a->level < level_alt)
      {
       if ((s_a->precurs >= postc) && !nul_upd && (s_a->asstyp != '@'))
         s_a->precurs += dif_lg;
       if ((s_a->postcurs >= postc) && (s_a->asstyp != '@'))
         s_a->postcurs += dif_lg;
       level_alt = s_a->level;
       (*ix_p)++;                 
       ix++;
      }
    else
      {
       PmeUpdAssStack (stk1_ass, iass, level_alt, s_a->level, dif_lg, postc, ix_p);
      }
   }
 
}
/*--------------------------------------------------------------------------*/ 
/* perform a replacement within a pattern object                            */
/*--------------------------------------------------------------------------*/ 

bool PmeObjReplace
(   
 st_mam_p     v_mam_p,
 st_pmv_p     pmv,
 pmt_assdef_p stk_ass,
 us_int       iass
)
{
  l_int          dif_lg;
  ul_int         trail_lg;
  pmt_assdef_p   s_a;
  ub_int_p       pt;
  ub_int_p       from_pt;
  ub_int_p       to_pt;
  ul_int         ix;
  us_int         ix2;
  ul_int         ci;
  ul_int         lg;
  ub_int         ca[5];
  st_sdc         ca_d;
  st_mdc         matobj_d;
  ul_int         prec;
  ul_int         postc;
  ub_int         arg_ty;
  us_int         vcc_id;

  st_conlst_p    con1_p;
  st_varlst_p    var1_p;
  st_namlst_p    nam1_p;
  st_cctlst_p    cct1_p;

  st_conlst_p    con_p;
  st_varlst_p    var_p;
  st_namlst_p    nam_p;
  st_cctlst_p    cct_p;
  st_cctelm_p    elm_p;

  con1_p  = (st_conlst_p) v_mam_p->concb.lst_d.pt;
  var1_p  = (st_varlst_p) v_mam_p->varcb.lst_d.pt;
  nam1_p  = (st_namlst_p) v_mam_p->namcb.lst_d.pt;
  cct1_p  = (st_cctlst_p) v_mam_p->cctcb.lst_d.pt;

  prec    = stk_ass->precurs;
  postc   = stk_ass->postcurs;
  arg_ty  = stk_ass->arg_ty;
  vcc_id  = stk_ass->subno;

  matobj_d.llg = pmv->b_p->acc_lg;
  matobj_d.lml = pmv->ml;
  matobj_d.pt  = pmv->b_p->blk_p;


  /* determine length of new string */


  switch (arg_ty)
    {
     case argtycon:
       {
        con_p = con1_p + (vcc_id - 1);
        lg    = con_p->val_d.lg;
        pt    = con_p->val_d.pt;
        break;
       }

     case argtyvar:
       {
        var_p = var1_p + (vcc_id - 1);
        lg    = var_p->val_d.lg;
        pt    = var_p->val_d.pt;
        break;
       }

     case argtycct:
       {
        cct_p = cct1_p + (vcc_id - 1);
        lg = 0;
        elm_p = (st_cctelm_p) cct_p->cct_d.pt;
        for (ix = 0; ix < cct_p->ct; ix++, elm_p++)
          {
           if (argtycon == elm_p->arg_ty)
             {
              con_p  = con1_p + (elm_p->vcc_id - 1);
              lg    += con_p->val_d.lg;
             }
           else
             {
              var_p = var1_p + (elm_p->vcc_id - 1);
              nam_p = nam1_p + (var_p->nm_id - 1);
              if (('$' == *nam_p->nam_d.pt) && (2 == nam_p->nam_d.lg))
                {
                 if (0 == prec)
                   {
                    ca[4] = '0';
                    ix2    = 3;
                   }
                 else
                   {
                    ci  = prec;
                    ix2  = 4;
                    while (ci > 0)
                      {
                       ca[ix2] = (ub_int) ((ci % 10) + '0');
                       ci = ci / 10;
                       ix2--;
                      }
                   }
                 ca_d.lg = 4 - ix2;
                 ca_d.pt = (unsigned char*)&ca[ix2 + 1];
                 lg += ca_d.lg;
                }
              else  
                {
                 lg += var_p->val_d.lg;
                }
             }
          }
        break;
       }
    }

  dif_lg = lg - (postc - prec);

  if ((matobj_d.llg + dif_lg) > matobj_d.lml)
    {
     _MXLBUF (pmv->errline, s_error);
	 return FALSE;
    }

  if (dif_lg != 0) 
    {
     /* shift trail to the left */
     
     trail_lg = matobj_d.llg - postc;
     if (dif_lg < 0) 
       {
         /* PTS FF 1120291 2003-Jan-24 */
         memmove (matobj_d.pt + postc + dif_lg, matobj_d.pt + postc,
             trail_lg);
       }
     
     /* shift trail to the right */
     else
       {
        from_pt = matobj_d.pt + (matobj_d.llg - 1);
        to_pt   = from_pt + dif_lg;
        
        for (ix = trail_lg; ix > 0; ix--, from_pt--, to_pt--)
          *to_pt = *from_pt;
       }
    }

  if (argtycct == arg_ty)
    {
     to_pt = matobj_d.pt + prec;
     elm_p = (st_cctelm_p) cct_p->cct_d.pt;

     for (ix = 0; ix < cct_p->ct; ix++, elm_p++)
       {
        if (argtycon == elm_p->arg_ty)
          {
           con_p = con1_p + (elm_p->vcc_id - 1);
           lg    = con_p->val_d.lg;
           pt    = con_p->val_d.pt;
          }
        else
          {
           var_p = var1_p + (elm_p->vcc_id - 1);
           nam_p = nam1_p + (var_p->nm_id - 1);
           if (('$' == *nam_p->nam_d.pt) && (2 == nam_p->nam_d.lg))
             {
              lg    = ca_d.lg;
              pt    = ca_d.pt;
             }
           else
             {
              lg    = var_p->val_d.lg;
              pt    = var_p->val_d.pt;
             }
          }
        memcpy (to_pt, pt, lg);
        to_pt += lg;
       }
    }
  else
    {
     memcpy (matobj_d.pt + prec, pt, lg);
    }

  matobj_d.llg      += dif_lg;
  pmv->b_p->acc_lg  = matobj_d.llg;

  /* adjust the other assignments */
  pmv->cursor += dif_lg;
  
  s_a = pmv->stk.ass;

if (TRUE)
  {
   ix2 = 0;
   while (ix2 < pmv->stk.ctass)
     {
      s_a = pmv->stk.ass + ix2;
      PmeUpdAssStack (pmv->stk.ass, iass, 1, s_a->level,  dif_lg, postc, &ix2); 
     }
  }
else  
  {
   for (ix2 = 0; ix2 < pmv->stk.ctass; ix2++, s_a++)
    {
     if (s_a->asstyp == '@') 
       {
        continue;
       }
     if (s_a->precurs >= postc)
        s_a->precurs += dif_lg;
     if (s_a->postcurs >= postc)
        s_a->postcurs += dif_lg;
    }  
}
  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* perform a replacement                                                    */
/*--------------------------------------------------------------------------*/ 

bool PmeReplace
(   
 st_mam_p     v_mam_p,
 st_pmv_p     pmv,
 pmt_assdef_p stk_ass,
 us_int       iass
)
{
 return PmeObjReplace (v_mam_p, pmv, stk_ass, iass);
}
/*--------------------------------------------------------------------------*/ 
/* perform an assigment                                                     */
/*--------------------------------------------------------------------------*/ 

void PmeAssign
(   
 st_mam_p     v_mam_p,
 st_pmv_p     pmv,
 pmt_assdef_p stk_ass
)
{
  rc_t           rc;
  us_int         vcc_id;
  st_dsc         val_d;
  st_varlst_p    var1_p;
  st_varlst_p    var_p;
  us_int         ix;
  ul_int         ci;
  char           ca[10];
  
  var1_p = (st_varlst_p) v_mam_p->varcb.lst_d.pt;
  vcc_id = stk_ass->subno;
  var_p  = var1_p + (vcc_id - 1);

  if (pmv->bufmatch)
    {

    }
  else
    {
     switch (stk_ass->asstyp)
       {
        case '$':
          {
           ci = stk_ass->postcurs;
           ix = sizeof (ca);
           for(;;)
             {
              ix--;
		      /* 08-08-1997 FJH */
              ca[ix] = (char) (ci % 10) + '0';
              ci = ci / 10;
              if (ci == 0) break;
             }
           val_d.lg = sizeof (ca) - ix;
           val_d.pt = /*PTS 1109360*/ (unsigned char*)&ca[ix];
           rc = PmmVarReplace (&v_mam_p->varcb, var_p, &val_d);
           break;
          }

        case '/':
          rc = PmmVarIncr (&v_mam_p->varcb, var_p);
          if (rc != RcRcdOk)
              _INTERR (pmv->errline, s_error, "variable not numeric");
          break;
        
		case '^':
          rc = PmmVarDecr (&v_mam_p->varcb, var_p);
          if (rc != RcRcdOk)
              _INTERR (pmv->errline, s_error, "variable not numeric");
          break;

        default:
          {
           if ((stk_ass->postcurs - stk_ass->precurs) <= max_uint)
             {
              val_d.lg = (us_int) (stk_ass->postcurs - stk_ass->precurs);
              if (stk_ass->asstyp == '@')
			       val_d.pt = pmv->strstack_d.pt + stk_ass->precurs;
			     else
			       val_d.pt = pmv->b_p->blk_p + stk_ass->precurs;
              rc = PmmVarReplace (&v_mam_p->varcb, var_p, &val_d);
             }
           else
             {
              _INTERR (pmv->errline, s_error, "assignment of long variables ");
             }
          }
     
       } 
    }
}

rc_t PmImmedAssign
(   
 st_pmv_p     pmv,
 st_varlst_p  var_p
)
{
 bool           brc;
 rc_t           rc;
 st_dsc         val_d;
 ul_int         precurs;

 brc = PmfPopPrec (pmv);

 precurs  = pmv->precurs;
 val_d.lg = (us_int) (pmv->cursor - precurs);
 val_d.pt = pmv->pt + precurs;
 
 rc = PmmVarReplace (&pmv->mam.varcb, var_p, &val_d);
 
 return rc = RcOk;
}

bool PmExec 
(
 st_pms_p   pms,
 ul_int     ml,
 ul_int_p   rl,
 st_sdc_p   pmv_patnam
)
{
  bool           brc;
  bool           elmbrc;
  bool           modflg;
  bool           sensitive;
  bool           anchored;
  bool           infront;
  bool           stop;
  bool           first;
  bool           exec;
  bool           int_err;
  us_int         ix;
  us_int         ix2;
  us_int         arg_ty;
  us_int         vcc_id;
  us_int         cct_id;
  ub_int         fcn_id;
  us_int         ct; 
  us_int         sl; 
  ul_int         li;
  ul_int         tmpcurs;
  ul_int         begcurs;
  ul_int         curcurs;
  ul_int         alt;
  ul_int         cur;
  us_int         tmpix;
  st_sdc_p       set_dp;
  st_sdc         DMY; 
  st_sdc         nm_d; 
  us_int         iass;
  us_int         ipas;
  s_int          incarn;
  pmt_assdef_p   stk_ass;
  st_d           buf_d;
  ub_int         buf [80];
  ub_int         nam [80];
  ub_int         val [80];
  us_int         rtc;
  st_mam_p       mam_p;
  st_pmv_p       pmv;
  st_dsc         val_d; 


  st_patlst_p    pat_p;
  st_fltlst_p    flt_p;
  st_conlst_p    con_p;
  st_varlst_p    var_p;
  st_cctlst_p    cct_p;
  st_cctelm_p    elm_p;
  st_setlst_p    set_p;
  st_exelst_p    exe_p;
  st_exelst_p    nxt_p;
  st_fxelst_p    fxe_p;
  st_cctlst_p    setcct_p;
  st_cctelm_p    setelm_p;

  char *         chr_p;

  /* initialization */

  pmv               = (st_pmv_p) pms->pmv;
  mam_p             = &pmv->mam;
  buf_d.lg          = sizeof (buf);
  buf_d.pt          = (ub_int_p) buf;

  if (pmv->deb.status == deb_not_init)
    {  
      pmv->cursor         = pms->cursor;
      pmv->savcursor      = pmv->cursor;
      pmv->savbuf_ix      = pmv->buf_ix;
      pmv->success        = FALSE;
      pmv->eomatch        = FALSE;
      pmv->stk.ctalt      = 0;
      pmv->stk.ctincarn   = 0;
      pmv->stk.ctprecstr  = 0;
      pmv->stk.ctprec     = 0;
      pmv->stk.ctass      = 0;
      pmv->stk.ctarb      = 0;
      pmv->curcall        = 0;
      pmv->curpat         = 0;
      pmv->curnode        = pmn__root;
      pmv->curincarn      = 0;
      pmv->precurs        = 0;
      pmv->precursstr     = 0;
      pmv->prebuf_ix      = 0;
      pmv->errline[0]     = 0;
      pmv->errcode        = 0;
      pmv->ml             = ml;
      pmv->steps          = 0;
      pmv->strstack_d.llg = 0;
      pmv->callstr_d.llg  = 0;
      pmv->callstr_d.pt   = 0;

      pmv->maxsteps     =  1000000 * pmv->mxnsteps;
    
      brc = PmfAltSucc (pmv, pmn__nil, pmn__nil);
      if (!brc) return FALSE;
    
      /* set cur.pattern */
      nm_d.lg = pmv_patnam->lg;
      nm_d.ml = pmv_patnam->lg;
      nm_d.pt = pmv_patnam->pt;
      
      brc = PmmPatFind (&mam_p->patcb, &mam_p->namcb, &nm_d, &pat_p);
      if (brc)
        {
         if (pmv->flt1_p == 0)
           {
            pmv->flt1_p = PmFlatList (mam_p);
            if (0 == pmv->flt1_p)
              {
               _INTERR (pmv->errline, s_error, "No space available for flat list");
               pmv->curpat = 0;
               pmv->eomatch = TRUE;
               pms->errcode = TRUE;
               return FALSE;
              }
           }
         
         flt_p        = pmv->flt1_p + pat_p->id - 1;
         pmv->fxe1_p  = (st_fxelst_p) flt_p->val_d.pt;
         
         if (pmv->fxe1_p == 0)
           {
            pmv->fxe1_p = PmFlatPat (mam_p, pat_p->id);
            if (0 == pmv->fxe1_p)
              {
               _INTERR (pmv->errline, s_error, "No space available for flat pattern");
               pmv->curpat = 0;
               pmv->eomatch = TRUE;
               pms->errcode = TRUE;
               return FALSE;
              }
           }
           
         pmv->curcall = pat_p->id;
         pmv->curpat  = pat_p->id;
        }
      else
        {
         memcpy (buf, nm_d.pt, nm_d.lg);
         buf[nm_d.lg] = 0;
         
         _PATNFD (pmv->errline, s_error, buf);
         pmv->curpat = 0;
         pmv->eomatch = TRUE;
        }
      if (pmv->trcp != 0)
        {
         memcpy (buf, pmv_patnam->pt, pmv_patnam->lg);
         buf[pmv_patnam->lg] = '>';
         buf_d.lg = pmv_patnam->lg + 1;
         
         rtc = RteFilRecord (fil_put, pmv->trcp, 78, &buf_d);
         buf_d.pt = pmv->b_p->blk_p;
         if (ml > max_uint)
           {
            sl       = max_uint;
            buf_d.lg = sl - 2;
            sl       = (us_int) ml;
           }
         else
           {
            sl       = (us_int) ml;
            buf_d.lg = (us_int) pmv->b_p->acc_lg;
           }
         while (NULL != (chr_p = strchr(/*PTS 1109360*/ (char*)buf_d.pt, 10))) *chr_p = ' ';
         while (NULL != (chr_p = strchr(/*PTS 1109360*/ (char*)buf_d.pt, 13))) *chr_p = ' ';
         rtc = RteFilRecord (fil_put, pmv->trcp, sl, &buf_d);
         buf_d.pt = buf;
        }
      pmv->deb.status = deb_init;
    }
  /* walk thru pattern */
  
  stop = FALSE;

  while ((!pmv->eomatch) && (!stop))
    { 
     if (pmv->deb.step_ty == deb_single_step)
       stop = TRUE;

	 pmv->steps++;
	 if ((pmv->maxsteps != 0) && (pmv->steps == pmv->maxsteps))
	   {
		_MXNSTP (pmv->errline, s_warning, pmv->mxnsteps);
		break; 
	   }

     fxe_p = pmv->fxe1_p + (pmv->curnode - 1);
     
     pat_p = pmv->pat1_p + (fxe_p->pat_id - 1);
     exe_p = (st_exelst_p)(pat_p->val_d.pt) + (fxe_p->exe_ix - 1);

     pmv->deb.info.nxt_curs  = 0;
     nxt_p = exe_p + 1;
     for (ix = fxe_p->exe_ix + 1; ix < pat_p->ct; ix++, nxt_p++)
       {
        if (nxt_p->curs != 0)
          {
           pmv->deb.info.nxt_curs = nxt_p->curs;
           break;
          }
       }

     pmv->deb.info.pat_id    = fxe_p->pat_id;
     pmv->deb.info.pat_ix    = fxe_p->exe_ix;
     pmv->deb.info.exe_curs  = exe_p->curs;
     pmv->deb.info.precursor = pmv->cursor;
     pmv->deb.info.nam_id    = pat_p->nm_id;


     brc = PmfAltSucc (pmv,  fxe_p->alt, fxe_p->succ);
     if (!brc)
       {
        _INTERR (pmv->errline, s_error, "Error in PmfAltSucc");
        break;
       }

     int_err = FALSE;

     switch (exe_p->fcn_ty)
       {
        case fcntypap:
          {
           switch (exe_p->fcn_id)
             {
              case fcnidpshprec:
                 brc = PmfPshPrec (pmv, pmv->cursor, pmv->buf_ix); 
                 break;
              case fcnidpopprec:
                 brc = PmfPopPrec (pmv); 
                 break;
              case fcnidpopass:
                 brc = PmfPopAss  (pmv); 
                 break;
              case fcnidpshstrprec:
                 brc = PmfPshPrecStr (pmv, pmv->strstack_d.llg); 
                 break;
              case fcnidpopstrprec:
                 brc = PmfPopPrecStr (pmv); 
                 break;
              case fcnidpopstr:
                 brc = PmfPopStr  (pmv); 
                 break;
              case fcnidprecurs:
                 brc = PmfPreCursor  (pmv); 
                 break;
              case fcnidpopimmed:
                 brc = PmfPopPrecImmed (pmv); 
                 break;
             }
           break;
          }

        case fcntypmp:
        case fcntycset:
        case fcntynset:
          {
           set_p  = pmv->set1_p + (exe_p->vcc_id - 1);
           set_dp = (st_sdc_p) &set_p->set_d;
           arg_ty = set_p->arg_ty;
           vcc_id = set_p->vcc_id;        

           modflg = FALSE;
             
           if (argtyvar == arg_ty)
             { 
              var_p  = pmv->var1_p + (vcc_id - 1);
              modflg = set_p->ctmod == var_p->ctmod;
              set_p->ctmod = var_p->ctmod;
             }

           if (argtycct == arg_ty)
             { 
              cct_p = pmv->cct1_p + (vcc_id - 1);
              ct    = cct_p->ct;
              elm_p = (st_cctelm_p) cct_p->cct_d.pt;
              for (ix = 0; ix < ct; ix++, elm_p++)
                {
                 if (argtyvar == elm_p->arg_ty)
                   {
                    var_p = pmv->var1_p + (elm_p->vcc_id - 1);
                    if (!modflg) modflg = elm_p->ctmod != var_p->ctmod;
                    elm_p->ctmod = var_p->ctmod;
                   }
                }
             }

           if (modflg)
             {
              brc = PmmSetSet (mam_p, vcc_id, arg_ty, (st_dsc_p) set_dp, 
                               pmv->con1_p, pmv->var1_p, pmv->cct1_p);
             }
           switch (exe_p->fcn_ty)
           {
            case fcntypmp:
             {
               switch (exe_p->fcn_id)
                 {
                  case fcnidany:
                     brc = PmlAny (pmv->pt, pmv->lg, &pmv->cursor, set_dp); 
                     break;
                  case fcnidbreak:
                     brc = PmlBreak (pmv->pt, pmv->lg, &pmv->cursor, set_dp);
                     break;
                  case fcnidbreakx:
                     brc = PmlBreakx (pmv, set_dp);
                     break;
                  case fcnidnotany:
                     brc = PmlNotany (pmv->pt, pmv->lg, &pmv->cursor, set_dp); 
                     break;
                  case fcnidspan:
                     brc = PmlSpan (pmv->pt, pmv->lg, &pmv->cursor, set_dp); 
                     break;
                  case fcnidbrake:
                     brc = PmlBrake (pmv->pt, pmv->lg, &pmv->cursor, set_dp); 
                     break;
                  case fcnidnotbrake:
                     brc = PmlNotbrake (pmv->pt, pmv->lg, &pmv->cursor, set_dp); 
                     break;
                  default: 
                     brc = FALSE;
                 }
              break;
             }
            case fcntycset:
             {
              anchored  = FALSE;
              infront   = FALSE;
              sensitive = FALSE;
              
              switch (exe_p->fcn_id)
                {
                 case fcnid_a:
                    anchored  = TRUE;
                    break;
                 case fcnid_b:
                    infront   = TRUE;
                    break;
                 case fcnid_ab:
                    anchored  = TRUE;
                    infront   = TRUE;
                    break;
                 case fcnid_c:
                    sensitive = TRUE;
                    break;
                 case fcnid_ac:
                    anchored  = TRUE;
                    sensitive = TRUE;
                    break;
                 case fcnid_bc:
                    infront   = TRUE;
                    sensitive = TRUE;
                    break;
                 case fcnid_abc:
                    anchored  = TRUE;
                    infront   = TRUE;
                    sensitive = TRUE;
                    break;
                }
              brc = PmlFindCset (pmv->ultab, pmv->pt, pmv->lg, &pmv->cursor, 
                                 anchored, infront, sensitive, set_dp);  
              break;
             }
            case fcntynset:
             {
              anchored  = FALSE;
              infront   = FALSE;
              sensitive = FALSE;
              
              switch (exe_p->fcn_id)
                {
                 case fcnid_a:
                    anchored  = TRUE;
                    break;
                 case fcnid_b:
                    infront   = TRUE;
                    break;
                 case fcnid_ab:
                    anchored  = TRUE;
                    infront   = TRUE;
                    break;
                 case fcnid_c:
                    sensitive = TRUE;
                    break;
                 case fcnid_ac:
                    anchored  = TRUE;
                    sensitive = TRUE;
                    break;
                 case fcnid_bc:
                    infront   = TRUE;
                    sensitive = TRUE;
                    break;
                 case fcnid_abc:
                    anchored  = TRUE;
                    infront   = TRUE;
                    sensitive = TRUE;
                    break;
                }
              brc = PmlFindNotCset (pmv->ultab, pmv->pt, pmv->lg, &pmv->cursor, 
                                    anchored, infront, sensitive, set_dp);  
              break;
             }
           }
           break;
          }

        case fcntypmi:
          {
           if (exe_p->arg_ty == argtyint)
             li = exe_p->vcc_id;
           else
             memcpy (&li, &exe_p->vcc_id, 4);
             
           switch (exe_p->fcn_id)
             {
              case fcnidpos:
                 brc = PmlPos (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidposgt:
                 brc = PmlPosGt (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidposge:
                 brc = PmlPosGe (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidposlt:
                 brc = PmlPosLt (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidposle:
                 brc = PmlPosLe (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidposne:
                 brc = PmlPosNe (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidrpos:
                 brc = PmlRpos (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidrposlt:
                 brc = PmlRposLt (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidrposle:
                 brc = PmlRposLe (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidrposge:
                 brc = PmlRposGe (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidrposgt:
                 brc = PmlRposGt (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidrposne:
                 brc = PmlRposNe (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidlen:
                 brc = PmlLen (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidtab:
                 brc = PmlTab (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidrtab:
                 brc = PmlRtab (pmv->lg, &pmv->cursor, li); 
                 break;
              case fcnidchr:
                 
				     val_d.lg = 1;
                 val_d.pt = val;
                 val[0]   = (ub_int) exe_p->vcc_id;

                 brc = PmlInStr (pmv->ultab, pmv->pt, pmv->lg, &pmv->cursor, 
                                 TRUE, TRUE, FALSE, (st_sdc_p) &val_d); 
                 break;
             }
           break;
          }

        case fcntypmc:
          {
           switch (exe_p->fcn_id)
             {
              case fcnidfbal:
                 brc = PmlFbal (pmv->pt, pmv->lg, &pmv->cursor);
                 break;
              case fcnidrem:
                 brc = PmlRem (pmv->pt, pmv->lg, &pmv->cursor);
                 break;
              case fcnidtrue:
                 brc = TRUE;
                 break;
              case fcnidfail:
                 brc = FALSE;
                 break;
              case fcnidcut:
				 pmv->stk.ctalt = 1;
                 brc = TRUE;
                 break;
              case fcnidsuccess:
                 pmv->eomatch = TRUE;
                 brc = TRUE;
                 break;
              case fcnidabort:
                 pmv->eomatch = TRUE;
                 brc = FALSE;
                 break;
             }
           break;
          }

        case fcntystr:
        case fcntyfstr:
          {
           arg_ty  = exe_p->arg_ty;
           vcc_id  = exe_p->vcc_id;        
           fcn_id  = exe_p->fcn_id;
           elm_p   = 0;            

           switch (arg_ty)
             {
              case argtycon:
                {
                 con_p = pmv->con1_p + (vcc_id - 1);
                 val_d = con_p->val_d;
                 ct    = 1;
                 break;
                }
              case argtyvar:
                {
                 var_p = pmv->var1_p + (vcc_id - 1);
                 val_d = var_p->val_d;
                 ct    = 1;
                 break;
                }
              case argtycct:
                {
                 cct_p   = pmv->cct1_p + (vcc_id - 1);
                 elm_p   = (st_cctelm_p) cct_p->cct_d.pt;
                 ct      = cct_p->ct;

                 pmv->cctstr_d.lg = 0;

                 for (ix2 = 0; ix2 < ct; ix2++, elm_p++)
                   {
                    if (arg_ty == argtycct)
                      {
                       if (argtycon == elm_p->arg_ty)
                         {
                          con_p = pmv->con1_p + (elm_p->vcc_id - 1);
                          val_d = con_p->val_d;
                         }
                       else
                         {
                          var_p = pmv->var1_p + (elm_p->vcc_id - 1);
                          val_d = var_p->val_d;
                         }
                      }
                    if ((pmv->cctstr_d.lg + val_d.lg) > max_uint)
                      {
                       _INTERR (pmv->errline, s_error, "Concatenated string too long");
                       int_err = TRUE;
                       break;
                      }
                    if ((pmv->cctstr_d.lg + val_d.lg) > pmv->cctstr_d.ml)
                      {
                       pmv->cctstr_d.pt = RteStdRealloc (pmv->cctstr_d.pt, 
                                                         pmv->cctstr_d.lg + val_d.lg);
                       pmv->cctstr_d.ml = pmv->cctstr_d.lg + val_d.lg;
                      }
                    memcpy (pmv->cctstr_d.pt + pmv->cctstr_d.lg, val_d.pt, val_d.lg);
                    pmv->cctstr_d.lg += val_d.lg;
                   }
                 val_d.lg = pmv->cctstr_d.lg;
                 val_d.pt = pmv->cctstr_d.pt;
                 break;
                }
             }
           
           if (int_err)
             break;

           anchored  = FALSE;
           infront   = FALSE;
           sensitive = FALSE;
 
           if (exe_p->fcn_ty == fcntyfstr)
             {
              switch (exe_p->fcn_id)
                {
                 case fcnid_a:
                    anchored  = TRUE;
                    break;
                 case fcnid_b:
                    infront   = TRUE;
                    break;
                 case fcnid_ab:
                    anchored  = TRUE;
                    infront   = TRUE;
                    break;
                 case fcnid_c:
                    sensitive = TRUE;
                    break;
                 case fcnid_ac:
                    anchored  = TRUE;
                    sensitive = TRUE;
                    break;
                 case fcnid_bc:
                    infront   = TRUE;
                    sensitive = TRUE;
                    break;
                 case fcnid_abc:
                    anchored  = TRUE;
                    infront   = TRUE;
                    sensitive = TRUE;
                    break;
                }
             }
           else
             {
             
              switch (exe_p->fcn_id)
                {
                 case fcnidcmpr:
                 case fcnidexact:
                    sensitive = TRUE;
                    anchored  = TRUE;
                    break;
                 case fcnidbreakmatch:
                    infront   = TRUE;
                    sensitive = TRUE;
                    break;
                 case fcnidmatch:
                    sensitive = TRUE;
                    break;
                 case fcnidlike:
                    anchored  = TRUE;
                    break;
                 case fcnidbreaklook:
                    infront   = TRUE;
                    break;
                 case fcnidlook:
                    break;
                 case fcnidtrailing:
                    sensitive = TRUE;
                    anchored  = TRUE;
                    infront   = TRUE;
                    break;
                }
             }

           brc = val_d.lg == 0;
           if (brc) break;
           
           brc = PmlInStr (pmv->ultab, pmv->pt, pmv->lg, &pmv->cursor, 
                           anchored, sensitive, infront, (st_sdc_p) &val_d); 
           break;
          }

        case fcntysset:
          {
           anchored  = FALSE;
           infront   = FALSE;
           sensitive = FALSE;

           switch (exe_p->fcn_id)
             {
              case fcnid_a:
                 anchored  = TRUE;
                 break;
              case fcnid_b:
                 infront   = TRUE;
                 break;
              case fcnid_ab:
                 anchored  = TRUE;
                 infront   = TRUE;
                 break;
              case fcnid_c:
                 sensitive = TRUE;
                 break;
              case fcnid_ac:
                 anchored  = TRUE;
                 sensitive = TRUE;
                 break;
              case fcnid_bc:
                 infront   = TRUE;
                 sensitive = TRUE;
                 break;
              case fcnid_abc:
                 anchored  = TRUE;
                 infront   = TRUE;
                 sensitive = TRUE;
                 break;
             }

           if (exe_p->arg_ty == argtyvar)
             {
              var_p  = pmv->var1_p + (vcc_id - 1);
              cct_id = var_p->val_d.ty;
             }
           else
             {
              cct_id = exe_p->vcc_id; 
             }
                 
           setcct_p   = pmv->cct1_p + (cct_id - 1);
           setelm_p   = (st_cctelm_p) setcct_p->cct_d.pt;

           brc     = FALSE;
           first   = TRUE;
           tmpcurs = pmv->cursor;
           tmpix   = pmv->buf_ix;

           for (ix = 0; ix < setcct_p->ct; ix++, setelm_p++)
             {
              pmv->cursor = tmpcurs;
              arg_ty      = setelm_p->arg_ty;
              vcc_id      = setelm_p->vcc_id;        
              elm_p       = 0;            

              switch (arg_ty)
                {
                 case argtycon:
                   {
                    con_p = pmv->con1_p + (vcc_id - 1);
                    val_d = con_p->val_d;
                    ct    = 1;
                    break;
                   }
                 case argtyvar:
                   {
                    var_p = pmv->var1_p + (vcc_id - 1);
                    val_d = var_p->val_d;
                    ct    = 1;
                    break;
                   }
                 case argtycct:
                   {
                    cct_p   = pmv->cct1_p + (vcc_id - 1);
                    elm_p   = (st_cctelm_p) cct_p->cct_d.pt;
                    ct      = cct_p->ct;

                    pmv->cctstr_d.lg = 0;

                    for (ix2 = 0; ix2 < ct; ix2++, elm_p++)
                      {
                       if (arg_ty == argtycct)
                         {
                          if (argtycon == elm_p->arg_ty)
                            {
                             con_p = pmv->con1_p + (elm_p->vcc_id - 1);
                             val_d = con_p->val_d;
                            }
                          else
                            {
                             var_p = pmv->var1_p + (elm_p->vcc_id - 1);
                             val_d = var_p->val_d;
                            }
                         }
                       if ((pmv->cctstr_d.lg + val_d.lg) > max_uint)
                         {
                          _INTERR (pmv->errline, s_error, "Concatenated string too long");
                          int_err = TRUE;
                          break;
                         }
                       if ((pmv->cctstr_d.lg + val_d.lg) > pmv->cctstr_d.ml)
                         {
                          pmv->cctstr_d.pt = RteStdRealloc (pmv->cctstr_d.pt, 
                                                            pmv->cctstr_d.lg + val_d.lg);
                          pmv->cctstr_d.ml = pmv->cctstr_d.lg + val_d.lg;
                         }
                       memcpy (pmv->cctstr_d.pt + pmv->cctstr_d.lg, val_d.pt, val_d.lg);
                       pmv->cctstr_d.lg += val_d.lg;
                      }
                    val_d.lg = pmv->cctstr_d.lg;
                    val_d.pt = pmv->cctstr_d.pt;
                    break;
                   }
                 /*if (int_err) PTS 1109360
                     break;     PTS 1109360*/
                }

              if (int_err) 
                break;

              if (val_d.lg == 0)
                elmbrc = TRUE;
              else
                elmbrc = PmlInStr (pmv->ultab, pmv->pt, pmv->lg, &pmv->cursor, 
                                   anchored, sensitive, infront, 
                                   (st_sdc_p) &val_d); 

              if (elmbrc)
                {
                 brc = TRUE;

                 if ((first) || 
                     (infront && (pmv->cursor < curcurs)) ||
                     (!infront && ((pmv->cursor - val_d.lg) < begcurs)))
                   {
                    begcurs = pmv->cursor - val_d.lg;
                    curcurs = pmv->cursor;
                   }
                 if (anchored) break;

                 first = FALSE;
                }
             }

           if (brc)
             {
              pmv->cursor = curcurs;
             }
           else
             {
              pmv->cursor = tmpcurs;
              pmv->buf_ix = tmpix;
             }

           break;
          }

        case fcntyass:
          {
           var_p = pmv->var1_p + (exe_p->vcc_id - 1);
           switch (exe_p->fcn_id)
             {
              case fcnidpshass:
                {
                 brc = PmfPshAss (pmv, exe_p->ass_ty,
                               exe_p->vcc_id, (st_sdc_p) &var_p->val_d);
                 if (!brc) 
                   {
                    _INTERR (pmv->errline, s_error, "Error in PmfPshAss");
                    int_err = TRUE;
                   }
                 break;
                }
              case fcnidpshstr:
                {
                 brc = PmfPshStr (pmv, exe_p->ass_ty,
                               exe_p->vcc_id, (st_sdc_p) &var_p->val_d);
                 if (!brc) 
                   {
                    _INTERR (pmv->errline, s_error, "Error in PmfPshStr");
                    int_err = TRUE;
                   }
                 break;
                }
              case fcnidexeass:
                {
                 brc = PmImmedAssign (pmv, var_p);
                 if (!brc) 
                   {
                    _INTERR (pmv->errline, s_error, "Error in PmImmedAssign");
                    int_err = TRUE;
                   }
                 break;
                }
             }
           break;
          }
        case fcntyapx:
          {
           /* build approx control blocks */
           
           brc = PmApproxPrep (pmv, exe_p);
           if (brc)
             {
              brc = PmApproxExec (pmv);
             }
           pmv->succ = pmn__nil;
           break;
          }

        case fcntyaso:
          {
           /* build asso parameter list and execute afterwards */
           
           brc = PmAssoPrep (pmv, exe_p);
           if (brc)
             {
              brc = PmAssoExec (pmv);
             }
           pmv->succ = pmn__nil;
           break;
          }

        case fcntycall:
          {
           /* build call list */
           
           brc = PmCallPrep (pmv, exe_p);
           if (brc)
             {
              brc = PmCallExec (pmv);
             }
           break;
          }

        case fcntyrpl:
          {
           /* vorläufiger Mißbrauch der Schnitstelle zur paral. Verarbeitung */
           DMY.lg = 0;
           DMY.ml = exe_p->arg_ty;
           DMY.pt = 0;

           brc = PmfPshAss (pmv, exe_p->ass_ty, 
                            exe_p->vcc_id, &DMY); 
           if (!brc) 
             {
              _INTERR (pmv->errline, s_error, "Error in PmfPshAss");
              int_err = TRUE;
             }
           break;
          }

        case fcntycll:
          {
           brc         = PmfCallPat (pmv, exe_p->vcc_id);
           if (!brc) 
             {
              int_err = TRUE;
              break;
             }

           flt_p       = pmv->flt1_p + (exe_p->vcc_id - 1);
           pmv->fxe1_p = (st_fxelst_p) flt_p->val_d.pt;

           if (pmv->fxe1_p == 0)
             {
              pmv->fxe1_p = PmFlatPat (mam_p, exe_p->vcc_id);
		      if (0 == pmv->fxe1_p)
		        {
                 _INTERR (pmv->errline, s_error, "No space available for flat pattern");
                 pmv->curpat = 0;
                 pmv->eomatch = TRUE;
                 pms->errcode = TRUE;
                 int_err = TRUE;
                 break;
		        }
             }
           break;
          }
        case fcntyarb:
          {
           brc = PmfPshArb (pmv, exe_p->vcc_id, exe_p->ctmod);
           if (!brc) 
             {
              _INTERR (pmv->errline, s_error, "Error in PmfPshArb");
              int_err = TRUE;
             }
		   break;
          }
        case fcntytst:
          {
           brc = PmfResetArb (pmv, (us_int) fxe_p->alt);
		   break;
          }
        case fcntyend:
          {
           exec = FALSE;
           brc = PmfPopArb (pmv, exec);
           if ((!brc) && (pmv->errline[0] != 0))
             {
              _INTERR (pmv->errline, s_error, "Error in PmfPopArb");
              int_err = TRUE;
             }
           break;
          }
        case fcntynot:
          {
     		  cur = pmv->curnode;
		     for (;;)
		       {
			     PmfPopAlt (pmv, &alt);
              if ((cur + 1) == alt) break;
		       }
		     brc = FALSE;
		     break;
          }
       }
     if (int_err) break;


     pmv->deb.info.cursor    = pmv->cursor;
     pmv->deb.info.brc       = brc;

     if (pmv->trcp != 0)
       {
        sprintf ((char_p) buf, "%6d %6d %6d %6d%6d %6d",
                 pmv->deb.info.pat_id, pmv->deb.info.exe_curs,
                 pmv->deb.info.pat_ix, pmv->deb.info.precursor,
                 pmv->deb.info.cursor, pmv->deb.info.brc);

        buf_d.lg = strlen ((char_p) buf);
        rtc = RteFilRecord (fil_put, pmv->trcp, sizeof (buf), &buf_d);
       }

     /* set next node */
     pmv->success = brc;
     if (pmv->success) 
       {
        pmv->curnode = pmv->succ;
       }
     else 
       {
        PmfPopAlt (pmv, &pmv->curnode);
		if (pmv->curnode != pmn__nil)
		  {
           flt_p       = pmv->flt1_p + (pmv->curpat - 1);
           pmv->fxe1_p = (st_fxelst_p) flt_p->val_d.pt;
		  }
       }

     if (pmv->curnode == pmn__nil) 
       {
        /* return form match to system */
        if (pmv->curincarn == 0) 
          pmv->eomatch = TRUE;
        else 
          {
           /* return to older pattern */
           if (pmv->success)
             {
              PmfPopRet (pmv);
              flt_p  = pmv->flt1_p + (pmv->curpat - 1);
              pmv->fxe1_p = (st_fxelst_p) flt_p->val_d.pt;
                           
              pmv->eomatch = pmv->curnode == pmn__nil;
             }
           /* return form match to system */
           else 
              pmv->eomatch = TRUE;
          } 
       }    
    } /* end of while */
  
  if (!pmv->eomatch)
    {
     pms->errline = pmv->errline;
     if (pmv->errline[0] == 0)
	   {
        pms->errcode = deb_no_eomatch;
        pmv->deb.status = deb_no_eomatch;
       }
     else
       {
        pms->errcode = FALSE;
        pmv->deb.status = deb_not_init;
       }
     return FALSE; 
    }

  /* do string-assignments */
  if (pmv->success) 
    {

     /* in 2 passes ... */
     for (ipas = 1; ipas <= 2; ipas++)
       {
        
        /* from youngest to oldest incarnation... */
        for (incarn = pmv->stk.ctincarn; incarn >= 0; incarn--)
          {
           /* for all assigments .... */
           for (iass = 1; iass <= pmv->stk.ctass; iass++) 
             {
              stk_ass = pmv->stk.ass + (iass - 1);
           
              /* of apropriate typ :   */
              if (stk_ass->incarn == (us_int) incarn)
                {
                 if (((ipas == 1) &&
                      ((stk_ass->asstyp == '.') || (stk_ass->asstyp == '$')))
                     ||                
                     ((ipas == 2) &&
                      ((stk_ass->asstyp == '>') || (stk_ass->asstyp == '/') || 
                       (stk_ass->asstyp == '^') || (stk_ass->asstyp == '<') || 
					   (stk_ass->asstyp == '@'))))
                   /* do it !!!!!! */
                   {
                    if (stk_ass->asstyp == '<')
                      {
                       if (!PmeReplace (mam_p, pmv, stk_ass, iass))
						  return FALSE;
                      }
                    else
                      {
                       PmeAssign  (mam_p, pmv, stk_ass);
                       if (pmv->trcp != 0)
                         {
                          memset (nam, 0, sizeof(nam));
                          /* Output to tracefile was testcase 
                             ignored yet.  FJH 21-01-1997 
                          var_p = var1_p +  (stk_ass->subno - 1);
                          memcpy (nam, var_p->nm, var_p->nm_lg);
                          memset (val, 0, sizeof(nam));
                          memcpy (val, var_p->val_d.pt, var_p->val_d.lg);
                          sprintf (buf, "%d %d %s %s", ipas, incarn, nam, val);
                          buf_d.pt = buf;
                          buf_d.lg = strlen ((char_p) buf);
                          buf = RteFilRecord (fil_put, pmv->trcp, sizeof (buf), &buf_d); */
                         }
                      }
                   }
                }
             }
          }                             
       }
     *rl = pmv->b_p->acc_lg;
    }

  if (!pmv->success)
    {
     pmv->cursor = pmv->savcursor;
     pmv->buf_ix = pmv->savbuf_ix;
    }

  pmv->deb.status = deb_not_init;

  pms->cursor  = pmv->cursor;

  pms->errline = pmv->errline;
  if (pmv->errline[0] == 0)
    pms->errcode = FALSE;
  else
    pms->errcode = TRUE;
  

  return pmv->success;
}


bool PmExecMatch 
(
 st_pms_p   pms,
 st_sdc_p   pmv_matobj_dp,
 st_sdc_p   pmv_patnam
)
{
 bool       brc;
 st_pmv_p   pmv;
 st_blktab  b;
 ul_int     lg;

 pmv = (st_pmv_p)pms->pmv;
 
 pmv->pt   = pmv_matobj_dp->pt;
 pmv->lg   = pmv_matobj_dp->lg;

 b.blk_p   = pmv_matobj_dp->pt;
 b.acc_lg  = pmv_matobj_dp->lg;

 pmv->b_p      = &b;
 pmv->buf_ix   = 0;
 pmv->last_ix  = 1;
 pmv->bufmatch = FALSE;

 /*pmv->deb.step_ty = deb_single_step; */

 lg  = pmv_matobj_dp->lg;
 brc = PmExec (pms, pmv_matobj_dp->ml, &lg, pmv_patnam);
 pmv_matobj_dp->lg = (us_int) lg;

 while (pmv->deb.status != deb_not_init)
   {
    lg  = pmv_matobj_dp->lg;
    brc = PmExec (pms, pmv_matobj_dp->ml, &lg, pmv_patnam);
    pmv_matobj_dp->lg = (us_int) lg;
   } 

 
 return brc;
}

bool PmExecLongMatch 
(
 st_pms_p   pms,
 st_mdc_p   pmv_matobj_dp,
 st_sdc_p   pmv_patnam
)
{
 bool       brc;
 st_pmv_p   pmv;
 st_blktab  b;

 pmv = (st_pmv_p)pms->pmv;
 
 pmv->pt   = pmv_matobj_dp->pt;
 pmv->lg   = pmv_matobj_dp->llg;

 b.blk_p   = pmv_matobj_dp->pt;
 b.acc_lg  = pmv_matobj_dp->llg;

 pmv->b_p      = &b;
 pmv->buf_ix   = 0;
 pmv->last_ix  = 1;
 pmv->bufmatch = FALSE;


 brc = PmExec (pms, pmv_matobj_dp->lml, &pmv_matobj_dp->llg, pmv_patnam);


 return brc;
}

bool PmExecPattern                  /* pattern execution              */
(
 st_pms_p        pms,               /* pointer to communication area  */
 char           *patnam,            /* pattern name                   */
 unsigned char  *matobj,            /* pointer to pattern object      */
 unsigned long  *lg,                /* length before and after exec.  */
 unsigned long   ml                 /* maximum length of pattern obj. */
)
{
 bool       brc;
 st_sdc     patnam_d;
 st_pmv_p   pmv;
 st_blktab  b;
 
 pmv = (st_pmv_p)pms->pmv;
 
 pmv->pt   = matobj;
 pmv->lg   = *lg;
 b.blk_p   = matobj;
 b.acc_lg  = *lg;

 pmv->b_p      = &b;
 pmv->buf_ix   = 0;
 pmv->last_ix  = 1;
 pmv->bufmatch = FALSE;
  
 patnam_d.lg  = strlen (patnam);
 patnam_d.ml  = patnam_d.lg;
 patnam_d.pt  = (ub_int_p) patnam;
                                 
 brc =  PmExec (pms, ml, lg, &patnam_d); 
 
 return brc;
}

bool PmExecInfo                     /* info on last call              */
(
 st_pms_p        pms,               /* pointer to communication area  */
 unsigned long  *lcurs,             /* long cursor                    */
 unsigned short *bno                /* block number                   */
)
{
 st_pmv_p   pmv;
 
 pmv = (st_pmv_p)pms->pmv;

 if (0 == pmv) return FALSE;
 
 *bno = pmv->buf_ix + 1;
 
 if (0 == pmv->buf_ix)
   *lcurs = pmv->cursor;
 else
   *lcurs = (pmv->b_p + (pmv->buf_ix - 1))->acc_lg + pmv->cursor;
 
 return TRUE;
}

/* End of PmExec */
