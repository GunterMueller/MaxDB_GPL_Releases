/*



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
#ifndef HPA83ODBCINSTEXT_H
#define HPA83ODBCINSTEXT_H

#define ODBCINST_SUCCESS				0
#define ODBCINST_WARNING				1
#define ODBCINST_ERROR					2

#define	ODBCINST_PROMPTTYPE_LABEL		0 /* readonly */
#define	ODBCINST_PROMPTTYPE_TEXTEDIT	1
#define	ODBCINST_PROMPTTYPE_LISTBOX		2
#define	ODBCINST_PROMPTTYPE_COMBOBOX	3
#define	ODBCINST_PROMPTTYPE_FILENAME	4

#define     INI_MAX_LINE            1000
#define     INI_MAX_PROPERTY_NAME   INI_MAX_LINE
#define     INI_MAX_PROPERTY_VALUE  INI_MAX_LINE

typedef struct	tODBCINSTPROPERTY
{
	struct tODBCINSTPROPERTY *pNext;				/* pointer to next property, NULL if last property										*/

	char	szName[INI_MAX_PROPERTY_NAME+1];		/* property name																		*/
	char	szValue[INI_MAX_PROPERTY_VALUE+1];		/* property value																		*/
	int		nPromptType;							/* PROMPTTYPE_TEXTEDIT, PROMPTTYPE_LISTBOX, PROMPTTYPE_COMBOBOX, PROMPTTYPE_FILENAME	*/
	char	**aPromptData;							/* array of pointers terminated with a NULL value in array. 							*/
	char	*pszHelp;								/* help on this property (driver setups should keep it short)							*/
	void	*pWidget;								/* CALLER CAN STORE A POINTER TO ? HERE													*/
	int		bRefresh;								/* app should refresh widget ie Driver Setup has changed aPromptData or szValue   		*/
	void 	*hDLL;									/* for odbcinst internal use... only first property has valid one 						*/
} ODBCINSTPROPERTY, *HODBCINSTPROPERTY;

#endif

