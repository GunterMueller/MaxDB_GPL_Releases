/*!
  @file           IFRUtil_RuntimeItem.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          Accessor for runtime interface and allocator.


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef IFRUTIL_RUNTIMEITEM_H
#define IFRUTIL_RUNTIMEITEM_H

#include "Interfaces/SQLDBC/SQLDBC_IRuntime.h"

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Util
 * The runtime item is a combination of runtime and allocator, as used 
 * in many places in the runtime. 
 */
class IFRUtil_RuntimeItem
{
public: 
    inline IFRUtil_RuntimeItem(SQLDBC_IRuntime& initruntime,
                               SAPDBMem_IRawAllocator& initallocator)
    :runtime(initruntime),allocator(initallocator)
    {}
    
    inline IFRUtil_RuntimeItem(SQLDBC_IRuntime& initruntime)
    :runtime(initruntime),allocator(initruntime.getGlobalAllocator())
    {}
    
    inline IFRUtil_RuntimeItem(const IFRUtil_RuntimeItem& copy)
    :runtime(copy.runtime),allocator(copy.allocator)
    {}

    inline IFRUtil_RuntimeItem(const IFRUtil_RuntimeItem& copy,
                               SAPDBMem_IRawAllocator& initallocator)
    :runtime(copy.runtime),allocator(initallocator)
    {}

    SQLDBC_IRuntime&        runtime;   //!< The referenced runtime.
    SAPDBMem_IRawAllocator& allocator; //!< The referenced allocator.
    
private:
    
    /** 
     * Assignment is prohibited as references cannot be reseated.
     */
    IFRUtil_RuntimeItem& operator =(const IFRUtil_RuntimeItem&);
};

IFR_END_NAMESPACE

#endif
