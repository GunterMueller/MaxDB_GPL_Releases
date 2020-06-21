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

#define  mxnmsize  15              /* max. name length                      */
#define  mxpnmsize 63              /* max. pattern name length              */

#define  argtyres  0               /* argument is a result                  */
#define  argtycon  1               /* argument is a constant                */
#define  argtyvar  2               /* argument is a variable                */
#define  argtycct  3               /* argument is a concatenation           */
#define  argtyint  4               /* argument is an immediate integer      */
#define  argtyset  5               /* argument is a set                     */
#define  argtypat  6               /* argument is a pattern                 */
#define  argtyptv  7               /* argument is a pattern variable        */
#define  argtylng  8               /* argument is an immediate long         */

#define  asstyimd  1               /* immediate assignment  ($)             */
#define  asstydef  2               /* deferred assignment   (.)             */
#define  asstyvar  3               /* ???????? assignment   (>)             */
#define  asstyrpl  4               /* replacement           (<)             */

#define  fcntypap   1              /* push and pop                          */
#define  fcntypmp   2              /* pattern match primitive               */
#define  fcntypmi   3              /* pattern match with integer            */
#define  fcntypmc   4              /* pattern match sets cursor only        */
#define  fcntystr   5              /* pattern match string function         */
#define  fcntyass   6              /* assigment                             */
#define  fcntyrpl   7              /* replacement                           */
#define  fcntycll   8              /* pattern call                          */
#define  fcntysub   9              /* goto pattern                          */
#define  fcntyapx  10              /* build approximation lists             */
#define  fcntyrst  11              /* breakx                                */
#define  fcntyarb  12              /* arbno                                 */
#define  fcntyend  13              /* end arbno                             */
#define  fcntytst  14              /* test arbno                            */
#define  fcntyaso  15              /* build association lists               */
#define  fcntynot  16              /* negation                              */
#define  fcntyfstr 20              /* find string                           */
#define  fcntysset 21              /* find string set                       */
#define  fcntycset 22              /* find character set                    */
#define  fcntynset 23              /* find not character set                */
#define  fcntycalc 30              /* compute                               */
#define  fcntycall 40              /* call                                  */


#define fcnidpshprec     1         /* push and pop                          */
#define fcnidpopprec     2         /*                                       */
#define fcnidpopass      3         /*                                       */
#define fcnidprecurs     4         /*                                       */
#define fcnidpopimmed    5         /*                                       */
#define fcnidpshstrprec  6         /* push and pop                          */
#define fcnidpopstrprec  7         /*                                       */
#define fcnidpopstr      8         /*                                       */
              
#define fcnidpshass      1         /* push assignment                       */
#define fcnidexeass      2         /* execute assignment                    */
#define fcnidpshstr      3         /* execute assignment                    */
              
#define fcnidpcall       1         /* pattern call                          */

#define fcnidgosub       1         /* goto pattern                          */
              
#define fcnidcmpr        1         /* string function                       */
#define fcnidmatch       2         /* match function                        */
#define fcnidlike        3         /* like function                         */
#define fcnidlook        4         /* look function                         */
#define fcnidbreakmatch  5         /* break match function                  */
#define fcnidbreaklook   6         /* break look function                   */
#define fcnidexact       7         /* exact function                        */
#define fcnidtrailing    8         /* test trailing string                  */
#define fcnidparstr      9         /* string parameter                      */

                                   /* Anchored  Break     CaseSensitive     */
#define fcnid_nil        0         /*                                       */
#define fcnid_a          1         /* A                                     */
#define fcnid_b          2         /*           B                           */
#define fcnid_ab         3         /* A         B                           */
#define fcnid_c          4         /*                     C                 */
#define fcnid_ac         5         /* A                   C                 */
#define fcnid_bc         6         /*           B         C                 */
#define fcnid_abc        7         /* A         B         C                 */
#define fcnid_all        9         /* general function id for function list */
              
#define fcnidpos         1         /* test cursor position                  */
#define fcnidrpos        2         /*                                       */
#define fcnidlen         3         /* next n characters                     */
#define fcnidtab         4         /* n characters from the beginning       */
#define fcnidrtab        5         /* n characters from the end             */
#define fcnidfault       6         /* no. of permitted faults               */
#define fcnidchr         7         /* immediate character comparison        */
#define fcnidposgt      11         /* test cursor position                  */
#define fcnidposge      12         /* test cursor position                  */
#define fcnidposlt      13         /* test cursor position                  */
#define fcnidposle      14         /* test cursor position                  */
#define fcnidparnum     15         /* number parameter                      */
#define fcnidposne      19         /* test cursor position                  */
#define fcnidrposgt     21         /* test cursor position                  */
#define fcnidrposge     22         /* test cursor position                  */
#define fcnidrposlt     23         /* test cursor position                  */
#define fcnidrposle     24         /* test cursor position                  */
#define fcnidrposne     29         /* test cursor position                  */

              
#define fcnidfbal        1         /* set cursor position                   */
#define fcnidrem         2         /*                                       */
#define fcnidtrue        3         /*                                       */
#define fcnidfail        4         /*                                       */
#define fcnidcut         5         /* clear alternation stack               */
#define fcnidsuccess     6         /*                                       */
#define fcnidabort       7         /*                                       */

#define fcnidany         1         /* pattern match                         */
#define fcnidbackspan    2         /*                                       */
#define fcnidbackbreak   3         /*                                       */
#define fcnidbreak       4         /*                                       */
#define fcnidnotany      5         /*                                       */
#define fcnidspan        6         /*                                       */
#define fcnidbrake       7         /*                                       */
#define fcnidnotbrake    8         /*                                       */
#define fcnidbreakx      9         /*                                       */
              
#define fcnidcc          1         /* not used so far                       */
#define fcnidbrkx        4         /*                                       */
#define fcnidxx          6         /*                                       */

#define fcnidincr        1         /*                                       */
#define fcniddecr        2         /*                                       */

#define  lstnrmrdi       1

#define  lstfcnrdi       1 
#define  lstpatrdi       2 

#define  fcnidscmpr      1         /*                                       */

#define  setfillini      1         /*                                       */
#define  setfilladd      2         /*                                       */

#define  vartynormal     0
#define  vartymline      1
#define  vartystrset     2
#define  vartystrcct     3

#define  namtyfcn        1
#define  namtypat        2
#define  namtyvar        3

#define  lsttysub        0
#define  lsttynam        1

typedef struct
{
 ub_int  ty;                       /* function type                         */
 ub_int  id;                       /* function identifier within type       */
 us_int  res;                      /* reserved, filler to get 4 alignment   */
 ub_int  nm_lg;                    /* actual name length                    */
 char    nm [mxnmsize];            /* internal function name                */
} st_fcnlst, *st_fcnlst_p;

typedef struct
{
 us_int  id;                       /* unique pattern identifier             */
 us_int  nm_id;                    /* unique pattern name identifier        */
 us_int  rdi;                      /* record identifier of exe list         */
 us_int  ct;                       /* no. of execution elements in pattern  */
 char  ty[2];                      /* type during table generation          */
 st_dsc  val_d;                    /* last desriptor                        */
} st_patlst, *st_patlst_p;

typedef struct                     /* old pattern list format               */
{
 us_int  id;                       /* unique pattern identifier             */
 ub_int  nm_lg;                    /* actual name length                    */
 char    nm [mxpnmsize];           /* pattern name  (short pattern name)    */
 us_int  rdi;                      /* record identifier of exe list         */
 us_int  ct;                       /* no. of execution elements in pattern  */
 char  ty[2];                      /* type during table generation          */
 st_dsc  val_d;                    /* last desriptor                        */
} st_patold, *st_patold_p;

typedef struct                     /* old pattern list format               */
{
 us_int  id;                       /* unique pattern identifier             */
 ub_int  nm_lg;                    /* actual name length                    */
 char    nm [mxpnmsize];           /* pattern name  (short pattern name)    */
 us_int  rdi;                      /* record identifier of exe list         */
 us_int  ct;                       /* no. of execution elements in pattern  */
 char  ty[2];                      /* type during table generation          */
 st_dsc  val_d;                    /* last desriptor                        */
} st_tptlst, *st_tptlst_p;

typedef struct
{
 st_dsc  val_d;                    /* last desriptor                        */
} st_fltlst, *st_fltlst_p;

typedef struct
{
 us_int  succ;                     /* succeeding node                       */
 us_int  alt;                      /* alternative node                      */
 ub_int  fcn_ty;                   /* function type                         */
 ub_int  fcn_id;                   /* function identifier within type       */
 ub_int  ass_ty;                   /* assignment type                       */
 ub_int  arg_ty;                   /* argument type                         */
 us_int  vcc_id;                   /* argument ID var,const or concat       */
 us_int  ctmod;                    /* modification count, signals change    */
 us_int  curs;                     /* cursor in original statement          */
} st_exelst, *st_exelst_p;

typedef struct
{
 ul_int  succ;                     /* succeeding node                       */
 ul_int  alt;                      /* alternative node                      */
 us_int  pat_id;                   /* pattern ID                            */
 us_int  exe_ix;                   /* pattern node in exelst                */
} st_fxelst, *st_fxelst_p;

typedef struct
{
 ul_int  succ;                     /* succeeding node                       */
 ul_int  alt;                      /* alternative node                      */
 ul_int  pos_ix;                   /* conversion index during fxelst build  */
} st_fxilst, *st_fxilst_p;

typedef struct
{
 st_exelst elm;
 us_int  curs;                     /* cursor in original statement          */
} st_exeneu, *st_exeneu_p;

typedef struct                     /* caution: st_exego == st_exelst !!     */
{
 us_int  succ;                     /* succeeding node                       */
 us_int  alt;                      /* alternative node                      */
 ub_int  fcn_ty;                   /* function type                         */
 ub_int  fcn_id;                   /* function identifier within type       */
 ub_int  sub_id;                   /* GOSUB-ID in pattern                   */
 ub_int  arg_ty;                   /* argument type                         */
 us_int  vcc_id;                   /* argument ID var,const or concat       */
 us_int  sub_ix;                   /* index to sub stack                    */
 us_int  curs;                     /* cursor in original statement          */
} st_exego, *st_exego_p;

typedef struct
{
 us_int  id;                       /* unique variable identifier            */
 ub_int  nm_lg;                    /* actual name length                    */
 char    nm [mxnmsize];            /* variable name                         */
 us_int  ctmod;                    /* mod. count, incremented on any change */
 us_int  rdi;                      /* record identifier of value            */
 ub_int  nocon;                    /* variable is not constant              */
 ub_int  var_ty;                   /* variable has multiple lines           */
 st_dsc  val_d;                    /* last desriptor                        */
} st_varold, *st_varold_p;

typedef struct
{
 us_int  id;                       /* unique variable identifier            */
 us_int  nm_id;                    /* ID in name list                       */
 us_int  ctmod;                    /* mod. count, incremented on any change */
 us_int  rdi;                      /* record identifier of value            */
 ub_int  nocon;                    /* variable is not constant              */
 ub_int  var_ty;                   /* variable has multiple lines           */
 st_dsc  val_d;                    /* last desriptor                        */
} st_varlst, *st_varlst_p;

typedef struct
{
 us_int  id;                       /* unique variable identifier            */
 us_int  rdi;                      /* record identifier     of value        */
 st_dsc  val_d;                    /* last desriptor                        */
} st_conlst, *st_conlst_p;


typedef struct
{
 us_int  id;                       /* unique set identifier                 */
 us_int  vcc_id;                   /* variable, constant,  or conatination  */
 us_int  arg_ty;                   /* argument type                         */
 us_int  rdi;                      /* record identifier of set              */
 us_int  ctmod;                    /* mod. count, incremented on any change */
 us_int  nocon;                    /* set is not constant                   */
 st_dsc  set_d;                    /* set desriptor                         */
} st_setlst, *st_setlst_p;

typedef struct
{
 us_int  id;                       /* unique concatenation identification   */
 us_int  ct;                       /* no. of elements in list               */
 us_int  rdi;                      /* record identifier of cc               */
 us_int  res;                      /* reserved to get alignment             */
 st_dsc  cct_d;                    /* cc desriptor                          */
} st_cctlst, *st_cctlst_p;

typedef struct
{
 us_int  vcc_id;                   /* variable or constant                  */
 us_int  arg_ty;                   /* argument type                         */
 us_int  ctmod;                    /* mod. count, incremented on any change */
 us_int  res;                      /* reserved to get alignment             */
} st_cctelm, *st_cctelm_p;

typedef struct
{
 us_int  id;                       /* unique name identifier                */
 us_int  fpv_id;                   /* pattern or variable ID                */
 us_int  nam_ty;                   /* function, pattern or variable  name   */
 us_int  lst_ty;                   /* type of list nam (1) or sub (0)       */
 st_dsc  nam_d;                    /* name desriptor                        */
} st_namlst, *st_namlst_p;


typedef struct
{
 us_int  id;                       /* unique name identifier                */
 us_int  fpv_id;                   /* pattern or variable ID                */
 us_int  nam_id;                   /* pattern or variable ID                */
 us_int  nam_ty;                   /* function, pattern or variable  name   */
 ul_int  stm_nm;                   /* position of pattern or variable name  */
 ul_int  stm_op;                   /* position of operator                  */
 ul_int  stm_beg;                  /* begin of statement body               */
 ul_int  stm_end;                  /* end of statement body                 */
 ub_int  stm_ty;                   /* statement type (; < : = > ?)          */
 ub_int  dmy;                      /* alignment                             */
 us_int  res;                      /* reserved                              */
} st_pdflst, *st_pdflst_p;


typedef struct
{
 us_int  id;                       /* unique RPN identifier                 */
 ub_int  nm_lg;                    /* actual name length                    */
 char    nm [mxpnmsize];           /* pattern name                          */
 us_int  rdi;                      /* record identifier of value            */
 st_dsc  val_d;                    /* last desriptor                        */
} st_rpnlst, *st_rpnlst_p;
/*
*/

#define      CTFCN    55

typedef struct
{
 ub_int  ty;                       /* function type                         */
 ub_int  id;                       /* function identifier within type       */
 char    nm [mxnmsize + 1];        /* internal function name                */
}st_lstfcnnm;

static st_lstfcnnm  lstfcnnm [CTFCN] =
{
 fcntypap, fcnidpshprec  , "PSHP"  ,      /* "PMF_PSH_PREC"  */
 fcntypap, fcnidpopprec  , "POPP" ,       /* "PMF_POP_PREC"  */
 fcntypap, fcnidpopass   , "POPA" ,       /* "PMF_POP_ASS"   */
 fcntypap, fcnidprecurs  , "PREC" ,       /* "PMF_PRE_CURS"  */

 fcntyass, fcnidpshass   , "PSHA"  ,      /* "PMF_PSH_ASS"   */

 fcntycll, fcnidpcall    , "PCALL",       /* "PMF_CALL_PAT"  */

 fcntysub, fcnidgosub    , "GOSUB",       /* "PMF_GOTO_PAT"  */

 fcntystr, fcnidcmpr     , "CMPR" ,       /* "STF_CMPR"      */
 fcntystr, fcnidexact    , "EXACT" ,      /* "STF_EXACT"     */
 fcntystr, fcnidmatch    , "MATCH" ,      /* "STF_CMPR"      */
 fcntystr, fcnidbreakmatch,"BRATCH" ,     /* "STF_CMPR"      */
 fcntystr, fcnidcc       , "CC"   ,       /* "PMF_CONCAT"    */
 fcntystr, fcnidlike     , "LIKE" ,       /* "PmpLike"       */
 fcntystr, fcnidlook     , "LOOK" ,       /* "PmpLook"       */
 fcntystr, fcnidbreaklook, "BROOK" ,      /* "PmpLook"       */
 fcntystr, fcnidtrailing , "TRAILING" ,   /* "PmpTrailing"   */
 fcntystr, fcnidparstr   , "PARSTR" ,     /*                 */

 fcntypmi, fcnidpos      , "POS"  ,       /* "STF_POS"       */
 fcntypmi, fcnidposgt    , "POSGT",       /* "STF_POS"       */
 fcntypmi, fcnidposge    , "POSGE"  ,     /* "STF_POS"       */
 fcntypmi, fcnidposlt    , "POSLT"  ,     /* "STF_POS"       */
 fcntypmi, fcnidposle    , "POSLE"  ,     /* "STF_POS"       */
 fcntypmi, fcnidposne    , "POSNE"  ,     /* "STF_POS"       */
 fcntypmi, fcnidrpos     , "RPOS" ,       /* "STF_RPOS"      */
 fcntypmi, fcnidrposgt   , "RPOSGT",      /* "STF_POS"       */
 fcntypmi, fcnidrposge   , "RPOSGE"  ,    /* "STF_POS"       */
 fcntypmi, fcnidrposlt   , "RPOSLT"  ,    /* "STF_POS"       */
 fcntypmi, fcnidrposle   , "RPOSLE"  ,    /* "STF_POS"       */
 fcntypmi, fcnidrposne   , "RPOSNE"  ,    /* "STF_POS"       */
 fcntypmi, fcnidlen      , "LEN"  ,       /* "STF_LEN"       */
 fcntypmi, fcnidtab      , "TAB"  ,       /* "STF_TAB"       */
 fcntypmi, fcnidrtab     , "RTAB" ,       /* "STF_RTAB"      */
 fcntypmi, fcnidfault    , "FAULT" ,      /* "STF_FAULT"     */
 fcntypmi, fcnidchr      , "CHR" ,        /* "STF_CHR"       */
 fcntypmi, fcnidparnum   , "PARNUM" ,     /*                 */

 fcntypmc, fcnidfbal     , "FBAL" ,       /* "STF_FBAL"      */
 fcntypmc, fcnidrem      , "REM"  ,       /* "STF_REM"       */
 fcntypmc, fcnidtrue     , "TRUE" ,       /* "PMF_SAY_OK"    */
 fcntypmc, fcnidfail     , "FAIL" ,       /* "PMF_SAY_NO"    */
 fcntypmc, fcnidcut      , "CUT" ,        /* "PMF_RESET_ALL" */
 fcntypmc, fcnidsuccess  , "SUCCESS" ,    /*  terminate now  */
 fcntypmc, fcnidabort    , "ABORT" ,      /*  terminate now  */

 fcntypmp, fcnidany      , "ANY"  ,       /* "STF_ANY"       */
 fcntypmp, fcnidbackspan , "BACKSPAN" ,   /* "STF_BACKSPAN"  */
 fcntypmp, fcnidbackbreak, "BACKBREAK",   /* "STF_BACKBREAK" */
 fcntypmp, fcnidbreak    , "BREAK",       /* "STF_BREAK"     */
 fcntypmp, fcnidbreakx   , "BREAKX",      /* "STF_BREAK"     */
 fcntypmp, fcnidnotany   , "NOTANY"  ,    /* "STF_NOTANY"    */
 fcntypmp, fcnidspan     , "SPAN" ,       /* "STF_SPAN"      */
 fcntypmp, fcnidbrake    , "SCAN" ,       /* "STF_SCAN"      */
 fcntypmp, fcnidbrake    , "BRAKE" ,      /* "STF_SCAN"      */
 fcntypmp, fcnidnotbrake , "NOTBRAKE" ,   /* "STF_SCAN"      */

 fcntycalc, fcnidincr ,    "INCR" ,       
 fcntycalc, fcniddecr ,    "DECR" ,  

 fcntyfstr,fcnid_all     , "FIND"         /* General FIND    */

};     

