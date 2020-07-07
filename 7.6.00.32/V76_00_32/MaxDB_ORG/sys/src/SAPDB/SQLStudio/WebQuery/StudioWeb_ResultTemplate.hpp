/*!**********************************************************************

  module: StudioWeb_ResultTemplate.hpp

  -----------------------------------------------------------------------


  responsible:  WolfgangA


  special area: WebQuery

  description:  result template class for WebQuery

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG




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




************************************************************************/

#ifndef STUDIOWEB_RESULTTEMPLATE_HPP
#define STUDIOWEB_RESULTTEMPLATE_HPP

#include "ToolsCommon/Tools_Array.hpp"
#include "ToolsCommon/Tools_Template.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  Class:        StudioWeb_ResultTemplate
  -----------------------------------------------------------------------------

  description:  SAPDBWeb 

                An instance of this class represents a HTML-Page with a Table
  -----------------------------------------------------------------------------
 */
class StudioWeb_ResultTemplate : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     StudioWeb_ResultTemplate
      
      description:  Constructor

      arguments:    wa [in] ,  webagent instance

      -------------------------------------------------------------------------
     */
     StudioWeb_ResultTemplate(sapdbwa_WebAgent &wa);

 
    /*!-------------------------------------------------------------------------
      function:     ~StudioWeb_ResultTemplate
      
      description:  Destructor
      -------------------------------------------------------------------------*/
     ~StudioWeb_ResultTemplate();

     /*!-------------------------------------------------------------------------
      function:     enableLogo
      
      description:  enable the Logo

      arguments:    bEnable   [in] ,  enable or disable the logo(default: disabled)

      return value: a reference to this object
      -------------------------------------------------------------------------*/
     StudioWeb_ResultTemplate & enableLogo(SAPDB_Bool bEnable = true)
		{
          m_bLogo = bEnable;
          return *this;
        } 

     

    /*!-------------------------------------------------------------------------
      function:     setHeadLine
      
      description:  sets the headline of the table

      arguments:    szHeadLine   [in] ,  the headline

      return value: a reference to the object
      -------------------------------------------------------------------------*/
     StudioWeb_ResultTemplate & setHeadLine(const Tools_DynamicUTF8String& szHeadLine  );

    /*!-------------------------------------------------------------------------
      function:     setTableTitle
      
      description:  sets the title of the table

      arguments:    szTableTitle   [in] ,  the title

      return value: a reference to the object
      -------------------------------------------------------------------------*/
     StudioWeb_ResultTemplate &  setTableTitle(const Tools_DynamicUTF8String& szTableTitle);

    /*!-------------------------------------------------------------------------
      function:     setColumnTitle

      description:  sets the title of a column

      arguments:    szColumnTitle   [in] ,  the title
                    nColumn         [in] ,  no of column

      return value: a reference to the object
      -------------------------------------------------------------------------*/
     StudioWeb_ResultTemplate &  setColumnTitle(const Tools_DynamicUTF8String& szColumnTitle,    
													const SAPDB_UInt2         nColumn    );

    /*!
      -------------------------------------------------------------------------
      function:     setCell
      
      description:  sets a cell of the table

      arguments:    szCell     [in] ,  the content for the cell
                    nRow       [in] ,  no of row
                    nColumn    [in] ,  no of column

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     StudioWeb_ResultTemplate &setCell    (const Tools_DynamicUTF8String& szCell, 
                                        const SAPDB_UInt2          nRow, 
                                        const SAPDB_UInt2          nColumn     );

    /*!
      -------------------------------------------------------------------------
      function:     setColumnSummary
      
      description:  sets the summary of a column

      arguments:    [in] szColumnSummary  , the summary
                    [in] nColumn          , no of column

      return value: a reference to the object
      -------------------------------------------------------------------------
     */
     StudioWeb_ResultTemplate &setColumnSummary(const Tools_DynamicUTF8String &szColumnSummary,  
												const SAPDB_UInt2            nColumn);
	 
     /*!-------------------------------------------------------------------------
      function:     addButton
      
      description:  adds a Button

      arguments:    szName    [in] ,button text
                    szLink    [in] ,  button link
					szTarget  [in] ,  button link
                    bDisabled [in] ,  button state

      return value: a reference to the object
      -------------------------------------------------------------------------     */
     StudioWeb_ResultTemplate &addButton(const Tools_DynamicUTF8String& szName,  
										 const Tools_DynamicUTF8String& szLink,
										 const Tools_DynamicUTF8String& szTarget,
										 const SAPDB_Bool         bDisabled = SAPDB_FALSE);

	 /*!-------------------------------------------------------------------------
		function:     setSelectedResult
    
		description:  set the select result no of dropdown list
    -------------------------------------------------------------------------*/
	 void setSelectedResult(SAPDB_Int2 nSelectedResult){ m_nSelectedResult = nSelectedResult;}

	 /*!-------------------------------------------------------------------------
		function:     resetStartValues
    
		description:  function to reset start values of the template
    -------------------------------------------------------------------------*/
	void resetStartValues ();

	 /*!-------------------------------------------------------------------------
		function:     resetTemplate
    
		description:  function to reset the template
    -------------------------------------------------------------------------*/
	void resetTemplate ();
	
	/*!-------------------------------------------------------------------------
		function:     setStartRow
    
		description:  function to set start row of template
    -------------------------------------------------------------------------*/
	void setStartRow (SAPDB_Int2 nCurrentRow){m_nCurrentRow  = nCurrentRow;}
	
	/*!-------------------------------------------------------------------------
		function:     setResults
    
		description:  function to set start row of template
    -------------------------------------------------------------------------*/
	StudioWeb_ResultTemplate &setResults(const Tools_DynamicUTF8String &sResult,
										 const SAPDB_UInt2          nRowNo);

	/*!-------------------------------------------------------------------------
		function:     setZoomText
  
		description:  function to set the zoom text
    -------------------------------------------------------------------------*/
	void setZoomText(const Tools_DynamicUTF8String &sZoomText){m_sZoomText = sZoomText;}

  private:
     virtual SAPDB_Int2 askForWriteCount(const Tools_DynamicUTF8String &sName);

     virtual Tools_DynamicUTF8String askForValue(const Tools_DynamicUTF8String &sName);
	 

     SAPDB_Int2        m_nMaxRow;
     SAPDB_Int2        m_nMaxCol;
     SAPDB_Bool        m_bTitleRow;
     SAPDB_Bool        m_bSummaryRow;
     SAPDB_Int2        m_nCurrentRow;
     SAPDB_Int2        m_nCurrentCol;
     SAPDB_Int2        m_nMaxButton;
     SAPDB_Int2        m_nCurrentButton;
     SAPDB_Bool        m_bLogo;

	 SAPDB_Int2		   m_nSelectedResult;
	 SAPDB_Int2		   m_nCurrentResultCount;
	 SAPDB_Int2		   m_nMaxResultCount;
     
     Tools_DynamicUTF8String   m_strHeader;
     Tools_DynamicUTF8String   m_strTableTitle;

	 Tools_DynamicUTF8String   m_sZoomText;

	 typedef Tools_Array<Tools_DynamicUTF8String> StringMap;
     typedef Tools_Array<StringMap>       RowMap;

	 StringMap  m_Results;
     StringMap  m_ColumnTitleMap;
     StringMap  m_ColumnSummaryMap;
     RowMap     m_RowMap;
     StringMap  m_ButtonMap;
}; 

/*! EndClass: StudioWeb_ResultTemplate */

#endif // _StudioWeb_ResultTemplate_HPP_

