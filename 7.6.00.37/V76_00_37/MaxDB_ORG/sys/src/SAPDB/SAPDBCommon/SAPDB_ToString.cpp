/****************************************************************************

  module      : SAPDB_ToStringClass.cpp

  -------------------------------------------------------------------------

  responsible : FrankH

  special area: Common
  description:  Value to String conversion i.e. for argument passing


  last changed: 2000-06-15  17:30
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "hsp77.h" /* sp77sprintf */
#include "RunTime/RTE_IInterface.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

//! maximum size of a format specification
#define MAX_CHARACTER_ALLOWED_IN_FORMAT 40

//! maximum width of a number presentiation
#define SAPDB_MAX_NUMBER_PRESENTATION_WIDTH 127

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

//------------ dtor ------
SAPDB_ToStringClass::~SAPDB_ToStringClass()
{
    if ( m_Buf )
    {
        RTE_IInterface::Instance().MsgListAllocator().Deallocate(m_Buf);
    }
}

//------------ copy ctor ------
SAPDB_ToStringClass::SAPDB_ToStringClass(SAPDB_ToStringClass const &toCopy)
{
    if ( this != &toCopy )
    {
        SAPDB_memcpy(&m_PresentationBuffer[0], &toCopy.m_PresentationBuffer[0], MAX_CHARACTER_ALLOWED_IN_PRESENTATION+1);

        if ( 0 != toCopy.m_Buf )
        {
            SAPDB_Int4 copyLength = toCopy.Length()+1;
            SAPDB_memset(&m_PresentationBuffer[0], 0, MAX_CHARACTER_ALLOWED_IN_PRESENTATION);
            if ( copyLength < (MAX_CHARACTER_ALLOWED_IN_PRESENTATION+1) )
            {
                SAPDB_memcpy(&m_PresentationBuffer[0], toCopy.m_Buf, copyLength);
                m_OutputString = &m_PresentationBuffer[0];
                m_Buf = 0;
            }
            else
            {
                m_Buf = (SAPDB_Char *)RTE_IInterface::Instance().MsgListAllocator().Allocate(copyLength);
                if ( m_Buf )
                {
                    SAPDB_memcpy(m_Buf, toCopy.m_Buf, copyLength);
                    m_OutputString = m_Buf;
                }
                else // if allocation failed, we cut input stringf to MAX_CHARACTER_ALLOWED_IN_PRESENTATION - "..."
                {
                    SAPDB_memset( &m_PresentationBuffer[0]
                                , '.'
                                , MAX_CHARACTER_ALLOWED_IN_PRESENTATION);
                    m_PresentationBuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION] = 0;
                    SAPDB_memcpy( &m_PresentationBuffer[0]
                                , toCopy.m_Buf
                                , MAX_CHARACTER_ALLOWED_IN_PRESENTATION-3);
                    m_OutputString = &m_PresentationBuffer[0];
                    m_Buf = 0;
                }
            }
        }
        else if ( toCopy.m_OutputString == &toCopy.m_PresentationBuffer[0] )
        {
            m_OutputString = &m_PresentationBuffer[0];
            m_Buf = 0;
        }
        else 
        {
            m_OutputString = toCopy.m_OutputString;
            m_Buf = 0;
        }
    }
}

// ----------------------- STRING -----------------------

SAPDB_ToStringClass & SAPDB_ToStringClass::operator=(const SAPDB_ToStringClass &toAssignFrom)
{
    if ( this == &toAssignFrom )
    {
        return *this;
    }

    SAPDB_memcpy(&m_PresentationBuffer[0], &toAssignFrom.m_PresentationBuffer[0], MAX_CHARACTER_ALLOWED_IN_PRESENTATION+1);

    if ( m_Buf ) // handle own dynamic allocated buffer
    {
        if ( toAssignFrom.m_Buf )
        {
            SAPDB_Int4 copyLength = toAssignFrom.Length()+1;
            if ( copyLength <= (this->Length()+1) )
            {
                SAPDB_memcpy(m_Buf, toAssignFrom.m_Buf, copyLength); // outputBuffer pointer correct already 
            }
            else
            {
                RTE_IInterface::Instance().MsgListAllocator().Deallocate(m_Buf);
                m_Buf = (SAPDB_Char *)RTE_IInterface::Instance().MsgListAllocator().Allocate(copyLength);
                if ( m_Buf )
                {
                    SAPDB_memcpy(m_Buf, toAssignFrom.m_Buf, copyLength);
                    m_OutputString = m_Buf;
                }
                else // if allocation failed, we cut input stringf to MAX_CHARACTER_ALLOWED_IN_PRESENTATION - "..."
                {
                    SAPDB_memset( &m_PresentationBuffer[0]
                                , '.'
                                , MAX_CHARACTER_ALLOWED_IN_PRESENTATION);
                    m_PresentationBuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION] = 0;
                    SAPDB_memcpy( &m_PresentationBuffer[0]
                                , toAssignFrom.m_Buf
                                , MAX_CHARACTER_ALLOWED_IN_PRESENTATION-3);
                    m_OutputString = &m_PresentationBuffer[0];
                    m_Buf = 0;
                }
            }
        }
            // cannot reuse! Free it
        else if ( toAssignFrom.m_OutputString == &toAssignFrom.m_PresentationBuffer[0] )
        {
            m_OutputString = &m_PresentationBuffer[0];
            RTE_IInterface::Instance().MsgListAllocator().Deallocate(m_Buf);
            m_Buf = 0;
        }
        else 
        {
            m_OutputString = toAssignFrom.m_OutputString;
            RTE_IInterface::Instance().MsgListAllocator().Deallocate(m_Buf);
            m_Buf = 0;
        }
    }
    else // ok otherwise we behave exactly as if copy consturctor is called
    {
        if ( 0 != toAssignFrom.m_Buf )
        {
            SAPDB_Int4 copyLength = toAssignFrom.Length()+1;
            SAPDB_memset(&m_PresentationBuffer[0], 0, MAX_CHARACTER_ALLOWED_IN_PRESENTATION);
            if ( copyLength < (MAX_CHARACTER_ALLOWED_IN_PRESENTATION+1) )
            {
                SAPDB_memcpy(&m_PresentationBuffer[0], toAssignFrom.m_Buf, copyLength);
                m_OutputString = &m_PresentationBuffer[0];
                m_Buf = 0;
            }
            else
            {
                m_Buf = (SAPDB_Char *)RTE_IInterface::Instance().MsgListAllocator().Allocate(copyLength);
                if ( m_Buf )
                {
                    SAPDB_memcpy(m_Buf, toAssignFrom.m_Buf, copyLength);
                    m_OutputString = m_Buf;
                }
                else // if allocation failed, we cut input stringf to MAX_CHARACTER_ALLOWED_IN_PRESENTATION - "..."
                {
                    SAPDB_memset( &m_PresentationBuffer[0]
                                , '.'
                                , MAX_CHARACTER_ALLOWED_IN_PRESENTATION);
                    m_PresentationBuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION] = 0;
                    SAPDB_memcpy( &m_PresentationBuffer[0]
                                , toAssignFrom.m_Buf
                                , MAX_CHARACTER_ALLOWED_IN_PRESENTATION-3);
                    m_OutputString = &m_PresentationBuffer[0];
                    m_Buf = 0;
                }
            }
        }
        else if ( toAssignFrom.m_OutputString == &toAssignFrom.m_PresentationBuffer[0] )
        {
            m_OutputString = &m_PresentationBuffer[0];
            m_Buf = 0;
        }
        else 
        {
            m_OutputString = toAssignFrom.m_OutputString;
            m_Buf = 0;
        }
    }

    return *this;
}

// ----------------------- STRING -----------------------

void SAPDB_ToStringClass::FillStringFormatBuffer( const SAPDB_Char *String,
                                             const SAPDB_UInt2 Width,
                                             const SAPDB_UInt2 Precision,
                                             const int         Flags)
{
  SAPDB_Char formatBuffer[MAX_CHARACTER_ALLOWED_IN_FORMAT+1];
  int StringLen;
  
  if (Precision > 0) 
  {
    const SAPDB_Char * nullPos = (const SAPDB_Char *) memchr (String, '\0', Precision);
    if (nullPos == NULL) 
    {
      StringLen = Precision;
    }
    else 
    {
      StringLen = (int) (nullPos - String) ;
    }
  }
  else 
  {
      StringLen = SAPDB_strlen ( String ) ;
  }

  if ( Precision == 0 
    && Width <= StringLen 
    && StringLen <= MAX_CHARACTER_ALLOWED_IN_PRESENTATION )
  {
      SAPDB_strcpy(&m_PresentationBuffer[0], String);
      m_OutputString = &m_PresentationBuffer[0];
      m_Buf          = 0;
  }
  else
  {
    int FormatBufferLen = 0 ;

    formatBuffer[FormatBufferLen] = '%' ;
    FormatBufferLen++ ;

    if (  Flags & left )
    {
      formatBuffer[FormatBufferLen] = '-' ;
      FormatBufferLen++ ;
    }

    if (  Flags & hex )
    {
        if ( Flags & uppercase )
        {
            if ( Precision > 0 )
              FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                              MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                              "%d.%dR", 
                                              Width, 
                                              Precision ) ;
            else
              FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                              MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen, 
                                              "%d.%dR", 
                                              Width,
                                              StringLen ) ;
        }
        else
        {
            if ( Precision > 0 )
              FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                              MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                              "%d.%dr", 
                                              Width, 
                                              Precision ) ;
            else
              FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                              MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen, 
                                              "%d.%dr", 
                                              Width,
                                              StringLen ) ;
        }
    }
    else
    {
        if ( Precision > 0 )
          FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                          MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                          "%d.%ds", 
                                          Width, 
                                          Precision ) ;
        else
          FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                          MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen, 
                                          "%ds", 
                                          Width ) ;
    }

    int OutputLen = Precision > Width ? Precision : Width ;

    if ( OutputLen < StringLen )
      OutputLen = StringLen ;

    if ( OutputLen > MAX_CHARACTER_ALLOWED_IN_PRESENTATION )
    {
        m_Buf = (SAPDB_Char *)RTE_IInterface::Instance().MsgListAllocator().Allocate(OutputLen + 1);
        if ( m_Buf )
        {
            sp77sprintf(m_Buf, OutputLen + 1, formatBuffer, String ) ;
            m_OutputString = m_Buf ;
        }
        else // if allocation failed, we cut input stringf to MAX_CHARACTER_ALLOWED_IN_PRESENTATION - "..."
        {
            SAPDB_memset( &m_PresentationBuffer[0]
                        , '.'
                        , MAX_CHARACTER_ALLOWED_IN_PRESENTATION);
            m_PresentationBuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION] = 0;
            sp77sprintf(m_PresentationBuffer, 
                        MAX_CHARACTER_ALLOWED_IN_PRESENTATION-3, 
                        formatBuffer, 
                        String );
            m_OutputString = m_PresentationBuffer ;
            m_Buf = 0;
        }
    }
    else
    {
        m_PresentationBuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION] = 0;
        sp77sprintf(m_PresentationBuffer, 
            MAX_CHARACTER_ALLOWED_IN_PRESENTATION+1, 
            formatBuffer, 
            String );
        m_OutputString = m_PresentationBuffer ;
        m_Buf = 0;
    }
  }
}

// ----------------------- BOOL / INT / UINT  -----------------------

void SAPDB_ToStringClass::FillBoolAlphaFormatBuffer( const SAPDB_Bool isTrue,
                                                     const SAPDB_UInt2 width,
                                                     const int combinedFlags)
{
    if ( combinedFlags & uppercase )
    {
        FillStringFormatBuffer(isTrue ? "TRUE" : "FALSE", width, width, combinedFlags);
    }
    else
    {
        FillStringFormatBuffer(isTrue ? "true" : "false", width, width, combinedFlags);
    }
}

//-------------------

inline void SAPDB_ToStringClass::FillNumberFormatBuffer( SAPDB_Char *formatBuffer,
                                                         const SAPDB_UInt2 Width,
                                                         const int         Flags,
                                                         const bool        SignedValue,
                                                         const bool        QuadWord)
{
    SAPDB_UInt4 usedWidth;

    m_Buf          = NULL ;

    int FormatBufferLen = 0 ;

    // silently, but reasonable.... PTS 1110611
    usedWidth = ( Width > SAPDB_MAX_NUMBER_PRESENTATION_WIDTH ? 
                    SAPDB_MAX_NUMBER_PRESENTATION_WIDTH : Width );

    formatBuffer[FormatBufferLen] = '%' ;
    FormatBufferLen++ ;

    if ( Flags & showbase ) 
    {
        formatBuffer[FormatBufferLen] = '#' ;
        FormatBufferLen++ ;
    }

    if ( Flags & showpos )
    {
        formatBuffer[FormatBufferLen] = '+' ;
        FormatBufferLen++ ;
    }

    if (( Flags & showpoint ) && ( usedWidth ))
	{
      formatBuffer[FormatBufferLen] = '0';
      FormatBufferLen++ ;
	}

    if ( usedWidth )
      if ( Flags & left )
        FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                        MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                        "-%d",
                                        usedWidth );
      else
        FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen,
                                        MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                        "%d",
                                        usedWidth );

    if ( Flags & ptr )
    {
        if (Flags & uppercase) {
            formatBuffer[FormatBufferLen] = 'P';
        }
        else {
            formatBuffer[FormatBufferLen] = 'p';
        }
    }
    else 
    {
        if (QuadWord) {
            formatBuffer[FormatBufferLen] = 'q';
            FormatBufferLen++ ;
        }
        if ( Flags & oct )
          formatBuffer[FormatBufferLen] = 'o' ;
        else if ( Flags & hex )
          formatBuffer[FormatBufferLen] = Flags & uppercase ? 'X' : 'x';
        else if ( SignedValue )
          formatBuffer[FormatBufferLen] = 'd' ;
        else
          formatBuffer[FormatBufferLen] = 'u' ;
    }

    FormatBufferLen++ ;
    formatBuffer[FormatBufferLen] = '\0' ;
}

//---------------------

void SAPDB_ToStringClass::FillSignedNumberFormatBuffer( const SAPDB_Int8  Number,
                                                   const SAPDB_UInt2 Width,
                                                   const int         Flags,
                                                   const bool        QuadWord )
{
    if ( Flags & boolalpha )
    {
        FillBoolAlphaFormatBuffer(Number != 0, Width, Flags);
    }
    else
    {
        SAPDB_Char formatBuffer[MAX_CHARACTER_ALLOWED_IN_FORMAT+1];
        FillNumberFormatBuffer(&formatBuffer[0], Width, Flags, true, QuadWord);

        SAPDB_Char numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH+1];
        numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH] = 0;

        if ( QuadWord )
        {
            sp77sprintf(numberPresentation, 
                SAPDB_MAX_NUMBER_PRESENTATION_WIDTH, 
                formatBuffer, 
                Number );
        }
        else
        {
            sp77sprintf(numberPresentation, 
                SAPDB_MAX_NUMBER_PRESENTATION_WIDTH, 
                formatBuffer, 
                (SAPDB_Int4)Number ) ;
        }

        FillStringFormatBuffer(&numberPresentation[0], 0, 0, noformat );
    }
}

//---------------------

void SAPDB_ToStringClass::FillUnsignedNumberFormatBuffer( const SAPDB_UInt8 Number,
                                                     const SAPDB_UInt2 Width,
                                                     const int         Flags,
                                                     const bool        QuadWord )
{
    if ( Flags & boolalpha )
    {
        FillBoolAlphaFormatBuffer(Number != 0, Width, Flags);
    }
    else
    {
        SAPDB_Char formatBuffer[MAX_CHARACTER_ALLOWED_IN_FORMAT+1];
        FillNumberFormatBuffer(&formatBuffer[0], Width, Flags, false, QuadWord);

        SAPDB_Char numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH+1];
        numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH] = 0;

        if ( QuadWord )
        {
            sp77sprintf(numberPresentation, 
                SAPDB_MAX_NUMBER_PRESENTATION_WIDTH, 
                formatBuffer, 
                Number );
        }
        else
        {
            sp77sprintf(numberPresentation, 
                SAPDB_MAX_NUMBER_PRESENTATION_WIDTH, 
                formatBuffer, 
                (SAPDB_UInt4)Number ) ;
        }

        FillStringFormatBuffer(&numberPresentation[0], 0, 0, noformat );
    }
}

// ------------------------ BOOL ---------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Bool Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillSignedNumberFormatBuffer(Number ? (SAPDB_Int8)true : (SAPDB_Int8)false, 0, Flag1 | Flag2 | Flag3 | Flag4) ;
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Bool  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillSignedNumberFormatBuffer(Number ? (SAPDB_Int8)true : (SAPDB_Int8)false, Width, Flag1 | Flag2 | Flag3 | Flag4) ;
}

// ----------------------- INT1  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int1  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt1)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, 0, combinedFlags );
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int1  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt1)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, Width, combinedFlags );
}

// ----------------------- UINT1  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt1  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, 0, Flag1 | Flag2 | Flag3 | Flag4);
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt1  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, Width, Flag1 | Flag2 | Flag3 | Flag4);
}

// ----------------------- INT2  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int2  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt2)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, 0, combinedFlags );
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int2  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt2)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, Width, combinedFlags );
}

// ----------------------- UINT2  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt2  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, 0, Flag1 | Flag2 | Flag3 | Flag4);
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt2  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, Width, Flag1 | Flag2 | Flag3 | Flag4);
}

// ----------------------- INT4  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int4  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt4)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, 0, combinedFlags );
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int4  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt4)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, Width, combinedFlags );
}

// ----------------------- UINT4  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt4  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, 0, Flag1 | Flag2 | Flag3 | Flag4);
}

// -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt4  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, Width, Flag1 | Flag2 | Flag3 | Flag4);
}

#if !defined (BIT64) && !defined (_WIN64)
// ----------------------- INT8  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int8  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillSignedNumberFormatBuffer( Number, 0, Flag1 | Flag2 | Flag3 | Flag4, true);
}

// -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Int8  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillSignedNumberFormatBuffer( Number, Width, Flag1 | Flag2 | Flag3 | Flag4, true);
}

// ----------------------- UINT8  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt8  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( Number, 0, Flag1 | Flag2 | Flag3 | Flag4, true);
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_UInt8  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( Number, Width, Flag1 | Flag2 | Flag3 | Flag4, true);
}
#endif /* !defined (BIT64) && !defined (_WIN64) */

#if defined (_WIN32) && defined (_WIN64)
// ----------------------- long  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const long  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillSignedNumberFormatBuffer( (SAPDB_Int4)Number, 0, Flag1 | Flag2 | Flag3 | Flag4, false);
}

// -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const long  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillSignedNumberFormatBuffer( (SAPDB_Int4)Number, Width, Flag1 | Flag2 | Flag3 | Flag4, false);
}

// ----------------------- unsigned long  ---------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const unsigned long  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt4)Number, 0, Flag1 | Flag2 | Flag3 | Flag4, false);
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const unsigned long  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt4)Number, Width, Flag1 | Flag2 | Flag3 | Flag4, false);
}

#endif

// ----------------------- LONG  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Long Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
#if defined (BIT64) || defined(_WIN64)
    FillSignedNumberFormatBuffer( (SAPDB_Int8)Number, 0, Flag1 | Flag2 | Flag3 | Flag4, true );
#else
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt4)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, 0, combinedFlags );
#endif
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Long  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
#if defined (BIT64) || defined(_WIN64)
    FillSignedNumberFormatBuffer( (SAPDB_Int8)Number, Width, Flag1 | Flag2 | Flag3 | Flag4, true );
#else
    int        combinedFlags = Flag1 | Flag2 | Flag3 | Flag4;
    SAPDB_Int8 outputNumber;

    if ( (combinedFlags & (hex|oct)) != 0 )
    {
        outputNumber = (SAPDB_Int8)((SAPDB_UInt4)Number);
    }
    else
    {
        outputNumber = (SAPDB_Int8)Number;
    }

    FillSignedNumberFormatBuffer( outputNumber, Width, combinedFlags );
#endif
}

// ----------------------- ULONG  -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_ULong  Number,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
#if defined (BIT64) || defined(_WIN64)
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, 0, Flag1 | Flag2 | Flag3 | Flag4, true);
#else
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, 0, Flag1 | Flag2 | Flag3 | Flag4);
#endif
}

// -----------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_ULong  Number,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
#if defined (BIT64) || defined(_WIN64)
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, Width, Flag1 | Flag2 | Flag3 | Flag4, true);
#else
    FillUnsignedNumberFormatBuffer( (SAPDB_UInt8)Number, Width, Flag1 | Flag2 | Flag3 | Flag4);
#endif
}

// ----------------------- REAL4 / REAL8 -------------------

void SAPDB_ToStringClass::FillRealFormatBuffer( const SAPDB_UInt2 Width,
                                           const SAPDB_UInt2 Precision,
                                           const int         Flags,
                                           const SAPDB_Real8 Real )
{
    SAPDB_Char formatBuffer[MAX_CHARACTER_ALLOWED_IN_FORMAT+1];
    SAPDB_UInt4 usedWidth;
    SAPDB_UInt4 usedPrecision;

    int FormatBufferLen = 0 ;

    // silently, but reasonable.... PTS 1110611
    usedWidth = ( Width > SAPDB_MAX_NUMBER_PRESENTATION_WIDTH ?
                        SAPDB_MAX_NUMBER_PRESENTATION_WIDTH : Width );

    // silently, but reasonable.... PTS 1110611
    usedPrecision = ( Precision > SAPDB_MAX_NUMBER_PRESENTATION_WIDTH ?
                        SAPDB_MAX_NUMBER_PRESENTATION_WIDTH : Precision );

    formatBuffer[FormatBufferLen] = '%' ;
    FormatBufferLen++ ;

    if ( Flags & showpos )
    {
        formatBuffer[FormatBufferLen] = '+' ;
        FormatBufferLen++ ;
    }

    if ( Flags & showpoint )
	{
      formatBuffer[FormatBufferLen] = '0';
      FormatBufferLen++ ;
	}

    if ( usedWidth )
      if ( Flags & left )
        FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                        MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                        "-%d", 
                                        usedWidth );
      else
        FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                        MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                        "%d", 
                                        usedWidth );

    if ( usedPrecision )
      FormatBufferLen += sp77sprintf( formatBuffer+FormatBufferLen, 
                                      MAX_CHARACTER_ALLOWED_IN_FORMAT-FormatBufferLen,
                                      ".%d", 
                                      usedPrecision ) ;

    if ( Flags & scientific )
        formatBuffer[FormatBufferLen] = Flags & uppercase ? 'E' : 'e';
    else
      formatBuffer[FormatBufferLen] = 'f' ;

    FormatBufferLen++ ;
    formatBuffer[FormatBufferLen] = '\0' ;

    SAPDB_Char numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH+1];
    numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH] = 0;
    sp77sprintf(numberPresentation, SAPDB_MAX_NUMBER_PRESENTATION_WIDTH, formatBuffer, Real ) ;

    FillStringFormatBuffer(&numberPresentation[0], 0, 0, noformat );
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Real8  Real,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillRealFormatBuffer(0, 0, Flag1 | Flag2 | Flag3 | Flag4, Real ) ;
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Real8  Real,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillRealFormatBuffer(Width, 0, Flag1 | Flag2 | Flag3 | Flag4, Real ) ;
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const SAPDB_Real8  Real,
                                const SAPDB_UInt2 Width ,
                                const SAPDB_UInt2 Precision ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    FillRealFormatBuffer(Width, Precision, Flag1 | Flag2 | Flag3 | Flag4, Real ) ;
}


// ----------------------- VOID * (pointer) ----------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const void *Addr,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    SAPDB_Char formatBuffer[MAX_CHARACTER_ALLOWED_IN_FORMAT+1];
    int Flags = Flag1 |Flag2 | Flag3 | Flag4;

    FillNumberFormatBuffer(&formatBuffer[0], 0, Flags | (( Flags & hex ) ? 0 : ptr) ) ;

    SAPDB_Char numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH+1];
    numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH] = 0;
    sp77sprintf(numberPresentation, SAPDB_MAX_NUMBER_PRESENTATION_WIDTH, formatBuffer, Addr ) ;

    FillStringFormatBuffer(&numberPresentation[0], 0, 0, noformat );
}

// --------------------------

SAPDB_ToStringClass::SAPDB_ToStringClass( const void *Addr,
                                const SAPDB_UInt2 Width ,
                                const fmtflags Flag1,
                                const fmtflags Flag2,
                                const fmtflags Flag3,
                                const fmtflags Flag4)
{
    SAPDB_Char formatBuffer[MAX_CHARACTER_ALLOWED_IN_FORMAT+1];
    int Flags = Flag1 |Flag2 | Flag3 | Flag4;

    FillNumberFormatBuffer(&formatBuffer[0], Width, Flags | (( Flags & hex ) ? 0 : ptr) ) ;

    SAPDB_Char numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH+1];
    numberPresentation[SAPDB_MAX_NUMBER_PRESENTATION_WIDTH] = 0;
    sp77sprintf(numberPresentation, SAPDB_MAX_NUMBER_PRESENTATION_WIDTH, formatBuffer, Addr ) ;

    FillStringFormatBuffer(&numberPresentation[0], 0, 0, noformat );
}

// --------------------------

SAPDB_Int4 const SAPDB_ToStringClass::Length() const
{
    return SAPDB_strlen(const_cast<SAPDB_Char *>(m_OutputString));
}
/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

SAPDB_ToStringClass SAPDB_ToString( SAPDB_Char const *prefix
                                  , SAPDB_ToStringClass & valueString
                                  , SAPDB_Char const *postfix)
{
    SAPDB_Char presentationbuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION+1]; // behaviour of SAPDB_ToStringClass adopted...

    SAPDB_ToStringClass prefixString( prefix ? prefix : "" );
    SAPDB_ToStringClass postfixString( postfix ? postfix : "" );

    SAPDB_UInt4 neededLength = prefixString.Length() + valueString.Length() + postfixString.Length();
    SAPDB_Char *resultBuffer;
    if ( neededLength > MAX_CHARACTER_ALLOWED_IN_PRESENTATION )
    {
        resultBuffer = (SAPDB_Char *)RTE_IInterface::Instance().MsgListAllocator().Allocate(neededLength+1);
        if ( !resultBuffer )
        {
            resultBuffer = &presentationbuffer[0];
        }
    }
    else
    {
        resultBuffer = &presentationbuffer[0];
    }

    if ( neededLength > MAX_CHARACTER_ALLOWED_IN_PRESENTATION 
     && resultBuffer == &presentationbuffer[0] )
    {
        // fallback to presentation buffer ... == behaviour of SAPDB_ToStringClass adopted...

        SAPDB_Int4 spaceLeft = MAX_CHARACTER_ALLOWED_IN_PRESENTATION-3; // dont forget the...

        do
        {
            if ( prefixString.Length() <= spaceLeft )
            {
                SAPDB_strcpy(resultBuffer, (SAPDB_Char *)prefixString);
                spaceLeft -= prefixString.Length();
            }
            else
            {
                SAPDB_memcpy(resultBuffer, (SAPDB_Char *)prefixString, spaceLeft);
                break;
            }

            if ( valueString.Length() <= spaceLeft )
            {
                SAPDB_strcpy(resultBuffer + prefixString.Length(), (SAPDB_Char *)valueString);
                spaceLeft -= valueString.Length();
            }
            else
            {
                SAPDB_memcpy(resultBuffer + prefixString.Length(), (SAPDB_Char *)valueString, spaceLeft);
                break;
            }

            if ( postfixString.Length() <= spaceLeft )
            {
                SAPDB_strcpy(resultBuffer + prefixString.Length() + valueString.Length(), (SAPDB_Char *)postfixString);
            }
            else
            {
                SAPDB_memcpy(resultBuffer + prefixString.Length() + valueString.Length(), (SAPDB_Char *)postfixString, spaceLeft);
            }
            spaceLeft = 0; // done...
        } while ( spaceLeft > 0 );

        presentationbuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION-3] = '.';
        presentationbuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION-2] = '.';
        presentationbuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION-1] = '.';
        presentationbuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION] = 0;
    }
    else
    {
        SAPDB_strcpy(resultBuffer, (SAPDB_Char *)prefixString);
        SAPDB_strcpy(resultBuffer + prefixString.Length(), (SAPDB_Char *)valueString);
        SAPDB_strcpy(resultBuffer + prefixString.Length() + valueString.Length(), (SAPDB_Char *)postfixString);
    }

    return SAPDB_ToStringClass(resultBuffer);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

