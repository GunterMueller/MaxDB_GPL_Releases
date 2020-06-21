/*!
  @file           RTE_ItemRegister.hpp
  @author         JoergM
  @ingroup        RTE  
  @brief          Item registration 
  @see            


  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/

#ifndef RTE_ITEMREGISTER_HPP
#define RTE_ITEMREGISTER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"


#include "RunTime/MemoryManagement/RTEMem_UnregisteredAllocator.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  class: RTE_ItemRegister
  description: Item registration template class
  
  Items have an identifier, a reference to there object and an optional baseIdentifier,
  if object is based on another object.

  The pointers to other item infos are stored inside, visible only to the RTE_ItemRegister.

  The template argument ItemType does not need any special member function or feature.
  Even a base type like 'double' could be used.

  To complete the class, the static 'Instance()' member function must be implemented.

  Example:
  
    // implementation of Instance (must be in separate module)

    typedef RTE_ItemRegister<SAPDBMem_IRawAllocator> RTEMem_AllocatorRegister;

    RTEMem_AllocatorRegister * RTEMem_AllocatorRegister::m_Instance = 0;

    RTEMem_AllocatorRegister & RTEMem_AllocatorRegister::Instance()
    {
        SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_AllocatorRegister));
        if ( !m_Instance )
        {
           m_Instance = new ( space ) RTEMem_AllocatorRegister();
        }
        return *m_Instance;
    }
 */

/*

    Comment on the additional coding for slow compilation:

    The RTE_ItemRegister is very elegant and efficient, but unfortunately it is not very robust:
    The objects that are registered are allocated by the caller and not by the register itself,
    so the linked chain structure used to register the objects is within the objects themselves.
    So, if the caller registers an object and deletes it afterwards without deregistering, the 
    linked chain of RTE_ItemRegister is corrupt, possibly leading to a crash when it is accessed
    for the next time. In a case like that, the cause for the crash is very hard to find, because 
    the object has already been deleted.
    To make finding the cause for a corruption easier, if compiled slow the RTE_ItemRegister makes
    a copy of every object that is registered. With this copy, the register chain is checked for
    consistency every time a new object is being registered or deregistered. As an identifier is
    also kept in the copy made, the object that has been deleted without dregistration can be
    identified.

*/
template <class ItemType>
class RTE_ItemRegister
{
public:
    /*!
      @class Info
      @brief Info structure for registerable items

      Items have an identifier, a reference to their object and an optional baseIdentifier,
      if object is based on another object.

      The pointers to other item infos are stored inside, visible only to the RTE_ItemRegister.
     */
    class Info
    {
    public:
        /*!
          @brief ctor 
          Careful with identifiers! They are not copied and must exist during lifecycle of info item.
          A base identifier is usefull if the item is directly depending from a base item. I.e an 
          allocator that uses a base allocator for memory...

          @param identifier [in] info item identifier
          @param pItem [in] pointer to iten
          @param baseIdentifier [in] info base identifier
         */
        Info(const SAPDB_UTF8 *identifier, ItemType *pItem=0, const SAPDB_UTF8 *baseIdentifier=0)
            : m_Prev(0),
              m_Next(0),
              m_Identifier(identifier),
              m_Item(pItem),
              m_BackupCopy(NULL),
              m_BaseIdentifier( baseIdentifier ? baseIdentifier : (const SAPDB_UTF8 *)0 )
        {}

        /*!
          @brief Return identifier given to item
          @return const SAPDB_UTF8 * Identifier
         */
        const SAPDB_UTF8 *GetIdentifier() const
        {
            return m_Identifier; 
        }

        /*!
          @brief Return a reference to the item

          Since ItemType is a template parameter, any action
          the item supports are allowed.

          @return Reference to Item
         */
        ItemType & GetItem() const
        {
            return *m_Item;
        }

        /*!
          @brief  Return identifier given to base item

          This is optional, since not all items are derived from
          a base item. In such a case an empty string is returned.

          @return const SAPDB_UTF8 * BaseIdentifier or ""
         */
        const SAPDB_UTF8 *GetBaseIdentifier() const 
        {
            return ( 0 == m_BaseIdentifier ? (const SAPDB_UTF8 *)"" : m_BaseIdentifier );
        }

        /*!
          @brief  Return constant pointer to item pointer

          This call is used in parsing register list (see Next() member function below)

          NOTE: It is very discouraged to use this outside of parsing code...

          @return Pointer to current successor of this item
         */
        Info *GetSuccessor() const { return m_Next; }

    private:
        Info             *m_Prev;       //!< if linked pointer to previous item
        Info             *m_Next;       //!< if linked pointer to next item
        const SAPDB_UTF8 *m_Identifier; //!< item identifier
        ItemType         *m_Item;       //!< the item reference
        const SAPDB_UTF8 *m_BaseIdentifier; //!< item base identifier
        Info             *m_BackupCopy; //!< this is really USED in the slow kernel only, but for compatibilitys sake it is included in all speeds
        friend class RTE_ItemRegister<ItemType>;
    }; /*! @endclass Info */

    /*! 
        @brief Register an item
    
        The item is entered at the end of the internal list of the registration.
        Since the complete list is scanned to prevent double registration, no
        pointer to the end of list is needed.

        To enforce a reregistration do an Deregister() call first!
        @param itemInfo [in] the item to register

        @return false if already registered (no update done), true else
     */
    SAPDB_Bool Register(Info &itemInfo)
    {
        RTESync_LockedScope lock(Spinlock());

        if(m_makeBackupCopy)
        {
            CheckConsistency();
        }

#if defined(SAPDB_SLOW)
        if ( SearchItemInfo( &itemInfo) )
        {
            RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ITEMREGISTER_CORRUPT,"Trying to reregister" ,SAPDB_ToString(&itemInfo),SAPDB_ToString(0),SAPDB_ToString(itemInfo.GetIdentifier())));
            return false; // found itemInfo in list -> no reregistration!
        }
#endif
        itemInfo.m_Prev = 0;
        itemInfo.m_Next = 0;

        if(m_makeBackupCopy)
        {
            AddBackupCopy(itemInfo);
        }

        if ( 0 == m_First ) // check if list is empty 
        {
            m_First = m_Last = &itemInfo;
        }
        else
        {   // put always at end of list
            m_Last->m_Next = &itemInfo;
            itemInfo.m_Prev = m_Last;

            if(m_makeBackupCopy)
            {
                m_Last->m_BackupCopy->m_Next = itemInfo.m_BackupCopy;
                itemInfo.m_BackupCopy->m_Prev = m_Last->m_BackupCopy;
            }
            m_Last = &itemInfo;
        }

        ++m_Count;
        return true;
    }
    /*! 
        @brief Deregister an item
    
        The item is removed from the internal list of the registration.
        For removal the list is scanned from beginning to the element
        @param itemInfo [in] the item to deregister
        @return true if successfully removed, false if not found
     */
    SAPDB_Bool Deregister(Info &itemInfo)
    {
        RTESync_LockedScope lock(Spinlock());

        if ( itemInfo.m_Next )
        {
            itemInfo.m_Next->m_Prev = itemInfo.m_Prev;
            if(m_makeBackupCopy)
            {
                itemInfo.m_BackupCopy->m_Next->m_Prev = itemInfo.m_BackupCopy->m_Prev;
            }

            if ( m_First == &itemInfo )
            {
                m_First = itemInfo.m_Next;
            }

            if ( itemInfo.m_Prev )
            {
                itemInfo.m_Prev->m_Next = itemInfo.m_Next;
                if(m_makeBackupCopy)
                {
                    itemInfo.m_BackupCopy->m_Prev->m_Next = itemInfo.m_BackupCopy->m_Next;
                }

                if ( m_Last == &itemInfo )
                {
                    m_Last = itemInfo.m_Prev;
                }
            }
        }
        else if ( itemInfo.m_Prev )
        {
            itemInfo.m_Prev->m_Next = itemInfo.m_Next;
            if(m_makeBackupCopy)
            {
                itemInfo.m_BackupCopy->m_Prev->m_Next = itemInfo.m_BackupCopy->m_Next;
            }

            if ( m_Last == &itemInfo )
            {
                m_Last = itemInfo.m_Prev;
            }
        }
        else
        {
            if ( m_First != &itemInfo
              || m_Last  != &itemInfo )
            {
                return false;
            }
            else
            {
                m_First = m_Last = 0;
            }
        }

        if ( m_makeBackupCopy )
        {
            DeleteBackupCopy( itemInfo );
        }

        --m_Count;
        return true;
    }

    /*!
      @brief Get the spinlock reference of the register
      @return A spinlock that can be used as parameter for RTESys_LockedScope constructor
     */
    RTESync_Spinlock& Spinlock()
    {
        return m_Spinlock;
    }
    
protected:
    /*!
       @brief protected ctor only used once during initialization
       @param makeBackupCopy [in] flag to set if consistency check and backup copies are wanted
     */
    RTE_ItemRegister(SAPDB_Bool makeBackupCopy) : m_First(0), m_Last(0), m_Count(0), m_makeBackupCopy(makeBackupCopy) {}
    Info *m_First;      //!< First in list of registered items
    SAPDB_Int4 m_Count; //!< Current item count

private:

    /*!
      @brief Validate if searched item is in register 

      Locking of register was be done outside of this routine
      Optional the predecessor in list is retrieved

      @param searchedInfo [in] info to search for

      @return true if item is in register (pPredecessor set to 0 if first element), false else
     */
    SAPDB_Bool SearchItemInfo(const Info *pSearchedInfo)
    {
        if ( 0 == m_First )
        {
            return false;
        }

        if ( m_First == pSearchedInfo )
        {
            return true;
        }

        /* validate if searched item is in register */
        for ( Info *pInfo = m_First; pInfo; pInfo = pInfo->m_Next )
        {
            if ( pInfo == pSearchedInfo )
            {
                return true;
            }
        }
        return false;
    }

    /*!
      @brief Add a backup copy to item object

      To allow checking of consistency the backup copy is created using a special not registered allocator

      @param itemInfo [in] info to backup copy
     */
    void AddBackupCopy(Info &itemInfo)
    {
        // make a copy of the Info itself (used for consistency checks) in "own" memory
        itemInfo.m_BackupCopy = (Info *)RTEMem_UnregisteredAllocator::Instance().Allocate(sizeof(Info));
        *itemInfo.m_BackupCopy = itemInfo;
        // now make copies of the identifiers in "own" memory, too - in case an inconsistency is detected,
        // the original identifiers have already been deleted
        SAPDB_UTF8 *copyOfIdentifier = (SAPDB_UTF8 *)RTEMem_UnregisteredAllocator::Instance().Allocate(strlen((char *)itemInfo.m_Identifier)+1);
        strcpy((char *)copyOfIdentifier,(char *)itemInfo.m_Identifier);
        SAPDB_UTF8 *copyOfBaseIdentifier = NULL;
        if(itemInfo.m_BaseIdentifier)
        {
            copyOfBaseIdentifier = (SAPDB_UTF8 *)RTEMem_UnregisteredAllocator::Instance().Allocate(strlen((char *)itemInfo.m_BaseIdentifier)+1);
            strcpy((char *)copyOfBaseIdentifier,(char *)itemInfo.m_BaseIdentifier);
        }
        itemInfo.m_BackupCopy->m_Identifier = copyOfIdentifier;
        itemInfo.m_BackupCopy->m_BaseIdentifier = copyOfBaseIdentifier;
    }

    /*!
      @brief Delete a backup copy in item object

      To allow checking of consistency the backup copy was created using a special not registered allocator

      @param itemInfo [in] info to backup copy
     */
    void DeleteBackupCopy(Info &itemInfo)
    {
        RTEMem_UnregisteredAllocator::Instance().Deallocate((void *)itemInfo.m_BackupCopy->m_Identifier);
        if(itemInfo.m_BackupCopy->m_BaseIdentifier)
        {
            RTEMem_UnregisteredAllocator::Instance().Deallocate((void *)itemInfo.m_BackupCopy->m_BaseIdentifier);
        }
        RTEMem_UnregisteredAllocator::Instance().Deallocate((void *)itemInfo.m_BackupCopy);
    }
    
    /*!
      @brief Check the consistency of the backup copy

      The complete list of backup objects is parsed and verified that the backup object point to the corresponding item references.
      Incosistency are reported as message.
     */
    void CheckConsistency(void)
    {
        if ( m_First )
        {
            SAPDB_Int4 elementCounter = 0;
            for ( Info *current = m_First,*backup = m_First->m_BackupCopy; 
                    current != 0; 
                    backup = backup->m_Next,current = current->m_Next,elementCounter++ )
            {
                if(current->m_Item && backup->m_Item && current->m_Item != backup->m_Item)
                {
                    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ITEMREGISTER_CORRUPT,(char *)backup->m_Identifier ,SAPDB_ToString(current->m_Item),SAPDB_ToString(backup->m_Item),SAPDB_ToString(elementCounter)));
                }

                if ( current->m_Prev && current->m_Prev->m_Next != current )
                {
                    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ITEMREGISTER_CORRUPT,"Pointer chain broken" ,SAPDB_ToString(current),SAPDB_ToString(current->m_Prev->m_Next),SAPDB_ToString(elementCounter)));
                }
            }
            if ( !m_Last || m_Count != elementCounter )
            {
                RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ITEMREGISTER_CORRUPT,"First item but no last item or bad count", SAPDB_ToString(m_First->m_Item),SAPDB_ToString(0),SAPDB_ToString(m_Count - elementCounter)));
            }
        }
        else
        {
            if ( m_Last || m_Count != 0 )
            {
                RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ITEMREGISTER_CORRUPT,"No first item but last item", SAPDB_ToString(0),SAPDB_ToString(m_Last->m_Item),SAPDB_ToString(m_Count)));
            }
        }
    }

    RTESync_Spinlock m_Spinlock; //!< Spinlock needed to protect the register access
    Info *m_Last;                //!< Last in list of registered items
    SAPDB_Bool m_makeBackupCopy; //!< flag if set, backup copy and consistency checks are used
}; /*! endclass: RTE_ItemRegister */

#endif  /* RTE_ITEMREGISTER_HPP */
