/*!
  \file    Loader_Common.hpp
  \author  SteffenS
  \brief   Common definitions for FASTLOADER implementation

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#ifndef LOADER_COMMON_HPP
#define LOADER_COMMON_HPP

#include "Loader/Loader_Types.hpp"
#include "SAPDBCommon/SAPDB_string.h"       // SAPDB_memcpy
#include "hsp77.h"                          // encoding

#include "vsp001.h"                         // message types ...
#include "vin00.h"                          // date and time conversion


//#define LOADER_ASSIGN_DATA      -3      // sollte eigentlich fuer FASTLOAD nicht notwendig sein
#define LOADER_EMPTY_DATA       -4
#define LOADER_DEFAULT_DATA     -5


/*
#ifndef LOADER_TRACING
#define LOADER_TRACING 1
#endif
*/


#if defined (LOADER_SWAPPED_UCS2)
#define    LOADER_SWAP_TYPE  LOADER_SWAPKIND_SWAPPED
#else
#define    LOADER_SWAP_TYPE  LOADER_SWAPKIND_NORMAL
#endif

const LOADER_Int4    LOADER_STAMP_LENGTH    = 9;
const LOADER_Char    LOADER_BLANK           = '\x20';


const LOADER_Int4    LOA_MAX_REPLY_LENGTH   = 1024;


// char data types
static char
LOADER_MAP_CHARDATATYPE[39] =
    {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0};

// long data types
static char
LOADER_MAP_LONGDATATYPE[39] =
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0};


// number data types
static char
LOADER_MAP_NUMBERDATATYPE[39] =
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0};


// special constant data types
static LOADER_Char
LOADER_MAP_SPECCONSTDATATYPE[39] =
    {0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


// blob data types
static LOADER_Char
LOADER_MAP_BLOBDATATYPE[39] =
    {0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0};


// unicode (string) data types
static LOADER_Char
LOADER_MAP_UNICODEDATATYPE[39] =
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0};




// HOSTTYPE MAPPINGs
static LOADER_Char
LOADER_MAP_NUMBER_HOSTTYPE[35] = 
    {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0};

static LOADER_Char
LOADER_MAP_CHAR_HOSTTYPE[35] =
    {0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1};


static LOADER_Char
LOADER_MAP_SAP_VARCHAR_HOSTTYPE[35] =
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1};        // includes LOADER_HostType_SAP_LONG_BINARY


static LOADER_StringEncoding
LOA_MAP_HOSTTYPE_TO_ENCODING[35] =
    {LOADER_STRINGENCODING_UNKNOWN,         // 0
     LOADER_STRINGENCODING_BINARY,          // 1
     LOADER_STRINGENCODING_ASCII,           // 2
     LOADER_STRINGENCODING_UNKNOWN,         // 3
     LOADER_STRINGENCODING_UTF8,            // 4
     LOADER_STRINGENCODING_UNKNOWN,         // 5
     LOADER_STRINGENCODING_UNKNOWN,         // 6
     LOADER_STRINGENCODING_UNKNOWN,         // 7
     LOADER_STRINGENCODING_UNKNOWN,         // 8
     LOADER_STRINGENCODING_UNKNOWN,         // 9
     LOADER_STRINGENCODING_UNKNOWN,         // 10
     LOADER_STRINGENCODING_UNKNOWN,         // 11
     LOADER_STRINGENCODING_UNKNOWN,         // 12
     LOADER_STRINGENCODING_UNKNOWN,         // 13
     LOADER_STRINGENCODING_UNKNOWN,         // 14
     LOADER_STRINGENCODING_UNKNOWN,         // 15
     LOADER_STRINGENCODING_UNKNOWN,         // 16
     LOADER_STRINGENCODING_UNKNOWN,         // 17
     LOADER_STRINGENCODING_UNKNOWN,         // 18
     LOADER_STRINGENCODING_UNKNOWN,         // 19
     LOADER_STRINGENCODING_UCS2,            // 20
     LOADER_STRINGENCODING_UCS2SWAPPED,     // 21
     LOADER_STRINGENCODING_UNKNOWN,         // 22
     LOADER_STRINGENCODING_UNKNOWN,         // 23
     LOADER_STRINGENCODING_UNKNOWN,         // 24
     LOADER_STRINGENCODING_UNKNOWN,         // 25
     LOADER_STRINGENCODING_UNKNOWN,         // 26
     LOADER_STRINGENCODING_UNKNOWN,         // 27
     LOADER_STRINGENCODING_UNKNOWN,         // 28
     LOADER_STRINGENCODING_UNKNOWN,         // 29
     LOADER_STRINGENCODING_BINARY,          // 30
     LOADER_STRINGENCODING_ASCII,           // 31
     LOADER_STRINGENCODING_UTF8,            // 32
     LOADER_STRINGENCODING_UCS2,            // 33
     LOADER_STRINGENCODING_UCS2SWAPPED};    // 34


static LOADER_Byte
LOADER_Latin1_ToLowerMap[256] =
{
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
 0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
 0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
 0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
 0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
 0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
 0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
 0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
 0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
 0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
 0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
 0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF
};

static LOADER_Byte
LOADER_lLatin1_ToUpperMap[256] =
{
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
 0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
 0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
 0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
 0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
 0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
 0x58,0x59,0x5A,0x7B,0x7C,0x7D,0x7E,0x7F,
 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
 0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
 0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
 0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
 0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
 0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF
};


// Encodings; directly related to LOADER_StringEncoding
static const tsp77encoding *pLOADER_Encoding[7] = {NULL,                         // unknown
                                                   sp77encodingAscii,
                                                   sp77encodingUCS2,
                                                   sp77encodingUCS2Swapped,
                                                   sp77encodingUTF8,
                                                   sp77encodingUCS2Native,
                                                   NULL};                        // binary


const LOADER_Byte    UNDEFINED_BYTE         = static_cast<unsigned char>(csp_undef_byte);
const LOADER_Char    DEFINED_BYTE           = csp_defined_byte;
const LOADER_Char    DEFINED_BYTE_ASCII     = '\x20';
const LOADER_Char    DEFINED_BYTE_EBCDIC    = '\x40';
const LOADER_Char    DEFINED_BYTE_UNICODE   = csp_unicode_def_byte;

const LOADER_Char szDefByteArray[39] =
{DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE_ASCII,     DEFINED_BYTE_EBCDIC,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE_ASCII,     DEFINED_BYTE_EBCDIC,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE_ASCII,     DEFINED_BYTE_ASCII,
 DEFINED_BYTE,          DEFINED_BYTE_ASCII, DEFINED_BYTE,           DEFINED_BYTE,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE_ASCII,
 DEFINED_BYTE_EBCDIC,   DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE,
 DEFINED_BYTE_UNICODE,  DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE_ASCII,
 DEFINED_BYTE_EBCDIC,   DEFINED_BYTE,       DEFINED_BYTE_UNICODE,   DEFINED_BYTE_UNICODE,
 DEFINED_BYTE_UNICODE,  DEFINED_BYTE,       DEFINED_BYTE};



const LOADER_Char szFillByteArray[39] =
{DEFINED_BYTE,         DEFINED_BYTE,       DEFINED_BYTE_ASCII,   DEFINED_BYTE_EBCDIC,
 DEFINED_BYTE,         DEFINED_BYTE,       DEFINED_BYTE_ASCII,   DEFINED_BYTE_EBCDIC,
 DEFINED_BYTE,         DEFINED_BYTE,       DEFINED_BYTE_ASCII,   DEFINED_BYTE_ASCII,
 DEFINED_BYTE,         DEFINED_BYTE_ASCII, DEFINED_BYTE,         DEFINED_BYTE,
 DEFINED_BYTE,         DEFINED_BYTE,       DEFINED_BYTE,         DEFINED_BYTE_ASCII,
 DEFINED_BYTE_EBCDIC,  DEFINED_BYTE,       DEFINED_BYTE,         DEFINED_BYTE,
 DEFINED_BYTE_ASCII,   DEFINED_BYTE,       DEFINED_BYTE,         DEFINED_BYTE,
 DEFINED_BYTE,         DEFINED_BYTE,       DEFINED_BYTE,         DEFINED_BYTE_ASCII,
 DEFINED_BYTE_EBCDIC,  DEFINED_BYTE,       DEFINED_BYTE_ASCII,   DEFINED_BYTE_ASCII,
 DEFINED_BYTE_ASCII,   DEFINED_BYTE,       DEFINED_BYTE};



/*!
  -----------------------------------------------------------------------------
 
  function:     GetDefinedByte
  -----------------------------------------------------------------------------

  description:  Defines the define byte depending on the data type.

  arguments:    DataType    [IN]  - data type to get the def byte for
  
  returnvalue:  defined byte
  -----------------------------------------------------------------------------
*/
inline LOADER_Char
GetDefinedByte(LOADER_SQLType SQLDataType)
{
    return szDefByteArray[SQLDataType];
}
// GetDefinedByte()

/*!
  -----------------------------------------------------------------------------
 
  function:     GetFillByte
  -----------------------------------------------------------------------------

  description:  Defines the fill byte depending on the data type.

  arguments:    DataType    [IN]  - data type to get the fill byte for
  
  returnvalue:  fill byte
  -----------------------------------------------------------------------------
*/
inline LOADER_Char
GetFillByte(LOADER_SQLType DataType)
{
    return szFillByteArray[DataType];
}
// GetFillByte()


const char *
LOADER_HostTypeToString(const LOADER_HostType hosttype);

const char *
LOADER_SQLTypeToString(const LOADER_SQLType hosttype);


typedef struct Loader_ColumnData
{
    LOADER_Int4                 PosInStmt;
    
    LOADER_Bool                 PtrUsed_ls00;
    LOADER_Byte*                pPtrToData;
    LOADER_Byte*                pDataBuffer;
    LOADER_Int4                 lDataBufferLength;
    LOADER_Int4                 lDataLength;

    LOADER_HostType             HostType;
    LOADER_Char                 cDefByte;
    LOADER_Char                 cFillByte;
    LOADER_Char                 bUnicodeConversion;

    LOADER_Int4                 LengthInfoByteCount;
    LOADER_Int4                 DataOffset;

    LOADER_SQLColumnInfo        SQLColumnInfo;
    
    LOADER_Bool                 ExtHexRepresentation;
    LOADER_Bool                 IsNILValue;
} Loader_ColumnData;

typedef enum Loader_FLPacketStatus
{
    LOA_PACKET_INITIAL = 1,
    LOA_PACKET_INPROCESS,
    LOA_PACKET_NEEDDATA,
    LOA_PACKET_FILLED
} Loader_FLPacketStatus;


typedef struct LOADER_OIPartInfo
{
    LOADER_Int2     Count;
    LOADER_Int4*    pPartKind;
    LOADER_String*  pData;
}
LOADER_OIPartInfo;


typedef struct LOADER_Parameter
{
    LOADER_HostType     HostType;                 //!< The host type.
    LOADER_Byte*        pszData;                  //!< Pointer to the data.
    LOADER_Length*      LengthIndicator;          //!< Length of the data or special indicator.
    LOADER_Length       BytesLength;              //!< Length of the data buffer in bytes. 
    LOADER_Int4*        PosIndicator;             //!< Start position for reading of long data. Not used for FASTLOAD
    LOADER_Bool         AddrBound;                //!< Flag: Bound by Address.
    LOADER_Bool         Terminate;                //!< Flag: If the out Parameter should be zero terminated
}
LOADER_Parameter;


typedef struct Loader_TableDescription
{
    LOADER_Char*           pszAuthID;              // DB Identifier: max 32 char (= 64 Byte in UNICODE case)
    LOADER_Char*           pszTableName;           // DB Identifier: max 32 char (= 64 Byte in UNICODE case)

    LOADER_Char**          pszColumnNames;         // DB Identifiers (array of ptrs): max 32 char (= 64 Byte in UNICODE case)

    LOADER_Int2            lColumnCount;            // sp7t_field_cnt_F
    LOADER_SwapKind        KernelSwap;             // sp7t_kernel_swap_F
    LOADER_Int2            lKernelLoc;             // sp7t_kernel_loc_F
    LOADER_Int2            lConstraintCount;       // sp7t_constraint_cnt_F
    LOADER_Int2            lFiller;                // sp7t_filler_F

    LOADER_SQLColumnInfo*  pColumnInfoArray;           // sp7t_fields_F    csp_max_fields = 1024
} Loader_TableDescription;


typedef tin_date_mask LOADER_DateTimeMask;


#endif  //LOADER_COMMON_HPP