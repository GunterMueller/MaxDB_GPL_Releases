/*
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



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

/*!**********************************************************************

  module: Studio_Statements.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: SQLStudio

  description:  class for breaking a stream of statements 
				seperated by e.g. '//' or '--' into single statements

  see also:		/


************************************************************************/

#ifndef STUDIO_STATEMENTS_HPP
#define STUDIO_STATEMENTS_HPP


#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"


class Studio_Statements
{

public:

	   /*!-------------------------------------------------------------------------
		  function:     Studio_Statements
		  description:  Constructor

		  arguments:    [in] sStatements, statement bundle

		  return value: / 
	  --------------------------------------------------------------------------*/
		Studio_Statements(const Tools_DynamicUTF8String&  sStatements);

		/*!-------------------------------------------------------------------------
		  function:     StudioWeb_WebQuery
		  description:  Destructor
		  arguments:    /

		  return value: / 
      --------------------------------------------------------------------------*/
		~Studio_Statements(){;}


		typedef enum _statementType{

			statementUnknown = 0,
			statementSelect = 1,
			statementShow = 2,
			statementExplain = 3,
			statementCommand = 4,
			statementCallProcedure = 5
		}statementType;

		/*!-------------------------------------------------------------------------
		  function:     getCurrentStatement
		  description:  get the current statement of the statement stream
		  arguments:    /

		  return value: current statement
      --------------------------------------------------------------------------*/
		const Tools_DynamicUTF8String& getCurrentStatement() const {return m_sCurrentStatement;}

		/*!-------------------------------------------------------------------------
		  function:     getCurrentStatementType
		  description:  get the type of the current statement of the statement stream
		  arguments:    /

		  return value: statementType
      --------------------------------------------------------------------------*/
		 statementType getCurrentStatementType();

		/*!-------------------------------------------------------------------------
		  function:     endOfStatements
		  description:  are the more statements ?
		  arguments:    /

		  return value: true if there are no more statements
      --------------------------------------------------------------------------*/
		SAPDB_Bool endOfStatements() { return m_bEndOfStatements;}


		/*!-------------------------------------------------------------------------
			function:      nextStatement
			description:   get next statement
			arguments:     /
		
			return value:  return next statement of statement stream
			
		--------------------------------------------------------------------------*/
		const Tools_DynamicUTF8String &nextStatement();

private:
		/*!-------------------------------------------------------------------------
		    function:      isLineComment
		    description:   check if line is comment
		    arguments:     [in] sLine, line to be examined
		
		    return value:  true, if line is comment line
		
		--------------------------------------------------------------------------*/
		SAPDB_Bool isLineComment(const Tools_DynamicUTF8String& sLine);

		/*!-------------------------------------------------------------------------
		    function:      buildOneStatement
		    description:   get a statement out of a statement stream
		    arguments:     [in] sStatementTail, rest of the statement to handle
		
		    return value:  one valid statement
		
		--------------------------------------------------------------------------*/
		SAPDB_Bool buildOneStatement(Tools_DynamicUTF8String &sStatementTail,
									 Tools_DynamicUTF8String &sOneStatement);

		/*!-------------------------------------------------------------------------
		    function:      getLine
		    description:   get next line
		    arguments:     [in/out] sStatementTail, rest of statement, will be reduced
		                   [out] sLine, line to be examined
		
		    return value:  true, if success
		
		--------------------------------------------------------------------------*/
		SAPDB_Bool getLine(Tools_DynamicUTF8String &sStatementTail, 
						   Tools_DynamicUTF8String &sLine);			

private:

	SAPDB_Bool	m_bEndOfStatements;		

	Tools_DynamicUTF8String  m_sStatements;
    Tools_DynamicUTF8String  m_sCurrentStatement;

	Tools_DynamicUTF8String m_sComment;
	Tools_DynamicUTF8String m_sComment2;

	
	Tools_DynamicUTF8String m_sStatementTail;
	
	

};

#endif