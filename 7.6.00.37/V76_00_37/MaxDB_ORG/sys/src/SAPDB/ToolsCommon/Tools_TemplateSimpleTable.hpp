/*! 
  -----------------------------------------------------------------------------
 
  module: Tools_TemplateSimpleTable.hpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: universal stuff for web access
   
  description:  a class for creating simple Tables in SAP MiniApps style

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

#ifndef _Tools_TemplateSimpleTable_HPP_
#define _Tools_TemplateSimpleTable_HPP_

#include "ToolsCommon/Tools_Template.hpp"
#include "ToolsCommon/Tools_Array.hpp"

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  Class:        Tools_TemplateSimpleTable
  -----------------------------------------------------------------------------

  description:  Tools 

                An instance of this class represents a HTML-Page with a Table
  -----------------------------------------------------------------------------
 */
class Tools_TemplateSimpleTable : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     Tools_TemplateSimpleTable
      -------------------------------------------------------------------------
      description:  Constructor

      arguments:    wa [IN]  - webagent instance

      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable  ( sapdbwa_WebAgent       &   wa );

 
    /*!
      -------------------------------------------------------------------------
      function:     ~Tools_TemplateSimpleTable
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~Tools_TemplateSimpleTable ( );

     /*!
      -------------------------------------------------------------------------
      function:     enableLogo
      -------------------------------------------------------------------------
      description:  enable the Logo

      arguments:    bEnable   [IN] - enable or disable the logo (default: disabled)

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable & enableLogo ( bool bEnable = true )
        {
          m_bLogo = bEnable;
          return *this;
        } 

     /*!
      -------------------------------------------------------------------------
      function:     enableGroupbox
      -------------------------------------------------------------------------
      description:  enable the Groupbox style

      arguments:    bEnable   [IN] - enable or disable the style (default: disabled)

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable & enableGroupbox ( bool bEnable = true )
        {
          m_bGroup = bEnable;
          return *this;
        } 

    /*!
      -------------------------------------------------------------------------
      function:     setHeadLine
      -------------------------------------------------------------------------
      description:  sets the headline of the table

      arguments:    szHeadLine   [IN] - the headline

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable & setHeadLine      ( const SAPDB_UTF8 * szHeadLine   );

    /*!
      -------------------------------------------------------------------------
      function:     setTableTitle
      -------------------------------------------------------------------------
      description:  sets the title of the table

      arguments:    szTableTitle   [IN] - the title

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable &  setTableTitle    ( const SAPDB_UTF8 * szTableTitle );

    /*!
      -------------------------------------------------------------------------
      function:     setColumnTitle
      -------------------------------------------------------------------------
      description:  sets the title of a column

      arguments:    szColumnTitle   [IN] - the title
                    nColumn         [IN] - no of column

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable &  setColumnTitle   ( const SAPDB_UTF8 * szColumnTitle,    
                                                     const  int         nColumn     );

    /*!
      -------------------------------------------------------------------------
      function:     setCell
      -------------------------------------------------------------------------
      description:  sets a cell of the table

      arguments:    szCell     [IN] - the content for the cell
                    nRow       [IN] - no of row
                    nColumn    [IN] - no of column

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable &  setCell          ( const SAPDB_UTF8 * szCell, 
                                                     const int          nRow, 
                                                     const int          nColumn      );

    /*!
      -------------------------------------------------------------------------
      function:     setColumnSummary
      -------------------------------------------------------------------------
      description:  sets the summary of a column

      arguments:    szColumnSummary [IN] - the summary
                    nColumn         [IN] - no of column

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable &  setColumnSummary ( const SAPDB_UTF8 * szColumnSummary,  
                                                     const int          nColumn      );

     /*!
      -------------------------------------------------------------------------
      function:     addButton
      -------------------------------------------------------------------------
      description:  adds a Button

      arguments:    szName    [IN] - button text
                    szLink    [IN] - button link
          szTarget  [IN] - button target
                    bDisabled [IN] - button state

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     Tools_TemplateSimpleTable &  addButton   ( const SAPDB_UTF8 * szName,  
                                                const SAPDB_UTF8 * szLink,
                                                const SAPDB_UTF8 * szTarget,
                                                const bool         bDisabled = false);

  private:
     virtual SAPDB_Int2 askForWriteCount  ( const Tools_DynamicUTF8String & szName );

     virtual Tools_DynamicUTF8String askForValue ( const Tools_DynamicUTF8String & szName );


     SAPDB_Int2        m_nMaxRow;
     SAPDB_Int2        m_nMaxCol;
     bool              m_bTitleRow;
     bool              m_bSummaryRow;
     SAPDB_Int2        m_nCurrentRow;
     SAPDB_Int2        m_nCurrentCol;
     SAPDB_Int2        m_nMaxButton;
     SAPDB_Int2        m_nCurrentButton;
     bool              m_bLogo;
     bool              m_bGroup;

     Tools_DynamicUTF8String   m_strHeader;
     Tools_DynamicUTF8String   m_strTableTitle;

     typedef Tools_Array<Tools_DynamicUTF8String> StringMap;
     typedef Tools_Array<StringMap>       RowMap;

     StringMap  m_ColumnTitleMap;
     StringMap  m_ColumnSummaryMap;
     RowMap     m_RowMap;
     StringMap  m_ButtonMap;
}; 

/*! EndClass: Tools_TemplateSimpleTable */

#endif // _Tools_TemplateSimpleTable_HPP_

