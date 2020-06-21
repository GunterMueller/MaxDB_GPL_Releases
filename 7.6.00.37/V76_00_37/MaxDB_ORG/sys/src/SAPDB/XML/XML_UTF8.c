/*!***************************************************************************

  module:       XML_UTF8.c

  ------------------------------------------------------------------------------

  responsible:  FerdiF

  special area: XML UTF-8

  description:  basis functions for handling XML UTF-8 character strings

  see also:     

  ------------------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

 #include <string.h>
 #include "SAPDB/XML/XML_UTF8.h"
 #include "SAPDBCommon/SAPDB_string.h"

/*------------------------------------------------------------------------------*/
static SAPDB_Int UTF8SingleCharSize[256] =
    {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6
    };

/*------------------------------------------------------------------------------*/
/* UTF-8 String containing all Whitespaces defined in XML                       */
/* see also document: "http://www.w3.org/TR/REC-xml"                            */
static const SAPDB_UTF8 *XML_Whitespaces = (SAPDB_UTF8 *)"\x20\x09\x0a\x0d";

/*------------------------------------------------------------------------------*/
/* Following table is used by function: XML_UTF8IsBaseChar()                    */
/* to determine if a Unicode code character is a <BaseChar> as  defined in XML  */
/* see also document: "http://www.w3.org/TR/REC-xml"                            */
static XML_UnicodeCharCode UTF8BaseChar[][2] =
    {
        { 0x0041, 0x005A }, { 0x0061, 0x007A }, { 0x00C0, 0x00D6 }, { 0x00D8, 0x00F6 },
        { 0x00F8, 0x00FF }, { 0x0100, 0x0131 }, { 0x0134, 0x013E }, { 0x0141, 0x0148 },
        { 0x014A, 0x017E }, { 0x0180, 0x01C3 }, { 0x01CD, 0x01F0 }, { 0x01F4, 0x01F5 },
        { 0x01FA, 0x0217 }, { 0x0250, 0x02A8 }, { 0x02BB, 0x02C1 }, { 0x0386, 0x0386 },
        { 0x0388, 0x038A }, { 0x038C, 0x038C }, { 0x038E, 0x03A1 }, { 0x03A3, 0x03CE },
        { 0x03D0, 0x03D6 }, { 0x03DA, 0x03DA }, { 0x03DC, 0x03DC }, { 0x03DE, 0x03DE },
        { 0x03E0, 0x03E0 }, { 0x03E2, 0x03F3 }, { 0x0401, 0x040C }, { 0x040E, 0x044F },
        { 0x0451, 0x045C }, { 0x045E, 0x0481 }, { 0x0490, 0x04C4 }, { 0x04C7, 0x04C8 },
        { 0x04CB, 0x04CC }, { 0x04D0, 0x04EB }, { 0x04EE, 0x04F5 }, { 0x04F8, 0x04F9 },
        { 0x0531, 0x0556 }, { 0x0559, 0x0559 }, { 0x0561, 0x0586 }, { 0x05D0, 0x05EA },
        { 0x05F0, 0x05F2 }, { 0x0621, 0x063A }, { 0x0641, 0x064A }, { 0x0671, 0x06B7 },
        { 0x06BA, 0x06BE }, { 0x06C0, 0x06CE }, { 0x06D0, 0x06D3 }, { 0x06D5, 0x06D5 },
        { 0x06E5, 0x06E6 }, { 0x0905, 0x0939 }, { 0x093D, 0x093D }, { 0x0958, 0x0961 },
        { 0x0985, 0x098C }, { 0x098F, 0x0990 }, { 0x0993, 0x09A8 }, { 0x09AA, 0x09B0 },
        { 0x09B2, 0x09B2 }, { 0x09B6, 0x09B9 }, { 0x09DC, 0x09DD }, { 0x09DF, 0x09E1 },
        { 0x09F0, 0x09F1 }, { 0x0A05, 0x0A0A }, { 0x0A0F, 0x0A10 }, { 0x0A13, 0x0A28 },
        { 0x0A2A, 0x0A30 }, { 0x0A32, 0x0A33 }, { 0x0A35, 0x0A36 }, { 0x0A38, 0x0A39 },
        { 0x0A59, 0x0A5C }, { 0x0A5E, 0x0A5E }, { 0x0A72, 0x0A74 }, { 0x0A85, 0x0A8B },
        { 0x0A8D, 0x0A8D }, { 0x0A8F, 0x0A91 }, { 0x0A93, 0x0AA8 }, { 0x0AAA, 0x0AB0 },
        { 0x0AB2, 0x0AB3 }, { 0x0AB5, 0x0AB9 }, { 0x0ABD, 0x0ABD }, { 0x0AE0, 0x0AE0 },
        { 0x0B05, 0x0B0C }, { 0x0B0F, 0x0B10 }, { 0x0B13, 0x0B28 }, { 0x0B2A, 0x0B30 },
        { 0x0B32, 0x0B33 }, { 0x0B36, 0x0B39 }, { 0x0B3D, 0x0B3D }, { 0x0B5C, 0x0B5D },
        { 0x0B5F, 0x0B61 }, { 0x0B85, 0x0B8A }, { 0x0B8E, 0x0B90 }, { 0x0B92, 0x0B95 },
        { 0x0B99, 0x0B9A }, { 0x0B9C, 0x0B9C }, { 0x0B9E, 0x0B9F }, { 0x0BA3, 0x0BA4 },
        { 0x0BA8, 0x0BAA }, { 0x0BAE, 0x0BB5 }, { 0x0BB7, 0x0BB9 }, { 0x0C05, 0x0C0C },
        { 0x0C0E, 0x0C10 }, { 0x0C12, 0x0C28 }, { 0x0C2A, 0x0C33 }, { 0x0C35, 0x0C39 },
        { 0x0C60, 0x0C61 }, { 0x0C85, 0x0C8C }, { 0x0C8E, 0x0C90 }, { 0x0C92, 0x0CA8 },
        { 0x0CAA, 0x0CB3 }, { 0x0CB5, 0x0CB9 }, { 0x0CDE, 0x0CDE }, { 0x0CE0, 0x0CE1 },
        { 0x0D05, 0x0D0C }, { 0x0D0E, 0x0D10 }, { 0x0D12, 0x0D28 }, { 0x0D2A, 0x0D39 },
        { 0x0D60, 0x0D61 }, { 0x0E01, 0x0E2E }, { 0x0E30, 0x0E30 }, { 0x0E32, 0x0E33 },
        { 0x0E40, 0x0E45 }, { 0x0E81, 0x0E82 }, { 0x0E84, 0x0E84 }, { 0x0E87, 0x0E88 },
        { 0x0E8A, 0x0E8A }, { 0x0E8D, 0x0E8D }, { 0x0E94, 0x0E97 }, { 0x0E99, 0x0E9F },
        { 0x0EA1, 0x0EA3 }, { 0x0EA5, 0x0EA5 }, { 0x0EA7, 0x0EA7 }, { 0x0EAA, 0x0EAB },
        { 0x0EAD, 0x0EAE }, { 0x0EB0, 0x0EB0 }, { 0x0EB2, 0x0EB3 }, { 0x0EBD, 0x0EBD },
        { 0x0EC0, 0x0EC4 }, { 0x0F40, 0x0F47 }, { 0x0F49, 0x0F69 }, { 0x10A0, 0x10C5 },
        { 0x10D0, 0x10F6 }, { 0x1100, 0x1100 }, { 0x1102, 0x1103 }, { 0x1105, 0x1107 },
        { 0x1109, 0x1109 }, { 0x110B, 0x110C }, { 0x110E, 0x1112 }, { 0x113C, 0x113C },
        { 0x113E, 0x113E }, { 0x1140, 0x1140 }, { 0x114C, 0x114C }, { 0x114E, 0x114E },
        { 0x1150, 0x1150 }, { 0x1154, 0x1155 }, { 0x1159, 0x1159 }, { 0x115F, 0x1161 },
        { 0x1163, 0x1163 }, { 0x1165, 0x1165 }, { 0x1167, 0x1167 }, { 0x1169, 0x1169 },
        { 0x116D, 0x116E }, { 0x1172, 0x1173 }, { 0x1175, 0x1175 }, { 0x119E, 0x119E },
        { 0x11A8, 0x11A8 }, { 0x11AB, 0x11AB }, { 0x11AE, 0x11AF }, { 0x11B7, 0x11B8 },
        { 0x11BA, 0x11BA }, { 0x11BC, 0x11C2 }, { 0x11EB, 0x11EB }, { 0x11F0, 0x11F0 },
        { 0x11F9, 0x11F9 }, { 0x1E00, 0x1E9B }, { 0x1EA0, 0x1EF9 }, { 0x1F00, 0x1F15 },
        { 0x1F18, 0x1F1D }, { 0x1F20, 0x1F45 }, { 0x1F48, 0x1F4D }, { 0x1F50, 0x1F57 },
        { 0x1F59, 0x1F59 }, { 0x1F5B, 0x1F5B }, { 0x1F5D, 0x1F5D }, { 0x1F5F, 0x1F7D },
        { 0x1F80, 0x1FB4 }, { 0x1FB6, 0x1FBC }, { 0x1FBE, 0x1FBE }, { 0x1FC2, 0x1FC4 },
        { 0x1FC6, 0x1FCC }, { 0x1FD0, 0x1FD3 }, { 0x1FD6, 0x1FDB }, { 0x1FE0, 0x1FEC },
        { 0x1FF2, 0x1FF4 }, { 0x1FF6, 0x1FFC }, { 0x2126, 0x2126 }, { 0x212A, 0x212B },
        { 0x212E, 0x212E }, { 0x2180, 0x2182 }, { 0x3041, 0x3094 }, { 0x30A1, 0x30FA },
        { 0x3105, 0x312C }, { 0xAC00, 0xD7A3 }
    };
static const unsigned int UTF8BaseCharDim1 =
    sizeof(UTF8BaseChar) / (sizeof(XML_UnicodeCharCode) * 2);

/*------------------------------------------------------------------------------*/
/* Following table is used by function: XML_UTF8IsIdeographic()                 */
/* to determine if a Unicode code character is a <Ideographic> as defined       */
/* in XML                                                                       */
/* see also document: "http://www.w3.org/TR/REC-xml"                            */
static XML_UnicodeCharCode UTF8Ideographic[][2] =
    {
        { 0x4E00, 0x9FA5 }, { 0x3007, 0x3007 }, { 0x3021, 0x3029 }
    };
static const unsigned int UTF8IdeographicDim1 =
    sizeof(UTF8Ideographic) / (sizeof(XML_UnicodeCharCode) * 2);

/*------------------------------------------------------------------------------*/
/* Following table is used by function: XML_UTF8IsCombiningChar()               */
/* to determine if a Unicode code character is a <CombiningChar> as defined     */
/* in XML                                                                       */
/* see also document: "http://www.w3.org/TR/REC-xml"                            */
static XML_UnicodeCharCode UTF8CombiningChar[][2] =
    {
        { 0x0300, 0x0345 }, { 0x0360, 0x0361 }, { 0x0483, 0x0486 }, { 0x0591, 0x05A1 },
        { 0x05A3, 0x05B9 }, { 0x05BB, 0x05BD }, { 0x05BF, 0x05BF }, { 0x05C1, 0x05C2 },
        { 0x05C4, 0x05C4 }, { 0x064B, 0x0652 }, { 0x0670, 0x0670 }, { 0x06D6, 0x06DC },
        { 0x06DD, 0x06DF }, { 0x06E0, 0x06E4 }, { 0x06E7, 0x06E8 }, { 0x06EA, 0x06ED },
        { 0x0901, 0x0903 }, { 0x093C, 0x093C }, { 0x093E, 0x094C }, { 0x094D, 0x094D },
        { 0x0951, 0x0954 }, { 0x0962, 0x0963 }, { 0x0981, 0x0983 }, { 0x09BC, 0x09BC },
        { 0x09BE, 0x09BE }, { 0x09BF, 0x09BF }, { 0x09C0, 0x09C4 }, { 0x09C7, 0x09C8 },
        { 0x09CB, 0x09CD }, { 0x09D7, 0x09D7 }, { 0x09E2, 0x09E3 }, { 0x0A02, 0x0A02 },
        { 0x0A3C, 0x0A3C }, { 0x0A3E, 0x0A3E }, { 0x0A3F, 0x0A3F }, { 0x0A40, 0x0A42 },
        { 0x0A47, 0x0A48 }, { 0x0A4B, 0x0A4D }, { 0x0A70, 0x0A71 }, { 0x0A81, 0x0A83 },
        { 0x0ABC, 0x0ABC }, { 0x0ABE, 0x0AC5 }, { 0x0AC7, 0x0AC9 }, { 0x0ACB, 0x0ACD },
        { 0x0B01, 0x0B03 }, { 0x0B3C, 0x0B3C }, { 0x0B3E, 0x0B43 }, { 0x0B47, 0x0B48 },
        { 0x0B4B, 0x0B4D }, { 0x0B56, 0x0B57 }, { 0x0B82, 0x0B83 }, { 0x0BBE, 0x0BC2 },
        { 0x0BC6, 0x0BC8 }, { 0x0BCA, 0x0BCD }, { 0x0BD7, 0x0BD7 }, { 0x0C01, 0x0C03 },
        { 0x0C3E, 0x0C44 }, { 0x0C46, 0x0C48 }, { 0x0C4A, 0x0C4D }, { 0x0C55, 0x0C56 },
        { 0x0C82, 0x0C83 }, { 0x0CBE, 0x0CC4 }, { 0x0CC6, 0x0CC8 }, { 0x0CCA, 0x0CCD },
        { 0x0CD5, 0x0CD6 }, { 0x0D02, 0x0D03 }, { 0x0D3E, 0x0D43 }, { 0x0D46, 0x0D48 },
        { 0x0D4A, 0x0D4D }, { 0x0D57, 0x0D57 }, { 0x0E31, 0x0E31 }, { 0x0E34, 0x0E3A },
        { 0x0E47, 0x0E4E }, { 0x0EB1, 0x0EB1 }, { 0x0EB4, 0x0EB9 }, { 0x0EBB, 0x0EBC },
        { 0x0EC8, 0x0ECD }, { 0x0F18, 0x0F19 }, { 0x0F35, 0x0F35 }, { 0x0F37, 0x0F37 },
        { 0x0F39, 0x0F39 }, { 0x0F3E, 0x0F3E }, { 0x0F3F, 0x0F3F }, { 0x0F71, 0x0F84 },
        { 0x0F86, 0x0F8B }, { 0x0F90, 0x0F95 }, { 0x0F97, 0x0F97 }, { 0x0F99, 0x0FAD },
        { 0x0FB1, 0x0FB7 }, { 0x0FB9, 0x0FB9 }, { 0x20D0, 0x20DC }, { 0x20E1, 0x20E1 },
        { 0x302A, 0x302F }, { 0x3099, 0x309A }
    };
static const unsigned int UTF8CombiningCharDim1 =
    sizeof(UTF8CombiningChar) / (sizeof(XML_UnicodeCharCode) * 2);

/*------------------------------------------------------------------------------*/
/* Following table is used by function: XML_UTF8IsDigit()                       */
/* to determine if a Unicode code character is a <Digit> as defined in XML      */
/* see also document: "http://www.w3.org/TR/REC-xml"                            */
static XML_UnicodeCharCode UTF8Digit[][2] =
    {
        { 0x0030, 0x0039 }, { 0x0660, 0x0669 }, { 0x06F0, 0x06F9 }, { 0x0966, 0x096F },
        { 0x09E6, 0x09EF }, { 0x0A66, 0x0A6F }, { 0x0AE6, 0x0AEF }, { 0x0B66, 0x0B6F },
        { 0x0BE7, 0x0BEF }, { 0x0C66, 0x0C6F }, { 0x0CE6, 0x0CEF }, { 0x0D66, 0x0D6F },
        { 0x0E50, 0x0E59 }, { 0x0ED0, 0x0ED9 }, { 0x0F20, 0x0F29 }
    };
static const unsigned int UTF8DigitDim1 =
    sizeof(UTF8Digit) / (sizeof(XML_UnicodeCharCode) * 2);

/*------------------------------------------------------------------------------*/
/* Following table is used by function: XML_UTF8IsExtender()                    */
/* to determine if a Unicode code character is a <Extender> as defined in XML   */
/* see also document: "http://www.w3.org/TR/REC-xml"                            */
static XML_UnicodeCharCode UTF8Extender[][2] =
    {
        { 0x00B7, 0x00B7 }, { 0x02D0, 0x02D0 }, { 0x02D1, 0x02D1 }, { 0x0387, 0x0387 },
        { 0x0640, 0x0640 }, { 0x0E46, 0x0E46 }, { 0x0EC6, 0x0EC6 }, { 0x3005, 0x3005 },
        { 0x3031, 0x3035 }, { 0x309D, 0x309E }, { 0x30FC, 0x30FE }
    };
static const unsigned int UTF8ExtenderDim1 =
    sizeof(UTF8Extender) / (sizeof(XML_UnicodeCharCode) * 2);

/*------------------------------------------------------------------------------*/
/* this is based on Roman Czyborra's description of the algorithm               */
SAPDB_Int XML_UTF8CharFromCode(SAPDB_UTF8 *dest, XML_UnicodeCharCode code )
{
    SAPDB_UTF8 *dest0 = dest;
    if (code < 0x80)
        *(dest++) = (char)code;
    else if (code < 0x800) {
        *(dest++) = 0xc0 | (char)(code>>6);
        *(dest++) = 0x80 | (char)code & 0x3f;
    }
    else if (code < 0x10000UL) {
        *(dest++) = 0xe0 | (char)(code>>12);
        *(dest++) = 0x80 | (char)(code>>6) & 0x3f;
        *(dest++) = 0x80 | (char)code & 0x3f;
    }
    else if (code < 0x200000UL) {
        *(dest++) = 0xf0 | (char)(code>>18);
        *(dest++) = 0x80 | (char)(code>>12) & 0x3f;
        *(dest++) = 0x80 | (char)(code>>6) & 0x3f;
        *(dest++) = 0x80 | (char)code & 0x3f;
    };
    return (dest - dest0);
}

/*------------------------------------------------------------------------------*/
/* this ought to return the Unicode equivalent of the UTF-8 char                */
XML_UnicodeCharCode XML_UTF8CharToCode( const SAPDB_UTF8 *text )
{
    XML_UnicodeCharCode code;
    int i;
    SAPDB_Int len = XML_UTF8SingleCharLength( text );
    if ( !len )
        return (XML_UnicodeCharCode) 0xFFFFFFFF;

    if (len == 1)
        return *text;
    code = (*text & (0xff >> (len + 1))); /* get 1st byte */
    for ( i = 1; i < len; i++ )
        code = (code << 6) | (text[i] & 0x3f);
    return code;
}

/*------------------------------------------------------------------------------*/
SAPDB_Int XML_UTF8GetChar(SAPDB_UTF8 *dest, const SAPDB_UTF8 *src)
{
    SAPDB_Int len = XML_UTF8SingleCharLength( src );
    SAPDB_memcpy ( dest, src, len );
    return len;
}

/*------------------------------------------------------------------------------*/
SAPDB_Bool XML_UTF8IsBaseChar( XML_UnicodeCharCode c )
{
    unsigned int i;
    SAPDB_Bool rc = SAPDB_FALSE;

    if ( !( c < UTF8BaseChar[0][0] ))
    {
        for ( i=1; i < UTF8BaseCharDim1; i++ )
            if ( c < UTF8BaseChar[i][0] )
                break;
        if ( c <= UTF8BaseChar[i-1][1] )
            rc = SAPDB_TRUE;
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
SAPDB_Bool XML_UTF8IsCombiningChar( XML_UnicodeCharCode c)
{
    unsigned int i;
    SAPDB_Bool rc = SAPDB_FALSE;

    if ( !(c < UTF8CombiningChar[0][0] ))
    {
        for ( i=1; i < UTF8CombiningCharDim1; i++ )
            if ( c < UTF8CombiningChar[i][0] )
                break;
        if ( c <= UTF8CombiningChar[i-1][1] )
            rc = SAPDB_TRUE;
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
SAPDB_Bool XML_UTF8IsDigit( XML_UnicodeCharCode c )
{
    unsigned int i;
    SAPDB_Bool rc = SAPDB_FALSE;

    if ( !(c < UTF8Digit[0][0] ))
    {
        for ( i=1; i < UTF8DigitDim1; i++ )
            if ( c < UTF8Digit[i][0] )
                break;
        if ( c <= UTF8Digit[i-1][1] )
            rc = SAPDB_TRUE;
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
SAPDB_Bool XML_UTF8IsExtender( XML_UnicodeCharCode c )
{
    unsigned int i;
    SAPDB_Bool rc = SAPDB_FALSE;

    if ( !(c < UTF8Extender[0][0] ))
    {
        for ( i=1; i < UTF8ExtenderDim1; i++ )
            if ( c < UTF8Extender[i][0] )
                break;
        if ( c <= UTF8Extender[i-1][1] )
            rc = SAPDB_TRUE;
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
SAPDB_Bool XML_UTF8IsIdeographic( XML_UnicodeCharCode c)
{
    unsigned int i;
    SAPDB_Bool rc = SAPDB_FALSE;

    if ( !(c < UTF8Ideographic[0][0] ))
    {
        for ( i=1; i < UTF8IdeographicDim1; i++ )
            if ( c < UTF8Ideographic[i][0] )
                break;
        if ( c <= UTF8Ideographic[i-1][1] )
            rc = SAPDB_TRUE;
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
SAPDB_Bool XML_UTF8IsLetter(XML_UnicodeCharCode c)
{
    SAPDB_Bool rc = SAPDB_FALSE;

    if ( XML_UTF8IsBaseChar( c ) || XML_UTF8IsIdeographic( c ) )
        rc = SAPDB_TRUE;
    return rc;
}

/*------------------------------------------------------------------------------*/
SAPDB_Bool XML_UTF8IsNameChar(XML_UnicodeCharCode c)
{
    SAPDB_Bool rc = SAPDB_FALSE;

    if ( XML_UTF8IsLetter( c ) || XML_UTF8IsDigit( c )
            || c == '.' || c == '-' ||  c =='_' ||  c ==':'
            || XML_UTF8IsCombiningChar( c ) || XML_UTF8IsExtender( c ) )
        rc = SAPDB_TRUE;
    return rc;
}

/*------------------------------------------------------------------------------*/
SAPDB_Int XML_UTF8SingleCharLength (const SAPDB_UTF8 *text)
{
    return UTF8SingleCharSize[ *text ];
}

/*------------------------------------------------------------------------------*/
SAPDB_UTF8* XML_UTF8SkipWhiteSpaces( SAPDB_UTF8 *p )
{
    return p + strspn( (char*)p, (char*)XML_Whitespaces );
}

/*------------------------------------------------------------------------------*/
SAPDB_UTF8* XML_UTF8NormalizeValue( SAPDB_UTF8 *p )
{
    SAPDB_UTF8 *pr = XML_UTF8SkipWhiteSpaces( p );
    SAPDB_UTF8 *pe, *ptmp;
    SAPDB_Bool insertWS = SAPDB_FALSE;

    p = pr;
    pe = p;

    while ( *p )
    {
        ptmp = XML_UTF8SkipWhiteSpaces( p );
        if ( p != ptmp )
        {
            p = ptmp;
            insertWS = SAPDB_TRUE;
        }
        else
        {
            if ( insertWS )
            {
                insertWS = SAPDB_FALSE;
                *pe = ' ';
                ++pe;
            }
            *pe = *p;
            ++pe;
            ++p;
        }
    }
    *pe = '\0';
    return pr;
}
