/*!
  @file           DBMCli_Logo.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          database manager client logo printer

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



\endif
*/

#ifndef _DBMCli_Logo_HPP_
#define _DBMCli_Logo_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*!
  @brief     Class for dbm logo objects
 */
class DBMCli_Logo
{
  public:

    /*! @brief constructor */
    DBMCli_Logo  ( ) {}

    /*! @brief destructor */
    ~DBMCli_Logo ( ) {}

    /*! @brief Print the logo */
    void PrintLogo 
      ( const DBMCli_String & sComponent,
              SAPDB_Bool      bPrint = true );

    /*! @brief Print the  version */
    void PrintVersion 
      ( const DBMCli_String & sComponent,
              SAPDB_Bool      bPrint = true );

}; // end DBMCli_Logo

#endif // _DBMCli_Logo_HPP_

