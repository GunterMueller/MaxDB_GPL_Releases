/*!
    @file     SharedSQL_CommandCacheIterator.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Cache for SQLCommands, shared
    @see            

\if EMIT_LICENCE
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

\endif
*/

#ifndef SHAREDSQL_COMMANDCACHEITERATOR_HPP
#define SHAREDSQL_COMMANDCACHEITERATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

class SharedSQL_CommandCache;
class SharedSQL_CachedCommand;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_CommandCacheIterator
    @brief          Iterator for the CommandCache
 */
class SharedSQL_CommandCacheIterator
{
public:
    /*!
        @brief  Constructor
    */
    SharedSQL_CommandCacheIterator( void ) 
		: mCommand(0)
        , mSlot(0)
        , mSlotCount(0)
        , mCommandCache(0)
    {}

    /*!
        @brief  Constructor 
        @param  Command      [in]
        @param  Slot         [in]
        @param  SlotCount    [in]
        @param  CommandCache [in]
        @return 
    */
    SharedSQL_CommandCacheIterator(   SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot, 
                                      SharedSQL_HashValue SlotCount, SharedSQL_CommandCache* CommandCache ) 
		 : mCommand(Command)
         , mSlot(Slot)
         , mSlotCount(SlotCount)
         , mCommandCache(CommandCache)
	{}

    /*!
        @brief  Destructor
    */
    ~SharedSQL_CommandCacheIterator( void );

    /*!
        @brief   Operator *
        @return  SharedSQL_CachedCommand*
    */
    SharedSQL_CachedCommand& operator*() const
    {
        SAPDBERR_ASSERT_STATE( IsValid() );
        return *mCommand;
    }

    /*!
        @brief   Operator ==
        @param   I [in] - Iterator to compare with
        @return  TRUE if equal, else FALSE
    */
    bool operator==( const SharedSQL_CommandCacheIterator& I ) const
    {
        return ( mCommand == I.mCommand );
    }
    /*!
        @brief   Operator ->
        @return  SharedSQL_CachedCommand*
    */
    SharedSQL_CachedCommand* operator->() const
    {
        SAPDBERR_ASSERT_STATE( IsValid() );
        return mCommand;
    }

    /*!
        @brief   Operator ++
        @return  SharedSQL_CommandCacheIterator&
    */
    SharedSQL_CommandCacheIterator& operator++() ;

    /*!
        @brief  Retruns wheather or not the iterator is valid
        @return SAPDB_Bool
    */
    SAPDB_Bool IsValid() const
    {
        return (mCommand != 0) && (mCommandCache != 0);
    }

protected:
    /// The current hash table slot
    SharedSQL_HashValue             mSlot;
    /// The number of hash table slots
    SharedSQL_HashValue             mSlotCount;
    /// Pointer to the cuttent command
    SharedSQL_CachedCommand*        mCommand;
    /// Pointer to the command cache to be iterated
    SharedSQL_CommandCache*         mCommandCache;
};  // SharedSQL_CommandCacheIterator

#endif  /* SHAREDSQL_COMMANDCACHEITERATOR_HPP */
