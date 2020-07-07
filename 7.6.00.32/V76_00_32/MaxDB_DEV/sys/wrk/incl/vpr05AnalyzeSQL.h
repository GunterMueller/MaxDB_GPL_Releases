/*!**************************************************************************

  module      : vpr05AnalyzeSQL.h

  -------------------------------------------------------------------------

  responsible :

  special area:
  description : Analyzes a sql statement and stores analysis data
  in StmtAnalysisDesc


  last changed: 2000-08-18  17:17
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

#ifndef VPR05_ANALYZESQL_H
#define VPR05_ANALYZESQL_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gpr01.h"
#include "vpr05c.h"

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
  Declaration: tpr05_StmtAnalysisDesc
  Description: structure for describing a SQL-Statements

  NOTE: CursorPos, UsingPos  are 0, if not found
 */
typedef struct tpr05_StmtAnalysisDesc {
  tpr01_KeywordTypeEnum StmtType;     /*first keyword of SQL-Statement (SELECT, UPDATE ...)*/
  tpr01_KeywordTypeEnum StmtOption;   /*option for a SQL-Statement (NEXT, FIRST ...)*/
  tsp00_Int4 CursorPos;               /*position of hostvariable which contains cursorname*/
  tsp00_Int4 CursorLength;            /*length of hostvariable which contains cursorname*/
  enum tpr_gsymboltypes CursorType;   /*type of symbol where cursor come from (paramter, identifier...)*/
  enum tpr_gsymboltypes FetchPosType; /*type of symbol (paramter, identifier...) which contains parameter for fetch [pos|relative|absolute...]*/
  tsp00_Int4 UsingPos;                /*position of using-clause in SQL-Statement*/
  tpr01_Cacheable Cacheable;           /*flags if parseinfos are cacheable or not*/
} tpr05_StmtAnalysisDesc;

typedef tpr05_SymbolIterator tpr05_KeywordIterator;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

int pr05cAnalyseSQLStatement (tpr05_String *SQLStatement, tpr05_StmtAnalysisDesc *aStmtAnalysisDesc);
tpr05_KeywordIterator *pr05AnalyseSQLKeywordNewIterator(tpr05_String *SQLStatement);
void pr05AnalyseSQLKeywordDeleteIterator(tpr05_KeywordIterator *Iter);
tpr05_KeywordIterator *pr05AnalyseSQLKeywordInitIterator(tpr05_String *SQLStatement, tpr05_KeywordIterator *Iter);
tpr05_Symbol pr05AnalyseSQLKeywordEnum(tpr05_KeywordIterator *Iter);

/*======================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
#endif /* VPR05_ANALYZESQL_H */
