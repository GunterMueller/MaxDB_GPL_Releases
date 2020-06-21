/*!
  \file    DBMSrvCmd_SilentDispatcherUsingCommand.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   base class for event administration commands

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

#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp"


DBMSrvCmd_EventAdministrationCommand::DBMSrvCmd_EventAdministrationCommand(
        CommandKey commandKey,
        bool isObsolete,
        const char* aLongHelp,
        bool isHidden)
        : DBMSrv_Command( commandKey, isObsolete, aLongHelp, isHidden ) {
}

tcn00_Error DBMSrvCmd_EventAdministrationCommand::run(
                    VControlDataT* vcontrol,
                    CommandT* command,
                    DBMSrv_Reply& theReply) {
    tcn00_Error rc(OK_CN00);
    theReply.startWithOK();

    // create lock object
    DBMSrvMsg_Error lockError;

    DBMSrvShM_LineAccessorResourceLock eventAdminLock(DBMSrvShM_InternalResource::EventAdministration, lockError);
    if( !lockError.IsEmpty() )
        return theReply.startWithMessageList(lockError);

    // now try to lock resource "event administration"
    DBMSrvShM_LineAccessorResourceLock::LockState lockState;
    lockState = eventAdminLock.lock(lockError);
    
    if( DBMSrvShM_LineAccessorResourceLock::LockStateLocked != lockState )
        return theReply.startWithMessageList(lockError);

    // resource is locked here

    // get kernel parameter access
    DBMSrvMsg_Error oParamError;
    DBMSrvPar_Parameters* kernelParameters = DBMSrvPar_Parameters::GetInstance(vcontrol->dbname, true, oParamError);
    if( kernelParameters != NULL ) {
        // let derived class do the work
        rc = runEventAdministrationCommand(vcontrol, kernelParameters, command, theReply);
        // release kernel parameter access
        delete kernelParameters;
    }

    // try to unlock resource
    lockState = eventAdminLock.unlock(lockError);
    if( DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked != lockState )
        oParamError.Overrule(lockError);

    if( !oParamError.IsEmpty() )
        rc = theReply.startWithMessageList(oParamError);

    return rc;
}
