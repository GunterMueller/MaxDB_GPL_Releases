/*!
  @file     Converter_Map.hpp
  @author   TorstenS
  @author   TillL
  @ingroup  Converter
  @brief    Map for pageNos
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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


#ifndef CONVERTER_MAP_HPP
#define CONVERTER_MAP_HPP


#include "Container/Container_Vector.hpp"
#include "Converter/Converter_BitMapPage.hpp"
#include "Converter/Converter_Chain.hpp"
#include "Converter/Converter_MapEntry.hpp"
#include "Converter/Converter_PageFrameAllocator.hpp"
#include "Converter/Converter_PermMapSection.hpp"

class Converter_MapSection;
class Converter_MapSectionData;
class Converter_PermMapSectionData;
class Data_PageRecoveryMode;
class Kernel_Dump;
class SAPDBMem_IRawAllocator;

///  buffer which contains converter map entries (== the map column)
typedef Container_Vector<Converter_MapEntry> Converter_MapVector;

/*!
   @class   Converter_Map
   @brief   Map for pageNo's
 */
class Converter_Map
{
    friend class Converter_MapSectionData;
    friend class Converter_PermMapSectionData;
    friend class Converter_PermMapSection::ChangedEntryIterator;
    friend class Converter_MapClusterIterator;

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Type definitions                                                */
    /* -----------------------------------------------------------------------*/

    /// an object containing info about whether a leaf page is changed
    typedef Converter_ChainNode<Converter_MapEntryNo>       ChangedInfo;

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction and destruction                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   alloc [in] allocator for map resources
     */
    Converter_Map( SAPDBMem_IRawAllocator   &allocator )
    :
    m_EntryVector                ( allocator ),
    m_ChangedInfoVector          ( allocator ),
    m_IsPageUsedAfterRestorePages( allocator ),
    m_EntryClusterSize           ( 0 ),
    m_SectionCount               ( 0 ),
    m_PeriodCount                ( 0 ),
    m_LeafPageSize               ( 0 ),
    m_HighestPossiblePageNo      ( 0 )
    {}

    /*!
       @brief   Initializes map.
       @param   entryClusterSize [in] number of map entries within a cluster
                                      of entries (inner period)
       @param   sectionCount     [in] number of sections. Each section is
                                      protected by it's own latch
       @param   leafPageSize     [in] total size of a converter page in byte
       @return  (SAPDB_Bool) true if successful
     */

    void Initialize(
        const Converter_MapEntryCount   entryClusterSize,
        const SAPDB_UInt                sectionCount,
        const SAPDB_UInt                leafPageSize );

    /*!
       @brief   Erases the map and frees all pages
       @return  none
     */

    void Shutdown();

    /*!
       @brief   Insert a leaf page into the map at restart
       @param   section    [in|out] corresponding map section
       @param   page           [in] page frame holding the leaf page
       @param   basePageNo     [in] base page number of given page
       @param   usedEntryCount [in] number of used entries in the page        
       @param   freeEntryCount [in] number of free entries on the page
       @return  none
     */

    void IntegratePage(
        Converter_PermMapSection    &section,
        Converter_LeafPage          &page,
        const Data_PageNo           basePageNo,
        const SAPDB_UInt            usedEntryCount,
        const SAPDB_UInt            freeEntryCount );

    /*!
        @brief  Expands the whole map for a given number of data pageNo's.
                The number of pageNo's is used to calculate the number of
                needed periods, which has to be allocated. This method may
                require reallocation of the whole map! The number of
                added periods is returned. The period count is adjusted.
        @param  maxPageNo       [in] requested map capacity
        @param  addPeriodCount [out] number of added periods
        @return (SAPDB_Bool) true if successful
    */

    SAPDB_Bool Expand(
        const Data_PageNo maxPageNo,
        SAPDB_Int         &addPeriodCount ); // PTS 1126701 TS 2003-12-19

    /*!
        @brief  Expands one section by appending unused entries, after
                the whole map was expanded.
        @param  section       [in|out] corresponding section object
        @param  addPeriodCount    [in] number of added periods
        @return none
    */

    void ExpandOneSection(
        Converter_MapSection    &section,
        const SAPDB_Int         addPeriodCount ); // PTS 1126701 TS 2003-12-19

    /*!
       @brief   Deletes an unused page and the memory is released.
       @param   entryNo [in] number of entry with unused page
       @return  none
     */

    void DeletePage( const Converter_MapEntryNo entryNo );

    void DeletePage( Converter_MapEntry &entry );

    /* -----------------------------------------------------------------------*/
    /*! @name Map structure and map operations                                */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Calculates section number of a given data page number
       @param   pageNo [in] specified pageno
       @return  (SAPDB_UInt) section number
     */
    SAPDB_UInt CalcSectionNo( const Data_PageNo   pageNo ) const
    {
        const SAPDB_UInt           periodSize = m_EntryClusterSize * m_SectionCount;
        const Converter_MapEntryNo entryNo    = pageNo / GetLeafPageCapacity();
        return (entryNo % periodSize) / m_EntryClusterSize;
    }

    /*!
       @brief   Prepares specified page for write i/o, i.e. the page
                is marked as i/o pending.
       @param   section [in|out] corresponding section object
       @param   entry       [in] map entry
       @return  none
    */

    void PreparePageForIO(
        Converter_PermMapSection    &section,
        Converter_MapEntry          &entry );

    /*!
       @brief   Finishes page i/o, i.e. the pending i/o flag wil be reseted.
       @param   section [in/out] corresponding section object
       @param   entry       [in] map entry
       @return  none
     */

    void ResetPageStateAfterIO(
        Converter_PermMapSection    &section,
        Converter_MapEntry          &entry );

    /*!
       @brief   Mark given converter page as changed 
       @param   section [in|out] corresponding map section
       @param   page    [in]     leaf page
       @return  none
     */

    void MarkPageAsChanged(
        Converter_PermMapSection&   section,
        Converter_LeafPage&         leafPage )
    {
        SAPDBERR_ASSERT_ARGUMENT( leafPage.IsAssigned());

        const Data_PageNo           basePageNo = leafPage.ReadBasePageNo();
        const Converter_MapEntryNo  entryNo    = CalcEntryNo( basePageNo );

        section.MarkPageAsChanged( entryNo );
    }

    /* -----------------------------------------------------------------------*/
    /*! @name Pageno management                                               */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Retrieves a free pageno. If necessary, a new leaf page is
                allocated by 'framealloc'
       @param   section [in/out] corresponding map section
       @param   pageNo     [out] unused pageno
       @return  (Converter_ReturnCode)
                Converter_Ok,
                Converter_FrameAllocationFailed
     */

    Converter_ReturnCode RequestNewPageNo(
        Converter_MapSection    &section,
        Data_PageNo             &pageNo );

    /*!
       @brief   Checks whether specified pageno is still free and returns it
       @param   section [in/out] corresponding map section
       @param   pageNo      [in] specified data pageno
       @return  (RequestSpecifiedNewPageNoRc)
                rsnpRcOkay,
                rsnpRcPendingIo,
                rsnpRcPageNoInUse,
                rsnpRcFrameAllocationFailed

       - The given pageno is reserved in the specified map section and returned.
       - Is it assumed implicitely that the corresponding page is recoverable (== permanent).
       - If the page is currently in i/o state, the specified page no
         cannot be reserved, and the function returns false.
     */

    enum RequestSpecifiedNewPageNoRc{
        rsnpRcOkay                  = Converter_Ok,
        rsnpRcPendingIo             = Converter_ConverterPageIOPending,
        rsnpRcPageNoInUse           = Converter_PageNoAlreadyInUse,
        rsnpRcFrameAllocationFailed = Converter_FrameAllocationFailed
    };
    RequestSpecifiedNewPageNoRc RequestSpecifiedNewPageNo(
        Converter_PermMapSection    &section,
        const Data_PageNo           pageNo );

    /*!
       @brief   Puts back the given temporary pageno
       @param   section [in/out] corresponding map section
       @param   pageNo      [in] specified data pageno
       @param   bladdr     [out] current i/o address of pageno
       @return  (Converter_ReturnCode)
                Converter_Ok,
                Converter_PageNoAlreadyFree
     */
    Converter_ReturnCode FreeTempPageNo(
        Converter_MapSection    &section,
        const Data_PageNo       pageNo,
        IOMan_BlockAddress      &bladdr );

    /*!
       @brief   Puts back the given permanent pageno
       @param   section          [in|out] corresponding map section
       @param   snapShotVersion      [in] converter version of the snapshot
       @param   pageNo               [in] specified data pageno
       @param   bladdr              [out] current i/o address of pageno
       @param   bPageUpdated        [out] true if save pages is requested
       @param   bPendingPageUpdate  [out] true if save pages pending flag is set
       @return  (Converter_ReturnCode)
                Converter_Ok,
                Converter_PageNoAlreadyFree
     */

    Converter_ReturnCode FreePermPageNo(
        Converter_PermMapSection    &section,
        const Converter_Version     &snapShotVersion,
        const Data_PageNo           pageNo,
        IOMan_BlockAddress          &bladdr,
        bool                        &bPageUpdated,
        bool                        &bPendingPageUpdate );

    /*!
       @brief   Starts recovery of used pagenos. This method must be called prior to
                the first call of RestoreUsedPageNos(), cause it's responsible for
                the memory allocation.
       @return  (SAPDB_Bool) true if successful
    */

    SAPDB_Bool BeginRestoreUsedPageNos()
    {
        return m_IsPageUsedAfterRestorePages.Resize(GetMapEntryCount(), false);
    }

    /*!
       @brief   Performs recovery of used pagenos. Note that the method
                BeginRestoreUsedPageNos has to be called first. The given
                Iterator 'slotiter' points to a bitmap containing a base data
                pageno and a map of bits indicating the 'in-use' state
                of the corresponding converter leaf page entry.
       @param   section         [in|out] corresponding map section
       @param   snapShotVersion [in]     converter version of the snapshot
       @param   slotIter        [in]     iterator pointing to bitmap
       @return  (Converter_ReturnCode) Converter_Ok, Converter_FrameAllocationFailed,
     */

    Converter_ReturnCode RestoreUsedPageNos(
        Converter_PermMapSection                    &section,
        const Converter_Version                     &snapShotVersion,
        const Converter_BitMapPage::ConstIterator&  slotIter );

    /*!
       @brief   Finishes recovery of used pagenos of specified section and
                deallocation of memory. This method must be called after
                that all bitmap pages containing used pagenos have been
                examined with calls to RestoreUsedPageNos().
       @param   section         [in|out] corresponding map section
       @param   snapShotVersion [in]     converter version of the snapshot
       @return  none
     */

    void FinishSectionRestoreUsedPageNos(
        Converter_PermMapSection    &section,
        const Converter_Version     &snapShotVersion );

    /*!
       @brief   Finishes recovery of used pagenos, i.e. needed memory of
                auxillary dstructures will be released. This method must
                be called after that all bitmap pages containing used
                pagenos have been examined with calls to RestoreUsedPageNos()
                and FinishSectionRestoreUsedPageNos().
       @return  none
     */

    void FinishRestoreUsedPageNos()
    {
        m_IsPageUsedAfterRestorePages.Delete();
    }

    /* -----------------------------------------------------------------------*/
    /*! @name Conversion                                                      */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Reads and returns actual i/o address of specified data page
                If an error eccor, the invalid block address is returned.
       @param   pageNo [in] specified data pageno
       @param   bladdr [out] requested block address
       @return  (Converter_ReturnCode)
                Converter_Ok,
                Converter_NoConverterEntry,
                Converter_PageNoOutOfRange,
                Converter_InvalidBlockAddressFound
     */

    Converter_ReturnCode GetBlockAddress(
        const Data_PageNo  pageNo,
        IOMan_BlockAddress &bladdr ) const;


    /*!
       @brief   Sets i/o address of specified temporary data page
       @param   pageNo   [in] specified data pageno
       @param   addr     [in] block i/o address to be set
       @param   oldaddr [out] block i/o address currently occupied
       @return  (Converter_ReturnCode) Converter_Ok, Converter_PageNoNotReserved

       - The i/o address of a given page number 'pageno' is written
         to the corresponding converter leaf page.
         Additional the entry in use and the save pages requested (SPR)
         information is set. Only in case of a restore data the SPR
         is not set.
       - If the corresponding page entry is free (has not been reserved),
         an error is returned.
     */

    Converter_ReturnCode SetTempBlockAddress(
        const Data_PageNo           pageNo,
        const IOMan_BlockAddress    &addr,
        IOMan_BlockAddress          &oldaddr );

    /*!
       @brief   Sets i/o address of specified permanent data page
       @param   section          [in|out] corresponding map section
       @param   pageNo               [in] specified data pageno
       @param   addr                 [in] block i/o address to be set
       @param   oldaddr             [out] block i/o address currently occupied
       @param   bPageUpdated        [out] true if save pages is requested
       @param   bPendingPageUpdate  [out] true if save pages pending flag is set
       @param   bRestoreDataActive   [in] a data recovery is running
       @return  (SetPermBlockRc) 
                - spbaRcOkay, 
                - spbaRcPageNoNotReserved

       - The i/o address of a given page number 'pageno' is written
         to the corresponding converter leaf page.
         Additional the entry in use and the save pages requested (SPR)
         information is set. Only in case of a restore data the SPR
         is not set. Finaly the changed information for the leaf page is set.
       - If the corresponding page entry is free (has not been reserved),
         an error is returned.
     */
    enum SetPermBlockRc{
        spbaRcOkay              = Converter_Ok,
        spbaRcPageNoNotReserved = Converter_PageNoNotReserved
    };
    SetPermBlockRc SetPermBlockAddress(
        Converter_PermMapSection    &section,
        const Data_PageNo           pageNo,
        const IOMan_BlockAddress    &addr,
        IOMan_BlockAddress          &oldaddr,
        bool                        &bPageUpdated,
        bool                        &bPendingPageUpdate,
        bool                        bRestoreDataActive = false );

    /* -----------------------------------------------------------------------*/
    /*! @name Statistics and info                                             */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Returns true if the page specified by the given
                map index is changed.
       @return  (SAPDB_Bool)
    */

    SAPDB_Bool PageIsChanged( const Converter_MapEntryNo entryNo ) const{
        return !m_ChangedInfoVector[ entryNo ].IsInvalid();
    }

    /*!
       @brief   Returns the total count of map entries
       @return  (Converter_MapEntryCount) map entry count
     */

    Converter_MapEntryCount GetMapEntryCount() const{
        return m_PeriodCount * m_SectionCount * m_EntryClusterSize;
    }

    /*!
       @brief   Returns the actual capacity of pagenos of the whole map
       @return  (SAPDB_Int) pageno count
     */

    SAPDB_Int GetHighestPossiblePageNo() const{
        return m_HighestPossiblePageNo;
    }

    /*!
       @brief   Returns the number of data pages which can be
                referenced by a converter leaf page
       @return  (Converter_PageEntryCount)
     */

    Converter_PageEntryCount GetLeafPageCapacity() const{
        static Converter_PageEntryCount capa = Converter_LeafPage::GetEntryCount (m_LeafPageSize);
        return capa;
    }

    /* -----------------------------------------------------------------------*/
    /*! @name SnapShot                                                        */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief   Checks if the converter leaf page specified by the given
               basePageNo, has the same block address as the given block
               address.
      @param   basePageNo [in] data base page number
      @param   block      [in] block address of the converter leaf page
      @return  SAPDB_Bool
    */

    SAPDB_Bool IsMember(
        const Data_PageNo           basePageNo,
        const IOMan_BlockAddress    &block );

    /* -----------------------------------------------------------------------*/
    /*! @name Diagnose                                                        */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Dumps the map entries into the kernel dump file
       @param   dump [in|out] kernel dump file
       @return  none
     */
    void DumpEntries( Kernel_Dump    &dump );

    /*!
       @brief   Check the correctness of the Converter_Map. If an inconsistency
                is detected and emergency shutdown is executed. Note that this
                is cost intensive method, which should not be called permanently.
       @param   section [in|out] corresponding map section
       @return  none
     */

    void Verify( Converter_MapSection &section ) const;


    /*!
       @brief   Calculate the begin of a cluster in the converter map
       @param   section [in] map section
       @param   periodno [in] map period
       @return  address of first entry in cluster
     */

    Converter_MapEntryNo EntryClusterBeginNo(
        const SAPDB_UInt    section,
        const SAPDB_UInt    periodno )  const
    {
        SAPDB_UInt clusterno = periodno * m_SectionCount + section;
#if defined(HPUX) && defined(HP_IA64)
        clusterno = periodno * m_SectionCount + section;
        // two step calculation to fix strange optimisation error in HP IA64:
        // cc: HP aC++/ANSI C B3910B A.05.57 [June 23 2004] - henrik
#endif
        return clusterno * m_EntryClusterSize;
    }

    /*!
       @brief   Calculate the address of the entry behind a cluster in the converter map
       @param   section [in] map section
       @param   periodno [in] map period
       @return  address behind a cluster
     */

    Converter_MapEntryNo EntryClusterEndNo(
        const SAPDB_UInt    secno,
        const SAPDB_UInt    periodno ) const{
        return EntryClusterBeginNo(secno + 1, periodno);
    }

    /*!
       @brief   return the number of periods in the map
       @return  number of periods
     */

    SAPDB_UInt GetPeriodCount()
    {
        return m_PeriodCount;
    }


private:

    // Calculates the period count necessary to store a given number of pageNos

    SAPDB_UInt CalcPeriodCount( const Data_PageNo maxpageno ) const
    {
        SAPDB_UInt periodsize = m_EntryClusterSize * m_SectionCount;
        Converter_MapEntryNo maxentryno = (maxpageno / GetLeafPageCapacity()) + 1;
        return (maxentryno / periodsize ) + 1;
    }

    Converter_MapEntry& GetEntry( const Converter_MapEntryNo entryNo){
        return m_EntryVector[ entryNo ];
    }

    const Converter_MapEntry& GetEntry( const Converter_MapEntryNo entryNo) const{
        return m_EntryVector[ entryNo ];
    }

    Data_PageNo CalcBasePageNo( const Converter_MapEntryNo entryNo ) const{
        return entryNo * GetLeafPageCapacity();
    }

    Converter_MapEntryNo CalcEntryNo( const Data_PageNo pageNo ) const{
        return pageNo / GetLeafPageCapacity();
    }

    Converter_PageEntryNo CalcPageEntryNo( const Data_PageNo pageno) const{
        return pageno % GetLeafPageCapacity();
    }

    SAPDB_Bool UseUnusedEntryAndGetPage(
        Converter_LeafPage            &page,
        const Data_PageRecoveryMode   &recoveryMode,
        const Converter_MapEntryNo    entryNo );

    enum SetBlockRc{
        sbaRcOkay              = Converter_Ok,
        sbaRcPageNoNotReserved = Converter_PageNoNotReserved,
    };
    SetBlockRc SetBlockAddress(
        Converter_LeafPage          &page,
        const IOMan_BlockAddress    &addr,
        IOMan_BlockAddress          &oldaddr,
        const Converter_PageEntryNo pageEntryNo );

    void RemoveUnusedPageAfterRestore( // PTS 1132116 TS 2004-10-25
        const Converter_MapEntryNo  entryNo,
        Converter_PermMapSection&   section,
        const Converter_Version&    snapShotVersion );

private:

    /// buffer containing the map entries

    Converter_MapVector m_EntryVector;

    /// buffer containing an array of changed info objects

    Container_Vector<ChangedInfo>   m_ChangedInfoVector;

    /// used when freeing unused pagenos after recovery if the vector
    /// item is false the corresponding page could be set to free.

    Container_Vector<SAPDB_Bool>    m_IsPageUsedAfterRestorePages;

    /// number of subsequent(!) map entries (=> converter leaf pages)
    /// protected by one converter region. Default is eight

    Converter_MapEntryCount m_EntryClusterSize;

    /// number of sections. Each section is protected by it's own latch

    SAPDB_UInt  m_SectionCount;

    /// a period is the product of sectionCount and m_EntryClusterSize.
    /// The periodCount is the number of periods in the map

    SAPDB_UInt m_PeriodCount;

    /// size of a converter leaf page in bytes

    SAPDB_UInt  m_LeafPageSize;

    /// is the highest possible pageNo addressable with the map

    Data_PageNo  m_HighestPossiblePageNo;

};

#endif // CONVERTER_MAP_HPP
