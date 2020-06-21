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
/* Pattern Match Primitives                                                 */
/*--------------------------------------------------------------------------*/ 

#include "idl/idl2xml/pen00.h"
#include "idl/idl2xml/mam.h"
#include "idl/idl2xml/rte.h" 
#include "idl/idl2xml/pms00.h" 
#include "idl/idl2xml/pmexec.h" 
#include "idl/idl2xml/pmmadm.h" 
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmsmsg.h"

#define M__NAME "PMLPP"
                 
/*--------------------------------------------------------------------------*/ 
/* match remaining string                                                   */
/*--------------------------------------------------------------------------*/ 

bool PmlRem  
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p
)
{
  *cursor_p = lg;
  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* match a string = directly compare string to substr behind cursor         */
/*--------------------------------------------------------------------------*/ 

bool PmlInStr 
(
 ub_int    ultab[256],
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 bool      anchored,
 bool      sensitive,
 bool      infront,
 st_sdc_p  substr_dp
)
{
  us_int    i;
  ub_int_p  sp;
  ub_int_p  subp;

  us_int    sublg;
  ul_int    rem_lg;
  ul_int    cursor;

  if (0 == substr_dp->lg) return TRUE;
          
  cursor = *cursor_p;
  sublg  = substr_dp->lg;
  rem_lg = lg - cursor;
  
  while (sublg <= rem_lg)
    {
     i     = sublg;
     subp  = substr_dp->pt;
     sp    = pt + cursor;
     while (i > 0) 
       {
        if (*sp != *subp)
          {
           if (sensitive) break;
           if (*sp != ultab[*subp]) break;
          }
        sp++;
        subp++;
        i--;
       }
     if (i != 0)
       {
        if (anchored) return FALSE;
        cursor++;
        rem_lg--;
       }
     else
       {
        if (!infront)
          cursor += substr_dp->lg;
        *cursor_p = cursor;
        return TRUE;
       }
    }
  return FALSE;

  
}

bool PmlFindCset 
(
 ub_int    ultab[256],
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 bool      anchored,
 bool      infront,
 bool      sensitive,
 st_sdc_p  set_dp
)
{
  bool        brc;
  ul_int      i;
  ul_int      cursor;
  ub_int_p    sp;
  ub_int_p    setp;
  
  cursor = *cursor_p;
  setp   = set_dp->pt;       
  sp     = pt + cursor;
  
  if (cursor == lg) return FALSE;

  if (anchored)
    {
     if (*(setp + *sp))
       {
        if (!infront)
          (*cursor_p)++;
        return TRUE;
       }
     else
       {
        if (sensitive) return FALSE;
        if (*(setp + ultab[*sp]))
          {
           if (!infront)
             (*cursor_p)++;
           return TRUE;
          }
        else
          {
           return FALSE;
          }
       }
    }
  
  brc = TRUE; 
  
  /* for( ;brc; ) PTS 1109360 end of loop never reached*/
    {
     if (cursor < lg)
       {
        for (i = cursor; i < lg; i++)
          {
           if (*(setp + *sp))
             {
              if (infront)
                *cursor_p = cursor;
              else
                *cursor_p = cursor + 1;
              return TRUE;
             }
		   else
             {
              if (!sensitive) 
                {
                 if (*(setp + ultab[*sp]))
                   {
                    if (infront)
                      *cursor_p = cursor;
                    else
                      *cursor_p = cursor + 1;
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
  /* return FALSE;  PTS 1109360 statement not reached*/
}

bool PmlFindNotCset 
(
 ub_int    ultab[256],
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 bool      anchored,
 bool      infront,
 bool      sensitive,
 st_sdc_p  set_dp
)
{
  bool        brc;
  ul_int      i;
  ul_int      cursor;
  ub_int_p    sp;
  ub_int_p    setp;
  
  cursor = *cursor_p;
  setp   = set_dp->pt;       
  sp     = pt + cursor;
  
  if (cursor == lg) return FALSE;

  if (anchored)
    {
     if (sensitive)
       {
        if (!*(setp + *sp))
          {
           if (!infront)
             (*cursor_p)++;
           return TRUE;
          }
       }
     else
       {
        if ((!*(setp + *sp)) && (!*(setp + ultab[*sp])))
          {
           if (!infront)
             (*cursor_p)++;
           return TRUE;
          }
       }
     return FALSE;
    }
  
  brc = TRUE; 
  
  /*while (brc)  PTS 1109360 end of loop never reached*/
    {
     if (cursor < lg)
       {
        for (i = cursor; i < lg; i++)
          {
           if (sensitive) 
             {
              if (!*(setp + *sp))
                {
                 if (infront)
                    *cursor_p = cursor;
                 else
                    *cursor_p = cursor + 1;
                 return TRUE;
                }
             }
		   else
             {
              if ((!*(setp + *sp)) && (!*(setp + ultab[*sp])))
                {
                 if (infront)
                    *cursor_p = cursor;
                 else
                    *cursor_p = cursor + 1;
                 return TRUE;
                }
             }  
           sp++;                         
           cursor++;
          }
       }
     return FALSE;
    }

  /* return FALSE; PTS 1109360 statement not reached*/
}

                                     
/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmlPos 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return pos == *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmlPosGt 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p > pos;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmlPosGe 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p >= pos;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmlPosLt 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p < pos;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmlPosLe 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p <= pos;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmlPosNe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p != pos;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmlRpos 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p == (lg - pos);                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmlRposLt 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p < (lg - pos);                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmlRposLe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p <= (lg - pos);                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmlRposGe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p >= (lg - pos);                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmlRposGt 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p > (lg - pos);                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmlRposNe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
)
{
  return *cursor_p != (lg - pos);                                    
}                                     


/*--------------------------------------------------------------------------*/ 
/* adds len to the cursor position if it fits                               */
/*--------------------------------------------------------------------------*/ 

bool PmlLen 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    len
)
{
  if ((*cursor_p + len) <= lg)
    {
     *cursor_p += len;
     return TRUE;
    }
  else
    return FALSE;
}                                     

/*--------------------------------------------------------------------------*/ 
/* moves the cursor to position 'tab' in string                             */
/*--------------------------------------------------------------------------*/ 

bool PmlTab
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    tab
)
{
  if ((*cursor_p <= tab) && (tab <= lg))
    {
     *cursor_p = tab;
     return TRUE;
    }
  else
    return FALSE;
}                                     

/*--------------------------------------------------------------------------*/ 
/* moves the cursor to position lg - 'tab' in string                        */
/*--------------------------------------------------------------------------*/ 

bool PmlRtab
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    tab
)
{
  if (*cursor_p <= (lg - tab))
    {
     *cursor_p = lg - tab;
     return TRUE;
    }
  else
    return FALSE;
}                                     


/*--------------------------------------------------------------------------*/ 
/* test whether a character is in a set of characters                       */
/*--------------------------------------------------------------------------*/ 

bool PmlAny 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
)
{

  if (*cursor_p < lg)
    {
     if ( *(set_dp->pt + *(pt + *cursor_p)) )
       {
        (*cursor_p)++;
        return TRUE;
       }
    }

  return FALSE;
}

/*--------------------------------------------------------------------------*/ 
/* test whether a character is not in a set of characters                   */
/*--------------------------------------------------------------------------*/ 

bool PmlNotany 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
)
{

  if (*cursor_p < lg)
    {
     if (!*(set_dp->pt + *(pt + *cursor_p)) )
       {
        (*cursor_p)++;
        return TRUE;
       }
    }

  return FALSE;
}

/*--------------------------------------------------------------------------*/ 
/* skip all characters in the string contained in a set of characters       */
/* starting from the cursor position, at least one character has to match   */
/*--------------------------------------------------------------------------*/ 

bool PmlSpan 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  ul_int    i;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p < lg)
    {
     sp   = pt + *cursor_p;
     setp = set_dp->pt;
     
     if ( *(setp + *sp) )
       {
        (*cursor_p)++;
        for (i = *cursor_p; i < lg; i++)
          {
           sp++;                         
           if (!*(setp + *sp) ) break;
           (*cursor_p)++;
          }
        return TRUE;
       }
    }

  return FALSE;
}


/*--------------------------------------------------------------------------*/ 
/* break at first character in string contained in a set of characters      */
/*--------------------------------------------------------------------------*/ 

bool PmlBreak 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  ul_int    i;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p < lg)
    {
     sp   = pt + *cursor_p;
     setp = set_dp->pt;
     
     for (i = *cursor_p; i < lg; i++)
       {
        if (*(setp + *sp) )
          {
           *cursor_p = i;
           return TRUE;
          }
        sp++;                         
       }
    }

  return FALSE;
}
/*--------------------------------------------------------------------------*/ 
/* brakes at the first character in string contained in a set of characters */
/*--------------------------------------------------------------------------*/ 

bool PmlBrake 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  ul_int    i;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p < lg)
    {
     sp   = pt + *cursor_p;
     setp = set_dp->pt;
     
     for (i = *cursor_p; i < lg; i++)
       {
        if (*(setp + *sp) )
          {
           *cursor_p = i + 1;
           return TRUE;
          }
        sp++;                         
       }
    }

  return FALSE;                      
}
/*--------------------------------------------------------------------------*/ 
/* brakes at the first character not contained in a set of characters       */
/*--------------------------------------------------------------------------*/ 

bool PmlNotbrake 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  ul_int    i;
  ub_int_p  setp;
  ub_int_p  sp;
  
  
  if (*cursor_p < lg)
    {
     sp   = pt + *cursor_p;
     setp = set_dp->pt;
     
     for (i = *cursor_p; i < lg; i++)
       {
        if (!*(setp + *sp) )
          {
           *cursor_p = i + 1;
           return TRUE;
          }
        sp++;                         
       }
    }

  return FALSE;                      
}


/*--------------------------------------------------------------------------*/ 
/* search for balancing right parenthethis, skip quoted strings             */
/*--------------------------------------------------------------------------*/ 

bool PmlFbal
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p
)
{
  ul_int    i;
  l_int     ct;
  bool      qot;
  ub_int_p  sp;
  
  i   = *cursor_p;
  ct  = 0;
  qot = FALSE; 
  sp  = pt + i;
  
  while ((i < lg) && (ct >= 0)) 
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
  if (ct < 0)
    { 
     *cursor_p = i - 1;
     return TRUE;
    }
  else
    { 
     return FALSE;
    }
}

bool PmlBreakx 
(
 st_pmv_p   pmv,
 st_sdc_p  set_dp
)
{
  pmt_matchpos_p stk_alt;
  
  if (PmlBreak (pmv->pt, pmv->lg, &pmv->cursor, set_dp))
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

