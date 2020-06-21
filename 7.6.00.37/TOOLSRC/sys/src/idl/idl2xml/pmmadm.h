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
#define PmrAllocSize 512

#define PmrNoCb   12
#define PmrPermCb 9
#define PmrAllCb  0
#define PmrFcnCb  1
#define PmrPatCb  2
#define PmrConCb  3
#define PmrVarCb  4
#define PmrCctCb  5
#define PmrNamCb  6
#define PmrSetCb  7
#define PmrPdfCb  8
#define PmrRpnCb  9
#define PmrTptCb  10
#define PmrArgCb  11


typedef struct
{
 RcdHandle   RcdH;                  /* record handle without index          */
 st_mdc      lst_d;                 /* list descriptor                      */
 st_mdc      elm_d;                 /* element descriptor                   */
 us_int      ix;                    /* position in lstcb list               */
 us_int      ct;                    /* list count: no. of elements          */
 us_int      sz;                    /* element size                         */  
 us_int      recno;                 /* record number                        */ 
}st_lstcb, *st_lstcb_p;

typedef struct
{
 DstHandle       DstH;
 RcdHandle       RcdH;
}st_mlhandle, *st_mlhandle_p;

typedef struct
{
 st_lstcb        rescb[PmrNoCb];
 st_lstcb        allcb;             /* not used so far                      */
 st_lstcb        fcncb;             /* function control block               */
 st_lstcb        patcb;             /* pattern control block                */
 st_lstcb        concb;             /* constant control block               */
 st_lstcb        varcb;             /* variable control block               */
 st_lstcb        cctcb;             /* concatenation control block          */
 st_lstcb        namcb;             /* GOSUB control block                  */
 st_lstcb        setcb;             /* set control block                    */
 st_lstcb        pdfcb;             /* control block for PDF file           */
 st_lstcb        rpncb;             /* RPN control block                    */
 st_lstcb        tptcb;             /* control block for temporary patterns */
 st_lstcb        fltcb;             /* dymanic fltcb                        */
 FilHandle       FilH;
 DstHandle       DstHFix;
 DstHandle       DstHCon;
 DstHandle       DstHSet;
 DstHandle       DstHVar;
 DstHandle       DstHCct;
 DstHandle       DstHNam;
 DstHandle       DstHRpn;
 DstHandle       DstHTpt;
 DstHandle       DstHArg;
 DstHandle       DstHBuf;
 BlkHandle       BlkHBuf;
 RcdHandle       RcdHBuf;
 us_int          BufBlkSize;
 bool            PatternListMoved;
 char            filename[64];
 us_int          errcode;
 char            errline[80];
 PmvEnv          pmv;
}st_mam, *st_mam_p;



rc_t PmmTmpCreateNeu
(
 st_mam_p v_mam_p,
 ul_int     blksize,
 st_lstcb_p v_varcb_p
);


rc_t PmmBufCreate
(
 st_mam_p v_mam_p
);

rc_t PmmBufAdd                     /* add record to end of buffer  */
(
 st_mam_p     v_mam_p,
 st_dsc_p     r_rcd_dp             /* pointer to record descriptor        */
);

rc_t PmmBufLastNo                  /* get last block number  */
(
 st_mam_p     v_mam_p,
 us_int_p     blk_no
);

rc_t PmmBufInfo                   /* get last block number and table pointer */
(
 st_mam_p     v_mam_p,
 us_int_p     blk_no,
 ub_int_p    *blktab_p
);

rc_t PmmFcnDesc
(
 st_lstcb_p     v_lcb_p
);

bool PmmFcnFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_fcnlst_p    *w_fcn_p
);

bool PmmFcnIdTy
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id,
 us_int         r_ty,
 st_fcnlst_p    *w_fcn_p
);

rc_t PmmFcnLst
(
 st_lstcb_p v_lcb_p
);

rc_t PmmPatDesc
(
 st_lstcb_p     v_lcb_p
);

rc_t PmmPatAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp
);

rc_t PmmPatExeAdd
(
 st_lstcb_p     v_lcb_p,
 st_patlst_p    v_pat_p,
 st_exelst_p    v_exe_p
);

rc_t PmmPatLstAdd
(
 st_lstcb_p     v_lcb_p,
 st_patlst_p    v_pat_p,
 st_dsc_p       r_lst_dp
);

bool PmmPatFind
(
 st_lstcb_p     v_lcb_p,
 st_lstcb_p     v_ncb_p,
 st_sdc_p       r_nm_dp,
 st_patlst_p    *w_pat_p
);

rc_t PmmPatForAdd
(
 st_lstcb_p     v_lcb_p,
 st_lstcb_p     v_ncb_p,
 st_sdc_p       r_nm_dp,
 st_patlst_p    *w_pat_p
);

rc_t PmmPatDelete
(
 st_lstcb_p     v_lcb_p,
 st_patlst_p    v_pat_p
);

rc_t PmmTptAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp
);

rc_t PmmTptExeAdd
(
 st_lstcb_p     v_lcb_p,
 st_tptlst_p    v_pat_p,
 st_exelst_p    v_exe_p
);

rc_t PmmTptLstAdd
(
 st_lstcb_p     v_lcb_p,
 st_tptlst_p    v_pat_p,
 st_dsc_p       r_lst_dp
);

bool PmmTptFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_tptlst_p    *w_pat_p
);

rc_t PmmTptForAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_tptlst_p    *w_pat_p
);

rc_t PmmTptDelete
(
 st_lstcb_p     v_lcb_p,
 st_tptlst_p    v_pat_p
);

rc_t PmmTptTest
(
 st_lstcb_p  v_lcb_p
);

rc_t PmmConAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp
);

rc_t PmmConAppend
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp,
 us_int         con_id
);

bool PmmConFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp,
 st_conlst_p    *w_con_p
);

rc_t PmmConDesc
(
 st_lstcb_p     v_lcb_p
);

rc_t PmmConForAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_val_dp,
 st_conlst_p    *w_con_p
);

rc_t PmmVarDesc
(
 st_lstcb_p     v_lcb_p
);

rc_t PmmVarAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp
);

rc_t PmmVar2Dst
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    r_var_p
);

rc_t PmrVarAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp
);

rc_t PmmVarReplace
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    v_var_p,
 st_dsc_p       r_val_dp
);

rc_t PmmVarIncr
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    v_var_p
);

rc_t PmmVarDecr
(
 st_lstcb_p     v_lcb_p,
 st_varlst_p    v_var_p
);

bool PmmVarFind
(
 st_lstcb_p     v_lcb_p,
 st_lstcb_p     v_ncb_p,
 st_sdc_p       r_nm_dp,
 st_varlst_p    *w_var_p
);

rc_t PmmVarForAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp,
 st_varlst_p    *w_var_p
);

rc_t PmmVarAddorRep
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp,
 st_varlst_p    *w_var_p
);

rc_t PmmCctDesc
(
 st_lstcb_p     v_lcb_p
);

bool PmmCctFind
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id,
 st_cctelm_p    *w_elm_p,
 st_cctlst_p    *w_cct_p
);

bool PmmCctSearch
(
 st_lstcb_p     v_lcb_p,
 us_int         r_ct,
 st_cctelm_p    r_ell_p,
 st_cctelm_p    r_elm_p,
 st_cctlst_p    *w_cct_p
);

rc_t PmmCctAdd
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id,
 us_int         r_voc_id,
 us_int         r_ty
);

bool PmmCctlstAdd
(
 st_lstcb_p     v_lcb_p,
 us_int         r_ct,
 st_cctlst_p    r_cct_p,
 st_cctelm_p    r_elm_p
);

rc_t PmmSetDesc
(
 st_lstcb_p     v_lcb_p
);

bool PmmSetFill
(
 ub_int    setfunc,
 st_dsc_p  val_dp,
 st_dsc_p  set_dp
);

bool PmmSetSet
(
 st_mam_p       v_mam_p,
 us_int         r_vcc_id,
 us_int         r_ty,
 st_dsc_p       set_dp,
 st_conlst_p    con1_p,
 st_varlst_p    var1_p,
 st_cctlst_p    cct1_p
);

rc_t PmmSetAdd
(
 st_mam_p       v_mam_p,
 us_int         r_id,
 us_int         r_vcc_id,
 us_int         r_ty
);

bool PmmSetFind
(
 st_lstcb_p     v_lcb_p,
 us_int         r_vcc_id,
 us_int         r_arg_ty,
 st_setlst_p    *w_set_p
);

rc_t PmmSetForAdd
(
 st_mam_p       v_mam_p,
 us_int         r_vcc_id,
 us_int         r_arg_ty,
 st_setlst_p    *w_set_p
);

rc_t PmmDstCreate
(
 st_mam_p v_mam_p,
 ul_int   blksize
);

rc_t PmmDstVarCreate
(
 st_mam_p      v_mam_p,
 st_mlhandle_p mlh_p, 
 st_varlst_p   var_p
);

rc_t PmmDstVarErase
(
 st_mlhandle_p mlh_p
);

rc_t PmmDstVarDelete
(
 st_mlhandle_p mlh_p
);

rc_t PmmElmAdd
(
 st_lstcb_p     v_lcb_p,
 st_dsc_p       r_rcd_dp             /* pointer to record descriptor        */
);

rc_t PmmValAdd
(
 st_lstcb_p     v_lcb_p,
 st_dsc_p       r_rcd_dp             /* pointer to record descriptor        */
);

rc_t PmmBldLst
(
 st_lstcb_p     v_lcb_p
);

rc_t PmmRpnDesc
(
 st_lstcb_p     v_lcb_p
);

bool PmmRpnFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_rpnlst_p    *w_rpn_p
);

rc_t PmmRpnAdd
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 st_sdc_p       r_val_dp
);

bool PmmNamFind
(
 st_lstcb_p     v_lcb_p,
 st_sdc_p       r_nm_dp,
 ub_int_p      *nm_p,
 us_int_p       ty_p,
 us_int_p       id_p,
 us_int_p       fpv_id_p
);

rc_t PmmPdfAdd
(
 st_lstcb_p     v_lcb_p,         /* &pmv->newmam.rescb[PmrPdfCb] */
 st_sdc_p       r_nm_dp,
 ub_int         stm_ty,
 ul_int         stm_nm,
 ul_int         stm_op,
 ul_int         stm_beg,
 ul_int         stm_end
);
