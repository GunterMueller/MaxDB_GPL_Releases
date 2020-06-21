/*!**************************************************************************

module      : IFTools_sqlcliInput.h

-------------------------------------------------------------------------

responsible : D031096

special area:
description : Interface Runtime 8.0

description:  Provides a class that handles the input for command line tool sqlcli

last changed: 2003-01-30
see also    :

-------------------------------------------------------------------------

copyright:    Copyright (c) 2003-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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




*****************************************************************************/
#ifndef IFTOOLS_SQLCLIINPUT_H
#define IFTOOLS_SQLCLIINPUT_H

#include "SAPDB/Interfaces/tools/IFTools_sqlcliProperties.h"

#define  MAX_CMD_KEYWORD_LEN 20

/**
* @ingroup sqlcli
* @brief Handles input for command line tool sqlcli. 
*/

class IFTools_sqlcliInput {
public:
	inline ~IFTools_sqlcliInput        (){}

	enum sqlcliInput_cmdtype{    
		cmdtype_stmt       =  1,
		cmdtype_exit       =  2,
		cmdtype_sqlmode    =  3,
		cmdtype_status     =  4,
		cmdtype_showUsers  =  5, 
		cmdtype_showTables =  6,
		cmdtype_showColumns=  7,
		cmdtype_showProcs  =  8,
		cmdtype_showViews  =  9,
		cmdtype_outfile    = 10,
		cmdtype_autocommit = 11,
		cmdtype_connect    = 12,
		cmdtype_help       = 20,
		cmdtype_shellcmd   = 21,
		cmdtype_edit       = 22,
		cmdtype_input      = 23,
		cmdtype_print      = 24,
		cmdtype_reset      = 25,  
		cmdtype_alignment  = 26,  
		cmdtype_fieldsep   = 27,
		cmdtype_disconnect = 28,  
       	cmdtype_showIndices= 29,
       	cmdtype_usePreparedStmt= 30,
       	cmdtype_showSchemas= 31,
       	cmdtype_pager      = 32,
       	cmdtype_multiline  = 33,
		cmdtype_eof        = 98,  
		cmdtype_unknown    = 99
	};

	enum sqlcliInput_autocommittype{    
		autocommit_on       =  1,
		autocommit_off      =  2
	};

	enum sqlcliInput_connecttype{    
		connect_dbname   =  1,
		connect_dbhost   =  2,
		connect_user     =  3,
		connect_xuser    =  4
	};

	typedef struct {
		char rawString [MAX_CMD_KEYWORD_LEN];  
		SQLDBC_Int4 length;
	}cmdKeyword ;

	typedef struct {
		SQLDBC_Int4 entry;
		cmdKeyword keyword [3];  /**/
	} cmdKeywordTable;

	static const char * getshortcutHelp(); 
	void printSplashScreen(FILE * out);
	static IFTools_sqlcliInput* getNewInputHandler(IFTools_sqlcliProperties   *m_sqlcliProp);
	virtual char* getnextCMD()=0;
	inline  sqlcliInput_cmdtype getCMDType() {return this->m_cmdtype;}
	static SQLDBC_SQLMode getSQLModeFromString(char* sqlmode);
	
protected:
	static cmdKeywordTable mycmdKeywordTab [ ];
	static cmdKeywordTable sqlmodeKeywordTab [ ];
	static cmdKeywordTable autocommitKeywordTab [ ];
	static cmdKeywordTable connectKeywordTab [ ];
	IFTools_sqlcliInput  (IFTools_sqlcliProperties   *m_sqlcliProp);
	sqlcliInput_cmdtype analyzeSQL(char* pIdentifier);
	static SQLDBC_Int4 analyzeKeyword(char* pIdentifier, SQLDBC_Int4 IdentLength, cmdKeywordTable myCMDKeywTab[] ,SQLDBC_Int4 numElementsTab);
	SQLDBC_Int4 nextSymbol(char* pIdentifier, int* currPos);
    SQLDBC_Int4 isInternalCMD(char* pIdentifier, int* currPos, int* identLen);
    SQLDBC_Bool startprocess (const char **args);

	IFTools_sqlcliProperties    *m_sqlcliProp;     //!<environment   
	sqlcliInput_cmdtype         m_cmdtype;
};

#endif
