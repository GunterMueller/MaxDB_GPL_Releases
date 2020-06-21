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
#include "idl/idl2xml/pms00.h" 
#include "idl/idl2xml/pmsprm.h" 
#include "idl/idl2xml/pmsmsg.h"

                 
/*--------------------------------------------------------------------------*/ 
/* match remaining string                                                   */
/*--------------------------------------------------------------------------*/ 

bool PmsRem  
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p
)
{
  *cursor_p = str_dp->lg;
  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* match a string = directly compare string to substr behind cursor         */
/*--------------------------------------------------------------------------*/ 

bool PmsCmpr 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
)
{
  us_int    i;
  ub_int_p  sp;
  ub_int_p  subp;
  
  if (substr_dp->lg > 0)
    {
     if ((*cursor_p + substr_dp->lg) > str_dp->lg)
       {
        return FALSE;
       }
     else
       {
        i    = substr_dp->lg;
        sp   = str_dp->pt + *cursor_p;
        subp = substr_dp->pt;
        while ((i > 0) && (*sp == *subp)) 
          {
           sp++;
           subp++;
           i--;
          }
        if (i != 0)
          {
           return FALSE;
          }
        else
          {
           *cursor_p += substr_dp->lg;
          }
       }
    }
  return TRUE;
}

/*--------------------------------------------------------------------------*/ 
/* non-case-sensitive string match                                          */
/*--------------------------------------------------------------------------*/ 

bool PmsLike
(
 ub_int    ultab[256],
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
)
{
  us_int    i;
  ub_int_p  sp;
  ub_int_p  subp;
  
  if (substr_dp->lg > 0)
    {
     if ((*cursor_p + substr_dp->lg) > str_dp->lg)
       {
        return FALSE;
       }
     else
       {
        i    = substr_dp->lg;
        sp   = str_dp->pt + *cursor_p;
        subp = substr_dp->pt;
        while (i > 0) 
          {
           if (*sp != *subp) 
             {
              if (*sp != ultab[*subp]) break;
             }
           sp++;
           subp++;
           i--;
          }
        if (i != 0)
          {
           return FALSE;
          }
        else
          {
           *cursor_p += substr_dp->lg;
          }
       }
    }
  return TRUE;
}
/*--------------------------------------------------------------------------*/ 
/* match a string = in the match subject str                                */
/*--------------------------------------------------------------------------*/ 

bool PmsMatch 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
)
{
  us_int    lg;
  us_int    rem_lg;
  us_int    cursor;
          
  cursor = *cursor_p;
  rem_lg = str_dp->lg - cursor;
  lg     = substr_dp->lg;
  
  while (lg <= rem_lg)
    {
     if (PmsCmpr (str_dp, &cursor, substr_dp)) 
       {
        *cursor_p = cursor;
        return TRUE;
       }
     cursor++;
     rem_lg--;
    }
  return FALSE;
}
                                     
/*--------------------------------------------------------------------------*/ 
/* match a string = in the match subject str                                */
/*--------------------------------------------------------------------------*/ 

bool PmsLook 
(
 ub_int    ultab[256],
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
)
{
  us_int    lg;
  us_int    rem_lg;
  us_int    cursor;
          
  cursor = *cursor_p;
  rem_lg = str_dp->lg - cursor;
  lg     = substr_dp->lg;
  
  while (lg <= rem_lg)
    {
     if (PmsLike (ultab, str_dp, &cursor, substr_dp)) 
       {
        *cursor_p = cursor;
        return TRUE;
       }
     cursor++;
     rem_lg--;
    }
  return FALSE;
}
                                     
/*--------------------------------------------------------------------------*/ 
/* compare cursor with position                                             */
/*--------------------------------------------------------------------------*/ 

bool PmsPos 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return pos == *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position (GT)                                        */
/*--------------------------------------------------------------------------*/ 

bool PmsPosGt 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return pos > *cursor_p;                                    
}                                     
/*--------------------------------------------------------------------------*/ 
/* compare cursor with position (GE)                                        */
/*--------------------------------------------------------------------------*/ 

bool PmsPosGe
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return pos >=*cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position (LT)                                        */
/*--------------------------------------------------------------------------*/ 

bool PmsPosLt
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return pos < *cursor_p;                                    
}                                     
/*--------------------------------------------------------------------------*/ 
/* compare cursor with position (LE)                                        */
/*--------------------------------------------------------------------------*/ 

bool PmsPosLe
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return pos <= *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position (NE)                                        */
/*--------------------------------------------------------------------------*/ 

bool PmsPosNe
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return pos != *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string                            */
/*--------------------------------------------------------------------------*/ 

bool PmsRpos 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return  (str_dp->lg - pos) == *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string (LT)                       */
/*--------------------------------------------------------------------------*/ 

bool PmsRposLt 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return  (str_dp->lg - pos) < *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string (LE)                       */
/*--------------------------------------------------------------------------*/ 

bool PmsRposLe
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return  (str_dp->lg - pos) <= *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string (GE)                       */
/*--------------------------------------------------------------------------*/ 

bool PmsRposGe
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return  (str_dp->lg - pos) >= *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string (GT)                       */
/*--------------------------------------------------------------------------*/ 

bool PmsRposGt
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return  (str_dp->lg - pos) > *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* compare cursor with position at end of string (NE)                       */
/*--------------------------------------------------------------------------*/ 

bool PmsRposNe
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
)
{
  return  (str_dp->lg - pos) != *cursor_p;                                    
}                                     

/*--------------------------------------------------------------------------*/ 
/* adds len to the cursor position if it fits                               */
/*--------------------------------------------------------------------------*/ 

bool PmsLen 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    len
)
{
  if ((*cursor_p + len) <= str_dp->lg)
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

bool PmsTab
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    tab
)
{
  if ((*cursor_p <= tab) && (tab <= str_dp->lg))
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

bool PmsRtab
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    tab
)
{
  if (*cursor_p <= (str_dp->lg - tab))
    {
     *cursor_p = str_dp->lg - tab;
     return TRUE;
    }
  else
    return FALSE;
}                                     


/*--------------------------------------------------------------------------*/ 
/* test whether a character is in a set of characters                       */
/*--------------------------------------------------------------------------*/ 

bool PmsAny 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{

  if (*cursor_p < str_dp->lg)
    {
     if ( *(set_dp->pt + *(str_dp->pt + *cursor_p)) )
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

bool PmsNotany 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{

  if (*cursor_p < str_dp->lg)
    {
     if (!*(set_dp->pt + *(str_dp->pt + *cursor_p)) )
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

bool PmsSpan 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  us_int    i;
  us_int    lg;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p < str_dp->lg)
    {
     sp   = str_dp->pt + *cursor_p;
     lg   = str_dp->lg;
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
/* skip all characters in the string contained in a set of characters       */
/* starting from the cursor position, at least one character has to match   */
/*--------------------------------------------------------------------------*/ 

bool PmsBackSpan 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  us_int    i;
  us_int    lg;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p > 0)
    {
     sp   = str_dp->pt + *cursor_p;
     lg   = str_dp->lg;
     setp = set_dp->pt;
     
     if ( *(setp + *sp) )
       {
        (*cursor_p)--;
        for (i = *cursor_p; i > 0; i--)
          {
           sp--;                         
           if (!*(setp + *sp) ) break;
           (*cursor_p)--;
          }
        return TRUE;
       }
    }

  return FALSE;
}

/*--------------------------------------------------------------------------*/ 
/* break at first character in string contained in a set of characters      */
/*--------------------------------------------------------------------------*/ 

bool PmsBreak 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  us_int    i;
  us_int    lg;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p < str_dp->lg)
    {
     sp   = str_dp->pt + *cursor_p;
     lg   = str_dp->lg;
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

bool PmsBrake 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  us_int    i;
  us_int    lg;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p < str_dp->lg)
    {
     sp   = str_dp->pt + *cursor_p;
     lg   = str_dp->lg;
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

bool PmsNotbrake 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  us_int    i;
  us_int    lg;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p < str_dp->lg)
    {
     sp   = str_dp->pt + *cursor_p;
     lg   = str_dp->lg;
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
/* break at first character in string contained in a set of characters      */
/*--------------------------------------------------------------------------*/ 

bool PmsBackBreak 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
)
{
  us_int    i;
  us_int    lg;
  ub_int_p  sp;
  ub_int_p  setp;
  
  
  if (*cursor_p > 0)
    {
     sp   = str_dp->pt + *cursor_p;
     lg   = str_dp->lg;
     setp = set_dp->pt;
     
     for (i = *cursor_p; i > 0; i--)
       {
        if (*(setp + *sp) )
          {
           *cursor_p = i;
           return TRUE;
          }
        sp--;                         
       }
    }

  return FALSE;
}

/*--------------------------------------------------------------------------*/ 
/* search for balancing right parenthethis, skip quoted strings             */
/*--------------------------------------------------------------------------*/ 

bool PmsFbal
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p
)
{
  us_int    i;
  us_int    lg;
  s_int     ct;
  bool      qot;
  ub_int_p  sp;
  
  i   = *cursor_p;
  ct  = 0;
  qot = FALSE; 
  lg  = str_dp->lg;
  sp  = str_dp->pt + i;
  
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


