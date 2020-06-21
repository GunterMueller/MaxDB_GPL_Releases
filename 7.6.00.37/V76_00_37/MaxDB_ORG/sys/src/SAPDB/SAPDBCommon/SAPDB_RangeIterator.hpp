/*!
  @file    SAPDB_RangeIterator.hpp
  @ingroup Common
  @author  UweH
  @author  MartinB
  @brief   Defines a template for numeric iterators which have a restricted valuerange

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
*/
#ifndef SAPDB_RangeIterator_hpp
#define SAPDB_RangeIterator_hpp



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

/*****************************************************************************/
/*!
   @brief template for numeric iterators, which have only a restricted range
          of valid values and which are used cyclic (e.g. Log_RawDeviceIterator)
*/
/*****************************************************************************/
template <class tNumericBaseClass, class tDistanceClass>
class SAPDB_RangeIterator
{
public:

    /*!
        @brief        Standard constructor
    */
    SAPDB_RangeIterator()
    {}

    /*!
        @brief        Constructor
        @param        begin [in] min. value for this iterator
        @param        offs  [in] startvalue of this iterator
        @param        end   [in] max. value for this iterator
    */
    SAPDB_RangeIterator
        (tNumericBaseClass                    begin,
         tNumericBaseClass                    offs,
         tNumericBaseClass                    end)
        :
         m_Begin  (begin), 
         m_Offset (offs),
         m_End    (end)
    {
        SAPDBERR_ASSERT_ARGUMENT(begin <=  end);
        SAPDBERR_ASSERT_ARGUMENT(offs >= begin);
        SAPDBERR_ASSERT_ARGUMENT(offs  <= end);
    }

    /*!
        @brief        Compares with other iterator
        @param        iter [in]    other
        @return       (SAPDB_Bool) true if unequal
    */
    SAPDB_Bool operator!=
        (const SAPDB_RangeIterator&   iter) const
    {
        return (m_Offset != iter.m_Offset);
    }

    /*!
        @brief        Prefix increment; advances iterator
        @return       (SAPDB_RangeIterator&) this instance
    */
    SAPDB_RangeIterator& operator++()
    {
        if (m_Offset == m_End)
        {
            m_Offset = m_Begin ;
        }
        else
        {
            m_Offset = m_Offset + 1;
        }
        return *this;
    }

    /*!
        @brief        Prefix decrement; steps back iterator
        @return       (SAPDB_RangeIterator&) this instance
    */
    SAPDB_RangeIterator& operator--()
    {
        if (m_Offset == m_Begin) 
        {
            m_Offset = m_End ;
        }
        else
        {
            m_Offset = m_Offset - 1;
        }
        return *this;
    }

    /*!
        @brief        Increment; advances iterator by given number of steps
        @param        steps  [in]  number of steps
        @return       (SAPDB_RangeIterator&) this instance
    */
    SAPDB_RangeIterator& operator+=
        (tDistanceClass                     steps)
    {
        m_Offset = Later(steps);
        return *this;
    }

    /*!
        @brief        Increment; steps back iterator by given number of steps
        @param        steps  [in]  number of steps
        @return       (SAPDB_RangeIterator&) this instance
    */
    SAPDB_RangeIterator& operator-=
        (tDistanceClass                     steps)
    {
        m_Offset = Earlier(steps);
        return *this;
    }

    /*!
        @brief        Returns new iterator positioned given number of steps behind actual iterator
        @param        steps  [in]  number of steps
        @return       (SAPDB_RangeIterator) 
    */
    SAPDB_RangeIterator operator+
        (tDistanceClass               steps)
    {
        return SAPDB_RangeIterator(m_Begin, Later(steps), m_End);
    }

    /*!
        @brief        Returns new iterator positioned given number of steps before actual iterator
        @param        steps  [in]  number of steps
        @return       (SAPDB_RangeIterator) 
    */
    SAPDB_RangeIterator operator-
        (tDistanceClass               steps)
    {
        return SAPDB_RangeIterator(m_Begin, Earlier(steps), m_End);
    }

    /*!
        @brief        Sets position of iterator
        @param        offs [in] position to be set
    */
    void SetPosition
        (tNumericBaseClass                offs)
    {
        SAPDBERR_ASSERT_ARGUMENT(m_Begin <= offs);
        SAPDBERR_ASSERT_ARGUMENT(offs    <= m_End);
        m_Offset = offs;
    }

    /*!
        @brief        Returns actual position of the iterator
    */
    tNumericBaseClass GetPosition() const
    {
        return m_Offset;
    }

    /*!
        @brief        Sets end offset of an exisiting iterator
        @param        end [in] offset to be set
    */
    void SetEndOffset
        (tNumericBaseClass                 end)
    {
        SAPDBERR_ASSERT_ARGUMENT(m_Offset <= end);
        m_End = end;
    }

    /*!
        @brief        Returns the maximum value of the range of this iterator
    */
    tNumericBaseClass GetEndOffset() const
    {
        return m_End;
    }

    /*!
        @brief        Returns the minimum value of the range of this iterator
    */
    tNumericBaseClass GetBeginOffset() const
    {
        return m_Begin;
    }

    /*!
        @brief        Returns distance from actual iter position to the maximum value
        @return       (tNumericBaseClass) distance 
    */
    tDistanceClass GetEndDistance() const
    {
        return (m_End - m_Offset);
    }

    /*!
        @brief        Returns distance from actual iter position to given offset
        @param        offset [in]  offset "behind" actual iter position
        @return       (tNumericBaseClass) distance 
    */
    tDistanceClass GetDistanceTo
        (tNumericBaseClass                offset) const
    {
        #ifdef SAPDB_SLOW
        if ( offset < m_Begin )
        {
            Kernel_VTrace() << "SAPDB_RangeIterator::GetDistanceTo: offset: " << offset
                            << ", m_Begin: " << m_Begin
                            << ", m_End: " << m_End;
            SAPDBERR_ASSERT_ARGUMENT(offset >= m_Begin);
        }
        if ( offset > m_End )
        {
            Kernel_VTrace() << "SAPDB_RangeIterator::GetDistanceTo: offset: " << offset
                            << ", m_Begin: " << m_Begin
                            << ", m_End: " << m_End;
            SAPDBERR_ASSERT_ARGUMENT(offset <= m_End);
        }
        #endif

        return ((offset >= m_Offset ) ? 
                 offset - m_Offset :
                  (m_End - m_Offset + 1) 
                 + (offset - m_Begin));
    }

    /*!
        @brief        Returns distance from given offset to actual iter position
        @param        offset [in]  offset "before" actual iter position
        @return       (tNumericBaseClass) distance 
    */
    tDistanceClass GetDistanceFrom
        (tNumericBaseClass                offset) const
    {
        SAPDBERR_ASSERT_ARGUMENT(offset >= m_Begin);
        SAPDBERR_ASSERT_ARGUMENT(offset <= m_End);

        return ((offset <= m_Offset) ? 
                 m_Offset - offset : (m_End - offset + 1) + (m_Offset - m_Begin));
    }

    /*!
        @brief        Returns distance from begin offset to end offset
        @return       (tNumericBaseClass) the number of integral values of the range of this iterator
    */
    tDistanceClass GetSize () const
    {
        return m_End - m_Begin + 1;
    }

private:

    tNumericBaseClass Later
        (SAPDB_UInt                     steps) const
    {
        SAPDBERR_ASSERT_ARGUMENT(m_End - m_Begin + 1 >= steps);

        return (m_Offset() > (m_End - steps)) ? 
                m_Begin + steps - (m_End - m_Offset + 1) : m_Offset + steps;
    }

    tNumericBaseClass Earlier
        (SAPDB_UInt                     steps) const
    {
        SAPDBERR_ASSERT_ARGUMENT( m_End - m_Begin + 1 >= steps );

        return (m_Offset() < m_Begin + steps) ? 
                m_End - steps + (m_Offset - m_Begin + 1) : m_Offset - steps;
    }

    tNumericBaseClass                m_Begin; // PTS 1125634 mb 2003-11-27 class changed
    tNumericBaseClass                m_Offset;
    tNumericBaseClass                m_End;   // PTS 1125634 mb 2003-11-27 class changed
};

#endif  // SAPDB_RangeIterator_hpp



