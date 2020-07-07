/*!
  @file           IFR_ParseInfoCache.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          A cache of parse information
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
#ifndef IFR_PARSEINFOCACHE_H
#define IFR_PARSEINFOCACHE_H

#include "Interfaces/Runtime/IFR_ParseInfo.h"

IFR_BEGIN_NAMESPACE

class IFR_ParseInfoCacheImpl;

/**
 * A cache of parse information. 
 */
class IFR_ParseInfoCache
{
public:
    /**
     * Creates a new parse info cache, setting the maximum number of
     * statements to cache to <code>maxcachesize</code>.
     * @param maxcachesize The number of statements to cache at most, -1 for
     *        unlimited.
     * @param runtimeitem The runtime to be used.
     */
    IFR_ParseInfoCache(IFR_Int4 maxcachesize, IFRUtil_RuntimeItem& runtimeitem, IFR_Bool& memory_ok);   

    /**
     * Destructor.
     */
    ~IFR_ParseInfoCache();
    
    /**
     * Adds a parse info to the cache. The parse info is considered to
     * be locked when this method is entered, as after insertion all
     * other prepared statements may access this item. 
     * @param parseinfo The parse info to add.
     */
    void addParseInfo(IFR_ParseInfo* parseinfo, IFR_Bool& memory_ok);

    /**
     * Retrieves a parse info for a specific statement.
     * @param statement The statement to look for.
     * @param isolationlevel The isolation level of the statement.
     * @param paraminfohash The hash of the parameters, if any.
     * @return The parse info found (newly allocated), or 0 if there is none.
     */
    IFR_ParseInfo *getParseInfo(const IFR_String& statement, IFR_Int4 isolationlevel, IFR_Int4 paraminfohash, IFR_Bool& parameternotmatch);
    
    /**
     * Forgets all stored informations about parse information.
     */
    void forgetAll();
    
    /**
     * Called when a parse info is reaching a reference count of 0. Then, 
     * it must be decided whether to keep, or not to keep the parse info.
     * A parse info cannot do this by itself, as it must be prevented that
     * the cache is making changes (cached/not cached) on the parse info.
     */
    void checkReleaseParseInfo(IFR_ParseInfoData* pidata, IFR_Bool& memory_ok);


private:
  IFR_ParseInfoCacheImpl *m_impl; //!< reference to info cache implementation class
};

IFR_END_NAMESPACE

#endif
