/*! 
  -----------------------------------------------------------------------------
  module: hls041.h
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

#ifndef HLS041
#define HLS041

#include "gls00.h"


/*!
  -----------------------------------------------------------------------------

  function:     ls04GetShortInfo
  -----------------------------------------------------------------------------

  description:  Gets short info for count of parameters out of an answer packet.
                The function allocates space for the Fields array members that
                has to be freed by the caller afterwards. In case of error no
                space is allocated.

  arguments:    SqlPacket    [IN]    - packet
                FieldsAvail  [IN]    - Number of max available Fields array members
                Fields       [OUT]   - Array of pointers to tls00_ParamInfo structures.
                                        These will get filled with the data of
                                        the shortinfo part. The space for the info
                                        is allocated whithin this function.
                FieldCount   [OUT]  - value that stores the number of fields for
                                       which a description is delivered. If
                                       this number is bigger than FieldsAvail an
                                       error is returned.

  returnvalue:  errOK_els00           - NO errors
                errUnknownPart_els98  - no shortinfo part in packet or number of
                                         available fields to little
  -----------------------------------------------------------------------------
*/
int ls04GetShortInfo(tsp1_packet     *SqlPacket,
                     tsp00_Int2       FieldsAvail,
                     tls00_ParamInfo *Fields[],
                     tsp00_Int2      &FieldCount);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls04GetParseID
  -----------------------------------------------------------------------------

  description:  Gets the parse id out of an answer packet.

  arguments:    SqlPacket   [IN]   - Packet
                pszParseId  [OUT]  - Initialized buffer to store parse id;
                                      Has to be at least of length
                                      MAX_PARSEID_LENGTH_LS00 + 1
                                      (not checked anyway).

  returnvalue:  errOK_els00           - NO errors
                errUnknownPart_els98  - no parseid part in packet or buffer
                                         to small to hold parseid
  -----------------------------------------------------------------------------
*/
int ls04GetParseID(tsp1_packet *SqlPacket, tsp00_Addr pszParseId);



/*!
  -----------------------------------------------------------------------------

  function:     ls04GetColumnNames
  -----------------------------------------------------------------------------

  description:  Gets the column names out of a part of part kind column names 
                that is located in an answer packet.
                Usually this kind of part is delivered together with a part of
                part kind shortinfo. So the FieldCount here should be the same
                as in GetShortInfo.
                The function allocates space for the Fields array members that
                has to be freed by the caller afterwards. In case of error no
                space is allocated.

  arguments:    SqlPacket       [IN]   - Packet
                FieldsAvail     [IN]   - Number of max available Fields array members
                Fields          [OUT]  - Array of pointers to tsp00_KnlIdentifierc.
                                          These will get filled with the column names.
                                          The space for the names is allocated by the 
                                          function.
                FieldCount      [OUT]  - value that stores the number of fields for
                                          which a description is delivered. If
                                          this number is bigger than FieldsAvail an
                                          error is returned.

  returnvalue:  errOK_els00             - NO errors
                errUnknownPart_els98    - no columnnames part in packet or number of
                                          available fields to little
  -----------------------------------------------------------------------------
*/
int
ls04GetColumnNames(tsp1_packet          *SqlPacket,
                   tsp00_Int2            FieldsAvail,
                   tsp00_KnlIdentifierc *Fields[],
                   tsp00_Int2           &FieldCount);


/*!
  -----------------------------------------------------------------------------

  function:     ls04GetColumnNamesRaw
  -----------------------------------------------------------------------------

  description:  Gets the column names out of a part (buffer) with given start
                point. Names are returned in UTF8-coding.

  arguments:    pSqlPart       [IN]  - Part
                lSqlPartOffset [IN]  - Starting point in Part to read
                Fields         [OUT] - Array of pointers to tsp00_KnlIdentifierc
                                         getting filled with the column names.
                                         The space for the names must be allocated
                                         by the caller.
                FieldCount     [IN]  - Count of column names to get

  returnvalue:  none
  -----------------------------------------------------------------------------
*/
void
ls04GetColumnNamesRaw(tsp1_part            *pSqlPart,
                      tsp00_Int4            lSqlPartOffset,
                      tsp00_KnlIdentifierc *Fields[],
                      tsp00_Int2            FieldCount,
                      tls00_CodeType        CodeType);

/*!
  -----------------------------------------------------------------------------

  function:     ls04GetResultCount
  -----------------------------------------------------------------------------

  description:  Gets the resultcount out of a part of part kind resultcount
                that is located in an answer packet.

  arguments:    SqlPacket       [IN]   - Packet
                pszResultCount  [OUT]  - Pointer to the raw resultcount upon completion.
                                          The space is allocated within the function
                                          and must be freed by the caller.
                                          The raw resultcount contains the undef-byte,
                                          too!!!
                                          If the pointer is a NULL-pointer no value
                                          will be returned but the NULL value remains.
                lResultCount    [OUT]  - Resultcount as an int

  returnvalue:  errOK_els00            - NO errors
                errUnknownPart_els98   - no resultcount part in packet or error
                                          while converting resultcount to integer
  -----------------------------------------------------------------------------
*/
int ls04GetResultCount(tsp1_packet  *SqlPacket,
                       tsp00_Addr   *pszResultCount,
                       tsp00_Int4 & lResultCount);



/*
  -----------------------------------------------------------------------------

  function:     ls04GetBdinfo
  -----------------------------------------------------------------------------

  description:  Gets a bdinfo out of a part of part kind bdinfo
                that is located in an answer packet.

  arguments:    pSqlPacket [IN]   - packet structure
                pBdinfo    [OUT]  - contains the bdinfo in case of success.

  returnvalue:  errOK_els00           - NO errors
                errUnknownPart_els98  - no bdinfopart in packet
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04GetBdinfo(tsp1_packet *pSqlPacket, tls00_BDInfo* pBdinfo);



/*
  -----------------------------------------------------------------------------
 
  function:     ls04GetRawDataPtr
  -----------------------------------------------------------------------------

  description:  Returns a raw pointer to part of a certain part kind in an
                answer packet.

  arguments:    pSqlPacket [IN]   - packet structure
                pszDataPtr [OUT]  - pointer to data part in case of success
                lDataLen   [OUT]  - length of data part
                PartKind   [IN]   - requested part kind; defaults to sp1pk_data

  returnvalue:  errOK_els00           - NO errors
                errUnknownPart_els98  - no part of requested part kind in packet
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04GetRawDataPtr(tsp1_packet         *pSqlPacket,
                  tsp00_Addr          &pszDataPtr,
                  tsp00_Int4          &lDataLen,
                  tsp1_part_kind_Enum  PartKind = sp1pk_data);


/*
  -----------------------------------------------------------------------------

  function:     ls04GetLONGDescriptor

  -----------------------------------------------------------------------------

  description:  Reads long descriptors from a received packet that contains a
                part of part kind long data.
                Puts the descriptors to the required position (starting at lStartMember)
                in the descriptor array.

  arguments:    pSqlPacket       [IN] - receive packet
                lNNLongCnt       [IN] - count of descriptors to extract
                pDescriptorArray [IN] - descriptor array
                lStartMember     [IN] - starting member in array for writing
                                         descriptors

  returnvalue:  errOK_els00           - in case of NO errors
                errUnknownPart_els98  - no part of part kind long data found
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls04GetLONGDescriptor(tsp1_packet           *pSqlPacket,
                      tsp00_Int4             lNNLongCnt,
                      tsp00_LongDescriptor *&pDescriptorArray,
                      tsp00_Int4             lStartMember);


#endif //ifndef HLS041