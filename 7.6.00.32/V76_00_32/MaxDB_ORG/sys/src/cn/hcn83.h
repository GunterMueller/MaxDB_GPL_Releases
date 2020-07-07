/*


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
#if defined(CN72) && defined(_WIN32)
  #line 3 "d:\\v72\\sys\\src\\cn\\hcn83.h"
#endif 

/*! 
  -----------------------------------------------------------------------------
 
  module: hcn83.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer
 
  description:  specification module for special sql access
                - index handling
  
  version:      7.2./7.3

  -----------------------------------------------------------------------------
 
                          Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/
 

#ifndef _HCN83_H_
#define _HCN83_H_

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include "gcn00.h"

/* 
  -----------------------------------------------------------------------------
  specification public macros
  -----------------------------------------------------------------------------
*/

/* 
  -----------------------------------------------------------------------------
  specification public functions
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     cn83RecreateIndex
  -----------------------------------------------------------------------------

  description:  recreates specified index

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn83RecreateIndex
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

#endif /* #ifndef _HCN83_H_ */
