/*!
  @file           Msg_Registry.hpp
  @author         JoergM
  @ingroup        RunTime
  @brief          Registring messaging message lists

  Implementation found in Msg_List.cpp

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
#ifndef MSG_REGISTRY_HPP
#define MSG_REGISTRY_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Messages/Msg_IRegistry.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class Msg_List;
class SAPDBMem_IRawAllocator;
/*!
  The registry uses slot arrays linked as single linked list. The slot array list only grows.

  Registry slot array structure

  SAPDB_ULong                                               | Magic number 0xADD177E5
  SlotArray *                                               | Pointer to next slot page of 0 if last slot page
  SAPDB_UInt4[MSG_REGISTRY_SLOT_ARRAY_CLUSTER_COUNT]        | Array of free slots in chunk counters
  Msg_RegistrySlot [MSG_REGISTRY_SLOT_ARRAY_SIZE]           | Array of msg list pointers + page index + usage count
 */

//! Magic number to identify a registry slot page
#define MSG_REGISTRY_SLOT_ARRAY_MAGIC 0xADD177E5UL
//! Number of slot entries per cluster (power of 2)
#define MSG_REGISTRY_SLOTS_PER_CLUSTER         16
//! Number of clusters used to accelarate free slot search
#define MSG_REGISTRY_SLOT_ARRAY_CLUSTER_COUNT  31
//! Number of slot entries in registry slot page
#define MSG_REGISTRY_SLOT_ARRAY_SIZE   ( MSG_REGISTRY_SLOT_ARRAY_CLUSTER_COUNT * MSG_REGISTRY_SLOTS_PER_CLUSTER )

class Msg_RegistryIterator;
class Msg_Registry;
/*!
  @class Msg_RegistrySlot
  @brief The slot itself just contains the pointer
  this class allows a forward declaration in Msg_List.hpp which separates implementation from usage...
 */
class Msg_RegistrySlot
{
public:
    /*!
      @brief only default ctor needed since using classes are friends...
     */
    Msg_RegistrySlot()
    : m_pMsg(0)
    , m_SlotIndex(0)
    , m_UsageCount(0)
    {}

    /*!
      @brief initialize a slot
      @param slotIndex [in] the slot index
      @return none
     */
    void Initialize(SAPDB_UInt4 const slotIndex)
    {
        m_pMsg = 0;
        m_SlotIndex = slotIndex;
        m_UsageCount = 0;
    }
private:
    Msg_List       *m_pMsg;         //!< pointer to registered message or 0 if slot is unsed
    SAPDB_UInt4     m_SlotIndex;    //!< slot index of this slot
    SAPDB_UInt4     m_UsageCount;   //!< iterator usage count

    /*!
      @brief deregister and wait for current iterator usage count to become zero
     */
    void WaitForZeroUsageCountAndDeregister();

    /*!
      @brief increment current iterator usage count
     */
    void IncrementUsageCount();

    /*!
      @brief decrement current iterator usage count
     */
    void DecrementUsageCount();

    friend class Msg_RegistryIterator;
    friend class Msg_Registry;
}; /*! @endclass Msg_RegistrySlot */

/*!
   @class Msg_RegistryIterator
   @brief message registry iterator

   This iterator can be used to iterate through all registered messages
   Each iterator is allowed to block the last Msg_List pointer returned from
   deregistration until the next pointer is inspected. This allows to savely
   use the message list even if some other thread tries to destroy it concurrently
 */
class Msg_RegistryIterator
{
public:
    /*!
     @brief ctor
     */
    Msg_RegistryIterator() : m_NextPage(-1), m_NextSlot(0)
    {}

    /*!
     @brief copy ctor
     */
    Msg_RegistryIterator(const Msg_RegistryIterator &toCopy);

    /*!
      @brief private assignment operator
      @param toCopy [in] iterator to copy from
      @return reference to iterator
     */
    Msg_RegistryIterator & operator=(const Msg_RegistryIterator &toCopy);

    /*!
     @brief get first message

     Restart iteration with the first message
     IMPORTANT: The message deregistration is blocked only until the next NextMessage call or the destruction.
     You should only use the returned pointer to show the message content. If you copy it, the copied message
     will register itself etc...

     @return pointer to first message (only valid until next call of NextMessage() or dtor!!!) or 0 if no more messages
     */
    Msg_List const * FirstMessage();

    /*!
     @brief get next message

     IMPORTANT: The message deregistration is blocked only until the next NextMessage call or the destruction.
     You should only use the returned pointer to show the message content. If you copy it, the copied message
     will register itself etc...

     @return pointer to next message (only valid until next call of NextMessage() or dtor!!!) or 0 if no more messages
     */
    Msg_List const * NextMessage();

    /*!
     @brief dtor
     */
    ~Msg_RegistryIterator();
private:

    /*!
      @brief Use current or advance m_NextSlot and m_NextPage until a valid slot is found or no more slots are found

      Used to find first valid slot and to increment its usage count

      @return 0 or pointer to page corresponding to m_NextPage and containing a valid m_NextSlot
     */
    void *LockFirst();

    /*!
      @brief Always advance m_NextSlot and m_NextPage until a valid slot is found or no more slots are found

      Used to find next valid slot and to increment its usage count

      @param pArray [in] pointer to array to lock
      @param pMsg [in] next message locked
      @return 0 or pointer to page corresponding to m_NextPage and containing a valid m_NextSlot
     */
    void *LockNext(void *pArray, Msg_List const *&pMsg);

    /*!
      @brief Release last locked slot
      @return 0 or pointer to page corresponding to m_NextPage
     */
    void *ReleaseLast();

    SAPDB_Int4  m_NextPage; //!< next page index
    SAPDB_UInt4 m_NextSlot; //!< next slot index
}; /*! @endclass Msg_RegistryIterator */

/*!---------------------------------------------------------------------------
  @class Msg_Registry
  @brief Singleton for registring and deregistring of Msg_List instances
  
    Provides Msg_List internal used allocator and allows to iterate through all
    messages registered.
*/
class Msg_Registry : public Msg_IRegistry
{
public:
    /*!
      @brief The slot arrays are linked as single linked list. They always grow on demand

      Registry slot array structure

      SAPDB_ULong            | Magic number 0xADD177E5
      SlotArray *            | Pointer to next slot page of 0 if last slot page
      SAPDB_UInt4[60]        | Array of free slots in chunk counters
      Msg_RegistrySlot [480] | Array of msg list pointers + page index + usage count
     */
    struct SlotArray
    {
        SAPDB_ULong      magic;       //!< Helps to identify slot arrays in core dumps...
        SlotArray       *pNextArray;  //!< pointer to next slot array
        SAPDB_UInt4      freeCount[MSG_REGISTRY_SLOT_ARRAY_CLUSTER_COUNT]; //!< Free count
        Msg_RegistrySlot slots[MSG_REGISTRY_SLOT_ARRAY_SIZE];              //!< the message pointer slot array
        SlotArray       *pThis;       //!< back pointer to this slot array
        /*! construct a new slot array */
        SlotArray();
    };

    static Msg_Registry &Instance();

    /*!
        @brief ctor
     */
    Msg_Registry() 
    : m_pLastPage(0)
    , m_LastSlot(0)
    , m_MessageLimit(0)
    {}

    /*!
        @brief dtor
     */
    virtual ~Msg_Registry() {}

    /*!
        @brief Register a message list into global array
        @return pointer to slot which must be used to deregister
     */
    virtual Msg_RegistrySlot const *Register(Msg_List const &msg);

    /*!
        @brief Deregister a message list from global array
        @param pSlot[in] pointer to slot where message is registered
        @return none
     */
    virtual void Deregister(Msg_RegistrySlot const *&pSlot);

    /*!
      @brief Get an iterator for messages registered in registry.
      NOTE: Such an iterator must synchronize with deregistration! Therefore 
      while the iterator is used, the last returned message pointers slot
      is blocked against the deregistration of that message until the NextMessage()
      function of the iterator is called, or the iterator is destructed.
     */
    virtual Msg_RegistryIterator GetIterator() { Msg_RegistryIterator iterator; return iterator; }

    /*!
      @brief get the allocator to be used for all msg list allocation internally
      @return reference to emergency allocator
     */
    virtual SAPDBMem_IRawAllocator &Allocator();

    /*!
    @brief   Returns the limitation of messages contained in a single list
    @param   none
    @return  Current limitation of contained messages ( 0 = unlimited )
    */
    virtual SAPDB_UInt4 GetMessageLimit() const {  return m_MessageLimit; }

    /*!
    @brief   Sets the limitation of messages contained in a single list

    Limitation means the if number of messages that are contained in a message exceed
    the given number, only the topmost message can still be overloaded and all other
    message added only lead to an extra 'ignoring message' that shows the
    limitation and the number of ignored messages. Adding details is not updating main
    message, but only updates the 'ignoring message'.

    NOTE: This function is global to all Msg_Lists created. If called more than ones, the
    Msg_List always takes the maximum of the Msg_List contained messages and the actual 
    message limit value (regarding the top most and the 'ignoring message')

    The first time the message limit is exceeded due to an overload, the message
    to overload with replaces the first message and the second message is replaced by an
    SDBMSG_MESSAGES_IGNORED_MESSAGES message. See UpdateIgnoringMessage() for details.

    @param   messageLimit [in] new limitation of contained messages ( 0 = unlimited )
    @return  Former limitation of contained messages ( 0 = unlimited )
    */
    virtual SAPDB_UInt4 SetMessageLimit(SAPDB_UInt4 messageLimit)
    {
        SAPDB_UInt4 oldMessageLimit = m_MessageLimit; 
        m_MessageLimit = messageLimit;
        return oldMessageLimit; 
    }

private:
    /*!
      @brief get pointer to next slot array to inspect
      @param pLastArray [in] pointer to last slot array inspected
      @return slot array pointer of 0 if no more slot arrays
     */
    virtual SlotArray *GetNextSlotArray(SlotArray *pLastArray)
    {
        return ( pLastArray == 0 ? &m_FirstPage : pLastArray->pNextArray );
    }

    /*!
      @brief append a new slot array
      @param pLastArray [in] pointer to last slot array in list
      @return true if append succeeded, false if completly out of memory 
     */
    virtual SAPDB_Bool AppendNewArray(SlotArray *pLastArray);

    SlotArray   m_FirstPage; //!< the first slot array page
    SlotArray * m_pLastPage; //!< pointer to last page used for registration
    SAPDB_UInt4 m_LastSlot;  //!< last slot used for registration
    SAPDB_UInt4 m_MessageLimit; //!< limit of message list length
    static Msg_Registry * m_Instance; //!< the message registry instance
    friend class Msg_RegistryIterator;
}; /*! @endclass Msg_Registry */
#endif  /* MSG_REGISTRY_HPP */
