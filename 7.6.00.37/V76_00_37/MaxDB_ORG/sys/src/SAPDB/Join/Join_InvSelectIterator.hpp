/*!
  @file           Join_InvSelectIterator.hpp
  @author         ThomasA
  @brief          defines an iterator for an inversion list

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

#ifndef JOIN_INVSELECTITERATOR_HPP
#define JOIN_INVSELECTITERATOR_HPP

#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "Container/Container_Vector.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "Transaction/Trans_Context.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_JobJoinInvSelector.hpp"

class Join_InvSelectServer; //<! forward declaration for server implementation, see Join_InvSelectIterator.cpp

/*!
   @brief defines an iterator returning qualified base table rows of a given inversion list.
          The special quality of this iterator is the fact that the inversion list may be
          worked off in parallel by several servers. The decision whether to run in parallel
          is made by the iterator itself depending on the length of the inversion list and
          the concurrent Join_InvSelectServer's running.

          The chief purpose of this iterator is to do read I/O in parallel. Since indexes tend to
          random key access to the base table, it is very likely that each key of the inversion list
          requires a disk I/O. In this case it makes sense to 
 */
class Join_InvSelectIterator
{
    typedef tgg00_BasisError IteratorError;
public :
    /*!
       @brief constructor, intializes the iterator and stores the iterator globals 
     */
    Join_InvSelectIterator (
        tgg00_TransContext      &Trans,
        SQLMan_MessBlock        &messBlock,
        tgg00_FileId            &InvTree,
        tgg00_SelectFieldsParam &SelectParam,
        tgg00_StackDesc         &StackDesc,
        const SAPDB_Int4        &maxServerTasks);
    /*!
       @brief destructor, stop all servers, if necessary. 
     */
    ~Join_InvSelectIterator();
    /*!
      @brief seals off the iteration of the current inversion list. All servers are terminated.
     */
    void Close();
    /*!
       @brief returns the next row from the iterator. The row is constructed as described
              by the stack description and is returned in the result buffer given at
              iterator construction.
     */
    tgg00_BasisError NextRow (void* buf);
    /*!
       @brief initializes for iteration over a given inversion list identified by an inversion key (secondary key).

       @param[in] invKey identifies the inversion list to be iterated.
       @param[in] startKey identifies the starting point inside the inversion list. No rows having keys less than
                            startKey are considered by the iterator.
       @param[in] stopKey identifies the last primary key inside the inversion list. No rows having keys greater
                           than stopKey are considered by the iterator.
       @param[in] InvRangeSet tell how to interpret startKey and stopKey.
     */
    tgg00_BasisError Open (tgg00_Lkey       &invKey,
        tgg00_Lkey              &startKey,
        tgg00_Lkey              &stopKey,
        tgg00_BdInvSet          &InvRangeSet);

    SAPDB_Int GetMaxServerTasks();
private :
    // methods

    /*!
       @brief aborts iterator execution, i.e. stops all running servers if necessary 
     */
    bool Abort() { return m_stop; }
    /*!
      @brief fills the key buffer with keys of the current inv list
     */
    bool FillKeyBuffer();
    /*!
       @brief distibutes the keys of the key buffer to the server tasks
     */
    void EnterRequest (int keyCount);
    /*!
       @brief returns the file id od the primary data tree
     */
    tgg00_FileId& GetTree() { return m_tree; }
    /*!
       @brief tells the iterator, that a server may provide rowCount rows. After that event
              the iterator is resumed if necessary
              
       @param[in] rowCount number of rows provides by the calling server
     */
    void IncRowCount(int rowCount);
    /*!
      @brief returns true, if iterator is is suspended; otherwise false.
     */
    inline bool IsSuspended() const;
    /*!
      @brief routine used to return the result of method NextRow. In case of an error all servers are stopped
     */
    inline tgg00_BasisError NextRowResult(tgg00_BasisError e);
    /*!
       @brief informs the iterator that a server finished execution of a request
     */
    void NotifyRequestFinished();
    /*!
       @brief resumes the iterator, if idle 
     */
    inline void Resume();
    /*!
       @brief stops all servers which are working for the iterator
     */
    void StopServers();
    /*!
       @brief stores the last key of the key buffer. This becomes the new starting point for b03get_inv
     */
    void StoreLastKey (const tsp00_Key& key, int keyLength);
    /*!
       @brief starts a server identified by serverId. The required knowledge is transported via an instance
       of ServerInfo, which is placed in the mtree of the mess block.
     */
    void StartServer(int serverId);
    /*!
       @brief copies a key
     */
    inline static void CopyKey(const tgg00_Lkey& source, tgg00_Lkey& dest);
    /*!
       @brief returns true, if servers have been started for the iterator; otherwise false.
     */
    bool CreatedParallelServers();
    /*!
       @brief returns the next qualified row of the base table. This method is used only if no
       parallel execution takes place.
     */
    tgg00_BasisError NextRecord(void* buf);
    /*!
       @brief returns true, if no error has been occurred; otherwise false.
     */
    inline bool NoError() { return (0 == m_error); }
    /*!
       @brief suspends the iterator, until a server provides a row or all servers are ready
     */
    void Suspend();
    /*!
       @brief sets maximum number of server tasks for parallel execution
     */
    void SetMaxServerTasks(SAPDB_Int4 max); 

    /// returns the messblock for the servertasks
    const tgg00_MessBlock& GetMessBlock() const
    {
        return m_messBlock;
    }

    // members
    static bool                             m_serversInUse;      //<! true, if a parallel Join_InvSelectIterator is running
                                                                 //<! only on iterator may execute parallel at a time
    RTESync_Spinlock                        m_lock;              //<! synchonization object of the iterator
    SQLMan_MessBlock&                       m_messBlock;         //<! reference to the mess block decribing the output and qualification
    tgg00_FileId                            m_tree;              //<! file id of the base table b* tree
    tgg00_SelectFieldsParam&                m_selFieldsParam;    //<! select context  
    tgg00_FileId&                           m_invFileId;         //<! file id of the index
    tgg00_Lkey*                             m_pInvKey;           //<! pointer to the current secondary key
    tgg00_Lkey                              m_startKey;          //<! current start key in the inversion list 
    tgg00_Lkey*                             m_pStopKey;          //<! pointer to the base table stop key
    IteratorError                           m_error;             //<! global iterator error
    tgg00_BdInvSet                          m_bdInvSet;          //<! describes meaning of start and stop key
    bool                                    m_stop;              //<! if true, abort iterator
    bool                                    m_suspended;         //<! true, if iterator is suspended
    bool                                    m_isOpen;            //<! true, if iterator is opened
    bool                                    m_countStatistics;   //<! true, if parallel execution has still to be counted
    SAPDBMem_IRawAllocator&                 m_allocator;         //<! allocator to be used for memory allocation
    int                                     m_maxServers;        //<! maximal number of servers to be used for iteration
    Container_Vector<Join_InvSelectServer*> m_servers;           //<! vector of servers working for the iterator
    tgg00_TransContext&                     m_coordinatorTrans;  //<! transaction context 
    int                                     m_countRowsProvided; //<! number of rows provided by all servers
    int                                     m_countRowsHandled;  //<! number of rows executed by the iterator
    int                                     m_runningServers;    //<! number of servers running for the current request
    int                                     m_startedServers;    //<! number of servers started for the currently open inv list
    int                                     m_currServer;        //<! pointer to server which will next be asked for a row 
    SAPDB_Int4                              m_keyCount;          //<! number of keys in key buffer
    tgg00_BasisError                        m_getInvError;       //<! return of last b03get_inv call
    tgg00_KeylistBuf                        m_keyBuffer;         //<! buffer containing keys returned by b03get_inv
    SAPDB_Byte*                             m_pKeyBuffer;        //<! pointer to current key inside m_keyBuffer 
    SrvTasks_WaitingTask                          m_WaitingTask; //<! waiting task context for coordinator of servertasks
    SrvTasks_JobList<SrvTasks_JobJoinInvSelector> m_Jobs;        //<! list of started parallel jobs
    friend class Join_InvSelectServer;
    friend void Join_InvSelectByServer (      Trans_Context                      &trans,
                                        const SrvTasks_JobJoinInvSelector::Input &input);
};

#endif
