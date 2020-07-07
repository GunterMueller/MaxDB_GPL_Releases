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
#if !defined(OPTIONS_H)
#define OPTIONS_H

// ******************************************************************************
#ifdef  OptionsImport
#undef  OptionsImport
#define OptionsImport __declspec(dllimport)
#else
#define OptionsImport
#endif
#ifdef  OptionsExport
#undef  OptionsExport
#define OptionsExport __declspec(dllexport)
#else
#define OptionsExport
#endif

#include "ContentStorage/ZString.h"
#include "ContentStorage/XArray.h"

// ******************************************************************************
class ZString;
typedef XArray<ZString> XArrayZString;
// ******************************************************************************
class OptionsImport OptionsExport Options  
{
	ZString*m_OptionsFileName;
public:
			Options(char* homeDir,char* optionsFileName);
	virtual ~Options();

	bool	GetOption(const char* section, const char* name,ZString& value, const char* defaultValue);
	bool	GetOption(char* section,char* name,bool defaultValue);
	int		GetOption(char* section,char* name,int defaultValue);

	bool	SetOption( const char* section, const char* name, const char* value);

	bool	GetSections(XArrayZString& sections);

	bool	GetSection(char* section,ZString& value);

	bool	GetKeys(char* section,XArrayZString& keys);
	bool	GetKeyValues(char* section,XArrayZString& entries);
	bool	SetKeyValues(char* section,XArrayZString& entries);

	bool	DeleteSection(char* section);
};
// ******************************************************************************

#endif // !defined(OPTIONS_H)
