/*!
  @file           SrvTasks_JobJoinInvSelector.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines class SrvTasks_JobJoinInvSelector

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
#ifndef SrvTasks_JobJoinInvSelector_HPP
#define SrvTasks_JobJoinInvSelector_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
//-------------------------------------------------------------------------------
/// This job is used to prefetch for the join inv selector
class SrvTasks_JobJoinInvSelector : public SrvTasks_Job
{
public:
    /// structure used to transport required knowledge to servers.
    struct Input
	{
        void  *m_pIterator;//<! iterator (coordinator).
        void  *m_pServer;  //<! the server 
        void  *m_pTrans;   //<! coordinator transaction
        /// constructor
        Input (void   *pServerInfo,
               void  *pServer,
               void  *pTrans)
        : m_pIterator(pServerInfo)
        , m_pServer(pServer)
        , m_pTrans(pTrans)
        {}
    };
public:
    /// constructor
    SrvTasks_JobJoinInvSelector(SAPDB_UInt   serverid,
                                const Input &input)
    : SrvTasks_Job(jobJoinInvSelect,Attributes(serverid))
    , m_Input(input)
    {}
    virtual void Deallocate() {}
protected:
    virtual Result ExecuteIntern (Trans_Context &trans);
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const
    {
        jobCopy = new (allocator) SrvTasks_JobJoinInvSelector(m_Attributes.Id, m_Input);
        if ( 0 == jobCopy )
        {
            AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
            return false;
        }
        jobCopy->SetAttributes(m_Attributes);
        return true;
    }
private:
    Input m_Input; //<! input values for this job
};
//-------------------------------------------------------------------------------
void Join_InvSelectByServer (      Trans_Context                      &trans,
                             const SrvTasks_JobJoinInvSelector::Input &input);
#endif // SrvTasks_JobJoinInvSelector_HPP
