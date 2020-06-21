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
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmdesc.h" 
#include "idl/idl2xml/pmstck.h"
#include "idl/idl2xml/pmsmsg.h"

/* static BITFLD MATCH,                         Matching Maske */
/* 		             P,     1-bit fuer fehlerfreie Musterteile */
/* 	              STAR,     0-bit hinter *-Wildcards im Muster */
/*                   N,   0-bit, wenn erstes Zeichen im Muster */
/*      T[UCHAR_MAX+1],        charakt. Vektoren aller Zeichen */
/*      TT[UCHAR_MAX+1];     ... modifiziert fuer Multipattern */

/* static int M;                                  Musterlaenge */
/* static int Withstars;             flag: Muster enthaelt '*' */
/* static int Laststar;    flag: letztes Musterzeichen ist '*' */
/* static int Leftstar;    Position-1 des ersten '*' im Muster */
/* static int Ignore;       flag: Gross- gleich Kleinbuchstabe */



#define VecClr(B,n)     ( (B) &= ~(1L<<(n)) )
#define VecSet(B,n)     ( (B) |= (1L<<(n)) )
#define VecIsClr(B,n)   ( ((B) & (1L<<(n)))==0 )
#define VecIsSet(B,n)   ( ((B) & (1L<<(n))) )

#define VecRShift(B,n)  ( (B) >> n )
#define VecLShift(B,n)  ( (B) << n )
#define VecAll()        ( (t_bitvec)~0L )
#define VecNull()       ( 0L )
#define VecOr(B1,B2)    ( (B1) | (B2) )
#define VecAnd(B1,B2)   ( (B1) & (B2) )


bool PmApproxPrep
(
 st_pmv_p    pmv,
 st_exelst_p r_exe_p
)
{
  bool           modflg;
  bool           brc;
  bool           sensitive;
  bool           anchored;
  bool           negative;
  bool           exact;
  us_int         ct; 
  us_int         skip_ct; 
  us_int         ix;
  us_int         ix2;
  ub_int         cix;
  st_apxcb_p     apx_p;
  us_int         arg_ty;
  us_int         vcc_id;
  ub_int         fcn_id;
  st_exelst_p    exe_p;
  st_setlst_p    set_p;
  st_setlst_p    set1_p;
  st_cctlst_p    cct_p;
  st_cctlst_p    cct1_p;
  st_conlst_p    con_p;
  st_conlst_p    con1_p;
  st_varlst_p    var_p;
  st_varlst_p    var1_p;
  st_cctelm_p    elm_p;
  st_mam_p       mam_p;
  st_sdc_p       set_dp;
  st_dsc         val_d; 
  us_int         index;
  ub_int_p       pt;
  
  mam_p   = &pmv->mam;
  con1_p  = (st_conlst_p) mam_p->concb.lst_d.pt;
  var1_p  = (st_varlst_p) mam_p->varcb.lst_d.pt;
  cct1_p  = (st_cctlst_p) mam_p->cctcb.lst_d.pt;
  set1_p  = (st_setlst_p) mam_p->setcb.lst_d.pt;

  apx_p = &pmv->apx;

  for (ix = 0; ix < MAX_CHAR; ix++)
    {
     apx_p->t[ix] = VecAll();
    }

  apx_p->match    = VecAll();
  apx_p->skip     = VecAll();
  apx_p->exact    = VecNull();
  apx_p->patlen   = 0;
  apx_p->anchlen  = 0;
  apx_p->withskip = FALSE;
  apx_p->anchored = FALSE;
  apx_p->fault    = 0;
  apx_p->first    = VecAll();
  apx_p->anchor   = VecAll();
  VecClr (apx_p->first, 0);
  skip_ct         = 0;
  index    = 0;
  
  ix = r_exe_p->alt - 1;

  exe_p = r_exe_p; 

  while ((exe_p + 1)->succ != 0)
    {
     exe_p++;
     switch (exe_p->fcn_ty)
       {
        case fcntypmp:
        case fcntycset:
        case fcntynset:
          {
           if (apx_p->patlen > BITVEC_MAX)
             {
              /* Pattern too long */
              return FALSE;
             }
           
           set_p  = set1_p + (exe_p->vcc_id - 1);
           set_dp = (st_sdc_p) &set_p->set_d;
           arg_ty = set_p->arg_ty;
           vcc_id = set_p->vcc_id;        

           modflg = FALSE;
             
           if (argtyvar == arg_ty)
             { 
              var_p  = var1_p + (vcc_id - 1);
              modflg = set_p->ctmod == var_p->ctmod;
              set_p->ctmod = var_p->ctmod;
             }

           if (argtycct == arg_ty)
             { 
              cct_p = cct1_p + (vcc_id - 1);
              ct    = cct_p->ct;
              elm_p = (st_cctelm_p) cct_p->cct_d.pt;
              for (ix = 0; ix < ct; ix++, elm_p++)
                {
                 if (argtyvar == elm_p->arg_ty)
                   {
                    var_p = var1_p + (elm_p->vcc_id - 1);
                    if (!modflg) modflg = elm_p->ctmod != var_p->ctmod;
                    elm_p->ctmod = var_p->ctmod;
                   }
                }
             }


           anchored  = FALSE;
           sensitive = FALSE;
           negative  = FALSE;

           switch (exe_p->fcn_ty)
             {
              case fcntypmp:
                {
                 switch (exe_p->fcn_id)
                   {
                    case fcnidbrake:
                      sensitive = TRUE;
                      break;
                    case fcnidany:
                      anchored  = TRUE;
                      sensitive = TRUE;
                      break;
                    case fcnidnotbrake:
                      sensitive = TRUE;
                      negative  = TRUE;
                      break;
                    case fcnidnotany:
                      anchored  = TRUE;
                      sensitive = TRUE;
                      negative  = TRUE;
                      break;
                   }
                 break;
                }
              case fcntynset:
                negative  = TRUE;
              case fcntycset:
                {
                 switch (exe_p->fcn_id)
                   {
                    case fcnid_a:
                      anchored  = TRUE;
                      break;
                    case fcnid_c:
                      sensitive = TRUE;
                      break;
                    case fcnid_ac:
                      anchored  = TRUE;
                      sensitive = TRUE;
                      break;
                   }
                 break;
                }
            }
           
           pt = set_dp->pt;

           if (modflg)
             {
              brc = PmmSetSet (mam_p, vcc_id, arg_ty, (st_dsc_p) set_dp,
                               con1_p, var1_p, cct1_p);
             }
           
           if (!anchored)
             {
              if (index > 0) 
                {
                 apx_p->withskip = TRUE;
                 VecClr(apx_p->skip, index - 1);   /*JH*/
                }
              if (apx_p->anchored)
   		        {
				 if (0 == apx_p->anchlen)
				   apx_p->anchlen = index;
				}  
			  else
			    {
			     skip_ct++;
			    }  
             }
           
              
          for (ix = 0; ix < MAX_CHAR; ix++)
             {
              if (negative)
                {
                 if (!*(pt + ix))
                   {
                    if (sensitive)
                      {
                       VecClr( apx_p->t[ix], index);
                      }
                    else
                      {
                       if ((!*(pt + ix)) && (!*(pt + pmv->ultab[ix])))
                         {
                          VecClr( apx_p->t[ix], index);
                         }
                      }
/*
                    if ((!sensitive) && (pmv->ultab[ix] != 0))
                      {
                       VecClr( apx_p->t[pmv->ultab[ix]], index);
                      }
*/
                   }
                }
              else
                {
                 if (*(pt + ix))
                   {
                    VecClr( apx_p->t[ix], index);
                    if ((!sensitive) && (pmv->ultab[ix] != 0))
                      {
                       VecClr( apx_p->t[pmv->ultab[ix]], index);
                      }
                   }
                }
             }

           if ((!apx_p->anchored) && (0 == skip_ct))
              apx_p->anchored = TRUE;
           index++;
           apx_p->patlen++;
           break;
          }
        case fcntypmi:
          {
           switch (exe_p->fcn_id)
             {
              case fcnidfault:
                {
                 apx_p->fault = exe_p->vcc_id;
                 break;
                }
              case fcnidlen:
                {
                 if ((exe_p->vcc_id + apx_p->patlen) > (BITVEC_MAX + 1)) 
                   {
                    /* Pattern too long */
                    return FALSE; 
                   }
                 for (ix = 0;ix < exe_p->vcc_id; ix++)
                   {
                    for (ix2 = 0; ix2 < MAX_CHAR; ix2++)
                      {
                       VecClr( apx_p->t[ix2], index);
                      } 
                    index++;
                    apx_p->patlen++;
                   }
                 break;
                }
              default:
                   /* Function not allowed in APPROX */
              return FALSE;
             }
           break;
          }
        case fcntystr:
        case fcntyfstr:
          {
           arg_ty = exe_p->arg_ty;
           vcc_id = exe_p->vcc_id;        
           fcn_id = exe_p->fcn_id;
           elm_p  = 0;            
           
           switch (arg_ty)
             {
              case argtycon:
                {
                 con_p   = con1_p + (vcc_id - 1);
                 val_d   = con_p->val_d;
                 ct      = 1;
                 break;
                }
              case argtyvar:
                {
                 var_p   = var1_p + (vcc_id - 1);
                 val_d   = var_p->val_d;
                 ct      = 1;
                 break;
                }
              case argtycct:
                {
                 cct_p   = cct1_p + (vcc_id - 1);
                 elm_p   = (st_cctelm_p) cct_p->cct_d.pt;
                 ct      = cct_p->ct;
                 break;
                }
             }

           for (ix = 0; ix < ct; ix++, elm_p++)
             {
              if (arg_ty == argtycct)
                {
                 if (argtycon == elm_p->arg_ty)
                   {
                    con_p = con1_p + (elm_p->vcc_id - 1);
                    val_d = con_p->val_d;
                   }
                 else
                   {
                    var_p = var1_p + (elm_p->vcc_id - 1);
                    val_d = var_p->val_d;
                   }
                }      

              anchored  = FALSE;
              sensitive = FALSE;
              exact     = FALSE;
 
              if (exe_p->fcn_ty == fcntyfstr)
                {
                 switch (exe_p->fcn_id)
                   {
                    case fcnid_a:
                       anchored  = TRUE;
                       break;
                    case fcnid_c:
                       sensitive = TRUE;
                       break;
                    case fcnid_ac:
                       anchored  = TRUE;
                       sensitive = TRUE;
                       break;
                   }
                }
              else
                {
                 switch (exe_p->fcn_id)
                   {
                    case fcnidcmpr:
                       sensitive = TRUE;
                       anchored  = TRUE;
                       break;
                    case fcnidexact:
                       sensitive = TRUE;
                       anchored  = TRUE;
                       exact     = TRUE;
                       break;
                    case fcnidmatch:
                       sensitive = TRUE;
                       break;
                    case fcnidlike:
                       anchored  = TRUE;
                       break;
                    case fcnidlook:
                       break;
                   }
                }
              if (anchored)
                {
                 if ((!apx_p->anchored) && (0 == skip_ct))
                   {
                    apx_p->anchored = TRUE;
                   }
                }
              else
                {
                 if (index > 0) 
                   {
                    apx_p->withskip = TRUE;
                    VecClr(apx_p->skip, index - 1);    /*JH*/
                   }
                 if (apx_p->anchored)
		           {
				    if (0 == apx_p->anchlen)
				      apx_p->anchlen = index;
				   }  
				 else
				   {
			        skip_ct++;
				   }  
                 }
              if ((val_d.lg + apx_p->patlen) > (BITVEC_MAX + 1)) 
                {
                 /* Pattern too long */
                 return FALSE; 
                }
              
              for (ix2 = 0; ix2 < val_d.lg; ix2++)
                {
                 cix = *(val_d.pt + ix2);
                 VecClr(apx_p->t[cix], index);
                 if (!sensitive)
                   {
                    if (pmv->ultab[cix] != 0) 
                      VecClr(apx_p->t[pmv->ultab[cix]], index);
                   }
                 if (exact)
                    VecSet(apx_p->exact, index);
                 apx_p->patlen++;
                 index++;
                }
             }
           break;
          }
        default:
        /* Function type not allowed in APPROX */
        return FALSE;

       }
 
    } 
 
  apx_p->match = VecRShift(apx_p->first, 1);
  VecClr(apx_p->match, apx_p->patlen - 1); 
  if (apx_p->anchored)
    {
     if (!apx_p->withskip)
	   apx_p->anchlen = apx_p->patlen;
	 apx_p->anchor = VecAll();
     VecClr(apx_p->anchor, apx_p->anchlen - 1); 
    }
  r_exe_p = exe_p; 
 
  return TRUE;
 
}


bool PmApproxExec
(
 st_pmv_p   pmv
)
{ 
  
  
  t_bitvec   STATES[MISMATCH_MAX+2];
  t_bitvec   S, SS, SJ, SOR, TC, *SP;
/*t_bitvec   SR, SA, SD, ST; */
  ub_int     *t;
  char       *match = NULL;
  ul_int     ix;
  ul_int     lg;
  ub_int_p   pt;
  us_int     d;
  us_int     err;
  us_int     amatcherr;                  /* Zahl der Fehler des Matches */
  st_apxcb_p apx_p;
/*  bool       found;
    t_bitvec   anchmask; */

  apx_p = &pmv->apx;
  pt    = (pmv->b_p + pmv->buf_ix)->blk_p;
  err   = apx_p->fault;
  
  lg        = (us_int)(pmv->b_p + pmv->buf_ix)->acc_lg;
  pt        = (pmv->b_p + pmv->buf_ix)->blk_p;
  
  amatcherr = 0;
  SS        = VecAll();                            /* SS ist S<j><d-1> */
  SJ        = VecAll();    
  SP        = STATES;           /* Initialisiere die Zustandsvektoren */
  
  for ( d = 0; d <= apx_p->fault; d++)  
    SP[d] = SS;
 
  for (ix = pmv->cursor; ix < lg; ix++)
    {          /* fuer jedes Textzeichen */
     
     t  = pt + ix;
     
     TC = *(apx_p->t + *t);          /* charakt. Vektor fuer das Zeichen */
     
     for (d = 0; d <= err; ++d )    /* fuer jeden moegl. Fehler */
       { 
        S   = SP[d];                                    /* S ist S<j><d> */
        SOR = VecOr(VecLShift(S,1),TC);     
        if (d > 0) 
          {
/*		   SS = VecAnd( SP[d-1], VecLShift(VecAnd(SJ,SP[d-1]),1) ); */
		   SS = VecAnd( SJ,      VecLShift(VecAnd(SJ,SP[d-1]),1) );
    	   SS = VecAnd( VecOr(apx_p->exact,SS), SOR );
/*
		   SA = VecAnd (SOR, SJ);
		   SD = VecAnd (SOR, VecLShift (SP[d-1],1));
    	   SR = VecAnd (SOR, VecLShift (SJ, 1));
		   ST = VecAnd (SR, VecAnd(SA, SD));
		   if (SS != ST)
		     {
        	  amatcherr   = d;
		     }
*/
          }
        else 
          SS = SOR;
        
        if (apx_p->withskip) 
		  SS = VecAnd(SS,VecOr(S,apx_p->skip));
        
        
        if ( VecOr(SS,apx_p->match) != VecAll() ) 
          { /* Match mit d Fehlern */
    	   amatcherr   = d;
           pmv->cursor = ix + 1;
    	   return TRUE;
          }
        SP[d] = SS;
        SJ = S;
       }
    }
  return FALSE;
}
