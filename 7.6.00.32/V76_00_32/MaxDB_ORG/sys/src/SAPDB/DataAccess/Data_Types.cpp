  /*!***************************************************************************

  module:       Data_Types.cpp
  created:      2002-09-06
  responsible:  UweH
  special area: DataAccess
  description:  Base types of this component
  see also:     

  ------------------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG



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


  *****************************************************************************/

#include "DataAccess/Data_Types.hpp"

/* --------------------------------------------------------------------------- */
const char * Data_AccessModeStrings [Data_AccessMode_DONT_USE] =
{
    "ForRead",
    "ForIntend",
    "ForUpdate",
    "ForStructureChange",
    "ForFirstIndexOrLeafAccess",
    "ForLeafAccess"
};

/* --------------------------------------------------------------------------- */
const char * Data_ReleaseModeStrings [Data_ReleaseMode_DONOT_USE] =
{
    "Changed",
    "Unchanged"
};

/* --------------------------------------------------------------------------- */
const char * Data_LRUChainInfoStrings [Data_LRUChainInfo_DONT_USE] =
{
    "LruNormal",
    "LruMiddle",
    "LruLast"
};

/* --------------------------------------------------------------------------- */
const char * Data_AddressingModeStrings [Data_AddressingModes] =
{
    "Static",
    "Dynamic"
};

/* --------------------------------------------------------------------------- */
const char * Data_RecoverabilityStrings [Data_RecoveryModes] =
{
    "Recoverable (perm)",
    "NotRecoverable (temp)"
};

/* --------------------------------------------------------------------------- */
SAPDB_Int4 Data_IndexLeafCaching = 0; // PTS 1136998 UH 2006-01-23
