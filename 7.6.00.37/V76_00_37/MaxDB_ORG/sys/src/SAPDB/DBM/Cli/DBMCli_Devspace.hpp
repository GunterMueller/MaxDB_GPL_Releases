/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Devspace.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Devspace Class
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
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

#ifndef _DBMCLI_DEVSPACE_HPP_
#define _DBMCLI_DEVSPACE_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;
class DBMCli_Devspace;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum 
{ 
	DBMCLI_LOGMODE_UNKNOWN = 0,
	DBMCLI_LOGMODE_SINGLE  = 1,
	DBMCLI_LOGMODE_DUAL    = 2,
	DBMCLI_LOGMODE_DEMO    = 3
} DBMCli_LogMode;

typedef enum
{
  DBMCLI_DEVSPACECLASS_UNKNOWN = 0,
  DBMCLI_DEVSPACECLASS_SYS     = 1,
  DBMCLI_DEVSPACECLASS_DATA    = 2,
  DBMCLI_DEVSPACECLASS_LOG     = 3
} DBMCli_DevspaceClass;

typedef enum
{
  DBMCLI_DEVSPACETYPE_UNKNOWN = 0,
  DBMCLI_DEVSPACETYPE_FILE    = 1,
  DBMCLI_DEVSPACETYPE_RAW     = 2,
  DBMCLI_DEVSPACETYPE_LINK    = 3
} DBMCli_DevspaceType;

typedef DBMCli_Array< DBMCli_Devspace, DBMCli_Devspace > DBMCli_DevspaceArray;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Devspace
  -----------------------------------------------------------------------------
  description: Handle a single devspace.
  -----------------------------------------------------------------------------
*/
class DBMCli_Devspace
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Devspace
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    nClass    [IN]  - class of devspace
                    nNumber   [IN]  - type of devspace
                    nPages    [IN]  - size of devspace in pages
                    sLocation [IN]  - location (path+name) of devspace
                    nType     [IN]  - device type of devspace
      prototypes:   4
      -------------------------------------------------------------------------
    */
    DBMCli_Devspace ( );

    DBMCli_Devspace ( const DBMCli_DevspaceClass nClass,
                      const SAPDB_Int            nNumber );

    DBMCli_Devspace ( const DBMCli_DevspaceClass nClass,
                      const SAPDB_Int            nNumber,
                      const SAPDB_Int4           nPages,
                      const DBMCli_String      & sLocation,
                      const DBMCli_DevspaceType  nType );

    DBMCli_Devspace ( const DBMCli_String & sName,
                      const DBMCli_String & sPages,
                      const DBMCli_String & sLocation,
                      const DBMCli_String & sType );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Devspace
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Devspace ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear devspace informations.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetProperties
      -------------------------------------------------------------------------
      description:  Set devspace informations.
      -------------------------------------------------------------------------
    */
    void SetProperties ( const DBMCli_DevspaceClass nClass = DBMCLI_DEVSPACECLASS_DATA,
                         const SAPDB_Int            nNumber = 0,
                         const SAPDB_Int4           nPages = 0,
                         const DBMCli_String      & sLocation = "",
                         const DBMCli_DevspaceType  nType = DBMCLI_DEVSPACETYPE_FILE,
                         const SAPDB_Bool           bMirrored = false,
                         const DBMCli_String      & sMirroredLocation = "",
                         const DBMCli_DevspaceType  nMirroredType = DBMCLI_DEVSPACETYPE_FILE);

    /*!
      -------------------------------------------------------------------------
      function:     SetMirroredProperties 
      -------------------------------------------------------------------------
      description:  Set mirrored devspace informations.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    void SetMirroredProperties ( const SAPDB_Bool      bMirrored,
                                 const DBMCli_String & sMirroredLocation,
                                 const DBMCli_String   sMirroredType );

    void SetMirroredProperties ( const SAPDB_Bool           bMirrored,
                                 const DBMCli_String      & sMirroredLocation,
                                 const DBMCli_DevspaceType  nMirroredType );

    /*!
      -------------------------------------------------------------------------
      function:     PrepareMirroredProperties 
      -------------------------------------------------------------------------
      description:  prepare mirror data
      -------------------------------------------------------------------------
    */
    void PrepareMirroredProperties (const DBMCli_String & sMirroredLocation,
                                    const DBMCli_String & sMirroredType );
    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Complete
      -------------------------------------------------------------------------
      return value: True, if devspace is complete.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Complete ( );

    /*!
      -------------------------------------------------------------------------
      function:     Put
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Put ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     PutPreparedMirror
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool PutPreparedMirror ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     DeleteMirror
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool DeleteMirror ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Reintegrate
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
     SAPDB_Bool Reintegrate ( Msg_List & oMsgList );
    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     Class
      -------------------------------------------------------------------------
    */
    DBMCli_DevspaceClass Class ( ) { return m_nClass; };

    /*!
      -------------------------------------------------------------------------
      function:     ClassName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & ClassName ( ) { return m_sClassName; };

    /*!
      -------------------------------------------------------------------------
      function:     Number
      -------------------------------------------------------------------------
    */
    SAPDB_Int Number ( ) { return m_nNumber; };

    /*!
      -------------------------------------------------------------------------
      function:     Pages
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 Pages ( ) { return m_nPages; };

    /*!
      -------------------------------------------------------------------------
      function:     Location
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Location ( ) { return m_sLocation; };

    /*!
      -------------------------------------------------------------------------
      function:     Type
      -------------------------------------------------------------------------
    */
    DBMCli_DevspaceType Type ( ) { return m_nType; };

    /*!
      -------------------------------------------------------------------------
      function:     TypeName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & TypeName ( ) { return m_sTypeName; };

    /*!
      -------------------------------------------------------------------------
      function:     Mirrored
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Mirrored ( ) { return m_bMirrored; };

    /*!
      -------------------------------------------------------------------------
      function:     MirroredLocation
      -------------------------------------------------------------------------
    */
    const DBMCli_String & MirroredLocation ( ) { return m_sMirroredLocation; };

    /*!
      -------------------------------------------------------------------------
      function:     MirroredType
      -------------------------------------------------------------------------
    */
    DBMCli_DevspaceType MirroredType ( ) { return m_nMirroredType; };

    /*!
      -------------------------------------------------------------------------
      function:     MirroredTypeName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & MirroredTypeName ( ) { return m_sMirroredTypeName; };

    /*!
      -------------------------------------------------------------------------
      function:     PreparedMirroredLocation
      -------------------------------------------------------------------------
    */
    const DBMCli_String & PreparedMirroredLocation ( ) { return m_sPreparedMirroredLocation; };

    /*!
      -------------------------------------------------------------------------
      function:     MirroredType
      -------------------------------------------------------------------------
    */
    DBMCli_DevspaceType PreparedMirroredType ( ) { return m_nPreparedMirroredType; };

    /*!
      -------------------------------------------------------------------------
      function:     PreparedMirroredTypeName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & PreparedMirroredTypeName ( ) { return m_sPreparedMirroredTypeName; };

    void SetLocation ( const DBMCli_String & sLocation ) { m_sLocation = sLocation; };
    void SetPages ( const SAPDB_Int4 nPages ) { m_nPages = nPages; };
    void SetDeviceType ( const DBMCli_DevspaceType nType ) { m_nType = nType; };
    void SetMirrored ( const SAPDB_Bool bMirrored ) { m_bMirrored = bMirrored; };
    void SetMirroredLocation ( const DBMCli_String & sMirroredLocation ) { m_sMirroredLocation = sMirroredLocation; };
    void SetMirroredDeviceType ( const DBMCli_DevspaceType nMirroredType ) { m_nMirroredType = nMirroredType; };

    /*!  
      EndChapter: Properties
    */

  private:
    void AssignClass ( const DBMCli_String & sClass );

    void AssignName ( const DBMCli_DevspaceClass nClass,
                      const SAPDB_Int            nNumber );

    void AssignClassName ( const DBMCli_DevspaceClass nClass );

    void AssignType ( const DBMCli_String & sType,
                      const SAPDB_Bool      bMirrored = false );

    void AssignTypeName ( const DBMCli_DevspaceType nType,
                          const SAPDB_Bool          bMirrored = false );

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String        m_sName;

    DBMCli_DevspaceClass m_nClass;
    DBMCli_String        m_sClassName;
    SAPDB_Int            m_nNumber;
    SAPDB_Int4           m_nPages;
    DBMCli_String        m_sLocation;
    DBMCli_DevspaceType  m_nType;
    DBMCli_String        m_sTypeName;

    SAPDB_Bool           m_bMirrored;
    DBMCli_String        m_sMirroredLocation;
    DBMCli_DevspaceType  m_nMirroredType;
    DBMCli_String        m_sMirroredTypeName;

    DBMCli_String        m_sPreparedMirroredLocation;
    DBMCli_DevspaceType  m_nPreparedMirroredType;
    DBMCli_String        m_sPreparedMirroredTypeName;

}; 

/*! EndClass: DBMCli_Devspace */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Devspaces
  -----------------------------------------------------------------------------
  description: Handle database devspaces access.
  -----------------------------------------------------------------------------
*/
class DBMCli_Devspaces 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Devspaces
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Devspaces ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Devspaces
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Devspaces ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the Devspaces object members.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the devspaces.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Init
      -------------------------------------------------------------------------
      description:  Init the devspaces.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Init ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Add
      -------------------------------------------------------------------------
      description:  Add a devspace.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Add ( const DBMCli_String        & sClass,
                     const DBMCli_String        & sLocation,
                     const DBMCli_String        & sType,
                     const DBMCli_String        & sSize,
                     const DBMCli_String        & sMirroredLocation,
                     const DBMCli_String        & sMirroredType,
                           Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Reintegrate
      -------------------------------------------------------------------------
      description:  Reintegrate a bad devspace.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Reintegrate ( const DBMCli_String        & sName,
                                   Msg_List & oMsgList );
    
    /*!
      -------------------------------------------------------------------------
      function:     NewDevspace
      -------------------------------------------------------------------------
      description:  A new devspace.
      arguments:    sClass [OUT] - class of devspace
      return value: Reference to a devspace object.
      -------------------------------------------------------------------------
    */
    DBMCli_Devspace & NewDevspace( const DBMCli_String & sClass );

    /*!
      -------------------------------------------------------------------------
      function:     InitDevspace
      -------------------------------------------------------------------------
      description:  Initialize a devspace.
      arguments:    sClass [OUT] - class of devspace
      -------------------------------------------------------------------------
    */
    void InitDevspace( DBMCli_Devspace & oDevspace );

    /*!
      -------------------------------------------------------------------------
      function:     DevspaceArray
      -------------------------------------------------------------------------
      description:  Get the array of devspaces.
      return value: Reference to an array of devspaces objects.
      -------------------------------------------------------------------------
    */
    DBMCli_DevspaceArray & DevspaceArray ( ) { return m_aDevspace; }; 

    /*!
      -------------------------------------------------------------------------
      function:     ClassFromClassName
      -------------------------------------------------------------------------
      description:  Get the class value from a string.
      return value: Class value.
      -------------------------------------------------------------------------
    */
    DBMCli_DevspaceClass ClassFromClassName ( const DBMCli_String & sClass );

    /*!
      -------------------------------------------------------------------------
      function:     ClassNameFromClass
      -------------------------------------------------------------------------
      description:  Get the class name from a class value.
      return value: Class value.
      -------------------------------------------------------------------------
    */
    DBMCli_String ClassNameFromClass ( const DBMCli_DevspaceClass nClass );

    /*!
      -------------------------------------------------------------------------
      function:     TypeFromTypeName
      -------------------------------------------------------------------------
      description:  Get the type value from a string.
      return value: Type value.
      -------------------------------------------------------------------------
    */
    DBMCli_DevspaceType TypeFromTypeName ( const DBMCli_String & sType );

    /*!
      -------------------------------------------------------------------------
      function:     TypeNameFromType
      -------------------------------------------------------------------------
      description:  Get the type name from a type value.
      return value: Type value.
      -------------------------------------------------------------------------
    */
    DBMCli_String TypeNameFromType ( const DBMCli_DevspaceType nType );

    /*!
      -------------------------------------------------------------------------
      function:     TypeCharFromType
      -------------------------------------------------------------------------
      description:  Get the class name from a class value.
      return value: Class value.
      -------------------------------------------------------------------------
    */
    DBMCli_String TypeCharFromType ( const DBMCli_DevspaceType nType );

    /*!
      -------------------------------------------------------------------------
      function:     IndexByName
      -------------------------------------------------------------------------
      description:  Get a index from a devspace by the name.
      return value: Index of devspace in array.
      -------------------------------------------------------------------------
    */
    SAPDB_Int IndexByName ( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     LogMode
      -------------------------------------------------------------------------
    */
    DBMCli_LogMode LogMode ( ) { return m_nLogMode; };

    /*!
      -------------------------------------------------------------------------
      function:     LogModeName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & LogModeName( ) { return m_sLogMode; };

    /*!
      -------------------------------------------------------------------------
      function:     MaxLogDevs
      -------------------------------------------------------------------------
    */
    SAPDB_Int MaxLogDevs ( ) { return m_nMaxLogDevs; };

    /*!
      -------------------------------------------------------------------------
      function:     UsedLogDevs
      -------------------------------------------------------------------------
    */
    SAPDB_Int UsedLogDevs ( ) { return m_nUsedLogDevs; };

    /*!
      -------------------------------------------------------------------------
      function:     MaxDataDevs
      -------------------------------------------------------------------------
    */
    SAPDB_Int MaxDataDevs ( ) { return m_nMaxDataDevs; };

    /*!
      -------------------------------------------------------------------------
      function:     UsedDataDevs
      -------------------------------------------------------------------------
    */
    SAPDB_Int UsedDataDevs ( ) { return m_nUsedDataDevs; };

    /*!
      -------------------------------------------------------------------------
      function:     MaxDataPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int MaxDataPages ( ) { return m_nMaxDataPages; };

    /*!
      -------------------------------------------------------------------------
      function:     Complete
      -------------------------------------------------------------------------
      description:  Check whether devspaces info are complete.
      return value: True, if devspaces are complete.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Complete ( );


  private:
    void AssignLogMode ( const DBMCli_String & sLogMode );

    DBMCli_String GetNextLocation( const DBMCli_String & sLocation );

    /*!
      -------------------------------------------------------------------------
      function:     DataPagesAfterRestart
      -------------------------------------------------------------------------
      description:  Calculate the datapages after restart for data devspaces.
      return value: Calculated datapages.
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 DataPagesAfterRestart ( );

    /*!
      -------------------------------------------------------------------------
      function:     CalcSysDevspacePages
      -------------------------------------------------------------------------
      description:  Calculate the size of the system devspace.
      return value: Calculated system devspace size.
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 CalcSysDevspacePages( );

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_Devspace m_oNewDevspace;

    DBMCli_DevspaceArray m_aDevspace;


    DBMCli_LogMode m_nLogMode;
    DBMCli_String  m_sLogMode;
    SAPDB_Int      m_nMaxLogDevs;
    SAPDB_Int      m_nUsedLogDevs;
    SAPDB_Int      m_nMaxDataDevs;
    SAPDB_Int      m_nUsedDataDevs;
    SAPDB_Int      m_nMaxDataPages;
}; 

/*! EndClass: DBMCli_Devspaces */

#endif // _DBMCLI_DEVSPACE_HPP_

