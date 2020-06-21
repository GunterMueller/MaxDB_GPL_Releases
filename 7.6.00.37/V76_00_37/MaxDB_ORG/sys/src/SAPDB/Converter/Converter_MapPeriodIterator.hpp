/*!
  @file     Converter_MapPeriodIterator.hpp
  @author   Henrik
  @ingroup  Converter
  @brief    Iterator over one map period 
*/

/*
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

*/


#ifndef CONVERTER_MAPPERIODITERATOR_HPP
#define CONVERTER_MAPPERIODITERATOR_HPP

#include "Converter/Converter_MapClusterIterator.hpp"
#include "Converter/Converter_MapControl.hpp"

/*!
   @class   Converter_MapPeriodIterator
   @brief   iterator over the periods in a converter map
 */
class Converter_MapPeriodIterator
{

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction and destruction                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   alloc [in] allocator for map resources
     */
    Converter_MapPeriodIterator( Converter_MapControl  &map,
                                  const SAPDB_UInt      section,
                                  const SAPDB_UInt      period,
                                  const SAPDB_UInt      numOfPeriods,
                                  const IOMan_DeviceNo  volNo
                                  )
    :
    m_pMapControl(&map),
    m_section(section),
    m_volNo(volNo),
    m_period(period),
    m_endPeriod(period + numOfPeriods)
    {
        if (m_period != m_endPeriod)
        {
            m_clusterIter =  m_pMapControl->ClusterBegin(m_section, m_period, m_volNo);
            m_clusterIterEnd = m_pMapControl->ClusterEnd(m_section, m_period, m_volNo);
        }
    };

    Converter_MapPeriodIterator()
    :
    m_pMapControl(NULL)
    {
    }

    /* -----------------------------------------------------------------------*/
    /*! @name Iterator usage                                                  */
    /* -----------------------------------------------------------------------*/
    //@{

    /*!
       @brief   Returns the next pageNo in the current period
       @param   pno     [out] page number 
       @param   blockNo [out] block number on volume of pno
    */

    void GetNextPageNo(Data_PageNo &pno, IOMan_BlockNo &blockNo)
    {
        m_clusterIter.GetNextPageNo(pno, blockNo);

        while ((m_clusterIter == m_clusterIterEnd) && (m_period != m_endPeriod))
        {
            m_period++;
            if (m_period != m_endPeriod)
            {
                m_clusterIter    = m_pMapControl->ClusterBegin(m_section, m_period, m_volNo);
                m_clusterIterEnd = m_pMapControl->ClusterEnd(m_section, m_period, m_volNo);
                m_clusterIter.GetNextPageNo(pno, blockNo);
            }
        }
    }

   SAPDB_Bool operator==(const Converter_MapPeriodIterator &comp) const
   {
        if ((m_period == m_endPeriod) && (m_period == comp.m_period))
        {
            return SAPDB_TRUE;
        }
        else
        {
            return (m_period      == comp.m_period) &&
                   (m_pMapControl == comp.m_pMapControl) &&
                   (m_section     == comp.m_section);
        }
   }

   void operator=(const Converter_MapPeriodIterator &source) 
   {
       m_pMapControl    = source.m_pMapControl;
       m_section        = source.m_section;
       m_period         = source.m_period;
       m_endPeriod      = source.m_endPeriod;
       m_volNo          = source.m_volNo;
       m_clusterIter    = source.m_clusterIter;
       m_clusterIterEnd = source.m_clusterIterEnd;
   }




private:

    /// the map 

    Converter_MapControl *m_pMapControl;

    /// my section
    SAPDB_UInt m_section;

    /// 

    SAPDB_UInt  m_period;
    SAPDB_UInt  m_endPeriod;

    /// 

    IOMan_DeviceNo  m_volNo;

    Converter_MapClusterIterator m_clusterIter;
    Converter_MapClusterIterator m_clusterIterEnd;
};

#endif // CONVERTER_MAP_HPP
