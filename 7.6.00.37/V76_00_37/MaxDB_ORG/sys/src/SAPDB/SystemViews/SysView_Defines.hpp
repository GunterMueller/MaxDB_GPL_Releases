/*!
  @file           SysView_Defines.hpp
  @author         ElkeZ
  @ingroup        SystemViews
  @brief   This file contains the defines for the system view table columns.
  @see            
*/
/*

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



*/



#ifndef SYSVIEW_DEFINES_HPP
#define SYSVIEW_DEFINES_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*!
    @brief   trace topic to control the debug output
    of the systemviews
*/

extern SAPDBTrace_Topic SysView_Trace;

/* following are the names of systemview-columns */

#define SV_ACCESSCOUNT                      "ACCESSCOUNT"                       /*!<_*/
#define SV_ACTIVE                           "ACTIVE"                            /*!<_*/
#define SV_ADDRESS                          "ADDRESS"                           /*!<_*/
#define SV_ADDRESSINGMODE                   "ADDRESSINGMODE"                    /*!<_*/
#define SV_ADMINSTATEDATE                   "ADMINSTATEDATE"                    /*!<_*/
#define SV_ADDNODEDATE                      "ADDNODEDATE"                       /*!<_*/
#define SV_ALLOBJECTCOUNT                   "ALLOBJECTCOUNT"                    /*!<_*/
#define SV_ALLOCATECOUNT                    "ALLOCATECOUNT"                     /*!<_*/
#define SV_ALLOCATEDSIZE                    "ALLOCATEDSIZE"                     /*!<_*/
#define SV_ALLOCATEOBJECTCOUNT              "ALLOCATEOBJECTCOUNT"               /*!<_*/
#define SV_ALLOCATORNAME                    "ALLOCATORNAME"                     /*!<_*/
#define SV_APPLICATIONNODE                  "APPLICATIONNODE"                   /*!<_*/
#define SV_APPLICATIONPROCESS               "APPLICATIONPROCESS"                /*!<_*/
#define SV_APPLICATIONTYPE                  "APPLICATIONTYPE"                   /*!<_*/
#define SV_AUTOMATICOVERWRITE               "AUTOMATICOVERWRITE"                /*!<_*/
#define SV_AVGCOLLISIONCOUNT                "AVGCOLLISIONCOUNT"                 /*!<_*/
#define SV_AVGEXECUTETIME                   "AVGEXECUTETIME"                    /*!<_*/
#define SV_AVGHASHTABLECHAINENTRYCOUNT      "AVGHASHTABLECHAINENTRYCOUNT"       /*!<_*/
#define SV_AVGLOBCOLUMNLENGTH               "AVGLOBCOLUMNLENGTH"                /*!<_*/
#define SV_AVGLOCKCOUNT                     "AVGLOCKCOUNT"                      /*!<_*/
#define SV_AVGLOCKOBJECTWAITTIME            "AVGLOCKOBJECTWAITTIME"             /*!<_*/
#define SV_AVGMAXSPINLOOPCOUNT              "AVGMAXSPINLOOPCOUNT"               /*!<_*/
#define SV_AVGMAXYIELDLOOPCOUNT             "AVGMAXYIELDLOOPCOUNT"              /*!<_*/
#define SV_AVGMEMORYDELTA                   "AVGMEMORYDELTA"                    /*!<_*/
#define SV_AVGREADSTREAMBUFFERCOUNT         "AVGREADSTREAMBUFFERCOUNT"          /*!<_*/
#define SV_AVGREADSTREAMROWCOUNT            "AVGREADSTREAMROWCOUNT"             /*!<_*/
#define SV_AVGRELEASEMEMORYCHUNKSIZE        "AVGRELEASEMEMORYCHUNKSIZE"         /*!<_*/
#define SV_AVGREQUESTMEMORYCHUNKSIZE        "AVGREQUESTMEMORYCHUNKSIZE"         /*!<_*/
#define SV_AVGROWCOUNTPERLEAF               "AVGROWCOUNTPERLEAF"                /*!<_*/
#define SV_AVGROWCOUNTPERSECONDARYKEY       "AVGROWCOUNTPERSECONDARYKEY"        /*!<_*/
#define SV_AVGROWLENGTH                     "AVGROWLENGTH"                      /*!<_*/
#define SV_AVGSECONDARYKEYLENGTH            "AVGSECONDARYKEYLENGTH"             /*!<_*/
#define SV_AVGSEPARATORLENGTH               "AVGSEPARATORLENGTH"                /*!<_*/
#define SV_AVGTOTALSPINLOOPCOUNT            "AVGTOTALSPINLOOPCOUNT"             /*!<_*/
#define SV_AVGTOTALYIELDLOOPCOUNT           "AVGTOTALYIELDLOOPCOUNT"            /*!<_*/
#define SV_AVGUSEDENTRYCOUNT                "AVGUSEDENTRYCOUNT"                 /*!<_*/
#define SV_AVGUSEDENTRYCOUNTPERCENTAGE      "AVGUSEDENTRYCOUNTPERCENTAGE"       /*!<_*/
#define SV_AVGUSEDSIZEPERCENTAGEINLEAVES    "AVGUSEDSIZEPERCENTAGEINLEAVES"     /*!<_*/
#define SV_AVGVAROBJECTLENGTH               "AVGVAROBJECTLENGTH"                /*!<_*/
#define SV_AVGWAITTIMEFORNEWCONSISTENTVIEW  "AVGWAITTIMEFORNEWCONSISTENTVIEW"   /*!<_*/
#define SV_AVGWRITESTREAMBUFFERCOUNT        "AVGWRITESTREAMBUFFERCOUNT"         /*!<_*/
#define SV_AVGWRITESTREAMROWCOUNT           "AVGWRITESTREAMROWCOUNT"            /*!<_*/
#define SV_BADINDEXCOUNT                    "BADINDEXCOUNT"                     /*!<_*/
#define SV_BASEALLOCATECOUNT                "BASEALLOCATECOUNT"                 /*!<_*/
#define SV_BASEALLOCATORNAME                "BASEALLOCATORNAME"                 /*!<_*/
#define SV_BASEDEALLOCATECOUNT              "BASEDEALLOCATECOUNT"               /*!<_*/
#define SV_BLOCKALLOCATORUSEDSIZE           "BLOCKALLOCATORUSEDSIZE"            /*!<_*/
#define SV_BUILD                            "BUILD"                             /*!<_*/
#define SV_BYTESRECEIVED                    "BYTESRECEIVED"                     /*!<_*/
#define SV_BYTESREPLIED                     "BYTESREPLIED"                      /*!<_*/
#define SV_CACHEENTRYCOUNT                  "CACHEENTRYCOUNT"                   /*!<_*/
#define SV_CALCULATEDSIZE                   "CALCULATEDSIZE"                    /*!<_*/
#define SV_CALLCOUNT                        "CALLCOUNT"                         /*!<_*/
#define SV_CALLSTACK                        "CALLSTACK"                         /*!<_*/
#define SV_CANCELFLAG                       "CANCELFLAG"                        /*!<_*/
#define SV_CATALOGCACHEUSEDSIZE             "CATALOGCACHEUSEDSIZE"              /*!<_*/
#define SV_CHANGEABLE                       "CHANGEABLE"                        /*!<_*/
#define SV_CHANGEDDATASIZE                  "CHANGEDDATASIZE"                   /*!<_*/
#define SV_CHUNKCOUNT                       "CHUNKCOUNT"                        /*!<_*/
#define SV_CHUNKSIZE                        "CHUNKSIZE"                         /*!<_*/
#define SV_CLASSGUID                        "CLASSGUID"                         /*!<_*/
#define SV_CLASSNAME                        "CLASSNAME"                         /*!<_*/
#define SV_CLEANUPCOUNT                     "CLEANUPCOUNT"                      /*!<_*/
#define SV_CLEANUPCOMMANDCOUNT              "CLEANUPCOMMANDCOUNT"               /*!<_*/
#define SV_COLLISIONCOUNT                   "COLLISIONCOUNT"                    /*!<_*/
#define SV_COLLISIONRATE                    "COLLISIONRATE"                     /*!<_*/
#define SV_COLUMNNAME                       "COLUMNNAME"                        /*!<_*/
#define SV_COMMANDCOUNT                     "COMMANDCOUNT"                      /*!<_*/
#define SV_COMMANDID                        "COMMANDID"                         /*!<_*/
#define SV_COMMENT                          "COMMENT"                           /*!<_*/
#define SV_COMMITSUBTRANSCOUNT              "COMMITSUBTRANSCOUNT"               /*!<_*/
#define SV_COMMUNICATIONCOUNT               "COMMUNICATIONCOUNT"                /*!<_*/
#define SV_CONFIGUREDSIZE                   "CONFIGUREDSIZE"                    /*!<_*/
#define SV_CONNECTSTATE                     "CONNECTSTATE"                      /*!<_*/
#define SV_CONSISTENTVIEWID                 "CONSISTENTVIEWID"                  /*!<_*/
#define SV_CONTAINERNUMBER                  "CONTAINERNUMBER"                   /*!<_*/
#define SV_CONVERTERUSEDSIZE                "CONVERTERUSEDSIZE"                 /*!<_*/
#define SV_CONVERTERVERSION                 "CONVERTERVERSION"                  /*!<_*/
#define SV_CORRECTIONLEVEL                  "CORRECTIONLEVEL"                   /*!<_*/
#define SV_CPPEXCEPTIONCOUNT                "CPPEXCEPTIONCOUNT"                 /*!<_*/
#define SV_CPULOAD                          "CPULOAD"                           /*!<_*/
#define SV_CPUNUMBER                        "CPUNUMBER"                         /*!<_*/
#define SV_CREATEDATE                       "CREATEDATE"                        /*!<_*/
#define SV_CREATEOBJECTCOUNT                "CREATEOBJECTCOUNT"                 /*!<_*/
#define SV_CURRENTEXECUTECOUNT              "CURRENTEXECUTECOUNT"               /*!<_*/
#define SV_CURRENTSTATISTICSDELTACOUNT      "CURRENTSTATISTICSDELTACOUNT"       /*!<_*/
#define SV_CURRENTUNLOADSTATEMENTCOUNT      "CURRENTUNLOADSTATEMENTCOUNT"       /*!<_*/
#define SV_CYCLERELATEDENDOFFSET            "CYCLERELATEDENDOFFSET"             /*!<_*/
#define SV_DATABASEIDENTIFIER               "DATABASEIDENTIFIER"                /*!<_*/
#define SV_DATACACHEUSEDSIZE                "DATACACHEUSEDSIZE"                 /*!<_*/
#define SV_DATAFULL                         "DATAFULL"                          /*!<_*/
#define SV_DATEFORMAT                       "DATEFORMAT"                        /*!<_*/
#define SV_DBPROCEDURENAME                  "DBPROCEDURENAME"                   /*!<_*/
#define SV_DBPROCEDURESCHEMANAME            "DBPROCEDURESCHEMANAME"             /*!<_*/
#define SV_DEADLOCKCOUNT                    "DEADLOCKCOUNT"                     /*!<_*/
#define SV_DEALLOCATECOUNT                  "DEALLOCATECOUNT"                   /*!<_*/
#define SV_DEFAULTCODE                      "DEFAULTCODE"                       /*!<_*/
#define SV_DEFINEDLOBCOLUMNCOUNT            "DEFINEDLOBCOLUMNCOUNT"             /*!<_*/
#define SV_DELAYTIME                        "DELAYTIME"                         /*!<_*/
#define SV_DELETECACHEDOBJECTCOUNT          "DELETECACHEDOBJECTCOUNT"           /*!<_*/
#define SV_DELETECOMMANDCOUNT               "DELETECOMMANDCOUNT"                /*!<_*/
#define SV_DELETECOUNT                      "DELETECOUNT"                       /*!<_*/
#define SV_DELETEOBJECTCOUNT                "DELETEOBJECTCOUNT"                 /*!<_*/
#define SV_DELETEPLANCOUNT                  "DELETEPLANCOUNT"                   /*!<_*/
#define SV_DESCRIPTION                      "DESCRIPTION"                       /*!<_*/
#define SV_DESTINATIONMAXTASKRUNNABLETIME   "DESTINATIONMAXTASKRUNNABLETIME"    /*!<_*/
#define SV_DESTINATIONTASKGROUP             "DESTINATIONTASKGROUP"              /*!<_*/
#define SV_DEVICESTATE                      "DEVICESTATE"                       /*!<_*/
#define SV_DISPATCHCOUNT                    "DISPATCHCOUNT"                     /*!<_*/
#define SV_DISTINCTVALUECOUNT               "DISTINCTVALUECOUNT"                /*!<_*/
#define SV_DROPCONTAINERCOUNT               "DROPCONTAINERCOUNT"                /*!<_*/
#define SV_DROPVOLUME                       "DROPVOLUME"                        /*!<_*/
#define SV_DUPLICATEKEYCOUNT                "DUPLICATEKEYCOUNT"                 /*!<_*/
#define SV_EMERGENCYCLEANUPCOUNT            "EMERGENCYCLEANUPCOUNT"             /*!<_*/
#define SV_ENCRYPTION                       "ENCRYPTION"                        /*!<_*/
#define SV_ENDDATE                          "ENDDATE"                           /*!<_*/
#define SV_ENTERLRU                         "ENTERLRU"                          /*!<_*/
#define SV_ENTRYCOUNT                       "ENTRYCOUNT"                        /*!<_*/
#define SV_ERRORCODE                        "ERRORCODE"                         /*!<_*/
#define SV_ERRORTEXT                        "ERRORTEXT"                         /*!<_*/
#define SV_EXCLUSIVEACCESSCOUNT             "EXCLUSIVEACCESSCOUNT"              /*!<_*/
#define SV_EXTENDEDTASKSTATE                "EXTENDEDTASKSTATE"                 /*!<_*/
#define SV_EXECUTECOUNT                     "EXECUTECOUNT"                      /*!<_*/
#define SV_EXECUTETIME                      "EXECUTETIME"                       /*!<_*/
#define SV_FAILEDALLOCATECOUNT              "FAILEDALLOCATECOUNT"               /*!<_*/
#define SV_FETCHROWCOUNT                    "FETCHROWCOUNT"                     /*!<_*/
#define SV_FILEDIRECTORYROOT                "FILEDIRECTORYROOT"                 /*!<_*/
#define SV_FILEDIRECTORYUSEDSIZE            "FILEDIRECTORYUSEDSIZE"             /*!<_*/
#define SV_FILEID                           "FILEID"                            /*!<_*/
#define SV_FILESTATE                        "FILESTATE"                         /*!<_*/
#define SV_FIRSTNEEDEDLOGOFFSET             "FIRSTNEEDEDLOGOFFSET"              /*!<_*/
#define SV_FLUSHMODE                        "FLUSHMODE"                         /*!<_*/
#define SV_FOUNDERRORCOUNT                  "FOUNDERRORCOUNT"                   /*!<_*/
#define SV_FREEPAGECOUNT                    "FREEPAGECOUNT"                     /*!<_*/
#define SV_GROUPCOMMITCOUNT                 "GROUPCOMMITCOUNT"                  /*!<_*/
#define SV_HASHVALUE                        "HASHVALUE"                         /*!<_*/
#define SV_HASHTABLESLOT                    "HASHTABLESLOT"                     /*!<_*/
#define SV_HASHTABLESLOTCOUNT               "HASHTABLESLOTCOUNT"                /*!<_*/
#define SV_HISTORYDATASIZE                  "HISTORYDATASIZE"                   /*!<_*/
#define SV_HISTORYDIRECTORYROOT             "HISTORYDIRECTORYROOT"              /*!<_*/
#define SV_HISTORYLAST                      "HISTORYLAST"                       /*!<_*/
#define SV_HISTORYPAGECOUNT                 "HISTORYPAGECOUNT"                  /*!<_*/
#define SV_HISTORYROOT                      "HISTORYROOT"                       /*!<_*/
#define SV_HITRATE                          "HITRATE"                           /*!<_*/
#define SV_HOLDERCOUNT                      "HOLDERCOUNT"                       /*!<_*/
#define SV_HOLDERTASKID                     "HOLDERTASKID"                      /*!<_*/
#define SV_HOPCOUNT                         "HOPCOUNT"                          /*!<_*/
#define SV_ID                               "ID"                                /*!<_*/
#define SV_IDLECOUNT                        "IDLECOUNT"                         /*!<_*/
#define SV_IDLETIME                         "IDLETIME"                          /*!<_*/
#define SV_INCREMENTALBACKUPSIZE            "INCREMENTALBACKUPSIZE"             /*!<_*/
#define SV_INDEXNAME                        "INDEXNAME"                         /*!<_*/
#define SV_INITSTANDBYDATE                  "INITSTANDBYDATE"                   /*!<_*/
#define SV_INSERTCOMMANDCOUNT               "INSERTCOMMANDCOUNT"                /*!<_*/
#define SV_INSERTCOUNT                      "INSERTCOUNT"                       /*!<_*/
#define SV_INTERVALSTARTTIME                "INTERVALSTARTTIME"                 /*!<_*/
#define SV_INVALIDATECOUNT                  "INVALIDATECOUNT"                   /*!<_*/
#define SV_ISOLATIONLEVEL                   "ISOLATIONLEVEL"                    /*!<_*/
#define SV_INSTALLSERVERDBVERSION           "INSTALLSERVERDBVERSION"            /*!<_*/
#define SV_INTERNAL                         "INTERNAL"                          /*!<_*/
#define SV_INVALIDATECOUNT                  "INVALIDATECOUNT"                   /*!<_*/
#define SV_IOCOUNT                          "IOCOUNT"                           /*!<_*/
#define SV_IODIRECTION                      "IODIRECTION"                       /*!<_*/
#define SV_IOMANAGEMENTUSEDSIZE             "IOMANAGEMENTUSEDSIZE"              /*!<_*/
#define SV_IOPAGECOUNT                      "IOPAGECOUNT"                       /*!<_*/
#define SV_IOTIME                           "IOTIME"                            /*!<_*/
#define SV_IOTIMEMEASUREMENTCOUNT           "IOTIMEMEASUREMENTCOUNT"            /*!<_*/
#define SV_IOWAITCOUNT                      "IOWAITCOUNT"                       /*!<_*/
#define SV_IOWAITTIME                       "IOWAITTIME"                        /*!<_*/
#define SV_IOWAITTIMEMEASUREMENTCOUNT       "IOWAITTIMEMEASUREMENTCOUNT"        /*!<_*/
#define SV_ISASSIGNED                       "ISASSIGNED"                        /*!<_*/
#define SV_ISRESERVED                       "ISRESERVED"                        /*!<_*/
#define SV_ISOLATIONLEVEL                   "ISOLATIONLEVEL"                    /*!<_*/
#define SV_LASTJOBDESCRIPTION               "LASTJOBDESCRIPTION"                /*!<_*/
#define SV_KERNELPROCESSID                  "KERNELPROCESSID"                   /*!<_*/
#define SV_KERNELVARIANT                    "KERNELVARIANT"                     /*!<_*/
#define SV_KEYDEFINED                       "KEYDEFINED"                        /*!<_*/
#define SV_KEYLEAFPAGECOUNT                 "KEYLEAFPAGECOUNT"                  /*!<_*/
#define SV_KEYPARTITIONCOUNT                "KEYPARTITIONCOUNT"                 /*!<_*/
#define SV_LASTDATABACKUPID                 "LASTDATABACKUPID"                  /*!<_*/
#define SV_LASTDATABACKUPLOGSEQUENCE        "LASTDATABACKUPLOGSEQUENCE"         /*!<_*/
#define SV_LASTDATABACKUPVERSION            "LASTDATABACKUPVERSION"             /*!<_*/
#define SV_LASTEMERGENCYHISTORYCLEANUP      "LASTEMERGENCYHISTORYCLEANUP"       /*!<_*/
#define SV_LASTENTEREDTASKID                "LASTENTEREDTASKID"                 /*!<_*/
#define SV_LASTFULLDATABACKUPSUCCESSFUL     "LASTFULLDATABACKUPSUCCESSFUL"      /*!<_*/
#define SV_LASTFULLDATABACKUPVERSION        "LASTFULLDATABACKUPVERSION"         /*!<_*/
#define SV_LASTHISTORYSEQUENCE              "LASTHISTORYSEQUENCE"               /*!<_*/
#define SV_LASTKNOWNOFFSET                  "LASTKNOWNOFFSET"                   /*!<_*/
#define SV_LASTKNOWNSEQUENCE                "LASTKNOWNSEQUENCE"                 /*!<_*/
#define SV_LASTLOGBACKUPID                  "LASTLOGBACKUPID"                   /*!<_*/
#define SV_LASTOPENDATE                     "LASTOPENDATE"                      /*!<_*/
#define SV_LASTSAVEPOINTDATE                "LASTSAVEPOINTDATE"                 /*!<_*/
#define SV_LASTSAVEPOINTID                  "LASTSAVEPOINTID"                   /*!<_*/
#define SV_LASTSAVEPOINTRELEASEVERSION      "LASTSAVEPOINTRELEASEVERSION"       /*!<_*/
#define SV_LASTSAVEPOINTREASON              "LASTSAVEPOINTREASON"               /*!<_*/
#define SV_LASTSYNCHRONIZEDATE              "LASTSYNCHRONIZEDATE"               /*!<_*/
#define SV_LASTVALIDLOGOFFSETSENT           "LASTVALIDLOGOFFSETSENT"            /*!<_*/
#define SV_LASTVALIDLOGOFFSETRECEIVED       "LASTVALIDLOGOFFSETRECEIVED"        /*!<_*/
#define SV_LASTWRITE                        "LASTWRITE"                         /*!<_*/
#define SV_LOBDIRECTORYROOT                 "LOBDIRECTORYROOT"                  /*!<_*/
#define SV_LOBSIZE                          "LOBSIZE"                           /*!< PTS 1133892 M.Ki */
#define SV_LOCALNODE                        "LOCALNODE"                         /*!<_*/
#define SV_LOCKCOUNT                        "LOCKCOUNT"                         /*!<_*/
#define SV_LOCKESCALATIONCOUNT              "LOCKESCALATIONCOUNT"               /*!<_*/
#define SV_LOCKESCALATIONVALUE              "LOCKESCALATIONVALUE"               /*!<_*/
#define SV_LOCKMODE                         "LOCKMODE"                          /*!<_*/
#define SV_LOCKOBJECTCOUNT                  "LOCKOBJECTCOUNT"                   /*!<_*/
#define SV_LOCKOBJECTCOLLISIONCOUNT         "LOCKOBJECTCOLLISIONCOUNT"          /*!<_*/
#define SV_LOCKREQUESTTIMEOUT               "LOCKREQUESTTIMEOUT"                /*!<_*/
#define SV_LOCKSTATE                        "LOCKSTATE"                         /*!<_*/
#define SV_LOGFULL                          "LOGFULL"                           /*!<_*/
#define SV_LOGMODE                          "LOGMODE"                           /*!<_*/
#define SV_LOGQUEUEUSEDSIZE                 "LOGQUEUEUSEDSIZE"                  /*!<_*/
#define SV_LOGVOLUMEENABLED                 "LOGVOLUMEENABLED"                  /*!< PTS 1132161 mb */
#define SV_LOGWRITERSUSPENDED               "LOGWRITERSUSPENDED"                /*!< PTS 1132161 mb */
#define SV_LONGDIRECTORYROOT                "LONGDIRECTORYROOT"                 /*!<_*/
#define SV_LRUENTRYCOUNT                    "LRUENTRYCOUNT"                     /*!<_*/
#define SV_MACHINEUSEDUSERTIME              "MACHINEUSEDUSERTIME"               /*!<_*/
#define SV_MACHINEUSEDSYSTEMTIME            "MACHINEUSEDSYSTEMTIME"             /*!<_*/
#define SV_MACHINEIDLETIME                  "MACHINEIDLETIME"                   /*!<_*/
#define SV_MAJORVERSION                     "MAJORVERSION"                      /*!<_*/
#define SV_MASSCOMMAND                      "MASSCOMMAND"                       /*!<_*/
#define SV_MASTERLOCALNODE                  "MASTERLOCALNODE"                   /*!<_*/
#define SV_MAXCOLLISIONCOUNT                "MAXCOLLISIONCOUNT"                 /*!<_*/
#define SV_MAXCOMMANDCOUNT                  "MAXCOMMANDCOUNT"                   /*!<_*/
#define SV_MAXEXECUTETIME                   "MAXEXECUTETIME"                    /*!<_*/
#define SV_MAXENTRYCOUNT                    "MAXENTRYCOUNT"                     /*!<_*/
#define SV_MAXHASHTABLECHAINENTRYCOUNT      "MAXHASHTABLECHAINENTRYCOUNT"       /*!<_*/
#define SV_MAXLOBCOLUMNLENGTH               "MAXLOBCOLUMNLENGTH"                /*!<_*/
#define SV_MAXLOCKCOUNT                     "MAXLOCKCOUNT"                      /*!<_*/
#define SV_MAXMAXSPINLOOPCOUNT              "MAXMAXSPINLOOPCOUNT"               /*!<_*/
#define SV_MAXMAXYIELDLOOPCOUNT             "MAXMAXYIELDLOOPCOUNT"              /*!<_*/
#define SV_MAXMEMORYDELTA                   "MAXMEMORYDELTA"                    /*!<_*/
#define SV_MAXPARSEIDCOUNT                  "MAXPARSEIDCOUNT"                   /*!<_*/
#define SV_MAXPENDINGIOCOUNT                "MAXPENDINGIOCOUNT"                 /*!<_*/
#define SV_MAXPERMANENTDATAPAGENUMBER       "MAXPERMANENTDATAPAGENUMBER"        /*!<_*/
#define SV_MAXPERMANENTSTATICDATAPAGENUMBER "MAXPERMANENTSTATICDATAPAGENUMBER"  /*!<_*/
#define SV_MAXREADSTREAMBUFFERCOUNT         "MAXREADSTREAMBUFFERCOUNT"          /*!<_*/
#define SV_MAXREADSTREAMROWCOUNT            "MAXREADSTREAMROWCOUNT"             /*!<_*/
#define SV_MAXRELEASEMEMORYCHUNKSIZE        "MAXRELEASEMEMORYCHUNKSIZE"         /*!<_*/
#define SV_MAXREQUESTMEMORYCHUNKSIZE        "MAXREQUESTMEMORYCHUNKSIZE"         /*!<_*/
#define SV_MAXROWCOUNTPERLEAF               "MAXROWCOUNTPERLEAF"                /*!<_*/
#define SV_MAXROWCOUNTPERSECONDARYKEY       "MAXROWCOUNTPERSECONDARYKEY"        /*!<_*/
#define SV_MAXROWLENGTH                     "MAXROWLENGTH"                      /*!<_*/
#define SV_MAXSECONDARYKEYLENGTH            "MAXSECONDARYKEYLENGTH"             /*!<_*/
#define SV_MAXSEPARATORLENGTH               "MAXSEPARATORLENGTH"                /*!<_*/
#define SV_MAXSPINLOOPCOUNT                 "MAXSPINLOOPCOUNT"                  /*!<_*/
#define SV_MAXTASKRUNNABLETIME              "MAXTASKRUNNABLETIME"               /*!<_*/
#define SV_MAXTEMPORARYDATAPAGENUMBER       "MAXTEMPORARYDATAPAGENUMBER"        /*!<_*/
#define SV_MAXTOTALSPINLOOPCOUNT            "MAXTOTALSPINLOOPCOUNT"             /*!<_*/
#define SV_MAXTOTALYIELDLOOPCOUNT           "MAXTOTALYIELDLOOPCOUNT"            /*!<_*/
#define SV_MAXUSEDENTRYCOUNT                "MAXUSEDENTRYCOUNT"                 /*!<_*/
#define SV_MAXUSEDENTRYCOUNTPERCENTAGE      "MAXUSEDENTRYCOUNTPERCENTAGE"       /*!<_*/
#define SV_MAXUSEDFILEID                    "MAXUSEDFILEID"                     /*!<_*/
#define SV_MAXUSEDHASHTABLESLOTCOUNT        "MAXUSEDHASHTABLESLOTCOUNT"         /*!<_*/
#define SV_MAXUSEDPERMANENTSIZE             "MAXUSEDPERMANENTSIZE"              /*!<_*/
#define SV_MAXUSEDRUNQUEUELENGTH            "MAXUSEDRUNQUEUELENGTH"             /*!<_*/
#define SV_MAXUSEDSESSIONID                 "MAXUSEDSESSIONID"                  /*!<_*/
#define SV_MAXUSEDSIZE                      "MAXUSEDSIZE"                       /*!<_*/
#define SV_MAXUSEDSIZEPERCENTAGE            "MAXUSEDSIZEPERCENTAGE"             /*!<_*/
#define SV_MAXUSEDSIZEPERCENTAGEINLEAVES    "MAXUSEDSIZEPERCENTAGEINLEAVES"     /*!<_*/
#define SV_MAXUSEDSTACK                     "MAXUSEDSTACK"                      /*!<_*/
#define SV_MAXUSEDTEMPORARYSIZE             "MAXUSEDTEMPORARYSIZE"              /*!<_*/
#define SV_MAXUSEDTRANSACTIONNUMBER         "MAXUSEDTRANSACTIONNUMBER"          /*!<_*/
#define SV_MAXWAITCOUNTPERPAGE              "MAXWAITCOUNTPERPAGE"               /*!<_*/
#define SV_MAXWAITTIMEFORNEWCONSISTENTVIEW  "MAXWAITTIMEFORNEWCONSISTENTVIEW"   /*!<_*/
#define SV_MAXWRITESTREAMBUFFERCOUNT        "MAXWRITESTREAMBUFFERCOUNT"         /*!<_*/
#define SV_MAXWRITESTREAMROWCOUNT           "MAXWRITESTREAMROWCOUNT"            /*!<_*/
#define SV_MAXYIELDLOOPCOUNT                "MAXYIELDLOOPCOUNT"                 /*!<_*/
#define SV_MEASUREMENTINTERVALS             "MEASUREMENTINTERVALS"              /*!<_*/
#define SV_MEASUREMENTINTERVALTIME          "MEASUREMENTINTERVALTIME"           /*!<_*/
#define SV_MEMORYRELEASECOUNT               "MEMORYRELEASECOUNT"                /*!<_*/
#define SV_MEMORYREQUESTCOUNT               "MEMORYREQUESTCOUNT"                /*!<_*/
#define SV_MINCOLLISIONCOUNT                "MINCOLLISIONCOUNT"                 /*!<_*/
#define SV_MINDIFFERENCE                    "MINDIFFERENCE"                     /*!<_*/
#define SV_MINEXECUTETIME                   "MINEXECUTETIME"                    /*!<_*/
#define SV_MINLOBCOLUMNLENGTH               "MINLOBCOLUMNLENGTH"                /*!<_*/
#define SV_MINLOCKCOUNT                     "MINLOCKCOUNT"                      /*!<_*/
#define SV_MINMAXSPINLOOPCOUNT              "MINMAXSPINLOOPCOUNT"               /*!<_*/
#define SV_MINMAXYIELDLOOPCOUNT             "MINMAXYIELDLOOPCOUNT"              /*!<_*/
#define SV_MINMEMORYDELTA                   "MINMEMORYDELTA"                    /*!<_*/
#define SV_MINORVERSION                     "MINORVERSION"                      /*!<_*/
#define SV_MINREADSTREAMBUFFERCOUNT         "MINREADSTREAMBUFFERCOUNT"          /*!<_*/
#define SV_MINREADSTREAMROWCOUNT            "MINREADSTREAMROWCOUNT"             /*!<_*/
#define SV_MINRELEASEMEMORYCHUNKSIZE        "MINRELEASEMEMORYCHUNKSIZE"         /*!<_*/
#define SV_MINREQUESTMEMORYCHUNKSIZE        "MINREQUESTMEMORYCHUNKSIZE"         /*!<_*/
#define SV_MINROWCOUNTPERLEAF               "MINROWCOUNTPERLEAF"                /*!<_*/
#define SV_MINROWCOUNTPERSECONDARYKEY       "MINROWCOUNTPERSECONDARYKEY"        /*!<_*/
#define SV_MINROWLENGTH                     "MINROWLENGTH"                      /*!<_*/
#define SV_MINSECONDARYKEYLENGTH            "MINSECONDARYKEYLENGTH"             /*!<_*/
#define SV_MINSEPARATORLENGTH               "MINSEPARATORLENGTH"                /*!<_*/
#define SV_MINTOTALSPINLOOPCOUNT            "MINTOTALSPINLOOPCOUNT"             /*!<_*/
#define SV_MINTOTALYIELDLOOPCOUNT           "MINTOTALYIELDLOOPCOUNT"            /*!<_*/
#define SV_MINUSEDSIZEPERCENTAGEINLEAVES    "MINUSEDSIZEPERCENTAGEINLEAVES"     /*!<_*/
#define SV_MINWAITTIMEFORNEWCONSISTENTVIEW  "MINWAITTIMEFORNEWCONSISTENTVIEW"   /*!<_*/
#define SV_MINWRITESTREAMBUFFERCOUNT        "MINWRITESTREAMBUFFERCOUNT"         /*!<_*/
#define SV_MINWRITESTREAMROWCOUNT           "MINWRITESTREAMROWCOUNT"            /*!<_*/
#define SV_MODE                             "MODE"                              /*!<_*/
#define SV_MOVEABLETASKCOUNT                "MOVEABLETASKCOUNT"                 /*!<_*/
#define SV_MOVETASKTOTASKGROUPID            "MOVETASKTOTASKGROUPID"             /*!<_*/
#define SV_MOVETIME                         "MOVETIME"                          /*!<_*/
#define SV_NAME                             "NAME"                              /*!<_*/
#define SV_NOTSAVEDPERCENTAGE               "NOTSAVEDPERCENTAGE"                /*!<_*/
#define SV_NOTSAVEDSIZE                     "NOTSAVEDSIZE"                      /*!<_*/
#define SV_NODE                             "NODE"                              /*!<_*/
#define SV_NONEXCLUSIVEACCESSCOUNT          "NONEXCLUSIVEACCESSCOUNT"           /*!<_*/
#define SV_NUMERICVALUE                     "NUMERICVALUE"                      /*!<_*/
#define SV_OBJECTID                         "OBJECTID"                          /*!<_*/
#define SV_OBJECTSIZE                       "OBJECTSIZE"                        /*!<_*/
#define SV_OCCUPANCY                        "OCCUPANCY"                         /*!<_*/
#define SV_OLDESTNOTSAVEDLOGOFFSET          "OLDESTNOTSAVEDLOGOFFSET"           /*!<_*/
#define SV_OLDESTNOTSAVEDLOGSEQUENCE        "OLDESTNOTSAVEDLOGSEQUENCE"         /*!<_*/
#define SV_OMSCALLACTIVE                    "OMSCALLACTIVE"                     /*!<_*/
#define SV_OMSCALLBACKCOUNT                 "OMSCALLBACKCOUNT"                  /*!<_*/
#define SV_OMSCALLBACKMETHOD                "OMSCALLBACKMETHOD"                 /*!<_*/
#define SV_OMSCOMPONENT                     "OMSCOMPONENT"                      /*!<_*/
#define SV_OMSDATASIZE                      "OMSDATASIZE"                       /*!<_*/
#define SV_OMSDIAGENTRY                     "OMSDIAGNOSTICENTRY"                /*!<_*/
#define SV_OMSHEAPUSEDSIZE                  "OMSHEAPUSEDSIZE"                   /*!<_*/
#define SV_OMSLOCKCOLLISIONCOUNT            "OMSLOCKCOLLISIONCOUNT"             /*!<_*/
#define SV_OMSLOCKMODE                      "OMSLOCKMODE"                       /*!<_*/
#define SV_OMSLOCKREQUESTTIMEOUT            "OMSLOCKREQUESTTIMEOUT"             /*!<_*/
#define SV_OMSPROCEDURENAME                 "OMSPROCEDURENAME"                  /*!<_*/
#define SV_OMSREQUESTTIMEOUTCOUNT           "OMSREQUESTTIMEOUTCOUNT"            /*!<_*/
#define SV_OMSUNLOADEDVERSIONSIZE           "OMSUNLOADEDVERSIONSIZE"            /*!<_*/
#define SV_OMSVERSIONID                     "OMSVERSIONID"                      /*!<_*/
#define SV_ONLINESTATEDATE                  "ONLINESTATEDATE"                   /*!<_*/
#define SV_OPENTRANSACTIONCOUNT             "OPENTRANSACTIONCOUNT"              /*!<_*/
#define SV_OPENTRANSACTIONFILEROOT          "OPENTRANSACTIONFILEROOT"           /*!<_*/
#define SV_OPENVERSION                      "OPENVERSION"                       /*!<_*/
#define SV_OPERATINGSYSTEM                  "OPERATINGSYSTEM"                   /*!<_*/
#define SV_OUTOFDATAEXCEPTIONCOUNT          "OUTOFDATAEXCEPTIONCOUNT"           /*!<_*/
#define SV_OUTOFMEMORYEXCEPTIONCOUNT        "OUTOFMEMORYEXCEPTIONCOUNT"         /*!<_*/
#define SV_OWN                              "OWN"                               /*!<_*/
#define SV_OWNER                            "OWNER"                             /*!<_*/
#define SV_PACKAGENAME                      "PACKAGENAME"                       /*!<_*/
#define SV_PAGE                             "PAGE"                              /*!<_*/
#define SV_PAGECOUNT                        "PAGECOUNT"                         /*!<_*/
#define SV_PAGINGFILEUSEDSIZE               "PAGINGFILEUSEDSIZE"                /*!<_*/
#define SV_PARAMETERNAME                    "PARAMETERNAME"                     /*!<_*/
#define SV_PARSECOUNT                       "PARSECOUNT"                        /*!<_*/
#define SV_PARSEID                          "PARSEID"                           /*!<_*/
#define SV_PATH                             "PATH"                              /*!<_*/
#define SV_MIRROREDPATH                     "MIRRORPATH"                        /*!<_*/
#define SV_PEERPROCESSID                    "PEERPROCESSID"                     /*!<_*/
#define SV_PENDINGIOCOUNT                   "PENDINGIOCOUNT"                    /*!<_*/
#define SV_PENDINGLOGSIZE                   "PENDINGLOGSIZE"                    /*!<_*/
#define SV_PENDINGOPERATIONCOUNT            "PENDINGOPERATIONCOUNT"             /*!<_*/
#define SV_PERMANENT                        "PERMANENT"                         /*!<_*/
#define SV_PHYSICALMEMORYSIZE               "PHYSICALMEMORYSIZE"                /*!<_*/
#define SV_PHYSICALCPUNUMBER                "PHYSICALCPUNUMBER"                 /*!<_*/
#define SV_PHYSICALMEMORYUSABLE             "PHYSICALMEMORYUSABLE"              /*!<_*/
#define SV_PHYSICALREADCOUNT                "PHYSICALREADCOUNT"                 /*!<_*/
#define SV_PHYSICALWRITECOUNT               "PHYSICALWRITECOUNT"                /*!<_*/
#define SV_PLANSIZE                         "PLANSIZE"                          /*!<_*/
#define SV_PREPARECOUNT                     "PREPARECOUNT"                      /*!<_*/
#define SV_PREPARETIME                      "PREPARETIME"                       /*!<_*/
#define SV_PREPARESTANDBYDATE               "PREPARESTANDBYDATE"                /*!<_*/
#define SV_PRIMARYFILEID                    "PRIMARYFILEID"                     /*!< PTS 1134676 M.Ki. */
#define SV_PROCEDURENAME                    "PROCEDURENAME"                     /*!<_*/
#define SV_PROCESSUSEDUSERTIME              "PROCESSUSEDUSERTIME"               /*!<_*/
#define SV_PROCESSUSEDSYSTEMTIME            "PROCESSUSEDSYSTEMTIME"             /*!<_*/
#define SV_PROCESSORTYPE                    "PROCESSORTYPE"                     /*!<_*/
#define SV_QUALIFIEDROWCOUNT                "QUALIFIEDROWCOUNT"                 /*!<_*/
#define SV_QUEUECOUNT                       "QUEUECOUNT"                        /*!<_*/
#define SV_QUEUEID                          "QUEUEID"                           /*!< PTS 1132161 mb */
#define SV_QUEUEINSERTCOUNT                 "QUEUEINSERTCOUNT"                  /*!<_*/
#define SV_QUEUEMAXUSED                     "QUEUEMAXUSED"                      /*!<_*/
#define SV_QUEUEOVERFLOWCOUNT               "QUEUEOVERFLOWCOUNT"                /*!<_*/
#define SV_QUEUESIZE                        "QUEUESIZE"                         /*!<_*/
#define SV_QUERYREWRITE                     "QUERYREWRITE"                      /*!<_*/
#define SV_READABSOLUTETIME                 "READABSOLUTETIME"                  /*!<_*/
#define SV_READCACHEDOBJECTCOUNT            "READCACHEDOBJECTCOUNT"             /*!<_*/
#define SV_READCACHEDOBJECTVIAKEYCOUNT      "READCACHEDOBJECTVIAKEYCOUNT"       /*!<_*/
#define SV_READCOUNT                        "READCOUNT"                         /*!<_*/
#define SV_READERWRITERLOCK                 "READERWRITERLOCK"                  /*!<_*/
#define SV_READERWRITERLOCKADDRESS          "READERWRITERLOCKADDRESS"           /*!<_*/
#define SV_READERWRITERLOCKINDEX            "READERWRITERLOCKINDEX"             /*!<_*/
#define SV_READOBJECTCOUNT                  "READOBJECTCOUNT"                   /*!<_*/
#define SV_READOBJECTHISTORYACCESSCOUNT     "READOBJECTHISTORYACCESSCOUNT"      /*!<_*/
#define SV_READOBJECTVIAKEYCOUNT            "READOBJECTVIAKEYCOUNT"             /*!<_*/
#define SV_READPAGECOUNT                    "READPAGECOUNT"                     /*!<_*/
#define SV_READRELATIVETIME                 "READRELATIVETIME"                  /*!<_*/
#define SV_READROWCOUNT                     "READROWCOUNT"                      /*!<_*/
#define SV_READTIME                         "READTIME"                          /*!<_*/
#define SV_READTIMEMEASUREMENTCOUNT         "READTIMEMEASUREMENTCOUNT"          /*!<_*/
#define SV_READVAROBJECTCOUNT               "READVAROBJECTCOUNT"                /*!<_*/
#define SV_REFERENCECOUNT                   "REFERENCECOUNT"                    /*!<_*/
#define SV_REGIONACCESSCOUNT                "REGIONACCESSCOUNT"                 /*!<_*/
#define SV_REGIONID                         "REGIONID"                          /*!<_*/
#define SV_REGIONNAME                       "REGIONNAME"                        /*!<_*/
#define SV_REGISTERSTANDBYDATE              "REGISTERSTANDBYDATE"               /*!<_*/
#define SV_RELEASECONTAINERPAGECOUNT        "RELEASECONTAINERPAGECOUNT"         /*!<_*/
#define SV_RELEASEMEMORYCOUNT               "RELEASEMEMORYCOUNT"                /*!<_*/
#define SV_RELEASEOBJECTPAGECOUNT           "RELEASEOBJECTPAGECOUNT"            /*!<_*/
#define SV_RELOADSTATEMENTCOUNT             "RELOADSTATEMENTCOUNT"              /*!<_*/
#define SV_REMOVELRU                        "REMOVELRU"                         /*!<_*/
#define SV_REMOVENODEDATE                   "REMOVENODEDATE"                    /*!<_*/
#define SV_REQUESTMODE                      "REQUESTMODE"                       /*!<_*/
#define SV_REQUESTORCOUNT                   "REQUESTORCOUNT"                    /*!<_*/
#define SV_REQUESTORTASKID                  "REQUESTORTASKID"                   /*!<_*/
#define SV_REQUESTSTATE                     "REQUESTSTATE"                      /*!<_*/
#define SV_RESERVEDCLUSTERAREASIZE          "RESERVEDCLUSTERAREASIZE"           /*!<_*/
#define SV_RESETDATE                        "RESETDATE"                         /*!<_*/
#define SV_RESIZEHASHTABLECOUNT             "RESIZEHASHTABLECOUNT"              /*!<_*/
#define SV_RESTARTCONVERTERVERSION          "RESTARTCONVERTERVERSION"           /*!<_*/
#define SV_RESTARTENTRYDATE                 "RESTARTENTRYDATE"                  /*!<_*/
#define SV_RESTARTENTRYTYPE                 "RESTARTENTRYTYPE"                  /*!<_*/
#define SV_RESTARTIOSEQUENCE                "RESTARTIOSEQUENCE"                 /*!<_*/
#define SV_RESTARTOFFSETOFENTRYINPAGE       "RESTARTOFFSETOFENTRYINPAGE"        /*!<_*/
#define SV_RESTARTOFFSETOFPAGEONVOLUME      "RESTARTOFFSETOFPAGEONVOLUME"       /*!<_*/
#define SV_RESTARTRECORDUSEDSIZE            "RESTARTRECORDUSEDSIZE"             /*!<_*/
#define SV_RESTARTSTANDBYDATE               "RESTARTSTANDBYDATE"                /*!<_*/
#define SV_RESUMETASKCOUNT                  "RESUMETASKCOUNT"                   /*!<_*/
#define SV_ROLLBACKSUBTRANSCOUNT            "ROLLBACKSUBTRANSCOUNT"             /*!<_*/
#define SV_ROLLEDOUT                        "ROLLEDOUT"                         /*!<_*/
#define SV_ROOT                             "ROOT"                              /*!<_*/
#define SV_ROWCOUNT                         "ROWCOUNT"                          /*!<_*/
#define SV_ROWID                            "ROWID"                             /*!<_*/
#define SV_ROWIDHEX                         "ROWIDHEX"                          /*!<_*/
#define SV_ROWIDLENGTH                      "ROWIDLENGTH"                       /*!<_*/
#define SV_ROWLOCKCOUNT                     "ROWLOCKCOUNT"                      /*!<_*/
#define SV_ROWREGIONCOUNT                   "ROWREGIONCOUNT"                    /*!<_*/
#define SV_RUNNABLE                         "RUNNABLE"                          /*!<_*/
#define SV_SAVEPOINTCOUNT                   "SAVEPOINTCOUNT"                    /*!<_*/
#define SV_SCANOBJECTCOUNT                  "SCANOBJECTCOUNT"                   /*!<_*/
#define SV_SCANOBJECTHISTORYACCESSCOUNT     "SCANOBJECTHISTORYACCESSCOUNT"      /*!<_*/
#define SV_SCANOBJECTVIAKEYRANGECOUNT       "SCANOBJECTVIAKEYRANGECOUNT"        /*!<_*/
#define SV_SCHEMANAME                       "SCHEMANAME"                        /*!<_*/
#define SV_SECONDARYKEYCOUNT                "SECONDARYKEYCOUNT"                 /*!<_*/
#define SV_SELFREADABSOLUTETIME             "SELFREADABSOLUTETIME"              /*!<_*/
#define SV_SELFREADCOUNT                    "SELFREADCOUNT"                     /*!<_*/
#define SV_SELFREADPAGECOUNT                "SELFREADPAGECOUNT"                 /*!<_*/
#define SV_SELFREADTIMEMEASUREMENTCOUNT     "SELFREADTIMEMEASUREMENTCOUNT"      /*!<_*/
#define SV_SELFWRITEABSOLUTETIME            "SELFWRITEABSOLUTETIME"             /*!<_*/
#define SV_SELFWRITECOUNT                   "SELFWRITECOUNT"                    /*!<_*/
#define SV_SELFWRITEPAGECOUNT               "SELFWRITEPAGECOUNT"                /*!<_*/
#define SV_SELFWRITETIMEMEASUREMENTCOUNT    "SELFWRITETIMEMEASUREMENTCOUNT"     /*!<_*/
#define SV_SESSIONID                        "SESSIONID"                         /*!<_*/
#define SV_SESSIONTYPE                      "SESSIONTYPE"                       /*!<_*/
#define SV_SLEEPABSOLUTETIME                "SLEEPABSOLUTETIME"                 /*!<_*/
#define SV_SLEEPCOUNT                       "SLEEPCOUNT"                        /*!<_*/
#define SV_SLEEPRELATIVETIME                "SLEEPRELATIVETIME"                 /*!<_*/
#define SV_SLEEPTIMEMEASUREMENTCOUNT        "SLEEPTIMEMEASUREMENTCOUNT"         /*!<_*/
#define SV_SLOT                             "SLOT"                              /*!<_*/
#define SV_SOURCEMAXTASKRUNNABLETIME        "SOURCEMAXTASKRUNNABLETIME"         /*!<_*/
#define SV_SOURCETASKGROUP                  "SOURCETASKGROUP"                   /*!<_*/
#define SV_SPINLOCKPOOLNAME                 "SPINLOCKPOOLNAME"                  /*!<_*/
#define SV_SPINLOCKCOLLISIONCOUNT           "SPINLOCKCOLLISIONCOUNT"            /*!<_*/
#define SV_SPINLOOPCOUNT                    "SPINLOOPCOUNT"                     /*!<_*/
#define SV_SQLCMD                           "SQLCMD"                            /*!<_*/
#define SV_SQLCMDFLUSHED                    "SQLCMDFLUSHED"                     /*!<_*/
#define SV_SQLCMDLENGTH                     "SQLCMDLENGTH"                      /*!<_*/
#define SV_SQLDATASIZE                      "SQLDATASIZE"                       /*!<_*/
#define SV_SQLLOCKCOLLISIONCOUNT            "SQLLOCKCOLLISIONCOUNT"             /*!<_*/
#define SV_SQLLOCKMODE                      "SQLLOCKMODE"                       /*!<_*/
#define SV_SQLLOCKREQUESTTIMEOUT            "SQLLOCKREQUESTTIMEOUT"             /*!<_*/
#define SV_SQLMODE                          "SQLMODE"                           /*!<_*/
#define SV_SQLREQUESTTIMEOUTCOUNT           "SQLREQUESTTIMEOUTCOUNT"            /*!<_*/
#define SV_STANDBYROLE                      "STANDBYROLE"                       /*!<_*/
#define SV_STANDBYSTATEDATE                 "STANDBYSTATEDATE"                  /*!<_*/
#define SV_STATEMENTID                      "STATEMENTID"                       /*!<_*/
#define SV_STARTDATE                        "STARTDATE"                         /*!<_*/
#define SV_STATE                            "STATE"                             /*!<_*/
#define SV_STATEMENT                        "STATEMENT"                         /*!<_*/
#define SV_STATEMENTCODETYPE                "STATEMENTCODETYPE"                 /*!<_*/
#define SV_STATEMENTEXECUTETIME             "STATEMENTEXECUTETIME"              /*!<_*/
#define SV_STATEMENTSIZE                    "STATEMENTSIZE"                     /*!<_*/
#define SV_STATEMENTWAITABSOLUTETIME        "STATEMENTWAITABSOLUTETIME"         /*!<_*/
#define SV_STATEMENTWAITRELATIVETIME        "STATEMENTWAITRELATIVETIME"         /*!<_*/
#define SV_STATISTICSDELTACOUNT             "STATISTICSDELTACOUNT"              /*!<_*/
#define SV_SUBTRANSACTIONLEVEL              "SUBTRANSACTIONLEVEL"               /*!<_*/
#define SV_STATUS                           "STATUS"                            /*!<_*/
#define SV_SUCCESSFULACCESSCOUNT            "SUCCESSFULACCESSCOUNT"             /*!<_*/
#define SV_SUSPENDABSOLUTETIME              "SUSPENDABSOLUTETIME"               /*!<_*/
#define SV_SUSPENDCOUNT                     "SUSPENDCOUNT"                      /*!<_*/
#define SV_SUSPENDRELATIVETIME              "SUSPENDRELATIVETIME"               /*!<_*/
#define SV_SUSPENDTIMEMEASUREMENTCOUNT      "SUSPENDTIMEMEASUREMENTCOUNT"       /*!<_*/
#define SV_SUCCESSFULSYNCHRONIZEDATE        "SUCCESSFULSYNCHRONIZEDATE"         /*!<_*/
#define SV_SYNCHRONIZEDATE                  "SYNCHRONIZEDATE"                   /*!<_*/
#define SV_SYNCHRONIZERECEIVECOUNT          "SYNCHRONIZERECEIVECOUNT"           /*!<_*/
#define SV_SYNCHRONIZERETRYCOUNT            "SYNCHRONIZERETRYCOUNT"             /*!<_*/
#define SV_SYNCHRONIZESENDCOUNT             "SYNCHRONIZESENDCOUNT"              /*!<_*/
#define SV_TABLEID                          "TABLEID"                           /*!<_*/
#define SV_TABLELOCKCOUNT                   "TABLELOCKCOUNT"                    /*!<_*/
#define SV_TABLENAME                        "TABLENAME"                         /*!<_*/
#define SV_TABLEOWNER                       "TABLEOWNER"                        /*!<_*/
#define SV_TABLEREGIONCOUNT                 "TABLEREGIONCOUNT"                  /*!<_*/
#define SV_TAKEOVERDATE                     "TAKEOVERDATE"                      /*!<_*/
#define SV_TASKCLUSTER                      "TASKCLUSTER"                       /*!<_*/
#define SV_TASKCOUNT                        "TASKCOUNT"                         /*!<_*/
#define SV_TASKGROUPBLOCKED                 "TASKGROUPBLOCKED"                  /*!<_*/
#define SV_TASKGROUPID                      "TASKGROUPID"                       /*!<_*/
#define SV_TASKGROUPSLEEPTIME               "TASKGROUPSLEEPTIME"                /*!<_*/
#define SV_TASKID                           "TASKID"                            /*!<_*/
#define SV_TASKRESCEDULECOUNT               "TASKRESCEDULECOUNT"                /*!<_*/
#define SV_TASKSTATE                        "TASKSTATE"                         /*!<_*/
#define SV_TASKTYPE                         "TASKTYPE"                          /*!<_*/
#define SV_TERMINATECOUNT                   "TERMINATECOUNT"                    /*!<_*/
#define SV_THREADID                         "THREADID"                          /*!<_*/
#define SV_THREADNAME                       "THREADNAME"                        /*!<_*/
#define SV_TIMEOUT                          "TIMEOUT"                           /*!<_*/
#define SV_TIMEOUTEXCEPTIONCOUNT            "TIMEOUTEXCEPTIONCOUNT"             /*!<_*/
#define SV_TOTALCLUSTERAREASIZE             "TOTALCLUSTERAREASIZE"              /*!<_*/
#define SV_TOTALSIZE                        "TOTALSIZE"                         /*!<_*/
#define SV_TOTALTASKRUNNABLETIME            "TOTALTASKRUNNABLETIME"             /*!<_*/
#define SV_TRACE                            "TRACE"                             /*!<_*/
#define SV_TRANSACTIONCOUNT                 "TRANSACTIONCOUNT"                  /*!<_*/
#define SV_TRANSACTIONID                    "TRANSACTIONID"                     /*!<_*/
#define SV_TRANSACTIONREGIONCOUNT           "TRANSACTIONREGIONCOUNT"            /*!<_*/
#define SV_TREEINDEXSIZE                    "TREEINDEXSIZE"                     /*!<_*/
#define SV_TREELEAVESSIZE                   "TREELEAVESSIZE"                    /*!<_*/
#define SV_TREELEVELCOUNT                   "TREELEVELCOUNT"                    /*!<_*/
#define SV_TYPE                             "TYPE"                              /*!<_*/
#define SV_UNLOADSTATEMENTCOUNT             "UNLOADSTATEMENTCOUNT"              /*!<_*/
#define SV_UNSUCCESSFULACCESSCOUNT          "UNSUCCESSFULACCESSCOUNT"           /*!<_*/
#define SV_UNUSEDSIZE                       "UNUSEDSIZE"                        /*!<_*/
#define SV_UPDATECACHEDOBJECTCOUNT          "UPDATECACHEDOBJECTCOUNT"           /*!<_*/
#define SV_UPDATEOBJECTCOUNT                "UPDATEOBJECTCOUNT"                 /*!<_*/
#define SV_UPDATEVAROBJECTCOUNT             "UPDATEVAROBJECTCOUNT"              /*!<_*/
#define SV_USABLESIZE                       "USABLESIZE"                        /*!<_*/
#define SV_USABLESTACKSIZE                  "USABLESTACKSIZE"                   /*!<_*/
#define SV_USEDCLUSTERAREASIZE              "USEDCLUSTERAREASIZE"               /*!<_*/
#define SV_USEDDATASIZE                     "USEDDATASIZE"                      /*!<_*/
#define SV_USEDENTRYCOUNT                   "USEDENTRYCOUNT"                    /*!<_*/
#define SV_USEDENTRYCOUNTPERCENTAGE         "USEDENTRYCOUNTPERCENTAGE"          /*!<_*/
#define SV_USEDHASHTABLESLOTCOUNT           "USEDHASHTABLESLOTCOUNT"            /*!<_*/
#define SV_USEDPAGECOUNT                    "USEDPAGECOUNT"                     /*!<_*/
#define SV_USEDPERMANENTSIZE                "USEDPERMANENTSIZE"                 /*!<_*/
#define SV_USEDRUNQUEUELENGTH               "USEDRUNQUEUELENGTH"                /*!<_*/
#define SV_USEDSIZE                         "USEDSIZE"                          /*!<_*/
#define SV_USEDSIZENOTONVOLUMES             "USEDSIZENOTONVOLUMES"              /*!<_*/
#define SV_USEDSIZEONVOLUMES                "USEDSIZEONVOLUMES"                 /*!<_*/
#define SV_USEDSIZEPERCENTAGE               "USEDSIZEPERCENTAGE"                /*!<_*/
#define SV_USEDSTACKSIZE                    "USEDSTACKSIZE"                     /*!<_*/
#define SV_USEDSYSTEMTIME                   "USEDSYSTEMTIME"                    /*!<_*/
#define SV_USEDTEMPORARYSIZE                "USEDTEMPORARYSIZE"                 /*!<_*/
#define SV_USEDUSERTIME                     "USEDUSERTIME"                      /*!<_*/
#define SV_USERID                           "USERID"                            /*!<_*/
#define SV_USERNAME                         "USERNAME"                          /*!<_*/
#define SV_VALUE                            "VALUE"                             /*!<_*/
#define SV_VALUEEQUALITY                    "VALUEEQUALITY"                     /*!<_*/
#define SV_VAROBJECTCONTAINER               "VAROBJECTCONTAINER"                /*!<_*/
#define SV_VIRTUALREADCOUNT                 "VIRTUALREADCOUNT"                  /*!<_*/
#define SV_VIRTUALMEMORYSIZE                "VIRTUALMEMORYSIZE"                 /*!<_*/
#define SV_VIRTUALMEMORYUSABLE              "VIRTUALMEMORYUSABLE"               /*!<_*/
#define SV_VOLUMEID                         "VOLUMEID"                          /*!<_*/
#define SV_WAITABSOLUTETIME                 "WAITABSOLUTETIME"                  /*!<_*/
#define SV_WAITCOUNT                        "WAITCOUNT"                         /*!<_*/
#define SV_WAITINGFORPAGE                   "WAITINGFORPAGE"                    /*!<_*/
#define SV_WAITINGFORROOT                   "WAITINGFORROOT"                    /*!<_*/
#define SV_WAITINGFORTASK                   "WAITINGFORTASK"                    /*!<_*/
#define SV_WAITINGTASKID                    "WAITINGTASKID"                     /*!<_*/
#define SV_WAITRELATIVETIME                 "WAITRELATIVETIME"                  /*!<_*/
#define SV_WAITTIMEMEASUREMENTCOUNT         "WAITTIMEMEASUREMENTCOUNT"          /*!<_*/
#define SV_WRITEABSOLUTETIME                "WRITEABSOLUTETIME"                 /*!<_*/
#define SV_WRITECOUNT                       "WRITECOUNT"                        /*!<_*/
#define SV_WRITEPAGECOUNT                   "WRITEPAGECOUNT"                    /*!<_*/
#define SV_WRITERELATIVETIME                "WRITERELATIVETIME"                 /*!<_*/
#define SV_WRITETIME                        "WRITETIME"                         /*!<_*/
#define SV_WRITETIMEMEASUREMENTCOUNT        "WRITETIMEMEASUREMENTCOUNT"         /*!<_*/
#define SV_WRITETRANSACTIONCOUNT            "WRITETRANSACTIONCOUNT"             /*!<_*/
#define SV_WRITETRANSACTIONID               "WRITETRANSACTIONID"                /*!<_*/
#define SV_YIELDLOOPCOUNT                   "YIELDLOOPCOUNT"                    /*!<_*/

// 371 unique definitions found.

// Table column counts:

#define SV_CC_ACTIVECONFIGURATION                4 /*!<_*/
#define SV_CC_BACKTRACE                          4 /*!<_*/
#define SV_CC_BACKUPINFORMATION                  8 /*!<_*/
#define SV_CC_BACKUPTHREADS                     10 /*!<_*/
#define SV_CC_CACHESTATISTICS                    6 /*!<_*/
#define SV_CC_CACHESTATISTICSRESET               7 /*!<_*/
#define SV_CC_CLASSCONTAINERS                   11 /*!<_*/
#define SV_CC_CLASSCONTAINERSTATISTICS          14 /*!<_*/
#define SV_CC_CLASSCONTAINERSTATISTICSRESET     11 /*!<_*/
#define SV_CC_COMMANDCACHESTATISTICS            22 /*!<_*/
#define SV_CC_COMMANDCACHESTATISTICSRESET       15 /*!<_*/
#define SV_CC_COMMANDSTATISTICS                 34 /*!<_*/
#define SV_CC_COMMANDSTATISTICSRESET            15 /*!<_*/
#define SV_CC_DATACACHE                          9 /*!<_*/
#define SV_CC_DATASTATISTICS                    17 /*!<_*/
#define SV_CC_DATASTATISTICSRESET                5 /*!<_*/
#define SV_CC_DATAVOLUMES                       11 /*!<_*/
#define SV_CC_DIAGNOSE                           4 /*!<_*/
#define SV_CC_EVENTS                             6 /*!<_*/
#define SV_CC_FILEDIRECTORIES                   10 /*!<_*/
#define SV_CC_FILES                              8 /*!<_*/
#define SV_CC_HISTORYCLEANUPSTATISTICS          14 /*!<_*/
#define SV_CC_HISTORYCLEANUPSTATISTICSRESET     14 /*!<_*/
#define SV_CC_HOTSTANDBYCOMPONENT               14 /*!<_*/
#define SV_CC_HOTSTANDBYGROUP                   12 /*!<_*/
#define SV_CC_INDEXSTORAGEDETAILS               17 /*!<_*/
#define SV_CC_INSTANCE                           5 /*!<_*/
#define SV_CC_IOBUFFERCACHES                     9 /*!<_*/
#define SV_CC_IOTHREADSTATISTICS                12 /*!<_*/
#define SV_CC_IOTHREADSTATISTICSRESET           13 /*!<_*/
#define SV_CC_KERNELMESSAGES                     2 /*!<_*/
#define SV_CC_LOCKS                             12 /*!<_*/
#define SV_CC_LOCKSTATISTICS                    21 /*!<_*/
#define SV_CC_LOCKSTATISTICSRESET               20 /*!<_*/
#define SV_CC_LOGINFORMATION                     7 /*!<_*/
#define SV_CC_LOGSTATISTICS                     12 /*!<_*/
#define SV_CC_LOGSTATISTICSRESET                 4 /*!<_*/
#define SV_CC_LOGQUEUESTATISTICS                 8 /*!< PTS 1132161 mb */
#define SV_CC_LOGQUEUESTATISTICSRESET            9 /*!< PTS 1132161 mb */
#define SV_CC_LOGVOLUMES                         5 /*!<_*/
#define SV_CC_MACHINECONFIGURATION               4 /*!<_*/
#define SV_CC_MACHINEUTILIZATION                 7 /*!<_*/
#define SV_CC_MEMORYALLOCATORSTATISTICS         11 /*!<_*/
#define SV_CC_MEMORYALLOCATORSTATISTICSRESET     9 /*!<_*/
#define SV_CC_MEMORYHOLDERS                      3 /*!<_*/
#define SV_CC_MISCELLANEOUS                      5 /*!<_*/
#define SV_CC_OBJECTLOCKS                        5 /*!<_*/
#define SV_CC_OMSDIAGNOSE                        3 /*!<_*/ 
#define SV_CC_OMSLOCKS                           5 /*!<_*/
#define SV_CC_OMSPROCEDURESTATISTICS            54 /*!<_*/
#define SV_CC_OMSPROCEDURESTATISTICSRESET       55 /*!<_*/
#define SV_CC_OMSVERSIONS                        7 /*!<_*/
#define SV_CC_OPTIMIZERINFORMATION               6 /*!<_*/
#define SV_CC_PAGELOCKS                          4 /*!<_*/
#define SV_CC_READERWRITERLOCKINFORMATION        4 /*!<_*/
#define SV_CC_READERWRITERLOCKSTATISTICS         6 /*!<_*/
#define SV_CC_READERWRITERLOCKSTATISTICSRESET    7 /*!<_*/
#define SV_CC_READERWRITERLOCKWAITINGTASKS       2 /*!<_*/
#define SV_CC_REGIONINFORMATION                  3 /*!<_*/
#define SV_CC_REGIONSTATISTICS                   9 /*!<_*/
#define SV_CC_REGIONSTATISTICSRESET              7 /*!<_*/
#define SV_CC_RESTARTINFORMATION                24 /*!<_*/
#define SV_CC_SCHEMAS                            5 /*!<_*/
#define SV_CC_SERVERTASKS                        4 /*!<_*/
#define SV_CC_SESSIONS                          22 /*!<_*/
#define SV_CC_SNAPSHOTS                          3 /*!<_*/
#define SV_CC_SPINLOCKPOOLSTATISTICS            18 /*!<_*/
#define SV_CC_SPINLOCKPOOLSTATISTICSRESET       19 /*!<_*/
#define SV_CC_SPINLOCKSTATISTICS                 7 /*!<_*/
#define SV_CC_SPINLOCKSTATISTICSRESET            8 /*!<_*/
#define SV_CC_TABLESTORAGEDETAILS               21 /*!<_*/
#define SV_CC_TASKGROUPSTATISTICS               11 /*!<_*/
#define SV_CC_TASKGROUPSTATISTICSRESET           9 /*!<_*/
#define SV_CC_TASKLOADBALANCINGINFORMATION       5 /*!<_*/
#define SV_CC_TASKLOADBALANCINGTASKGROUPSTATES   7 /*!<_*/
#define SV_CC_TASKLOADBALANCINGTASKMOVES         7 /*!<_*/
#define SV_CC_TASKSTATISTICS                    55 /*!<_*/
#define SV_CC_TASKSTATISTICSRESET               37 /*!<_*/
#define SV_CC_TRANSACTIONS                      15 /*!<_*/
#define SV_CC_TRANSACTIONHISTORY                 7 /*!<_*/
#define SV_CC_UNLOADEDSTATEMENTS                 2 /*!<_*/
#define SV_CC_VERSION                            9 /*!<_*/

// Table estimated rows:
#define SV_ER_ACTIVECONFIGURATION               300 /*!<_*/
#define SV_ER_BACKTRACE                           1 /*!<_*/
#define SV_ER_BACKUPINFORMATION                   1 /*!<_*/
#define SV_ER_BACKUPTHREADS                      10 /*!<_*/
#define SV_ER_CACHESTATISTICS                     5 /*!<_*/
#define SV_ER_CACHESTATISTICSRESET                5 /*!<_*/
#define SV_ER_CLASSCONTAINERS                   255 /*!<_*/
#define SV_ER_CLASSCONTAINERSTATISTICS          255 /*!<_*/
#define SV_ER_CLASSCONTAINERSTATISTICSRESET     255 /*!<_*/
#define SV_ER_COMMANDCACHESTATISTICS              1 /*!<_*/
#define SV_ER_COMMANDCACHESTATISTICSRESET         1 /*!<_*/
#define SV_ER_COMMANDSTATISTICS                5000 /*!<_*/
#define SV_ER_COMMANDSTATISTICSRESET           5000 /*!<_*/
#define SV_ER_DATACACHE                           5 /*!<_*/
#define SV_ER_DATASTATISTICS                      5 /*!<_*/
#define SV_ER_DATASTATISTICSRESET                 1 /*!<_*/
#define SV_ER_DATAVOLUMES                        10 /*!<_*/
#define SV_ER_DIAGNOSE                           50 /*!<_*/
#define SV_ER_EVENTS                             85 /*!<_*/
#define SV_ER_FILEDIRECTORIES                    25 /*!<_*/
#define SV_ER_FILES                           50000 /*!<_*/
#define SV_ER_HISTORYCLEANUPSTATISTICS            5 /*!<_*/
#define SV_ER_HISTORYCLEANUPSTATISTICSRESET       5 /*!<_*/
#define SV_ER_HOTSTANDBYCOMPONENT                 1 /*!<_*/
#define SV_ER_HOTSTANDBYGROUP                     1 /*!<_*/
#define SV_ER_INDEXSTORAGEDETAILS              3000 /*!<_*/
#define SV_ER_INSTANCE                            1 /*!<_*/
#define SV_ER_IOBUFFERCACHES                      1 /*!<_*/
#define SV_ER_IOTHREADSTATISTICS                 10 /*!<_*/
#define SV_ER_IOTHREADSTATISTICSRESET            10 /*!<_*/
#define SV_ER_KERNELMESSAGES                    520 /*!<_*/
#define SV_ER_LOCKS                            1000 /*!<_*/
#define SV_ER_LOCKSTATISTICS                      1 /*!<_*/
#define SV_ER_LOCKSTATISTICSRESET                 1 /*!<_*/
#define SV_ER_LOGINFORMATION                      1 /*!<_*/
#define SV_ER_LOGSTATISTICS                       1 /*!<_*/
#define SV_ER_LOGSTATISTICSRESET                  1 /*!<_*/
#define SV_ER_LOGVOLUMES                          2 /*!<_*/
#define SV_ER_MACHINECONFIGURATION                1 /*!<_*/
#define SV_ER_MACHINEUTILIZATION                  1 /*!<_*/
#define SV_ER_MEMORYALLOCATORSTATISTICS         100 /*!<_*/
#define SV_ER_MEMORYALLOCATORSTATISTICSRESET    100 /*!<_*/
#define SV_ER_MEMORYHOLDERS                     500 /*!<_*/
#define SV_ER_MISCELLANEOUS                      10 /*!<_*/
#define SV_ER_OBJECTLOCKS                         1 /*!<_*/
#define SV_ER_OMSDIAGNOSE                       100 /*!<_*/
#define SV_ER_OMSLOCKS                          100 /*!<_*/
#define SV_ER_OMSPROCEDURESTATISTICS            100 /*!<_*/
#define SV_ER_OMSPROCEDURESTATISTICSRESET       100 /*!<_*/
#define SV_ER_OMSVERSIONS                       100 /*!<_*/
#define SV_ER_OPTIMIZERINFORMATION            15000 /*!<_*/
#define SV_ER_PAGELOCKS                          50 /*!<_*/
#define SV_ER_READERWRITERLOCKINFORMATION         1 /*!<_*/
#define SV_ER_READERWRITERLOCKSTATISTICS          1 /*!<_*/
#define SV_ER_READERWRITERLOCKSTATISTICSRESET     1 /*!<_*/
#define SV_ER_READERWRITERLOCKWAITINGTASKS        1 /*!<_*/
#define SV_ER_REGIONINFORMATION                 100 /*!<_*/
#define SV_ER_REGIONSTATISTICS                   25 /*!<_*/
#define SV_ER_REGIONSTATISTICSRESET              25 /*!<_*/
#define SV_ER_RESTARTINFORMATION                  1 /*!<_*/
#define SV_ER_SCHEMAS                            20 /*!<_*/
#define SV_ER_SNAPSHOTS                           5 /*!<_*/
#define SV_ER_SERVERTASKS                       150 /*!<_*/
#define SV_ER_SESSIONS                           50 /*!<_*/
#define SV_ER_SPINLOCKPOOLSTATISTICS             20 /*!<_*/
#define SV_ER_SPINLOCKPOOLSTATISTICSRESET        20 /*!<_*/
#define SV_ER_SPINLOCKSTATISTICS                 20 /*!<_*/
#define SV_ER_SPINLOCKSTATISTICSRESET            20 /*!<_*/
#define SV_ER_TABLESTORAGEDETAILS             12000 /*!<_*/
#define SV_ER_TASKGROUPSTATISTICS                10 /*!<_*/
#define SV_ER_TASKGROUPSTATISTICSRESET           10 /*!<_*/
#define SV_ER_TASKLOADBALANCINGINFORMATION        1 /*!<_*/
#define SV_ER_TASKLOADBALANCINGTASKGROUPSTATES   10 /*!<_*/
#define SV_ER_TASKLOADBALANCINGTASKMOVES         10 /*!<_*/
#define SV_ER_TASKSTATISTICS                     50 /*!<_*/
#define SV_ER_TASKSTATISTICSRESET                50 /*!<_*/
#define SV_ER_TRANSACTIONS                       50 /*!<_*/
#define SV_ER_TRANSACTIONHISTORY                 50 /*!<_*/
#define SV_ER_UNLOADEDSTATEMENTS               5000 /*!<_*/
#define SV_ER_VERSION                             1 /*!<_*/

/*!
    @brief   defines the kind of data used to fill a column
    of a SystemView
*/
typedef enum
{
    ITOVT_NULL,
    ITOVT_INT2,
    ITOVT_INT4,
    ITOVT_UINT4,
    ITOVT_INT8,
    ITOVT_UINT8,
    ITOVT_CHARPTR,
    ITOVT_UNICODEPTR,
    ITOVT_UTF8PTR,
    ITOVT_TRANSNO,
    ITOVT_LOCKREQMODE,
    ITOVT_LOCKREQSTATE,
    ITOVT_SURROGATE,
    ITOVT_LONGCOLREF,
    ITOVT_KNLIDENTIFIER,
    ITOVT_TRANSLOCKMODE,
    ITOVT_PARSEID,
    ITOVT_SQLMODE,
    ITOVT_DATETIMEFORMAT,
    ITOVT_BOOL,
    ITOVT_YESNO,
    ITOVT_ONOFF,
    ITOVT_USER,
    ITOVT_SCHEMA,
    ITOVT_DBPROC,
    ITOVT_CODETYPE,
	ITOVT_QUERYREWRITEMODE
}
SysView_ITOVarType;

/*!
    @brief   defines the dataypes which can be used for columns
    of SystemViews
*/

typedef enum
{
    ITOCT_CHAR,
    ITOCT_CHARBYTE,
    ITOCT_CHARUNICODE,
    ITOCT_FIXED,
    ITOCT_LONGASCII,
    ITOCT_LONGUNICODE,
    ITOCT_TIMESTAMP
}
SysView_ITOColType;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



#endif  /* SYSVIEW_DEFINES_HPP */

