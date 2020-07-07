/*! 
  -----------------------------------------------------------------------------
 
  module: DBMSrvBHist_ListProducer.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  TiloH
 
  special area: backup history handling by the DBMServer
   
  description:  implementing a class suppling basic constants for creating an
                output list

  version:      7.4.4 and higher 
  -----------------------------------------------------------------------------
 
                          Copyright (c) 2002-2005 SAP AG
 
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

//
//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp"


//
//-----------------------------------------------------------------------------
// members class DBMSrvBHist_ListProducer
//-----------------------------------------------------------------------------
//

const char * DBMSrvBHist_ListProducer::CONT_FLAG_END     ="END     \n";
const char * DBMSrvBHist_ListProducer::CONT_FLAG_CONTINUE="CONTINUE\n";
