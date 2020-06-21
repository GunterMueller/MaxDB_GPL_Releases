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
#if !defined(ZSTRING_H)
#define ZSTRING_H

#include <stdarg.h>

#include "geo00.h"

// ******************************************************************************
#ifdef  ZStringImport
#undef  ZStringImport
#define ZStringImport __declspec(dllimport)
#else
#define ZStringImport
#endif
#ifdef  ZStringExport
#undef  ZStringExport
#define ZStringExport __declspec(dllexport)
#else
#define ZStringExport
#endif
// ******************************************************************************
class ZStringImport ZStringExport ZString  
{
protected:
	char*	m_Buf;
	int		m_Length;
	int		m_Alloc;
	int		m_AllocInitalSize;
	int		m_AllocIncrement;
public:
			ZString();
			ZString(int initAlloc,int incrAlloc);
			ZString(const char* s);
			ZString(const char* s,int l);
			ZString(const ZString& s);
	virtual ~ZString();

	int		GetSize()	const				{ return m_Length;};
	int		GetAllocSize()					{ return m_Alloc;};
	bool	SetAllocIncrement(int incrAlloc);
	bool	SetSize(int l,bool useAllocIncr);
	bool	SetBuf( const ZString& s);
	bool	SetBuf(const char* s);
	bool	SetBuf( const char* s,int l);

	bool	Add( const ZString& s);
	bool	Add(const char* s);
	bool	Add(const char* s,int l);
	bool	AddPrefix(const ZString& s);
	bool	AddPrefix(const char* s);
	bool	AddPrefix(const char* s,int l);

	char&	operator [](int idx);

	operator char*() const;
	ZString& operator =(const char* s);
	ZString& operator =(const ZString& s);

	ZString& operator +=(const ZString& s);
	ZString& operator +=(const char* s);

//	bool	Format(char* fmt,char* s);
//	bool	Format(char* fmt,char* s1,char* s2);
	bool	Format(const char* format, ...);

	bool	Break(ZString& s1,ZString& s2,char breakChar);
	bool	RBreak(ZString& s1,ZString& s2,char breakChar);
	bool	Break(unsigned int len,ZString& s1,ZString& s2);
	bool	RBreak(unsigned int len,ZString& s1,ZString& s2);

	char&	Last();

	bool	Find(char c,int& pos,int startPos=0);
	bool	Find(char* s,int l,int& pos,int startPos=0);
	bool	Find(ZString& s,int& pos,int startPos=0);
	bool	RFind(char c,int& pos,int startPos=-1);
	bool	RFind(char* s,int l,int& pos,int startPos=-1);
	bool	RFind(ZString& s,int& pos,int startPos=-1);

	bool	operator ==(const ZString& s);
	bool	operator ==(const char* s);
	bool	operator !=(const ZString& s);
	bool	operator !=(const char* s);

	bool	SubString(ZString& subString,int startPos,int endPos=-1);

	bool	CharToHex(char* s,int l,bool upperHex=true);
	bool	HexToChar(char* s,int l);
	bool	Base64Decode(char* in,int lIn);

	bool	RemoveChars(char c);
	bool	RemoveChars(char c,bool trailing);

	ZString&ToLower();
	ZString&ToUpper();
protected:
	void	Init(int initAlloc,int incrAlloc);
#ifdef WIN32
	bool	FormatV(char* format, va_list argList);
#endif    

};
// ******************************************************************************
#endif // !defined(ZSTRING_H)
