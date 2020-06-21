/*! 
  -----------------------------------------------------------------------------
  module: hls29_dataupdate.h
  -----------------------------------------------------------------------------
 
  responsible:  Hans GeorgB

  special area: Replication Manager
 
  description:  dataupdate
                
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

#ifndef HLS29
#define HLS29

#include "gls00.h"

// Declarations - Application specific structures

/*!
  -----------------------------------------------------------------------------
  function:     UpdatePackageDataRecord 
  -----------------------------------------------------------------------------
  description:  DataUpdate main function

  arguments:    pMetaDataDef  [IN]    - Structure that contains all information on
                                         the dataload command given by the user
                pDBInfo       [IN]    - Structure with all information on the running 
                                         session and the database connected to
                pszErrText    [OUT]   - Error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
  -----------------------------------------------------------------------------
*/
tsp00_Int4 UpdatePackageDataRecord( MetaDataDef    *pMetaDataDef,
                              tls00_DBInfo   *pDBInfo,
                              tsp00_Addr      ErrText);

#endif /*HLS29*/
