/*!
  -----------------------------------------------------------------------------
  module: Tools_Object.hpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: tools base class 
  description:  tools base class

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

#ifndef _Tools_Object_HPP_
#define _Tools_Object_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "Messages/Msg_List.hpp"

/*!
----------------------------------------------------------------------------
Class:        Tools_Object
-----------------------------------------------------------------------------
description:  tools base class
-----------------------------------------------------------------------------
*/
class Tools_Object {

public:
  /*!
    -------------------------------------------------------------------------
    function:     LastMessage
    -------------------------------------------------------------------------
    description:  return the last error message

    return value: the message list object
    -------------------------------------------------------------------------
   */
   const Msg_List & LastMessage
     ( ) 
   { return m_oMessage;}

protected:

   void SetMessage
     ( const Msg_List & oMsg) 
   {m_oMessage = oMsg;}
   void ClearMessage
     ( )
   {m_oMessage.ClearMessageList();}

private:
   Msg_List      m_oMessage;

}; /*! EndClass: Tools_Object */

#endif // _Tools_Object_HPP_
