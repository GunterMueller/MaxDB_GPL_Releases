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
/* #include "pms.h" */
#include "idl/idl2xml/pms00.h" 
#include "idl/idl2xml/pmexec.h"
#include "idl/idl2xml/pmmadm.h"
#include "idl/idl2xml/pmradm.h"
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmdesc.h" 
#include "idl/idl2xml/pmstck.h"
#include "idl/idl2xml/pmprim.h" 
#include "idl/idl2xml/pmsmsg.h"
#include "idl/idl2xml/pmbufr.h"

#define M__NAME "PMSFX"

st_fltlst_p PmFlatList 
(
 st_mam_p  mam_p
)
{
 ub_int_p   pt;
 ul_int     sz;

 pt = 0;
 sz = mam_p->patcb.ct * sizeof (st_fltlst);
 pt = RteStdRealloc (pt, sz);
 mam_p->fltcb.lst_d.pt = pt;
 mam_p->fltcb.ct       = mam_p->patcb.ct;
 memset (pt, 0, sz);
 return (st_fltlst_p) pt;
}


ul_int PmFlatPatCount 
(
 st_patlst_p  pat1_p,
 us_int       pat_id
)
{
 st_patlst_p  pat_p;
 st_exego_p   ego_p;
 ul_int       ct;
 ul_int       pat_ct;
 ul_int       ix;


 ct     = 0;
 pat_p  = pat1_p + pat_id - 1;
 pat_ct = pat_p->ct;
 ego_p  = (st_exego_p) pat_p->val_d.pt;
 
 for (ix = 1; ix <= pat_ct; ix++, ego_p++)
   {
	if (ego_p->fcn_ty == fcntysub)
	  ct += PmFlatPatCount (pat1_p, ego_p->vcc_id);
   }
 return pat_ct + ct;
}

void PmFlatPatIndex 
(
 st_patlst_p  pat1_p,
 st_fxelst_p  fxe1_p,
 us_int       pat_id,
 ul_int_p     offp,
 ul_int_p     ctp
)
{
 ul_int       ct;
 ul_int       pat_ct;
 ul_int       ix;

 st_patlst_p  pat_p;
 st_exego_p   ego_p;
 st_fxilst_p  fxi_p;

 ct       = 0;
 pat_p    = pat1_p + pat_id - 1;
 pat_ct   = pat_p->ct;
 ego_p    = (st_exego_p) pat_p->val_d.pt;
 fxi_p    = (st_fxilst_p) (fxe1_p + *offp);
 (*offp) += pat_ct;

 for (ix = 1; ix <= pat_ct; ix++, ego_p++, fxi_p++)
   {
    (*ctp)++;
	fxi_p->pos_ix = *ctp;
	if (ego_p->fcn_ty == fcntysub)
	  PmFlatPatIndex (pat1_p, fxe1_p, ego_p->vcc_id, offp, ctp);
    
   }

 return;
}

void PmFlatPatBuild 
(
 st_patlst_p  pat1_p,
 st_fxelst_p  fxe1_p,
 us_int       pat_id,
 ul_int       succ_up,
 ul_int_p     ctp
)
{
 ul_int       ct;
 ul_int       pat_ct;
 ul_int       ix;
 ul_int       succ;
 ul_int       succ_down;
 ul_int       alt;

 st_patlst_p  pat_p;
 st_exego_p   ego_p;
 st_fxelst_p  fxe_p;
 st_fxelst_p  sav_p;
 st_fxilst_p  fxi_p;

 ct     = *ctp;
 pat_p  = pat1_p + pat_id - 1;
 pat_ct = pat_p->ct;
 ego_p  = (st_exego_p) pat_p->val_d.pt;
 fxi_p  = (st_fxilst_p) (fxe1_p + *ctp);
 fxe_p  = (st_fxelst_p) (fxe1_p + *ctp);
 sav_p  = fxe_p;
 (*ctp) += pat_ct; 

 for (ix = 1; ix <= pat_ct; ix++, ego_p++)
   {
	succ = ego_p->succ;
	alt  = ego_p->alt;
  
	if (alt != 0)
	  alt = (fxi_p + alt - 1)->pos_ix;
 
	if (succ != 0)
	  succ = (fxi_p + succ - 1)->pos_ix;

	if (ego_p->fcn_ty == fcntysub)
	  {
	   if (0 == succ)
		 succ_down = succ_up;
	   else
		 succ_down = succ;

	   PmFlatPatBuild (pat1_p, fxe1_p, ego_p->vcc_id, succ_down, ctp);

	   succ = (fxi_p + ix - 1)->pos_ix + 1;
	  }
    
	fxe_p = fxe1_p + (fxi_p + (ix - 1))->pos_ix - 1;
	fxe_p->alt = alt;
	
	if (0 == succ)
      fxe_p->succ = succ_up;
	else
	  fxe_p->succ = succ;

   }
 /* 
 if (ct > 0)
   {
    for (ix = 1; ix <= pat_ct; ix++, fxi_p++)
      {
       fxe_p = fxe1_p + fxi_p->pos_ix - 1;
	   if (0 == fxe_p->succ)
		  fxe_p->succ = 1 + *ctp;
	  }
   }
 */

 return;
}

void PmFlatPatExe 
(
 st_fxelst_p  *fxe_pp,
 st_patlst_p  pat1_p,
 us_int       pat_id
)
{
 st_patlst_p  pat_p;
 st_exego_p   ego_p;
 ul_int       pat_ct;
 us_int       ix;

 pat_p  = pat1_p + pat_id - 1;
 pat_ct = pat_p->ct;
 ego_p  = (st_exego_p) pat_p->val_d.pt;
 
 for (ix = 1; ix <= pat_ct; ix++, ego_p++)
   {
	(*fxe_pp)->pat_id = pat_id;
	(*fxe_pp)->exe_ix = ix;
    (*fxe_pp)++;
	if (ego_p->fcn_ty == fcntysub)
	  PmFlatPatExe (fxe_pp, pat1_p, ego_p->vcc_id);
   }
 return;
}

st_fxelst_p PmFlatPat
(
 st_mam_p     mam_p,
 us_int       pat_id
)
{
 st_patlst_p  pat1_p;
 st_patlst_p  pat_p;
 st_fltlst_p  flt1_p;
 st_fltlst_p  flt_p;
 st_fxelst_p  fxe1_p;
 st_fxelst_p  fxe_p;
 ul_int       ct;
 ul_int       off;
 ul_int       succ;

 pat1_p  = (st_patlst_p) mam_p->patcb.lst_d.pt;
 pat_p   = pat1_p + pat_id - 1;
 flt1_p  = (st_fltlst_p) mam_p->fltcb.lst_d.pt;
 flt_p   = flt1_p + pat_id - 1;

 ct = PmFlatPatCount (pat1_p, pat_id);

 fxe1_p = 0;
 fxe1_p = (st_fxelst_p) RteStdRealloc (fxe1_p, ct * sizeof (st_fxelst)); 
 if (0 == fxe1_p) return fxe1_p;
 flt_p->val_d.pt = (ub_int_p) fxe1_p;
 
 ct  = 0;
 off = 0;

 PmFlatPatIndex (pat1_p, fxe1_p, pat_id, &off, &ct);
 
 ct   = 0;
 succ = 0;
 PmFlatPatBuild (pat1_p, fxe1_p, pat_id, succ, &ct);

 fxe_p = fxe1_p;

 PmFlatPatExe (&fxe_p, pat1_p, pat_id);
 
 return fxe1_p;
}

