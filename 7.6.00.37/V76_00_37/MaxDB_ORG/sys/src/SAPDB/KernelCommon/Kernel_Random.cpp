/*!*********************************************************************

    @file         Kernel_Random.cpp

    @author       DanielD

    @brief        pseudo random numbers in presence of kernel tasking

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
***********************************************************************/

#include "KernelCommon/Kernel_Random.hpp"

#include <string.h>

#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_IWorkerJob.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "heo55k.h"
#include "hgg08.h"

/*----------------------------------------*/

/*!

  @brief random number generators used by kernel

 */
static RTECrypto_RNG *SysRNG = 0;
static RTECrypto_RNG *SHA1PRNG = 0;

/*!

  @class GetRandomData

 */
class GetRandomData
    : public RTETask_IWorkerJob
{
public:
    GetRandomData (RTE_TaskId taskid, void * buf, int bufsize);
    virtual RTE_TaskId const GetTaskId() const;
    virtual void DoJob();
    bool GetNextBlock ();

private:
    RTE_TaskId taskid;
    void     * buf;
    int        bufsize;
    bool       ok;
};

/*----------------------------------------*/

GetRandomData::
GetRandomData (
    RTE_TaskId taskid_init,
    void * buf_init,
    int bufsize_init)
{
    if (taskid_init == 0) {
        RTETask_ITask * thisTask = RTETask_ITask::Self ();
        taskid_init = thisTask->ID ();
    }
    this->taskid = taskid_init;
    this->buf = buf_init;
    this->bufsize = bufsize_init;
}

/*----------------------------------------*/

RTE_TaskId const GetRandomData::
GetTaskId() const
{
    return this->taskid;
}

/*----------------------------------------*/

void GetRandomData::
DoJob()
{
	if( SysRNG == 0 )
		SysRNG = RTECrypto_RNG::createInstance( RTECrypto_RNG::SysRNG, RTEMem_RteAllocator::Instance( ));

	if( SHA1PRNG == 0 )
		SHA1PRNG = RTECrypto_RNG::createInstance( RTECrypto_RNG::SHA1PRNG, RTEMem_RteAllocator::Instance( ));

	this->ok = SysRNG->nextBytes( this->buf, this->bufsize );
	if( this->ok )
		return;

	this->ok = SHA1PRNG->nextBytes( this->buf, this->bufsize );
}

/*----------------------------------------*/

bool GetRandomData::
GetNextBlock ()
{
    this->ok = true;
    RTETask_CallWorker (*this);
    return this->ok;
}

/*----------------------------------------*/

/*!

  @class Region

 */
class Region
{
public:
    Region (RTE_TaskId taskid);
    ~Region ();
private:
    RTE_TaskId taskid;
};

/*----------------------------------------*/

Region::
Region (
    RTE_TaskId taskid_init)
{
    this->taskid = taskid_init;
    vbegexcl (taskid_init, g08RandomGenerator);
}

/*----------------------------------------*/

Region::
~Region ()
{
    vendexcl (this->taskid, g08RandomGenerator);
}

/*----------------------------------------*/


Kernel_Random::
Kernel_Random ()
: initialized (false)
{
}

/*----------------------------------------*/

bool Kernel_Random::
nextBytes (
    void * buf,
    int buflen)
{
    RTE_TaskId taskid = RTETask_ITask::Self ()->ID ();
    Region region (taskid);

    if (!initialized) {
        if (!this->initialize ()) {
            return false;
        }
    }
    GetRandomData job (taskid, this->data, sizeof (this->data));
    unsigned char * target = (unsigned char *) buf;
    int bytesRemaining = buflen;
    /*
     * get lock
     */
    /*
     * generate data
     */
    while (bytesRemaining > 0) {
        /*
         * use cached random data
         */
        int bytesAvailable = (int) (this->readend - this->readpos);
        if (bytesAvailable > 0) {
            int bytesToCopy;
            if (bytesRemaining > bytesAvailable) {
                bytesToCopy = bytesAvailable;
            }
            else {
                bytesToCopy = bytesRemaining;
            }
            memcpy (target, this->readpos, bytesToCopy);
            target += bytesToCopy;
            this->readpos += bytesToCopy;
            bytesRemaining -= bytesToCopy;
            if (bytesRemaining <= 0) {
                return true;
            }
        }
        /*
         * get more data
         */
        if (!job.GetNextBlock ()) {
            return false;
        }
        this->readpos = this->data;
    }
    /*
     * release lock
     */
    return true;
}

/*----------------------------------------*/

bool Kernel_Random::
initialize ()
{
    // TODO: init lock
    this->readend = this->data + sizeof (this->data);
    this->readpos = this->readend; // so that first read must fill data
    this->initialized = true;
    return true;
}

/*----------------------------------------*/

Kernel_Random Kernel_Random::Generator;
