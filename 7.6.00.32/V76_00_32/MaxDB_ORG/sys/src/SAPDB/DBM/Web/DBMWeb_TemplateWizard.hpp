/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateWizard.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Wizard
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

#ifndef _DBMWEB_TEMPLATEWIZARD_HPP_
#define _DBMWEB_TEMPLATEWIZARD_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "DBM/Cli/DBMCli_Wizard.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DBMWEB_TEMPLWIZARD_STATE_DEFAULT  0
#define DBMWEB_TEMPLWIZARD_STATE_WORK     1
#define DBMWEB_TEMPLWIZARD_STATE_READY    2
#define DBMWEB_TEMPLWIZARD_STATE_ERROR    3

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateWizard
  -----------------------------------------------------------------------------
  description:  Wizard
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateWizard : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateWizard
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateWizard ( sapdbwa_WebAgent & wa,
                            DBMCli_Wizard * pWizard,
                            DBMCli_Database * pDatabase,
                            SAPDB_Int nStep,
                            SAPDB_Int nState = DBMWEB_TEMPLWIZARD_STATE_WORK,
                            const DBMCli_String & sMessage = "");
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateWizard
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateWizard ( );

    void SetMediumType( DBMCli_MediumTypeEnum nMediumType ) { m_nMediumType = nMediumType; };
    void SetParamGroup( DBMCli_ParameterGroup nParamGroup ) { m_nParamGroup = nParamGroup; };
    void SetDevspaceClass( DBMCli_DevspaceClass nDevspaceClass ) { m_nDevspaceClass = nDevspaceClass; };
    void SetDevspaceMirrored( SAPDB_Bool bDevspaceMirrored ) { m_bDevspaceMirrored = bDevspaceMirrored; };
    void SetLose( SAPDB_Bool bLose ) { m_bLose = bLose; };
    void SetReinstall( SAPDB_Bool bReinstall ) { m_bReinstall = bReinstall; };
 
  private:
    virtual SAPDB_Int2 askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue ( const Tools_DynamicUTF8String & szName );
    virtual bool askForContinue ( const Tools_DynamicUTF8String & szName );

    SAPDB_Int2 askForWriteCountStep1 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep2 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep3 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep4 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep5 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep6 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep7 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep8 ( const Tools_DynamicUTF8String & szName );
    SAPDB_Int2 askForWriteCountStep9 ( const Tools_DynamicUTF8String & szName );

    void askForValueStep1 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep2 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep3 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep4 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep5 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep6 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep7 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep8 ( const Tools_DynamicUTF8String & szName );
    void askForValueStep9 ( const Tools_DynamicUTF8String & szName );

    bool askForContinueParams ( const Tools_DynamicUTF8String & szName );
    bool FindNextParam ( );

    bool askForContinueDevspaces ( const Tools_DynamicUTF8String & szName );
    bool FindNextDevspace ( );
    bool FindSystemDevspace ( );
    
  private:
    DBMCli_Wizard * m_Wizard;
    DBMCli_Database * m_Database;

    SAPDB_Int m_nStep;
    SAPDB_Int m_nState;
    SAPDB_Int m_nItem;
    DBMCli_String m_sValue;
    DBMCli_String m_sMessage;

    DBMCli_MediumTypeEnum m_nMediumType;
    DBMCli_ParameterGroup m_nParamGroup; 
    
    DBMCli_DevspaceClass m_nDevspaceClass;
    SAPDB_Bool m_bDevspaceMirrored;

    SAPDB_Bool m_bLose;
    SAPDB_Bool m_bReinstall;
}; 

/*! EndClass: DBMWeb_TemplateWizard */

#endif // _DBMWEB_TEMPLATEWIZARD_HPP_

