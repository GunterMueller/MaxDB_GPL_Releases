/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateFrame.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Frame page
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

#ifndef _DBMWEB_TEMPLATEFRAME_HPP_
#define _DBMWEB_TEMPLATEFRAME_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLFRAME_LOGON  = 0,
	DBMWEB_TEMPLFRAME_WORK   = 1,
	DBMWEB_TEMPLFRAME_WIZARD = 2
} DBMWeb_TemplFrame_Type;

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateFrame
  -----------------------------------------------------------------------------
  description:  An instance of this class represents the HTML-Frame-Page.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateFrame : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateFrame
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateFrame ( sapdbwa_WebAgent       & wa,
                           DBMWeb_TemplFrame_Type   nFrameType,
                           DBMCli_String          & sSessionID);

    DBMWeb_TemplateFrame ( sapdbwa_WebAgent       & wa,
                           DBMWeb_TemplFrame_Type   nFrameType,
                           DBMCli_String          & sSessionID,
                           DBMCli_String          & sName);

    DBMWeb_TemplateFrame ( sapdbwa_WebAgent       & wa,
                           DBMWeb_TemplFrame_Type   nFrameType,
                           DBMCli_String          & sSessionID,
                           DBMCli_String          & sAction,
                           DBMCli_String          & sServer,
                           DBMCli_String          & sDatabase,
                           DBMCli_String          & sUser,
                           DBMCli_String          & sPassword);
     
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateFrame
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateFrame ( );

    /*!
      -------------------------------------------------------------------------
      function:     setWorkURL
      -------------------------------------------------------------------------
      description:  Set the URL for the work area 

      arguments:    sWorkURL [IN] - the URL
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateFrame & setWorkURL(const DBMCli_String & sWorkURL );

  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    DBMWeb_TemplFrame_Type m_nFrameType;

    DBMCli_String m_sWorkURL;
    DBMCli_String m_sAction;
    DBMCli_String m_sServer;
    DBMCli_String m_sDatabase;
    DBMCli_String m_sUser;
    DBMCli_String m_sPassword;
    DBMCli_String m_sSessionID;
    DBMCli_String m_sName;
}; 

/*! EndClass: DBMWeb_TemplateFrame */

#endif // _DBMWEB_TEMPLATEFRAME_HPP_

