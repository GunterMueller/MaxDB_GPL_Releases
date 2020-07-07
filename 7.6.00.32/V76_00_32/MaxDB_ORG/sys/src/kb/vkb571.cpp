/*!**************************************************************************
    module:       vkb571.cpp
    author:       UweH, JuergenA
    special area: Logging
    description:  KB_RefreshSurrogSeq_interface
    last changed: 2001-04-19
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
#include "Logging/Log_ActionRefreshSurrogSeq.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "hkb571.h" // check of PASCAL header vkb571
//----------------------------------------------------------------------------
externPascal void kb571GetOldestCompatibleVersionString (
    tsp00_C10             VAR_ARRAY_REF  version)
{
    Kernel_Version oldestCompatibleVersion = Kernel_Migration::GetOldestCompatibleVersion();
    version[0] = oldestCompatibleVersion.MajorVersion() + 48;
    version[1] = '.';
    version[2] = oldestCompatibleVersion.MinorVersion() + 48;
    version[3] = '.';
    version[4] = oldestCompatibleVersion.CorrectionLevel() + 48;
    version[5] = '.';
    version[6] = oldestCompatibleVersion.BuildNo() + 48;
    version[7] = ' ';
    version[8] = ' ';
    version[9] = ' ';
}
//----------------------------------------------------------------------------
externPascal void kb571WriteRefreshSeqAfterImage (tgg00_TransContext &TransContext,
                                                  tsp00_Int4          SequenceCount,
                                                  const tsp00_Addr    pSequenceList)
                                                  
{
    Log_ActionRefreshSequence AfterImage (SequenceCount, pSequenceList);
    AfterImage.WriteAfterImage (TransContext);
}
//----------------------------------------------------------------------------
externPascal void kb571WriteRefreshSurrogAfterImage (tgg00_TransContext    &TransContext,
                                                     const tgg00_Surrogate &Surrogate,
                                                     const tgg00_Surrogate &SysKey)
{
    Log_ActionRefreshSurrogate AfterImage (Surrogate, SysKey);
    AfterImage.WriteAfterImage (TransContext);
}
//----------------------------------------------------------------------------
Kernel_TaskWaitList kb571SurrogateFlushWaitList;
//----------------------------------------------------------------------------
externPascal void kb571InitializeWaitList ()
{
	// necessary as long as we have a C++ Main so the compiler generates
    // code to call the constructors of global variables.
    kb571SurrogateFlushWaitList = Kernel_TaskWaitList();
}
//----------------------------------------------------------------------------
externPascal void kb571AllSurrogateWaitsResume ()
{
    kb571SurrogateFlushWaitList.ResumeAll();
}
//----------------------------------------------------------------------------
externPascal void kb571SurrogateWaitInsert (
    tsp00_TaskId                         taskid,
    tgg00_WaitContext     VAR_VALUE_REF  waitcontext)
{
    kb571SurrogateFlushWaitList.InsertAsLast(taskid, waitcontext);
}
