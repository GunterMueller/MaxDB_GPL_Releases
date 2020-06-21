/*!
  @file           IFRUtil_UniqueID.cpp
  @author         D039759
  @ingroup        IFR_Util
  @brief          Unique ID with prefix.

  @see            

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
#include "Interfaces/Runtime/Util/IFRUtil_UniqueID.h"

#include <stdio.h>

//----------------------------------------------------------------------
IFRUtil_UniqueID::IFRUtil_UniqueID(IFR_String& prefix,
                                   IFRUtil_RuntimeItem& runtime,
                                   IFR_Bool& memory_ok)
:IFRUtil_RuntimeItem(runtime),
 m_counter(0),
 m_prefix(prefix, memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    
    SQLDBC_IRuntime::Error error;
    this->runtime.createCounter(m_counter, 
                                m_prefix.getAllocator(), 
                                error);
    
    //<<< MEMCHECK
    if(m_counter == 0 || error) {
        memory_ok = false;
    }
}

//----------------------------------------------------------------------
IFRUtil_UniqueID::IFRUtil_UniqueID(const char *prefix, 
                                   IFR_StringEncoding encoding, 
                                   SQLDBC_IRuntime& iruntime,
                                   SAPDBMem_IRawAllocator& iallocator,
                                   IFR_Bool& memory_ok)
                                   
:IFRUtil_RuntimeItem(iruntime, iallocator),
 m_counter(0),
 m_prefix(prefix, encoding, iallocator, memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    
    SQLDBC_IRuntime::Error error;
    runtime.createCounter(m_counter, 
                          m_prefix.getAllocator(), 
                          error);
    //<<< MEMCHECK
    if(m_counter == 0 || error) {
        memory_ok = false;
    }
    //>>> MEMCHECK
}


//----------------------------------------------------------------------
IFRUtil_UniqueID::~IFRUtil_UniqueID()
{
    if(m_counter != 0) {
        SQLDBC_IRuntime::Error ignored;
        runtime.destroyCounter(m_counter, 
                               m_prefix.getAllocator(), 
                               ignored);
    }
}

//----------------------------------------------------------------------
void
IFRUtil_UniqueID::setPrefix(const char* str, IFR_Bool& memory_ok)
{
    m_prefix.setBuffer(str, IFR_StringEncodingAscii, memory_ok);
}

//----------------------------------------------------------------------
void
IFRUtil_UniqueID::getNextID(IFR_String& id, IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    if(m_counter==0) {
        memory_ok = false;
        return;
    }
    IFR_Int4 value=runtime.nextCounter(m_counter);
    char buffer[32];
    sprintf(buffer, "%d", value);
    id.assign(m_prefix, memory_ok);
    id.append(buffer, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    return;
}
