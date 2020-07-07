/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_EnumDatabase.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Enum Database Class
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_EnumDatabase.hpp"

/*    
  =============================================================================
  class: DBMCli_EnumDatabase
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_EnumDatabase :: DBMCli_EnumDatabase
  -----------------------------------------------------------------------------
*/
DBMCli_EnumDatabase :: DBMCli_EnumDatabase ( )
{
}

DBMCli_EnumDatabase :: DBMCli_EnumDatabase ( const DBMCli_String & sName,
                                             const DBMCli_String & sInstRoot,
                                             const DBMCli_String & sVersion,
                                             const DBMCli_String & sKernel,
                                             const DBMCli_String & sState)
                     : m_sName( sName ),
                       m_sInstRoot( sInstRoot ),
                       m_oVersion( sVersion ),
                       m_sKernel( sKernel ),
                       m_sState( sState )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_EnumDatabase :: ~DBMCli_EnumDatabase
  -----------------------------------------------------------------------------
*/
DBMCli_EnumDatabase :: ~DBMCli_EnumDatabase ( )
{
}


