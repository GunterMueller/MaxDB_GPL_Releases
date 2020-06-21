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
#ifdef WIN32
#define OptionsExport
#endif

#include "ContentStorage/Options.h"
#include "RunTime/RTE_IniFileHandling.h" /* nocheck */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <WINDOWS.H>
#endif

//#define NOXTRACE
//#include "TRACE.h"
// ******************************************************************************
Options::Options(char* homeDir,char* optionsFileName)
{
	m_OptionsFileName = new ZString();
	if(!optionsFileName)
		return;
	if(homeDir)
	{
		*m_OptionsFileName = homeDir;
#ifdef WIN32
		m_OptionsFileName->Add("\\");
#else
		m_OptionsFileName->Add("/");
#endif
		m_OptionsFileName->Add(optionsFileName);
	}
	else
	{
		*m_OptionsFileName = optionsFileName;
	}
}
// ******************************************************************************
Options::~Options()
{
	delete m_OptionsFileName;
}
// ******************************************************************************
bool Options::GetOption( const char* section, const char* name,ZString& value, const char* defaultValue)
{
	DWORD l		= 12000;
#ifndef WIN32
    RTE_IniFileResult   ok = true;
    tsp00_ErrTextc      errText;
#endif
retry:
	char* buf	= new char[l];
	if(!buf)
		return false;
#ifdef WIN32
	DWORD len = ::GetPrivateProfileString(section,name,defaultValue,buf,l,(char*)*m_OptionsFileName);
#else
    DWORD len = RTE_GetConfigString( (char*) *m_OptionsFileName,
                                    section, name, buf, l, errText, ok );
#endif
	if(len == l-1 || (len == l-2 && (section == 0 || name == 0) && (buf[len]==0 && buf[len+1]==0)))
	{
		delete buf;
		l = 2*l;
		goto retry;
	}
	value.SetBuf(buf,len);
	delete buf;
	return true;
}
// ******************************************************************************
bool Options::GetSection(char* section,ZString& value)
{
	DWORD l		= 12000;
retry:
	char* buf	= new char[l];
	if(!buf)
		return false;
#ifdef WIN32
	DWORD len = ::GetPrivateProfileSection(section,buf,l,(char*)*m_OptionsFileName);
#else
    DWORD len = 0;
    // ...
#endif
	if(len == l-2)
	{
		delete buf;
		l = 2*l;
		goto retry;
	}
	value.SetBuf(buf,len);
	delete buf;
	return true;
}
// ******************************************************************************
bool Options::GetOption(char* section,char* name,bool defaultValue)
{
#ifdef WIN32
	DWORD val = ::GetPrivateProfileInt(section,name,defaultValue,(char*)*m_OptionsFileName);
#else
    DWORD val = 0;
    // ...
#endif
	return val!=0;
}
// ******************************************************************************
int Options::GetOption(char* section,char* name,int defaultValue)
{
#ifdef WIN32
	DWORD val = ::GetPrivateProfileInt(section,name,defaultValue,(char*)*m_OptionsFileName);
#else
    DWORD val = 0;
    // ...
#endif
	return (int)val;
}
// ******************************************************************************
bool Options::SetOption( const char* section, const char* name, const char* value)
{
#ifdef WIN32
	return 0!= ::WritePrivateProfileString(section,name,value,(char*)*m_OptionsFileName);
#else
    // ...
    return false;
#endif
}
// ******************************************************************************
bool Options::GetSections(XArrayZString& sections)
{	
	sections.SetSize(0);
	ZString s;
	if(!GetOption(0,0,s,""))
		return false;
	ZString x;
	int pos	= 0, startPos = 0;
	while(s.Find((char)0,pos,startPos))
	{
		if(pos==startPos)
		{
			startPos++;
			continue;
		}
		x.SetBuf(((char*)s)+startPos,pos-startPos);
		sections.Add(x);
		startPos = pos+1;
	}
	return true;
}
// ******************************************************************************
bool Options::GetKeys(char* section,XArrayZString& keys)
{
	keys.SetSize(0);
	ZString s;
	if(!GetOption(section,0,s,""))
		return false;
	ZString x;
	int pos	= 0, startPos = 0;
	while(s.Find((char)0,pos,startPos))
	{
		if(pos==startPos+1)
			continue;
		x.SetBuf(((char*)s)+startPos,pos-startPos);
		keys.Add(x);
		startPos = pos+1;
	}
	return true;
}
// ******************************************************************************
bool Options::GetKeyValues(char* section,XArrayZString& keys)
{
	keys.SetSize(0);
	ZString s;
	if(!GetSection(section,s))
		return false;
	ZString x;
	int pos	= 0, startPos = 0;
	while(s.Find((char)0,pos,startPos))
	{
		if(pos==startPos+1)
			continue;
		x.SetBuf(((char*)s)+startPos,pos-startPos);
		keys.Add(x);
		startPos = pos+1;
	}
	return true;
}
// ******************************************************************************
bool Options::SetKeyValues(char* section,XArrayZString& entries)
{
	ZString s(1000,1000);
	for(int i=0;i<entries.GetSize();i++)
	{	
		ZString& x = entries.GetAt(i);
		s.Add(x);
		s.Add("",1);
	}
	s.Add("",1);
#ifdef WIN32
	return 0!= ::WritePrivateProfileSection(section,(char*)s,(char*)*m_OptionsFileName);
#else
    // ...
    return false;
#endif
}
// ******************************************************************************
bool Options::DeleteSection(char* section)
{
#ifdef WIN32
	return 0!=::WritePrivateProfileString(section,NULL,NULL,(char*)*m_OptionsFileName);
#else
    // ...
    return false;
#endif
}
// ******************************************************************************

