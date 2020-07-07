/****************************************************************************

  module      : vak687.cpp

  -------------------------------------------------------------------------

  responsible : GertG

  special area: 
  description : Join aux functions


  last changed: 2004-01-18  12:09
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2004-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "ggg00.h"
#include "hgg941.h"
#include "hak687.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


externPascal void a687_check_memory_usage(
       tgg00_TransContext VAR_VALUE_REF TransContext, 
        SAPDB_Int4 global_added, 
        SAPDB_ULong VAR_VALUE_REF usedbytes1 )
{
    SAPDB_ULong _usedbytes2, _maxusedbytes, _ctrlbytes;
    gg941CalcStatistics( TransContext, _usedbytes2, _maxusedbytes, _ctrlbytes );
    if ( ! usedbytes1 == _usedbytes2 - global_added )
        RTE_Crash( SAPDBErr_Exception( __FILE__, __LINE__, SAPDBERR_ASSERT_STATE_FAILED, "(vak680 memory leek)"));
}

/*--------------------------------------------------------------------------*/
