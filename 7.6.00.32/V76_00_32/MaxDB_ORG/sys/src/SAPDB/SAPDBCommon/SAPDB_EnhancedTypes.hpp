/*!*****************************************************************************

  module:       SAPDB_EnhancedTypes.hpp

  ------------------------------------------------------------------------------

  responsible:  Till Luhmann

  special area: SAPDBCommon

  description:  Base types with useful functionality added 
                
  see also:     

  Copyright (c) 2000-2005 SAP AG

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

#ifndef SAPDB_ENHANCEDTYPES_HPP
#define SAPDB_ENHANCEDTYPES_HPP


#include "SAPDBCommon/SAPDB_Types.hpp"


/*!*****************************************************************************

    class: SAPDB_Invalidatable

    description: Encapsules base type
    
    Offers possibility of adding a specific value
    which indicates that an instance is invalid.

*******************************************************************************/
template <class T, T InvalidValue>
class SAPDB_Invalidatable
{
    typedef SAPDB_Invalidatable<T, InvalidValue>    Invalidatable;

public:

    typedef T                                       ValueType;

    /*!-------------------------------------------------------------------------
    function:     SAPDB_Invalidatable()
    description:  Default constructor
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    SAPDB_Invalidatable()
        : 
        m_Value(InvalidValue)
    {}
   
    /*!-------------------------------------------------------------------------
    function:     SAPDB_Invalidatable()
    description:  Constructor; sets value
    arguments:    value [in]    value to be assigned
    return value: none
    --------------------------------------------------------------------------*/
    SAPDB_Invalidatable
        (const ValueType&               value)
        : 
         m_Value(value)
    {}
   
    /*!-------------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator
    arguments:    value [in]    value to be assigned
    return value: (Invalidatable&) this instance
    --------------------------------------------------------------------------*/
    Invalidatable& operator=
        (const ValueType&               value)
    { 
        m_Value = value; 
        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     operator()()
    description:  Function operator 
    arguments:    none
    return value: (ValueType) actual value
    --------------------------------------------------------------------------*/
    ValueType operator()() const
    { 
        return m_Value; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator ValueType()
    description:  Type conversion operator; converts to ValueType
    arguments:    none
    return value: (ValueType) actual value
    --------------------------------------------------------------------------*/
    operator ValueType() const
    { 
        return m_Value; 
    }

    /*!-------------------------------------------------------------------------
    function:     Invalidate()
    description:  Assigns an invalid value
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    void Invalidate()
    {
        m_Value = InvalidValue;
    }

    /*!-------------------------------------------------------------------------
    function:     IsValid()
    description:  Checks whether actual value is valid
    arguments:    none
    return value: (SAPDB_Bool) true if valid
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsValid() const
    {
        return m_Value != InvalidValue;
    }

    /*!-------------------------------------------------------------------------
    function:     IsInvalid()
    description:  Checks whether actual value is invalid
    arguments:    none
    return value: (SAPDB_Bool) true if invalid
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsInvalid() const
    {
        return m_Value == InvalidValue;
    }

    /*!-------------------------------------------------------------------------
    function:     operator+=()
    description:  Addition operator
    arguments:    value [in]    value to be added
    return value: (Invalidatable&) this instance
    --------------------------------------------------------------------------*/
    Invalidatable& operator+=
        (const ValueType&               value) 
    {   
        m_Value += value; 
        return *this; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator-=()
    description:  Substraction operator
    arguments:    value [in]    value to be subtracted
    return value: (Invalidatable&) this instance
    --------------------------------------------------------------------------*/
    Invalidatable& operator-=
        (const ValueType&               value) 
    {   
        m_Value -= value; 
        return *this; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator+*=()
    description:  Multiplication operator
    arguments:    value [in]    value to be multiplied
    return value: (Invalidatable&) this instance
    --------------------------------------------------------------------------*/
    Invalidatable& operator*=
        (const ValueType&               value) 
    {   
        m_Value *= value; 
        return *this; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator/=()
    description:  Division operator
    arguments:    value [in]    value to be divided by
    return value: (Invalidatable&) this instance
    --------------------------------------------------------------------------*/
    Invalidatable& operator/=
        (const ValueType&               value) 
    {   
        m_Value /= value; 
        return *this; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator%=()
    description:  Modulo operator
    arguments:    value [in]    modulo value
    return value: (Invalidatable&) this instance
    --------------------------------------------------------------------------*/
    Invalidatable& operator%=
        (const ValueType&               value) 
    {   
        m_Value %= value; 
        return *this; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator++(int)
    description:  Postfix increment
    arguments:    none
    return value: (Invalidatable) old value
    --------------------------------------------------------------------------*/
    Invalidatable operator++(int)
    {   
        Invalidatable inv(m_Value);
        ++m_Value;
        return inv; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator++()
    description:  Prefix increment
    arguments:    none
    return value: (Invalidatable) new value
    --------------------------------------------------------------------------*/
    Invalidatable operator++()
    {   
        ++m_Value;
        return *this; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator--(int)
    description:  Postfix decrement
    arguments:    none
    return value: (Invalidatable) old value
    --------------------------------------------------------------------------*/
    Invalidatable operator--(int)
    {   
        Invalidatable inv(m_Value);
        --m_Value;
        return inv; 
    }

    /*!-------------------------------------------------------------------------
    function:     operator--()
    description:  Prefix decrement
    arguments:    none
    return value: (Invalidatable) new value
    --------------------------------------------------------------------------*/
    Invalidatable operator--()
    {   
        --m_Value;
        return *this; 
    }

protected:

    /*--------------------------------------------------------------------------
    declaration: m_Value
    description: actual value
    --------------------------------------------------------------------------*/
    ValueType               m_Value;
};
/*!*****************************************************************************

   endclass: SAPDB_Invalidatable

*******************************************************************************/


#endif // SAPDB_ENHANCEDTYPES_HPP
