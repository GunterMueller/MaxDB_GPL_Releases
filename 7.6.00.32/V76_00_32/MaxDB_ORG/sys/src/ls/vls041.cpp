/*
  -----------------------------------------------------------------------------
  module: vls041.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Supplement Function to packet functions in ls04 module
                
  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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
#include "hls04.h"
#include "hls05.h"
#include "hls07.h"
#include "hls041.h"
#include "hls98msg.h"
#include "hls99.h"


/*
  -----------------------------------------------------------------------------
  function:     ls04GetShortInfo
  -----------------------------------------------------------------------------
*/
int ls04GetShortInfo(tsp1_packet        *SqlPacket,
                     tsp00_Int2         FieldsAvail,
                     tls00_ParamInfo    *Fields[],
                     tsp00_Int2     &   FieldCount)
{
    ROUTINE_DBG_MEO00 ("ls04GetShortInfo");

    tsp00_Int4 ErrCode  = errUnknownPart_els98;                     // Initialize to error.
    tsp1_part  *SqlPart = NULL;

    if ( true == ls04GetPart(SqlPacket, sp1pk_shortinfo, SqlPart) ) // ATTENTION: returns a BOOL !!!
    {
        FieldCount = ls04GetPartArgCount(SqlPart);                  // count of args = count of params
        if (FieldsAvail >= FieldCount)                              // the opposite should never happen
        {
            tsp00_Int4 PartOffset = 0;
            tsp00_Int2 i          = 0;
            tsp00_Int4 Length     = sizeof(tls00_ParamInfo);

            for (i; i < FieldCount; ++i)
            {
                Fields[i] = new tls00_ParamInfo;
                ls04BufFromPartEx(SqlPart, PartOffset, Length, (char*)Fields[i], Length);
                PartOffset += Length;
            }

            ErrCode = errOK_els00;
        }
        else    // in case of else the ErrCode has the right value
        {
            TRACE_PRNF_MLS99(("ls04GetShortInfo", "Too many arguments in part (%d): array too small (%d).\n",
                                                  FieldCount, FieldsAvail));
        }
    }
    else
    {
        TRACE_PRNF_MLS99(("ls04GetShortInfo", "No shortinfo part in packet.\n"));
    }

    return ErrCode;
}
// ls04GetShortInfo()


/*
  -----------------------------------------------------------------------------
  function:     ls04GetParseID
  -----------------------------------------------------------------------------
*/
int
ls04GetParseID(tsp1_packet *SqlPacket, tsp00_Addr pszParseId)
{
    ROUTINE_DBG_MEO00 ("ls04GetParseID");

    tsp00_Int4  ErrCode = errOK_els00;
    bool        rc      = true;

    tsp1_part   *SqlPart = NULL;

    rc = ls04GetPart(SqlPacket, sp1pk_parsid, SqlPart);
    if (false == rc)
    {
        rc = ls04GetPart(SqlPacket, sp1pk_parsid_of_select, SqlPart);
        if (false == rc)
        {
            TRACE_PRNF_MLS99(("ls04GetParseID", "No parseid part in packet.\n"));
            ErrCode = errUnknownPart_els98;
        }
    }

    if (true == rc)
    {
        tsp00_Int4 lPartLen = ls04GetPartLength(SqlPart);

        memset(pszParseId, 0, MAX_PARSEID_LENGTH_LS00 + 1);
        ls04BufFromPartEx(SqlPart, 0, lPartLen, pszParseId, MAX_PARSEID_LENGTH_LS00 + 1);
    }

    return ErrCode;
}
// ls04GetParseID()



/*
  -----------------------------------------------------------------------------
  function:     ls04GetColumnNamesRaw
  -----------------------------------------------------------------------------
*/
void
ls04GetColumnNamesRaw(tsp1_part            *pSqlPart,
                      tsp00_Int4            lSqlPartOffset,
                      tsp00_KnlIdentifierc *Fields[],
                      tsp00_Int2            FieldCount,
                      tls00_CodeType        CodeType)
{
    ROUTINE_DBG_MEO00 ("ls04GetColumnNamesRaw");

    tsp00_Int4           lNameLen = 0;
    tsp00_KnlIdentifierc myTempID;

    bool bUnicodeDB = (ctAscii_els00 != CodeType);

    for (tsp00_Int2 i = 0; i < FieldCount; ++i)
    {
        char cLength;
    
        ls04BufFromPartEx(pSqlPart, lSqlPartOffset, sizeof(char), &cLength, sizeof(char));
        lSqlPartOffset += sizeof(char);

        lNameLen = cLength;         // Implicit conversion from char to int4

        // temporarily save the column name
        ls04BufFromPartEx(pSqlPart, lSqlPartOffset, lNameLen, myTempID.asCharp(), sizeof(tsp00_KnlIdentifierc));
        lSqlPartOffset += lNameLen;

        myTempID[lNameLen] = 0;     // add trailing 0 for subsequent strchr

        // Return names in UTF8
        ls05EscapeQuotesInDBIdentifier(myTempID.asCharp(), lNameLen, Fields[i]->asCharp(), bUnicodeDB, DOUBLE_QUOTE_LS00);
    }   // end for (tsp00_Int2 i = 0; i < FieldCount; ++i)

}
// ls04GetColumnNamesRaw()


/*
  -----------------------------------------------------------------------------
  function:     ls04GetColumnNames
  -----------------------------------------------------------------------------
*/
int
ls04GetColumnNames(tsp1_packet          *SqlPacket,
                   tsp00_Int2            FieldsAvail,
                   tsp00_KnlIdentifierc *Fields[],
                   tsp00_Int2           &FieldCount)
{
    ROUTINE_DBG_MEO00 ("ls04GetColumnNames");

    tsp00_Int4 rc  = errOK_els00;                     // Initialize to error.
    tsp1_part  *SqlPart = NULL;

    if ( true == ls04GetPart(SqlPacket, sp1pk_columnnames, SqlPart) )
    {
        FieldCount = ls04GetPartArgCount(SqlPart);                  // count of args = count of params
        if (FieldsAvail >= FieldCount)                              // the opposite should never happen
        {
            for (tsp00_Int2 i = 0; i < FieldCount; ++i)
            {
                Fields[i] = new tsp00_KnlIdentifierc;
                memset(Fields[i], BLANK_LS00, sizeof(tsp00_KnlIdentifierc));
            }

            if (true == ls04IsDBUnicode(SqlPacket))
            {
                ls04GetColumnNamesRaw(SqlPart, 0, Fields, FieldCount, ctUCS2Native_els00);
            }
            else
            {
                ls04GetColumnNamesRaw(SqlPart, 0, Fields, FieldCount, ctAscii_els00);
            }

        }
        else    // in case of else the ErrCode has the right value
        {
            rc  = errUnknownPart_els98;
            TRACE_PRNF_MLS99(("ls04GetColumnNames", "Too many arguments in part (%d): array too small (%d).\n",
                                                    FieldCount, FieldsAvail));
        }
    }
    else
    {
        rc  = errUnknownPart_els98;
        TRACE_PRNF_MLS99(("ls04GetColumnNames", "No columnnames part in packet.\n"));
    }

    return rc;
}
// ls04GetColumnNames()


/*
  -----------------------------------------------------------------------------
  function:     ls04GetResultCount
  -----------------------------------------------------------------------------
*/
int ls04GetResultCount(tsp1_packet  *SqlPacket,
                       tsp00_Addr   *pszResultCount,
                       tsp00_Int4 & lResultCount)
{
    ROUTINE_DBG_MEO00 ("ls04GetResultCount");

    tsp00_Int4 ErrCode     = errOK_els00;
    tsp1_part  *SqlPart    = NULL;
    tsp00_Addr pszmyResCnt = NULL;

    if ( true == ls04GetPart(SqlPacket, sp1pk_resultcount, SqlPart) )
    {
        // Delivered resultcount has a length of 7 bytes; the first byte is the undef-byte
        // The returned raw pointer includes the undef byte; for the int value it is removed
        tsp00_Int4 lPartLen = ls04GetPartLength(SqlPart);

        pszmyResCnt = new char[mxsp_resnum];
        
        ls04BufFromPartEx(SqlPart, 0, lPartLen, pszmyResCnt, lPartLen);

        ErrCode = ls07NumberToInt(&pszmyResCnt[1], lResultCount);
        if (ErrCode != errOK_els00)
        {
            TRACE_PRNF_MLS99(("ls04GetResultCount", "Error converting resultcount number to int (Err%d).\n",
                                                    ErrCode));
            ErrCode = errUnknownPart_els98;     // Reset error code
        }
        else
        {
            if (NULL != pszResultCount)
            {
                *pszResultCount = pszmyResCnt;
            }
            else
            {
                delete [] pszmyResCnt;
                pszmyResCnt = NULL;
            }
        }
    }
    else
    {
        ErrCode = errUnknownPart_els98;
        TRACE_PRNF_MLS99(("ls04GetResultCount", "No resultcount part in packet.\n"));
    }

    return ErrCode;
}
// ls04GetResultCount()


/*
  -----------------------------------------------------------------------------
  function:     ls04GetBdinfo
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04GetBdinfo(tsp1_packet *pSqlPacket, tls00_BDInfo* pBDInfo)
{
    ROUTINE_DBG_MEO00 ("ls04GetBdinfo");

    tsp00_Int4  rc       = errOK_els00;
    tsp1_part  *pSqlPart = NULL;

    if ( false == ls04GetPart(pSqlPacket, sp1pk_bdinfo, pSqlPart) )
    {
        TRACE_PRNF_MLS99(("ls04GetBdinfo", "Error: no bdinfo part in packet.\n"));

        rc = errUnknownPart_els98;
    }
    else
    {
        pBDInfo->bdiLength_ls00 = ls04GetPartLength(pSqlPart);
        ls04BufFromPartEx(pSqlPart, 0, pBDInfo->bdiLength_ls00, pBDInfo->szbdiData_ls00, pBDInfo->bdiLength_ls00);
    }

    return rc;
}
// ls04GetBdinfo()


/*
  -----------------------------------------------------------------------------
  function:     ls04GetRawDataPtr
  -----------------------------------------------------------------------------

*/
tsp00_Int4
ls04GetRawDataPtr(tsp1_packet         *pSqlPacket,
                  tsp00_Addr          &pszDataPtr,
                  tsp00_Int4          &lDataLen,
                  tsp1_part_kind_Enum  PartKind)
{
    ROUTINE_DBG_MEO00 ("ls04GetRawDataPtr");

    tsp00_Int4  rc       = errOK_els00;
    tsp1_part  *pSqlPart = NULL;

    if ( false == ls04GetPart(pSqlPacket, PartKind, pSqlPart) )
    {
        TRACE_PRNF_MLS99(("ls04GetRawDataPtr", "Error: no part of part kind %d in packet.\n", PartKind));

        rc = errUnknownPart_els98;
    }
    else
    {
        lDataLen = ls04GetPartLength(pSqlPart);
        ls04BufPtrFromPart(pSqlPart, 0, pszDataPtr);
    }

    return rc;
}
// ls04GetRawDataPtr()


/*
  -----------------------------------------------------------------------------
  function:     ls04GetLONGDescriptor
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04GetLONGDescriptor(tsp1_packet             *pSqlPacket,
                      tsp00_Int4              lNNLongCnt,
                      tsp00_LongDescriptor *& pDescriptorArray,
                      tsp00_Int4              lStartMember)
{
    ROUTINE_DBG_MEO00 ("ls04GetLONGDescriptor");

    // This routine is used to get long descriptors for putval-packets as well as for
    // getval-packets. Both packets have to be handled sligtly different:
    
    // putval
    // Kernel sends only descriptors with the packet. Thus the vallen will always be 0 and
    // the offset is actually always set to
    //   Offset = LongDescSize_ls00 + 1 + pDesc->ld_vallen() = LongDescSize_ls00 + 1 which
    // corresponds to length of descriptors + length of undef-byte

    // getval
    // Kernel sends together with the descriptors the data, too. In case that all descriptors
    // fit into the packet we navigate including the length of sent data. If the data is too
    // long to fit all needed descriptors into the packet they are simply not send. This is
    // catched with the condition (Offset < lPartLen) because the Offset then computes to a
    // value gt part length.


    tsp00_Int4  rc       = errOK_els00;
    tsp00_Int4  lPartLen = 0;
    tsp1_part  *pPart    = NULL;

    if ( false == ls04GetPart(pSqlPacket, sp1pk_longdata, pPart) )
    {
        rc = errUnknownPart_els98;
    }
    else
    {
        lPartLen = ls04GetPartLength(pPart);

        tsp00_Int4            Offset = 1;               // jump over undef-signal
        tsp00_LongDescriptor *pDesc  = &pDescriptorArray[lStartMember];

        // first value is always a descriptor
        ls04BufFromPartEx(pPart, Offset, LongDescSize_ls00, REINTERPRET_CAST(char*, pDesc), LongDescSize_ls00);
        Offset += LongDescSize_ls00 + 1 + pDesc->ld_vallen();

        // get remaining descriptors if there are any
        for (int i = 1; ( (i < lNNLongCnt) && (Offset < lPartLen) ); ++i)
        {
            pDesc  = &pDescriptorArray[lStartMember + i];
            ls04BufFromPartEx(pPart, Offset, LongDescSize_ls00, REINTERPRET_CAST(char*, pDesc), LongDescSize_ls00);
            Offset += LongDescSize_ls00 + 1 + pDesc->ld_vallen();
        }
    }

    return rc;
}
//  ls04GetLONGDescriptor()
