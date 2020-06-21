/*!================================================================
 module:    vpr08c.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | SQL Runtime System
 
 description: SQL Runtime System
 
 see:  
 



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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr03.h"
#include "gpr04.h"
#include "gpr05.h"
#include "gpr08.h"
#include "vpr05c.h"
#include "vpr01SQL.h"
#include "vpr01Con.h"
#include "vpr02Con.h"
#include "vpr01Cursor.h"
#include "vpr03Part.h"
#include "vpr07Macro.h"
#include "vpr01Trace.h"
#include "vpr08c.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  EXPORTED FUNCTIONS                                              *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function:     pr08SetFetchOptions

  see also: 

  Description:  Sets the different Parameters depending on fetchspec that
                are required for processing fetch 
    
  NOTE that function replace the old searchfetch and makefetchinto functions

  Arguments:
    encoding [in] SQL descriptor

  return value: void
  
 */    
void pr08SetFetchOptions(tpr01_SQLDesc *SQLDesc)
{
    sqlcatype *sqlca = SQLDesc->SQL->GetSqlca(SQLDesc);
    sqlcxatype *sqlcxap = sqlca->sqlcxap;
    sqlcxap->xacfetspec = CPR_FE_FETCH;
                 
    if (sqlcxap)
    {
      sqlcxap->xacfetspec = pr08MapFetchSpec(&SQLDesc->StmtNameDesc->StmtTypeDesc);
    } /*end if sqlcxap */        
}


/*!
  Function:     pr08MapFetchSpec
  
  see also: 
  
  Description:  Gets the fetchspec e.g the fetchtype of a given StmtTypeDesc
  
  Arguments: StmtTypeDesc [in] StmtType Descriptor returned from Analyse

  return value: The FetchSpec 
*/    

tpr00_FetchSpecEnum pr08MapFetchSpec(tpr05_StmtAnalysisDesc *StmtTypeDesc)
{
  tpr00_FetchSpecEnum FetchSpec = CPR_FE_FETCH;
  
  switch (StmtTypeDesc->StmtOption) {
  case (KeywordFIRST_epr01) : {
    FetchSpec = CPR_FE_FIRST;
    break;
  }            
  case (KeywordLAST_epr01) : {
    FetchSpec = CPR_FE_LAST;
    break;
  }                        	  
  case (KeywordNEXT_epr01) : {
    FetchSpec = CPR_FE_NEXT;
    break;
  }
  case (KeywordSAME_epr01) : {
    FetchSpec = CPR_FE_SAME;
    break;
  }
  case (KeywordRELATIVE_epr01) :
  case (KeywordABSOLUTE_epr01) : {
    FetchSpec = CPR_FE_POSC;
    /*if fetchpos given by parameter or ? */
    if (StmtTypeDesc->FetchPosType == CPR_S_PARAMETER_NAME) {
      /*fetchpos given by parameter or ? */
      FetchSpec = CPR_FE_POSV;
              }
    break;
  }
  case(KeywordPOS_epr01)   :  {
    FetchSpec = CPR_FE_POSC;
    if (StmtTypeDesc->FetchPosType == CPR_S_PARAMETER_NAME) {
      /*fetchpos given by parameter or ? */
      FetchSpec = CPR_FE_POSV;
    }
    break;
  }
  case (KeywordPREV_epr01) : {
    FetchSpec = CPR_FE_PREV;
    break; 
  }
  default : {
    break;
  }
  } /*end switch*/
  return FetchSpec;
}

/*!
  Function:     pr08cPutCursorNametoPart

  see also: 

  Description:  Puts the cursorname to a part of the order packet
  
  NOTE that offset and free_part_space will be updated. Offset contains 
  the position in part after last put. Free_part_space contains the current
  free space in part.
    
  Arguments:
    Cursorname      [in]     pointer to a tpr05_String that contains the name of cursor
    part_Ptr        [in]     pointer to the part
    offset          [in|out] start position for writing in part, will be updated
    free_part_space [in|out] max free space in part
    PacketEncoding  [in]     encoding type of order packet 

  return value:  0   : processing successed
                -777 : error can not convert from unicode
                -808 : Not yet implemented
                -706 : SQL statement too long (buffer overflow)
 */
tpr_runtime_errors_epr00 pr08cPutCursorNametoPart(tpr05_String  *CursorName, 
                                            tsp1_part     *part_Ptr, 
                                            tsp00_Int4    *offset, 
                                            tsp00_Int4    *free_part_space,
                                            tpr05_StringEncoding PacketEncoding)
{
     tpr_runtime_errors_epr00 ret = cpr_p_ok;
     
     if (CursorName) 
     {
        char QuoteSymBeg = '\"'; 
        char *QuoteSymEnd = "\" "; 
                        
        /*put cursor name to part and enclose them in "..." */
        ret = pr03PartConverttoPart(part_Ptr, 
                                    offset,
                                    free_part_space,
                                    PacketEncoding,
                                    &QuoteSymBeg,
                                    1,
                                    sp77encodingAscii);
        if (ret != cpr_p_ok)
        /*error occured*/                            
        {
            return ret;            
        }
                    
        ret = pr03PartConverttoPart(part_Ptr, 
                                    offset,
                                    free_part_space,
                                    PacketEncoding,
                                    CursorName->rawString,
                                    PR07MIN(CursorName->cbLen,sizeof(tsp00_KnlIdentifierc)),
                                    CursorName->encodingType);
        if (ret != cpr_p_ok)
        /*error occured*/                            
        {
            return ret;        
        }                            
        
        ret = pr03PartConverttoPart(part_Ptr, 
                                    offset,
                                    free_part_space,
                                    PacketEncoding,
                                    QuoteSymEnd,
                                    2,
                                    sp77encodingAscii);
        if (ret != cpr_p_ok)
        /*error occured*/                            
        {
           return ret;         
        }                            
                                                                   
        /* früher wurde an dieser Stelle der Ergname gemerkt                                 
              strcpy((char*)ergn, CursorDesc->szCursorName); */	      
        return ret;
    }
    return ret;
}


/*!
  Function:     pr08cExpandParameterstoPart

  see also: 

  Description:  Analyze Statement, put it to part and expand parameters 
                if they occurred.
    
  Arguments:
    SQLDesc         [in]     pointer to a SQL descriptor
    part_Ptr        [in]     pointer to the part
    offset          [in|out] start position for writing in part, will be updated
    free_part_space [in|out] max free space in part
    PacketEncoding  [in]     encoding type of order packet 

  return value:  0   : processing successed
                -777 : error can not convert from unicode
                -808 : Not yet implemented
                -706 : SQL statement too long (buffer overflow)
                -819 : parameter list not allowed
 */
tpr_runtime_errors_epr00 pr08cExpandParameterstoPart( tpr01_SQLDesc *SQLDesc,
                                                      tsp00_Int4    StmtOffset, 
                                                      tsp1_part     *part_Ptr, 
                                                      tsp00_Int4    *PacketOffset, 
                                                      tsp00_Int4    *free_part_space,
                                                      tpr05_StringEncoding PacketEncoding)
{
    tpr_runtime_errors_epr00        erg = 0; /*return value*/
    tpr05_CharBuffer    *pStatement       = &SQLDesc->StmtNameDesc->SQLStatement->rawString[StmtOffset];  /*pointer to statement*/
    tsp00_Int4           StmtLength       = SQLDesc->StmtNameDesc->SQLStatement->cbLen - StmtOffset;   /*length of statement*/
    tpr05_StringEncoding StmtEncodingType = SQLDesc->StmtNameDesc->SQLStatement->encodingType; /*encoding type of statement*/ 
            
    tsp00_Int4        pAnalysePos  = 1;                          /*position from which analyse of statement will be continued*/
    tsp00_Int4        actPos       = 0;                          /*actual pos of analyse in statement */
    tsp00_Int4        SymbolPos    = 0;                          /*position of recognized symbol in statement*/
    tsp00_Int4        SymbolLength = 0;                          /*length of recognized symbol */
    tpr00_Symbol      Symbol       = CPR_S_UNKNOWN;              /*recognized symbol*/ 

    tsp00_Int4    numExpand;            /*count for number of parameters to expand*/
    tsp00_Int4    begBuf=0;             /*current lower bound of statement for expand to part*/
    tsp00_Int4    endBuf=StmtLength>0 ? StmtLength : 0;   /*current upper of statement for expand to part*/
    tsp00_Int4    paramcnt = 0;         /*number of parameter founded in pa area*/
    tsp00_Int4    cbExpandParam = 1;    /*counter for number of params expanded starting with number none*/
    sqlparpointer sqlpa;               /*pointer to sqlpa entry which contains data about current hostvariable*/           
    
    /*if nothing to analyze return*/
    if (StmtLength <= 0)
          return erg;
    
    if ( SQLDesc->ka->kapacount > 0)  
    /*  >= 0 means static  SQL 
        ==-1 means dynamic SQL 
    */
    {
        if  (SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordPREPARE_epr01)
        /*if prepare do nothing*/
        {
            /*put the sql-statement to orderpacket */
            erg = pr03PartConverttoPart(part_Ptr, 
                                        PacketOffset,
                                        free_part_space,
                                        PacketEncoding,
                                        &pStatement[begBuf],
                                        (endBuf-begBuf),
                                        StmtEncodingType);
            return erg;
        }

        pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, StmtEncodingType);
        pAnalysePos = actPos;
        
        while (Symbol != CPR_S_EOF) 
        /*for all parameters in statement */
        {
            tsp00_Int4  i;
            BOOLEAN     bIndikator = FALSE;      /*flag for indikator*/
            BOOLEAN     ParelFound = FALSE;      /*a real parameter was found (not parlo, parst)*/
            tpr05_CharBuffer symb_paramter[14];  /*contains parameter string (:P_1 ...)*/
            tsp00_Int4  cbLenSymb_parameter = 0; /*current length of sym_parameter*/
            /*find next parameter*/
            while ( Symbol != CPR_S_PARAMETER_NAME )
            {
                if ( Symbol == CPR_S_EOF )
                /*analyze finish*/
                {
                    /*put the sql-statement to orderpacket */
                    erg = pr03PartConverttoPart(part_Ptr, 
                                                PacketOffset,
                                                free_part_space,
                                                PacketEncoding,
                                                &pStatement[begBuf],
                                                (endBuf-begBuf),
                                                StmtEncodingType);
                    return erg;
                }
                pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, StmtEncodingType);
                /*set upper bound before parameter pos*/
                endBuf = pAnalysePos-1;
                pAnalysePos = actPos;
            }                                     

            numExpand  = 1; /*default number of paramter to expand is one*/
            ParelFound = FALSE;
            
            do
            /* The loop determines the type of hostvariable. In case of 
               type struct the number of components (members) will be
               taken as count for expand parameters. Note that the loop 
               parameter will be ignored. Note also that the sqlpa area 
               contains two entries for a struct, one with kind sqlparst
               and one with kind sqlparel. Thats why, only if a real 
               parameter founded, the loop will exit.                  */ 
            {            
                sqlpa = (sqlparpointer) &SQLDesc->sqlxa->sqlpap[(int)SQLDesc->ka->kapaindex+paramcnt-1];                    
                switch (sqlpa->el.pakind)
                /*kapaindex point to the first parameter in sqlpa area for the
                  current statement*/
                {            
                    case SQLPARLO:
                    /*loop parameter (0)*/
                    {
                        paramcnt++;
                        /*only count*/
                        numExpand = 0;
                        break;
                    }
                    case SQLPARST:
                    /*parameter is struct (1)*/
                    {
                        paramcnt++;
                        /*compute number of struct members*/
                        numExpand = sqlpa->st.paelcnt;
                        break;
                    }
                    case SQLPAREL:
                    /*parameter (2)*/
                    {
                        paramcnt++;
                        ParelFound = TRUE;
                        break;
                    }
                    default:
                    {
                        ParelFound = TRUE;
                        break;
                    }
                }/*end switch*/        
            } 
            while (! ParelFound);    
            
            /*put the parsed part of sql-statement to orderpacket (sqlcode between parameters)*/
            erg = pr03PartConverttoPart(part_Ptr, 
                                        PacketOffset,
                                        free_part_space,
                                        PacketEncoding,
                                        &pStatement[begBuf],
                                        (endBuf-begBuf),
                                        StmtEncodingType);
            if (erg != cpr_p_ok)
            /*error occured*/                            
            {
                return erg;        
            }                            

            /*get next symbol - may be it`s a indikator */
            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, StmtEncodingType);
            /*set lower bound after recognized parameter*/
            begBuf = pAnalysePos-1;
            pAnalysePos = actPos;            
            
            if (Symbol == CPR_S_PARAMETER_NAME)
            /*indikator found*/
            {
                bIndikator = TRUE;
                /*set lower bound after recognized indikator*/
                begBuf = pAnalysePos-1;
            }             

            /*if (paramcnt>kapacnt)
            {
                return cpr_paramlist_too_short;   
            }*/
            

            /*expand first paramter*/
            if (bIndikator)
            {
                cbLenSymb_parameter = sp77sprintf(&symb_paramter[0],
                                                  14,
                                                  " :P_%d :IND",
                                                  cbExpandParam++);                  
            }
            else
            {
                cbLenSymb_parameter = sp77sprintf(&symb_paramter[0],
                                                  14,
                                                  " :P_%d",
                                                  cbExpandParam++);                  
            }
                
            erg = pr03PartConverttoPart(part_Ptr, 
                                        PacketOffset,
                                        free_part_space,
                                        PacketEncoding,
                                        &symb_paramter[0],
                                        cbLenSymb_parameter,
                                        sp77encodingAscii);
            if (erg != cpr_p_ok)
            /*error occured*/                            
            {
                return erg;        
            }                            
        
            /*if num > 1 expand remaining parameters*/
            for (i = 1; i < numExpand; i++)
            {
  
                if (bIndikator)
                {
                    cbLenSymb_parameter = sp77sprintf(&symb_paramter[0],
                                                      14,
                                                      ",:P_%d :IND",
                                                      cbExpandParam++);                  
                }
                else
                {
                    cbLenSymb_parameter = sp77sprintf(&symb_paramter[0],
                                                      14,
                                                      ",:P_%d",
                                                      cbExpandParam++);                  
                }

                erg = pr03PartConverttoPart(part_Ptr, 
                                            PacketOffset,
                                            free_part_space,
                                            PacketEncoding,
                                            &symb_paramter[0],
                                            cbLenSymb_parameter,
                                            sp77encodingAscii);
               if (erg != cpr_p_ok)
               /*error occured*/                            
               {
                   return erg;        
               }                            
            }
            /*get next symbol*/
            pr05cNextSymbol(pStatement, StmtLength, pAnalysePos, &SymbolPos, &SymbolLength, &Symbol, &actPos, StmtEncodingType);
            endBuf = pAnalysePos-1;
            pAnalysePos = actPos;
        } /*end while*/   
    }/*end if kapacnt >= -1*/
    /*put the last part of sql-statement to orderpacket */
    erg = pr03PartConverttoPart(part_Ptr, 
                                PacketOffset,
                                free_part_space,
                                PacketEncoding,
                                &pStatement[begBuf],
                                (endBuf-begBuf),
                                StmtEncodingType);
    return erg;
}


/*!
  Function:     pr08cPutDeclaretoPart

  see also: 

  Description:  Expands Declare command to part in order packet
    
  NOTE that in this function only the first part up to "...cursor for" 
  will put to the part in order packet. The SELECT statement will added
  later.

  Arguments:
    SQLDesc         [in]     pointer to a SQL descriptor
    prstate         [in]     state of command (cpr_state_command or cpr_state_decl_with_hold)
    part_Ptr        [in]     pointer to the part
    offset          [in|out] start position for writing in part, will be updated
    free_part_space [in|out] max free space in part
    PacketEncoding  [in]     encoding type of order packet 

  return value:  0   : processing successed
                -777 : error can not convert from unicode
                -808 : Not yet implemented
                -706 : SQL statement too long (buffer overflow)
 */
tpr_runtime_errors_epr00 pr08cPutDeclaretoPart(  tpr01_SQLDesc *SQLDesc, 
                                                tsp00_Int2    prstate,
                                                tsp1_part     *part_Ptr, 
                                                tsp00_Int4    *offset, 
                                                tsp00_Int4    *free_part_space,
                                                tpr05_StringEncoding PacketEncoding)
                                         
{

    char *declare_symb          = "DECLARE ";
    char *cursor_for_symb       = " CURSOR FOR ";
    char *cursor_with_hold_symb = " CURSOR WITH HOLD FOR ";
    tpr05_String *CursorName    = & (SQLDesc->CursorDesc->CursorNameDesc.CursorName);   
    tpr_runtime_errors_epr00  erg = cpr_p_ok;
    
    /*put 'DECLARE           ' in orderpacket*/
    erg = pr03PartConverttoPart(part_Ptr, 
                                offset,
                                free_part_space,
                                PacketEncoding,
                                declare_symb,
                                8,
                                sp77encodingAscii);
    if (erg != cpr_p_ok)
    /*error occured*/                            
    {
        return erg;        
    }                            
 
    /*put cursor name to order packet*/

    erg = pr08cPutCursorNametoPart(CursorName, 
                              part_Ptr, 
                              offset, 
                              free_part_space,
                              PacketEncoding);

    if (erg != cpr_p_ok)
    /*error occured*/                            
    {
        return erg;        
    }                            

    /*put CURSOR FOR -Literal in order packet */
    if   (prstate == cpr_state_command)
    {
        erg = pr03PartConverttoPart(part_Ptr, 
                                    offset,
                                    free_part_space,
                                    PacketEncoding,
                                    cursor_for_symb,
                                    12,
                                    sp77encodingAscii);
    }
    else /*prstate = cpr_state_decl_with_hold */
    {
        erg = pr03PartConverttoPart(part_Ptr, 
                                    offset,
                                    free_part_space,
                                    PacketEncoding,
                                    cursor_with_hold_symb,
                                    22,
                                    sp77encodingAscii);
    }

    return erg;
}

/*!
  Function:     pr08cCheckMassCmd

  see also: 

  Description:  Checks if statement is a mass command and set the 
                corresponding flag in segment header.

  Arguments:
    SQLDesc [in] a pointer to tpr01_SQLDesc

  return value: void
 */
void pr08cCheckMassCmd(tpr01_SQLDesc *SQLDesc)
{
    sqlcatype *sqlca = SQLDesc->SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQLDesc->SQL->GetSqlxa(SQLDesc);

    int erg = 0;
    /*check if mass cmd flag must set*/
    if ( 0 != pr04GetLoopCnt(sqlca, sqlxa, NULL) )
    {
        /*set mass cmd flag in order interface*/
        pr03SegmentSetAttribute(SQLDesc->ConDesc->SegmDesc->segmPtr, Mass_Cmd_epr03, TRUE);
    }
    
}


/*!
  Function:     p08cputStatementtoPart

  see also: 

  Description:  Puts SQL-Statements depending on its encoding type
                to order interface, expands cursor name,
                checks whether structure variables and question marks still 
                have to be converted or expanded and much more. 

  Arguments:
    SQLDesc [in|out] pointer to a tpr01_SQLDesc

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes actually written into buffer
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed
    </UL>
 */

int pr08cputStatementtoPart(tpr01_SQLDesc *SQLDesc)
{ 

  M90TRACE(M90_TR_ENTRY, "pr08cputStatementtoPart", 0);
  {
    tsp1_part *part_Ptr = SQLDesc->ConDesc->ga->gareqptr; 
    sqlcatype *sqlca = SQLDesc->SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQLDesc->SQL->GetSqlxa(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    tpr01_StmtNameDesc *StmtNameDesc = SQLDesc->StmtNameDesc;

    tsp00_Int4 free_part_space = 0; /*free space in order packet*/
    tsp00_Int4 offset = 0;        /*position for continue writing to part*/
    
    /*find part*/
    part_Ptr = pr03PartFind(sqlra, sp1pk_command);
    
    if (part_Ptr == NULL) 
    {
        part_Ptr = pr03SegmentAddPart(SQLDesc->ConDesc->SegmDesc, sp1pk_command);   
    }
    
    /*check part, because may be that pr03SegmentAddPart can not add a part*/
    if (part_Ptr) 
    {    
        tpr_runtime_errors_epr00 erg    = cpr_p_ok; /*return value of putPart-functions*/
        free_part_space = pr03PartGetFreePartSpace(part_Ptr);
        
        switch(StmtNameDesc->StmtTypeDesc.StmtType)
        {
            case (KeywordFETCH_epr01)  :        
            case (KeywordFFETCH_epr01) :        
            case (KeywordMFETCH_epr01) :
            case (KeywordPUTVAL_epr01) :
            case (KeywordGETVAL_epr01) :
            case (KeywordCLOSE_epr01) :
            {
                tsp00_Uint4 buflen = 0;
                tsp00_Int4 actPos = 0; /*actual position in statement up to contuine analyze and expand parameters*/
                
                
                if(    StmtNameDesc->StmtTypeDesc.StmtType == KeywordFETCH_epr01
                    || StmtNameDesc->StmtTypeDesc.StmtType == KeywordFFETCH_epr01 
                    || StmtNameDesc->StmtTypeDesc.StmtType == KeywordMFETCH_epr01 
                  )
                {
                    /*set options for fetch command*/
                    pr08SetFetchOptions(SQLDesc);
                }    

                /*put first part (up to cursorname) of sqlStatement to part*/
                /* Three cases - parameter name, identifier, unnamed result set. */
                switch(StmtNameDesc->StmtTypeDesc.CursorType) {
                case (CPR_S_PARAMETER_NAME): {
                    buflen = StmtNameDesc->StmtTypeDesc.CursorPos-1;  
                    /*NOTE that CursorPos is pascal counting, starting with 1 not 0*/
                    actPos = buflen + StmtNameDesc->StmtTypeDesc.CursorLength;
                    break;
                }
                case (CPR_S_IDENTIFIER): {
                    pr05IfCom_String_toupperBuf(&StmtNameDesc->SQLStatement->rawString[StmtNameDesc->StmtTypeDesc.CursorPos-1], 
                                                StmtNameDesc->StmtTypeDesc.CursorLength, 
                                                StmtNameDesc->SQLStatement->encodingType);
                    buflen = StmtNameDesc->StmtTypeDesc.CursorPos + StmtNameDesc->StmtTypeDesc.CursorLength;
                    /* We must use not only the cursor name, but also the space that 
                       follows in the statement. Therefore we must add the size of a
                       single char in the resp. encoding. */
                    buflen += StmtNameDesc->SQLStatement->encodingType->fixedCharacterSize > 1 ? 1 : 0;
                    actPos = buflen;  
                    break;
                }
                case (CPR_S_STRING_LITERAL): {
                    buflen = StmtNameDesc->StmtTypeDesc.CursorPos + StmtNameDesc->StmtTypeDesc.CursorLength;
                    /* We must use not only the cursor name, but also the space that 
                       follows in the statement. Therefore we must add the size of a
                       single char in the resp. encoding. */
                    buflen += StmtNameDesc->SQLStatement->encodingType->fixedCharacterSize > 1 ? 1 : 0;
                    actPos = buflen;  
                    break;
                }
                default:
                case (CPR_S_UNKNOWN): {
                      buflen = StmtNameDesc->StmtTypeDesc.CursorPos;
                      actPos = buflen;
                }
                }
                erg = pr03PartConverttoPart(part_Ptr, 
                                            &offset,
                                            &free_part_space,
                                            SQLDesc->PacketEncoding,
                                            StmtNameDesc->SQLStatement->rawString,
                                            PR07MIN(buflen,StmtNameDesc->SQLStatement->cbLen),
                                            StmtNameDesc->SQLStatement->encodingType);
                
                if (erg != cpr_p_ok)
                /*error occured*/                            
                {
                     pr01TraceRuntimeError(sqlca , sqlxa , erg);   
                }                            
                
                if ( StmtNameDesc->StmtTypeDesc.CursorType == CPR_S_PARAMETER_NAME)
                /* cursor name comes from hostvariable */
                {
                    tpr05_String *CursorName;
                    
                    if (SQLDesc->CursorDesc && SQLDesc->CursorDesc->CursorNameDesc.CursorName.cbLen != 0)
                    {  
                        CursorName = &(SQLDesc->CursorDesc->CursorNameDesc.CursorName);
                                            
                        erg = pr08cPutCursorNametoPart( CursorName, 
                                                        part_Ptr, 
                                                        &offset, 
                                                        &free_part_space,
                                                        SQLDesc->PacketEncoding);
    
                        if (erg != cpr_p_ok)
                        /*error occured*/                            
                        {
                            pr01TraceRuntimeError(sqlca , sqlxa , erg);   
                        }
                   }
                   else
                   {
                      pr01TraceRuntimeError(sqlca , sqlxa , cpr_unknown_cursor_name); 
                   }                                 
                }
                    
                /* expand using or into descriptor to part*/
                if (StmtNameDesc->StmtTypeDesc.UsingPos > 0)
                {
                    tsp00_Int4 i, anz_da_param = 0;
                    char *param_symb = ", :A";
                    char *into_symb  = "INTO "; 
                    
                    if (sqlca->sqlcxap->xasqldap || StmtNameDesc->ka->kapacount)
                    /*if a sqlda exists*/
                    {
                            /*compute number of paramters in sqlda */
                      if (sqlca->sqlcxap->xasqldap) {
                            switch (sqlca->sqldbmode) 
                            {
                                case cpr_kind_oracle:
                                case cpr_kind_sapr3:
                                {
                                    anz_da_param = ((struct SQLDA *) sqlca->sqlcxap->xasqldap)->F;
                                    break; 
                                }
                                default :
                                {
                                    /* cpr_kind_internal */
                                    anz_da_param = ((sqldatype *) sqlca->sqlcxap->xasqldap)->sqld;
                                    
                                }
                            }
                      }
                      else {
                        anz_da_param = StmtNameDesc->ka->kapacount;
                      }
                            /*expand into*/
                            erg = pr03PartConverttoPart(part_Ptr, 
                                                        &offset,
                                                        &free_part_space,
                                                        SQLDesc->PacketEncoding,
                                                        into_symb,
                                                        5,
                                                        sp77encodingAscii);
                            if (erg != cpr_p_ok)
                            /*error occured*/                            
                            {
                                pr01TraceRuntimeError(sqlca , sqlxa , erg);   
                            }                            
                        
                            /*expand first parameter*/
                            erg = pr03PartConverttoPart(part_Ptr, 
                                                        &offset,
                                                        &free_part_space,
                                                        SQLDesc->PacketEncoding,
                                                        &param_symb[2],
                                                        2,
                                                        sp77encodingAscii);
                            if (erg != cpr_p_ok)
                            /*error occured*/                            
                            {
                                pr01TraceRuntimeError(sqlca , sqlxa , erg);
                            }                            
                        
                            /*expand parameters*/
                             for (i = 1;  i < anz_da_param; i++)  
                             {
                                erg = pr03PartConverttoPart(part_Ptr, 
                                                            &offset,
                                                            &free_part_space,
                                                            SQLDesc->PacketEncoding,
                                                            param_symb,
                                                            4,
                                                            sp77encodingAscii);
                                if (erg != cpr_p_ok)
                                /*error occured*/                            
                                {
                                    pr01TraceRuntimeError(sqlca , sqlxa , erg);
                                }                            
                            } /*end for*/
                    } /*end sqlda exists*/
                    else
                    /*no sqlda found then expand the rest of original statement. Maybe this cannot parsed*/
                    {
                        erg = pr03PartConverttoPart(part_Ptr, 
                                                    &offset,
                                                    &free_part_space,
                                                    SQLDesc->PacketEncoding,
                                                    &StmtNameDesc->SQLStatement->rawString[actPos],
                                                    StmtNameDesc->SQLStatement->cbLen-actPos,
                                                    StmtNameDesc->SQLStatement->encodingType);


                        if (erg != cpr_p_ok)
                        /*error occured*/                            
                        {
                            pr01TraceRuntimeError(sqlca , sqlxa , erg);
                        }                            
                    }
                }
                else
                {
                    /*expand occuring parameters up position actPos*/
                    erg = pr08cExpandParameterstoPart( SQLDesc,
                                                       actPos, 
                                                       part_Ptr, 
                                                       &offset,
                                                       &free_part_space,
                                                       SQLDesc->PacketEncoding);
                    if (erg != cpr_p_ok)
                    /*error occured*/                            
                    {
                        pr01TraceRuntimeError(sqlca , sqlxa , erg);
                    } 
                }
                
                break;
            }        
            
            case (KeywordCREATE_epr01) :
            case (KeywordDROP_epr01) :
            case (KeywordDESCRIBE_epr01) :
            case (KeywordNEXT_epr01) :
            { 
                erg = pr03PartConverttoPart(part_Ptr, 
                                            &offset,
                                            &free_part_space,
                                            SQLDesc->PacketEncoding,
                                            StmtNameDesc->SQLStatement->rawString,
                                            StmtNameDesc->SQLStatement->cbLen,
                                            StmtNameDesc->SQLStatement->encodingType);
                if (erg != cpr_p_ok)
                /*error occured*/                            
                {
                    pr01TraceRuntimeError(sqlca , sqlxa , erg);
                }                            
                break;
            }        
            
            default :
            /*all other commands f.i. declare, select, insert, update, proc, delete, show ...*/
            {
                /*if mass command set flag in segment header*/
                pr08cCheckMassCmd(SQLDesc);


                /* if declare statement -> expand statement */
                if (SQLDesc->pr)
                {
                    tsp00_Int2 prstate = SQLDesc->pr->prstate;    
                
                    if ((prstate == cpr_state_decl_with_hold)
                         && StmtNameDesc->StmtTypeDesc.StmtType == KeywordSELECT_epr01
                        )
                    {
                        erg = pr08cPutDeclaretoPart(SQLDesc, 
                                                    prstate,
                                                    part_Ptr, 
                                                    &offset, 
                                                    &free_part_space,
                                                    SQLDesc->PacketEncoding);
                                               
                        if(erg != cpr_p_ok)
                        /*error occured*/
                        {
                            pr01TraceRuntimeError(sqlca , sqlxa , erg);
                        }                        
                    }    
                } /*end analyze Declare*/

                /* put the rest of statement to part and analyse if parameters occurs and expand them */
                erg = pr08cExpandParameterstoPart(  SQLDesc,
                                                    0,
                                                    part_Ptr, 
                                                    &offset,
                                                    &free_part_space,
                                                    SQLDesc->PacketEncoding);
 
                if(erg != cpr_p_ok)
                /*error occured*/
                {
                    pr01TraceRuntimeError(sqlca , sqlxa , erg);
                }                        
                        
                
    
            } /*end default*/
         }/*end switch*/   
     
    } /*end if partptr*/
    
     /*finish part*/
     pr03SegmentFinishPart(SQLDesc->ConDesc->SegmDesc);

     M90TRACE(M90_TR_EXIT, "pr08cPutStatementtoPart", 0);
  } /*M90Trace*/    
  return 0;
}        

void p08analyzeconnectstatm (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgatype *sqlga, sqlgaentry *ga, sqlkaentry *ka, tsp00_Int4 buflen, char *buf)
{
  tpr05_String ConString;
  ConString.allocationType = constant_epr05;
  ConString.encodingType = sp77encodingAscii;
  ConString.rawString=buf;
  ConString.cbLen=buflen-1;    
  pr02ConAnalyzeConnectStmt (sqlca, sqlxa, sqlga, ga, ka, buflen, &ConString); 
}
