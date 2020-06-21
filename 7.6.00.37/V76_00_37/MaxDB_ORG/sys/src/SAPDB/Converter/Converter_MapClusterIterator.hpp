/*!
  @file     Converter_MapClusterIterator.hpp
  @author   Henrik
  @ingroup  Converter
  @brief    Iterator over one map cluster, returns only pages of a given volume, used to drop a volume
            constains Converter_LeafPageEntryIteratorDrop 
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


#ifndef CONVERTER_MAPCLUSTERITERATOR_HPP
#define CONVERTER_MAPCLUSTERITERATOR_HPP

#include "Converter/Converter_Page.hpp"
#include "Converter/Converter_Map.hpp"

/*!
   @class   Converter_MapClusterIterator
   @brief   Iterator over one map cluster, constains Converter_LeafPageEntryIteratorDrop 
 */
class Converter_MapClusterIterator
{

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction and destruction                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   map [in] 
       @param   position [in] start position of iterator
       @param   positionEnd [in] end position of iterator
       @param   volNo [in] volume number of pages in converter map
     */
    Converter_MapClusterIterator( Converter_Map         &map,
                                  const SAPDB_UInt      position,
                                  const SAPDB_UInt      positionEnd,
                                  const IOMan_DeviceNo  volNo)
    :
    m_pMap(&map),
    m_volNo(volNo),
    m_position(position),
    m_positionEnd(positionEnd)
    {
        // search first assigned page in cluster
        while ((m_position != m_positionEnd) && !m_pMap->GetEntry(m_position).PageIsAssigned())
        {
            m_position++;
        }
        if (m_position != m_positionEnd)
        {   // initialize subiterator
            m_leafPageIter    = m_pMap->GetEntry(m_position).GetPage().BeginDrop(m_volNo);
            m_leafPageIterEnd = m_pMap->GetEntry(m_position).GetPage().EndDrop(m_volNo);
        }
    }

    /*!
       @brief   default constructor
    */
    Converter_MapClusterIterator()
    :
    m_pMap(NULL)
    {
    }

    /*!
       @brief   returns next page number 
       @param   pno     [out] page number 
       @param   blockNo [out] block number on volume of pno
    */
    void GetNextPageNo(Data_PageNo &pno, IOMan_BlockNo &blockNo )
    {
        m_leafPageIter.GetNextPageNo(pno, blockNo);

        // no valid pno found -> subiterator is at end
        // increment position
        while ((m_leafPageIter == m_leafPageIterEnd) && (m_position != m_positionEnd))
        {
            m_position++;
            // find assigned page in cluster
            while ((m_position != m_positionEnd) && !m_pMap->GetEntry(m_position).PageIsAssigned())
            {
                m_position++;
            }
            if (m_position != m_positionEnd)
            {
                m_leafPageIter    = m_pMap->GetEntry(m_position).GetPage().BeginDrop(m_volNo);
                m_leafPageIterEnd = m_pMap->GetEntry(m_position).GetPage().EndDrop(m_volNo);

                m_leafPageIter.GetNextPageNo(pno, blockNo);
            }
        }
    }

    /*!
       @brief          compare operator
       @param          iter[in] other iterator
       @return         none
     */
   SAPDB_Bool operator==(const Converter_MapClusterIterator &comp) const
   {
        if ((m_position == m_positionEnd) && (m_position == comp.m_position))
        {
            return SAPDB_TRUE;
        }
        else
        {
            return (m_position == comp.m_position) &&
                  (m_pMap   == comp.m_pMap) &&
                  (m_leafPageIter == comp.m_leafPageIter);
        }
   }

    /*!
       @brief          assignment operator
       @param          iter[in] other iterator
       @return         none
     */
   void operator=(const Converter_MapClusterIterator &source) 
   {
       m_pMap            = source.m_pMap;
       m_position        = source.m_position;
       m_positionEnd     = source.m_positionEnd;
       m_volNo           = source.m_volNo;
       m_leafPageIter    = source.m_leafPageIter;
       m_leafPageIterEnd = source.m_leafPageIterEnd;
   }



private:

    /// the map 
    Converter_Map    *m_pMap;

    /// iterator position
    SAPDB_UInt        m_position;
    /// iterator end position
    SAPDB_UInt        m_positionEnd;

    /// volume number
    IOMan_DeviceNo    m_volNo;

    /// subiterator
    Converter_LeafPageEntryIteratorDrop m_leafPageIter;
    /// subiterator end
    Converter_LeafPageEntryIteratorDrop m_leafPageIterEnd;
};

#endif // CONVERTER_MAP_HPP
