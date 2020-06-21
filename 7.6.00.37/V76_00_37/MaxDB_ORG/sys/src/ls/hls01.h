/* 
  -----------------------------------------------------------------------------
 
  module: hls01.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Functions for communication with client

  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1997-2005 SAP AG-2001
 
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


#ifndef HLS01
#define HLS01

#include "gls00.h"
#include "hls98msg.h"

/*!
  -----------------------------------------------------------------------------
 
  function:     ctrlservopen

  -----------------------------------------------------------------------------

  description:  Initializes the REPMServer session data, log file and trace file.
                The function will be called by the REPMServer framework at the
                startup of the REPMServer.
                
                Initializes the following parameters:
                SQLMODE             = INTERNAL
                Isolations Level    = 3
                date type           = Internal
                time type           = Internal
                stamp Typ           = Internal
                digit grouping symbol = blank
                decimal symbol      = point
                string separator    = comma
                string delimiter    = QUOTE_LS00
                NULL string         = "?                   "    (20 zeichen)
                TRUE string         = "TRUE      "              (10 zeichen)
                FALSE string        = "FALSE     "              (10 zeichen)
                output language     = "DEU"
                code type           = ASCII
                Ignore Termchar set = true;


  arguments:    SessionData     [IN|OUT] - load session structure
                DBName          [IN]     - db name
                DBRoot          [IN]     - db root directory
                CancelFlag      [IN]     - address that signals cancel request
                LoggedOn        [IN]     - shows if client is located on the same
                                           server as the server program (not used)

  returnvalue:  No
  -----------------------------------------------------------------------------
*/


/*!
  -----------------------------------------------------------------------------
 
  function:     ctrlservclose

  -----------------------------------------------------------------------------

  description:  REPMServer shutdown

                Cleans up the REPMServer session data. The 
                function will be called by the REPMServer framework at the
                shutdown of the REPMServer.
                Releases still existing db connection.

  arguments:    SessionData [IN]  - load session info structure

  returnvalue:  No
  -----------------------------------------------------------------------------
*/


/*!
  -----------------------------------------------------------------------------
 
  function:     ctrlservcommand

  -----------------------------------------------------------------------------

  description:  This function executes one REPMServer command. The 
                function will be called by the REPMServer framework.

  arguments:    SessionData [IN]  - load session info structure
                RequestData [IN]  - command to execute
                RequestLen  [IN]  - length of command
                ReplyData   [OUT] - answer text
                ReplyLen    [OUT] - length of answer text
                ReplyLenMax [IN]  - max length of answer text

  returnvalue:  No
  -----------------------------------------------------------------------------
*/



inline tsp00_Int4
ls01CheckForCancel(bool *pCancelFlag)
{
    tsp00_Int4 rc = (true == *pCancelFlag) ? errCancelRequest_els98 :
											 STATIC_CAST(tsp00_Int4, errOK_els00);
    return rc;
}
#endif /*HLS01*/