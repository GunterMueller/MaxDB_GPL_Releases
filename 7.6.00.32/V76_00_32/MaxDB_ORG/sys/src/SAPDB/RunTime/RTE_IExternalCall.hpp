/*!
  @file           RTE_IExternalCall.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          External command call interface
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



#ifndef RTE_IEXTERNALCALL_HPP
#define RTE_IEXTERNALCALL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

class RTE_IExternalCallReplyContext;
class SAPDBErr_MessageList;
class SAPDBMem_IRawAllocator;

/*!
   @brief Spawn an external process to restart / shutdown the database
   @param startOrStop [in] if TRUE a restart is wanted, if FALSE a shutdown is wanted
   @return true if dbmstart or dbmstop succefully spawned, false if not (message generated)
 */
SAPDB_Bool RTE_SpawnDbmstartOrStop( SAPDB_Bool startOrStop );

/*!
   ReplyHandler supplied by user of RTE_IExternalCall interface
   NOTE: reply is only valid while in handler. Any result must be copied before leaving handler. This is
   normally done in the replyContext, which is caller specific.
 */
typedef SAPDB_Bool (*RTE_IExternalCallReplyHandler)( SAPDB_Char const             *command ///< the command used
                                                    ,SAPDB_Char const             *reply   ///< the reply received
                                                    ,RTE_IExternalCallReplyContext *replyContext ///< the own context
                                                    ,SAPDBErr_MessageList         &errList  ///< message in case of trouble
                                                   );
/*!
  @brief Interpret output of external command expecting 'OK'
  @param command [in] used command string
  @param replyContext [in] context for the call
  @param errList [out] errorlist to be filled if parsing failes
  @return true if call succeeded, false else (errList filled)
 */
SAPDB_Bool RTE_ExternalCallReplyOk( SAPDB_Char const              *command,
                                    SAPDB_Char const              *reply,
                                    RTE_IExternalCallReplyContext *, // reply context not needed
                                    SAPDBErr_MessageList          &errList);

/*!
  @brief Completely ignore the reply
  @param command [in] used command string
  @param replyContext [in] context for the call
  @param errList [out] errorlist to be filled if parsing failes
  @return true if call succeeded, false else (errList filled)
 */
SAPDB_Bool RTE_ExternalCallIgnoreReply( SAPDB_Char const              *command,
                                        SAPDB_Char const              *reply,
                                        RTE_IExternalCallReplyContext *, // reply context not needed
                                        SAPDBErr_MessageList          &errList);

class RTE_IExternalCall
{
public:
    static SAPDB_UInt4 defaultReplySize;
    /*!
     @brief Create an external call interface with optional given initial reply buffer size

     Allocator internally used is a runtime internal allocator.

     @param errList [inout] error report list if 0 is returned
     @param initialReplySize [in] initial reply buffer size
     @return pointer to the interface of 0 if call failed
     */
    static RTE_IExternalCall *CreateExternalCall( SAPDBErr_MessageList & errList
                                                , SAPDB_UInt4 initialReplySize=defaultReplySize );

    /*!
     @brief Create an external call interface with own allocator and optional given initial reply buffer size

     Allocator is used both for allocating the ExternnalCall Interface and the reply buffer

     @param errList [inout] error report list if 0 is returned
     @param ownAllocator [in] allocator to be used
     @param initialReplySize [in] initial reply buffer size
     @return pointer to the interface of 0 if call failed
     */
    static RTE_IExternalCall *CreateExternalCall( SAPDBErr_MessageList & errList
                                                , SAPDBMem_IRawAllocator& ownAllocator
                                                , SAPDB_UInt4 initialReplySize=defaultReplySize );

    /*!
     @brief Destroy an external call interface
     @param externalCall [in] the external call interface to destroy
     */
    static void DestroyExternalCall( RTE_IExternalCall *externalCall );

    /*!
     @brief Execute the given external command

     Calls the given command, waits for the reply and let the given reply handler work on the reply.
     Example for a reply handler is the supplied RTE_ExternalCallReplyOk(), that does not need a context
     @param command [in] the complete command including all arguements as a single string
     @param replyHandler [in] the reply handler called to handle the reply
     @param replyContext [in] the caller specific reply context
     @param errList [inout] the error list filled in case of problems
     @return true if call succeeded, false is not. This is the result of reply handling if call was sucessfully issued!
     */
    virtual SAPDB_Bool ExecuteCommand( SAPDB_Char const              * command,
                                       RTE_IExternalCallReplyHandler   replyHandler,
                                       RTE_IExternalCallReplyContext * replyContext,
                                       SAPDBErr_MessageList          & errList) = 0;

    /*!
      @brief Return allocator associated with the external call
      @return the allocator as defined in CreateExternalCall().
     */
    virtual SAPDBMem_IRawAllocator &GetAllocator() = 0;
};

#endif /* RTE_IEXTERNALCALL_HPP */
