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
#include "idl/idl2xml/rte.h"
#include "idl/idl2xml/mam.h"
#include "idl/idl2xml/pms.h"
#include "idl/idl2xml/pms00.h"
#include "idl/idl2xml/pmexec.h"
#include "idl/idl2xml/pmmadm.h"
#include "idl/idl2xml/pmradm.h"
#include "idl/idl2xml/pms32.h"

#define FilAtL_ct  3
#define DstAtL_ct  5
#define ZipCode    999


rc_t PmmRcdHandleNeu
(
 st_mam_p    v_mam_p,
 st_lstcb_p  v_varcb_p
)
{
 rc_t          rc;
 st_lstcb      varcb;
 us_int        ix;
 us_int        ix2;
 st_varlst_p   var_p;
 st_namlst_p   nam_p;
 st_varold_p   varold_p;
 
 if (v_varcb_p == 0)
   {
    v_mam_p->fcncb = CbList[0];
    v_mam_p->patcb = CbList[1];
    v_mam_p->concb = CbList[2];
    v_mam_p->cctcb = CbList[4];
    v_mam_p->namcb = CbList[5];
    v_mam_p->setcb = CbList[6];
    v_mam_p->pdfcb = CbList[7];
    varcb = CbList[3];
    v_mam_p->rescb[PmrNamCb] = v_mam_p->namcb;
    var_p = (st_varlst_p) varcb.lst_d.pt;
   }
 else
   {
    var_p = (st_varlst_p) v_varcb_p->lst_d.pt;
    varcb = *v_varcb_p;
   }
 
 
 varold_p = (st_varold_p) var_p; 
  
 for (ix = 0; ix < varcb.ct; ix++, var_p++, varold_p++)
   {
    if ((varcb.lst_d.llg / varcb.ct) == sizeof(st_varold))
      {
       var_p->id     = varold_p->id;
       var_p->ctmod  = varold_p->ctmod;
       var_p->rdi    = varold_p->rdi;
       var_p->nocon  = varold_p->nocon;
       var_p->var_ty = varold_p->var_ty;
       var_p->val_d  = varold_p->val_d;
       
       nam_p = (st_namlst_p) v_mam_p->namcb.lst_d.pt;

       for (ix2 = 0; ix2 < v_mam_p->namcb.ct; ix2++, nam_p++)
         {
          if (nam_p->nam_ty != namtyvar) continue;
          if (nam_p->fpv_id == var_p->id) break;
         }
       var_p->nm_id = nam_p->id;
      }
    rc = PmmVar2Dst (&v_mam_p->varcb, var_p);
    if (rc != RcRcdOk) return rc;
   }

 v_mam_p->varcb.lst_d.lml = 1; 

 return RcRcdOk;
}

rc_t PmmDstCreate
(
 st_mam_p v_mam_p,
 ul_int   blksize
)
{
 rc_t          rc;

 rc = PmrInit (v_mam_p);

 v_mam_p->varcb.recno = lstnrmrdi;
 v_mam_p->varcb.sz    = sizeof (st_varlst);
 v_mam_p->varcb.ct    = 0;

 return RcRcdOk;

}


rc_t PmmTmpCreateNeu
(
 st_mam_p   v_mam_p,
 ul_int     blksize,
 st_lstcb_p v_varcb_p
)
{
 rc_t          rc;

 rc = PmrInit (v_mam_p);

 v_mam_p->varcb.recno = lstnrmrdi;
 v_mam_p->varcb.sz    = sizeof (st_varlst);
 v_mam_p->varcb.ct    = 0;

 rc = PmmRcdHandleNeu (v_mam_p, v_varcb_p);

 return RcDstOk;

}



rc_t PmrNamAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 us_int         r_fpv_id,
 us_int         r_nam_ty
)
{
 rc_t          rc;
 st_namlst     nl;
 st_dsc        nl_d;
 us_int        ct;
 ub_int        n0;

 nl_d.lg = v_lcb_p->sz;
 nl_d.ty = 0;
 nl_d.pt = (ub_int_p) &nl;

 memset (&nl, 0, v_lcb_p->sz);

 nl.id       = v_lcb_p->ct + 1;
 nl.fpv_id   = r_fpv_id;
 nl.nam_ty   = r_nam_ty;
 nl.lst_ty   = lsttynam;
 
 nl.nam_d.lg = r_nm_dp->lg + 1;

 rc = PmrElmAdd (&v_lcb_p->lst_d, &nl_d, &v_lcb_p->ct);
 if (rc != RcRcdOk) return rc;

 rc = PmrElmAdd (&v_lcb_p->elm_d, (st_dsc_p) r_nm_dp, &ct);
 if (rc != RcRcdOk) return rc;
 
 n0      = 0;
 nl_d.lg = 1;
 nl_d.ty = 0;
 nl_d.pt = &n0;

 rc = PmrElmAdd (&v_lcb_p->elm_d, &nl_d, &ct);
 if (rc != RcRcdOk) return rc;

 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);
                     
 return rc;
}


rc_t PmmPdfAdd
(
 st_lstcb_p     v_lcb_p,         /* &pmv->newmam.rescb[PmrPdfCb] */
 st_sdc_p       r_nm_dp,
 ub_int         stm_ty,
 ul_int         stm_nm,
 ul_int         stm_op,
 ul_int         stm_beg,
 ul_int         stm_end
)
{
 rc_t          rc;
 st_pdflst     pl;
 st_dsc        pl_d;
 ub_int_p      nm_p;

 pl_d.lg = sizeof (st_pdflst);
 pl_d.ty = 0;
 pl_d.pt = (ub_int_p) &pl;

 memset (&pl, 0, pl_d.lg);

 pl.id       = v_lcb_p->ct + 1;
 pl.stm_ty   = stm_ty;
 pl.stm_op   = stm_op;
 pl.stm_nm   = stm_nm;
 pl.stm_beg  = stm_beg;
 pl.stm_end  = stm_end;
 
 if (stm_ty != ';')
   {
    rc = PmmNamFind (v_lcb_p - v_lcb_p->ix + PmrNamCb, r_nm_dp, 
                    &nm_p, &pl.nam_ty, &pl.nam_id, &pl.fpv_id);
    if (rc != RcRcdOk) return rc;
   }

 rc = PmrElmAdd (&v_lcb_p->lst_d, &pl_d, &v_lcb_p->ct);
 if (rc != RcRcdOk) return rc;


 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);
                     
 return rc;
}


rc_t PmmVarAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp
)
{
 rc_t          rc;
 st_varlst_p   var_p;

 rc = PmrVarAdd (v_lcb_p, r_nm_dp, r_val_dp);
 if (RcRcdOk != rc) return rc;

 var_p = (st_varlst_p) v_lcb_p->lst_d.pt;
 var_p = var_p + v_lcb_p->ct - 1;

 rc = PmmVar2Dst (v_lcb_p + PmrNoCb, var_p);

 return rc;
}

rc_t PmmVar2Dst
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    var_p
)
{
 rc_t          rc;
 st_varlst     vl;
 st_dsc        vl_d;

 vl_d.lg = v_lcb_p->sz;
 vl_d.ty = 0;
 vl_d.pt = (ub_int_p) &vl;

 memset (&vl, 0, v_lcb_p->sz);

 vl.id       = v_lcb_p->ct + 1;
 vl.nm_id    = var_p->nm_id;
 vl.ctmod    = 1;
 vl.rdi      = vl.id + 1; 
 vl.nocon    = var_p->nocon;

 vl.val_d.lg = var_p->val_d.lg;
 vl.val_d.pt = RteStdMalloc(var_p->val_d.lg);
 if (vl.val_d.pt == 0) return RcError;
 memcpy (vl.val_d.pt, var_p->val_d.pt, var_p->val_d.lg);


 rc = PmrElmAdd (&v_lcb_p->lst_d, &vl_d, &v_lcb_p->ct);

 
 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);
                     
 
 return rc;
}

rc_t PmrVarAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp
)
{
 rc_t          rc;
 st_varlst     vl;
 st_dsc        vl_d;
 us_int        ct;
 st_lstcb_p    namcb_p;

 vl_d.lg = v_lcb_p->sz;
 vl_d.ty = 0;
 vl_d.pt = (ub_int_p) &vl;

 memset (&vl, 0, v_lcb_p->sz);

 vl.id       = v_lcb_p->ct + 1;

 namcb_p     = v_lcb_p - v_lcb_p->ix + PmrNamCb;

 rc = PmrNamAdd (namcb_p, r_nm_dp, vl.id, namtyvar);
 if (rc != RcRcdOk) return rc;

 vl.nm_id    = namcb_p->ct;
 vl.ctmod    = 1;
 vl.rdi      = vl.id + 1; 
 
 vl.val_d.lg = r_val_dp->lg;

 rc = PmrElmAdd (&v_lcb_p->lst_d, &vl_d, &v_lcb_p->ct);

 rc = PmrElmAdd (&v_lcb_p->elm_d, (st_dsc_p) r_val_dp, &ct);
 if (rc != RcRcdOk) return rc;
 
 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);
                     

 return rc;
}

rc_t PmmVarReplace
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    v_var_p,
 st_dsc_p       r_val_dp
)
{

 if (v_var_p->val_d.lg != r_val_dp->lg)
 {
   RteStdFree (v_var_p->val_d.pt); 
   v_var_p->val_d.pt = RteStdMalloc (r_val_dp->lg);
   if (v_var_p->val_d.pt == 0) 
	   return RcError;
 }

 v_var_p->val_d.lg = r_val_dp->lg;
 memcpy (v_var_p->val_d.pt, r_val_dp->pt, r_val_dp->lg);
 v_var_p->ctmod++;

 return RcOk;

}


rc_t PmmVarIncr
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    v_var_p
)
{
 rc_t          rc;
 st_dsc        val_d;
 ub_int        val[100];
 us_int        overflow;
 s_int        ix;


 if (v_var_p->val_d.lg > 100) return RcRcdErr;

 memcpy(&val[1], v_var_p->val_d.pt, v_var_p->val_d.lg);
 val[0] = '0';
 overflow = 1;
 
 for (ix = v_var_p->val_d.lg;ix >= 0; ix--)
 {
   if ((val[ix] < '0') || (val[ix] > '9')) return RcRcdErr;
   if (overflow)
   {
     if (val[ix] == '9') val[ix] = '0';
     else
     {
       val[ix]++;
       overflow = 0;
     }
   }
 }
 
 if (val[0] == '0') 
 { 
   val_d.pt = &val[1];
   val_d.lg = v_var_p->val_d.lg;
 }
 else
 {
   val_d.pt = &val[0];
   val_d.lg = v_var_p->val_d.lg + 1;
 }
     
 rc = PmmVarReplace(v_lcb_p, v_var_p, &val_d);

 v_var_p->ctmod++;

 return rc;

}

rc_t PmmVarDecr
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    v_var_p
)
{
 rc_t          rc;
 st_dsc        val_d;
 ub_int        val[100];
 us_int        overflow;
 s_int        ix;

 if (v_var_p->val_d.lg > 100) return RcRcdErr;

 memcpy(&val[1], v_var_p->val_d.pt, v_var_p->val_d.lg);
 val[0] = '0';
 overflow = 1;
 
 for (ix = v_var_p->val_d.lg;ix >= 0; ix--)
 {
   if ((val[ix] < '0') || (val[ix] > '9')) return RcRcdErr;
   if (overflow)
   {
     if (val[ix] == '0') val[ix] = '9';
     else
     {
       val[ix]--;
       overflow = 0;
     }
   }
 }
 
 val_d.pt = &val[1];
 val_d.lg = v_var_p->val_d.lg;
     
 rc = PmmVarReplace(v_lcb_p, v_var_p, &val_d);


 v_var_p->ctmod++;

 return rc;

}

bool PmmFcnFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_fcnlst_p    *w_fcn_p
)
{
 us_int        ix;
 us_int        lg;

 st_fcnlst_p   fcn_p;

 if (0 == v_lcb_p->ct) return FALSE;

 lg    = r_nm_dp->lg;
 fcn_p = (st_fcnlst_p) v_lcb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, fcn_p++)
   {
    if (lg != fcn_p->nm_lg) continue;
    if (0 == memcmp (r_nm_dp->pt, fcn_p->nm, lg)) break;
   }

 if (ix == v_lcb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_fcn_p) = fcn_p;
    return TRUE;
   }
}

bool PmmFcnIdTy
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id,
 us_int         r_ty,
 st_fcnlst_p    *w_fcn_p
)
{
 us_int        ix;
 st_fcnlst_p   fcn_p;

 if (0 == v_lcb_p->ct) return FALSE;

 fcn_p = (st_fcnlst_p) v_lcb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, fcn_p++)
   {
    if (r_ty != fcn_p->ty) continue;
    if (r_id == fcn_p->id) break;
   }

 if (ix == v_lcb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_fcn_p) = fcn_p;
    return TRUE;
   }
}

rc_t PmmFcnLst
(
 st_lstcb_p v_lcb_p
)
{
 rc_t          rc;
 us_int        ix;
 st_fcnlst     fl;
 st_dsc        fl_d;
 st_sdc        nm_d;

 fl_d.lg = sizeof (st_fcnlst);
 fl_d.ty = 0;
 fl_d.pt = (ub_int_p) &fl;

 nm_d.ml = 0;
 nm_d.pt = (ub_int_p) fl.nm;

 for (ix = 0; ix < CTFCN; ix++)
    {
     fl.ty    = lstfcnnm[ix].ty;
     fl.id    = lstfcnnm[ix].id;
	  fl.res   = 0;
     fl.nm_lg = strlen (lstfcnnm[ix].nm);

     memset (fl.nm, 0, mxnmsize);
     memcpy (fl.nm, lstfcnnm[ix].nm, fl.nm_lg);

     rc = PmrElmAdd (&v_lcb_p->lst_d, &fl_d, &v_lcb_p->ct);
     if (rc != RcRcdOk) return rc;

     nm_d.lg = fl.nm_lg;

     rc = PmrNamAdd (v_lcb_p - v_lcb_p->ix + PmrNamCb, &nm_d, (us_int)(ix + 1), namtyfcn);
     if (rc != RcRcdOk) return rc;
   }
 
 return rc;

}

rc_t PmmConAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp
)
{
 rc_t          rc;
 st_conlst     cl;
 st_dsc        cl_d;
 st_dsc        val_d;
 st_lstcb_p    cb_p;                  
 us_int        ct;
 	                 
 cl_d.lg = v_lcb_p->sz;
 cl_d.ty = 0;
 cl_d.pt = (ub_int_p) &cl;

 memset (&cl, 0, v_lcb_p->sz);

 cl.id    = v_lcb_p->ct + 1;
 cl.rdi   = cl.id + 1; 

 cl.val_d.lg = r_val_dp->lg;
 cb_p = v_lcb_p; 
 rc = PmrElmAdd (&cb_p->lst_d, &cl_d, &cb_p->ct);

 val_d.lg = r_val_dp->lg;
 val_d.ty = 0;
 val_d.pt = r_val_dp->pt;

 rc = PmrElmAdd (&cb_p->elm_d, &val_d, &ct);

 PmrDesc ( (st_mam_p) (cb_p - cb_p->ix), PmrConCb);

 return rc;
}

bool PmmConFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp,
 st_conlst_p    *w_con_p
)
{
 us_int        ix;
 us_int        lg;
 st_conlst_p   con_p;

 if (0 == v_lcb_p->ct) return FALSE;

 lg    = r_val_dp->lg;
 con_p = (st_conlst_p) v_lcb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, con_p++)
   {
    if (con_p->val_d.lg != lg) continue;
    if (0 == memcmp (r_val_dp->pt, con_p->val_d.pt, lg)) break;
   }

 if (ix == v_lcb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_con_p) = con_p;
    return TRUE;
   }
}

rc_t PmmConAppend
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp,
 us_int         con_id
)
{
 return  PmrElmAppend (v_lcb_p, (st_dsc_p) r_val_dp, con_id);
}

rc_t PmmConForAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp,
 st_conlst_p    *w_con_p
)
{
 rc_t          rc;
 
 if (PmmConFind (v_lcb_p, r_val_dp, w_con_p)) return RcRcdOk;

 rc = PmmConAdd (v_lcb_p, r_val_dp);
 if (RcRcdOk != rc) return rc;

 rc = PmmConFind (v_lcb_p, r_val_dp, w_con_p);

 return rc;
}

bool PmmVarFind
(
 st_lstcb_p     v_lcb_p,
 st_lstcb_p     v_ncb_p,
 st_sdc_p       r_nm_dp,
 st_varlst_p    *w_var_p
)
{
 us_int        ix;
 us_int        lg;
 st_varlst_p   var_p;
 st_namlst_p   nam_p;

 (*w_var_p) = 0;
 if (0 == v_lcb_p->ct) return FALSE;

 lg    = r_nm_dp->lg;
 var_p = (st_varlst_p) v_lcb_p->lst_d.pt;
 nam_p = (st_namlst_p) v_ncb_p->lst_d.pt;

 for (ix = 0; ix < v_ncb_p->ct; ix++, nam_p++)
   {
    if (nam_p->nam_ty != namtyvar) continue;
    if (lg != (nam_p->nam_d.lg - 1)) continue;
    if (0 == memcmp (r_nm_dp->pt, nam_p->nam_d.pt, lg)) break;
   }

 if (ix == v_ncb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_var_p) = var_p + (nam_p->fpv_id - 1);
    return TRUE;
   }

}

rc_t PmmVarForAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp,
 st_varlst_p    *w_var_p
)
{
 rc_t          rc;
 
 if (PmmVarFind (v_lcb_p, v_lcb_p - v_lcb_p->ix + PmrNamCb, r_nm_dp, w_var_p))
    return RcRcdOk;

 rc = PmmVarAdd (v_lcb_p, r_nm_dp, r_val_dp);
 if (RcRcdOk != rc) return rc;

 rc = PmmVarFind (v_lcb_p, v_lcb_p - v_lcb_p->ix + PmrNamCb, r_nm_dp, w_var_p);
 
 return rc;
}

rc_t PmmVarAddorRep
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp,
 st_varlst_p    *w_var_p
)
{
 rc_t          rc;
 st_varlst_p   var_p;
 
 if (PmmVarFind (v_lcb_p, v_lcb_p - v_lcb_p->ix + PmrNamCb, r_nm_dp, w_var_p)) 
   {
    rc = PmrElmReplace (v_lcb_p, (st_dsc_p) r_val_dp, (*w_var_p)->id);
    if (RcRcdOk != rc) return rc;
	var_p = (st_varlst_p) (v_lcb_p + PmrNoCb)->lst_d.pt;
	var_p = var_p + ((*w_var_p)->id - 1);
    rc = PmmVarReplace (v_lcb_p + PmrNoCb, var_p, (st_dsc_p) r_val_dp);
   }
 else
   {
    rc = PmmVarAdd (v_lcb_p, r_nm_dp, r_val_dp);
    if (RcRcdOk != rc) return rc;
   }
 rc = PmmVarFind (v_lcb_p, v_lcb_p - v_lcb_p->ix + PmrNamCb, r_nm_dp, w_var_p);
 
 return rc;
}

bool PmmCctFind
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id,
 st_cctelm_p    *w_elm_p,
 st_cctlst_p    *w_cct_p
)
{
 st_cctlst_p   cct_p;


 if ((0 == v_lcb_p->ct) || (r_id > v_lcb_p->ct) || (r_id == 0)) return FALSE;

 cct_p = ((st_cctlst_p) v_lcb_p->lst_d.pt) + r_id - 1;

 if (0 == cct_p->ct) return FALSE;

 (*w_cct_p) = cct_p;
 (*w_elm_p) = (st_cctelm_p) cct_p->cct_d.pt;

 return TRUE;
}

bool PmmCctSearch
(
 st_lstcb_p     v_lcb_p,
 us_int         r_ct,
 st_cctelm_p    r_ell_p,
 st_cctelm_p    r_elm_p,
 st_cctlst_p    *w_cct_p
)
{
 us_int        ix;
 us_int        ix2;
 st_cctelm_p   el1_p;
 st_cctelm_p   el2_p;
 st_cctlst_p   cct_p;
 ub_int        eqcct;


 if (0 == v_lcb_p->ct) return FALSE;

 cct_p = (st_cctlst_p) v_lcb_p->lst_d.pt;

 if (0 == v_lcb_p->ct) return FALSE;

 eqcct = FALSE;
 
 for (ix = 0; ix < v_lcb_p->ct; ix++, cct_p++)
   {

    if (cct_p->ct < (r_ct + 1)) continue;

    el1_p = r_ell_p;
    el2_p = (st_cctelm_p) cct_p->cct_d.pt;

    eqcct = TRUE;
    for (ix2 = 0; ix2 < r_ct; ix2++, el1_p++, el2_p++)
      {
       if ((el1_p->vcc_id != el2_p->vcc_id) || 
           (el1_p->arg_ty != el2_p->arg_ty)) 
        {
          eqcct = FALSE;
          break; 
        }
      }
    if (!eqcct) continue;

    el2_p = (st_cctelm_p) cct_p->cct_d.pt;
    el2_p += r_ct;

    eqcct = (r_elm_p->vcc_id == el2_p->vcc_id) && 
            (r_elm_p->arg_ty == el2_p->arg_ty);

    if (eqcct) break;
   }

 if (eqcct)
   {
    (*w_cct_p) = cct_p;
    return TRUE;
   }
 else
   {
    return FALSE;
   }
}


rc_t PmmCctAdd
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id,
 us_int         r_voc_id,
 us_int         r_ty
)
{
 rc_t          rc;
 st_cctelm     el;
 st_dsc        el_d;
 st_cctlst     cl;
 st_dsc        cl_d;
 st_lstcb_p    cb_p;                  
   
 el_d.lg = sizeof (el);
 el_d.ty = 0;
 el_d.pt = (ub_int_p) &el;

 el.arg_ty = r_ty;   
 el.vcc_id = r_voc_id;   
 el.ctmod  = 0;   
 el.res    = 0;   

 if (r_id == (v_lcb_p->ct + 1))
   {
    cl_d.lg = v_lcb_p->sz;
    cl_d.ty = 0;
    cl_d.pt = (ub_int_p) &cl;

    memset (&cl, 0, v_lcb_p->sz);

    cl.id  = r_id;
    cl.rdi = r_id + 1;
    cl.ct  = 0;

    cb_p = v_lcb_p; 
    rc = PmrElmAdd (&cb_p->lst_d, &cl_d, &cb_p->ct);
   }

 rc = PmrElmAppend (v_lcb_p, &el_d, r_id);
 
 rc = PmrDesc ((st_mam_p) (v_lcb_p - v_lcb_p->ix), PmrCctCb );
 
 return rc;
}

bool PmmCctlstAdd
(
 st_lstcb_p     v_lcb_p,
 us_int         r_ct,
 st_cctlst_p    r_cct_p,
 st_cctelm_p    r_elm_p
)
{
 rc_t          rc;
 us_int        ix;
 us_int        id;
 st_cctelm_p   ell_p;

 id = v_lcb_p->ct + 1;

 for (ix = 0; ix < r_ct; ix++)
   {
    ell_p = (st_cctelm_p) r_cct_p->cct_d.pt;
    ell_p += ix;
    rc = PmmCctAdd (v_lcb_p, id, ell_p->vcc_id, ell_p->arg_ty);
    if (rc != RcRcdOk) return rc;
   }  

 rc = PmmCctAdd (v_lcb_p, id, r_elm_p->vcc_id, r_elm_p->arg_ty);

 return rc;
}



bool PmmSetFill
(
 ub_int    setfunc,
 st_dsc_p  val_dp,
 st_dsc_p  set_dp
)
{
  ub_int_p   tp;
  ub_int_p   sp;
  ub_int     i;
  us_int     lg;
 
  tp = set_dp->pt; 
  lg = set_dp->lg; 

  if (setfillini == setfunc)
     memset (tp, 0, lg);
  
  sp = val_dp->pt; 

  for (i = 0; i < val_dp->lg; i++, sp++)
    {
     if (lg < *sp) return FALSE;
     
     *(tp + *sp) = TRUE;         
    }
  
  return TRUE;
}                                     


bool PmmSetSet
(
 st_mam_p       v_mam_p,
 us_int         r_vcc_id,
 us_int         r_ty,
 st_dsc_p       set_dp,
 st_conlst_p    con1_p,
 st_varlst_p    var1_p,
 st_cctlst_p    cct1_p
)
{
 bool        brc;
 ub_int      cct;
 us_int      id;
 us_int      ty;
 us_int      ct;
 st_cctlst_p cct_p;
 st_cctelm_p elm_p;
 st_varlst_p var_p;
 st_dsc      val_d; 
 ub_int      setfunc;
 st_conlst_p con_p;

 setfunc = setfillini;
 cct     = r_ty == argtycct;



 if (cct)
   {
    cct_p = cct1_p + (r_vcc_id - 1);
    elm_p = (st_cctelm_p) cct_p->cct_d.pt;
    ct    = 0;
    if (r_vcc_id > v_mam_p->rescb[PmrCctCb].ct) return FALSE;
   }

 do
   {
    if (cct)
      {
       ty    = elm_p->arg_ty;
       id    = elm_p->vcc_id;
       elm_p++;
       ct++;
       cct = ct < cct_p->ct;
      }
    else
      {
       ty    = r_ty;
       id    = r_vcc_id;
      }        

    if (ty ==  argtycon)
      {
       con_p = con1_p + (id - 1);
       val_d = con_p->val_d;
      }
    else
      {
       var_p = var1_p + (id - 1);
       val_d = var_p->val_d;
      }


    brc = PmmSetFill (setfunc, &val_d, set_dp);

    setfunc = setfilladd;
   }  while (cct);

 return brc;

}

rc_t PmmSetAdd
(
 st_mam_p       v_mam_p,
 us_int         r_id,
 us_int         r_vcc_id,
 us_int         r_ty
)
{
 rc_t          rc;
 bool          brc;
 st_setlst     sl;
 st_dsc        sl_d;
 st_dsc        set_d;
 ub_int        set[256];
 st_lstcb_p    cb_p;                  
 us_int        ct;
 st_conlst_p   con_p;
 st_varlst_p   var_p;
 st_cctlst_p   cct_p;
 
 cb_p  = &v_mam_p->rescb[PmrSetCb];

 sl_d.lg = cb_p->sz;
 sl_d.ty = 0;
 sl_d.pt = (ub_int_p) &sl;

 memset (&sl, 0, sl_d.lg);

 sl.id     = r_id;
 sl.vcc_id = r_vcc_id;
 sl.arg_ty = r_ty;
 sl.rdi    = r_id + 1;

 sl.set_d.lg = 256;

 rc = PmrElmAdd (&cb_p->lst_d, &sl_d, &cb_p->ct);

 con_p = (st_conlst_p) v_mam_p->rescb[PmrConCb].lst_d.pt;
 var_p = (st_varlst_p) v_mam_p->rescb[PmrVarCb].lst_d.pt;
 cct_p = (st_cctlst_p) v_mam_p->rescb[PmrCctCb].lst_d.pt;

 set_d.lg = sizeof (set);
 set_d.ty = 0;
 set_d.pt = set;

 brc = PmmSetSet (v_mam_p, r_vcc_id, r_ty, &set_d, con_p, var_p, cct_p);
 if (!brc) return RcRcdErr;

 rc = PmrElmAdd (&cb_p->elm_d, &set_d, &ct);

 PmrDesc ( v_mam_p, PmrSetCb);

 return rc;
}

bool PmmSetFind
(
 st_lstcb_p     v_lcb_p,
 us_int         r_vcc_id,
 us_int         r_arg_ty,
 st_setlst_p    *w_set_p
)
{
 us_int        ix;
 st_setlst_p   set_p;
                     
                      
 if (0 == v_lcb_p->ct) return FALSE;

 set_p = (st_setlst_p) v_lcb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, set_p++)
   {
    if ((r_vcc_id == set_p->vcc_id) && (r_arg_ty == set_p->arg_ty)) break;
   }

 if (ix == v_lcb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_set_p) = set_p;
    return TRUE;
   }
}

rc_t PmmSetForAdd
(
 st_mam_p       v_mam_p,
 us_int         r_vcc_id,
 us_int         r_arg_ty,
 st_setlst_p    *w_set_p
)
{
 rc_t          rc;
 
 if (PmmSetFind (&v_mam_p->rescb[PmrSetCb], r_vcc_id, r_arg_ty, w_set_p)) return RcRcdOk;

 rc = PmmSetAdd (v_mam_p, (us_int)(v_mam_p->rescb[PmrSetCb].ct + 1), r_vcc_id, r_arg_ty);
 if (RcRcdOk != rc) return rc;

 if (RcRcdOk == rc)
   {
    if (PmmSetFind (&v_mam_p->rescb[PmrSetCb], r_vcc_id, r_arg_ty, w_set_p)) return rc;
   }
 return RcRcdErr;
}


rc_t PmmPatAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp
)
{
 rc_t          rc;
 st_patlst     pl;
 st_dsc        pl_d;
 us_int        ct;
 st_lstcb_p    namcb_p;

 pl_d.lg = v_lcb_p->sz;
 pl_d.ty = 0;
 pl_d.pt = (ub_int_p) &pl;

 memset (&pl, 0, v_lcb_p->sz);

 pl.id    = v_lcb_p->ct + 1;

 namcb_p  = v_lcb_p - v_lcb_p->ix + PmrNamCb;

 rc = PmrNamAdd (namcb_p, r_nm_dp, pl.id, namtypat);

 pl.nm_id = namcb_p->ct;

 pl.rdi   = pl.id + 2; 

 rc = PmrElmAdd (&v_lcb_p->lst_d, &pl_d, &v_lcb_p->ct);
 if (rc != RcRcdOk) return rc;

 rc = PmrElmAdd (&v_lcb_p->elm_d, &pl.val_d, &ct);
 if (rc != RcRcdOk) return rc;

 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);

return rc;
}


rc_t PmmPatLstAdd
(
 st_lstcb_p     v_lcb_p,
 st_patlst_p    v_pat_p,
 st_dsc_p       r_lst_dp
)
{
 rc_t          rc;
 ul_int        key;
 st_dsc        key_d;

 key      = v_pat_p->rdi;
 key_d.lg = sizeof (key);
 key_d.ty = 0;
 key_d.pt = (ub_int_p) &key;

 rc = PmrElmAppend (v_lcb_p, r_lst_dp, v_pat_p->id);
 if (rc != RcRcdOk) return rc;

 v_pat_p->ct = r_lst_dp->lg / sizeof (st_exelst);

 rc = PmrDesc ((st_mam_p) (v_lcb_p - v_lcb_p->ix), PmrPatCb);

 return rc;
}

bool PmmPatFind
(
 st_lstcb_p     v_lcb_p,
 st_lstcb_p     v_ncb_p,
 st_sdc_p       r_nm_dp,
 st_patlst_p    *w_pat_p
)
{
 us_int        ix;
 us_int        lg;
 st_namlst_p   nam1_p;
 st_namlst_p   nam_p;

 st_patlst_p   pat_p;

 if (0 == v_lcb_p->ct) return FALSE;

 lg     = r_nm_dp->lg;
 pat_p  = (st_patlst_p) v_lcb_p->lst_d.pt;
 nam1_p = (st_namlst_p) v_ncb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, pat_p++)
   {
    nam_p = nam1_p + (pat_p->nm_id - 1);
    if (lg != (nam_p->nam_d.lg - 1)) continue;
    if (0 == memcmp (nam_p->nam_d.pt, r_nm_dp->pt, lg)) break;
   }

 if (ix == v_lcb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_pat_p) = pat_p;
    return TRUE;
   }
}

rc_t PmmPatForAdd
(
 st_lstcb_p     v_lcb_p,
 st_lstcb_p     v_ncb_p,
 st_sdc_p       r_nm_dp,
 st_patlst_p    *w_pat_p
)
{
 rc_t          rc;
 
 if (PmmPatFind (v_lcb_p, v_ncb_p, r_nm_dp, w_pat_p)) return RcRcdOk;

 rc = PmmPatAdd (v_lcb_p, r_nm_dp);
 if (RcRcdOk != rc) return rc;

 if (PmmPatFind (v_lcb_p, v_ncb_p, r_nm_dp, w_pat_p)) return rc;

 return RcRcdErr;
}



bool PmmRpnFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_rpnlst_p    *w_rpn_p
)
{
 us_int        ix;
 us_int        lg;

 st_rpnlst_p   rpn_p;


 if (0 == v_lcb_p->ct) return FALSE;

 lg    = r_nm_dp->lg;
 rpn_p = (st_rpnlst_p) v_lcb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, rpn_p++)
   {
    if (lg != rpn_p->nm_lg) continue;
    if (0 == memcmp (r_nm_dp->pt, rpn_p->nm, lg)) break;
   }

 if (ix == v_lcb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_rpn_p) = rpn_p;
    return TRUE;
   }
}

rc_t PmmRpnAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp
)
{
 rc_t          rc;
 st_rpnlst     rl;
 st_dsc        rl_d;
 us_int        ct;
 
 rl_d.lg = v_lcb_p->sz;
 rl_d.ty = 0;
 rl_d.pt = (ub_int_p) &rl;

 memset (&rl, 0, v_lcb_p->sz);

 rl.id    = v_lcb_p->ct + 1;
 rl.nm_lg = (ub_int) r_nm_dp->lg;
 memcpy (rl.nm, r_nm_dp->pt, rl.nm_lg);
 rl.rdi   = rl.id + 1; 
 
 rl.val_d.lg = r_val_dp->lg;
 
 rc = PmrElmAdd (&v_lcb_p->lst_d, &rl_d, &v_lcb_p->ct);

 rc = PmrElmAdd (&v_lcb_p->elm_d, (st_dsc_p) r_val_dp, &ct);
 
 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), PmrRpnCb);

 return rc;
}

rc_t PmmTptAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp
)
{
 rc_t          rc;
 st_tptlst     pl;
 st_dsc        pl_d;
 us_int        ct;

 pl_d.lg = v_lcb_p->sz;
 pl_d.ty = 0;
 pl_d.pt = (ub_int_p) &pl;

 memset (&pl, 0, v_lcb_p->sz);

 pl.id    = v_lcb_p->ct + 1;
 pl.nm_lg = (ub_int) r_nm_dp->lg;
 memcpy (pl.nm, r_nm_dp->pt, pl.nm_lg);
 pl.rdi   = pl.id + v_lcb_p->recno; 

 rc = PmrElmAdd (&v_lcb_p->lst_d, &pl_d, &v_lcb_p->ct);

 rc = PmrElmAdd (&v_lcb_p->elm_d, &pl.val_d, &ct);

 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);

 return RcRcdOk;
}

rc_t PmmTptExeAdd
(
 st_lstcb_p     v_lcb_p,
 st_tptlst_p    v_pat_p,
 st_exelst_p    v_exe_p
)
{
 rc_t          rc;
 st_dsc        el_d;

 el_d.lg = sizeof (st_exelst);
 el_d.ty = 0;
 el_d.pt = (ub_int_p) v_exe_p;

 rc = PmrElmAppend (v_lcb_p, &el_d, v_pat_p->id);
 if (rc != RcOk) return rc;

 PmrDesc ( (st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);

 return rc;
}


bool PmmTptFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_tptlst_p    *w_pat_p
)
{
 us_int        ix;
 us_int        lg;
 st_tptlst_p   pat_p;

 if (0 == v_lcb_p->ct) return FALSE;

 lg    = r_nm_dp->lg;
 pat_p = (st_tptlst_p) v_lcb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, pat_p++)
   {
    if (lg != pat_p->nm_lg) continue;
    if (0 == memcmp (r_nm_dp->pt, pat_p->nm, lg)) break;
   }

 if (ix == v_lcb_p->ct)
   {
    return FALSE;
   }
 else
   {
    (*w_pat_p) = pat_p;
    return TRUE;
   }
}

rc_t PmmTptForAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_tptlst_p    *w_pat_p
)
{
 rc_t          rc;
 
 if (PmmTptFind (v_lcb_p, r_nm_dp, w_pat_p)) return RcRcdOk;

 rc = PmmTptAdd (v_lcb_p, r_nm_dp);
 if (RcRcdOk != rc) return rc;

 if (RcRcdOk == rc)
   {
    if (PmmTptFind (v_lcb_p, r_nm_dp, w_pat_p)) return rc;
   }
 return RcRcdErr;
}

rc_t PmmTptDelete
(
 st_lstcb_p     v_lcb_p,
 st_tptlst_p    v_pat_p
)
{
 rc_t          rc;


 v_pat_p->ct = 0;

 rc = PmrLstDelete (v_lcb_p, v_pat_p->id);

 if (rc == RcOk)
   {
	v_lcb_p->ct--;
	v_lcb_p->lst_d.llg -= v_lcb_p->sz;
   }

 return rc;
}

rc_t PmmTptTest
(
 st_lstcb_p  v_lcb_p
)
{
 us_int      ix;
 ul_int      ct;
 ul_int      lg;
 st_patlst_p pat_p;
 ub_int_p    pt;

 ct    = 0;
 lg    = 0;
 pat_p = (st_patlst_p) v_lcb_p->lst_d.pt;
 pt    = v_lcb_p->elm_d.pt;

 
 for (ix = 0; ix < v_lcb_p->ct; ix++, pat_p++)
   {
    if (pat_p->val_d.pt != (pt + (ct * sizeof (st_exelst))))
	  {
		return RcError;
	  }
    if (pat_p->val_d.pt != (v_lcb_p->elm_d.pt + lg))
	  {
		return RcError;
	  }
	lg += pat_p->val_d.lg;
	ct += pat_p->ct;
   }
 if (lg != v_lcb_p->elm_d.llg)
   return RcError;
 else
   return RcOk;

}


bool PmmNamFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 ub_int_p      *nm_p,
 us_int_p       ty_p,
 us_int_p       id_p,
 us_int_p       fpv_id_p
)
{
 us_int        ix;
 us_int        ix2;
 us_int        lg;
 ub_int        c;

 st_namlst_p   nam_p;

 if (0 == v_lcb_p->ct) return FALSE;

 lg    = r_nm_dp->lg;
 nam_p = (st_namlst_p) v_lcb_p->lst_d.pt;

 for (ix = 0; ix < v_lcb_p->ct; ix++, nam_p++)
   {
    if (lg != (nam_p->nam_d.lg - 1)) continue;
    if (0 == memcmp (r_nm_dp->pt, nam_p->nam_d.pt, lg)) break;
    if (namtyfcn == nam_p->nam_ty)
      {
       for (ix2 = 0; ix2 < lg; ix2++)
         {
          c = *(r_nm_dp->pt + ix2);
          if (toupper(c) !=  *(nam_p->nam_d.pt + ix2)) break;
         }
       if (ix2 == lg) break;
      }
   }

 if (ix == v_lcb_p->ct)
   {
    (*ty_p)     = 0;
    (*id_p)     = 0;
    (*fpv_id_p) = 0;
    (*nm_p)     = 0;
    return FALSE;
   }
 else
   {
    (*ty_p)     = nam_p->nam_ty;
    (*id_p)     = nam_p->id;
    (*fpv_id_p) = nam_p->fpv_id;
    (*nm_p)     = nam_p->nam_d.pt;
    return TRUE;
   }
}
