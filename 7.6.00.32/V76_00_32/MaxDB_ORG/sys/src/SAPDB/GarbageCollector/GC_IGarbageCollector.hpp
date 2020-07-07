/*!
  @defgroup       GarbageColletion Remove history information not needed anymore by any transaction 
*/
/*!
  @ingroup        GarbageColletion
  @file           GC_IGarbageCollector.hpp
  @author         FerdiF
  @brief          defines an interface to the GarbageCollection
*/
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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef GC_IGARBAGECOLLECTOR_HPP
#define GC_IGARBAGECOLLECTOR_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"

/*!
  @interface      GC_IGarbageCollector
  @brief          this is an Interface to garbage collector
*/
class GC_IGarbageCollector
{
public:   
    /*!
    @brief          This method increments the statistic counter for create object
                    entries handled by garbage collector
    */
    virtual void IncHistCreateObjFileCount() = 0;

    /*!
    @brief          This method increments the statistic counter for delete object
                    entries handled by garbage collector
    */
    virtual void IncHistDeleteObjCount() = 0;

    /*!
    @brief          This method increments the statistic counter for drop object
                    entries handled by garbage collector
    */
    virtual void IncHistDropObjFileCount() = 0;

    /*!
    @brief          This method increments the statistic counter for history file
                    entries released by the garbage collector
    @returns        
    */
    virtual void IncHistEntryReleaseCount() = 0;

    /*!
    @brief          This method increments the statistic counter for lock object
                    entries handled by garbage collector
    */
    virtual void IncHistLockObjCount() = 0;

    /*!
    @brief          This method increments the statistic counter for insert object
                    entries handled by garbage collector
    */
    virtual void IncHistInsertObjCount() = 0;

    /*!
    @brief          This method increments the statistic counter for new object
                    entries handled by garbage collector
    */
    virtual void IncHistNewObjCount() = 0;

    /*!
    @brief          This method increments the statistic counter for update object
                    entries handled by garbage collector
    */
    virtual void IncHistUpdateObjCount() = 0;

    /*!
    @brief          This method increments the statistic counter for objects 
                    released ( deleted ) by the garbage collector
    @returns        
    */
    virtual void IncObjReleaseCount() = 0;

    /*!
    @brief          This method increments the statistic counter for pages  
                    released ( deleted ) by the garbage collector, because they 
                    became empty
    @returns        
    */
    virtual void IncReleaseEmptyPageCount() = 0;

    /*!
    @brief          This method returns if the garbage collector is to be 
                    interrupted because of a savepoint request.
    @returns        true, if GarbagaCollection is to be interrupted, else false
    */
    virtual bool IsInterrrupted () const = 0;
};

#endif
