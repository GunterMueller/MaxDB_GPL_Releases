/* 
  -----------------------------------------------------------------------------
 
  module: vcn50.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer User Management
   
  description:  DBMServer User Management - Implementation

  version:      7.2.* 

  -----------------------------------------------------------------------------


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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "heo02.h"
#include "hsp02.h"
#include "geo47.h"
#include "heo11.h"
#include "hcn20.h"
#include "hcn42.h"
#include "hcn51.h"
#include "hcn90.h"
#include "hcn50.h"

#include "RunTime/RTE_UserProfileContainer.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Permissions.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
// server right to text structure
//  {DBMRightFileFull_CN50        , _T("FileFull"          ) , _T("File access (read and write)"      )}, 
//  {DBMRightFileRead_CN50        , _T("FileRead"          ) , _T("File access (read only)"           )}, 

#define ServerRightToText                                    \
{                                                            \
  {DBMRightDBInfoRead_CN50      , _T("DBInfoRead"        ) , _T("Request status data"               )}, \
  {DBMRightSystemCmd_CN50       , _T("SystemCmd"         ) , _T("Execute operating system commands" )}, \
  {DBMRightExecLoad_CN50        , _T("ExecLoad"          ) , _T("Execute the LOAD program"          )}, \
  {DBMRightUserMgm_CN50         , _T("UserMgm"           ) , _T("User management"                   )}, \
  {DBMRightDBFileRead_CN50      , _T("DBFileRead"        ) , _T("Database file access (read only)"  )}, \
  {DBMRightBackupSave_CN50      , _T("Backup"            ) , _T("Saving backups"                    )}, \
  {DBMRightInstallMgm_CN50      , _T("InstallMgm"        ) , _T("Installation management"           )}, \
  {DBMRightLoadSysTab_CN50      , _T("LoadSysTab"        ) , _T("Load the system tables"            )}, \
  {DBMRightParamCheckWrite_CN50 , _T("ParamCheckWrite"   ) , _T("Parameter access (checked write)"  )}, \
  {DBMRightParamFull_CN50       , _T("ParamFull"         ) , _T("Parameter access (read and write)" )}, \
  {DBMRightParamRead_CN50       , _T("ParamRead"         ) , _T("Parameter access (read only)"      )}, \
  {DBMRightDBStart_CN50         , _T("DBStart"           ) , _T("Start database instance"           )}, \
  {DBMRightDBStop_CN50          , _T("DBStop"            ) , _T("Stop database instance"            )}, \
  {DBMRightBackupRestore_CN50   , _T("Recovery"          ) , _T("Restoring backups"                 )}, \
  {DBMRightAccessSQL_CN50       , _T("AccessSQL"         ) , _T("Access to SQL session"             )}, \
  {DBMRightAccessUtility_CN50   , _T("AccessUtility"     ) , _T("Access to utility session"         )}, \
  {DBMRightShMAdmin_CN50        , _T("SharedMemoryMgm"   ) , _T("Shared memory management"          )}, \
  {DBMRightSCDAdmin_CN50        , _T("SchedulerMgm"      ) , _T("Scheduler management"              )}, \
  {DBMRightSCDUse_CN50          , _T("Scheduling"        ) , _T("Scheduler use"                     )}, \
  {DBMRightEvtDispAdmin_CN50    , _T(DBMCliEvtDisp_Permissions_EvtDispAdmin_Name) , _T(DBMCliEvtDisp_Permissions_EvtDispAdmin_Description)}, \
  {DBMRightEvtDispUse_CN50      , _T(DBMCliEvtDisp_Permissions_EvtDispUse_Name) , _T(DBMCliEvtDisp_Permissions_EvtDispUse_Description)}, \
  {DBMRightUnused25_CN50        , _T(""                  ) , _T(""                                  )}, \
  {DBMRightUnused26_CN50        , _T(""                  ) , _T(""                                  )}, \
  {DBMRightUnused27_CN50        , _T(""                  ) , _T(""                                  )}, \
  {DBMRightUnused28_CN50        , _T(""                  ) , _T(""                                  )}, \
  {DBMRightUnused29_CN50        , _T(""                  ) , _T(""                                  )}, \
  {DBMRightUnused30_CN50        , _T(""                  ) , _T(""                                  )}, \
  {DBMRightUnused31_CN50        , _T(""                  ) , _T(""                                  )}, \
  {DBMMaskNothing_CN50          , _T(""                  ) , _T(""                                  )}  \
}

#define GUIRightToText                                       \
{                                                            \
  {DBMGUIRight01_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight02_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight03_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight04_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight05_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight06_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight07_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight08_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight09_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight10_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight11_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight12_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight13_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight14_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight15_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight16_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight17_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight18_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight19_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight20_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight21_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight22_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight23_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight24_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight25_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight26_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight27_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight28_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight29_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight30_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMGUIRight31_CN50           , _T(""                  ) , _T(""                                  )}, \
  {DBMMaskNothing_CN50          , _T(""                  ) , _T(""                                  )}  \
}

// Keywords
#define KEY_SVRRIGHTS            _T("SERVERRIGHTS"   )
#define KEY_GUIRIGHTS            _T("GUIRIGHTS"      )
#define KEY_PWD                  _T("PASSWORD"       )
#define KEY_2PWD                 _T("SECONDPASSWORD" )
#define KEY_DISABLED             _T("DISABLED"       )
#define KEY_COMMENT              _T("COMMENT"        )
#define KEY_USERTYPE             _T("USERTYPE"       )
#define KEY_YES                  _T("YES"            )
#define KEY_NO                   _T("NO"             )

// Special Characters
#define CHAR_ASSIGN              _T('=')
#define CHAR_RIGHTDELIMETER      _T(',')
#define CHAR_RIGHTGRANT          _T('+')
#define CHAR_RIGHTREMOVE         _T('-')
   
    
/*    
  =============================================================================
  class:        cn50DBMUser
  =============================================================================
 */

/*
  -----------------------------------------------------------------------------
  public constructor cn50DBMUser :: cn50DBMUser
  -----------------------------------------------------------------------------
 */
cn50DBMUser :: cn50DBMUser ( )
{
  clear();
} // end cn50DBMUser :: cn50DBMUser

/*
  -----------------------------------------------------------------------------
  public constructor cn50DBMUser :: cn50DBMUser
  -----------------------------------------------------------------------------
 */
cn50DBMUser :: cn50DBMUser ( const char * szDBNameL )
{
  clear();
  setDBName(szDBNameL);
} // end cn50DBMUser :: cn50DBMUser

/*
  -----------------------------------------------------------------------------
  public constructor cn50DBMUser :: cn50DBMUser
  -----------------------------------------------------------------------------
 */
cn50DBMUser :: cn50DBMUser ( const char * szDBNameL, const char * szUserName )
{
  clear();
  setDBName(szDBNameL);
  setUserName(szUserName);
  reload();
} // end cn50DBMUser :: cn50DBMUser

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setDBName
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setDBName ( const char * szValue )
{
  szDBName.rawAssign(szValue);
  return *this;
} // end cn50DBMUser :: setDBName

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setUserName
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setUserName ( const char * szValue )
{
  Data.szUserName.rawAssign(szValue);
  bDirty = true;
  bNameDirty = true;
  return *this;
} // end cn50DBMUser :: setUserName

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setMasterPwd
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setMasterPwd ( const char * szValue )
{
  tsp00_Namec tmpName;
  tsp00_Name  tmpValue;

  cn90StrUpperCopy(tmpName, szValue, false);

  tmpValue.c2p(tmpName);

  s02applencrypt( tmpValue, Data.cryptMasterPwd);

  bDirty = true;
  bPwdDirty = true;

  return *this;
} // end cn50DBMUser :: setMasterPwd

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setSecondPwd
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setSecondPwd ( const char * szValue )
{

  if (strlen(szValue) == 0) {
    if (Data.bSecondPwd) {
      bDirty = true;
      bSecondPwdDirty = true;
    } // end if
    Data.bSecondPwd = false;
  } else {
    tsp00_Namec tmpName;
    tsp00_Name  tmpValue;

    cn90StrUpperCopy(tmpName, szValue, false);

    tmpValue.c2p(tmpName);
 
    s02applencrypt( tmpValue, Data.cryptSecondPwd);
    Data.bSecondPwd = true;

    bDirty = true;
    bSecondPwdDirty = true;
  } // end if

  return *this;
} // end cn50DBMUser :: setSecondPwd

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: checkMasterPwd
  -----------------------------------------------------------------------------
 */
bool cn50DBMUser :: checkMasterPwd ( const char * szValue )
{
  tsp00_Namec     tmpName;
  tsp00_CryptPw   tmpMasterPwd;
  tsp00_Name      tmpValue;

  cn90StrUpperCopy(tmpName, szValue, false);

  tmpValue.c2p(tmpName);

  s02applencrypt( tmpValue, tmpMasterPwd);

  bLoggedOn =     (memcmp(&tmpMasterPwd, &Data.cryptMasterPwd, sizeof(tsp00_CryptPw)) == 0) 
               && !isDisabled();

  return  bLoggedOn;
} // end cn50DBMUser :: checkMasterPwd

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: checkSecondPwd
  -----------------------------------------------------------------------------
 */
bool cn50DBMUser :: checkSecondPwd ( const char * szValue ) 
{
  tsp00_Namec     tmpName;
  tsp00_CryptPw   tmpSecondPwd;
  tsp00_Name      tmpValue;

  cn90StrUpperCopy(tmpName, szValue, false);

  tmpValue.c2p(tmpName);

  s02applencrypt( tmpValue, tmpSecondPwd);

  bLoggedOn =    (memcmp(&tmpSecondPwd, &Data.cryptSecondPwd, sizeof(tsp00_CryptPw)) == 0) 
              && Data.bSecondPwd  
              && !isDisabled();

  return bLoggedOn;
} // end cn50DBMUser :: checkSecondPwd

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setColdUser
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setColdUser ( bool const bValue, bool const bUnset )
{
  if (bValue && bUnset) {
    cn50DBMUser  usrDBM(this->getDBName(), cn50DBMUser::getColdUser(this->getDBName()));
    if (usrDBM.existsOnDisk() && (strcmp(usrDBM.getUserName(), this->getUserName()) != 0)) {
      usrDBM.setColdUser(false);
      usrDBM.save();
    } // end if
  } // end if

  Data.nProperties = (bValue) ? (Data.nProperties | FIRSTDBM_CN50) : (Data.nProperties & (~FIRSTDBM_CN50));
//  Data.bColdUser = bValue;

  bDirty = true;
  return *this;
} // end cn50DBMUser :: setColdUser

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setSDBAUser
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setSDBAUser ( bool const bValue )
{
  Data.nProperties = (bValue) ? (Data.nProperties | SYSDBA_CN50) : (Data.nProperties & (~SYSDBA_CN50));
//  Data.bSDBAUser   = bValue;
  
  bDirty = true;
  return *this;
} // end cn50DBMUser :: setSDBAUser

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setSAPUser
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setSAPUser ( bool const bValue )
{
  Data.nProperties = (bValue) ? (Data.nProperties | SAPUSR_CN50) : (Data.nProperties & (~SAPUSR_CN50));
  
  bDirty = true;
  return *this;
} // end cn50DBMUser :: setSAPUser

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setDisabled
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setDisabled ( bool const bValue )
{
  Data.bDisabled = bValue;

  bDirty = true;
  return *this;
} // end cn50DBMUser :: setDisabled

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setServerRights
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setServerRights ( tsp00_Uint4 const nMask, tsp00_Uint4 const nValues )
{
  Data.dbmServerRights = (Data.dbmServerRights & ~nMask) | nValues;

  bDirty = true;
  return *this;
} // end cn50DBMUser :: setServerRights

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setGUIRights
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setGUIRights ( tsp00_Uint4 const nMask, tsp00_Uint4 const nValues )
{
  Data.dbmGUIRights = (Data.dbmGUIRights & ~nMask) | nValues;

  bDirty = true;
  return *this;
} // end cn50DBMUser :: setGUIRights

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setComment
  -----------------------------------------------------------------------------
 */
cn50DBMUser & cn50DBMUser :: setComment ( const char * szValue )
{
 
  Data.szComment.rawAssign(szValue);

  bDirty = true;
  return *this;
} // end cn50DBMUser :: setComment

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: getMasterPwd
  -----------------------------------------------------------------------------
 */
tsp00_C64c cn50DBMUser :: getMasterPwd ( bool bUncrypt ) const
{
  tsp00_C64c      szPwd;

  if (bUncrypt) {
    cn90CalculateUncrypt(Data.cryptMasterPwd, szPwd, false);
  } else {
    cn90CryptToCryptString(Data.cryptMasterPwd, szPwd);
  } // end if

  return szPwd;
} // end cn50DBMUser :: getMasterPwd

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: getClearMasterPwd
  -----------------------------------------------------------------------------
 */
tsp00_Namec cn50DBMUser :: getClearMasterPwd ( ) const
{
  tsp00_C64c      szPwd;
  tsp00_Namec     szReturn;

  szPwd = getMasterPwd(true);

  szReturn.rawAssign(szPwd.asCharp());

  return szReturn;

} // end cn50DBMUser :: getClearMasterPwd

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: getSecondPwd
  -----------------------------------------------------------------------------
 */
tsp00_C64c cn50DBMUser :: getSecondPwd ( bool bUncrypt ) const
{
  tsp00_C64c      szPwd;

  if (bUncrypt) {
    cn90CalculateUncrypt(Data.cryptSecondPwd, szPwd, false);
  } else {
    cn90CryptToCryptString(Data.cryptSecondPwd, szPwd);
  } // end if

  return szPwd;
} // end cn50DBMUser :: getSecondPwd

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: isServerRight
  -----------------------------------------------------------------------------
 */
bool cn50DBMUser :: isServerRight ( tsp00_Uint4 const nRights, bool const bAnd ) const
{
  bool bReturn = false;

  if (bAnd) {
    bReturn = ((Data.dbmServerRights & nRights) == nRights);
  } else {
    bReturn = ((Data.dbmServerRights & nRights) != DBMMaskNothing_CN50);
  } // end if

  return bReturn || (nRights == DBMMaskNothing_CN50);
} // end cn50DBMUser :: isServerRight

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: isGUIRight
  -----------------------------------------------------------------------------
 */
bool cn50DBMUser :: isGUIRight ( tsp00_Uint4 const nRights, bool const bAnd ) const
{
  bool bReturn = false;

  if (bAnd) {
    bReturn = ((Data.dbmGUIRights & nRights) == nRights);
  } else {
    bReturn = ((Data.dbmGUIRights & nRights) != DBMMaskNothing_CN50);
  } // end if

  return bReturn;
} // end cn50DBMUser :: isGUIRight

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: getData
  -----------------------------------------------------------------------------
 */
void cn50DBMUser :: getData ( _TCHAR * pBuffer ) const
{
  _TCHAR       * pCurrent       = pBuffer;
  int            nIndex         = 0;
  RightToTextT   SvrRightList[] = ServerRightToText;
  RightToTextT   GUIRightList[] = GUIRightToText;
  const _TCHAR * pValue         = NULL;

  if (pCurrent != NULL) {

    // server rights
    _stprintf(pCurrent, _T("%s%c"), KEY_SVRRIGHTS, CHAR_ASSIGN);
    pCurrent = pCurrent + _tcslen(pCurrent);
    for (nIndex = 0; SvrRightList[nIndex].nRight != DBMMaskNothing_CN50; nIndex++) {
      if (isServerRight(SvrRightList[nIndex].nRight) && _tcslen(SvrRightList[nIndex].szName) > 0) {
        _stprintf(pCurrent, _T("%s%c"), SvrRightList[nIndex].szName, 
                                        CHAR_RIGHTDELIMETER);
        pCurrent = pCurrent + _tcslen(pCurrent);
      } // end if
    } // end for
    if (*(pCurrent - 1) == CHAR_RIGHTDELIMETER) {
      --pCurrent;
    } // end if
    *pCurrent = _T('\n');
    ++pCurrent;

    // GUI rights
    _stprintf(pCurrent, _T("%s%c"), KEY_GUIRIGHTS, CHAR_ASSIGN);
    pCurrent = pCurrent + _tcslen(pCurrent);
    for (nIndex = 0; GUIRightList[nIndex].nRight != DBMMaskNothing_CN50; nIndex++) {
      if (isGUIRight(GUIRightList[nIndex].nRight) && _tcslen(GUIRightList[nIndex].szName) > 0) {
        _stprintf(pCurrent, _T("%s%c"), GUIRightList[nIndex].szName, 
                                        CHAR_RIGHTDELIMETER);
        pCurrent = pCurrent + _tcslen(pCurrent);
      } // end if
    } // end for
    if (*(pCurrent - 1) == CHAR_RIGHTDELIMETER) {
      --pCurrent;
    } // end if
    *pCurrent = _T('\n');
    ++pCurrent;

    // exists second pwd
    _stprintf(pCurrent, _T("%s%c%s\n"), KEY_2PWD, 
                                        CHAR_ASSIGN,
                                        isSecondPwd() ? KEY_YES : KEY_NO);
    pCurrent = pCurrent + _tcslen(pCurrent);

    // Disabled
    _stprintf(pCurrent, _T("%s%c%s\n"), KEY_DISABLED, 
                                        CHAR_ASSIGN,
                                        isDisabled() ? KEY_YES : KEY_NO);
    pCurrent = pCurrent + _tcslen(pCurrent);

    // Comment
    _stprintf(pCurrent, _T("%s%c%s\n"), KEY_COMMENT, 
                                        CHAR_ASSIGN,
                                        getComment().asCharp());
    pCurrent = pCurrent + _tcslen(pCurrent);

    // Usertype
    _stprintf(pCurrent, _T("%s%c%s\n"), KEY_USERTYPE, 
                                        CHAR_ASSIGN,
                                        getType());
    pCurrent = pCurrent + _tcslen(pCurrent);

  } // end if

} // end cn50DBMUser :: getData

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: getUPCOwner
  -----------------------------------------------------------------------------
 */
tcn00_UserNamec cn50DBMUser :: getUPCOwner ( )  
{
  tcn00_UserNamec         szUPCOwner;

  m_oEvent.ClearMessageList();

  RTE_UserProfileContainer oProfile;
  oProfile.Open(szDBName, true, m_oEvent);

  szUPCOwner.Init();

  if (m_oEvent.IsEmpty()) {
    bool retVal = oProfile.GetOwnerName(szUPCOwner, m_oEvent);

    if (!retVal) {
      szUPCOwner.Init();
    } // end if
  } // end if

  return szUPCOwner;
} // end cn50DBMUser :: getUPCOwner

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setSupportGroup
  -----------------------------------------------------------------------------
 */
bool cn50DBMUser :: setSupportGroup (tsp00_Namec const & szGroupname )
{
  m_oEvent.ClearMessageList();

  RTE_UserProfileContainer  oProfile;
  oProfile.Open(szDBName, true, m_oEvent);

  if (m_oEvent.IsEmpty()) {
    oProfile.SetGroupName(szGroupname, m_oEvent);
  } // end if

  return (m_oEvent.IsEmpty());
} // end cn50DBMUser :: setSupportGroup

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: listRights
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: listRights ( _TCHAR * pBuffer, const _TCHAR * szMode ) const
{
  tcn00_Error    nFuncReturn    = OK_CN00;
  _TCHAR       * pCurrent       = pBuffer;
  int            nIndex         = 0;
  RightToTextT   SvrRightList[] = ServerRightToText;
  RightToTextT   GUIRightList[] = GUIRightToText;
  RightToTextT * RightList      = NULL;
  bool           bServer        = true;

  if (pCurrent != NULL) {

    if ( _tcsicmp(szMode, KEY_SVRRIGHTS ) == 0 ) {
      RightList = &SvrRightList[0];
    } else if ( _tcsicmp(szMode, KEY_GUIRIGHTS ) == 0 ) {
      RightList = &GUIRightList[0];
      bServer = false;
    } // end if

    if (RightList != NULL) {
      for (nIndex = 0; RightList[nIndex].nRight != DBMMaskNothing_CN50; nIndex++) {
        if (_tcslen(RightList[nIndex].szName) > 0) {
          _stprintf(pCurrent, _T("%-20s%s%c%s%s%s"),
                                          RightList[nIndex].szName, 
                                          VALUE_SEPSTRING_CN00,
                                          bServer ?  isServerRight(RightList[nIndex].nRight) ? CHAR_RIGHTGRANT : CHAR_RIGHTREMOVE
                                                  :  isGUIRight(RightList[nIndex].nRight)    ? CHAR_RIGHTGRANT : CHAR_RIGHTREMOVE,
                                          VALUE_SEPSTRING_CN00,
                                          RightList[nIndex].szDescription,
                                          LINE_SEPSTRING_CN00);
          pCurrent = pCurrent + _tcslen(pCurrent);
        } // end if
      } // end for
    } else {
      nFuncReturn = ERR_PARAM_CN00;
    } // end if
  } // end if

  return nFuncReturn;
} // end cn50DBMUser :: listRights

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: reload
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: reload ( )
{
  FUNCTION_DBG_MCN00_1("cn50DBMUser :: reload");

  tcn00_Error     nFuncReturn = OK_CN00;

  m_oEvent.ClearMessageList();

  if ((szDBName.length() == 0) || (Data.szUserName.length() == 0)) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
    nFuncReturn = m_oEvent.ID();
  } else {

    m_oEvent.ClearMessageList();
    RTE_UserProfileContainer  oProfile;
    oProfile.Open(szDBName, true, m_oEvent);

    if (!m_oEvent.IsEmpty()) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
      nFuncReturn = m_oEvent.ID();
    } else {

      nFuncReturn = reload(Data, oProfile);
      if (nFuncReturn == OK_CN00) {
        bExistsOnDisk   = true;
        bDirty          = false;
        bPwdDirty       = false;
        bSecondPwdDirty = false;
        bNameDirty      = false;
      } // end if

    } // end if
  } // end if

  return nFuncReturn;
} // end cn50DBMUser :: reload

  
/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: save
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: save ( bool bRecover )
{
  FUNCTION_DBG_MCN00_1("cn50DBMUser :: save");

  tcn00_Error   nFuncReturn = OK_CN00;
  
  if ((szDBName.length() == 0) || (Data.szUserName.length() == 0)) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
    nFuncReturn = m_oEvent.ID();
  } // end if

  if (nFuncReturn == OK_CN00) {
    if (!bRecover) {
      if (isColdUser() && (this->bPwdDirty || this->bNameDirty)) {
        nFuncReturn = saveColdUsr();
      } // end if
      if (isSDBAUser() && bExistsOnDisk && (this->bPwdDirty || this->bSecondPwdDirty || this->bNameDirty)) {
        nFuncReturn = saveSYSDBAUsr();
      } // end if
    } // end if
  } // end if
  
  if (nFuncReturn == OK_CN00) {
    m_oEvent.ClearMessageList();

    RTE_UserProfileContainer Profile;
    Profile.Open(szDBName, false, m_oEvent);

    if (!m_oEvent.IsEmpty()) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRSAVE));
      nFuncReturn = m_oEvent.ID();
    } else {
      setSizes(Data);
      Data.recHeader.nRecordId   = UserRecordFormat_Curr_CN50;
      Data.recHeader.nRecordSize = sizeof(Data);
      bool retVal = Profile.SetProfile(Data.szUserName, sizeof(DBMUserData), &Data, m_oEvent);
      if (!retVal) {
        m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRSAVE));
        nFuncReturn = m_oEvent.ID();
      } else {
       // make a copy from the upc
        DBMSrvMsg_Error oError;
        cn42BackupFile(szDBName, FGET_USER_CN42, oError);
        bExistsOnDisk   = true;
        bDirty          = false;
        bPwdDirty       = false;
        bSecondPwdDirty = false;
        bNameDirty      = false;
      } // end if
    } // end if
  } // end if

  return nFuncReturn;

} // end cn50DBMUser :: save

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: deleteUsr
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: deleteUsr ( )
{
  FUNCTION_DBG_MCN00_1("cn50DBMUser :: deleteUsr");

  tcn00_Error   nFuncReturn = OK_CN00;
  
  if ((szDBName.length() == 0) || (Data.szUserName.length() == 0)) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
    nFuncReturn = m_oEvent.ID();
  } // end if

  if (nFuncReturn == OK_CN00) {
    if (isSDBAUser()) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRDELSDBA));
      nFuncReturn = m_oEvent.ID();
    } else if (isColdUser()) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRDELDBM));
      nFuncReturn = m_oEvent.ID();
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {

    m_oEvent.ClearMessageList();
    RTE_UserProfileContainer Profile;
    Profile.Open(szDBName, false, m_oEvent);

    if (!m_oEvent.IsEmpty()) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRSAVE));
      nFuncReturn = m_oEvent.ID();
    } else {
      bool retVal = Profile.DelProfile(Data.szUserName, m_oEvent);
      if (!retVal) {
        m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRSAVE));
        nFuncReturn = m_oEvent.ID();
      } // end if
    } // end if
  } // end if

  return nFuncReturn;
} // end cn50DBMUser :: deleteUsr

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: clear
  -----------------------------------------------------------------------------
 */
void cn50DBMUser :: clear ( )
{
  setDBName       ( "" );
  setUserName     ( "" );
  setMasterPwd    ( "" );
  setSecondPwd    ( "" );
  setComment      ( "" );

  Data.nProperties = 0;
  setColdUser     ( false );
  setSDBAUser     ( false );

  setDisabled     ( false );

  setServerRights ( DBMMaskAll_CN50, DBMMaskNothing_CN50 );
  setGUIRights    ( DBMMaskAll_CN50, DBMMaskNothing_CN50 );

  bExistsOnDisk   = false;
  bDirty          = false;
  bPwdDirty       = false;
  bSecondPwdDirty = false;
  bNameDirty      = false;
  bLoggedOn       = false;

  setSizes(Data);

} // end cn50DBMUser :: clear

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setData
  -----------------------------------------------------------------------------
 */
tcn00_Error  cn50DBMUser :: setData ( const _TCHAR * szData )
{
  _TCHAR        szToken[1024];
  _TCHAR      * pValue   = NULL;
  long          nToken   = 1;
  tcn00_Error   nReturn  = OK_CN00;
  
  if (szData != NULL) {

    // analyze tokens
    while (cn90GetToken(szData, szToken, nToken, 1024)) {
      pValue = _tcschr(szToken, CHAR_ASSIGN);

      if (pValue != NULL) {
        *pValue = CHAR_STRINGTERM_CN90;
        pValue++;

        if        ( _tcsicmp(szToken, KEY_SVRRIGHTS ) == 0 ) {
          RightToTextT  RightList[] = ServerRightToText;
          setRights(pValue, Data.dbmServerRights, RightList);
        } else if ( _tcsicmp(szToken, KEY_GUIRIGHTS ) == 0 ) {
          RightToTextT  RightList[] = GUIRightToText;
          setRights(pValue, Data.dbmGUIRights, RightList);
        } else if ( _tcsicmp(szToken, KEY_PWD       ) == 0 ) {
          cn90Uncrypt(pValue, false);
          if (!ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(pValue)) {
            nReturn = ERR_PARAM_CN00;
          } // end if
          if (nReturn == OK_CN00) {
            setMasterPwd(pValue);
          } else { 
            break;
          } // end if
        } else if ( _tcsicmp(szToken, KEY_2PWD      ) == 0 ) {
          cn90Uncrypt(pValue, false);
          if (strlen(pValue) > 0) {
            if (!ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(pValue)) {
              nReturn = ERR_PARAM_CN00;
            } // end if
          } // end if
          if (nReturn == OK_CN00) {
            setSecondPwd(pValue);
          } else { 
            break;
          } // end if
        } else if ( _tcsicmp(szToken, KEY_DISABLED  ) == 0 ) { 
          if        ( _tcsicmp(pValue, KEY_YES ) == 0 ) {
            setDisabled();
          } else if ( _tcsicmp(pValue, KEY_NO  ) == 0 ) {
            setDisabled(false);
          } // end if    
        } else if ( _tcsicmp(szToken, KEY_COMMENT   ) == 0 ) { 
          cn90StripQuotes(pValue);
          setComment(pValue);
        } else {
          nReturn = ERR_PARAM_CN00;
          break;
        } // end if    

      } // end if

      nToken++;
    } // end while

  } // end if

  return nReturn;
} // end cn50DBMUser :: setData

/*
  -----------------------------------------------------------------------------
  public function cn50DBMUser :: setRights
  -----------------------------------------------------------------------------
 */
cn50DBMUser &  cn50DBMUser :: setRights(_TCHAR * szValue, tsp00_Uint4 & nValue, RightToTextT RightList[])
{
  _TCHAR   * pRight     = szValue;
  _TCHAR   * pGrant     = NULL;
  _TCHAR   * pNextRight = NULL;
  long       nIndex     = 0;

  while (pRight) {

    // Terminate right entry
    pNextRight = _tcschr(pRight, CHAR_RIGHTDELIMETER);
    if (pNextRight != NULL) {
      *pNextRight = CHAR_STRINGTERM_CN90;
      pNextRight++;
    } // end if

    // look for grant sign
    pGrant = pRight;
    pRight++;
    
    // look for right value
    for (nIndex = 0; RightList[nIndex].nRight != DBMMaskNothing_CN50; nIndex++) {
      if (_tcsicmp(pRight, RightList[nIndex].szName) == 0) {
        break;
      } // end if
    } // end for

    // set/unset right
    switch (*pGrant) {
      case CHAR_RIGHTGRANT:
        nValue = nValue | RightList[nIndex].nRight;        
        break;
      case CHAR_RIGHTREMOVE:
        nValue = nValue & ~RightList[nIndex].nRight;        
        break;
      default:
        break;
    } // end switch

    // an the next
    pRight = pNextRight;
  } // end while

  return *this;
} // end cn50DBMUser :: setRights

/*
  -----------------------------------------------------------------------------
  protected function cn50DBMUser :: getType
  -----------------------------------------------------------------------------
 */
const char * cn50DBMUser :: getType
      ( ) const
{
  const char * pType = "";

  if (isColdUser()) {
    pType = USERTYPE_DBM_CN50;
  } else if (isSDBAUser()) {
    pType = USERTYPE_DBA_CN50;
  } else if (isSAPUser()) {
    pType = USERTYPE_SAP_CN50;
  } // end if

  return pType;
} // end cn50DBMUser :: getType 

/*
  -----------------------------------------------------------------------------
  protected function cn50DBMUser :: getUser
  -----------------------------------------------------------------------------
 */
tcn00_UserNamec cn50DBMUser :: getUser 
      ( const char *             szDBName,
        tsp00_Uint4              nProperty )
{
  tsp00_DbNamec oDB;
  oDB.rawAssign(szDBName);

  return getUser(oDB, nProperty);
} // end cn50DBMUser :: getUser

/*
  -----------------------------------------------------------------------------
  protected function cn50DBMUser :: getUser
  -----------------------------------------------------------------------------
 */
tcn00_UserNamec cn50DBMUser :: getUser 
      ( const tsp00_DbNamec    & szDBName,
        tsp00_Uint4              nProperty )
{
   tcn00_UserNamec aUser;
   cn50DBMUser aContainer(szDBName);

   aUser.Init();

   aUser = aContainer.getUser(nProperty);

   // fallback to configuration
   if (aUser.length() == 0) {
     switch (nProperty) {
     case FIRSTDBM_CN50: 
       {
         cn51DBMConfigValue  cfgColdUsr(szDBName, (*(tsp00_Namec *) "SAPDB12"));
         cn50DBMUser oUser(szDBName, cfgColdUsr);
         if (oUser.lastEvent().IsEmpty()) {
           oUser.setColdUser(true, false);
           oUser.save();
           aUser = oUser.getUserName();
           cfgColdUsr = "";
         } // end if
       } 
       break;
     case SYSDBA_CN50:
       {
         cn51DBMConfigValue  cfgColdUsr(szDBName, (*(tsp00_Namec *) "SAPDB5"));
         cn50DBMUser oUser(szDBName, cfgColdUsr);
         if (oUser.lastEvent().IsEmpty()) {
           oUser.setSDBAUser(true);
           oUser.save();
           aUser = oUser.getUserName();
           cfgColdUsr = "";
         } // end if
       } 
       break;

     } // end switch
   } // end if

    // fallback to paramfile (if exists)
    if ((aUser.length() == 0) && (nProperty == FIRSTDBM_CN50)) {
        tcn002_XpValueString     szNameXP;
        tcn00_UserNamec          szName;
        DBMSrvMsg_Error          oErrorToIgnore;

        // ignore error because it is a fallback 
        if (cn20XParamGetValue( szDBName, PAN_CONTROLID, szNameXP, oErrorToIgnore)) {
            szName.rawAssign(szNameXP);
            cn50DBMUser oUser(szDBName, szName);
            if (oUser.lastEvent().IsEmpty()) {
                oUser.setColdUser(true, false);
                oUser.save();
                aUser = oUser.getUserName();
            } else {
                tcn002_XpValueString     szPwXP;
                tcn00_UserNamec          szPw;
                if (cn20XParamGetValue( szDBName, PAN_CONTROLPW, szPwXP, oErrorToIgnore)) {
                    szPw.rawAssign(szPwXP);
                    cn50DBMUser newUser;
                    newUser.clear();
                    newUser.setDBName    ( szDBName    )
                        .setUserName     ( szName      )
                        .setMasterPwd    ( szPw        )
                        .setServerRights ( DBMMaskAll_CN50, DBMUserDefault_CN50 )
                        .setGUIRights    ( DBMMaskAll_CN50, DBMUserDefaultGUI_CN50 )
                        .setColdUser     (true, false);

                    newUser.save(true);
                    aUser = newUser.getUserName();
                } // end if
            } // end if
        } // end if
    } // end if 

    return aUser;
} // end cn50DBMUser :: getUser 
/*
  -----------------------------------------------------------------------------
 */
tcn00_UserNamec cn50DBMUser :: getUser 
      ( tsp00_Uint4         nProperty )
{
  tcn00_Error             nFuncReturn    = OK_CN00;
  tcn00_UserNamec         szUser;
  tcn00_UserNamec         szUserName;

  m_oEvent.ClearMessageList();
  RTE_UserProfileContainer  oProfile;
  oProfile.Open(szDBName, true, m_oEvent);

  tsp00_Uint4             nToken = FIRST_TOKEN;
  DBMUserData             oUser;
  tsp00_Int4              nLength;

  szUser.Init();

  if (m_oEvent.IsEmpty()) {
    while (m_oEvent.IsEmpty()) {
      nLength = sizeof(oUser);
      oProfile.ScanProfiles(nToken, nLength, &oUser, szUserName, m_oEvent);

      if (m_oEvent.IsEmpty() && (nToken == NO_TOKEN)) {
        break;
      } // end if

      if (!m_oEvent.IsEmpty() && (nLength != sizeof(oUser))) {
//        oUser.szDBName   = szDBName;
        oUser.szUserName = szUserName;
        reload (oUser, oProfile);
      } // end if

      if (m_oEvent.IsEmpty() && ((oUser.nProperties & nProperty) == nProperty)) {
        szUser = oUser.szUserName;
        break;
      } // end if

      if(m_oEvent.IsEmpty() && (nToken == LAST_TOKEN)) {
        break;
      } // end if

    } // end while

  } // end if

  return szUser;
} // end cn50DBMUser :: getUser

/*
  -----------------------------------------------------------------------------
  protected function cn50DBMUser :: saveColdUsr
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: saveColdUsr()
{

  tsp00_DbNamec               dbName = getDBName();
  DBMSrvKnl_State::StateValue oState = DBMSrvKnl_State::State(dbName);

  DBMSrvPar_Parameters  * pXParamData = DBMSrvPar_Parameters::GetInstance(dbName, true, m_oEvent);
  if (pXParamData != NULL) {

    // write CONTROLPASSWORD
    if (bPwdDirty) {
      if (!pXParamData->SetValueWithCheck(PAN_CONTROLPW, 
                                          this->getMasterPwd(true),
                                          true,
                                          oState == DBMSrvKnl_State::StateOffline ?
                                          DBMSrvPar_Parameter::XPWriteModePermanent:
                                          DBMSrvPar_Parameter::XPWriteModeAll,
                                          m_oEvent) ) {
        delete pXParamData;
        return false;
      } // end if
    } // end if

    // write CONTROLUSERID
    if (bNameDirty) {
      if (!pXParamData->SetValueWithCheck(PAN_CONTROLID, 
                                          this->getUserName(),
                                          true,
                                          oState == DBMSrvKnl_State::StateOffline ?
                                          DBMSrvPar_Parameter::XPWriteModePermanent:
                                          DBMSrvPar_Parameter::XPWriteModeAll,
                                          m_oEvent) ) {
        delete pXParamData;
        return false;
      } // end if
    } // end if

    // commit parameters
    if (pXParamData->CheckAll(m_oEvent) ) {
      pXParamData->Commit(m_oEvent);
    } // end if
    delete pXParamData;

  } // end if

  return m_oEvent.ID(); 
} // end cn50DBMUser :: saveColdUsr

/*
  -----------------------------------------------------------------------------
  protected function cn50DBMUser :: saveSYSDBAUsr
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: saveSYSDBAUsr()
{
  FUNCTION_DBG_MCN00_1("cn50DBMUser :: saveSYSDBAUsr");

  tcn00_Error     nFuncReturn = OK_CN00;
  char            szCommand[500];
  cn50DBMUser     usrDBA    ( szDBName, getSYSDBA());

  // chekc database state
  if (DBMSrvKnl_State::State(szDBName) != DBMSrvKnl_State::StateOnline) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
    nFuncReturn = m_oEvent.ID();
  } // end if

  // check user sysdba record
  if ((nFuncReturn == OK_CN00) && !usrDBA.lastEvent().IsEmpty()) {
    //
    m_oEvent = usrDBA.lastEvent();
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAUNKNOWN));
    nFuncReturn = m_oEvent.ID();
  } // end if

  // check sysdba property
  if ((nFuncReturn == OK_CN00) && !DBMSrvKnl_Session::Execute (szDBName,
                                                               usrDBA.getUserName().asCharp(),
                                                               usrDBA.getClearMasterPwd().asCharp(),
                                                               "SELECT * FROM SYSDD.VERSION WHERE SYSDBA=USER",
                                                               m_oEvent)) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAWRONG));
    nFuncReturn = m_oEvent.ID();
  } // end if

  // alter password
  if ((nFuncReturn == OK_CN00) && bPwdDirty) {
    sprintf(szCommand, "ALTER PASSWORD %s %s", getUserName().asCharp(), 
                                               getMasterPwd(true).asCharp() );
    if (!DBMSrvKnl_Session::Execute (szDBName.asCharp(),
                                      usrDBA.getUserName().asCharp(),
                                      usrDBA.getClearMasterPwd().asCharp(),
                                      szCommand,
                                      m_oEvent)) {
      nFuncReturn = m_oEvent.ID();
    } // end if
  } // end if

  // alter second password
  if ((nFuncReturn == OK_CN00) && bSecondPwdDirty && (getSecondPwd(true).length() > 0)) {
    sprintf(szCommand, "ALTER USER %s %s PASSWORD %s", getUserName().asCharp(), 
                                                       isSecondPwd() ? "ADD" : "DROP", 
                                                       getSecondPwd(true).asCharp());
    if (!DBMSrvKnl_Session::Execute (szDBName.asCharp(),
                                      usrDBA.getUserName().asCharp(),
                                      usrDBA.getClearMasterPwd().asCharp(),
                                      szCommand,
                                      m_oEvent)) {
      nFuncReturn = m_oEvent.ID();
    } // end if
  } // end if

  // alter name
  if ((nFuncReturn == OK_CN00) && bNameDirty) {
    sprintf(szCommand, "RENAME USER %s TO %s", usrDBA.getUserName().asCharp(), 
                                               getUserName().asCharp()); 
    if (!DBMSrvKnl_Session::Execute (szDBName.asCharp(),
                                      usrDBA.getUserName().asCharp(),
                                      usrDBA.getClearMasterPwd().asCharp(),
                                      szCommand,
                                      m_oEvent)) {
      nFuncReturn = m_oEvent.ID();
    } // end if
  } // end if

  return nFuncReturn; 
} // end cn50DBMUser :: saveSYSDBAUsr

/*
  -----------------------------------------------------------------------------
  pivate function cn50DBMUser :: setSizes
  -----------------------------------------------------------------------------
 */
void cn50DBMUser :: setSizes ( DBMUserData & locData ) const
{
  locData.s_szDBName        = sizeof(locData.szDBNameX);
  locData.s_szUserName      = sizeof(locData.szUserName);
  locData.s_cryptMasterPwd  = sizeof(locData.cryptMasterPwd);
  locData.s_cryptSecondPwd  = sizeof(locData.cryptSecondPwd);
  locData.s_bSecondPwd      = sizeof(locData.bSecondPwd);
  locData.s_nProperties     = sizeof(locData.nProperties);
  locData.s_bDisabled       = sizeof(locData.bDisabled);
  locData.s_dbmServerRights = sizeof(locData.dbmServerRights);
  locData.s_dbmGUIRights    = sizeof(locData.dbmGUIRights);
  locData.s_szComment       = sizeof(locData.szComment);
} // end cn50DBMUser :: setSizes

/*
  -----------------------------------------------------------------------------
  protected function cn50DBMUser :: reload
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: reload (DBMUserData & oData, RTE_UserProfileContainer & Profile) 
{
  FUNCTION_DBG_MCN00_1("cn50DBMUser :: reload");

  tcn00_Error     nFuncReturn = OK_CN00;
  tsp00_Int4      nLength     = sizeof(DBMUserData);
  tsp00_Int4      nHeadLength = sizeof(DBMUserData);
  DBMUserData   * pHeader     = NULL;

  m_oEvent.ClearMessageList();

  // read profile into header structure with dynamic buffer
  pHeader = REINTERPRET_CAST(DBMUserData *, new char[nHeadLength]); 
  if (pHeader != NULL) {
    m_oEvent.ClearMessageList();
    bool retVal = Profile.GetProfile(oData.szUserName, nHeadLength, pHeader, m_oEvent);
    if (!retVal) {
      // length does not match -> old record format
      if (nHeadLength > sizeof(DBMUserData)) {
        // alloc appropriate size
        delete pHeader;
        pHeader = REINTERPRET_CAST(DBMUserData *, new char[nHeadLength]); 
        if (pHeader != NULL) {
          // read record again
          m_oEvent.ClearMessageList();
          bool retVal = Profile.GetProfile(oData.szUserName, nHeadLength, pHeader, m_oEvent);
          if (!retVal) {
            m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
            nFuncReturn = m_oEvent.ID();
          } // end if
        } else {
          m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
          nFuncReturn = m_oEvent.ID();
        } // end if
      } else {
        m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
        nFuncReturn = m_oEvent.ID();
      } // end if
    } // end if
  } else {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    nFuncReturn = m_oEvent.ID();
  } // end if

  // now we have a record with header (or not)

  // check header data
  if (nFuncReturn == OK_CN00) {
    if (pHeader->recHeader.nRecordId   != UserRecordFormat_Curr_CN50 ||
        pHeader->recHeader.nRecordSize != sizeof(Data)                  ) {
      // shit happens! migrate
      Profile.CloseContainer (m_oEvent);
      nFuncReturn = migrateRecord(szDBName, oData.szUserName, pHeader);
    } // end if
  } // end if

  if (pHeader != NULL) {
    delete pHeader;
  } // end if

  // read really
  if (nFuncReturn == OK_CN00) {
    m_oEvent.ClearMessageList();
    bool retVal = Profile.GetProfile(Data.szUserName, nLength, &Data, m_oEvent);
    if (!retVal) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
      nFuncReturn = m_oEvent.ID();
    } // end if
  } // end if

  return nFuncReturn;
} // end cn50DBMUser :: reload

/*
  -----------------------------------------------------------------------------
  pivate function cn50DBMUser :: migrateRecord
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: migrateRecord ( const tsp00_DbNamec   & szDBNameL,
                                           const tcn00_UserNamec & szUserName,
                                           const DBMUserData     * pHeader ) 
{
  FUNCTION_DBG_MCN00_1("cn50DBMUser :: migrateRecord");

  tcn00_Error             nFuncReturn   = OK_CN00;
  tsp00_Uint2             nSourceRecord = pHeader->recHeader.nRecordId;
  tsp00_Uint2             nTargetRecord = 0;

  m_oEvent.ClearMessageList();

  RTE_UserProfileContainer  oProfile;
  oProfile.Open(szDBNameL, false, m_oEvent);

  if (!m_oEvent.IsEmpty()) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
    nFuncReturn = m_oEvent.ID();
  } else {

    // look for version 0 records
    nSourceRecord = (nSourceRecord  > UserRecordFormat_Curr_CN50) ? 0 : nSourceRecord;

    if (nSourceRecord < UserRecordFormat_Curr_CN50) {
      nTargetRecord = nSourceRecord + 1;
      do {
        switch (nTargetRecord) {
          case UserRecordFormat_1_CN50:
            nFuncReturn = convFrom0To1(szUserName, oProfile);
            break;
          case UserRecordFormat_2_CN50:
            nFuncReturn = convFrom1To2(szUserName, oProfile);
            break;
          case UserRecordFormat_3_CN50:
            nFuncReturn = convFrom2To3(szUserName, oProfile);
            break;
          default:
            // do nothing, version doesn't  exist
            break;
        } // end switch
        ++nTargetRecord;
      } while (nTargetRecord <  UserRecordFormat_Curr_CN50 && nFuncReturn == OK_CN00);

    } else if (pHeader->recHeader.nRecordSize != sizeof(DBMUserData_1)) {
      // only different sizes
      nFuncReturn = convFromXToX(szUserName, oProfile);
    } // end if

  } // end if

  return nFuncReturn;
} // end cn50DBMUser :: migrateRecord

/*
  -----------------------------------------------------------------------------
  pivate function cn50DBMUser :: convFrom1To2
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: convFrom1To2 ( const tcn00_UserNamec    szUserName,
                                          RTE_UserProfileContainer & oProfile )
{
  DBMUserData_1       Data_1;
  tsp00_Int4          nLength_1 = sizeof(DBMUserData_1);
  DBMUserData_2       Data_2;
  
  m_oEvent.ClearMessageList();

  // read old version
  bool retVal = oProfile.GetProfile(szUserName, nLength_1, &Data_1, m_oEvent);
  if (!retVal) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
  } else {
    if (nLength_1 != sizeof(DBMUserData_1)) { 
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
    } else {

//      Data_2.szDBName        = Data_1. szDBName;
      Data_2.szUserName      = Data_1.szUserName;
      Data_2.cryptMasterPwd  = Data_1.cryptMasterPwd;
      Data_2.cryptSecondPwd  = Data_1.cryptSecondPwd;
      Data_2.bSecondPwd      = Data_1.bSecondPwd;

      Data_2.nProperties     = 0;

      Data_2.nProperties     = (Data_1.bColdUser) ? (Data_2.nProperties | FIRSTDBM_CN50) : Data_2.nProperties;

      Data_2.nProperties     = (Data_1.bSDBAUser) ? (Data_2.nProperties | SYSDBA_CN50)   : Data_2.nProperties;

      Data_2.bDisabled       = Data_1.bDisabled;
      Data_2.dbmServerRights = Data_1.dbmServerRights;
      Data_2.dbmGUIRights    = Data_1.dbmGUIRights;
      Data_2.szComment       = Data_1.szComment;

      setSizes(Data_2);
      Data_2.recHeader.nRecordId   = UserRecordFormat_Curr_CN50;
      Data_2.recHeader.nRecordSize = sizeof(DBMUserData_2);

      m_oEvent.ClearMessageList();
      bool retVal = oProfile.DelProfile(Data_2.szUserName, m_oEvent);
      if (!retVal) {
        m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
      } else {
        m_oEvent.ClearMessageList();
        bool retVal = oProfile.SetProfile(Data_2.szUserName, sizeof(DBMUserData_2), &Data_2, m_oEvent);
        if (!retVal) {
          m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
        } // end if
      } // end if
    } // end if
  } // end if
   
  return m_oEvent.ID();
} // end cn50DBMUser :: convFrom1To2

/*
  -----------------------------------------------------------------------------
  pivate function cn50DBMUser :: convFrom2To3
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser::convFrom2To3(
        const tcn00_UserNamec szUserName,
        RTE_UserProfileContainer & oProfile ) 
{
    DBMUserData_2 Data_2;
    tsp00_Int4 nLength_2(sizeof(DBMUserData_2));
  
    m_oEvent.ClearMessageList();

    bool retVal(false);
    do { // this is no loop!
        if( !oProfile.GetProfile(szUserName, nLength_2, &Data_2, m_oEvent) )
            break;

        if (nLength_2 != sizeof(DBMUserData_2))
            break;

        if (Data_2.nProperties & FIRSTDBM_CN50) {
            // grant new permissions only to her...
            Data_2.dbmServerRights = Data_2.dbmServerRights + DBMRightShMAdmin_CN50    
                                                            + DBMRightSCDAdmin_CN50    
                                                            + DBMRightSCDUse_CN50      
                                                            + DBMRightEvtDispAdmin_CN50
                                                            + DBMRightEvtDispUse_CN50;
        } // end if

        setSizes(Data_2);
        Data_2.recHeader.nRecordId   = UserRecordFormat_Curr_CN50;
        Data_2.recHeader.nRecordSize = sizeof(DBMUserData_2);

        m_oEvent.ClearMessageList();
        if( !oProfile.DelProfile(Data_2.szUserName, m_oEvent) )
            break;

        m_oEvent.ClearMessageList();
        if( !oProfile.SetProfile(Data_2.szUserName, sizeof(DBMUserData_2), &Data_2, m_oEvent) )
            break;
        
        retVal = true;
        break;
    } while(true);
   
    if (!retVal) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
    }
    return m_oEvent.ID();
}

/*
  -----------------------------------------------------------------------------
  pivate function cn50DBMUser :: convFrom0To1
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: convFrom0To1 ( const tcn00_UserNamec    szUserName,
                                          RTE_UserProfileContainer & oProfile )
{
  DBMUserData_0       Data_0;
  tsp00_Int4          nLength_0 = sizeof(DBMUserData_0);
  DBMUserData_1       Data_1;
  
  m_oEvent.ClearMessageList();
  
  // read old version
  bool retVal = oProfile.GetProfile(szUserName, nLength_0, &Data_0, m_oEvent);
  if (!retVal) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
  } else {
    if (nLength_0 != sizeof(DBMUserData_0)) { 
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
    } else {

      Data_1.szDBName        = Data_0. szDBName;
      Data_1.szUserName      = Data_0.szUserName;
      Data_1.cryptMasterPwd  = Data_0.cryptMasterPwd;
      Data_1.cryptSecondPwd  = Data_0.cryptSecondPwd;
      Data_1.szSystemName    = Data_0.szSystemName;
      Data_1.cryptSystemPwd  = Data_0.cryptSystemPwd;
      Data_1.bSecondPwd      = Data_0.bSecondPwd;
      Data_1.bDBMUser        = Data_0.bDBMUser;
      Data_1.bSQLUser        = Data_0.bSQLUser;
      Data_1.bExistsInDB     = Data_0.bExistsInDB;
      Data_1.bColdUser       = Data_0.bColdUser;
      Data_1.bSDBAUser       = Data_0.bSDBAUser;
      Data_1.eUserMode       = Data_0.eUserMode;
      Data_1.bDisabled       = Data_0.bDisabled;
      Data_1.dbmServerRights = Data_0.dbmServerRights;
      Data_1.dbmGUIRights    = Data_0.dbmGUIRights;
      Data_1.szComment       = Data_0.szComment;

      Data_1.recHeader.nRecordId   = UserRecordFormat_Curr_CN50;
      Data_1.recHeader.nRecordSize = sizeof(DBMUserData_1);

      m_oEvent.ClearMessageList();
      bool retVal = oProfile.DelProfile(Data_1.szUserName, m_oEvent);
      if (!retVal) {
        m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
      } else {
        m_oEvent.ClearMessageList();
        bool retVal = oProfile.SetProfile(Data_1.szUserName, sizeof(DBMUserData_1), &Data_1, m_oEvent);
        if (!retVal) {
          m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
        } // end if
      } // end if
    } // end if
  } // end if
   
  return m_oEvent.ID();
} // end cn50DBMUser :: convFrom0To1

/*
  -----------------------------------------------------------------------------
  pivate function cn50DBMUser :: convFromXToX
  -----------------------------------------------------------------------------
 */
tcn00_Error cn50DBMUser :: convFromXToX ( const tcn00_UserNamec    szUserName,
                                          RTE_UserProfileContainer & oProfile )
{
  FUNCTION_DBG_MCN00_1("cn50DBMUser :: convFromXToX");

  m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFRM));
  return m_oEvent.ID();
} // end cn50DBMUser :: convFromXToX

/*
  -----------------------------------------------------------------------------
  public static function cn50DBMUser :: tryRestoreIfEmpty
  -----------------------------------------------------------------------------
 */
bool  cn50DBMUser :: tryRestoreIfEmpty
    ( const tsp00_DbNamec & szDBName )
{
  if (isEmpty(szDBName)) {
    DBMSrvMsg_Error oError;
    cn42RestoreFile(szDBName, FGET_USER_CN42, oError);
    return true;
  } // end if
  
  return false;
} // end cn50DBMUser :: tryRestoreIfEmpty

/*
  -----------------------------------------------------------------------------
  public static function cn50DBMUser :: isEmpty
  -----------------------------------------------------------------------------
 */
bool  cn50DBMUser :: isEmpty
    ( const tsp00_DbNamec & szDBName )
{
  // check if upc is empty 
  tsp00_Int4              nLength = 0;
  tsp00_Uint4             nToken = FIRST_TOKEN;
  tcn00_UserNamec         szUserName;
  Msg_List                m_oEvent;

  m_oEvent.ClearMessageList();

  RTE_UserProfileContainer oProfile;
  oProfile.Open(szDBName, true, m_oEvent);

  oProfile.ScanProfiles(nToken, nLength, NULL, szUserName, m_oEvent);
    
  if(m_oEvent.IsEmpty() && nToken == NO_TOKEN) {
    return true;
  } // end if
  
  return false;
} // end cn50DBMUser :: isEmpty
