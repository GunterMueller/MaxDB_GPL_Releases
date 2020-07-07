/*!
  @file           SrvTasks_JobCollection.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines the class SrvTasks_JobCollection.

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
#ifndef SrvTasks_JobCollection_HPP
#define SrvTasks_JobCollection_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ggg00.h"
//--------------------------------------------------------------------------------------
/// This job is used for any prefetch task.
class SrvTasks_JobCollection : public SrvTasks_Job
{
public:
    /// This creates an object (prefetchObjetcsCoordinator)
    SrvTasks_JobCollection();
    /// This creates a server which reads a blob from the given position (prefetchBlob)
    SrvTasks_JobCollection (const tgg00_FileId      &lobFileId,
                            const SAPDB_UInt         lobPosition,
                            const SAPDB_UInt         lobBytesToRead );
    /// This creates a server which reads a file from given leaf page (prefetchPages)
    SrvTasks_JobCollection (const tgg00_FileId      &fileid,
                                  tsp00_PageNo       pageNo);
    /// This creates a server which reads from a given key position (prefetchSelect)
    SrvTasks_JobCollection (const tgg00_FileId      &fileid,
                            const tgg00_Lkey        &key);
protected:
    /// This executes the job and must be overlayed
    virtual Result ExecuteIntern (Trans_Context &trans);
    /// This creates a copy of the job (including all pointers)
    /// This is used by servertasks if nobody wants to wait for the jobs end
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const;
    virtual void Deallocate() {}
private:
    void OmsReadAheadCoordinator ();
    void InitMember();
private:
    /// Used to specify a file
    tgg00_FileId      m_FileId;
    /// Used to specify a position within a lob file
    SAPDB_UInt        m_LobPosition;
    /// used to specify the number of bytes in a lob file to be read
    SAPDB_UInt        m_LobBytesToRead;
    /// Used to specify a specific page from where to read
    tsp00_PageNo      m_PageNo;
    /// USed to specify a speicific key from where to read
    tgg00_Lkey        m_Key;
};
#endif // SrvTasks_JobCollection_HPP
