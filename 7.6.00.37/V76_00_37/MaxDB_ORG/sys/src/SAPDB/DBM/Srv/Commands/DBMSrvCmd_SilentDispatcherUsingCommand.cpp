/*!
  \file    DBMSrvCmd_SilentDispatcherUsingCommand.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   definition of classes for handling DBMServer
           dealing with the autoextender

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

#include "hcn42.h"
#include "hcn50.h"
#include "hcn51.h"

#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp"
#include "DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoExtend.hpp"
#include "DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoUpdateStatistics.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp"
#include "DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineIteratorEventing.hpp"

#include "RunTime/System/RTESys_ProcessState.hpp"

#define SLEEP_MAX             300000 // milli seconds
#define SLEEP_FIRST_INTERVALL    600 // milli seconds
#define SLEEP_MULTIPLIER           2


DBMSrvCmd_SilentDispatcherUsingCommand::DBMSrvCmd_SilentDispatcherUsingCommand(
        CommandKey commandKey,
        const char* longHelp )
        : DBMSrv_Command( commandKey, false, longHelp),
          m_UnprotectedBeforeOK(false),
          m_UnprotectedAfterOK(false),
          m_ExecuteUnprotectedAfter(false)
{
    cn42GetFileName(cn00DBMServerData::vcontrol()->dbname, FGET_SEVTDISPCFG_CN42, m_Configfile);
}

tcn00_Error DBMSrvCmd_SilentDispatcherUsingCommand::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    DBMSrvMsg_Error beforeError;
    tcn00_Error rc(runUnProtectedBefore(vcontrol, command, beforeError));
    
    if( rc == OK_CN00 )
        theReply.startWithOK();
    else
        return rc;

    // lock dispatcher communication
    DBMSrvMsg_Error lockError;
    DBMSrvShM_LineAccessorResourceLock communicationLock(
        DBMSrvShM_InternalResource::SilentEventDispatcherCommunication, lockError);

    if( !lockError.IsEmpty() )
        return theReply.startWithMessageList(lockError);

    DBMSrvShM_LineAccessorResourceLock::LockState lockState;
    lockState = communicationLock.lock(lockError);

    if( DBMSrvShM_LineAccessorResourceLock::LockStateLocked != lockState )
        return theReply.startWithMessageList(lockError);

    // resource is locked for us here

    m_SilentDispatcherProps.setFileName(m_Configfile.asCharp());

    DBMSrvMsg_Error loadError;
    m_SilentDispatcherProps.load(loadError); // we ignore the load error here

    tcn00_Error rcImpl(OK_CN00);
    rcImpl = runLockProtected( vcontrol, command, theReply);

    lockState = communicationLock.unlock(lockError);
    if( DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked != lockState )
        rc = theReply.startWithMessageList(lockError);

    if( m_ExecuteUnprotectedAfter ) {
        DBMSrvMsg_Error afterError;
        rcImpl = runUnProtectedAfter(vcontrol, command, afterError);
        if( !afterError.IsEmpty() )
            theReply.startWithMessageList(afterError);
    }

    if( rcImpl == OK_CN00 && rc == OK_CN00 )
        return OK_CN00;
    else if( rcImpl != OK_CN00 )
        return rcImpl;
    else
        return rc;
}

tcn00_Error DBMSrvCmd_SilentDispatcherUsingCommand::runLockProtected(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    DBMSrvMsg_Error errOut;
    theReply.startWithOK();
  
    if( command->oArguments.getCount() != 1 )
        // return silently
        return OK_CN00;

    if( 0 == strcmp(command->oArguments.getValue(1).CharPtr(), Start) ) {
        // we're called because kernel will leave offline state
        bool dispatcherUp = isDispatcherUp(errOut);
        if( !errOut.IsEmpty() )
            return theReply.startWithMessageList(errOut);

        if( !dispatcherUp ) {
            bool mustStart(migrateDispatcherConfiguration(vcontrol, theReply, errOut));
            if( mustStart && errOut.IsEmpty() ) {
                startDispatcher(errOut);
            }
        }
    }
    else if( 0 == strcmp(command->oArguments.getValue(1).CharPtr(), StopAndDelete) ) {
        // we're called from db_drop command
        if( stopDispatcher(errOut) ) {
            // the dispatcher is stopped successfully
            // we can delete the configuration file
            deleteConfigurationFile(errOut);
        }
    }
    else if( 0 == strcmp(command->oArguments.getValue(1).CharPtr(), StopAll) ) {
        // kernel went offline
        stopDispatcher(errOut);
    }

    if( errOut.IsEmpty() )
        return OK_CN00;
    else
        return theReply.startWithMessageList(errOut);
}

bool DBMSrvCmd_SilentDispatcherUsingCommand::migrateDispatcherConfiguration(
        VControlDataT* vcontrol,
        DBMSrv_Reply& theReply,
        DBMSrvMsg_Error& errOut) {

    // check if event dispatcher must be started
    cn51DBMConfigValue  cfgRunDispatcher (vcontrol->dbname, CFG_RUNDISPATCHERFLAG_CN51);

    bool runEvtDisp(false);
    runEvtDisp = stricmp(cfgRunDispatcher, "yes") == 0;
    
    // backward compatibility
    cn51DBMConfigValue  cfgAutoExt (vcontrol->dbname, CFG_AUTOEXTFLAG_CN51);
    if( stricmp(cfgAutoExt, "yes")== 0 ) {
        tsp00_C512c newval;
        newval.rawAssign("no");
        cfgAutoExt = newval;
        newval.rawAssign("yes");
        cfgRunDispatcher = newval;
        runEvtDisp = true;
    }

    if( !runEvtDisp )
        return false;

    tni34_AVirtualFile newConfigFile(
            m_Configfile.asCharp(),
            tni34_AVirtualFile::FromDisk,
            tni34_AVirtualFile::DoNotOpenNow);

    if( newConfigFile.IsGood() && !newConfigFile.Exists() ) {
        // a new file does not exist yes

        DBMCliEvtDisp_Properties newProps(Tools_DynamicUTF8String(m_Configfile.asCharp()));

        tsp00_Pathc oldConfigfileName;
        cn42GetFileName(cn00DBMServerData::vcontrol()->dbname, FGET_EVTDISPCONF_CN42, oldConfigfileName);
        tni34_AVirtualFile oldConfigFile(
                oldConfigfileName.asCharp(),
                tni34_AVirtualFile::FromDisk,
                tni34_AVirtualFile::DoNotOpenNow);
        Tools_Properties oldProps(Tools_DynamicUTF8String(oldConfigfileName.asCharp()));
        if( oldConfigFile.Exists() )
            oldProps.load(errOut);
        if( oldProps.isEmpty() ) {
            // no configuration in rundirectory, check the env directory
            tsp00_Pathc oldDefaultConfigfileName;
            cn42GetFileName(cn00DBMServerData::vcontrol()->dbname, FGET_EVTDISPCDEF_CN42, oldDefaultConfigfileName);
            tni34_AVirtualFile oldDefaultConfigFile(
                    oldDefaultConfigfileName.asCharp(),
                    tni34_AVirtualFile::FromDisk,
                    tni34_AVirtualFile::DoNotOpenNow);
            if( oldDefaultConfigFile.Exists() )
                oldProps.load(errOut, Tools_DynamicUTF8String(oldDefaultConfigfileName.asCharp()));

            if( oldProps.isEmpty() ) {
                tsp00_C512c no;
                no.rawAssign("no");
                cfgRunDispatcher = no;
                runEvtDisp = false;
            }
        }

        if( !oldProps.isEmpty() ) {
            // auto extend
            Tools_DynamicUTF8String autoExtProp(
                    oldProps.getProperty(Tools_DynamicUTF8String("DB_ABOVE_LIMIT")));
            if( autoExtProp.Length() > 0 ) {
                // transfer entry
                Tools_DynamicUTF8String newExtendEvent;
                Tools_DynamicUTF8String autoExtendCondition;
                Tools_DynamicUTF8String autoExtendHandler;
                getEventCategoryNameFromID( sp31ei_db_filling_above_limit, theReply, newExtendEvent);
                DBMSrvCmdAuto_AutoExtend::buildConfigLine(
                        newExtendEvent,
                        DBMSrvCmdAuto_AutoExtend::m_PercentageDefault,
                        autoExtendCondition,
                        autoExtendHandler);

                newProps.setProperty(autoExtendCondition, autoExtendHandler);
            }
            // update statistics
            Tools_DynamicUTF8String updStatProp(
                    oldProps.getProperty(Tools_DynamicUTF8String("UPDATE_STATISTICS")));
            if( updStatProp.Length() > 0 ) {
                // transfer entry
                Tools_DynamicUTF8String newUpdStatEvent;
                Tools_DynamicUTF8String updStatCondition;
                Tools_DynamicUTF8String updStatHandler;
                getEventCategoryNameFromID( sp31ei_upd_stat_wanted, theReply, newUpdStatEvent);
                DBMSrvCmdAuto_AutoUpdateStatistics::buildConfigLine(
                        newUpdStatEvent,
                        updStatCondition,
                        updStatHandler);

                newProps.setProperty(updStatCondition, updStatHandler);
            }
            
            // add version number
            bool mustStore(false);
            if( !newProps.isEmpty() ) {
                
                newProps.setProperty(
                        Tools_DynamicUTF8String(DBMCliEvtDisp_Properties::FileVersionKey),
                        Tools_DynamicUTF8String("7.6.00.04"));
                mustStore = true;
            }            
            
            // store the transferred lines if necessary
            if( !mustStore || !newProps.store(errOut) ) {
                tsp00_C512c no;
                no.rawAssign("no");
                cfgRunDispatcher = no;
                runEvtDisp = false;
            }
        }        
        
        // delete old file, do not delete default configuration file in env folder, because there might
        // be other databases that use it and that are still to be migrated
        oldConfigFile.Remove();
    }

    return runEvtDisp;
}

void DBMSrvCmd_SilentDispatcherUsingCommand::getEventHandler(
    const Tools_DynamicUTF8String& aEventName,
    Tools_DynamicUTF8String& outKey,
    Tools_DynamicUTF8String& outValue) {
    
    outKey.Erase();
    outValue.Erase();

    Tools_List<Tools_Property*>* allLines = m_SilentDispatcherProps.getAllProperties();
    if( allLines != NULL ) {
        Tools_List<Tools_Property*>::iterator it(allLines->begin());
        while( it != allLines->end() ) {
            Tools_DynamicUTF8String key((*it)->getKey());
            if( key.Find(aEventName) != Tools_DynamicUTF8String::NPos ) {
                outKey = key;
                outValue = (*it)->getValue();
                delete *it;
            }
            ++it;
        }
        delete allLines;
    }
}

bool DBMSrvCmd_SilentDispatcherUsingCommand::isDispatcherUp(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    DBMSrvShM_LineIteratorEventing internalDispatchers(errOut, true);

    if( !internalDispatchers.isValid() )
        return false;

    int linesFound(0);
    int dispatcherPid(0);
    int instanceID(0);
    while( internalDispatchers.hasNext() ) {
        ++linesFound;
        dispatcherPid = internalDispatchers.getDispatcherProcessID();
        instanceID = internalDispatchers.getInstanceID();
        internalDispatchers.next();
    }

    if( linesFound != 1 )
        return false;

    RTE_OSPid dispatcherOSPid(dispatcherPid);
    RTESys_ProcessState procState(dispatcherOSPid);
    bool isUp(procState.State() == RTESys_ProcessState::Alive);
    if( !isUp ) {
        // there is an internal dispatcher line, but the dispatcher is not up, so we remove the line
        DBMSrvShM_LineAccessorEventing internalDispatcher(instanceID, errOut);
        if( errOut.IsEmpty() )
            internalDispatcher.removeLineEventing(errOut);
    }
    return isUp;
}

bool DBMSrvCmd_SilentDispatcherUsingCommand::stopDispatcher(DBMSrvMsg_Error& theErrList) {

    theErrList.ClearMessageList();
    bool dispatcherUp(isDispatcherUp(theErrList));
    if( !theErrList.IsEmpty() ) {
        theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODISPATCHERSTOP));
        return false;
    }

    if( !dispatcherUp ) {
        if( m_SilentDispatcherProps.getCount()==1 )
            return deleteConfigurationFile( theErrList );
        else
            return true;
    } // end if

    DBMSrvShM_LineIteratorEventing internalDispatchers(theErrList, true);
    if( !internalDispatchers.isValid() ) {
        theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODISPATCHERSTOP));
        return false;
    }

    int linesFound(0);
    int instanceID(-1);
    while( internalDispatchers.hasNext() ) {
        ++linesFound;
        instanceID = internalDispatchers.getInstanceID();
        internalDispatchers.next();
    }

    if( linesFound > 1 ) {
        theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODISPATCHERSTOP));
        return false;
    }

    if( linesFound == 0 )
        return true;

    DBMSrvShM_LineAccessorEventing theInternalDispatcher(instanceID, theErrList);
    if( !theErrList.IsEmpty() || !theInternalDispatcher.doCancel(theErrList) ) {
        theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODISPATCHERSTOP));
        return false;
    }

    // now wait until the line is removed...
    SAPDB_UInt4 nSleepTime       = 0;
    SAPDB_UInt4 nSleepIntervall  = SLEEP_FIRST_INTERVALL;
    while( nSleepTime < SLEEP_MAX && theInternalDispatcher.existsEventing(theErrList) ) {
        if( !theErrList.IsEmpty() ) {
            theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODISPATCHERSTOP));
            return false;
        }
        RTE_ISystem::DoSleep(nSleepIntervall);
        nSleepTime += nSleepIntervall;
        nSleepIntervall *= SLEEP_MULTIPLIER;
    }

    if( theInternalDispatcher.existsEventing(theErrList) ) {
        theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODISPATCHERSTOP));
        return false;
    }

    if( m_SilentDispatcherProps.getCount()==1 )
        return deleteConfigurationFile( theErrList );
    else
        return true;
}

bool DBMSrvCmd_SilentDispatcherUsingCommand::deleteConfigurationFile(DBMSrvMsg_Error& errOut) {
    // we delete the configuration file
    if( m_SilentDispatcherProps.deleteStorage(errOut) ) {
        // write dbm configuration
        cn51DBMConfigValue cfgRunDispatcher(
                cn00DBMServerData::vcontrol()->dbname, 
                CFG_RUNDISPATCHERFLAG_CN51);
        tsp00_C512c no;
        no.rawAssign("no");
        cfgRunDispatcher = no;
        return true;
    }
    else
        return false;
}

bool DBMSrvCmd_SilentDispatcherUsingCommand::isConfiguredToRun(DBMSrvMsg_Error& errOut) {
    cn51DBMConfigValue cfgRunDispatcher(
            cn00DBMServerData::vcontrol()->dbname, 
            CFG_RUNDISPATCHERFLAG_CN51);
    
    return 0 == strcmp("yes", cfgRunDispatcher);
}

bool DBMSrvCmd_SilentDispatcherUsingCommand::startDispatcher(DBMSrvMsg_Error& theErrList) {

    theErrList.ClearMessageList();
    bool dispatcherUp(isDispatcherUp(theErrList));
    
    if( !theErrList.IsEmpty() )
        return false;

    if( dispatcherUp )
        return true;

    char upwdBuffer[256];
    DBMSrvProc_ArgumentList theArgList;

    // find the dbm super user
    cn50DBMUser superUser(
        cn00DBMServerData::vcontrol()->dbname,
        cn50DBMUser::getColdUser(cn00DBMServerData::vcontrol()->dbname));

    // build command line
    theArgList.AddArgument("start");
    theArgList.AddArgument("-internal");
    theArgList.AddArgument(m_Configfile.asCharp());
    theArgList.AddArgument("-l");
    tsp00_Pathc logFile;
    cn42GetFileName(cn00DBMServerData::vcontrol()->dbname, FGET_EVTDISPPRT_CN42, logFile);
    theArgList.AddArgument(logFile);
    theArgList.AddArgument("-d");
    theArgList.AddArgument(cn00DBMServerData::vcontrol()->dbname.asCharp());
    theArgList.AddArgument("-u");
    sprintf(upwdBuffer, "%s,%s",
            superUser.getUserName().asCharp(),
            superUser.getClearMasterPwd().asCharp());
    theArgList.AddArgument(upwdBuffer);

    DBMSrvProc_ServerProcess dispatcherProc(
        DBMSrvProc_ServerProcessCommandLineConverter::KeyEventDispatcherProgram,
        theArgList);

    if( dispatcherProc.started() ) {
        SAPDB_UInt4 nSleepTime       = 0;
        SAPDB_UInt4 nSleepIntervall  = SLEEP_FIRST_INTERVALL;
        while( nSleepTime < SLEEP_MAX && !isDispatcherUp(theErrList) ) {
            if( !theErrList.IsEmpty() )
                return false;
            RTE_ISystem::DoSleep(nSleepIntervall);
            nSleepTime += nSleepIntervall;
            nSleepIntervall *= SLEEP_MULTIPLIER;
        }

        dispatcherUp = isDispatcherUp(theErrList);
        if( !theErrList.IsEmpty() )
            return false;

        if( dispatcherUp ) {
            // write dbm configuration
            cn51DBMConfigValue cfgRunDispatcher(
                    cn00DBMServerData::vcontrol()->dbname, 
                    CFG_RUNDISPATCHERFLAG_CN51);
            tsp00_C512c yes;
            yes.rawAssign("yes");
            cfgRunDispatcher = yes;
            return true;
        }
        else {
            dispatcherProc.getMessagesInto(theErrList);
            theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODISPATCHERSTART));
            return false;
        }
    }
    else {
        dispatcherProc.getMessagesInto(theErrList);
        theErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EXECUTE));
        return false;
    }
}

tcn00_Error DBMSrvCmd_SilentDispatcherUsingCommand::getEventCategoryNameFromID(
        const tsp31_event_ident_Enum eventID,
        DBMSrv_Reply& theReply,
        Tools_DynamicUTF8String& eventCategoryName ) {

    eventCategoryName = DBMSrvCmd_EventAdministrationCommand::m_EventCategoryUnknown;
            
    DBMSrvMsg_Error oParamError;
    DBMSrvPar_Parameters* kernelParameters =
        DBMSrvPar_Parameters::GetInstance(cn00DBMServerData::vcontrol()->dbname, true, oParamError);

    if( kernelParameters == NULL ) {
        return theReply.startWithMessageList(oParamError);
    }

    DBMSrvPar_ClassIterator classEventsIterator(kernelParameters, DBMSrvPar_Parameter::XPClassValueEvents);
    while( *classEventsIterator != NULL ) {
        DBMSrvPar_XPLine* helpLine((*classEventsIterator)->pHelp);
        if( helpLine != NULL && helpLine->Line() != NULL ) {
            tsp31_event_ident_Enum currentID = (tsp31_event_ident_Enum) atoi(helpLine->Line());
            if( currentID == eventID ) {
                eventCategoryName = (*classEventsIterator)->szParameterName.asCharp();
                break;
            }
        }
        ++classEventsIterator;
    }

    delete kernelParameters;
    return OK_CN00;
}

const char* const DBMSrvCmd_SilentDispatcherUsingCommand::StopAndDelete("stopAndDelete");
const char* const DBMSrvCmd_SilentDispatcherUsingCommand::StopAll("stopAll");
const char* const DBMSrvCmd_SilentDispatcherUsingCommand::Start("start");
