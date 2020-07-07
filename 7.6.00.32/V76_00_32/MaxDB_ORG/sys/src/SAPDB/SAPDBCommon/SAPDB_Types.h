/*!***************************************************************************

  @file    SAPDB_Types.h
  @author  JoergM
  @ingroup Common
  @brief   Contains base type definitions

  The declarations were move to SAPDB_Types.h
  to allow inclusion by C sources  [DD 16.03.2000]

  This explicitly DO NOT MEAN that base of all implementation is the obselete language called C!

  last changed: 2005-08-25  15:07
  see also    :

  -------------------------------------------------------------------------

\if EMIT_LICENCE
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

\endif

//
//   FreeBSD portions added by Kai Mosebach, 
//   For more informations see : http://www.komadev.de/sapdb
//
//
    
*****************************************************************************/

#ifndef SAPDB_TYPES_H
#define SAPDB_TYPES_H

/*!
@brief basic data type declaring bool value of fixed size of one byte

NOTE the C++ type bool is NOT equivalent with SAPDB_Bool1! Use SAPDB_Bool instead!

This type is used in all interface functions to Pascal code.
 */
typedef unsigned char       SAPDB_Bool1;        /* byte (1 byte, unsigned) */

#if defined(OSF1)
/*!
 Compaq True64 alias DEC OSF1 complains with a warning if a function should returns a value,
 but it actually returns a const value. 
 Example:

  SAPDB_Bool1 f()
  {
      return SAPDB_FALSE;
  }

  cxx: Warning: xxx.cpp, line yyy: type qualifier is meaningless
          on cast type (D:castqualtyp)
     return SAPDB_FALSE;

 Therefore this stupid definition was made.  JörgM 15.1.2001
 */
# define SAPDB_CONSTANT_CONST
#else
/*! this compiler has no problem with cons value */
# define SAPDB_CONSTANT_CONST const
#endif

/*! fixed size true constant */
#define SAPDB_TRUE          ((SAPDB_CONSTANT_CONST SAPDB_Bool1)1)
/*! fixed size false constant */
#define SAPDB_FALSE         ((SAPDB_CONSTANT_CONST SAPDB_Bool1)0)
/*!
@brief basic boolean types for logical expressions
             (machine-dependent size)

  NOTE A C boolean expression is of type 'int'

  If the C++ compiler does not support 'bool', CPP_BOOL_NOT_IMPLEMENTED gets defined.
 */
#if (defined (AIX) && __IBMCPP__ < 500) || ( (defined (SUN) || defined (SOLARIS)) && !defined(_BOOL) ) \
  || !defined (__cplusplus)
/*! indicates platform does not define bool as type */
# define CPP_BOOL_NOT_IMPLEMENTED
#endif

#ifdef CPP_BOOL_NOT_IMPLEMENTED
#if defined(WIN32) || defined(AIX) || defined(LINUX) || defined(OSF1) \
 || defined(HPUX) || defined(NMP) || defined(SUN) || defined(SOLARIS) || defined(FREEBSD)
  typedef unsigned char SAPDB_Bool; /*!< a C++ bool size is 1 byte on these platforms */
  typedef SAPDB_Bool    bool; /*!< support for compiler not defining bool */
#endif
#else
  typedef bool SAPDB_Bool; /*!< bool (size not necessary 1 byte) */
#endif /* CPP_BOOL_NOT_IMPLEMENTED */

#ifdef CPP_BOOL_NOT_IMPLEMENTED
/*! support for compilers not defining true */
#  define     true    ((SAPDB_CONSTANT_CONST SAPDB_Bool)1)
/*! support for compilers not defining false */
#  define     false   ((SAPDB_CONSTANT_CONST SAPDB_Bool)0)
#endif /* CPP_BOOL_NOT_IMPLEMENTED */

/*--------------------------------------------------------------------------
declaration: SAPDB_{Byte | Char}
description: basic data types with size 1 byte
--------------------------------------------------------------------------*/
typedef unsigned char       SAPDB_Byte;         /*!< byte (1 byte, unsigned) */
typedef char                SAPDB_Char;         /*!< char (1 byte, signed)   */

/*--------------------------------------------------------------------------
declaration: SAPDB_{UInt1 | Int1 | UInt2 | Int2 | UInt4 | Int4}
description: basic integer data types with named size (1, 2 and 4 bytes)
--------------------------------------------------------------------------*/
typedef unsigned char       SAPDB_UInt1;        /*!< integer (1 byte, unsigned) */
typedef signed char         SAPDB_Int1;         /*!< integer (1 byte, signed)   */
typedef unsigned short      SAPDB_UInt2;        /*!< integer (2 byte, unsigned) */
typedef signed short        SAPDB_Int2;         /*!< integer (2 byte, signed)   */
typedef unsigned int        SAPDB_UInt4;        /*!< integer (4 byte, unsigned) */
typedef signed int          SAPDB_Int4;         /*!< integer (4 byte, signed)   */

/*--------------------------------------------------------------------------
declaration: SAPDB_{UInt | Int}
description: basic integer data types for fastest execution
             (machine-dependent size)
--------------------------------------------------------------------------*/
typedef unsigned int        SAPDB_UInt;      /*!< integer (fast, unsigned) */
typedef signed int          SAPDB_Int;       /*!< integer (fast, signed)   */

/*--------------------------------------------------------------------------
declaration: SAPDB_{ULong | Long}
description: basic long data types for fastest execution
             (machine-dependent size)
--------------------------------------------------------------------------*/
#if defined(_WIN64)
typedef unsigned __int64    SAPDB_ULong;      /*!< long (fast, unsigned) */
typedef signed __int64      SAPDB_Long;       /*!< long (fast, signed)   */
#else
typedef unsigned long       SAPDB_ULong;      /*!< long (fast, unsigned) */
typedef signed long         SAPDB_Long;       /*!< long (fast, signed)   */
#endif

/*--------------------------------------------------------------------------
declaration: SAPDB_{UInt8 | Int8}
description: system dependent basic data types
--------------------------------------------------------------------------*/
#if defined(_WIN32) || defined(_WIN64)
#   if defined (_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64
        typedef unsigned __int64    SAPDB_UInt8;     /*!< integer (8 byte, unsigned) */
        typedef signed __int64      SAPDB_Int8;      /*!< integer (8 byte, signed)   */
#   else
#       error __int64 type not supported
#   endif
#else
#   if defined (BIT64)
        typedef unsigned long       SAPDB_UInt8;     /*!< integer (8 byte, unsigned) */
        typedef signed long         SAPDB_Int8;      /*!< integer (8 byte, signed)   */
#   else
        typedef unsigned long long  SAPDB_UInt8;     /*!< integer (8 byte, unsigned) */
        typedef signed long long    SAPDB_Int8;      /*!< integer (8 byte, signed)   */
#   endif
#endif /* UNIX(tm) tested on AIX,DEC,LINUX,HP,SNI, FREEBSD, SOLARIS and SUN ;-) */

/*--------------------------------------------------------------------------
declaration: SAPDB_{Real4 | Real8}
description: basic floating point number data types
--------------------------------------------------------------------------*/
typedef float                  SAPDB_Real4;     /*!< single precision real (4byte) */
typedef double                 SAPDB_Real8;     /*!< double precision real (8byte) */

/*--------------------------------------------------------------------------
declaration: SAPDB_{UTF8 | UTF16 | UCS2 | UCS4 }
description: Basis element types of unicode characters
--------------------------------------------------------------------------*/
/*! UTF8 unicode character */
typedef SAPDB_Byte             SAPDB_UTF8;
/*! UTF16 unicode character */
typedef SAPDB_UInt2            SAPDB_UTF16;
/*! UCS2 unicode character */
typedef SAPDB_UInt2            SAPDB_UCS2;
/*! UCS4 unicode character */
typedef SAPDB_UInt4            SAPDB_UCS4;

/*--------------------------------------------------------------------------
declaration: SAPDB_MAX_{ INT2 | INT4 }
description: maximum values of signed integer types
--------------------------------------------------------------------------*/
/*! Maximum 2 byte signed integer */
#define SAPDB_MAX_INT2        (32767)
/*! Maximum 4 byte signed integer */
#define SAPDB_MAX_INT4        (2147483647)

/*--------------------------------------------------------------------------
declaration: SAPDB_MAX_{BYTE | UINT1 | UINT2 | UINT4 | UINT8 }
description: maximum values of unsigned integer types
--------------------------------------------------------------------------*/
/*! Maximum byte value */
#define SAPDB_MAX_BYTE         (0xFF)
/*! Maximum 1 byte unsigned integer */
#define SAPDB_MAX_UINT1        (0xFF)
/*! Maximum 2 byte unsigned integer */
#define SAPDB_MAX_UINT2        (0xFFFF)
/*! Maximum 4 byte unsigned integer */
#define SAPDB_MAX_UINT4        (((SAPDB_UInt4)((SAPDB_Int4)(-1))))  /* 0xFFFFFFFFUL          */
/*! Maximum 8 byte unsigned integer */
#define SAPDB_MAX_UINT8        (((SAPDB_UInt8)((SAPDB_Int8)(-1))))  /* 0xFFFFFFFFFFFFFFFFULL */

/*--------------------------------------------------------------------------
declaration: SAPDB_BITS_PER_{BYTE | UINT1 | UINT2 | UINT4 | UINT8 }
description: number of bits in integer types
--------------------------------------------------------------------------*/
/*! Bits in 1 byte */
#define SAPDB_BITS_PER_BYTE    8
/*! Bits in 1 byte unsigned integer */
#define SAPDB_BITS_PER_UINT1   8
/*! Bits in 2 byte unsigned integer */
#define SAPDB_BITS_PER_UINT2   16
/*! Bits in 4 byte unsigned integer */
#define SAPDB_BITS_PER_UINT4   32
/*! Bits in 8 byte unsigned integer */
#define SAPDB_BITS_PER_UINT8   64

/*--------------------------------------------------------------------------
declaration: Definitions for shift operations, pointer arithmetics
description: defines log2(byte size) for each basic data type with named size
--------------------------------------------------------------------------*/
/*! log2 of bytes size of unsigned 1 byte integer */
#define SAPDB_UINT1_LOG2SIZE   0
/*! log2 of bytes size of signed 1 byte integer */
#define SAPDB_INT1_LOG2SIZE    0
/*! log2 of bytes size of unsigned 2 byte integer */
#define SAPDB_UINT2_LOG2SIZE   1
/*! log2 of bytes size of signed 2 byte integer */
#define SAPDB_INT2_LOG2SIZE    1
/*! log2 of bytes size of unsigned 4 byte integer */
#define SAPDB_UINT4_LOG2SIZE   2
/*! log2 of bytes size of signed 4 byte integer */
#define SAPDB_INT4_LOG2SIZE    2
/*! log2 of bytes size of unsigned 8 byte integer */
#define SAPDB_UINT8_LOG2SIZE   3
/*! log2 of bytes size of signed 8 byte integer */
#define SAPDB_INT8_LOG2SIZE    3
/*! log2 of bytes size of singled precision float (4byte) */
#define SAPDB_REAL4_LOG2SIZE   2
/*! log2 of bytes size of double precision float (8byte) */
#define SAPDB_REAL8_LOG2SIZE   3

/*--------------------------------------------------------------------------
declaration: SAPDB_ALIGNMENT
description: structure alignment (128bit)
             use this alignment and forget alignment problems until you
             face 256bit machines
--------------------------------------------------------------------------*/
/*! default alignment for structures */
#define SAPDB_ALIGNMENT       16

/*! byte alignment mocro (rounds up to next multiple of given alignment) */
#define SAPDB_ALIGN(_value, _alignment)   ((((SAPDB_ULong)(_value) + (_alignment) - 1)/(_alignment))*(_alignment))
/*! 2 byte alignment mocro (rounds up to next multiple of 2) */
#define SAPDB_ALIGN_2BYTE(_value)         ((((SAPDB_ULong)(_value) + 1) / 2) * 2)
/*! 4 byte alignment mocro (rounds up to next multiple of 4) */
#define SAPDB_ALIGN_4BYTE(_value)         ((((SAPDB_ULong)(_value) + 3) / 4) * 4)
/*! 8 byte alignment mocro (rounds up to next multiple of 8) */
#define SAPDB_ALIGN_8BYTE(_value)         ((((SAPDB_ULong)(_value) + 7) / 8) * 8)
/*--------------------------------------------------------------------------
declaration: UTF8
description: Macro for conversion of char* strings to UTF8 strings
--------------------------------------------------------------------------*/
/*! UTF8 cast macro */
#define UTF8(_string) (SAPDB_UTF8*)_string

/*--------------------------------------------------------------------------
declaration: SAPDB_MIN,SAPDB_MAX
description: Macro calculation of the minimum and maximum of two numbers
--------------------------------------------------------------------------*/
/*! minimum macro */
#define SAPDB_MIN(a, b)  (((a) < (b)) ? (a) : (b))
/*! maximum macro */
#define SAPDB_MAX(a, b)  (((b) < (a)) ? (a) : (b)) 

#endif /* SAPDB_TYPES_H */

