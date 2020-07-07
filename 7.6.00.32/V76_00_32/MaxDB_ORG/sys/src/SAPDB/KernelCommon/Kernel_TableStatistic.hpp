/*!
  @file           Kernel_TableStatistic.hpp
  @author         TorstenS
  @ingroup        KernelCommon
  @brief          This class is used to encapsulate statistic informations
                  needed for table statistic

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#ifndef KERNEL_TABLE_STATISTIC_HPP
#define KERNEL_TABLE_STATISTIC_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"

#include "KernelCommon/Kernel_Common.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @class   This class is used to encapsulate statistic informations
            needed for table statistic
*/

class Kernel_TableStatistic
{
public:
    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!
        @brief  Constructs a statistic handle to encapsulate the internal
                tgg00_TableStatInfo for primary table statistics requests.
        @param  tableStatistic [in|out] table statistic structure
    */

    Kernel_TableStatistic( tgg00_TableStatInfo& tableStatistic )
    :
    m_TableStatistic( tableStatistic ),
    m_NumberOfLeafNodes( 0 ),
    m_NumberOfIndexNodes( 0 ),
    m_ClusterCount( 0 ),
    m_TotalLeafCovering( 0 ),
    m_TotalIndexCovering( 0 ),
    m_RootCovering( 0 ),
    m_MinIndexCovering( SAPDB_MAX_UINT4 ),
    m_MaxIndexCovering( 0 ),
    m_RootLevel( LEAF_LEVEL_BD00 ),
    m_MinLeafCovering( SAPDB_MAX_UINT4 ),
    m_MaxLeafCovering( 0 ),
    m_RecordCount( 0 ),
    m_MinRecordCount( SAPDB_MAX_UINT4 ),
    m_MaxRecordCount( 0 ),
    m_TotalRecordLength( 0 ),
    m_MinRecordLength( SAPDB_MAX_UINT4 ),
    m_MaxRecordLength( 0 ),
    m_TotalKeyLength( 0 ),
    m_MinKeyLength( SAPDB_MAX_UINT4 ),
    m_MaxKeyLength( 0 ),
    m_NumberOfLongColumns( 0 ),
    m_MinLongColumnLength( SAPDB_MAX_UINT4 ),
    m_MaxLongColumnLength( 0 ),
    m_TotalLongColumnLength( 0 ),
    m_MinLongColumnPages( SAPDB_MAX_UINT4 ),
    m_MaxLongColumnPages( 0 ),
    m_TotalLongColumnPages( 0 ),
    m_NumberOfSeparators( 0 ),
    m_MinSeparatorLength( SAPDB_MAX_UINT4 ),
    m_MaxSeparatorLength( 0 ),
    m_TotalSeparatorLength( 0 )
    {}

    /* -----------------------------------------------------------------------*/
    /*! endchapter: constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Manipulation                                                 */
    /* -----------------------------------------------------------------------*/

    /*!
        @brief   This method is used to transfer the determined statistic
                 values into the given structure tgg00_TableStatInfo so that
                 this informations coud be used by the sql manager pascal code.
    */
    void CalculateResult()
    {
        SAPDBERR_ASSERT_STATE( 0 < m_NumberOfLeafNodes  );

        if( 0 >= m_NumberOfLeafNodes )
            return;

        m_TableStatistic.TabStatNodes_gg00      = m_NumberOfLeafNodes + m_NumberOfIndexNodes;
        m_TableStatistic.TabStatIndexNodes_gg00 = m_NumberOfIndexNodes;
        m_TableStatistic.TabStatLeafnodes_gg00  = m_NumberOfLeafNodes;
        m_TableStatistic.TabStatRootLevel_gg00  = m_RootLevel;

        if( LEAF_LEVEL_BD00 == m_RootLevel )
        {
            m_TableStatistic.TabStatTreeCov_gg00     =
                static_cast< SAPDB_Int4>( m_TotalLeafCovering / m_NumberOfLeafNodes );
            m_TableStatistic.TabStatIndexCov_gg00    = 0;
            m_TableStatistic.TabStatMinIndexCov_gg00 = 0;
            m_TableStatistic.TabStatMaxIndexCov_gg00 = 0;
        }
        else
        {
            m_TableStatistic.TabStatTreeCov_gg00  =
                static_cast< SAPDB_Int4>(( m_TotalLeafCovering + m_TotalIndexCovering )/
                                         (m_NumberOfLeafNodes + m_NumberOfIndexNodes ));

            m_TableStatistic.TabStatIndexCov_gg00 =
                static_cast< SAPDB_Int4>( m_TotalIndexCovering / m_NumberOfIndexNodes );

            m_TableStatistic.TabStatMinIndexCov_gg00 = m_MinIndexCovering;
            m_TableStatistic.TabStatMaxIndexCov_gg00 = m_MaxIndexCovering;

        }

        if( 0 < m_NumberOfSeparators )
        {
            m_TableStatistic.TabStatAveSepLength_gg00 =
                static_cast< SAPDB_Int2>( m_TotalSeparatorLength / m_NumberOfSeparators );
            m_TableStatistic.TabStatMinSepLength_gg00 = m_MinSeparatorLength;
            m_TableStatistic.TabStatMaxSepLength_gg00 = m_MaxSeparatorLength;
        }
        else
        {
            m_TableStatistic.TabStatAveSepLength_gg00 = 0;
            m_TableStatistic.TabStatMinSepLength_gg00 = 0;
            m_TableStatistic.TabStatMaxSepLength_gg00 = 0;
        }

        m_TableStatistic.TabStatLeafCov_gg00 =
            static_cast< SAPDB_Int4>( m_TotalLeafCovering / m_NumberOfLeafNodes );
        m_TableStatistic.TabStatRootCov_gg00 = static_cast< SAPDB_Int4>( m_RootCovering );
        m_TableStatistic.TabStatRecords_gg00 = static_cast< SAPDB_Int4>( m_RecordCount );

        if( 0 < m_RecordCount )
        {
            m_TableStatistic.TabStatMinLeafCov_gg00 = m_MinLeafCovering;
            m_TableStatistic.TabStatMaxLeafCov_gg00 = m_MaxLeafCovering;

            m_TableStatistic.TabStatMaxRecsPerPage_gg00 = m_MaxRecordCount;
            m_TableStatistic.TabStatMinRecsPerPage_gg00 = m_MinRecordCount;
            m_TableStatistic.TabStatRecsPerPage_gg00    =
                static_cast< SAPDB_Int4>( m_RecordCount / m_NumberOfLeafNodes );

            m_TableStatistic.TabStatAveRecLength_gg00   =
                static_cast<SAPDB_Int4>( m_TotalRecordLength / m_RecordCount );

            m_TableStatistic.TabStatMinRecLength_gg00   = m_MinRecordLength;
            m_TableStatistic.TabStatMaxRecLength_gg00   = m_MaxRecordLength;

            m_TableStatistic.TabStatAveKeyLength_gg00 =
                static_cast< SAPDB_Int2>( m_TotalKeyLength / m_RecordCount );

            m_TableStatistic.TabStatMinKeyLength_gg00 = m_MinKeyLength;
            m_TableStatistic.TabStatMaxKeyLength_gg00 = m_MaxKeyLength;
        }
        else
        {
            m_TableStatistic.TabStatMinLeafCov_gg00 = 0;
            m_TableStatistic.TabStatMaxLeafCov_gg00 = 0;

            m_TableStatistic.TabStatMaxRecsPerPage_gg00 = 0;
            m_TableStatistic.TabStatMinRecsPerPage_gg00 = 0;
            m_TableStatistic.TabStatRecsPerPage_gg00    = 0;

            m_TableStatistic.TabStatAveRecLength_gg00   = 0;
            m_TableStatistic.TabStatMinRecLength_gg00   = 0;
            m_TableStatistic.TabStatMaxRecLength_gg00   = 0;

            m_TableStatistic.TabStatAveKeyLength_gg00 = 0;
            m_TableStatistic.TabStatMinKeyLength_gg00 = 0;
            m_TableStatistic.TabStatMaxKeyLength_gg00 = 0;
        }

        if( 0 < m_NumberOfLongColumns )
        {
            m_TableStatistic.TabStatDefinedStringCols_gg00 = m_NumberOfLongColumns;

            m_TableStatistic.TabStatAveLenStringCol_gg00   =
                static_cast<SAPDB_Int4>( m_TotalLongColumnLength / m_NumberOfLongColumns );
            m_TableStatistic.TabStatMinLenStringCol_gg00   = m_MinLongColumnLength;
            m_TableStatistic.TabStatMaxLenStringCol_gg00   = m_MaxLongColumnLength;

            m_TableStatistic.TabStatAveStringColPages_gg00   =
                static_cast< SAPDB_Int4>( m_TotalLongColumnPages / m_NumberOfLongColumns );
            m_TableStatistic.TabStatMinStringColPages_gg00   = m_MinLongColumnPages;
            m_TableStatistic.TabStatMaxStringColPages_gg00   = m_MaxLongColumnPages;
        }
        else
        {
            m_TableStatistic.TabStatDefinedStringCols_gg00 = 0;

            m_TableStatistic.TabStatAveLenStringCol_gg00   = 0;
            m_TableStatistic.TabStatMinLenStringCol_gg00   = 0;
            m_TableStatistic.TabStatMaxLenStringCol_gg00   = 0;

            m_TableStatistic.TabStatAveStringColPages_gg00   = 0;
            m_TableStatistic.TabStatMinStringColPages_gg00   = 0;
            m_TableStatistic.TabStatMaxStringColPages_gg00   = 0;
        }
        m_TableStatistic.TabStatAllStringColPages_gg00   =          // we have an empty short columnfile
            static_cast< SAPDB_Int4>( m_TotalLongColumnPages );
        
        m_TableStatistic.TabStatClusterCount_gg00 = m_ClusterCount; 
    }

    /*!
        @brief  This method is used to transfer the determined statistic
                values of the short column file into the base file statistic.
                Is is assumed that the short column file statistic are
                executed before the table statistics code.
        @param  shortColStatistic [in|out] short statistic structure
    */
    void IntegrateShortColumnStatistic( const Kernel_TableStatistic  &shortColStat )
    {
        SAPDBERR_ASSERT_STATE( 0 < shortColStat.m_NumberOfLeafNodes  );

        const SAPDB_UInt metaDataLength = cgg_rec_key_offset + sizeof( tgg00_Surrogate );

        m_TotalLongColumnPages = shortColStat.m_NumberOfLeafNodes + shortColStat.m_NumberOfIndexNodes;;
        m_NumberOfLongColumns  = shortColStat.m_RecordCount;

        if( 0 < shortColStat.m_RecordCount )
        {
            m_MinLongColumnPages   = 1;
            m_MaxLongColumnPages   = 1;

            m_MinLongColumnLength   = shortColStat.m_MinRecordLength -  metaDataLength;
            m_MaxLongColumnLength   = shortColStat.m_MaxRecordLength - metaDataLength;
            m_TotalLongColumnLength = shortColStat.m_TotalRecordLength - ( shortColStat.m_RecordCount * metaDataLength );
        }
    }

    /*!
        @brief  Set covering and level of the root page and execute AddIndexCovering.
        @param  covering [in] number of used bytes
        @param  rootLevel [in] level of root page
    */
    void AddRootCovering(
        const Data_RecordLength covering,
        const Data_Level        rootLevel )
    {
        m_RootCovering = (( covering * 100 ) / FULLCOVERING_BD00 );
        m_RootLevel    = rootLevel;
        AddIndexCovering( covering );
    }

    /*!
        @brief  Accumulate the non leaf covering in percent and adjust the 
                values for minimum and maximum covering. Finaly the number 
                of non leaf nodes is incremented.
        @param  covering [in] number of used bytes
   */
    void AddIndexCovering( const Data_RecordLength   covering )
    {
        const SAPDB_UInt percentCovering = (( covering * 100 ) / FULLCOVERING_BD00 );

        m_MinIndexCovering = SAPDB_MIN( m_MinIndexCovering, percentCovering );
        m_MaxIndexCovering = SAPDB_MAX( m_MaxIndexCovering, percentCovering );
        m_TotalIndexCovering += percentCovering;
        m_NumberOfIndexNodes++;
    }

    /*!
        @brief  Accumulate the leaf covering in percent and adjust the 
                values for minimum and maximum covering. Finaly the number 
                of leaf nodes is incremented.
        @param  covering [in] number of used bytes
    */
    void AddLeafCovering( const Data_RecordLength   covering )
    {
        const SAPDB_UInt percentCovering = (( covering * 100 ) / FULLCOVERING_BD00 );

        m_MinLeafCovering = SAPDB_MIN( m_MinLeafCovering, percentCovering );
        m_MaxLeafCovering = SAPDB_MAX( m_MaxLeafCovering, percentCovering );
        m_TotalLeafCovering += percentCovering;
        m_NumberOfLeafNodes++;
    }

    /*!
        @brief   Accumulate the record count and adjust the values for
                 minimum and maximum records on a leaf page.
        @param   recordCount [in] number of records
    */
    void AddRecordCount( const Data_RecordCount   recordCount )
    {
        m_MinRecordCount = SAPDB_MIN( m_MinRecordCount, recordCount );
        m_MaxRecordCount = SAPDB_MAX( m_MaxRecordCount, recordCount );
        m_RecordCount += recordCount;
    }

    /*!
        @brief  Accumulate the length of all records and keys and adjust 
                the values for the minimum and maximum limit of record and 
                key length.
        @param  recordLength [in] length of the record
                keyLength    [in] length of the key
    */
    void AddRecord(
        const Data_RecordLength   recordLength,
        const Data_RecordLength   keyLength )
    {
        m_MinRecordLength = SAPDB_MIN( m_MinRecordLength, recordLength );
        m_MaxRecordLength = SAPDB_MAX( m_MaxRecordLength, recordLength );
        m_TotalRecordLength += recordLength;

        m_MinKeyLength = SAPDB_MIN( m_MinKeyLength, keyLength );
        m_MaxKeyLength = SAPDB_MAX( m_MaxKeyLength, keyLength );
        m_TotalKeyLength += keyLength;
    }

    /*!
        @brief   Accumulate the length of the long column given in bytes and
                 pages and adjust the values for the minimum and maximum limit 
                 of column length. Finaly the number of long columns will be
                 incremented.
        @param   longColumnLength [in] length of long column in bytes
                 longColumnPages  [in] length of long column in bytes
   */
    void AddLongColumn(
        const Data_RecordLength longColumnLength,
        const Data_PageCount    longColumnPages )
    {
        m_NumberOfLongColumns++;

        m_MinLongColumnLength = SAPDB_MIN( m_MinLongColumnLength, longColumnLength );
        m_MaxLongColumnLength = SAPDB_MAX( m_MaxLongColumnLength, longColumnLength );
        m_TotalLongColumnLength += longColumnLength;

        m_MinLongColumnPages = SAPDB_MIN( m_MinLongColumnPages, longColumnPages );
        m_MaxLongColumnPages = SAPDB_MAX( m_MaxLongColumnPages, longColumnPages );
        m_TotalLongColumnPages += longColumnPages;
    }

    /*!
        @brief   Accumulate the length of all separators and adjust the values 
                 for the minimum and maximum limit of separator length.
        @param   separatorLength [in] length of the separator
    */
    void AddSeparator( const Data_RecordLength   separatorLength )
    {
        if( 0 < separatorLength )
        {
            m_MinSeparatorLength = SAPDB_MIN( m_MinSeparatorLength, separatorLength );
            m_MaxSeparatorLength = SAPDB_MAX( m_MaxSeparatorLength, separatorLength );
            m_TotalSeparatorLength += separatorLength;
            m_NumberOfSeparators++;
        }
    }
    /*!
        @brief Assign the number of detected cluster
        @param clusterCount [in] number of cluster 
    */
    void SetClusterCount( const SAPDB_UInt clusterCount ){
        m_ClusterCount = clusterCount;
    }
    
    /* -----------------------------------------------------------------------*/
    /*! endchapter:  Manipulation                                             */
    /* -----------------------------------------------------------------------*/

private:

    // Reference to the given  internal structure tgg00_TableStatInfo
    // containing the statistic information for table statistic
    tgg00_TableStatInfo&    m_TableStatistic;

    // Number of leaf nodes
    Data_PageCount  m_NumberOfLeafNodes;

    // Number of non leaf nodes
    Data_PageCount  m_NumberOfIndexNodes;
    
    // Number of cluster needed to store the leaves
    Data_PageCount m_ClusterCount;

    // The sum of all percental coverings of all leaf pages
    SAPDB_ULong m_TotalLeafCovering;

    // The sum of all percental coverings of all non leaf pages
    SAPDB_ULong m_TotalIndexCovering;

    // Number of levels of the tree
    Data_Level  m_RootLevel;

    // The covering of the root page, if the tree is at least of level 1
    SAPDB_ULong m_RootCovering;

    // The lowest covering on one index page found in the given file
    Data_RecordLength   m_MinIndexCovering;

    // The biggest covering on one index page found in the given file
    Data_RecordLength   m_MaxIndexCovering;

    //The lowest covering on one leaf page found in the given file
    Data_RecordLength   m_MinLeafCovering;

    //The biggest covering on one leaf page found in the given file
    Data_RecordLength   m_MaxLeafCovering;

    //Total number of records within the complete file
    Data_RecordCount    m_RecordCount;

    //The lowest record count on one page found in the given file
    Data_RecordCount    m_MinRecordCount;

    //The hightest record count on one page found in the given file
    Data_RecordCount    m_MaxRecordCount;

    //Is the sum of all record lengths found in the given file.
    SAPDB_Real8 m_TotalRecordLength;

    //The smallest record on one page found in the given file
    Data_RecordLength   m_MinRecordLength;

    //The biggest record on one page found in the given file
    Data_RecordLength   m_MaxRecordLength;

    //Is the sum of all key lengths found in the given file.
    SAPDB_Real8 m_TotalKeyLength;

    //The smallest key on one page found in the given file
    Data_RecordLength   m_MinKeyLength;

    //The biggest key on one page found in the given file
    Data_RecordLength   m_MaxKeyLength;

    //Total number of defined long columns within given file
    Data_RecordCount m_NumberOfLongColumns;

    //The smallest long column in the given file
    Data_RecordLength   m_MinLongColumnLength;

    //The biggest long column in the given file
    Data_RecordLength   m_MaxLongColumnLength;

    //Is the sum of all long column lengths found in the given file.
    SAPDB_Real8 m_TotalLongColumnLength;

    //The number of pages of the smallest long column in the given file
    Data_RecordLength   m_MinLongColumnPages;

    // The number of pages of the biggest long column in the given file
    Data_RecordLength   m_MaxLongColumnPages;

    //Is the sum of all long column pages found in the given file.
    SAPDB_ULong m_TotalLongColumnPages;

    //Is the number of all separators found in the given file.
    SAPDB_ULong m_NumberOfSeparators;

    //The smallest separator on one page found in the given file
    Data_RecordLength   m_MinSeparatorLength;

    //The biggest separator on one page found in the given file
    Data_RecordLength   m_MaxSeparatorLength;

    //Is the sum of all separator lengths found in the given file.
    SAPDB_Real8 m_TotalSeparatorLength;
};

#endif  /* KERNEL_TABLE_STATISTIC_HPP */

