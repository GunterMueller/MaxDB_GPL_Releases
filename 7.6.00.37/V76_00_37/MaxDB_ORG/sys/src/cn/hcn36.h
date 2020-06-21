/*!
    \file    hcn36.h
    \author  TiloH
    \ingroup cn layer
    \brief   basic functions used in vcn35.cpp, ..., vcn355.cpp

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

#if !defined(_HCN36_H_)
#define _HCN36_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni00b.h"

#include<string.h>
#if defined (WIN32)
#include<windows.h>
#elif defined (UNIX)
#include<ctype.h>
#else
#error hcn36.h only coded for WIN32 and UNIX (define one of them).
#endif


#include "hni33.h"
#include "hni34.h"
#include "hni35.h"


// -----------------------------------------------------------------------------
// constants, types and function declarations
// -----------------------------------------------------------------------------

bool cn36_StrAlloc(char * & Dest, size_t Length);
bool cn36_StrAllocCpy(char * & Dest, const char * Src);
void cn36_StrNCpy(char * Dest, const char * Src, size_t Length);
void cn36_StrNCat(char * Dest, const char * Src, size_t Length);
bool cn36_StrNAllocCpy(char * & Dest, const char * Src, size_t Length);
void cn36_StrDealloc(char * & String);

void cn36_StrUpr(char * String);

int cn36_StrToInt(int & TheInt, const char * Buf);
int cn36_StrNToInt(int & TheInt, const char * Buf, size_t Length);
int cn36_IntToStrN(char *Buf, size_t Size, int i);

int cn36_StrToUInt(unsigned int & TheUInt, const char * Buf);
int cn36_StrNToUInt(unsigned int & TheUInt, const char * Buf, size_t Length);

int cn36_StrToLong(long & TheLong, const char *Buf);
int cn36_StrNToLong(long & TheLong, const char * Buf, size_t Length);

int cn36_StrToInt8(teo00_Int8 & TheInt8, const char *Buf);
int cn36_StrNToInt8(teo00_Int8 & TheInt8, const char *Buf, size_t Length);

int cn36_StrToUInt8(teo00_Uint8 & TheUInt8, const char * Buf);
int cn36_StrNToUInt8(teo00_Uint8 & TheUInt8, const char * Buf, size_t Length);
int cn36_UInt8ToStrN(char *Buf, size_t Size, teo00_Uint8 i);

int cn36_StrNConsistsOfDigits(const char *String, size_t Length);
int cn36_StrConsistsOfDigits(const char *String);

const char * cn36_FirstNonDigitOf(const char * string, size_t stringLength);

#if defined (WIN32)
const teo00_Int8    MAX_INT8_CN36=teo00_Int8(9223372036854775807);      //==2^63-1
const teo00_Uint8   MAX_UINT8_CN36=teo00_Uint8(18446744073709551615);   //==2^64-1
#elif defined (BIT64)
const teo00_Int8    MAX_INT8_CN36=teo00_Int8(9223372036854775807L);     //==2^63-1
const teo00_Uint8   MAX_UINT8_CN36=teo00_Uint8(18446744073709551615UL); //==2^64-1
#else
const teo00_Int8    MAX_INT8_CN36=teo00_Int8(9223372036854775807LL);    //==2^63-1
const teo00_Uint8   MAX_UINT8_CN36=teo00_Uint8(18446744073709551615ULL);//==2^64-1
#endif
const teo00_Int8    MIN_INT8_CN36=-MAX_INT8_CN36-1;                     //==-(2^63)

      char * cn36_TerminatingZeroOf(      char * s);
const char * cn36_TerminatingZeroOf(const char * s);

const char * cn36_FirstNonWhiteSpaceOf(const char * s);
      char * cn36_FirstNonWhiteSpaceOf(      char * s);
const char * cn36_FirstWhiteSpaceOf(const char * s);
      char * cn36_FirstWhiteSpaceOf(      char * s);

const char * cn36_FirstNonWhiteSpaceOf(const char * String, size_t StringLength);
const char * cn36_FirstWhiteSpaceOf(const char * String, size_t StringLength);

const char * cn36_StartOfTrailingWhiteSpaces(const char * s);
      char * cn36_StartOfTrailingWhiteSpaces(      char * s);

/*!
   \brief Function for removing the outest pair of double qoutes of the string, if
   any such pair exists. String is moved forward in place so no memory is freed.
   
   \param s [IN] string, from which the double quotes should be removed.
   \return pointer to the resulting string (same as argument s)*/
   
char * cn36_RemoveOuterDoubleQuotes(char * s);
char * cn36_RemoveSpaces(char * string);
void cn36_GetStringBetweenDoubleQuotesOrWhiteSpaces(const char * & Start, const char * & End);

const char * cn36_StrChr(const char *String, const char Character);
      char * cn36_StrChr(      char *String, const char Character);
const char * cn36_StrNChr(const char *String, const char Character, size_t Length);
      char * cn36_StrNChr(      char *String, const char Character, size_t Length);
const char * cn36_StrNRChr(const char *string, size_t length, const char character);
const char * cn36_LoopedStrChr(const char *String, char Character, int Number);
      char * cn36_LoopedStrChr(      char *String, char Character, int Number);
const char * cn36_LoopedStrNChr(const char * string, size_t length, char character, int number);

const char * cn36_StrNStr(const char *string1, size_t lengthString1, const char * string2);
      char * cn36_StrNStr(      char *string1, size_t lengthString1, const char * string2);

const char * cn36_StrRStr(const char * string, const char * stringToFind);

int cn36_StrStrNCmp(const char * String1, const char *String2, size_t Length2);
int cn36_StrNUprCmp(const char *String1, const char *String2, size_t Length);
int cn36_StrStrNUprCmp(const char * String1, const char *String2, size_t Length2);
int cn36_StrUprCmp(const char * String1, const char *String2);
int cn36_StrStrSpaceCmp(const char * String1, const char *String2);

bool cn36_ConstructCompletePath(char *& CompletePath, const char * Directory, const char * FileName);
int cn36_IsAbsolutePath(const char * Path);


extern char MsgBuf_cn36[1024];

#define MaxLengthOfProtFileName_cn36 255
extern char PathOfProtFile_cn36[MaxLengthOfProtFileName_cn36+1];

int cn36_AppendFileToFile(const char * Src, const char *Dest);
int cn36_MoveFile(const char * Src, const char *Dest);

const char *cn36_GetHostName();

const char * cn36_BaseNameFrom(const char *Path);
      char * cn36_BaseNameFrom(      char *Path);

const char * cn36_EndOfDirectory(const char *Path);
      char * cn36_EndOfDirectory(      char *Path);

const char * cn36_NextPathPart(const char *Path);
int          cn36_IsPathDelimiter(const char *Path);

int cn36_PathCmp(const char * Path1, const char *Path2);
int cn36_PathNCmp(const char * Path1, const char *Path2, size_t Length);

int cn36_CheckExist(const char * FileName);
int cn36_CheckRead(const char * pszFileName);
int cn36_CheckExe(const char * pszProgramName);

void cn36_Sleep(int Seconds);
long cn36_GetCurrentTime();
long cn36_SecondsBetween(long time1, long time2);

int cn36_CompareEnvVarNames(const char * EnvironmentVariable1, const char * EnvironmentVariable2);
const char * cn36_GetEnv(const char * EnvironmentVariable);

int          cn36_PutEnv(const char * VariableName,
                         const char * VariableValue);
int          cn36_PutEnv(const char * VariableNameAndValue);
void         cn36_UnsetEnv(const char * VariableName);

class tcn36_DateTimeFormat
{
  public:
    enum PartType{Year=0, Month=1, Day=2, Hour=3, Minute=4, Second=5, AMPM=6, FixedEnd=7, Nothing=8};
    enum AMPMType{None, AM, PM};
    enum DelimiterType{NoDelimiter, NonDigit, Space};

    tcn36_DateTimeFormat();
    tcn36_DateTimeFormat(const char * TheFormatString);
    ~tcn36_DateTimeFormat();

    int ObjectIsGood() {return IsGoodFlag;}
    int IsValidFormat() {return IsValidFlag;}

    size_t GivePosition(PartType SearchedType) const {return (SearchedType<Nothing)?Position[SearchedType]:0;}
    size_t GiveDigits(PartType SearchedType) const   {return (SearchedType<Nothing)?Digits[SearchedType]:0;}

    PartType      GiveType(int Part) const {return (Part<Nothing)?Type[Part]:Nothing;}
    int           GiveNumberOfGaps(int Part) const {return (Part<Nothing)?GapsInFront[Part]:0;}
    size_t        GivePositionFromGap(int Part) const {return (Part<Nothing)?RelativePositionToGap[Part]:0;}
    DelimiterType GiveDelimiter(int Part) const {return (Part<Nothing)?Delimiter[Part]:NoDelimiter;}

    void operator=(const char * NewFormatString);
    char * CreateCopyOfString() const;
    const char * giveFormatString() const;

    static AMPMType StartsWithAMPM(const char *String, size_t & CharsUsedByAMPM);
    int SetAMPM(AMPMType AMPMFlag, char * String) const;

  private:
    char * FormatString;

    size_t Position[Nothing];
    size_t Digits[Nothing];

    PartType      Type[Nothing];
    int           GapsInFront[Nothing];
    size_t        RelativePositionToGap[Nothing];
    DelimiterType Delimiter[Nothing];

    int IsGoodFlag,
        IsValidFlag;
};

class tcn36_DateTimeString
{
  public:
    tcn36_DateTimeString(void);
    tcn36_DateTimeString(int i);
    tcn36_DateTimeString(const char * String, const tcn36_DateTimeFormat & Format);
    tcn36_DateTimeString(const char * String, const tcn36_DateTimeFormat & Format, const char * & EndOfDateTime);

    void   SetTo();
    void   SetTo(const char * String, const tcn36_DateTimeFormat & Format);
    void   SetTo(const char * String, const tcn36_DateTimeFormat & Format, const char * & EndOfDateTime);
    int    IsGood();
    char * CreateDateTimeString(const tcn36_DateTimeFormat & Format) const;
    int    operator<=(const tcn36_DateTimeString & OtherDateTime) const;
    int    operator==(const tcn36_DateTimeString & OtherDateTime) const;
    int    operator!=(const tcn36_DateTimeString & OtherDateTime) const;
    int    operator>(const tcn36_DateTimeString & OtherDateTime) const;

  private:
    void SetMonthDaySecondsTo(int TheYear, int TheMonth, int TheDay, int TheHour, int TheMinute, int TheSecond);
    void MonthDaySecondsToInts(int & TheMonth, int & TheDay, int & TheHour, int & TheMinute, int & TheSecond) const;
    void IntNCpy(char * DestString, int TheInt, size_t Chars) const;

    int Year;
    int MonthDaySecond;

    static int MonthConversionFactor;
    static int DayConversionFactor;
    static int SecondsPerHour;
    static int SecondsPerMinute;

    static int MaxMonthDaySeconds;  //used to generate compiler warnings if int is too small
};

#endif
