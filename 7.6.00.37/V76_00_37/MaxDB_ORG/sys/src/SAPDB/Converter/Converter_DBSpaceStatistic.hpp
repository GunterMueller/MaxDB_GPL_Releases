/*!
  @file           Converter_DBSpaceStatistic.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          Encapsulation of tgg00_DbSpaceInfo
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


#ifndef CONVERTER_DBSPACE_STATISTIC_HPP
#define CONVERTER_DBSPACE_STATISTIC_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "ggg00.h"   // PASCAL: GG_basic_constants_and_types

#include "SAPDBCommon/SAPDB_Types.hpp"

/*!
  @class    Converter_DBSpaceStatistic
  @brief    Is used to capsulate the old pascal tgg00_DbSpaceInfo structure
            used to get data base statistic values for the old pascal AK interface.
 */

class Converter_DBSpaceStatistic
{
public:

    /// constructor; pointer to data base space info is given
    Converter_DBSpaceStatistic( tgg00_DbSpaceInfo* pDBSpaceInfo )
    :m_pDBSpaceInfo( pDBSpaceInfo )
    {}

    /*!
       @brief   Calulactes the percent values for temp and perm pages. Both
                values have to be assigned before and the total page count too!
       @return  none
     */

    void CalculatePercentValues()
    {
        m_pDBSpaceInfo->db_perm_percent =
            static_cast< SAPDB_Int2 >((GetAllPermPageCount() * 100.0) / GetTotalPageCount());

        m_pDBSpaceInfo->db_temp_percent =
            static_cast< SAPDB_Int2 >((GetTempPageCount() * 100.0) / GetTotalPageCount());
    }

    /*!
       @brief   Assign total number of addressable data pages
       @param   value [in] page count
       @return  none
     */

    void SetTotalPageCount( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_total_pages = value;
    }

    /*!
       @brief   Assign total number of permanent addressable data pages
       @param   value [in] page count
       @return  none
     */

    void SetDataPermPageCount( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_perm_pages = value;
    }

    /*!
       @brief   Assign used number of permanent data and converter pages
       @param   value [in] page count
       @return  none
     */

    void SetAllPermPageCount( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_real_perm_used = value;
    }

    /*!
       @brief  Assign used number of temporary data pages
       @param   value [in] page count
       @return  none
     */

    void SetTempPageCount( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_temp_pages = value;
    }

    /*!
       @brief  Assign used number of permanent, temporary data and convter pages
       @param   value [in] page count
       @return  none
     */

    void SetAllUsedPageCount( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_used_pages = value;
    }

    /*!
       @brief   Assign number of permanent pages changed since last successful
                data base backup
       @param   value [in] page count
       @return  none
     */

    void SetModifiedPageCount( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_updated_pages = value;
    }

    /*!
       @brief   Assign maximum used perm pages. At each savepoint the peak is determined.
       @param   value [in] page count
       @return  none
     */

    void SetMaxPermPagePeak( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_max_perm_used = value;
    }

    /*!
       @brief   Assign total number of used blocks.
       @param   value [in] block count
       @return  none
     */

    void SetUsedFBMBlocks( const SAPDB_Int value ){
        m_pDBSpaceInfo->db_used_blocks = value;
    }

private:

    /// Get all used perm page count
    SAPDB_Int GetAllPermPageCount() const{
        return m_pDBSpaceInfo->db_real_perm_used;
    }

    /// Get total addressable data page count
    SAPDB_Int GetTotalPageCount() const{
        return m_pDBSpaceInfo->db_total_pages;
    }

    /// Get all used temp page count
    SAPDB_Int GetTempPageCount() const{
        return m_pDBSpaceInfo->db_temp_pages;
    }

private:

    tgg00_DbSpaceInfo *m_pDBSpaceInfo;
};

#endif  /* CONVERTER_DBSPACE_STATISTIC_HPP */
