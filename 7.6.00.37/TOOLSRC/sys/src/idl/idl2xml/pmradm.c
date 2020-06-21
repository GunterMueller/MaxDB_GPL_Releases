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



rc_t PmrInit 
(
 st_mam_p v_mam_p
)
{
 us_int     ix;
 st_lstcb_p cp;
  
 cp = v_mam_p->rescb;
 
 for (ix = 0; ix <= PmrNoCb; ix++, cp++)
   {
    cp->ix = ix;

    switch (ix)
      {
       case PmrAllCb:
         cp->sz = 0;
         break;
       case PmrFcnCb:
         cp->sz = sizeof(st_fcnlst);
         break;
       case PmrPatCb:
         cp->sz = sizeof(st_patlst);
         break;
       case PmrConCb:
         cp->sz = sizeof(st_conlst);
         break;
       case PmrVarCb:
         cp->sz = sizeof(st_varlst);
         break;
       case PmrCctCb:
         cp->sz = sizeof(st_cctlst);
         break;
       case PmrNamCb:
         cp->sz = sizeof(st_namlst);
         break;
       case PmrSetCb:
         cp->sz = sizeof(st_setlst);
         break;
       case PmrPdfCb:
         cp->sz = sizeof(st_pdflst);
         break;
       case PmrRpnCb:
         cp->sz = sizeof(st_rpnlst);
         break;
       case PmrTptCb:
         cp->sz = sizeof(st_tptlst);
         break;
       case PmrArgCb:
         cp->sz = sizeof(st_tptlst);
         break;
      }
   }
 return RcOk;
}

rc_t PmrDesc 
(
 st_mam_p v_mam_p,
 s_int    desc_ix
)
{
 us_int      ix;
 us_int      ix2;
 us_int      from_ix;
 us_int      to_ix;
 ul_int      lg;
 st_lstcb_p  cp;
 ub_int_p    lp;
 ub_int_p    ep;
 st_fcnlst_p fcn_p;  
 st_patlst_p pat_p;  
 st_patold_p patold_p;  
 st_conlst_p con_p;  
 st_varlst_p var_p;  
 st_varold_p varold_p;  
 st_cctlst_p cct_p;  
 st_namlst_p nam_p;  
 st_setlst_p set_p;  
 st_pdflst_p pdf_p;  
 st_rpnlst_p rpn_p;  
 st_tptlst_p tpt_p;  
 st_varlst_p arg_p;  
 
 if (desc_ix < 0)
   {
    from_ix = 0;
    to_ix   = PmrNoCb - 1;
   }
 else
   {
    from_ix = desc_ix;
    to_ix   = desc_ix;
   }
 cp   = v_mam_p->rescb;
 cp  += from_ix;
 
 for (ix = from_ix; ix <= to_ix; ix++, cp++)
   {
    lg = 0;
    lp = cp->lst_d.pt;
    ep = cp->elm_d.pt;
    
    for (ix2 = 0; ix2 < cp->ct; ix2++)
      {
         
       switch (cp->ix)
         {
          case PmrAllCb:
             break;
          case PmrFcnCb:
             fcn_p = ((st_fcnlst_p)lp) + ix2;
             break;
          case PmrPatCb:
			 if (cp->sz == sizeof(st_patlst))
			   {
                pat_p = ((st_patlst_p)lp) + ix2;
                pat_p->val_d.pt = ep + lg;
                pat_p->ct = pat_p->val_d.lg / sizeof(st_exelst);
                lg += pat_p->val_d.lg;
			   }
     		 else
			   {
                patold_p = ((st_patold_p)lp) + ix2;
                patold_p->val_d.pt = ep + lg;
                patold_p->ct = patold_p->val_d.lg / sizeof(st_exelst);
                lg += patold_p->val_d.lg;
			   }
			 break;

          case PmrConCb:
             con_p = ((st_conlst_p)lp) + ix2;
             con_p->val_d.pt = ep + lg;
             lg += con_p->val_d.lg;
             break;
          case PmrVarCb:
 			    if (cp->sz == sizeof(st_varlst))
               {
                var_p = ((st_varlst_p)lp) + ix2;
                var_p->val_d.pt = ep + lg;
                lg += var_p->val_d.lg;
               }
             else
               {
                varold_p = ((st_varold_p)lp) + ix2;
                varold_p->val_d.pt = ep + lg;
                lg += varold_p->val_d.lg;
               }
             break;
          case PmrCctCb:
             cct_p = ((st_cctlst_p)lp) + ix2;
             cct_p->cct_d.pt = ep + lg;
             lg += cct_p->cct_d.lg;
             break;
          case PmrNamCb:
             nam_p = ((st_namlst_p)lp) + ix2;
             nam_p->nam_d.pt = ep + lg;
             lg += nam_p->nam_d.lg;
             break;
          case PmrSetCb:
             set_p = ((st_setlst_p)lp) + ix2;
             set_p->set_d.pt = ep + lg;
             lg += set_p->set_d.lg;
             break;
          case PmrPdfCb:
             pdf_p = ((st_pdflst_p)lp) + ix2;
             break;
          case PmrRpnCb:
             rpn_p = ((st_rpnlst_p)lp) + ix2;
             rpn_p->val_d.pt = ep + lg;
             lg += rpn_p->val_d.lg;
             break;
          case PmrTptCb:
             tpt_p = ((st_tptlst_p)lp) + ix2;
             tpt_p->val_d.pt = ep + lg;
             tpt_p->ct = tpt_p->val_d.lg / sizeof(st_exelst);
             lg += tpt_p->val_d.lg;
             break;
          case PmrArgCb:
             arg_p = ((st_varlst_p)lp) + ix2;
             arg_p->val_d.pt = ep + lg;
             lg += arg_p->val_d.lg;
             break;
         }
      }
   }
 return RcOk;
}

rc_t PmrDescAll 
(
 st_mam_p v_mam_p
)
{
 return PmrDesc (v_mam_p, -1); 
}

rc_t PmrElmAdd
(
 st_mdc_p       v_d_p,
 st_dsc_p       r_rcd_dp,            /* pointer to record descriptor        */
 us_int_p       v_ct_p
)
{
 ub_int_p  pt;
 l_int     sz;

/* if (_HEAPOK != _heapchk ()) 
 	sz = 0;
    sz = required over available memory */
 sz = (v_d_p->llg + r_rcd_dp->lg) - v_d_p->lml;
 
 if (sz > 0)
   {
    /* required memory in multiples of PmrAllocSize */
    sz = ((sz + PmrAllocSize - 1) / PmrAllocSize) * PmrAllocSize;
    pt = v_d_p->pt;

    /* request enough memory */
    v_d_p->pt = RteStdRealloc (pt, v_d_p->lml + sz);
    if (v_d_p->pt == 0) return RcError;

	/* clear additional memory */
    memset (v_d_p->pt + v_d_p->lml, 0, sz);

    /* update maximum length */
    v_d_p->lml += (us_int) sz;
   }

 /* copy new element */
 memcpy (v_d_p->pt + v_d_p->llg, 
         r_rcd_dp->pt, r_rcd_dp->lg);
 
 /* update current length */
 v_d_p->llg += r_rcd_dp->lg;
 
 /* increment no. of elements */
 (*v_ct_p)++;
 
 return RcOk;
}

rc_t PmrElmInsert
(
 st_mdc_p       v_d_p,
 st_dsc_p       r_rcd_dp,            /* pointer to record descriptor        */
 ul_int         r_lg,            
 us_int_p       v_ct_p
)
{
 ub_int_p  pt;
 ub_int_p  pt2;
 l_int     sz;
 ul_int    ix;
 ul_int    lg;

 /* sz = required over available memory */
 sz = (v_d_p->llg + r_rcd_dp->lg) - v_d_p->lml;
 
 if (sz > 0)
   {
    /* required memory in multiples of PmrAllocSize */
    sz = ((sz + PmrAllocSize - 1) / PmrAllocSize) * PmrAllocSize;
    pt = v_d_p->pt;

    /* request enough memory */
    v_d_p->pt = RteStdRealloc (pt, v_d_p->lml + sz);
    if (v_d_p->pt == 0) return RcError;

	/* clear additional memory */
    memset (v_d_p->pt + v_d_p->lml, 0, sz);

    /* update maximum length */
    v_d_p->lml += (us_int) sz;
   }


 if (r_lg < v_d_p->llg)
   {
    lg  = v_d_p->llg - r_lg;
    pt  = v_d_p->pt + v_d_p->llg + r_rcd_dp->lg - 1;
    pt2 = v_d_p->pt + v_d_p->llg - 1;
    ix  = lg;
    
	for (ix = lg; ix > 0; ix--, pt--, pt2--)
	  {
	   *pt = *pt2;
	  }
   }

 /* copy new element */
 memcpy (v_d_p->pt + r_lg, 
         r_rcd_dp->pt, r_rcd_dp->lg);
 
 /* update current length */
 v_d_p->llg += r_rcd_dp->lg;

 /* increment no. of elements */
 (*v_ct_p)++;
 
 return RcOk;
}

rc_t PmrElmAppend
(
 st_lstcb_p     v_lcb_p,
 st_dsc_p       r_rcd_dp,            /* pointer to record descriptor        */
 us_int         r_id
)
{
 rc_t      rc;
 us_int    ix;
 ul_int    lg;
 ub_int_p  pt;
 st_dsc_p  dp;
 us_int_p  ctp;
 us_int    dmy_ct;
 
 pt = v_lcb_p->lst_d.pt;
 lg = 0;
 
 for (ix = 0; ix < r_id; ix++)
   {
    switch (v_lcb_p->ix)
      {
       case PmrConCb:
         dp  = &((st_conlst_p)pt)->val_d; 
         ctp = &dmy_ct; 
         break;
       case PmrVarCb:
         dp  = &((st_varlst_p)pt)->val_d; 
         ctp = &dmy_ct; 
         break;
       case PmrPatCb:
         dp  = &((st_patlst_p)pt)->val_d; 
         ctp = &((st_patlst_p)pt)->ct; 
         break;
       case PmrTptCb:
         dp  = &((st_tptlst_p)pt)->val_d; 
         ctp = &((st_tptlst_p)pt)->ct; 
         break;
       case PmrCctCb:
         dp  = &((st_cctlst_p)pt)->cct_d; 
         ctp = &((st_cctlst_p)pt)->ct; 
         break;
       case PmrNamCb:
         dp  = &((st_namlst_p)pt)->nam_d; 
         ctp = &dmy_ct; 
         break;
       default:
         return RcError;
         break;
      }
    pt += v_lcb_p->sz;
    lg += dp->lg;
   }
 if (v_lcb_p->elm_d.llg == lg)
   {
    rc = PmrElmAdd (&v_lcb_p->elm_d, r_rcd_dp, ctp);
    dp->pt = v_lcb_p->elm_d.pt + lg - dp->lg;
    dp->lg += r_rcd_dp->lg;
   }
 else
   {
    rc = PmrElmInsert (&v_lcb_p->elm_d, r_rcd_dp, lg, ctp);
    dp->pt = v_lcb_p->elm_d.pt + lg - dp->lg;
    dp->lg += r_rcd_dp->lg;
    rc = PmrDesc ((st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);
   }          
 return RcOk;
}


rc_t PmrElmDelete
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id,
 ul_int_p       w_lg,
 st_dsc_p      *w_dp
)
{
 us_int    ix;
 us_int    dmy_ct;
 ul_int    lg;
 ub_int_p  pt;
 st_dsc_p  dp;
 us_int_p  ctp;

 pt = v_lcb_p->lst_d.pt;
 lg = 0;
 
 for (ix = 0; ix < r_id; ix++)
   {
    switch (v_lcb_p->ix)
      {
       case PmrVarCb:
         dp  = &((st_varlst_p)pt)->val_d; 
         ctp = &dmy_ct;
         break;
       case PmrConCb:
         dp  = &((st_conlst_p)pt)->val_d; 
         ctp = &dmy_ct;
         break;
       case PmrPatCb:
         dp  = &((st_patlst_p)pt)->val_d; 
         ctp = &((st_patlst_p)pt)->ct; 
         break;
       case PmrTptCb:
         dp  = &((st_tptlst_p)pt)->val_d; 
         ctp = &((st_tptlst_p)pt)->ct; 
         break;
       case PmrCctCb:
         dp  = &((st_cctlst_p)pt)->cct_d; 
         ctp = &((st_cctlst_p)pt)->ct; 
         break;
       case PmrNamCb:
         dp  = &((st_namlst_p)pt)->nam_d; 
         ctp = &dmy_ct;
         break;
       default:
         return RcError;
         break;
      }
    pt += v_lcb_p->sz;
    lg += dp->lg;
   }

 if (v_lcb_p->elm_d.llg != lg)
   {
    memmove (dp->pt, dp->pt + dp->lg, v_lcb_p->elm_d.llg - lg);
   }          

 v_lcb_p->elm_d.llg -= dp->lg;
 
 lg    -= dp->lg;
 dp->lg = 0;
 *ctp   = 0;
 *w_dp  = dp;
 *w_lg  = lg;

 return RcOk;
}

rc_t PmrLstDelete
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id
)
{
 st_dsc_p  dp;
 ul_int    lg;

 return PmrElmDelete (v_lcb_p, r_id, &lg, &dp);
}

rc_t PmrElmReplace
(
 st_lstcb_p     v_lcb_p,
 st_dsc_p       r_rcd_dp,            /* pointer to record descriptor        */
 us_int         r_id
)
{
 rc_t      rc;
 ul_int    lg;
 st_dsc_p  dp;
 us_int    ct;
 
 rc = PmrElmDelete (v_lcb_p, r_id, &lg, &dp);

 rc = PmrElmInsert (&v_lcb_p->elm_d, r_rcd_dp, lg, &ct);

 dp->lg = r_rcd_dp->lg;

 rc = PmrDesc ((st_mam_p) (v_lcb_p - v_lcb_p->ix), v_lcb_p->ix);

 return RcOk;
}


void PmrTermFree 
(
 st_mam_p v_mam_p
)
{
 us_int         ix;
 st_lstcb_p     cb;
 st_fltlst_p    flt_p;

 cb = v_mam_p->rescb;

 for (ix = 0; ix < PmrNoCb; ix++, cb++)
   {
    if (cb->lst_d.pt != 0)
      {
       RteStdFree (cb->lst_d.pt); 
      }
    if (cb->elm_d.pt != 0)
      {
       RteStdFree (cb->elm_d.pt); 
      }
   }

 if (0 != v_mam_p->fltcb.lst_d.pt)
    {
	 flt_p = (st_fltlst_p) v_mam_p->fltcb.lst_d.pt;
	 for (ix = 0; ix < v_mam_p->fltcb.ct; ix++, flt_p++)
	   {
        if (0 != flt_p->val_d.pt)
 	      RteStdFree (flt_p->val_d.pt);
	   }
 	 RteStdFree (v_mam_p->fltcb.lst_d.pt);
    }
}

rc_t PmrTerm 
(
 st_mam_p v_mam_p
)
{
 rc_t       rc;
 us_int     ix;
 us_int     ix2;
 us_int     ct;
 us_int     no;
 st_lstcb_p cp;
 ul_int     sz;
 char       fn[64];
 file_p     ch;            
 st_lstcb_p cb;
 st_lstcb_p cb1;
             
 rc = PmrDescAll (v_mam_p);             
 cp = v_mam_p->rescb;
 sz = 0;

 cp->recno = PmrFilID;
        
 cb1 = RteStdMalloc (PmrAllocSize);       
 
 memset (cb1, 0, sizeof(PmrAllocSize));
  
 cb = cb1;
  
 for (ix = 0; ix < PmrPermCb; ix++, cp++, cb++)
   {
    sz += cp->lst_d.lml + cp->elm_d.lml;
    *cb = *cp;
   }
 
 strcpy (fn, v_mam_p->filename);
 ct = (us_int)(sz / PmrAllocSize);
 rc = RteBioAllocate (fn, (s_uint)(ct + 1), PmrAllocSize);
 if (rc != RcRteOk) return RcError;

 rc = RteBioOpen (fn, &ch);
 if (rc != RcRteOk) return RcError;
 
 no = 1;
 rc = RteBioBlock (ch, no, PmrAllocSize, bio_write, (b_uint_p) cb1);
 if (rc != RcRteOk) return RcError;
 
 RteStdFree (cb1);       

 no = 2;
 cp = v_mam_p->rescb;
 
 for (ix = 0; ix < PmrPermCb; ix++, cp++)
   {
    ct = (us_int) (cp->lst_d.lml / PmrAllocSize);
    for (ix2 = 0; ix2 < ct; ix2++)
      { 
       rc = RteBioBlock (ch, no, PmrAllocSize, bio_write, 
                         cp->lst_d.pt + PmrAllocSize * ix2);
       if (rc != RcRteOk) return RcError;
       no++;
      }
    ct = (us_int) (cp->elm_d.lml / PmrAllocSize);
    for (ix2 = 0; ix2 < ct; ix2++)
      {
       rc = RteBioBlock (ch, no, PmrAllocSize, bio_write,
                         cp->elm_d.pt + PmrAllocSize * ix2);
       if (rc != RcRteOk) return RcError;
       no++;
      }
   }

 rc = RteBioClose (&ch);

 PmrTermFree (v_mam_p);
 
 return RcOk;
}

rc_t PmrFilOpen 
(
 st_mam_p v_mam_p,
 char     *fn
)
{
 rc_t       rc;
 us_int     ix;
 us_int     ix2;
 us_int     ct;
 us_int     no;
 st_lstcb_p cp;
 ul_int     sz;
 file_p     ch;            
 st_lstcb_p cb;
 st_lstcb_p cb1;
 ub_int_p   pt;
             
 rc = RteBioOpen (fn, &ch);
 if (rc != RcRteOk) return RcError;
 
 cb1 = RteStdMalloc (PmrAllocSize);       

 
 no = 1;
 rc = RteBioBlock (ch, no, PmrAllocSize, bio_read, (b_uint_p) cb1);
 if (rc != RcRteOk) return RcError;

 cp = v_mam_p->rescb;
 cb = cb1;
 sz = 0;

 if (cb->recno != PmrFilID) return RcWarning;

 for (ix = 0; ix < PmrPermCb; ix++, cp++, cb++)
   {
    *cp = *cb;
    sz += cp->lst_d.lml + cp->elm_d.lml;
   }
 
 RteStdFree (cb1);

 pt = 0;
 pt = RteStdRealloc (pt, sz);
 
 no = 2;
 cp = v_mam_p->rescb;
 
 for (ix = 0; ix < PmrPermCb; ix++, cp++)
   {
    ct = (us_int) (cp->lst_d.lml / PmrAllocSize);
    cp->lst_d.pt = pt;
    
    for (ix2 = 0; ix2 < ct; ix2++)
      {
       rc = RteBioBlock (ch, no, PmrAllocSize, bio_read, pt);
       if (rc != RcRteOk) return RcError;
       pt += PmrAllocSize;
       no++;
      }

    ct = (us_int) (cp->elm_d.lml / PmrAllocSize);
    cp->elm_d.pt = pt;

    for (ix2 = 0; ix2 < ct; ix2++)
      {
       rc = RteBioBlock (ch, no, PmrAllocSize, bio_read, pt);
       if (rc != RcRteOk) return RcError;
       pt += PmrAllocSize;
       no++;
      }
   }

 rc = RteBioClose (&ch);
 
 rc = PmrDescAll (v_mam_p);             

 cp = v_mam_p->rescb;

 for (ix = 0; ix < PmrPermCb; ix++, cp++)
   *(cp + PmrNoCb) = *cp;

 PmrInit (v_mam_p);

 return RcOk;
}

rc_t PmrPatOpen
(
 st_mam_p v_mam_p
)
{
 us_int        ix;
 us_int        ix2;
 ul_int        sz;
 ub_int_p      pt;
 st_patold_p   patold_p;
 st_patlst_p   pat_p;
 st_namlst_p   nam_p;

 v_mam_p->PatternListMoved = FALSE;
 
 sz = v_mam_p->patcb.ct * sizeof (st_patlst);

 if ((mxnmsize == mxpnmsize) ||
	 (v_mam_p->patcb.lst_d.llg == sz))
   return RcOk;

 pt = 0;
 pt = RteStdRealloc (pt, sz);

 if (pt == 0) 
   return RcError;

 memset (pt, 0, sz);
 
 pat_p    = (st_patlst_p) pt;
 patold_p = (st_patold_p) v_mam_p->patcb.lst_d.pt;
 nam_p    = (st_namlst_p) v_mam_p->namcb.lst_d.pt;

 for (ix = 0; ix < v_mam_p->patcb.ct; ix++, pat_p++, patold_p++)
   {
	 pat_p->id     = patold_p->id;
	 pat_p->rdi    = patold_p->rdi;
	 pat_p->ct     = patold_p->ct;
	 pat_p->val_d  = patold_p->val_d;
	 memcpy (pat_p->ty, patold_p->ty, sizeof(patold_p->ty));
   
    nam_p    = (st_namlst_p) v_mam_p->namcb.lst_d.pt;

    for (ix2 = 0; ix2 <  v_mam_p->namcb.ct; ix2++, nam_p++)
      {
       if (nam_p->nam_ty != namtypat) continue;
       if (nam_p->fpv_id == (ix + 1)) break;
      }
 	 pat_p->nm_id  = nam_p->id;
   }

 v_mam_p->patcb.lst_d.pt   = pt;
 v_mam_p->patcb.sz         = sizeof (st_patlst);
 v_mam_p->PatternListMoved = TRUE;

 return RcOk;
}

rc_t PmrStrAdd
(
 st_mdc_p       v_strstk_p,
 st_mdc_p       r_str_dp            /* pointer to new string        */
)
{
 ub_int_p  pt;
 l_int     sz;

/* if (_HEAPOK != _heapchk ()) */
/*	sz = 0; */
/* sz = required over available memory */

 sz = (v_strstk_p->llg + r_str_dp->llg) - v_strstk_p->lml;
 
 if (sz > 0)
   {
    /* required memory in multiples of PmrAllocSize */
    sz = ((sz + PmrAllocSize - 1) / PmrAllocSize) * PmrAllocSize;
    pt = v_strstk_p->pt;

    /* request enough memory */
    v_strstk_p->pt = RteStdRealloc (pt, v_strstk_p->lml + sz);
    if (v_strstk_p->pt == 0) return RcError;

	/* clear additional memory */
    memset (v_strstk_p->pt + v_strstk_p->lml, 0, sz);

    /* update maximum length */
    v_strstk_p->lml += (us_int) sz;
   }

 /* copy new element */
 memcpy (v_strstk_p->pt + v_strstk_p->llg, 
         r_str_dp->pt, r_str_dp->llg);
 
 /* update current length */
 v_strstk_p->llg += r_str_dp->llg;
 
 
 return RcOk;
}
