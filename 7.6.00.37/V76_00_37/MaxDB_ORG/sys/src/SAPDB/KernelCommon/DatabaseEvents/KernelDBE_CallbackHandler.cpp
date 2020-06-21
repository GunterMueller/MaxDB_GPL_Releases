/*!
  \file    KernelDBE_CallbackHandler.cpp
  \author  MarcW
  \ingroup Database Events Management, parameter handling
  \brief   

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

#include "KernelCommon/DatabaseEvents/KernelDBE_CallbackHandler.hpp"
#include "Messages/Msg_List.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventDefinition.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "Converter/Converter_IEvent.hpp"
#include "Logging/Log_Volume.hpp"
#include "heo60.h"
#include "hgg01_2.h"

SAPDB_Bool KernelDBE_CallbackHandler::initializeEventCategory(
        RTEConf_Parameter::Name eventCategoryName,
        RTEConf_Parameter::Type paramType,
        tsp31_event_ident_Enum eventCategoryIndex,
        Msg_List &errOut) {

    SAPDB_Bool success(false);
    errOut.ClearMessageList();

    if( eventCategoryIndex >= sp31ei_last_but_not_used ) {
        // TODO: report an error
        success = false;
        return success;
    }

    ParamValueUnionType initialValue;
    // read the initial value
    switch( paramType ) {
        case RTEConf_Parameter::t_Integer:
            success = RTEConf_ParameterAccess::Instance()->GetInteger(
                    eventCategoryName,
                    initialValue.IntVal,
                    errOut);
            break;
        case RTEConf_Parameter::t_Real:
            success = RTEConf_ParameterAccess::Instance()->GetReal(
                    eventCategoryName,
                    initialValue.RealVal,
                    errOut);
            break;
        case RTEConf_Parameter::t_CryptInfo:
            success = RTEConf_ParameterAccess::Instance()->GetCryptInfo(
                    eventCategoryName,
                    initialValue.CryptVal,
                    errOut);
            break;
        case RTEConf_Parameter::t_String:
            RTEConf_Parameter::String initialStringValue;
            success = RTEConf_ParameterAccess::Instance()->GetString(
                    eventCategoryName,
                    initialStringValue,
                    errOut);
            if( success )
                initialValue = initialStringValue;
            break;
        default:
            success = false;
            break;
    }

    if( !success ) {
        // TODO: report an error
        return success;
    }

    // make other kernel parts aware of the new event situation
    switch( eventCategoryIndex ) {
        case sp31ei_db_filling_above_limit:
            success = updateCategoryDBFillingAboveLimit(errOut, initialValue.StringVal, NULL);
            break;
        case sp31ei_db_filling_below_limit:
            success = updateCategoryDBFillingBelowLimit(errOut, initialValue.StringVal, NULL);
            break;
        case sp31ei_log_above_limit:
            success = updateCategoryLogFillingAboveLimit(errOut, initialValue.StringVal, NULL);
            break;
        //...
        default:
            break;
    }

    // store the initial value
    m_ActiveValues[eventCategoryIndex] = initialValue;
    return success;
}


SAPDB_Bool KernelDBE_CallbackHandler::eventCategoryUpdated(
        RTEConf_Parameter::Name eventCategoryName,
        RTEConf_Parameter::Type paramType,
        void *newValue,
        void *context,
        Msg_List &errOut) {

    SAPDB_Bool success(false);
    errOut.ClearMessageList();

    // get the context
    KernelDBE_ContextProvider* contextProvider = (KernelDBE_ContextProvider*)context;
    tsp31_event_ident_Enum thisCategoryIndex = contextProvider->getContext();

    ParamValueUnionType theNewValue;
    success = theNewValue.assign(paramType, newValue);
    if( !success ) {
        // TODO: report an error
        return false;
    }

    // now we have the new value in theNewValue
    //             the old value in m_ActiveValues[thisCategoryIndex]

    // make other kernel parts aware of the new event situation
    switch( thisCategoryIndex ) {
        case sp31ei_db_filling_above_limit:
            success = updateCategoryDBFillingAboveLimit(
                    errOut,
                    theNewValue.StringVal,
                    m_ActiveValues[thisCategoryIndex].StringVal);
            break;
        case sp31ei_db_filling_below_limit:
            success = updateCategoryDBFillingBelowLimit(
                    errOut,
                    theNewValue.StringVal,
                    m_ActiveValues[thisCategoryIndex].StringVal);
            break;
        case sp31ei_log_above_limit:
            success = updateCategoryLogFillingAboveLimit(
                    errOut,
                    theNewValue.StringVal,
                    m_ActiveValues[thisCategoryIndex].StringVal);
            break;
        default:
            break;
    }

    // finally make the new value the next old value
    m_ActiveValues[thisCategoryIndex] = theNewValue;

    // send an event about the change in the events
    tsp31_event_description EventDescription;
    g01event_init (EventDescription);   
    EventDescription.sp31ed_priority.becomes(sp31ep_low);
    EventDescription.sp31ed_ident.becomes(sp31ei_event);
    EventDescription.sp31ed_text_len = 0;
    EventDescription.sp31ed_eventcnt = 0;
    EventDescription.sp31ed_value_1  = 0;
    EventDescription.sp31ed_value_2  = 0;  
    vinsert_event (EventDescription);

    return success;
}

SAPDB_Bool KernelDBE_CallbackHandler::updateCategoryDBFillingAboveLimit(
        Msg_List& errOut,
        RTEConf_Parameter::String newValue,
        RTEConf_Parameter::String oldValue) {

    // this object iterates on differences between old an new event category
    SAPDBDBE_EventCategory eventCat( errOut, (const SAPDB_Char*)newValue, (const SAPDB_Char*)oldValue);

    if( !eventCat.isValid() ) {
        // TODO: report an error
        return false;
    }

    while(eventCat.hasNext() ) {
        const SAPDBDBE_EventDefinition& eventDef(eventCat.getNext());
        switch (eventDef.getStatus()) {
            case SAPDBDBE_EventDefinition::EDS_Added : 
                Converter_IEvent::Instance().AddEvent(eventDef.getValue() * 100, true, 
                    eventDef.getPriority());
                break;
            case SAPDBDBE_EventDefinition::EDS_Modified :
                Converter_IEvent::Instance().DelEvent(eventDef.getValue() * 100, true);
                Converter_IEvent::Instance().AddEvent(eventDef.getValue() * 100, true, 
                    eventDef.getPriority());
                break;
            case SAPDBDBE_EventDefinition::EDS_Removed :
                Converter_IEvent::Instance().DelEvent(eventDef.getValue() * 100, true);
                break;
            }
        }
    return true;
}

SAPDB_Bool KernelDBE_CallbackHandler::updateCategoryDBFillingBelowLimit(
        Msg_List& errOut,
        RTEConf_Parameter::String newValue,
        RTEConf_Parameter::String oldValue) {

    // this object iterates on differences between old an new event category
    SAPDBDBE_EventCategory eventCat( errOut, (const SAPDB_Char*)newValue, (const SAPDB_Char*)oldValue);

    if( !eventCat.isValid() ) {
        // TODO: report an error
        return false;
    }

    while(eventCat.hasNext() ) {
        const SAPDBDBE_EventDefinition& eventDef(eventCat.getNext());
        switch (eventDef.getStatus()) {
            case SAPDBDBE_EventDefinition::EDS_Added : 
                Converter_IEvent::Instance().AddEvent(eventDef.getValue() * 100, false, 
                    eventDef.getPriority());
                break;
            case SAPDBDBE_EventDefinition::EDS_Modified :
                Converter_IEvent::Instance().DelEvent(eventDef.getValue() * 100, false);
                Converter_IEvent::Instance().AddEvent(eventDef.getValue() * 100, false, 
                    eventDef.getPriority());
                break;
            case SAPDBDBE_EventDefinition::EDS_Removed :
                Converter_IEvent::Instance().DelEvent(eventDef.getValue() * 100, false);
                break;
            }
    }
    return true;
}

SAPDB_Bool KernelDBE_CallbackHandler::updateCategoryLogFillingAboveLimit(
        Msg_List& errOut,
        RTEConf_Parameter::String newValue,
        RTEConf_Parameter::String oldValue) {

    SAPDBDBE_EventCategory eventCat( errOut, (const SAPDB_Char*)newValue, (const SAPDB_Char*)oldValue);

    if( !eventCat.isValid() ) {
        // TODO: report an error
        return false;
    }

    while(eventCat.hasNext() ) {
        const SAPDBDBE_EventDefinition& eventDef(eventCat.getNext());
        switch (eventDef.getStatus()) {
            case SAPDBDBE_EventDefinition::EDS_Added : 
                Log_Volume::Instance().AddLogFillingEvent(eventDef.getValue() * 100, eventDef.getPriority());
                break;
            case SAPDBDBE_EventDefinition::EDS_Modified :
                Log_Volume::Instance().DelLogFillingEvent(eventDef.getValue() * 100);
                Log_Volume::Instance().AddLogFillingEvent(eventDef.getValue() * 100, eventDef.getPriority());
                break;
            case SAPDBDBE_EventDefinition::EDS_Removed :
                Log_Volume::Instance().DelLogFillingEvent(eventDef.getValue() * 100);
                break;
            }
    }

    return true;
}


KernelDBE_CallbackHandler::KernelDBE_ContextProvider
    KernelDBE_CallbackHandler::contextProviders[sp31ei_last_but_not_used]=
        {
            KernelDBE_ContextProvider(sp31ei_nil),
            KernelDBE_ContextProvider(sp31ei_filler1),
            KernelDBE_ContextProvider(sp31ei_autosave),
            KernelDBE_ContextProvider(sp31ei_filler3),
            KernelDBE_ContextProvider(sp31ei_filler4),
            KernelDBE_ContextProvider(sp31ei_db_filling_above_limit),
            KernelDBE_ContextProvider(sp31ei_db_filling_below_limit),
            KernelDBE_ContextProvider(sp31ei_event),
            KernelDBE_ContextProvider(sp31ei_log_above_limit),
            KernelDBE_ContextProvider(sp31ei_filler9),
            KernelDBE_ContextProvider(sp31ei_filler10),
            KernelDBE_ContextProvider(sp31ei_upd_stat_wanted),
            KernelDBE_ContextProvider(sp31ei_filler12),
            KernelDBE_ContextProvider(sp31ei_error),
            KernelDBE_ContextProvider(sp31ei_admin),
            KernelDBE_ContextProvider(sp31ei_backupresult),
            KernelDBE_ContextProvider(sp31ei_checkdata),
            KernelDBE_ContextProvider(sp31ei_databasefull),
            KernelDBE_ContextProvider(sp31ei_logfull),
            KernelDBE_ContextProvider(sp31ei_online),
            KernelDBE_ContextProvider(sp31ei_outofsessions),
            KernelDBE_ContextProvider(sp31ei_standby),
            KernelDBE_ContextProvider(sp31ei_systemerror),
            KernelDBE_ContextProvider(sp31ei_logsegmentfull)
        };
      
KernelDBE_CallbackHandler::ParamValueUnionType
    KernelDBE_CallbackHandler::m_ActiveValues[sp31ei_last_but_not_used];

void KernelDBE_CallbackHandler::ParamValueUnionType::operator=(const RTEConf_Parameter::Integer& aInteger) {
    IntVal = aInteger;
}

void KernelDBE_CallbackHandler::ParamValueUnionType::operator=(const RTEConf_Parameter::Real& aReal) {
    RealVal = aReal;
}

void KernelDBE_CallbackHandler::ParamValueUnionType::operator=(const RTEConf_Parameter::String aString) {
    STRCPY_UTF8((SAPDB_Char*)StringVal, (SAPDB_Char*)aString);
}

void KernelDBE_CallbackHandler::ParamValueUnionType::operator=(const RTEConf_Parameter::CryptInfo aCrypt) {
    SAPDB_memcpy(CryptVal, aCrypt, sizeof(RTEConf_Parameter::CryptInfo));
}

SAPDB_Bool KernelDBE_CallbackHandler::ParamValueUnionType::assign(
        const RTEConf_Parameter::Type aType,
        const void* aNewValue ) {

    switch( aType ) {
        case RTEConf_Parameter::t_Integer:
            *this = *((const RTEConf_Parameter::Integer*) aNewValue);
            break;
        case RTEConf_Parameter::t_Real:
            *this = *((const RTEConf_Parameter::Real*) aNewValue);
            break;
        case RTEConf_Parameter::t_CryptInfo:
            *this = *((const RTEConf_Parameter::CryptInfo*) aNewValue);
            break;
        case RTEConf_Parameter::t_String:
            *this = (RTEConf_Parameter::String) aNewValue;
            break;
        default:
            // TODO: report an error
            return false;
    }
    return true;
}