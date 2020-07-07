/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Media.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Media Class
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

#ifndef _DBMCLI_MEDIA_HPP_
#define _DBMCLI_MEDIA_HPP_

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
class DBMCli_Medium;
class DBMCli_MediumParallel;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum
{
  DBMCLI_MEDIUMTYPE_UNKNOWN  = 0,
  DBMCLI_MEDIUMTYPE_SINGLE   = 1,
  DBMCLI_MEDIUMTYPE_PARALLEL = 2
} DBMCli_MediumTypeEnum;

typedef enum
{
  DBMCLI_MEDIUMDEVICETYPE_UNKNOWN = 0,
  DBMCLI_MEDIUMDEVICETYPE_FILE    = 1,
  DBMCLI_MEDIUMDEVICETYPE_TAPE    = 2,
  DBMCLI_MEDIUMDEVICETYPE_PIPE    = 3
} DBMCli_MediumDeviceTypeEnum;

typedef DBMCli_Array< DBMCli_Medium, DBMCli_Medium > DBMCli_MediumArray;
typedef DBMCli_Array< DBMCli_MediumParallel, DBMCli_MediumParallel > DBMCli_MediumParallelArray;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_MediumType
  -----------------------------------------------------------------------------
  description: 
  -----------------------------------------------------------------------------
*/
class DBMCli_MediumType
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_MediumType
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   3
      -------------------------------------------------------------------------
    */
    DBMCli_MediumType ( );
    DBMCli_MediumType ( const DBMCli_String & sMediumType );
    DBMCli_MediumType ( const DBMCli_MediumTypeEnum nMediumType );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_MediumType
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_MediumType ( );

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    DBMCli_MediumTypeEnum Value ( ) { return m_nMediumType; };

    /*!
      -------------------------------------------------------------------------
      function:     SetValue
      -------------------------------------------------------------------------
    */
    void SetValue ( DBMCli_MediumTypeEnum nMediumType ) { m_nMediumType = nMediumType; };

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String Name ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetName
      -------------------------------------------------------------------------
    */
    void SetName ( const DBMCli_String & sName );

  private:

    DBMCli_MediumTypeEnum m_nMediumType;
};

/*! EndClass: DBMCli_MediumType */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_MediumDeviceType
  -----------------------------------------------------------------------------
  description: 
  -----------------------------------------------------------------------------
*/
class DBMCli_MediumDeviceType
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_MediumDeviceType
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   3
      -------------------------------------------------------------------------
    */
    DBMCli_MediumDeviceType ( );
    DBMCli_MediumDeviceType ( const DBMCli_String & sMediumDeviceType );
    DBMCli_MediumDeviceType ( const DBMCli_MediumDeviceTypeEnum nMediumDeviceType );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_MediumDeviceType
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_MediumDeviceType ( );

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    DBMCli_MediumDeviceTypeEnum Value ( ) { return m_nMediumDeviceType; };

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    DBMCli_String Name ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetName
      -------------------------------------------------------------------------
    */
    void SetName ( const DBMCli_String & sName );

  private:

    DBMCli_MediumDeviceTypeEnum m_nMediumDeviceType;
};

/*! EndClass: DBMCli_MediumDeviceType */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_MediumExternType
  -----------------------------------------------------------------------------
  description: 
  -----------------------------------------------------------------------------
*/
class DBMCli_MediumExternType
{
  public:

    typedef enum
    {
      ExtTypeUnknown = 0,
      ExtTypeNone    = 1,
      ExtTypeNsr     = 2,
      ExtTypeAdsm    = 3,
      ExtTypeBack    = 4,
      ExtTypeOmni    = 5
    } ExtType;

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_MediumExternType
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   3
      -------------------------------------------------------------------------
    */
    DBMCli_MediumExternType ( );
    DBMCli_MediumExternType ( const DBMCli_String & sMediumExternType );
    DBMCli_MediumExternType ( const ExtType nMediumExternType );

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    ExtType Value ( ) const { return m_nMediumExternType; };

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    DBMCli_String Name ( ) const;

  private:

    ExtType m_nMediumExternType;
};

/*! EndClass: DBMCli_MediumExternType */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Medium
  -----------------------------------------------------------------------------
  description: Handle a single medium.
  -----------------------------------------------------------------------------
*/
class DBMCli_Medium
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Medium
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_Medium ( );

    DBMCli_Medium ( const DBMCli_String & sGroup,
                    const DBMCli_String & sName,
                    const DBMCli_String & sLocation,
                    const DBMCli_String & sDeviceType,
                    const DBMCli_String & sBackupType,
                    const DBMCli_String & sPages,
                    const DBMCli_String & sBlockSize,
                    const DBMCli_String & sOverwrite,
                    const DBMCli_String & sAutoloader,
                    const DBMCli_String & sOSCommand,
                    const DBMCli_String & sExtern,
                    const DBMCli_String & sCreated,
                    const DBMCli_String & sModified,
                    const DBMCli_String & sOldDefinition);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Medium
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Medium ( );

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
    DBMCli_Database & GetDatabase ( ) const { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear medium informations.
      -------------------------------------------------------------------------
    */
    void Clear ( );

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
      function:     Put
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Put ( const DBMCli_String & sLocation,
                     const DBMCli_String & sDeviceType,
                     const DBMCli_String & sBackupType,
                     const DBMCli_String & sSize,
                     const DBMCli_String & sBlockSize,
                     const DBMCli_String & sOverwrite,
                     const DBMCli_String & sAutoloader,
                     const DBMCli_String & sOSCommand,
                     const DBMCli_String & sExtern,
                     Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Delete
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Delete ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     SetPropertiesByLine
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    void SetPropertiesByLine (DBMCli_ResultBuf & oLine );

    /*!
      -------------------------------------------------------------------------
      function:     SetProperties
      -------------------------------------------------------------------------
      arguments:    
      return value: 
      -------------------------------------------------------------------------
    */
    void SetProperties ( const DBMCli_String & sGroup,
                         const DBMCli_String & sName,
                         const DBMCli_String & sLocation = "",
                         const DBMCli_String & sDeviceType = "",
                         const DBMCli_String & sBackupType = "",
                         const DBMCli_String & sSize = "",
                         const DBMCli_String & sBlockSize = "",
                         const DBMCli_String & sOverwrite = "",
                         const DBMCli_String & sAutoloader = "",
                         const DBMCli_String & sOSCommand = "",
                         const DBMCli_String & sExtern = "");

    /*!
      -------------------------------------------------------------------------
      function:     ChangeLocation
      -------------------------------------------------------------------------
      arguments:    
      return value: 
      -------------------------------------------------------------------------
    */
    void ChangeLocation ( const DBMCli_String & sLocation ) {m_sLocation = sLocation;}

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     FullName
      -------------------------------------------------------------------------
    */
    DBMCli_String FullName ( );

    /*!
      -------------------------------------------------------------------------
      function:     Group
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Group ( ) const { return m_sGroup; };

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) const { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     Location
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Location ( ) const { return m_sLocation; };

    /*!
      -------------------------------------------------------------------------
      function:     DeviceType
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DeviceType ( ) const { return m_sDeviceType; };

    /*!
      -------------------------------------------------------------------------
      function:     BackupType
      -------------------------------------------------------------------------
    */
    const DBMCli_String & BackupType ( ) const { return m_sBackupType; };

    /*!
      -------------------------------------------------------------------------
      function:     Pages
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Pages ( ) const { return m_sPages; };

    /*!
      -------------------------------------------------------------------------
      function:     Blocksize
      -------------------------------------------------------------------------
    */
    const DBMCli_String & BlockSize ( ) const { return m_sBlockSize; };

    /*!
      -------------------------------------------------------------------------
      function:     Overwrite
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Overwrite ( ) const { return m_sOverwrite; };

    /*!
      -------------------------------------------------------------------------
      function:     Autoloader
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Autoloader ( ) const { return m_sAutoloader; };

    /*!
      -------------------------------------------------------------------------
      function:     OSCommand
      -------------------------------------------------------------------------
    */
    const DBMCli_String & OSCommand ( ) const { return m_sOSCommand; };

    /*!
      -------------------------------------------------------------------------
      function:     Extern
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Extern ( ) const { return m_sExtern; };

    /*!
      -------------------------------------------------------------------------
      function:     Created
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Created ( ) const { return m_sCreated; };

    /*!
      -------------------------------------------------------------------------
      function:     Modified
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Modified ( ) const { return m_sModified; };

    /*!
      -------------------------------------------------------------------------
      function:     OldDefinition
      -------------------------------------------------------------------------
    */
    const DBMCli_String & OldDefinition ( ) const { return m_sOldDefinition; };

    /*!  
      EndChapter: Properties
    */

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String m_sGroup;
    DBMCli_String m_sName;
    DBMCli_String m_sLocation;
    DBMCli_String m_sDeviceType;
    DBMCli_String m_sBackupType;
    DBMCli_String m_sPages;
    DBMCli_String m_sBlockSize;
    DBMCli_String m_sOverwrite;
    DBMCli_String m_sAutoloader;
    DBMCli_String m_sOSCommand;
    DBMCli_String m_sExtern;
    DBMCli_String m_sCreated;
    DBMCli_String m_sModified;
    DBMCli_String m_sOldDefinition;
}; 

/*! EndClass: DBMCli_Medium */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_MediumParallel
  -----------------------------------------------------------------------------
  description: Handle parallel medium.
  -----------------------------------------------------------------------------
*/
class DBMCli_MediumParallel
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_MediumParallel
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_MediumParallel ( );

    DBMCli_MediumParallel ( const DBMCli_String & sName,
                            const DBMCli_String & sDeviceType,
                            const DBMCli_String & sBackupType,
                            const DBMCli_String & sBlockSize,
                            const DBMCli_String & sOverwrite,
                            const DBMCli_String & sAutoloader,
                            const DBMCli_String & sExtern,
                            const DBMCli_String & sCreated,
                            const DBMCli_String & sModified,
                            const DBMCli_String & sOldDefinition);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Media
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_MediumParallel ( );

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
    DBMCli_Database & GetDatabase ( ) const { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the media object members.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetProperties
      -------------------------------------------------------------------------
      arguments:    
      -------------------------------------------------------------------------
    */
    void SetProperties ( const DBMCli_String & sName,
                         const DBMCli_String & sDeviceType = "",
                         const DBMCli_String & sBackupType = "",
                         const DBMCli_String & sBlockSize = "",
                         const DBMCli_String & sOverwrite = "",
                         const DBMCli_String & sAutoloader = "",
                         const DBMCli_String & sExtern = "" );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) const { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     DeviceType
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DeviceType ( ) const { return m_sDeviceType; };

    /*!
      -------------------------------------------------------------------------
      function:     BackupType
      -------------------------------------------------------------------------
    */
    const DBMCli_String & BackupType ( ) const { return m_sBackupType; };

    /*!
      -------------------------------------------------------------------------
      function:     BlockSize
      -------------------------------------------------------------------------
    */
    const DBMCli_String & BlockSize ( ) const { return m_sBlockSize; };

    /*!
      -------------------------------------------------------------------------
      function:     Overwrite
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Overwrite ( ) const { return m_sOverwrite; };

    /*!
      -------------------------------------------------------------------------
      function:     Autoloader
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Autoloader ( ) const { return m_sAutoloader; };

    /*!
      -------------------------------------------------------------------------
      function:     Extern
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Extern ( ) const { return m_sExtern; };

    /*!
      -------------------------------------------------------------------------
      function:     Created
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Created ( ) const { return m_sCreated; };

    /*!
      -------------------------------------------------------------------------
      function:     Modified
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Modified ( ) const { return m_sModified; };

    /*!
      -------------------------------------------------------------------------
      function:     OldDefinition
      -------------------------------------------------------------------------
    */
    const DBMCli_String & OldDefinition ( ) const { return m_sOldDefinition; };

    /*!
      -------------------------------------------------------------------------
      function:     MediaCount
      -------------------------------------------------------------------------
    */
    SAPDB_Int MediaCount ( );

    /*!  
      EndChapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     IndexByLocation
      -------------------------------------------------------------------------
      description:  Get a index from a medium of this group by the the location
      return value: Index of medium in array.
      -------------------------------------------------------------------------
    */
    SAPDB_Int IndexByLocation ( const DBMCli_String & sLocation );

    /*!
      -------------------------------------------------------------------------
      function:     MediumArray
      -------------------------------------------------------------------------
      description:  Get the array of media.
      return value: Reference to an array of medium objects.
      -------------------------------------------------------------------------
    */
    DBMCli_MediumArray & MediumArray ( ) { return m_aMedium; }; 
    const DBMCli_MediumArray & MediumArray ( ) const { return m_aMedium; }; 

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String m_sName;
    DBMCli_String m_sDeviceType;
    DBMCli_String m_sBackupType;
    DBMCli_String m_sBlockSize;
    DBMCli_String m_sOverwrite;
    DBMCli_String m_sAutoloader;
    DBMCli_String m_sExtern;
    DBMCli_String m_sCreated;
    DBMCli_String m_sModified;
    DBMCli_String m_sOldDefinition;

    DBMCli_MediumArray m_aMedium;
}; 

/*! EndClass: DBMCli_MediumParallel */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Media
  -----------------------------------------------------------------------------
  description: Handle media.
  -----------------------------------------------------------------------------
*/
class DBMCli_Media
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Media
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Media ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Media
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Media ( );

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
      description:  Clear the media object members.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the media.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     NewMedium
      -------------------------------------------------------------------------
      description:  A new medium.
      return value: Reference to a medium object.
      -------------------------------------------------------------------------
    */
    DBMCli_Medium & NewMedium( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     IndexByName
      -------------------------------------------------------------------------
      description:  Get a index from a medium by the name.
      return value: Index of medium in array.
      -------------------------------------------------------------------------
    */
    SAPDB_Int IndexByName ( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     NewMediumParallel
      -------------------------------------------------------------------------
      description:  A new parallel medium.
      return value: Reference to a medium object.
      -------------------------------------------------------------------------
    */
    DBMCli_MediumParallel & NewMediumParallel( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     IndexByNameParallel
      -------------------------------------------------------------------------
      description:  Get a index from a parallel medium by the name.
      return value: Index of parallel medium in array.
      -------------------------------------------------------------------------
    */
    SAPDB_Int IndexByNameParallel ( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     MediumParallelArray
      -------------------------------------------------------------------------
      description:  Get the array of media.
      return value: Reference to an array of MediumParallel objects.
      -------------------------------------------------------------------------
    */
    DBMCli_MediumParallelArray & MediumParallelArray ( ) { return m_aMediumParallel; }; 

    /*!
      -------------------------------------------------------------------------
      function:     MediumArray
      -------------------------------------------------------------------------
      description:  Get the array of media.
      return value: Reference to an array of medium objects.
      -------------------------------------------------------------------------
    */
    DBMCli_MediumArray & MediumArray ( ) { return m_aMedium; }; 


  private:
    DBMCli_Database * m_pDatabase;

    SAPDB_Int m_nMaxParallelDevices;

    DBMCli_Medium m_oNewMedium;
    DBMCli_MediumParallel m_oNewMediumParallel;

    DBMCli_MediumParallelArray m_aMediumParallel;
    DBMCli_MediumArray m_aMedium;
}; 

/*! EndClass: DBMCli_Media */

#endif // _DBMCLI_MEDIA_HPP_

