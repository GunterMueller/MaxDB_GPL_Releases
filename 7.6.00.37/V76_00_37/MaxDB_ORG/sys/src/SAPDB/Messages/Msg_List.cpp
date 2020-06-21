/*!
  @file           Msg_List.cpp
  @author         RaymondR/JoergM/RobinW
  @ingroup        RunTime
  @brief          Creating, queueing and managing message lists

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
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/Messages/SDBMsg_Messages.h"

#if defined(WIN32)
# include <tchar.h>
#else
# include <ctype.h>
# include <stdio.h>
# include <string.h>
#endif
# include <stdlib.h>

#include  "SAPDBCommon/SAPDB_sprintf.h"
#include  "SAPDBCommon/SAPDB_string.h"
#ifdef SAPDBERR_MESSAGELIST_STILL_SUPPORTED
#include  "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#endif
#include  "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include  "RunTime/RTE_IInterface.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include  "Messages/Msg_SimpleArg.hpp"

extern SAPDBTrace_Topic Messages_Trace; ///< global message trace object

#ifndef LIBOMS
#include  "Messages/Msg_Registry.hpp" /* nocheck */
#include  "RunTime/MemoryManagement/RTEMem_Allocator.hpp" /*nocheck*/
#include  "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp" /*nocheck*/
#endif // LIBOMS

/*!
  message enumeration
 */
enum SDBMsg_Messages_Enumeration
{
    SDBMSG_MESSAGES_ENUMERATION
};

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! only byte values in range [ 0 - LAST_SINGLE_BYTE_NUMBER ] are send as single byte */
#define LAST_SINGLE_BYTE_NUMBER ((SAPDB_Byte)0xF6)

/*! 2 byte transfer indicator */
#define FOLLOWED_BY_1_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 1))
/*! 3 byte transfer indicator */
#define FOLLOWED_BY_2_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 2))
/*! 4 byte transfer indicator */
#define FOLLOWED_BY_3_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 3))
/*! 5 byte transfer indicator */
#define FOLLOWED_BY_4_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 4))
/*! 6 byte transfer indicator */
#define FOLLOWED_BY_5_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 5))
/*! 7 byte transfer indicator */
#define FOLLOWED_BY_6_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 6))
/*! 8 byte transfer indicator */
#define FOLLOWED_BY_7_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 7))
/*! 9 byte transfer indicator */
#define FOLLOWED_BY_8_BYTES     ((SAPDB_Byte)(LAST_SINGLE_BYTE_NUMBER + 8))
/*! illegal number */
#define NUMBER_ILLEGAL          ((SAPDB_Byte)0xFF)

/*! get number of bytes following */
#define NUMBER_OF_BYTES_FOLLOWED(n_) (((SAPDB_Byte)n_)>LAST_SINGLE_BYTE_NUMBER?(((SAPDB_Byte)n_)-LAST_SINGLE_BYTE_NUMBER):0)

/*! separator for additional arguments */
#define ADDITIONAL_ARGS_STR                   ":"

/*! message list in registry slot already illegal */
#define MSG_LIST_IN_SLOT_ILLEGAL ((Msg_List const *)1)

#define MSG_LIST_LIMIT 0 ///< unlimited is a reasonable default...

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*! 64 bit alignment round up to sizeof SAPDB_UInt8 */
#define  ALIGN_VAL_64BIT(_val) ((((_val) - 1) / sizeof( SAPDB_UInt8 ) + 1 ) * sizeof( SAPDB_UInt8 ))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*! heap reserved as emergency space */
#define MSG_LIST_EMERGENCY_SPACE (64*1024)
SAPDB_ULong MsgList_EmergencySpace[MSG_LIST_EMERGENCY_SPACE/sizeof(SAPDB_ULong)]; ///< emergency space

#if defined(SAPDBERR_MESSAGELIST_STILL_SUPPORTED)
SAPDB_UInt4           const Msg_List::ErrorMsg = Msg_List::Error; ///< error 
SAPDB_UInt4           const Msg_List::WarningMsg = Msg_List::Warning; ///< warning
SAPDB_UInt4           const Msg_List::InfoMsg = Msg_List::Info; ///< info
#else
SAPDB_UInt4           const Msg_List::ErrorMsg = 1U; ///< error
SAPDB_UInt4           const Msg_List::WarningMsg = 2U; ///< warning
SAPDB_UInt4           const Msg_List::InfoMsg = 3U; ///< info
#endif
RTE_ISystem::DateTime const Msg_List::EmptyDateTimeValue = { 0,0,0,0,0,0,0,0 }; ///< empty timestamp

#ifndef LIBOMS
Msg_Registry            *Msg_Registry::m_Instance = 0; ///< message registry singleton pointer
#endif // LIBOMS
//----------------------------------------------------------------

#ifndef LIBOMS
/*!
  @class MsgList_Allocator
  @brief message list common allocator

  The specialty of this allocator is the emergency page, which is static memory on heap.
 */
class MsgList_Allocator : public SAPDBMem_IRawAllocator
{
public:
    /*!
      @brief ctor based on RTEMem_Allocator
      @return none
     */
   MsgList_Allocator()
   : m_Allocator(RTEMem_Allocator::Instance())
   , m_EmergencyAllocs(0)
   , m_EmergencyDeallocs(0)
   , m_EmergencyErrors(0)
   , m_EmergencyMaxBytesUsed(0)
   , m_EmergencyChunks(0)
   , m_EmergencySpace((SAPDB_Byte *)&MsgList_EmergencySpace[0])
   , m_NextEmergencySpacePointer((SAPDB_Byte *)&MsgList_EmergencySpace[0])
   {
      static RTEMem_AllocatorInfo AllocatorInfo((const SAPDB_UTF8 *)"MsgList_EmergencyAllocator", this, (const SAPDB_UTF8 *)"");
      RTEMem_AllocatorRegister::Instance().Register(AllocatorInfo);
   }

   /*!
      @brief          Does nothing

    */
   virtual ~MsgList_Allocator() {}

   /*!
      @brief          Allocates contiguous memory for &lt;ByteCount> bytes.
              The memory is aligned as indicated by the 'GetAlignmentSize' 
              method.
              The memory is not initialised.
      @param          ByteCount [in] The size in bytes of the memory to be allocated. This 
                      argument must be greater zero.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;ByteCount> 
                      bytes. If exception handling is disabled, a NULL pointer will
                      be re turned in case of insufficient memory.
      @exception      From 'SAPDBMem_IRawAllocator' derived classes may throw a
                      'SAPDBMem_BadAllocException' if the allocation fails.

    */
   virtual void* Allocate(SAPDB_ULong ByteCount)
   {
       void *addr = ( m_Allocator.DoNotUseForMessageOutputAllocation()
                    ? (void *)0 : m_Allocator.Allocate(ByteCount) );
       if ( !addr )
       {
           addr = EmergencyAllocate(ByteCount);
       }
       return addr;
   }

   /*!
      @brief          works like realloc
      @param          ByteCount [in] The size in bytes of the memory to be allocated. This 
                      argument must be greater zero.
      @param          Hint [in] If possible, the allocator tries to reserve the memory at
                      the same position as given by Hint. This is useful for
                      resizing a memory block. If the reallocation was succesfull
                      the return value and Hint are identical.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;ByteCount> 
                      bytes. If exception handling is disabled, a NULL pointer will
                      be re turned in case of insufficient memory.          
   */

   virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
   {
       void *addr = ( m_Allocator.DoNotUseForMessageOutputAllocation()
                    ? (void *)0 : m_Allocator.Allocate(ByteCount, Hint) );
       if ( !addr )
       {
           addr = EmergencyAllocate(ByteCount);
       }
       return addr;
   }

   /*!
      @brief          Frees the memory behind &lt;p>.
      @param          p [in] 
      @return         none


                    The memory must have been allocated by a previous call to
                    'Allocate'. This is checked by assertions.
                    !!! An implementation of this method may not throw an 
                    exception !!!

    */

   virtual void Deallocate(void* p)
   {
       if ( !EmergencyDeallocate(p) )
       {
           m_Allocator.Deallocate(p);
       }
   }

   /*!
      @brief          Checks the integrity of the allocator.
      @return         bool (only in the non-exception case)
      @exception      SAPDBMem_ConsistencyException 


                    This function checks the inner state of the allocator 
                    concerning consistency. Therefore, the total memory controlled 
                    by this allocator is checked.
                    If an inconsistency is detected, an excpetion is thrown.
                    The default implementation does nothing.

    */
   virtual bool CheckConsistency() { return m_Allocator.CheckConsistency(); }

   /*!
      @brief          returns the size af a chunk.
      @param          p [int] The address of the chunk. This must be the result of
                      a previous call of the Allocate method of the allocator.
                      The default implementation returns GetInvalidSize().
      @return         the internal size of the chunk in bytes. The size may differ
              from the required size of the corresponding Allocate call.

    */
   virtual SAPDB_ULong GetChunkSize(const void* p) { return m_Allocator.GetChunkSize(p); }

   /*!
      @brief          calculate statistics
      @param          BytesUsed [out] bytes used
      @param          MaxBytesUsed [out] maximum number of bytes used
      @param          BytesControlled [out] bytes controlled
      @return         none

    */
   virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                               SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled)
   {
       BytesUsed = (m_NextEmergencySpacePointer - m_EmergencySpace);
       MaxBytesUsed = m_EmergencyMaxBytesUsed;
       BytesControlled = MSG_LIST_EMERGENCY_SPACE;
   }

   /*!
      @brief          base allocator call statistics
      @param          CountAlloc [out] number of base allocator allocations - always 0
      @param          CountDealloc [out] number of base allocator deallocations - always 0
      @return         none

    */
   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
   {
       CountAlloc = 0;
       CountDealloc = 0;
   }

   /*!
      @brief          allocator call statistics
      @param          CountAlloc [out] number of allocations
      @param          CountDealloc [out] number of deallocations
      @return         none

    */
   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
   {
       CountAlloc = (SAPDB_ULong)m_EmergencyAllocs;
       CountDealloc = (SAPDB_ULong)m_EmergencyDeallocs;
   }

   /*!
     @brief get number of failed allocations
     @return number of failed allocations
    */
   virtual SAPDB_ULong GetAllocFailedCount() const { return m_EmergencyErrors; }

   /*!
     @brief get allocator identifier
     @return 'MsgList_EmergencyAllocator'
    */
   virtual const SAPDB_UTF8 *GetIdentifier() const { return (SAPDB_UTF8 const *)"MsgList_EmergencyAllocator"; }
private:
    /*!
      @brief Emergency deallocation does not return memory...
      @param p [in] the candidate
      @return pointer to emergency space or 0 if emptied
     */
    void *EmergencyAllocate(SAPDB_ULong ByteCount)
    {
        RTE_IInterface::Instance().AtomicModify(m_EmergencyAllocs, (SAPDB_Int8)1);
        while ( ByteCount > 0 )
        {
            SAPDB_Byte *addr = m_NextEmergencySpacePointer;
            SAPDB_UInt4 totalBytesUsed = addr - m_EmergencySpace;
            if ( ByteCount <= (MSG_LIST_EMERGENCY_SPACE - totalBytesUsed) )
            {
                RTE_IInterface::Instance().AtomicModify(m_EmergencyChunks, 1);
                SAPDB_Byte *nextAddr = addr + ByteCount + CalcAlignmentOffset(addr+ByteCount);
                if ( RTE_IInterface::Instance().AtomicCompareAndExchange( (void **)&m_NextEmergencySpacePointer
                                               , addr
                                               , nextAddr
                                               , (void **)&addr) )
                {
                    if ( m_EmergencyMaxBytesUsed < totalBytesUsed )
                    {
                        m_EmergencyMaxBytesUsed = totalBytesUsed;
                    }
                    return addr;
                }
                RTE_IInterface::Instance().AtomicModify(m_EmergencyChunks, -1);
            }
            else
            {
                ++m_EmergencyErrors;
                break;
            }
        }
        return 0;
    }

    /*!
      @brief Emergency deallocation does not return memory...
      @param p [in] the candidate
      @return true if candidate is in emergency allocation range, false if outside
     */
    SAPDB_Bool EmergencyDeallocate(void* p)
    {
        if (  (((SAPDB_Byte *)p) >= m_EmergencySpace)
              && (((SAPDB_Byte *)p) <= (m_EmergencySpace+MSG_LIST_EMERGENCY_SPACE)) )
        {
            RTE_IInterface::Instance().AtomicModify(m_EmergencyDeallocs, (SAPDB_Long)1);   
            RTE_IInterface::Instance().AtomicModify(m_EmergencyChunks, -1);
            SAPDB_Byte *addr = m_NextEmergencySpacePointer;

            RTE_IInterface::Instance().ReadMemoryBarrier();  // protect against some other threads parallel emergency allocate...

            // if last chunk was released, reset emergency next chunk pointer to start of emergency area
            if ( m_EmergencyChunks == 0 )
            {
                RTE_IInterface::Instance().AtomicCompareAndExchange( (void **)&m_NextEmergencySpacePointer
                                         , addr
                                         , m_EmergencySpace
                                         , (void **)&addr );
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    SAPDBMem_IRawAllocator &m_Allocator;      //!< the allocator used for normal allocation
    SAPDB_UInt8  m_EmergencyAllocs;           //!< the number of emergency allocs
    SAPDB_UInt8  m_EmergencyDeallocs;         //!< the number of emergency deallocs
    SAPDB_Int    m_EmergencyErrors;           //!< the number of failed emergency allocation attempts
    SAPDB_UInt4  m_EmergencyMaxBytesUsed;     //!< the maximum number of bytes used from emergency chunk
    SAPDB_UInt4  m_EmergencyChunks;           //!< the number of emergency chunks in use
    SAPDB_Byte  *m_EmergencySpace;            //!< the emergency space used
    SAPDB_Byte  *m_NextEmergencySpacePointer; //!< the next emergency space to return
};
#endif // LIBOMS

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

//---------------------------------------------------------------
// Load And Store Value compressed
//---------------------------------------------------------------

/*!
  @brief Copy C String handling

  The call copies the given string into buffer

  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param stringToCopy [in] the string to copy
 */
static inline void CopyCString( SAPDB_Char *&ptr
                              , SAPDB_UInt4 &sizeLeft
                              , SAPDB_UInt4 &neededSize
                              , SAPDB_Char const *stringToCopy);

/*!
  @brief Copy indented C String handling

  The call copies the indentation and the given string into buffer.
  Indentiation consists of a newline plus indentLevel * MSG_LIST_INDENTATION.

  @param indentLevel [in] the indentation level
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param stringToCopy [in] the string to copy
 */
static inline void CopyIndentedCString( SAPDB_UInt4 indentLevel
                                      , SAPDB_Char *&ptr
                                      , SAPDB_UInt4 &sizeLeft
                                      , SAPDB_UInt4 &neededSize
                                      , SAPDB_Char const *stringToCopy);

/*!
  @brief Copy argument tag and value
  
  The call copies the given number into buffer using format TAG="%u" for SAPDB_UInt4
  The call copies the given number into buffer using format TAG="%qu" for SAPDB_UInt8
  The call copies the given number into buffer using format TAG="%s" for SAPDB_Char const *
  The call copies the given number into buffer using format TAG="%s" for SAPDB_UTF8 const * with replacement of ",<,>,& and '

  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param argumentTag [in] the string to copy
  @param argumentNumber [in] the argument value
 */
static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_UInt4 argumentNumber);

/*!
  @brief Copy argument tag and value
  
  The call copies the given number into buffer using format TAG="%u" for SAPDB_UInt4
  The call copies the given number into buffer using format TAG="%qu" for SAPDB_UInt8
  The call copies the given number into buffer using format TAG="%s" for SAPDB_Char const *
  The call copies the given number into buffer using format TAG="%s" for SAPDB_UTF8 const * with replacement of ",<,>,& and '

  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param argumentTag [in] the string to copy
  @param argumentNumber [in] the argument value
 */
static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_UInt8 argumentNumber);
/*!
  @brief Copy argument tag and value
  
  The call copies the given number into buffer using format TAG="%u" for SAPDB_UInt4
  The call copies the given number into buffer using format TAG="%qu" for SAPDB_UInt8
  The call copies the given number into buffer using format TAG="%s" for SAPDB_Char const *
  The call copies the given number into buffer using format TAG="%s" for SAPDB_UTF8 const * with replacement of ",<,>,& and '

  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param argumentTag [in] the string to copy
  @param argumentString [in] the argument value
 */
static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_Char const *argumentString);
/*!
  @brief Copy argument tag and value
  
  The call copies the given number into buffer using format TAG="%u" for SAPDB_UInt4
  The call copies the given number into buffer using format TAG="%qu" for SAPDB_UInt8
  The call copies the given number into buffer using format TAG="%s" for SAPDB_Char const *
  The call copies the given number into buffer using format TAG="%s" for SAPDB_UTF8 const * with replacement of ",<,>,& and '

  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param argumentTag [in] the string to copy
  @param argumentString [in] the argument value
 */
static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_UTF8 const *argumentString);


/*!
  @brief Copy indented argument tag and number value
  
  The call copies the given number into buffer using format TAG="%u"

  @param indentLevel [in] the indentation level
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param argumentTag [in] the string to copy
  @param argumentNumber [in] the argument value
 */
static inline void CopyIntentedArgument( SAPDB_UInt4 indentLevel
                                             , SAPDB_Char *&ptr
                                             , SAPDB_UInt4 &sizeLeft
                                             , SAPDB_UInt4 &neededSize
                                             , SAPDB_Char const *argumentTag
                                             , SAPDB_UInt4 argumentNumber);

/*!
  @brief Copy indented argument tag and string value
  
  The call copies the given number into buffer using format TAG="%s" replacing ",< and >

  @param indentLevel [in] the indentation level
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param argumentTag [in] the string to copy
  @param argumentString [in] the argument string value
 */
static inline void CopyIntentedArgument( SAPDB_UInt4 indentLevel
                                             , SAPDB_Char *&ptr
                                             , SAPDB_UInt4 &sizeLeft
                                             , SAPDB_UInt4 &neededSize
                                             , SAPDB_Char const *argumentTag
                                             , SAPDB_Char const *argumentString);

/*!
  @brief Copy indented argument tag and string value
  
  The call copies the given number into buffer using format TAG="%s" replacing ",< and >

  @param indentLevel [in] the indentation level
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param argumentTag [in] the string to copy
  @param argumentString [in] the argument string value
 */
static inline void CopyIntentedArgument( SAPDB_UInt4 indentLevel
                                                , SAPDB_Char *&ptr
                                                , SAPDB_UInt4 &sizeLeft
                                                , SAPDB_UInt4 &neededSize
                                                , SAPDB_Char const *argumentTag
                                                , SAPDB_UTF8 const *argumentString);

/*!
  @brief Copy indented C String handling

  The call copies the indentation and the start tag into buffer.
  Then it copies the indentation and the given string into buffer.
  Finally it copies the indentation and the end tag into buffer.

  Indentiation consists of a newline plus indentLevel * MSG_LIST_INDENTATION.

  @param indentLevel [in] the indentation level
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param tagToUsed [in] the tag name to use
  @param stringToCopy [in] the string to copy
 */
static inline void CopyIndentedTaggedCString( SAPDB_UInt4 indentLevel
                                            , SAPDB_Char *&ptr
                                            , SAPDB_UInt4 &sizeLeft
                                            , SAPDB_UInt4 &neededSize
                                            , SAPDB_Char const *tagToUsed
                                            , SAPDB_Char const *stringToCopy );

/*!
  @brief Copy indented XML String handling

  The call copies the indentation and the given XML string into buffer.
  Any '<' or '&' is escaped by '&lt;' respectivly '&amp;'. Quote are passed.
  Indentation consists of a newline plus indentLevel * MSG_LIST_INDENTATION.

  @param indentLevel [in] the indentation level
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param tagToUse [in] the tag name to use
  @param stringToCopy [in] the string to copy
 */
static inline void CopyIndentedTaggedXMLString( SAPDB_UInt4 indentLevel
                                              , SAPDB_Char *&ptr
                                              , SAPDB_UInt4 &sizeLeft
                                              , SAPDB_UInt4 &neededSize
                                              , SAPDB_Char const *tagToUse
                                              , SAPDB_UTF8 const *stringToCopy );

/*!
  @brief Copy indented XML String handling

  The call copies the indentation and the given XML string into buffer.
  Any '<', '>' or '&' is escaped by '&lt;', '&gt;' respectivly '&amp;'.
  Quote are escaped by '&apos;' and '&quot;'
  Indentation consists of a newline plus indentLevel * MSG_LIST_INDENTATION.

  @param indentLevel [in] the indentation level
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param stringToCopy [in] the string to copy
 */
static inline void CopyIndentedXMLString( SAPDB_UInt4 indentLevel
                                        , SAPDB_Char *&ptr
                                        , SAPDB_UInt4 &sizeLeft
                                        , SAPDB_UInt4 &neededSize
                                        , SAPDB_UTF8 const *stringToCopy );

/*!
  @brief Copy XML String handling

  The call copies the given XML string into buffer.
  Any '<', '>' or '&' is escaped by '&lt;', '&gt;' respectivly '&amp;'.
  Quote are escaped by '&apos;' and '&quot;'

  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param stringToCopy [in] the string to copy
 */
static inline void CopyXMLString( SAPDB_Char *&ptr
                                , SAPDB_UInt4 &sizeLeft
                                , SAPDB_UInt4 &neededSize
                                , SAPDB_UTF8 const *stringToCopy );

/*!
  @brief calculate needed size of value to store as compressed number
  @param value [in]
  @return number of bytes needed
 */
static inline SAPDB_UInt4 CalcSizeOfValue( SAPDB_UInt2 const value );

/*!
  @brief calculate needed size of value to store as compressed number
  @param value [in]
  @return number of bytes needed
 */
static inline SAPDB_UInt4 CalcSizeOfValue( SAPDB_UInt4 const value );

/*!
  @brief calculate needed size of value to store as compressed number
  @param value [in]
  @return number of bytes needed
 */
static inline SAPDB_UInt4 CalcSizeOfValue( SAPDB_UInt8 const value );

/*!
  @brief calculate needed size of timestamp to store as compressed numbers
  @param dateTime [in]
  @return number of bytes needed
 */
static inline SAPDB_UInt4 CalcSizeOfDateTime( RTE_ISystem::DateTime const &dateTime );

/*!
  @brief calculate needed size of string
  @param start [in]
  @param offset [in]
  @param maxOffset [in]
  @return number of bytes needed
 */
static inline SAPDB_UInt4 CalcSizeOfString( SAPDB_Char const *start
                                          , SAPDB_UInt4 offset
                                          , SAPDB_UInt4 maxOffset );

/*!
  @brief load expected values from compressed number
  @param value [out]
  @param bytesLeft [inout] number of bytes left
  @param ptr [in]
  @return got value or false
 */
static inline SAPDB_Bool LoadExpectedValue( SAPDB_UInt2 &value
                                   , SAPDB_UInt4 &bytesLeft
                                   , SAPDB_Byte const *&ptr );

/*!
  @brief load expected values from compressed number
  @param value [out]
  @param bytesLeft [inout] number of bytes left
  @param ptr [in]
  @return got value or false
 */
static inline SAPDB_Bool LoadExpectedValue( SAPDB_UInt4 &value
                                   , SAPDB_UInt4 &bytesLeft
                                   , SAPDB_Byte const *&ptr );

/*!
  @brief load expected timestamp from compressed numbers
  @param dateTime [out]
  @param bytesLeft [inout] number of bytes left
  @param ptr [in]
  @return got value or false
 */
static inline SAPDB_Bool LoadExpectedDateTime( RTE_ISystem::DateTime &dateTime
                                             , SAPDB_UInt4 &bytesLeft
                                             , SAPDB_Byte const *&ptr );

/*!
  @brief load skip to end of string
  @param string [out]
  @param bytesLeft [inout] number of bytes left
  @param ptr [in]
  @return got value or false
 */
static inline SAPDB_Bool LoadSkipToEndOfString( SAPDB_Char const * &string
                                              , SAPDB_UInt4 &bytesLeft
                                              , SAPDB_Byte const *&ptr );

/*!
  @brief load skip to end of UTF8 string
  @param string [out]
  @param bytesLeft [inout] number of bytes left
  @param ptr [in]
  @return got value or false
 */
static inline SAPDB_Bool LoadSkipToEndOfUTF8String( SAPDB_UTF8 const * &string
                                                  , SAPDB_UInt4 &bytesLeft
                                                  , SAPDB_Byte const *&ptr );

/*!
  @brief Load a compressed UInt2 value from buffer

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param bytesUsed [out] number of bytes used
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool LoadValue( SAPDB_UInt2 &value
                           , SAPDB_UInt4 bufferSizeInBytes
                           , SAPDB_Byte const *pBuffer
                           , SAPDB_UInt4 &bytesUsed );

/*!
  @brief Load a compressed UInt4 value from buffer

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param bytesUsed [out] number of bytes used
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool LoadValue( SAPDB_UInt4 &value
                           , SAPDB_UInt4 bufferSizeInBytes
                           , SAPDB_Byte const *pBuffer
                           , SAPDB_UInt4 &bytesUsed );

/*!
  @brief Load a compressed UInt8 value from buffer

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param bytesUsed [out] number of bytes used
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool LoadValue( SAPDB_UInt8 &value
                           , SAPDB_UInt4 bufferSizeInBytes
                           , SAPDB_Byte const *pBuffer
                           , SAPDB_UInt4 &bytesUsed );

/*!
  @brief Store a needed value as compressed number

  @param valueName [in]
  @param value [in]
  @param bytesLeft [inout]
  @param ptr [out] the buffer to be filled
  @param savedBytes [out] number of bytes saved
  @param errList [out] message list
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool StoreNeededValue( SAPDB_Char const *valueName
                                         , SAPDB_UInt8 const value
                                         , SAPDB_UInt4      &bytesLeft
                                         , SAPDB_Byte *     &ptr
                                         , SAPDB_UInt4      &savedBytes
                                         , Msg_List         &errList );

/*!
  @brief Store a needed value as compressed number

  @param valueName [in]
  @param value [in]
  @param bytesLeft [inout]
  @param ptr [out] the buffer to be filled
  @param savedBytes [out] number of bytes saved
  @param errList [out] message list
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool StoreNeededValue( SAPDB_Char const *valueName
                                         , SAPDB_UInt4 const value
                                         , SAPDB_UInt4      &bytesLeft
                                         , SAPDB_Byte *     &ptr
                                         , SAPDB_UInt4      &savedBytes
                                         , Msg_List         &errList );

/*!
  @brief Store a optional value as compressed number

  @param valueName [in]
  @param value [in]
  @param bytesLeft [inout]
  @param ptr [out] the buffer to be filled
  @param savedBytes [out] number of bytes saved
  @param allowTruncate [in] allow value truncation
  @param errList [out] message list
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool StoreOptionalValue( SAPDB_Char const *valueName
                                           , SAPDB_UInt2 const value
                                           , SAPDB_UInt4      &bytesLeft
                                           , SAPDB_Byte *     &ptr
                                           , SAPDB_UInt4      &savedBytes
                                           , SAPDB_Bool const  allowTruncate
                                           , Msg_List         &errList );

/*!
  @brief Store a optional value as compressed number

  @param valueName [in]
  @param value [in]
  @param bytesLeft [inout]
  @param ptr [out] the buffer to be filled
  @param savedBytes [out] number of bytes saved
  @param allowTruncate [in] allow value truncation
  @param errList [out] message list
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool StoreOptionalValue( SAPDB_Char const *valueName
                                           , SAPDB_UInt4 const value
                                           , SAPDB_UInt4      &bytesLeft
                                           , SAPDB_Byte *     &ptr
                                           , SAPDB_UInt4      &savedBytes
                                           , SAPDB_Bool const  allowTruncate
                                           , Msg_List         &errList );

/*!
  @brief Store a optional timestamp as compressed numbers

  @param valueName [in]
  @param dateTime [in]
  @param bytesLeft [inout]
  @param ptr [out] the buffer to be filled
  @param savedBytes [out] number of bytes saved
  @param allowTruncate [in] allow value truncation
  @param errList [out] message list
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool StoreOptionalDateTime( SAPDB_Char const *           valueName
                                              , RTE_ISystem::DateTime const &dateTime
                                              , SAPDB_UInt4                 &bytesLeft
                                              , SAPDB_Byte *                &ptr
                                              , SAPDB_UInt4                 &savedBytes
                                              , SAPDB_Bool const             allowTruncate
                                              , Msg_List                    &errList );

/*!
  @brief Store a optional string

  @param valueName [in]
  @param varPart [in]
  @param offset [in]
  @param maxOffset [in]
  @param bytesLeft [inout]
  @param ptr [out] the buffer to be filled
  @param savedBytes [out] number of bytes saved
  @param allowTruncate [in] allow value truncation
  @param errList [out] message list
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool StoreOptionalString( SAPDB_Char const *valueName
                                            , SAPDB_Char const *varPart
                                            , SAPDB_UInt4 const offset
                                            , SAPDB_UInt4 const maxOffset
                                            , SAPDB_UInt4      &bytesLeft
                                            , SAPDB_Byte *     &ptr
                                            , SAPDB_UInt4      &savedBytes
                                            , SAPDB_Bool const  allowTruncate
                                            , Msg_List         &errList );

/*!
  @brief store a UInt2 into buffer using compression

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param neededSize [out]
  @return true if call succeeded, false if not enaugh space left in buffer
 */
static inline SAPDB_Bool StoreValue( SAPDB_UInt2 const value
                                   , SAPDB_UInt4 bufferSizeInBytes
                                   , SAPDB_Byte *pBuffer
                                   , SAPDB_UInt4 &neededSize );

/*!
  @brief store a UInt4 into buffer using compression

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param neededSize [out]
  @return true if call succeeded, false if not enaugh space left in buffer
 */
static inline SAPDB_Bool StoreValue( SAPDB_UInt4 const value
                                   , SAPDB_UInt4 bufferSizeInBytes
                                   , SAPDB_Byte *pBuffer
                                   , SAPDB_UInt4 &neededSize );

/*!
  @brief store a UInt8 into buffer using compression

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param neededSize [out]
  @return true if call succeeded, false if not enaugh space left in buffer
 */
static inline SAPDB_Bool StoreValue( SAPDB_UInt8 const value
                                   , SAPDB_UInt4 bufferSizeInBytes
                                   , SAPDB_Byte *pBuffer
                                   , SAPDB_UInt4 &neededSize );

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/

void Msg_List::TraceMessageCopy() const
{
#ifndef LIBOMS
    // LIBOMS does have a problem to access Messages_Trace object
    // since it is a kernel global variable
    if ( Messages_Trace.TracesLevel(1) )
    {
      if ( 0 != m_pMessageData
        && SAPDB_strcasecmp(Component(), MSG_MSGOUTPUT_COMPONENT)
        && ( Type() == Error
          || (Type() == Warning && Messages_Trace.TracesLevel(2))
          || (Type() == Info && Messages_Trace.TracesLevel(3)) ) )
      {
          // message output has to protect itself from recursive call to protect deadlocks
          RTE_IInterface::Instance().DiagMessage(Msg_List(*this));
      }
    }
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const * const Msg_List::DateTime( ShortTimeBuffer &buffer ) const
{ 
    if ( 0 != m_pMessageData )
    {
        SAPDB_sprintf ( &buffer[0], sizeof(ShortTimeBuffer), MSG_LIST_SHORT_TIME_HEADER_FORMAT_STR,
                        m_pMessageData->DateTime.Year,
                        m_pMessageData->DateTime.Month,
                        m_pMessageData->DateTime.Day,
                        m_pMessageData->DateTime.Hour,
                        m_pMessageData->DateTime.Minute,
                        m_pMessageData->DateTime.Second );
    }
    else
    {
        buffer[0] = 0;
    }

    return &buffer[0];
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const * const Msg_List::DateTime( LongTimeBuffer &buffer ) const
{ 
    if ( 0 != m_pMessageData )
    {
        SAPDB_sprintf ( &buffer[0], sizeof(LongTimeBuffer), MSG_LIST_LONG_TIME_HEADER_FORMAT_STR,
                        m_pMessageData->DateTime.Year,
                        m_pMessageData->DateTime.Month,
                        m_pMessageData->DateTime.Day,
                        m_pMessageData->DateTime.Hour,
                        m_pMessageData->DateTime.Minute,
                        m_pMessageData->DateTime.Second,
                        m_pMessageData->DateTime.Milliseconds );
    }
    else
    {
        buffer[0] = 0;
    }

    return &buffer[0];
}

/*---------------------------------------------------------------------------*/

void Msg_List::InsertDetailOfMessageList( SAPDB_UInt4 const          ID,
                                          SAPDB_Char  const * const  Component,
                                          SAPDB_Char  const * const  FileName,
                                          SAPDB_UInt4 const          LineNumber,
                                          SAPDB_Char  const * const  Message,
                                          SAPDB_UInt4 const          ArgCount,
                                          Msg_IOptArg const **       ArgArray )
{
    if ( 0 != Message )
    {
        if ( 0 == m_pMessageData )
        { // Inserting a detail into an empty message creates an info message....
            m_NumOfSubMessages = 0;
            m_NumOfDetails = 0;
            m_ObjectRefCnt = 0;
            m_pNextMessage = 0;
            m_OutputSequenceNumber = 0;
            m_OutputDateTime = EmptyDateTimeValue;
            m_OutputProcessId = 0;
            m_OutputThreadId = 0;
            m_OutputTaskId = 0;
            m_OutputSchedulerId = 0;
            m_DumpTruncated = 0;
            FillMessageList( Msg_List::Info
                           , Component
                           , FileName
                           , LineNumber
                           , ID
                           , Message
                           , ArgCount
                           , ArgArray );
            return;
        }

        // limit reached! Message must be ignored
        SAPDB_UInt4 usedLimit = RTE_IInterface::Instance().GetMessageLimit();
        if ( usedLimit != 0 
          && m_NumOfMessages >= usedLimit )
        {
            UpdateIgnoringMessage( 0
                                 , usedLimit
                                 , 1);
            return;
        }

        // --- create a new 'MessageList' object which will be appended
        Msg_List *pDetailMessage = new (RTE_IInterface::Instance().MsgListAllocator()) Msg_List;

        if ( !pDetailMessage )
        {
            return; // cannot insert detail is silently ignored
        }

        pDetailMessage->m_NumOfSubMessages = 0;
        pDetailMessage->m_NumOfDetails = 0;
        pDetailMessage->m_ObjectRefCnt = 0;
        pDetailMessage->m_pNextMessage = 0;
        pDetailMessage->m_OutputSequenceNumber = 0;
        pDetailMessage->m_OutputDateTime = EmptyDateTimeValue;
        pDetailMessage->m_OutputProcessId = 0;
        pDetailMessage->m_OutputThreadId = 0;
        pDetailMessage->m_OutputTaskId = 0;
        pDetailMessage->m_OutputSchedulerId = 0;
        pDetailMessage->m_DumpTruncated = 0;
        pDetailMessage->FillMessageList(
                         false
                       , Msg_List::Info
                       , Component
                       , FileName
                       , LineNumber
                       , ID
                       , m_pMessageData->DateTime
                       , Message
                       , ArgCount
                       , ArgArray );
        Msg_List *pInsertDetail = this;
        for ( SAPDB_UInt4 iDetail = m_NumOfDetails; iDetail > 0 ; --iDetail )
        {
            if ( !pInsertDetail->m_pNextMessage )
            {
                // some details do not exist
                // correct number of details silently...
                m_NumOfDetails = (m_NumOfDetails - iDetail);
            }
            else
            {
                pInsertDetail = pInsertDetail->m_pNextMessage;
            }
        }

        ++m_NumOfMessages;
        ++m_NumOfDetails;
        pDetailMessage->m_NumOfMessages = 0;
        pDetailMessage->m_pNextMessage = pInsertDetail->m_pNextMessage;
        pInsertDetail->m_pNextMessage = pDetailMessage;
    }
}
/*---------------------------------------------------------------------------*/

void Msg_List::FillMessageList( SAPDB_Bool  const            DoRegister,
                                SAPDB_UInt4 const            Type,
                                SAPDB_Char  const * const    Component,
                                SAPDB_Char  const * const    FileName,
                                SAPDB_UInt4 const            LineNumber,
                                SAPDB_UInt4 const            ID,
                                RTE_ISystem::DateTime const &DateTime,
                                SAPDB_Char  const * const    Message,
                                SAPDB_UInt4 const            ArgCount,
                                Msg_IOptArg const **         ArgArray )
{
    if ( 0 != Message )
    {
        Msg_IOptArg const * ValidArgs[MSG_LIST_ARG_MAX];
        SAPDB_UInt4         TagLength[MSG_LIST_ARG_MAX];
        SAPDB_UInt4         ValueLength[MSG_LIST_ARG_MAX];
        SAPDB_UInt4         ValidArgCount;
        SAPDB_UInt4         SizeOfArgumentStringOffsetArray;

        SAPDB_UInt4 iArg;
        for ( iArg = 0, ValidArgCount = 0; iArg < ArgCount && iArg < MSG_LIST_ARG_MAX; iArg++ )
        {
            Msg_IOptArg const * pArg = ArgArray[iArg];
            if ( pArg && pArg->Tag() && pArg->Value() )
            {
                ValidArgs[ValidArgCount] = pArg;
                ++ValidArgCount;
            }
        }

        // Base structure size + space of argument string offset array
        SizeOfArgumentStringOffsetArray = (ValidArgCount * 2 * sizeof(SAPDB_UInt4));

        SAPDB_UInt4 MessageDataTotalSize = sizeof(MessageData) + SizeOfArgumentStringOffsetArray;

        for ( iArg = 0; iArg < ValidArgCount; iArg++ )
        {
            TagLength[iArg] = SAPDB_strlen( ValidArgs[iArg]->Tag() ) + 1 ;
            ValueLength[iArg] = SAPDB_strlen( (SAPDB_Char *)ValidArgs[iArg]->Value() ) + 1;
            // add tag and value string space
            MessageDataTotalSize += (TagLength[iArg] + ValueLength[iArg]);
        }

        SAPDB_UInt4 ComponentSize     = SAPDB_strlen(Component) + 1;
        SAPDB_UInt4 FileNameSize      = SAPDB_strlen(FileName) + 1;
        SAPDB_UInt4 MessageSize       = SAPDB_strlen(Message) + 1;

        MessageDataTotalSize = ALIGN_VAL_64BIT( MessageDataTotalSize + ComponentSize + FileNameSize + MessageSize );

        m_pMessageData  = (MessageData*)RTE_IInterface::Instance().MsgListAllocator().Allocate(MessageDataTotalSize);
// for memory leak search
//        printf("Allocate m_pMessageData %p\n", m_pMessageData);
        if ( 0 == m_pMessageData )
        {
            return;
        }

        SAPDB_memset(m_pMessageData, 0, MessageDataTotalSize);

        m_ObjectRefCnt  = 1;
        m_NumOfMessages = 1;

        m_pMessageData->Version                   = MSG_LIST_VERSION;
        m_pMessageData->DataLen                   = MessageDataTotalSize;
        m_pMessageData->DataRefCnt                = 1; // - This data is referenced once
        m_pMessageData->Type                      = Type;
        m_pMessageData->ID                        = ID;
        m_pMessageData->DateTime                  = DateTime;                                                                            
        m_pMessageData->LineNumber                = LineNumber;

        SAPDB_UInt4 NextStringOffset = SizeOfArgumentStringOffsetArray;

        m_pMessageData->ComponentStringOffset     = NextStringOffset;
        NextStringOffset += ComponentSize;

        m_pMessageData->FileNameStringOffset      = NextStringOffset;
        NextStringOffset += FileNameSize;

        m_pMessageData->MessageStringOffset       = NextStringOffset;
        NextStringOffset += MessageSize;

        m_pMessageData->ArgumentCount             = ValidArgCount;

        SAPDB_UInt4 *pArgumentOffsetArray = (SAPDB_UInt4 *)&m_pMessageData->VariablePart[0];

        for ( iArg = 0; iArg < ValidArgCount; iArg++ )
        {
            *pArgumentOffsetArray++ = NextStringOffset;
            NextStringOffset += TagLength[iArg];
            *pArgumentOffsetArray++ = NextStringOffset;
            NextStringOffset += ValueLength[iArg];
        }

        SAPDB_memcpy( m_pMessageData->VariablePart + m_pMessageData->ComponentStringOffset, Component, ComponentSize );
        SAPDB_memcpy( m_pMessageData->VariablePart + m_pMessageData->FileNameStringOffset,  FileName,  FileNameSize );
        SAPDB_memcpy( m_pMessageData->VariablePart + m_pMessageData->MessageStringOffset,   Message,   MessageSize );

        pArgumentOffsetArray = (SAPDB_UInt4 *)&m_pMessageData->VariablePart[0];

        for ( iArg = 0; iArg < ValidArgCount; iArg++ )
        {
            static const SAPDB_Char tagCharMap[128] = 
            { 
                0,   '?', '?', '?', '?', '?', '?', '?', 
                '?', '?', '?', '?', '?', '?', '?', '?', 
    //
                '?', '?', '?', '?', '?', '?', '?', '?', 
                '?', '?', '?', '?', '?', '?', '?', '?', 
    // 
                '?', '?', '?', '?', '?', '?', '?', '?', 
                '?', '?', '?', '?', '?', '?', '?', '?', 
    // 
                '0', '1', '2', '3', '4', '5', '6', '7', 
                '8', '9', '?', '?', '?', '?', '?', '?', 
    // 
                '?', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 
                'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
    // 
                'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 
                'X', 'Y', 'Z', '?', '?', '?', '?', '_', 
    //
                '?', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 
                'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
    // 
                'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 
                'X', 'Y', 'Z', '?', '?', '?', '?', '?', 
            };

            SAPDB_Int4 iTag;
            SAPDB_Char const *pTag = ValidArgs[iArg]->Tag();
            for ( iTag = 0; pTag[iTag] != 0; iTag++ )
            {
                (m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg])[iTag] = tagCharMap[pTag[iTag]&0x7F];
            }
            (m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg])[iTag] = 0;
            SAPDB_memcpy( m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg+1], ValidArgs[iArg]->Value(), ValueLength[iArg] );
        }

        if ( DoRegister ) 
        {
            m_pRegistrySlot = RTE_IInterface::Instance().MsgListRegister(*this);
        }
        else
        {
            m_pRegistrySlot = 0;
        }
    }
}

/*---------------------------------------------------------------------------*/

void Msg_List::FillMessageList( SAPDB_UInt4 const          Type,
                                SAPDB_Char  const * const  Component,
                                SAPDB_Char  const * const  FileName,
                                SAPDB_UInt4 const          LineNumber,
                                SAPDB_UInt4 const          ID,
                                SAPDB_Char  const * const  Message,
                                SAPDB_UInt4 const          ArgCount,
                                Msg_IOptArg const **       ArgArray )
{
    if ( 0 != Message )
    {
        RTE_ISystem::DateTime now;
        RTE_IInterface::Instance().GetLocalDateTime( now );

        FillMessageList( true,
                         Type,
                         Component,
                         FileName,
                         LineNumber,
                         ID,
                         now,
                         Message,
                         ArgCount,
                         ArgArray );
    }  
}

/*---------------------------------------------------------------------------*/

Msg_List::~Msg_List()
{ 
    if ( 0 != m_pMessageData )
    {
        DoClear();  // if needed remove old content
    }
}

/*---------------------------------------------------------------------------*/

void Msg_List::DoClear()
{
    RTE_IInterface::Instance().MsgListDeregister(m_pRegistrySlot);

    // --- first of all we should release next message object!!!!
    if ( 0 != m_pNextMessage )
    {
        // - are we the only one holding this next message object?
        if ( 1 == m_pNextMessage->GetObjectRef() )
        {
#if 0 /* working but recursive... long lists have produced stack overflow during destruction... */
            destroy (m_pNextMessage, RTE_IInterface::Instance().MsgListAllocator());
            m_pNextMessage = 0;
#else
#if 0
            // we loop very often, because the list is a single linked list. Bad performce O(2)...
            do {
                Msg_List ** ppLastMessageToDestroy = &m_pNextMessage;
                Msg_List *  pLastMessageToDestroy  =  m_pNextMessage;

                while (       pLastMessageToDestroy->m_pNextMessage
                     && (1 == pLastMessageToDestroy->m_pNextMessage->GetObjectRef()) )
                {
                    ppLastMessageToDestroy = &(pLastMessageToDestroy->m_pNextMessage);
                    pLastMessageToDestroy  =   pLastMessageToDestroy->m_pNextMessage;
                }

                // destroy always clears given reference! *ppLastMessageToDestroy = 0 is implicit!!!
                destroy(*ppLastMessageToDestroy, RTE_IInterface::Instance().MsgListAllocator());

            } while ( m_pNextMessage != 0 );
#else
            SAPDB_UInt4 destroyableMessageCount = 0;
            Msg_List *  pLastMessageToDestroy = m_pNextMessage;

            // first loop to count sub entries to be destroyed before
            while (       pLastMessageToDestroy->m_pNextMessage
                 && (1 == pLastMessageToDestroy->m_pNextMessage->GetObjectRef()) )
            {
                pLastMessageToDestroy  =   pLastMessageToDestroy->m_pNextMessage;
                ++destroyableMessageCount;
            }

            if ( destroyableMessageCount > 0 )
            {
                // array of pointer to pointer to m_pNextMessage entries... 
                Msg_List *** pPointerToDestroy;
                pPointerToDestroy  = (Msg_List ***)RTE_IInterface::Instance().MsgListAllocator().Allocate(destroyableMessageCount * sizeof(Msg_List **));

                if ( !pPointerToDestroy )
                {
                    /* fallback to slow implementation ... O(2) */
                    // we loop very often, because the list is a single linked list ....
                    do {
                        Msg_List ** ppLastMessageToDestroy = &m_pNextMessage;
                        pLastMessageToDestroy = m_pNextMessage;

                        while (       pLastMessageToDestroy->m_pNextMessage
                             && (1 == pLastMessageToDestroy->m_pNextMessage->GetObjectRef()) )
                        {
                            ppLastMessageToDestroy = &(pLastMessageToDestroy->m_pNextMessage);
                            pLastMessageToDestroy  =   pLastMessageToDestroy->m_pNextMessage;
                        }

                        // destroy always clears given reference! *ppLastMessageToDestroy = 0 is implicit!!!
                        destroy(*ppLastMessageToDestroy, RTE_IInterface::Instance().MsgListAllocator());

                    } while ( m_pNextMessage != 0 );
                }
                else
                {
                    pLastMessageToDestroy = m_pNextMessage;
                    SAPDB_UInt4 iPointer;
                    // second loop to fill the pointer to pointer array
                    for ( iPointer = 0; iPointer < destroyableMessageCount; ++iPointer )
                    {
                        // fill in in reverse order!
                        pPointerToDestroy[destroyableMessageCount - iPointer - 1] = &(pLastMessageToDestroy->m_pNextMessage);
                        pLastMessageToDestroy = pLastMessageToDestroy->m_pNextMessage;
                    }

                    // third loop to destroy
                    for ( iPointer = 0; iPointer < destroyableMessageCount; ++iPointer )
                    {
                        // destroy always clears given reference! *ppLastMessageToDestroy = 0 is implicit!!!
                        destroy(*pPointerToDestroy[iPointer], RTE_IInterface::Instance().MsgListAllocator());
                    }
                    RTE_IInterface::Instance().MsgListAllocator().Deallocate(pPointerToDestroy);
                }
            }

            // finally we destroy the next message
            destroy( m_pNextMessage, RTE_IInterface::Instance().MsgListAllocator());
#endif
#endif
        }
        else
        {
            // --- subtract object reference of the next object!
            m_pNextMessage->SubtractObjectRef();
        }
    }

    if ( m_ObjectRefCnt > 0 )
    {
        --m_ObjectRefCnt;
    }

    if ( m_pMessageData->DataRefCnt > 0 )
	{
        --(m_pMessageData->DataRefCnt);
	}

    if ( 0 == m_pMessageData->DataRefCnt )
    {
// for memory leak search
//        printf("Deallocate m_pMessageData %p\n", m_pMessageData);
        RTE_IInterface::Instance().MsgListAllocator().Deallocate(m_pMessageData);
    }
    
    m_pMessageData  = 0;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool Msg_List::PushMessage ( Msg_List const &MessageList )
{
    SAPDB_Bool result = true;
    // --- don't append empty and equal message lists!
    if ( 0 != MessageList.m_pMessageData
        &&
        MessageList.m_pMessageData != m_pMessageData )
    {
        if ( 0 != m_pMessageData )
        {
            Msg_List tempCopy(*this);     // save old content

            DoClear();                    // clear old content 

            CopySelf(true, MessageList);  // copy new content

            Msg_List *pCurrMsg = this;
            SAPDB_UInt4 numberOfPushedMessages = 1;
        
            // --- step to the 'MessageList' end
            while ( 0 != pCurrMsg->m_pNextMessage )
            {
                // --- check if a list splitting is necessary because 
                //     of a multiple object reference. This means two or more
                //     objects are sharing the same data. Pushing a new message
                //     must not affect the contents of other message lists!!
                if ( pCurrMsg->m_pNextMessage->GetObjectRef() > 1 )
                {
                    pCurrMsg->m_pNextMessage->SubtractObjectRef();
                    pCurrMsg->m_pNextMessage = pCurrMsg->m_pNextMessage->SplitObjectList();
                    // --- is there still another object?
                    if ( 0 != pCurrMsg->m_pNextMessage )
                    {
                        ++numberOfPushedMessages;
                        pCurrMsg = pCurrMsg->m_pNextMessage;
                    }
                    else
                    {
                        result = false; // mark out of memory situation
                    }
                }
                else
                {
                    ++numberOfPushedMessages;
                    pCurrMsg = pCurrMsg->m_pNextMessage;
                }
            }
            m_NumOfMessages = numberOfPushedMessages;

            pCurrMsg->m_pNextMessage = new (RTE_IInterface::Instance().MsgListAllocator()) Msg_List;
            if ( !pCurrMsg->m_pNextMessage )
            {
                result = false; // mark out of memory situation
            }
            else
            {
                pCurrMsg->m_pNextMessage->CopySelf(true, tempCopy);  // append old content 
                if ( (m_NumOfMessages-1) == m_NumOfDetails )
                {
                    if ( !tempCopy.CheckIfHandledAsList() )
                    {
                        pCurrMsg->m_pNextMessage->m_NumOfMessages = 0;
                    }
                }
                m_NumOfMessages += tempCopy.m_NumOfMessages;

                SAPDB_UInt4 usedLimit = RTE_IInterface::Instance().GetMessageLimit();
                if ( usedLimit != 0 
                  && m_NumOfMessages > usedLimit )
                {
                    SAPDB_UInt4 iMsg;
                    SAPDB_UInt4 messagesToIgnore = m_NumOfMessages - usedLimit;
                    Msg_List *pLastMsgToIgnore = 0; // only for -Wall
                    Msg_List *pMsg = m_pNextMessage;
                    for ( iMsg = 0; iMsg < messagesToIgnore; ++iMsg )
                    {
                        pLastMsgToIgnore = pMsg;
                        pMsg = pMsg->m_pNextMessage;
                    }
                    --(m_pNextMessage->m_ObjectRefCnt);
                    // if second message was referenced by others, we have to keep the link reference count intact
                    if ( m_pNextMessage->m_ObjectRefCnt > 1 )
                    {
                        ++(pMsg->m_ObjectRefCnt);
                    }
                    else // otherwise we destroy the complete submessage list
                    {
                        if ( pLastMsgToIgnore )
                        {
                            pLastMsgToIgnore->m_pNextMessage = 0;
                        }
                        m_pNextMessage->m_NumOfMessages = messagesToIgnore;
                        destroy (m_pNextMessage, RTE_IInterface::Instance().MsgListAllocator());
                    }

                    m_pNextMessage     = pMsg;
                    m_NumOfMessages    = usedLimit;
                    m_NumOfSubMessages = 0;
                    m_NumOfDetails     = 0;
                    UpdateIgnoringMessage( 0
                                         , usedLimit
                                         , messagesToIgnore );
                }
            }
        }
        else
        {
            CopySelf(true, MessageList);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*/

Msg_List* Msg_List::SplitObjectList()
{
    Msg_List*      pMessageList;
    
    pMessageList = new (RTE_IInterface::Instance().MsgListAllocator()) Msg_List;
    if ( 0 != pMessageList )
    {
        pMessageList->m_pMessageData  = m_pMessageData;
        if ( 0 != m_pMessageData )
        {
            pMessageList->m_OutputSequenceNumber = 0;
            pMessageList->m_OutputDateTime = EmptyDateTimeValue;
            pMessageList->m_OutputProcessId = 0;
            pMessageList->m_OutputThreadId = 0;
            pMessageList->m_OutputTaskId = 0;
            pMessageList->m_OutputSchedulerId = 0;
            pMessageList->m_DumpTruncated = false;

            pMessageList->m_NumOfMessages = m_NumOfMessages;
            pMessageList->m_NumOfSubMessages = m_NumOfSubMessages;
            pMessageList->m_NumOfDetails = m_NumOfDetails;
            pMessageList->m_pRegistrySlot = 0;

            ++(pMessageList->m_pMessageData->DataRefCnt);
            pMessageList->m_ObjectRefCnt = 1;
        
            if ( 0 != m_pNextMessage )
            {
                pMessageList->m_pNextMessage = m_pNextMessage->SplitObjectList();
            }
            else
            {
                pMessageList->m_pNextMessage = 0;
            }
        }
    }
    return pMessageList;
}

/*---------------------------------------------------------------------------*/

void Msg_List::CopySelf( SAPDB_Bool const DoRegister,
                         Msg_List const &MessageList )
{
    if ( 0 == m_pMessageData && 0 != MessageList.m_pMessageData )
    {
        m_ObjectRefCnt         = 0;
        m_OutputSequenceNumber = 0;
        m_OutputDateTime       = EmptyDateTimeValue;
        m_OutputProcessId      = 0;
        m_OutputThreadId       = 0;
        m_OutputTaskId         = 0;
        m_OutputSchedulerId    = 0;
        m_DumpTruncated        = false;
    }

    if ( 0 == MessageList.m_pMessageData )
    {
        m_pMessageData = 0;
    }
    else
    {
        SAPDB_UInt4 usedLimit = RTE_IInterface::Instance().GetMessageLimit();
        if ( usedLimit != 0
          && MessageList.m_NumOfMessages > usedLimit )
        {
            SAPDB_UInt4 messagesToIgnore = MessageList.m_NumOfMessages - usedLimit;
            Msg_List *pMsg = MessageList.m_pNextMessage;
            while ( --messagesToIgnore > 0 )
            {
                pMsg = pMsg->m_pNextMessage;
            }

            m_pMessageData     = MessageList.m_pMessageData;
            m_pNextMessage     = pMsg;
            m_NumOfMessages    = usedLimit;
            m_NumOfSubMessages = 0;
            m_NumOfDetails     = 0;
            UpdateIgnoringMessage( 0
                                 , usedLimit
                                 // second message will be replaced...
                                 , 1 + (MessageList.m_NumOfMessages - usedLimit) );
        }
        else
        {
            m_pMessageData     = MessageList.m_pMessageData;
            m_pNextMessage     = MessageList.m_pNextMessage;
            m_NumOfMessages    = MessageList.m_NumOfMessages;
            m_NumOfSubMessages = MessageList.m_NumOfSubMessages;
            m_NumOfDetails     = MessageList.m_NumOfDetails;
        }

        if ( DoRegister )
        {
            m_pRegistrySlot    = RTE_IInterface::Instance().MsgListRegister(*this);
        }
        else
        {
            m_pRegistrySlot    = 0;
        }

        ++(m_pMessageData->DataRefCnt);
        ++m_ObjectRefCnt;

        // --- add object reference!
        if ( 0 != m_pNextMessage )
        {
            ++(m_pNextMessage->m_ObjectRefCnt);
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 const *Msg_List::MessageWithArguments( SAPDB_UInt4 bufferSize,
                                                  SAPDB_Char *buffer,
                                                  SAPDB_UInt4 &neededPascalSize ) const
{
    if ( 0 == m_pMessageData )
    {
        neededPascalSize = 0;
        if ( bufferSize > 0 )
        {
            *buffer = 0;
        }
        return (SAPDB_UTF8 const *)"";
    }

    SAPDB_Char *ptr = buffer;
    SAPDB_UInt4 sizeLeft = bufferSize;
    SAPDB_UInt4 resultLen;

    resultLen = SAPDB_strlen(Message()); neededPascalSize = resultLen;
    if ( sizeLeft >= resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, Message(), resultLen); ptr += resultLen; }

    SAPDB_UInt4 *pArgumentOffsetArray = (SAPDB_UInt4 *)m_pMessageData->VariablePart;
    for ( SAPDB_UInt4 iArg = 0; iArg < m_pMessageData->ArgumentCount; ++iArg )
    {
        if ( sizeLeft ) { --sizeLeft; *ptr++ = ','; ++neededPascalSize; }

        resultLen = SAPDB_strlen(m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg+1]); neededPascalSize += resultLen;
        if ( sizeLeft >= resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg+1], resultLen); ptr += resultLen; }
    }

    if ( sizeLeft ) { *ptr++ = 0; }

    if ( neededPascalSize > bufferSize )
    {
        return (SAPDB_UTF8 const *)"buffer space exhausted";
    }

    return (SAPDB_UTF8 *)buffer;

}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 const *Msg_List::SubstituteArgumentTags( SAPDB_Char const  *textWithTags
                                                  , SAPDB_UInt4 const  tagCount
                                                  , SAPDB_Char const **tagNameArray
                                                  , SAPDB_Char const **tagValueArray
                                                  , SAPDB_UInt4        bufferSize
                                                  , SAPDB_Char        *pBuffer
                                                  , SAPDB_UInt4       &neededSize
                                                  , SAPDB_Bool         addListOfExtraArguments )
{
    SAPDB_Char *ptr = pBuffer;
    SAPDB_UInt4 sizeLeft = bufferSize;
    SAPDB_UInt4 resultLen;
    SAPDB_Char const *pText = textWithTags;
    resultLen = 0;
    neededSize = 0;

    SAPDB_Bool * pTagsUsed = 0; // only for -Wall
    
    if ( tagCount > 0 && addListOfExtraArguments )
    {   // used to detect unused tags...
        pTagsUsed = (SAPDB_Bool*)alloca(tagCount * sizeof(SAPDB_Bool));
        SAPDB_memset(pTagsUsed, 0, tagCount * sizeof(SAPDB_Bool));
    }

    while ( *pText )
    {
        if ( *pText != '$' ) // normal character in short text
        {
            neededSize++;
            if ( sizeLeft > 1 )
            {
                --sizeLeft;
                *ptr = *pText;
                ++ptr;
            }
            ++pText;
        }
        else // tag replacement code
        {
            SAPDB_Char const *pTagToReplaceStart = 0;
            SAPDB_Char const *pTagToReplaceEnd   = 0;
            SAPDB_UInt4 tagToReplaceLength = 0;

            if ( *(pText+1) == '$' ) // $$ is no tag but a single dollar sign ;-)
            {
                neededSize++;
                if ( sizeLeft > 1 )
                {
                    --sizeLeft;
                    *ptr = *pText;
                    ++ptr;
                }
                pText += 2;
            }
            else
            {
                pTagToReplaceStart = pText+1;
                pTagToReplaceEnd = pTagToReplaceStart;

                while ( *pTagToReplaceEnd != 0 ) // search tag end and calculate tag length
                {
                    if ( *pTagToReplaceEnd == '$' )
                        break;
                    ++pTagToReplaceEnd;
                    ++tagToReplaceLength;
                }

                if ( *pTagToReplaceEnd != '$' ) // tag was not terminated
                {
                    neededSize++;
                    if ( sizeLeft > 1 )
                    {
                        --sizeLeft;
                        *ptr = *pText;
                        ++ptr;
                    }
                    ++pText;
                }
                else // valid terminated tag found in short text
                {
                    SAPDB_UInt4 iArg;
                    // search matching argument
                    for ( iArg = 0; iArg < tagCount; ++iArg )
                    {
                        SAPDB_Char const *pTag;
                        pTag = tagNameArray[iArg];
                        if ( SAPDB_strncasecmp(pTag, pTagToReplaceStart, tagToReplaceLength) == 0 
                          && pTag[tagToReplaceLength] == 0 )
                        {
                            SAPDB_Char const *pValue = tagValueArray[iArg];
                            resultLen = SAPDB_strlen(pValue); neededSize += resultLen;
                            if ( sizeLeft >= resultLen ) 
                            {
                                sizeLeft -= resultLen; SAPDB_memcpy(ptr, pValue, resultLen); ptr += resultLen;
                            }
                            else if ( sizeLeft > 1 )
                            {
                                SAPDB_memcpy(ptr, pValue, sizeLeft-1); ptr += (sizeLeft-1); sizeLeft = 1;
                            }
                            pText += (tagToReplaceLength + 2);
                            if ( addListOfExtraArguments )
                            {
                                pTagsUsed[iArg] = true;
                            }
                            break;
                        }
                    }

                    if ( iArg >= tagCount ) // tag not found, copy it into destination
                    {
                        neededSize += tagToReplaceLength + 2; // tag + framing $s
                        if ( sizeLeft > 1 )
                        {
                            --sizeLeft;
                            *ptr = *pText; // copy first '$'
                            ++ptr;
                        }
                        ++pText;
                        if ( sizeLeft > tagToReplaceLength )
                        {
                            sizeLeft -= tagToReplaceLength;
                            SAPDB_memcpy(ptr, pText, tagToReplaceLength); // copy tag
                            ptr += tagToReplaceLength;
                        }
                        else if ( sizeLeft > 1 )
                        {
                            SAPDB_memcpy(ptr, pText, sizeLeft-1); // copy tag
                            ptr += (sizeLeft-1);
                            sizeLeft = 1;
                        }
                        pText += tagToReplaceLength;
                        if ( sizeLeft > 1 )
                        {
                            --sizeLeft;
                            *ptr = *pText; // copy second '$'
                            ++ptr;
                        }
                        ++pText;
                    }
                }
            }
        }
    }

    if ( addListOfExtraArguments )
    {
        // add arguments not found in short text
        for ( SAPDB_UInt4 iArg = 0; iArg < tagCount; ++iArg )
        {
            if ( !pTagsUsed[iArg] && *tagNameArray[iArg] != '_' ) // suppress unused internal tags
            {
                SAPDB_Char const *pTag = tagNameArray[iArg];
                SAPDB_Char const *pValue = tagValueArray[iArg];

                neededSize++;
                if ( sizeLeft > 1 )
                {
                    --sizeLeft;
                    *ptr = ',';
                    ++ptr;
                }

                resultLen = SAPDB_strlen(pTag); neededSize += resultLen;
                if ( sizeLeft >= resultLen )
                {
                    sizeLeft -= resultLen; SAPDB_memcpy(ptr, pTag, resultLen); ptr += resultLen;
                }
                else if ( sizeLeft > 1 )
                {
                    SAPDB_memcpy(ptr, pTag, sizeLeft-1); ptr += (sizeLeft-1); sizeLeft = 1;
                }

                neededSize++;
                if ( sizeLeft > 1 )
                {
                    --sizeLeft;
                    *ptr = '=';
                    ++ptr;
                }

                resultLen = SAPDB_strlen(pValue); neededSize += resultLen;
                if ( sizeLeft >= resultLen )
                {
                    sizeLeft -= resultLen; SAPDB_memcpy(ptr, pValue, resultLen); ptr += resultLen;
                }
                else if ( sizeLeft > 1 )
                {
                    SAPDB_memcpy(ptr, pValue, sizeLeft-1); ptr += (sizeLeft-1); sizeLeft = 1;
                }
            }
        }
    }

    ++neededSize; if ( sizeLeft ) { *ptr++ = 0; }

    if ( neededSize > bufferSize )
    {
        return (SAPDB_UTF8 const *)"buffer space exhausted";
    }

    return (SAPDB_UTF8 *)pBuffer;

}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 const *Msg_List::MessageWithInsertedArguments( SAPDB_UInt4 bufferSize,
                                                          SAPDB_Char *buffer,
                                                          SAPDB_UInt4 &neededSize,
                                                          SAPDB_Bool  addListOfExtraArguments ) const
{
    if ( 0 == m_pMessageData )
    {
        neededSize = 0;
        if ( bufferSize > 0 )
        {
            *buffer = 0;
        }
        return (SAPDB_UTF8 const *)"";
    }

    SAPDB_UInt4  tagCount;
    SAPDB_Char const **tagNameArray;
    SAPDB_Char const **tagValueArray;

    tagCount = m_pMessageData->ArgumentCount;

    SAPDB_UInt4 *pArgumentOffsetArray = (SAPDB_UInt4 *)m_pMessageData->VariablePart;

    /*

    Internal tags  _TYPE = TypeAsString()
                   _COMP = Component()
                   _ID   = SAPDB_ToString(DateTime(timebuffer))
                   _FILE = FileName()
                   _LINE = SAPDB_ToString(LineNumber())
                   _TIME = DateTime(timebuffer)

             optional tags (settable by SetCallerIdentification() )

                   _PROCESS = SAPDB_ToString(m_OutputProcessId)
                   _THREAD = SAPDB_ToString(m_OutputThreadId)
                   _TASK = SAPDB_ToString(m_OutputTaskId)
                   _UKT = SAPDB_ToString(m_OutputSchedulerId)

            = 10

                   _TEXT !!! Recursiv !!! not used !!!
                   _NO !!! pure internal not used in message text
                   _DETAILS !!! pure internal not used in message text
     */
    ++tagCount; // _TYPE = TypeAsString()

    ++tagCount; // _COMP = Component()

    ++tagCount; // _ID   = SAPDB_ToString(ID())

    ++tagCount; // _TIME = DateTime(timebuffer)

    ++tagCount; // _FILE = FileName()

    ++tagCount; // _LINE = SAPDB_ToString(LineNumber())

    if ( 0 != m_OutputProcessId )
        ++tagCount; // _PROCESS = SAPDB_ToString(m_OutputProcessId)

    if ( 0 != m_OutputThreadId )
        ++tagCount; // _THREAD = SAPDB_ToString(m_OutputThreadId)

    if ( 0 != m_OutputTaskId )
        ++tagCount; // _TASK = SAPDB_ToString(m_OutputTaskId)

    if ( 0 != m_OutputSchedulerId )
        ++tagCount; // _UKT = SAPDB_ToString(m_OutputSchedulerId)

    tagNameArray = (SAPDB_Char const **)alloca(tagCount * sizeof(SAPDB_Char *));
    tagValueArray = (SAPDB_Char const **)alloca(tagCount * sizeof(SAPDB_Char *));

    SAPDB_UInt4 iArg;
    for ( iArg = 0; iArg < m_pMessageData->ArgumentCount; ++iArg )
    {
        tagNameArray[iArg] = m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg];
        tagValueArray[iArg] = m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg+1];
    }

    tagNameArray[iArg] = MSG_LIST_MESSAGE_TYPE_TAG;
    tagValueArray[iArg] = TypeAsString();

    ++iArg;
    tagNameArray[iArg] = MSG_LIST_COMPONENT_TAG;
    tagValueArray[iArg] = Component();

    ++iArg;
    tagNameArray[iArg] = MSG_LIST_ID_TAG;
    SAPDB_ToStringClass msgId(ID());
    tagValueArray[iArg] = (SAPDB_Char const *)msgId;

    ++iArg;
    tagNameArray[iArg] = MSG_LIST_TIMESTAMP_TAG;
    LongTimeBuffer timeBuffer;
    tagValueArray[iArg] = DateTime(timeBuffer);

    ++iArg;
    tagNameArray[iArg] = MSG_LIST_FILE_NAME_TAG;
    tagValueArray[iArg] = FileName();

    ++iArg;
    tagNameArray[iArg] = MSG_LIST_LINE_NUMBER_TAG;
    SAPDB_ToStringClass msgLineNumber(LineNumber());
    tagValueArray[iArg] = (SAPDB_Char const *)msgLineNumber;

    SAPDB_ToStringClass msgProcessId(m_OutputProcessId);
    if ( 0 != m_OutputProcessId )
    {
        ++iArg;
        tagNameArray[iArg] = MSG_LIST_PROCESS_ID_TAG;
        tagValueArray[iArg] = (SAPDB_Char const *)msgProcessId;
    }

    SAPDB_ToStringClass msgThreadId(m_OutputThreadId, SAPDB_ToStringClass::hex);
    if ( 0 != m_OutputThreadId )
    {
        ++iArg;
        tagNameArray[iArg] = MSG_LIST_THREAD_ID_TAG;
        tagValueArray[iArg] = (SAPDB_Char const *)msgThreadId;
    }

    SAPDB_ToStringClass msgTaskId(m_OutputTaskId);
    if ( 0 != m_OutputTaskId )
    {
        ++iArg;
        tagNameArray[iArg] = MSG_LIST_TASK_ID_TAG;
        tagValueArray[iArg] = (SAPDB_Char const *)msgTaskId;
    }

    SAPDB_ToStringClass msgUktId(m_OutputSchedulerId);
    if ( 0 != m_OutputSchedulerId )
    {
        ++iArg;
        tagNameArray[iArg] = MSG_LIST_UKT_ID_TAG;
        tagValueArray[iArg] = (SAPDB_Char const *)msgUktId;
    }

    return SubstituteArgumentTags( Message()
                                 , tagCount
                                 , tagNameArray
                                 , tagValueArray
                                 , bufferSize
                                 , buffer
                                 , neededSize
                                 , addListOfExtraArguments );
}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 const *Msg_List::Old_Output( SAPDB_UInt4 bufferSize,
                                        SAPDB_Char *buffer,
                                        SAPDB_UInt4 &neededSize,
                                        Msg_List &errList) const
{
    if ( 0 == m_pMessageData )
    {
        neededSize = 0;
        if ( bufferSize > 0 )
        {
            *buffer = 0;
        }
        return (SAPDB_UTF8 const *)"";
    }

    SAPDB_UInt4 iMsg;
    Msg_List const *pMsg = this;
    SAPDB_Char entryLine[1024];
    SAPDB_Char diagLine[1024];

    SAPDB_Char *ptr = buffer;
    SAPDB_UInt4 sizeLeft = bufferSize;
    SAPDB_UInt4 resultLen;
    SAPDB_UInt4 entryLen;

    SAPDB_sprintf ( entryLine, sizeof(entryLine), MSG_LIST_SHORT_TIME_HEADER_FORMAT_STR " %#-10.10x ",
                m_OutputDateTime.Year,
                m_OutputDateTime.Month,
                m_OutputDateTime.Day,
                m_OutputDateTime.Hour,
                m_OutputDateTime.Minute,
                m_OutputDateTime.Second,
                m_OutputThreadId );
    entryLen = SAPDB_strlen(entryLine);

    switch( m_pMessageData->Type )
    {
    case Error:
        SAPDB_sprintf(entryLine+entryLen, sizeof(entryLine)-entryLen, "ERR ");
        break;
    case Warning:
        SAPDB_sprintf(entryLine+entryLen, sizeof(entryLine)-entryLen, "WRN ");
        break;
    default:
        SAPDB_sprintf(entryLine+entryLen, sizeof(entryLine)-entryLen, "    ");
        break;
    }
    entryLen = SAPDB_strlen(entryLine);

    SAPDB_sprintf(entryLine+entryLen, sizeof(entryLine)-entryLen, "%5u %8.8s ",
                m_pMessageData->ID, Component() );
    entryLen = SAPDB_strlen(entryLine);

    neededSize = 0;
    for ( iMsg = 0; iMsg < m_NumOfMessages; iMsg++, pMsg = pMsg->m_pNextMessage )
    {
        if ( !pMsg->m_pMessageData )
        {
            continue;
        }

        SAPDB_sprintf ( diagLine, sizeof(diagLine), 
                        "%s:%u\n",
                        pMsg->FileName(), pMsg->LineNumber() );
        resultLen = SAPDB_strlen(diagLine); neededSize += (entryLen + resultLen);
        if ( sizeLeft > (entryLen + resultLen) ) { sizeLeft -= (entryLen + resultLen); SAPDB_memcpy(ptr, entryLine, entryLen); ptr += entryLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf ( diagLine, sizeof(diagLine), MSG_LIST_SHORT_TIME_HEADER_FORMAT_STR " ",
                        pMsg->DateTime().Year,
                        pMsg->DateTime().Month,
                        pMsg->DateTime().Day,
                        pMsg->DateTime().Hour,
                        pMsg->DateTime().Minute,
                        pMsg->DateTime().Second );
        resultLen = SAPDB_strlen(diagLine); neededSize += (entryLen + resultLen);
        if ( sizeLeft > (entryLen + resultLen) ) { sizeLeft -= (entryLen + resultLen); SAPDB_memcpy(ptr, entryLine, entryLen); ptr += entryLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "%s ", pMsg->Component() );
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        switch( pMsg->m_pMessageData->Type )
        {
        case Error:
            SAPDB_sprintf(diagLine, sizeof(diagLine), "ERROR ");
            break;
        case Warning:
            SAPDB_sprintf(diagLine, sizeof(diagLine), "WARNING ");
            break;
        default:
            SAPDB_sprintf(diagLine, sizeof(diagLine), "INFO ");
            break;
        }
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "%5u\n", pMsg->m_pMessageData->ID);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "%s", pMsg->Message() );
        resultLen = SAPDB_strlen(diagLine); neededSize += (entryLen + resultLen);
        if ( sizeLeft > (entryLen + resultLen) ) { sizeLeft -= (entryLen + resultLen); SAPDB_memcpy(ptr, entryLine, entryLen); ptr += entryLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_UInt4 *pArgumentOffsetArray = (SAPDB_UInt4 *)pMsg->m_pMessageData->VariablePart;
        SAPDB_UInt4 iArg;

        for ( iArg = 0; iArg < pMsg->m_pMessageData->ArgumentCount; ++iArg )
        {
            SAPDB_sprintf(diagLine, sizeof(diagLine), ",%s",
                pMsg->m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg+1]);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }
        }

        ++neededSize; if ( sizeLeft ) { *ptr++ = '\n'; --sizeLeft; } // terminate line
    }

    ++neededSize; if ( sizeLeft ) { *ptr = 0; } // terminate output

    if ( neededSize > bufferSize )
    {
        return (SAPDB_UTF8 const *)"buffer space exhausted";
    }

    return (SAPDB_UTF8 *)buffer;
}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 const *Msg_List::XML_Output( SAPDB_UInt4 bufferSize,
                                        SAPDB_Char *buffer,
                                        SAPDB_UInt4 &neededSize,
                                        Msg_List &errList) const
{
    if ( 0 == m_pMessageData )
    {
        neededSize = 0;
        if ( bufferSize > 0 )
        {
            *buffer = 0;
        }
        return (SAPDB_UTF8 const *)"";
    }

    SAPDB_Char diagLine[1024];

    SAPDB_Char *ptr = buffer;
    SAPDB_UInt4 sizeLeft = bufferSize;

    neededSize = 0;

    CopyCString(ptr, sizeLeft, neededSize, "<" MSG_LIST_MESSAGE_LIST_TAG);

    CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_MESSAGE_NUMBER_TAG, m_OutputSequenceNumber);

    if ( m_OutputProcessId != 0 )
    {
        CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_PROCESS_ID_TAG, m_OutputProcessId);
    }

    if ( m_OutputThreadId != 0 )
    {
        SAPDB_Char threadIdAsHexString[12];
        SAPDB_sprintf(threadIdAsHexString,12,"0x%X",m_OutputThreadId);
        CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_THREAD_ID_TAG,threadIdAsHexString );
    }

    if ( m_OutputTaskId != 0 )
    {
        CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_TASK_ID_TAG, m_OutputTaskId);
    }

    if ( m_OutputSchedulerId != 0 )
    {
        CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_UKT_ID_TAG, m_OutputSchedulerId);
    }

    SAPDB_sprintf ( diagLine, sizeof(diagLine), MSG_LIST_LONG_TIME_HEADER_FORMAT_STR,
                    m_OutputDateTime.Year,
                    m_OutputDateTime.Month,
                    m_OutputDateTime.Day,
                    m_OutputDateTime.Hour,
                    m_OutputDateTime.Minute,
                    m_OutputDateTime.Second,
                    m_OutputDateTime.Milliseconds );
    CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_TIMESTAMP_TAG, diagLine);

    CopyCString(ptr, sizeLeft, neededSize, ">");

    XML_ContainedMessages(m_NumOfMessages, 0, (SAPDB_UInt4)-1, 1, sizeLeft, ptr, neededSize, errList);

    // End tag for this message list
    CopyIndentedCString(1, ptr, sizeLeft, neededSize, "</" MSG_LIST_MESSAGE_LIST_TAG ">");

    ++neededSize; if ( sizeLeft ) { *ptr = 0; } // terminate line

    return (SAPDB_UTF8 *)buffer;
}

/*!
  @brief update the 'ignoring message' in case of message limit exceeded

  First inspect the second message in the message list for being an 'IgnoringMessage'.

  If it does not exist, it is the special case of a 'maximum of 1 message'. In this case
  the ignoring message is always added... even if the limit is exceeded... Otherwise
  the 'ignore' information would be lost.

  If it is no 'ignoring message' the second message found is replaced by an ignoring message.
  The number of messages ignored in that case is (1 + additionalMessagesToIgnore). The
  current main messages may no longer contain details or submessages. Therefore the detail
  count and the submessages count is reset if no new main message is given.

  The new main message is replacing the old main message. The details and submessages count
  of the first message is always reset if the second message must indicate ignoring of messages

  If an 'ignoring message' is found, its current number of ignored messages is retrieved and
  the additionalMessagesToIgnore is added. The result is used as parameter value for the 
  details tag. The 'usedLimit' is always used to replace the former
  'usedLimit'.

  The tags 
  SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__LIMIT
  and 
  SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__ENTRY_COUNT
  are defined in SDBMsg_Messages.h. The message number Messages_IGNORED_MESSAGES is also
  found in that header file as enumeration macro.

  @param pNewMainMsg [in] new main message to replace old first message (can be 0 for adding detail)
  @param usedLimit [in] the value of message list length limit found when message list was to be added
  @param additionalMessagesToIgnore [in] number of additional messages to ignore
 */
void Msg_List::UpdateIgnoringMessage( Msg_List *pMsg
                                    , SAPDB_UInt4 usedLimit
                                    , SAPDB_UInt4 additionalMessagesToIgnore)
{
    if ( !m_pNextMessage ) // strange case for usedLimit == 1
    {
        m_pNextMessage = new (RTE_IInterface::Instance().MsgListAllocator()) Msg_List(
	        Msg_List::Warning,
	        SDBMSG_MESSAGES_IGNORED_MESSAGES,
	        Msg_Arg(SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__LIMIT,usedLimit),
	        Msg_Arg(SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__ENTRY_COUNT,additionalMessagesToIgnore)
        );
        m_NumOfMessages = (m_pNextMessage ? 2 : 1);
    }
    else
    {
        Msg_List *pOldSecondMessage = m_pNextMessage;
        SAPDB_UInt4 messageToIgnore = additionalMessagesToIgnore;

        if ( m_pNextMessage->m_pMessageData->ID == (SAPDB_UInt4)Messages_IGNORED_MESSAGES )
        {
            SAPDB_UInt2 iArg;
            SAPDB_UInt4 *pArgumentOffsetArray = (SAPDB_UInt4 *)m_pNextMessage->m_pMessageData->VariablePart;
            // search and add old entry count if already ignoring...
            for ( iArg = 0; iArg < m_pNextMessage->m_pMessageData->ArgumentCount; ++iArg )
            {
                if ( 0 == strcmp( m_pNextMessage->m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg],
                                    SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__ENTRY_COUNT ) )
                {
                    messageToIgnore += strtoul( m_pNextMessage->m_pMessageData->VariablePart
                                            + pArgumentOffsetArray[2*iArg+1], 0, 0);
                }
            }
        }

        // create new ignore message
        m_pNextMessage = new (RTE_IInterface::Instance().MsgListAllocator()) Msg_List(
	        Msg_List::Warning,
	        SDBMSG_MESSAGES_IGNORED_MESSAGES,
	        Msg_Arg(SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__LIMIT,usedLimit),
	        Msg_Arg(SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__ENTRY_COUNT,messageToIgnore)
        );
        if ( !m_pNextMessage )
        {
            m_pNextMessage = pOldSecondMessage;
            return;
        }

        // reuse link in old message
        ++(m_pNextMessage->m_ObjectRefCnt);
        m_pNextMessage->m_pNextMessage = pOldSecondMessage->m_pNextMessage;

        --(pOldSecondMessage->m_ObjectRefCnt);
        // if old second message was referenced by others, we have to keep the link reference count intact
        if ( pOldSecondMessage->m_ObjectRefCnt > 1 )
        {
            ++(m_pNextMessage->m_pNextMessage->m_ObjectRefCnt);
        }
        else // otherwise we destroy the single message which is replaced now
        {
            pOldSecondMessage->m_pNextMessage = 0;
            pOldSecondMessage->m_NumOfMessages = 1;
            destroy (pOldSecondMessage, RTE_IInterface::Instance().MsgListAllocator());
        }
    }

    // finally reset details and submessages counter!
    // they would otherwise confuse the XML_Output generation completely...
    m_NumOfSubMessages = 0;
    m_NumOfDetails = 0;
}

/*---------------------------------------------------------------------------*/

void Msg_List::XML_ContainedMessages( SAPDB_UInt4 containedMessages
                                    , SAPDB_UInt4 firstMessage
                                    , SAPDB_UInt4 firstSubMessage
                                    , SAPDB_UInt4 listLevel
                                    , SAPDB_UInt4 &sizeLeft
                                    , SAPDB_Char *&ptr
                                    , SAPDB_UInt4 &neededSize
                                    , Msg_List &errList) const
{
    SAPDB_UInt4 iMsg;
    Msg_List const *pMsg = this;
    SAPDB_UInt4 outstandingDetails = 0;

    for ( iMsg = 0; iMsg < containedMessages; iMsg++, pMsg = pMsg->m_pNextMessage )
    {
        if ( (pMsg->m_NumOfSubMessages > (pMsg->m_NumOfDetails+1)) 
          && ((firstMessage+iMsg) != firstSubMessage) )
        {
            if ( outstandingDetails != 0 ) // should not happen
            {
                --listLevel;
                CopyIndentedCString( 2 * listLevel, ptr, sizeLeft, neededSize, "</" MSG_LIST_CONTAINED_DETAILS_LIST_TAG ">");
            }

            CopyIndentedCString( 2 * listLevel, ptr, sizeLeft, neededSize, "<" MSG_LIST_CONTAINED_MESSAGE_LIST_TAG ">");

            SAPDB_UInt4 innerListCounter = pMsg->m_NumOfSubMessages;
            pMsg->XML_ContainedMessages( innerListCounter, firstMessage + iMsg, firstMessage + iMsg, listLevel+1, sizeLeft, ptr, neededSize, errList );

            CopyIndentedCString( 2 * listLevel, ptr, sizeLeft, neededSize, "</" MSG_LIST_CONTAINED_MESSAGE_LIST_TAG ">");

            iMsg += innerListCounter;
            while ( innerListCounter-- > 1 )
            {
                pMsg = pMsg->m_pNextMessage;
            }
        }
        else
        {
            // Message start tag

            CopyIndentedCString(2 * listLevel, ptr, sizeLeft, neededSize, "<" MSG_LIST_MESSAGE_TAG);

            CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_MESSAGE_NUMBER_TAG, 1 + firstMessage + iMsg);
            if ( Msg_List::Info != pMsg->Type() )
            {
                CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_MESSAGE_TYPE_TAG, pMsg->TypeAsString() );
            }
            CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_ID_TAG, pMsg->ID());
            CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_COMPONENT_TAG, pMsg->Component());
            CopyArgument(ptr, sizeLeft, neededSize, MSG_LIST_TEXT_TAG, (SAPDB_UTF8 const *)pMsg->Message());

            CopyCString(ptr, sizeLeft, neededSize, ">");

            SAPDB_UInt4 tagIndent = 2 * listLevel + 1;
            SAPDB_UInt4 argIndent = tagIndent + 1;

            // Argument tags including arguments of message as attributes

            CopyIndentedCString(tagIndent, ptr, sizeLeft, neededSize, "<" MSG_LIST_ARGUMENTS_TAG);

            CopyIntentedArgument(argIndent, ptr, sizeLeft, neededSize, MSG_LIST_FILE_NAME_TAG, (SAPDB_UTF8 const *)pMsg->FileName());
            CopyIntentedArgument(argIndent, ptr, sizeLeft, neededSize, MSG_LIST_LINE_NUMBER_TAG, pMsg->LineNumber());

            LongTimeBuffer timeBuffer;
            CopyIntentedArgument(argIndent, ptr, sizeLeft, neededSize, MSG_LIST_TIMESTAMP_TAG,  pMsg->DateTime(timeBuffer));

            if ( pMsg->NumOfDetails() > 0 )
            {
                CopyIntentedArgument(argIndent, ptr, sizeLeft, neededSize, MSG_LIST_DETAILS_TAG, pMsg->NumOfDetails());
            }

            SAPDB_UInt4 *pArgumentOffsetArray = (SAPDB_UInt4 *)pMsg->m_pMessageData->VariablePart;
            SAPDB_UInt4 iArg;

            for ( iArg = 0; iArg < pMsg->m_pMessageData->ArgumentCount; ++iArg )
            {
                CopyIntentedArgument( argIndent, ptr, sizeLeft, neededSize
                                           , (SAPDB_Char const *)(pMsg->m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg])
                                           , (SAPDB_UTF8 const *)(pMsg->m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg+1]));
            }

            CopyIndentedCString(tagIndent, ptr, sizeLeft, neededSize, " />");

            // End tag for this message
            CopyIndentedCString(2 * listLevel, ptr, sizeLeft, neededSize, "</" MSG_LIST_MESSAGE_TAG ">");

            if ( pMsg->NumOfDetails() > 0 )
            {
                outstandingDetails = pMsg->NumOfDetails() + 1;
                CopyIndentedCString( 2 * listLevel, ptr, sizeLeft, neededSize, "<" MSG_LIST_CONTAINED_DETAILS_LIST_TAG ">");
                ++listLevel;
            }
        }

        if ( outstandingDetails > 0 )
        {
            --outstandingDetails;
            if ( outstandingDetails == 0 )
            {
                --listLevel;
                CopyIndentedCString( 2 * listLevel, ptr, sizeLeft, neededSize, "</" MSG_LIST_CONTAINED_DETAILS_LIST_TAG ">");
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const *Msg_List::HexDump_Output( SAPDB_UInt4 bufferSize, 
                                            SAPDB_Char *buffer,
                                            SAPDB_UInt4 &neededSize,
                                            Msg_List &errList) const
{
    if ( 0 == m_pMessageData )
    {
        neededSize = 0;
        if ( bufferSize > 0 )
        {
            *buffer = 0;
        }
        return (SAPDB_Char const *)"";
    }

    SAPDB_UInt4 iMsg;
    Msg_List const *pMsg = this;
    SAPDB_Char *ptr = buffer;

    SAPDB_UInt4 neededSpace = 2 * sizeof(SAPDB_UInt4);
    if ( neededSpace < bufferSize )
    {
        SAPDB_sprintf( ptr,
                       bufferSize,
                       "%08x",
                       m_NumOfMessages );
        ptr += 2 * sizeof(SAPDB_UInt4);
    }

    for ( iMsg = 0; iMsg < m_NumOfMessages; iMsg++, pMsg = pMsg->m_pNextMessage )
    {
        if ( 0 == pMsg->m_pMessageData )
        {
            continue;
        }

        neededSpace += 2 * (2 * sizeof(SAPDB_UInt4) + pMsg->m_pMessageData->DataLen) + 1;
        if ( neededSpace < bufferSize )
        {
            SAPDB_sprintf(  ptr,
                            bufferSize - (ptr - buffer),
                            "\n%08x%08x%*.*r",
                            iMsg,
                            pMsg->m_pMessageData->DataLen,
                            pMsg->m_pMessageData->DataLen,
                            pMsg->m_pMessageData->DataLen,
                            pMsg->m_pMessageData);
            ptr += SAPDB_strlen(ptr);
        }
    }

    if ( neededSpace > bufferSize )
    {
        return "buffer space exhausted";
    }

    return buffer;
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const *Msg_List::Diagnostic_Output(SAPDB_UInt4 bufferSize, 
                                              SAPDB_Char *buffer,
                                              SAPDB_UInt4 &neededSize,
                                              Msg_List &errList) const
{
    if ( 0 == m_pMessageData )
    {
        neededSize = 0;
        if ( bufferSize > 0 )
        {
            *buffer = 0;
        }
        return (SAPDB_Char const *)"";
    }

    SAPDB_Char diagLine[1024];

    SAPDB_UInt4 iMsg;
    Msg_List const *pMsg = this;
    SAPDB_Char *ptr = buffer;
    SAPDB_UInt4 sizeLeft = bufferSize;
    SAPDB_UInt4 resultLen;

    neededSize = 0;

    for ( iMsg = 0; iMsg < m_NumOfMessages; iMsg++, pMsg = pMsg->m_pNextMessage )
    {
        SAPDB_sprintf(diagLine, sizeof(diagLine), "MessageNo=%u\n", iMsg);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_pMessageData=@%p\n", pMsg->m_pMessageData);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        if ( pMsg->m_pMessageData )
        {
            SAPDB_sprintf(diagLine, sizeof(diagLine), "Version=%u\n", pMsg->m_pMessageData->Version);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "DataLen=%u\n", pMsg->m_pMessageData->DataLen);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "DataRefCnt=%u\n", pMsg->m_pMessageData->DataRefCnt);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "Type=%u\n", pMsg->m_pMessageData->Type);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "ID=%u\n", pMsg->m_pMessageData->ID);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            LongTimeBuffer timeBuffer;

            SAPDB_sprintf(diagLine, sizeof(diagLine), "DateTime=%s\n", pMsg->DateTime(timeBuffer) );
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "LineNumber=%u\n", pMsg->m_pMessageData->LineNumber);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "ComponentStringOffset=%u\n", pMsg->m_pMessageData->ComponentStringOffset);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "FileNameStringOffset=%u\n", pMsg->m_pMessageData->FileNameStringOffset);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "MessageStringOffset=%u\n", pMsg->m_pMessageData->MessageStringOffset);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "ArgumentCount=%u\n", pMsg->m_pMessageData->ArgumentCount);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_UInt4 *pArgumentOffsetArray = (SAPDB_UInt4 *)pMsg->m_pMessageData->VariablePart;
            SAPDB_UInt4 iArg;

            for ( iArg = 0; iArg < pMsg->m_pMessageData->ArgumentCount; ++iArg )
            {
                SAPDB_sprintf(diagLine, sizeof(diagLine), "Arg %d TagOffset=%u\n", iArg, pArgumentOffsetArray[2*iArg]);
                resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
                if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

                SAPDB_sprintf(diagLine, sizeof(diagLine), "Arg %d ValueOffset=%u\n", iArg, pArgumentOffsetArray[2*iArg+1]);
                resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
                if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }
            }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "Component=%s\n", pMsg->Component());
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "FileName=%s\n", pMsg->FileName());
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            SAPDB_sprintf(diagLine, sizeof(diagLine), "Message=%s\n", pMsg->Message());
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

            for ( iArg = 0; iArg < pMsg->m_pMessageData->ArgumentCount; ++iArg )
            {
                SAPDB_sprintf(diagLine, sizeof(diagLine), "Arg %d %s=\"%s\"\n",
                    iArg,
                    pMsg->m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg],
                    pMsg->m_pMessageData->VariablePart + pArgumentOffsetArray[2*iArg+1]);
                resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
                if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }
            }
        }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_pRegistrySlot=@%p\n", pMsg->m_pRegistrySlot);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_NumOfMessages=%u\n", pMsg->m_NumOfMessages);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_NumOfSubMessages=%u\n", pMsg->m_NumOfSubMessages);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_NumOfDetails=%u\n", pMsg->m_NumOfDetails);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_ObjectRefCnt=%u\n", pMsg->m_ObjectRefCnt);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_pNextMessage=@%p\n", pMsg->m_pNextMessage);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_OutputSequenceNumber=%qu\n", pMsg->m_OutputSequenceNumber);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf ( diagLine, sizeof(diagLine), "m_OutputDateTime=" MSG_LIST_LONG_TIME_HEADER_FORMAT_STR "\n",
                        m_OutputDateTime.Year,
                        m_OutputDateTime.Month,
                        m_OutputDateTime.Day,
                        m_OutputDateTime.Hour,
                        m_OutputDateTime.Minute,
                        m_OutputDateTime.Second,
                        m_OutputDateTime.Milliseconds );
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_OutputProcessId=%u\n", pMsg->m_OutputProcessId);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_OutputThreadId=%u\n", pMsg->m_OutputThreadId);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_OutputTaskId=%u\n", pMsg->m_OutputTaskId);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_OutputSchedulerId=%u\n", pMsg->m_OutputSchedulerId);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        SAPDB_sprintf(diagLine, sizeof(diagLine), "m_DumpTruncated=%u\n", pMsg->m_DumpTruncated);
        resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
        if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }

        if ( !m_pMessageData )
        {
            SAPDB_sprintf(diagLine, sizeof(diagLine), "MessageNo=%u (empty)\n", iMsg);
            resultLen = SAPDB_strlen(diagLine); neededSize += resultLen;
            if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; SAPDB_memcpy(ptr, diagLine, resultLen); ptr += resultLen; }
            break;
        }

    }

    ++neededSize;
    if ( sizeLeft > 0 )
    {
        *ptr = 0; // zero termination if possible
        --sizeLeft;
    }

    if ( neededSize > bufferSize )
    {
        return (SAPDB_Char const *)"buffer space exhausted";
    }

    return buffer;
}

//----------------------------------------------------------------

SAPDB_Bool Msg_List::CalcNeededSizeInBytes ( SAPDB_UInt4 &neededSizeInBytes,
                                             SAPDB_UInt4 &minimumSizeInBytes,
                                             Msg_List    &errList ) const
{
    minimumSizeInBytes = CalcSizeOfValue( m_OutputSequenceNumber ) 
                       + CalcSizeOfValue( m_NumOfMessages );

    SAPDB_UInt4 version = MSG_LIST_VERSION;
    neededSizeInBytes = minimumSizeInBytes
                      + CalcSizeOfValue( version );

    Msg_List const *pNext;
    for ( pNext = this; pNext->m_pMessageData; pNext = pNext->NextMessage() )
    {
        SAPDB_UInt4 singleMsgNeededSizeInBytes = 0;
        MessageData const *pData = pNext->m_pMessageData;
        SAPDB_Char const *varPart = pData->VariablePart;
        SAPDB_UInt4       maxOffset = pData->DataLen - (varPart - (SAPDB_Char const *)pData);

        singleMsgNeededSizeInBytes += CalcSizeOfValue( pData->ID );
        singleMsgNeededSizeInBytes += CalcSizeOfDateTime( pData->DateTime );
        singleMsgNeededSizeInBytes += CalcSizeOfValue( pData->Type                  );
        singleMsgNeededSizeInBytes += CalcSizeOfValue( pData->LineNumber            );

        singleMsgNeededSizeInBytes += CalcSizeOfString( varPart, pData->ComponentStringOffset, maxOffset );
        singleMsgNeededSizeInBytes += CalcSizeOfString( varPart, pData->FileNameStringOffset,  maxOffset );
        singleMsgNeededSizeInBytes += CalcSizeOfString( varPart, pData->MessageStringOffset,   maxOffset );
        singleMsgNeededSizeInBytes += CalcSizeOfValue( pData->ArgumentCount         );

        SAPDB_UInt4 argumentCount = pData->ArgumentCount;
        SAPDB_UInt4 const *pArgOffsets = (SAPDB_UInt4 const *)varPart;
        if ( (argumentCount * 2 * sizeof(SAPDB_UInt4)) < maxOffset )
        {
            for ( SAPDB_UInt4 iArg = 0; iArg < argumentCount; iArg++ )
            {
                singleMsgNeededSizeInBytes += CalcSizeOfString( varPart, pArgOffsets[2*iArg],   maxOffset );
                singleMsgNeededSizeInBytes += CalcSizeOfString( varPart, pArgOffsets[2*iArg+1], maxOffset );
            }
        }

        // allows size check for this message
        singleMsgNeededSizeInBytes += CalcSizeOfValue( singleMsgNeededSizeInBytes );

        neededSizeInBytes += singleMsgNeededSizeInBytes;
    }

    // Fix number of submessages to allow sub message list parsing after load
    for ( pNext = this; pNext->m_pMessageData; pNext = pNext->NextMessage() )
    {
        neededSizeInBytes += CalcSizeOfValue(pNext->NumOfMessages());
    }

    // Output identification is a nice to have feature, so load as least important...
    neededSizeInBytes += CalcSizeOfDateTime( m_OutputDateTime );
    neededSizeInBytes += CalcSizeOfValue( m_OutputProcessId );
    neededSizeInBytes += CalcSizeOfValue( m_OutputThreadId );
    neededSizeInBytes += CalcSizeOfValue( m_OutputTaskId );
    neededSizeInBytes += CalcSizeOfValue( m_OutputSchedulerId );

    // last entry is total dump size
    neededSizeInBytes += CalcSizeOfValue(neededSizeInBytes);

    return true;
}

//----------------------------------------------------------------

SAPDB_Bool Msg_List::StoreSingleMessage ( Msg_List const    *pMsg
                                        , SAPDB_UInt4       &bytesLeft
                                        , SAPDB_Byte *      &ptr
                                        , SAPDB_UInt4       &singleMsgSavedBytes
                                        , SAPDB_Bool  const  allowTruncate
                                        , Msg_List &         errList ) const
{
    MessageData const *pData = pMsg->m_pMessageData;
    SAPDB_Char const *varPart = pData->VariablePart;
    SAPDB_UInt4       maxOffset = pData->DataLen - (varPart - (SAPDB_Char const *)pData);

    if ( !StoreOptionalValue( "ID", pData->ID
                            , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalDateTime( "DateTime", pData->DateTime
                               , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalValue( "Type", pData->Type
                            , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalValue( "LineNumber", pData->LineNumber
                            , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }

    if ( !StoreOptionalString( "Component", varPart, pData->ComponentStringOffset, maxOffset
                             , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalString( "FileName", varPart, pData->FileNameStringOffset, maxOffset
                             , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalString( "Message", varPart, pData->MessageStringOffset, maxOffset
                             , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalValue( "LineNumber", pData->ArgumentCount
                            , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }

    SAPDB_UInt4 argumentCount = pData->ArgumentCount;
    SAPDB_UInt4 const *pArgOffsets = (SAPDB_UInt4 const *)varPart;
    if ( (argumentCount * 2 * sizeof(SAPDB_UInt4)) < maxOffset )
    {
        for ( SAPDB_UInt4 iArg = 0; iArg < argumentCount; iArg++ )
        {
            if ( !StoreOptionalString( "MessageTag", varPart, pArgOffsets[2*iArg], maxOffset
                                     , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
            if ( !StoreOptionalString( "MessageValue", varPart, pArgOffsets[2*iArg+1], maxOffset
                                     , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
        }
    }

    // allows size check for this message
    if ( !StoreOptionalValue( "SingleMessageSize", singleMsgSavedBytes
                            , bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList) ) { return false; }
    return true;
}

//--------------------------------------------------------------------------------------------

SAPDB_Bool Msg_List::Store ( SAPDB_UInt4 const  bufferSizeInBytes,
                             SAPDB_Bool  const  allowTruncate,
                             void * const       pBuffer,
                             SAPDB_UInt4       &savedBytes,
                             Msg_List &         errList ) const
{
    SAPDB_UInt4 bytesLeft = bufferSizeInBytes;
    SAPDB_Byte *ptr = (SAPDB_Byte *)pBuffer;

    savedBytes = 0;

    if ( !StoreNeededValue( "OutputSequenceNumber", m_OutputSequenceNumber
                          , bytesLeft, ptr, savedBytes, errList ) ) { return false; }
    if ( !StoreNeededValue( "NumberOfMessages", m_NumOfMessages
                          , bytesLeft, ptr, savedBytes, errList ) ) { return false; }

    SAPDB_UInt4 version = MSG_LIST_VERSION;
    if ( !StoreOptionalValue( "VERSION", version
                            , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }

    SAPDB_UInt4 iMsg = 1;
    for ( Msg_List const *pMsg = this; pMsg && pMsg->m_pMessageData; pMsg = pMsg->NextMessage(), ++iMsg )
    {
        SAPDB_UInt4 singleMsgSavedBytes = 0;

        if ( !StoreSingleMessage ( pMsg, bytesLeft, ptr, singleMsgSavedBytes, allowTruncate, errList ) )
        {
            savedBytes += singleMsgSavedBytes;
            SAPDB_ToStringClass number(iMsg);
            errList.PushMessage(Msg_List(Error, SDBMSG_MESSAGES_STORE_CONTAINED_MESSAGE
                          , Msg_SimpleArg("INDEX", (SAPDB_UTF8 const *)(SAPDB_Char const *)number) ) );
            return false;
        }
        savedBytes += singleMsgSavedBytes;
    }

    Msg_List const *pNext;
    // Fix number of submessages to allow sub message list parsing after load
    for ( pNext = this->NextMessage(); pNext && pNext->m_pMessageData; pNext = pNext->NextMessage() )
    {
        if ( !StoreOptionalValue( "NumberOfMessages", pNext->NumOfMessages()
                                , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }
    }

    // Output identification is a nice to have feature, so load as least important...
    if ( !StoreOptionalDateTime( "OutputDateTime", m_OutputDateTime
                               , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalValue( "ProcessId", m_OutputProcessId
                            , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalValue( "ThreadId", m_OutputThreadId
                            , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalValue( "TaskId", m_OutputTaskId
                            , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }
    if ( !StoreOptionalValue( "SchedulerId", m_OutputSchedulerId
                            , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }

    // Fix number of details per messages to allow detail list parsing after load
    for ( pNext = this; pNext && pNext->m_pMessageData; pNext = pNext->NextMessage() )
    {
        if ( !StoreOptionalValue( "NumberOfSubDetails", pNext->NumOfDetails()
                                , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }
        if ( !StoreOptionalValue( "NumberOfSubMessages", pNext->NumOfSubMessages()
                                , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }
    }

    // last entry is total dump size
    if ( !StoreOptionalValue( "TotalDumpSize", savedBytes
                            , bytesLeft, ptr, savedBytes, allowTruncate, errList) ) { return false; }

    return true;
}

//---------------------------------------------------------------------------------

SAPDB_Bool Msg_List::Load( SAPDB_UInt4 const  bufferSizeInBytes,
                           void * const       pBuffer,
                           SAPDB_Bool        &wasTruncated,
                           Msg_List &         errList )
{
    if ( 0 != m_pMessageData )
    {
        DoClear();  // if needed remove old content first
    }

    Msg_List **ppNextMsg = 0;

    SAPDB_UInt4 bytesLeft = bufferSizeInBytes;
    SAPDB_UInt4 bytesNeeded;
    SAPDB_Byte const *ptr = (SAPDB_Byte const *)pBuffer;

    m_DumpTruncated = 1;
    wasTruncated = true; // mostly feared, but not really critical...

    // the only really needed value are the output sequence number and the number of messages which always must be first
    if ( !LoadValue(m_OutputSequenceNumber, bytesLeft, ptr, bytesNeeded) )
    {
        SAPDB_ToStringClass number(bytesNeeded);
        SAPDB_ToStringClass bufferSize(bufferSizeInBytes);
        SAPDB_ToStringClass bufferSizeLeft(bytesLeft);
        errList = Msg_List( Error
                          , SDBMSG_MESSAGES_LOAD_VALUE
                          , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)"OutputSequenceNumber")
                          , Msg_SimpleArg("BYTES_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)bufferSize )
                          , Msg_SimpleArg("BYTES_FREE", (SAPDB_UTF8 const *)(SAPDB_Char const *)bufferSizeLeft )
                          , Msg_SimpleArg("BYTES_NEEDED", (SAPDB_UTF8 const *)(SAPDB_Char const *)number ) );
        return false;
    }
    bytesLeft -= bytesNeeded; ptr += bytesNeeded;

    SAPDB_UInt4 numberOfContainedMessages;
    if ( !LoadValue(numberOfContainedMessages, bytesLeft, ptr, bytesNeeded) )
    {
        SAPDB_ToStringClass number(bytesNeeded);
        SAPDB_ToStringClass bufferSize(bufferSizeInBytes);
        SAPDB_ToStringClass bufferSizeLeft(bytesLeft);
        errList = Msg_List( Error
                          , SDBMSG_MESSAGES_LOAD_VALUE
                          , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)"NumberOfMessages")
                          , Msg_SimpleArg("BYTES_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)bufferSize )
                          , Msg_SimpleArg("BYTES_FREE", (SAPDB_UTF8 const *)(SAPDB_Char const *)bufferSizeLeft )
                          , Msg_SimpleArg("BYTES_NEEDED", (SAPDB_UTF8 const *)(SAPDB_Char const *)number ) );
        return false;
    }
    bytesLeft -= bytesNeeded; ptr += bytesNeeded;

    SAPDB_UInt4 version;
    if ( !LoadExpectedValue(version, bytesLeft, ptr) ) { return true; }
    if ( version != MSG_LIST_VERSION )
    {
        return true; // Ok accepted, but only the sequence number... Version differs...
    }

    for ( SAPDB_UInt4 iMsg = 0; iMsg < numberOfContainedMessages; iMsg++ )
    {
        MessageData tmpData;
        SAPDB_Byte const *startOfSingleMessage = ptr;

        if ( !LoadExpectedValue(tmpData.ID,                   bytesLeft, ptr) ) { return true; }
        if ( !LoadExpectedDateTime(tmpData.DateTime,          bytesLeft, ptr) ) { return true; }
        if ( !LoadExpectedValue(tmpData.Type,                 bytesLeft, ptr) ) { return true; }
        if ( !LoadExpectedValue(tmpData.LineNumber,           bytesLeft, ptr) ) { return true; }

        SAPDB_Char const *component;
        if ( !LoadSkipToEndOfString(component,                bytesLeft, ptr) ) { return true; }
        SAPDB_Char const *filename;
        if ( !LoadSkipToEndOfString(filename,                 bytesLeft, ptr) ) { return true; }
        SAPDB_Char const *message;
        if ( !LoadSkipToEndOfString(message,                  bytesLeft, ptr) ) { return true; }

        Msg_SimpleArg tmpArgArray[MSG_LIST_ARG_MAX];
        Msg_IOptArg const *argArray[MSG_LIST_ARG_MAX];

        SAPDB_UInt4 argumentCount;
        if ( !LoadExpectedValue(tmpData.ArgumentCount,        bytesLeft, ptr) ) { return true; }
        argumentCount = tmpData.ArgumentCount;
        
        for ( SAPDB_UInt4 iArg = 0; iArg < argumentCount; iArg++ )
        {
            if ( !LoadSkipToEndOfString(tmpArgArray[iArg].TagRef(), bytesLeft, ptr) ) { return true; }
            if ( !LoadSkipToEndOfUTF8String(tmpArgArray[iArg].ValueRef(), bytesLeft, ptr) ) { return true; }
            argArray[iArg] = tmpArgArray[iArg].GetArg();
        }

        // size check for this message
        SAPDB_UInt4 foundSizeOfSingleMessage = (ptr - startOfSingleMessage);
        SAPDB_UInt4 expectedSizeOfSingleMessage;
        if ( !LoadExpectedValue(expectedSizeOfSingleMessage,  bytesLeft, ptr) ) { return true; }
        if ( foundSizeOfSingleMessage != expectedSizeOfSingleMessage )          { return true; }

        Msg_List msgToAdd( tmpData.Type,
                           tmpData.ID,
                           component,
                           filename,
                           tmpData.LineNumber,
                           tmpData.DateTime,
                           message,
                           argumentCount,
                           argArray );

        if ( 0 == ppNextMsg )
        {
            CopySelf(true, msgToAdd);
            ppNextMsg = &this->m_pNextMessage;
        }
        else
        {
            this->m_NumOfMessages = (iMsg+1);
            *ppNextMsg = new (RTE_IInterface::Instance().MsgListAllocator()) Msg_List(msgToAdd);
            if ( !*ppNextMsg )
            {
                return true;
            }
            ppNextMsg = &(*ppNextMsg)->m_pNextMessage;
        }
    }

    Msg_List const *pNext;
    // Fix number of submessages to allow sub message list parsing after load
    for ( pNext = this->NextMessage(); pNext; pNext = pNext->NextMessage() )
    {
        SAPDB_UInt4 numberOfMessages;
        if ( !LoadExpectedValue(numberOfMessages, bytesLeft, ptr) ) { return true; }

        // we know it is no danger to cast away constness, since we just created it dynamically
        (const_cast<Msg_List *>(pNext))->m_NumOfMessages = numberOfMessages;
    }

    // Output identification is a nice to have feature, so load as least important...
    if ( !LoadExpectedDateTime(m_OutputDateTime,          bytesLeft, ptr) ) { return true; }
    if ( !LoadExpectedValue(m_OutputProcessId,            bytesLeft, ptr) ) { return true; }
    if ( !LoadExpectedValue(m_OutputThreadId,             bytesLeft, ptr) ) { return true; }
    if ( !LoadExpectedValue(m_OutputTaskId,               bytesLeft, ptr) ) { return true; }
    if ( !LoadExpectedValue(m_OutputSchedulerId,          bytesLeft, ptr) ) { return true; }

    // Fix number of detail messages to allow detail list parsing after load
    for ( pNext = this; pNext; pNext = pNext->NextMessage() )
    {
        SAPDB_UInt4 numberOfDetails;
        if ( !LoadExpectedValue(numberOfDetails, bytesLeft, ptr) ) { return true; }
        // we know it is no danger to cast away constness, since we just created it dynamically
        (const_cast<Msg_List *>(pNext))->m_NumOfDetails = numberOfDetails;

        SAPDB_UInt4 numberOfSubMessages;
        if ( !LoadExpectedValue(numberOfSubMessages, bytesLeft, ptr) ) { return true; }
        // we know it is no danger to cast away constness, since we just created it dynamically
        (const_cast<Msg_List *>(pNext))->m_NumOfSubMessages = numberOfSubMessages;
    }

    SAPDB_UInt4 foundTotalDumpSize = (ptr - (SAPDB_Byte const *)pBuffer);

    SAPDB_UInt4 expectedTotalDumpSize;
    if ( !LoadExpectedValue(expectedTotalDumpSize, bytesLeft, ptr) ) { return true; }

    if ( foundTotalDumpSize != expectedTotalDumpSize ) { return true; }

    // finally we know it is complete...
    m_DumpTruncated = 0;
    wasTruncated = false;

    return true;
}

/*---------------------------------------------------------------------------*/

#ifdef SAPDBERR_MESSAGELIST_STILL_SUPPORTED

inline static SAPDB_UInt4 BuildMessageString  ( SAPDB_Char  const * const  Message,
                                                SAPDB_Char  const * const  Args[],
                                                SAPDB_UInt4 const          NumOfArgs,
                                                SAPDB_Char  * const        MessageString )
{
  SAPDB_Char const *  pMessage       = Message;
  SAPDB_Char *        pMessageString = MessageString;
  SAPDB_UInt4         ArgCnt         = 0;
  SAPDB_Char const *  pArg;

  do
  {
    // --- format specifier '%' found?
    if ( *pMessage == '%' )
    {
      // --- only %s or %S allowed!
      if ((*(pMessage + 1) == 's' ) || ( *(pMessage + 1) == 'S' ))
      {
        pMessage++;
        
        if ( ArgCnt < NumOfArgs )
        {
          pArg = Args[ArgCnt++];
          
          while ( *pArg != '\0' )
            *(pMessageString++) = *(pArg++);
        }
      }
      else
        *(pMessageString++) = *pMessage;
    }
    else
      *(pMessageString++) = *pMessage;
  } 
  while ( *(pMessage++) != '\0' );  // --- loop until end of message


  // --- There are additional arguments but no more %s or %S format specifier.
  if ( ArgCnt < NumOfArgs )
  {
    pMessageString -= 1;
    pArg            = ADDITIONAL_ARGS_STR;

    while ( *pArg != '\0' )
      *(pMessageString++) = *(pArg++);
    
    do
    {
      pArg = Args[ArgCnt++];
      
      while ( *pArg != '\0' )
        *(pMessageString++) = *(pArg++);

      if ( ArgCnt < NumOfArgs )
        *(pMessageString++) = ',';
      else
        *(pMessageString++) = '\0';
    } 
    while ( ArgCnt < NumOfArgs );
  }
	
  return SAPDB_strlen(MessageString);
}

//--------------

Msg_List::Msg_List( SAPDB_Char  const * const  Component,
                    SAPDB_Char  const * const  FileName,
                    SAPDB_UInt4 const          LineNumber,
                    MessageType const          Type,
                    MessageID   const          ID,
                    SAPDB_Char  const * const  Message, 
                    SAPDB_UInt4 const          NumOfArgs,
                    SAPDB_Char  const * const  Arg0,
                    SAPDB_Char  const * const  Arg1,
                    SAPDB_Char  const * const  Arg2,
                    SAPDB_Char  const * const  Arg3,
                    SAPDB_Char  const * const  Arg4,
                    SAPDB_Char  const * const  Arg5,
                    SAPDB_Char  const * const  Arg6,
                    SAPDB_Char  const * const  Arg7,
                    SAPDB_Char  const * const  Arg8,
                    SAPDB_Char  const * const  Arg9 )
    : m_pRegistrySlot(0)
    , m_NumOfMessages(0)
    , m_NumOfSubMessages(0)
    , m_NumOfDetails(0)
    , m_ObjectRefCnt(0)
    , m_pMessageData(0)
    , m_pNextMessage(0)
    , m_OutputSequenceNumber(0)
    , m_OutputDateTime(EmptyDateTimeValue)
    , m_OutputProcessId(0)
    , m_OutputThreadId(0)
    , m_OutputTaskId(0)
    , m_OutputSchedulerId(0)
    , m_DumpTruncated(0)
{
    SAPDB_UInt4 ArgCnt = 0;
    SAPDB_Char const * Args[MSG_LIST_SINGLE_ARGS_MAX];
    SAPDB_UInt4 MessageStringSize = SAPDB_strlen( Message ) + SAPDB_strlen(ADDITIONAL_ARGS_STR) + 1;

    if ( Arg0 ) { Args[ArgCnt] = Arg0; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg1 ) { Args[ArgCnt] = Arg1; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg2 ) { Args[ArgCnt] = Arg2; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg3 ) { Args[ArgCnt] = Arg3; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg4 ) { Args[ArgCnt] = Arg4; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg5 ) { Args[ArgCnt] = Arg5; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg6 ) { Args[ArgCnt] = Arg6; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg7 ) { Args[ArgCnt] = Arg7; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg8 ) { Args[ArgCnt] = Arg8; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg9 ) { Args[ArgCnt] = Arg9; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }

    SAPDB_Char * pMessageString = (SAPDB_Char*)alloca(MessageStringSize);

    if ( 0 != pMessageString )
    {
        SAPDB_UInt4 buildMessageStringSize =
            BuildMessageString( Message, Args, ArgCnt, pMessageString );
    }

    FillMessageList( Type
                   , Component
                   , FileName
                   , LineNumber
                   , ID
                   , (pMessageString != 0 ? pMessageString : Message)
                   , 0
                   , 0 );
    TraceMessageCopy();
}

//-----------

Msg_List::Msg_List( SAPDB_Char  const * const  Component,
                    SAPDB_Char  const * const  FileName,
                    SAPDB_UInt4 const          LineNumber,
                    SAPDB_UInt4 const          SystemRC,
                    MessageType const          Type,
                    MessageID   const          ID,
                    SAPDB_Char  const * const  Message,
                    SAPDB_UInt4 const          NumOfArgs,
                    SAPDB_Char  const * const  Arg0,
                    SAPDB_Char  const * const  Arg1,
                    SAPDB_Char  const * const  Arg2,
                    SAPDB_Char  const * const  Arg3,
                    SAPDB_Char  const * const  Arg4,
                    SAPDB_Char  const * const  Arg5,
                    SAPDB_Char  const * const  Arg6,
                    SAPDB_Char  const * const  Arg7,
                    SAPDB_Char  const * const  Arg8,
                    SAPDB_Char  const * const  Arg9 )
    : m_pRegistrySlot(0)
    , m_NumOfMessages(0)
    , m_NumOfSubMessages(0)
    , m_NumOfDetails(0)
    , m_ObjectRefCnt(0)
    , m_pMessageData(0)
    , m_pNextMessage(0)
    , m_OutputSequenceNumber(0)
    , m_OutputDateTime(EmptyDateTimeValue)
    , m_OutputProcessId(0)
    , m_OutputThreadId(0)
    , m_OutputTaskId(0)
    , m_OutputSchedulerId(0)
    , m_DumpTruncated(0)
{
    SAPDB_UInt4 ArgCnt = 0;
    SAPDB_Char const * Args[MSG_LIST_SINGLE_ARGS_MAX];
    SAPDB_UInt4 MessageStringSize = SAPDB_strlen( Message ) + SAPDB_strlen(ADDITIONAL_ARGS_STR) + 1;

    if ( Arg0 ) { Args[ArgCnt] = Arg0; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg1 ) { Args[ArgCnt] = Arg1; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg2 ) { Args[ArgCnt] = Arg2; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg3 ) { Args[ArgCnt] = Arg3; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg4 ) { Args[ArgCnt] = Arg4; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg5 ) { Args[ArgCnt] = Arg5; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg6 ) { Args[ArgCnt] = Arg6; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg7 ) { Args[ArgCnt] = Arg7; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg8 ) { Args[ArgCnt] = Arg8; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( Arg9 ) { Args[ArgCnt] = Arg9; MessageStringSize += SAPDB_strlen( Args[ArgCnt++] ); }
    if ( SystemRC != 0 )
    {
        MessageStringSize += 32;
    }

    SAPDB_Char * pMessageString = (SAPDB_Char*)alloca(MessageStringSize);

    if ( 0 != pMessageString )
    {
        SAPDB_UInt4 buildMessageStringSize =
            BuildMessageString( Message, Args, ArgCnt, pMessageString );
        if ( SystemRC != 0 )
        {
            SAPDB_sprintf(pMessageString + buildMessageStringSize,
                MessageStringSize - buildMessageStringSize, "[%d]", SystemRC);
        }
    }

    FillMessageList( Type
                   , Component
                   , FileName
                   , LineNumber
                   , ID
                   , (pMessageString != 0 ? pMessageString : Message)
                   , 0
                   , 0 );
    TraceMessageCopy();
}

#endif    

/*---------------------------------------------------------------------------*/

void Msg_List::SetCallerIdentification( SAPDB_UInt4 const outputThreadId
                                      , SAPDB_UInt4 const outputTaskId
                                      , SAPDB_UInt4 const outputSchedulerId)
{
    m_OutputProcessId = RTE_IInterface::Instance().ProcessID();

    if ( 0 != outputThreadId )
    {
        m_OutputThreadId  = outputThreadId;
    }
    if ( 0 != outputTaskId )
    {
        m_OutputTaskId    = outputTaskId;
    }
    if ( 0 != outputSchedulerId )
    {
        m_OutputSchedulerId = outputSchedulerId;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool Msg_List::SetOutputIdentification( SAPDB_UInt8 const outputSequenceNumber
                                            , SAPDB_UInt4 const outputThreadId
                                            , SAPDB_UInt4 const outputTaskId
                                            , SAPDB_UInt4 const outputSchedulerId)
{
    SAPDB_Bool hasNewIdentification;
    SAPDB_UInt8 oldValue;
    hasNewIdentification = RTE_IInterface::Instance().AtomicCompareAndExchange( m_OutputSequenceNumber
                                                    , (SAPDB_UInt8)0
                                                    , outputSequenceNumber
                                                    , oldValue);
    if ( hasNewIdentification )
    {
        RTE_IInterface::Instance().GetLocalDateTime( m_OutputDateTime );
        SetCallerIdentification(outputThreadId, outputTaskId, outputSchedulerId);
    }
    return hasNewIdentification;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt8 const Msg_List::GetOutputSequenceNumber() const
{
    // atomic read needs a non const reference... so we const_cast here...
    SAPDB_UInt8 *pNumber = const_cast<SAPDB_UInt8 *>(&m_OutputSequenceNumber);
    return RTE_IInterface::Instance().AtomicRead(*pNumber);
}

/*---------------------------------------------------------------------------*/
SAPDB_UTF8 const * const  Msg_List::ArgValue(SAPDB_Char const * argumentName) const
{
    for (SAPDB_UInt4 argumentNo = 0; argumentNo < m_pMessageData->ArgumentCount; ++argumentNo) {
        if (ArgTag(argumentNo) != 0 && SAPDB_strcmp(ArgTag(argumentNo), argumentName) == 0) {
            return ArgValue(argumentNo);
        } // end if
    } // end for
    return 0;
}

/*---------------------------------------------------------------------------*/

#ifndef LIBOMS
Msg_Registry::SlotArray::SlotArray()
: magic(MSG_REGISTRY_SLOT_ARRAY_MAGIC)
, pNextArray(0)
{
    for ( SAPDB_UInt4 iCluster = 0; iCluster < MSG_REGISTRY_SLOT_ARRAY_CLUSTER_COUNT; ++iCluster)
    {
        freeCount[iCluster] = MSG_REGISTRY_SLOTS_PER_CLUSTER;
    }

    for ( SAPDB_UInt4 iSlot = 0; iSlot < MSG_REGISTRY_SLOT_ARRAY_SIZE; ++iSlot)
    {
        slots[iSlot].Initialize(iSlot);
    }

    pThis = this;
}

    /*!
      @brief deregister and wait for current iterator usage count to become zero
     */
void Msg_RegistrySlot::WaitForZeroUsageCountAndDeregister()
{
    m_pMsg = (Msg_List *)MSG_LIST_IN_SLOT_ILLEGAL;
    RTE_IInterface::Instance().MemoryBarrier();  // enforce global visibility of illegal pointer
    do
    {
        RTE_IInterface::Instance().ReadMemoryBarrier();
        if ( m_UsageCount == 0 )
        {
            break;
        }
        RTE_IInterface::Instance().GiveUpTimeSlice();
    } while ( m_UsageCount > 0 );

    m_pMsg = 0;
    RTE_IInterface::Instance().WriteMemoryBarrier();

    // pointer to SlotArray is stored just behind complete slot array
    Msg_Registry::SlotArray *pArrayToUse = *(Msg_Registry::SlotArray **)(this + (MSG_REGISTRY_SLOT_ARRAY_SIZE - m_SlotIndex));

    RTE_IInterface::Instance().AtomicModify(pArrayToUse->freeCount[m_SlotIndex/MSG_REGISTRY_SLOTS_PER_CLUSTER], 1);
}

    /*!
      @brief increment current iterator usage count
     */
void Msg_RegistrySlot::IncrementUsageCount()
{
    RTE_IInterface::Instance().AtomicModify(m_UsageCount, 1);
}

    /*!
      @brief decrement current iterator usage count
     */
void Msg_RegistrySlot::DecrementUsageCount()
{
    RTE_IInterface::Instance().AtomicModify(m_UsageCount, -1);
}

/*---------------------------------------------------------------------------*/

Msg_RegistryIterator::Msg_RegistryIterator(const Msg_RegistryIterator &toCopy)
        : m_NextPage(toCopy.m_NextPage)
        , m_NextSlot(toCopy.m_NextSlot)
{
    if ( m_NextPage >= 0 )
    {
        if ( !LockFirst() )
        {
            m_NextPage = -1;
            m_NextSlot = 0;
        }
    }
}

/*---------------------------------------------------------------------------*/

Msg_RegistryIterator& Msg_RegistryIterator::operator=(const Msg_RegistryIterator &toCopy)
{
    if ( this != &toCopy )
    {
        m_NextPage = toCopy.m_NextPage;
        m_NextSlot = toCopy.m_NextSlot;
        if ( m_NextPage >= 0 )
        {
            if ( !LockFirst() )
            {
                m_NextPage = -1;
                m_NextSlot = 0;
            }
        }
    }
    return *this;
}

/*---------------------------------------------------------------------------*/

Msg_RegistryIterator::~Msg_RegistryIterator()
{
    ReleaseLast();
}

/*---------------------------------------------------------------------------*/

void *Msg_RegistryIterator::LockFirst()
{
    Msg_Registry::SlotArray *pArray = &Msg_Registry::Instance().m_FirstPage;

    for ( SAPDB_Int4 iPage = 0; iPage < m_NextPage; ++iPage )
    {
        pArray = pArray->pNextArray;
        if ( !pArray )
        {
            break;
        }
    }

    while ( pArray )
    {
        if ( m_NextSlot >= MSG_REGISTRY_SLOT_ARRAY_SIZE )
        {
            ++m_NextPage;
            pArray = pArray->pNextArray;
            m_NextSlot = 0;
        }
        else
        {
            if ( !pArray->slots[m_NextSlot].m_pMsg 
              || MSG_LIST_IN_SLOT_ILLEGAL == pArray->slots[m_NextSlot].m_pMsg )
            {
                m_NextSlot++;
            }
            else
            {
                pArray->slots[m_NextSlot].IncrementUsageCount();
                RTE_IInterface::Instance().ReadMemoryBarrier();
                if ( pArray->slots[m_NextSlot].m_pMsg
                  && MSG_LIST_IN_SLOT_ILLEGAL != pArray->slots[m_NextSlot].m_pMsg )
                {
                    return pArray;
                }
                pArray->slots[m_NextSlot].DecrementUsageCount();
                m_NextSlot++;
            }
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

void *Msg_RegistryIterator::LockNext(void *pGiven, Msg_List const *&pMsg)
{
    Msg_Registry::SlotArray *pArray = (Msg_Registry::SlotArray *)pGiven;
    ++m_NextSlot;

    while ( pArray )
    {
        if ( m_NextSlot >= MSG_REGISTRY_SLOT_ARRAY_SIZE )
        {
            ++m_NextPage;
            pArray = pArray->pNextArray;
            m_NextSlot = 0;
        }
        else
        {
            pMsg = pArray->slots[m_NextSlot].m_pMsg;
            if ( !pMsg 
              || MSG_LIST_IN_SLOT_ILLEGAL == pMsg )
            {
                m_NextSlot++;
            }
            else
            {
                pArray->slots[m_NextSlot].IncrementUsageCount();
                RTE_IInterface::Instance().ReadMemoryBarrier();
                pMsg = pArray->slots[m_NextSlot].m_pMsg;
                if ( pMsg 
                  && MSG_LIST_IN_SLOT_ILLEGAL != pMsg )
                {
                    return pArray;
                }
                pArray->slots[m_NextSlot].DecrementUsageCount();
                m_NextSlot++;
            }
        }
    }

    pMsg = 0;
    return 0;
}

/*---------------------------------------------------------------------------*/

void *Msg_RegistryIterator::ReleaseLast()
{
    if ( m_NextPage >= 0 )
    {
        Msg_Registry::SlotArray *pArray = &Msg_Registry::Instance().m_FirstPage;

        for ( SAPDB_Int4 iPage = 0; iPage < m_NextPage; ++iPage )
        {
            pArray = pArray->pNextArray;
            if ( !pArray )
                break;
        }

        if ( pArray
          && m_NextSlot < MSG_REGISTRY_SLOT_ARRAY_SIZE
          && pArray->slots[m_NextSlot].m_pMsg )
        {
            pArray->slots[m_NextSlot].DecrementUsageCount();
            return pArray;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

Msg_List const * Msg_RegistryIterator::FirstMessage()
{
    Msg_List const * pFirstMsg = 0;
    Msg_Registry::SlotArray *pArray;

    ReleaseLast();

    m_NextPage = 0;
    m_NextSlot = 0;

    pArray = (Msg_Registry::SlotArray *)LockFirst();

    if ( pArray )
    {
        pFirstMsg = pArray->slots[m_NextSlot].m_pMsg;
    }
    else
    {
        m_NextPage = -1;
        m_NextSlot = 0;
    }

    return pFirstMsg;
}

/*---------------------------------------------------------------------------*/

Msg_List const * Msg_RegistryIterator::NextMessage()
{
    Msg_List const * pNextMsg = 0;
    Msg_Registry::SlotArray *pArray = (Msg_Registry::SlotArray *)LockNext(ReleaseLast(), pNextMsg);

    if ( pArray )
    {
        pNextMsg = pArray->slots[m_NextSlot].m_pMsg;
    }

    return pNextMsg;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool Msg_Registry::AppendNewArray(SlotArray *pArrayToUse)
{
    if ( !pArrayToUse->pNextArray )
    {
        SlotArray *pNewNextArray = new ( RTE_IInterface::Instance().MsgListAllocator() ) SlotArray;
        if ( !pNewNextArray )
        {
            return false; //! completely out of memory... 
        }

        SlotArray *pOldNextArray = 0;

        if ( !RTE_IInterface::Instance().AtomicCompareAndExchange( (void **)&pArrayToUse->pNextArray
                                       , pOldNextArray
                                       , pNewNextArray
                                       , (void **)&pOldNextArray ) )
        {
            // a parallel thread with a second allocation was already successfull...
            destroy( pNewNextArray, RTE_IInterface::Instance().MsgListAllocator() );
        }
    }

    return true;
}

/*---------------------------------------------------------------------------*/

Msg_Registry& Msg_Registry::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(Msg_Registry));
    if (0 == m_Instance )
    {
        m_Instance = new(Space) Msg_Registry;
    }
    return *m_Instance;
}

/*---------------------------------------------------------------------------*/

Msg_RegistrySlot const *Msg_Registry::Register(Msg_List const &msg)
{
    Msg_Registry::SlotArray *pArrayToUse = m_pLastPage;
    SAPDB_UInt4 slotToUse = m_LastSlot;
    SAPDB_UInt4 firstSlotToTest = 0;
    SAPDB_Bool newArrayUsed = false;

    if ( pArrayToUse )
    {
        ++slotToUse;

        if ( slotToUse >= MSG_REGISTRY_SLOT_ARRAY_SIZE )
        {
            pArrayToUse = &m_FirstPage;
            slotToUse = 0;
        }
    }
    else
    {
        pArrayToUse = &m_FirstPage;
        newArrayUsed = true;
        slotToUse   = 0;
    }

    firstSlotToTest = slotToUse;

    while ( pArrayToUse != 0 )
    {
        SAPDB_UInt4 clusterToUse = (slotToUse/MSG_REGISTRY_SLOTS_PER_CLUSTER);
        while ( clusterToUse < MSG_REGISTRY_SLOT_ARRAY_CLUSTER_COUNT )
        {
            SAPDB_UInt4 freeCount = pArrayToUse->freeCount[clusterToUse];

            if ( freeCount > 0 )
            {
                do
                {
                    Msg_List const *pFoundMsg; // only needed to read back who occupied the slot, but else ignored
                    if ( RTE_IInterface::Instance().AtomicCompareAndExchange( (void const **)&(pArrayToUse->slots[slotToUse].m_pMsg)
                                                  , (void const *)0
                                                  , &msg
                                                  , (void const **)&pFoundMsg ) )
                    {
                        if ( newArrayUsed )
                        {
                            m_LastSlot = 0;
                            m_pLastPage = pArrayToUse;
                        }
                        m_LastSlot = slotToUse;
                        RTE_IInterface::Instance().AtomicModify(pArrayToUse->freeCount[clusterToUse], -1);

                        // return pointer to use registry slot
                        return &pArrayToUse->slots[slotToUse];
                    }
                } while ( ((++slotToUse)%MSG_REGISTRY_SLOTS_PER_CLUSTER) != 0 );
            }

            ++clusterToUse;
            slotToUse = clusterToUse * MSG_REGISTRY_SLOTS_PER_CLUSTER;
        }

        // parsed complete page
        slotToUse = 0;

        if ( firstSlotToTest != 0 )
        {
            firstSlotToTest = 0; // rescan a second time
            pArrayToUse = &m_FirstPage;
        }
        else
        {
            if ( !AppendNewArray(pArrayToUse) )
            {
                Msg_List emergencyMessage(
                        msg.Type()
                      , SDBMSG_MESSAGES_OUT_OF_MEMORY
                      , Msg_Arg("BYTE_NEEDED", sizeof(SlotArray))
                      , Msg_Arg("ALLOCATOR", RTE_IInterface::Instance().MsgListAllocator().GetIdentifier() )
                                         );
                emergencyMessage.Overrule(msg);
                RTE_IInterface::Instance().DiagMessage(emergencyMessage); // cannot register... try to report...
                return 0; //! completely out of memory... Registration ignored...
            }
            newArrayUsed = true;
            pArrayToUse = pArrayToUse->pNextArray;
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*/

void Msg_Registry::Deregister(Msg_RegistrySlot const *&pConstSlot)
{
    if ( pConstSlot )
    {
        /*
            The deregister function is called by the Msg_List, that does not modify the Msg_RegistrySlot
            by itself. Therefore it contains only a pointer to the 'const Msg_RegistrySlor' which is cast
            to a non const slot here. The '&*' is for the old HP compiler aCC X.03.33.01...
         */
        Msg_RegistrySlot *pSlot = const_cast<Msg_RegistrySlot *>(&*pConstSlot);
        pSlot->WaitForZeroUsageCountAndDeregister();
        pConstSlot = 0;
    }
}

/*---------------------------------------------------------------------------*/

SAPDBMem_IRawAllocator &Msg_Registry::Allocator()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(MsgList_Allocator));
    static MsgList_Allocator *pEmergencyAllocator = 0;
    if (0 == pEmergencyAllocator )
    {
        pEmergencyAllocator = new(Space) MsgList_Allocator;
    }
    return *pEmergencyAllocator;
}

/*!
  @brief do the registry dump
  called from C wrapper
  @return none
 */
static void DoMessageRegistryDump()
{
    Msg_Registry &msgRegistry = Msg_Registry::Instance();
    Msg_RegistryIterator msgIterator = msgRegistry.GetIterator();

    RTE_IInterface::Instance().DiagMessage(Msg_List(Msg_List::Error, SDBMSG_MESSAGES_REGISTRY_DUMP_START));

    Msg_List completeMessageList;
    for ( Msg_List const *pMessage = msgIterator.FirstMessage(); pMessage; pMessage = msgIterator.NextMessage() )
    {
        if ( 0 == pMessage->GetOutputSequenceNumber() )
        {
            RTE_IInterface::Instance().DiagMessage(*pMessage);
        }
    }
}

/*!
  @brief abort the registry dump
  called from  signal handler
  @return none
 */
static void AbortMessageRegistryDump()
{
    RTE_IInterface::Instance().DiagMessage(Msg_List(Msg_List::Error, SDBMSG_MESSAGES_REGISTRY_DUMP_ABORT));
}

/*!
  @brief end the registry dump
  called from  signal handler
  @return none
 */
static void EndOfMessageRegistryDump()
{
    RTE_IInterface::Instance().DiagMessage(Msg_List(Msg_List::Error, SDBMSG_MESSAGES_REGISTRY_DUMP_END));
}

#if !defined (_WIN32)
#include <signal.h>
#include <setjmp.h>
#if defined(OSF1)
extern "C" int _Esigaction(int, const struct sigaction *, struct sigaction *);
#endif

/*UNIX*/static sigjmp_buf emergencyJumpTarget; ///< setjmp target

/*!
  @brief signal handler
  @param Signal [in]
  @return none
 */
/*UNIX*/extern "C" void MessageRegistryDumpSignalHandler(int Signal)
/*UNIX*/{
/*UNIX*/    siglongjmp(emergencyJumpTarget, Signal != 0 ? Signal : 1);
/*UNIX*/}
#endif

/*!
  @brief message registry dump
  @return none
 */
extern "C" void Msg_RegistryDump()
{
#if !defined (_WIN32)    
/*UNIX*/    struct sigaction old_sigsegvForMessageRegistryDump;
/*UNIX*/    struct sigaction old_sigbusForMessageRegistryDump;
/*UNIX*/    sigset_t    old_setForMessageRegistryDump;
/*UNIX*/    if ( 0 == sigsetjmp( emergencyJumpTarget, 1) )
/*UNIX*/    {  
/*UNIX*/        struct sigaction saForMessageRegistryDump;
/*UNIX*/        sigset_t         mask_setForMessageRegistryDump;
/*UNIX*/
/*UNIX*/        saForMessageRegistryDump.sa_handler = MessageRegistryDumpSignalHandler;
/*UNIX*/        saForMessageRegistryDump.sa_flags   = 0;
/*UNIX*/        sigemptyset( &saForMessageRegistryDump.sa_mask );
/*UNIX*/
/*UNIX*/        sigaction(SIGSEGV, &saForMessageRegistryDump, &old_sigsegvForMessageRegistryDump);
/*UNIX*/        sigaction(SIGBUS, &saForMessageRegistryDump, &old_sigbusForMessageRegistryDump);
/*UNIX*/
/*UNIX*/        sigfillset(&mask_setForMessageRegistryDump);
/*UNIX*/        sigdelset(&mask_setForMessageRegistryDump, SIGSEGV);
/*UNIX*/        sigdelset(&mask_setForMessageRegistryDump, SIGBUS);
/*UNIX*/        sigprocmask(SIG_SETMASK, &mask_setForMessageRegistryDump, &old_setForMessageRegistryDump);
#else
/*WIN*/     __try 
/*WIN*/     {
#endif
                DoMessageRegistryDump();
#if !defined (_WIN32)
/*UNIX*/    }
/*UNIX*/    else
/*UNIX*/    {
/*UNIX*/        AbortMessageRegistryDump();
/*UNIX*/    }
/*UNIX*/
/*UNIX*/    sigaction(SIGSEGV, &old_sigsegvForMessageRegistryDump, 0);
/*UNIX*/    sigaction(SIGBUS, &old_sigbusForMessageRegistryDump, 0);
/*UNIX*/    sigprocmask(SIG_SETMASK, &old_setForMessageRegistryDump, (sigset_t*)0);
#else
/*WIN*/     }
/*WIN*/     __except( EXCEPTION_EXECUTE_HANDLER ) 
/*WIN*/     {
/*WIN*/          AbortMessageRegistryDump();
/*WIN*/     }
#endif
            EndOfMessageRegistryDump();
}
#endif // LIBOMS

// static inline functions

//---------------------------------------------------------------

static inline void CopyCString( SAPDB_Char *&ptr
                              , SAPDB_UInt4 &sizeLeft
                              , SAPDB_UInt4 &neededSize
                              , SAPDB_Char const *stringToCopy )
{
    SAPDB_UInt4 stringLen = SAPDB_strlen(stringToCopy);
    neededSize += stringLen;

    if ( stringLen < sizeLeft )
    {
        SAPDB_memcpy(ptr, stringToCopy, stringLen+1);
        sizeLeft -= stringLen;
        ptr += stringLen;
    }
    else
    {
        sizeLeft = 0;
    }
}

//---------------------------------------------------------------

static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_UInt4 argumentNumber)
{
    SAPDB_Char diagLine[1024];
    SAPDB_sprintf(diagLine, sizeof(diagLine), " %s=\"%u\"", argumentTag, argumentNumber );
    CopyCString(ptr, sizeLeft, neededSize, diagLine);
}

//---------------------------------------------------------------

static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_UInt8 argumentNumber)
{
    SAPDB_Char diagLine[1024];
    SAPDB_sprintf(diagLine, sizeof(diagLine), " %s=\"%qu\"", argumentTag, argumentNumber );
    CopyCString(ptr, sizeLeft, neededSize, diagLine);
}

//---------------------------------------------------------------

static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_Char const *argumentString)
{
    CopyCString(ptr, sizeLeft, neededSize, " ");
    CopyCString(ptr, sizeLeft, neededSize, argumentTag);
    CopyCString(ptr, sizeLeft, neededSize, "=\"");
    CopyCString(ptr, sizeLeft, neededSize, argumentString);
    CopyCString(ptr, sizeLeft, neededSize, "\"");
}

//---------------------------------------------------------------

static inline void CopyArgument( SAPDB_Char *&ptr
                               , SAPDB_UInt4 &sizeLeft
                               , SAPDB_UInt4 &neededSize
                               , SAPDB_Char const *argumentTag
                               , SAPDB_UTF8 const *argumentString)
{
    CopyCString(ptr, sizeLeft, neededSize, " ");
    CopyCString(ptr, sizeLeft, neededSize, argumentTag);
    CopyCString(ptr, sizeLeft, neededSize, "=\"");
    CopyXMLString(ptr, sizeLeft, neededSize, argumentString);
    CopyCString(ptr, sizeLeft, neededSize, "\"");
}

//---------------------------------------------------------------

static inline void CopyIntentedArgument( SAPDB_UInt4 indentLevel
                                       , SAPDB_Char *&ptr
                                       , SAPDB_UInt4 &sizeLeft
                                       , SAPDB_UInt4 &neededSize
                                       , SAPDB_Char const *argumentTag
                                       , SAPDB_UInt4 argumentNumber)
{
    SAPDB_Char diagLine[1024];
    SAPDB_sprintf(diagLine, sizeof(diagLine), "%s=\"%u\"", argumentTag, argumentNumber );
    CopyIndentedCString(indentLevel, ptr, sizeLeft, neededSize, diagLine);
}

//---------------------------------------------------------------

static inline void CopyIntentedArgument( SAPDB_UInt4 indentLevel
                                       , SAPDB_Char *&ptr
                                       , SAPDB_UInt4 &sizeLeft
                                       , SAPDB_UInt4 &neededSize
                                       , SAPDB_Char const *argumentTag
                                       , SAPDB_Char const *argumentString)
{
    SAPDB_Char diagLine[4096];
    SAPDB_sprintf(diagLine, sizeof(diagLine), "%s=\"%s\"", argumentTag, argumentString );
    CopyIndentedCString(indentLevel, ptr, sizeLeft, neededSize, diagLine);
}

//---------------------------------------------------------------

static inline void CopyIntentedArgument( SAPDB_UInt4 indentLevel
                                        , SAPDB_Char *&ptr
                                        , SAPDB_UInt4 &sizeLeft
                                        , SAPDB_UInt4 &neededSize
                                        , SAPDB_Char const *argumentTag
                                        , SAPDB_UTF8 const *argumentString)
{
    CopyIndentedCString(indentLevel, ptr, sizeLeft, neededSize, " ");
    CopyCString(ptr, sizeLeft, neededSize, argumentTag);
    CopyCString(ptr, sizeLeft, neededSize, "=\"");
    CopyXMLString(ptr, sizeLeft, neededSize, argumentString);
    CopyCString(ptr, sizeLeft, neededSize, "\"");
}

/*!
  @brief Indentation handling

  Each call adds a newline indentLevel * MSG_LIST_INDENTATION into buffer
  @param ptr [inout] pointer to position in buffer
  @param sizeLeft [inout] size left in buffer
  @param neededSize [inout] number of bytes totally needed
  @param indentLevel [in] the indentation level
  @return true if call succeeded, false if incomplete buffer
 */
static inline void Indentation( SAPDB_Char *&ptr
                              , SAPDB_UInt4 &sizeLeft
                              , SAPDB_UInt4 &neededSize
                              , SAPDB_UInt4 indentLevel )
{
    SAPDB_UInt4 stringLen = SAPDB_strlen("\n") + indentLevel * SAPDB_strlen(MSG_LIST_INDENTATION);

    CopyCString( ptr, sizeLeft, neededSize, "\n");
    for ( SAPDB_UInt4 iLevel = 0; iLevel < indentLevel; iLevel++ )
    {
        CopyCString( ptr, sizeLeft, neededSize, MSG_LIST_INDENTATION);
    }
}

/*---------------------------------------------------------------------------*/

static inline void CopyIndentedCString( SAPDB_UInt4 indentLevel
                                      , SAPDB_Char *&ptr
                                      , SAPDB_UInt4 &sizeLeft
                                      , SAPDB_UInt4 &neededSize
                                      , SAPDB_Char const *stringToCopy)
{
    Indentation( ptr, sizeLeft, neededSize, indentLevel );

    CopyCString( ptr, sizeLeft, neededSize, stringToCopy);
}

/*---------------------------------------------------------------------------*/

static inline void CopyIndentedTaggedCString( SAPDB_UInt4 indentLevel
                                            , SAPDB_Char *&ptr
                                            , SAPDB_UInt4 &sizeLeft
                                            , SAPDB_UInt4 &neededSize
                                            , SAPDB_Char const *tagToUse
                                            , SAPDB_Char const *stringToCopy )
{
    Indentation( ptr, sizeLeft, neededSize, indentLevel );
    CopyCString( ptr, sizeLeft, neededSize, "<");
    CopyCString( ptr, sizeLeft, neededSize, tagToUse);
    CopyCString( ptr, sizeLeft, neededSize, ">");

    Indentation( ptr, sizeLeft, neededSize, indentLevel+1 );
    CopyCString( ptr, sizeLeft, neededSize, stringToCopy);

    Indentation( ptr, sizeLeft, neededSize, indentLevel );
    CopyCString( ptr, sizeLeft, neededSize, "</");
    CopyCString( ptr, sizeLeft, neededSize, tagToUse);
    CopyCString( ptr, sizeLeft, neededSize, ">");
}

/*---------------------------------------------------------------------------*/

static inline void CopyIndentedXMLString( SAPDB_UInt4 indentLevel
                                        , SAPDB_Char *&ptr
                                        , SAPDB_UInt4 &sizeLeft
                                        , SAPDB_UInt4 &neededSize
                                        , SAPDB_UTF8 const *stringToCopy )
{
    Indentation( ptr, sizeLeft, neededSize, indentLevel );

    CopyXMLString( ptr, sizeLeft, neededSize, stringToCopy );
}

/*---------------------------------------------------------------------------*/
/*! define */
#define XML_LESS_THAN_REPLACEMENT "&lt;"
/*! define */
#define XML_GREATER_THAN_REPLACEMENT "&gt;"
/*! define */
#define XML_AMPERSAND_REPLACEMENT "&amp;"
/*! define */
#define XML_DOUBLE_QUOTE_REPLACEMENT "&quot;"
/*! define */
#define XML_SINGLE_QUOTE_REPLACEMENT "&apos;"

static inline void CopyXMLString( SAPDB_Char *&ptr
                                , SAPDB_UInt4 &sizeLeft
                                , SAPDB_UInt4 &neededSize
                                , SAPDB_UTF8 const *stringToCopy )
{
    SAPDB_Int4  iString;
    SAPDB_UTF8  charToCopy;
    SAPDB_UInt4 resultLen;

    resultLen = 0;
    for ( iString = 0; (charToCopy = stringToCopy[iString]) != 0; ++iString )
    {
        switch(charToCopy)
        {
        case (SAPDB_UTF8)'<':
            resultLen += SAPDB_strlen(XML_LESS_THAN_REPLACEMENT);
            if ( sizeLeft > resultLen )
            {
                SAPDB_memcpy(ptr, XML_LESS_THAN_REPLACEMENT, SAPDB_strlen(XML_LESS_THAN_REPLACEMENT) );
                ptr += SAPDB_strlen(XML_LESS_THAN_REPLACEMENT);
            }
            break;
        case (SAPDB_UTF8)'>':
            resultLen += SAPDB_strlen(XML_GREATER_THAN_REPLACEMENT);
            if ( sizeLeft > resultLen )
            {
                SAPDB_memcpy(ptr, XML_GREATER_THAN_REPLACEMENT, SAPDB_strlen(XML_GREATER_THAN_REPLACEMENT) );
                ptr += SAPDB_strlen(XML_GREATER_THAN_REPLACEMENT);
            }
            break;
        case (SAPDB_UTF8)'&':
            resultLen += SAPDB_strlen(XML_AMPERSAND_REPLACEMENT);
            if ( sizeLeft > resultLen )
            {
                SAPDB_memcpy(ptr, XML_AMPERSAND_REPLACEMENT, SAPDB_strlen(XML_AMPERSAND_REPLACEMENT) );
                ptr += SAPDB_strlen(XML_AMPERSAND_REPLACEMENT);
            }
            break;
        case (SAPDB_UTF8)'"':
            resultLen += SAPDB_strlen(XML_DOUBLE_QUOTE_REPLACEMENT);
            if ( sizeLeft > resultLen )
            {
                SAPDB_memcpy(ptr, XML_DOUBLE_QUOTE_REPLACEMENT, SAPDB_strlen(XML_DOUBLE_QUOTE_REPLACEMENT) );
                ptr += SAPDB_strlen(XML_DOUBLE_QUOTE_REPLACEMENT);
            }
            break;
        case (SAPDB_UTF8)'\'':
            resultLen += SAPDB_strlen(XML_SINGLE_QUOTE_REPLACEMENT);
            if ( sizeLeft > resultLen )
            {
                SAPDB_memcpy(ptr, XML_SINGLE_QUOTE_REPLACEMENT, SAPDB_strlen(XML_SINGLE_QUOTE_REPLACEMENT) );
                ptr += SAPDB_strlen(XML_SINGLE_QUOTE_REPLACEMENT);
            }
            break;
        default:
            ++resultLen;
            if ( sizeLeft > resultLen )
            {
                *(SAPDB_UTF8 *)ptr = charToCopy;
                ++ptr;
            }
            break;
        }
    }

    if ( sizeLeft > resultLen )
    {
        *ptr = 0; // zero termination if possible
    }

    neededSize += resultLen;

    if ( sizeLeft > resultLen ) { sizeLeft -= resultLen; }
    else { sizeLeft = 0; }
}

/*---------------------------------------------------------------------------*/

static inline void CopyIndentedTaggedXMLString( SAPDB_UInt4 indentLevel
                                              , SAPDB_Char *&ptr
                                              , SAPDB_UInt4 &sizeLeft
                                              , SAPDB_UInt4 &neededSize
                                              , SAPDB_Char const *tagToUse
                                              , SAPDB_UTF8 const *stringToCopy )
{
    Indentation( ptr, sizeLeft, neededSize, indentLevel );
    CopyCString( ptr, sizeLeft, neededSize, "<");
    CopyCString( ptr, sizeLeft, neededSize, tagToUse);
    CopyCString( ptr, sizeLeft, neededSize, ">");

    CopyIndentedXMLString( indentLevel + 1, ptr, sizeLeft, neededSize, stringToCopy );

    Indentation( ptr, sizeLeft, neededSize, indentLevel );
    CopyCString( ptr, sizeLeft, neededSize, "</");
    CopyCString( ptr, sizeLeft, neededSize, tagToUse);
    CopyCString( ptr, sizeLeft, neededSize, ">");
}

//---------------------------------------------------------------

static inline SAPDB_UInt4 CalcSizeOfDateTime( RTE_ISystem::DateTime const &dateTime )
{
    SAPDB_UInt4 totalBytesNeeded;

    totalBytesNeeded  = CalcSizeOfValue( (SAPDB_UInt2)dateTime.Year );
    totalBytesNeeded += CalcSizeOfValue( (SAPDB_UInt2)dateTime.Month );
    totalBytesNeeded += CalcSizeOfValue( (SAPDB_UInt2)dateTime.Day );
    totalBytesNeeded += CalcSizeOfValue( (SAPDB_UInt2)dateTime.DayOfWeek );
    totalBytesNeeded += CalcSizeOfValue( (SAPDB_UInt2)dateTime.Hour );
    totalBytesNeeded += CalcSizeOfValue( (SAPDB_UInt2)dateTime.Minute );
    totalBytesNeeded += CalcSizeOfValue( (SAPDB_UInt2)dateTime.Second );
    totalBytesNeeded += CalcSizeOfValue( (SAPDB_UInt2)dateTime.Milliseconds );

    return totalBytesNeeded;
}

//----------------------------------------------------------------

static inline SAPDB_UInt4 CalcSizeOfValue( SAPDB_UInt2 const value )
{
    SAPDB_UInt4 bytesNeeded;

    StoreValue(value, 0, 0, bytesNeeded );
    return bytesNeeded;
}

//----------------------------------------------------------------

static inline SAPDB_UInt4 CalcSizeOfValue( SAPDB_UInt4 const value )
{
    SAPDB_UInt4 bytesNeeded;

    StoreValue(value, 0, 0, bytesNeeded );
    return bytesNeeded;
}

//----------------------------------------------------------------

static inline SAPDB_UInt4 CalcSizeOfValue( SAPDB_UInt8 const value )
{
    SAPDB_UInt4 bytesNeeded;

    StoreValue(value, 0, 0, bytesNeeded );
    return bytesNeeded;
}

//----------------------------------------------------------------

static inline SAPDB_UInt4 CalcSizeOfString( SAPDB_Char const *start, SAPDB_UInt4 offset, SAPDB_UInt4 maxOffset )
{
    if ( offset >= maxOffset )
    {
        return 1;
    }

    for ( SAPDB_UInt4 iChar = 0; (iChar + offset) < maxOffset; iChar++ )
    {
        if ( start[iChar] != 0 )
        {
            return (iChar+1);
        }
    }
    return 1;
}

//----------------------------------------------------------------

static inline SAPDB_Bool LoadExpectedValue( SAPDB_UInt2 &value
                                          , SAPDB_UInt4 &bytesLeft
                                          , SAPDB_Byte const *&ptr )
{
    SAPDB_UInt2 loadedValue;
    SAPDB_UInt4 bytesNeeded;

    if ( !LoadValue(loadedValue, bytesLeft, ptr, bytesNeeded) )
    {
        return false;
    }
    bytesLeft -= bytesNeeded;
    ptr += bytesNeeded;
    value = loadedValue;
    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool LoadExpectedValue( SAPDB_UInt4 &value
                                          , SAPDB_UInt4 &bytesLeft
                                          , SAPDB_Byte const *&ptr )
{
    SAPDB_UInt4 loadedValue;
    SAPDB_UInt4 bytesNeeded;

    if ( !LoadValue(loadedValue, bytesLeft, ptr, bytesNeeded) )
    {
        return false;
    }
    bytesLeft -= bytesNeeded;
    ptr += bytesNeeded;
    value = loadedValue;
    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool LoadExpectedDateTime( RTE_ISystem::DateTime &dateTime
                                             , SAPDB_UInt4 &bytesLeft
                                             , SAPDB_Byte const *&ptr )
{
    dateTime = Msg_List::EmptyDateTimeValue;

    SAPDB_UInt2 dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.Year = (SAPDB_Int2)dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.Month = (SAPDB_Int2)dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.Day = (SAPDB_Int2)dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.DayOfWeek = (SAPDB_Int2)dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.Hour = (SAPDB_Int2)dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.Minute = (SAPDB_Int2)dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.Second = (SAPDB_Int2)dateTimePartValue;
    if ( !LoadExpectedValue(dateTimePartValue, bytesLeft, ptr) ) { return false; }
    dateTime.Milliseconds = (SAPDB_Int2)dateTimePartValue;

    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool LoadSkipToEndOfString( SAPDB_Char const * &string
                                              , SAPDB_UInt4 &bytesLeft
                                              , SAPDB_Byte const *&ptr )
{
    if ( !bytesLeft )
    {
        string = 0;
        return false;
    }

    string = (SAPDB_Char *)ptr;

    while ( bytesLeft > 0 && *ptr )
    {
        --bytesLeft;
        ++ptr;
    }
    if ( *ptr )
    {
        string = 0;
        return false;
    }

    // skip zero termination
    --bytesLeft; ++ptr;
    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool LoadSkipToEndOfUTF8String( SAPDB_UTF8 const * &string
                                    , SAPDB_UInt4 &bytesLeft
                                    , SAPDB_Byte const *&ptr )
{
    SAPDB_Char const *pString;

    if ( !LoadSkipToEndOfString( pString
                               , bytesLeft
                               , ptr ) )
    {
        string = 0;
        return false;
    }

    string = (SAPDB_UTF8 const *)pString;
    return true;
}

/*
  @brief Load a compressed UInt2 value from buffer

  @param value [out]
  @param bufferSizeInBytes [in]
  @param pBuffer [in] the buffer to be filled
  @param bytesUsed [out] number of bytes used
  @return true if call succeeded, false if incomplete buffer (bytesUsed = 0) or illegal number representation (bytesUsed>0)
 */
static inline SAPDB_Bool LoadValue( SAPDB_UInt2 &value
                           , SAPDB_UInt4 bufferSizeInBytes
                           , SAPDB_Byte const *pBuffer
                           , SAPDB_UInt4 &bytesUsed )
{
    if ( !pBuffer || bufferSizeInBytes == 0 )
    {
        bytesUsed = 0;
    }
    else
    {
        switch( pBuffer[0] )
        {
        default:
            bytesUsed = 1;
            value   = pBuffer[0];
            return true;
        case FOLLOWED_BY_1_BYTES:
            if ( bufferSizeInBytes < 2 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 2;
            value   = pBuffer[1];
            return true;
        case FOLLOWED_BY_2_BYTES:
            if ( bufferSizeInBytes < 3 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 3;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            return true;
        case FOLLOWED_BY_3_BYTES:
            bytesUsed = 4;
            break;
        case FOLLOWED_BY_4_BYTES:
            bytesUsed = 5;
            break;
        case FOLLOWED_BY_5_BYTES:
            bytesUsed = 6;
            break;
        case FOLLOWED_BY_6_BYTES:
            bytesUsed = 7;
            break;
        case FOLLOWED_BY_7_BYTES:
            bytesUsed = 8;
            break;
        case FOLLOWED_BY_8_BYTES:
            bytesUsed = 9;
            break;
        case NUMBER_ILLEGAL:
            bytesUsed = 1;
            break;
        }
    }

    return false;
}

/*
  @brief Load a compressed UInt4 value from buffer

  @param value [out]
  @param bufferSizeInBytes [in]
  @param pBuffer [in] the buffer to be filled
  @param bytesUsed [out] number of bytes used
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool LoadValue( SAPDB_UInt4 &value
                           , SAPDB_UInt4 bufferSizeInBytes
                           , SAPDB_Byte const *pBuffer
                           , SAPDB_UInt4 &bytesUsed )
{
    if ( !pBuffer || bufferSizeInBytes == 0 )
    {
        bytesUsed = 0;
    }
    else
    {
        switch( pBuffer[0] )
        {
        default:
            bytesUsed = 1;
            value   = pBuffer[0];
            return true;
        case FOLLOWED_BY_1_BYTES:
            if ( bufferSizeInBytes < 2 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 2;
            value   = pBuffer[1];
            return true;
        case FOLLOWED_BY_2_BYTES:
            if ( bufferSizeInBytes < 3 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 3;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            return true;
        case FOLLOWED_BY_3_BYTES:
            if ( bufferSizeInBytes < 4 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 4;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            return true;
        case FOLLOWED_BY_4_BYTES:
            if ( bufferSizeInBytes < 5 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 5;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            value <<= 8;
            value  += pBuffer[4];
            return true;
        case FOLLOWED_BY_5_BYTES:
            bytesUsed = 6;
            break;
        case FOLLOWED_BY_6_BYTES:
            bytesUsed = 7;
            break;
        case FOLLOWED_BY_7_BYTES:
            bytesUsed = 8;
            break;
        case FOLLOWED_BY_8_BYTES:
            bytesUsed = 9;
            break;
        case NUMBER_ILLEGAL:
            bytesUsed = 1;
            break;
        }
    }

    return false;
}

/*
  @brief Load a compressed UInt8 value from buffer

  @param value [out]
  @param bufferSizeInBytes [in]
  @param pBuffer [in] the buffer to be filled
  @param bytesUsed [out] number of bytes used
  @return true if call succeeded, false if incomplete buffer
 */
static inline SAPDB_Bool LoadValue( SAPDB_UInt8 &value
                           , SAPDB_UInt4 bufferSizeInBytes
                           , SAPDB_Byte const *pBuffer
                           , SAPDB_UInt4 &bytesUsed )
{
    if ( !pBuffer || bufferSizeInBytes == 0 )
    {
        bytesUsed = 0;
    }
    else
    {
        switch( pBuffer[0] )
        {
        default:
            bytesUsed = 1;
            value   = pBuffer[0];
            return true;
        case FOLLOWED_BY_1_BYTES:
            if ( bufferSizeInBytes < 2 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 2;
            value   = pBuffer[1];
            return true;
        case FOLLOWED_BY_2_BYTES:
            if ( bufferSizeInBytes < 3 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 3;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            return true;
        case FOLLOWED_BY_3_BYTES:
            if ( bufferSizeInBytes < 4 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 4;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            return true;
        case FOLLOWED_BY_4_BYTES:
            if ( bufferSizeInBytes < 5 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 5;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            value <<= 8;
            value  += pBuffer[4];
            return true;
        case FOLLOWED_BY_5_BYTES:
            if ( bufferSizeInBytes < 6 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 6;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            value <<= 8;
            value  += pBuffer[4];
            value <<= 8;
            value  += pBuffer[5];
            return true;
        case FOLLOWED_BY_6_BYTES:
            if ( bufferSizeInBytes < 7 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 7;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            value <<= 8;
            value  += pBuffer[4];
            value <<= 8;
            value  += pBuffer[5];
            value <<= 8;
            value  += pBuffer[6];
            return true;
        case FOLLOWED_BY_7_BYTES:
            if ( bufferSizeInBytes < 8 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 8;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            value <<= 8;
            value  += pBuffer[4];
            value <<= 8;
            value  += pBuffer[5];
            value <<= 8;
            value  += pBuffer[6];
            value <<= 8;
            value  += pBuffer[7];
            return true;
        case FOLLOWED_BY_8_BYTES:
            if ( bufferSizeInBytes < 9 )
            {
                bytesUsed = 0;
                break;
            }
            bytesUsed = 9;
            value   = pBuffer[1];
            value <<= 8;
            value  += pBuffer[2];
            value <<= 8;
            value  += pBuffer[3];
            value <<= 8;
            value  += pBuffer[4];
            value <<= 8;
            value  += pBuffer[5];
            value <<= 8;
            value  += pBuffer[6];
            value <<= 8;
            value  += pBuffer[7];
            value <<= 8;
            value  += pBuffer[8];
            return true;
        case NUMBER_ILLEGAL:
            bytesUsed = 1;
            break;
        }
    }

    return false;
}

//----------------------------------------------------------------

static inline SAPDB_Bool StoreNeededValue( SAPDB_Char const *valueName
                                         , SAPDB_UInt8 const value
                                         , SAPDB_UInt4      &bytesLeft
                                         , SAPDB_Byte *     &ptr
                                         , SAPDB_UInt4      &savedBytes
                                         , Msg_List         &errList )
{
    SAPDB_UInt4 neededBytes;
    if ( !StoreValue(value, bytesLeft, ptr, neededBytes) )
    {
        SAPDB_ToStringClass numberString(value);
        SAPDB_ToStringClass sizeString(bytesLeft + savedBytes);
        SAPDB_ToStringClass savedString(savedBytes);
        SAPDB_ToStringClass neededString(neededBytes);
        errList = Msg_List( Msg_List::Error
                          , SDBMSG_MESSAGES_STORE_VALUE
                          , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)valueName)
                          , Msg_SimpleArg("MEMBER_VALUE", (SAPDB_UTF8 const *)(SAPDB_Char const *)numberString)
                          , Msg_SimpleArg("BYTE_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)sizeString)
                          , Msg_SimpleArg("BYTES_USED", (SAPDB_UTF8 const *)(SAPDB_Char const *)savedString)
                          , Msg_SimpleArg("BYTES_NEEDED", (SAPDB_UTF8 const *)(SAPDB_Char const *)neededString) );
        bytesLeft = 0;
        return false;
    }
    bytesLeft -= neededBytes;
    savedBytes += neededBytes;
    ptr += neededBytes;
    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool StoreNeededValue( SAPDB_Char const *valueName
                                         , SAPDB_UInt4 const value
                                         , SAPDB_UInt4      &bytesLeft
                                         , SAPDB_Byte *     &ptr
                                         , SAPDB_UInt4      &savedBytes
                                         , Msg_List         &errList )
{
    SAPDB_UInt4 neededBytes;
    if ( !StoreValue(value, bytesLeft, ptr, neededBytes) )
    {
        SAPDB_ToStringClass numberString(value);
        SAPDB_ToStringClass sizeString(bytesLeft + savedBytes);
        SAPDB_ToStringClass savedString(savedBytes);
        SAPDB_ToStringClass neededString(neededBytes);
        errList = Msg_List( Msg_List::Error
                          , SDBMSG_MESSAGES_STORE_VALUE
                          , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)valueName)
                          , Msg_SimpleArg("MEMBER_VALUE", (SAPDB_UTF8 const *)(SAPDB_Char const *)numberString)
                          , Msg_SimpleArg("BYTE_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)sizeString)
                          , Msg_SimpleArg("BYTES_USED", (SAPDB_UTF8 const *)(SAPDB_Char const *)savedString)
                          , Msg_SimpleArg("BYTES_NEEDED", (SAPDB_UTF8 const *)(SAPDB_Char const *)neededString) );
        bytesLeft = 0;
        return false;
    }
    bytesLeft -= neededBytes;
    savedBytes += neededBytes;
    ptr += neededBytes;
    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool StoreOptionalValue( SAPDB_Char const * valueName
                                           , SAPDB_UInt2 const  value
                                           , SAPDB_UInt4       &bytesLeft
                                           , SAPDB_Byte *      &ptr
                                           , SAPDB_UInt4       &savedBytes
                                           , SAPDB_Bool const   allowTruncate
                                           , Msg_List          &errList )
{
    SAPDB_UInt4 neededBytes;
    if ( !StoreValue(value, bytesLeft, ptr, neededBytes) )
    {
        if ( allowTruncate )
        {
            bytesLeft = 0;
        }
        else
        {
            SAPDB_ToStringClass numberString(value);
            SAPDB_ToStringClass sizeString(bytesLeft + savedBytes);
            SAPDB_ToStringClass savedString(savedBytes);
            SAPDB_ToStringClass neededString(neededBytes);
            errList = Msg_List( Msg_List::Error
                              , SDBMSG_MESSAGES_STORE_VALUE
                              , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)valueName)
                              , Msg_SimpleArg("MEMBER_VALUE", (SAPDB_UTF8 const *)(SAPDB_Char const *)numberString)
                              , Msg_SimpleArg("BYTE_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)sizeString)
                              , Msg_SimpleArg("BYTES_USED", (SAPDB_UTF8 const *)(SAPDB_Char const *)savedString)
                              , Msg_SimpleArg("BYTES_NEEDED", (SAPDB_UTF8 const *)(SAPDB_Char const *)neededString) );
            bytesLeft = 0;
            return false;
        }
    }
    else
    {
        bytesLeft -= neededBytes;
        savedBytes += neededBytes;
        ptr += neededBytes;
    }
    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool StoreOptionalValue( SAPDB_Char const * valueName
                                           , SAPDB_UInt4 const  value
                                           , SAPDB_UInt4       &bytesLeft
                                           , SAPDB_Byte *      &ptr
                                           , SAPDB_UInt4       &savedBytes
                                           , SAPDB_Bool const   allowTruncate
                                           , Msg_List          &errList )
{
    SAPDB_UInt4 neededBytes;
    if ( !StoreValue(value, bytesLeft, ptr, neededBytes) )
    {
        if ( allowTruncate )
        {
            bytesLeft = 0;
        }
        else
        {
            SAPDB_ToStringClass numberString(value);
            SAPDB_ToStringClass sizeString(bytesLeft + savedBytes);
            SAPDB_ToStringClass savedString(savedBytes);
            SAPDB_ToStringClass neededString(neededBytes);
            errList = Msg_List( Msg_List::Error
                              , SDBMSG_MESSAGES_STORE_VALUE
                              , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)valueName)
                              , Msg_SimpleArg("MEMBER_VALUE", (SAPDB_UTF8 const *)(SAPDB_Char const *)numberString)
                              , Msg_SimpleArg("BYTE_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)sizeString)
                              , Msg_SimpleArg("BYTES_USED", (SAPDB_UTF8 const *)(SAPDB_Char const *)savedString)
                              , Msg_SimpleArg("BYTES_NEEDED", (SAPDB_UTF8 const *)(SAPDB_Char const *)neededString) );
            bytesLeft = 0;
            return false;
        }
    }
    else
    {
        bytesLeft -= neededBytes;
        savedBytes += neededBytes;
        ptr += neededBytes;
    }
    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool StoreOptionalDateTime( SAPDB_Char const *           valueName
                                              , RTE_ISystem::DateTime const &dateTime
                                              , SAPDB_UInt4                 &bytesLeft
                                              , SAPDB_Byte *                &ptr
                                              , SAPDB_UInt4                 &savedBytes
                                              , SAPDB_Bool const             allowTruncate
                                              , Msg_List                    &errList )
{
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.Year
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.Month
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.Day
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.DayOfWeek
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.Hour
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.Minute
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.Second
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 
    if ( !StoreOptionalValue( valueName, (SAPDB_UInt2)dateTime.Milliseconds
        , bytesLeft, ptr, savedBytes, allowTruncate, errList ) ) { return false; } 

    return true;
}

//----------------------------------------------------------------

static inline SAPDB_Bool StoreOptionalString( SAPDB_Char const *valueName
                                            , SAPDB_Char const *varPart
                                            , SAPDB_UInt4 const offset
                                            , SAPDB_UInt4 const maxOffset
                                            , SAPDB_UInt4      &bytesLeft
                                            , SAPDB_Byte *     &ptr
                                            , SAPDB_UInt4      &savedBytes
                                            , SAPDB_Bool const  allowTruncate
                                            , Msg_List         &errList )
{
    SAPDB_UInt4 neededBytes = 0;

    if ( offset >= maxOffset )
    {
        SAPDB_ToStringClass offsetString(offset);
        SAPDB_ToStringClass maxOffsetString(maxOffset);
        SAPDB_ToStringClass sizeString(bytesLeft + savedBytes);
        SAPDB_ToStringClass savedString(savedBytes);
        errList = Msg_List( Msg_List::Error
                          , SDBMSG_MESSAGES_STORE_ILLEGAL_OFFSET
                          , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)valueName)
                          , Msg_SimpleArg("BYTE_OFFSET", (SAPDB_UTF8 const *)(SAPDB_Char const *)offsetString)
                          , Msg_SimpleArg("BYTE_MAX_OFFSET", (SAPDB_UTF8 const *)(SAPDB_Char const *)maxOffsetString)
                          , Msg_SimpleArg("BYTE_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)sizeString)
                          , Msg_SimpleArg("BYTES_USED", (SAPDB_UTF8 const *)(SAPDB_Char const *)savedString) );
        return false;
    }

    SAPDB_Char const *source = varPart + offset;

    for ( neededBytes = 0; (neededBytes + offset) < maxOffset; ++neededBytes )
    {
        if ( source[neededBytes] == 0 )
        {
            break;
        }
    }
    ++neededBytes;

    if ( (offset + neededBytes) > maxOffset )
    {
        SAPDB_ToStringClass valueString(source, (SAPDB_UInt2)neededBytes, (SAPDB_UInt2)neededBytes);
        SAPDB_ToStringClass offsetString(offset);
        SAPDB_ToStringClass maxOffsetString(maxOffset);
        errList = Msg_List( Msg_List::Error
                          , SDBMSG_MESSAGES_STORE_UNTERMINATED_STRING
                          , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)valueName)
                          , Msg_SimpleArg("MEMBER_VALUE", (SAPDB_UTF8 const *)(SAPDB_Char const *)valueString)
                          , Msg_SimpleArg("BYTE_OFFSET", (SAPDB_UTF8 const *)(SAPDB_Char const *)offsetString)
                          , Msg_SimpleArg("BYTE_MAX_OFFSET", (SAPDB_UTF8 const *)(SAPDB_Char const *)maxOffsetString) );
        return false;
    }

    if ( neededBytes <= bytesLeft )
    {
        SAPDB_memcpy(ptr, source, neededBytes);
        ptr += neededBytes;
        bytesLeft -= neededBytes;
        savedBytes += neededBytes;
    }
    else if ( bytesLeft && allowTruncate )
    {
        SAPDB_memcpy(ptr, source, bytesLeft-1);
        ptr += bytesLeft;
        savedBytes += bytesLeft;
        *ptr = 0;
        bytesLeft = 0;
    }
    else if ( bytesLeft || !allowTruncate )
    {
        SAPDB_ToStringClass sizeString(bytesLeft + savedBytes);
        SAPDB_ToStringClass savedString(savedBytes);
        SAPDB_ToStringClass neededString(neededBytes);
        errList = Msg_List( Msg_List::Error
                          , SDBMSG_MESSAGES_STORE_STRING
                          , Msg_SimpleArg("MEMBER_NAME", (SAPDB_UTF8 const *)valueName)
                          , Msg_SimpleArg("MEMBER_VALUE", (SAPDB_UTF8 const *)source)
                          , Msg_SimpleArg("BYTE_SIZE", (SAPDB_UTF8 const *)(SAPDB_Char const *)sizeString)
                          , Msg_SimpleArg("BYTES_USED", (SAPDB_UTF8 const *)(SAPDB_Char const *)savedString)
                          , Msg_SimpleArg("BYTES_NEEDED", (SAPDB_UTF8 const *)(SAPDB_Char const *)neededString) );
        bytesLeft = 0;
        return false;
    }
    return true;
}

/*
  @brief store a UInt2 into buffer using compression

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param neededSize [out]
  @return true if call succeeded, false if not enaugh space left in buffer
 */
static inline SAPDB_Bool StoreValue( SAPDB_UInt2 const value
                            , SAPDB_UInt4 bufferSizeInBytes
                            , SAPDB_Byte *pBuffer
                            , SAPDB_UInt4 &neededSize )
{
    if ( value > LAST_SINGLE_BYTE_NUMBER )
    {
        if ( value < (1<<8) )
        {
            neededSize = 2;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[1] = (SAPDB_Byte)value;
            pBuffer[0] = FOLLOWED_BY_1_BYTES;
        }
        else
        {
            neededSize = 3;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[2] = (SAPDB_Byte)(value&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(value>>8);
            pBuffer[0] = FOLLOWED_BY_2_BYTES;
        }
    }
    else
    {
        neededSize = 1;
        if ( neededSize > bufferSizeInBytes ) return false;
        pBuffer[0] = (SAPDB_Byte)value;
    }
    return true;
}

/*
  @brief store a UInt4 into buffer using compression

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param neededSize [out]
  @return true if call succeeded, false if not enaugh space left in buffer
 */
static inline SAPDB_Bool StoreValue( SAPDB_UInt4 const value
                            , SAPDB_UInt4 bufferSizeInBytes
                            , SAPDB_Byte *pBuffer
                            , SAPDB_UInt4 &neededSize )
{
    if ( value > LAST_SINGLE_BYTE_NUMBER )
    {
        if ( value < (1<<8) )
        {
            neededSize = 2;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[1] = (SAPDB_Byte)value;
            pBuffer[0] = FOLLOWED_BY_1_BYTES;
        }
        else if ( value < (1<<16) )
        {
            neededSize = 3;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[2] = (SAPDB_Byte)(value&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(value>>8);
            pBuffer[0] = FOLLOWED_BY_2_BYTES;
        }
        else if ( value < (1<<24) )
        {
            neededSize = 4;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[3] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt4 tmpValue = value>>8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_3_BYTES;
        }
        else
        {
            neededSize = 5;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[4] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt4 tmpValue = value>>8;
            pBuffer[3] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_4_BYTES;
        }
    }
    else
    {
        neededSize = 1;
        if ( neededSize > bufferSizeInBytes ) return false;
        pBuffer[0] = (SAPDB_Byte)value;
    }
    return true;
}


/*
  @brief store a UInt8 into buffer using compression

  @param value [in]
  @param bufferSizeInBytes [in]
  @param pBuffer [out] the buffer to be filled
  @param neededSize [out]
  @return true if call succeeded, false if not enaugh space left in buffer
 */
static inline SAPDB_Bool StoreValue( SAPDB_UInt8 const value
                            , SAPDB_UInt4 bufferSizeInBytes
                            , SAPDB_Byte *pBuffer
                            , SAPDB_UInt4 &neededSize )
{
    if ( value > LAST_SINGLE_BYTE_NUMBER )
    {
        if ( value < (((SAPDB_UInt8)1)<<8) )
        {
            neededSize = 2;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[1] = (SAPDB_Byte)value;
            pBuffer[0] = FOLLOWED_BY_1_BYTES;
        }
        else if ( value < (((SAPDB_UInt8)1)<<16) )
        {
            neededSize = 3;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[2] = (SAPDB_Byte)(value&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(value>>8);
            pBuffer[0] = FOLLOWED_BY_2_BYTES;
        }
        else if ( value < (((SAPDB_UInt8)1)<<24) )
        {
            neededSize = 4;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[3] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt8 tmpValue = value>>8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_3_BYTES;
        }
        else if ( value < (((SAPDB_UInt8)1)<<32) )
        {
            neededSize = 5;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[4] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt8 tmpValue = value>>8;
            pBuffer[3] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_4_BYTES;
        }
        else if ( value < (((SAPDB_UInt8)1)<<40) )
        {
            neededSize = 6;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[5] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt8 tmpValue = value>>8;
            pBuffer[4] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[3] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_5_BYTES;
        }
        else if ( value < (((SAPDB_UInt8)1)<<48) )
        {
            neededSize = 7;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[6] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt8 tmpValue = value>>8;
            pBuffer[5] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[4] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[3] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_6_BYTES;
        }
        else if ( value < (((SAPDB_UInt8)1)<<56) )
        {
            neededSize = 8;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[7] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt8 tmpValue = value>>8;
            pBuffer[6] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[5] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[4] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[3] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_7_BYTES;
        }
        else
        {
            neededSize = 9;
            if ( neededSize > bufferSizeInBytes ) return false;
            pBuffer[8] = (SAPDB_Byte)(value&0x0FF);
            SAPDB_UInt8 tmpValue = value>>8;
            pBuffer[7] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[6] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[5] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[4] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[3] = (SAPDB_Byte)(tmpValue&0x0FF);
            tmpValue >>= 8;
            pBuffer[2] = (SAPDB_Byte)(tmpValue&0x0FF);
            pBuffer[1] = (SAPDB_Byte)(tmpValue>>8);
            pBuffer[0] = FOLLOWED_BY_8_BYTES;
        }
    }
    else
    {
        neededSize = 1;
        if ( neededSize > bufferSizeInBytes ) return false;
        pBuffer[0] = (SAPDB_Byte)value;
    }
    return true;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
