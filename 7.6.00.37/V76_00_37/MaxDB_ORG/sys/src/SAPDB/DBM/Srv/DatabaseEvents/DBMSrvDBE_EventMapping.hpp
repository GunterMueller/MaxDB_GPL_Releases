/*!
  \file    DBMSrvDBE_EventMapping.hpp
  \author  MarcW
  \ingroup handling of database events
  \brief   acessto the event session

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

#if !defined(DBMSrvDBE_EventMapping_hpp)
#define DBMSrvDBE_EventMapping_hpp

#include "vsp0031.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

class DBMSrvDBE_EventMapping {
public:
    Tools_DynamicUTF8String name;
    tsp31_event_ident_Enum id;
};

#endif // DBMSrvDBE_EventMapping_hpp
