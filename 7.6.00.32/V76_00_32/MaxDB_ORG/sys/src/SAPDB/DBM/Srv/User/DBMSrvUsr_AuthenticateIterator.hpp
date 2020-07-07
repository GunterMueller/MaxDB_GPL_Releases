/*!
  \file    DBMSrvUsr_AuthenticateIterator.hpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   declaration of class to iterate through the allowed authentication methods

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#if !defined(DBMSrvUsr_AuthenticateIterator_hpp)
#define DBMSrvUsr_AuthenticateIterator_hpp

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "ToolsCommon/Tools_Array.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "RunTime/Security/RTESec_Authentication.hpp"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------
class DBMSrvPar_Parameters;

/*! \brief A class to iterate through the allowed authentication methods
   
 */

class DBMSrvUsr_AuthenticateIterator : public RTESec_Authentication::Iterator
{
  public:
    DBMSrvUsr_AuthenticateIterator
     ( const char                                 * szDBName ,
       const Tools_Array<Tools_DynamicUTF8String> & oRequested );

    bool next(const RTESec_Authentication * & pAuthentication);

private:
    void  readArray
      ( const DBMSrvPar_Parameters           * pXParamData,
        const SAPDB_UTF8                     * szParameter,
        Tools_Array<Tools_DynamicUTF8String> & oArray ) const;

    bool isAllowed
      ( const RTESec_Authentication                * pAuthentication,
        const Tools_Array<Tools_DynamicUTF8String> & oArray ) const;

    bool isDenied
      ( const RTESec_Authentication                * pAuthentication,
        const Tools_Array<Tools_DynamicUTF8String> & oArray ) const;

    Tools_Array<Tools_DynamicUTF8String> m_oAllowed;
    Tools_Array<Tools_DynamicUTF8String> m_oDenied;
    Tools_Array<Tools_DynamicUTF8String> m_oRequested;

}; // end class DBMSrvUsr_AuthenticateIterator

#endif // DBMSrvUsr_AuthenticateIterator_hpp
