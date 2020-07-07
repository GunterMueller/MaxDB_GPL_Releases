/*!
  @file           Converter_Converter.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        Converter
  @brief          general converter header file
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


#ifndef CONVERTER_CONVERTER_HPP
#define CONVERTER_CONVERTER_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_IBackUp.hpp"
#include "Converter/Converter_IDiagnose.hpp"
#include "Converter/Converter_IDump.hpp"
#include "Converter/Converter_IPageIO.hpp"
#include "Converter/Converter_ISavePoint.hpp"
#include "Converter/Converter_IDropVolume.hpp"
#include "Converter/Converter_IManager.hpp"
#include "Converter/Converter_IMigration.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_IPageNoMapping.hpp"
#include "Converter/Converter_IPageNoManager.hpp"
#include "Converter/Converter_IVerify.hpp"
#include "Converter/Converter_IEvent.hpp"
#include "Converter/Converter_Index.hpp"
#include "Converter/Converter_MapControl.hpp"
#include "Converter/Converter_TempMapControl.hpp"
#include "Converter/Converter_PermMapControl.hpp"
#include "Converter/Converter_PermStaticMapControl.hpp"
#include "Converter/Converter_Types.hpp"

#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_Types.hpp"

#include "FreeBlockManagement/FBM_IManager.hpp"

#include "RunTime/RTE_Types.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"

#include "DataAccess/Data_Types.hpp"

#include "KernelCommon/DatabaseEvents/KernelDBE_Event.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_SynchronizedTaskWaitList.hpp"
#include "KernelCommon/Kernel_SynchronizedDBFullList.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Logging/Log_Savepoint.hpp"



/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

class Converter_DBSpaceStatistic;
class Kernel_Dump;
class RTETask_ITask;
class Msg_List;

/*!
  @class    Converter_Converter
  @brief    the converter
 */
class Converter_Converter : public Converter_IBackUp,
            public Converter_IDiagnose,
            public Converter_IDump,
            public Converter_IPageIO,
            public Converter_ISavePoint,
            public Converter_IDropVolume,
            public Converter_IManager,
            public Converter_IMigration,
            public Converter_IPageNoManager,
            public Converter_IPageNoMapping,
            public Converter_ICommon,
            public Converter_IVerify,
            public Converter_IEvent
{
public:


    /* -----------------------------------------------------------------------*/

    /*! @name Singelton */

    /*!
       @brief          static method to create a singelton of the type Converter_Converter
       @return         (Converter_Converter&amp;) reference to the instance of Converter_Converter
     */
    static        Converter_Converter& CreateInstance ();

    /*!
       @brief          returns the reference to the sigelton instance of Converter_Converter
       @return         (Converter_Converter&amp;) reference to the instance of Converter_Converter
     */
    static Converter_Converter & Instance  ()
    {
        SAPDBERR_ASSERT_STATE (m_Instance != NULL);
        return *m_Instance;
    }

    virtual Converter_Version Version() const{
        return m_Version;
    }

    virtual SAPDB_Bool BackUpIsActive() const{
        return (m_SaveDataIsActiv || m_SavePagesIsActiv);
    }

    virtual Data_PageCount GetNumberOfAllPages() const{
        return  m_NumAllPages;
    }

    virtual void GetDbSpaceStatistics(
        const RTETask_ITask         &task,
        Converter_DBSpaceStatistic  &dbSpaceInfo );

    virtual void ResetSystemViewDataStatistics();

    virtual SAPDB_UInt8 GetSystemViewResetTimeStamp() const{
        return m_ResetTimeStamp;
    }

    virtual SAPDB_UInt UpdatedPagesSinceLastSaveData(
        const RTETask_ITask  &task )
    {
        return( m_PermStaticMapControl.NumUpdatedPermPages( task ) +
                m_PermMapControl.NumUpdatedPermPages( task ));
    }

    SAPDB_UInt GetNumberOfUsedPages() const{
        return( m_NumPermPagesUsed + m_NumPermConvPages + m_NumTempPagesUsed );
    }

    SAPDB_UInt GetNumberOfChangedConvPages() const
    {
        return( m_PermMapControl.GetNumberOfChangedConvPages() +
                m_PermStaticMapControl.GetNumberOfChangedConvPages() );
    }

    virtual SAPDB_UInt UsedConverterPages() const{
        return( m_NumPermConvPages + m_NumTempConvPages );
    }

    virtual SAPDB_UInt GetPeakForTempPages() const{
        return( m_TemporaryPeakFilling );
    }

    virtual SAPDB_UInt GetPeakForFrozenTempPages() const{
        return( m_FrozenTemporaryPeakFilling );
    }

    virtual SAPDB_UInt GetPeakForPermPages() const{
        return( m_PermanentPeakFilling );
    }
    virtual SAPDB_UInt GetPeakForFrozenPermPages() const{
        return( m_FrozenPermanentPeakFilling );
    }

    virtual SAPDB_UInt GetNumberOfTempPages() const{
        return( m_NumTempPagesUsed );
    }

    virtual SAPDB_UInt GetNumberOfPermPages() const{
        return( m_NumPermPagesUsed );
    }

    SAPDB_UInt GetNumberOfSnapShotPages() const{
        return( m_NumSnapShotPages );
    }

    virtual bool IsDBFull() const;

    virtual SAPDB_Bool SecuritySpaceReached() const;

    virtual Converter_GarbageCollectionReturnCode GarbageCollectionNeeded() const;

    virtual bool CheckAndStartSavepoint(
        RTETask_ITask   &task,
        Msg_List        &msgList ) const;

    virtual SAPDB_Bool SavepointNeeded() const{
        return SavepointNeeded( NumChangedPages());
    }

    virtual Converter_ReturnCode Restart(
        const RTETask_ITask             &task,
        const Converter_Version         &ConverterVersion,
        const IOMan_PackedBlockAddress  &packedRootBlockAddr,
        const Converter_Version         &snapShotVersion,
        const IOMan_PackedBlockAddress  &packedSnapShotRootBlockAddr,
        const Kernel_DateTime           &snapShotCreationTimeStamp,
        const SAPDB_Bool                bIncConverterVersion,
        const SAPDB_Bool                bLastSaveDataSuccessful,
        const Data_PageNo               &MaxDynamicPageNo,
        const Data_PageNo               &MaxStaticPageNo,
        const SAPDB_UInt                NumAllPages,
        const SAPDB_UInt                volumeNoBitCount );

    virtual void ReadPages(
        const RTETask_ITask &task,
        SAPDB_UInt          pagerIndex,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO );

    virtual void ReadSnapShotPages(
        const RTETask_ITask &task,
        SAPDB_UInt          pagerIndex,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO );


    virtual Converter_ReturnCode RestartForRestore(
        const RTETask_ITask             &task,
        const Converter_Version         &ConverterVersion,
        const IOMan_PackedBlockAddress  &packedRootBlockAddr,
        const Converter_Version         &snapShotVersion,
        const IOMan_PackedBlockAddress  &packedSnapShotRootBlockAddr,
        const Kernel_DateTime           &snapShotCreationTimeStamp,
        const SAPDB_Bool                bLastSaveDataSuccessful,
        const Data_PageNo               &MaxDynamicPageNo,
        const Data_PageNo               &MaxStaticPageNo,
        const SAPDB_UInt                NumAllPages,
        const SAPDB_UInt                volumeNoBitCount );

    virtual void Create(
        const RTETask_ITask &task,
        const SAPDB_UInt    numAllPages );

    virtual void  CreateForRestore(
        const RTETask_ITask     &task,
        const Converter_Version &ConverterVersion,
        const SAPDB_Bool        bLastSaveDataSuccessful,
        const Data_PageNo       &maxDynamicPageNo,
        const Data_PageNo       &maxStaticPageNo,
        const SAPDB_UInt        numAllPages );

    virtual void Shutdown( const RTETask_ITask  &task );

    virtual SAPDB_Bool FlushSerial(
        const RTETask_ITask         &task,
        Converter_Version           &ConverterVersion,
        Data_PageNo                 &MaxStaticPageNo,
        Data_PageNo                 &MaxDynamicPageNo,
        IOMan_PackedBlockAddress    &packedRootBlockAddr );

    virtual SAPDB_Bool FlushSerialForColdSave(
        const RTETask_ITask         &task,
        IOMan_PackedBlockAddress    &packedRootBlockAddr)
    {
        // SAPDBERR_ASSERT_STATE( BackUpIsActive() ); collision with diagnose repair

        Converter_Version   dummyConverterVersion;
        Data_PageNo         dummyMaxStaticPageNo;
        Data_PageNo         dummyMaxDynamicPageNo;

        return(  FlushSerial( task, dummyConverterVersion, dummyMaxStaticPageNo,
                              dummyMaxDynamicPageNo, packedRootBlockAddr ));
    }

    virtual void FlushParallel(
        const RTETask_ITask         &task,
        Converter_Version           &ConverterVersion,
        Data_PageNo                 &MaxStaticPageNo,
        Data_PageNo                 &MaxDynamicPageNo,
        IOMan_PackedBlockAddress    &packedRootBlockAddr,
        SAPDB_UInt                  &NumPages,
        SAPDB_UInt                  &NumIO );

    virtual void WritePages(
        const RTETask_ITask &task,
        const SAPDB_UInt    iSection,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO );

    virtual SAPDB_Bool Expand(
        const RTETask_ITask &task,
        const SAPDB_UInt    numPagesAdded );

    SAPDB_Bool Shrink (
        const RTETask_ITask &task,
        const SAPDB_UInt    numPagesRemoved );

    void PrepareForDropVolume( const RTETask_ITask  &task );

    virtual Converter_ReturnCode BeginSaveData(
        const RTETask_ITask  &task,
        Msg_List             &errMsg );

    virtual Converter_ReturnCode BeginSavePages(
        const RTETask_ITask  &task,
        Msg_List             &errMsg );

    virtual void  EndSave(
        const RTETask_ITask &task,
        const SAPDB_Bool    bBackUpSuccessfullyFinished );

    virtual SAPDB_UInt GetNumPagesForBackUp(){
        return( m_NumPagesForBackUp );
    }

    virtual SAPDB_UInt GetNumConvPagesForBackUp(){
        return (m_NumStatConvPagesForBackUp + m_NumDynConvPagesForBackUp);
    }

    virtual SAPDB_Bool CheckBackUp( const RTETask_ITask  &task ) const
    {
        return
            (0 == FBM_IManager::Instance().NumBlocksMarkedForBackup( task.ID()))
            &&
            m_PermMapControl.PageNoListIsEmpty()
            &&
            m_PermStaticMapControl.PageNoListIsEmpty();
    }

    virtual bool  ResetDataPageAfterBackUp(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const Data_PageNo&          pageNo,
        const Data_AddressingMode&  addrMode )
    {
        if( ! m_Active ){
            return true;
        }
        if( Data_Dynamic == addrMode ){
            return InternResetDataPageAfterBackUp( task, msgList, m_PermMapControl, pageNo );
        }else if( Data_Static == addrMode ){
            return InternResetDataPageAfterBackUp( task, msgList, m_PermStaticMapControl, pageNo );
        }
        SAPDBERR_ASSERT_STATE( Data_Dynamic == addrMode ); // unknown addr mode
        return false;
    }

    virtual bool SetBlockAddressOfRestoredPage(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const IOMan_BlockAddress&   block,
        const Data_PageNo&          pageNo,
        const Data_AddressingMode&  addrMode )
    {
        SAPDBERR_ASSERT_STATE( m_RestoreDataActive || m_RestorePagesActive );

        switch( addrMode )
        {
        case Data_Dynamic:
            return m_PermMapControl.SetBlockAddressOfRestoredPage(
                       task, msgList, pageNo, m_SnapShotVersion, block, m_RestoreDataActive );
        case Data_Static:
            return m_PermStaticMapControl.SetBlockAddressOfRestoredPage(
                       task, msgList, pageNo, m_SnapShotVersion, block, m_RestoreDataActive );
        default:
            {
                return false;
            }
        }
    }

    virtual SAPDB_Bool GetStaticBitMapPage( Converter_BitMapPage &bitMapPage ){
        return m_PermStaticMapControl.GetBitMapPage( bitMapPage );
    }

    virtual SAPDB_Bool GetDynamicBitMapPage( Converter_BitMapPage &bitMapPage ){
        return m_PermMapControl.GetBitMapPage( bitMapPage );
    }

    virtual void RemoveBitMapPages()
    {
        m_PermStaticMapControl.RemoveBitMapPages();
        m_PermMapControl.RemoveBitMapPages();
    }

    virtual SAPDB_Bool GetStaticArchiveLeafPage( Converter_LeafPage  &leafPage ){
        return m_PermStaticMapControl.GetArchiveLeafPage( leafPage );
    }

    virtual SAPDB_Bool GetDynamicArchiveLeafPage( Converter_LeafPage &leafPage ){
        return m_PermMapControl.GetArchiveLeafPage( leafPage );
    }

    virtual void RemoveArchiveLeafPages()
    {
        m_PermStaticMapControl.RemoveArchiveLeafPages();
        m_PermMapControl.RemoveArchiveLeafPages();
    }

    virtual SAPDB_Bool BeginRestoreUsedPageNos()
    {
        return (m_PermStaticMapControl.BeginRestoreUsedPageNos() &&
                m_PermMapControl.BeginRestoreUsedPageNos());
    }

    virtual void RestoreUsedPageNos(
        const RTETask_ITask         &task,
        const Converter_BitMapPage &UsedPageNoBitMapPage )
    {
        SAPDBERR_ASSERT_STATE( m_RestorePagesActive );

        SAPDB_Int DiffNumPermPagesUsed = 0;

        if( Data_Dynamic == UsedPageNoBitMapPage.ReadPageAddressingMode())
        {
            m_PermMapControl.RestoreUsedPageNos( task, UsedPageNoBitMapPage,
                                                 m_SnapShotVersion, DiffNumPermPagesUsed );

            m_NumPermPagesUsed.Increment( DiffNumPermPagesUsed );
        }
        else if( Data_Static == UsedPageNoBitMapPage.ReadPageAddressingMode())
        {
            m_PermStaticMapControl.RestoreUsedPageNos( task, UsedPageNoBitMapPage,
                    m_SnapShotVersion, DiffNumPermPagesUsed );

            m_NumPermPagesUsed.Increment( DiffNumPermPagesUsed );
        }
    }

    virtual void RestoreConverterPage(
        RTETask_ITask&      task,
        Converter_LeafPage& leafPage );


    virtual void FinishRestoreUsedPageNos( const RTETask_ITask  &task )
    {
        SAPDBERR_ASSERT_STATE( m_RestorePagesActive );

        SAPDB_Int DiffNumPermPagesUsed = 0;

        m_PermStaticMapControl.FinishRestoreUsedPageNos( task,
                m_SnapShotVersion, DiffNumPermPagesUsed );

        m_NumPermPagesUsed.Increment( DiffNumPermPagesUsed );

        m_PermMapControl.FinishRestoreUsedPageNos( task,
                m_SnapShotVersion, DiffNumPermPagesUsed );

        m_NumPermPagesUsed.Increment( DiffNumPermPagesUsed );

        CheckFillingOfDataBase();
    }

    virtual void  BeginSavepoint( const RTETask_ITask  &task )
    {
        m_MaxUsedStaticPageNo  = m_PermStaticMapControl.SetSavepointActiveFlag( task );
        m_MaxUsedDynamicPageNo = m_PermMapControl.SetSavepointActiveFlag( task );
    }

    virtual void EndSavepoint(
        const RTETask_ITask &task,
        Converter_Version   &converterVersion );

    virtual void FreePageNosAfterSavepoint( const RTETask_ITask  &task );

    virtual Converter_ReturnCode GetPositionForReading(
        const RTETask_ITask             &task,
        const Data_PageNo               &pageNo,
        const Data_PageRecoveryMode     &pageRecoveryMode,
        IOMan_BlockAddress              &block );

    virtual void GetPositionForWriting(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &pageRecoveryMode,
        const Converter_Version     &pageConverterVersion,
        const SAPDB_Bool            bReqSequential,
        IOMan_BlockAddress          &block );

    virtual void SetPositionForWriting(
        const RTETask_ITask         &task,
        const Converter_Version     &pageConverterVersion,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &pageRecoveryMode,
        const IOMan_BlockAddress    &block );

    virtual void RequestNewTempPageNo(
        const RTETask_ITask &task,
        Data_PageNo         &newPageNo )
    {
        RequestNewPageNo( task, false, m_TempMapControl, newPageNo );

        m_NumTempPagesUsed.Increment();
        SAPDBTRACE_WRITELN( Converter_Trace, 7, "TempPagesUsed: " << m_NumTempPagesUsed );
    }

    virtual void RequestNewPermPageNo(
        const RTETask_ITask &task,
        const bool          bIgnoreSecureLimit,
        Data_PageNo         &newPageNo )
    {
        RequestNewPageNo( task, bIgnoreSecureLimit, m_PermMapControl, newPageNo );

        m_NumPermPagesUsed.Increment();
        SAPDBTRACE_WRITELN( Converter_Trace, 7, "PermPagesUsed: " << m_NumPermPagesUsed );
    }

    virtual void RequestNewStaticPageNo(
        const RTETask_ITask  &task,
        Data_PageNo          &newPageNo )
    {
        RequestNewPageNo( task, false, m_PermStaticMapControl, newPageNo );

        m_NumPermPagesUsed.Increment();
        SAPDBTRACE_WRITELN( Converter_Trace, 7, "PermPagesUsed: " << m_NumPermPagesUsed );
    }

    virtual SAPDB_Bool RequestSpecificiedNewStaticPageNo(
        const RTETask_ITask &task,
        const Data_PageNo   &pageNo );

    virtual SAPDB_Bool FreeTempPageNo(
        const RTETask_ITask &task,
        const Data_PageNo   &pageNo );

    virtual SAPDB_Bool FreePermPageNo(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Converter_Version     &pageConverterVersion )
    {
        return( FreePermPageNo( task, m_PermMapControl, pageNo, pageConverterVersion ));
    }

    virtual SAPDB_Bool FreeStaticPageNo(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Converter_Version     &pageConverterVersion )
    {
        return( FreePermPageNo( task, m_PermStaticMapControl, pageNo, pageConverterVersion ));
    }

    virtual Converter_ReturnCode HandleDBFull(
        Trans_Context   &trans,
        SAPDB_UInt      numDataPagesRequested );

    virtual Data_PageNo MaxTempPageNo() const{
        return m_TempMapControl.MaxPageNo();
    }

    virtual Data_PageNo MaxPermPageNo() const{
        return m_PermMapControl.MaxPageNo();
    }

    virtual Data_PageNo MaxStaticPageNo() const{
        return m_PermStaticMapControl.MaxPageNo();
    }

    virtual Converter_ReturnCode BeginCheckData(
        const RTETask_ITask &task,
        Msg_List            &errMsg );

    virtual void EndCheckData(
        const RTETask_ITask &task,
        Data_PageCount      &NumStaticDataPages,
        Data_PageCount      &NumPermDataPages,
        Data_PageCount      &NumPermConvPages,
        Data_PageCount      &NumReleaseBlocks );

    virtual void EndFailedCheckData( const RTETask_ITask   &task );

    virtual void MarkPageNoAsUsed(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_AddressingMode   &addrMode )
    {
        if( Data_Dynamic == addrMode )
            m_PermMapControl.MarkPageNoAsUsed( task, pageNo );
        else if( Data_Static == addrMode )
            m_PermStaticMapControl.MarkPageNoAsUsed( task, pageNo );
    }

    virtual SAPDB_Bool AddEvent(
        const SAPDB_Int                RelativeThreshold,
        const SAPDB_Bool               bOverflow,
        const tsp31_event_prio_Param   Priority)
    {
        // data area filling below 70 percent is not relevant for
        // knldiag output
        const bool bWriteIntoKnldiag = RelativeThreshold < 7000 ? false : true;

        return m_DBUsageMonitor.AddEvent(
                   RelativeThreshold, bOverflow, Priority, bWriteIntoKnldiag );
    }

    virtual SAPDB_Bool DelEvent(
        const SAPDB_Int    RelativeThreshold,
        const SAPDB_Bool   bOverflow )
    {
        return m_DBUsageMonitor.DelEvent(RelativeThreshold, bOverflow);
    }

    virtual void Dump(
        const RTETask_ITask &task,
        Kernel_Dump         &Dump,
        const SAPDB_Bool    bEnterRegion );

    virtual void SetBlockAddressForMigratedPage(
        const RTETask_ITask&         task,
        const IOMan_BlockAddress    &block,
        const Data_PageNo           &pageNo,
        const SAPDB_Bool            bSetToOccupied = SAPDB_TRUE );

    virtual IOMan_BlockAddress DetermineAndSetBlockAddressForMigratedPage(
        const RTETask_ITask &task,
        const Data_PageNo   &pageNo );

    virtual SAPDB_Bool GetBlockAddress(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &recMode,
        IOMan_BlockAddress          &block );

    virtual SAPDB_Bool SetNewBlockAddress(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &recMode,
        const IOMan_BlockAddress    &blockAddr );

    virtual SAPDB_Bool DeleteBlockAddress(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &recMode );

    virtual SAPDB_UInt GetVolumeNoBitCount() const{
        return( m_VolumeNoBitCount );
    }

    Kernel_DateTime GetSnapShotCreationTime() const{
        return( m_SnapShotCreationTimeStamp );
    }

    /// Increment number of converter pages used for permanent data page numbers.

    void IncrementPermConvPageCount(){
        m_NumPermConvPages.Increment();
    }

    /// Decrement number of converter pages used for permanent data page numbers.

    void DecrementPermConvPageCount(){
        m_NumPermConvPages.Decrement();
    }

    /// Increment number of converter pages used for temporary data page numbers.

    void IncrementTempConvPageCount(){
        m_NumTempConvPages.Increment();
    }

    /// Decrement number of converter pages used for temporary data page numbers.

    void DecrementTempConvPageCount(){
        m_NumTempConvPages.Decrement();
    }

    /// Returns the frozen converter snapshot version

    Converter_Version GetSnapShotVersion() const{
        return( m_SnapShotVersion );
    }

    /// checks if the given converter leaf page is already part of
    /// the converter map. If so true is returned else false.

    SAPDB_Bool IsLeafPageMember(
        const RTETask_ITask         &task,
        const Converter_LeafPage    &page );

    /// adjust number of snapshot pages. see description of
    /// member data m_NumSnapShotPages

    void IncrementSnapShotSize(){
        m_NumSnapShotPages.Increment();
    }

    /// Increment number of used permanent (dynamic and static) pages
    void IncrementNumUsedPermPages(){
        m_NumPermPagesUsed.Increment();
    }


    Converter_TempMapControl &GetTempMapControl() {
        return m_TempMapControl;
    }

    Converter_PermMapControl &GetPermMapControl() {
        return m_PermMapControl;
    }

    Converter_PermStaticMapControl  &GetPermStaticMapControl() {
        return m_PermStaticMapControl;
    }

private:

    /// Is a list of data volume specific block addresses. Each item
    /// is a list of block address belonging to the same data volume.

    typedef Container_Vector<Converter_Index::BlockVector> VolumeList;

    /* -----------------------------------------------------------------------*/
    /*! @name private methods                                                 */
    /* -----------------------------------------------------------------------*/

    /// Assignment operator is not supported

    Converter_Converter& operator=( const Converter_Converter &converter );

    /// Copy constructor is not supported

    Converter_Converter( const Converter_Converter &converter );

    /*!
       @brief          default constructor
       @param          allocator [in|out] Allocator for memory allocation
       @return         none
     */

    Converter_Converter( SAPDBMem_IRawAllocator  &allocator );

    /*!
       @brief          initialize primitive members of the converter
       @param          NumAllPages [in] maximum number of pages which can be stored in the data base.
                       this corresponds to the sum of size of all devices
       @return         none
     */

    void InitializeBase( const SAPDB_UInt NumAllPages );

    /*!
       @brief          Expand a particular map of the converter
       @param          task          [in] own task
       @param          recoveryMode  [in] RecoveryMode of the map to expand
       @param          numPagesAdded [in] value by which the map is to expand
       @return         Returns true if the expansion succeded
     */

    SAPDB_Bool ExpandMap(
        const RTETask_ITask          &task,
        const Data_PageRecoveryMode  &recoveryMode,
        const SAPDB_UInt             numPagesAdded );

    /*!
       @brief          Triggers events or entries into the knldiag if the filling of the
                       data base passes critical values
       @return         none
     */

    void CheckFillingOfDataBase()
    {
        const SAPDB_UInt usedBlocks = FBM_IManager::Instance().GetNumberOfUsedBlocks();
        m_DBUsageMonitor.Watch( SAPDB_MAX( usedBlocks, GetNumberOfUsedPages()));
    }

    /*!
       @brief          Returns true if it's possible to execute a savepoint.
                       If the converter is not active or in maintenance or a savepoint
                       is still requested no new savepoint is possible.
       @return         true if a savepoint is executable
     */

    bool SavepointIsPossible() const
    {
        if( ! m_Active ) // converter is offline
            return( false );

        if( m_InMaintenanceMode ) // converter is in special mode
            return( false );

        if( Log_Savepoint::Instance().IsActive() ) // SVP is still requested
            return( false );

        return( true );
    }

    /// see description of public method SavepointNeeded
    bool SavepointNeeded( const SAPDB_Int4 numChangedPages ) const;

    /*!
        @brief  Request for unsused pageNo's for the given map control.
                If no pageNo is available, the task will be suspended
                as long as a pageNo is available. If bIgnoreSecureLimit
                is set to true (not default behaviour) no secure space
                limit is considered, i.e. the task will be suspended if
                the data area is realy full. If not bIgnoreSecureLimit
                is set a secure space is held.
        @param  task              [in] own task
        @param  mapControl    [in|out] corresponding map control
        @param  bIgnoreSecureLimit[in] use complete data area
        @param  newPageNo        [out] new pageNo
        @return none
    */

    void RequestNewPageNo(
        const RTETask_ITask     &task,
        const bool              bIgnoreSecureLimit,
        Converter_MapControl    &mapControl,
        Data_PageNo             &newPageNo );

    /*!
       @brief   Puts back the given  permanent page number to
                the "pool" of unused numbers.
       @param   task                 [in] own task
       @param   mapControl       [in|out] corresponding map control
       @param   pageNo               [in] number of the page to release
       @param   pageConverterVersion [in] converter version of the page to freed
       @return  (SAPDB_Bool) true if successfully executed
     */

    SAPDB_Bool FreePermPageNo(
        const RTETask_ITask         &task,
        Converter_PermMapControl    &mapControl,
        const Data_PageNo           pageNo,
        const Converter_Version     &pageConverterVersion );

    /*!
       @brief   Is used to read the converter materialized by the create
                snapshot command. All used pages of are snapshot converter
                are marked as occupied in the FBM. The snapshot converter
                won't be integrated in converter maps. It's assumed that
                this method is called after the proper converter was read.
       @param   task                    [in] own task
       @param   packedSnapShotRootBlock [in] packed block address of the
                                             root of the snapshot converter.
       @return  none
     */

    void RestartSnapShot(
        const RTETask_ITask             &task,
        const IOMan_PackedBlockAddress  &packedSnapShotRootBlock );

    /*!
        @brief  Returns the numbers of updated converter and data pages
                in th emain memory.
        @return SAPDB_Int4
    */

    SAPDB_Int4 NumChangedPages() const;

    /*!
        @brief  Adjust the peaks for the permanent and temporary
                filling of the data area. This will be done at
                the point of restart and each savepoint.
        @return none
    */

    void CheckDataAreaPeakFilling()
    {
        if( m_TemporaryPeakFilling < m_NumTempPagesUsed ){
            m_TemporaryPeakFilling = m_NumTempPagesUsed;
        }

        if( m_PermanentPeakFilling < m_NumPermPagesUsed ){
            m_PermanentPeakFilling = m_NumPermPagesUsed;
        }

        if( m_FrozenTemporaryPeakFilling < m_NumTempPagesUsed ){
            m_FrozenTemporaryPeakFilling = m_NumTempPagesUsed;
        }

        if( m_FrozenPermanentPeakFilling < m_NumPermPagesUsed ){
            m_FrozenPermanentPeakFilling = m_NumPermPagesUsed;
        }
    }

    /*!
       @brief   Checks if the given number of pages is addressable with the converter.
                This check includes a security limit of four percent, but not more
                than 4.000 pages.
       @param   numPagesRequested [in] number of requested pages
       @return  true is given number of pages is addressable
     */

    // PTS 1124331 TS 2003-09-23
    SAPDB_Bool IsSpaceAvailable(  const SAPDB_Int4 numPagesRequested ) const
    {
        return(( numPagesRequested + GetNumberOfUsedPages() ) < m_SecuritySpaceLimit );
    }

    /*!
        @brief  configuration parameter VOLUMENO_BIT_COUNT is checked and
                set once if needed. The check sould protect the database
                against unauthorized modifications of the configuration file
                inparticular the VOLUMENO_BIT_COUNT parameter, because this one
                is relevant for the intrepretation of the converter root block
                address in the restart record.
        @param  volumeNoBitCount [in] current VOLUMENO_BIT_COUNT from the config file
        @return none
    */

    void CheckAndMigrateConfigurationParameter(
        const SAPDB_UInt volumeNoBitCount ) const;
    /*!
        @brief  update the total number of usable pages, adjust the filling thresholds
                and resume all tasks being suspended because pageNo's are exhausted
        @param  task          [in] own task
        @param  numPagesAdded [in] number of pages to be added to the current page count
        @return none
    */

    void AdjustDataBaseSize(
        const RTETask_ITask &task,
        const SAPDB_Int    numPagesAdded );

    /*!
        @brief  Set secure space limit; 2 percent of data base size but not more than 50 pages
        @return none
    */

    void SetSecuritySpaceLimit(){
        m_SecuritySpaceLimit = ( m_NumAllPages - SAPDB_MIN( m_NumAllPages/50, 50));
    }


    bool InternResetDataPageAfterBackUp(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        Converter_PermMapControl&   mapControl,
        const Data_PageNo&          pageNo );

private:

    // Contains pointer of the converter singleton

    static Converter_Converter  *m_Instance;

    // This is a named allocator used by the converter to fulfill
    // all converter specific memory allocations.
    // DO NOT REMOVE THIS FROM TOP, BECAUSE OF C++ INITIALIZATION LIST

    RTEMem_AllocatorWrapper     m_Allocator;

    // The blocking size defines a couple of strict ascending
    // ordered map entries protected by one latch, i.e. the
    // first eight leaf pages will be protected via latch one
    // the next eight leaves will be protected via latch two etc.
    // After all latches are used one time the next couple of
    // leaves will be protected again vi latch one.

    const SAPDB_UInt    m_MapBlockingSize;

    // Defines the number of bits in a four byte converter block
    // address reserved for the logical volume id

    const SAPDB_UInt    m_VolumeNoBitCount;

    // Number of converter latches used to synchronize the map access
    // Dynamic and static map are using the same latches for the
    // same range of values.

    const SAPDB_UInt    m_NumRegions;

    // BaseId of the first converter latch. Precondition is
    // that all converter latche id's are in strict ascending
    // order.

    const RTE_RegionId    m_BaseRegionId;

    // Defines the size of a converter page in bytes.

    const SAPDB_UInt    m_PageSize;

    /// This is the administrator for the management all pages
    /// belonging to the temporary pageId domain. The temporary
    /// pages are not recoverable and therefore they are handled
    /// as dynmic addressed pages.

    Converter_TempMapControl    m_TempMapControl;

    /// This is the administrator for the management all pages
    /// belonging to the permanent pageId domain.

    Converter_PermMapControl    m_PermMapControl;

    /// This is the administrator for the management all pages
    /// belonging to the permanent static pageId domain.

    Converter_PermStaticMapControl  m_PermStaticMapControl;

    // This is the handle for the converter index used to manage
    // all converter index pages. The converter index is used to
    // find all converter leaf pages during restart.

    Converter_Index     m_Index;

    // This is the current converter version used to manage the state
    // of the blocks (FBM) changed in a savepoint cycle. After a
    // successful savepoint this version will be incremented.

    Converter_Version   m_Version;

    // This is the current block address of the converter root page.
    // Each successful savepoint modifies this block address.

    IOMan_BlockAddress  m_RootBlockAddress;

    // This converter version is used to identify a converter snapshot created
    // with a special administration command. This snapshot offers the possibilty
    // to freeze a data stock and to reuse it after a couple of time without seeing
    // the changes made after the snapshot creation.

    Converter_Version   m_SnapShotVersion;

    // This is the savepoint 'timestamp' of the restart record which
    // was frozen with the create snapshot command.

    Kernel_DateTime m_SnapShotCreationTimeStamp;

    // Determine last time, when systemview DataStatistic was reseted
    SAPDB_UInt8   m_ResetTimeStamp;

    // Total number of used permanent (dynamic and static) pages

    RTESync_InterlockedCounter<SAPDB_UInt>  m_NumPermPagesUsed;

    // Total number of used temporary pages

    RTESync_InterlockedCounter<SAPDB_UInt>  m_NumTempPagesUsed;

    // Total number of converter pages containing the mapping
    // information for permanent data pages

    RTESync_InterlockedCounter<SAPDB_UInt>  m_NumPermConvPages;

    // Total number of converter pages containing the mapping
    // information for temporary data pages

    RTESync_InterlockedCounter<SAPDB_UInt>  m_NumTempConvPages;

    // Number of converter leaves and data pages being part
    // of the snapshot.

    RTESync_InterlockedCounter<SAPDB_UInt>  m_NumSnapShotPages;

    // Is the highest number of temporary pages used since
    // the data base is restarted. At each savepoint is value
    // will be updated.

    SAPDB_UInt  m_TemporaryPeakFilling;

    // Is the highest number of temporary pages used since
    // the last reset statement of the system view DataStatistics
    SAPDB_UInt  m_FrozenTemporaryPeakFilling;

    // Is the highest number of permanent pages since
    // the data base is restarted. At each savepoint is value
    // will be updated.

    SAPDB_UInt  m_PermanentPeakFilling;

    // Is the highest number of permanent pages used since
    // the last reset statement of the system view DataStatistics

    SAPDB_UInt  m_FrozenPermanentPeakFilling;

    // Number of addressable data pages within the data area

    SAPDB_UInt  m_NumAllPages;

    // Security limit is 4 percent of all addressable data pages
    // but not more than 200 pages

    SAPDB_UInt  m_SecuritySpaceLimit;

    // Number of data pages at the moment of data backup.
    // This number of pages will be written into the backup file.

    SAPDB_UInt  m_NumPagesForBackUp;

    // Number of converter leaves at the moment of data backup
    // containing the mapping for static data pageNos.

    SAPDB_UInt  m_NumStatConvPagesForBackUp;

    // Number of converter leaves at the moment of data backup
    // containing the mapping for static data pageNos.

    SAPDB_UInt  m_NumDynConvPagesForBackUp;

    // Highest permanent pageNo within the dynamic map,
    // updated at the beginning of a savepoint.

    Data_PageNo m_MaxUsedDynamicPageNo;

    // Highest permanent pageNo within the static map,
    // updated at the beginning of a savepoint.

    Data_PageNo m_MaxUsedStaticPageNo;

    // converter states

    SAPDB_Bool  m_Active;
    SAPDB_Bool  m_InMaintenanceMode;
    SAPDB_Bool  m_SavePagesIsActiv;
    SAPDB_Bool  m_SaveDataIsActiv;
    SAPDB_Bool  m_RestoreDataActive;
    SAPDB_Bool  m_RestorePagesActive;
    SAPDB_Bool  m_ColdVerifyIsActive;

    // This is a task wait queue used to manage all tasks
    // requesting for a pageNo and suspended because
    // the pageNo could not be provided (db full situation)

    Kernel_SynchronizedDBFullList   m_DBFullWaitQueue;

    // This is a task wait queue used to manage all tasks
    // requesting for a static pageNo and suspended because
    // the pageNo could not be provided (db full situation)


    Kernel_SynchronizedTaskWaitList m_FreePageNoWaitQueue;

    // This is a task wait queue used to suspend the task which
    // collides during extension of the converter maps caused by
    // a add data volume with a savepoint.

    Kernel_SynchronizedTaskWaitList m_ExpandConverterWaitQueue;

    // This is used to monitor the data area utilization, i.e
    // the passing of defined limits will be written into the
    // knldiag and an event is send.

    KernelDBE_EventSet  m_DBUsageMonitor;

    // This is a collection of volume specific lists. Each list
    // contains the block addresses of all converter leaf pages
    // stored on one data volume in ascending order. This is used
    // to speed up the restart of the converter by taking advantage
    // of the spreading of the converter leaf pages over all data
    // volumes.

    VolumeList  *m_VolumeList;
};

#endif  /* CONVERTER_CONVERTER_HPP */
