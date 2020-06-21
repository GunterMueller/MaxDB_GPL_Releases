/* 
  -----------------------------------------------------------------------------
  module: vls04.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  SQL-Packet handling
                
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

#include <assert.h>

#include "gls00.h"
#include "hls04.h"
#include "hls05.h"
#include "hls99.h"
#include "hls98msg.h"

#include "hsp77.h"      // unicode printing interface
/*
  -----------------------------------------------------------------------------
  Chapter: global functions
  -----------------------------------------------------------------------------
 */


/*
  -----------------------------------------------------------------------------
  function:     ls04InitPacket
  -----------------------------------------------------------------------------
*/
void ls04InitPacket (tsp1_packet  *SqlPacket, tsp00_Int4  PacketSize)
{
    ROUTINE_DBG_MEO00 ("ls04InitPacket");

    // Set default values in right order
    SqlPacket->sp1_header.sp1h_mess_code    = csp_ascii;
    SqlPacket->sp1_header.sp1h_filler1      = 0;
    memset(SqlPacket->sp1_header.sp1h_appl_version, DEF_BYTE_LS00, 5);
    memset(SqlPacket->sp1_header.sp1h_application,  DEF_BYTE_LS00, 3);
    SqlPacket->sp1_header.sp1h_varpart_size = PacketSize - sizeof (tsp1_packet_header);
    SqlPacket->sp1_header.sp1h_varpart_len  = 0;
    SqlPacket->sp1_header.sp1h_filler2      = 0;
    SqlPacket->sp1_header.sp1h_no_of_segm   = 0;
    memset(SqlPacket->sp1_header.sp1h_filler3, DEF_BYTE_LS00, 8);
}
// ls04InitPacket()


/*
  -----------------------------------------------------------------------------
  function:     ls04InitSegment
  -----------------------------------------------------------------------------
*/
int ls04InitSegment (tsp1_packet             *SqlPacket,
                     tsp1_segment           **SqlSegment,
                     tsp1_segment_kind_Enum   SegmKind,
                     tsp1_producer_Enum       CmdProducer)
{
    ROUTINE_DBG_MEO00 ("ls04InitSegment");

    teo00_Int4    SegmOffset     = 0;
    teo00_Int4    iVarPartSize   = SqlPacket->sp1_header.sp1h_varpart_size;
    teo00_Int4    iVarPartLength = SqlPacket->sp1_header.sp1h_varpart_len;
    tsp1_segment *CurrSegm       = NULL;

    // Check free space in segment.
    if (iVarPartSize - iVarPartLength < sizeof (tsp1_segment_header))
    {
        TRACE_PRNF_MLS99(("ls04InitSegment", "Not enough free space in packet to initialize segment (%d bytes).",
                                             iVarPartSize - iVarPartLength));
        TRACE_PCKT_MLS99("ls04InitPart", SqlPacket);

        return errInitSegment_els98;
    }
    
    // compute segment OFFSET
    short i;
    for (i=0; i<SqlPacket->sp1_header.sp1h_no_of_segm; i++)
    {
        CurrSegm = (tsp1_segment*) &SqlPacket->sp1_varpart()[SegmOffset];
        SegmOffset += CurrSegm->sp1s_segm_len();
    }

    // Assign segment specific header values
    CurrSegm                     = (tsp1_segment*) &SqlPacket->sp1_varpart()[SegmOffset];
    CurrSegm->sp1s_segm_len()    = sizeof (tsp1_segment_header);
    CurrSegm->sp1s_segm_offset() = SegmOffset;
    CurrSegm->sp1s_no_of_parts() = 0;
    CurrSegm->sp1s_own_index()   = SqlPacket->sp1_header.sp1h_no_of_segm + 1;
    CurrSegm->sp1s_segm_kind().becomes(SegmKind);

    // Set default values
    if (sp1sk_cmd == SegmKind)         // message typ ist 'request'
    { 
        CurrSegm->sp1c_mess_type().becomes(sp1m_dbs);
        CurrSegm->sp1c_sqlmode().becomes(sp1sm_session_sqlmode);
        CurrSegm->sp1c_producer().becomes(CmdProducer);
        
        // Default for commit immediately is TRUE.
        // This can be changed with the user command AUTOCOMMIT ON/OFF (takes effect in ls04FastInit)
        CurrSegm->sp1c_commit_immediately() = true;
        CurrSegm->sp1c_ignore_costwarning() = false;
        CurrSegm->sp1c_prepare()            = false;
        CurrSegm->sp1c_with_info()          = false;
        CurrSegm->sp1c_mass_cmd()           = false;
        CurrSegm->sp1c_parsing_again()      = false;
        CurrSegm->sp1c_command_options().clear();
        CurrSegm->sp1c_filler1()            = DEF_BYTE_LS00;
        memset(CurrSegm->sp1c_filler2(), DEF_BYTE_LS00, 8);
        memset(CurrSegm->sp1c_filler3(), DEF_BYTE_LS00, 8);
    }
    else                                   // message typ ist 'result'
    {
        memset(CurrSegm->sp1r_sqlstate(), '0', 5);
        CurrSegm->sp1r_returncode()     = 0;
        CurrSegm->sp1r_errorpos()       = 0;
        CurrSegm->sp1r_extern_warning().clear();
        CurrSegm->sp1r_intern_warning().clear();
        CurrSegm->sp1r_function_code()  = 0;
        CurrSegm->sp1r_filler1()        = 0;
        memset (CurrSegm->sp1c_filler2(), DEF_BYTE_LS00, 8);
    }

    // Let SqlSegment now point to the initialized segment
    *SqlSegment = CurrSegm;

    // Adjust packet informations for the segment
    SqlPacket->sp1_header.sp1h_varpart_len += (*SqlSegment)->sp1s_segm_len();
    SqlPacket->sp1_header.sp1h_no_of_segm  += 1;    

    return errOK_els00;
}
// ls04InitSegment()


/*
  -----------------------------------------------------------------------------
  function:     ls04InitPart
  -----------------------------------------------------------------------------
*/
int ls04InitPart (tsp1_packet          *SqlPacket,
                  tsp1_segment         *SqlSegment,
                  tsp1_part           **SqlPart,
                  tsp1_part_kind_Enum   PartKind)
{
    ROUTINE_DBG_MEO00 ("ls04InitPart");

    tsp00_Int4      PartOffset   = 0;
    tsp00_Int4      iVarPartSize = SqlPacket->sp1_header.sp1h_varpart_size;
    tsp00_Int4      iVarPartLen  = SqlPacket->sp1_header.sp1h_varpart_len;
    tsp1_part       *CurrPart;

    // check free space
    if ( (iVarPartSize - iVarPartLen) < sizeof(tsp1_part_header))
    {
        TRACE_PRNF_MLS99(("ls04InitPart", "Not enough free space in packet to initialize first part (%d bytes).",
                                          iVarPartSize - iVarPartLen));

        TRACE_PCKT_MLS99 ("ls04InitPart", SqlPacket);
        return errInitPart_els98;
    }

    // invalid initialization ?
    if (SqlSegment->sp1s_own_index() != SqlPacket->sp1_header.sp1h_no_of_segm)
    {
        TRACE_PRNF_MLS99(("ls04InitPart", "Wrong part in this segment"));
        TRACE_PCKT_MLS99 ("ls04InitPart", SqlPacket);
        return errInitPart_els98;
    }

    // compute part offset
    PartOffset = SqlSegment->sp1s_segm_offset() + SqlSegment->sp1s_segm_len();

    // Assign pointer to segment pointer in the packet
    CurrPart = (tsp1_part*) &SqlPacket->sp1_varpart()[PartOffset];

    // Assign part specific header values
    CurrPart->sp1p_part_kind().becomes(PartKind);
    CurrPart->sp1p_segm_offset()    = SqlSegment->sp1s_segm_offset();
    CurrPart->sp1p_buf_len()        = 0;
    CurrPart->sp1p_buf_size()       = iVarPartSize - PartOffset - sizeof (tsp1_part_header);

    // Set DEFAULT values
    CurrPart->sp1p_attributes().clear();
    CurrPart->sp1p_arg_count()      = 1;

    *SqlPart = CurrPart;                                        // Reassign pointer to current part
    SqlSegment->sp1s_segm_len()     += sizeof(tsp1_part_header);
    SqlSegment->sp1s_no_of_parts()  += 1;
    SqlPacket->sp1_header.sp1h_varpart_len += sizeof(tsp1_part_header);

    return errOK_els00;
}
// ls04InitPart()


/*
  -----------------------------------------------------------------------------
  function:     ls04FinishPart - overloaded
  -----------------------------------------------------------------------------
*/
int ls04FinishPart(tsp1_packet *SqlPacket, tsp1_part *SqlPart)
{
    ROUTINE_DBG_MEO00 ("ls04FinishPart");
    
    tsp00_Int4 rc = errOK_els00;

    // part header is always aligned (16 bytes)
    tsp00_Int4 AlignedLen = ALIGN_8BYTE_EO00(SqlPart->sp1p_buf_len());

    TRACE_PRNF_MLS99(("ls04FinishPart", "part length = %d, aligned length = %d\n",
                                        SqlPart->sp1p_buf_len(), AlignedLen));

    // Check free space
    if ( (AlignedLen - SqlPart->sp1p_buf_len()) > SqlPart->sp1p_buf_size() )
    {
        rc = errFinishPart_els98;
    }
    else
    {
        // Refresh segment and packet length values to the aligned length
        tsp1_segment *pSqlSegment = (tsp1_segment*)&SqlPacket->sp1_varpart()[SqlPart->sp1p_segm_offset()];
    
        pSqlSegment->sp1s_segm_len()           += AlignedLen;
        SqlPacket->sp1_header.sp1h_varpart_len += AlignedLen;
    }

    return rc;
}
// ls04FinishPart()


/*
  -----------------------------------------------------------------------------
  function:     ls04FinishPart - overloaded
  -----------------------------------------------------------------------------
*/
int ls04FinishPart(tsp1_packet *SqlPacket, tsp1_part *SqlPart, tsp00_Int4 FreeSpace)
{
    ROUTINE_DBG_MEO00 ("ls04FinishPart(FreeSpace)");
    
    tsp00_Int4 rc = errOK_els00;

    SqlPart->sp1p_buf_len() += FreeSpace;       // Assign length to leave space
    
    // part header is always aligned (16 bytes)
    tsp00_Int4 AlignedLen = ALIGN_8BYTE_EO00(SqlPart->sp1p_buf_len());

    TRACE_PRNF_MLS99(("ls04FinishPart(FreeSpace)", "part length = %d, assigned length = %d\n",
                                                   SqlPart->sp1p_buf_len(),
                                                   AlignedLen));

    // Check free space
    if ( (AlignedLen - SqlPart->sp1p_buf_len()) > SqlPart->sp1p_buf_size() )
    {
        rc = errFinishPart_els98;
    }
    else
    {
        // Refresh segment and packet length values to the aligned length
        tsp1_segment *pSqlSegment = (tsp1_segment*)&SqlPacket->sp1_varpart()[SqlPart->sp1p_segm_offset()];
    
        pSqlSegment->sp1s_segm_len()            += AlignedLen;
        SqlPacket->sp1_header.sp1h_varpart_len  += AlignedLen;
    }

    return rc;
}
//  ls04FinishPart(FreeSpace)


/*
  -----------------------------------------------------------------------------
  function:     ls04FastInit
  -----------------------------------------------------------------------------
*/
int ls04FastInit(tsp1_packet         *SqlPacket, 
                 tsp1_segment       **SqlSegm, 
                 tsp1_part          **SqlPart, 
                 tls00_DBInfo        *DBInfo,
                 tsp1_producer_Enum   CmdProducer)    // default = sp1pr_internal_cmd
{
    ROUTINE_DBG_MEO00 ("ls04FastInit");

    SAPDB_memcpy(REINTERPRET_CAST(char*, &SqlPacket->sp1_header),
                 DBInfo->dbiPktInitBlock_ls00,
                 sizeof(tsp1_packet_header));
    SAPDB_memcpy(REINTERPRET_CAST(char*, &SqlPacket->sp1_varpart()),
                 DBInfo->dbiPktInitBlock_ls00 + sizeof(tsp1_packet_header),
                 sizeof(tsp1_segment_header) + sizeof(tsp1_part_header));

    *SqlSegm = &SqlPacket->sp1_segm();
    *SqlPart = &SqlPacket->sp1_segm().sp1p_part();

    if (true == ls00UseUnicodeOI(DBInfo))
    {
        if ( (sw_full_swapped == DBInfo->dbiOwnSwap_ls00) ||
             (sw_part_swapped == DBInfo->dbiOwnSwap_ls00) )
        {
            SqlPacket->sp1_header.sp1h_mess_code = csp_unicode_swap;
        }
        else
        {
            SqlPacket->sp1_header.sp1h_mess_code = csp_unicode;
        }
    }

    //*
    //*     Reset producer in segment if necessary
    //* The function ls04InitSegment sets per default the user to sp1pr_user_cmd
    //*
    //* All internal commands are send with sp1pr_internal_cmd. 
    //* All external commands are send with sp1pr_user_cmd.
    //* In any case will the producer be set to installation if the flag 
    //* 'installation on' is set.

    if (true == DBInfo->dbiSession_ls00.siInstallOn_ls00)
    {
        // In this case the statement can access system tables
        (*SqlSegm)->sp1c_producer().becomes(sp1pr_installation);
    }
    else
    {
        if (sp1pr_user_cmd != CmdProducer)  // sp1pr_user_cmd is the default
        {
            (*SqlSegm)->sp1c_producer().becomes(CmdProducer);
        }
    }

    
    if (sp1pr_user_cmd == CmdProducer)      // external commands
    {
        ls04SetSqlMode(*SqlSegm, DBInfo->dbiSession_ls00.siSQLMode_ls00);
        ls04SetACFlag(*SqlSegm, DBInfo->dbiSession_ls00.siAutoCommit_ls00);
    }
    else
    {
        // Internal commands:   Sql mode = INTERNAL and
        //                      commit immediately flag = OFF
        ls04SetSqlMode(*SqlSegm, sp1sm_internal);
        ls04SetACFlag(*SqlSegm, false);
    }

    return errOK_els00;
}
//  ls04FastInit()



/*!
  -----------------------------------------------------------------------------
  function:     ls04CharToPart
  -----------------------------------------------------------------------------
*/
tsp00_Int2 ls04CharToPart(tsp1_part *SqlPart, char PutChar)
{
    ROUTINE_DBG_MEO00 ("ls04CharToPart");

    teo00_Int2  rc = errOK_els00;

    if (SqlPart->sp1p_buf_len() >= SqlPart->sp1p_buf_size())
    {
        rc = errWriteBuf2Part_els98;
    }
    else
    {
        SqlPart->sp1p_buf()[SqlPart->sp1p_buf_len()] = PutChar;
        SqlPart->sp1p_buf_len() += 1;
    }

    return rc;
}
// ls04CharToPart()


/*
  -----------------------------------------------------------------------------
  function:     ls04BufToPart
  -----------------------------------------------------------------------------
*/
tsp00_Int2 ls04BufToPart(tsp1_part *SqlPart, tsp00_Addr Buffer, tsp00_Int4 BufLen)
{
    ROUTINE_DBG_MEO00 ("ls04BufToPart");

    assert(BufLen >= 0);
    assert(&SqlPart->sp1p_buf()[SqlPart->sp1p_buf_len()] != NULL);
    assert(Buffer != NULL);

    tsp00_Int2  rc = errOK_els00;
    
    if (BufLen > SqlPart->sp1p_buf_size() - SqlPart->sp1p_buf_len() )
    {
        rc = errWriteBuf2Part_els98;
    }
    else
    {
        SAPDB_memcpy(&SqlPart->sp1p_buf()[SqlPart->sp1p_buf_len()], Buffer, BufLen);
        SqlPart->sp1p_buf_len() += BufLen;
    }

    return rc;
}
// ls04BufToPart()


/*
  -----------------------------------------------------------------------------
  function:     ls04BufFromPart
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04BufFromPart(tsp1_packet         *SqlPacket,
                tsp1_part_kind_Enum  PartKind_esp01,
                tsp00_Int4           Offset,
                tsp00_Int4           Count,
                tsp00_Addr           Buffer,
                tsp00_Int4           BufLen)
{
    ROUTINE_DBG_MEO00 ("ls04BufFromPart");

    tsp00_Int4 rc = errOK_els00;
    
    tsp1_part*  pSqlPart    = NULL;
    bool        bPartFound  = false;

    bPartFound = ls04GetPart(SqlPacket, PartKind_esp01, pSqlPart);
    if (true == bPartFound)
    {
        // Read the requested number of characters (sp1p_buf points BEHIND part header here)
        SAPDB_memcpy(Buffer, &pSqlPart->sp1p_buf()[Offset], Count);
    }
    if (false == bPartFound)
    {
        TRACE_PRNF_MLS99( ("ls04BufFromPart", "Part of part kind %d not found in packet\n",
                                              STATIC_CAST(int, PartKind_esp01)) );
        TRACE_PCKT_MLS99 ("ls04BufFromPart", SqlPacket);
        
        rc = errUnknownPart_els98;
    }
    return rc;
}
//  ls04BufFromPart()


/*
  -----------------------------------------------------------------------------
  function:     ls04GetPart
  -----------------------------------------------------------------------------
*/
bool ls04GetPart(tsp1_packet          *SqlPacket,
                 tsp1_part_kind_Enum   PartKind,
                 tsp1_part           *&SqlPart)
{
    ROUTINE_DBG_MEO00 ("ls04GetPart");

    tsp00_Int4 PartOffset = sizeof(tsp1_segment_header);
    tsp00_Int2 PartNo     = SqlPacket->sp1_segm().sp1s_no_of_parts();
    bool       bFound     = false;
    
    // Find part in the packet and set part pointer to this position
    for (int i=0; i < PartNo; ++i) 
    {
        SqlPart = REINTERPRET_CAST(tsp1_part*, &SqlPacket->sp1_varpart()[PartOffset]);
        if (PartKind == SqlPart->sp1p_part_kind())
        {
            bFound = true;
            break;
        }
        else    // Move on to the next part
        {
            PartOffset += ALIGN_8BYTE_EO00(sizeof(tsp1_part_header) + SqlPart->sp1p_buf_len());
        }
    }

    return (bFound);
}
//  ls04GetPart()


/*
  -----------------------------------------------------------------------------
  function:     ls04RcFromPacket - overloaded
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls04RcFromPacket(tsp1_packet *SqlPacket, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls04RcFromPacket");

    tsp00_Int4 lErrorPos = 0;

    return (ls04RcFromPacket(SqlPacket, ErrText, lErrorPos));
}
//  ls04RcFromPacket()


/*
  -----------------------------------------------------------------------------
  function:     ls04RcFromPacket - overloaded
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04RcFromPacket(tsp1_packet *SqlPacket, tsp00_Addr ErrText, tsp00_Int4 &ErrorPos)
{
    ROUTINE_DBG_MEO00 ("ls04RcFromPacket");

    tsp1_segment *SqlSegm = &SqlPacket->sp1_segm();
    tsp1_part    *SqlPart = NULL;
    tsp00_Int4    ErrCode = SqlSegm->sp1r_returncode();

    if (0 == ErrCode)   // corresponds to errOK_els00
    {
        TRACE_PRNF_MLS99(("ls04RcFromPacket", "No Error in recieve-packet.\n"));        
    }
    else           // In case the ErrCode != 0 we have in any case a part of part kind errortext!
    {
        tsp00_Int4 lMaxBufLen = 256;
        ErrorPos              = SqlSegm->sp1r_errorpos();        // Get the error position to return

        if (true == ls04GetPart(SqlPacket, sp1pk_errortext, SqlPart) )
        {
            tsp00_Int4 len = ls04GetPartLength(SqlPart);
            tsp00_Int4 pos = 0;

            // We write everything in utf8
            if ( (csp_unicode_swap == SqlPacket->sp1_header.sp1h_mess_code) || 
                 (csp_unicode      == SqlPacket->sp1_header.sp1h_mess_code)  )
            {
                pos = sp77sprintfUnicode(sp77encodingUTF8, ErrText, lMaxBufLen, SQL_ERRTEXTU_LS04,
                                                                                STATIC_CAST(int, ErrCode),
                                                                                len / 2,
                                                                                SqlPart->sp1p_buf().asCharp());
                ls05StrTrimRight(ErrText, pos);

                pos = sp77sprintfUnicode(sp77encodingUTF8, ErrText + pos, lMaxBufLen, SQL_ERRPOS_TEXT_LS04,
                                                                                      ErrorPos);
            }
            else if (csp_ascii == SqlPacket->sp1_header.sp1h_mess_code)
            {
                pos = sp77sprintfUnicode(sp77encodingUTF8, ErrText + pos, lMaxBufLen, SQL_ERRTEXTA_LS04,
                                                                                      STATIC_CAST(int, ErrCode),
                                                                                      len,
                                                                                      SqlPart->sp1p_buf().asCharp());
                ls05StrTrimRight(ErrText, pos);

                pos = sp77sprintfUnicode(sp77encodingUTF8, ErrText + pos, lMaxBufLen, SQL_ERRPOS_TEXT_LS04,
                                                                                      ErrorPos);
            }

        }
        else
        {
            sp77sprintf(ErrText, lMaxBufLen, "SQL error %i (error position: %d)\n", STATIC_CAST(int, ErrCode),
                                                                                    ErrorPos);
        }
    }

    return ErrCode;
}
//  ls04RcFromPacket()

