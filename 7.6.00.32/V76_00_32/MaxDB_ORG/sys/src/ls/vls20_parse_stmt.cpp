/*
  -----------------------------------------------------------------------------
  module: vls20_parse_stmt.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of INSERT/UPDATE statement creation for DB parsing

  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

*/

#include "gls00.h"
#include "hls13.h"
#include "hls31_odbc_Result.h"
#include "hls20_parse_stmt.h"
#include "hls99.h"
#include "hls98msg.h"

#define DBFUNC_NAME_OFFSET_LS20 3

static char const *szDBFunctionNames_ls20[6] = {"USER","USERGROUP","STAMP","DATE","TIME","TIMESTAMP"};
static char const *szDuplicateString_ls20[4] = {" ", " IGNORE DUPLICATES", " REJECT DUPLICATES", " UPDATE DUPLICATES"};

/*
  -----------------------------------------------------------------------------
  function:     ls20_ParseInsertStmt
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ParseInsertStmt(MetaDataDef*&             pMetaDataDef,
                    tls00_TransformationRTE*  pTransformationRTE,
                    tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20ParseInsertStmt");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Longint    rc      = errOK_els00;

    tls00_TransformationCmd *pTransformationCmd = pMetaDataDef->getTransformationModel()->getTransformationCmd();
    tls00_DBInfo            *pDBInfo            = pMetaDataDef->getConnection();



    tls00_MultColSpec      &MultCol       = pTransformationCmd->trColSpec_ls00;
    tls00_TableSpec        &TableSpec     = pTransformationCmd->trTable_ls00;
    tls00_TableDescription *pTI           = &pTransformationRTE->rte_TableInfo_ls00;
    tls00_Column*           pColumn       = NULL;

    char*                  pszCmd         = NULL;
    SAPDB_UInt4            _lCmdLen       = 0;
    bool                   _bAssignments  = false;
    tsp00_Int4             i              = 0;
    tsp00_Uint4            _lPos          = 0;

    if (NULL != pTransformationRTE->rte_pStmtParsed_ls00)
    {
        rc = ls13Parse(pTransformationRTE->rte_pStmtParsed_ls00, pDBInfo,
                       pTransformationRTE->rte_ParseId_ls00, pTI, pszErrText, false, true);
    }
    else
    {
        //*
        //*     - Check if user specified column names in command; if not get them here
        //*     - Check for assignments in command
        //*
        if (0 == MultCol.mcsCount_ls00)
        {
            if (0 != TableSpec.tsTabSchema_ls00.length() || 0 != TableSpec.tsTabName_ls00.length())
                rc = ls13GetColumnNames(pMetaDataDef, TableSpec.tsTabSchema_ls00.asCharp(), TableSpec.tsTabName_ls00.asCharp(), pTI, pszErrText);
            else
                rc = ls13GetColumnNames(pMetaDataDef,
                                        pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp(),
                                        pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                        pTI, pszErrText);

            // No columns specified in command: get'em from db
            if (errOK_els00 != rc)
            {
                return STATIC_CAST(tsp00_Int4, rc);
            }
            else
            {
                // adapt column count
                MultCol.mcsCount_ls00 = pTI->tdFieldCount_ls00;

                // Create structure MultCol
                for (int i=0; i < pTI->tdFieldCount_ls00; i++)
                {
                    if (NULL == MultCol.mcsColumn_ls00[i])
                    {
                        MultCol.mcsColumn_ls00[i] = new tls00_Column;
                        memset(MultCol.mcsColumn_ls00[i], DEF_BYTE_LS00, sizeof(tls00_Column));
                    }
                    _tcscpy(MultCol.mcsColumn_ls00[i]->colName_ls00.asCharp(), pTI->tdColName_ls00[i]->asCharp());
                }
            }
        }
        else
        {
            // User specified columns in DATALOAD command 
            // --> assignments are possible; we check for assignments and add their length here already
            for (int j=0, k=0; j < MultCol.mcsCount_ls00; ++j)
            {
                pColumn = MultCol.mcsColumn_ls00[j];

                if ( (asUnknown_els00 == pColumn->colAssign_ls00.fasAsgnTyp_ls00) ||
                     (asSeqno_els00   == pColumn->colAssign_ls00.fasAsgnTyp_ls00) )     // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
                {
                    // Column names are of interest only if those columns must be filled with
                    // DATA from DATA file; that doesn't apply for assignments
                    pTI->tdColName_ls00[k] = new tsp00_KnlIdentifierc;
                    _tcscpy(pTI->tdColName_ls00[k]->asCharp(), pColumn->colName_ls00.asCharp());
                    ++k;
                }
                else
                {
                    if (0 != pColumn->colAssign_ls00.fasAsgnLen_ls00)
                    {
                        _lCmdLen += pColumn->colAssign_ls00.fasAsgnLen_ls00;
                    }
                    else
                    {
                        _lCmdLen += _tcslen(szDBFunctionNames_ls20[pColumn->colAssign_ls00.fasAsgnTyp_ls00-DBFUNC_NAME_OFFSET_LS20]);
                    }
                }
            }
            if (_lCmdLen > 0)
            {
                _bAssignments = true;
            }
        }

        //*
        //*     Build INSERT statement and parse it
        //* First time parsing is done as if the user did not specify assignments. This
        //* way the kernel returns information on the columns to handle. This information
        //* is necessary for second time parsing in case of assignments: in case of
        //* literal assignment we have to decide whether to include the DATA in single
        //* quotes or not (depending on whether the columns are number or char columns).
        //* 
        _lCmdLen += (MultCol.mcsCount_ls00 + 12) * sizeof(tsp00_KnlIdentifier);

        pszCmd = new char[_lCmdLen];
        pszCmd[0] = '\0';

        // The table name MUST be given in any case (somehow)
        if (0 != TableSpec.tsTabName_ls00.length())
        {
            if ( 0 != TableSpec.tsTabSchema_ls00.length() )
            {
                _lPos = sp77sprintf(pszCmd, _lCmdLen, "INSERT INTO \"%s\".\"%s\" (\"%s",
                                                      TableSpec.tsTabSchema_ls00.asCharp(),
                                                      TableSpec.tsTabName_ls00.asCharp(),
                                                      MultCol.mcsColumn_ls00[0]->colName_ls00.asCharp());
            }
            else
            {
                _lPos = sp77sprintf(pszCmd, _lCmdLen, "INSERT INTO \"%s\" (\"%s", 
                                                      TableSpec.tsTabName_ls00.asCharp(),
                                                      MultCol.mcsColumn_ls00[0]->colName_ls00.asCharp());
            }
        }
        else
        {
//            if ( 0 != pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.length())
            if ( 0 != pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length())
            {
                _lPos = sp77sprintf(pszCmd, _lCmdLen, "INSERT INTO \"%s\".\"%s\" (\"%s",
                                                      pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp(),
                                                      pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                                      MultCol.mcsColumn_ls00[0]->colName_ls00.asCharp());
            }
            else
            {
                _lPos = sp77sprintf(pszCmd, _lCmdLen, "INSERT INTO \"%s\" (\"%s", 
                                                      pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                                      MultCol.mcsColumn_ls00[0]->colName_ls00.asCharp());
            }
        }   // end else of if (0 != TableSpec.tsTabName_ls00.length())

        for (i=1; i < MultCol.mcsCount_ls00; i++)
        {
            _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, "\",\"%s", MultCol.mcsColumn_ls00[i]->colName_ls00.asCharp());
        }
        _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, "\") VALUES(?");

        // Define pos in statement where value specification starts for later use in case
        // the user specified assignments in command
        size_t _lValueStartPos = _lPos - sizeof(char);

        for (i=1; i < MultCol.mcsCount_ls00; ++i)
        {
            _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, ",?");
        }
        _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, ") %s", szDuplicateString_ls20[pTransformationCmd->trDplMode_ls00]);

        TRACE_PRNF_MLS99(("ls20_ParseInsertStmt", "Stmt to parse: %s\n", pszCmd) );

        // Let's parse
        rc = ls13Parse(pszCmd, pDBInfo, pTransformationRTE->rte_ParseId_ls00, pTI, pszErrText, false, true);
        if ( (errOK_els00 == rc) && (true == _bAssignments) )
        {
            TRACE_PRNF_MLS99(("ls20_ParseInsertStmt", "Assignments in load command; parsing 2nd time\n") );

            //*
            //*     Build INSERT statement a second time and parse again if the user
            //*     specified assignments in command
            //*

            // We have filled
            //                   pTI->tdParamArray_ls00
            // with the first execution of ls13Parse;
            // we'll use it here for literal assignments to decide wether the value has to
            // be included in single quotes or not (DATA type);

            tls00_FieldAsgn* pAssignment = NULL;
            
            pszCmd[_lValueStartPos] = 0;    // reinitialize pszCmd string to the pos where the value spec begins
            _lPos                   = _lValueStartPos;

            for (i=0; i < MultCol.mcsCount_ls00; ++i)
            {
                pAssignment = &MultCol.mcsColumn_ls00[i]->colAssign_ls00;
                
                if ( (asUnknown_els00 == MultCol.mcsColumn_ls00[i]->colAssign_ls00.fasAsgnTyp_ls00) ||
                     (asSeqno_els00   == MultCol.mcsColumn_ls00[i]->colAssign_ls00.fasAsgnTyp_ls00) )   // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
                {
                    _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, "?,");
                }
                else
                {
                    switch (pAssignment->fasAsgnTyp_ls00)
                    {
                        case asLiteral_els00:
                        {
                            if (1 == szMap_NumberDataType_ls00[pTI->tdParamArray_ls00[i]->piBasicInfo_ls00.fbDataType_ls00])
                            {
                                _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, "%s,", pAssignment->fasAsgnVal_ls00);
                            }
                            else
                            {
                                _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, "'%s',", pAssignment->fasAsgnVal_ls00);
                            }
                            break;
                        }
                        default:
                        {
                            _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, 
                                                            "%s,", szDBFunctionNames_ls20[pAssignment->fasAsgnTyp_ls00-DBFUNC_NAME_OFFSET_LS20]);
                        }
                    }   // end switch (MultCol.mcsColumn_ls00[i]->colAssign_ls00.fasAsgnTyp_ls00)
                }
            
            }
            _lPos -= sizeof(char);
            _lPos += sp77sprintf(pszCmd+_lPos, _lCmdLen-_lPos, ") %s", szDuplicateString_ls20[pTransformationCmd->trDplMode_ls00]);


            // Before running ls13Parse the 2nd time we have to free
            //                   pTI->tdParamArray_ls00
            // in order to avoid memory leaks
            for (tsp00_Int4 j=0; j < pTI->tdFieldCount_ls00; ++j)
            {
                if (NULL != pTI->tdParamArray_ls00[j])
                {
                    delete pTI->tdParamArray_ls00[j];
                    pTI->tdParamArray_ls00[j] = NULL;
                }
            }
            pTI->tdFieldCount_ls00 = 0;

            TRACE_PRNF_MLS99(("ls20_ParseInsertStmt", "Stmt to parse (2nd): %s\n", pszCmd) );

            rc = ls13Parse(pszCmd, pDBInfo, pTransformationRTE->rte_ParseId_ls00, pTI, pszErrText, false, true);
        }   // end if ( (errOK_els00 == rc) && (true == _bAssignments) )
    }   // end else of if (NULL != pTransformationRTE->rte_pStmtParsed_ls00)

    if (NULL == pTransformationRTE->rte_pStmtParsed_ls00)
    {
        _lCmdLen = _tcslen(pszCmd);         // Reusing _lCmdLen
        pTransformationRTE->rte_pStmtParsed_ls00 = new char[_lCmdLen + sizeof(char)];
        SAPDB_memcpy(pTransformationRTE->rte_pStmtParsed_ls00, pszCmd, _lCmdLen);
        pTransformationRTE->rte_pStmtParsed_ls00[_lCmdLen] = 0;
    }
    if (NULL != pszCmd)
    {
        delete [] pszCmd;
        pszCmd = NULL;
    }

    return STATIC_CAST(tsp00_Int4, rc);
}
//  ls20ParseInsertStmt()


/*
  -----------------------------------------------------------------------------
  function:     ls20ParseUpdateStmt
  -----------------------------------------------------------------------------
  Fills  pTransformationRTE->rte_TableInfo_ls00
  Adapts pTransformationCmd->trSetColIndex_ls00 (in case of assignments)
*/
SAPDB_Int4
ls20ParseUpdateStmt(MetaDataDef*&             pMetaDataDef,
                    tls00_TransformationRTE*&  pTransformationRTE,
                    tsp00_Addr                 pszErrText)
{
    ROUTINE_DBG_MEO00("ls20ParseUpdateStmt");

    SAPDB_Int4 rc = errOK_els00;

    tls00_TransformationCmd *pTransformationCmd = pMetaDataDef->getTransformationModel()->getTransformationCmd();
    tls00_DBInfo            *pDBInfo            = pMetaDataDef->getConnection();

    // Prepares the DataUpdate command as a update mass cmd
    tls00_Column*       pColumn      = NULL;
    tls00_MultColSpec*  pMultColSpec = &pTransformationCmd->trColSpec_ls00;   

    SAPDB_Int4          k            = 0;
    SAPDB_Int4          iKeyColCnt   = 0;
    SAPDB_Int4          iWhereColCnt = 0;
    bool                _bAssignment = false;
    char*               pszCmd       = NULL;
    SAPDB_Int4          _lCmdLen     = 0;
    SAPDB_Int4          i            = 0;

    if (NULL != pTransformationRTE->rte_pStmtParsed_ls00)
    {
        rc = ls13Parse(pTransformationRTE->rte_pStmtParsed_ls00,
                       pDBInfo,
                       pTransformationRTE->rte_ParseId_ls00,
                       &pTransformationRTE->rte_TableInfo_ls00,
                       pszErrText, false, true);
    }
    else
    {
        for (i = 0; i < pMultColSpec->mcsCount_ls00; ++i)
        {
            pColumn = pMultColSpec->mcsColumn_ls00[i];
            if (true == pColumn->colIsKey_ls00)
            {
                ++iKeyColCnt;
            }

            if ( (asUnknown_els00 != pColumn->colAssign_ls00.fasAsgnTyp_ls00) &&
                 (asSeqno_els00   != pColumn->colAssign_ls00.fasAsgnTyp_ls00) )     // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
            {
                _bAssignment = true;
                _lCmdLen += pColumn->colAssign_ls00.fasAsgnLen_ls00;
            }
        }
        iWhereColCnt = pTransformationCmd->trSetColIndex_ls00 - iKeyColCnt;


        _lCmdLen += (pMultColSpec->mcsCount_ls00 + 24)*sizeof(tsp00_KnlIdentifierc);
        
        pszCmd     = new char[_lCmdLen];
        pszCmd[0]  = 0;

        //*
        //*     <update_statement>::= UPDATE [OF] <table_name> SET <set_update_clause>,...
        //*
        //*  There must be at least one SET column!
        //*
        if (0 != pTransformationCmd->trTable_ls00.tsTabSchema_ls00.length())
        {
            k = sp77sprintf(pszCmd, _lCmdLen, "UPDATE \"%s\".\"%s\" SET", pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp(),
                                                                          pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());
        }
        else
        {
            k = sp77sprintf(pszCmd, _lCmdLen, "UPDATE \"%s\" SET", pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());
        }

        tsp00_Uint4 _lValueStartPos = k;

        //Set columns start behind access/key columns in structure MultColSpec
        for (i = pTransformationCmd->trSetColIndex_ls00; i < pMultColSpec->mcsCount_ls00; ++i)
        {
            k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = ?,", pMultColSpec->mcsColumn_ls00[i]->colName_ls00.asCharp() );
        }
        --k;        // Remove last comma

        
        // [ KEY <key_spec>,...]
        if ( 0 != iKeyColCnt )
        {
            k += sp77sprintf(pszCmd+k, _lCmdLen - k, " KEY "); 

            for (i = 0; i < pTransformationCmd->trSetColIndex_ls00; ++i)
            {
                if ( true == pMultColSpec->mcsColumn_ls00[i]->colIsKey_ls00 )
                {
                    k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = ?,",
                                                             pMultColSpec->mcsColumn_ls00[i]->colName_ls00.asCharp());
                }
            }
            --k;        // Remove last comma
        }

        // <search_condition> 
        if ( iWhereColCnt > 0 )
        {
            k += sp77sprintf(pszCmd+k, _lCmdLen - k, " WHERE "); 

            for (i = 0; i < pTransformationCmd->trSetColIndex_ls00; ++i)
            {
                if ( ( false == pMultColSpec->mcsColumn_ls00[i]->colIsKey_ls00 ) &&
                     ( false == pMultColSpec->mcsColumn_ls00[i]->colIsSet_ls00 ) )

                {
                    if ( true  == pTransformationCmd->trInternal_ls00 )
                         k += sp77sprintf(pszCmd+k, _lCmdLen - k, "  %s    = ? AND", pMultColSpec->mcsColumn_ls00[i]->colName_ls00.asCharp());
                    else
                         k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = ? AND", pMultColSpec->mcsColumn_ls00[i]->colName_ls00.asCharp());
                }
            }
            pszCmd[k - 3] = 0;      // Remove last 'AND'
        }
        else
        {
            pszCmd[k] = 0;          // finish command with \0
        }

        // Parse statement
        // function returns parse id and shortinfo
        rc = ls13Parse(pszCmd, pDBInfo, pTransformationRTE->rte_ParseId_ls00, &pTransformationRTE->rte_TableInfo_ls00, pszErrText, false, true);
        if (errOK_els00 == rc)
        {
            // In case of assignments this value will be decreased by 1 for every assignment in qualification columns
            // for later use in ls20InitDataSource
            SAPDB_Int4 _lTmpSetColIndex = pTransformationCmd->trSetColIndex_ls00;    

            if (true == _bAssignment)
            {
                //*
                //*     In case of assignments we need to reparse the statement to let
                //*     the kernel do the assignment upon delivery (instead of parameterizing
                //*     all values with ? we parameterize the assignments with the real value
                //*

                // <update_statement>::=

                // Reuse pszCmd

                k = _lValueStartPos;

                //Set columns start behind access/key columns in structure MultColSpec
                tls00_FieldAsgn *pAssignment = NULL;
                for (i = pTransformationCmd->trSetColIndex_ls00; i < pMultColSpec->mcsCount_ls00; ++i)
                {
                    pColumn     = pMultColSpec->mcsColumn_ls00[i];
                    pAssignment = &pColumn->colAssign_ls00;

                    k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = ?,", pColumn->colName_ls00.asCharp());

                    if ( (asUnknown_els00 != pAssignment->fasAsgnTyp_ls00) &&
                         (asSeqno_els00   != pAssignment->fasAsgnTyp_ls00) )
                    {
                        k -= 2*sizeof(char);
                        switch (pAssignment->fasAsgnTyp_ls00)
                        {
                            case asLiteral_els00:
                            {
                                if (1 == szMap_NumberDataType_ls00[pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[i-pTransformationCmd->trSetColIndex_ls00]->piBasicInfo_ls00.fbDataType_ls00])
                                {
                                    k += sp77sprintf(pszCmd+k, _lCmdLen-k, "%s,", pAssignment->fasAsgnVal_ls00);
                                }
                                else
                                {
                                    k += sp77sprintf(pszCmd+k, _lCmdLen-k, "'%s',", pAssignment->fasAsgnVal_ls00);
                                }
                                break;
                            }
                            default:
                            {
                                k += sp77sprintf(pszCmd+k, _lCmdLen-k, "%s,", szDBFunctionNames_ls20[pAssignment->fasAsgnTyp_ls00-3]);
                            }
                        }   // end switch (pAssignment->fasAsgnTyp_ls00)
                    }   // end if (asUnknown_els00 != pAssignment->fasAsgnTyp_ls00)
                }   // end for (i = pTransformationCmd->trSetColIndex_ls00; i < pMultColSpec->mcsCount_ls00; ++i)
                --k;        // Remove last comma

                // [ KEY <key_spec>,...]
                if ( 0 != iKeyColCnt )
                {
                    k += sp77sprintf(pszCmd+k, _lCmdLen - k, " KEY "); 
                    for (i = 0; i < pTransformationCmd->trSetColIndex_ls00; ++i)
                    {
                        if ( true == pMultColSpec->mcsColumn_ls00[i]->colIsKey_ls00 )
                        {
                            k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = ?,", pMultColSpec->mcsColumn_ls00[i]->colName_ls00.asCharp());
                        }
                    }
                    --k;        // Remove last comma
                }

                // <search_condition> 
                if ( 0 != iWhereColCnt )
                {
                    tsp00_Uint4 _lSetColCnt = pMultColSpec->mcsCount_ls00 - iWhereColCnt;

                    k += sp77sprintf(pszCmd+k, _lCmdLen - k, " WHERE "); 
                    for (i = 0; i < pTransformationCmd->trSetColIndex_ls00; ++i)
                    {
                        pColumn     = pMultColSpec->mcsColumn_ls00[i];
                        pAssignment = &pColumn->colAssign_ls00;

                        if ( ( false == pColumn->colIsKey_ls00 ) && ( false == pColumn->colIsSet_ls00) )
                        {
                            if ( (asUnknown_els00 == pAssignment->fasAsgnTyp_ls00) ||
                                 (asSeqno_els00 == pAssignment->fasAsgnTyp_ls00) )
                            {
                                k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = ? AND", pColumn->colName_ls00.asCharp());
                            }
                            else
                            {
                                --_lTmpSetColIndex;

                                switch (pAssignment->fasAsgnTyp_ls00)
                                {
                                    case asLiteral_els00:
                                    {
                                        if (1 == szMap_NumberDataType_ls00[pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[i+_lSetColCnt]->piBasicInfo_ls00.fbDataType_ls00])
                                        {
                                            k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = %s AND", pColumn->colName_ls00.asCharp(),
                                                                                                         pAssignment->fasAsgnVal_ls00);
                                        }
                                        else
                                        {
                                            if ( true  == pTransformationCmd->trInternal_ls00 )
                                                 k += sp77sprintf(pszCmd+k, _lCmdLen - k, "    %s = '%s' AND", pColumn->colName_ls00.asCharp(),
                                                                                                               pAssignment->fasAsgnVal_ls00);
                                            else
                                                k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = '%s' AND", pColumn->colName_ls00.asCharp(),
                                                                                                               pAssignment->fasAsgnVal_ls00);
                                        }
                                        break;
                                    }
                                    default:
                                    {
                                        k += sp77sprintf(pszCmd+k, _lCmdLen - k, " \"%s\" = %s AND", pColumn->colName_ls00.asCharp(),
                                                                                                     szDBFunctionNames_ls20[pAssignment->fasAsgnTyp_ls00-3]);
                                    }
                                }   // end switch (pAssignment->fasAsgnTyp_ls00)
                            }   // end if (asUnknown_els00 != pAssignment->fasAsgnTyp_ls00)
                        }
                    }
                    pszCmd[k - 3] = 0;      // Remove last 'AND'
                }
                else
                {
                    // finish command with \0
                    pszCmd[k] = 0;
                }

            
                // Before running ls13Parse the 2nd time we have to free
                //                   pTI->tdParamArray_ls00
                // in order to avoid memory leaks
                for (tsp00_Int4 j=0; j < pTransformationRTE->rte_TableInfo_ls00.tdFieldCount_ls00; ++j)
                {
                    if (NULL != pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[j])
                    {
                        delete pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[j];
                        pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[j] = NULL;
                    }
                }
                pTransformationRTE->rte_TableInfo_ls00.tdFieldCount_ls00 = 0;

                // Parse statement
                // function returns parse id and shortinfo
                rc = ls13Parse(pszCmd, pDBInfo, pTransformationRTE->rte_ParseId_ls00, &pTransformationRTE->rte_TableInfo_ls00, pszErrText, false, true);
            }   // end if (true == _bAssignment)

            if (errOK_els00 == rc)
            {
                pTransformationCmd->trSetColIndex_ls00 = _lTmpSetColIndex;
            }
        }   // end if ( (errOK_els00 == rc = ls13Parse(... )
    }   // end else of if (NULL != pTransformationRTE->rte_pStmtParsed_ls00)



    if (NULL == pTransformationRTE->rte_pStmtParsed_ls00)
    {

        _lCmdLen = _tcslen(pszCmd);         // Reusing _lCmdLen
        pTransformationRTE->rte_pStmtParsed_ls00 = new char[_lCmdLen + sizeof(char)];
        SAPDB_memcpy(pTransformationRTE->rte_pStmtParsed_ls00, pszCmd, _lCmdLen);
        pTransformationRTE->rte_pStmtParsed_ls00[_lCmdLen] = 0;
    }

    delete [] pszCmd;
    pszCmd = NULL;

    return rc;
}
// ls20ParseUpdateStmt()