/*



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
#ifndef StudioWeb_DEFINITIONS_HPP
#define StudioWeb_DEFINITIONS_HPP

/*!**********************************************************************

  module: StudioWeb_Definitions.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  datatypes for WebQuery

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/




#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"

#define QUOTETAG 		"&quot;"

#define QUOTE			"\""

#define SPACE			" "

#define SPACETAG		"%20"

#define  WQ_LOGON_FRAME_PATH "wqlogon.htm"

#define  WQ_MAINFRAME_PATH  "wqframe.htm"

#define  WQ_MAINFRAMENOTREE_PATH  "wqframenotree.htm";

#define  WQ_TREEBUTTONS_PATH "wqtreebuttons.htm"

const SAPDB_Int4 cnstLimitForLong = 128;

#define DISABLED_TRUE		"true"
#define DISABLED_FALSE		"false"

enum StudioWeb_FetchDirection
{
	StudioWeb_FetchDirNone = 0,
	StudioWeb_FetchDirFirst = 1,
    StudioWeb_FetchDirPrevious = 2,
    StudioWeb_FetchDirNext = 3,    
	StudioWeb_FetchDirLast =4
};

enum StudioWeb_DisableButton
{
	StudioWeb_DisableNone = 0,
	StudioWeb_DisableFirst = 1,
    StudioWeb_DisablePrevious = 2,
    StudioWeb_DisableNext = 3,    
	StudioWeb_DisableLast =4,
	StudioWeb_DisableNextLast = 5,
	StudioWeb_DisableFirstPrev = 6
};

   
#endif