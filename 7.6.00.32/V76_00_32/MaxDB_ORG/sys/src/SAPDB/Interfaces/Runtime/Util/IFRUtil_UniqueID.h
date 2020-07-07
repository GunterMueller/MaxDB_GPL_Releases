/*!
  @file           IFRUtil_UniqueID.h
  @author         D039752
  @ingroup        IFR_Util
  @brief          Unique ID having common prefix.

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
#ifndef IFRUTIL_UNIQUEID
#define IFRUTIL_UNIQUEID

#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Util
 * Unique id generator for cursor name and/or similar stuff.
 */
class IFRUtil_UniqueID 
    : public IFRUtil_RuntimeItem
{ 
public:
    /**
     * Creates an unique id generator.
     * @param prefix The prefix of the unique id.
     * @param runtime The runtime environment.
     */
    IFRUtil_UniqueID(IFR_String& prefix,
                     IFRUtil_RuntimeItem& runtime,
                     IFR_Bool& memory_ok);
    
    /**
     * Creates an unique id generator.
     * @param prefix The prefix of the unique id.
     * @param encoding The encoding of <code>prefix</code>.
     * @param runtime The runtime environment.
     * @param allocator The allocator to be used.
     */
    IFRUtil_UniqueID(const char *prefix, 
                     IFR_StringEncoding encoding,
                     SQLDBC_IRuntime& runtime,
                     SAPDBMem_IRawAllocator& allocator,
                     IFR_Bool& memory_ok);

    /**
     * Destructor.
     */
    ~IFRUtil_UniqueID();
    
    /**
     * Sets a new prefix.
     * @param str The new prefix.
     * @param memory_ok Memory check flag.
     */
    void setPrefix(const char *prefix, IFR_Bool& memory_ok);

    /**
     * Retrieves the next unique id, and stores it in <code>id</code>.
     * @param id The destination where to store the generated id.
     */
    void getNextID (IFR_String& id, IFR_Bool& memory_ok);
    
private:
    SQLDBC_IRuntime::CounterHandle m_counter;  //!< The counter.
    IFR_String                     m_prefix;   //!< The prefix string.
};

IFR_END_NAMESPACE

#endif  
    
