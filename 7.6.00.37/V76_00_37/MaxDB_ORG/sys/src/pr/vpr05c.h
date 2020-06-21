/*!**************************************************************************

  module      : vpr05c.h

  -------------------------------------------------------------------------

  responsible : 

  special area:
  description : Contains functions for analysis sql statements

  last changed: 2000-08-16  17:17
  see also    :

  change history : 
        
        D025086 08-11-2000 New functions pr05cOpenSymbol and pr05cEnumSymbol

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

#ifndef VPR05C_H
#define VPR05C_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gpr01.h" 

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define PR01_MAX_KEYWORD_LEN    (sizeof(tsp00_Identifier))  /* tsp00_Identifier*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  Declaration: pr05cKeywordStruct
  Description: structure of keyword
*/
typedef struct pr05cKeywordStruct {
  char rawString [PR01_MAX_KEYWORD_LEN];  
  tsp00_Int4 length;
} pr05cKeywordStruct;


/*!
  Declaration: pr05cKeywordTableStruct
  Description: structure of keyword table
  
  NOTE that keyword is a array of three KeywordStructs, 
  keyword[0] contains keyword encoded in ASCII,
  keyword[1] contains keyword encoded in UCS2,
  keyword[2] contains keyword encoded in UCS2Swapped.
 */
typedef struct pr05cKeywordTableStruct {
  tpr01_KeywordTypeEnum entry;
  pr05cKeywordStruct keyword [3];  /**/
} pr05cKeywordTableStruct;

typedef tsp00_Int2 tpr05_Symbol;

typedef struct tpr05_SymbolIterator {
  tpr05_String *IString;   /* pointer to universal String */
  tsp00_Int4 AnalysePos;   /* position from which analyse of statement will be continued */
  tsp00_Int4 SymbolPos;    /* position of recognized symbol in statement */
  tsp00_Int4 SymbolLength; /*length of recognized symbol */
  tsp00_Int2 Symbol;       /*recognized symbol*/
  tsp00_Int4 actPos;       /* actual pos of analyse in statement */
} tpr05_SymbolIterator;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

tsp00_Int4 pr05cChToInt4 (char *buf, tsp00_Int4 posanf, tsp00_Int4 len,
                          tpr05_StringEncoding entype);

void pr05cNextSymbol (char *buf, tsp00_Int4 buflen, tsp00_Int4 posanf, tsp00_Int4 *sympos,
                      tsp00_Int4 *symlen, tsp00_Int2 *symb, tsp00_Int4 *accpos, 
                      tpr05_StringEncoding entype);

void p05cnext2symbol(char *buf, tsp00_Int4 buflen, tsp00_Int4 posanf, 
                     tsp00_Int4 *sympos, tsp00_Int4 *symlen, tsp00_Int2 *symb, 
                     tsp00_Int4 *accpos, char c_minus);

tpr01_KeywordTypeEnum pr05cGetKeyword (tpr05_CharBuffer *pIdentifier, 
                                       tsp00_Int4 IdentLength, 
                                       tpr05_StringEncoding encodingType, 
                                       pr05cKeywordTableStruct *KeywordTab,
                                       tsp00_Int4 numElementsTab);

void pr05cInitKeywordTable (pr05cKeywordTableStruct *keywordTable, tsp00_Int4 numberElements);

tpr05_SymbolIterator *pr05cSymbolOpen(tpr05_String *IString, tpr05_SymbolIterator*Iter);

tpr05_Symbol pr05cSymbolEnum(tpr05_SymbolIterator *Iter);

tpr01_KeywordTypeEnum pr05cSymbolGetKeyword (tpr05_SymbolIterator *Iter, 
                                             pr05cKeywordTableStruct *KeywordTab, 
                                             tsp00_Int4 numElementsTab);

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
#endif  /* VPR05C_H */

