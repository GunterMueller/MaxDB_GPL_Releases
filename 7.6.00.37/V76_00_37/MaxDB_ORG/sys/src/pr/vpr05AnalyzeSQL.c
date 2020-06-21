/*!*************************************************************************

  module      : vpr05AnalyzeSQL.c

  -------------------------------------------------------------------------

  responsible : 

  special area:
  description : Analyzes a sql statement and stores analysis data 
  in StmtAnalysisDesc

  last changed: 2000-01-20  17:17
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
#include "gpr03m.h"
#include "hsp81.h"
#include "vpr07Macro.h"
#include "vpr05AnalyzeSQL.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  Declaration: pr05AnalyzeSQLKeywordTabInitilized
  Description: if the three keyword tables are initialized 
               then pr01StmtNameKeywordTabInitilized is cpr_is_init
 */
static pr05AnalyzeSQLKeywordTabInitilized = cpr_is_empty;


/*!
  Declaration: pr05AnalyzeSQLKeywordTab
  Description: contains keywords for analysis of first keyword of sql-statements
  
  NOTE that keywords must be in alphabetical order for binary search !!!
        
 */
static pr05cKeywordTableStruct pr05AnalyzeSQLKeywordTab [ ] = {
{ KeywordALTER_epr01,       {{"ALTER"      ,0},{"",0},{"",0}} },
{ KeywordAPPEND_epr01,      {{"APPEND"     ,0},{"",0},{"",0}} },
{ KeywordPROC_epr01,        {{"CALL"       ,0},{"",0},{"",0}} },
{ KeywordCLOSE_epr01,       {{"CLOSE"      ,0},{"",0},{"",0}} },
{ KeywordCLOSE_epr01,       {{"CLOSE"      ,0},{"",0},{"",0}} },
{ KeywordCOMMIT_epr01,      {{"COMMIT"     ,0},{"",0},{"",0}} },
{ KeywordCONNECT_epr01,     {{"CONNECT"    ,0},{"",0},{"",0}} },
{ KeywordCREATE_epr01,      {{"CREATE"     ,0},{"",0},{"",0}} },
{ KeywordPROC_epr01,        {{"DBPROC"     ,0},{"",0},{"",0}} },
{ KeywordDECLARE_epr01,     {{"DECLARE"    ,0},{"",0},{"",0}} },
{ KeywordDELETE_epr01,      {{"DELETE"     ,0},{"",0},{"",0}} },
{ KeywordDESCRIBE_epr01,    {{"DESCRIBE"   ,0},{"",0},{"",0}} }, 
{ KeywordDROP_epr01,        {{"DROP"       ,0},{"",0},{"",0}} },
{ KeywordEXIST_epr01,       {{"EXIST"      ,0},{"",0},{"",0}} },
{ KeywordFETCH_epr01,       {{"FETCH"      ,0},{"",0},{"",0}} },
{ KeywordFFETCH_epr01,      {{"FFETCH"     ,0},{"",0},{"",0}} },
{ KeywordGETVAL_epr01,      {{"GETVAL"     ,0},{"",0},{"",0}} },
{ KeywordGRANT_epr01,       {{"GRANT"      ,0},{"",0},{"",0}} },
{ KeywordINSERT_epr01,      {{"INSERT"     ,0},{"",0},{"",0}} },
{ KeywordMFETCH_epr01,      {{"MFETCH"     ,0},{"",0},{"",0}} },
{ KeywordNEXT_epr01,        {{"NEXT"       ,0},{"",0},{"",0}} },
{ KeywordOPEN_epr01,        {{"OPEN"       ,0},{"",0},{"",0}} },
{ KeywordPREPARE_epr01,     {{"PREPARE"    ,0},{"",0},{"",0}} },
{ KeywordPROC_epr01,        {{"PROC"       ,0},{"",0},{"",0}} },
{ KeywordPUTVAL_epr01,      {{"PUTVAL"     ,0},{"",0},{"",0}} },
{ KeywordREVOKE_epr01,      {{"REVOKE"     ,0},{"",0},{"",0}} },
{ KeywordROLLBACK_epr01,    {{"ROLLBACK"   ,0},{"",0},{"",0}} },
{ KeywordSELECT_epr01,      {{"SELECT"     ,0},{"",0},{"",0}} },
{ KeywordSET_epr01,         {{"SET"        ,0},{"",0},{"",0}} },
{ KeywordSHOW_epr01,        {{"SHOW"       ,0},{"",0},{"",0}} }, 
{ KeywordUPDATE_epr01,      {{"UPDATE"     ,0},{"",0},{"",0}} }
};

/*!
  Declaration: pr05cOptionKeywordTab
  Description: contains keywords for analysis of option keywords
               after the first keyword of sql-statements
  
  NOTE that kewords must be in alphabetical order for binary search 
 */
static pr05cKeywordTableStruct pr05AnalyzeSQLOptionKeywordTab [ ] = {
{ KeywordABSOLUTE_epr01,  {{"ABSOLUTE"    ,0},{"",0},{"",0}} },
{ KeywordDIRECT_epr01,    {{"DIRECT"      ,0},{"",0},{"",0}} },
{ KeywordFIRST_epr01,     {{"FIRST"       ,0},{"",0},{"",0}} },
{ KeywordINDEX_epr01,     {{"INDEX"       ,0},{"",0},{"",0}} },
{ KeywordISOLATION_epr01, {{"ISOLATION"   ,0},{"",0},{"",0}} },
{ KeywordLAST_epr01,      {{"LAST"        ,0},{"",0},{"",0}} },
{ KeywordNEXT_epr01,      {{"NEXT"        ,0},{"",0},{"",0}} },
{ KeywordPOS_epr01,       {{"POS"         ,0},{"",0},{"",0}} },
{ KeywordPREV_epr01,      {{"PREV"        ,0},{"",0},{"",0}} },
{ KeywordRELATIVE_epr01,  {{"RELATIVE"    ,0},{"",0},{"",0}} },
{ KeywordRELEASE_epr01,   {{"RELEASE"     ,0},{"",0},{"",0}} },
{ KeywordSAME_epr01,      {{"SAME"        ,0},{"",0},{"",0}} },
{ KeywordUNIQUE_epr01,    {{"UNIQUE"      ,0},{"",0},{"",0}} },
{ KeywordWORK_epr01,      {{"WORK"        ,0},{"",0},{"",0}} }
};

/*!
  Declaration: pr05AnalyzeSQLLastKeywordTab
  Description: contains keywords for analysis some special options at the end sql-statements
  
  NOTE that kewords must be in alphabetical order for binary search 
 */
static pr05cKeywordTableStruct pr05AnalyzeSQLLastKeywordTab [ ] = {
{ KeywordDESCRIPTOR_epr01, {{"DESCRIPTOR",0},{"",0},{"",0}} },
{ KeywordFOR_epr01,        {{"FOR"       ,0},{"",0},{"",0}} },
{ KeywordFROM_epr01,       {{"FROM"      ,0},{"",0},{"",0}} },
{ KeywordINTO_epr01,       {{"INTO"      ,0},{"",0},{"",0}} },
{ KeywordUSING_epr01,      {{"USING"     ,0},{"",0},{"",0}} },
};


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


/*!
  Function:     pr05cAnalyseSQLStatement

  see also: 
  
  Description:  Analyse SQL-Statements and store information about it in 
                tpr01_StmtTypeDesc. Afterwards the keywords and its 
                properties which will be recognized
          
        <pre>                StmtOption  CursorPos     Usingpos  <br>
           ----------------------------------------------    <br> 
           append    -           -               -           <br> 
           close     -           x               -           <br> 
           create    x           -               -           <br> 
           declare   -           x               -           <br> 
           delete    -           -               -           <br> 
           describe  -           x               x           <br> 
           drop      -           -               -           <br> 
           fetch     x           x               x           <br> 
           ffetch    x           x               x           <br> 
           getval    -           x               x           <br> 
           insert    -           -               -           <br> 
           mfetch    x           x               x           <br> 
           next      -           -               -           <br> 
           open      -           x               -           <br> 
           proc      -           -               -           <br> 
           putval    -           x               x           <br> 
           select    x           -               x           <br> 
           show      -           -               -           <br> 
           update    -           -               -           <br> 
           commit    x           -               -           <br> 
           rollback  x           -               -           <br> 

     </pre>
  
  Arguments:
        StmtDesc [in | out] pointer to a tpr01_StmtNameDesc

  return value: int 
    <UL>
         0 : error
         1 : processing succeeds
    </UL>
 */
int pr05cAnalyseSQLStatement (tpr05_String *SQLStatement, tpr05_StmtAnalysisDesc *aStmtAnalysisDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr05cAnalyseSQLStatement", 0);
  {
    tpr05_CharBuffer     *pStatement   = SQLStatement->rawString;   /*pointer to statement*/
    tsp00_Int4           StmtLength   = SQLStatement->cbLen;        /*length of statement*/
    tpr05_StringEncoding encodingType = SQLStatement->encodingType; /*encoding type of statement*/ 
    tsp00_Int4           pAnalysePos  = 1;                          /*position from which analyse of statement will be continued*/
    tsp00_Int4           actPos       = 0;                          /*actual pos of analyse in statement */
    tsp00_Int4           SymbolPos    = 0;                          /*position of recognized symbol in statement*/
    tsp00_Int4           SymbolLength = 0;                          /*length of recognized symbol */
    tpr00_Symbol         Symbol       = CPR_S_UNKNOWN;              /*recognized symbol*/ 

    /* initialze StmtTypeDesc */ 
    aStmtAnalysisDesc->StmtOption    = KeywordNOT_FOUND_epr01;
    aStmtAnalysisDesc->CursorPos     = 0;
    aStmtAnalysisDesc->CursorLength  = 0;
    aStmtAnalysisDesc->CursorType    = CPR_S_UNKNOWN;
    aStmtAnalysisDesc->FetchPosType  = CPR_S_UNKNOWN;
    aStmtAnalysisDesc->UsingPos      = 0;
    aStmtAnalysisDesc->Cacheable     = notcacheable_epr01;
    
    /* initialize keyword tables */
    if( pr05AnalyzeSQLKeywordTabInitilized != cpr_is_init )
    {
        pr05cInitKeywordTable (pr05AnalyzeSQLKeywordTab,       PR07NUMELEM(pr05AnalyzeSQLKeywordTab));
        pr05cInitKeywordTable (pr05AnalyzeSQLOptionKeywordTab, PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab));
        pr05cInitKeywordTable (pr05AnalyzeSQLLastKeywordTab,   PR07NUMELEM(pr05AnalyzeSQLLastKeywordTab));
        pr05AnalyzeSQLKeywordTabInitilized = cpr_is_init;
    }
 
    /* analyze fist keyword of sql-statement */
    pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
    pAnalysePos = actPos;

    /* begin analyze - ignore leading '(' */
    while ( Symbol == CPR_S_LEFTPAR )
    {
        pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
        pAnalysePos = actPos;
    }


    switch ( aStmtAnalysisDesc->StmtType = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLKeywordTab, PR07NUMELEM(pr05AnalyzeSQLKeywordTab)) )
    {
        
        case KeywordCREATE_epr01 :
        {
            /* looking for UNIQUE or INDEX*/
            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
            pAnalysePos = actPos;
            aStmtAnalysisDesc->StmtOption = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLOptionKeywordTab,PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab)) ;
            if (aStmtAnalysisDesc->StmtOption == KeywordUNIQUE_epr01)
            {
                pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                pAnalysePos = actPos;
                aStmtAnalysisDesc->StmtOption = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLOptionKeywordTab,PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab)) ;
            }  
            break;    
        }

        case KeywordSELECT_epr01 :
        {
            tpr01_KeywordTypeEnum myKeyword; /*saves the recognized keyword for following comparsion*/
            aStmtAnalysisDesc->Cacheable = cacheable_epr01;
            /* looking for StmtOption (fist, next ... )*/ 
            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
            pAnalysePos = actPos;
            aStmtAnalysisDesc->StmtOption = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLOptionKeywordTab,PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab)) ;
            
            /* recognize "into" literal */
            do
            {
                pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                pAnalysePos = actPos;
                myKeyword = pr05cGetKeyword(&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLLastKeywordTab,PR07NUMELEM(pr05AnalyzeSQLLastKeywordTab)) ;

                if ( myKeyword == KeywordINTO_epr01 )
                {
                    /*INTO was found - set using pos */
                    aStmtAnalysisDesc->UsingPos = SymbolPos;
		    aStmtAnalysisDesc->StmtOption = myKeyword;
                }    
                
            } while (   Symbol    != CPR_S_EOF
                     && myKeyword != KeywordFROM_epr01);
            break;    
        }

        case KeywordDESCRIBE_epr01 :
        case KeywordOPEN_epr01 :
        case KeywordFETCH_epr01 :
        case KeywordFFETCH_epr01 :
        case KeywordMFETCH_epr01 :
        case KeywordGETVAL_epr01 :
        case KeywordPUTVAL_epr01 :
        {
            tpr01_KeywordTypeEnum myKeyword; /*saves the recognized keyword for following comparsion*/
            /* looking for StmtOption (fist, next ... )*/ 
            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
            pAnalysePos = actPos;
            aStmtAnalysisDesc->StmtOption = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLOptionKeywordTab,PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab)) ;
 
             /* recognize fetch parameter first,pos ...*/
            if (aStmtAnalysisDesc->StmtOption != KeywordNOT_FOUND_epr01)
            /* means one of the StmtOptions (first,last ...) was detected */
            {
                switch (aStmtAnalysisDesc->StmtOption)
                /*analyze if fetchPos comes from parameter or ? */
                {
                        case KeywordABSOLUTE_epr01 :
                        case KeywordRELATIVE_epr01 : 
                        case KeywordPOS_epr01 :
                        /*analyze position parameter*/
                        {
                            int parcnt = 0;  /*counts parenthesis*/
                            /*parse over and count '(' */
                            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                            pAnalysePos = actPos;
                            while (1) 
                            {   
                                if(Symbol == CPR_S_LEFTPAR)
                                {
                                    parcnt++;
                                    pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                                    pAnalysePos = actPos;
                                }
                                else 
                                  break;
                            } 
                            aStmtAnalysisDesc->FetchPosType = Symbol;
                            if (aStmtAnalysisDesc->FetchPosType == CPR_S_PLUS 
                             || aStmtAnalysisDesc->FetchPosType == CPR_S_MINUS)
                            {
                                    /*parse over integer value*/ 
                                    pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                                    pAnalysePos = actPos;
                            }                             
                            
                            /*parse over and count ')'*/
                            while (1)
                            {
                                pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                                pAnalysePos = actPos;
                                if(Symbol == CPR_S_RIGHTPAR)
                                {
                                    parcnt--;
                                }
                                else 
                                {
                                  break;
                                }
                            }
                            /*if (parcnt != 0)
                            {
                                syntax error   
                            }*/
                            break;
                        }   
                }/*switch pos,abs,rel*/
                pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                pAnalysePos = actPos;
            }
            /*get current keyword from keyword tab "pr05AnalyzeSQLLastKeywordTab"*/
            myKeyword = pr05cGetKeyword(&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLLastKeywordTab,PR07NUMELEM(pr05AnalyzeSQLLastKeywordTab));
            if( (   Symbol == CPR_S_IDENTIFIER
                 || Symbol == CPR_S_PARAMETER_NAME
                 || Symbol == CPR_S_STRING_LITERAL
                 || Symbol == CPR_S_CSTRING_LITERAL 
                )
                 && myKeyword != KeywordINTO_epr01
                 && myKeyword != KeywordUSING_epr01 
              )
            /*asuming first identifier, paramter which was found, will be the cursorname*/
            {
                aStmtAnalysisDesc->CursorPos    = SymbolPos;
                aStmtAnalysisDesc->CursorLength = SymbolLength;
                aStmtAnalysisDesc->CursorType   = Symbol; 
            }           
            else {
                aStmtAnalysisDesc->CursorPos    = SymbolPos-1;
                aStmtAnalysisDesc->CursorLength = 0;
            }
            
            /* recognize using or into descriptor */
            do
            {
                if ( myKeyword == KeywordUSING_epr01 )
                /* using claus found */ 
                {
                    aStmtAnalysisDesc->UsingPos = SymbolPos;
                    break;
                } /* using */
                else if ( myKeyword == KeywordINTO_epr01 )
                {
                    int into_pos = SymbolPos;
                    /*searching for keyword descriptor*/
                    pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                    pAnalysePos = actPos;
                    if ( KeywordDESCRIPTOR_epr01 == pr05cGetKeyword(&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLLastKeywordTab,PR07NUMELEM(pr05AnalyzeSQLLastKeywordTab)) )
                    {
                        aStmtAnalysisDesc->UsingPos = into_pos;
                        break; 
                    }
                }    
                pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
                pAnalysePos = actPos;
                myKeyword = pr05cGetKeyword(&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLLastKeywordTab,PR07NUMELEM(pr05AnalyzeSQLLastKeywordTab)) ;
                
            } while (Symbol != CPR_S_EOF);
            
            break;    
        }
        
        case KeywordCLOSE_epr01 :
        case KeywordDECLARE_epr01 :
        {
            /* recognize cursor name */ 
            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
            pAnalysePos = actPos;
            
            if(   Symbol == CPR_S_IDENTIFIER
               || Symbol == CPR_S_PARAMETER_NAME
               || Symbol == CPR_S_STRING_LITERAL
               || Symbol == CPR_S_CSTRING_LITERAL )
            {
                aStmtAnalysisDesc->CursorPos    = SymbolPos;
                aStmtAnalysisDesc->CursorLength = SymbolLength;
                aStmtAnalysisDesc->CursorType   = Symbol;
            }           
            break;    
        }
        case KeywordALTER_epr01 :
        case KeywordGRANT_epr01 :
        case KeywordREVOKE_epr01:
        case KeywordEXIST_epr01 :
        {
            aStmtAnalysisDesc->StmtType = KeywordDDL_epr01;            
            break;         
        }
        case KeywordSET_epr01 :
        {
            /* looking for ISOLATION*/
            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
            pAnalysePos = actPos;
            aStmtAnalysisDesc->StmtOption = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLOptionKeywordTab,PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab)) ;
            if (aStmtAnalysisDesc->StmtOption == KeywordISOLATION_epr01)
            {
              aStmtAnalysisDesc->StmtType = KeywordSETISOLEVEL_epr01;            
            }  
            break;    
        }
        case KeywordCOMMIT_epr01 :
        case KeywordROLLBACK_epr01 :
        {
          /* looking for WORK RELEASE */
          pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
          pAnalysePos = actPos;
          aStmtAnalysisDesc->StmtOption = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLOptionKeywordTab,PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab)) ;
          if (aStmtAnalysisDesc->StmtOption == KeywordWORK_epr01) {
              pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, encodingType);
              pAnalysePos = actPos;
              aStmtAnalysisDesc->StmtOption = pr05cGetKeyword (&pStatement[SymbolPos-1], SymbolLength, encodingType, pr05AnalyzeSQLOptionKeywordTab,PR07NUMELEM(pr05AnalyzeSQLOptionKeywordTab)) ;
            }  
          break;    
        }
        case KeywordDELETE_epr01 :
        case KeywordINSERT_epr01 :
        case KeywordPROC_epr01 :
        case KeywordSHOW_epr01 :
        case KeywordUPDATE_epr01 :
        {
          aStmtAnalysisDesc->Cacheable = cacheable_epr01;
          break;    
        }
    } /*end switch*/
    M90TRACE(M90_TR_EXIT, "pr05cAnalyseSQLStatement", 0);
    return 1;
  }
}

/*!
  Function:     pr05AnalyseSQLKeywordNewIterator

  see also: 
  
  Description:  allocates and opens the given universal string for 
                parsing over all keywords.
                The keywords are given by an costant Array 
                pr05AnalyzeSQLKeywordTab.
  
  Arguments:
    SQLStatement     [in]  pointer to a universal string containing a SQL Statemnt
    
  return value:  The initalised KeywordIterator.
                 Pointing before the first symbol of the given IString.
 */
tpr05_KeywordIterator *pr05AnalyseSQLKeywordNewIterator(tpr05_String *SQLStatement)
{ 
  tpr05_KeywordIterator *Iter;
  Iter = pr03mAllocat(sizeof(tpr05_KeywordIterator), "KeywordIterator");
  if (Iter)
    pr05AnalyseSQLKeywordInitIterator(SQLStatement, Iter);
  return Iter;
}

/*!
  Function:     pr05AnalyseSQLKeywordDeleteIterator

  see also: 
  
  Description:  destroys an KeywordIterator. And gives the memory allocated 
                by pr05AnalyseSQLKeywordNewIterator free.
                 

  Arguments:
    Iter           [in]  pointer to Input KeywordIterator 
    
  return value: none 
 */
void pr05AnalyseSQLKeywordDeleteIterator(tpr05_KeywordIterator *Iter)
{ 
  if (Iter)
    pr03mFree(Iter, "KeywordIterator");
}

/*!
  Function:     pr05AnalyseSQLKeywordInitIterator

  see also: 
  
  Description:  Opens a KeywordIterator for parsing ofer the given unversal
                String. After opening the iterator points before the 
                first symbol of the given String 
  Arguments:
    Iter           [in]  pointer to Input KeywordIterator 
    SQLStatement   [in]  pointer to an universal String 
    
  return value: pointer to the opened iterator or NULL if failed.
 */
tpr05_KeywordIterator *pr05AnalyseSQLKeywordInitIterator(tpr05_String *SQLStatement, tpr05_KeywordIterator *Iter)
{ 
  return pr05cSymbolOpen(SQLStatement, Iter);
}

/*!
  Function:     pr05AnalyseSQLKeywordEnum

  see also: 
  
  Description:  Enumerates over all keyword in the given Iterator/String. 
                The Iterator has to be opened by calling After opening the iterator points before the 
                first symbol of the given String pr05AnalyseSQLKeywordInitIterator 
                or pr05AnalyseSQLKeywordNewIterator.
  Arguments:
    Iter           [in]  pointer to Input SymbolIterator 
    
  return value: tpr01_KeywordTypeEnum 
    <UL>
        <LI>keyword                : if identifier is a keyword
        <LI>KeywordNOT_FOUND_epr01 : if the iterator passed the end of the string. 
    </UL>
 */
tpr05_Symbol pr05AnalyseSQLKeywordEnum(tpr05_KeywordIterator *Iter)
{
  tpr05_Symbol Symbol = pr05cSymbolEnum(Iter);
  while (Symbol != CPR_S_EOF) {
    tpr01_KeywordTypeEnum StmtType = pr05cSymbolGetKeyword(Iter, pr05AnalyzeSQLKeywordTab, PR07NUMELEM(pr05AnalyzeSQLKeywordTab));
    if (StmtType != KeywordNOT_FOUND_epr01)
      return StmtType;
  }
  return KeywordNOT_FOUND_epr01;
}

const char *StatementTypeInAscii(tpr01_KeywordTypeEnum aKeyword)
{
  switch (aKeyword)
  {
        case (  KeywordABSOLUTE_epr01):
                return "ABSOLUTE";
        case (  KeywordADABAS_epr01):
                return "ADABAS";
        case (  KeywordALTER_epr01):
                return "ALTER";
        case (  KeywordANSI_epr01):
                return "ANSI";
        case (  KeywordANSIORA_epr01):
                return "ANSIORA";
        case (  KeywordAPPEND_epr01):
                return "APPEND";
        case (  KeywordAT_epr01):
                return "AT";
        case (  KeywordCACHELIMIT_epr01):
                return "CACHELIMIT";
        case (  KeywordCHARACTER_epr01):
                return "CHARACTER";
        case (  KeywordCLOSE_epr01):
                return "CLOSE";
        case (  KeywordCONNECT_epr01):
                return "CONNECT";
        case (  KeywordCONNECTC_epr01):
                return "CONNECTC";
        case (  KeywordCREATE_epr01):
                return "CREATE";
        case (  KeywordDB2_epr01):
                return "DB2";
        case (  KeywordDDL_epr01):
                return "DDL Command";
        case (  KeywordDECLARE_epr01):
                return "DECLARE";
        case (  KeywordDELETE_epr01):
                return "DELETE";
        case (  KeywordDESCRIBE_epr01):
                return "DESCRIBE";
        case (  KeywordDESCRIPTOR_epr01):
                return "DECRIPTOR";
        case (  KeywordDIRECT_epr01):
                return "DIRECT";
        case (  KeywordDISTINCT_epr01):
                return "DISTINCT";
        case (  KeywordDROP_epr01):
                return "DROP";
        case (  KeywordEXIST_epr01):
                return "EXIST";
        case (  KeywordEXPLICIT_epr01):
                return "EXPLICIT";
        case (  KeywordFETCH_epr01):
                return "FETCH";
        case (  KeywordFFETCH_epr01):
                return "FFETCH";
        case (  KeywordFIRST_epr01):
                return "FIRST";
        case (  KeywordFOR_epr01):
                return "FOR";
        case (  KeywordGETVAL_epr01):
                return "GETVAL";
        case (  KeywordGRANT_epr01):
                return "GRANT";
        case (  KeywordIDENTIFIED_epr01):
                return "IDENTIFIED";
        case (  KeywordIMPLICIT_epr01):
                return "IMPLICIT";
        case (  KeywordINDEX_epr01):
                return "INDEX";
        case (  KeywordINSERT_epr01):
                return "INSERT";
        case (  KeywordINTERNAL_epr01):
                return "INTERNAL";
        case (  KeywordINTO_epr01):
                return "INTO";
        case (  KeywordISOLATION_epr01):
                return "ISOLATION";
        case (  KeywordLAST_epr01):
                return "LAST";
        case (  KeywordLEVEL_epr01):
                return "LEVEL";
        case (  KeywordLOCK_epr01):
                return "LOCK";
        case (  KeywordMFETCH_epr01):
                return "MFETCH";
        case (  KeywordNEXT_epr01):
                return "NEXT";
        case (  KeywordNOLOG_epr01):
                return "NOLOG";
        case (  KeywordNORMAL_epr01):
                return "NORMAL";
        case (  KeywordOPEN_epr01):
                return "OPEN";
        case (  KeywordORACLE_epr01):
                return "ORACLE";
        case (  KeywordPOS_epr01):
                return "POS";
        case (  KeywordPREPARE_epr01):
                return "PREPARE";
        case (  KeywordPREV_epr01):
                return "PREV";
        case (  KeywordPROC_epr01):
                return "PROC";
        case (  KeywordPUTVAL_epr01):
                return "PUTVAL";
        case (  KeywordRELATIVE_epr01):
                return "RELATIVE";
        case (  KeywordREVOKE_epr01):
                return "REVOKE";
        case (  KeywordSAME_epr01):
                return "SAME";
        case (  KeywordSAPR3_epr01):
                return "SAPR3";
        case (  KeywordSELECT_epr01):
                return "SELECT";
        case (  KeywordSET_epr01):
                return "SET";
        case (  KeywordSHOW_epr01):
                return "SHOW";
        case (  KeywordSQLDB_epr01):
                return "SQLDB";
        case (  KeywordSQLMODE_epr01):
                return "SQLMODE";
        case (  KeywordTIMEOUT_epr01):
                return "TIMEOUT";
        case (  KeywordUNIQUE_epr01):
                return "UNIQUE";
        case (  KeywordUPDATE_epr01):
                return "UPDATE";
        case (  KeywordUSING_epr01):
                return "USING";
       case (  KeywordNOT_FOUND_epr01):
                return "keyword not found";
       
        default:
          return "not in keyword list";
   } 
} 

/* test programm for AnalyzeSQL 

void main (int argc, char** argv)
{ 
 char *stmtarray[]=
 { "S\0E\0L\0E\0C\0T\0 \0*\0 \0F\0R\0O\0M\0 \0D\0U\0A\0L\0;\0",
   "F\0E\0T\0C\0H\0 \0F\0I\0R\0S\0T\0 \0:\0r\0t\0u\0 \0I\0N\0T\0O\0 \0:\0N\0a\0s\0e\0 \0U\0S\0I\0N\0G\0 \0b\0l\0a\0e\0h\0"
 };
 int lengtharray[]=
 {
   57,
   121
 };
 
 int i;              
 tpr05_StmtAnalysisDesc  myDesc;
 tpr05_String *myString = pr05IfCom_String_NewDynString(150, sp77encodingUCS2Swapped);

 strcpy(myString->rawString,argv[1]);
 myString->cbLen = strlen(myString->rawString);
 myString->encodingType = sp77encodingAscii;
 pr05cAnalyseSQLStatement (myString,&myDesc);
 
 printf ("                        1         2         3         4         5         6\n");
 printf ("               123456789012345678901234567890123456789012345678901234567890 \n");
 printf ("SQL-Statement: %s \n",myString->rawString);
 printf ("SQL-Statement: %s \n",myString->rawString);
 printf ("Analysis \n=========\n");
 printf ("StmtTypeDesc.StmtType    : %s \n",StatementTypeInAscii(myDesc.StmtType));
 printf ("StmtTypeDesc.StmtOption  : %s \n",StatementTypeInAscii(myDesc.StmtOption));
 printf ("StmtTypeDesc.CursorPos   : %d\n",myDesc.CursorPos);
 printf ("StmtTypeDesc.CursorLen   : %d\n",myDesc.CursorLength);
 printf ("StmtTypeDesc.CursorTyp   : %d\n",myDesc.CursorType);
 printf ("StmtTypeDesc.FetchPosTyp : %d\n",myDesc.FetchPosType);
 printf ("StmtTypeDesc.UsingPos    : %d\n",myDesc.UsingPos);

 myString->encodingType = sp77encodingUCS2Swapped;
 
 for (i=0;i<PR07NUMELEM(stmtarray); i++)
 {
        memset(myString->rawString,0,lengtharray[i]);
        memcpy(myString->rawString,stmtarray[i],lengtharray[i]);
        myString->cbLen = lengtharray[i];
        pr05cAnalyseSQLStatement (myString,&myDesc);
 
        printf ("SQL-Statement: %s \n",myString->rawString);
        printf ("Analysis \n=========\n");
        printf ("StmtTypeDesc.StmtType    : %s \n",StatementTypeInAscii(myDesc.StmtType));
        printf ("StmtTypeDesc.StmtOption  : %s \n",StatementTypeInAscii(myDesc.StmtOption));
        printf ("StmtTypeDesc.CursorPos   : %d\n",myDesc.CursorPos);
        printf ("StmtTypeDesc.CursorLen   : %d\n",myDesc.CursorLength);
        printf ("StmtTypeDesc.CursorTyp   : %d\n",myDesc.CursorType);
        printf ("StmtTypeDesc.FetchPosTyp : %d\n",myDesc.FetchPosType);
        printf ("StmtTypeDesc.UsingPos    : %d\n",myDesc.UsingPos);
  }
}
*/
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

