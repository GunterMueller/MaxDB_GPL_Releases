/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateBackup.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Backup
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

#ifndef _DBMWEB_TEMPLATEBACKUP_HPP_
#define _DBMWEB_TEMPLATEBACKUP_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Backup.hpp"
#include "DBM/Cli/DBMCli_Media.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLBACKUP_MODE_MEDIA      = 0,
	DBMWEB_TEMPLBACKUP_MODE_AUTOLOGOFF = 1,
	DBMWEB_TEMPLBACKUP_MODE_SELECT     = 2,
	DBMWEB_TEMPLBACKUP_MODE_STATE      = 3,
	DBMWEB_TEMPLBACKUP_MODE_REPLACE    = 4,
	DBMWEB_TEMPLBACKUP_MODE_READY      = 5,
	DBMWEB_TEMPLBACKUP_MODE_SHOWSTATE  = 6
} DBMWeb_TemplBackup_Mode;

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateBackup
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                database Backup.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateBackup : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateBackup
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateBackup ( sapdbwa_WebAgent & wa,
                            DBMCli_Backup & oBackup,
                            DBMCli_Media & oMedia,
                            DBMWeb_TemplBackup_Mode nMode,
                            DBMCli_BackupType & oBackupType,
                            DBMCli_MediumType & oMediumType,
                            const DBMCli_String & sMediumName );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateBackup
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateBackup ( );

  private:
    virtual bool initTemplate( );
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    virtual bool                    askForContinue   ( const Tools_DynamicUTF8String & szName );

  private:
    SAPDB_Int2 askForWriteCountMedia      ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountAutologOff ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountSelect     ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountReplace    ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountReady      ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountState      ( const Tools_DynamicUTF8String & szName );

    void askForValueMedia      ( const Tools_DynamicUTF8String & szName, DBMCli_String & sValue );
    void askForValueSelect     ( const Tools_DynamicUTF8String & szName, DBMCli_String & sValue );
    void askForValueReplace    ( const Tools_DynamicUTF8String & szName, DBMCli_String & sValue );
    void askForValueReady      ( const Tools_DynamicUTF8String & szName, DBMCli_String & sValue );
    void askForValueState      ( const Tools_DynamicUTF8String & szName, DBMCli_String & sValue );

    virtual bool askForContinueMedia ( const Tools_DynamicUTF8String & szName );

    bool FindNextMedium ( );

  private:
    
    DBMCli_Backup           & m_oBackup;
    DBMCli_Media            & m_oMedia;
    DBMWeb_TemplBackup_Mode   m_nMode;
    DBMCli_BackupType       & m_oBackupType;
    DBMCli_MediumType       & m_oMediumType;
    DBMCli_String             m_sMediumName;
    SAPDB_Int2                m_nMediumIndex;

    SAPDB_Int2 m_nMedium;
}; 

/*! EndClass: DBMWeb_TemplateBackup */

#endif // _DBMWEB_TEMPLATEBACKUP_HPP_

