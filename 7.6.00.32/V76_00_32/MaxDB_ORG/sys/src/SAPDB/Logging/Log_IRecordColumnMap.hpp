
/*!**************************************************************************

    module      : Log_IRecordColumnMap.hpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : interface of a class containing the altered columns of a record

    last changed: 2001-04-12

    -------------------------------------------------------------------------

    copyright:    Copyright (c) 2000-2005 SAP AG


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


#ifndef LOG_I_RECORD_COLUMN_MAP_H
#define LOG_I_RECORD_COLUMN_MAP_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"

/*==========================================================================*
*  CLASSES                                                                  * 
*===========================================================================*/


//===================================================================================
/*! class:       Log_IRecordColumnMap

    description: interface of a class containing the altered columns of a record
*/

class Log_IRecordColumnMap
{
    
public:
    
    //--------------------------------------------------------------Log_IRecordColumnMap---
    /*! function:    ExistsEntry

        return result: true, if the entry exists in the record column map

        arguments:   StackEntry [in]
    */
    virtual bool ExistsEntry (const tgg00_StackEntry &StackEntry) const = 0;
    /*! function:    InsertValue
        inserts a value into the record column map
        arguments:   StackEntry [in]
    */
    virtual void AppendColumn (
        const tgg00_StackEntry &StackEntry,
        const tgg00_Rec        &Image,
        tgg00_BasisError       &Error) = 0;
};
/*! endclass: Log_IRecordColumnMap */

#endif  /* LOG_I_RECORD_COLUMN_MAP_H */
