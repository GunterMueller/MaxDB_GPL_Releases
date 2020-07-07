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
#define ZStringExport
#endif

#include "ContentStorage/ZString.h"

#include <string.h>
#include <stdio.h>

#include <stdlib.h> 
#include <ctype.h>

#ifdef WIN32
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>					// only needed for Format() & unicode
#endif

// ******************************************************************************
ZString::ZString()
{
	Init(50,50);
}
// ******************************************************************************
ZString::ZString(int initAlloc,int incrAlloc)
{
	Init(initAlloc,incrAlloc);
}
// ******************************************************************************
ZString::ZString(const char* s)
{
	if(s)
	{
		int l = strlen(s);
		Init(l,50);
		SetBuf(s,l);
	}
	else
		Init(50,50);
}
// ******************************************************************************
ZString::ZString(const char* s,int l)
{
	Init(l,50);
	SetBuf(s,l);
}
// ******************************************************************************
ZString::ZString(const ZString& s)
{
	Init(s.m_Alloc,s.m_AllocIncrement);
	SetBuf(s.m_Buf,s.m_Length);
}
// ******************************************************************************
ZString::~ZString()
{
	delete m_Buf;
}
// ******************************************************************************
ZString::operator char*() const
{
	return m_Buf;
}
// ******************************************************************************
char& ZString::operator [](int idx)
{
	return m_Buf[idx];
}
// ******************************************************************************
ZString& ZString::operator =(const char*s)
{
	if(s)
		SetBuf(s,strlen(s));
	else
		SetSize(0,false);
	return *this;
}
// ******************************************************************************
ZString& ZString::operator =(const ZString& s)
{
	SetBuf(s.m_Buf,s.m_Length);	
	return *this;
}
// ******************************************************************************
bool ZString::operator ==( const ZString& s)
{
	if(m_Length != s.GetSize())
		return false;
	return strncmp(m_Buf,(char*)s,m_Length)==0;
}
// ******************************************************************************
bool ZString::operator ==( const char* s)
{
	if(!s)
		return false;
	if(m_Length != (int)strlen(s))
		return false;
	return strncmp(m_Buf,s,m_Length)==0;
}
// ******************************************************************************
bool ZString::operator !=( const ZString& s)
{
	return !((*this)==s);
}
// ******************************************************************************
bool ZString::operator !=( const char* s)
{
	return !((*this)==s);
}
// ******************************************************************************
ZString& ZString::operator +=(const ZString& s)
{
	Add(s);
	return *this;
}
// ******************************************************************************
ZString& ZString::operator +=(const char* s)
{
	Add(s);
	return *this;
}
// ******************************************************************************
void ZString::Init(int initAlloc,int incrAlloc)
{
	m_Buf				= 0;
	m_Alloc				= 0;
	m_Length			= 0;
	m_AllocInitalSize	= initAlloc;
	m_AllocIncrement	= incrAlloc;

	m_Buf				= new char[m_AllocInitalSize+1];
	m_Buf[m_Length]		= 0;
	m_Alloc				= m_AllocInitalSize;
}
// ******************************************************************************
bool ZString::Add( const ZString& s)
{
	return Add(s,s.GetSize());
}
// ******************************************************************************
bool ZString::Add( const char* s)
{
	if(!s)
		return Add("",1);
	return Add(s,strlen(s));
}
// ******************************************************************************
bool ZString::Add(const char* s,int l)
{
	int ll = m_Length;
	if(!SetSize(ll+l,true))
		return false;
	memcpy(m_Buf+ll,s,l);
	m_Buf[m_Length] = 0;
	return true;
}
// ******************************************************************************
bool ZString::AddPrefix( const ZString& s)
{
	return AddPrefix(s,s.GetSize());
}
// ******************************************************************************
bool ZString::AddPrefix(const char* s)
{
	if(!s)
		return true;
	return AddPrefix(s,strlen(s));
}
// ******************************************************************************
bool ZString::AddPrefix(const char* s,int l)
{
	int ll = m_Length;
	if(!SetSize(ll+l,true))
		return false;
	memmove(m_Buf+l,m_Buf,ll);
	memcpy(m_Buf,s,l);
	m_Buf[m_Length] = 0;
	return true;
}
// ******************************************************************************
bool ZString::SetBuf( const char* s,int l)
{
	if(!SetSize(l,false))
		return false;
	memcpy(m_Buf,s,l);
	m_Buf[l]	= 0;
	m_Length	= l;
	return true;
}
// ******************************************************************************
bool ZString::SetBuf( const char* s)
{
	return SetBuf(s,strlen(s));
}
// ******************************************************************************
bool ZString::SetBuf(const ZString& s)
{
	return SetBuf(s,s.GetSize());
}
// ******************************************************************************
bool ZString::SetAllocIncrement(int incrAlloc)
{
	if(incrAlloc <= 0)
		return false;
	m_AllocIncrement = incrAlloc;
	return true;
}
// ******************************************************************************
bool ZString::SetSize(int l,bool useAllocIncr)
{
	if(l<0)
		return false;
	int incr = useAllocIncr?m_AllocIncrement:0;
	if(!m_Buf)
	{
		m_Buf		= new char[l+1+incr];
		if(!m_Buf)
		{
			m_Length = 0;
			return false;
		}
		m_Buf[l]	= 0;
		m_Alloc		= l+incr;
		m_Length	= l;
		return true;
	}
	if(l<=m_Alloc)
	{	
		m_Length	= l;
		m_Buf[l]	= 0;
		return true;
	}
	else
	{
		char* b		= new char[l+1+incr];
		if(!b)
		{
			m_Length = 0;
			return false;
		}
		memcpy(b,m_Buf,m_Length);
		delete m_Buf;
		m_Buf		= b;
		m_Buf[l]	= 0;
		m_Alloc		= l+incr;
		m_Length	= l;
		return true;
	}
}
// ******************************************************************************
/*
bool ZString::Format(char* fmt,char* s)
{
	if(!fmt)
		return false;
	if(!s)
		s = "";
	SetSize(strlen(fmt)+strlen(s),false);
	sprintf(m_Buf,fmt,s);
	m_Length = strlen(m_Buf);
	return true;
}
// ******************************************************************************
bool ZString::Format(char* fmt,char* s1,char* s2)
{
	if(!fmt)
		return false;
	if(!s1) s1 = "";
	if(!s2) s2 = "";
	SetSize(strlen(fmt)+strlen(s1)+strlen(s2),false);
	sprintf(m_Buf,fmt,s1,s2);
	m_Length = strlen(m_Buf);
	return true;
}
*/
// ******************************************************************************
bool ZString::Break(unsigned int len,ZString& s1,ZString& s2)
{
	if(len>(unsigned int)GetSize())
	{
		s1 = *this;
		s2 = "";
		return false;
	}
	s1.SetBuf(m_Buf,len);
	s2.SetBuf(m_Buf+len,GetSize()-len);
	return true;
}
// ******************************************************************************
bool ZString::RBreak(unsigned int len,ZString& s1,ZString& s2)
{
	if(len>(unsigned int)GetSize())
	{
		s1 = "";
		s2 = *this;
		return false;
	}
	int l = GetSize()-len;
	s1.SetBuf(m_Buf,l);
	s2.SetBuf(m_Buf+l,len);
	return true;
}
// ******************************************************************************
bool ZString::Break(ZString& s1,ZString& s2,char breakChar)
{
	char* p = strchr(m_Buf,breakChar);
	if(!p)
	{
		s1 = *this;
		s2 = "";
		return false;
	}
	s1.SetBuf(m_Buf,p-m_Buf);
	s2.SetBuf(p+1,GetSize()-(p-m_Buf+1));
	return true;
}
// ******************************************************************************
bool ZString::RBreak(ZString& s1,ZString& s2,char breakChar)
{
	char* p = strrchr(m_Buf,breakChar);
	if(!p)
	{
		s1 = "";
		s2 = *this;
		return false;
	}
	s1.SetBuf(m_Buf,p-m_Buf);
	s2.SetBuf(p+1,GetSize()-(p-m_Buf+1));
	return true;
}
// ******************************************************************************
bool ZString::Find(char c,int& pos,int startPos)
{
	pos = GetSize();
	if(startPos>=GetSize() || startPos <0)
		return false;
	char* p		= m_Buf+startPos;
	char* pEnd	= m_Buf+GetSize();
	while(p<pEnd && *p != c)
		p++;
	if(p>=pEnd)
		return false;
	pos = p-m_Buf;
	return true;
}
// ******************************************************************************
bool ZString::RFind(char c,int& pos,int startPos)
{
	pos = 0;
	if(startPos == -1)
		startPos = GetSize()-1;
	if(startPos < 0 || startPos >= GetSize())
		return false;
	char* p		= m_Buf+startPos;
	char* pEnd	= m_Buf-1;
	while(p>pEnd && *p != c)
		p--;
	if(p<=pEnd)
		return false;
	pos = p-m_Buf;
	return true;
}
// ******************************************************************************
bool ZString::Find(ZString& s,int& pos,int startPos)
{
	return Find((char*)s,s.GetSize(),pos,startPos);
}
// ******************************************************************************
bool ZString::Find(char* s,int l,int& pos,int startPos)
{
	pos = GetSize();
	if(startPos>=GetSize() || startPos <0)
		return false;
	char* p		= m_Buf+startPos;
	char* pEnd	= m_Buf+GetSize();
	while(p<pEnd && strncmp(p,s,l)!=0)
		p++;
	if(p>=pEnd)
		return false;
	pos = p-m_Buf;
	return true;
}
// ******************************************************************************
bool ZString::RFind(ZString& s,int& pos,int startPos)
{
	return RFind((char*)s,s.GetSize(),pos,startPos);
}
// ******************************************************************************
bool ZString::RFind(char* s,int l,int& pos,int startPos)
{
	pos = 0;
	if(startPos == -1)
		startPos = GetSize()-1;
	if(startPos < 0 || startPos >= GetSize())
		return false;
	char* p		= m_Buf+startPos;
	char* pEnd	= m_Buf-1;
	if(p+l>m_Buf+GetSize())
		p = m_Buf+GetSize()-l;
	while(p>pEnd && strncmp(p,s,l)!=0)
		p--;
	if(p<=pEnd)
		return false;
	pos = p-m_Buf;
	return true;
}
// ******************************************************************************
bool ZString::SubString(ZString& toString,int startPos,int endPos)
{
	int l = GetSize();
	if((startPos >= l && l > 0) || startPos < 0)
		return false;
	if(endPos > l || endPos < 0)
		return false;
	if(startPos>endPos)
		return false;
	toString.SetBuf(m_Buf+startPos,endPos-startPos);
	return true;
}
// ******************************************************************************
char& ZString::Last()
{
	return m_Buf[GetSize()-1];
}
// ******************************************************************************
bool ZString::RemoveChars(char c)
{
	bool rc =  RemoveChars(c,true);
	rc		&= RemoveChars(c,false);
	return rc;
}
// ******************************************************************************
bool ZString::RemoveChars(char c,bool trailing)
{
	if(GetSize()==0)
		return true;
	if(trailing)
	{
		int l = m_Length;
		while(l > 0 && m_Buf[l-1] == c)
			l--;
		SetSize(l,false);
	}
	else
	{
		int l = m_Length;
        int i = 0;
		for(i=0;i<l && m_Buf[i] == c;i++)
		{
		}
		if(i!=0)
		{
			memmove(m_Buf,m_Buf+i,m_Length-i);
			SetSize(m_Length-i,false);
		}
	}
	return true;
}
// ******************************************************************************
ZString& ZString::ToLower()
{
	for(int i=0;i<m_Length;i++)
		m_Buf[i] = tolower(m_Buf[i]);
	return *this;
}
// ******************************************************************************
ZString& ZString::ToUpper()
{
	for(int i=0;i<m_Length;i++)
		m_Buf[i] = toupper(m_Buf[i]);
	return *this;
}
// ******************************************************************************
bool ZString::HexToChar(char* s,int l)
{
	if(l<0 || l%2)
		return false;
	if(!SetSize(l/2,false))
		return false;
	char* buf		= (char*)(*this);
	const char* hs  = "0123456789ABCDEF";
    int   i = 0;
	for(i=0;i<l-1;i+=2)
	{
		unsigned char c1= ((unsigned char*)s)[i];
		unsigned char c2= ((unsigned char*)s)[i+1];
		const char* p1 = strchr(hs,toupper(c1));
		const char* p2 = strchr(hs,toupper(c2));
		if(!(p1 && p2))
			return false;
		buf[i/2]		= (p1-hs)*16+(p2-hs);
	}
	return true;
}
// ******************************************************************************
bool ZString::CharToHex(char* s,int l,bool upperHex)
{
	if(l<0)
		return false;
	if(!SetSize(2*l,false))
		return false;
	const char* hs	= upperHex?"0123456789ABCDEF":"0123456789abcdef";
	char* buf		= (char*)(*this);
	for(int i=0;i<l;i++)
	{
		unsigned char c = ((unsigned char*)s)[i];
		char h1			= hs[c/16];
		char h2			= hs[c%16];
		buf[2*i]		= h1;
		buf[2*i+1]		= h2;
	}
	return true;
}
// ******************************************************************************
bool ZString::Base64Decode(char* in,int lIn)
{     
	unsigned char inbuf [4], outbuf [4];

	SetSize(lIn,false);
	SetSize(0,false);

	int inIdx	= 0;
	int inbufIdx= 0;
	bool endText= false;
	while(true) 
	{
		if (inIdx >= lIn)
				break;    
		unsigned char ch = in[inIdx++];
		bool ignore = false;
		if ((ch >= 'A') && (ch <= 'Z'))
				ch = ch - 'A';
		else if ((ch >= 'a') && (ch <= 'z'))
				ch = ch - 'a' + 26;      
		else if ((ch >= '0') && (ch <= '9'))
				ch = ch - '0' + 52;
		else if (ch == '+')
				ch = 62;       
		else if (ch == '=') /*no op -- can't ignore this one*/
				endText = true;       
		else if (ch == '/')
				ch = 63;
		else
				ignore = true; 
		if(!ignore) 
		{
			int inbufChars = 3;
			bool doBreak = false;                 
			if (endText) 
			{                
				if(inbufIdx == 0)
					break;                        
				if((inbufIdx == 1) || (inbufIdx == 2))
					inbufChars = 1;
				else
					inbufChars = 2;                
				inbufIdx = 3;                
				doBreak = true;
			}

			inbuf[inbufIdx++] = ch;        
			if (inbufIdx == 4) 
			{                
				inbufIdx	= 0;                
				outbuf [0]	= (inbuf [0] << 2) | ((inbuf [1] & 0x30) >> 4);                
				outbuf [1]	= ((inbuf [1] & 0x0F) << 4) | ((inbuf [2] & 0x3C) >> 2);                
				outbuf [2]	= ((inbuf [2] & 0x03) << 6) | (inbuf [3] & 0x3F);

				Add((char*)outbuf,inbufChars);
			}        
			if(doBreak)
				break;
		}
	}
	return true;
}
// ******************************************************************************






// ******************************************************************************
// formatting (using wsprintf style formatting)
#ifdef WIN32
bool ZString::Format(const char* format, ...)
{
	va_list argList;
	va_start(argList, format);
	bool rc = FormatV( (char*) format, argList);
	va_end(argList);
	return rc;
}
// ******************************************************************************
#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define TCHAR_ARG		TCHAR
#define WCHAR_ARG		wchar_t
#define CHAR_ARG		char
#define DOUBLE_ARG		double
// ******************************************************************************
bool ZString::FormatV(char* format, va_list argList)
{
	va_list argListSave = argList;
					// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (char* pFormat = format; *pFormat != '\0'; pFormat = _tcsinc(pFormat))
	{
		// handle '%' character, but watch out for '%%'
		if (*pFormat != '%' || *(pFormat = _tcsinc(pFormat)) == '%')
		{
			nMaxLen += _tclen(pFormat);
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *pFormat != '\0'; pFormat = _tcsinc(pFormat))
		{
			// check for valid flags
			if (*pFormat == '#')
				nMaxLen += 2;   // for '0x'
			else if (*pFormat == '*')
				nWidth = va_arg(argList, int);
			else if (*pFormat == '-' || *pFormat == '+' || *pFormat == '0' ||
				*pFormat == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = _ttoi(pFormat);
			for (; *pFormat != '\0' && _istdigit(*pFormat); pFormat = _tcsinc(pFormat))
				;
		}

		int nPrecision = 0;
		if (*pFormat == '.')
		{
			// skip past '.' separator (width.precision)
			pFormat = _tcsinc(pFormat);

			// get precision and skip it
			if (*pFormat == '*')
			{
				nPrecision = va_arg(argList, int);
				pFormat = _tcsinc(pFormat);
			}
			else
			{
				nPrecision = _ttoi(pFormat);
				for (; *pFormat != '\0' && _istdigit(*pFormat); pFormat = _tcsinc(pFormat))
					;
			}
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		switch (*pFormat)
		{
			// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				pFormat = _tcsinc(pFormat);
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				pFormat = _tcsinc(pFormat);
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				pFormat = _tcsinc(pFormat);
				break;
		}

		// now should be on specifier
		switch (*pFormat | nModifier)
		{
			// single characters
			case 'c':
			case 'C':
				nItemLen = 2;
				va_arg(argList, TCHAR_ARG);
				break;
			case 'c'|FORCE_ANSI:
			case 'C'|FORCE_ANSI:
				nItemLen = 2;
				va_arg(argList, CHAR_ARG);
				break;
			case 'c'|FORCE_UNICODE:
			case 'C'|FORCE_UNICODE:
				nItemLen = 2;
				va_arg(argList, WCHAR_ARG);
				break;

			// strings
			case 's':
			{
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = lstrlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
				break;
			}

			case 'S':
			{
#ifndef _UNICODE
				WCHAR_ARG* pstrNextArg = va_arg(argList, WCHAR_ARG*);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#else
				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = lstrlenA(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#endif
				break;
			}

			case 's'|FORCE_ANSI:
			case 'S'|FORCE_ANSI:
			{
				char* pstrNextArg = va_arg(argList, char*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = lstrlenA(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
				break;
			}

#ifndef _MAC
			case 's'|FORCE_UNICODE:
			case 'S'|FORCE_UNICODE:
			{
				WCHAR_ARG* pstrNextArg = va_arg(argList, WCHAR_ARG*);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
				break;
			}
#endif
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			nItemLen = max(nItemLen, nWidth);
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
		}
		else
		{
			switch (*pFormat)
			{
				// integers
				case 'd':
				case 'i':
				case 'u':
				case 'x':
				case 'X':
				case 'o':
					va_arg(argList, int);
					nItemLen = 32;
					nItemLen = max(nItemLen, nWidth+nPrecision);
					break;

				case 'e':
				case 'f':
				case 'g':
				case 'G':
					va_arg(argList, DOUBLE_ARG);
					nItemLen = 128;
					nItemLen = max(nItemLen, nWidth+nPrecision);
					break;

				case 'p':
					va_arg(argList, void*);
					nItemLen = 32;
					nItemLen = max(nItemLen, nWidth+nPrecision);
					break;

				// no output
				case 'n':
					va_arg(argList, int*);
					break;

				default:
//							 unknown formatting option
					;
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	SetSize(nMaxLen,false);
	int l = _vstprintf((char*)(*this), format, argListSave);
	SetSize(l,false);

	va_end(argListSave);

	return true; // ???
}
#else // UNIX
bool ZString::Format(const char* format, ...)
{
    char      buf[2048];
	va_list   argList;

	va_start(argList, format);
	vsprintf( buf, format, argList );
	va_end(argList);

    return this->SetBuf( buf );
}
#endif
// ******************************************************************************
