/*!
    \file    ToolsParsersUtil_String.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a very simple C-like string class

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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

    \endif
*/

#if !defined(TOOLSPARSERSUTIL_STRING_HPP)
#define TOOLSPARSERSUTIL_STRING_HPP

#include <string.h>
#include <ctype.h>

#include "SAPDB/SAPDBCommon/SAPDB_string.h"

/*! \brief a very very simple C-like string class */
class ToolsParsersUtil_String
{
  public:
    ToolsParsersUtil_String()
        :s(0)
    {
    }

    ToolsParsersUtil_String(size_t size)
        :s(0)
    {
        alloc(size);
    }

    ToolsParsersUtil_String(const char * string)
        :s(0)
    {
        allocCopy(string);
    }

    ToolsParsersUtil_String(const char *start, const char * end)
        :s(0)
    {
        allocCopy(start, end);
    }

    void operator =(const ToolsParsersUtil_String & otherString)
    {
        allocCopy(otherString);
    }

    ~ToolsParsersUtil_String()
    {
        if(0!=s)
            delete [] s;
    }

    bool isValid()
    {
        return 0!=s;
    }

    const char * end() const
    {
        return s+strlen(s);
    }

    /*size_t getLength()
    {
        return strlen(s);
    }*/

    static const char * end(const char * s)
    {
        return s+strlen(s);
    }

    bool alloc(size_t length)
    {
        bool rc=true;

        if(0!=s)
            delete [] s;

        s=new char [length+1];

        if(0==s)
            rc=false;
        else
            s[0]='\0';

        return rc;
    }

    bool allocCopy(const char * src)
    {
        bool rc=alloc(strlen(src));

        if(rc)
            SAPDB_strcpy(s, src);

        return rc;
    }

    bool allocCopy(const char *srcStart, const char *srcEnd)
    {
        bool rc=alloc(srcEnd-srcStart);

        if(rc)
        {
            SAPDB_strncpy(s, srcStart, srcEnd-srcStart);
            s[srcEnd-srcStart]='\0';
        }

        return rc;
    }

    bool equals(const char * otherString) const
    {
        return (0==SAPDB_strcmp(s, otherString));
    }

    bool equalsIgnoreCase(const char * otherString) const {
        return (0==SAPDB_strcasecmp(s, otherString));
    }

    bool equals(const char * otherString, const char * otherStringEnd) const
    {
        return (strlen(s)==(otherStringEnd-otherString) && 0==SAPDB_strncmp(s, otherString, otherStringEnd-otherString));
    }

    bool equalsIgnoreCase(const char * otherString, const char * otherStringEnd) const {
        return (strlen(s)==(otherStringEnd-otherString) && 0==SAPDB_strncasecmp(s, otherString, otherStringEnd-otherString));
    }

    static const char * findPositionOf(const char * s, const char *endOfS, const char c)
    {
        while('\0'!=(*s) && s!=endOfS && (*s)!=c)
            s++;

        return s;
    }

    static const char * getPositionBehindNext(const char * s, const char c)
    {
        const char * rc=strchr(s, c);

        if(0==rc)
            rc=end(s);
        else
        {
            if(c==(*rc))
                rc++;
        }

        return rc;
    }

    static const char * findPositionBehindNext(const char * s, const char *endOfS, const char c)
    {
        while('\0'!=(*s) && s!=endOfS && (*s)!=c)
            s++;

        if(c==(*s))
            s++;

        return s;
    }

    static const char * findPositionOf(const char * s, const char *endOfS, const char * subString)
    {
        bool found=false;

        while('\0'!=(*s) && s!=endOfS && !found)
        {
            const char * s1=s;
            const char * s2=subString;

            while('\0'!=(*s1) && s1!=endOfS && '\0'!=(*s2) && (*s1)==(*s2))
            {
                s1++;
                s2++;
            }

            if('\0'==(*s2))
                found=true;
            else
                s++;
        }

        return s;
    }

    static const char * findPositionBehindNext(const char * s, const char *endOfS, const char * subString)
    {
        bool found=false;
        const char * rc=endOfS;

        while('\0'!=(*s) && s!=endOfS && !found)
        {
            const char * s1=s;
            const char * s2=subString;

            while('\0'!=(*s1) && s1!=endOfS && '\0'!=(*s2) && (*s1)==(*s2))
            {
                s1++;
                s2++;
            }

            if('\0'==(*s2))
            {
                found=true;
                rc=s1;
            }
            else
                s++;
        }

        return rc;
    }

    const char * getNextWhiteSpace() const
    {
        const char * rc=s;

        while('\0'!=(*rc) && !isspace(*rc))
            rc++;

        return rc;
    }

    static const char * getNextWhiteSpace(const char * s)
    {
        while('\0'!=(*s) && !isspace(*s))
            s++;

        return s;
    }

    static const char * getNextWhiteSpace(const char * s, const char *endOfS)
    {
        while('\0'!=(*s) && s!=endOfS && !isspace(*s))
            s++;

        return s;
    }

    static const char * getNextNonWhiteSpace(const char * s)
    {   
        while('\0'!=(*s) && isspace(*s))
            s++;

        return s;
    }

    static const char * getNextNonWhiteSpace(const char * s, const char * endOfS)
    {   
        while('\0'!=(*s) && endOfS!=s && isspace(*s))
            s++;

        return s;
    }

    static bool isHexDigit(char c)
    {
        return (0!=isxdigit(c));
    }

    static char getHexValueFor(char c)
    {
        if(isHexDigit(c))
            return ('0'<=c && c<='9')?(c-'0'):(toupper(c)-'A');
        else
            return -1;
    }

    operator const char * () const
    {
        return s;
    }

    char & operator [](size_t i)
    {
        return s[i];
    }
    
  private:
    char * s;
};

#endif
