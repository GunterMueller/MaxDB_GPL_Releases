/*!*****************************************************************************

  module:       Tools_DynamicUTF8String.hpp

  ------------------------------------------------------------------------------

  responsible:  Till Luhmann

  special area: 

  description:  UTF8 string class for the SAPDB tools group

                - no exceptions
                - no templates
                - uses new and delete

  see also:     

  copyright:            Copyright (c) 2000-2005 SAP AG

  ------------------------------------------------------------------------------


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




*******************************************************************************/


#ifndef Tools_DynamicUTF8String_HPP
#define Tools_DynamicUTF8String_HPP

#include "ToolsCommon/Tools_DynamicUTF8StringBuffer.hpp"

#define _Tools_UTF8Ptr(_a)  ((Tools_DynamicUTF8String::ConstPointer) _a)
#define _Tools_CharPtr(_a)  ((const char *)                          _a)

/*!*****************************************************************************

    class: Tools_DynamicUTF8String

*******************************************************************************/
class Tools_DynamicUTF8String
{
public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Static Members and Data Types                                */
    /*!-------------------------------------------------------------------------
    declaration : ElementType 
    description:  Element type of a string
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8StringElement                 ElementType;
    /*!-------------------------------------------------------------------------
    declaration : BasisElementType
    description:  Basis type of the string element (this is byte here)
    --------------------------------------------------------------------------*/
    typedef ElementType::BasisElementType           BasisElementType;
    /*!-------------------------------------------------------------------------
    declaration : Pointer
    description:  Pointer to data of a string
    --------------------------------------------------------------------------*/
    typedef ElementType::Pointer                    Pointer;
    /*!-------------------------------------------------------------------------
    declaration : ConstPointer
    description:  Pointer to constant data of a string
    --------------------------------------------------------------------------*/
    typedef ElementType::ConstPointer               ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : Iterator
    description:  Iterator referencing data of a string
    --------------------------------------------------------------------------*/
    typedef ElementType::Iterator                   Iterator;
    /*!-------------------------------------------------------------------------
    declaration : ConstIterator
    description:  Iterator referencing constant data of a string
    --------------------------------------------------------------------------*/
    typedef ElementType::ConstIterator              ConstIterator;
    /*!-------------------------------------------------------------------------
    declaration : ReverseIterator
    description:  Reverse iterator referencing data of a string
    --------------------------------------------------------------------------*/
    typedef ElementType::ReverseIterator            ReverseIterator;
    /*!-------------------------------------------------------------------------
    declaration : ConstReverseIterator
    description:  Reverse iterator referencing constant data of a string
    --------------------------------------------------------------------------*/
    typedef ElementType::ConstReverseIterator       ConstReverseIterator;
    /*!-------------------------------------------------------------------------
    declaration : BufferType
    description:  Type of the string buffer 
    --------------------------------------------------------------------------*/
    typedef Tools_DynamicUTF8StringBuffer           BufferType;
    /*!-------------------------------------------------------------------------
    declaration : BasisElementCount
    description:  Count of basis elements of a string 
                  (one string element may consist of several contiguous basis 
                   elements)
    --------------------------------------------------------------------------*/
    typedef BufferType::SizeType                    BasisElementCount;
    /*!-------------------------------------------------------------------------
    declaration : BasisElementIndex 
    description:  Position within the sequence of basis elements of a string
    --------------------------------------------------------------------------*/
    typedef BasisElementCount                       BasisElementIndex;
    /*!-------------------------------------------------------------------------
    declaration : ElementCount
    description:  Count of elements of a string
    --------------------------------------------------------------------------*/
    typedef BasisElementCount                       ElementCount;
    /*!-------------------------------------------------------------------------
    declaration : ElementIndex 
    description:  Element position within a string 
                  (this is a logical, not a physical index!)
    --------------------------------------------------------------------------*/
    typedef ElementCount                            ElementIndex;

    /*!-------------------------------------------------------------------------
    declaration : ConversionResult
    description:  Result of string conversion
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8Basis::ConversionResult       ConversionResult;
    /*!-------------------------------------------------------------------------
    declaration : UCS2Pointer
    description:  Pointer to ucs-2 data
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8Basis::UCS2Pointer            UCS2Pointer;
    /*!-------------------------------------------------------------------------
    declaration : UCS2ConstPointer
    description:  Pointer to constant ucs-2 data
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8Basis::UCS2ConstPointer       UCS2ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : ASCIIPointer
    description:  Pointer to ascii data
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8Basis::ASCIIPointer           ASCIIPointer;
    /*!-------------------------------------------------------------------------
    declaration : UCS2Char
    description:  PUCS2 character element
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8Basis::UCS2Char               UCS2Char;
    /*!-------------------------------------------------------------------------
    declaration : ASCIIConstPointer
    description:  Pointer to constant ascii data
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8Basis::ASCIIConstPointer      ASCIIConstPointer;

    /*!-------------------------------------------------------------------------
    declaration:  NPos
    description:  Returns invalid element index position which is used in several
                  string functions
    --------------------------------------------------------------------------*/
    static ElementIndex NPos;

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Static Members and Data Types                             */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and Destructors                                 */
    /*!-------------------------------------------------------------------------
    function:     Tools_DynamicUTF8String()
    description:  Constructor; constructs empty string

    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    explicit Tools_DynamicUTF8String
        ()
    {}

    /*!-------------------------------------------------------------------------
    function:     Tools_DynamicUTF8String()
    description:  Copy constructor

                  - Assigns string 'str'.
                  - If 'from' is given, a substring of 'str' beginning at 
                    buffer index 'from' and ending at the string end is assigned.
                  - 'from' may range from 0 to 'str'.BasisSize().
                  - If 'len' is specified, at most 'len' characters are assigned.
                  
    arguments:    str    [in]   string to be assigned
                  from   [in]   buffer element position within 'str'
                  len    [in]   max. count of string elements to be assigned
    return value: none
    prototypes:   3
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String
        (const Tools_DynamicUTF8String& str)
    {
        AssignSource(str);
    }

    Tools_DynamicUTF8String
        (const Tools_DynamicUTF8String& str,
         BasisElementIndex              from)
    {
        AssignSource(str, from);
    }

    Tools_DynamicUTF8String
        (const Tools_DynamicUTF8String& str,
         BasisElementIndex              from,
         ElementCount                   len)
    {
        AssignSource(str, from, len);
    }

    /*!-------------------------------------------------------------------------
    function:     Tools_DynamicUTF8String()
    description:  Constructor; assigns memory block

                  - A copy of 'src' is assigned to the present string.
                  - Null pointer 'src' is not allowed.
                  - If only 'src' is given, the copy extends up to the 
                    first null-terminator encountered within 'src'.
                  - If 'srclen' is provided, it specifies the absolute count
                    of elements after 'src' to be assigned. In this case,
                    null-terminators are treated and assigned as normal elements.
                  - The buffer length of the string may not exceed MaxSize().
                  - The SAPDB_Char* operator version works properly 
                    only with 7-Bit ascii characters.
    arguments:    src    [in]   memory block to be assigned
                  srclen [in]   absolute count of elements of 'src' to be assigned
    return value: none
    prototypes:   3
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String
        (const SAPDB_Char*              src)
    {
        assert(IsAscii7(src));
        Assign((ConstPointer)src);
    }

    Tools_DynamicUTF8String
        (ConstPointer                   src)
    {
        Assign(src);
    }

    Tools_DynamicUTF8String
        (ConstPointer                   src, 
         ElementCount                   srclen)
    {
        Assign(src, srclen);
    }

    /*!-------------------------------------------------------------------------
    function:     Tools_DynamicUTF8String()
    description:  Constructor; assigns pattern 
                  
                  - Assigns pattern consisting of 'count' copies of a 
                    given single element.
                  - The buffer length of the string may not exceed MaxSize().

    arguments:    count  [in]   number of copies of 'elem'
                  elem   [in]   element to be assigned
    return value: none
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String
        (ElementCount                   count,
         BasisElementType               elem)
    {
        Assign(count, elem);
    }

    /*!-------------------------------------------------------------------------
    function:     Tools_DynamicUTF8String()
    description:  Constructor; assigns string between two given iterators

                  - The string found between iterator 'from' and 'to' is
                    assigned to the constructed string.
                  - The buffer length of the string may not exceed MaxSize().

    arguments:    from   [in]   iterator positioned at beginning of string
                  to     [in]   iterator positioned at end of string
    return value: none
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String
        (ConstIterator                  from,
         ConstIterator                  to)
    {
        Assign(from, to);
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and Destructors                              */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Status functions                                             */
    /*!-------------------------------------------------------------------------
    function:     Empty()
    description:  Checks whether string is empty
    arguments:    none
    return value: (SAPDB_Bool) 
    --------------------------------------------------------------------------*/
    SAPDB_Bool Empty() const
    {
        return IsAssigned() ? m_Buffer.IsEmpty() : true;
    }
    
    /*!-------------------------------------------------------------------------
    function:     Size()
    description:  Returns the count of elements in the string
    arguments:    none
    return value: (ElementCount) 
    --------------------------------------------------------------------------*/
    ElementCount Size() const
    {
        if (!IsAssigned())
            return 0;

        ConstIterator iter = Begin();
        ConstIterator end  = End();
        ElementCount count = 0;
        while (ToPtr(iter) < ToPtr(end))
        {
            ++iter;
            ++count;
        }
        return count;
    }

    /*!-------------------------------------------------------------------------
    function:     BasisSize()
    description:  Returns count of buffer elements in string buffer
    arguments:    none
    return value: (BasisElementCount)
    --------------------------------------------------------------------------*/
    BasisElementCount BasisSize() const
    {
        return IsAssigned() ? m_Buffer.ElementCount() : 0;
    }

    /*!-------------------------------------------------------------------------
    function:     Length()
    description:  Same as Size()
    arguments:    none
    return value: (ElementCount) 
    --------------------------------------------------------------------------*/
    ElementCount Length() const
    {
        return Size();
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Status functions                                          */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Capacity functions                                           */
    /*!-------------------------------------------------------------------------
    function:     Reserve()
    description:  Reserves buffer space for given number of buffer elements

                  - If actual Capacity() exceeds 'count', nothing is done.
    arguments:    count [in]    requested count of elements
    return value: none
    --------------------------------------------------------------------------*/
    void Reserve
        (BasisElementCount                count)
    {
        CheckSizeOverflow(count);
        m_Buffer.ProvideCapacity(count, BasisSize());
    }

    /*!-------------------------------------------------------------------------
    function:     Capacity()
    description:  Returns the actual buffer capacity of the string
    
                  - The count of elements which can be 
                    accepted without reallocation is returned.
    arguments:    none
    return value: (BasisElementCount) 
    --------------------------------------------------------------------------*/
    BasisElementCount Capacity() const
    {
        return IsAssigned() ? m_Buffer.Capacity() : 0;
    }

    /*!-------------------------------------------------------------------------
    function:     MaxSize()
    description:  Returns the maximum length of a string in elements
    
                  - The maximum count of elements which can be 
                    accepted  w i t h  reallocation is returned.
    arguments:    none
    return value: (BasisElementCount) 
    --------------------------------------------------------------------------*/
    BasisElementCount MaxSize() const
    {
        return m_Buffer.MaxCapacity();
    }

    /*!-------------------------------------------------------------------------
    function:     Resize()
    description:  Resizes the number of elements of the present string

                  - This function ensures that Size() henceforth returns 'count'.
                  - If it must lengthen the string, it appends elements with 
                    value 'elem'.
    arguments:    count [in]  requested element count
                  elem  [in]  fill element (default: ElementType::NullTerminator()) 
    return value: none
    --------------------------------------------------------------------------*/
    void Resize
        (ElementCount                   count,
         BasisElementType               elem    = ElementType::NullTerminator())
    {
        count <= Size() ? Erase(count) : Append(count - Size(), elem);
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Capacity functions                                        */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Assign functions                                             */
    /*!-------------------------------------------------------------------------
    function:     Assign(), operator=()
    description:  Assigns string
                  
                  - A copy of 'str' is assigned to the present string.
                  - If 'from' is given, a substring of 'str' beginning at 
                    character index 'from' and ending at the string end is assigned.
                  - 'from' may range from 0 to 'str'.BasisSize().
                  - If 'len' is specified, at most 'len' elements are assigned.
    arguments:    str    [in]     string to be assigned
                  from   [in]     buffer element position within 'str'
                  len    [in]     max. count of string elements to be assigned
    return value: (Tools_DynamicUTF8String&) *this
    prototypes:   4
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& operator=
        (const Tools_DynamicUTF8String& str)
    {
        AssignSource(str);
        return *this;
    }

    Tools_DynamicUTF8String& Assign
        (const Tools_DynamicUTF8String& str)
    {
        AssignSource(str);
        return *this;
    }

    Tools_DynamicUTF8String& Assign
        (const Tools_DynamicUTF8String& str,
         BasisElementIndex              from)
    {
        AssignSource(str, from);
        return *this;
    }

    Tools_DynamicUTF8String& Assign
        (const Tools_DynamicUTF8String& str,
         BasisElementIndex              from,
         ElementCount                   len)
    {
        AssignSource(str, from, len);
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Assign(), operator=()
    description:  Assigns memory block

                  - A copy of 'src' is assigned to the present string.
                  - Null pointer 'src' is allowed (assigns empty string).
                  - If only 'src' is given, the copy extends up to the 
                    first null-terminator encountered after 'src'.
                  - If 'srclen' is provided, it specifies the absolute count
                    of elements after 'src' to be assigned. In this case,
                    null-terminators are treated and assigned as normal elements.
                  - The buffer length of the string may not exceed MaxSize().
                  - The SAPDB_Char* operator version works properly 
                    only with 7-Bit ascii characters.
    arguments:    src    [in]   memory block to be assigned
                  srclen [in]   absolute count of elements to be assigned
    return value: (Tools_DynamicUTF8String&) *this
    prototypes:   4
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& operator=
        (const SAPDB_Char*              src)
    {
        assert(IsAscii7(src));
        return Assign((ConstPointer)src);
    }

    Tools_DynamicUTF8String& operator=
        (ConstPointer                   src)
    {
        return Assign(src);
    }

    Tools_DynamicUTF8String& Assign
        (ConstPointer                   src)
    {
        AssignSource(src, StringSize(src));
        return *this;
    }

    Tools_DynamicUTF8String& Assign
        (ConstPointer                   src,
         ElementCount                   srclen)
    {
        AssignSource(src, StringSize(src, srclen));
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Assign()
    description:  Assigns string between two given iterators


                  - The string between iterator 'from' and 'to' is
                    assigned to the present string.
                  - The length of the modified string may not exceed MaxSize().
    arguments:    from   [in]   iterator positioned at beginning of string
                  to     [in]   iterator positioned behind end of string
    return value: (Tools_DynamicUTF8String&) *this
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Assign
        (ConstIterator                  from,
         ConstIterator                  to)
    {
        assert(ElementType::IsValid(from));
        assert(ElementType::IterDiff(from, to) >= 0);

        AssignSource(ToPtr(from), ElementType::IterDiff(from, to));
    
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Assign(), operator=()
    description:  Assigns single element/pattern of single element
                  
                  - A single element is assigned to the string.
                  - If 'count' is specified, a pattern consisting of 'count' 
                    copies of 'elem' is assigned.
                  - The buffer length of the string may not exceed MaxSize().
    arguments:    count [in]    number of copies of 'elem'
                  elem  [in]    element to be assigned
    return value: (Tools_DynamicUTF8String&) *this
    prototypes:   3
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& operator=
        (BasisElementType               elem)
    {
        return Assign(elem);
    }

    Tools_DynamicUTF8String& Assign
        (BasisElementType               elem)
    {
        AssignSource(&elem, 1);
        return *this;
    }

    Tools_DynamicUTF8String& Assign
        (ElementCount                   count,
         BasisElementType               elem)
    {
        Erase();
        Append(count, elem);
        return *this;
    }


    /*!-------------------------------------------------------------------------
    function:     AssignRaw()
    description:  Assigns memory block

                  - A copy of 'src' is assigned to the present string.
                  - Null pointer 'src' is allowed (assigns empty string).
                  - The parameter 'srclen' specifies the absolute count
                    of basis elements after 'src' to be assigned, that is it
					specifies the physical length in Bytes of src.
					Null-terminators are treated and assigned as normal 
					basis elements.
                  - The buffer length of the string may not exceed MaxSize().
    arguments:    src    [in]   memory block to be assigned
                  srclen [in]   absolute count of basis elements to be assigned
    return value: (Tools_DynamicUTF8String&) *this
    --------------------------------------------------------------------------*/
	Tools_DynamicUTF8String& AssignRaw
        (ConstPointer                   src,
         BasisElementCount              srcbasislen)
    {
        AssignSource(src, srcbasislen);
        return *this;
    }


    /* -----------------------------------------------------------------------*/
    /*! endchapter: Assign functions                                          */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Compare functions                                            */
    /*!-------------------------------------------------------------------------
    function:     Compare()
    description:  Compares present string with string/substring or memory block
    
                  - A lexical comparison of the present string with an argument
                    string 'str' is performed.
                  - The present string may be restricted to a substring by 
                    specifying 'from' and 'len'. If only 'from' is given, the 
                    substring begins at buffer index 'from' and ends at the 
                    string end. If also 'len' is specified, the substring consists 
                    of at most 'len' string elements.
                  - 'from' may range from 0 to BasisSize().
                  - If a string 'str' is to be compared, it may be 
                    restricted to a substring by specifying 'strlen' and 'strfrom'.
    arguments:    from    [in]    buffer position within the present string
                  len     [in]    max. count of string elements of the present string
                  str     [in]    string to be compared
                  strfrom [in]    buffer position within 'str'
                  strlen  [in]    max. count of string elements of substring of 'str'
    return value: -  0, if strings are equal
                  - -1, if the present string is lexically smaller than the argument
                  - +1, if the present string is lexically greater than the argument
    prototypes:   5
    --------------------------------------------------------------------------*/
    SAPDB_Bool operator==(const Tools_DynamicUTF8String& str) const
    {
        return Compare(str) == 0 ? true : false;
    }

    SAPDB_Bool operator!=(const Tools_DynamicUTF8String& str) const
    {
        return Compare(str) != 0 ? true : false;
    }

    int Compare
        (const Tools_DynamicUTF8String& str) const
    {
        return Compare(ToPtr(Begin()), 
                       ToPtr(str.Begin()), 
                       BasisSize(), 
                       StringSize(str.Begin(), str.End()));
    }

    int Compare
        (BasisElementIndex              from,
         const Tools_DynamicUTF8String& str) const
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        return Compare(ToPtr(iter), 
                       ToPtr(str.Begin()), 
                       StringSize(iter, End()), 
                       StringSize(str.Begin(), str.End()));
    }

    int Compare
        (BasisElementIndex              from,
         ElementCount                   len,
         const Tools_DynamicUTF8String& str) const
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        return Compare(ToPtr(iter), 
                       ToPtr(str.Begin()), 
                       StringSize(iter, len, End()), 
                       StringSize(str.Begin(), str.End()));
    }

    int Compare
        (BasisElementIndex              from,
         ElementCount                   len,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              strfrom) const
    {
        ConstIterator iter    = GetIteratorAtBasis(from);
        ConstIterator striter = str.GetIteratorAtBasis(strfrom);
        return Compare(ToPtr(iter), 
                       ToPtr(striter), 
                       StringSize(iter, len, End()), 
                       StringSize(striter, str.End()));
    } 

    int Compare
        (BasisElementIndex              from,
         ElementCount                   len,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              strfrom,
         ElementCount                   strlen) const
    {
        ConstIterator iter    = GetIteratorAtBasis(from);
        ConstIterator striter = str.GetIteratorAtBasis(strfrom);
        return Compare(ToPtr(iter), 
                       ToPtr(striter), 
                       StringSize(iter, len, End()), 
                       StringSize(striter, strlen, str.End()));
    }

    /*!-------------------------------------------------------------------------
    function:     Compare()
    description:  Compares present string with string/substring or memory block
    
                  - A lexical comparison of the present string with a memory block 
                    beginning at 'src' is performed.
                  - The present string may be restricted to a substring by 
                    specifying 'from' and 'len'. If only 'from' is given, the 
                    substring begins at buffer index 'from' and ends at the 
                    string end. If also 'len' is specified, the substring consists 
                    of at most 'len' string elements.
                  - 'from' may range from 0 to BasisSize().
                  - If 'srclen' is not specified, 'src' is compared up to the 
                    first null-terminator encountered.
                  - If 'srclen' is provided, it specifies the absolute count
                    of elements within 'src' to be compared. In this case,
                    null-terminators are treated as normal elements.
                  - The SAPDB_Char* operator versions work properly 
                    only with 7-Bit ascii characters.
    arguments:    from    [in]    buffer position within the present string
                  len     [in]    max. count of string elements of the present string
                  src     [in]    memory block to be compared
                  srclen  [in]    absolute count of elements after 'src' to be compared
    return value: -  0, if strings are equal
                  - -1, if the present string is lexically smaller than the argument
                  - +1, if the present string is lexically greater than the argument
    prototypes:   8
    --------------------------------------------------------------------------*/
    SAPDB_Bool operator==(const SAPDB_Char* src) const
    {
        assert(IsAscii7(src));
        return Compare((ConstPointer)src) == 0 ? true : false;
    }

    SAPDB_Bool operator!=(const SAPDB_Char* src) const
    {
        assert(IsAscii7(src));
        return Compare((ConstPointer)src) != 0 ? true : false;
    }

    int Compare
        (ConstPointer                   src) const
    {
        return Compare(ToPtr(Begin()), 
                       src, 
                       BasisSize(), 
                       StringSize(src));
    }

    int Compare
        (ConstPointer                   src,
         ElementCount                   srclen) const
    {
        return Compare(ToPtr(Begin()), 
                       src, 
                       BasisSize(), 
                       StringSize(src, srclen));
    }

    int Compare
        (BasisElementIndex              from,
         ConstPointer                   src) const
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        return Compare(ToPtr(iter), 
                       src, 
                       StringSize(iter, End()), 
                       StringSize(src));
    }

    int Compare
        (BasisElementIndex              from,
         ConstPointer                   src,
         ElementCount                   srclen) const
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        return Compare(ToPtr(iter), 
                       src, 
                       StringSize(iter, End()), 
                       StringSize(src, srclen));
    }

    int Compare
        (BasisElementIndex              from,
         ElementCount                   len,
         ConstPointer                   src) const
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        return Compare(ToPtr(iter), 
                       src, 
                       StringSize(iter, len, End()), 
                       StringSize(src));
    }

    int Compare
        (BasisElementIndex              from,
         ElementCount                   len,
         ConstPointer                   src,
         ElementCount                   srclen) const
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        return Compare(ToPtr(iter), 
                       src, 
                       StringSize(iter, len, End()), 
                       StringSize(src, srclen));
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Compare functions                                         */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Direct Character Access                                      */
    /*!-------------------------------------------------------------------------
    function:     GetIteratorAt()
    description:  Returns iterator pointing to indicated string element
    
                  - 'pos' may range from 0 to Size().
    arguments:    pos [in]      string element position within the present string
    return value: ConstIterator/Iterator
    prototypes:   2
    --------------------------------------------------------------------------*/
    ConstIterator GetIteratorAt
         (ElementIndex                  pos) const
    {
        ConstIterator iter = Begin() + pos; // uses ConstIterator::operator+()

        assert(ToPtr(iter) <= ToPtr(End()));
        return iter;
    }

    Iterator GetIteratorAt
         (ElementIndex                  pos)
    {
        Iterator iter = Begin() + pos;      // uses Iterator::operator+()

        assert(ToPtr(iter) <= ToPtr(End()));
        return iter;
    }

    /*!-------------------------------------------------------------------------
    function:     GetIteratorAtBasis(), GetReverseIteratorAtBasis()
    description:  Returns iterator pointing to indicated buffer element
    
                  - 'pos' may range from 0 to BasisSize()     (iterator).
                  - 'pos' may range from 0 to BasisSize() - 1 (reverse iterator).
                  - 'pos' must indicate the first character of a string element
                  - The reverse versions point behind the string element at 'pos'.
    arguments:    pos [in]      basis element position within the present string
    return value: ConstIterator/Iterator
    prototypes:   4
    --------------------------------------------------------------------------*/
    ConstIterator GetIteratorAtBasis
         (BasisElementIndex             pos) const
    {
        ConstIterator iter = ToPtr(Begin()) + pos;
        assert((ToPtr(iter) == ToPtr(End())) ||
               (ToPtr(iter) <  ToPtr(End())  && ElementType::IsValid(iter)));
        return iter;
    }

    Iterator GetIteratorAtBasis
         (BasisElementIndex             pos)
    {
        Iterator iter = ToPtr(Begin()) + pos;
        assert((ToPtr(iter) == ToPtr(End())) ||
               (ToPtr(iter) <  ToPtr(End())  && ElementType::IsValid(iter)));
        return iter;
    }

    ConstReverseIterator GetReverseIteratorAtBasis
         (BasisElementIndex             pos) const
    {
        ConstIterator iter = GetIteratorAtBasis(pos);
        assert(iter != End());
        return ConstReverseIterator(ToPtr(++iter));
    }

    ReverseIterator GetReverseIteratorAtBasis
         (BasisElementIndex             pos)
    {
        Iterator iter = GetIteratorAtBasis(pos);
        assert(iter != End());
        return ReverseIterator(ToPtr(++iter));
    }

    /*!-------------------------------------------------------------------------
    function:     operator[](), At()
    description:  Returns indicated buffer element
    
                  - For basic string types, the buffer elements are indexed
                    with the same index positions as the string elements.
                  - For extended string types, like utf-8 and utf-16, this
                    is not the case. With these, string index <= buffer index
                    holds in all circumstances. If you want to access elements
                    of extended strings directly, you should use the GetIteratorAt()
                    methods.
                  - The operator methods do not perform range checks.
    arguments:    pos [in]      buffer element position within the present string
    return value: indicated element/reference to it
    prototypes:   4
    --------------------------------------------------------------------------*/
    BasisElementType operator[]
        (BasisElementIndex              pos) const
    {
        assert(pos < BasisSize());
        return ToPtr(Begin())[pos];
    }

    BasisElementType& operator[]
        (BasisElementIndex              pos)
    {
        assert(pos < BasisSize());
        return ToPtr(Begin())[pos];
    }

    const BasisElementType& At
        (BasisElementIndex              pos) const
    {
        CheckBasisRange(pos);
        return ToPtr(Begin())[pos];
    }

    BasisElementType& At
        (BasisElementIndex              pos)
    {
        CheckBasisRange(pos);
        return ToPtr(Begin())[pos];
    }

    /*!-------------------------------------------------------------------------
    function:     StrPtr()
    description:  Returns string contents as a pointer to a null-terminated
                  memory block
                  
                  - Attention: By default, strings are not null-terminated.
                  - StrPtr() performs three checks to guarantee that a 
                    null-terminated string be returned:
                  - If the string is unassigned, a pointer to a static, 
                    null-terminated memory block with zero length is returned.
                  - If the string is assigned, it is checked if the last buffer 
                    element is a null terminator. If so, we're done.
                    If not, a null-terminator is appended behind the actual
                    end of the string. This may result in reallocation of the 
                    whole string. If reallocation fails, a pointer to a static, 
                    null-terminated memory block with zero length is returned.
                  - see also: Data()
    arguments:    none
    return value: (ConstPointer) pointer to null-terminated string
    --------------------------------------------------------------------------*/
    ConstPointer StrPtr()
    {
        if (!IsAssigned())
            return EmptyString();
            
        if (m_Buffer.IsEmpty())
            return EmptyString();
        
        // m_Buffer has no reverse iterator. Using End() to obtain iterator.
        ReverseIterator iter = m_Buffer.End();
        if (*iter != ElementType::NullTerminator())
        {
            if (m_Buffer.ProvideCapacity(m_Buffer.ElementCount() + 1) == false)
            {
                return EmptyString();
            }
            *(m_Buffer.End()) = ElementType::NullTerminator();            
        }
        return ToPtr(Begin());
    }
    /*!-------------------------------------------------------------------------
    function:     CharPtr()
    description:  Returns string contents as a pointer to a null-terminated
                  memory block
                  
                  see StrPtr

    arguments:    none
    return value: (const SAPDB_Char *) pointer to null-terminated string
    --------------------------------------------------------------------------*/
    const SAPDB_Char * CharPtr()
    {
      return (const SAPDB_Char *) StrPtr();
    }

    /*!-------------------------------------------------------------------------
    function:     Data()
    description:  Returns string contents as a pointer to a memory block
                  
                  - If the string is assigned, a raw pointer to the string
                    data is returned (not necessarily null-terminated).
                  - If the string is unassigned, a null pointer is returned.
                  - see also: StrPtr()
    arguments:    none
    return value: (ConstPointer) pointer to memory block or null pointer
    --------------------------------------------------------------------------*/
    ConstPointer Data() const
    {
        return IsAssigned() ? ToPtr(Begin()) : 0;
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Direct Character Access                                   */
    /* -----------------------------------------------------------------------*/
    
    /* -----------------------------------------------------------------------*/
    /*! chapter: Substrings                                                   */
    /*!-------------------------------------------------------------------------
    function:     SubStr()
    description:  Returns substring 

                  - A substring of the present string beginning at buffer element
                    'from' and ending at the string end is constructed and returned.
                  - 'from' may range from 0 to BasisSize().
                  - If 'len' is specified, at most 'len' elements of
                    the present string are assigned.

    arguments:    from   [in]   buffer position within the present string
                  len    [in]   max. count of string elements in substring
    return value: (Tools_DynamicUTF8String) substring
    prototypes:   2
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String SubStr
        (BasisElementIndex              from) const
    {
        return Tools_DynamicUTF8String(*this, from);
    }

    Tools_DynamicUTF8String SubStr
        (BasisElementIndex              from,
         ElementCount                   len) const
    {
        return Tools_DynamicUTF8String(*this, from, len);
    }

    /*!-------------------------------------------------------------------------
    function:     SubStrBasis()
    description:  Returns substring 

                  - A substring of the present string beginning at buffer element
                    'from' and ending at buffer element 'to' is returned
                  - 'from' may range from 0 to BasisSize().
                  - 'to' may range from 0 to BasisSize().

    arguments:    from   [in]   buffer start position within the present string
                  to     [in]   buffer end position within the present string
    return value: (Tools_DynamicUTF8String) substring
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String SubStrBasis
        (BasisElementIndex              from,
         BasisElementIndex              to) const
    {
        assert(from <= to);

        return Tools_DynamicUTF8String
                    (GetIteratorAtBasis(from), GetIteratorAtBasis(to));
    }

    /*!-------------------------------------------------------------------------
    function:     SubStrElem()
    description:  Returns substring 

                  - A substring of the present string beginning at element
                    'from' and ending at the string end is constructed and returned.
                  - 'from' may range from 0 to BasisSize().
                  - If 'len' is specified, at most 'len' elements of
                    the present string are assigned.

    arguments:    from   [in]   element position within the present string
                  len    [in]   max. count of string elements in substring
    return value: (Tools_DynamicUTF8String) substring
    prototypes:   2
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String SubStrElem
        (ElementIndex                   from) const
    {
        return Tools_DynamicUTF8String(GetIteratorAt(from), End());
    }

    Tools_DynamicUTF8String SubStrElem
        (ElementIndex                   from,
         ElementCount                   len) const
    {
        ConstIterator iter = GetIteratorAt(from);
        ConstIterator end  = ToPtr(iter) + StringSize(iter, len, End());

        return Tools_DynamicUTF8String(iter, end);
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Substrings                                                */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Append functions                                             */
    /*!-------------------------------------------------------------------------
    function:     Append(), operator+=()
    description:  Appends string/substring

                  - String 'str' is appended to the present string.
                  - If 'from' is given, a substring of 'str' beginning at 
                    buffer index 'from' and ending at the string end is appended.
                  - 'from' may range from 0 to 'str'.BasisSize().
                  - If 'len' is specified, at most 'len' characters are appended.
                  - The length of the result string may not exceed MaxSize().
    arguments:    str    [in]   string to be appended
                  from   [in]   buffer position within 'str'
                  len    [in]   max. count of string elements to be appended
                  
    return value: (Tools_DynamicUTF8String&)  manipulated string (*this)
    prototypes:   4
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& operator+=
        (const Tools_DynamicUTF8String& str)
    {
        AppendSource(ToPtr(str.Begin()), StringSize(str.Begin(), str.End()));
        return *this;
    }

    Tools_DynamicUTF8String& Append
        (const Tools_DynamicUTF8String& str)
    {
        AppendSource(ToPtr(str.Begin()), StringSize(str.Begin(), str.End()));
        return *this;
    }

    Tools_DynamicUTF8String& Append
        (const Tools_DynamicUTF8String& str,
         BasisElementIndex              from)
    {
        ConstIterator iter = str.GetIteratorAtBasis(from);
        AppendSource(ToPtr(iter), StringSize(iter, str.End()));
        return *this;
    }

    Tools_DynamicUTF8String& Append
        (const Tools_DynamicUTF8String& str,
         BasisElementIndex              from,
         ElementCount                   len)
    {
        ConstIterator iter = str.GetIteratorAtBasis(from);
        AppendSource(ToPtr(iter), StringSize(iter, len, str.End()));
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Append()
    description:  Appends memory block
                  
                  - A copy of 'src' is appended to the present string.
                  - Null pointer 'src' is not allowed.
                  - If only 'src' is given, the copy extends up to the 
                    first null-terminator encountered after 'src'.
                  - If 'srclen' is provided, it specifies the absolute count
                    of elements after 'src' to be appended. In this case,
                    null-terminators are treated as normal elements.
                  - The length of the result string may not exceed MaxSize().
                  - The SAPDB_Char* operator version works properly 
                    only with 7-Bit ascii characters.
    arguments:    src    [in]    memory block to be appended
                  srclen [in]    absolute count of elements to be appended
    return value: (Tools_DynamicUTF8String&) manipulated string (*this)
    prototypes:   4
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& operator+=
        (const SAPDB_Char*              src)
    {
        assert(IsAscii7(src));
        return Append((ConstPointer)src);
    }

    Tools_DynamicUTF8String& operator+=
        (ConstPointer                   src)
    {
        return Append(src);
    }

    Tools_DynamicUTF8String& Append
        (ConstPointer                   src)
    {
        AppendSource(src, StringSize(src));
        return *this;
    }

    Tools_DynamicUTF8String& Append
        (ConstPointer                   src,
         ElementCount                   srclen)
    {
        AppendSource(src, StringSize(src, srclen));
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Append(), operator+=()
    description:  Appends single element/pattern of single element
                  
                  - A single element is appended to the string.
                  - If 'count' is specified, a pattern consisting of 'count' 
                    copies of 'elem' is appended.
                  - The buffer length of the string may not exceed MaxSize().
    arguments:    count [in]    number of copies of 'elem'
                  elem  [in]    element to be appended
    return value: (Tools_DynamicUTF8String&) *this
    prototypes:   3
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& operator+=
        (BasisElementType               elem)
    {
        return Append(elem);
    }

    Tools_DynamicUTF8String& Append
        (BasisElementType               elem)
    {
        AppendSource(&elem, 1);
        return *this;
    }

    Tools_DynamicUTF8String& Append
        (ElementCount                   count,
         BasisElementType               elem)
    {
        CheckSizeOverflow(BasisSize() + count);
        m_Buffer.Append(count, elem);
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Append()
    description:  Appends string between two given iterators
                  
                  - The string between iterator 'from' and 'to' is
                    appended to the present string.
                  - The length of the modified string may not exceed MaxSize().
    arguments:    from   [in]   iterator positioned at beginning of string
                  to     [in]   iterator positioned behind end of string
    return value: (Tools_DynamicUTF8String&) manipulated string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Append
        (ConstIterator                  from,
         ConstIterator                  to)
    {
        assert(ElementType::IsValid(from));
        assert(ElementType::IterDiff(from, to) >= 0);

        AppendSource(ToPtr(from), ElementType::IterDiff(from, to));
    
        return *this;
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Append functions                                          */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Insert functions                                             */
    /*!-------------------------------------------------------------------------
    function:     Insert()
    description:  Inserts string/substring

                  - String 'str' is inserted into the present string before 
                    element indicated by 'at'.
                  - 'at' may range from 0 to Size().
                  - If 'from' is given, a substring of 'str' beginning at 
                    buffer index 'from' and ending at the string end is inserted.
                  - 'from' may range from 0 to 'str'.BasisSize().
                  - If 'len' is specified, at most 'len' characters are inserted.
                  - The length of the result string may not exceed MaxSize().
    arguments:    at    [in]   insert buffer position within the present string
                  str   [in]   string to be inserted
                  from  [in]   buffer position within 'str'
                  len   [in]   max. count of elements to be inserted
    return value: (Tools_DynamicUTF8String&)  manipulated string (*this)
    prototypes:   3
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Insert
        (ElementIndex                   at,
         const Tools_DynamicUTF8String& str)
    {
        InsertSource(at, ToPtr(str.Begin()), 
                     StringSize(str.Begin(), str.End()));
        return *this;
    }

    Tools_DynamicUTF8String& Insert
        (BasisElementIndex              at,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              from)
    {
        ConstIterator iter = str.GetIteratorAtBasis(at);
        InsertSource(at, ToPtr(iter), StringSize(iter, str.End()));
        return *this;
    }

    Tools_DynamicUTF8String& Insert
        (BasisElementIndex              at,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              from,
         ElementCount                   len)
    {
        ConstIterator iter = str.GetIteratorAtBasis(at);
        InsertSource(at, ToPtr(iter), StringSize(iter, len, str.End()));
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Insert()
    description:  Inserts memory block
                  
                  - A copy of 'src' is inserted into the present string
                    before the buffer element indicated by 'at'.
                  - Null pointer 'src' is allowed (inserts nothing).
                  - If only 'src' is given, the source extends up to the 
                    first null-terminator encountered after 'src'.
                  - If 'srclen' is provided, it specifies the absolute count
                    of elements after 'src' to be inserted. In this case,
                    null-terminators are treated as normal elements.
                  - 'at' may range from 0 to Size().
                  - The buffer length of the string may not exceed MaxSize().
    arguments:    at     [in]   insert buffer position within the present string
                  src    [in]   memory block to be inserted
                  srclen [in]   absolute count of elements to be inserted
    return value: (Tools_DynamicUTF8String&) manipulated string (*this)
    prototypes:   3
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Insert
        (BasisElementIndex              at,
         const SAPDB_Char*              src)
    {
        assert(IsAscii7(src));
        InsertSource(at, (ConstPointer)src, StringSize((ConstPointer)src));
        return *this;
    }

    Tools_DynamicUTF8String& Insert
        (BasisElementIndex              at,
         ConstPointer                   src)
    {
        InsertSource(at, src, StringSize(src));
        return *this;
    }

    Tools_DynamicUTF8String& Insert
        (BasisElementIndex              at,
         ConstPointer                   src,
         ElementCount                   srclen)
    {
        InsertSource(at, src, StringSize(src, srclen));
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Insert()
    description:  Inserts single element/pattern of single element
                  
                  - A single element is inserted to the string before 
                    the buffer element indicated by 'at'.
                  - If 'count' is specified, a pattern consisting of 'count' 
                    copies of 'elem' is inserted.
                  - The buffer length of the string may not exceed MaxSize().
    arguments:    at    [in]   insert buffer position/iterator within the present string
                  count [in]   number of copies of 'elem'
                  elem  [in]   element to be inserted
    return value: (Tools_DynamicUTF8String&) *this
    prototypes:   4
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Insert
        (BasisElementIndex              at,
         BasisElementType               elem)
    {
        return Insert(GetIteratorAtBasis(at), 1, elem);
    }

    Tools_DynamicUTF8String& Insert
        (ConstIterator                  at,
         BasisElementType               elem)
    {
        return Insert(at, 1, elem);
    }

    Tools_DynamicUTF8String& Insert
        (BasisElementIndex              at,
         ElementCount                   count,
         BasisElementType               elem)
    {
        return Insert(GetIteratorAtBasis(at), count, elem);
    }

    Tools_DynamicUTF8String& Insert
        (ConstIterator                  at,
         ElementCount                   count,
         BasisElementType               elem)
    {
        assert(ElementType::IsValid(at));
        assert(ToPtr(at) >= ToPtr(Begin()));
        assert(ToPtr(at) <= ToPtr(End()));

        CheckSizeOverflow(BasisSize() + count);
        m_Buffer.Insert(ElementType::IterDiff(Begin(), at), count, elem);
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Insert()
    description:  Inserts string between two given iterators
                  
                  - The string between iterator 'from' and 'to' is inserted into 
                    the present string before the element indicated by 'at'.
                  - The length of the modified string may not exceed MaxSize().
    arguments:    at     [in]   iterator pointing into the present string
                  from   [in]   iterator positioned at beginning of insert string
                  to     [in]   iterator positioned at end of insert string
    return value: (Tools_DynamicUTF8String&) manipulated string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Insert
        (ConstIterator                  at,
         ConstIterator                  from,
         ConstIterator                  to)
    {
        assert(ElementType::IsValid(at));
        assert(ToPtr(at) >= ToPtr(Begin()));
        assert(ToPtr(at) <= ToPtr(End()));
        assert(ElementType::IsValid(from));
        assert(ElementType::IsValid(to));
        assert(ElementType::IterDiff(from, to) >= 0);

        m_Buffer.Insert(ElementType::IterDiff(Begin(), at), ToPtr(from), 
                        ElementType::IterDiff(from, to));

        return *this;
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Insert functions                                          */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Erase functions                                             */
    /*!-------------------------------------------------------------------------
    function:     Erase()
    description:  Erases string/substring
                  
                  - The contents of the present string is erased.
                  - If 'from' is specified, a trailing substring beginning at
                    buffer element indicated by 'from' is erased.
                  - 'from' may range from 0 to BasisSize()
                  - If 'len' is specified, at most'len' elements are erased.
    arguments:    from   [in]   erase buffer position within the present string
                  len    [in]   max. count of elements to be erased
    return value: (Tools_DynamicUTF8String&) manipulated string (*this)
    prototypes:   3
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Erase()
    {
        if (IsAssigned())
            m_Buffer.Clear();
        return *this;
    }

    Tools_DynamicUTF8String& Erase
        (BasisElementIndex              from)
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        m_Buffer.Erase(ElementType::IterDiff(Begin(), iter), 
                       StringSize(iter, End()));
        return *this;
    }

    Tools_DynamicUTF8String& Erase
        (BasisElementIndex              from,
         ElementCount                   len)
    {
        ConstIterator iter = GetIteratorAtBasis(from);
        m_Buffer.Erase(ElementType::IterDiff(Begin(), iter), 
                       StringSize(iter, len, End()));
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     Erase()
    description:  Erases single element

                  - The element indicated by 'at' is erased from the string.
    arguments:    at   [in]   iterator indicating the element to be erased
    return value: (ConstIterator)  iterator pointing to the first element behind the
                                   removed element, or to End()
    --------------------------------------------------------------------------*/
    ConstIterator Erase
        (ConstIterator                  at)
    {
        assert(ElementType::IsValid(at));
        assert(ToPtr(at) >= ToPtr(Begin()));
        assert(ToPtr(at) <= ToPtr(End()));

        m_Buffer.Erase(ElementType::IterDiff(Begin(), at), 
                       ElementType::Size(at));
        return at;
    }

    /*!-------------------------------------------------------------------------
    function:     Erase()
    description:  Erases substring

                  - The substring between 'from' and 'to' is erased.
    arguments:    from [in]  erase start iterator within the present string
                  to   [in]  erase end iterator within the present string
    return value: (ConstIterator)  iterator pointing to the first char behind the
                                   removed substring, or to End()
    --------------------------------------------------------------------------*/
    ConstIterator Erase
        (ConstIterator                  from,
         ConstIterator                  to)
    {
        assert(ElementType::IsValid(from));
        assert(ToPtr(from) >= ToPtr(Begin()));
        assert(ToPtr(from) <= ToPtr(End()));
        assert(ToPtr(to) == ToPtr(End()) ? true : ElementType::IsValid(to));
        assert(ToPtr(to) >= ToPtr(Begin()));
        assert(ToPtr(to) <= ToPtr(End()));
        assert(ElementType::IterDiff(from, to) >= 0);

        m_Buffer.Erase(ElementType::IterDiff(Begin(), from), 
                       ElementType::IterDiff(from, to));
        return from;
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Erase functions                                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Copy functions                                               */
    /*!-------------------------------------------------------------------------
    function:     Copy()
    description:  Copies substring to given destination
                  
                  - 'len' elements of the present string starting at buffer 
                     index 'from' are copied to the address indicated by 'dest'.
                  - Null pointer 'dest' is not allowed.
                  - 'from' may range from 0 to Size().
                  - No memory check concerning 'dest' is performed.
                  - Attention: Arguments 'len' and 'from' are in opposite order
                    in this method (C++ standard)
    arguments:    dest   [in]   destination address for the copy
                  len    [in]   max. count of elements to be copied
                  from   [in]   copy position within the present string
    return value: (BasisElementCount) count of buffer elements copied
    --------------------------------------------------------------------------*/
    BasisElementCount Copy
        (Pointer                        dest,
         ElementCount                   len,
         BasisElementIndex              from = 0) const
    {
        BasisElementCount count = StringSize(Begin(), len, End());
    
        m_Buffer.Copy(dest, ElementType::IterDiff(Begin(), GetIteratorAtBasis(from)), 
                      count);
        return count;
    }
    
    /* -----------------------------------------------------------------------*/
    /*! endchapter: Copy functions                                            */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Replace functions                                            */
    /*!-------------------------------------------------------------------------
    function:     Replace()
    description:  Replaces substring with string/substring or memory block
                  
                  - A substring starting at buffer index 'from' and with at most 
                    'len' string elements is replaced by string 'str'.
                  - 'from' may range from 0 to BasisSize().
                  - The substring to be replaced may also be indicated by
                    iterators 'from' and 'to'.
                  - In this case, 'from' and 'to' may range from Begin() to End().
                  - If 'strfrom' is specified, the removed part is replaced by 
                    a trailing substring of 'str' beginning at buffer element 
                    indicated by 'strfrom'.
                  - 'strfrom' may range from 0 to 'str'.BasisSize().
                  - If also 'strlen' is specified, at most 'strlen' string elements 
                    are inserted.
                  - If 'src' is given, the removed part is replaced by the 
                    memory block pointed to by 'src'.
                  - Null pointer 'src' is not allowed.
                  - If only 'src' is given, the source extends up to the 
                    first null-terminator encountered after 'src'.
                  - If 'srclen' is provided, it specifies the absolute count
                    of elements after 'src' to be inserted. In this case,
                    null-terminators are treated as normal elements.
                  - If 'count' and 'elem' are given, the removed part is replaced
                    by a pattern consisting of 'count' copies of 'elem'.
                  - If 'repfrom' and 'repto' are given, the removed part is replaced by 
                    the element sequence between 'repfrom' and 'repto'.
                  - The length of the modified string may not exceed MaxSize().
    arguments:    from    [in]   erase buffer position/iterator within the present string
                  len     [in]   max. count of string elements in substring to be erased
                  str     [in]   string to be inserted
                  strfrom [in]   substring buffer position within 'str'
                  strlen  [in]   max. count of substring elements of 'str'
                  src     [in]   memory block to be inserted
                  srclen  [in]   absolute of elements of 'src' to be inserted
                  count   [in]   number of copies of 'elem'
                  elem    [in]   element to be inserted
                  repfrom [in]   iterator indicating beginning of insert sequence
                  repto   [in]   iterator indicating end of insert sequence
    return value: (Tools_DynamicUTF8String&)  manipulated string (*this)
    prototypes:   14
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Replace
        (BasisElementIndex              from,
         ElementCount                   len,
         const Tools_DynamicUTF8String& str)
    {
        ReplacePart(GetIteratorAtBasis(from), len, ToPtr(str.Begin()), 
                    StringSize(str.Begin(), str.End()));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (BasisElementIndex              from,
         ElementCount                   len,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              strfrom)
    {
        ConstIterator striter = str.GetIteratorAtBasis(strfrom);
        ReplacePart(GetIteratorAtBasis(from), len, ToPtr(striter), 
                    StringSize(striter, str.End()));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (BasisElementIndex              from,
         ElementCount                   len,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              strfrom,
         ElementCount                   strlen)
    {
        ConstIterator striter = str.GetIteratorAtBasis(strfrom);
        ReplacePart(GetIteratorAtBasis(from), len, ToPtr(striter), 
                    StringSize(striter, strlen, str.End()));
        return *this;
    }
  
    Tools_DynamicUTF8String& Replace
        (ConstIterator                  from,
         ConstIterator                  to,
         const Tools_DynamicUTF8String& str)
    {
        ReplacePart(from, to, ToPtr(str.Begin()), 
                    StringSize(str.Begin(), str.End()));
        return *this;
    }

    Tools_DynamicUTF8String& Replace
        (ConstIterator                  from,
         ConstIterator                  to,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              strfrom)
    {
        ConstIterator striter = str.GetIteratorAtBasis(strfrom);
        ReplacePart(from, to, ToPtr(striter), 
                    StringSize(striter, str.End()));
        return *this;
    }

    Tools_DynamicUTF8String& Replace
        (ConstIterator                  from,
         ConstIterator                  to,
         const Tools_DynamicUTF8String& str,
         BasisElementIndex              strfrom,
         ElementCount                   strlen)
    {
        ConstIterator striter = str.GetIteratorAtBasis(strfrom);
        ReplacePart(from, to, ToPtr(striter), 
                    StringSize(striter, strlen, str.End()));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (BasisElementIndex              from,
         ElementCount                   len,
         ConstPointer                   src)
    {
        ReplacePart(GetIteratorAtBasis(from), len, src, StringSize(src));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (BasisElementIndex              from,
         ElementCount                   len,
         ConstPointer                   src, 
         ElementCount                   srclen)
    {
        ReplacePart(GetIteratorAtBasis(from), len, src, StringSize(src, srclen));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (ConstIterator                  from,
         ConstIterator                  to,
         ConstPointer                   src)
    {
        ReplacePart(from, to, src, StringSize(src));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (ConstIterator                  from,
         ConstIterator                  to,
         ConstPointer                   src, 
         ElementCount                   srclen)
    {
        ReplacePart(from, to, src, StringSize(src, srclen));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (BasisElementIndex              from,
         ElementCount                   len,
         ElementCount                   count,
         BasisElementType               elem)
    {
        ConstIterator fromiter   = GetIteratorAtBasis(from);
        BasisElementCount cutlen = StringSize(fromiter, len, End());
        BasisElementIndex cutpos = ElementType::IterDiff(Begin(), fromiter);
        CheckSizeOverflow(BasisSize() - cutlen + count);
        m_Buffer.Replace(cutpos, cutlen, count, elem);
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (ConstIterator                  from,
         ConstIterator                  to,
         ElementCount                   count,
         BasisElementType               elem)
    {
        assert(ElementType::IsValid(from));
        assert(ToPtr(from) >= ToPtr(Begin()));
        assert(ToPtr(from) <= ToPtr(End()));
        assert(ElementType::IsValid(to));
        assert(ToPtr(to) >= ToPtr(Begin()));
        assert(ToPtr(to) <= ToPtr(End()));
        assert(ElementType::IterDiff(from, to) >= 0);

        BasisElementCount cutlen = ElementType::IterDiff(from, to);
        BasisElementIndex cutpos = ElementType::IterDiff(Begin(), from);
        CheckSizeOverflow(BasisSize() - cutlen + count);
        m_Buffer.Replace(cutpos, cutlen, count, elem);
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (BasisElementIndex              from,
         ElementCount                   len,
         ConstIterator                  repfrom,
         ConstIterator                  repto)
    {
        assert(ElementType::IsValid(repfrom));
        assert(ElementType::IsValid(repto));
        assert(ElementType::IterDiff(repfrom, repto) >= 0);

        ReplacePart(GetIteratorAtBasis(from), len, ToPtr(repfrom), 
                    ElementType::IterDiff(repfrom, repto));
        return *this;
    }
    
    Tools_DynamicUTF8String& Replace
        (ConstIterator                  from,
         ConstIterator                  to,
         ConstIterator                  repfrom,
         ConstIterator                  repto)
    {
        assert(ElementType::IsValid(repfrom));
        assert(ElementType::IsValid(repto));
        assert(ElementType::IterDiff(repfrom, repto) >= 0);

        ReplacePart(from, to, ToPtr(repfrom), 
                    ElementType::IterDiff(repfrom, repto));
        return *this;
    }
    
    /* -----------------------------------------------------------------------*/
    /*! endchapter: Replace functions                                         */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Iterator support                                             */
    /*!-------------------------------------------------------------------------
    function:     Begin()
    description:  Returns iterator pointing to beginning of the string
    arguments:    none
    return value: (ConstIterator/Iterator) 
    prototypes:   2
    --------------------------------------------------------------------------*/
    ConstIterator Begin() const
    {
        return IsAssigned() ? ConstIterator(m_Buffer.Begin())
                            : ConstIterator(EmptyString());
    }

    Iterator Begin()
    {
        return IsAssigned() ? Iterator(m_Buffer.Begin()) 
                            : Iterator();
    }

    /*!-------------------------------------------------------------------------
    function:     End()
    description:  Returns iterator pointing to end of the string
    arguments:    none
    return value: (ConstIterator/Iterator) 
    prototypes:   2
    --------------------------------------------------------------------------*/
    ConstIterator End() const
    {
        return IsAssigned() ? ConstIterator(m_Buffer.End()) 
                            : ConstIterator(EmptyString());
    }

    Iterator End()
    {
        return IsAssigned() ? Iterator(m_Buffer.End()) 
                            : Iterator();
    }

    /*!-------------------------------------------------------------------------
    function:     RBegin()
    description:  Returns reverse iterator pointing to reverse beginning 
                  of the string
    arguments:    none
    return value: (ConstReverseIterator/ReverseIterator) 
    prototypes:   2
    --------------------------------------------------------------------------*/
    ConstReverseIterator RBegin() const
    {
        return IsAssigned() ? ConstReverseIterator(m_Buffer.End())
                            : ConstReverseIterator(EmptyString());
    }

    ReverseIterator RBegin()
    {
        return IsAssigned() ? ReverseIterator(m_Buffer.End())
                            : ReverseIterator();
    }

    /*!-------------------------------------------------------------------------
    function:     REnd()
    description:  Returns reverse iterator pointing to reverse end of the 
                  string
    arguments:    none
    return value: (ConstReverseIterator/ReverseIterator) 
    prototypes:   2
    --------------------------------------------------------------------------*/
    ConstReverseIterator REnd() const
    {
        return IsAssigned() ? ConstReverseIterator(m_Buffer.Begin())
                            : ConstReverseIterator(EmptyString());
    }

    ReverseIterator REnd()
    {
        return IsAssigned() ? ReverseIterator(m_Buffer.Begin())
                            : ReverseIterator();
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Iterator support                                          */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Find functions                                               */
    /*!-------------------------------------------------------------------------
    function:     Find(), RFind()
    description:  Searches substring
    
                  - The first (last) occurrence of a given element sequence is 
                    searched for in the present string.
                  - The sequence may be specified either by string 'searchstr'
                    or by pointer 'seqptr'. If only 'seqptr' is given, the
                    length of the sequence is limited by the first
                    null-terminator encountered. Alternatively, the sequence
                    length my be specified by 'seqlen'.
                  - Also a single basis element 'elem' may be searched for. 
                  - If specified, the search starts at buffer position 'from',
                    otherwise at the beginning of the string.
                  - There is no limit to 'from'.

    arguments:    searchstr [in]  string to be searched for
                  seqptr    [in]  pointer to element sequence to be searched for
                  seqlen    [in]  count of string elements of sequence starting at 'seqptr' 
                  elem      [in]  single basis element to be searched for
                  from      [in]  buffer position within the present string
                                  (where the search starts)
    return value: (BasisElementIndex) buffer index of sequence if found,
                                      NPos if not found
    prototypes:   14
    --------------------------------------------------------------------------*/
    BasisElementIndex Find
        (const Tools_DynamicUTF8String& searchstr,
         BasisElementIndex              from        = 0) const
    {
        return FindSequence (from, 
                             searchstr.Begin(), 
                             searchstr.End());
    }

    BasisElementIndex Find
        (ConstPointer                   seqptr,
         BasisElementIndex              from        = 0) const
    {
        return FindSequence (from, 
                             seqptr, 
                             seqptr + StringSize(seqptr));
    }

    BasisElementIndex Find
        (ConstPointer                   seqptr,
         BasisElementIndex              from,
         ElementCount                   seqlen) const
    {
        return FindSequence (from,
                             seqptr,
                             seqptr + StringSize(seqptr, seqlen));
    }

    BasisElementIndex Find
        (BasisElementType               elem,
         BasisElementIndex              from        = 0) const
    {
        return FindSequence (from,
                             &elem,
                             ConstPointer(&elem) + 1);
    }


    BasisElementIndex RFind
        (const Tools_DynamicUTF8String& searchstr,
         BasisElementIndex              from        = NPos) const
    {
        return ReverseFindSequence (from, 
                                    searchstr.Begin(), 
                                    searchstr.End());
    }

    BasisElementIndex RFind
        (ConstPointer                   seqptr,
         BasisElementIndex              from        = NPos) const
    {
        return ReverseFindSequence (from, 
                                    seqptr, 
                                    seqptr + StringSize(seqptr));
    }

    BasisElementIndex RFind
        (ConstPointer                   seqptr,
         BasisElementIndex              from,
         ElementCount                   seqlen) const
    {
        return ReverseFindSequence (from,
                                    seqptr,
                                    seqptr + StringSize(seqptr, seqlen));
    }

    BasisElementIndex RFind
        (BasisElementType               elem,
         BasisElementIndex              from        = NPos) const
    {
        return ReverseFindSequence (from,
                                    &elem,
                                    ConstPointer(&elem) + 1);
    }

    /*!-------------------------------------------------------------------------
    function:     FindFirstOf()
    description:  Searches first occurrence of any single element from set of elements
    
                  - In contrast to Find(), this method does not search the
                    present string for an entire search sequence, 
                    but for the first occurrence of any single element 
                    of a set of elements. The order of the elements in the
                    set is not relevant.
                  - The set may be specified either by string 'setstr'
                    or by pointer 'setptr'. If only 'setptr' is given, the
                    set is limited by the first null-terminator encountered. 
                    Alternatively, the set length my be specified by 'setlen'.
                  - Also a single basis element 'elem' may be searched for. 
                  - If specified, the search starts at buffer position 'from',
                    otherwise at the beginning of the string.
                  - There is no limit to 'from'.
    arguments:    set       [in]  string defining set of elements to be searched for
                  setptr    [in]  pointer to element set to be searched for
                  setlen    [in]  count of string elements of set starting at 'setptr' 
                  elem      [in]  single basis element to be searched for
                  from      [in]  buffer position within the present string
                                  (where the search starts)
    return value: (BasisElementIndex) buffer index of first occurrence if found,
                                      NPos if not found
    prototypes:   4
    --------------------------------------------------------------------------*/
    BasisElementIndex FindFirstOf
        (const Tools_DynamicUTF8String& set,
         BasisElementIndex              from        = 0) const
    {
        return FindFirstOfSet (from, 
                               set.Begin(), 
                               set.End());
    }

    BasisElementIndex FindFirstOf
        (ConstPointer                   setptr,
         BasisElementIndex              from        = 0) const
    {
        return FindFirstOfSet (from, 
                               setptr, 
                               setptr + StringSize(setptr));
    }

    BasisElementIndex FindFirstOf
        (ConstPointer                   setptr,
         BasisElementIndex              from,
         ElementCount                   setlen) const
    {
        return FindFirstOfSet (from,
                               setptr,
                               setptr + StringSize(setptr, setlen));
    }

    BasisElementIndex FindFirstOf
        (BasisElementType               elem,
         BasisElementIndex              from        = 0) const
    {
        return FindFirstOfSet (from,
                               &elem,
                               ConstPointer(&elem) + 1);
    }

    /*!-------------------------------------------------------------------------
    function:     FindLastOf()
    description:  Searches last occurrence of any single element from set of elements
    
                  - see FindFirstOf() for details

    return value: (BasisElementIndex) buffer index of last occurrence if found,
                                      NPos if not found
    prototypes:   4
    --------------------------------------------------------------------------*/
    BasisElementIndex FindLastOf
        (const Tools_DynamicUTF8String& set,
         BasisElementIndex              from        = NPos) const
    {
        return FindLastOfSet (from, 
                              set.Begin(), 
                              set.End());
    }

    BasisElementIndex FindLastOf
        (ConstPointer                   setptr,
         BasisElementIndex              from        = NPos) const
    {
        return FindLastOfSet (from, 
                              setptr, 
                              setptr + StringSize(setptr));
    }

    BasisElementIndex FindLastOf
        (ConstPointer                   setptr,
         BasisElementIndex              from,
         ElementCount                   setlen) const
    {
        return FindLastOfSet (from,
                              setptr,
                              setptr + StringSize(setptr, setlen));
    }

    BasisElementIndex FindLastOf
        (BasisElementType               elem,
         BasisElementIndex              from        = NPos) const
    {
        return FindLastOfSet (from,
                              &elem,
                              ConstPointer(&elem) + 1);
    }

    /*!-------------------------------------------------------------------------
    function:     FindFirstNotOf()
    description:  Searches first occurrence of any single element which is
                  not contained within a given set of elements
    
                  - see FindFirstOf() for details

    return value: (BasisElementIndex) buffer index of first occurrence if found,
                                      NPos if not found
    prototypes:   4
    --------------------------------------------------------------------------*/
    BasisElementIndex FindFirstNotOf
        (const Tools_DynamicUTF8String& set,
         BasisElementIndex              from        = 0) const
    {
        return FindFirstNotOfSet (from, 
                                  set.Begin(), 
                                  set.End());
    }

    BasisElementIndex FindFirstNotOf
        (ConstPointer                   setptr,
         BasisElementIndex              from        = 0) const
    {
        return FindFirstNotOfSet (from, 
                                  setptr, 
                                  setptr + StringSize(setptr));
    }

    BasisElementIndex FindFirstNotOf
        (ConstPointer                   setptr,
         BasisElementIndex              from,
         ElementCount                   setlen) const
    {
        return FindFirstNotOfSet (from,
                                  setptr,
                                  setptr + StringSize(setptr, setlen));
    }

    BasisElementIndex FindFirstNotOf
        (BasisElementType               elem,
         BasisElementIndex              from        = 0) const
    {
        return FindFirstNotOfSet (from,
                                  &elem,
                                  ConstPointer(&elem) + 1);
    }

    /*!-------------------------------------------------------------------------
    function:     FindLastNotOf()
    description:  Searches last occurrence of any single element which is
                  not contained within a given set of elements
    
                  - see FindFirstOf() for details

    return value: (BasisElementIndex) buffer index of last occurrence if found,
                                      NPos if not found
    prototypes:   4
    --------------------------------------------------------------------------*/
    BasisElementIndex FindLastNotOf
        (const Tools_DynamicUTF8String& set,
         BasisElementIndex              from        = NPos) const
    {
        return FindLastNotOfSet (from, 
                                 set.Begin(), 
                                 set.End());
    }

    BasisElementIndex FindLastNotOf
        (ConstPointer                   setptr,
         BasisElementIndex              from        = NPos) const
    {
        return FindLastNotOfSet (from, 
                                 setptr, 
                                 setptr + StringSize(setptr));
    }

    BasisElementIndex FindLastNotOf
        (ConstPointer                   setptr,
         BasisElementIndex              from,
         ElementCount                   setlen) const
    {
        return FindLastNotOfSet (from,
                                 setptr,
                                 setptr + StringSize(setptr, setlen));
    }

    BasisElementIndex FindLastNotOf
        (BasisElementType               elem,
         BasisElementIndex              from        = NPos) const
    {
        return FindLastNotOfSet (from,
                                 &elem,
                                 ConstPointer(&elem) + 1);
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Find functions                                            */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Conversion routines                                          */
    /*!-------------------------------------------------------------------------
    function:     ConvertFromUCS2()
    description:  Converts and assigns an ucs-2 string

                  - Source string is located between 'srcBeg' and 'srcEnd'.

    arguments:    srcBeg  [in]  pointer to begin of source
                  srcEnd  [in]  pointer behind end of source
    return value: (ConversionResult) 
                  Success, SourceExhausted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertFromUCS2
        (UCS2ConstPointer               srcBeg,
         UCS2ConstPointer               srcEnd);

    /*!-------------------------------------------------------------------------
    function:     ConvertFromUCS2_Unaligned_Swap()
    description:  Converts and assigns an ucs-2 string
                  supporting 'SAPDB UCS2'

                  - Allows unaligned UCS2 strings
                  - defines UCS2 swap type specification when executing

                  - Source string is located between 'srcBeg' and 'srcEnd'.

    arguments:    srcBeg  [in]  pointer to begin of source
                  srcEnd  [in]  pointer behind end of source
    return value: (ConversionResult) 
                  Success, SourceExhausted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertFromUCS2_Unaligned_Swap
        (UCS2Char *srcBeg,
         UCS2Char *srcEnd);

    /*!-------------------------------------------------------------------------
    function:     ConvertFromUCS2_Unaligned_Swap()
    description:  Converts and assigns an ucs-2 string
                  supporting 'SAPDB UCS2'

                  - Allows unaligned UCS2 strings
                  - Needs UCS2 swap type specification 'srcSwapped'
                  - 'false' (Highbyte,Lowbyte) 
                  - 'true' (LowByte,HighByte)
                  - Source string is located between 'srcBeg' and 'srcEnd'.

    arguments:    srcBeg       [in]  pointer to begin of source
                  srcEnd       [in]  pointer behind end of source
                  srcSwapped   [in]  bool defining swap type
    return value: (ConversionResult) 
                  Success, SourceExhausted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertFromUCS2_Unaligned_Swap
        (UCS2Char        *srcBeg,
         UCS2Char        *srcEnd,
         const SAPDB_Int  srcSwapped);


    /*!-------------------------------------------------------------------------
    function:     ConvertFromInt()
    description:  Converts and assigns an Integer to the string

    arguments:    nNumber [in]  the integer
                  nBase   [in]  the base to convert (default 10)

    return value: (ConversionResult) 
                  Success, SourceExhausted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertFromInt
        (SAPDB_Int8   nNumber,
         SAPDB_Int    nBase = 10);

    /*!-------------------------------------------------------------------------
    function:     ConvertToUCS2()
    description:  Converts present utf-8 string to ucs-2 string

                  - Destination string is located between 'destBeg' and 'destEnd'.
                  - On function return, 'destAt' points behind the last
                    ucs-2 character converted.

    arguments:    destBeg [in]  pointer to begin of destination buffer
                  destEnd [in]  pointer behind end of destination buffer
                  destAt  [out] pointer to position in dest buffer after function returned
    return value: (ConversionResult) 
                  Success, SourceExhausted, SourceCorrupted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertToUCS2
        (UCS2Pointer                    destBeg,
         UCS2ConstPointer               destEnd,
         UCS2Pointer&                   destAt) const;

    /*!-------------------------------------------------------------------------
    function:     ConvertToUCS2_Unaligned_Swap() - overloaded
    description:  Converts present utf-8 string to ucs-2 string
                  supporting 'SAPDB UCS2'

                  - Allows unaligned UCS2 strings
                  - defines UCS2 swap type specification when executing
                  
                  - Destination string is located between 'destBeg' and 'destEnd'.
                  - On function return, 'destAt' points behind the last
                    ucs-2 character converted.

    arguments:    destBeg [in]  pointer to begin of destination buffer
                  destEnd [in]  pointer behind end of destination buffer
                  destAt  [out] pointer to position in dest buffer after function returned
    return value: (ConversionResult) 
                  Success, SourceExhausted, SourceCorrupted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertToUCS2_Unaligned_Swap
        (UCS2Char *        destBeg,
         UCS2Char * const  destEnd,
         UCS2Char *&       destAt) const;

    /*!-------------------------------------------------------------------------
    function:     ConvertToUCS2_Unaligned_Swap() - overloaded
    description:  Converts present utf-8 string to ucs-2 string
                  supporting 'SAPDB UCS2'

                  - Allows unaligned UCS2 strings
                  - needs UCS2 swap type specification for execution
                  
                  - Destination string is located between 'destBeg' and 'destEnd'.
                  - On function return, 'destAt' points behind the last
                    ucs-2 character converted.

    arguments:    destBeg     [in]  pointer to begin of destination buffer
                  destEnd     [in]  pointer behind end of destination buffer
                  destAt      [out] pointer to position in dest buffer after function returned
                  destSwapped [in]  defines destination swap
    return value: (ConversionResult) 
                  Success, SourceExhausted, SourceCorrupted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertToUCS2_Unaligned_Swap
        (UCS2Char *        destBeg,
         UCS2Char * const  destEnd,
         UCS2Char *&       destAt,
         const bool        destSwapped) const;


    /*!-------------------------------------------------------------------------
    function:     ConvertFromASCII_Latin1()
    description:  Converts and assigns an ascii latin1 string

                  - Source string is located between 'srcBeg' and 'srcEnd'.
                  - Source is interpreted as 8-Bit ASCII according 
                    to IS8859-LATIN1.

    arguments:    srcBeg  [in]  pointer to begin of source
                  srcEnd  [in]  pointer behind end of source
    return value: (ConversionResult) 
                  Success, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertFromASCII_Latin1
        (ASCIIConstPointer               srcBeg,
         ASCIIConstPointer               srcEnd);

    /*!-------------------------------------------------------------------------
    function:     ConvertToASCII_Latin1()
    description:  Converts present utf-8 string to ascii latin1 string

                  - Destination string is located between 'destBeg' and 'destEnd'.
                  - On function return, 'destAt' points behind the last
                    ascii character converted.
                  - The replacement character 'replChar' is set in output 
                    for all those utf-8 multibyte characters 
                    which cannot be converted to ASCII Latin 1. 
                  - 'replChar' must be different from '\0'.

    arguments:    destBeg  [in]  pointer to begin of destination buffer
                  destEnd  [in]  pointer behind end of destination buffer
                  destAt   [out] pointer to position in dest buffer after function returned
                  replChar [in]  replacement character (default = -1)
    return value: (ConversionResult) 
                  Success, SourceExhausted, SourceCorrupted, TargetExhausted
    --------------------------------------------------------------------------*/
    ConversionResult ConvertToASCII_Latin1
        (ASCIIPointer                    destBeg,
         ASCIIConstPointer               destEnd,
         ASCIIPointer&                   destAt,
         SAPDB_Char                      replChar = -1) const;

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Conversion routines                                       */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Utilities                                                    */
    /*!-------------------------------------------------------------------------
    function:     ToUpper()
    description:  Converts present utf-8 string to uppercase (in place)
    arguments:    none
    return value: (Tools_DynamicUTF8String&) modified string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& ToUpper();

    /*!-------------------------------------------------------------------------
    function:     ToLower()
    description:  Converts present utf-8 string to lowercase (in place)
    arguments:    none
    return value: (Tools_DynamicUTF8String&) modified string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& ToLower();

    /*!-------------------------------------------------------------------------
    function:     Trim()
    description:  Deletes leading and trailing characters (in place)

                  Default: WhiteSpace = Space, Tab, CR, LF, FF
    arguments:    wspIn [IN] - characterset to trim
    return value: (Tools_DynamicUTF8String&) modified string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& Trim(const SAPDB_Char* wsp = " \t\n\r\f")
    {
        return TrimLeading(wsp).TrimTrailing(wsp);
    }

		/*!-------------------------------------------------------------------------
    function:     TrimLeft()
    description:  Deletes leading whitespaces (in place)

                  - WhiteSpace = Space, Tab, CR, LF, FF
    arguments:    none
    return value: (Tools_DynamicUTF8String&) modified string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& TrimLeft()
    {
        const SAPDB_Char* wsp = " \t\n\r\f";

        BasisElementIndex first = FindFirstNotOf((ConstPointer)wsp, 0, (ElementCount) strlen(wsp));

        if (first != 0 && first != NPos)
            Erase(0, first);

        return *this;
    }

	/*!-------------------------------------------------------------------------
    function:     TrimRight()
    description:  Deletes trailing whitespaces (in place)

                  - WhiteSpace = Space, Tab, CR, LF, FF
    arguments:    none
    return value: (Tools_DynamicUTF8String&) modified string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& TrimRight()
    {
        const SAPDB_Char* wsp = " \t\n\r\f";

        BasisElementIndex last  = FindLastNotOf((ConstPointer)wsp, NPos, (ElementCount) strlen(wsp));

        if (last != NPos) 
            Erase(last+Tools_UTF8Basis::CharacterSize(&At(last)));

        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     TrimLeading()
    description:  Deletes leading characters (in place)

                  Default: WhiteSpace = Space, Tab, CR, LF, FF
    arguments:    wspIn [IN] - characterset to trim
    return value: (Tools_DynamicUTF8String&) modified string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& TrimLeading(const SAPDB_Char* wsp = " \t\n\r\f")
    {
        BasisElementIndex first = FindFirstNotOf((ConstPointer)wsp, 0, (ElementCount) strlen(wsp));

        if (first != 0 && first != NPos)
            Erase(0, first);

        if (first == NPos)
            Erase();

        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     TrimTrailing()
    description:  Deletes trailing characters (in place)

                  Default: WhiteSpace = Space, Tab, CR, LF, FF
    arguments:    wspIn [IN] - characterset to trim
    return value: (Tools_DynamicUTF8String&) modified string (*this)
    --------------------------------------------------------------------------*/
    Tools_DynamicUTF8String& TrimTrailing(const SAPDB_Char* wsp = " \t\n\r\f")
    {
        BasisElementIndex last  = FindLastNotOf((ConstPointer)wsp, NPos, (ElementCount) strlen(wsp));

        if (last != NPos) 
            Erase(last+Tools_UTF8Basis::CharacterSize(&At(last)));

        if (last == NPos)
            Erase();

        return *this;
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Utilities                                                 */
    /* -----------------------------------------------------------------------*/
protected:

    /*--------------------------------------------------------------------------
    function:     CheckSizeOverflow()
    description:  Checks whether requested number of buffer elements exceeds
                  the maximum string size

                  - If 'count' exceeds MaxSize(), an exception is thrown.
    arguments:    count [in]    count of elements to be reserved
    return value: none
    --------------------------------------------------------------------------*/
    void CheckSizeOverflow
        (BasisElementCount              count) const
    {
        assert(count < MaxSize());
    }

    /*--------------------------------------------------------------------------
    function:     CheckBasisRange()
    description:  Checks whether given buffer element index is valid

                  - 'pos' may range from 0 to BasisSize()-1.
    arguments:    pos [in]      buffer element position within the present string
    return value: none
    --------------------------------------------------------------------------*/
    void CheckBasisRange
        (BasisElementIndex              pos) const
    {
        assert(pos < BasisSize());
    }

    /*--------------------------------------------------------------------------
    function:     StringSize()
    description:  Returns physical size of a string
    
                  - The length in buffer elements of a substring starting at 
                    iterator position 'from' and ending at 'end' is returned.
                  - If 'from' and 'len' are given, at most 'len' string elements
                    are counted. If 'len' is chosen too large, i.e. it exceeds
                    'end', StringSize('from') is returned.
    arguments:    from [in]   iterator start position
                  end  [in]   end of string
                  len  [in]   count of string elements of the substring
    return value: (BasisElementCount) 
    prototypes:   2
    --------------------------------------------------------------------------*/
    static BasisElementCount StringSize
        (ConstIterator                  from,
         ConstIterator                  end)
    {
        return ElementType::IterDiff(from, end);
    }

    static BasisElementCount StringSize
        (ConstIterator                  from,
         ElementCount                   len,
         ConstIterator                  end)
    {
        ConstIterator iter = from;
        while (ToPtr(iter) < ToPtr(end) && len > 0)
        {
           ++iter;
           --len;
        }

        return ElementType::IterDiff
               (from, ToPtr(end) < ToPtr(iter) ? end : iter);
    }

    /*--------------------------------------------------------------------------
    function:     StringSize()
    description:  Returns physical size of a memory block
    
                  - The count of buffer elements of a memory block starting at 'src' 
                    is determined. 
                  - If only 'src' is given, the size is given by the count of buffer 
                    elements between 'src' and the first null-terminator
                    encountered, which by itself is not included.
                  - If 'src' is a null pointer, 0 is returned.
                  - If also 'srclen' is provided, it specifies the total number
                    of elements to be within 'src'. In this case, null-terminators
                    are treated and counted as normal elements.
    arguments:    src    [in]  pointer to memory block
                  srclen [in]  absolute count of elements after 'src'
    return value: (BasisElementCount) 
                  If only 'src' is specified and no null-terminator is found,
                  SAPDB_MaxValue(BasisElementCount) is returned.
    prototypes:   2
    --------------------------------------------------------------------------*/
    static BasisElementCount StringSize
        (ConstPointer                   src)
    {
        static BasisElementCount maxlen = NPos;
        if (src == 0) return 0;

        ConstIterator iter(src);
        BasisElementCount srclen = maxlen;

        while ((srclen > 0) && (*iter != ElementType::NullTerminator()))
        {
             --srclen;
             ++iter;
        }
        // returns maxlen, if no null-terminator found
        return ElementType::IterDiff(src, iter);
    }

    static BasisElementCount StringSize
        (ConstPointer                   src,
         ElementCount                   srclen)
    {
        if (src == 0) return 0;

        ConstIterator iter(src);
        // in this function, null terminators are counted as normal elements
        while (srclen > 0)
        {
             --srclen;
             ++iter;
        }
        return ElementType::IterDiff(src, iter);
    }

    /*--------------------------------------------------------------------------
    function:     Compare()
    description:  Compares two memory blocks
    
                  - A lexical comparison of two memory blocks is performed,
                    based on the static string element functions 
                    ElementType::IsEqualTo() and ElementType::IsLessThan().
                  - The memory blocks are starting at 'lhs' and 'rhs', respectively.
                  - 'lsize' and 'rsize' specify the size in buffer elements 
                    of 'lhs' and 'rhs', respectively.
    arguments:    lhs    [in]    pointer to first memory block
                  rhs    [in]    pointer to second memory block
                  lsize  [in]    size in buffer elements of 'lhs'
                  rsize  [in]    size in buffer elements of 'rhs'
    return value: -  0, if memory blocks are equal
                  - -1, if 'lhs' is lexically smaller than 'rhs'
                  - +1, if 'lhs' is lexically greater than 'rhs'
    --------------------------------------------------------------------------*/
    static int Compare
        (ConstPointer                   lhs, 
         ConstPointer                   rhs, 
         BasisElementCount              lsize, 
         BasisElementCount              rsize)
    {
        int comp = ElementType::Compare(lhs, rhs, lsize < rsize ? lsize : rsize);

        if (comp != 0)
            return comp;
        else
            if (lsize < rsize)
                return -1;
            else
                if (lsize == rsize)
                    return 0;
                else
                    return 1;
    }

    /*--------------------------------------------------------------------------
    function:     AssignSource()
    description:  Assigns contents of memory block to present string
                  
                  - A memory block with 'srclen' basis elements beginning at 'src'
                    is assigned to the present string.
                  - If 'src' is null, the string is empty afterwards.
                  - It is not checked whether there are string terminators
                    within the memory block.
    arguments:    src     [in]     pointer to memory block
                  srclen  [in]     number of basis elements in memory block
    return value: none
    --------------------------------------------------------------------------*/
    void AssignSource
        (ConstPointer                   src,
         BasisElementCount              srclen)
    {
        CheckSizeOverflow(srclen);
        m_Buffer.ProvideCapacity(srclen, 0);
        m_Buffer.Clear();
        if (src != 0 && srclen > 0)
            m_Buffer.Append(src, srclen);
    }

    /*--------------------------------------------------------------------------
    function:     AssignSource()
    description:  Assigns string/substring to present string
                  
                  - Assigns string 'str'.
                  - If 'from' is given, a substring of 'str' beginning at 
                    character index 'from' and ending at the string end is assigned.
                  - 'from' may range from 0 to 'str'.Size().
                  - If 'len' is specified, at most 'len' characters are assigned.
                  
    arguments:    str    [in]   string to be assigned
                  from   [in]   element position within 'str'
                  len    [in]   max. count of elements to be assigned
    return value: none
    --------------------------------------------------------------------------*/
    void AssignSource
        (const Tools_DynamicUTF8String& str)
    {
        AssignSource(ToPtr(str.Begin()), 
                     StringSize(str.Begin(), str.End()));
    }

    void AssignSource
        (const Tools_DynamicUTF8String& str, 
         BasisElementIndex              from)
    {
        ConstIterator iter = str.GetIteratorAtBasis(from);
        AssignSource(ToPtr(iter), StringSize(iter, str.End()));
    }

    void AssignSource
        (const Tools_DynamicUTF8String& str, 
         BasisElementIndex              from,
         ElementCount                   len)
    {
        ConstIterator iter = str.GetIteratorAtBasis(from);
        AssignSource(ToPtr(iter), StringSize(iter, len, str.End()));
    }

    /*--------------------------------------------------------------------------
    function:     AppendSource()
    description:  Appends contents of memory block to present string
                  
                  - A memory block with 'srclen' buffer elements beginning at 'src'
                    is appended to the present string.
                  - If 'src' is null, nothing is done.
                  - It is not checked whether there are string terminators
                    within the memory block.
    arguments:    src    [in]     pointer to memory block
                  srclen [in]     number of buffer elements in memory block
    return value: none
    --------------------------------------------------------------------------*/
    void AppendSource
        (ConstPointer                   src,
         BasisElementCount              srclen)
    {
        if (src    == 0) return;
        if (srclen == 0) return;

        CheckSizeOverflow(BasisSize() + srclen);
        m_Buffer.Append(src, srclen);
    }

    /*--------------------------------------------------------------------------
    function:     InsertSource()
    description:  Inserts contents of memory block into present string at given 
                  position
                  
                  - A memory block with 'srclen' buffer elements beginning at 'src'
                    is inserted into the present string at buffer position 'at'.
                  - 'at' may range from 0 to Size().
                  - If 'src' is null, nothing is done.
                  - It is not checked whether there are string terminators
                    within the memory block.
    arguments:    at     [in]   insert buffer position within the present string
                  src    [in]   pointer to memory block
                  srclen [in]   number of buffer elements in memory block
    return value: none
    --------------------------------------------------------------------------*/
    void InsertSource
        (BasisElementIndex              at,
         ConstPointer                   src,
         BasisElementCount              srclen)
    {
        if (src    == 0) return;
        if (srclen == 0) return;

        CheckSizeOverflow(BasisSize() + srclen);
        ConstIterator iter = GetIteratorAtBasis(at);
        m_Buffer.Insert(ElementType::IterDiff(Begin(), iter), src, srclen);
    }

    /*--------------------------------------------------------------------------
    function:     ReplacePart()
    description:  Replaces substring with given source
                  
                  - A substring of the present string beginning at 'from' 
                    is replaced with 'srclen' buffer elements found at 'src'.
                  - The substring end must be specified either by its element
                    count 'len' or by iterator 'to'.
    arguments:    from    [in]  iterator pointing to beginning of substring
                  to      [in]  iterator pointing to end of substring
                  len     [in]  max. count of elements in substring
                  src     [in]  pointer to source
                  srclen  [in]  size of source in buffer elements
    return value: none
    --------------------------------------------------------------------------*/
    void ReplacePart
        (ConstIterator                  from,
         ElementCount                   len,
         ConstPointer                   src,
         BasisElementCount              srclen)
    {
        assert(ElementType::IsValid(from));
        assert(ToPtr(from) >= ToPtr(Begin()));
        assert(ToPtr(from) <= ToPtr(End()));

        BasisElementCount cutlen = StringSize(from, len, End());
        BasisElementIndex cutpos = ElementType::IterDiff(Begin(), from);
        CheckSizeOverflow(BasisSize() - cutlen + srclen);
        m_Buffer.Replace(cutpos, cutlen, src, srclen);
    }

    void ReplacePart
        (ConstIterator                  from,
         ConstIterator                  to,
         ConstPointer                   src,
         BasisElementCount              srclen)
    {
        assert( ElementType::IsValid ( from ));
        assert( ElementType::IsValid ( to ));
        assert( ToPtr(from) >= ToPtr(Begin()));
        assert( ToPtr(from) <= ToPtr(End()));
        assert( ToPtr(to) >= ToPtr(Begin()));
        assert( ToPtr(to) <= ToPtr(End()));
        assert( ElementType::IterDiff ( from, to ) >= 0 );

        BasisElementCount cutlen = ElementType::IterDiff ( from, to );
        BasisElementIndex cutpos = ElementType::IterDiff ( Begin(), from );

        CheckSizeOverflow ( BasisSize() - cutlen + srclen );
        m_Buffer.Replace  ( cutpos, cutlen, src, srclen );
    }

    /*--------------------------------------------------------------------------
    function:     FindSequence()
    description:  Searches substring
    
                  - The first occurrence of a given element sequence is 
                    searched in the present string starting at buffer index 'from'.
                  - The sequence is limited by 'seqBeg' and 'seqEnd'.
                  - There is no limit to 'from'.

    arguments:    from      [in]  buffer position within the present string
                                  (where the search starts)
                  seqBeg    [in]  iterator at begin of element sequence
                  seqEnd    [in]  iterator behind end of element sequence
                  
    return value: (BasisElementIndex) 
                  - found:               buffer index of sequence,
                  - not found:           NPos,
                  - 'from' out of range: NPos,
                  - input seq. empty:    'from' if not out of range
    --------------------------------------------------------------------------*/
    BasisElementIndex FindSequence
        (BasisElementIndex              from,
         const ConstIterator&           seqBeg,
         const ConstIterator&           seqEnd) const;

    /*--------------------------------------------------------------------------
    function:     ReverseFindSequence()
    description:  Searches substring
    
                  - The last occurrence of a given element sequence is 
                    searched in the present string starting at buffer index 'from'
                    versus the beginning of the string.
                  - The sequence is limited by 'seqBeg' and 'seqEnd'.
                  - There is no limit to 'from'.

    arguments:    from      [in]  buffer position within the present string
                                  (where the search starts)
                  seqBeg    [in]  iterator at begin of element sequence
                  seqEnd    [in]  iterator behind end of element sequence
                  
    return value: (BasisElementIndex) 
                  - found:               buffer index of sequence,
                  - not found:           NPos,
                  - 'from' out of range: NPos
                  - input seq. empty:    'from' if not out of range
    --------------------------------------------------------------------------*/
    BasisElementIndex ReverseFindSequence
        (BasisElementIndex              from,
         const ConstIterator&           seqBeg,
         const ConstIterator&           seqEnd) const;

    /*--------------------------------------------------------------------------
    function:     FindFirstOfSet()
    description:  Searches first occurrence of any element from set
    
                  - The first occurrence of any element of a given 
                    set of elements is searched in the present string.
                  - The set is limited by 'setBeg' and 'setEnd'.
                  - There is no limit to 'from'.

    arguments:    from      [in]  buffer position within the present string
                                  (where the search starts)
                  setBeg    [in]  iterator at beginning of set of elements
                  setEnd    [in]  iterator at end of set of element
                  
    return value: (BasisElementIndex) 
                  found:               buffer index of first occurrence
                  not found:           NPos
                  empty input set:     NPos
                  'from' out of range: NPos
    --------------------------------------------------------------------------*/
    BasisElementIndex FindFirstOfSet
        (BasisElementIndex              from,
         const ConstIterator&           setBeg,
         const ConstIterator&           setEnd) const;

    /*--------------------------------------------------------------------------
    function:     FindLastOfSet()
    description:  Searches last occurrence of any element from set
    
                  - The last occurrence of any element of a given 
                    set of elements is searched in the present string.
                  - The set is limited by 'setBeg' and 'setEnd'.
                  - There is no limit to 'from'.

    arguments:    from      [in]  buffer position within the present string
                                  (where the search starts)
                  setBeg    [in]  iterator at beginning of set of elements
                  setEnd    [in]  iterator at end of set of element
                  
    return value: (BasisElementIndex) 
                  found:               buffer index of last occurrence
                  not found:           NPos
                  empty input set:     NPos
                  'from' out of range: NPos
    --------------------------------------------------------------------------*/
    BasisElementIndex FindLastOfSet
        (BasisElementIndex              from,
         const ConstIterator&           setBeg,
         const ConstIterator&           setEnd) const;

    /*--------------------------------------------------------------------------
    function:     FindFirstNotOfSet()
    description:  Searches first occurrence of any element  n o t  from set
    
                  - The first occurrence of any element which is not in a given 
                    set of elements is searched in the present string.
                  - The set is limited by 'setBeg' and 'setEnd'.
                  - There is no limit to 'from'.

    arguments:    from      [in]  buffer position within the present string
                                  (where the search starts)
                  setBeg    [in]  iterator at beginning of set of elements
                  setEnd    [in]  iterator at end of set of element
                  
    return value: (BasisElementIndex) 
                  found:               buffer index of first element not from set
                  not found:           NPos
                  empty input set:     NPos
                  'from' out of range: NPos
    --------------------------------------------------------------------------*/
    BasisElementIndex FindFirstNotOfSet
        (BasisElementIndex              from,
         const ConstIterator&           setBeg,
         const ConstIterator&           setEnd) const;

    /*--------------------------------------------------------------------------
    function:     FindLastNotOfSet()
    description:  Searches last occurrence of any element  n o t  from set
    
                  - The last occurrence of any element which is not in a given 
                    set of elements is searched in the present string.
                  - The set is limited by 'setBeg' and 'setEnd'.
                  - There is no limit to 'from'.

    arguments:    from      [in]  buffer position within the present string
                                  (where the search starts)
                  setBeg    [in]  iterator at beginning of set of elements
                  setEnd    [in]  iterator at end of set of element
                  
    return value: (BasisElementIndex) 
                  found:               buffer index of last element not from set
                  not found:           NPos
                  empty input set:     NPos
                  'from' out of range: NPos
    --------------------------------------------------------------------------*/
    BasisElementIndex FindLastNotOfSet
        (BasisElementIndex              from,
         const ConstIterator&           setBeg,
         const ConstIterator&           setEnd) const;

    /*--------------------------------------------------------------------------
    function:     GetMatchPosition(), ReverseGetMatchPositionI() + II()
    description:  Searches for match position of single element
    
                  - The basis element 'elem' is searched in the element
                    sequence starting at 'seqptr' with length 'seqlen'.
                  - For the reverse algorithm, 'seqptr' denotes the end
                    of the sequence.

    arguments:    strIter   [in] (reverse) iterator at (reverse) beginning element sequence
                  strEnd    [in] (reverse) iterator at (reverse) end element sequence
                  pattIter  [in] (reverse) iterator at element to be searched
                  
    return value: (ConstIterator/ConstReverseIterator) 
                  (reverse) iterator at element in sequence if found,
                  uninitialized iterator if not found
    --------------------------------------------------------------------------*/
    static ConstIterator GetMatchPosition
        (ConstIterator                  strIter, 
         const ConstIterator&           strEnd,
         const ConstIterator&           pattIter)
    {
        for (; strIter != strEnd; ++strIter)
            if (ElementType::Compare(ToPtr(strIter), ToPtr(pattIter)) == 0)
                return strIter;
        return ConstIterator();
    }

    static ConstReverseIterator GetReverseMatchPositionI
        (ConstReverseIterator           strIter, 
         const ConstReverseIterator&    strEnd,
         const ConstIterator&           pattIter)
    {
        for (; strIter != strEnd; ++strIter)
            if (ElementType::Compare(ToPtr(strIter), ToPtr(pattIter)) == 0)
                return strIter;
        return ConstReverseIterator();
    }

    static ConstReverseIterator GetReverseMatchPositionII
        (ConstIterator                  strIter, 
         const ConstIterator&           strEnd,
         const ConstReverseIterator&    pattIter)
    {
        for (; strIter != strEnd; ++strIter)
            if (ElementType::Compare(ToPtr(strIter), ToPtr(pattIter)) == 0)
                return ConstReverseIterator(ToPtr(strIter));
        return ConstReverseIterator();
    }

    /*--------------------------------------------------------------------------
    function:     IsAssigned()
    description:  Indicates whether string buffer is assigned
    arguments:    none
    return value: (SAPDB_Bool)
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsAssigned() const
    { 
        return m_Buffer.IsAssigned();
    }

    /*--------------------------------------------------------------------------
    function:     IsAscii7()
    description:  Indicates whether given string contains only ascii-7 characters
    arguments:    src    [in]   memory block to be checked
    return value: (SAPDB_Bool)
    --------------------------------------------------------------------------*/
    static SAPDB_Bool IsAscii7
        (const SAPDB_Char*              src)
    { 
        if (src == 0) 
            return true;

        while (*src != ElementType::NullTerminator())
        {
            if ((*src++ & 0x80) != 0)   // check whether high bit is set
                return false;
        }
        return true;
    }

    /*--------------------------------------------------------------------------
    function:     EmptyString()
    description:  Returns pointer to empty string
    arguments:    none
    return value: (Pointer)
    --------------------------------------------------------------------------*/
    static ConstPointer EmptyString()
    {
        static BasisElementType _empty_str(0);
        return &_empty_str;
    }

    /*----------------------------------------------------------------------
    function:     ToPtr()
    description:  Converts iterator to pointer
    arguments:    iter [in]    iterator to be converted
    return value: (ConstPointer/Pointer) 
    prototypes.   4
    ----------------------------------------------------------------------*/
    static ConstPointer ToPtr
        (ConstIterator                  iter)
    {
        return ElementType::ToPtr(iter);
    }

    static Pointer ToPtr
        (Iterator                       iter)
    {
        return ElementType::ToPtr(iter);
    }

    static ConstPointer ToPtr
        (ConstReverseIterator           iter)
    {
        return ElementType::ToPtr(iter);
    }

    static Pointer ToPtr
        (ReverseIterator                iter)
    {
        return ElementType::ToPtr(iter);
    }

    /*--------------------------------------------------------------------------
    declaration : m_Buffer
    description:  Buffer member containing the string
    --------------------------------------------------------------------------*/
    BufferType  m_Buffer;
};
/*!*****************************************************************************

   endclass: Tools_DynamicUTF8String

*******************************************************************************/



#endif //Tools_DynamicUTF8String_HPP


