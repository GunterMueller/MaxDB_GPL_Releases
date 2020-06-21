/*!**************************************************************************

  module      : vpr05c.c

  -------------------------------------------------------------------------

  responsible : 

  special area:
  description : Contains functions for analysis sql statements

  last changed: 2000-09-08  9:04
  see also    :

  -------------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "vpr05c.h"
#include "vpr07Macro.h"
#include "vpr05IfCom_String.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef WIN32
#define ISSPACE(c) (isspace(c) || c == 0x10)
#else
#define ISSPACE(c) (isspace(c))
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
  Function:     pr05cGetEncodingPos

  see also: 

  Description:  computes depending on encoding type the offset for next 
                character and position of zero byte in UCS2-Strings which 
                containing 7-bit Ascii.

  Arguments:
    entype  [in]  encoding type
    posanf  [in]  begin position
    offset  [out] offset to get next character (1 for Ascii, 2 for UCS2)
    zeropos [out] position in UCS2 character wich containing zero byte in
                  cases of 7-bit Ascii

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes actually written into buffer
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed
    </UL>
 */
tsp00_Int4 pr05cGetEncodingPos(tpr05_StringEncoding entype, tsp00_Int4 posanf, tsp00_Int4 *offset, tsp00_Int4 *zeropos)
{
  tsp00_Int4 pos;
  if (entype == sp77encodingAscii) {
    *zeropos = 0;
    pos = posanf;
    *offset = 1;
  } else if (entype == sp77encodingUCS2) {
    *zeropos = -1;
    pos = posanf + 1;
    *offset = 2;
  } else if (entype == sp77encodingUCS2Swapped) {
    *zeropos = 1;
    pos = posanf;
    *offset = 2;
  } else {
    *zeropos = 0;
    pos = posanf;
    *offset = 1;
  }
  return pos;
}

/*!
  Function:     pr05cChToInt4

  see also: 

  Description:  Converts unicode or ascii characters into an tsp00_Int4.

  Arguments:
    buf     [in]  buffer to scan
    posanf  [in]  begin position in buffer
    len     [in]  length of part of buffer to scan
    entype  [in]  encoding type of buffer

  return value: converted integer number
 */
tsp00_Int4 pr05cChToInt4 (char *buf, tsp00_Int4 posanf, tsp00_Int4 len,
                          tpr05_StringEncoding entype)
{
  const int limitmax =  7;
  const int maxintl  = 10;

  int i, n, l;
  int zeropos, offset, pos;
  tsp00_Int4 intv;
  tsp00_Int4 intv4limit;

  pos = pr05cGetEncodingPos (entype, posanf, &offset, &zeropos);

  intv4limit = (csp_maxint4 - 9) / 10;
  intv = 0;
  l = len - 1;

  if (len > maxintl) {
    l = maxintl - 1;
  }
  
  for (i = 0; i <= l; i += offset) {
    n = buf[pos-1+i] - '0';
    if (intv <= intv4limit) {
      intv = intv * 10 + n;
    } else {
      if ((intv == intv4limit + 1) && (n <= limitmax))
        intv = intv * 10 + n;
      else
        intv = intv;
    }
  }
  return intv;
}

/*!
  Function:     pr05cNextSymbol

  see also: 
  
  Description:  Gets next token from an input string
    
  Arguments:
    buf           [in]  pointer to character array
    buflen        [in]  length of buf in bytes
    posanf        [in]  start position in buf for searching
    sympos        [out] position of token found in buf
    symlen        [out] length of token found in buf
    symb          [out] type of token found in buf
    accpos        [out] points to next char behind token
    entype        [in]  encoding type (ascii, ucs2, ucs2swapped)

  return value:   void
 */

void pr05cNextSymbol (char *buf, tsp00_Int4 buflen, tsp00_Int4 posanf, tsp00_Int4 *sympos,
		      tsp00_Int4 *symlen, tsp00_Int2 *symb, tsp00_Int4 *accpos, 
		      tpr05_StringEncoding entype)
{
  static const char p05sqldelim [] = "\011 !#%&*()-+=;:'\",.<>/?";
  char stringend;
  tsp00_Int4 pos, i, j, dif;
  int offset, zeropos;
  char c;

  /* CAUTION: Function parameters posanf, sympos and accpos use Pascal-like
              indexing (array indices starting at 1)
  */

  /* Depending on encoding type, several offset values have to be set:
     - zeropos: offset relative to the actual buffer position to
       look for an adjacent zero byte in a UCS2 character
     - pos: shifted start position in case of UCS2 or UCS2 swapped
     - offset: how many bytes to skip when moving to next character
  */
  pos = pr05cGetEncodingPos(entype, posanf, &offset, &zeropos);

  *symlen = 0;
  dif = 0;

  if (pos > buflen) {
    *symb = CPR_S_EOF;
    *sympos = *accpos;
    return;
  }

  /* eat spaces */
  /* ' ' changed to isspace, according to ignore tabs, cr etc. PTS 1108057 */
  while (pos < buflen && ISSPACE(buf[pos-1]) && 
	 PR07CIS_ASCII (buf[pos-1+zeropos], entype))
    pos += offset;

  if (pos > buflen)
    *symb = CPR_S_EOF;
  else {
    if (buf[pos-1] == 'X' && 
        PR07CIS_ASCII (buf[pos-1+zeropos], entype) &&
        (pos+offset) < buflen &&
	buf[pos+offset] == '\'' && 
	PR07CIS_ASCII (buf[pos+offset+zeropos], entype)) {
      /* don't allow hex values */
      pos += offset;
      *symb = CPR_S_UNKNOWN;
    }	
    *sympos = pos;
    if (entype == sp77encodingUCS2)
      --*sympos;

    if (!PR07CIS_ASCII (buf[pos-1+zeropos], entype) ||
	(PR07CIS_ASCII (buf[pos-1+zeropos], entype) &&
	 ((c = buf[pos-1]) < 'A' || c > 'Z') &&
	 (c < 'a' || c > 'z') &&
	 (c < '0' || c > '9') &&
	 c != '$' && c != '^' && c != '#' && c != '_' && 
	 c != '"' && c != '.' && c != '\'' && c != '>' &&
	 c != '<' && c != ':' && c != CPR_PREPARECHR &&
	 c != '=' && c != '+' && c != '-' &&
	 c != '/' && c != ',' && c != '(' &&
	 c != ')' && c != CPR_PC_ENDSIGN && c != ';' &&
	 c != CPR_ASTERISK)) {
      /* first character of next token to be recognized is UCS2 or
         UCS2 swapped or 8-Bit ASCII not contained in case statement below, 
	 so read until sql delimiter symbol has been found 
      */
      *symb = CPR_S_IDENTIFIER;
      
      do {
	pos += offset;
      } while (pos <= buflen && 
	       (!strchr (p05sqldelim, buf[pos-1]) || 
		!PR07CIS_ASCII (buf[pos-1+zeropos], entype)));      
    } else {
      /* first character of next token to be recognized is 7-Bit ASCII */
      switch (buf[pos-1]) {
      case 'A': {}
      case 'B': {}
      case 'C': {}
      case 'D': {}
      case 'E': {}
      case 'F': {}
      case 'G': {}
      case 'H': {}
      case 'I': {}
      case 'J': {}
      case 'K': {}
      case 'L': {}
      case 'M': {}
      case 'N': {}
      case 'O': {}
      case 'P': {}
      case 'Q': {}
      case 'R': {}
      case 'S': {}
      case 'T': {}
      case 'U': {}
      case 'V': {}
      case 'W': {}
      case 'X': {}
      case 'Y': {}
      case 'Z': {}
      case 'a': {}
      case 'b': {}
      case 'c': {}
      case 'd': {}
      case 'e': {}
      case 'f': {}
      case 'g': {}
      case 'h': {}
      case 'i': {}
      case 'j': {}
      case 'k': {}
      case 'l': {}
      case 'm': {}
      case 'n': {}
      case 'o': {}
      case 'p': {}
      case 'q': {}
      case 'r': {}
      case 's': {}
      case 't': {}
      case 'u': {}
      case 'v': {}
      case 'w': {}
      case 'x': {}
      case 'y': {}
      case 'z': {}
      case '$': {}
      case '^': {}
      case '#': {}
      case '_': {
	/* identifier with first letter ASCII, following letters
           mixed ASCII/UCS2 (UCS2 swapped)
	*/

	*symb = CPR_S_IDENTIFIER;
	do {
	  pos += offset;
	}
	while ((pos <= buflen)
	       && (!PR07CIS_ASCII (buf[pos-1+zeropos], entype)
		   || !(((c = buf[pos-1]) < 'a' || c > 'z') 
			&& (c < 'A' || c > 'Z')
			&& (c < '0' || c > '9') && c != '_' && c != '^' 
			&& c != '#' && c != '$')));			
	break;
      }
      case '"' : {
	/* quoted identifier */
	*sympos += offset;
	*symb = CPR_S_UNKNOWN;

	pos += offset;
	stringend = 0;
	*symb = CPR_S_IDENTIFIER;
	do {
	  /* take care of embedded quotes */
	  while (pos <= buflen && 
		 (buf[pos-1] != '"' || !PR07CIS_ASCII (buf[pos-1+zeropos], entype)))
	    pos += offset;

          if (pos <= buflen - offset) {
            if (buf[pos-1+offset] == '"' && PR07CIS_ASCII (buf[pos-1+offset+zeropos], entype)){
              pos += 2 * offset;
            } else	  
              stringend = 1;
          } else
            stringend = 1;
	} while (!stringend);
	pos += offset;
	dif = offset;
	break;
      }
      default : {
	switch (buf[pos-1]) {
	case '.' : {}
	case '0' : {}
	case '1' : {}
	case '2' : {}
	case '3' : {}
	case '4' : {}
	case '5' : {}
	case '6' : {}
	case '7' : {}
	case '8' : {}
	case '9' : {
	  /* token starting with ASCII digit */
	  *symb = CPR_S_UNKNOWN;

	  if ((buf[pos-1] == '.') && PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
	    *symb = CPR_S_POINT;
	    pos += offset;
	    while ((c = buf[pos-1]) >= '0' && c <= '9' && 
		   PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
	      *symb = CPR_S_FIXED_POINT_LITERAL;
	      pos += offset;
	    }	
	    if (pos > buflen)
	      *symb = CPR_S_EOF;
	  } else {
	    *symb = CPR_S_UNSIGNED_INTEGER;
	    pos += offset;
	    while ((pos <= buflen) && ((c = buf[pos-1]) >= '0') && (c <= '9') &&
		   PR07CIS_ASCII (buf[pos-1+zeropos], entype))
	      pos += offset;
	    if (pos <= buflen) {
	      if ((buf[pos-1] == '.') && PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
		*symb = CPR_S_FIXED_POINT_LITERAL;
		do
		  pos += offset;
		while (!((c = buf[pos-1]) < '0' || c > '9' ||
			 !PR07CIS_ASCII (buf[pos-1+zeropos], entype)));
		if (pos > buflen + offset)
		  *symb = CPR_S_EOF;
	      }	
	    }
	  }
	  if (((int ) *symb != CPR_S_EOF) && (pos <= buflen)) {
	    if ((buf[pos-1] == 'E') && ((int ) *symb != CPR_S_POINT) && 
		PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
	      pos += offset;
	      if ((((c = buf[pos-1]) == '+') || (c == '-')) && 
		  PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
		pos += offset;
	      }
	      if (((c = buf[pos-1]) >= '0') && (c <= '9') && 
		  PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
		*symb = CPR_S_FLOATING_POINT_LITERAL;
		i = 0;
		do {
		  i++;
		  pos += offset;
		} while (!((((c = buf[pos-1]) < '0') || (c > '9')) && 
			   PR07CIS_ASCII (buf[pos-1+zeropos], entype)));
		if (pos > buflen + offset)
		  *symb = CPR_S_EOF;
		if (i > 2)
		  *symb = CPR_S_UNKNOWN;
	      } else {
		*symb = CPR_S_UNKNOWN;
	      }	
	    }
	  }
	  break;
	}
	case '\'' : {
	  *symb = CPR_S_STRING_LITERAL;
	  stringend = 0;
	  pos += offset;
	  *sympos += offset;
	  j = pos;
          do {
            if (pos > buflen) {
              stringend = 1;
              *symb = CPR_S_EOF;
            } else {
              if (pos < buflen-offset+1) {
                if ((buf[pos-1] == '\'') && PR07CIS_ASCII (buf[pos-1+zeropos], entype) &&
                    (buf[pos-1+offset] == '\'') && PR07CIS_ASCII (buf[pos-1+offset+zeropos], entype)) {
                    j += 2*offset;
                    pos += 2*offset;
                    stringend = 0;
                  }
              }
              if (((buf[pos-1] == '\'') && PR07CIS_ASCII (buf[pos-1+zeropos], entype) && 
		   (pos+offset/2 >= buflen)) || 
                  ((pos < buflen-offset+1) && 
		   (buf[pos-1] == '\'') && PR07CIS_ASCII (buf[pos-1+zeropos], entype) &&
                   (buf[pos-1+offset] != '\'') && PR07CIS_ASCII (buf[pos-1+offset+zeropos], entype))) {
                pos += offset;
                stringend = 1;
              } else {
                j += offset;
                pos += offset;
              }
            }
          } while (!stringend);
	  dif = pos - j;
	  break;
	}
	case '>' : {
	  *symb = CPR_S_UNKNOWN;
	  pos += offset;
	  if (buf[pos-1] == '=' && PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
	    *symb = CPR_S_UNKNOWN;
	    pos += offset;
	  }
	  break;
	}	
	case '<' : {
	  *symb = CPR_S_UNKNOWN;
	  pos += offset;
	  if ((buf[pos-1] == '=') && PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
	    *symb = CPR_S_UNKNOWN;
	    pos += offset;
	  } else {
	    if ((buf[pos-1] == '>') && PR07CIS_ASCII (buf[pos-1+zeropos], entype)) {
	      *symb = CPR_S_UNKNOWN;
	      pos += offset;
	    }	
	  }
	  break;
	}
	case ':' : {
	  /* Caution: no check if parameter name contains
             only ASCII characters */
	  *symb = CPR_S_UNKNOWN;
	  pos += offset;
	  if ((c = buf[pos-1]) >= 'a' && c <= 'z' || c >= 'A' 
	      && c <= 'Z'
	      || c >= '0' && c <= '9' || c == '_') {	
	    *symb = CPR_S_PARAMETER_NAME;
	    do {
	      pos += offset;
	    } while ((pos <= buflen) && 
		     !(((c = buf[pos-1]) < 'a' || c > 'z') 
		       && (c < 'A' || c > 'Z') && (c < '0' || c > '9') 
		       && c != '.' && c != CPR_LEFTIND_PAR 
		       && c != CPR_RIGHTIND_PAR && c != '-' 
		       && c != '^' && c != '#' && c != '$' && c != '_'));
	  }
	  break;
	}
	case CPR_PREPARECHR : {
	  *symb = CPR_S_QUESTION_MARK;
	  pos += offset;
	  break;
	}
	case '=' : {
	  *symb = CPR_S_EQUAL;
	  pos += offset;
	  break;
	}
	case '+' : {
	  *symb = CPR_S_PLUS;
	  pos += offset;
	  break;
	}
	case '-' : {
	  *symb = CPR_S_MINUS;
	  pos += offset;
	  break;
	}
	case '/' : {
	  *symb = CPR_S_DIVIDE;
	  pos += offset;
	  break;
	}
	case ',' : {
	  *symb = CPR_S_COMMA;
	  pos += offset;
	  break;
	}
	case '(' : {
	  *symb = CPR_S_LEFTPAR;
	  pos += offset;
	  break;
	}
	case ')' : {
	  *symb = CPR_S_RIGHTPAR;
	  pos += offset;
	  break;
	}
	case CPR_PC_ENDSIGN : {
	  *symb = CPR_S_EOF;
	  break;
	}
	case ';' : {
	  *symb = CPR_S_SEMICOLON;
	  pos += offset;
	  break;
	}
	case CPR_ASTERISK : {
	  *symb = CPR_S_ASTERISK;
	  pos += offset;
	  break;
	}
	default : {
	  /* skip characters till sql delimiter has been recognized */
	  *symb = CPR_S_UNKNOWN;
	  do {
	    pos += offset;
	  } while (pos <= buflen && !strchr (p05sqldelim, buf[pos -1]));	
	  break;
	}
	}
	break;
      } 
      } /* end switch */      
    }
    if (pos > buflen + offset)
      *symb = CPR_S_EOF;
    *symlen = pos - *sympos - dif;    
    *accpos = pos;
    if (entype == sp77encodingUCS2) {
      --*symlen;
      --*accpos;
    }
  }
}

/*!
  Function:     pr05cSymbolOpen;

  see also: 
  
  Description:  Opens an given input String IString for iteration over
                all symbols
    
  Arguments:
    IString      pointer to an universal String;
    Iter   [in]  pointer to Input SymbolIterator
    

  return value:  The initalised Symboliterator.
                 Pointing before the first symbol of the given IString.
 */
tpr05_SymbolIterator *pr05cSymbolOpen(tpr05_String *IString, tpr05_SymbolIterator*Iter)
{
  if (Iter) {
    Iter->AnalysePos  = 1;
    Iter->actPos       = 0;
    Iter->SymbolPos    = 0;
    Iter->SymbolLength = 0;
    Iter->Symbol       = CPR_S_UNKNOWN;
    Iter->IString = IString;
  }
  return Iter;
}

/*!
  Function:     pr05cSymbolEnum(tpr05_SymbolIterator *Iter);

  see also: 
  
  Description:  Gets next item from a given Symbol Iterator
    
  Arguments:
    Iter   [in]  pointer to Input SymbolIterator (Opend with p05cOpenSymbol)

  return value:   tpr05_Symbol
 */
tpr05_Symbol pr05cSymbolEnum(tpr05_SymbolIterator *Iter)
{
  pr05cNextSymbol(Iter->IString->rawString, 
                  Iter->IString->cbLen,
                  Iter->AnalysePos,
                  &Iter->SymbolPos,
                  &Iter->SymbolLength,
                  &Iter->Symbol,
                  &Iter->actPos,
                  Iter->IString->encodingType);
  Iter->AnalysePos = Iter->actPos;
  return Iter->Symbol;   
}

/*!
  Function:     pr05cSymbolGetKeyword

  see also: 
  
  Description:  Gets Identifier and determines keyword from given 
                keyword table. This function is equal to pr05cGetKeyword but
                it uses an iterator instead of positioning directly.
  Arguments:
    Iter   [in]  pointer to Input SymbolIterator 
    KeywordTab    [in] pr05cKeywordTableStruct[] contains keywords in alphabetical order 
    cbNumElements [in] number of keywords in KeywordTab

  return value: tpr01_KeywordTypeEnum 
    <UL>
        <LI>KeywordNOT_FOUND_epr01 : if identifier isn`t a keyword
        <LI>keyword                : if identifier is a keyword
    </UL>
 */
tpr01_KeywordTypeEnum pr05cSymbolGetKeyword (tpr05_SymbolIterator *Iter, 
                                             pr05cKeywordTableStruct *KeywordTab,
                                             tsp00_Int4 numElementsTab)
{
  return pr05cGetKeyword (&Iter->IString->rawString[Iter->SymbolPos-1], Iter->SymbolLength, Iter->IString->encodingType, KeywordTab, numElementsTab);
}

/*!
  Function:     p05cnext2symbol

  see also: 
  
  Description:  Old next symbol routine - all calls of this function 
  should be replaced and the following code should be eliminated  

  return value:   void
 */
void p05cnext2symbol(char *buf, tsp00_Int4 buflen, tsp00_Int4 posanf, tsp00_Int4 *sympos,
tsp00_Int4 *symlen, tsp00_Int2 *symb, tsp00_Int4 *accpos, char c_minus)
{
static const char p05sqldelim [] = "\011 !#%&*()-+=;:'\",.<>/?";
   char stringend;
   tsp00_Int4 pos, i, j, dif ;
   int double_quote;
   char c;
   *symlen = 0;
   dif = 0;
   pos = posanf;
   while (pos < buflen && buf[pos-1] == ' ')
      pos++;
   if (pos > buflen)
      *symb = CPR_S_EOF;
   else
   {
      if (buf[pos-1] == 'X' && buf[pos] == '\'')
      {
	 pos++;
	 *symb = CPR_S_UNKNOWN;
      }	
      *sympos = pos;      
      switch (buf[pos -1])	 
      {
	 case 'A': {}
	 case 'B': {}
	 case 'C': {}
	 case 'D': {}
	 case 'E': {}
	 case 'F': {}
	 case 'G': {}
	 case 'H': {}
	 case 'I': {}
	 case 'J': {}
	 case 'K': {}
	 case 'L': {}
	 case 'M': {}
	 case 'N': {}
	 case 'O': {}
	 case 'P': {}
	 case 'Q': {}
	 case 'R': {}
	 case 'S': {}
	 case 'T': {}
	 case 'U': {}
	 case 'V': {}
	 case 'W': {}
	 case 'X': {}
	 case 'Y': {}
	 case 'Z': {}
	 case 'a': {}
	 case 'b': {}
	 case 'c': {}
	 case 'd': {}
	 case 'e': {}
	 case 'f': {}
	 case 'g': {}
	 case 'h': {}
	 case 'i': {}
	 case 'j': {}
	 case 'k': {}
	 case 'l': {}
	 case 'm': {}
	 case 'n': {}
	 case 'o': {}
	 case 'p': {}
	 case 'q': {}
	 case 'r': {}
	 case 's': {}
	 case 't': {}
	 case 'u': {}
	 case 'v': {}
	 case 'w': {}
	 case 'x': {}
	 case 'y': {}
	 case 'z': {}
	 case '$': {}
	 case '^': {}
	 case '#': {}
	 case '_': {	   
	    *symb = CPR_S_IDENTIFIER;
	    double_quote = 0;
	    do {
	       pos++;
	    }
	    while ((pos <= buflen) && !(((c = buf[pos + -1]) < 'a' || c > 'z') 
		    && (c < 'A' || c > 'Z')
	       && (c < '0' || c > '9') && c != '_' && c != '^' 
		       && c != '#' && c != '$'));			
	    if (pos > buflen) {
	      pos--;
	      dif = -1;
	    }
	    break;
	 }
	 case '"' : {
	    *sympos = pos + 1;
	    *symb = CPR_S_UNKNOWN;
	    pos++;
	    stringend = 0;
	    *symb = CPR_S_IDENTIFIER;
	    double_quote = 1;
	    do
	    {
	       while (pos <= buflen && buf[pos + -1] != '"')
		  pos++;
	       if (buf[pos] == '"')
		  pos += 2;
	       else
		  stringend = 1;
	    }	
	    while (!stringend);
	    pos++;
	    dif = 1;
	    break;
	 }
	 default : {
	    switch (buf[pos-1])
	    {
	       case '.' : {}
	       case '0' : {}
	       case '1' : {}
	       case '2' : {}
	       case '3' : {}
	       case '4' : {}
	       case '5' : {}
	       case '6' : {}
	       case '7' : {}
	       case '8' : {}
	       case '9' : {
		  *symb = CPR_S_UNKNOWN;
		  if (buf[pos + -1] == '.')
		  {
		     *symb = CPR_S_POINT;
		     pos++;
		     while ((c = buf[pos-1]) >= '0' && c <= '9')
		     {
			*symb = CPR_S_FIXED_POINT_LITERAL;
			pos++;
		     }	
		     if (pos > buflen)
			*symb = CPR_S_EOF;
		  }		
		  else	
		  {
		     *symb = CPR_S_UNSIGNED_INTEGER;
		     pos++;
		     while ((pos <= buflen) && (c = buf[pos-1]) >= '0' && c <= '9')
			pos++;
		     if (pos > buflen) {
		       pos--;
		       dif = -1;
		     }
		     else
		     {
			if (buf[pos + -1] == '.')
			{
			   *symb = CPR_S_FIXED_POINT_LITERAL;
			   do
			      pos++;
			   while (!((c = buf[pos-1]) < '0' || c > '9'));
			   if (pos > buflen)
			      *symb = CPR_S_EOF;
			}	
		     }
		  }
		  if ((int ) *symb != CPR_S_EOF)
		  {
		     if (buf[pos + -1] == 'E' && (int ) *symb != CPR_S_POINT)
		     {
			pos++;
			if ((c = buf[pos + -1]) == '+' || c == '-')
			{
			   pos++;
			}
			if ((c = buf[pos + -1]) >= '0' && c <= '9')
			{
			   *symb = CPR_S_FLOATING_POINT_LITERAL;
			   i = 0;
			   do
			   {
			      i++;
			      pos++;
			   }
			   while (!((c = buf[pos-1]) < '0' || c > '9'));
			   if (pos > buflen)
			      *symb = CPR_S_EOF;
			   if (i > 2)
			      *symb = CPR_S_UNKNOWN;
			}	
			else
			   *symb = CPR_S_UNKNOWN;
		     }	
		  }
		  break;
	       }
	       case '\'' : {
		  *symb = CPR_S_STRING_LITERAL;
		  stringend = 0;
		  pos++;
		  *sympos = pos;
		  j = pos;
		  do
		  {
		     while (buf[pos-1] != '\'' && pos < buflen)
		     {
			buf[j-1] = buf[pos-1];
			j++;
			pos++;
		     }
		     if (pos > buflen)
		     {
			stringend = 1;
			*symb = CPR_S_EOF;
		     }
		     else
		     {
			stringend = 1;
			pos++;
			if (buf[pos-1] == '\'')
			{
			   buf[j-1] = buf[pos-1];
			   j++;
			   pos++;
			   stringend = 0;
			}
		     }
		  }
		  while (!stringend);
		  dif = pos - j;
		  break;
	       }
	       case '>' : {
		  *symb = CPR_S_UNKNOWN;
		  pos++;
		  if (buf[pos-1] == '=')
		  {
		     *symb = CPR_S_UNKNOWN;
		     pos++;
		  }
		  break;
	       }	
	       case '<' : {
		  *symb = CPR_S_UNKNOWN;
		  pos++;
		  if (buf[pos-1] == '=')
		  {
		     *symb = CPR_S_UNKNOWN;
		     pos++;
		  }	
		  else
		  {
		     if (buf[pos-1] == '>')
		     {
			*symb = CPR_S_UNKNOWN;
			pos++;
		     }	
		  }
		  break;
	       }
	       case ':' : {
		  *symb = CPR_S_UNKNOWN;
		  pos++;
		  if ((c = buf[pos-1]) >= 'a' && c <= 'z' || c >= 'A' 
			  && c <= 'Z'
		     || c >= '0' && c <= '9' || c == '_') {	
		     *symb = CPR_S_PARAMETER_NAME;
		     do {
			pos++;
		     }
		     while ((pos <= buflen) && !(((c = buf[pos-1]) < 'a' || c > 'z') 
			      && (c < 'A' || c > 'Z') && (c < '0' || c > '9') 
			      && c != '.' && c != CPR_LEFTIND_PAR 
			      && c != CPR_RIGHTIND_PAR && c != c_minus 
			      && c != '^' && c != '#' && c != '$' && c != '_'));
		  }
		  if (pos > buflen) {
		    pos--;
		    dif = -1;
		  }
		  break;
	       }
	       case CPR_PREPARECHR : {
		  *symb = CPR_S_PARAMETER_NAME;
		  pos++;
		  break;
	       }
	       case '=' : {
		  *symb = CPR_S_EQUAL;
		  pos++;
		  break;
	       }
	       case '+' : {
		  *symb = CPR_S_PLUS;
		  pos++;
		  break;
	       }
	       case '-' : {
		  *symb = CPR_S_MINUS;
		  pos++;
		  break;
	       }
	       case '/' : {
		  *symb = CPR_S_DIVIDE;
		  pos++;
		  break;
	       }
	       case ',' : {
		  *symb = CPR_S_COMMA;
		  pos++;
		  break;
	       }
	       case '(' : {
		  *symb = CPR_S_LEFTPAR;
		  pos++;
		  break;
	       }
	       case ')' : {
		  *symb = CPR_S_RIGHTPAR;
		  pos++;
		  break;
	       }
	       case CPR_PC_ENDSIGN : {
		  *symb = CPR_S_EOF;
		  break;
	       }
	       case ';' : {
		  *symb = CPR_S_SEMICOLON;
		  pos++;
		  break;
	       }
	       case CPR_ASTERISK : {
		  *symb = CPR_S_ASTERISK;
		  pos++;
		  break;
	       }
	       default : {
		  *symb = CPR_S_UNKNOWN;
		     do {
			pos++;
		     }
                     while (pos <= buflen && !strchr (p05sqldelim, buf [pos -1]));	
		  break;
	       }
	    }
	    break;
	 } 
      } /* end switch */      
      if (pos > buflen)
	 *symb = CPR_S_EOF;
      *symlen = pos - *sympos - dif;
      *accpos = pos;
   }
}	

/*!
  Function:     pr05cGetKeyword

  see also: 
  
  Description:  Gets Identifier and determines keyword from given 
                keyword table
  
  NOTE function uses binary search, keyword table must
       be initialized and sorted in alphabetical !!! order
   
  Arguments:
    pIdentifier   [in] tpr05_CharBuffer * contains identifier
    IdentLength   [in] length of identifier  
    encodingType  [in] encoding type of identifier 
    KeywordTab    [in] pr05cKeywordTableStruct[] contains keywords in alphabetical order 
    cbNumElements [in] number of keywords in KeywordTab

  return value: tpr01_KeywordTypeEnum 
    <UL>
        <LI>KeywordNOT_FOUND_epr01 : if identifier isn`t a keyword
        <LI>keyword                : if identifier is a keyword
    </UL>
 */
tpr01_KeywordTypeEnum pr05cGetKeyword (tpr05_CharBuffer *pIdentifier, 
                                       tsp00_Int4 IdentLength, 
                                       tpr05_StringEncoding encodingType, 
                                       pr05cKeywordTableStruct *KeywordTab,
                                       tsp00_Int4 numElementsTab)
{
  M90TRACE(M90_TR_ENTRY, "pr05cGetKeyword", 0);
  {
    int begin = 1;            /*lower bound of search scope*/
    int end = numElementsTab; /*upper bound of search scope*/ 
    int pos;                  /*actual position*/
    int erg;                  /*result off keyword compare*/ 
    int encoding;             /*stores position of keyword in KeywordTab depending on */
    int compareLength;        /*stores length in bytes for memcompare*/
    tpr05_CharBuffer UpperIdentifier[2*PR01_MAX_KEYWORD_LEN]; /*pIdentifier in upper cases*/
    
    if (IdentLength > 2*PR01_MAX_KEYWORD_LEN)
    /* identifier is too large for a keyword*/
    {
         return KeywordNOT_FOUND_epr01;
    }
    
    /*set encoding type for use the rigth encoding of keyword*/
    if (encodingType == sp77encodingUCS2) 
    {
        encoding = 1;  /*take UCS2 Keywords from KeywordTab*/
    } 
    else if (encodingType == sp77encodingUCS2Swapped) 
    {
        encoding = 2;  /*take swapped UCS2 Keywords from KeywordTab*/
    }
    else 
    {
        encoding = 0; /*take Ascii Keywords from KeywordTab*/
    } 
     
    /*upper pidentifier for case insensitive compare*/
    memcpy(&UpperIdentifier[0],pIdentifier,IdentLength);
    pr05IfCom_String_toupperBuf (&UpperIdentifier[0], 
                                 IdentLength, 
                                 encodingType); 
    
    /* searching for keyword */ 
    do
    {
        pos = (begin+end)/2;
        compareLength = PR07MIN(IdentLength, KeywordTab[pos-1].keyword[encoding].length);
        erg = memcmp (&UpperIdentifier, KeywordTab[pos-1].keyword[encoding].rawString, compareLength);
        if (erg == 0)
        /*(sub)keyword matches*/
        {
            if (IdentLength == KeywordTab[pos-1].keyword[encoding].length)
            {
                return KeywordTab[pos-1].entry;
            } 
            else
            /* identifier too small or too large for current keyword */ 
            {
              if (IdentLength > KeywordTab[pos-1].keyword[encoding].length)
                erg = 1;
              else
                erg = -1;
            }    
        }
       
        if (erg < 0)
        /*continue serching in lower scope*/
        {
            end = pos-1;
        }
        else
        /*continue serching in upper scope*/
        {
            begin = pos+1;     
        }
    }while(begin <= end);  /*identifier not recognized as a keyword*/

    M90TRACE(M90_TR_EXIT, "pr05cGetKeyword", 0);
    return KeywordNOT_FOUND_epr01;
  }
}

/*!
  Function:     pr05cInitKeywordTable

  see also: 
  
  Description:  Initialize the keyword table 
  
  Arguments:   
        keywordTable [in|out] a pointer to a pr05cKeywordTableStruct
        which will be initialize.

  return value:  
 */

void pr05cInitKeywordTable (pr05cKeywordTableStruct *keywordTable, tsp00_Int4 numberElements)
{
  M90TRACE(M90_TR_ENTRY, "pr05cInitKeywordTables", 0);
  {
    tsp00_Int4 i;  
    tsp00_Uint4 out;
    /*initialize pr01StmtNameKeywordTab*/    
    for (i=0; i< numberElements; i++)
    {
        /*set length*/
        tsp00_Int4 keyLen = (tsp00_Int4) strlen(keywordTable[i].keyword[0].rawString);
        
        keywordTable[i].keyword[0].length = (tsp00_Int4) keyLen;
        keywordTable[i].keyword[1].length = (tsp00_Int4) 2*keyLen;
        keywordTable[i].keyword[2].length = (tsp00_Int4) 2*keyLen;
        
        /*set keywords*/
        sp81ASCIItoUCS2 ((tsp81_UCS2Char*)keywordTable[i].keyword[1].rawString,
                          2*keyLen, 
                          0, 
                          &out, 
                          (tsp00_Byte *)keywordTable[i].keyword[0].rawString, 
                          keyLen);

        sp81ASCIItoUCS2 ((tsp81_UCS2Char*)keywordTable[i].keyword[2].rawString, 
                          2*keyLen, 
                          1, 
                          &out, 
                          (tsp00_Byte *)keywordTable[i].keyword[0].rawString, 
                          keyLen);
    }

    M90TRACE(M90_TR_EXIT, "pr01StmtNameGetKeyword", 0);
  }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
