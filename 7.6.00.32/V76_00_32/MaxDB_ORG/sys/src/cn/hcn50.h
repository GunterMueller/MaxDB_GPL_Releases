/*!
  -----------------------------------------------------------------------------

  module: hcn50.h

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer User Management

  description:  DBMServer User Management - Specification

  version:      7.2.*

  -----------------------------------------------------------------------------

  Copyright (c) 1998-2005 SAP AG

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

#ifndef _HCN50_H_
#define _HCN50_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gcn00.h"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*
  -----------------------------------------------------------------------------
  specification public macros
  -----------------------------------------------------------------------------
 */
// all and nothing
#define DBMMaskAll_CN50                 ((tsp00_Uint4) 0xFFFFFFFF)
#define DBMMaskNothing_CN50             ((tsp00_Uint4) 0x00000000)

// server rights
#define DBMRightUserMgm_CN50            ((tsp00_Uint4) 0x00000001)
#define DBMRightInstallMgm_CN50         ((tsp00_Uint4) 0x00000002)
#define DBMRightSystemCmd_CN50          ((tsp00_Uint4) 0x00000004)
#define DBMRightFileFull_CN50           ((tsp00_Uint4) 0x00000008)
#define DBMRightFileRead_CN50           ((tsp00_Uint4) 0x00000010)
#define DBMRightDBFileRead_CN50         ((tsp00_Uint4) 0x00000020)
#define DBMRightAccessUtility_CN50      ((tsp00_Uint4) 0x00000040)
#define DBMRightBackupRestore_CN50      ((tsp00_Uint4) 0x00000080)
#define DBMRightBackupSave_CN50         ((tsp00_Uint4) 0x00000100)
#define DBMRightParamFull_CN50          ((tsp00_Uint4) 0x00000200)
#define DBMRightParamCheckWrite_CN50    ((tsp00_Uint4) 0x00000400)
#define DBMRightUnused11_CN50           ((tsp00_Uint4) 0x00000800)
#define DBMRightUnused12_CN50           ((tsp00_Uint4) 0x00001000)
#define DBMRightParamRead_CN50          ((tsp00_Uint4) 0x00002000)
#define DBMRightAccessSQL_CN50          ((tsp00_Uint4) 0x00004000)
#define DBMRightExecLoad_CN50           ((tsp00_Uint4) 0x00008000)
#define DBMRightLoadSysTab_CN50         ((tsp00_Uint4) 0x00010000)
#define DBMRightDBStop_CN50             ((tsp00_Uint4) 0x00020000)
#define DBMRightDBStart_CN50            ((tsp00_Uint4) 0x00040000)
#define DBMRightDBInfoRead_CN50         ((tsp00_Uint4) 0x00080000)
#define DBMRightShMAdmin_CN50           ((tsp00_Uint4) 0x00100000)
#define DBMRightSCDAdmin_CN50           ((tsp00_Uint4) 0x00200000)
#define DBMRightSCDUse_CN50             ((tsp00_Uint4) 0x00400000)
#define DBMRightEvtDispAdmin_CN50       ((tsp00_Uint4) 0x00800000)
#define DBMRightEvtDispUse_CN50         ((tsp00_Uint4) 0x01000000)
#define DBMRightUnused25_CN50           ((tsp00_Uint4) 0x02000000)
#define DBMRightUnused26_CN50           ((tsp00_Uint4) 0x04000000)
#define DBMRightUnused27_CN50           ((tsp00_Uint4) 0x08000000)
#define DBMRightUnused28_CN50           ((tsp00_Uint4) 0x10000000)
#define DBMRightUnused29_CN50           ((tsp00_Uint4) 0x20000000)
#define DBMRightUnused30_CN50           ((tsp00_Uint4) 0x40000000)
#define DBMRightUnused31_CN50           ((tsp00_Uint4) 0x80000000)

// GUI rights
#define DBMGUIRight01_CN50              ((tsp00_Uint4) 0x00000001)
#define DBMGUIRight02_CN50              ((tsp00_Uint4) 0x00000002)
#define DBMGUIRight03_CN50              ((tsp00_Uint4) 0x00000004)
#define DBMGUIRight04_CN50              ((tsp00_Uint4) 0x00000008)
#define DBMGUIRight05_CN50              ((tsp00_Uint4) 0x00000010)
#define DBMGUIRight06_CN50              ((tsp00_Uint4) 0x00000020)
#define DBMGUIRight07_CN50              ((tsp00_Uint4) 0x00000040)
#define DBMGUIRight08_CN50              ((tsp00_Uint4) 0x00000080)
#define DBMGUIRight09_CN50              ((tsp00_Uint4) 0x00000100)
#define DBMGUIRight10_CN50              ((tsp00_Uint4) 0x00000200)
#define DBMGUIRight11_CN50              ((tsp00_Uint4) 0x00000400)
#define DBMGUIRight12_CN50              ((tsp00_Uint4) 0x00000800)
#define DBMGUIRight13_CN50              ((tsp00_Uint4) 0x00001000)
#define DBMGUIRight14_CN50              ((tsp00_Uint4) 0x00002000)
#define DBMGUIRight15_CN50              ((tsp00_Uint4) 0x00004000)
#define DBMGUIRight16_CN50              ((tsp00_Uint4) 0x00008000)
#define DBMGUIRight17_CN50              ((tsp00_Uint4) 0x00010000)
#define DBMGUIRight18_CN50              ((tsp00_Uint4) 0x00020000)
#define DBMGUIRight19_CN50              ((tsp00_Uint4) 0x00040000)
#define DBMGUIRight20_CN50              ((tsp00_Uint4) 0x00080000)
#define DBMGUIRight21_CN50              ((tsp00_Uint4) 0x00100000)
#define DBMGUIRight22_CN50              ((tsp00_Uint4) 0x00200000)
#define DBMGUIRight23_CN50              ((tsp00_Uint4) 0x00400000)
#define DBMGUIRight24_CN50              ((tsp00_Uint4) 0x00800000)
#define DBMGUIRight25_CN50              ((tsp00_Uint4) 0x01000000)
#define DBMGUIRight26_CN50              ((tsp00_Uint4) 0x02000000)
#define DBMGUIRight27_CN50              ((tsp00_Uint4) 0x04000000)
#define DBMGUIRight28_CN50              ((tsp00_Uint4) 0x08000000)
#define DBMGUIRight29_CN50              ((tsp00_Uint4) 0x10000000)
#define DBMGUIRight30_CN50              ((tsp00_Uint4) 0x40000000)
#define DBMGUIRight31_CN50              ((tsp00_Uint4) 0x80000000)


// default for dbm user
#define DBMUserDefaultGUI_CN50          ((tsp00_Uint4) DBMMaskNothing_CN50)
#define DBMUserDefault_CN50             ((tsp00_Uint4) DBMRightUserMgm_CN50            + \
                                                       DBMRightInstallMgm_CN50         + \
                                                       DBMRightSystemCmd_CN50          + \
                                                       DBMRightFileFull_CN50           + \
                                                       DBMRightFileRead_CN50           + \
                                                       DBMRightDBFileRead_CN50         + \
                                                       DBMRightAccessUtility_CN50      + \
                                                       DBMRightBackupRestore_CN50      + \
                                                       DBMRightBackupSave_CN50         + \
                                                       DBMRightParamFull_CN50          + \
                                                       DBMRightParamCheckWrite_CN50    + \
                                                       DBMRightParamRead_CN50          + \
                                                       DBMRightAccessSQL_CN50          + \
                                                       DBMRightExecLoad_CN50           + \
                                                       DBMRightLoadSysTab_CN50         + \
                                                       DBMRightDBStop_CN50             + \
                                                       DBMRightDBStart_CN50            + \
                                                       DBMRightDBInfoRead_CN50         + \
                                                       DBMRightShMAdmin_CN50           + \
                                                       DBMRightSCDAdmin_CN50           + \
                                                       DBMRightSCDUse_CN50             + \
                                                       DBMRightEvtDispAdmin_CN50       + \
                                                       DBMRightEvtDispUse_CN50)

// Record identifier
#define UserRecordFormat_3_CN50                        ((tsp00_Uint4) 3) // for granting new permissions
                                                                         // DBMRightShMAdmin_CN50,...,
                                                                         // DBMRightEvtDispUse_CN50 to first dbm user
#define UserRecordFormat_2_CN50                        ((tsp00_Uint4) 2)
#define UserRecordFormat_1_CN50                        ((tsp00_Uint4) 1)

#define UserRecordFormat_Curr_CN50                     UserRecordFormat_3_CN50

// others
#define INVALID_USER_CN50                              "SYS"

// special properties
#define FIRSTDBM_CN50                                  ((tsp00_Uint4) 1)
#define SYSDBA_CN50                                    ((tsp00_Uint4) 2)
#define SAPUSR_CN50                                    ((tsp00_Uint4) 4)

// user types
#define USERTYPE_DBM_CN50                              "DBM"
#define USERTYPE_DBA_CN50                              "DBA"
#define USERTYPE_SAP_CN50                              "SAP"

// user type argument for DBMServer commands
#define USER_TYPE_CN50          "user-type"
#define ASSIGN_CHAR_CN50        '='


/*
  -----------------------------------------------------------------------------
  forward deklarations
  -----------------------------------------------------------------------------
 */
class RTE_UserProfileContainer;

/*!
  -----------------------------------------------------------------------------
  Class:        cn50DBMUser
  -----------------------------------------------------------------------------

  description:  DBM User Object

                An instance of this class represents an User of the
                database instance

  -----------------------------------------------------------------------------
 */
class cn50DBMUser
{
  public:

    /*!
      Chapter: Construction and destruction of class
     */

    /*!
      -------------------------------------------------------------------------
      function:     cn50DBMUser
      -------------------------------------------------------------------------
      description:  "Read"-Constructor

                    This constructor loads the specified user from the
                    user data container. If there are no user data available
                    an empty user object will be created an existsOnDisk()
                    returns false.

      arguments:    szDBName   [IN] - the name of the database
                    szUserName [IN] - the name of the user

      prototypes:   3
      -------------------------------------------------------------------------
     */
     cn50DBMUser();
     cn50DBMUser(const char * szDBName);
     cn50DBMUser(const char * szDBName, const char * szUserName);

    /*!
      EndChapter: Construction and destruction of class
     */

    /*!
      Chapter: Write data to object
     */

    /*!
      -------------------------------------------------------------------------
      function:     setDBName
      -------------------------------------------------------------------------
      description:  set the name of the database instance

      arguments:    szValue    [IN] - database name
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setDBName       ( const char * szValue );

    /*!
      -------------------------------------------------------------------------
      function:     setUserName
      -------------------------------------------------------------------------
      description:  set the name of the user

      arguments:    szValue    [IN] - user name

      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setUserName     ( const char * szValue );

    /*!
      -------------------------------------------------------------------------
      function:     setMasterPwd
      -------------------------------------------------------------------------
      description:  set the first (master) password of the user

      arguments:    szValue    [IN] - passsword

      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setMasterPwd    ( const char * szValue );

    /*!
      -------------------------------------------------------------------------
      function:     setSecondPwd
      -------------------------------------------------------------------------
      description:  set the second password of the user

      arguments:    szValue    [IN] - passsword

      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setSecondPwd    ( const char * szValue );

    /*!
      -------------------------------------------------------------------------
      function:     setLoggedOn
      -------------------------------------------------------------------------
      description:  set the logged on state

      return value: the logged on state
      -------------------------------------------------------------------------
     */
     bool           setLoggedOn      (bool bIn = true) {bLoggedOn=bIn;return bLoggedOn;}

    /*!
      -------------------------------------------------------------------------
      function:     setColdUser
      -------------------------------------------------------------------------
      description:  set ColdUser flag

                    ColdUser means, this is the user for operations at
                    the cold kernel

      arguments:    bValue    [IN] - new value for flag (default = true)

      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setColdUser     ( bool const bValue = true, bool const bUnset = true );

    /*!
      -------------------------------------------------------------------------
      function:     setSDBAUser
      -------------------------------------------------------------------------
      description:  set SDBAUser flag

                    SDBAUser means, this is the user used on
                    "ACTIVATE SERVERDB"

      arguments:    bValue    [IN] - new value for flag (default = true)

      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setSDBAUser     ( bool const bValue = true );

    /*!
      -------------------------------------------------------------------------
      function:     setSAPUser
      -------------------------------------------------------------------------
      description:  set SAPUser flag

      arguments:    bValue    [IN] - new value for flag (default = true)

      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setSAPUser     ( bool const bValue = true );


    /*!
      -------------------------------------------------------------------------
      function:     setDisabled
      -------------------------------------------------------------------------
      description:  enable/disable account

      arguments:    bValue    [IN] - new value for flag (default = true)

      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setDisabled     ( bool const bValue = true );

    /*!
      -------------------------------------------------------------------------
      function:     setServerRights
      -------------------------------------------------------------------------
      description:  set rights for server access

      arguments:    nMask     [IN] - only the right in this mask will be
                                     affected (DBMRight*_CN50 - values)
                    nValues   [IN] - the new values for the rights specified
                                     in nMask (DBMRight*_CN50 - values)
      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setServerRights ( tsp00_Uint4 const nMask, tsp00_Uint4 const nValues );

    /*!
      -------------------------------------------------------------------------
      function:     setGUIRights
      -------------------------------------------------------------------------
      description:  set rights for GUI access

      arguments:    nMask     [IN] - only the right in this mask will be
                                     affected (DBMRightGUI*_CN50 - values)
                    nValues   [IN] - the new values for the rights specified
                                     in nMask (DBMRightGUI*_CN50 - values)
      return value: the C++ object
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setGUIRights    ( tsp00_Uint4 const nMask, tsp00_Uint4 const nValues );

    /*!
      -------------------------------------------------------------------------
      function:     setData
      -------------------------------------------------------------------------
      description:  set the user data by data string

                    Format of data string is
                    <property>=<value> <property>=<value> ...
                    Properties are:
                      SERVERRIGHTS
                      GUIRIGHTS
                      PASSWORD
                      SECONDPASSWORD

                    For server rights use the logical names with + or -
                    e.g. SERVERRIGHTS=+LoadSysTab,-AccessSQL
                    grants the right "LoadSysTab" and removes the
                    right "AccessSQL"

      arguments:    szData    [IN] - data string

      return value: DBMServer Error-Code
      -------------------------------------------------------------------------
     */
     tcn00_Error setData ( const _TCHAR * szData );

    /*!
      -------------------------------------------------------------------------
      function:     setComment
      -------------------------------------------------------------------------
      description:  set the comment

      arguments:    szValue    [IN] - database name
      -------------------------------------------------------------------------
     */
     cn50DBMUser & setComment      ( const char * szValue );

    /*!
      EndChapter: Write data to object
     */

    /*!
      Chapter: Use object data
     */

    /*!
      -------------------------------------------------------------------------
      function:     checkMasterPwd
      -------------------------------------------------------------------------
      description:  check the first (master) password  of the user

                    check will fail if the user is disbaled

      arguments:    szValue   [IN] - password

      return value: true on success, otherwise false
      -------------------------------------------------------------------------
     */
     bool           checkMasterPwd  ( const char * szValue );

    /*!
      -------------------------------------------------------------------------
      function:     checkSecondPwd
      -------------------------------------------------------------------------
      description:  check the second password of the user

                    check will fail if the user is disbaled or there is
                    no second password

      arguments:    szValue   [IN] - password

      return value: true on success, otherwise false
      -------------------------------------------------------------------------
     */
     bool           checkSecondPwd  ( const char * szValue );

    /*!
      -------------------------------------------------------------------------
      function:     getDBName
      -------------------------------------------------------------------------
      description:  read database name

      return value: database name
      -------------------------------------------------------------------------
     */
     tsp00_DbNamec    getDBName       ( ) const {return szDBName;}

    /*!
      -------------------------------------------------------------------------
      function:     getUserName
      -------------------------------------------------------------------------
      description:  read user name

      return value: user name
      -------------------------------------------------------------------------
     */
     tcn00_UserNamec  getUserName     ( ) const {return Data.szUserName;}

    /*!
      -------------------------------------------------------------------------
      function:     getMasterPwd
      -------------------------------------------------------------------------
      description:  read the first (master) password

      return value: password (appl-crypted or clear )

      prototypes:   2
      -------------------------------------------------------------------------
     */
     tsp00_CryptPw    getMasterPwd    ( ) const {return Data.cryptMasterPwd;};
     tsp00_C64c       getMasterPwd    ( bool bUncrypt ) const;

    /*!
      -------------------------------------------------------------------------
      function:     getClearMasterPwd
      -------------------------------------------------------------------------
      description:  read the first (master) password

      return value: clear password
      -------------------------------------------------------------------------
     */
     tsp00_Namec      getClearMasterPwd    ( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     getSecondPwd
      -------------------------------------------------------------------------
      description:  read the second password

      return value: password (appl-crypted)

      prototypes:   2
      -------------------------------------------------------------------------
     */
     tsp00_CryptPw    getSecondPwd    ( ) const {return Data.cryptSecondPwd;}
     tsp00_C64c       getSecondPwd    ( bool bUncrypt ) const;

    /*!
      -------------------------------------------------------------------------
      function:     isSecondPwd
      -------------------------------------------------------------------------
      description:  is there a second password

      return value: true on success, otherwise false
      -------------------------------------------------------------------------
     */
     bool           isSecondPwd     ( ) const {return Data.bSecondPwd;}

    /*!
      -------------------------------------------------------------------------
      function:     isColdUser
      -------------------------------------------------------------------------
      description:  check Colduser property

      return value: value of flag
      -------------------------------------------------------------------------
     */
     bool           isColdUser     ( ) const {return ((Data.nProperties & FIRSTDBM_CN50) != 0);};

    /*!
      -------------------------------------------------------------------------
      function:     isSDBAUser
      -------------------------------------------------------------------------
      description:  check SYSDBA property

      return value: value of flag
      -------------------------------------------------------------------------
     */
     bool           isSDBAUser     ( ) const {return ((Data.nProperties & SYSDBA_CN50) != 0);}

    /*!
      -------------------------------------------------------------------------
      function:     isSAPUser
      -------------------------------------------------------------------------
      description:  check SAPUser property

      return value: value of flag
      -------------------------------------------------------------------------
     */
     bool           isSAPUser     ( ) const {return ((Data.nProperties & SAPUSR_CN50) != 0);}

    /*!
      -------------------------------------------------------------------------
      function:     isDisabled
      -------------------------------------------------------------------------
      description:  read disabled flag

      return value: value of flag
      -------------------------------------------------------------------------
     */
     bool           isDisabled     ( ) const {return Data.bDisabled;}

    /*!
      -------------------------------------------------------------------------
      function:     getServerRights
      -------------------------------------------------------------------------
      description:  read the server rights bitfield

      return value: server rights
      -------------------------------------------------------------------------
     */
     tsp00_Uint4      getServerRights ( ) const {return Data.dbmServerRights;}

    /*!
      -------------------------------------------------------------------------
      function:     getGUIRights
      -------------------------------------------------------------------------
      description:  read the GUI rights bitfield

      return value: GUI rights
      -------------------------------------------------------------------------
     */
     tsp00_Uint4      getGUIRights    ( ) const {return Data.dbmGUIRights;}

    /*!
      -------------------------------------------------------------------------
      function:     isServerRight
      -------------------------------------------------------------------------
      description:  check if server right is set

      arguments:    nRights   [IN] - rights to check
                    bAnd      [IN] - return true only, if all rights specified
                                     in nRight are set (default false - returns
                                     true if only one right is set)

      return value: true on success, otherwise false
      -------------------------------------------------------------------------
     */
     bool           isServerRight   ( tsp00_Uint4 const nRights, bool const bAnd = false ) const;

    /*!
      -------------------------------------------------------------------------
      function:     isGUIRight
      -------------------------------------------------------------------------
      description:  check if GUI right is set

      arguments:    nRights   [IN] - rights to check
                    bAnd      [IN] - return true only, if all rights specified
                                     in nRight are set (default false - returns
                                     true if only one right is set)

      return value: true on success, otherwise false
      -------------------------------------------------------------------------
     */
     bool           isGUIRight      ( tsp00_Uint4 const nRights, bool const bAnd = false ) const;

    /*!
      -------------------------------------------------------------------------
      function:     getComment
      -------------------------------------------------------------------------
      description:  read comment

      return value: comment
      -------------------------------------------------------------------------
     */
     tsp00_C64c     getComment      ( ) const {return Data.szComment;}
    /*!
      -------------------------------------------------------------------------
      function:     getData
      -------------------------------------------------------------------------
      description:  read user data for DBMServer answer

      arguments:    pBuffer  [OUT] - buffer for the data
      -------------------------------------------------------------------------
     */
     void           getData         ( _TCHAR * pBuffer ) const;

    /*!
      -------------------------------------------------------------------------
      function:     getUPCOwner
      -------------------------------------------------------------------------
      description:  read the owner of UPC

      return value: user name
      -------------------------------------------------------------------------
     */
     tcn00_UserNamec getUPCOwner     ( );

    /*!
      -------------------------------------------------------------------------
      function:     setSupportGroup
      -------------------------------------------------------------------------
      description:  set the file group ("support" group) for the UPC

      return value: user name
      -------------------------------------------------------------------------
     */
     bool setSupportGroup            (tsp00_Namec const & szGroupname );

    /*!
      -------------------------------------------------------------------------
      function:     tryRestoreIfEmpty
      -------------------------------------------------------------------------
      description:  Restores the backup copy of the profile if orignal profile is empty,
                    This function dioes not check wheter restore was succesfull
                    or not. Try a reload to check success.
      arguments:    szDBName [IN] - name of database instance
      return value: restore tried or not
      -------------------------------------------------------------------------
     */
     static bool  tryRestoreIfEmpty ( const tsp00_DbNamec & szDBName );

    /*!
      -------------------------------------------------------------------------
      function:     isEmtpy
      -------------------------------------------------------------------------
      description:  checks whether the upc is empty or not
      arguments:    szDBName [IN] - name of database instance
      return value: successfull or not
      -------------------------------------------------------------------------
     */
     static bool  isEmpty ( const tsp00_DbNamec & szDBName );

    /*!
      -------------------------------------------------------------------------
      function:     getColdUser
      -------------------------------------------------------------------------
      description:  read the name of the Colduser
      arguments:    szDBName [IN] - name of database instance
      return value: user name
      prototypes:   2
      -------------------------------------------------------------------------
     */
     static tcn00_UserNamec getColdUser     ( const tsp00_DbNamec & szDBName ) {return getUser(szDBName, FIRSTDBM_CN50);}
            tcn00_UserNamec getColdUser     ( ) {return getUser( FIRSTDBM_CN50);}

    /*!
      -------------------------------------------------------------------------
      function:     getSYSDBA
      -------------------------------------------------------------------------
      description:  read the name of the SYSDBA
      arguments:    szDBName [IN] - name of database instance
      return value: user name
      prototypes:   2
      -------------------------------------------------------------------------
     */
     static tcn00_UserNamec  getSYSDBA       ( const tsp00_DbNamec & szDBName ) {return getUser(szDBName, SYSDBA_CN50);}
            tcn00_UserNamec  getSYSDBA       ( ) {return getUser(SYSDBA_CN50);}

    /*!
      -------------------------------------------------------------------------
      function:     getSAPUser
      -------------------------------------------------------------------------
      description:  read the name of the SAPUSer
      arguments:    szDBName [IN] - name of database instance
      return value: user name
      prototypes:   2
      -------------------------------------------------------------------------
     */
     static tcn00_UserNamec  getSAPUser      ( const tsp00_DbNamec & szDBName ) {return getUser(szDBName, SAPUSR_CN50);}
            tcn00_UserNamec  getSAPUser      ( ) {return getUser( SAPUSR_CN50);}


    /*!
      -------------------------------------------------------------------------
      function:     getUser
      -------------------------------------------------------------------------
      description:  read the name of the user with the specified property
      arguments:    szDBName  [IN] - name of database instance
                    nPorperty [IN] - property of user (e.g. FIRSTDBM_CN50,
                                     SYSDBA_CN50, SAPUSR_CN50)
      return value: user name
      prototypes:   2
      -------------------------------------------------------------------------
     */
    static tcn00_UserNamec getUser       ( const tsp00_DbNamec    & szDBName,
                                           tsp00_Uint4              nProperty );
    static tcn00_UserNamec getUser       ( const char *             szDBName,
                                           tsp00_Uint4              nProperty );
    tcn00_UserNamec        getUser       ( tsp00_Uint4              nProperty );

    /*!
      -------------------------------------------------------------------------
      function:     listRights
      -------------------------------------------------------------------------
      description:  read user data for DBMServer answer

      arguments:    pBuffer  [OUT] - buffer for the data
                    szMode   [IN]  - buffer with "SERVERRIGHTS" for server rights
                                     or "GUIRIGHTS" for GUI rights
     return value: DBMServer Error-Code
      -------------------------------------------------------------------------
     */
     tcn00_Error    listRights      ( _TCHAR * pBuffer, const _TCHAR * szMode ) const;

    /*!
      EndChapter: Use object data
     */

    /*!
      Chapter: Object state
     */

    /*!
      -------------------------------------------------------------------------
      function:     lastEvent
      -------------------------------------------------------------------------
      description:  get the last Event List object

      return value: the object
      -------------------------------------------------------------------------
     */
     const DBMSrvMsg_Error & lastEvent ( ) const {return m_oEvent;}


    /*!
      -------------------------------------------------------------------------
      function:     isLoggedOn
      -------------------------------------------------------------------------
      description:  is this user logged on

      return value: true if yes, otherwise false
      -------------------------------------------------------------------------
     */
     bool           isLoggedOn      ( ) const {return bLoggedOn;}

    /*!
      -------------------------------------------------------------------------
      function:     clear
      -------------------------------------------------------------------------
      description:  clear all data
      -------------------------------------------------------------------------
     */
     void           clear           ( );

    /*!
      -------------------------------------------------------------------------
      function:     isThis
      -------------------------------------------------------------------------
      description:  is this the user

      arguments:    szUserName - name to check

      return value: true if it is, otherwise false
      -------------------------------------------------------------------------
     */
     bool           isThis         ( tcn00_UserNamec const szUserName) const {return (stricmp(szUserName, Data.szUserName) == 0);}


    /*!
      -------------------------------------------------------------------------
      function:     existsOnDisk
      -------------------------------------------------------------------------
      description:  exists the user?

      return value: true if yes, otherwise false
      -------------------------------------------------------------------------
     */
     bool           existsOnDisk      ( ) const {return bExistsOnDisk;}
     
     
     /*!
      EndChapter: Object state
     */

    /*!
      Chapter: Disk access
     */

    /*!
      -------------------------------------------------------------------------
      function:     save
      -------------------------------------------------------------------------
      description:  save user to disk

      arguments:    bRecover - Recovery mode - create only upc entry

      return value: DBMServer Error-Code
      -------------------------------------------------------------------------
     */
     tcn00_Error    save            ( bool bRecover = false );

    /*!
      -------------------------------------------------------------------------
      function:     reload
      -------------------------------------------------------------------------
      description:  read user from disk

                    Unsaved data will be overwritten!

      return value: DBMServer Error-Code
      -------------------------------------------------------------------------
     */
     tcn00_Error    reload          ( );

    /*!
      -------------------------------------------------------------------------
      function:     deleteUsr
      -------------------------------------------------------------------------
      description:  delete this user entry at the disk

      return value: DBMServer Error-Code
      -------------------------------------------------------------------------
     */
     tcn00_Error    deleteUsr       ( );

    /*!
      EndChapter: Disk access
     */

public:
// always constant header format
    typedef struct DBMUserHeader_Tag {
        tsp00_Uint2      nRecordId;
        tsp00_Uint2      nRecordSize;
    } DBMUserHeader;

// current record format version 2
    typedef struct DBMUserData_2_Tag {
      DBMUserHeader   recHeader;

      tsp00_Uint2      s_szDBName;
      tsp00_DbNamec    szDBNameX;

      tsp00_Uint2      s_szUserName;
      tcn00_UserNamec  szUserName;

      tsp00_Uint2      s_cryptMasterPwd;
      tsp00_CryptPw    cryptMasterPwd;

      tsp00_Uint2      s_cryptSecondPwd;
      tsp00_CryptPw    cryptSecondPwd;

      tsp00_Uint2      s_bSecondPwd;
      bool             bSecondPwd;

      tsp00_Uint2      s_nProperties;
      tsp00_Uint4      nProperties;

      tsp00_Uint2      s_bDisabled;
      bool             bDisabled;

      tsp00_Uint2      s_dbmServerRights;
      tsp00_Uint4      dbmServerRights;

      tsp00_Uint2      s_dbmGUIRights;
      tsp00_Uint4      dbmGUIRights;

      tsp00_Uint2      s_szComment;
      tsp00_C64c       szComment;

    } DBMUserData_2;

protected:
    typedef DBMUserData_2 DBMUserData;

    // User record
    DBMUserData        Data;

    // last error message
    DBMSrvMsg_Error    m_oEvent;

    // some flags and data
    bool           bExistsOnDisk;
    bool           bDirty;
    bool           bPwdDirty;
    bool           bSecondPwdDirty;
    bool           bNameDirty;
    bool           bLoggedOn;
    tsp00_DbNamec  szDBName;

    // private type
    typedef struct RightToTextT {
      tsp00_Int4     nRight;
      const _TCHAR * szName;
      const _TCHAR * szDescription;
    } RightToTextT;

    // private functions
    bool            isDirty       ( ) const {return bDirty;}
    
    cn50DBMUser &   setRights     ( char        * szValue, 
                                    tsp00_Uint4 & nValue, 
                                    RightToTextT RightList[] );
    tcn00_Error            saveColdUsr   ( );
    tcn00_Error            saveSYSDBAUsr ( );
    void                   setSizes      ( DBMUserData            & Data ) const;
    tcn00_Error            reload        ( DBMUserData            & oData, 
                                    RTE_UserProfileContainer & Profile);
    const char *           getType       ( ) const;

    // Handling of old record formats
    tcn00_Error   migrateRecord   ( const tsp00_DbNamec    & szDBName,
                                    const tcn00_UserNamec  & szUserName,
                                    const DBMUserData      * pHeader );

    tcn00_Error   convFrom2To3    ( const tcn00_UserNamec    szUserName,
                                    RTE_UserProfileContainer & oProfile );

    tcn00_Error   convFrom1To2    ( const tcn00_UserNamec    szUserName,
                                    RTE_UserProfileContainer & oProfile );

    tcn00_Error   convFrom0To1    ( const tcn00_UserNamec    szUserName,
                                    RTE_UserProfileContainer & oProfile );

    tcn00_Error   convFromXToX    ( const tcn00_UserNamec    szUserName,
                                    RTE_UserProfileContainer & oProfile );

public:
    // old record format version 1
    typedef enum {
      SQLUsrModeDBA,
      SQLUsrModeRESOURCE,
      SQLUsrModeSTANDARD
    } SQLUserMode;

    typedef struct DBMUserData_1_Tag {
      DBMUserHeader   recHeader;

      tsp00_Uint2      s_szDBName;
      tsp00_DbNamec    szDBName;

      tsp00_Uint2      s_szUserName;
      tcn00_UserNamec  szUserName;

      tsp00_Uint2      s_cryptMasterPwd;
      tsp00_CryptPw    cryptMasterPwd;

      tsp00_Uint2      s_cryptSecondPwd;
      tsp00_CryptPw    cryptSecondPwd;

      tsp00_Uint2      s_szSystemName;
      tcn00_UserNamec  szSystemName;

      tsp00_Uint2      s_cryptSystemPwd;
      tsp00_CryptPw    cryptSystemPwd;

      tsp00_Uint2      s_bSecondPwd;
      bool             bSecondPwd;

      tsp00_Uint2      s_bDBMUser;
      bool             bDBMUser;

      tsp00_Uint2      s_bSQLUser;
      bool             bSQLUser;

      tsp00_Uint2      s_bExistsInDB;
      bool             bExistsInDB;

      tsp00_Uint2      s_bColdUser;
      bool             bColdUser;

      tsp00_Uint2      s_bSDBAUser;
      bool             bSDBAUser;

      tsp00_Uint2      s_eUserMode;
      SQLUserMode      eUserMode;

      tsp00_Uint2      s_bDisabled;
      bool             bDisabled;

      tsp00_Uint2      s_dbmServerRights;
      tsp00_Uint4      dbmServerRights;

      tsp00_Uint2      s_dbmGUIRights;
      tsp00_Uint4      dbmGUIRights;

      tsp00_Uint2      s_szComment;
      tsp00_C64c       szComment;

    } DBMUserData_1;

    // old record format version 0
    typedef struct  DBMUserData_0_Tag {
      tsp00_DbNamec    szDBName;
      tcn00_UserNamec  szUserName;
      tsp00_CryptPw    cryptMasterPwd;
      tsp00_CryptPw    cryptSecondPwd;
      tcn00_UserNamec  szSystemName;
      tsp00_CryptPw    cryptSystemPwd;
      bool             bSecondPwd;
      bool             bDBMUser;
      bool             bSQLUser;
      bool             bExistsInDB;
      bool             bColdUser;
      bool             bSDBAUser;
      SQLUserMode      eUserMode;
      bool             bDisabled;
      tsp00_Uint4      dbmServerRights;
      tsp00_Uint4      dbmGUIRights;
      tsp00_C64c       szComment;

    } DBMUserData_0;

}; // end class cn50DBMUser

/*! EndClass: cn50DBMUser */

#endif /* _HCN50_H_ */
