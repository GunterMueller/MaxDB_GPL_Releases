/*!
  -----------------------------------------------------------------------------

  module: gcn00_1.h

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBM Error Messages

  description:  specification of dbm error messages

  version:      min 7.2.

  Copyright (c) 1998-2005 SAP AG

  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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



  -----------------------------------------------------------------------------
*/


#ifndef _GCN00_1_H_
#define _GCN00_1_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Srv/Message/DBMSrvMsg_Info.hpp"
#include "hin100.h"

/*
  -----------------------------------------------------------------------------
  Definitions
  -----------------------------------------------------------------------------
*/

#define TERR_CN00_1               Msg_List::Error
#define TWRN_CN00_1               Msg_List::Warning
#define TINF_CN00_1               Msg_List::Info

#define DBM_CN00_1                "DBM"

#define N_CN00_1(_n)              STATIC_CAST(Msg_List::MessageID, (-25000+_n))

#define FUNCTION_DBG_MCN00_1(_s)   const char   * pName_cn00_1 = _s
#define FUNCTION_NAME_MCN00_1      pName_cn00_1

inline const Msg_List::MessageID cn00_1_ErrId
    ( const Msg_List::MessageID _a, 
      const Msg_List::MessageType _b, 
      const char   * const _c, 
      const char   * const _d ) 
{
  return _a;
} // end cn00_1_ErrId

inline const char   * cn00_1_ErrTxt
    ( const Msg_List::MessageID _a, 
      const Msg_List::MessageType _b, 
      const char   * const _c, 
      const char   * const _d ) 
{
  return _d;
} // end cn00_1_ErrTxt

/*
  -----------------------------------------------------------------------------
  Error Codes
  -----------------------------------------------------------------------------
*/
#define  OK_CN00_1                                  0 , TINF_CN00_1, DBM_CN00_1, "OK: everything works fine"

#define  ERR_CN00_1                     N_CN00_1(   1), TERR_CN00_1, DBM_CN00_1, "ERR: common error"
//#define  ERR_NOTIMPL_CN00_1             N_CN00_1(   2), TERR_CN00_1, DBM_CN00_1, "ERR_NOTIMPL: sorry, command not yet implemented"
#define  ERR_FILE_CN00_1                N_CN00_1(   3), TERR_CN00_1, DBM_CN00_1, "ERR_FILE: file handling error"
#define  ERR_PARAM_CN00_1               N_CN00_1(   4), TERR_CN00_1, DBM_CN00_1, "ERR_PARAM: wrong parameters"
#define  ERR_MEM_CN00_1                 N_CN00_1(   5), TERR_CN00_1, DBM_CN00_1, "ERR_MEM: memory allocation error"
//#define  ERR_RTE_CN00_1                 N_CN00_1(   6), TERR_CN00_1, DBM_CN00_1, "ERR_RTE: runtime environment error"
//#define  ERR_RTEEXT_CN00_1              N_CN00_1(   7), TERR_CN00_1, DBM_CN00_1, "ERR_RTEEXT: runtime environment error"
//#define  ERR_NOUTILSESSION_CN00_1       N_CN00_1(   8), TERR_CN00_1, DBM_CN00_1, "ERR_NOUTILSESSION: no utility session available"
//#define  ERR_NOSQLUSERSESSION_CN00_1    N_CN00_1(   9), TERR_CN00_1, DBM_CN00_1, "ERR_NOSQLUSERSESSION: no sql user session available"
//#define  ERR_NOBACKUPSESSION_CN00_1     N_CN00_1(  10), TERR_CN00_1, DBM_CN00_1, "ERR_NOBACKUPSESSION: no backup session available"
#define  ERR_NOHISTORYSESSION_CN00_1    N_CN00_1(  11), TERR_CN00_1, DBM_CN00_1, "ERR_NOHISTORYSESSION: no backup history session available"
//#define  ERR_SQL_CN00_1                 N_CN00_1(  12), TERR_CN00_1, DBM_CN00_1, "ERR_SQL: sql error"
#define  ERR_TOOSMALL_CN00_1            N_CN00_1(  13), TERR_CN00_1, DBM_CN00_1, "ERR_TOOSMALL: packet size too small for answer"
#define  ERR_NOMEDIA_CN00_1             N_CN00_1(  14), TERR_CN00_1, DBM_CN00_1, "ERR_NOMEDIA: media file not found"
#define  ERR_MENOTFOUND_CN00_1          N_CN00_1(  15), TERR_CN00_1, DBM_CN00_1, "ERR_MENOTFOUND: medium not found"
#define  ERR_MENOVALID_CN00_1           N_CN00_1(  16), TERR_CN00_1, DBM_CN00_1, "ERR_MENOVALID: invalid medium definition"
#define  ERR_BANOVALIDTYPE_CN00_1       N_CN00_1(  17), TERR_CN00_1, DBM_CN00_1, "ERR_BANOVALIDTYPE: invalid backup type"
#define  ERR_NOHISTORY_CN00_1           N_CN00_1(  18), TERR_CN00_1, DBM_CN00_1, "ERR_NOHISTORY: backup history file not found"
//#define  ERR_HIENTRYNOTFOUND_CN00_1     N_CN00_1(  19), TERR_CN00_1, DBM_CN00_1, "ERR_HIENTRYNOTFOUND: backup history entry not found"
//#define  ERR_NOHELP_CN00_1              N_CN00_1(  20), TERR_CN00_1, DBM_CN00_1, "ERR_NOHELP: xparam help file not found"
#define  ERR_XPNOTFOUND_CN00_1          N_CN00_1(  21), TERR_CN00_1, DBM_CN00_1, "ERR_XPNOTFOUND: parameter not found"
#define  ERR_XPSYNTAX_CN00_1            N_CN00_1(  22), TERR_CN00_1, DBM_CN00_1, "ERR_XPSYNTAX: xparam syntax error"
//#define  ERR_COMMAND_CN00_1             N_CN00_1(  23), TERR_CN00_1, DBM_CN00_1, "ERR_COMMAND: unknown command \"%s\""
#define  ERR_PARAMEXIST_CN00_1          N_CN00_1(  24), TERR_CN00_1, DBM_CN00_1, "ERR_PARAMEXIST: param file of database already exists"
#define  ERR_NOXPSESSION_CN00_1         N_CN00_1(  25), TERR_CN00_1, DBM_CN00_1, "ERR_NOXPSESSION: param session inactive"
#define  ERR_XPSESSION_CN00_1           N_CN00_1(  26), TERR_CN00_1, DBM_CN00_1, "ERR_XPSESSION: param session active"
#define  ERR_XPCHECK_CN00_1             N_CN00_1(  27), TERR_CN00_1, DBM_CN00_1, "ERR_XPCHECK: param check failure/request"
//#define  ERR_XPINVCONF_CN00_1           N_CN00_1(  28), TERR_CN00_1, DBM_CN00_1, "ERR_XPINFCONV: invalid value"
//#define  ERR_XPNOCHECK_CN00_1           N_CN00_1(  29), TERR_CN00_1, DBM_CN00_1, "ERR_NOCHECK: no running parameter check"
//#define  ERR_XPNOMODIFY_CN00_1          N_CN00_1(  30), TERR_CN00_1, DBM_CN00_1, "ERR_XPNOMODIFY: parameter must not be modified"
#define  ERR_XPDIRTY_CN00_1             N_CN00_1(  31), TERR_CN00_1, DBM_CN00_1, "ERR_XPDIRTY: param file is dirty"
#define  ERR_XPREADONLY_CN00_1          N_CN00_1(  32), TERR_CN00_1, DBM_CN00_1, "ERR_XPREADONLY: parameter is read only"
#define  ERR_SYSTEM_CN00_1              N_CN00_1(  33), TERR_CN00_1, DBM_CN00_1, "ERR_SYSTEM: system call error"
//#define  ERR_LOGON_CN00_1               N_CN00_1(  34), TERR_CN00_1, DBM_CN00_1, "ERR_LOGON: logon to DBM Server failed"
//#define  ERR_SYSLOGON_CN00_1            N_CN00_1(  35), TERR_CN00_1, DBM_CN00_1, "ERR_SYSLOGON: logon to system failed"
#define  ERR_EXECUTE_CN00_1             N_CN00_1(  36), TERR_CN00_1, DBM_CN00_1, "ERR_EXECUTE: error in program execution"
//#define  ERR_VOLCHECK_CN00_1            N_CN00_1(  37), TERR_CN00_1, DBM_CN00_1, "ERR_VOLCHECK: error while checking volume"
//#define  ERR_VOLTYPE_CN00_1             N_CN00_1(  38), TERR_CN00_1, DBM_CN00_1, "ERR_VOLTYPE: wrong volume type"
#define  ERR_STATE_CN00_1               N_CN00_1(  39), TERR_CN00_1, DBM_CN00_1, "ERR_STATE: cannot determine current db state"
#define  ERR_DBRUN_CN00_1               N_CN00_1(  40), TERR_CN00_1, DBM_CN00_1, "ERR_DBRUN: database is running"
#define  ERR_XPOVERFLOW_CN00_1          N_CN00_1(  41), TERR_CN00_1, DBM_CN00_1, "ERR_XPOVERFLOW: value out of margin"
#define  ERR_BAOVERWRITE_CN00_1         N_CN00_1(  42), TERR_CN00_1, DBM_CN00_1, "ERR_BAOVERWRITE: file already exists, cannot overwrite"
#define  ERR_NOUSER_CN00_1              N_CN00_1(  43), TERR_CN00_1, DBM_CN00_1, "ERR_NOUSER: required user not set"
#define  ERR_BASAVETYPECON_CN00_1       N_CN00_1(  44), TERR_CN00_1, DBM_CN00_1, "ERR_BASAVETYPECON: backup type conflicts with medium definition"
#define  ERR_BANOSAVETYPE_CN00_1        N_CN00_1(  45), TERR_CN00_1, DBM_CN00_1, "ERR_BANOSAVETYPE: backup type not defined"
#define  ERR_BANOAUTOMED_CN00_1         N_CN00_1(  46), TERR_CN00_1, DBM_CN00_1, "ERR_BANOAUTOMED: no autolog medium found in media list"
#define  ERR_DBEXIST_CN00_1             N_CN00_1(  47), TERR_CN00_1, DBM_CN00_1, "ERR_DBEXIST: database instance already exists"
//#define  ERR_USRSAVE_CN00_1             N_CN00_1(  48), TERR_CN00_1, DBM_CN00_1, "ERR_USRSAVE: could not save user data"
#define  ERR_USRREAD_CN00_1             N_CN00_1(  49), TERR_CN00_1, DBM_CN00_1, "ERR_USRREAD: could not read user data"
#define  ERR_USRFAIL_CN00_1             N_CN00_1(  50), TERR_CN00_1, DBM_CN00_1, "ERR_USRFAIL: user authorization failed"
#define  ERR_USREXISTS_CN00_1           N_CN00_1(  51), TERR_CN00_1, DBM_CN00_1, "ERR_USREXISTS: user entry already exists"
#define  ERR_NODBNAME_CN00_1            N_CN00_1(  52), TERR_CN00_1, DBM_CN00_1, "ERR_NODBNAME: database name required"
#define  ERR_DBNAME_CN00_1              N_CN00_1(  53), TERR_CN00_1, DBM_CN00_1, "ERR_DBNAME: do not use dbname"
#define  ERR_TOOMANYPARAM_CN00_1        N_CN00_1(  54), TERR_CN00_1, DBM_CN00_1, "ERR_TOOMANYPARAM: too many parameters"
#define  ERR_TOOFEWPARAM_CN00_1         N_CN00_1(  55), TERR_CN00_1, DBM_CN00_1, "ERR_TOOFEWPARAM: too few parameters"
#define  ERR_USRDELSELF_CN00_1          N_CN00_1(  56), TERR_CN00_1, DBM_CN00_1, "ERR_USRDELSELF: cannot delete current user"
#define  ERR_USRDELDBM_CN00_1           N_CN00_1(  57), TERR_CN00_1, DBM_CN00_1, "ERR_USRDELDBM: cannot delete DBM user"
#define  ERR_USRDELSDBA_CN00_1          N_CN00_1(  58), TERR_CN00_1, DBM_CN00_1, "ERR_USRDELSDBA: cannot delete sysdba"
#define  ERR_NEEDLOGON_CN00_1           N_CN00_1(  59), TERR_CN00_1, DBM_CN00_1, "ERR_NEEDLOGON: DBM Server logon required"
//#define  ERR_NEEDSYSLOGON_CN00_1        N_CN00_1(  60), TERR_CN00_1, DBM_CN00_1, "ERR_NEEDSYSLOGON: system logon required"
//#define  ERR_LASTREG_CN00_1             N_CN00_1(  61), TERR_CN00_1, DBM_CN00_1, "ERR_LASTREG: cannot delete last registration of instance"
//#define  ERR_NOENTRY_CN00_1             N_CN00_1(  62), TERR_CN00_1, DBM_CN00_1, "ERR_NOENTRY: no entry found"
#define  ERR_MISSRIGHT_CN00_1           N_CN00_1(  63), TERR_CN00_1, DBM_CN00_1, "ERR_MISSRIGHT: no server rights for this command"
#define  ERR_DBNORUN_CN00_1             N_CN00_1(  64), TERR_CN00_1, DBM_CN00_1, "ERR_DBNORUN: database is not running"
#define  ERR_NOUSERS_CN00_1             N_CN00_1(  65), TERR_CN00_1, DBM_CN00_1, "ERR_NOUSERS: no users found"
#define  ERR_VERSION_CN00_1             N_CN00_1(  66), TERR_CN00_1, DBM_CN00_1, "ERR_VERSION: incompatible versions (param and kernel)"
//#define  ERR_NOSERVICESESSION_CN00_1    N_CN00_1(  67), TERR_CN00_1, DBM_CN00_1, "ERR_NOSERVICESESSION: no service kernel session available"
#define  ERR_XPMIN_CN00_1               N_CN00_1(  68), TERR_CN00_1, DBM_CN00_1, "ERR_XPMIN: parameter value smaller than minimum allowed"
#define  ERR_XPMAX_CN00_1               N_CN00_1(  69), TERR_CN00_1, DBM_CN00_1, "ERR_XPMAX: parameter value higher than maximum allowed"
#define  ERR_XPVALUESET_CN00_1          N_CN00_1(  70), TERR_CN00_1, DBM_CN00_1, "ERR_XPVALUESET: parameter value not in value set"
#define  ERR_USRFRM_CN00_1              N_CN00_1(  71), TERR_CN00_1, DBM_CN00_1, "ERR_USRFRM: invalid user record format"
#define  ERR_NOREPLY_CN00_1             N_CN00_1(  72), TERR_CN00_1, DBM_CN00_1, "ERR_NOREPLY: no reply available"
#define  ERR_TOOLCHK_CN00_1             N_CN00_1(  73), TERR_CN00_1, DBM_CN00_1, "ERR_TOOLCHK: external backup tool not found"
#define  ERR_MEDIUMCHK_CN00_1           N_CN00_1(  74), TERR_CN00_1, DBM_CN00_1, "ERR_MEDIUMCHK: medium cannot be used with this external backup tool"
#define  ERR_PREPARE_CN00_1             N_CN00_1(  75), TERR_CN00_1, DBM_CN00_1, "ERR_PREPARE: preparation of backup operation failed"
#define  ERR_DBREQ_CN00_1               N_CN00_1(  76), TERR_CN00_1, DBM_CN00_1, "ERR_DBREQ: cannot start database kernel request"
#define  ERR_TOOLREQ_CN00_1             N_CN00_1(  77), TERR_CN00_1, DBM_CN00_1, "ERR_TOOLREQ: error while starting external backup tool"
#define  ERR_OPCHK_CN00_1               N_CN00_1(  78), TERR_CN00_1, DBM_CN00_1, "ERR_OPCHK: cannot check state of backup operation"
#define  ERR_POSTOP_CN00_1              N_CN00_1(  79), TERR_CN00_1, DBM_CN00_1, "ERR_POSTOP: error while finishing backup operation "
#define  ERR_BACKUPOP_CN00_1            N_CN00_1(  80), TERR_CN00_1, DBM_CN00_1, "ERR_BACKUPOP: backup operation was unsuccessful"
#define  ERR_CLEANUP_CN00_1             N_CN00_1(  81), TERR_CN00_1, DBM_CN00_1, "ERR_CLEANUP: cannot free all operational ressources after backup operation"
#define  ERR_MISSMIRR_CN00_1            N_CN00_1(  82), TERR_CN00_1, DBM_CN00_1, "ERR_MISSMIRR: missing data for mirror volume"
//#define  ERR_MAXDATA_CN00_1             N_CN00_1(  83), TERR_CN00_1, DBM_CN00_1, "ERR_MAXDATA: max data size exceeded"
//#define  ERR_MAXVOLCNT_CN00_1           N_CN00_1(  84), TERR_CN00_1, DBM_CN00_1, "ERR_MAXVOLCNT: max volume count exceeded"
//#define  ERR_NOADDD_CN00_1              N_CN00_1(  85), TERR_CN00_1, DBM_CN00_1, "ERR_NOADDD: configuration update not allowed (param file is inactive)"
//#define  ERR_KEYEXISTS_CN00_1           N_CN00_1(  86), TERR_CN00_1, DBM_CN00_1, "ERR_KEYEXISTS: installation key \"%s\" exists"
//#define  ERR_SERVICEDB_CN00_1           N_CN00_1(  87), TERR_CN00_1, DBM_CN00_1, "ERR_SERVICEDB: cannot register service kernel for %s"
//#define  ERR_WRONGINST_CN00_1           N_CN00_1(  88), TERR_CN00_1, DBM_CN00_1, "ERR_WRONGINST: DBM Server instroot is %s"
//#define  ERR_INVDBNAME_CN00_1           N_CN00_1(  89), TERR_CN00_1, DBM_CN00_1, "ERR_INVDBNAME: database name %s is invalid"
//#define  ERR_PARAMVER_CN00_1            N_CN00_1(  90), TERR_CN00_1, DBM_CN00_1, "ERR_PARAMVER: wrong version of param help file (cserv.pcf):\n%s\n%s"
#define  ERR_DBAUNKNOWN_CN00_1          N_CN00_1(  91), TERR_CN00_1, DBM_CN00_1, "ERR_DBAUNKNOWN: SYSDBA unknown"
#define  ERR_DBAWRONG_CN00_1            N_CN00_1(  93), TERR_CN00_1, DBM_CN00_1, "ERR_DBAWRONG: wrong SYSDBA"
//#define  ERR_EBFDIRTY_CN00_1            N_CN00_1(  94), TERR_CN00_1, DBM_CN00_1, "ERR_EBFDIRTY: invalid format of external backup history"
#define  ERR_NOEBIDLIST_CN00_1          N_CN00_1(  95), TERR_CN00_1, DBM_CN00_1, "ERR_NOEBIDLIST: no information on external backup IDs available"
//#define  ERR_IMPERSONATE_CN00_1         N_CN00_1(  97), TERR_CN00_1, DBM_CN00_1, "ERR_IMPERSONATE: cannot impersonate to database owner"
#define  ERR_ASCII_CN00_1               N_CN00_1(  98), TERR_CN00_1, DBM_CN00_1, "ERR_ASCII: 7-Bit ASCII overflow in parameter"
#define  ERR_NOTADMIN_CN00_1            N_CN00_1(  99), TERR_CN00_1, DBM_CN00_1, "ERR_NOTADMIN: database have to be in operational state ADMIN"
//#define  ERR_TOOMANYFILES_CN00_1        N_CN00_1( 100), TERR_CN00_1, DBM_CN00_1, "ERR_TOOMANYFILES: too many files to list"
//#define  ERR_INVUSRNAME_CN00_1          N_CN00_1( 101), TERR_CN00_1, DBM_CN00_1, "ERR_INVUSRNAME: user name %s is reserved"
//#define  ERR_SQLREADONLY_CN00_1         N_CN00_1( 102), TERR_CN00_1, DBM_CN00_1, "ERR_SQLREADONLY: sql session is read only"
//#define  ERR_BADVERSION_CN00_1          N_CN00_1( 103), TERR_CN00_1, DBM_CN00_1, "ERR_BADVERSION: invalid version: %s"
//#define  ERR_MIGRATE_CN00_1             N_CN00_1( 104), TERR_CN00_1, DBM_CN00_1, "ERR_MIGRATE: no migration possible from %s"
//#define  ERR_SHUTDOWN_CN00_1            N_CN00_1( 105), TERR_CN00_1, DBM_CN00_1, "ERR_SHUTDOWN: shutdown of database occured"
//#define  ERR_KNLDIRTY_CN00_1            N_CN00_1( 106), TERR_CN00_1, DBM_CN00_1, "ERR_KNLDIRTY: the backup history file is corrupted"
//#define  ERR_MDFDIRTY_CN00_1            N_CN00_1( 107), TERR_CN00_1, DBM_CN00_1, "ERR_MDFDIRTY: the backup media history file is corrupted"
//#define  ERR_NOVOLDIR_CN00_1            N_CN00_1( 108), TERR_CN00_1, DBM_CN00_1, "ERR_NOVOLDIR: the directory %s does not exist"
//#define  ERR_NOVOLLINK_CN00_1           N_CN00_1( 109), TERR_CN00_1, DBM_CN00_1, "ERR_NOVOLLINK: the symbolic link %s does not exist"
#define  ERR_BAWRONGMEDIUM_CN00_1       N_CN00_1( 110), TERR_CN00_1, DBM_CN00_1, "ERR_BAWRONGMEDIUM: the backup medium can not be used for the command"
#define  ERR_BAWRONGSAVETYPE_CN00_1     N_CN00_1( 111), TERR_CN00_1, DBM_CN00_1, "ERR_BAWRONGSAVETYPE: medium has wrong save type"
#define  ERR_NOPREVSTAGEBACKUP_CN00_1   N_CN00_1( 112), TERR_CN00_1, DBM_CN00_1, "ERR_NOPREVSTAGEBACKUP: no stage files were backed up previously."
#define  ERR_MEEXISTSASGROUP_CN00_1     N_CN00_1( 113), TERR_CN00_1, DBM_CN00_1, "ERR_MEEXISTSASGROUP: a media group with that name is already existing, use another name for your medium."
#define  ERR_MEEXISTSASMEDIUM_CN00_1    N_CN00_1( 114), TERR_CN00_1, DBM_CN00_1, "ERR_MEEXISTSASMEDIUM: a medium with that name is already existing, use another name for your media group."
#define  ERR_MEAMBIGUOUS_CN00_1         N_CN00_1( 115), TERR_CN00_1, DBM_CN00_1, "ERR_MEAMBIGUOUS: the medium can not be used, because there exist a single medium and a media group with the same name."
//#define  ERR_UTLIOSEQNEED_CN00_1        N_CN00_1( 116), TERR_CN00_1, DBM_CN00_1, "ERR_UTLIOSEQNEED: invalid value (%s) for first needed IO sequence reported by database kernel"
//#define  ERR_UTLIOSEQAVAIL_CN00_1       N_CN00_1( 117), TERR_CN00_1, DBM_CN00_1, "ERR_UTLIOSEQAVAIL: invalid value (%s) for first available IO sequence reported by database kernel"
//#define  ERR_LOGFULL_CN00_1             N_CN00_1( 118), TERR_CN00_1, DBM_CN00_1, "ERR_LOGFULL: The log area is full."
//#define  ERR_NOHSS_CN00_1               N_CN00_1( 119), TERR_CN00_1, DBM_CN00_1, "ERR_NOHSS: The command %s is not available for this operation."
//#define  ERR_HSS_CN00_1                 N_CN00_1( 120), TERR_CN00_1, DBM_CN00_1, "ERR_HSS: hot standby error"
#define  ERR_NORESTORESTRATEGY_CN00_1   N_CN00_1( 121), TERR_CN00_1, DBM_CN00_1, "ERR_NORESTORESTRATEGY: Can not determine a restore strategy from the backup history."
#define  ERR_BACKNOTINHIST_CN00_1       N_CN00_1( 122), TERR_CN00_1, DBM_CN00_1, "ERR_BACKNOTINHIST: The backup can not be found in the backup history."
#define  ERR_NOTMASTER_CN00_1           N_CN00_1( 123), TERR_CN00_1, DBM_CN00_1, "ERR_NOMASTER: This is not a master instance."
//#define  ERR_NOVERSIONMATCH_CN00_1      N_CN00_1( 124), TERR_CN00_1, DBM_CN00_1, "ERR_NOVERSIONMATCH: No matching version found at node %s."
//#define  ERR_NEEDADMI_CN00_1            N_CN00_1( 125), TERR_CN00_1, DBM_CN00_1, "ERR_NEEDADMI: The operating system user is not a member of the database administrators group."
#define  ERR_SERVICEDBNOTFOUND_CN00_1   N_CN00_1( 126), TERR_CN00_1, DBM_CN00_1, "ERR_SERVICEDBNOTFOUND: The service database could not be found."
//#define  ERR_ADDVOLNAMETOOLONG_CN00_1   N_CN00_1( 127), TERR_CN00_1, DBM_CN00_1, "ERR_ADDVOLNAMETOOLONG: The name of the new data volume (%s%s%s) is too long. Volume not added."
//#define  ERR_NOEXPLAIN_CN00_1           N_CN00_1( 128), TERR_CN00_1, DBM_CN00_1, "ERR_NOEXPLAIN: The command \"%s\" has no explanation."
//#define  ERR_ILLEGALVOLSIZE_CN00_1      N_CN00_1( 129), TERR_CN00_1, DBM_CN00_1, "ERR_ILLEGALVOLSIZE: Value \"%s\" of parameter \"%s\" not good for calculating next new volume's size."
//#define  ERR_AUTOEXT_ILLPERC_CN00_1     N_CN00_1( 130), TERR_CN00_1, DBM_CN00_1, "ERR_AUTOEXT_ILLPERC: Percentage \"%s\" is illegal."
//#define  ERR_AUTOEXT_ILLMODE_CN00_1     N_CN00_1( 131), TERR_CN00_1, DBM_CN00_1, "ERR_AUTOEXT_ILLMODE: Mode \"%s\" is illegal."
//#define  ERR_SHMPANIC_CN00_1            N_CN00_1( 132), TERR_CN00_1, DBM_CN00_1, "ERR_SHMPANIC: DBM shared memory panic! file: %s line: %s."
//#define  ERR_SHMRESLOCK_CN00_1          N_CN00_1( 133), TERR_CN00_1, DBM_CN00_1, "ERR_SHMRESLOCK: Resource is locked: %s."
//#define  ERR_EVENTNONEAVAIL_CN00_1      N_CN00_1( 134), TERR_CN00_1, DBM_CN00_1, "ERR_EVENTNONEAVAIL: no event available."
//#define  ERR_ADDVOLWRONGMODE_CN00_1     N_CN00_1( 135), TERR_CN00_1, DBM_CN00_1, "ERR_ADDVOLWRONGMODE: Operation not supported for volume mode %s."
//#define  ERR_ADDVOLERRFDEGRANGE_CN00_1  N_CN00_1( 136), TERR_CN00_1, DBM_CN00_1, "ERR_ADDVOLERRFDEGRANGE: Specified filling degree %s is out of range."
//#define  ERR_ILLEGALTIMEFMT_CN00_1      N_CN00_1( 137), TERR_CN00_1, DBM_CN00_1, "ERR_ILLEGALTIMEFMT: Time string %s is not legal."
//#define  ERR_UNKNOWNINFOTYPE_CN00_1     N_CN00_1( 138), TERR_CN00_1, DBM_CN00_1, "ERR_UNKNOWNINFOTYPE: InfoType %s unknown, variable part is %s."
//#define  ERR_NOSCHEDULERSTOP_CN00_1     N_CN00_1( 139), TERR_CN00_1, DBM_CN00_1, "ERR_NOSCHEDULERSTOP: Could not stop scheduler (pid %s)."
//#define  ERR_NOSCHEDULERSTART_CN00_1    N_CN00_1( 140), TERR_CN00_1, DBM_CN00_1, "ERR_NOSCHEDULERSTART: Could not start scheduler."
//#define  ERR_NOTJOBOWNER_CN00_1         N_CN00_1( 141), TERR_CN00_1, DBM_CN00_1, "ERR_NOTJOBOWNER: You are not the owner of the job."
//efine  ERR_JOBNOTEXIST_CN00_1         N_CN00_1( 142), TERR_CN00_1, DBM_CN00_1, "ERR_JOBNOTEXIST: Job does not exist."
//#define  ERR_PREDECESSORNOTEXIST_CN00_1 N_CN00_1( 143), TERR_CN00_1, DBM_CN00_1, "ERR_PREDECESSORNOTEXIST: Predecessor does not exist."
//#define  ERR_ILLJOBNUMBER_CN00_1        N_CN00_1( 144), TERR_CN00_1, DBM_CN00_1, "ERR_ILLJOBNUMBER: Job number \"%s\" is illegal."
//#define  ERR_JOBHASSUCCESSORS_CN00_1    N_CN00_1( 145), TERR_CN00_1, DBM_CN00_1, "ERR_JOBHASSUCCESSORS: Job has successors. Cannot delete it."
//#define  ERR_INSERVERPROCESSSTARTER_CN00_1 N_CN00_1( 146),TERR_CN00_1,DBM_CN00_1,"ERR_INSERVERPROCESSSTARTER: Could not start server process (used command: \"%s\")."
//#define  ERR_NOSERVERPROCESSRUNNING_CN00_1 N_CN00_1( 147),TERR_CN00_1,DBM_CN00_1,"ERR_NOSERVERPROCESSRUNNING: Could not start server process (used command: \"%s\")."
//#define  ERR_PROCSTARTERROR_CN00_1      N_CN00_1( 148), TERR_CN00_1, DBM_CN00_1, "ERR_PROCSTARTERROR: Could not start background process (errorcode: %s)."
//#define  ERR_PROCSPAWNFAILED_CN00_1     N_CN00_1( 149), TERR_CN00_1, DBM_CN00_1, "ERR_PROCSPAWNFAILED: Error spawning child process."
//#define  ERR_PROCWAITERROR_CN00_1       N_CN00_1( 150), TERR_CN00_1, DBM_CN00_1, "ERR_PROCWAITERROR: Error waiting for child process (errno: %s)."
//#define  ERR_PROCCHLDEXIT_CN00_1        N_CN00_1( 151), TERR_CN00_1, DBM_CN00_1, "ERR_PROCCHLDEXIT: Child process exited abnormally."
//#define  ERR_PROCSHMDESTERROR_CN00_1    N_CN00_1( 152), TERR_CN00_1, DBM_CN00_1, "ERR_PROCSHMDESTERRORT: Could not destroy shared memory (errno: %s)."
//#define  ERR_PROCSHMDETERROR_CN00_1     N_CN00_1( 153), TERR_CN00_1, DBM_CN00_1, "ERR_PROCSHMDETERROR: Could not detach from shared memory (errno: %s)."
//#define  ERR_PROCSHMDETCHLDERR_CN00_1   N_CN00_1( 154), TERR_CN00_1, DBM_CN00_1, "ERR_PROCSHMDETCHLDERR: Child could not detach from shared memory."
//#define  ERR_PROCSHMATTERROR_CN00_1     N_CN00_1( 155), TERR_CN00_1, DBM_CN00_1, "ERR_PROCSHMATTERROR: Could not attach to shared memory (errno: %s)."
//#define  ERR_PROCSHMCREATEERROR_CN00_1  N_CN00_1( 156), TERR_CN00_1, DBM_CN00_1, "ERR_PROCSHMCREATEERROR: Could not create shared memory (errno: %s)."
//#define  ERR_SCHEDULERUNEXPECTED_CN00_1 N_CN00_1( 157), TERR_CN00_1, DBM_CN00_1, "ERR_SCHEDULERUNEXPECTED: Unexpected scheduler reply."
/*
                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                !!                                          !!
                !! do not define any new error numbers here !!
                !!    use class DBMSrvMsg_Error instead!    !!
                !!       !!157 is the last one here!!       !!
                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

#define  ERR_UNKNOWN_CN00_1             N_CN00_1(1000), TERR_CN00_1, DBM_CN00_1, "ERR_UNKNOWN: unknown error"

#define  INF_DBMCMD_CN00_1              TINF_CN00_1, 0,              "command %s %s"
#define  INF_DBMERR_CN00_1              TINF_CN00_1, 0,              "error   %s"
#define  INF_DBMTRC_CN00_1              TINF_CN00_1, 0,              "trace   %s"

inline const void cn00_1_Trace
    ( const char   * const szTrace )
{
  
  DBMSrvMsg_Info aEvent("trace   %s", szTrace);
  tin100_GlobalLog::writeEntry (aEvent);
} // end cn00_1_Trace

#ifdef DBM_TRACE_FLAG
  #define DBMTRACE(szTrace)    cn00_1_Trace(szTrace)
#else  
  #define DBMTRACE(szTrace)
#endif 

#endif // _GCN00_1_H_
