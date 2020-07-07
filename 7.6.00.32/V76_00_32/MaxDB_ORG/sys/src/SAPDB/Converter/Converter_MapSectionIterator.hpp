/*!
  @file     Converter_MapSectionIterator.hpp
  @author   Henrik
  @ingroup  Converter
  @brief    Iterator over one map Section 
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


#ifndef CONVERTER_MAPSECTIONITERATOR_HPP
#define CONVERTER_MAPSECTIONITERATOR_HPP

#include "Converter/Converter_MapPeriodIterator.hpp"

/*!
   @class   Converter_MapSectionIterator
   @brief   Iterator over one map Section 
 */
class Converter_MapSectionIterator
{

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction and destruction                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   alloc [in] allocator for map resources
     */
    Converter_MapSectionIterator( Converter_MapControl         &map,
                                  const SAPDB_UInt      section,
                                  const SAPDB_UInt      numOfSections,
                                  const IOMan_DeviceNo  volNo,
                                  const RTETask_ITask &task
                                  )
    :
    m_pMapControl(&map),
    m_volNo(volNo),
    m_section(section),
    m_endSection(section + numOfSections)
    {
        if (m_section != m_endSection)
        {
            Converter_MapSection section = m_pMapControl->GetSection(task, m_section);

            m_periodIter = m_pMapControl->PeriodBegin(m_section, m_volNo);
            m_periodIterEnd = m_pMapControl->PeriodEnd(m_section, m_volNo);
        }
    };
    //@}

    /* -----------------------------------------------------------------------*/
    /*! @name Iterator usage                                                  */
    /* -----------------------------------------------------------------------*/
    //@{

    /*!
       @brief   Returns the next pageNo in the current section
       @param   task    [in] task
       @param   pno     [out] page number 
       @param   blockNo [out] block number on volume of pno
    */

    void GetNextPageNo(const RTETask_ITask &task, Data_PageNo &pno, IOMan_BlockNo &blockNo )
    {
        {
            Converter_MapSection section = m_pMapControl->GetSection(task, m_section);
            m_periodIter.GetNextPageNo(pno, blockNo);
        }

        while ((m_periodIter == m_periodIterEnd) && (m_section != m_endSection))
        {
            m_section++;
            if (m_section != m_endSection)
            {
                Converter_MapSection section = m_pMapControl->GetSection(task, m_section);

                m_periodIter    = m_pMapControl->PeriodBegin(m_section, m_volNo);
                m_periodIterEnd = m_pMapControl->PeriodEnd(m_section, m_volNo);
                m_periodIter.GetNextPageNo(pno, blockNo);
            }
        }
    }

   SAPDB_Bool operator==(const Converter_MapSectionIterator &comp) const
   {
        return  ((m_section == m_endSection) && (m_section == comp.m_section)) || 
                  ((m_pMapControl == comp.m_pMapControl) &&
                   (m_section == comp.m_section) &&
                   (m_volNo == comp.m_volNo) &&
                   (m_periodIter == comp.m_periodIter));
   }

private:

    /// the map 
    Converter_MapControl *m_pMapControl;

    /// 
    SAPDB_UInt  m_section;
    SAPDB_UInt  m_endSection;

    /// 
    IOMan_DeviceNo  m_volNo;

    Converter_MapPeriodIterator m_periodIter;
    Converter_MapPeriodIterator m_periodIterEnd;
};

#endif // CONVERTER_MAP_HPP
