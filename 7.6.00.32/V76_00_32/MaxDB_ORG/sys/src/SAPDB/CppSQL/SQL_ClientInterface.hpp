/*!
 * \file    SQL_ClientInterface.hpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Client interface
 */

/*

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



*/


#if !defined (__liveCacheSQL_INCLUDED)
#define __liveCacheSQL_INCLUDED


#include "lcbasetypes.h"    /* no check */
#include "livecachebase.h"  /* no check */

#if !defined (EXT_CLIENT_SQL)
#if !defined (__liveCache_INCLUDED)
#define __liveCache_INCLUDED

DEFINE_GUID(IID_IliveCacheSink, 
            0x8580E621,0x243F,0x11d1,0xB5,0xA1,0xAC,0x7F,0x01,0x06,0x8C,0x36);

#include "ilivecachesink.h"  /* no check */
#endif /* __liveCache_INCLUDED */
#endif /* EXT_CLIENT_SQL */

#undef  AFX_EXT_CLASS
#if defined(EXT_CLIENT_SQL_STATIC_LINK)
#define AFX_EXT_CLASS 
#else
#define AFX_EXT_CLASS DECLSPEC_IMPORT
#endif

#include "livecachebase.h"  /* no check */

#if defined(SQL_STD_STRING_SUPPORTED)
#include <string>
#endif


/************************************************************************/
/* Header files of classes to be known to application                   */
/************************************************************************/

#include "CppSQL/SQL_Option.hpp"
#include "CppSQL/SQL_Column.hpp"
#include "CppSQL/SQL_.hpp"
#include "CppSQL/SQL_Handle.hpp"


#endif /* __liveCacheSQL_INCLUDED */

