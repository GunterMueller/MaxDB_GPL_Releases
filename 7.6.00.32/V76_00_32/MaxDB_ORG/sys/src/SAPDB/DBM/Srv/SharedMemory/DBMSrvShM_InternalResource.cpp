/*! 
  \file    DBMSrvShM_InternalResource.cpp
  \author  MarcW
  \ingroup common classes for the DBMServer
  \brief   container for names of resources, of which access must be synchronized

\if EMIT_LICENCE

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


\endif
*/

#include "DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp"

const char* DBMSrvShM_InternalResource::AddVolume = "AddVolume";
const char* DBMSrvShM_InternalResource::EventAdministration = "EventAdministration";
const char* DBMSrvShM_InternalResource::SharedMemory = "SharedMemory";
const char* DBMSrvShM_InternalResource::SchedulerCommunication = "SchedCom";
const char* DBMSrvShM_InternalResource::SilentEventDispatcherCommunication = "SilEvtDispCom";
const char* DBMSrvShM_InternalResource::DBM_CommandLogFile ="DBM_CommandLogFile";
const char* DBMSrvShM_InternalResource::ParameterFileAccess = "ParamAccess";