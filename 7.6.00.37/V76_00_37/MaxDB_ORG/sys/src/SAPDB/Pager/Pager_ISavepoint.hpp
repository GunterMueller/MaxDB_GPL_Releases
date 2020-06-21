/*!
  @file           Pager_ISavepoint.hpp
  @author         Henrik
  @brief          Interface for Pager instantiation and savepoint control
  @see            


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



*/



#ifndef PAGER_ISAVEPOINT_HPP
#define PAGER_ISAVEPOINT_HPP

class Pager_ISavepoint;

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          Pager_ISavepoint
  @brief          Interface for Pager instantiation and savepoint control
*/
class Pager_ISavepoint {
public:
    // singleton

    /*!
       @brief          returns instance of Pager_ISavepoint/Pager_Controller. If
                       no instance exists a new one will be created
       @return         (new) singleton instance of Pager_ISavepoint

     */

    static Pager_ISavepoint& GetInstance();

    /*!
       @brief          initiate Savepoint,
                       wait for running pager_writer to finish
       @param          taskId [in] own taskID
       @return         none

     */

    virtual void BeginSavepoint (const tsp00_TaskId taskId) = 0;

    /*!
       @brief          flushes DataCache to disk
       @param          taskId           [in] own taskID
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions    [out] number of clustered I/O transactions
       @return         none

     */

    virtual void FlushDataCacheBeforePrepare (
        const tsp00_TaskId taskId,
        SAPDB_UInt         &numPagesWritten,
        SAPDB_UInt         &numIOActions) = 0;

    /*!
       @brief          loops as long as the last operation which is synchronized
                       with the savepoint has finished.
       @param          taskId           [in] own taskID
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions    [out] number of clustered I/O transactions
       @return         none

     */

    virtual void WaitForLastSvpSyncOperation (
        const tsp00_TaskId taskId,
        SAPDB_UInt         &numPagesWritten,
        SAPDB_UInt         &numIOActions) = 0;

    /*!
       @brief          flushes DataCache to disk
       @param          taskId           [in] own taskID
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions    [out] number of clustered I/O transactions
       @return         none

     */

    virtual void FlushDataCacheAfterPrepare (
        const tsp00_TaskId taskId,
        SAPDB_UInt         &numPagesWritten,
        SAPDB_UInt         &numIOActions) = 0;

    /*!
       @brief          triggers parallel flush of converter segments
       @param          taskId           [in] own taskId
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions    [out] number of clustered I/O transactions
       @return         none

     */

    virtual void FlushConverterParallel (
        const tsp00_TaskId taskId,
        SAPDB_UInt         &numPagesWritten,
        SAPDB_UInt         &numIOActions) = 0;

    /*!
       @brief          Savepoint finished,
       @param          taskId [in] own taskID
       @return         none
     */

    virtual void EndSavepoint (
        const tsp00_TaskId taskId) = 0;

	/*!
		@brief          Savepoint finished, disable Pager_Writer
		@param          taskId [in] own taskID
		@return         none
	  */
	virtual void EndSavepointForShutdown (
			const tsp00_TaskId taskId) = 0;
			
};

#endif  /* PAGER_ISAVEPOINT_HPP */


