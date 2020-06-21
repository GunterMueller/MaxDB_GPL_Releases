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

#define  pmn__nil   0
#define  pmn__root  1
  
/*  pmt_error  internal errors */
  
#define pme_err_invnode     0
#define pme_err_invsub      1
#define pme_err_invpat      2 
#define pme_err_mxnalt      3
#define pme_err_nomralt     4
#define pme_err_mxnincarn   5
#define pme_err_nomrincarn  6
#define pme_err_mxnass      7
#define pme_err_nomrass     8
#define pme_err_mxnprec     9
#define pme_err_nomrprec   10   
#define pme_err_notimp     11   
                 
#define pcc_mxnvar         50  /* max. variable defintions     */
#define pcc_mxnmlg         15  /* max. variable name length    */
#define pcc_first           1
#define pcc_next            2
#define pcc_min             3
#define pcc_max             4

#define deb_not_init        0
#define deb_init            1
#define deb_no_eomatch      2

#define deb_all_steps       0
#define deb_single_step     1

#define BITVEC_MAX         31
#define MAX_CHAR          256
#define MISMATCH_MAX        8

typedef unsigned long t_bitvec, *t_bitvec_p;



typedef struct   /* assignment definition in stack */
{
 us_int      incarn;
 char        asstyp;
 ub_int      arg_ty;
 ul_int      precursor;
 ul_int      postcursor;
 ul_int      precurs;
 ul_int      postcurs;
 us_int      prebuf_ix;
 us_int      postbuf_ix;
 us_int      subno;
 us_int      level; 
} pmt_assdef, *pmt_assdef_p;

typedef struct   /* iteration definition in stack */
{
 us_int      status; 
 us_int      for_ct;
 us_int      min_ct;
 us_int      max_ct;
 us_int      alt_ct;
} pmt_arbdef, *pmt_arbdef_p;


typedef struct   /* state in match for alternative / Return */
{
 us_int      incarn;
 us_int      mxincarn;
 us_int      sub_ix;
 us_int      callpat;
 ul_int      node;
 ul_int      precurs;
 us_int      pattern;
 us_int      prebuf_ix;
 us_int      ctarb;
 us_int      for_ct;
 us_int      res;
}pmt_matchpos, *pmt_matchpos_p;


typedef struct
{
  us_int         mxalt;        /* max. alternative stack count      */
  us_int         mxincarn;     /* max. incarnation count            */
  us_int         mxprec;       /* max. precursor count              */
  us_int         mxass;        /* max. assignment count             */
  us_int         mxarb;        /* max. iteration count              */
  us_int         ctalt;        /* alternative stack count           */
  us_int         ctincarn;     /* incarnation count                 */
  us_int         ctprec;       /* precursor count                   */
  us_int         ctprecstr;    /* precursor string count            */
  us_int         ctass;        /* assignment count                  */
  us_int         ctarb;        /* iteration count                   */
  pmt_matchpos_p alt;          /* pointer to alternative stack      */
  pmt_matchpos_p ret;          /* pointer to incarnation stack      */
  ul_int_p       prec;         /* pointer to precursor stack        */
  ul_int_p       precstr;      /* pointer to precursor string stack */
  us_int_p       preb;         /* pointer to prebuf_ix stack        */
  pmt_assdef_p   ass;          /* pointer to assignment stack       */
  pmt_arbdef_p   arb;          /* pointer to assignment stack       */
} st_pmvstack, *st_pmvstack_p;

typedef struct
{
 ul_int          precursor;
 ul_int          cursor;
 us_int          pat_id;
 us_int          pat_ix;
 us_int          nam_id;
 us_int          exe_curs;
 us_int          nxt_curs;
 us_int          brc;
} st_debinfo, *st_debinfo_p;

typedef struct
{
 st_debinfo      info;
 us_int          status;
 us_int          step_ty;
} st_debug, *st_debug_p;


typedef struct                 /* variable definition          */
{
 us_int         lg;
 us_int         ml;
 ub_int_p       pt; 
 char           nm [pcc_mxnmlg + 1];
} st_vdc, *st_vdc_p;

typedef struct                 /* global area variables        */
{
 us_int         ct;
 us_int         mc;
 st_vdc         va [pcc_mxnvar];
}st_vartab, *st_vartab_p;

typedef struct             /* block vector for buffer mode      */
{                          /* must conform with st_bktr !!!     */
 us_int         acc_ct;    /* accumulated block count           */
 us_int         ix;        /* block index in file               */
 ul_int         rcd_ct;    /* record count                      */
 ul_int         acc_lg;    /* accumulated length                */
 ub_int_p       blk_p;     /* block pointer                     */
}st_blktab, *st_blktab_p;

typedef struct
{
 us_int     patlen;      /* pattern length                       */
 us_int     anchlen;     /* length of anchored pattern           */
 bool       withskip;    /* pattern contains skips               */
 bool       anchored;    /* pattern runs in anchored mode        */
 us_int     firstskip;   /* postition of first skip              */
 us_int     fault;       /* no. of permitted faults              */
 t_bitvec   match;       /* matching mask                        */
 t_bitvec   anchor;      /* matching anchor mask                 */
 t_bitvec   exact;       /* 1-bit for no-fault string match      */     
 t_bitvec   skip;        /* 0-bit preceding MATCH or BRAKE       */
 t_bitvec   first;       /* 0-bit if first character in pattern  */
 t_bitvec   t[MAX_CHAR]; /* character specific test vectors      */
}
st_apxcb, *st_apxcb_p;

typedef struct
{
 st_pmvstack     stk;
 st_debug        deb;          /* debug status and orders         */
 st_vartab_p     vt_p;
 st_blktab_p     b_p;          /* pointer to block table          */
 ub_int_p        pt;           /* pointer to match object         */
 ul_int          lg;           /* length of match object          */
 ul_int          succ;         /* next node                       */
 ul_int          curnode;      /* current node                    */
 ul_int          maxsteps;
 ul_int          steps;
 ul_int          cursor;       /* cursor                          */
 ul_int          precurs;      /* Preccursor for assign           */
 ul_int          savcursor;    /* to save the cursor              */
 ul_int          postcursstr;  /* string cursor                   */
 ul_int          precursstr;   /* Preccursor for string assign    */
 us_int          prebuf_ix;    /* Precbuffer index for assign     */
 us_int          savbuf_ix;    /* to save the buffer index        */
 us_int          curcall;      /* current called pattern          */
 us_int          curpat;       /* current pattern                 */
 us_int          curincarn;    /* current incarnation             */
 bool            success;      /* intermediate result             */
 bool            eomatch;      /* end of match                    */
 bool            bufmatch;     /* buffer match                    */
 us_int          buf_ix;       /* current buffer index            */
 us_int          last_ix;      /* last buffer index               */
 ul_int          ml;           /* max. length of match object     */
 us_int          incarn;
 us_int          mxnsteps;
 stt_slong       tempstr;
 ub_int          rpnflg;        
 ub_int          errflg;        
 char            err[1];
 us_int          errcode;
 us_int          errline_lg;
 char            errline[pmc_mxerrline];
 st_mam          mam;
 st_mam          newmam;
 file_p          trcp;
 ub_int          ultab[256];
 st_apxcb        apx;
 st_conlst_p     con1_p;
 st_varlst_p     var1_p;
 st_cctlst_p     cct1_p;
 st_setlst_p     set1_p;
 st_patlst_p     pat1_p;
 st_fltlst_p     flt1_p;
 st_fxelst_p     fxe1_p;
 st_sdc          cctstr_d;
 st_mdc          callstr_d;
 st_mdc          strstack_d;
} st_pmv, *st_pmv_p;



                
