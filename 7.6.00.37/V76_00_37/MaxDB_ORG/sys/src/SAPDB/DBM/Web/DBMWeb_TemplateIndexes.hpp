/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateIndexes.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Indexes 
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

#ifndef _DBMWEB_TEMPLATEINDEXES_HPP_
#define _DBMWEB_TEMPLATEINDEXES_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLINDEXES_MODE_SELECT   = 0,
	DBMWEB_TEMPLINDEXES_MODE_INDEXES  = 1,
	DBMWEB_TEMPLINDEXES_MODE_RESULT   = 2
} DBMWeb_TemplIndexes_Mode;

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateIndexes
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                the index handling functions.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateIndexes : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateIndexes
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateIndexes ( sapdbwa_WebAgent & wa, 
                             DBMCli_Database * pDB,
                             DBMWeb_TemplIndexes_Mode nMode );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateIndexes
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateIndexes ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetRecoverMode
      -------------------------------------------------------------------------
    */
    void SetRecoverMode( SAPDB_Bool bRecoverMode ) { m_bRecoverMode = bRecoverMode; };

    /*!
      -------------------------------------------------------------------------
      function:     SetDetails
      -------------------------------------------------------------------------
    */
    void SetDetails( SAPDB_Bool bDetails ) { m_bDetails = bDetails; };

    /*!
      -------------------------------------------------------------------------
      function:     SetMaxRows
      -------------------------------------------------------------------------
    */
    void SetMaxRows( SAPDB_Int4 nMaxRows ) { m_nMaxRows = nMaxRows; };

  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    virtual bool                    askForContinue   ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    DBMCli_Database * m_pDB;

    DBMWeb_TemplIndexes_Mode m_nMode;

    SAPDB_Bool m_bRecoverMode;

    SAPDB_Bool m_bDetails;
    SAPDB_Int4 m_nMaxRows;
    SAPDB_Int4 m_nRow;

    DBMCli_ResultBuf m_oRow;

    DBMCli_String m_sOwner;
    DBMCli_String m_sTable;
    DBMCli_String m_sIndex;
    DBMCli_String m_sType;
    DBMCli_String m_sDate;
    DBMCli_String m_sUsed;
    DBMCli_String m_sDisabled;
    DBMCli_String m_sBad;
}; 

/*! EndClass: DBMWeb_TemplateIndexes */

#endif // _DBMWEB_TEMPLATEINDEXES_HPP_

