/****************************************************************************

  module      : Data_Row.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  description : definition of a row of a sql table 
  last changed: 2001-02-26  9:49
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "DataAccess/Data_Row.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill

bool Data_Row::CopyRow (Data_Row& Destination) const
{
    if (GetVarColOffset() < 0)
    {
        return this->Unpack (Destination);
    }
    else
    {
        tgg00_BasisError e = e_ok;
        SAPDB_RangeMove( __FILE__,   1,
            sizeof(*this), sizeof(Destination), 
            this, 1, &Destination, 1, GetRowLen(), e); 
        return (e_ok == e);
    }
}

//-----------------------------------------------------------------------------

namespace Data_Row_Namespace {
/*!
   @brief bit wise read and write of a packed row
 */
class Data_BitRow
{
public :
    /*!
       @brief constructor, positions to first byte after offset
     */
    inline Data_BitRow(Data_Row& Row, int Offset, bool doRead);
    /*!
       @brief stores the value in 'length' bits
     */
    inline void Append(SAPDB_Byte value, int length);
    /*!
       @brief returns the row length
     */
    inline int GetLength() const;
    /*!
       @brief reads 'length' bits and returns the value as byte
     */
    inline SAPDB_Byte Read(int length);
private :
    SAPDB_Byte* m_pCurr;   //!< current byte
    int         m_offset;  //!< offset in row
    Data_Row&   m_row;     //!< current row
};
};

using namespace Data_Row_Namespace;

//-----------------------------------------------------------------------------

inline Data_BitRow::Data_BitRow(Data_Row& Row, int Offset, bool DoRead) 
: m_pCurr (&Row.recBody_gg00()[Offset])
, m_offset(0)
, m_row(Row)
{
    if (!DoRead)
    {
        *m_pCurr = 0;
    }
}

//-----------------------------------------------------------------------------

inline void Data_BitRow::Append(SAPDB_Byte value, int length)
{
    int rest = 8 - m_offset;
    if (rest == length)
    {
        *m_pCurr |= value;
        ++m_pCurr;
        *m_pCurr = 0;
        m_offset = 0;
    }
    else
    {
        if (rest > length)
        {
            value <<= 8 - length;
            value >>= m_offset;
            *m_pCurr |= value;
            m_offset += length;
        }
        else
        {
            *m_pCurr |= (value << (8 - length)) >> m_offset;
            ++m_pCurr;
            m_offset = length - rest;
            *m_pCurr = value << (8 - m_offset);
        }
    }
}

//-----------------------------------------------------------------------------

inline int Data_BitRow::GetLength() const
{
    if (0 == m_offset)
    {
        return m_pCurr - reinterpret_cast<SAPDB_Byte*>(&m_row);
    }
    else
    {
        return m_pCurr - reinterpret_cast<SAPDB_Byte*>(&m_row) + 1;
    }
}

//-----------------------------------------------------------------------------

inline SAPDB_Byte Data_BitRow::Read(int length) 
{
    int rest       = 8 - m_offset;
    SAPDB_Byte res = (*m_pCurr << m_offset);
    res = (res >> (8 - length));
    if (rest == length)
    {
        m_offset = 0;
        ++m_pCurr;
    }
    else
    {
        if (rest > length)
        {
            m_offset += length;
        }
        else
        {
            ++m_pCurr;
            m_offset = length - rest;
            res |= (*m_pCurr >> (8 - m_offset));
        }
    }
    return res;
}

//-----------------------------------------------------------------------------

#ifndef __test

bool Data_Row::Pack (Data_Row& PackedRow) const
{
    if (IsPacked())
    {
        return false;
    }
    const SAPDB_Byte* p = reinterpret_cast<const SAPDB_Byte*>(&recBody_gg00()[0]) + GetKeyLen() + GetVarColOffset();
    Data_BitRow packedRow(PackedRow, GetKeyLen() + GetVarColOffset(), false); 
    for (int colno = 0; colno < GetVarColCount(); ++colno)
    {
        unsigned int length = *p++;
        if (length > 15)
        {
            return false;
        }
        if (*p != 0)
        {
            return false;
        }
        ++p;
        if ((0x80 == *p) && 2 == length)
        {
            packedRow.Append(0, 4);
            ++p;
            continue;
        }
        packedRow.Append(length, 4);
        packedRow.Append(*p++, 8);
        while (length-- > 2)
        {
            SAPDB_Byte value = (*p >> 4) * 10 + (*p & 0x0F); 
            if (value > 127)
            {
                return false;
            }
             packedRow.Append(value, 7);
             ++p;
        }
    }
    SAPDB_MemCopyNoCheck (&PackedRow.recBody_gg00()[0], &recBody_gg00()[0], GetKeyLen() + GetVarColOffset());
    PackedRow.SetRowLen (packedRow.GetLength());
    PackedRow.SetKeyLen (GetKeyLen());
    PackedRow.SetVarColCount(GetVarColCount());
    PackedRow.SetVarColOffset(-GetVarColOffset() - 1);
    return true;
}

//-----------------------------------------------------------------------------

static SAPDB_Byte DigitMap[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                                 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                                 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                                 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
                                 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
                                 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
                                 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
                                 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
                                 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                                 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99 };

//-----------------------------------------------------------------------------

bool Data_Row::Unpack (Data_Row& UnpackedRow) const
{
    if (IsPacked())
    {
        UnpackedRow.SetKeyLen(GetKeyLen());
        UnpackedRow.SetVarColCount(GetVarColCount());
        UnpackedRow.SetVarColOffset(-GetVarColOffset() - 1);
        int offset = UnpackedRow.GetKeyLen() + UnpackedRow.GetVarColOffset();
        SAPDB_MemCopyNoCheck (&UnpackedRow.recBody_gg00()[0], &recBody_gg00()[0], offset);
        Data_BitRow packedRow(*const_cast<Data_Row*>(this), offset, true); 
        SAPDB_Byte* pDest = &UnpackedRow.recBody_gg00()[offset];       
        for (int colno = 0; colno < GetVarColCount(); ++colno)
        {
            int length = packedRow.Read(4);
            if (0 == length)
            {
                *pDest++ = 2;
                *pDest++ = 0;
                *pDest++ = 0x80;
                continue;
            }
            *pDest++   = length;
            *pDest++   = 0;
            *pDest++   = packedRow.Read(8); // exponent
            while (length-- > 2)
            {
                int value = packedRow.Read(7); 
                //*pDest++  = ((value / 10) << 4) | (value % 10);
                *pDest++ = DigitMap[value];
            }
        }
       UnpackedRow.SetRowLen(pDest - (SAPDB_Byte*) &UnpackedRow);
       return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

#else

externC void vabort(bool);

bool Data_Row::Pack (Data_Row& PackedRow) const
{
    if (IsPacked())
    {
        return false;
    }
    const SAPDB_Byte* p = reinterpret_cast<const SAPDB_Byte*>(&recBody_gg00()[0]) + GetKeyLen() + GetVarColOffset();
    Data_BitRow packedRow(PackedRow, GetKeyLen() + GetVarColOffset(), false); 
    Data_BitRow reader(PackedRow, GetKeyLen() + GetVarColOffset(), true); 
    for (int colno = 0; colno < GetVarColCount(); ++colno)
    {
        unsigned int length = *p++;
        packedRow.Append(length, 8);
        switch (*p)
        {
        case 0x00 :
            packedRow.Append(0, 2);
            break;
        case 0x01 :
            packedRow.Append(1, 2);
            break;
        case 0x20 :
            packedRow.Append(2, 2);
            break;
       case 0xFF :
            packedRow.Append(3, 2);
            break;
       default :
           return false;
        }
        ++p;
        while (length-- > 1)
        {
             packedRow.Append(*p++, 8);
        }
    }
    SAPDB_MemCopyNoCheck (&PackedRow.recBody_gg00()[0], &recBody_gg00()[0], GetKeyLen() + GetVarColOffset());
    PackedRow.SetRowLen (packedRow.GetLength());
    PackedRow.SetKeyLen (GetKeyLen());
    PackedRow.SetVarColCount(GetVarColCount());
    PackedRow.SetVarColOffset(-GetVarColOffset() - 1);
    SAPDB_Byte* pp = (SAPDB_Byte*) &PackedRow;
    pp += PackedRow.GetRowLen();
    int restLen = GetRowLen() - (p - (SAPDB_Byte*) this);
    if (restLen > 0)
    {
        SAPDB_MemCopyNoCheck (pp, p, restLen);
        PackedRow.recLen_gg00() += restLen;
    }
    Data_Row check;
    pp = (SAPDB_Byte*) &PackedRow;
    pp += PackedRow.GetRowLen();
    memset (pp, 0xFF, sizeof(PackedRow) - PackedRow.GetRowLen());
    PackedRow.Unpack(check);
    if (0 != memcmp(&check, this, GetRowLen()))
    {
        vabort(true);
    }
    return true;
}

//-----------------------------------------------------------------------------

static SAPDB_Byte DigitMap[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                                 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                                 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                                 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
                                 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
                                 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
                                 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
                                 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
                                 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                                 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99 };

//-----------------------------------------------------------------------------

bool Data_Row::Unpack (Data_Row& UnpackedRow) const
{
    if (IsPacked())
    {
        UnpackedRow.SetKeyLen(GetKeyLen());
        UnpackedRow.SetVarColCount(GetVarColCount());
        UnpackedRow.SetVarColOffset(-GetVarColOffset() - 1);
        int offset = UnpackedRow.GetKeyLen() + UnpackedRow.GetVarColOffset();
        SAPDB_MemCopyNoCheck (&UnpackedRow.recBody_gg00()[0], &recBody_gg00()[0], offset);
        Data_BitRow packedRow(*const_cast<Data_Row*>(this), offset, true); 
        SAPDB_Byte* pDest = &UnpackedRow.recBody_gg00()[offset];       
        for (int colno = 0; colno < GetVarColCount(); ++colno)
        {
            int length = packedRow.Read(8);
            *pDest++   = length;
            switch (packedRow.Read(2))
            {
            case 0 :
                *pDest++  = 0x00;
                break;
            case 1 :
                *pDest++  = 0x01;
                break;
            case 2 :
                *pDest++  = 0x20;
                break;
            case 3 :
                *pDest++  = 0xFF;
            }
            while (length-- > 1)
            {
                *pDest++ = packedRow.Read(8); 
            }
        }
       UnpackedRow.SetRowLen(pDest - (SAPDB_Byte*) &UnpackedRow);
       int restLen = GetRowLen() - packedRow.GetLength();
       if (restLen > 0)
       {
           SAPDB_Byte* pCompressed = (SAPDB_Byte*) this + packedRow.GetLength();
           SAPDB_MemCopyNoCheck (pDest, pCompressed, restLen);
           UnpackedRow.recLen_gg00() += restLen;
       }
       return true;
    }
    return false;
}

#endif