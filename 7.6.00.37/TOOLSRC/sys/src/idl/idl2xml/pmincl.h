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

typedef unsigned char  ub_int, *ub_int_p;
typedef unsigned short us_int, *us_int_p;
typedef unsigned long  ul_int, *ul_int_p;

#endif

#ifndef _MAM_h

typedef struct
{
 unsigned short lg;                  
 unsigned short ty;                  
 unsigned char *pt;                  
} st_dsc, *st_dsc_p;

typedef struct
{
 st_dsc_p       table_dp;
 unsigned short ix;
 unsigned short id;
} MamHandle, *MamHandlep;

#endif

#ifndef _PMS00_h

#define  mxnmsize  15              

typedef struct
{
 us_int  id;
 us_int  fpv_id;
 us_int  nam_ty;
 us_int  lst_ty;
 st_dsc  nam_d;
} st_namlst, *st_namlst_p;


typedef struct
{
 ub_int  ty;                       
 ub_int  id;                       
 us_int  res;                      
 ub_int  nm_lg;                    
 char    nm [mxnmsize];            
} st_fcnlst, *st_fcnlst_p;

typedef struct
{
 us_int  id;
 us_int  nm_id;
 us_int  rdi;
 us_int  ct;
 char  ty[2];
 st_dsc  val_d;
} st_patlst, *st_patlst_p;


typedef struct
{
 us_int  succ;                     
 us_int  alt;                      
 ub_int  fcn_ty;                   
 ub_int  fcn_id;                   
 ub_int  ass_ty;                   
 ub_int  arg_ty;                   
 us_int  vcc_id;                   
 us_int  ctmod;                    
 us_int  curs;                     
} st_exelst, *st_exelst_p;

typedef struct
{
 st_exelst elm;
 us_int  curs;                     
} st_exeneu, *st_exeneu_p;

typedef struct
{
 us_int  id;
 us_int  nm_id;
 us_int  ctmod;
 us_int  rdi;
 ub_int  nocon;
 ub_int  var_ty;
 st_dsc  val_d;
} st_varlst, *st_varlst_p;

typedef struct
{
 us_int  id;                       
 us_int  rdi;                      
 st_dsc  val_d;                    
} st_conlst, *st_conlst_p;

typedef struct
{
 us_int  id;
 us_int  fpv_id;
 us_int  nam_id;
 us_int  nam_ty;
 ul_int  stm_nm;
 ul_int  stm_op;
 ul_int  stm_beg;
 ul_int  stm_end;
 ub_int  stm_ty;
 ub_int  dmy;
 us_int  res;
} st_pdflst, *st_pdflst_p;


typedef struct
{
 us_int  id;                       
 us_int  vcc_id;                   
 us_int  arg_ty;                   
 us_int  rdi;                      
 us_int  ctmod;                    
 us_int  nocon;                    
 st_dsc  set_d;                    
} st_setlst, *st_setlst_p;

typedef struct
{
 us_int  id;                       
 us_int  ct;                       
 us_int  rdi;                      
 us_int  res;                      
 st_dsc  cct_d;                    
} st_cctlst, *st_cctlst_p;

typedef struct
{
 us_int  voc_id;                   
 us_int  arg_ty;                   
 us_int  ctmod;                    
 us_int  res;                      
} st_cctelm, *st_cctelm_p;

typedef struct
{
 us_int  id;                       
 us_int  ct;                       
 us_int  rdi;                      
 us_int  res;                      
 st_dsc  sub_d;                    
} st_sublst, *st_sublst_p;

typedef struct                     
{
 us_int      pat_id;               
 us_int      node;                  
 us_int      rref;                 
 us_int      fref;                 
} st_subelm, *st_subelm_p;
/*
typedef struct                       
{
 ul_int         llg;                 
 ul_int         lml;                 
 ub_int_p       pt;                  
} st_mdc, *st_mdc_p;
*/
typedef struct
{
 MamHandle   RcdH;                  
 st_mdc      lst_d;                 
 st_mdc      elm_d;                 
 us_int      ix;                    
 us_int      ct;                    
 us_int      sz;                      
 us_int      recno;                  
}st_lstcb, *st_lstcb_p;

bool PmExecInitH
(
 st_pms_p    pms,
 st_lstcb_p  CbList_p,
 char_p      trc_p
);

bool PmExecInitHSet
(
 st_pms_p        pms,
 st_lstcb_p      CbList_p,
 char_p          trc_p,
 unsigned short  mxnalt,            /* maximum no. of alternations    */
 unsigned short  mxnass,            /* maximum no. of assignments     */
 unsigned short  mxnincarn,         /* maximum no. of incarnations    */
 unsigned short  mxnprec,           /* maximum no. of precursors      */
 unsigned short  mxnsteps           /* maximum no. of execution steps */
);

#endif
