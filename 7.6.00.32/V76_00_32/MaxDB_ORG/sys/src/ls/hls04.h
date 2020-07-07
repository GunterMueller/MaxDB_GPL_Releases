/*! 
  -----------------------------------------------------------------------------
 
  module: hls04.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Packets, segments and parts
                
  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1997-2005 SAP AG-2002
 
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
#ifndef HLS04
#define HLS04

#include "gls00.h"

#define  SQL_ERRTEXTA_LS04   _T("SQL error %i = %.*s")
#define  SQL_ERRTEXTU_LS04   _T("SQL error %i = %.*S")

#define  SQL_ERRPOS_TEXT_LS04 _T(" (error position: %d)")
/*!
  -----------------------------------------------------------------------------
 
  function:     ls04InitPacket

  -----------------------------------------------------------------------------

  description:  Initializes default values for packet header

  arguments:    SqlPacket  [IN]  - packet to be initialized
                PacketSize [IN]  - packet size

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void ls04InitPacket(tsp1_packet *SqlPacket, tsp00_Int4 PacketSize);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04InitSegment

  -----------------------------------------------------------------------------

  description:  Fill header of first segment of a packet (message).

  arguments:    SqlPacket   [IN]   - packet to fill
                SqlSegment  [OUT]  - segment to initialize (created here)
                SegmKind    [IN]   - segment type
                CmdProducer [IN]   - type of producer (defaults to sp1pr_user_cmd)

  returnvalue:  errOK_els00          - everything went fine
                errInitSegment_els98 - not enough memory to initialize
  -----------------------------------------------------------------------------
*/
int ls04InitSegment (tsp1_packet             *SqlPacket,
                     tsp1_segment           **SqlSegment,
                     tsp1_segment_kind_Enum   SegmKind,
                     tsp1_producer_Enum       CmdProducer = sp1pr_user_cmd);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04InitPart
  -----------------------------------------------------------------------------

  description:  Fill part header of a segment of a packet (message).

  arguments:    SqlPacket  [IN]   - packet to fill
                SqlSegment [IN]   - segment in packet
                SqlPart    [OUT]  - part to initialized (created here)
                PartKind   [IN]   - part type requested

  returnvalue:  errOK_els00       - everything went fine
                errInitPart_els98 - not enough memory to initialize or wrong part type
  -----------------------------------------------------------------------------
*/
int ls04InitPart (tsp1_packet           *SqlPacket,
                  tsp1_segment          *SqlSegment,
                  tsp1_part             **SqlPart,
                  tsp1_part_kind_Enum   PartKind);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04FinishPart - overloaded
  -----------------------------------------------------------------------------

  description:  Finish part in a segment and align border to 8 bytes.

  arguments:    SqlPacket  [IN]  - packet
                SqlPart    [IN]  - part to finish

  returnvalue:  errOK_els00         - everything went fine
                errFinishPart_els98 - not enough free space in part
                                       to finish it correctly
  -----------------------------------------------------------------------------
*/
int ls04FinishPart(tsp1_packet *SqlPacket, tsp1_part *SqlPart);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04FinishPart - overloaded

  -----------------------------------------------------------------------------

  description:  Finish part in a segment and align border to 8 bytes.

  arguments:    SqlPacket  [IN]  - packet
                SqlPart    [IN]  - part to finish
                FreeSpace  [IN]  - space to leave free in part

  returnvalue:  errOK_els00         - everything went fine
                errFinishPart_els98 - not enough free space in part
                                       to finish it correctly
  -----------------------------------------------------------------------------
*/
int ls04FinishPart(tsp1_packet *SqlPacket, tsp1_part *SqlPart, tsp00_Int4 FreeSpace);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04FastInit
  -----------------------------------------------------------------------------

  description:  Initialize packet using information from first initialization.
                Switches the producer to sp1pr_installation if INSTALLATION ON
                is signaled in DBInfo structure.
                Switches autocommit dependend on the value of the autocommit flag
                in DBInfo structure.
                Switches producer if desired (necessary for installation).
                Sets the sql mode.

  arguments:    SqlPacket    [IN]   - packet
                SqlSegment   [OUT]  - segment to create
                SqlPart      [OUT]  - part to create
                DBInfo       [IN]   - structure with info on the running session
                                       and the database connected to.
                CmdProducer  [IN]   - Flags the producer - may be sp1pr_internal_cmd
                                       or sp1pr_user_cmd (the latter for external
                                       cmds only).
                                       Defaults to sp1pr_internal_cmd.

  returnvalue:  errOK_els00           everything went fine
  -----------------------------------------------------------------------------
*/
int ls04FastInit(tsp1_packet         *SqlPacket, 
                 tsp1_segment       **SqlSegm, 
                 tsp1_part          **SqlPart, 
                 tls00_DBInfo        *DBInfo,
                 tsp1_producer_Enum   CmdProducer = sp1pr_internal_cmd);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04CharToPart
  -----------------------------------------------------------------------------

  description:  Write character to part and adjust the length of the part 
                respectively

  arguments:    SqlPart  [IN]  - part to fill
                PutChar  [IN]  - character to put in

  returnvalue:  errOK_els00            - everything went fine
                errWriteBuf2Part_els98 - no more space left in part to put
                                          character in
  -----------------------------------------------------------------------------
*/
tsp00_Int2 ls04CharToPart(tsp1_part *SqlPart, char PutChar);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04BufToPart
  -----------------------------------------------------------------------------

  description:  Write character buffer to part and adjust the length of the part 
                respectively

  arguments:    SqlPart  [IN]  - part to fill
                Buffer   [IN]  - character buffer to put in
                BufLen   [IN]  - Length of character buffer

  returnvalue:  errOK_els00            -  everything went fine
                errWriteBuf2Part_els98 - no more space left in part to put
                                          character in
  -----------------------------------------------------------------------------
*/
tsp00_Int2 ls04BufToPart(tsp1_part *SqlPart, tsp00_Addr Buffer, teo00_Int4 BufLen);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04BufFromPart

  -----------------------------------------------------------------------------

  description:  Read an array of characters out of a part characterized
                by a special part kind.

  arguments:    SqlPacket  [IN]      - packet
                iPartKind  [IN]      - part kind of part to read from
                Offset     [IN]      - starting point of the array to read in
                                        the part excluding (!) the part header
                Count      [IN]      - number of bytes to read
                Buffer     [IN|OUT]  - character buffer
                BufLen     [IN]      - length of character buffer

  returnvalue:  errOK_els00          - everything went fine
                errUnknownPart_els98 - no part of desired part kind in packet
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls04BufFromPart(tsp1_packet         *SqlPacket,
                           tsp1_part_kind_Enum  PartKind_esp01,
                           tsp00_Int4           Offset,
                           tsp00_Int4           Count,
                           tsp00_Addr           Buffer,
                           tsp00_Int4           BufLen);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04GetPart
  -----------------------------------------------------------------------------

  description:  Checks a packet for a part of a certain part kind.
                To read data from the part use ls04BufFromPartEx.

  arguments:    SqlPacket       [IN]   - packet
                PartKind_esp01  [IN]   - part kind of part to find in packet
                SqlPart         [OUT]  - pointer to the found part
                                          If the function returns false the pointer
                                          may not be of any use.

  returnvalue:  true   - everything went fine
                false  - part not found
  -----------------------------------------------------------------------------
*/
bool ls04GetPart(tsp1_packet           *SqlPacket,
                 tsp1_part_kind_Enum    PartKind_esp01,
                 tsp1_part            *&SqlPart);


/*!
  -----------------------------------------------------------------------------

  function:     ls04RcFromPacket - overloaded
  -----------------------------------------------------------------------------

  description:  Returns return code delivered by a reply packet. The caller has
                to make sure that the error text buffer has at least a size
                of 256 byte (not checked by the function).

  arguments:    SqlPacket [IN]  - reply packet to check
                ErrText   [OUT] - error text buffer (null terminated); filled
                                   only if kernel signals error in packet;
                                   returned text has the form:
                                   "SQL error <E number> = <E text> (error position: <E pos>)"

  returnvalue:  errOK_els00 - No error
                ErrCode     - Return code delivered by the reply packet
                               (kernel error)
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04RcFromPacket(tsp1_packet *SqlPacket, tsp00_Addr ErrText);



/*!
  -----------------------------------------------------------------------------
 
  function:     ls04RcFromPacket - overloaded
  -----------------------------------------------------------------------------

  description:  Returns return code delivered by a reply packet.  The caller has
                to make sure that the error text buffer has at least a size
                of 256 byte (not checked by the function).

  arguments:    SqlPacket [IN]  - reply packet to check
                ErrText   [OUT] - error text buffer (null terminated); filled
                                   only if kernel signals error in packet;
                                   returned text has the form:
                                   "SQL error <E number> = <E text> (error position: <E pos>)"
                ErrorPos  [OUT] - position of the part in the sent statement
                                   that caused the error; of no use if no error

  returnvalue:  errOK_els00 - No error
                ErrCode     - Return code delivered by the reply packet
                               (kernel error)
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04RcFromPacket(tsp1_packet  *SqlPacket, tsp00_Addr  ErrText, tsp00_Int4 & ErrorPos);


/*!
  -----------------------------------------------------------------------------

  function:     ls04BufPtrFromPart - inline
  -----------------------------------------------------------------------------

  description:  Delivers the pointer to the buf of a certain known part.
                To execute this function one MUST first execute ls04GetPart.
                Otherwise the result is unknown.

                The caller has to make sure that OFFSET is not out of range,
                i.e. OFFSET !> length of part buffer.

  arguments:    SqlPart    [IN]      - known part
                Offset     [IN]      - starting point of the array to read
                                        in the part excluding (!) the part header
                pszBuffer  [IN|OUT]  - pointer to the requested portion of
                                        the part

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
ls04BufPtrFromPart(tsp1_part   *SqlPart,
                   tsp00_Int4   Offset,
                   _TCHAR     *&pszBuffer)
{
    pszBuffer = &SqlPart->sp1p_buf()[Offset];       // sp1p_buf points BEHIND part header here
}



/*!
  -----------------------------------------------------------------------------

  function:     ls04BufFromPartEx - inline
  -----------------------------------------------------------------------------

  description:  Reads from a certain known part.
                To execute this function one MUST first execute ls04GetPart.
                Otherwise the result is unknown.

  arguments:    SqlPart  [IN]      - known part
                Offset   [IN]      - starting point of the array to read in
                                      the part excluding (!) the part header
                Count    [IN]      - number of bytes to read
                Buffer   [IN|OUT]  - character buffer
                BufLen   [IN]      - length of character buffer

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
ls04BufFromPartEx(tsp1_part  *SqlPart,
                  tsp00_Int4  Offset,
                  tsp00_Int4  Count,
                  tsp00_Addr  Buffer,
                  tsp00_Int4  BufLen)
{
    // Copy requested number of characters (sp1p_buf points BEHIND part header here)
    // but make sure to copy not more than fitting into buffer
    tsp00_Longint lCnt = (Count <= BufLen) ? Count : BufLen;
    SAPDB_memcpy(Buffer, &SqlPart->sp1p_buf()[Offset], lCnt);
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04GetPartLength - inline
  -----------------------------------------------------------------------------

  description:  Returns length of a part of a certain part kind.
                To do so one MUST execute ls04GetPart first. Otherwise the
                behavior is undefined.

  arguments:    SqlPart  [IN]  - part

  returnvalue:  Length of the part
  -----------------------------------------------------------------------------
*/
inline tsp00_Int4 ls04GetPartLength(tsp1_part *SqlPart)
{
    return SqlPart->sp1p_buf_len();
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04GetPartArgCount - inline

  -----------------------------------------------------------------------------

  description:  Returns count of arguments in a part of a certain part kind.
                To do so one MUST execute ls04GetPart first. Otherwise the
                behavior is undefined.

  arguments:    SqlPart  [IN]  - part

  returnvalue:  argument count
  -----------------------------------------------------------------------------
*/
inline tsp00_Int2  ls04GetPartArgCount(tsp1_part *SqlPart)
{
    return SqlPart->sp1p_arg_count();
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetACFlag   - inline
  -----------------------------------------------------------------------------

  description:  Sets the AUTOCOMMIT-flag after initializing the packet

  arguments:    SqlSegm     [IN]  - segment where to set the flag
                bAutoCommit [IN]  - Value to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetACFlag(tsp1_segment *SqlSegm, bool bAutoCommit)
{
    SqlSegm->sp1c_commit_immediately() = bAutoCommit;
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetProducer - inline
  -----------------------------------------------------------------------------

  description:  Sets the producer after initializing the packet

  arguments:    SqlSegm   [IN]  - segment where to set the flag
                Producer  [IN]  - Value to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetProducer(tsp1_segment *SqlSegm, tsp1_producer_Enum Producer)
{
    SqlSegm->sp1c_producer().becomes(Producer);
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetMCFlag   - inline
  -----------------------------------------------------------------------------

  description:  Sets the mass command flag after initializing the packet

  arguments:    SqlSegm   [IN]  - segment where to set the flag
                bMassCmd  [IN]  - Value to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetMCFlag(tsp1_segment *SqlSegm, bool bMassCmd)
{
    SqlSegm->sp1c_mass_cmd() = bMassCmd;
}


/*!
  -----------------------------------------------------------------------------

  function:     ls04SetPrepareFlag  - inline
  -----------------------------------------------------------------------------

  description:  Sets the prepare flag after initializing the packet

  arguments:    SqlSegm   [IN]  - segment where to set the flag
                bPrepare  [IN]  - Value to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetPrepareFlag(tsp1_segment *SqlSegm, bool bPrepare)
{
    SqlSegm->sp1c_prepare()= bPrepare;
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetMsgType  - inline

  -----------------------------------------------------------------------------

  description:  Sets the message type after initializing the packet

  arguments:    SqlSegm   [IN]  - segment where to set the flag
                MessType  [IN]  - Value to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetMsgType(tsp1_segment *SqlSegm, tsp1_cmd_mess_type_Enum MessType)
{
    SqlSegm->sp1c_mess_type().becomes(MessType);
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetWInfoFlag    - inline
  -----------------------------------------------------------------------------

  description:  Sets the with info flag after initializing the packet

  arguments:    SqlSegm    [IN]  - segment where to set the flag
                bWithInfo  [IN]  - Value to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetWInfoFlag(tsp1_segment *SqlSegm, bool bWithInfo)
{
    SqlSegm->sp1c_with_info() = bWithInfo;
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04ClearPartAttribute    - inline
  -----------------------------------------------------------------------------

  description:  Clears all attributes of a part.

  arguments:    SqlPart  [IN]  - part where to clear

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04ClearPartAttribute(tsp1_part *SqlPart)
{
    SqlPart->sp1p_attributes().clear();
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04AddPartAttribute    - inline
  -----------------------------------------------------------------------------

  description:  Adds an attribute to a part

  arguments:    SqlPart        [IN]  - part where to add

                PartAttribute  [IN]  - Attribute to add

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04AddPartAttribute(tsp1_part *SqlPart, tsp1_part_attributes_Enum PartAttribute)
{
    if (false == SqlPart->sp1p_attributes().includes(PartAttribute) )
    {
        SqlPart->sp1p_attributes().addElement(PartAttribute);
    }
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetPartAttribute    - inline
  -----------------------------------------------------------------------------

  description:  Sets the requested attribute within a part. Before doing so
                it clears all other attributes.

  arguments:    SqlPart        [IN]  - part where to add

                PartAttribute  [IN]  - Attribute to set

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
ls04SetPartAttribute(tsp1_part *SqlPart, tsp1_part_attributes_Enum PartAttribute)
{
    SqlPart->sp1p_attributes().clear();
    SqlPart->sp1p_attributes().addElement(PartAttribute);
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetSqlMode    - inline
  -----------------------------------------------------------------------------

  description:  Sets the sql mode flag after initializing the packet

  arguments:    SqlSegm  [IN]  - segment where to set the flag
                SqlMode  [IN]  - Value to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetSqlMode(tsp1_segment *SqlSegm, tsp1_sqlmode_Enum SqlMode)
{
    SqlSegm->sp1c_sqlmode().becomes(SqlMode);
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04SetPartKind    - inline
  -----------------------------------------------------------------------------

  description:  Sets the part kind of an already initialized part.

  arguments:    pSqlPart [IN]  - part where to set the flag
                PartKind [IN]  - Part kind to set.

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void ls04SetPartKind(tsp1_part *pSqlPart, tsp1_part_kind_Enum PartKind)
{
    pSqlPart->sp1p_part_kind().becomes(PartKind);
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04GetSegment    - inline
  -----------------------------------------------------------------------------

  description:  Returns the pointer to the segment in a packet.

  arguments:    pSqlPacket [IN]  - packet that contains the segment

  returnvalue:  pointer to segment
  -----------------------------------------------------------------------------
*/
inline tsp1_segment*
ls04GetSegment(tsp1_packet *pSqlPacket)
{
    return &pSqlPacket->sp1_segm();
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04IsDBUnicode
  -----------------------------------------------------------------------------

  description:  Tests if db is UNICODE. To do so at least a connection 
                to the db must be established.

  arguments:    pSqlPacket [IN]  - pointer to message packet

  returnvalue:  true    - db is unicode
                false   - the opposite
  -----------------------------------------------------------------------------
*/
inline bool
ls04IsDBUnicode(tsp1_packet_ptr pSqlPacket)
{
    return ( (csp_unicode_swap   == pSqlPacket->sp1_header.sp1h_mess_code) ||
             (csp_unicode        == pSqlPacket->sp1_header.sp1h_mess_code) ||
             (csp_unicode_native == pSqlPacket->sp1_header.sp1h_mess_code) );
}

#endif /*HLS04*/
