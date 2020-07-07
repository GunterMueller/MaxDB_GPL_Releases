/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateRecovery.hpp
  -----------------------------------------------------------------------------
  responsible:  
  special area: Database Manager WebAgent Extension
  description:  HTML Template for Recovery
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

#ifndef _DBMWEB_TEMPLATERECOVERY_HPP_
#define _DBMWEB_TEMPLATERECOVERY_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "DBM/Cli/DBMCli_Database.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateRecovery
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                database Recovery.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateRecovery : public Tools_Template 
{
  public:

    typedef enum  
    { 
	    MODE_SELRECTYPE,
	    MODE_SELDATASAVE,
	    MODE_SELMEDIUM,
      MODE_COMMITLIST,
      MODE_RECSTATE
    } Mode;

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateRecovery
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateRecovery ( sapdbwa_WebAgent & wa,
                              Mode               nMode,
                              const char       * pAction, 
                              DBMCli_Database  * pDatabase );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateRecovery
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateRecovery ( );

     /*!
      -------------------------------------------------------------------------
      function:     SetRecInstMode
      -------------------------------------------------------------------------
      description:  sets the recovery install mode (use db_activate ?)
      arguments:    pRecInstMode [IN] - the recovery inst mode
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateRecovery & SetRecMode ( const char * pRecMode) {m_sRecMode = pRecMode;
                                                                   return * this;}

    /*!
      -------------------------------------------------------------------------
      function:     SetRecType
      -------------------------------------------------------------------------
      description:  sets the recovery type
      arguments:    pRecType [IN] - the recovery type
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateRecovery & SetRecType ( const char * pRecType) {m_sRecType = pRecType;
                                                                   return * this;}
    /*!
      -------------------------------------------------------------------------
      function:     SetMedType
      -------------------------------------------------------------------------
      description:  sets the media type
      arguments:    pMedType [IN] - the media type type
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateRecovery & SetMedType ( const char * pMedType) {m_sMedType = pMedType;
                                                                   return * this;}

    /*!
      -------------------------------------------------------------------------
      function:     SetCheck
      -------------------------------------------------------------------------
      description:  sets the check flag (recover_check with service kernel)
      arguments:    pCheck [IN] - the flag
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateRecovery & SetCheck ( const char * pCheck) {m_sCheck = pCheck;
                                                               return * this;}

    /*!
      -------------------------------------------------------------------------
      function:     SetUntil
      -------------------------------------------------------------------------
      description:  sets the  until value
      arguments:    oUntil [IN] - the until value
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateRecovery & SetUntil ( const DBMCli_DateTime & oUntil){m_oUntil = oUntil;
                                                                         return * this;}
  private:
    virtual bool initTemplate                     ( );

    virtual SAPDB_Int2 askForWriteCount           ( const Tools_DynamicUTF8String & szName );

    SAPDB_Int2 askForWriteCountSelectType         ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountDataSaveList       ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountMediaList          ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountRecoveryList       ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountRecoverState       ( const Tools_DynamicUTF8String & szName );

    virtual Tools_DynamicUTF8String askForValue     ( const Tools_DynamicUTF8String & szName );

    Tools_DynamicUTF8String askForValueSelectType   ( const Tools_DynamicUTF8String & szName );
    Tools_DynamicUTF8String askForValueDataSaveList ( const Tools_DynamicUTF8String & szName );
    Tools_DynamicUTF8String askForValueMediaList    ( const Tools_DynamicUTF8String & szName );
    Tools_DynamicUTF8String askForValueRecoveryList ( const Tools_DynamicUTF8String & szName );
    Tools_DynamicUTF8String askForValueRecoverState ( const Tools_DynamicUTF8String & szName );

    virtual bool askForContinue                   ( const Tools_DynamicUTF8String & szName );

  private:
    DBMCli_Database         * m_pDatabase;
    Tools_DynamicUTF8String   m_sAction;
    Mode                      m_nMode;
    SAPDB_Int                 m_nCurrentItem;
    Tools_DynamicUTF8String   m_sRecMode;
    Tools_DynamicUTF8String   m_sRecType;
    Tools_DynamicUTF8String   m_sMedType;
    Tools_DynamicUTF8String   m_sCheck;
    DBMCli_DateTime           m_oUntil;

}; 

/*! EndClass: DBMWeb_TemplateRecovery */

#endif // _DBMWEB_TEMPLATERECOVERY_HPP_

