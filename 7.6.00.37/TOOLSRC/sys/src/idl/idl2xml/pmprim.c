/*--------------------------------------------------------------------------

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
/* Pattern Match Primitives for Buffer Match                                */
/*--------------------------------------------------------------------------*/ 

#include "idl/idl2xml/pen00.h"
#include "idl/idl2xml/mam.h"
#include "idl/idl2xml/rte.h" 
#include "idl/idl2xml/pms00.h" 
#include "idl/idl2xml/pmexec.h" 
#include "idl/idl2xml/pmmadm.h" 
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmdesc.h" 
#include "idl/idl2xml/pmstck.h"
#include "idl/idl2xml/pmprim.h" 
#include "idl/idl2xml/pmsmsg.h"
#include "idl/idl2xml/pmpset.h"

#define M__NAME "PMSPP"

/*--------------------------------------------------------------------------*/ 
/* conversion in place of all lower case letters 'a' to 'z' to upper case   */
/*--------------------------------------------------------------------------*/ 

void PmpUpper 
( 
 st_sdc_p str_dp
)
{
 us_int    i;
 ub_int_p  p;
 
 p = str_dp->pt;
 
 for (i = 0; i < str_dp->lg; i++, p++)
    *p = PmtUpper[*p];
}

/*--------------------------------------------------------------------------*/ 
/* conversion in place of all upper case letters 'A' to 'Z' to lower case   */
/*--------------------------------------------------------------------------*/ 

void PmpLower 
( 
 st_sdc_p str_dp
)
{
 us_int    i;
 ub_int_p  p;
 
 p = str_dp->pt;
 
 for (i = 0; i < str_dp->lg; i++, p++)
    *p = PmtLower[*p];
}
                 
/*--------------------------------------------------------------------------*/ 
/* comparison fo two strings including length                               */
/*--------------------------------------------------------------------------*/
                 
bool PmpLexeq 
( 
 st_sdc_p str1_dp,
 st_sdc_p str2_dp
)
{
 ub_int_p     s1p;
 ub_int_p     s2p;
 us_int       lg;
 us_int       i;
 
 lg = str1_dp->lg;
 
 if (lg == str2_dp->lg)
   {
    s1p = str1_dp->pt;
    s2p = str2_dp->pt;
    
    for (i = 0; i < lg; i++, s1p++, s2p++)
      {
       if (*s1p != *s2p) return FALSE;
      }
    return TRUE;
   }

  return FALSE;
}

/*--------------------------------------------------------------------------*/ 
/* initialize a character set from a descriptor string                      */
/*--------------------------------------------------------------------------*/ 

bool PmpIniSet
(
 st_sdc_p  str_dp,
 st_sdc_p  set_dp
)
{
  ub_int_p   tp;
  ub_int_p   sp;
  ub_int     i;
  us_int     lg;
 
  tp = set_dp->pt; 
  lg = set_dp->lg; 

  memset (tp, 0, lg);
  
  sp = str_dp->pt; 
  
          
  for (i = 0; i < str_dp->lg; i++, sp++)
    {
     if (lg < *sp) return FALSE;
     
     *(tp + *sp) = TRUE;         
    }
  
  return TRUE;
}                                     

/*--------------------------------------------------------------------------*/ 
/* initialize a character set from a string                                 */
/*--------------------------------------------------------------------------*/ 

bool PmpIniStr
(
 st_sdc_p  set_dp,
 char_p    str
)
{
  ub_int_p   tp;
  ub_int_p   sp;
  ub_int     i;
  us_int     lg;
  us_int     str_lg;
 
  tp         = set_dp->pt; 
  lg         = set_dp->ml; 
  set_dp->lg = lg;

  memset (tp, 0, lg);
  
  sp     = (ub_int_p) str;
  str_lg = strlen (str);

  for (i = 0; i < str_lg; i++, sp++)
    {
     if (lg < *sp) return FALSE;
     
     *(tp + *sp) = TRUE;         
    }
  
  return TRUE;
}                                     

/*--------------------------------------------------------------------------*/ 
/* concatenation of two strings into a resulting string                     */
/*--------------------------------------------------------------------------*/ 

void PmpConcat 
( 
 st_sdc_p str1_dp,
 st_sdc_p str2_dp,
 st_sdc_p str3_dp
)
{
 
 memcpy (str3_dp->pt,               str1_dp->pt, str1_dp->lg);
 memcpy (str3_dp->pt + str1_dp->lg, str2_dp->pt, str2_dp->lg);
                                                 
 str3_dp->lg = str1_dp->lg + str2_dp->lg;             
 

}                                                                  

/*--------------------------------------------------------------------------*/ 
/* find a string by it's name                                               */
/*--------------------------------------------------------------------------*/ 

bool PmpFind 
(   
 stt_tab_p tab_p,
 st_sdc_p  ind_dp,
 st_sdc_p  wert_dp
)
{
us_int    i;
stt_arr_p ap;
st_sdc    ind_d;
  
  ap = tab_p->arr;
  for (i = 0; i < tab_p->ct; i++, ap++)
    {
     ind_d.lg = ap->ind_lg;
     ind_d.pt = (ub_int_p) ap->ind;
     if (PmpLexeq ( &ind_d , ind_dp ))
       {
        wert_dp->lg = ap->wert_lg;
        wert_dp->pt = (ub_int_p) ap->wert;
        return TRUE;
       }
    }
  return FALSE;
}



/*--------------------------------------------------------------------------*/ 
/* match a string in the object string depending on options                 */
/*--------------------------------------------------------------------------*/ 

bool PmpInStr 
(
 st_pmv_p  pmv,
 bool      anchored,
 bool      sensitive,
 bool      infront,
 st_sdc_p  substr_dp
)
{
  us_int      i;
  us_int      r;
  bool        first;
  us_int      lg;
  us_int      cursor;
  us_int      precurs;
  us_int      savcurs;
  us_int      buf_ix;
  us_int      prebuf_ix;
  us_int      savbuf_ix;
  ub_int_p    pt;
  ub_int_p    sp;
  ub_int_p    subp;
  st_blktab_p b_p;
  
  cursor    = (us_int) pmv->cursor;
  precurs   = (us_int) cursor;
  buf_ix    = pmv->buf_ix;
  prebuf_ix = buf_ix;
  b_p       = pmv->b_p + buf_ix;
  pt        = b_p->blk_p;
  if (0 == buf_ix)
    {
     lg = (us_int) b_p->acc_lg;
    }
  else
    {
     lg = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
    }
  
  /* if null string,  result is TRUE */
  
  if (substr_dp->lg > 0)
    {
     r     = substr_dp->lg;        /* length of string */
     subp  = substr_dp->pt;        /* string pointer   */
     first = TRUE;
     while (r > 0)                 /* compare until r == 0 */
       {
        i = lg - cursor;           /* remainimg length in buffer */
        if (i >= r)
          {
           i = r;                  /* complete comparison */
           r = 0;
          }
        else
          {                        /* partial comparison */
           r -= i;                 /* remainig string length */
          }
        sp = pt + cursor;
        while (i > 0)             /* complete or partial comparison */
          {
           if (*sp != *subp) 
             {
              if (sensitive) break;                /* unequal if sensitive */
              if (*sp != pmv->ultab[*subp]) break; /* unequal in all cases */
             }
           if (first)                              /* save first equal     */
             {                                     /* position             */
              if (infront)                         /* if bratch or brook   */
                {
                 savbuf_ix = buf_ix;
                 savcurs   = cursor;
                }
              first = FALSE;              
             }
           sp++;
           subp++;
           i--;
          }
        if (i != 0)                                /* not equal            */
          {
           if (anchored) return FALSE;             /* exact or like        */
           precurs++;                              /* next try             */
           if (buf_ix != prebuf_ix)                /* previous block ?     */
             {
              buf_ix = prebuf_ix;
              b_p    = pmv->b_p + buf_ix;
              if (0 == buf_ix)
                 lg  = (us_int) b_p->acc_lg;
              else
                 lg  = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
              if (precurs == lg)
                {
                 buf_ix++;
                 b_p++;
                 prebuf_ix = buf_ix;
                 precurs   = 0;
                 lg        = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
                }
              pt     = b_p->blk_p;
             }
           cursor = precurs;
           first  = TRUE;
           subp   = substr_dp->pt;
           r      = substr_dp->lg;     
           continue;
          }
        if (r > 0)
          {
           if ((buf_ix + 1) == pmv->last_ix) return FALSE;
           b_p++;
           buf_ix++;
           prebuf_ix = buf_ix;
           pt        = b_p->blk_p;
           lg        = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
           cursor    = 0;
           precurs   = 0;
          }
       }
     pmv->cursor = (us_int) (sp - pt);
     pmv->buf_ix = buf_ix;
    }
  if (infront)
    {
     pmv->buf_ix = savbuf_ix;
     pmv->cursor = savcurs;
    }
  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* skip all characters in the string contained in a set of characters       */
/* starting from the cursor position, at least one character has to match   */
/*--------------------------------------------------------------------------*/ 

bool PmpSpan 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
)
{
  bool        brc;
  us_int      i;
  us_int      lg;
  us_int      cursor;
  us_int      buf_ix;
  ub_int_p    sp;
  ub_int_p    setp;
  st_blktab_p b_p;
  
  cursor = (us_int) pmv->cursor;
  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  
  if (0 == buf_ix)
    {
     lg = (us_int) b_p->acc_lg;
    }
  else
    {
     lg = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
    }
   
  if (cursor == lg)
    {
     if ((buf_ix + 1) == pmv->last_ix) return FALSE; 

     buf_ix++;
     b_p++;
     lg     = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
     cursor = 0;
    }

  setp = set_dp->pt;
  sp   = b_p->blk_p + cursor;
  
  if ( *(setp + *sp) )
    {
     cursor++;
     sp++;
     brc = TRUE;
     while (brc)
       {
        if (cursor < lg)
          {
           for (i = cursor; i < lg; i++)
             {
              if (!*(setp + *sp) ) break;
              cursor++;
              sp++;                         
             }
           if (i != lg) break; 
          }
        else
          {
           if ((buf_ix + 1) == pmv->last_ix) break; 

           buf_ix++;
           b_p++;
           lg     = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
           sp     = b_p->blk_p;
           cursor = 0;
          }
       }
     pmv->buf_ix = buf_ix;
     pmv->cursor = cursor;
     return TRUE;
    }
  else
    {
     return FALSE;
    }
  
}

/*--------------------------------------------------------------------------*/ 
/* brakes at the first character in string contained in a set of characters */
/*--------------------------------------------------------------------------*/ 

bool PmpBrake 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
)
{
  bool        brc;
  us_int      i;
  us_int      lg;
  us_int      cursor;
  us_int      buf_ix;
  ub_int_p    sp;
  ub_int_p    setp;
  st_blktab_p b_p;
  
  cursor = (us_int) pmv->cursor;
  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  setp   = set_dp->pt;       
  
  if (0 == buf_ix)
     lg = (us_int) b_p->acc_lg;
  else
     lg = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);

  sp = b_p->blk_p + cursor;

  brc = TRUE;
  while (brc)
    {
     if (cursor < lg)
       {
        for (i = cursor; i < lg; i++)
          {
           if (*(setp + *sp))
             {
              pmv->cursor = cursor + 1;
              pmv->buf_ix = buf_ix;
              return TRUE;
             } 
           sp++;                         
           cursor++;
          }
       }
     else
       {
        if ((buf_ix + 1) == pmv->last_ix) break; 

        buf_ix++;
        b_p++;
        lg     = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
        sp     = b_p->blk_p;
        cursor = 0;
       }
    }
  return FALSE;


}

/*--------------------------------------------------------------------------*/ 
/* brakes at the first character in string contained in a set of characters */
/*--------------------------------------------------------------------------*/ 

bool PmpNotbrake 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
)
{
  bool        brc;
  us_int      i;
  us_int      lg;
  us_int      cursor;
  us_int      buf_ix;
  ub_int_p    sp;
  ub_int_p    setp;
  st_blktab_p b_p;
  
  cursor = (us_int) pmv->cursor;
  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  setp   = set_dp->pt;       
  
  if (0 == buf_ix)
     lg = (us_int) b_p->acc_lg;
  else
     lg = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);

  sp = b_p->blk_p + cursor;

  brc = TRUE;
  while (brc)
    {
     if (cursor < lg)
       {
        for (i = cursor; i < lg; i++)
          {
           if (!*(setp + *sp))
             {
              pmv->cursor = cursor + 1;
              pmv->buf_ix = buf_ix;
              return TRUE;
             } 
           sp++;                         
           cursor++;
          }
       }
     else
       {
        if ((buf_ix + 1) == pmv->last_ix) break; 

        buf_ix++;
        b_p++;
        lg     = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
        sp     = b_p->blk_p;
        cursor = 0;
       }
    }
  return FALSE;


}
/*-----------------------------------------------------------------------------*/ 
/* breaks in front of a character in a string contained in a set of characters */
/*-----------------------------------------------------------------------------*/ 

bool PmpBreak 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
)
{
  bool        brc;
  us_int      i;
  us_int      lg;
  us_int      cursor;
  us_int      buf_ix;
  ub_int_p    sp;
  ub_int_p    setp;
  st_blktab_p b_p;  
  
  cursor = (us_int) pmv->cursor;
  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  setp   = set_dp->pt;       
  
  if (0 == buf_ix)
     lg = (us_int) b_p->acc_lg;
  else
     lg = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);

  sp = b_p->blk_p + cursor;

  brc = TRUE;
  while (brc)
    {
     if (cursor < lg)
       {
        for (i = cursor; i < lg; i++)
          {
           if (*(setp + *sp))
             {
              pmv->buf_ix = buf_ix;
              pmv->cursor = cursor;
              return TRUE;
             } 
           sp++;                         
           cursor++;
          }
       }
     else
       {
        if ((buf_ix + 1) == pmv->last_ix) break; 

        buf_ix++;
        b_p++;
        lg     = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
        sp     = b_p->blk_p;
        cursor = 0;
       }
    }
  return FALSE;


}

bool PmpBreakx 
(
 st_pmv_p   pmv,
 st_sdc_p  set_dp
)
{
  pmt_matchpos_p stk_alt;
  
  if (PmpBreak (pmv, set_dp))
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
        stk_alt->mxincarn  = pmv->stk.ctincarn;
        stk_alt->incarn    = pmv->curincarn;
        stk_alt->prebuf_ix = pmv->buf_ix;
        stk_alt->precurs   = pmv->cursor + 1;  /* what about buf_ix */
        return TRUE;
       }    
    }    
  else
    {
     return FALSE;
    }
}   

/*--------------------------------------------------------------------------*/ 
/* test whether a character is in a set of characters                       */
/*--------------------------------------------------------------------------*/ 

bool PmpAny 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
)
{
  us_int      lg;
  ub_int_p    pt;
  st_blktab_p b_p;
  
  b_p = pmv->b_p + pmv->buf_ix;
   
  if (0 == pmv->buf_ix)
     lg = (us_int) b_p->acc_lg;
  else
     lg = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
   
  if (pmv->cursor == lg)
    {
     if ((pmv->buf_ix + 1) == pmv->last_ix) return FALSE; 

     pmv->buf_ix++;
     b_p++;
     lg          = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
     pmv->cursor = 0;
    }

  pt = b_p->blk_p;
  
  if ( *(set_dp->pt + *(pt + pmv->cursor)) )
    {
     pmv->cursor++;
     return TRUE;
    }
  else
    {
     return FALSE;
    }
}

/*--------------------------------------------------------------------------*/ 
/* test whether a character is not in a set of characters                   */
/*--------------------------------------------------------------------------*/ 

bool PmpNotany 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
)
{
  us_int      lg;
  ub_int_p    pt;
  st_blktab_p b_p;
  
  b_p = pmv->b_p + pmv->buf_ix;
   
  if (0 == pmv->buf_ix)
     lg = (us_int) b_p->acc_lg;
  else
     lg = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
   
  if (pmv->cursor == lg)
    {
     if ((pmv->buf_ix + 1) == pmv->last_ix) return FALSE; 

     pmv->buf_ix++;
     b_p++;
     lg          = (us_int) (b_p->acc_lg - (b_p - 1)->acc_lg);
     pmv->cursor = 0;
    }

  pt = b_p->blk_p;
  
  if ( !*(set_dp->pt + *(pt + pmv->cursor)) )
    {
     pmv->cursor++;
     return TRUE;
    }
  else
    {
     return FALSE;
    }
}

bool PmpFindCset 
(
 st_pmv_p  pmv,
 bool      anchored,
 bool      infront,
 bool      sensitive,
 st_sdc_p  set_dp
)
{
  bool        brc;
  us_int      i;
  us_int      lg;
  us_int      cursor;
  us_int      buf_ix;
  ub_int_p    sp;
  ub_int_p    setp;
  st_blktab_p b_p;  
  
  cursor = (us_int) pmv->cursor;
  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  setp   = set_dp->pt;       
  
  lg = (us_int) b_p->acc_lg;
  sp = b_p->blk_p + cursor;
  
  if (anchored)
    {
     if (*(setp + *sp))
       {
        if (!infront)
          pmv->cursor++;
        return TRUE;
       }
     else
       {
        if (sensitive) return FALSE;
        if (*(setp + pmv->ultab[*sp]))
          {
           if (!infront)
             pmv->cursor++;
           return TRUE;
          }
        else
          {
           return FALSE;
          }
       }
    }
  
  brc = TRUE; 
  
  /*while (brc) PTS 1109360 end of loop never reached*/
    {
     if (cursor < lg)
       {
        for (i = cursor; i < lg; i++)
          {
           if (*(setp + *sp))
             {
              if (infront)
                pmv->cursor = cursor;
              else
                pmv->cursor = cursor + 1;
              return TRUE;
             }
		   else
             {
              if (!sensitive) 
                {
                 if (*(setp + pmv->ultab[*sp]))
                   {
                    if (infront)
                      pmv->cursor = cursor;
                    else
                      pmv->cursor = cursor + 1;
                    return TRUE;
                   }
                }
              
             }  
           sp++;                         
           cursor++;
          }
       }
     return FALSE;
    }
  /*return FALSE; PTS 1109360 statement not reached*/
}

/*--------------------------------------------------------------------------*/ 
/* match remaining buffer                                                   */
/*--------------------------------------------------------------------------*/ 

bool PmpRem  
(
 st_pmv_p  pmv
)
{
  
  pmv->buf_ix = pmv->last_ix - 1;
  
  if (0 == pmv->buf_ix)
    {
     pmv->cursor = (us_int) (pmv->b_p + pmv->buf_ix)->acc_lg;
    }
  else
    {
     pmv->cursor = (us_int) ((pmv->b_p + pmv->buf_ix)->acc_lg - 
                             (pmv->b_p + (pmv->buf_ix - 1))->acc_lg);
    }

  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmpPos 
(
 st_pmv_p  pmv,
 ul_int    pos
)
{
 
 if (0 == pmv->buf_ix)
   {
    return pos == pmv->cursor;                                    
   }
 else
   {
    return pos == (pmv->cursor + (pmv->b_p + (pmv->buf_ix - 1))->acc_lg);                                    
   }
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmpRpos 
(
 st_pmv_p  pmv,
 ul_int    pos
)
{
 
 if (0 == pmv->buf_ix)
   {
    return pmv->cursor == (pmv->b_p->acc_lg - pos);                                    
   }
 else
   {
    return ((pmv->b_p + (pmv->buf_ix - 1))->acc_lg + pmv->cursor) ==
           ((pmv->b_p + (pmv->last_ix - 1))->acc_lg - pos);
   }
}                                     

/*--------------------------------------------------------------------------*/ 
/* adds len to the cursor position if it fits                               */
/*--------------------------------------------------------------------------*/ 

bool PmpLen 
(
 st_pmv_p  pmv,
 ul_int    len
)
{
 ul_int      pos;
 us_int      buf_ix;
 st_blktab_p b_p;
 
  buf_ix = pmv->buf_ix;

  if (1 == pmv->last_ix)
    {
     if ((pmv->cursor  + len) <= pmv->b_p->acc_lg)
       {
        pmv->cursor += (us_int) len;
        return TRUE;
       }
     else
        return FALSE;
    }
  else
    {
     b_p = pmv->b_p + buf_ix;
     if (0 == buf_ix)
        pos = pmv->cursor + len; 
     else
        pos = (b_p - 1)->acc_lg + pmv->cursor + len; 
     while (pos > b_p->acc_lg)
       {
        if ((buf_ix + 1) == pmv->last_ix) return FALSE;
        buf_ix++;
        b_p++;
       }
     if (0 == buf_ix)
        pmv->cursor += (us_int) len;
     else
        pmv->cursor = (us_int) (pos - (b_p - 1)->acc_lg);
     pmv->buf_ix = buf_ix;
     return TRUE;
    }
}                                     

/*--------------------------------------------------------------------------*/ 
/* moves the cursor to position 'tab' in string                             */
/*--------------------------------------------------------------------------*/ 

bool PmpTab
(
 st_pmv_p  pmv,
 ul_int    tab
)
{
 ul_int      pos;
 us_int      buf_ix;
 st_blktab_p b_p;

  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  
  if (1 == pmv->last_ix)
    {
     if ((pmv->cursor <= tab) && (tab <= b_p->acc_lg))
       {
        pmv->cursor = (us_int) tab;
        return TRUE;
       }
     else
       return FALSE;
    }
  else
    {
     if (0 == buf_ix)
        pos = pmv->cursor; 
     else
        pos = (b_p - 1)->acc_lg + pmv->cursor; 

     if (pos > tab) return FALSE;
     
     while (tab > b_p->acc_lg)
       {
        if ((buf_ix + 1) == pmv->last_ix) return FALSE;
        buf_ix++;
        b_p++;
       }
     if (0 == buf_ix)
        pmv->cursor = (us_int) tab;
     else
        pmv->cursor = (us_int) (tab - (b_p - 1)->acc_lg);
     pmv->buf_ix = buf_ix;
     return TRUE;
    }
}                                     

/*--------------------------------------------------------------------------*/ 
/* moves the cursor to position lg - 'tab' in string                        */
/*--------------------------------------------------------------------------*/ 

bool PmpRtab
(
 st_pmv_p  pmv,
 ul_int    rtab
)
{
 ul_int      pos;
 ul_int      tab;
 us_int      buf_ix;
 st_blktab_p b_p;
  
  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  
  if (1 == pmv->last_ix)
    {
     if (pmv->cursor <= (b_p->acc_lg - rtab))
       {
        pmv->cursor = (us_int) (b_p->acc_lg - rtab);
        return TRUE;
       }
     else
       return FALSE;
    }
  else
    {
     if (0 == buf_ix)
        pos = pmv->cursor; 
     else
        pos = (b_p - 1)->acc_lg + pmv->cursor; 

     tab = (pmv->b_p + (pmv->last_ix - 1))->acc_lg - rtab;
     
     if (pos < tab) return FALSE;
     
     while (tab > b_p->acc_lg)
       {
        if ((buf_ix + 1) == pmv->last_ix) return FALSE;
        buf_ix++;
        b_p++;
       }
     if (0 == buf_ix)
        pmv->cursor = (us_int) (b_p->acc_lg - rtab);
     else
        pmv->cursor = (us_int) (pos - (b_p - 1)->acc_lg);
     pmv->buf_ix = buf_ix;
     return TRUE;
    }                                     
}                                     

/*--------------------------------------------------------------------------*/ 
/* search for balancing right parenthethis, skip quoted strings             */
/*--------------------------------------------------------------------------*/ 

bool PmpFbal
(
 st_pmv_p  pmv
)
{
  us_int      i;
  us_int      lg;
  us_int      buf_ix;
  s_int       ct;
  bool        qot;
  ub_int_p    sp;
  st_blktab_p b_p;

  
  buf_ix = pmv->buf_ix;
  b_p    = pmv->b_p + buf_ix;
  
  if(0 == buf_ix)
     lg  = (us_int) b_p->acc_lg;
  else
     lg  = (us_int) (b_p->acc_lg - (b_p -1)->acc_lg);
  
  
  i   = (us_int) pmv->cursor;
  ct  = 0;
  qot = FALSE; 
  sp  = b_p->blk_p + i;
  
  
  while (ct >= 0) 
    {
     if (i < lg)
       {
        switch (*sp)
          {
           case '\'': qot = !qot; break;
           case '(' : if (!qot) ct++; break;
           case ')' : if (!qot) ct--; break;
          }
        i++;
        sp++;
       }
     else
       {
        if ((buf_ix + 1) == pmv->last_ix)
          break;
        else
          {
           buf_ix++;
           b_p++;
           lg = (us_int) (b_p->acc_lg - (b_p -1)->acc_lg);
           sp = b_p->blk_p;
           i  = 0;
          }
       }
    }


  if (ct < 0)
    { 
     pmv->cursor = i - 1;
     pmv->buf_ix = buf_ix;
     return TRUE;
    }
  else
    { 
     return FALSE;
    }
}
