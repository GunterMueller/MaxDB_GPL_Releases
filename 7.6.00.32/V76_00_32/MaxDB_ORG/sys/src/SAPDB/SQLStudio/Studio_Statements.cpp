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

  module: Studio_Statements.cpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: SQLStudio

  description:  class for breaking a stream of statements 
				seperated by e.g. '//' or '--' into single statements

  see also:		/


************************************************************************/


#include "SAPDB/SQLStudio/Studio_Statements.hpp"
#include "SAPDB/SQLStudio/Studio_Functions.hpp"

//Constructor
Studio_Statements::Studio_Statements(const Tools_DynamicUTF8String&  sStatements)
{
	m_sComment = "//";
	m_sComment2 = "--";
	m_sCurrentStatement.Erase();

	m_sStatements = sStatements;

	m_sStatementTail = m_sStatements;

	m_bEndOfStatements = SAPDB_FALSE;

	nextStatement();

}


Studio_Statements::statementType Studio_Statements::getCurrentStatementType()
{    
    
    Tools_DynamicUTF8String sCurrentStatement;

    //skip leading new lines, tabs, blanks    
    SimplifyWS (m_sCurrentStatement,sCurrentStatement);

	sCurrentStatement.ToUpper();

    if (sCurrentStatement.Compare(0,6,Tools_DynamicUTF8String("SELECT")) == 0){
    
        return statementSelect;
	}
    else if (sCurrentStatement.Compare(0,7,Tools_DynamicUTF8String("DECLARE")) == 0){
    
		//Compare can be used because cursoo is ascii7, BasisElementIndex = ElementCount = 7
        if(sCurrentStatement.Compare(7,6,Tools_DynamicUTF8String("CURSOR")) == 0) return statementSelect;        
	}
    else if (sCurrentStatement.Compare(0,7,Tools_DynamicUTF8String("EXPLAIN")) == 0){
    
        return statementExplain;
	}
    else if (sCurrentStatement.Compare(0,4,Tools_DynamicUTF8String("CALL")) == 0){
    
        return statementCallProcedure;
	}
    else if (sCurrentStatement.Compare(0,4,"SHOW") == 0){
    
        return statementShow;
	}	
	else if (sCurrentStatement.Size() == 0){
    
        return statementUnknown;
	}
    else
        return statementCommand;
    

	return statementUnknown;
	
}


SAPDB_Bool Studio_Statements::buildOneStatement(Tools_DynamicUTF8String &sStatementTail,
												Tools_DynamicUTF8String &sOneStatement) 
{    
    
	sOneStatement.Erase();	

    if (sStatementTail.Size() < 1) return SAPDB_FALSE;

	Tools_DynamicUTF8String sLine;	
	    
    //find first not comment line
    getLine (sStatementTail, sLine);

    while (isLineComment(sLine)){
    
        if (getLine(sStatementTail, sLine) == SAPDB_FALSE) break;
    }
    
    sOneStatement.Append(sLine);
    
    if (sStatementTail.Size() > 0)
	{    
		//find next comment line
		getLine (sStatementTail, sLine);
		while (!isLineComment(sLine)){
     
			 sOneStatement.Append(sLine);
			 if (getLine(sStatementTail, sLine) == SAPDB_FALSE) break;
		}
    
	}

    //if statement is just \n \r \t or spaces make a recursive call
	Tools_DynamicUTF8String sCleanStatement;
    SimplifyWS (sOneStatement,sCleanStatement);
	if (sCleanStatement.Size() < 1){

		Tools_DynamicUTF8String sOneStatement2;        
		buildOneStatement(sStatementTail,sOneStatement2);												 
		sOneStatement = sOneStatement2;
	}        
	
	return SAPDB_TRUE;  
    
}


SAPDB_Bool Studio_Statements::isLineComment(const Tools_DynamicUTF8String& sLine) 
{                            
    
    Tools_DynamicUTF8String sCleanLine;
	SimplifyWS (sLine,sCleanLine);
    
    if (sCleanLine.SubStr(0,2).Compare(m_sComment) == 0)  return SAPDB_TRUE;    
	if (sCleanLine.SubStr(0,2).Compare(m_sComment2) == 0)  return SAPDB_TRUE;    

	
    return SAPDB_FALSE;    
	

}
SAPDB_Bool Studio_Statements::getLine(Tools_DynamicUTF8String &sStatementTail, 
									  Tools_DynamicUTF8String &sLine)
{   
    
    //init
    sLine.Erase();
    
    if (sStatementTail.Size() < 1) return SAPDB_FALSE;

	Tools_DynamicUTF8String::BasisElementIndex Pos;
	Tools_DynamicUTF8String strLf = "\n";
	Tools_DynamicUTF8String strCr = "\r";
	Tools_DynamicUTF8String strCrLf = "\r\n";

	Pos = sStatementTail.Find(strCrLf);
	if (Pos != Tools_DynamicUTF8String::NPos) {

		sLine = Tools_DynamicUTF8String(sStatementTail.Begin(),sStatementTail.GetIteratorAtBasis(Pos));
		//append a space because \n ... could be a seperator in a sql statement
		sLine.Append(" ");
		sStatementTail = sStatementTail.SubStr(Pos+2);
	}
	else{
		Pos = sStatementTail.Find(strCr);
		if (Pos != Tools_DynamicUTF8String::NPos) {
			
			sLine = Tools_DynamicUTF8String(sStatementTail.Begin(),sStatementTail.GetIteratorAtBasis(Pos));
			//append a space because \n ... could be a seperator in a sql statement
			sLine.Append(" ");
			sStatementTail = sStatementTail.SubStr(Pos+1);
		}
		else{
			Pos = sStatementTail.Find(strLf);
			if (Pos != Tools_DynamicUTF8String::NPos) {

				sLine = Tools_DynamicUTF8String(sStatementTail.Begin(),sStatementTail.GetIteratorAtBasis(Pos));
				//append a space because \n ... could be a seperator in a sql statement
				sLine.Append(" ");
				sStatementTail = sStatementTail.SubStr(Pos+1);
			}
			else{
				sLine = sStatementTail;
				sStatementTail.Erase();
			}
		}
	}
       
    return SAPDB_TRUE;   
}


const Tools_DynamicUTF8String& Studio_Statements::nextStatement()  
{    
    //initialization
    m_bEndOfStatements = SAPDB_TRUE;
    
    buildOneStatement(m_sStatementTail,m_sCurrentStatement);
	if (m_sCurrentStatement.Size() < 1)

        m_bEndOfStatements = SAPDB_TRUE;
	else
        m_bEndOfStatements = SAPDB_FALSE;
    
    return m_sCurrentStatement;
}









