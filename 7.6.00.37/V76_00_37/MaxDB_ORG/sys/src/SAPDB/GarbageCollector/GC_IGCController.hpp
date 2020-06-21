/*!
  @defgroup       GarbageColletion Remove history information not needed anymore by any transaction 
*/
/*!
  @ingroup        GarbageColletion
  @file           GC_IGCController.hpp
  @author         FerdiF
  @brief          defines an interface to the garbage collection control structur
*/
/*
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

*/

#ifndef GC_IGCONTROLLER_HPP
#define GC_IGCONTROLLER_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"

/*!
  @interface      GC_IGarbageCollector
  @brief          Interface to garbage collector
*/
class GC_IGCController
{
public:   
    /*!
    @brief  access GC_IGCController interface
    @returns pointer to interface
    */
    static GC_IGCController& Instance();

    /*!
    @brief  trigger garbage collection
    @param  TaskId           [in] task identification
    @param  withHighPriority [in] true   GCs are started immediately
                                  false  GCs are started after its defined 
                                         waiting space of time elapsed
    */
    virtual void TriggerGarbageCollection( 
        tsp00_TaskId   TaskId, 
        bool withHighPriority ) = 0;

    /*!
    @brief          register history file for garbage collection
    @param Trans         [in] transaction context 
    @param HistoryFileNo [in] file id of history file to be registered 
    */
    virtual void RegisterHistoryFile(
        tgg00_TransContext &Trans, 
        SAPDB_UInt4         HistoryFileNo) = 0;

    /*!
    @brief          clear up garbage collectiot context
    @param Trans    [in] transaction context 
    */
    virtual void ClearAll() = 0;

    /*!
    @brief          This method delivers a timestamp delivered by RTESys_Time() 
    @returns        timestamp
    */
    virtual SAPDB_UInt8 GetGCLastTimeStarted() = 0;
};

#endif
