/*****************************************************************************/
/*!

  @file Data_PrimTreeStatistic.hpp

  @author TorstenS

  @brief File statistics

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
/*****************************************************************************/


#ifndef DATA_PRIM_TREE_STATISTIC_HPP
#define DATA_PRIM_TREE_STATISTIC_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gbd00.h"

#include "KernelCommon/Kernel_Common.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PrimPage.hpp"
#include "KernelCommon/Kernel_TableStatistic.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

namespace {
class TempStatisticRecord;
}
class Data_ITreeAccess;

/*!
 *
 * @class Data_PrimTreeStatistic
 * @author TorstenS
 *
 * Implementation of table statistics and column statistics. This
 * class offers the possibility to get statistic information to the
 * given file. To reduce the runtime of the methods the functionality
 * is implemented two times.  On the one hand an exact calulation is
 * done, which will be expensive for huge files and on the other hand
 * an estimation is done, which will be cheaper but not exact.
 */
class Data_PrimTreeStatistic
{
public:
    /*!
     * @name Constructors and initialization
     */
    /*@{*/

    /*!
     * Constructs a handle for primary tree statistics.
     *
     * @param Current [in/out] identifier for the file to be handled
     */
    Data_PrimTreeStatistic( tbd_current_tree    &current )
    :
    m_Current( current ),
    m_Trans( *current.curr_trans ),
    m_TrError( current.curr_trans->trError_gg00 )
    {}

    /*@}*/

    /*!
     * @name Statistics determination
     */
    /*@{*/

    /*!
     *  This method is used to determine the exact number of distinct
     *  values in a given file. Furthermore the number of records and
     *  the number of leaves are counted. Note that this method reads
     *  all leaf nodes of the given file, which will take time if the
     *  given file is not small. The columns to be examined concerning
     *  the distinct values will be described by the parameter
     *  pColumnDescription.
     *
     * @param treeAccessor [in]         iterator to traverse tree
     * @param tempCurrent [in]          temporary file to hold the distinct values
     * @param columnCount [in]          number of column desciptions within 
     *                                   pColumnDescription
     * @param columnFirstPosition [in]  position of first column desciptions 
     *                             within pColumnDescription
     * @param columnLastPosition [in]   position of the last column desciptions
     *                                     within pColumnDescription
     * @param pColumnDescription [in]   column descriptions
     * @param numberOfLeaves [out]      number of counted leaf nodes 
     * @param numberOfRecords [out]     number of counted records
     * @param distinctValues [out]      contains the number of distinct values
     *                               for every given column.
     */
    void CalculateColumnStatistic(
        Data_ITreeAccess            &treeAccessor,
        tbd_current_tree            &tempCurrent,
        const SAPDB_Int2            columnCount,
        SAPDB_Int2                  columnFirstPosition,
        SAPDB_Int2                  columnLastPosition,
        const tgg00_StEntryAddr     pColumnDescription,
        Data_PageCount              &numberOfLeaves,
        Data_PageCount              &numberOfRecords,
        tgg00_ColumnDistinctValues  &distinctValues );

    /*!
     * This method is used to estimate the number of distinct values
     * in a given file. Furthermore the number of records and the
     * number of leaves are estimated. This method will read the leaf
     * pages step by step whereby the step width is given by the
     * parameter stepWidth. The columns to be examined concerning the
     * distinct values will be described by the parameter
     * pColumnDescription.
     *
     * @param treeAccessor [in]         iterator to traverse tree
     * @param tempCurrent         [in]  temporary file to hold the distinct values
     * @param columnCount         [in]  number of column desciptions within 
     *                               pColumnDescription
     * @param columnFirstPosition [in]  position of first column desciptions 
     *                               within pColumnDescription
     * @param columnLastPosition  [in]  position of the last column desciptions
     *                               within pColumnDescription
     * @param pColumnDescription  [in]  column descriptions
     * @param numberOfLeaves     [out]  number of counted leaf nodes 
     * @param numberOfRecords    [out]  number of counted records
     * @param distinctValues     [out]  contains the number of distinct values
     *                              for every given column.
     */
    void SampleColumnStatistic(
        Data_ITreeAccess            &treeAccessor,
        tbd_current_tree            &tempCurrent,
        const SAPDB_Int2            columnCount,
        SAPDB_Int2                  columnFirstPosition,
        SAPDB_Int2                  columnLastPosition,
        const tgg00_StEntryAddr     pColumnDescription,
        Data_PageCount              &numberOfLeaves,
        Data_PageCount              &numberOfRecords,
        tgg00_ColumnDistinctValues  &distinctValues );

    /*!
     * This method is used to estimate the number of distinct values
     * in a given file. Furthermore the number of records and the
     * number of leaves are estimated. This method will read as many
     * leaf nodes as the treeAccessor provides. Note that the used
     * estimation algorithmn from Thomas Koetter expects that no leaf
     * will be examined twice during the sampling. The columns to be
     * examined concerning the distinct values will be described by
     * the parameter pColumnDescription.
     *
     * @param treeAccessor [in]         iterator to traverse tree
     * @param tempCurrent [in]          temporary file to hold the distinct values
     * @param columnCount [in]          number of column desciptions within 
     *                              pColumnDescription
     * @param columnFirstPosition [in]  position of first column desciptions 
     *                               within pColumnDescription
     * @param columnLastPosition [in]   position of the last column desciptions
     *                              within pColumnDescription
     * @param pColumnDescription [in]   column descriptions
     * @param numberOfLeaves [out]      number of counted leaf nodes 
     * @param numberOfRecords [out]     number of counted records
     * @param distinctValues [out]      contains the number of distinct values
     *                              for every given column.
     */
    void SampleNewColumnStatistic(
        Data_ITreeAccess            &treeAccess,
        tbd_current_tree            &tempCurrent,
        const SAPDB_Int2            columnCount,
        SAPDB_Int2                  columnFirstPosition,
        SAPDB_Int2                  columnLastPosition,
        const tgg00_StEntryAddr     pColumnDescription,
        Data_PageCount              &numberOfLeaves,
        Data_PageCount              &numberOfRecords,
        tgg00_ColumnDistinctValues  &distinctValues );

    /*!
     * This method is used to calculate the exact number of leaves and
     * records within the given file. Note that this method reads all
     * leaf nodes of the given file, which will take time if the given
     * file is not small.
     *
     * @param numberOfLeaves [out]  number of counted leaf nodes 
     * @param numberOfRecords [out] number of counted records
     */
    void CalculateRecordsAndLeaves(
        Data_PageCount  &numberOfLeaves,
        Data_PageCount  &numberOfRecords );

    /*! 
        @brief  This method is used to determine the number of leaves which belongs 
                to the given tree and the number of clusters which are needed to 
                store the complete content. In the best case the numberOfClusters 
                should be nearly to the result of the formula numberOfLeaves DIV 
                configured cluster size. The number of cluster should be as little as
                possible. For determination of this values an iterator is used to
                run from the leftmost index node on level one to the rightmost one. 
       @param   numberOfLeaves   [out] number of counted leaves
       @param   numberOfClusters [out] number of clusters needed to store the counted leaves 
    */
      
    void CalculateCluster(
        Data_PageCount&     numberOfLeaves,
        Data_PageCount&     numberOfClusters );
        
    /*!
     * This method is used to estimate the number of leaves and
     * records within the given file. This method will read as many
     * leaf nodes as the treeAccessor provides. Note that the used
     * estimation algorithmn from Thomas Koetter expects that no leaf
     * will be examined twice during the sampling.
     *
     * @param treeAccessor [in]      iterator to traverse tree
     * @param numberOfLeaves [out]   number of counted leaf nodes 
     * @param numberOfRecords [out]  number of counted records
     */
    void SampleRecordsAndLeaves(
        Data_ITreeAccess        &treeAccessor,
        Data_PageCount          &numberOfLeaves,
        Data_PageCount          &numberOfRecords );

    /*!
     * This method is used to determine the table statistics for the
     * given file. Note that this method reads all nodes (leaf and non
     * leaf) of the given file, which will take time if the given file
     * is not small.
     *
     * @param treeAccessor [in]        iterator to traverse tree, this
     *                                 iterator should return both leaf
     *                                 and non-leaf nodes of the file
     * @param numberOfLongColumns [in] number of defined long columns
     * @param tableStatistic [in/out]  calculated table statistics
     */
    void CalculateTableStatistic(
        Data_ITreeAccess        &treeAccessor,
        const SAPDB_UInt        numberOfLongColumns,
        Kernel_TableStatistic   &tableStatistic );

    static SAPDB_Int4 EstimateDistinctValuesPerColumn(
        const SAPDB_Real8   estimatedNumberOfRecords,
        const SAPDB_Real8   sampledNumberOfRecords,
        const SAPDB_Real8   distinctInSample );

    /*@}*/

private:

    // Copy and assignment operator are not supported

    Data_PrimTreeStatistic& operator=( const Data_PrimTreeStatistic &pts );

    Data_PrimTreeStatistic( const Data_PrimTreeStatistic &pts );


    SAPDB_Bool ColumnStatisticForSinglePage(
        const SAPDB_Bool             bCountDuplicates,
        tbd_current_tree             &tempCurrent,
        TempStatisticRecord          &tempRecord,
        Data_PrimPage                &primPage,
        const SAPDB_Int2             columnCount,
        const SAPDB_Int2             columnFirstPosition,
        const SAPDB_Int2             columnLastPosition,
        const tgg00_StEntryAddr      pColumnDescription,
        tgg00_ColumnDistinctValues   &distinctValues );

    void TableStatisticForSingleLeafPage(
         Data_PrimPage          &primPage,
        Kernel_TableStatistic   &tableStatistic );

    void TableStatisticForSingleLeafPageWithLongColumns(
        const SAPDB_UInt        numberOfLongColumns,
         Data_PrimPage          &primPage,
        Kernel_TableStatistic   &tableStatistic );

    void TableStatisticForSingleNonLeafPage(
        Data_PrimPage           &primPage,
        Kernel_TableStatistic   &tableStatistic );

    void NewEstimateNumberOfDistinctValues(
        const SAPDB_Int8             estimatedNumberOfLeafPages,
        const SAPDB_Int8             estimatedNumberOfTableRecords,
        const SAPDB_Int8             sampleSize,
        const SAPDB_Int2             columnCount,
        tbd_current_tree             &tempCurrent,
        tgg00_ColumnDistinctValues   &distinctValues );

    void EstimateNumberOfDistinctValues(
        const SAPDB_Real8            estimatedNumberOfRecords,
        const SAPDB_Real8            sampledNumberOfRecords,
        const SAPDB_Int2             columnCount,
        tgg00_ColumnDistinctValues   &distinctValues );

    SAPDB_Bool CommandCanceled();

private:

    // Handle for the primary file to be considered
    tbd_current_tree    &m_Current;

    // Context of the current transaction
    tgg00_TransContext  &m_Trans;

    // Current error state
    tgg00_BasisError    &m_TrError;

};

#endif  /* DATA_PRIM_TREE_STATISTIC_HPP */
