/*!
    \file    vcn36.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementing basic functions used in vcn35.cpp, ..., vcn355.cpp

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2006 SAP AG

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

#include "hni31.h"
#include "hcn36.h"

#include "SAPDBCommon/SAPDB_string.h"

#include<sys/types.h>
#include<sys/stat.h>
#if defined (WIN32)
    #include <io.h>
    #define         F_OK                    0x0000
    #define         W_OK                    0x0010
    #define         R_OK                    0x0100
#elif defined(UNIX)
    #include<unistd.h>    /* F_OK, R_OK, X_OK */
    #include<fcntl.h>
    #include<sys/stat.h>
    #include<sys/types.h>
    #include<sys/utsname.h>
    #include<ctype.h>
#endif
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>
#include<limits.h>

#include "SAPDB/SAPDBCommon/SAPDB_string.h"

// -----------------------------------------------------------------------------
// class tcn36_CN36Error
// -----------------------------------------------------------------------------

class tcn36_CN36Error: public tni35_AError
{
  public:
    enum CN36ErrorCase
    {
        CaseOutOfMemory,
        CaseCanNotGetComputerName,
        CaseCanNotExportEnvironment,
        CaseUnknownDateTimeFormatString,
        CaseInternalError   //must remain the last error case (or change FindThisErrorInDescriptonArray())
    };

    tcn36_CN36Error(CN36ErrorCase ErrCase);
    tcn36_CN36Error(CN36ErrorCase ErrCase, const char *ErrInfo);
    tcn36_CN36Error(CN36ErrorCase ErrCase, int ErrInfo);

    virtual void FillErrorMsgBuffer() const;

    enum CN36ErrorInfoType {None, AString, AIntCode};

    struct CN36ErrorDescription // must be public for initialization of static member DBMSrvErrorToText
    {
        CN36ErrorCase       Case;
        CN36ErrorInfoType   InfoType;
        const char        * PlainErrorText;
        const char        * ExtendedErrorText;
    };

  private:
    static CN36ErrorDescription CN36ErrorToText[];
    const CN36ErrorDescription * ThisError;

    const  CN36ErrorDescription * FindThisErrorInDescriptonArray(CN36ErrorCase CaseToSet);
    int IntCode;

    static const int MaximalCharsForAnInt;
};


tcn36_CN36Error::CN36ErrorDescription tcn36_CN36Error::CN36ErrorToText[]=
{
    {CaseOutOfMemory,
     None,
     "Out of memory.",
     0},

    {CaseCanNotGetComputerName,
     AIntCode,
     "Can not determine the computer name.",
     "Can not determine the computer name (error %d)."},

    {CaseCanNotExportEnvironment,
     AString,
     "Can not export an environment variable.",
     "Can not export the environment variable %s."},

    {CaseUnknownDateTimeFormatString,
     AString,
     "An unknown date and time format string was specified.",
     "The date and time format string '%s' is unknown."},

    {CaseInternalError,
     None,
     "An internal error occured.",
     0}
};

const int tcn36_CN36Error::MaximalCharsForAnInt=20;

tcn36_CN36Error::tcn36_CN36Error(CN36ErrorCase ErrCase)
:tni35_AError(0) // do not set basis error, to avoid doing the same twice
{
    ThisError=FindThisErrorInDescriptonArray(ErrCase);
    Set();
    IntCode=0;
}

tcn36_CN36Error::tcn36_CN36Error(CN36ErrorCase ErrCase, const char *ErrInfo)
:tni35_AError(0) // do not set basis error, to avoid doing the same twice
{
    ThisError=FindThisErrorInDescriptonArray(ErrCase);
    Set(ErrInfo);
    IntCode=0;
}

tcn36_CN36Error::tcn36_CN36Error(CN36ErrorCase ErrCase, int ErrInfo)
:tni35_AError(0) // do not set basis error, to avoid doing the same twice
{
    ThisError=FindThisErrorInDescriptonArray(ErrCase);
    Set();
    IntCode=ErrInfo;
}

void tcn36_CN36Error::FillErrorMsgBuffer() const
{
    size_t LengthOfExtendedErrorMsg;

    switch(ThisError->InfoType)
    {
        case None:
            LengthOfExtendedErrorMsg=0;
        break;
        case AString:
            LengthOfExtendedErrorMsg=strlen(ThisError->ExtendedErrorText)-2+strlen(GiveErrorInfoBuffer());
        break;
        case AIntCode:
            LengthOfExtendedErrorMsg=strlen(ThisError->ExtendedErrorText)-2+MaximalCharsForAnInt;
        default:
            ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseInternalError));
    }

    if(ErrorMsgBufferLength_ni35-1 >= LengthOfExtendedErrorMsg)
    {
        switch(ThisError->InfoType)
        {
            case None:
                cn36_StrNCpy(ErrorMsgBuffer, ThisError->PlainErrorText, ErrorMsgBufferLength_ni35-1);
            break;
            case AString:
                sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, GiveErrorInfoBuffer());
            break;
            case AIntCode:
                sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, IntCode);
            break;
            default:
                ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseInternalError));
        }
    }
    else
        cn36_StrNCpy(ErrorMsgBuffer, ThisError->PlainErrorText, ErrorMsgBufferLength_ni35-1);
}

const tcn36_CN36Error::CN36ErrorDescription * tcn36_CN36Error::FindThisErrorInDescriptonArray(tcn36_CN36Error::CN36ErrorCase CaseToSet)
{
    tcn36_CN36Error::CN36ErrorDescription * rc;

    if(CN36ErrorToText[int(CaseToSet)].Case==CaseToSet)
        rc=CN36ErrorToText+int(CaseToSet);
    else
    {
        rc=CN36ErrorToText;

        while(rc->Case!=CaseToSet && rc->Case!=CaseInternalError)
            rc++;
    }

    return rc;
}

// -----------------------------------------------------------------------------
// functions of hcn36.h
// -----------------------------------------------------------------------------

bool cn36_StrAlloc(char * & Dest, size_t Length)
{
    bool rc=true;

    if(Dest!=0)
        delete [] Dest;

    Dest=new char [Length+1];

    if(0==Dest)
    {
        rc=false;
        ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseOutOfMemory));
    }
    else
        Dest[0]='\0';

    return rc;
}

bool cn36_StrAllocCpy(char * & Dest, const char *Src)
{
    bool rc=cn36_StrAlloc(Dest, strlen(Src));

    if(rc)
        SAPDB_strcpy(Dest, Src);

    return rc;
}

void cn36_StrNCpy(char * Dest, const char *Src, size_t Length)
{
    strncpy(Dest, Src, Length);
    Dest[Length]='\0';
}

void cn36_StrNCat(char * Dest, const char *Src, size_t Length)
{
    size_t Used=strlen(Dest);

    if(Length>Used)
        cn36_StrNCpy(Dest+Used, Src, Length-Used);
}

bool cn36_StrNAllocCpy(char * & Dest, const char *Src, size_t Length)
{
    bool rc=cn36_StrAlloc(Dest, Length);

    if(rc)
        cn36_StrNCpy(Dest, Src, Length);

    return rc;
}

void cn36_StrDealloc(char * & String)
{
    if(0!=String)
    {
        delete [] String;
        String=0;
    }
}


void cn36_StrUpr(char *String)
{
    #if defined (WIN32)
    _strupr(String);
    #elif defined (UNIX)
    while('\0'!= *String)
    {
        *String=toupper(*String);
        String++;
    }
    #else
        #error cn36_StrUpr() was only coded for UNIX and NT (define one of them).
    #endif
}

class tcn36_ADigitToIntMap
{
    public:
        enum MapType{NonDigitsZero, NonDigitsNegativ};

        tcn36_ADigitToIntMap(MapType Type)
        {
            int i;
            int NonDigitValue;

            if(Type==NonDigitsZero)
                NonDigitValue=0;
            else
                NonDigitValue=-1;

            for(i=0; i<=UCHAR_MAX; i++)
                Map[i]=NonDigitValue;

            Map[(unsigned char)'0']=0;
            Map[(unsigned char)'1']=1;
            Map[(unsigned char)'2']=2;
            Map[(unsigned char)'3']=3;
            Map[(unsigned char)'4']=4;
            Map[(unsigned char)'5']=5;
            Map[(unsigned char)'6']=6;
            Map[(unsigned char)'7']=7;
            Map[(unsigned char)'8']=8;
            Map[(unsigned char)'9']=9;
        }

        int operator[](unsigned char c) const
        {
            return Map[c];
        }

    private:
        int Map[int(UCHAR_MAX)+int(1)];
};

static tcn36_ADigitToIntMap DigitToIntMap_cn36(tcn36_ADigitToIntMap::NonDigitsZero);

static const char IntToCharMap_cn36[10]={'0','1','2','3','4','5','6','7','8','9'};

//converts Length chars from Buf to a I with MaxI as maximal possible value.
//returns 1 for success and 0 otherwise
template<class I> int cn36_StrTo(I & TheI, const char *Buf, size_t Length, I MaxI)
{
    int rc=1;

    const I MaxValueBeforeMultiply=MaxI/10;
    size_t Position=0;

    TheI=0;

    while(Position<Length)
    {
        if(isdigit(Buf[Position]))
            if(TheI<=MaxValueBeforeMultiply)
            {
                TheI*=10;
                I NextDigit=I(DigitToIntMap_cn36[Buf[Position]]);  // convert the character to an int (0..9)

                if(MaxI-NextDigit>=TheI)    //test TheI+NextDigit<=MaxI without risking an overflow
                    TheI+=NextDigit;
                else
                    rc=0;
            }
            else
                rc=0;

        Position++;
    }

    return rc;
}

//does the same as cn36_StrTo (converting a string into the number type I) but does not
//ignore signs
template<class I> int cn36_StrToSigned(I & TheI, const char *Buf, size_t Length, I MaxI, I MinI)
{
    int rc=1;

    const I MaxValueBeforeMultiply=MaxI/10;
    const I MinValueBeforeMultiply=MinI/10;
    int   IsPositive=1;
    int   FoundFirstDigit=0;
    size_t Position=0;

    TheI=0;

    while(Position<Length)
    {
        if(isdigit(Buf[Position]))
        {
            FoundFirstDigit=1;

            if((IsPositive && TheI<=MaxValueBeforeMultiply) ||
               (!IsPositive && TheI>=MinValueBeforeMultiply)   )
            {
                TheI*=10;
                I NextDigit=I(DigitToIntMap_cn36[Buf[Position]]);  // convert the character to an int (0..9)

                if(IsPositive)
                {
                    if(MaxI-NextDigit>=TheI)    //test MinI<=TheI+NextDigit<=MaxI without risking an overflow
                        TheI+=NextDigit;
                    else
                        rc=0;
                }
                else    // !Positive
                {
                    if(MinI+NextDigit<=TheI)    //test MinI<=TheI+NextDigit<=MaxI without risking an overflow
                        TheI-=NextDigit;
                    else
                        rc=0;
                }
            }
            else
                rc=0;
        }
        else
            if('-'==Buf[Position] && !FoundFirstDigit)
                IsPositive=(!IsPositive);

        Position++;
    }

    return rc;
}

int cn36_StrToInt(int & TheInt, const char *Buf)
{
    #if defined(WIN32)
    return cn36_StrToSigned<int>(TheInt, Buf, strlen(Buf), INT_MAX, INT_MIN);
    #elif defined (UNIX)
    return cn36_StrToSigned(TheInt, Buf, strlen(Buf), INT_MAX, INT_MIN); // the SNI-C++ compilers can not handle StrTo<int>
    #else
        #error cn36_StrToInt is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrNToInt(int & TheInt, const char *Buf, size_t Length)
{
    #if defined(WIN32)
    return cn36_StrToSigned<int>(TheInt, Buf, Length, INT_MAX, INT_MIN);
    #elif defined (UNIX)
    return cn36_StrToSigned(TheInt, Buf, Length, INT_MAX, INT_MIN);  // the SNI-C++ compilers can not handle StrTo<int>
    #else
        #error cn36_StrNToInt is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrToUInt(unsigned int & TheUInt, const char * Buf)
{
    #if defined(WIN32)
    return cn36_StrTo<unsigned int>(TheUInt, Buf, strlen(Buf), UINT_MAX);
    #elif defined (UNIX)
    return cn36_StrTo(TheUInt, Buf, strlen(Buf), UINT_MAX); // the SNI-C++ compilers can not handle StrTo<unsigned int>
    #else
        #error cn36_StrToUInt is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrNToUInt(unsigned int & TheUInt, const char * Buf, size_t Length)
{
    #if defined(WIN32)
    return cn36_StrTo<unsigned int>(TheUInt, Buf, Length, UINT_MAX);
    #elif defined (UNIX)
    return cn36_StrTo(TheUInt, Buf, Length, UINT_MAX);  // the SNI-C++ compilers can not handle StrTo<unsigned int>
    #else
        #error cn36_StrNToUInt is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrToLong(long & TheLong, const char *Buf)
{
    #if defined(WIN32)
    return cn36_StrToSigned<long>(TheLong, Buf, strlen(Buf), LONG_MAX, LONG_MIN);  
    #elif defined (UNIX)
    return cn36_StrToSigned(TheLong, Buf, strlen(Buf), /*long(*/LONG_MAX/*)*/, LONG_MIN);  // the SNI-C++ compilers can not handle StrToSigned<long>, long() is needed by sun64
    #else
        #error cn36_StrNToLong is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrNToLong(long & TheLong, const char *Buf, size_t Length)
{
    #if defined(WIN32)
    return cn36_StrToSigned<long>(TheLong, Buf, Length, LONG_MAX, LONG_MIN);
    #elif defined (UNIX)
    return cn36_StrToSigned(TheLong, Buf, Length, /*long(*/LONG_MAX/*)*/, LONG_MIN);  // the SNI-C++ compilers can not handle StrToSigned<long>, long() is needed by sun64
    #else
        #error cn36_StrNToLong is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrToInt8(teo00_Int8 & TheInt8, const char *Buf)
{
    #if defined(WIN32)
    return cn36_StrToSigned<teo00_Int8>(TheInt8, Buf, strlen(Buf), MAX_INT8_CN36, MIN_INT8_CN36);
    #elif defined (UNIX)
    return cn36_StrToSigned(TheInt8, Buf, strlen(Buf), MAX_INT8_CN36, MIN_INT8_CN36);   // the SNI-C++ compilers can not handle cn36_StrToSigned<teo00_Int8>
    #else
        #error cn36_StrToInt8 is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrNToInt8(teo00_Int8 & TheInt8, const char *Buf, size_t Length)
{
    #if defined(WIN32)
    return cn36_StrToSigned<teo00_Int8>(TheInt8, Buf, Length, MAX_INT8_CN36, MIN_INT8_CN36);
    #elif defined (UNIX)
    return cn36_StrToSigned(TheInt8, Buf, Length, MAX_INT8_CN36, MIN_INT8_CN36); // the SNI-C++ compilers can not handle cn36_StrToSigned<teo00_Int8>
    #else
        #error cn36_StrNToInt8 is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrToUInt8(teo00_Uint8 & TheUInt8, const char * Buf)
{
    #if defined(WIN32)
    return cn36_StrTo<teo00_Uint8>(TheUInt8, Buf, strlen(Buf), MAX_UINT8_CN36);
    #elif defined (UNIX)
    return cn36_StrTo(TheUInt8, Buf, strlen(Buf), MAX_UINT8_CN36); // the SNI-C++ compilers can not handle cn36_StrTo<teo00_Uint8>
    #else
        #error cn36_StrToUInt8 is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_StrNToUInt8(teo00_Uint8 & TheUInt8, const char * Buf, size_t Length)
{
    #if defined(WIN32)
    return cn36_StrTo<teo00_Uint8>(TheUInt8, Buf, Length, MAX_UINT8_CN36);
    #elif defined (UNIX)
    return cn36_StrTo(TheUInt8, Buf, Length, MAX_UINT8_CN36);   // the SNI-C++ compilers can not handle cn36_StrTo<teo00_Uint8>
    #else
        #error cn36_StrNToUInt8 is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_UInt8ToStrN(char *Buf, size_t Length, teo00_Uint8 i)
{
    int rc=1;

    char   String[100];
    size_t Pos=0;

    if(0==i)
        String[Pos++]='0';
    else
    {
        while(i>0)
        {
            String[Pos]=IntToCharMap_cn36[int(i%10)];
            Pos++;
            i/=10;
        }
    }

    if(Pos>Length-1)    // -1 for terminating '\0'
    {
        rc=0;         // Buf to small
        if(Length>0)
            Buf[0]='\0';
    }
    else
    {
        size_t Pos2=0;

        while(Pos>0)
        {
            Buf[Pos2]=String[Pos-1];
            Pos--;
            Pos2++;
        }

        Buf[Pos2]='\0';
    }

    return rc;
}

int cn36_IntToStrN(char *Buf, size_t Length, int i)
{
    int rc=1;

    char   String[100];
    size_t Pos=0;
    int    SmallerZero=0;

    if(i<0)
    {
        SmallerZero=1;
        i=-i;
    }

    if(0==i)
        String[Pos++]='0';
    else
    {
        while(i>0)
        {
            String[Pos]=IntToCharMap_cn36[int(i%10)];
            Pos++;
            i/=10;
        }
    }

    if((SmallerZero+Pos)>Length-1)    // -1 for terminating '\0'
    {
        rc=0;         // Buf to small

        if(Length>0)
            Buf[0]='\0';
    }
    else
    {
        size_t Pos2=0;

        if(SmallerZero)
            Buf[Pos2++]='-';

        while(Pos>0)
        {
            Buf[Pos2]=String[Pos-1];
            Pos--;
            Pos2++;
        }

        Buf[Pos2]='\0';
    }

    return rc;
}


int cn36_StrNConsistsOfDigits(const char *String, size_t Length)
{
    int rc=1;
    size_t i;

    if(0!=String)
    {
        for(i=0; rc && i<Length && '\0'!=String[i]; i++)
            if(!isdigit(String[i]))
                rc=0;
    }

    return rc;
}

int  cn36_StrConsistsOfDigits(const char *String)
{
    int rc=1;

    if(0!=String)
    {
        size_t i=0;

        while('\0'!=String[i] && rc)
        {
            if(!isdigit(String[i]))
                rc=0;

            i++;
        }
    }

    return rc;
}

const char * cn36_FirstNonDigitOf(const char * string, size_t stringLength)
{
    size_t i=0;

    while(i<stringLength && '\0'!=(*string) && isdigit(*string))
    {
        ++i;
        ++string;
    }

    return string;
}

char * cn36_TerminatingZeroOf(char * s)
{
    return s+strlen(s);
}

const char * cn36_TerminatingZeroOf(const char * s)
{
    return s+strlen(s);
}


char * cn36_FirstNonWhiteSpaceOf(char * s)
{
    while('\0'!=*s && isspace(*s))
        s++;

    return s;
}

const char * cn36_FirstNonWhiteSpaceOf(const char * s)
{
    while('\0'!=*s && isspace(*s))
        s++;

    return s;
}

char * cn36_FirstWhiteSpaceOf(char * s)
{
    while('\0'!=*s && !isspace(*s))
        s++;

    return s;
}

const char * cn36_FirstWhiteSpaceOf(const char * s)
{
    while('\0'!=*s && !isspace(*s))
        s++;

    return s;
}

const char * cn36_FirstNonWhiteSpaceOf(const char * String, size_t StringLength)
{
    size_t i=0;

    while(i<StringLength && '\0'!=*String && isspace(*String))
        i++,String++;

    return String;
}

const char * cn36_FirstWhiteSpaceOf(const char * String, size_t StringLength)
{
    size_t i=0;

    while(i<StringLength && '\0'!=*String && !isspace(*String))
        i++,String++;

    return String;
}

const char * cn36_StartOfTrailingWhiteSpaces(const char *s)
{
    const char *s1=s+strlen(s);

    while(s1!=s && isspace(*(s1-1)))
        s1--;

    return s1;
}

char * cn36_StartOfTrailingWhiteSpaces(char * s)
{
    char *s1=s+strlen(s);

    while(s1!=s && isspace(*(s1-1)))
        s1--;

    return s1;
}

char * cn36_RemoveOuterDoubleQuotes(char * s) // same as private function cn31_RemoveDoublequotes()
{
    size_t length=strlen(s);

    if(1<length && ('"'==s[0]) && ('"'==s[length-1]))   //at least two characters and first and last characters are double quotes
    {
        SAPDB_memmove(s, s+1, length-2); //copy character for character do not use strncpy, as source and destination overlap

        s[length-2]='\0';
    }

    return s;
}

char * cn36_RemoveSpaces(char * string)
{
    size_t pos=0;
    size_t copyPos=0;

    while('\0'!=string[pos])
    {
        if(!isspace(string[pos]))
            string[copyPos++]=string[pos];

        pos++;
    }

    string[copyPos]='\0';

    return string;
}

void cn36_GetStringBetweenDoubleQuotesOrWhiteSpaces(const char * & Start, const char * & End)
{
    End=Start;

    if(0!=Start)
    {
        int FoundValue=0;
        const char DoubleQuote='\"';

        if(DoubleQuote==Start[0])   //if the value starts with an '\"', we take all chars up to a second '\"'
        {
            End=strchr(Start+1, DoubleQuote);  //+1 we look for the second '\"' not for the one in Start[0]

            if(0!=End)   //if we found a secound '\"', take the string between these to '\"'
            {
                Start++; //start now behind the first '\"', End is already correct and even empty strings are OK
                FoundValue=1;
            }
        }

        if(!FoundValue)
            End=cn36_FirstWhiteSpaceOf(Start);
    }
}

const char * cn36_StrChr(const char *String, const char Character)
{
    if(0!=String)
        String=strchr(String, Character);

    return String;
}

char * cn36_StrChr(char *String, const char Character)
{
    if(0!=String)
        String=strchr(String, Character);

    return String;
}

const char * cn36_StrNChr(const char *String, const char Character, size_t Length)
{
    size_t Pos=0;

    while(Pos<Length && String[Pos]!=Character)
        Pos++;

    if(Pos>=Length)
        return 0;
    else
        return String+Pos;
}

char * cn36_StrNChr(char *String, const char Character, size_t Length)
{
    size_t Pos=0;

    while(Pos<Length && String[Pos]!=Character)
        Pos++;

    if(Pos>=Length)
        return 0;
    else
        return String+Pos;
}

const char * cn36_StrNRChr(const char *string, size_t length, const char character)
{
    while(0<length && string[length-1]!=character)
        length--;
    
    if(0<length)
        return string+(length-1);
    else
        return 0;
}

const char * cn36_LoopedStrChr(const char *String, char Character, int Number)
{
    if(0!=String && 0<Number)
    {
        String=strchr(String, Character);
        Number--;
    }

    while(0!=String && 0<Number)
    {
        String=strchr(String+1, Character); // String+1, because String points already to 'Character' from the previous strchr
        Number--;
    }

    return String;
}

char * cn36_LoopedStrChr(char *String, char Character, int Number)
{
    if(0!=String && 0<Number)
    {
        String=strchr(String, Character);
        Number--;
    }

    while(0!=String && 0<Number)
    {
        String=strchr(String+1, Character); // String+1, because String points already to 'Character' from the previous strchr
        Number--;
    }

    return String;
}

const char * cn36_LoopedStrNChr(const char * string, size_t length, char character, int number)
{
    size_t remainingLength=length;
    const char * pos=string;

    if(0==remainingLength || 0>=number)
        pos=0;

    if(0!=pos)
    {
        pos=cn36_StrNChr(pos, character, remainingLength);
        number--;

        if(0!=pos)
            remainingLength=length-(pos-string);
    }

    while(0!=pos && 0<number && 0<remainingLength)
    {
        pos=cn36_StrNChr(pos+1, character, remainingLength-1); // pos+1, because pos points already to 'character' from the previous strchr
        number--;

        if(0!=pos)
            remainingLength=length-(pos-string);
    }

    return pos;
}

const char * cn36_StrNStr(const char *string1, size_t lengthString1, const char * string2)
{
    const char * rc=0;
    size_t posS1=0;
    size_t posS2=0;
    size_t lengthString2=strlen(string2);

    while(0==rc && posS1<lengthString1 && '\0'!=string1[posS1])
    {
        if((lengthString1-posS1)>=lengthString2 && 0==strncmp(string1+posS1, string2, lengthString2))
            rc=string1+posS1;

        posS1++;
    }

    return rc;
}

char * cn36_StrNStr(char *string1, size_t lengthString1, const char * string2)
{
    char * rc=0;
    size_t posS1=0;
    size_t posS2=0;
    size_t lengthString2=strlen(string2);

    while(0==rc && posS1<lengthString1 && '\0'!=string1[posS1])
    {
        if((lengthString1-posS1)>=lengthString2 && 0==strncmp(string1+posS1, string2, lengthString2))
            rc=string1+posS1;

        posS1++;
    }

    return rc;
}

const char * cn36_StrRStr(const char * string, const char * stringToFind)
{
    bool         loopAgain=true;
    const char * rc=0;
    size_t       pos=strlen(string); //we start at the terminating zero byte of string
    size_t       lengthStringToFind=strlen(stringToFind);

    while(loopAgain)
    {
        if(0==strncmp(string+pos, stringToFind, lengthStringToFind)) //do we have a match at current position pos?
        {
            rc=string+pos;
            loopAgain=false;
        }

        if(0==pos)
            loopAgain=false;

        pos--;
    }

    return rc;
}

int cn36_StrStrNCmp(const char * String1, const char *String2, size_t Length2)
{
    int rc=strncmp(String1, String2, Length2);

    if(0==rc)
    {
        size_t Length1=strlen(String1);

        if(Length1>Length2)
            rc=1;
    }

    return rc;
}

int cn36_StrNUprCmp(const char *String1, const char *String2, size_t Length)
{
    int rc=0;
    size_t Pos=0;

    while(0==rc && Pos<Length && '\0'!=String2[Pos] && '\0'!=String1[Pos])
    {
        int S1=toupper(String1[Pos]),
            S2=toupper(String2[Pos]);

        if(S1<S2)
            rc=-1;
        else
            if(S1>S2)
                rc=1;

        Pos++;
    }

    if(0==rc && Pos<Length) //reached the end of one string. up to here both strings are equal
    {
        if('\0'==String1[Pos]) // at end of String1
        {
            if('\0'!=String2[Pos]) // String2 has not ended
                rc=-1;
        }
        else //at end of String2 (Pos==Length)||('\0'==String2[Pos])
            rc=1;   //String1 has not ended -> String2 must have ended (0==rc && Pos<Length && '\0'!=String1[Pos])
    }

    return rc;
}

int cn36_StrStrNUprCmp(const char * String1, const char *String2, size_t Length2)
{
    int rc=cn36_StrNUprCmp(String1, String2, Length2);

    if(0==rc)
    {
        size_t Length1=strlen(String1);

        if(Length1>Length2)
            rc=1;
    }

    return rc;
}

int cn36_StrUprCmp(const char * String1, const char *String2)
{
    int rc=0;
    size_t Pos=0;

    do
    {
        int S1=toupper(String1[Pos]),
            S2=toupper(String2[Pos]);

        if(S1<S2)
            rc=-1;
        else
            if(S1>S2)
                rc=1;

        Pos++;
    }
    while(0==rc && '\0'!=String2[Pos] && '\0'!=String1[Pos]);   //repeat until strings differ or end of one string is reached

    if(0==rc &&                                     //if strings do not differ (so at least one of them has ended) and ...
       ('\0'!=String2[Pos] || '\0'!=String1[Pos]))  //... if not both strings ended
    {
        if('\0'==String1[Pos])  //String1 has ended -> String2 is longer
            rc=-1;
        else                    //String1 has not ended -> String2 must have ended -> String1 is longer
            rc=1;
    }

    return rc;
}

int cn36_StrStrSpaceCmp(const char * String1, const char *String2)
{
    int rc=strncmp(String1, String2, strlen(String1));

    if(0==rc && '\0'!=(*cn36_FirstNonWhiteSpaceOf(String2+strlen(String1))))    //if not just spaces are following in String2
        rc=1;                                                                   //string2 is longer

    return rc;
}

#if defined (WIN32)
const char PathDelimiter_cn36='\\';
#elif defined (UNIX)
const char PathDelimiter_cn36='/';
#else
    #error vcn36.cpp is only coded for WIN32 and UNIX (define one of them)
#endif

bool cn36_ConstructCompletePath(char * & CompletePath, const char * Directory, const char * FileName)
{
    bool rc=false;

    size_t LengthOfDirectory=strlen(Directory);

    if(cn36_StrAlloc(CompletePath, LengthOfDirectory+1+strlen(FileName)))   // resevere space for the directory, \ or / (only inserted if needed) and the file name
    {
        SAPDB_strcpy(CompletePath, Directory);    // path starts with directory

        if(0==LengthOfDirectory ||                                  // if the directory is empty or
           (0<LengthOfDirectory &&                                  // (if at least one char in Directory
            PathDelimiter_cn36!=CompletePath[LengthOfDirectory-1])) // and last char is not \  or / respectivly)
        {
            CompletePath[LengthOfDirectory]=PathDelimiter_cn36; // append a \ or / respectivly
            ++LengthOfDirectory;
        }

        SAPDB_strcpy(CompletePath+LengthOfDirectory, FileName);

        rc=true;
    }

    return rc;
}

int cn36_IsAbsolutePath(const char * Path)
{
    int rc=0;

    #if defined (WIN32)
    if((strlen(Path)>=2 && isalpha(Path[0]) && ':'==Path[1]) ||                                         // starts with "<driveletter>:" or
       (strlen(Path)>=5 && '\\'==Path[0] && '\\'==Path[1] && 0!=strchr(Path+3, PathDelimiter_cn36)))    // with "\\<machine>\"
    {
        rc=1;
    }
    #elif defined (UNIX)
    if(strlen(Path)>=1 && PathDelimiter_cn36==Path[0])  //it starts with a "/"
        rc=1;
    #else
        #error cn36_IsAbsolutePath() is only coded for WIN32 and UNIX (define one of them)
    #endif

    return rc;
}

char MsgBuf_cn36[1024];

int cn36_AppendFileToFile(const char *Src, const char *Dest)
{
    int rc=1;

    tni34_AFile SrcFile(Src, tni34_AFile::FromDisk, 0, 1);

    if(SrcFile.IsGood())
    {
        tni34_AFile DestFile(Dest, tni34_AFile::ToDiskAppend, 1, 1);

        if(DestFile.IsGood())
        {
            long BytesRead, BytesWritten;
            char Buf[1024];

            while(rc && SrcFile.Read((void*)Buf, 1024, BytesRead) && BytesRead>0)
            {
                if(!DestFile.Write((void*)Buf, BytesRead, BytesWritten) || BytesWritten!=BytesRead) //TODO: use "quiet"-versions of Write and Read
                    rc=0;// report this error after the LogFile was closed, because it will be reported to the LogFile !
            }
        }
        else
            rc=0;

        if(!DestFile.Close())
            rc=0;
    }
    else
        rc=0;

    return rc;
}

int cn36_MoveFile(const char * Src, const char *Dest)
{
    int rc=1;

    tni34_AFile File(Src, tni34_AFile::FromDisk, 0, 0);  // do not create and do not open

    if(File.IsGood())
        rc=File.Rename(Dest);
    else
        rc=0;

    return rc;
}

const char * cn36_GetHostName()
{
    const char * rc;

    #if defined (WIN32)
    DWORD MaxNameLngth=MAX_COMPUTERNAME_LENGTH+1;
    static char ComputerName[MAX_COMPUTERNAME_LENGTH+1];

    if(GetComputerName(ComputerName, &MaxNameLngth))
        rc=ComputerName;
    else
    {
        rc=0;
        ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseCanNotGetComputerName, int(GetLastError())));
    }
    #elif defined (UNIX)
    static struct utsname UnameBuf;
    if(uname(&UnameBuf)>=0)
        rc=UnameBuf.nodename;
    else
    {
        rc=0;
        ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseCanNotGetComputerName, int(errno)));
    }
    #else
        #error  cn36_GetHostName() only coded for WIN32 and UNIX, define one of them
    #endif

    return rc;
}

const char * cn36_BaseNameFrom(const char *Path)
{
    const char * BaseName=strrchr(Path, PathDelimiter_cn36);

    if(0==BaseName)
        BaseName=Path;
    else
        BaseName++;

    return BaseName;
}

char * cn36_BaseNameFrom(char *Path)
{
    char * BaseName=strrchr(Path, PathDelimiter_cn36);

    if(0==BaseName)
        BaseName=Path;
    else
        BaseName++;

    return BaseName;
}

const char * cn36_EndOfDirectory(const char *Path)
{
    const char * End=strrchr(Path, PathDelimiter_cn36);

    if(0==End)
        End=Path;

    return End;
}

char * cn36_EndOfDirectory(char *Path)
{
    char * End=strrchr(Path, PathDelimiter_cn36);

    if(0==End)
        End=Path;

    return End;
}

const char * cn36_NextPathPart(const char *Path)
{
    return (PathDelimiter_cn36==Path[0])?(Path+1):Path;
}

int cn36_IsPathDelimiter(const char *Path)
{
    return (PathDelimiter_cn36==Path[0]);
}

int cn36_PathCmp(const char * Path1, const char *Path2)
{
    #if defined (WIN32)
    return cn36_StrUprCmp(Path1, Path2);
    #elif defined (UNIX)
    return strcmp(Path1, Path2);
    #else
        #error cn36_PathCmp is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_PathNCmp(const char * Path1, const char *Path2, size_t Length)
{
    #if defined (WIN32)
    return cn36_StrNUprCmp(Path1, Path2, Length);
    #elif defined (UNIX)
    return strncmp(Path1, Path2, Length);
    #else
        #error cn36_PathNCmp is only coded for WIN32 and UNIX (define one of them)
    #endif
}

int cn36_CheckExist(const char * FileName)
{
    int Exists=1;

#if defined (WIN32)
    if(-1==_access(FileName, F_OK))
        Exists=0;
#elif defined (UNIX)
    if(-1==access(FileName, F_OK))
        Exists=0;
#else
#error vcn36.cpp only coded for WIN32 and UNIX, define one of them
#endif

    return Exists;
}

int cn36_CheckRead(const char * pszFileName)
{
    int IsReadable=1;

#if defined (WIN32)
    if( -1==_access(pszFileName, R_OK))
      IsReadable=0;
#elif defined (UNIX)
    if( -1==access(pszFileName, R_OK))
      IsReadable=0;
#else
#error vcn36.cpp only coded for WIN32 and UNIX, define one of them
#endif

    return IsReadable;
}

int cn36_CheckExe(const char * pszProgramName)
{
    int IsExecutable=1;

#if defined (WIN32)
    struct _stat s_stat;

    if( -1==_stat(pszProgramName, &s_stat) || 0==(s_stat.st_mode & _S_IEXEC))
        IsExecutable=0;
#elif defined (UNIX)
    if( -1==access( pszProgramName, X_OK))
        IsExecutable=0;
#else
#error vcn36.cpp only coded for WIN32 and UNIX, define one of them
#endif

    return IsExecutable;
}

void cn36_Sleep(int Seconds)
{
    /*sprintf(MsgBuf_cn36,"Waiting %d second%s ... ", (int)Seconds, Seconds>1?"s":"");
    cn36_Verbose(MsgBuf_cn36);*/

    #if defined (WIN32)
    Sleep(DWORD(Seconds)*1000);
    #elif defined (UNIX)
    sleep(Seconds);
    #else
        #error vcn36.cpp only coded for UNIX and WIN32 (define one of them)
    #endif

    /*cn36_Verbose("Done.\n", NoPrefix);*/
}

long cn36_GetCurrentTime()
{
    return (long)time(0);
}

long cn36_SecondsBetween(long time1, long time2)
{
    long rc=time2-time1;

    if(time1>time2)
        rc= -rc;

    return rc;
}


int cn36_CompareEnvVarNames(const char * EnvironmentVariable1, const char * EnvironmentVariable2)
{
    #if defined(WIN32)
    return cn36_StrUprCmp(EnvironmentVariable1, EnvironmentVariable2);  //environment variables names (not values) are caseinsensitive on NT
    #elif defined(UNIX)
    return strcmp(EnvironmentVariable1, EnvironmentVariable2);  //environment variables names are casesensitiv on UNIX
    #else
        #error cn36_CompareEnvVarNames() only coded for WIN32 and UNIX, define one of them
    #endif
}

const char * cn36_GetEnv(const char * cpszEnvironmentVariable)
{
    return getenv(cpszEnvironmentVariable);
}

static ni31_List<char *> ListOfUsedEnvironmentStrings_cn36;

int cn36_PutEnv(const char * VariableNameAndValue) 
{
    int    rc=1;
    char * TmpString=0;

    if(!cn36_StrAlloc(TmpString, strlen(VariableNameAndValue))) 
        rc=0;
    else
    {
        strcpy(TmpString, VariableNameAndValue);
        char * pAssign=strchr(TmpString, '=');
        if (0!=pAssign)
        {
            *pAssign=0;
            ++pAssign;
        }
        rc=cn36_PutEnv(TmpString,(0==pAssign)?"":pAssign);
        cn36_StrDealloc(TmpString);  // free the memory
    }
    
    return rc;
} 

int cn36_PutEnv(const char * VariableName,
                const char * VariableValue)
{
    int    rc=1;
    char * NewEnvString=0;

    if(!cn36_StrAlloc(NewEnvString, strlen(VariableName)+1+strlen(VariableValue)))  // allocate space for new part of environment
        rc=0;
    else
    {
        sprintf(NewEnvString, "%s=%s", VariableName, VariableValue);

        #if defined(WIN32)
        rc=!_putenv(NewEnvString);
        #elif defined(UNIX)
        rc=!putenv(NewEnvString);
        #else
            #error cn36_PutEnv() only coded for WIN32 and UNIX, define one of them
        #endif

        if(!rc) // could not make NewEnvString a part of the environment
        {
            cn36_StrDealloc(NewEnvString);  // free the memory
            ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseCanNotExportEnvironment, NewEnvString));
        }
        else    // the (*NewEnvString) is now part of the environment, (*NewEnvString) must not be freed as long as it remains in is the environment!!
        {
            ni31_ListElem<char *> *h=ListOfUsedEnvironmentStrings_cn36.First();
            int FoundInList=0;

            while(0!=h && !FoundInList) //look through the list of former parts of the environment (put by this function)
            {
                if(0==strncmp(h->GiveInfo(), VariableName, strlen(VariableName)) && //the environment allready contained a string for that variable
                   '='==h->GiveInfo()[strlen(VariableName)])
                {
                    char * OldEnvironmentString=h->GiveInfo();  // get the old string
                    cn36_StrDealloc(OldEnvironmentString);      //free the memory of the old string (as it is not longer used by the environment)

                    (*h)=NewEnvString; //place the new string in the list

                    FoundInList=1;
                }

                h=h->Next();
            }

            if(!FoundInList)                                                // if the variable was puted for the first time to the environment
                if(0==ListOfUsedEnvironmentStrings_cn36.Add(NewEnvString))  //add the string to the list
                {
                    rc=0;
                    ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseOutOfMemory));
                }
        }
    }

    return rc;
}

// global pointer to the process environment
extern char **environ;

void cn36_UnsetEnv(const char * VariableName)
{
    char * OldEnvString=0;

    // remove pointer from process environment   
    int nSource = 0;
    int nTarget = 0;
    if (0!=environ) 
    {
        while (0!=environ[nSource]) 
        {
            environ[nTarget] = environ[nSource];
            if(0==strncmp(environ[nTarget], VariableName, strlen(VariableName)) && //the environment allready contained a string for that variable
              '='==environ[nTarget][strlen(VariableName)])
                OldEnvString=environ[nTarget]; // store old pointer for deletion
            else
                ++nTarget; // increment target only if is not the element to remove
            ++nSource;
        } // end while
        environ[nTarget]=0;
    } // end if

    // remove from our environment string list
    ni31_ListElem<char *> *Element=ListOfUsedEnvironmentStrings_cn36.First();
    ni31_ListElem<char *> *Successor=NULL;
    int FoundInList=0;
    while(0!=Element && !FoundInList) //look through the list of former parts of the environment (put by this function)
    {
        if(Element->GiveInfo() == OldEnvString) // is it from us?
        {
          if (NULL!=Successor)
            ListOfUsedEnvironmentStrings_cn36.Del(*Successor); // remove element from list
          else
            ListOfUsedEnvironmentStrings_cn36.Del();           // remove element from list
          cn36_StrDealloc(OldEnvString);             //free the memory of the old string (as it is not longer used by the environment)
          FoundInList=1;
        }
        Successor=Element;
        Element=Element->Next();
    }

} // end cn36_UnsetEnv

tcn36_DateTimeFormat::tcn36_DateTimeFormat()
    :FormatString(0)
{
    (*this)=""; //use the redefined operator = (const char *)
}

tcn36_DateTimeFormat::tcn36_DateTimeFormat(const char * TheFormatString)
    :FormatString(0)
{
    (*this)=TheFormatString; //use the redefined operator = (const char *)
}

tcn36_DateTimeFormat::~tcn36_DateTimeFormat()
{
    cn36_StrDealloc(FormatString);
}

void tcn36_DateTimeFormat::operator=(const char * NewFormatString)
{
    int i;

    IsGoodFlag=1,
    IsValidFlag=0;

    for(i=0; i<Nothing; i++)
    {
        Position[i]=Digits[i]=RelativePositionToGap[i]=0;
        Type[i]=Nothing;
        GapsInFront[i]=0;
        Delimiter[i]=NoDelimiter;
    }

    if(0!=NewFormatString && '\0'!=NewFormatString[0])
    {
        IsGoodFlag=cn36_StrAllocCpy(FormatString, NewFormatString);

        if(IsGoodFlag)
        {
            size_t   CurPos=0;
            PartType CurType=Nothing;
            size_t   AMPMDigits=0;
            AMPMType AMPMFlag;
            int GapsInFrontOfNextPart=0,
                RelativePositionToGapOfNextPart=0;

            int CurPart=0;  //a part is in the following something of the form "...<spaces><nonspaces><year|month...>"

            int PrevState=0,    // 0==last analyzed character was not a placeholder and no space, 1== last char was a space, 2== last char was part of a placeholder (yyyy, mm, ...)
                NewState;

            while('\0'!=FormatString[CurPos] && CurPart<Nothing)
            {
                switch(FormatString[CurPos])
                {
                    case 'y': CurType=Year;       break;
                    case 'm': CurType=Month;      break;
                    case 'd': CurType=Day;        break;
                    case 'H': CurType=Hour;       break;
                    case 'M': CurType=Minute;     break;
                    case 'S': CurType=Second;     break;
                    case 'A':
                    case 'a':
                    case 'P':
                    case 'p':
                    {
                        AMPMFlag=StartsWithAMPM(FormatString+CurPos, AMPMDigits);

                        if(None!=AMPMFlag)
                            CurType=AMPM;
                    }break;
                    default : CurType=Nothing;    break;
                }

                //correct the type of the current state
                if(Nothing!=CurType && 0==Digits[CurType])  //found part and found it for the first time
                    NewState=2;
                else
                    CurType=Nothing; //if we found a part for a second time, ignore it

                if(Nothing==CurType)
                {
                    if(isspace(FormatString[CurPos]))
                        NewState=1;
                    else
                        NewState=0;
                }

                //do what is necessary for a transition between the states
                if(0==NewState)
                {
                    if(0==PrevState && 0!=CurPos)   //from 0 to 0 -> a normal character is following a normal character and it's not a the first one (we initialized PrevState with 0)
                        Digits[FixedEnd]++;         //just one more character for the FixedEnd

                    if(1==PrevState || 2==PrevState || (0==PrevState && 0==CurPos))    //character follows a gap or a place holder or we have found a new FixedEnd
                    {
                        Position[FixedEnd]=CurPos;      //throw away information about a previous one by initializing all relevant variables
                        Digits[FixedEnd]=1;
                        RelativePositionToGap[FixedEnd]=0;  //Fixed ends can not have a relative position
                        Type[FixedEnd]=FixedEnd;

                        if(1==PrevState)                                    //if a Fixed end follows a gap
                            GapsInFront[FixedEnd]=GapsInFrontOfNextPart;    //the previous gaps count
                        else
                            GapsInFront[FixedEnd]=0;                        //otherwise they must be ignored

                        Delimiter[FixedEnd]=NoDelimiter;
                    }

                    if(2==PrevState)    //from 2 to 0 -> a normal character is following a placeholder
                    {
                        if(isdigit(FormatString[CurPos]))   //if this normal charcter ...
                            Delimiter[CurPart]=NoDelimiter; //...is a digit, the part is delimited by fixed length
                        else
                            Delimiter[CurPart]=NonDigit;    //...is no digit, the placeholder is delimited by the character

                        CurPart++;
                    }

                    if(CurPart<FixedEnd)                    //we have found a normal character, so if another part could follow
                        RelativePositionToGapOfNextPart++;  //that character add to the relative position of that part
                }

                if(1==NewState)
                {
                    if(0==PrevState)    //from 0 to 1 -> a space is following normal character
                    {
                        GapsInFrontOfNextPart++;             //this space starts a new gap in front of the next part (if one is coming)
                        RelativePositionToGapOfNextPart=0;   //all previous normal character do not count for the next part (as there is a new gap)

                        Delimiter[FixedEnd]=Space;  //the fixed end (ending with the previous normal character) is delimited by the space
                    }

                    //if(1==PrevState) do nothing as a white space following a white space is just the same gap

                    if(2==PrevState)    //from 2 to 1 -> a space is following a placeholder
                    {
                        Delimiter[CurPart]=Space;   //previous part is delimited by a space
                        CurPart++;                  

                        GapsInFrontOfNextPart++;    //equals here a =1, last was a placeholder, so it's the first gap for the next part
                    }
                }

                if(2==NewState)
                {
                    if(0==PrevState || 1==PrevState) //from 0,1 to 2 -> found a new placeholder
                    {                                //therefore a previous fixed end is invalidated
                        Position[FixedEnd]=0;        //assure that this information is lost
                        Digits[FixedEnd]=0;
                        RelativePositionToGap[FixedEnd]=0;
                        Type[FixedEnd]=Nothing;
                        GapsInFront[FixedEnd]=0;
                        Delimiter[FixedEnd]=NoDelimiter;
                    }

                    if(2==PrevState)    //from 2 to 2 -> a new placeholder, directly behind another placeholder (yyyymm...)
                    {
                        Delimiter[CurPart]=NoDelimiter; //part is delimited by fixed length
                        CurPart++; //this part is delimiting the previous part
                    }

                    GapsInFront[CurPart]=GapsInFrontOfNextPart;                     //we have a new part and we know, how many gaps are there
                    RelativePositionToGap[CurPart]=RelativePositionToGapOfNextPart; //we know the relative position

                    Position[CurType]=CurPos;                                       //the absolute position
                    Digits[CurType]=1;                                              //it has at least 1 digit
                    Type[CurPart]=CurType;                                          //and it has a type

                    GapsInFrontOfNextPart=0;            //the next part (if any exists) has no gap in front so far
                    RelativePositionToGapOfNextPart=0;  //and also no relative displacement

                    //go to the end of the newly found placeholder
                    if(AMPM==CurType)
                    {
                        CurPos+=(AMPMDigits-1); //skip the M from "AM" or "PM" to avoid conflicts with M for Minute (same with a.m., A.M., ...)
                        Digits[CurType]+=(AMPMDigits-1);    //adjust Digits[AMPM] accordingly
                    }
                    else
                        while(FormatString[CurPos+1]==FormatString[CurPos])
                        {
                            Digits[CurType]++;
                            CurPos++;
                        }
                }

                PrevState=NewState;
                CurPos++;
            }

            //if(Nothing==CurType && 0!=Digits[CurType] && CurPart<Nothing)
            if(0!=Digits[FixedEnd] && FixedEnd==Type[FixedEnd] && CurPart<FixedEnd) //if there is a fixed end and not all other parts are used...
            {                                                                       //...copy the part related information about fixed end to the current part
                RelativePositionToGap[CurPart]=RelativePositionToGap[FixedEnd];
                Type[CurPart]=FixedEnd;
                GapsInFront[CurPart]=GapsInFront[FixedEnd];
                Delimiter[CurPart]=Delimiter[FixedEnd];

                RelativePositionToGap[FixedEnd]=0; //and set the part related information of the last part to the defaults for signaling the part as unused
                Type[FixedEnd]=Nothing;
                GapsInFront[FixedEnd]=0;
                Delimiter[FixedEnd]=NoDelimiter;
            }

            if((2!=Digits[Year] && 4!=Digits[Year]) ||  //these constraints ure used by tcn36_DateTimeString::CreateDateTimeString()!
               2!=Digits[Month] ||
               2!=Digits[Day] ||
               2!=Digits[Hour] ||
               2!=Digits[Minute] ||
               2!=Digits[Second] ||
               (0!=Digits[AMPM] && 2!=Digits[AMPM] && 4!=Digits[AMPM]))
            {
                IsValidFlag=0;
                ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseUnknownDateTimeFormatString, NewFormatString));
            }
            else
                IsValidFlag=1;
        }
    }
}

char * tcn36_DateTimeFormat::CreateCopyOfString() const
{
    char * rc=0;

    if(0!=FormatString)
        cn36_StrAllocCpy(rc, FormatString);

    return rc;
}

const char * tcn36_DateTimeFormat::giveFormatString() const
{
    return (0==FormatString) ? "" : FormatString;
}

tcn36_DateTimeFormat::AMPMType tcn36_DateTimeFormat::StartsWithAMPM(const char *String, size_t & CharsUsedByAMPM)
{
    tcn36_DateTimeFormat::AMPMType rc=None;
    CharsUsedByAMPM=2;

    if(0==cn36_StrNUprCmp("AM", String, strlen("AM")))
        rc=AM;
    else
        if(0==cn36_StrNUprCmp("PM", String, strlen("PM")))
            rc=PM;
        else
        {
            CharsUsedByAMPM=4;

            if(0==cn36_StrNUprCmp("A.M.", String, strlen("A.M.")))
                rc=AM;
            else
                if(0==cn36_StrNUprCmp("P.M.", String, strlen("P.M.")))
                    rc=PM;
                else
                    CharsUsedByAMPM=0;
        }

    return rc;
}

int tcn36_DateTimeFormat::SetAMPM(AMPMType AMPMFlag, char * String) const
{
    int rc=1;

    if(0!=FormatString && IsValidFlag && 0==Digits[AMPM] && 0!=String && '\0'!=String[0])
    {
        int SmallLetter=0;

        if('a'==(*(FormatString+Position[AMPM])) || 'p'==(*(FormatString+Position[AMPM])))
            SmallLetter=1;

        switch(AMPMFlag)
        {
            case AM:
            {
                if(SmallLetter)
                    String[0]='a';
                else
                    String[0]='A';
            }break;

            case PM:
            {
                if(SmallLetter)
                    String[0]='p';
                else
                    String[0]='P';
            }break;

            default:
                ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseInternalError));
        }
    }
    else
        rc=0;

    return rc;
}


tcn36_DateTimeString::tcn36_DateTimeString(void)
{
    time_t ltime;
    struct tm * stime;

    time(&ltime);               //get current date and time
    stime=localtime(&ltime);    //convert according to timezone

    SetMonthDaySecondsTo(1900+stime->tm_year, 1+stime->tm_mon, stime->tm_mday, stime->tm_hour, stime->tm_min, stime->tm_sec);  //pay attention to the rules for a struct tm
}

tcn36_DateTimeString::tcn36_DateTimeString(int i)
{
    SetMonthDaySecondsTo(0, 1, 1, 0, 0, 0);   //set to 1.1.0000 at 00:00:00
}

tcn36_DateTimeString::tcn36_DateTimeString(const char * String, const tcn36_DateTimeFormat & Format)
{
    SetTo(String, Format);
}

tcn36_DateTimeString::tcn36_DateTimeString(const char * String, const tcn36_DateTimeFormat & Format, const char * & EndOfDateTime)
{
    SetTo(String, Format, EndOfDateTime);
}

void tcn36_DateTimeString::SetTo()
{
    time_t ltime;
    struct tm * stime;

    time(&ltime);               //get current date and time
    stime=localtime(&ltime);    //convert according to timezone

    SetMonthDaySecondsTo(1900+stime->tm_year, 1+stime->tm_mon, stime->tm_mday, stime->tm_hour, stime->tm_min, stime->tm_sec);  //pay attention to the rules for a struct tm
}

void tcn36_DateTimeString::SetTo(const char * String, const tcn36_DateTimeFormat & Format)
{
    const char * EndOfDateTime=0;

    SetTo(String, Format, EndOfDateTime);
}

void tcn36_DateTimeString::SetTo(const char * String, const tcn36_DateTimeFormat & Format, const char * & EndOfDateTime)
{
    int  Parts[tcn36_DateTimeFormat::Nothing]={0,1,1,0,0,0,0};   //set month and hour to 1

    int AllOk=1,
        i,j;

    size_t AMPMSize;
    tcn36_DateTimeFormat::AMPMType AMPMFlag=tcn36_DateTimeFormat::None;
    size_t LengthOfString=strlen(String);

    size_t UsedLength=0,
           PrevUsedLength=0;

    const char * PrevPos=String, * CurPos=String;

    for(i=0; tcn36_DateTimeFormat::Nothing!=Format.GiveType(i) && AllOk && i<tcn36_DateTimeFormat::Nothing; i++)
    {
        for(j=0; j<Format.GiveNumberOfGaps(tcn36_DateTimeFormat::PartType(i)); j++) //ignore the gaps in front of the next part
            CurPos=cn36_FirstNonWhiteSpaceOf(cn36_FirstWhiteSpaceOf(CurPos));

        if(Format.GiveDigits(Format.GiveType(i)))
        {
            UsedLength=(CurPos-String)+Format.GivePositionFromGap(i);

            if(tcn36_DateTimeFormat::AMPM==Format.GiveType(i))
            {
                AMPMFlag=tcn36_DateTimeFormat::StartsWithAMPM(String+UsedLength, AMPMSize); //determine if an am or pm is there

                if(tcn36_DateTimeFormat::None==AMPMFlag)   //no AM or PM was found in String
                {
                    CurPos=PrevPos;                         //ignore the whole part
                    UsedLength=PrevUsedLength;
                }
                else
                {
                    size_t CurLengthOfPart=AMPMSize;

                    if(tcn36_DateTimeFormat::NoDelimiter!=Format.GiveDelimiter(i))  //am/pm is delimited by a space or a NonDigit
                    {
                        while(isdigit(String[UsedLength+CurLengthOfPart]))
                            CurLengthOfPart++;
                    }

                    UsedLength+=CurLengthOfPart;
                    CurPos=String+UsedLength;
                }
            }
            else
                if(tcn36_DateTimeFormat::FixedEnd==Format.GiveType(i))
                {
                    size_t CurLengthOfPart=Format.GiveDigits(Format.GiveType(i));

                    if(LengthOfString<UsedLength+CurLengthOfPart)   //fixed ends are optional and can differ in length
                        UsedLength=LengthOfString;
                    else
                    {
                        if(tcn36_DateTimeFormat::NoDelimiter!=Format.GiveDelimiter(i))  //fixed end is delimited by a space
                        {
                            while(!isspace(String[UsedLength+CurLengthOfPart]))
                                CurLengthOfPart++;
                        }

                        UsedLength+=CurLengthOfPart;
                    }

                    CurPos=String+UsedLength;
                }
                else    //must be a part consisting of digits
                {
                    if(tcn36_DateTimeFormat::NoDelimiter==Format.GiveDelimiter(i))
                    {
                        if(LengthOfString<UsedLength+Format.GiveDigits(Format.GiveType(i)) ||
                           !cn36_StrNToInt(Parts[Format.GiveType(i)], String+UsedLength, Format.GiveDigits(tcn36_DateTimeFormat::PartType(i))))
                        {
                            AllOk=0;
                        }
                        else
                            UsedLength+=Format.GiveDigits(tcn36_DateTimeFormat::PartType(i));
                    }
                    else
                    {
                        if(LengthOfString>UsedLength)
                        {
                            size_t CurLengthOfPart=0;

                            while(isdigit(String[UsedLength+CurLengthOfPart]))  //not reached delimiter or '\0'
                                CurLengthOfPart++;

                            if(cn36_StrNToInt(Parts[Format.GiveType(i)], String+UsedLength, CurLengthOfPart))
                                UsedLength+=CurLengthOfPart;
                            else
                                AllOk=0;
                        }
                        else
                            AllOk=0;
                    }

                    if(AllOk)
                        CurPos=String+UsedLength;
                }
        }
        else
        {
            AllOk=0;
            ErrorHandler_ni35->Report(tcn36_CN36Error(tcn36_CN36Error::CaseInternalError));
        }

        PrevPos=CurPos;
        PrevUsedLength=UsedLength;
    }

    if(AllOk)
    {
        //deal with 2 digits for year
        if(2==Format.GiveDigits(tcn36_DateTimeFormat::Year))
        {
            int BaseYear=1900;

            if(Parts[tcn36_DateTimeFormat::Year]%100<30)
                BaseYear=2000;

            Parts[tcn36_DateTimeFormat::Year]=BaseYear+Parts[tcn36_DateTimeFormat::Year]%100;
        }

        //adjust hour for PM Flag
        if(tcn36_DateTimeFormat::PM==AMPMFlag && 0<=Parts[tcn36_DateTimeFormat::Hour] && Parts[tcn36_DateTimeFormat::Hour]<12)
           Parts[tcn36_DateTimeFormat::Hour]+=12;

        SetMonthDaySecondsTo(Parts[tcn36_DateTimeFormat::Year],
                             Parts[tcn36_DateTimeFormat::Month],
                             Parts[tcn36_DateTimeFormat::Day],
                             Parts[tcn36_DateTimeFormat::Hour],
                             Parts[tcn36_DateTimeFormat::Minute],
                             Parts[tcn36_DateTimeFormat::Second]);
    }
    else
        MonthDaySecond=0;

    EndOfDateTime=String+UsedLength;
}

int tcn36_DateTimeString::IsGood()
{
    return (0!=MonthDaySecond);
}

char * tcn36_DateTimeString::CreateDateTimeString(const tcn36_DateTimeFormat & Format) const
{
    char * rc=Format.CreateCopyOfString();

    if(0!=rc)
    {
        int Parts[1+(tcn36_DateTimeFormat::Second)]={0,1,1,0,0,0};   //set month and day to 1
        int i;

        Parts[tcn36_DateTimeFormat::Year]=Year;
        MonthDaySecondsToInts(Parts[tcn36_DateTimeFormat::Month],
                              Parts[tcn36_DateTimeFormat::Day],
                              Parts[tcn36_DateTimeFormat::Hour],
                              Parts[tcn36_DateTimeFormat::Minute],
                              Parts[tcn36_DateTimeFormat::Second]);

        if(0!=Format.GiveDigits(tcn36_DateTimeFormat::AMPM))
        {
            if(Parts[tcn36_DateTimeFormat::Hour]>=12)
            {
                Parts[tcn36_DateTimeFormat::Hour]-=12;
                Format.SetAMPM(tcn36_DateTimeFormat::PM, rc+Format.GivePosition(tcn36_DateTimeFormat::AMPM));
            }
            else
                Format.SetAMPM(tcn36_DateTimeFormat::AM, rc+Format.GivePosition(tcn36_DateTimeFormat::AMPM));
        }

        for(i=tcn36_DateTimeFormat::Year; i<=tcn36_DateTimeFormat::Second; i++)    //for all parts of a DateTimeString
            IntNCpy(rc+Format.GivePosition(tcn36_DateTimeFormat::PartType(i)), Parts[i], Format.GiveDigits(tcn36_DateTimeFormat::PartType(i))); //place the part in the string
    }

    return rc;
}

void tcn36_DateTimeString::SetMonthDaySecondsTo(int TheYear, int TheMonth, int TheDay, int TheHour, int TheMinute, int TheSecond)
{
    Year=TheYear;

    if(0<TheMonth && 12>=TheMonth &&
       0<TheDay && 31>=TheDay &&
       0<=TheHour && 23>=TheHour &&
       0<=TheMinute && 60>TheMinute &&
       0<=TheSecond && 60>TheSecond)
    {
        MonthDaySecond=TheMonth*MonthConversionFactor
                       +TheDay*DayConversionFactor
                       +TheHour*SecondsPerHour
                       +TheMinute*SecondsPerMinute
                       +TheSecond;
    }
    else
        MonthDaySecond=0;
}

void tcn36_DateTimeString::MonthDaySecondsToInts(int & TheMonth, int & TheDay, int & TheHour, int & TheMinute, int & TheSecond) const
{
    int h=MonthDaySecond;

    TheMonth=h/MonthConversionFactor;
    h%=MonthConversionFactor;

    TheDay=h/DayConversionFactor;
    h%=DayConversionFactor;

    TheHour=h/SecondsPerHour;
    h%=SecondsPerHour;

    TheMinute=h/SecondsPerMinute;
    TheSecond=h%SecondsPerMinute;
}

void tcn36_DateTimeString::IntNCpy(char * DestString, int TheInt, size_t Chars) const
{
    char Buffer[30];
    size_t Length;
    size_t CurPos=0;

    sprintf(Buffer, "%d", TheInt);
    Length=strlen(Buffer);

    while(Chars>Length) //create leading zeros
    {
        DestString[CurPos++]='0';
        Chars--;
    }

    strncpy(DestString+CurPos, Buffer+(Length-Chars), Chars); //place the integer itself, watch for Chars
}

int tcn36_DateTimeString::operator<=(const tcn36_DateTimeString & OtherDateTime) const
{ 
    return (Year<OtherDateTime.Year || (Year==OtherDateTime.Year && MonthDaySecond<=OtherDateTime.MonthDaySecond));
}

int tcn36_DateTimeString::operator==(const tcn36_DateTimeString & OtherDateTime) const
{
    return (Year==OtherDateTime.Year && MonthDaySecond==OtherDateTime.MonthDaySecond);
}

int tcn36_DateTimeString::operator!=(const tcn36_DateTimeString & OtherDateTime) const
{
    return (Year!=OtherDateTime.Year || MonthDaySecond!=OtherDateTime.MonthDaySecond);
}

int tcn36_DateTimeString::operator>(const tcn36_DateTimeString & OtherDateTime) const
{
    return (Year>OtherDateTime.Year || (Year==OtherDateTime.Year && MonthDaySecond>OtherDateTime.MonthDaySecond));
}

int tcn36_DateTimeString::MonthConversionFactor=10000000;
int tcn36_DateTimeString::DayConversionFactor=100000;
int tcn36_DateTimeString::SecondsPerHour=3600;
int tcn36_DateTimeString::SecondsPerMinute=60;

int tcn36_DateTimeString::MaxMonthDaySeconds=123186399; //used to generate compiler warnings if int is too small
