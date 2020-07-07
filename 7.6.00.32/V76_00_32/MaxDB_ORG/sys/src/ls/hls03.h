/*!
  -----------------------------------------------------------------------------
 
  module: hls03.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Functions to connect to the database
                
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


#ifndef HLS03
#define HLS03

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
  function:     ls03SQLConnect
  -----------------------------------------------------------------------------

  description:  Connects server to db kernel.

                Additionally the first packet is initialized and the header data
                is saved in dbiPktInitBlock_ls00 of DBInfo structure.
                The header contains values for:
                 - swap behavior of server itself
                 - name and version of load
                 - SegmentKind and PartKind indicate command packet
                These values may be changed when a packet is fast initialized.

                The terminal id is defined.

  arguments:    DBInfo   [IN]     - load session info structure 
                ErrText  [OUT]    - Error text

  returnvalue:  errOK_els00           - everything went fine
                errInitPacket_els98   - error initializing packet
                errDbHost_els98       - db host not found
                errSqlConnectDb_els98 - no connection to db established

                Logs error text to log file and returns error code and error
                text.
  -----------------------------------------------------------------------------
*/
int ls03SQLConnect(tls00_DBInfo *DBInfo, tsp00_Addr ErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls03SQLRelease
  -----------------------------------------------------------------------------

  description:  Releases db connection and flags this in DBInfo structure.

  arguments:    DBInfo   [IN]     - load session info structure 

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void ls03SQLRelease (tls00_DBInfo*  DBInfo);


/*!
  -----------------------------------------------------------------------------
  function:     ls03ProcessSQLPacket
  -----------------------------------------------------------------------------

  description:  Sends a built packet to the kernel and receives the answer
                packet.
                Analyses the error status of the sent packet (the status arrives
                with the answer packet).

  arguments:    pDBInfo      [IN]   - structure with all info on the running
                                       session and the database connected to
                lPcktNo      [IN]   - packet number to use for init
                lErrPos      [OUT]  - error position in sent packet in case of
                                       error; if no error of no use
                pszErrText   [OUT]  - error text

  returnvalue:  errOK_els00                 - no error
                communication error while sending or receiving the packet
                error caused by send statement/packet

                In case of error the function returns the error code, an
                error message and the error position.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls03ProcessSQLPacket(tls00_DBInfo *pDBInfo,
                     tsp00_Int4    lPcktNo,
                     tsp00_Int4   &lErrPos,
                     tsp00_Addr    pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls03SQLRequest
  -----------------------------------------------------------------------------

  description:  Sends request packet to db. In case of error the connection to
                db is released.

  arguments:    DBInfo   [IN]     - load session info structure 
                PacketNo [IN]     - index of packet in structure dbiPktSndList
                                    in load session info structure 
                ErrText  [OUT]    - Error text

  returnvalue:  errOK_els00       - everything went fine
                errDbComm_els98   - error at request (communication error)

                Logs error text to log file and returns error code and error
                text. The error text contains the kernel msg text, too.
  -----------------------------------------------------------------------------
*/
int ls03SQLRequest(tls00_DBInfo *DBInfo, tsp00_Int4 PacketNo, tsp00_Addr ErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls03SQLReceive
  -----------------------------------------------------------------------------

  description:  Receives answer packet from db. In case of error the connection
                to db is released.

  arguments:    DBInfo   [IN]     - load session info structure 
                PacketNo [IN]     - index of packet in structure dbiPktSndList
                                    in load session info structure 
                ErrText  [OUT]    - Error text

  returnvalue:  errOK_els00       - everything went fine
                errDbComm_els98   - error at receive (communication error)

                Logs error text to log file and returns error code and error
                text. The error text contains the kernel msg text, too.
  -----------------------------------------------------------------------------
*/
int ls03SQLReceive (tls00_DBInfo* DBInfo, tsp00_Int4 PacketNo, tsp00_Addr ErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls03GetHostname
  -----------------------------------------------------------------------------

  description:  Returns hostname.

  arguments:    DBHostname   [OUT]   - 
                szErrText    [OUT]   - Error text

  returnvalue:  errOK_els00       - everything went fine
                errDbHost_els98   - error getting hostname
  -----------------------------------------------------------------------------
*/
int ls03GetHostname(tsp00_NodeIdc &DBHostname, tsp00_ErrText szErrText);


#endif /*HLS03*/