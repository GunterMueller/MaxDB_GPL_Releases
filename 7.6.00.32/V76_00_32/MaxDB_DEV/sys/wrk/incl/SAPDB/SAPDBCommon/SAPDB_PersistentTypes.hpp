  /*!***************************************************************************

  module:       SAPDB_PersistentTypes.hpp

  ------------------------------------------------------------------------------

  responsible:  TillL

  special area: SAPDB Common

  description:  Template classes for types which operate on persistent data
                
                - SAPDB_Persistent:         Base class template for all persistent data types
                - SAPDB_IPersistentBitRange: Extended base class for bit range oriented persistent types.
                - SAPDB_PersistentBitRange: To be used with bit range oriented integer data types
                - SAPDB_PersistentBool:     To be used with boolean flags

                Template Arguments used throughout this module:
                - TInternal   internal type 
                - TPersistent persistent type
                - TPBytesize  assumed size of TPersistent in bytes
                - TPBitoffs   offset in bits within TPersistent 
                - TPBitsize   number of contiguous bits within TPersistent

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


#ifndef SAPDB_PERSISTENTTYPES_HPP
#define SAPDB_PERSISTENTTYPES_HPP


#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"


#if defined(WIN32)
#pragma warning( disable: 4800 )  // disable type conversion warning for unused methods
#endif


/*!*****************************************************************************

   class: SAPDB_Persistent

   description: Class template for all persistent data types
   
   - Base class of all classes that transform an internal data representation 
     to and from a persistent representation
   - Public type PersistentFormat is defined which describes the persistent data format.
   - Compile time size check:
      - To assure that TPersistent really matches the required persistent data size in bytes, 
        a compile time check is performed on whether sizeof(TPersistent) is identical to <TPBytesize>.
      - If given sizes are unequal, a compiler error:
        " error C2679: binary '=' : no operator defined which takes a right-hand operand of type ...".
        is generated.
      - If you are confronted with this error, check whether sizeof(TPersistent) is really 
        the size you mean.

   example: 

     Conventional implementation without use of SAPDB_Persistent:

     typedef SAPDB_UInt4 Version;
     typedef SAPDB_UInt4 Counter;

     struct PersistentHeader {                  // struct header should reproduce persistent format
        Version m_Version;                      // internal and persistent format and size may differ
        Counter m_Counter;                      // internal and persistent format and size may differ
     };

     Implementation using SAPDB_Persistent:

     typedef SAPDB_UInt4 Version;               // for internal use (independent from persistent use)
     typedef SAPDB_UInt4 Counter;               // for internal use (independent from persistent use)

     class Page {
        typedef SAPDB_Persistent<Version, SAPDB_UInt4, 4> PersistentVersion;     // for use with persistent read/write only
        typedef SAPDB_Persistent<Counter, SAPDB_UInt4, 4> PersistentCounter;     // for use with persistent read/write only

     struct PersistentHeader {
        PersistentVersion::PersistentFormat m_Version;    // guaranteed to have persistent format and size
        PersistentCounter::PersistentFormat m_Counter;    // guaranteed to have persistent format and size
        };
     ...
     };

    For further practical examples see Converter_Page.hpp, Kernel_Page.hpp

   arguments:
   - TInternal   defines your desired internal data format
   - TPersistent defines your desired persistent data format
   - TPBytesize  defines the persistent data size in bytes provided for an instance of TPersistent
                 (required for compile time size check against sizeof(TPersistent))

*******************************************************************************/
template <class TInternal, class TPersistent, unsigned int TPBytesize> 
class SAPDB_Persistent
{
public: 

    /*!-------------------------------------------------------------------------
    declaration: PersistentFormat
    description: Describes the persistent format of a class which implements this
                 interface
    --------------------------------------------------------------------------*/
    typedef TPersistent PersistentFormat;

    /*!-------------------------------------------------------------------------
    function:     SAPDB_Persistent()
    description:  Default constructor; 
                  requires default constructor for TInternal
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    SAPDB_Persistent() 
    {
        SAPDBERR_ASSERT_STATE(PersistentByteSizeCheck());
    }

    /*!-------------------------------------------------------------------------
    function:     SAPDB_Persistent()
    description:  Constructor; assigns value in internal format
    arguments:    value [in]     initialization value (internal type)
    return value: none
    --------------------------------------------------------------------------*/
    explicit SAPDB_Persistent
        (const TInternal&               value) 
        : 
        m_Value(value)
    {
        SAPDBERR_ASSERT_STATE(PersistentByteSizeCheck());
    }

    /*!-------------------------------------------------------------------------
    function:     SetInternalValue()
    description:  Assigns value in internal format
    arguments:    value [in]    value to be assigned (internal type)
    return value: none
    --------------------------------------------------------------------------*/
    void SetInternalValue
        (const TInternal&               value)
    { 
        m_Value = value;
    }

    /*!-------------------------------------------------------------------------
    function:     SetPersistentValue()
    description:  Assigns value in persistent format
    arguments:    value [in]    value to be assigned (persistent type)
    return value: none
    --------------------------------------------------------------------------*/
    void SetPersistentValue
        (const TPersistent&             value)
    { 
        m_Value = ConvertP2I(value); 
    }

    /*!-------------------------------------------------------------------------
    function:     GetPersistentValue()
    description:  Returns value of this instance in persistent format
    arguments:    none
    return value: (TPersistent) 
    --------------------------------------------------------------------------*/
    TPersistent GetPersistentValue() const
    { 
        return ConvertI2P(m_Value); 
    }

    /*!-------------------------------------------------------------------------
    function:     GetInternalValue()
    description:  Returns value of this instance in internal format
    arguments:    none
    return value: (TInternal) 
    --------------------------------------------------------------------------*/
    TInternal GetInternalValue() const
    { 
        return m_Value; 
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertInternalToPersistent()
    description:  Converts input value from internal to persistent format
    arguments:    value [in]   value to be converted (internal type)
    return value: (TPersistent) converted value
    --------------------------------------------------------------------------*/
    static TPersistent ConvertInternalToPersistent
        (const TInternal&               value)
    {
		return static_cast<TPersistent>(*&value);  
		//this strange expression is used due to the strange behaviour of 
		// the sun compiler which does not compile the line 
		//return static_cast<TPersistent>(value);
		//but the line written above
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertPersistentToInternal()
    description:  Converts input value from persistent to internal format
    arguments:    value [in]   value to be converted (persistent type)
    return value: (TInternal) converted value
    --------------------------------------------------------------------------*/
    static TInternal ConvertPersistentToInternal
        (const TPersistent&             value)
    {
        return static_cast<TInternal>(value);
    }

protected:

    /*!-------------------------------------------------------------------------
    function:     ConvertI2P()
    description:  Converts input value from internal to persistent format
                  (internal use only)
    arguments:    value [in]   value to be converted (internal type)
    return value: (TPersistent) converted value
    --------------------------------------------------------------------------*/
    virtual TPersistent ConvertI2P
        (const TInternal&               value) const
    {
        return ConvertInternalToPersistent(value);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertP2I()
    description:  Converts input value from persistent to internal format
                  (internal use only)
    arguments:    value [in]   value to be converted (persistent type)
    return value: (TInternal) converted value
    --------------------------------------------------------------------------*/
    virtual TInternal ConvertP2I
        (const TPersistent&             value) const
    {
        return ConvertPersistentToInternal(value);
    }

    /*!------------------------------------------------------------------------- 
    declaration: m_Value
    description: "Intrinsic value" of this instance
    --------------------------------------------------------------------------*/
    TInternal m_Value;

private:

    /*--------------------------------------------------------------------------
    function:     PersistentByteSizeCheck()
    description:  Performs size check and provokes compiler error if check fails
    arguments:    none
    return value: (bool)
    --------------------------------------------------------------------------*/
    static bool PersistentByteSizeCheck();
};
/*!*****************************************************************************

   endclass: SAPDB_Persistent

*******************************************************************************/



/*!*****************************************************************************

   class: SAPDB_IPersistentBitRange

   description: Extended interface for bit range oriented persistent types.

   - Interface for bit range oriented persistent data representations.

   arguments:
   - TPersistent defines your desired persistent format

*******************************************************************************/
template <class TPersistent> 
class SAPDB_IPersistentBitRange
{
public: 

    /*!-------------------------------------------------------------------------
    function:     SAPDB_IPersistentBitRange()
    description:  Default constructor
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    SAPDB_IPersistentBitRange() 
    {}

    /*!-------------------------------------------------------------------------
    function:     PersistentBitRangesDoNotOverlap()
    description:  Checks whether given bit formats do not overlap
    arguments:    op1 [in]    first operand
                  op2 [in]    second operand
    return value: (bool)
    --------------------------------------------------------------------------*/
    friend bool PersistentBitRangesDoNotOverlap
        (const SAPDB_IPersistentBitRange<TPersistent>& op1,
         const SAPDB_IPersistentBitRange<TPersistent>& op2)
    { 
        return (op1.GetPersistentBitFormat() &
                op2.GetPersistentBitFormat())  == 0;
    }

protected:

    /*!-------------------------------------------------------------------------
    function:     GetPersistentBitFormat()
    description:  Returns bit mask with all persistent bits switched to 1
    arguments:    none
    return value: (TPersistent) bit mask defining the persistent format
    --------------------------------------------------------------------------*/
    virtual TPersistent GetPersistentBitFormat() const = 0;

};
/*!*****************************************************************************

   endclass: SAPDB_IPersistentBitRange

*******************************************************************************/



/*!*****************************************************************************

   class: SAPDB_PersistentBitRange

   description: Class template to be used with bit range oriented integer data types

   - RESTRICTIONS:
   - Only integer types allowed for TPersistent (UInt1, UInt2, UInt4, UInt8)
   - TInternal must offer explicit or implicit type conversion to and from TPersistent

   - for bool types use SAPDB_PersistentBool
   - For further explanation see SAPDB_Persistent.

   arguments:
   - TInternal   internal type (must be defined elsewhere)
   - TPersistent defines your desired persistent format
   - TPBytesize  defines the persistent data size in bytes provided for an instance of TPersistent
                 (required for compile time size check against sizeof(TPersistent))
   - TPBitoffs    offset in bits within TPersistent at which the internal values are to be stored
                 (default 0)
   - TPBitsize    number of contiguous bits within TPersistent in which the internal values are to be stored
                 (default sizeof(TPersistent))

   example:

     In this example, it is assumed that several data values originating 
     from different internal types are "crushed" on a single persistent byte.

     Internal types:
     ---------------
     typedef enum {Data = 1, Converter, Log, Save} PageType;
     typedef enum {Small, Large} Bigness;
     typedef SAPDB_Byte FewBits; // value range: 0..3
     bool

     Layout of the persistent byte:
     ------------------------------
     bit 0:     unused
     bit 1,2,3: PageType
     bit 4:     BigNessType
     bit 5,6:   FewBits
     bit 7:     bool

     Efficient management of persistent data requires the following type definitions to be done:
     -------------------------------------------------------------------------------------------
     typedef SAPDB_PersistentBitRange<PageType, SAPDB_Byte, 1, 1, 3> PersistentPageType;
     typedef SAPDB_PersistentBitRange<Bigness,  SAPDB_Byte, 1, 4, 1> PersistentBigness;
     typedef SAPDB_PersistentBitRange<FewBits,  SAPDB_Byte, 1, 5, 2> PersistentFewBits;
     typedef SAPDB_PersistentBool              <SAPDB_Byte, 1, 7>    PersistentHasChanged;

     It is possible to work with the persistent types in the same way as with
     the corresponding, "underlying" internal types (like enum, int, bool):
     ------------------------------------------------------------------------
     PersistentPageType   p(Converter);
     PersistentBigness    s(Large);
     PersistentFewBits    h(3);
     PersistentHasChanged x(h > 0);

     PersistentPageType pp(Data);
     p = pp;

     switch (s.GetInternalValue()) {
        case Small: cout << "Small" << endl; break;
        case Large: cout << "Large" << endl; break;
        default:    cout << "not found" << endl; break;
     }
     switch (p.GetInternalValue()) {
        case Data:      cout << "Data" << endl; break;
        case Converter: cout << "Converter" << endl; break;
        default:        cout << "other" << endl; break;
     }
     cout << h.GetInternalValue() << endl;
     cout << (x.GetInternalValue() ? "true" : "false") << endl;


     To assign the (appropriately shifted) persistent value of an instance 
     of a persistent type (here: p, s, h, and x), to a persistent value 
     (here: head.m_Info, see below), use either the GetPersistentValue() or the
     AssignPersistentValueTo() method. The latter updates only the relevant
     bits in the "target" value, the former returns a persistent value, "as is":
     ------------------------------------------------------------------------
     struct Header {
        SAPDB_Byte  m_Info;
     ...};
     Header head;

     either like this:
     -----------------
     head.m_Info = p.GetPersistentValue() + s.GetPersistentValue() 
                 + h.GetPersistentValue() + x.GetPersistentValue();

     or like this:
     -------------
     head.m_Info = 0;                           0 000 0 00 0  (bit 0-7)
     p.AssignPersistentValueTo(head.m_Info);    0 010 0 00 0
     s.AssignPersistentValueTo(head.m_Info);    0 010 1 00 0
     h.AssignPersistentValueTo(head.m_Info);    0 010 1 11 0
     x.AssignPersistentValueTo(head.m_Info);    0 010 1 11 1

  
     To assign a persistent value (here: m_Info) to an instance of a persistent type
     (here: p, s, h, and x) use the AssignPersistentValueFrom() method.
     In the example below, head.m_Info ist the "source" which serves all 4 variables,
     each of them masking the relevant bits:
     ------------------------------------------------------------------------
     p.AssignPersistentValueFrom(head.m_Info);
     s.AssignPersistentValueFrom(head.m_Info);
     h.AssignPersistentValueFrom(head.m_Info);
     x.AssignPersistentValueFrom(head.m_Info);

     if (p.GetInternalValue() == Converter) ...
     and so on...

*******************************************************************************/
template <class TInternal, class TPersistent, unsigned int TPBytesize, 
          unsigned int TPBitoffs = 0, 
          unsigned int TPBitsize = SAPDB_BITS_PER_BYTE * sizeof(TPersistent)> 
class SAPDB_PersistentBitRange 
    : public SAPDB_Persistent<TInternal, TPersistent, TPBytesize>,
      public SAPDB_IPersistentBitRange<TPersistent>
{
public: 

    /*!-------------------------------------------------------------------------
    function:     SAPDB_PersistentBitRange()
    description:  Default constructor; requires default constructor for TInternal
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    SAPDB_PersistentBitRange() 
    {}

    /*!-------------------------------------------------------------------------
    function:     SAPDB_PersistentBitRange()
    description:  Constructor; assigns instance of class TInternal

                  - Checks whether input value fits persistent bit format 
                    defined by GetPersistentBitFormat(), i.e.,
                    whether 'value' & ~PersistentBitFormat == 0
    arguments:    value [in]   initialization value
    return value: none
    --------------------------------------------------------------------------*/
    SAPDB_PersistentBitRange
        (const TInternal&               value) 
        : 
        SAPDB_Persistent<TInternal, TPersistent, TPBytesize> (value) 
    {
        SAPDBERR_ASSERT_ARGUMENT(InternalValueFitsBitFormat(value));
    }

    /*!-------------------------------------------------------------------------
    function:     AssignPersistentValueFrom()
    description:  Assigns (reads) input value in persistent format

                  - Bits in 'value' other than in those defined in 
                    PersistentBitFormat are not relevant for the result.
    arguments:    value [in]   value to be assigned 
    return value: (TInternal) result in internal format
    --------------------------------------------------------------------------*/
    void AssignPersistentValueFrom
        (const TPersistent&             value) 
    { 
        this->m_Value = ConvertPersistentToInternal(value);
    }

    /*!-------------------------------------------------------------------------
    function:     AssignPersistentValueTo()
    description:  Assigns (writes) this value to 'value' after conversion to
                  persistent format

                  - Bits in 'value' other than in those defined in 
                    PersistentBitFormat remain unchanged.
    arguments:    value [in/out]   value which is to be updated
    return value: (TPersistent&) updated value
    --------------------------------------------------------------------------*/
    void AssignPersistentValueTo
        (TPersistent&                   value) const
    { 
        value =   (value & ~PersistentBitFormat) | 
                    ConvertInternalToPersistent(this->m_Value);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertInternalToPersistent()
    description:  Converts input value from internal to persistent format
    arguments:    value [in]   value to be converted (class TInternal)
    return value: (TPersistent) converted value
    --------------------------------------------------------------------------*/
    static TPersistent ConvertInternalToPersistent
        (const TInternal&               value)
    {
        SAPDBERR_ASSERT_ARGUMENT(InternalValueFitsBitFormat(value));

        return static_cast<TPersistent>
                  (TPersistent(value) << TPBitoffs);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertPersistentToInternal()
    description:  Converts input value from persistent to internal format
    arguments:    value [in]   value to be converted (class TPersistent)
    return value: (TInternal) converted value
    --------------------------------------------------------------------------*/
    static TInternal ConvertPersistentToInternal
        (const TPersistent&             value)
    {
        return static_cast<TInternal>
                    (static_cast<TPersistent>
                        ((value & PersistentBitFormat) >> TPBitoffs));
    }

protected:

    /*!------------------------------------------------------------------------- 
    declaration:  PersistentBitFormat
    description:  Static member containing appropriately shifted persistent 
                  bit format to be used with mask operations
    --------------------------------------------------------------------------*/
    static TPersistent PersistentBitFormat;

    /*!-------------------------------------------------------------------------
    function:     GetPersistentBitFormat()
    description:  Returns bit mask with persistent bits switched to 1
    arguments:    none
    return value: (TPersistent) bit mask defining the persistent format
    --------------------------------------------------------------------------*/
    virtual TPersistent GetPersistentBitFormat() const
    {
        return PersistentBitFormat; 
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertI2P()
    description:  Converts input value from internal to persistent format
                  (internal use only)
    arguments:    value [in]   value to be converted (class TInternal)
    return value: (TPersistent) converted value
    --------------------------------------------------------------------------*/
    virtual TPersistent ConvertI2P
        (const TInternal&               value) const
    {
        return ConvertInternalToPersistent(value);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertP2I()
    description:  Converts input value from persistent to internal format
                  (internal use only)
    arguments:    value [in]   value to be converted (class TPersistent)
    return value: (TInternal) converted value
    --------------------------------------------------------------------------*/
    virtual TInternal ConvertP2I
        (const TPersistent&                 value) const
    {
        return ConvertPersistentToInternal(value);
    }

private:

    /*!-------------------------------------------------------------------------
    function:     CreatePersistentBitFormat()
    description:  Creates persistent bit format (see PersistentBitFormat)
    arguments:    none
    return value: (TPersistent) bit format
    --------------------------------------------------------------------------*/
    static TPersistent CreatePersistentBitFormat();

    /*!-------------------------------------------------------------------------
    function:     InternalValueFitsBitFormat()
    description:  Checks whether input value fits internal bit format and range 
                  defined by <TPBitsize>
    arguments:    value [in]   value to be checked (internal type)
    return value: (bool)
    --------------------------------------------------------------------------*/
    static bool InternalValueFitsBitFormat
        (const TInternal&               value)
    {
		//return (ConvertInternalToPersistent (value)  & ~(PersistentBitFormat >> TPBitoffs)) == 0;
       return (TPersistent(value) & ~(PersistentBitFormat >> TPBitoffs)) == 0;
    }

};
/*!*****************************************************************************

   endclass: SAPDB_PersistentBitRange

*******************************************************************************/



/*!*****************************************************************************

   class: SAPDB_PersistentBool

   description: Class template to be used with boolean flags

   - persistent representation reserves 1 bit at specified offset within 
     persistent data format
   - persistent value of true is 1, of false is 0.
   - For further explanation see SAPDB_Persistent (above)

   example: see SAPDB_PersistentBitRange (above)

   arguments:
   - TPersistent defines your desired persistent format
   - TPBytesize  defines the persistent data size in bytes provided for an instance of TPersistent
                 (required for compile time size check against sizeof(TPersistent))
   - TPBitoffs   offset in bits within TPersistent at which the bool value is to be stored
                 (default 0)

*******************************************************************************/
template <class TPersistent, unsigned int TPBytesize, unsigned int TPBitoffs = 0> 
class SAPDB_PersistentBool 
    : public SAPDB_PersistentBitRange<bool, TPersistent, TPBytesize, TPBitoffs, 1>
{
    typedef SAPDB_PersistentBitRange<bool, TPersistent, TPBytesize, TPBitoffs, 1>
            Parent;
public: 


    /*!-------------------------------------------------------------------------
    function:     SAPDB_PersistentBool()
    description:  Constructor; assigns bool value.
    arguments:    value [in]   initialization value
    return value: none
    --------------------------------------------------------------------------*/
    SAPDB_PersistentBool(const bool& value = false) 
        : SAPDB_PersistentBitRange<bool, TPersistent, TPBytesize, TPBitoffs, 1>
          (value)
    {}

    /*!-------------------------------------------------------------------------
    function:     ConvertInternalToPersistent()
    description:  Converts input value from internal to persistent format
    arguments:    value [in]   value to be converted (bool)
    return value: (TPersistent) converted value
    --------------------------------------------------------------------------*/
    static TPersistent ConvertInternalToPersistent(const bool& istrue)
    {
        return istrue ? static_cast<TPersistent>(1 << TPBitoffs) 
                      : static_cast<TPersistent>(0);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertPersistentToInternal()
    description:  Converts input value from persistent to internal format
    arguments:    value [in]   value to be converted (persistent type)
    return value: (bool) converted value
    --------------------------------------------------------------------------*/
    static bool ConvertPersistentToInternal(const TPersistent& value)
    {
        return ((value & Parent::PersistentBitFormat) >> TPBitoffs) == 1; // true if 1
    }

protected:

    /*!-------------------------------------------------------------------------
    function:     ConvertI2P()
    description:  Converts input value from internal to persistent format
                  (internal use only)
    arguments:    value [in]   value to be converted (bool)
    return value: (TPersistent) converted value
    --------------------------------------------------------------------------*/
    virtual TPersistent ConvertI2P(const bool& istrue) const
    {
        return ConvertInternalToPersistent(istrue);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertP2I()
    description:  Converts input value from persistent to internal format
                  (internal use only)
    arguments:    value [in]   value to be converted (persistent type)
    return value: (bool) converted value
    --------------------------------------------------------------------------*/
    virtual bool ConvertP2I(const TPersistent& value) const
    {
        return ConvertPersistentToInternal(value);
    }

};
/*!*****************************************************************************

   endclass: SAPDB_PersistentBool

*******************************************************************************/





// -----------------------------------------------------------------------------
// Implementation:
// -----------------------------------------------------------------------------


/*******************************************************************************

   class: SAPDB_PersistentCompileTimeRequiredSizeDummy

   description: 
   - DO NOT USE -- IMPLEMENTATION DETAIL!
   - Dummy template which is used for size checks within class
     SAPDB_Persistent.
   arguments:
   - dummy_size     is used to define a byte array with <dummy_size> bytes

*******************************************************************************/
template<int dummy_size> 
class SAPDB_PersistentCompileTimeRequiredSizeDummy
{
    char buffer[dummy_size];
public:
    void operator = (const SAPDB_PersistentCompileTimeRequiredSizeDummy<dummy_size>& source) 
    {}
};
/*******************************************************************************

   endclass: SAPDB_PersistentCompileTimeRequiredSizeDummy

*******************************************************************************/


/*******************************************************************************

   class: SAPDB_Persistent

*******************************************************************************/
template <class TInternal, class TPersistent, unsigned int TPBytesize> 
bool SAPDB_Persistent<TInternal, TPersistent, TPBytesize>::PersistentByteSizeCheck()
{
    SAPDB_PersistentCompileTimeRequiredSizeDummy<sizeof(TPersistent)> checker1;
    SAPDB_PersistentCompileTimeRequiredSizeDummy<TPBytesize> checker2;

    // the following statement will provoke a compiler error 
    // if sizeof(TPersistent) differs from TPBytesize:
    checker1 = checker2;

    return true;
}
// -----------------------------------------------------------------------------


/*******************************************************************************

   class: SAPDB_PersistentBitRange

*******************************************************************************/
template <class TInternal, class TPersistent, unsigned int TPBytesize, 
          unsigned int TPBitoffs, unsigned int TPBitsize> 
TPersistent 
SAPDB_PersistentBitRange<TInternal, TPersistent, TPBytesize, TPBitoffs, TPBitsize>
::CreatePersistentBitFormat()
{
    TPersistent range = 1;
    for (int i=1; i<TPBitsize; i++) {
        range <<= 1;
        range++;
    }
    return static_cast<TPersistent>(range << TPBitoffs);
}
// -----------------------------------------------------------------------------

template <class TInternal, class TPersistent, unsigned int TPBytesize, 
          unsigned int TPBitoffs, unsigned int TPBitsize> 
TPersistent 
SAPDB_PersistentBitRange<TInternal, TPersistent, TPBytesize, TPBitoffs, TPBitsize>
        ::PersistentBitFormat = 
SAPDB_PersistentBitRange<TInternal, TPersistent, TPBytesize, TPBitoffs, TPBitsize>
        ::CreatePersistentBitFormat();




#endif // SAPDB_PERSISTENTTYPES_HPP
