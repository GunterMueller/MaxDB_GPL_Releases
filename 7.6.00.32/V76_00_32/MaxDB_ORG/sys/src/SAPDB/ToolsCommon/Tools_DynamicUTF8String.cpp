  /*!***************************************************************************

  module:       Tools_DynamicUTF8String.cpp

  ------------------------------------------------------------------------------

  responsible:  TillL

  special area: 

  description:  see .hpp

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


#include "ToolsCommon/Tools_UnicodeCaseTable.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"



// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ElementIndex 
Tools_DynamicUTF8String::NPos = SAPDB_MAX_UINT4;

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertFromUCS2
    (UCS2ConstPointer               srcBeg,
     UCS2ConstPointer               srcEnd)
{
    assert(srcBeg != 0);
    assert(srcBeg <= srcEnd);

    m_Buffer.Clear();

    ConversionResult    result  = Tools_UTF8Basis::Success;
    UCS2ConstPointer    srcAt   = srcBeg;
    Pointer             destAt;
    int                 factor  = 3;

    while ((srcAt < srcEnd) && (result == Tools_UTF8Basis::Success))
    {
        if (!m_Buffer.ProvideCapacity(factor * (Tools_DynamicUTF8StringBuffer::SizeType) (srcEnd - srcBeg)))
            return Tools_UTF8Basis::TargetExhausted;

        Pointer destBeg = ToPtr(End()); // not Begin()!
        Pointer destEnd = ToPtr(Begin()) + Capacity();

        result = Tools_UTF8Basis::ConvertFromUCS2
            (srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);

        m_Buffer.SetElementCount((Tools_DynamicUTF8StringBuffer::SizeType) (destAt - ToPtr(Begin())));
        factor *= 2;
    }

    return result;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertFromUCS2_Unaligned_Swap
    (UCS2Char *srcBeg,
     UCS2Char *srcEnd)
{
    assert(srcBeg != 0);
    assert(srcBeg <= srcEnd);

    m_Buffer.Clear();

    ConversionResult result = Tools_UTF8Basis::Success;

    short swapTest = 1;
    
    UCS2Char const       *srcAt   = srcBeg;
    Pointer               destAt;
    int                   factor  = 3;

    while ((srcAt < srcEnd) && (result == Tools_UTF8Basis::Success))
    {
        if (!m_Buffer.ProvideCapacity(factor * (Tools_DynamicUTF8StringBuffer::SizeType) (srcEnd - srcBeg)))
            return Tools_UTF8Basis::TargetExhausted;

        Pointer destBeg = ToPtr(End()); // not Begin()!
        Pointer destEnd = ToPtr(Begin()) + Capacity();

        result = Tools_UTF8Basis::KernelConvertFromUTF16(srcBeg,
                                                         srcEnd,
                                                         srcAt,
                                                         ((*(char *)&swapTest) ? true : false),
                                                         destBeg,
                                                         destEnd,
                                                         destAt);

        m_Buffer.SetElementCount((Tools_DynamicUTF8StringBuffer::SizeType) (destAt - ToPtr(Begin())));
        factor *= 2;
    }

    return result;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertFromUCS2_Unaligned_Swap
    (UCS2Char        *srcBeg,
     UCS2Char        *srcEnd,
     const SAPDB_Int  srcSwapped)
{
    assert(srcBeg != 0);
    assert(srcBeg <= srcEnd);

    m_Buffer.Clear();

    ConversionResult result = Tools_UTF8Basis::Success;

    UCS2Char const *srcAt   = srcBeg;
    Pointer         destAt;
    int             factor  = 3;

    while ((srcAt < srcEnd) && (result == Tools_UTF8Basis::Success))
    {
        if (!m_Buffer.ProvideCapacity(factor * (Tools_DynamicUTF8StringBuffer::SizeType) (srcEnd - srcBeg)))
            return Tools_UTF8Basis::TargetExhausted;

        Pointer destBeg = ToPtr(End()); // not Begin()!
        Pointer destEnd = ToPtr(Begin()) + Capacity();

        result = Tools_UTF8Basis::KernelConvertFromUTF16(srcBeg, srcEnd, srcAt, srcSwapped, destBeg, destEnd, destAt);

        m_Buffer.SetElementCount((Tools_DynamicUTF8StringBuffer::SizeType) (destAt - ToPtr(Begin())));
        factor *= 2;
    }

    return result;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertFromInt
  ( SAPDB_Int8   nNumber,
    SAPDB_Int    nBase )
{
  SAPDB_UTF8   aBuffer[30];
  SAPDB_UTF8*  pCurrent = 0;
  SAPDB_UTF8   Sign     = 0;
  SAPDB_Int    nDigit   = 0;

  pCurrent = &(aBuffer[29]);

  *pCurrent = 0;

  --pCurrent;

  if (nNumber < 0) {
    nNumber = nNumber * (-1);
    Sign = '-';
  } // end if

  if (nNumber > 0) {
    while (nNumber > 0) {
      nDigit = (SAPDB_Int) (nNumber % nBase);
      nNumber = (nNumber - nDigit) / nBase;
      switch (nDigit) {
        case  0:*pCurrent = '0';break;
        case  1:*pCurrent = '1';break;
        case  2:*pCurrent = '2';break;
        case  3:*pCurrent = '3';break;
        case  4:*pCurrent = '4';break;
        case  5:*pCurrent = '5';break;
        case  6:*pCurrent = '6';break;
        case  7:*pCurrent = '7';break;
        case  8:*pCurrent = '8';break;
        case  9:*pCurrent = '9';break;
        case 10:*pCurrent = 'A';break;
        case 11:*pCurrent = 'B';break;
        case 12:*pCurrent = 'C';break;
        case 13:*pCurrent = 'D';break;
        case 14:*pCurrent = 'E';break;
        case 15:*pCurrent = 'F';break;
      } // end switch
      --pCurrent;
    } // end while
    if (Sign != 0) {
      *pCurrent = Sign;
    } else {
      ++pCurrent;
    } // end if
  } else {
    *pCurrent = '0';
  } // end if

  Assign(pCurrent);

  return Tools_UTF8Basis::Success;
} 

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertToUCS2
    (UCS2Pointer                    destBeg,
     UCS2ConstPointer               destEnd,
     UCS2Pointer&                   destAt) const
{
    assert(destBeg != 0);
    assert(destBeg <= destEnd);

    ConstPointer srcBeg = ToPtr(Begin());
    ConstPointer srcEnd = ToPtr(End());
    ConstPointer srcAt;

    return Tools_UTF8Basis::ConvertToUCS2
                (srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertToUCS2_Unaligned_Swap
    (UCS2Char *        destBeg,
     UCS2Char * const  destEnd,
     UCS2Char *&       destAt) const
{
    assert(destBeg != 0);
    assert(destBeg <= destEnd);

    short swapTest = 1;

    ConstPointer srcBeg = ToPtr(Begin());
    ConstPointer srcEnd = ToPtr(End());
    ConstPointer srcAt;

    return Tools_UTF8Basis::KernelConvertToUTF16(srcBeg,
                                                 srcEnd,
                                                 srcAt,
                                                 destBeg,
                                                 destEnd,
                                                 destAt,
                                                 ((*(char *)&swapTest) ? true : false));
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertToUCS2_Unaligned_Swap
    (UCS2Char *        destBeg,
     UCS2Char * const  destEnd,
     UCS2Char *&       destAt,
     const bool        destSwapped) const
{
    assert(destBeg != 0);
    assert(destBeg <= destEnd);

    ConstPointer srcBeg = ToPtr(Begin());
    ConstPointer srcEnd = ToPtr(End());
    ConstPointer srcAt;

    return Tools_UTF8Basis::KernelConvertToUTF16(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt, destSwapped);
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertFromASCII_Latin1
    (ASCIIConstPointer               srcBeg,
     ASCIIConstPointer               srcEnd)
{
    assert(srcBeg != 0);
    assert(srcBeg <= srcEnd);

    m_Buffer.Clear();

    ConversionResult    result  = Tools_UTF8Basis::Success;
    ASCIIConstPointer   srcAt   = srcBeg;
    Pointer             destAt;

    if (!m_Buffer.ProvideCapacity(2 * (Tools_DynamicUTF8StringBuffer::SizeType) (srcEnd - srcBeg)))
        return Tools_UTF8Basis::TargetExhausted;

    const Pointer destBeg = ToPtr(Begin());
    const Pointer destEnd = destBeg + Capacity();

    result = Tools_UTF8Basis::ConvertFromASCII
        (srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);

    m_Buffer.SetElementCount((Tools_DynamicUTF8StringBuffer::SizeType) (destAt - ToPtr(Begin())));

    return result;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::ConversionResult 
Tools_DynamicUTF8String::ConvertToASCII_Latin1
    (ASCIIPointer                    destBeg,
     ASCIIConstPointer               destEnd,
     ASCIIPointer&                   destAt,
     SAPDB_Char                      replChar) const
{
    assert(destBeg  != 0);
    assert(destBeg  <= destEnd);
    assert(replChar != 0);

    ConstPointer srcBeg = ToPtr(Begin());
    ConstPointer srcEnd = ToPtr(End());
    ConstPointer srcAt;
    SAPDB_UInt   replCount;

    return Tools_UTF8Basis::ConvertToASCII
                (srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt, 
                 replChar, replCount);
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::BasisElementIndex 
Tools_DynamicUTF8String::FindSequence
    (BasisElementIndex              from,
     const ConstIterator&           seqBeg,
     const ConstIterator&           seqEnd) const
{
    assert(ToPtr(seqBeg) <= ToPtr(seqEnd));

    if (ToPtr(seqBeg) == ToPtr(seqEnd))    return 0;
    if (from >= BasisSize())               return NPos;

    ConstIterator     strIter    = GetIteratorAtBasis(from);
    ConstIterator     strEnd     = End();
    BasisElementCount seqLen     = StringSize(seqBeg, seqEnd);
    SAPDB_Int         seqByteLen = seqLen * sizeof(BasisElementType);

    if (seqLen == 0)                        return from;

    ConstIterator     endIter    = ToPtr(strEnd);

    // while match with first element of sequence found:
    while ((StringSize(strIter, strEnd) >= seqLen) 
           && 
           (strIter = GetMatchPosition(strIter, endIter, seqBeg)) != ConstIterator())
    {
        // check if whole sequence matches:
        if (ElementType::Compare(ToPtr(strIter), ToPtr(seqBeg), seqByteLen) == 0)
        {
            return ElementType::IterDiff(Begin(), strIter);
        }

        ++strIter;
    }
    return NPos;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::BasisElementIndex 
Tools_DynamicUTF8String::ReverseFindSequence
    (BasisElementIndex              from,
     const ConstIterator&           seqBeg,
     const ConstIterator&           seqEnd) const
{
    assert(ToPtr(seqBeg) <= ToPtr(seqEnd));

    if (ToPtr(seqBeg) == ToPtr(seqEnd))    return BasisSize();

    ConstReverseIterator strIter;
    
    if (from == NPos)
    {
        from    = BasisSize();
        strIter = RBegin();
    }
    else
    {
        // This differs from the "standard" found at MSVC++6.0,
        // where 'from' parameters are handled inconsistently 
        // in the Find() and RFind() methods.
        // The form below is consistent with FindSequence() and Find():
        if (from >= BasisSize()) return NPos;

        strIter = GetReverseIteratorAtBasis(from);
    }

    ConstIterator        strBegin   = Begin();
    ConstReverseIterator endIter    = REnd();
    BasisElementCount    seqLen     = StringSize(seqBeg, seqEnd);
    SAPDB_Int            seqByteLen = seqLen * sizeof(BasisElementType);

    if (seqLen == 0)        return from;

    // while match with first element of sequence found:
    while ((strIter = GetReverseMatchPositionI(strIter, endIter, seqBeg)) 
           != ConstReverseIterator())
    {
        ConstPointer pScan = ToPtr(strIter);

        // check if whole sequence matches:
        if ((StringSize(ToPtr(strBegin), pScan) + seqLen - 1 <= from) &&
            (ElementType::Compare(pScan, ToPtr(seqBeg), seqByteLen) == 0))
            return ElementType::IterDiff(strBegin, ConstIterator(pScan));

        ++strIter;
    }

    return NPos;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::BasisElementIndex 
Tools_DynamicUTF8String::FindFirstOfSet
    (BasisElementIndex              from,
     const ConstIterator&           setBeg,
     const ConstIterator&           setEnd) const
{
    assert(ToPtr(setBeg) <= ToPtr(setEnd));

    if ((from > BasisSize())   ||
        (StringSize(setBeg, setEnd) == 0)) return NPos;

    ConstIterator     strEnd    = End();
    ConstIterator     strIter   = GetIteratorAtBasis(from);

    while (strIter != strEnd)
    {
        if (GetMatchPosition(setBeg, setEnd, strIter) != ConstIterator())
        {
            return ElementType::IterDiff(Begin(), strIter);
        }
        ++strIter;
    }
    return NPos;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::BasisElementIndex 
Tools_DynamicUTF8String::FindLastOfSet
    (BasisElementIndex              from,
     const ConstIterator&           setBeg,
     const ConstIterator&           setEnd) const
{
    assert(ToPtr(setBeg) <= ToPtr(setEnd));

    ConstReverseIterator strIter;

    if (from == NPos)                                              
    {
        strIter = RBegin();                                        
    }
    else
    {
        if (from >= BasisSize()) return NPos;                      
        strIter = GetReverseIteratorAtBasis(from);                 
    }

    if (StringSize(setBeg, setEnd) == 0) return NPos;              

    ConstReverseIterator endIter    = REnd();

    while (strIter != endIter)
    {
        if (GetReverseMatchPositionII(setBeg, setEnd, strIter) !=  
            ConstReverseIterator())
        {
            return ElementType::IterDiff(Begin(), ToPtr(strIter));
        }
        ++strIter;
    }
    return NPos;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::BasisElementIndex 
Tools_DynamicUTF8String::FindFirstNotOfSet
    (BasisElementIndex              from,
     const ConstIterator&           setBeg,
     const ConstIterator&           setEnd) const
{
    assert(ToPtr(setBeg) <= ToPtr(setEnd));

    if ((from > BasisSize())   ||
        (StringSize(setBeg, setEnd) == 0)) return NPos;

    ConstIterator     strEnd    = End();
    ConstIterator     strIter   = GetIteratorAtBasis(from);

    while (strIter != strEnd)
    {
        if (GetMatchPosition(setBeg, setEnd, strIter) == ConstIterator())
        {
            return ElementType::IterDiff(Begin(), strIter);
        }
        ++strIter;
    }
    return NPos;
}

// -----------------------------------------------------------------------------
Tools_DynamicUTF8String::BasisElementIndex 
Tools_DynamicUTF8String::FindLastNotOfSet
    (BasisElementIndex              from,
     const ConstIterator&           setBeg,
     const ConstIterator&           setEnd) const
{
    assert(ToPtr(setBeg) <= ToPtr(setEnd));

    ConstReverseIterator strIter;

    if (from == NPos)                                              
    {
        strIter = RBegin();                                        
    }
    else
    {
        if (from >= BasisSize()) return NPos;                      
        strIter = GetReverseIteratorAtBasis(from);                 
    }

    if (StringSize(setBeg, setEnd) == 0) return NPos;              

    ConstReverseIterator endIter    = REnd();

    while (strIter != endIter)
    {
        if (GetReverseMatchPositionII(setBeg, setEnd, strIter) ==  
            ConstReverseIterator())
        {
            return ElementType::IterDiff(Begin(), ToPtr(strIter));
        }
        ++strIter;
    }
    return NPos;
}
// -----------------------------------------------------------------------------
Tools_DynamicUTF8String& Tools_DynamicUTF8String::ToUpper()
{
    ConstIterator    readBeg = Begin();
    ConstIterator    readEnd = Empty() ? ConstIterator(End()) : readBeg + 1;
    ConstPointer     readAt;
    SAPDB_UCS2       ucs2[2];
    UCS2Pointer      ucs2destAt;
    UCS2ConstPointer ucs2srcAt;
    Pointer          writeBeg = ToPtr(Begin());
    ConstPointer     writeEnd = ToPtr(End());
    Pointer          writeAt;

    while (readBeg != readEnd && writeBeg < writeEnd)
    {
        ucs2[1] = 0;
        Tools_UTF8Basis::ConvertToUCS2
            (ToPtr(readBeg), ToPtr(readEnd), readAt, ucs2, ucs2+2, ucs2destAt);

        if (ucs2[1] == 0)
            ucs2[0] = Tools_UCS2ToUpper(ucs2[0]);

        Tools_UTF8Basis::ConvertFromUCS2
            (ucs2, ucs2 + (ucs2[1] == 0 ? 1 : 2), ucs2srcAt, writeBeg, writeEnd, writeAt);

        readBeg  = readEnd++;
        writeBeg = writeAt;
    }

    return *this;
}
// -----------------------------------------------------------------------------
Tools_DynamicUTF8String& Tools_DynamicUTF8String::ToLower()
{
    ConstIterator    readBeg = Begin();
    ConstIterator    readEnd = Empty() ? ConstIterator(End()) : readBeg + 1;
    ConstPointer     readAt;
    SAPDB_UCS2       ucs2[2];
    UCS2Pointer      ucs2destAt;
    UCS2ConstPointer ucs2srcAt;
    Pointer          writeBeg = ToPtr(Begin());
    ConstPointer     writeEnd = ToPtr(End());
    Pointer          writeAt;

    while (readBeg != readEnd && writeBeg < writeEnd)
    {
        ucs2[1] = 0;
        Tools_UTF8Basis::ConvertToUCS2
            (ToPtr(readBeg), ToPtr(readEnd), readAt, ucs2, ucs2+2, ucs2destAt);

        if (ucs2[1] == 0)
            ucs2[0] = Tools_UCS2ToLower(ucs2[0]);

        Tools_UTF8Basis::ConvertFromUCS2
            (ucs2, ucs2 + (ucs2[1] == 0 ? 1 : 2), ucs2srcAt, writeBeg, writeEnd, writeAt);

        readBeg  = readEnd++;
        writeBeg = writeAt;
    }

    return *this;
}



