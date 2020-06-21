/*!
  \file    DBMSrvScd_Plan.cpp
  \author  MarcW
  \brief   plan implementation

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp"
#include "SAPDBCommon/SAPDB_string.h"

const char DBMSrvScd_Plan::SEPCHAR = '_';
time_t DBMSrvScd_Plan::m_StartTime(0);
bool DBMSrvScd_Plan::m_StartTimeSet(false);

DBMSrvScd_Plan::DBMSrvScd_Plan( const struct tm aPlannedTimeOfDay ) {

    setStartTime();

    resetStruct(&m_LastExecutionTime);
    resetStruct(&m_PlannedExecutionTime);
    m_PlannedExecutionTime.tm_hour = aPlannedTimeOfDay.tm_hour;
    m_PlannedExecutionTime.tm_min = aPlannedTimeOfDay.tm_min;
    m_PlannedExecutionTime.tm_sec = aPlannedTimeOfDay.tm_sec;
    initPlannedExecutionTime();
}

DBMSrvScd_Plan::DBMSrvScd_Plan( const char* aPlanDescription ) {

    setStartTime();

    resetStruct(&m_LastExecutionTime);
    resetStruct(&m_PlannedExecutionTime);
    if( aPlanDescription != NULL ) {
        const char* startOfLast = deserialize(aPlanDescription, &m_PlannedExecutionTime);
        deserialize(startOfLast, &m_LastExecutionTime);
        initPlannedExecutionTime();
    }
}

DBMSrvScd_Plan::DBMSrvScd_Plan( const DBMSrvScd_Plan& otherPlan )
        : m_LastExecutionTime( otherPlan.m_LastExecutionTime ), 
          m_PlannedExecutionTime( otherPlan.m_PlannedExecutionTime ) {
    setStartTime();
}

void DBMSrvScd_Plan::setStartTime() {
    if( m_StartTimeSet )
        return;
    else {
        m_StartTimeSet = true;
        time(&m_StartTime);
    }
}

void DBMSrvScd_Plan::initPlannedExecutionTime() {

    time_t now;
    time(&now);
    struct tm* nowStruct = localtime(&now);

    // set next planned execution
    m_PlannedExecutionTime.tm_mday = nowStruct->tm_mday;
    m_PlannedExecutionTime.tm_mon = nowStruct->tm_mon;
    m_PlannedExecutionTime.tm_year = nowStruct->tm_year;
    m_PlannedExecutionTime.tm_isdst = -1;

    time_t plannedExecutionTime(mktime(&m_PlannedExecutionTime));
    if( plannedExecutionTime <= now ) {
        // planned time is in the past
        m_PlannedExecutionTime.tm_mday = nowStruct->tm_mday + 1;
        mktime(&m_PlannedExecutionTime);
    }
}


void DBMSrvScd_Plan::updatePlannedExecutionTime( const struct tm aPlannedTimeOfDay ) {
    resetStruct(&m_LastExecutionTime);
    m_PlannedExecutionTime.tm_hour = aPlannedTimeOfDay.tm_hour;
    m_PlannedExecutionTime.tm_min = aPlannedTimeOfDay.tm_min;
    m_PlannedExecutionTime.tm_sec = aPlannedTimeOfDay.tm_sec;
}

char* DBMSrvScd_Plan::getTimeString() const {
    char* pt = new char[9]; // definition os asctime
    if( pt != NULL ) {
        sprintf(pt, "%02d:%02d:%02d",
            m_PlannedExecutionTime.tm_hour,
            m_PlannedExecutionTime.tm_min,
            m_PlannedExecutionTime.tm_sec);
    }
    return pt;
}

bool DBMSrvScd_Plan::itsTimeToRun() {
    // what time is it?
    time_t now;
    time(&now);

    time_t plannedTime(mktime(&m_PlannedExecutionTime));

    if( m_StartTime > plannedTime ) {
        // we started after planned time, so increment the planned time
        m_PlannedExecutionTime.tm_mday = m_LastExecutionTime.tm_mday + 1;
        plannedTime = mktime(&m_PlannedExecutionTime);
    }

    return plannedTime <= now;
}

void DBMSrvScd_Plan::updateExecutionTimes() {
    // what time is it?
    time_t now;
    time(&now);
    struct tm* nowStruct = localtime(&now);

    // set last exection time
    m_LastExecutionTime = *nowStruct;

    // set next planned execution
    m_PlannedExecutionTime.tm_mday = m_LastExecutionTime.tm_mday + 1;
    m_PlannedExecutionTime.tm_mon = m_LastExecutionTime.tm_mon;
    m_PlannedExecutionTime.tm_year = m_LastExecutionTime.tm_year;
    mktime(&m_PlannedExecutionTime);
}

struct tm DBMSrvScd_Plan::getLastPlannedExecutionTime() const {
    
    struct tm lpe;
    resetStruct(&lpe);

    lpe.tm_sec = m_PlannedExecutionTime.tm_sec;
    lpe.tm_min = m_PlannedExecutionTime.tm_min;
    lpe.tm_hour = m_PlannedExecutionTime.tm_hour;
    lpe.tm_mday = m_LastExecutionTime.tm_mday;
    lpe.tm_mon = m_LastExecutionTime.tm_mon + 1;
    lpe.tm_year = m_LastExecutionTime.tm_year + 1900;

    return lpe;
}

char* DBMSrvScd_Plan::serialize() {

    char* serializedPlan(new char[getLen(&m_PlannedExecutionTime) +
                                getLen(&m_LastExecutionTime)+1]);

    if( serializedPlan != NULL ) {
        sprintf(serializedPlan,
            "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c",
            m_PlannedExecutionTime.tm_sec,
            SEPCHAR,
            m_PlannedExecutionTime.tm_min,
            SEPCHAR,
            m_PlannedExecutionTime.tm_hour,
            SEPCHAR,
            m_PlannedExecutionTime.tm_mday,
            SEPCHAR,
            m_PlannedExecutionTime.tm_mon,
            SEPCHAR,
            m_PlannedExecutionTime.tm_year,
            SEPCHAR,
            m_PlannedExecutionTime.tm_wday,
            SEPCHAR,
            m_PlannedExecutionTime.tm_yday,
            SEPCHAR,
            m_LastExecutionTime.tm_sec,
            SEPCHAR,
            m_LastExecutionTime.tm_min,
            SEPCHAR,
            m_LastExecutionTime.tm_hour,
            SEPCHAR,
            m_LastExecutionTime.tm_mday,
            SEPCHAR,
            m_LastExecutionTime.tm_mon,
            SEPCHAR,
            m_LastExecutionTime.tm_year,
            SEPCHAR,
            m_LastExecutionTime.tm_wday,
            SEPCHAR,
            m_LastExecutionTime.tm_yday,
            SEPCHAR);
    }    
    return serializedPlan;
}

void DBMSrvScd_Plan::resetStruct( struct tm* atmStruct ) const {
    (*atmStruct).tm_sec = 0;
    (*atmStruct).tm_min = 0;
    (*atmStruct).tm_hour = 0;
    (*atmStruct).tm_mday = 1;
    (*atmStruct).tm_mon = 0;
    (*atmStruct).tm_year = 0;
    (*atmStruct).tm_wday = 0;
    (*atmStruct).tm_yday = 0;
    (*atmStruct).tm_isdst = 0;
#if defined( OSF1 )
    (*atmStruct).tm_gmtoff = 0L;
    (*atmStruct).tm_zone = NULL;
#endif
}

size_t DBMSrvScd_Plan::getLen( struct tm* atmStruct ) {
    size_t serLen(0);
    int* pInt = (int*) atmStruct;
    for( int i=0; i<8; i++ ) {
        int part = *(pInt+i);
        if( part == 0 )
            serLen++;
        while( part > 0 ) {
            serLen++;
            part = part / 10;
        }
        serLen++; // for the separator
    }
    return serLen;
}

const char* DBMSrvScd_Plan::deserialize( const char* aStart, struct tm* atmStruct ) {
    int* pInt = (int*)atmStruct;
    
    char* tempSerPlan = new char[strlen(aStart)+1];
    char* t1(tempSerPlan);
    SAPDB_strcpy( tempSerPlan, aStart );
    
    for( int i=0; i<8; i++ ) {
        char* iterator = strchr(t1, SEPCHAR);
        *(pInt+i) = atoi(t1);
        *iterator = SEPCHAR;
        t1 = iterator+1;
    }
    const char* t2 = aStart + (tempSerPlan-t1);
    delete[] tempSerPlan;
    return t2;
}
