/*!
  @file           Converter_PageCollector.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          This class is used to build io clusters for a given 
                  collection of data pages which belongs to the same file 

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



#ifndef CONVERTER_PAGECOLLECTOR_HPP
#define CONVERTER_PAGECOLLECTOR_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_PageNoList.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "IOManager/IOMan_DataPages.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class Data_BasePage;
class IOMan_ClusterAddress;
class RTETask_ITask;
class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  DECLARATION OF CLASSES, STRUCTURES, TYPES, UNIONS ...                    *
 *===========================================================================*/

/*!
   @class   Converter_PageCollector
   @brief   This class is used to build io clusters for a given collection of 
            data pages. All pages must belong to one file. The clusters were
            build according to the order the pages were given the the collector.
            see method DetermineCluster() 
 */
class Converter_PageCollector
{
public:

    /*!
        @brief  Constructor;  
        @param  allocator [in|out]   allocator
        @param  recMode   [in]       recovery mode of the pages which will be added later. 
                                     It is expected that all pages belong to the same file 
                                     and therefore have the same recover mode.
        @param  initialCapacity [in] estimated number of pages to be stored initialy
                                     Default value is 64. Values smaller than 64
                                     will be raised up to default value.  
    */
    Converter_PageCollector(
        SAPDBMem_IRawAllocator&         allocator,
        const Data_PageRecoveryMode&    recMode,
        const SAPDB_UInt                initialCapacity = 64 );

    /*!
        @brief  Initialization inclusive memory request so that no more 
                memory has to be allocated later. With each initialization
                the object will be reinitialized, i.e. old data will be lost
                but no new memory will be allocated!
        @return (bool) true if successfully
    */
    bool Initialize()
    { 
        m_ClusterCount = 0;
        this->Clear();
        if( m_ClusterItems.Reserve( m_InitialCapacity )){
            return true;
        }
        this->Delete();
        return false;
    }

    /// Destructor; returns all memory requested before
    ~Converter_PageCollector(){
        this->Delete();
    }

    /*!
        @brief  Returns the first logical id of a cluster. This does not
                means that a cluster exist!
        @return (SAPDB_UInt)
        
    */
    static SAPDB_UInt FirstClusterId(){
        return 1;
    }
    /*!
        @brief  Add a data page to the collector. The add order is decisive
                for the cluster building process.
        @param  pageNo [in] pageNo to be added
        @return (bool) false if add failed 
    */
    bool AppendPageNo( const Data_PageNo&  pageNo )
    {
        if( m_ClusterItems.GetSize() == m_ClusterItems.GetCapacity())
        {
            if( ! m_ClusterItems.Reserve( this->CalculateNewCapacity())){
                return false;
            }
        }
        return m_ClusterItems.InsertEnd( ClusterItem( pageNo ));
    }

    /*!
        @brief  Determination of the clusters which belongs to given data pageNo's.
                According to the order the data pages were given (AppendpageNo())
                clusters will be build. As long as the block address of a block
                belongs to the same volume as the predecessor and the blockNo is 
                exact one bigger than the predecessor both blocks (pages) belongs
                to the same cluster. Cluster numeration starts with FirstClusterId()
                and has no gaps. The largest clusterId could be requested by calling 
                GetClusterCount().
        @param  task [in|out] task
        @param  none
    */
    void DetermineCluster( RTETask_ITask&   task );

    /*!
        @brief  The given pageNoList and cluster address is filled according to
                the given clusterId. If the clusterId does not exist false is 
                returned. Otherwise the pageNoList contains a couple of data pages
                which belongs to the returned cluster address. The first logical
                data pageNo (index zero) belongs the the start address of the 
                cluster and so on.
        @param  clusterId   [in] identifies the cluster (starting with  FirstClusterId()
                                 to whom the cluster should belong 
        @param  pageNoList [out] used to add the pageNo's of the current cluster
                                 starting with index zero
        @param  cluster    [out] start block address and amount for the returned 
                                 data page collection
    */
    bool GetCluster(
        const SAPDB_UInt        clusterId,
        Data_PageNoList&        pageNoList,
        IOMan_ClusterAddress&   cluster );

    /*!
        @brief Returns the recovery mode all given pages belongs to
        @param (Data_PageRecoveryMode)
     */
    const Data_PageRecoveryMode& GetRecoveryMode() const{
        return m_RecMode;
    }

    /*!
        @brief Returns the configured value for clustered data page read
               operations (DATA_IO_BLOCK_COUNT). It is not possible to read
               more data pages with one io call than this value (given in
               pages) specifies.
    */
    SAPDB_UInt GetMaxClusterSize() const{
        return m_DataIOBlockCount;
    }

    /*!
        @brief Returns the number of clusters, which could be build after filling
               the pageCollector with pages and calling DetermineCluster(). At least
               one cluster should exist if one pageNo was added.
        @param (SAPDB_UInt)
    */
    SAPDB_UInt GetClusterCount() const{
        return m_ClusterCount;
    }
    
    /*!
        @brief Returns the number of pageNos, given to the pageCollector with 
               the AppendPageNo() method.
        @param (SAPDB_UInt)
    */
    SAPDB_UInt GetPageCount() const{
        return m_ClusterItems.GetSize();
    }
    
private:

    class ClusterItem
    {
    public:

        ClusterItem()
        :
        m_BlockCount( 0 ),
        m_ClusterID( 0 )
        {}

        ClusterItem( const Data_PageNo  pageNo )
        :
        m_PageNo( pageNo ),
        m_BlockCount( 0 ),
        m_ClusterID( 0 )
        {}

        void SetPageNo( const Data_PageNo  pageNo ){
            m_PageNo = pageNo;
        }

        void SetBlock( const IOMan_BlockAddress&  block ){
            m_Block = block;
        }

        void SetBlockCount( const SAPDB_UInt2  blockCount ){
            m_BlockCount = blockCount;
        }

        void SetClusterId( const SAPDB_UInt4 clusterId ){
            m_ClusterID = clusterId;
        }

        void IncrementBlockCount(){
            ++m_BlockCount;
        }
        
        Data_PageNo GetPageNo() const {
            return m_PageNo;
        }

        SAPDB_UInt4 GetClusterId() const {
            return m_ClusterID;
        }

        const IOMan_BlockAddress& GetBlock() const {
            return m_Block;
        }
        
        SAPDB_UInt2 GetBlockCount() const {
            return m_BlockCount;
        }

    private:

        // pageNo's of the data pages to be read
        Data_PageNo m_PageNo;

        // contains the block addresses for each given pageNo
        IOMan_BlockAddress  m_Block;

        // number of pages which belongs to the corresponding cluster
        SAPDB_UInt2  m_BlockCount;

        // defines the cluster to which the corresponding pageNo belongs; starting with 1
        SAPDB_UInt4  m_ClusterID;
    };
    
    typedef Container_Vector< ClusterItem > ClusterItemList;

private:

    void Clear(){
        m_ClusterItems.Clear();
    }

    void Delete(){
        m_ClusterItems.Delete();
    }
    
    SAPDB_UInt CalculateNewCapacity() const
    {
        const SAPDB_UInt currCapacity = m_ClusterItems.GetCapacity();
        const SAPDB_UInt expandValue = static_cast<SAPDB_UInt>( currCapacity * 0.2 );
        return SAPDB_MAX( m_DataIOBlockCount, expandValue ) + currCapacity;
    }
    
    bool SearchClusterHead(
        const SAPDB_UInt    clusterId,
        SAPDB_UInt&         blockIndex ) const
    {
        ClusterItemList::ConstIterator  iter    = m_ClusterItems.Begin();
        ClusterItemList::ConstIterator  endIter = m_ClusterItems.End();
        for( ; iter != endIter; iter++ )
        {
            if( clusterId == iter->GetClusterId())
            { // first occurance must be head of cluster
                blockIndex = m_ClusterItems.GetIndex( iter );
                return true;
            }
        }
        return false;
    }

    SAPDB_UInt AssignNewClusterHead(
        ClusterItem&        clusterItem,
        const SAPDB_UInt    clusterId,
        const SAPDB_UInt    clusterIndex )
    {
        clusterItem.SetClusterId( clusterId ); // set unique cluster id
        clusterItem.SetBlockCount( 1 );        // count number of blocks which belongs to this cluster
        ++m_ClusterCount;                      // count total number of clusters
        return clusterIndex;
    }

private:

    // Assignment operator is not supported
    Converter_PageCollector& operator=( const Converter_PageCollector& );

    // Copy constructor is not supported
    Converter_PageCollector( const Converter_PageCollector& );

private:

    SAPDBMem_IRawAllocator&  m_Allocator;

    // same recovery mode for all given pageNo's
    const Data_PageRecoveryMode  m_RecMode;

    // configured data page cluster size in pages
    const SAPDB_UInt  m_DataIOBlockCount;
    
    // initial capacity of the m_ClusterItems 
    const SAPDB_UInt m_InitialCapacity;
    
    // current number of cluster after DetermineCluster() was called
    SAPDB_UInt  m_ClusterCount;

    // list of items to be sorted based on block addresses
    ClusterItemList m_ClusterItems;
};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/


#endif  /* CONVERTER_PAGECOLLECTOR_HPP */

