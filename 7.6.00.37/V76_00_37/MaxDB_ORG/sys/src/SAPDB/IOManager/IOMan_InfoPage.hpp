/*!
  @file     IOMan_InfoPage.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Handler for a IOMan_InfoPage.
            A page handled by this class is stored at the begining of 
            each data base volume. It contains a various number of 
            administration informations of the current volume, for 
            example the logical device number and the size. This 
            informations are used to decide whether the volume is
            usable and in which state the volume is.

\if EMIT_LICENCE
\endif
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



#ifndef IOMAN_INFOPAGE_HPP
#define IOMAN_INFOPAGE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_Page74.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp" 


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// maximum length of db identifier
#define IOMAN_DB_IDENT_LENGTH   132

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @interface   IOMan_InfoPage
   @brief       Handler for IOMan_InfoPage. A page handled by this class is
                stored at the begining of each data base volume. It contains
                a various number of administration informations of the current
                volume, for example the logical device number and the size.
                This informations are used to decide whether the volume is
                usable and in which state the volume is.
 */

class IOMan_InfoPage : public Kernel_Page74
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Default constructor
       @return  none
     */

    IOMan_InfoPage()
    {}

    /*!
       @brief   Assigns frame and initializes frame contents
       @param   frame         [in] Page frame
       @param   devNo         [in] Logical volume identifier
       @param   capacity      [in] Capacity of the volume in pages
       @param   blockSize     [in] BlockSize in bytes
       @param   pDBIdentifier [in] Pointer to the unique identifier
       @return  none
     */

    void SetAndInitializeFrame(
        const PageFrame         &frame,
        const IOMan_DeviceNo    devNo,
        const IOMan_BlockCount  capacity,
        const SAPDB_UInt4       blockSize,
        const SAPDB_Byte       *pDBIdentifier )
    {
        SetFrame( frame );
        InitializeFrame( devNo, capacity, blockSize, pDBIdentifier );
    }

    /*!
       @brief   Initializes frame contents
       @param   devNo         [in] Logical volume identifier
       @param   capacity      [in] Capacity of the volume in pages
       @param   blockSize     [in] BlockSize in bytes
       @param   pDBIdentifier [in] Pointer to the unique identifier
       @return  none
     */

    void InitializeFrame(
        const IOMan_DeviceNo    devNo,
        const IOMan_BlockCount  capacity,
        const SAPDB_UInt4       blockSize,
        const SAPDB_Byte       *pDBIdentifier )
    {
        Kernel_Page74::InitializeFrame( Kernel_IOManInfoPageType );
        WritePageId( devNo );

        const SAPDB_UInt2 HeaderSize  = Kernel_Page74::GetHeaderSize();
        const SAPDB_UInt2 TrailerSize = Kernel_Page74::GetTrailerSize();

        // set the body of the frame to a defined value
        memset( m_Frame.GetPointer( HeaderSize, m_Frame.GetLength() - HeaderSize ),
                0x00, m_Frame.GetLength() - TrailerSize - HeaderSize );

        SetPrevLogicalVolNoToInvalid(),
        SetNextLogicalVolNoToInvalid(),
        SetCapacity( capacity );
        SetBlockSize( blockSize );
        SetBadBlockNo( 0 ),
        SetBadResetCount( 0 ),
        SetDBIdentifier( pDBIdentifier );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Frame access */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to write the capacity of the volume
                into the info page. The capacity is given in blocks.
       @param   capacity [in] Capacity of the volume in pages
       @return  none
     */

    void SetCapacity( const IOMan_BlockCount  capacity ){
        PersistentHeaderPtr()->m_Capacity = capacity;
    }

    /*!
       @brief   This method is used to write the logical device number of 
                the next logical volume into the info page. It is assumed
                that the info page is accessable.
       @param   nextVolNo [in] Logical volume identifier of the logical 
                               next volume
       @return  none
     */

    void SetNextLogicalVolNo( const IOMan_DeviceNo  nextVolNo )
    {
        SAPDBERR_ASSERT_STATE( GetInvalidLinkageVolNo() != nextVolNo );

        PersistentHeaderPtr()->m_NextLogicalDevNo = nextVolNo;
    }

    /*!
       @brief   This method is used to set the logical device number of 
                the next logical volume to invalid. It is assumed that 
                the info page is accessable.
       @return  none
     */

    void SetNextLogicalVolNoToInvalid(){
        PersistentHeaderPtr()->m_NextLogicalDevNo = GetInvalidLinkageVolNo();
    }

    /*!
       @brief   This method is used to write the logical device number of 
                the previous logical volume into the info page. It is 
                assumed that the info page is accessable.
       @param   prevVolNo [in] Logical volume identifier of the logical 
                               next volume
       @return  none
     */

    void SetPrevLogicalVolNo( const IOMan_DeviceNo    prevVolNo )
    {
        SAPDBERR_ASSERT_STATE( GetInvalidLinkageVolNo() != prevVolNo );

        PersistentHeaderPtr()->m_PrevLogicalDevNo = prevVolNo;
    }

    /*!
       @brief   This method is used to set the logical device number of 
                the previous logical volume to invalid. It is  assumed 
                that the info page is accessable.
       @return  none
     */

    void SetPrevLogicalVolNoToInvalid(){
        PersistentHeaderPtr()->m_PrevLogicalDevNo = GetInvalidLinkageVolNo();
    }

    /*!
       @brief   This method is used to write the block number of the bad
                block of the current volume into the info page. It is 
                assumed that the info page is accessable.
       @param   blockNo [in] block number of the bad block
       @return  none
     */

    void SetBadBlockNo( const IOMan_BlockNo   blockNo ){
        PersistentHeaderPtr()->m_BadBlockNo = blockNo;
    }

    /*!
       @brief   This method is used to increment the bad reset count. This 
                indicates how often a bad volume was reseted without 
                performing a data base restore.It is assumed that the 
                info page is accessable.
       @return  none
     */

    void IncrementBadResetCount(){
        PersistentHeaderPtr()->m_BadResetCount +=1;
    }

    /*!
       @brief   This method is used to read the logical device number of
                the volume the current page is stored in. This number must 
                be equal to the corresponding entry within the data base 
                configuration file. It is assumed that the info page is accessable.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetLogicalDevNo() const{
        return ReadPageId();
    }

    /*!
       @brief   This method is used to read the logical device number of
                the volume which is the next in logical order. This number
                must be equal to the logical next volume within the data 
                base configuration file. It is assumed that the info page is 
                accessable.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetNextLogicalDevNo() const{
        return( PersistentHeaderPtr()->m_NextLogicalDevNo );
    }

    /*!
       @brief   This method is used to read the logical device number of
                the volume which is the predecessor in logical order. This 
                number must be equal to the logical predecessor volume within 
                the data base configuration file. It is assumed that the info 
                page is accessable.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetPrevLogicalDevNo() const{
        return( PersistentHeaderPtr()->m_PrevLogicalDevNo );
    }

    /*!
       @brief   This method is used to read the capacity of the volume, The
                capacity is given in blocks and the block size is fixed for a
                volume and not changeable. This value must be equal to the 
                corresponding entry within the data base configuration file.
                It is assumed that the info page is accessable.
       @return  IOMan_BlockCount
     */

    IOMan_BlockCount GetCapacity() const{
        return( PersistentHeaderPtr()->m_Capacity );
    }

    /*!
       @brief   This method is used to read the block size of the volume, 
                The block size is fixed and not changeable. The default is
                8192 bytes. It is assumed that the info page is accessable.
       @return  SAPDB_UInt4
     */

    SAPDB_UInt4 GetBlockSize() const{
        return( PersistentHeaderPtr()->m_BlockSize );
    }

    /*!
       @brief   This method is used to read the number of the block which
                is responsible for setting the volume to bad. It is assumed 
                that the info page is accessable.
       @return  SAPDB_UInt4
     */

    SAPDB_UInt4 GetBadBlockNo() const{
        return( PersistentHeaderPtr()->m_BadBlockNo );
    }

    /*!
       @brief   This method is used to write the block address of the restart page.
                It is assumed that the info page is accessable.
       @return  IOMan_BlockAddress
     */

    void SetRestartPageAddress( const IOMan_BlockAddress restartPage )
    {
        PersistentHeaderPtr()->m_RestartPageVolumeNo = restartPage.GetDeviceNo();
        PersistentHeaderPtr()->m_RestartPageBlockNo  = restartPage.GetBlockNo();
    }

    /*!
       @brief   This method is used to read the block address of the restart page 
                It is assumed that the info page is accessable.
       @return  IOMan_BlockAddress
     */

    IOMan_BlockAddress GetRestartPageAddress() const
    {
        return IOMan_BlockAddress(PersistentHeaderPtr()->m_RestartPageVolumeNo,
                                  PersistentHeaderPtr()->m_RestartPageBlockNo );
    }

    /*!
       @brief   This method is used to read the unique identifier of the 
                volume. All volumes of the data base have the same 
                DBIdentifier! It is assumed that the info page is accessable.
       @return  (const SAPDB_Byte*) pointer to (not null terminated) string
     */

    const SAPDB_Byte* GetDBIdentifier() const{
        return( PersistentHeaderPtr()->m_DBIdent );
    }

    /*!
       @brief   This method is used to get the size of the DBIdentifier.
                It is assumed that the info page is accessable.
       @return  SAPDB_UInt4 size in bytes
     */

    SAPDB_UInt4 GetDBIdentifierSize() const
    {
        SAPDBERR_ASSERT_STATE( IOMAN_DB_IDENT_LENGTH ==
                               sizeof( PersistentHeaderPtr()->m_DBIdent ));

        return( sizeof( PersistentHeaderPtr()->m_DBIdent ));
    }

    /*!
       @brief   This method is used to validate if a predecessor is defined.
                It is assumed that the info page is accessable.
       @return  (bool) true if predecessor volume is definded
     */

    bool IsPrevLogicalDevNoValid() const{
        return( GetPrevLogicalDevNo() != GetInvalidLinkageVolNo());
    }

    /*!
       @brief   This method is used to validate if a successor is defined.
                It is assumed that the info page is accessable.
       @return  (bool) true if successor volume is definded
     */

    bool IsNextLogicalDevNoValid() const{
        return( GetNextLogicalDevNo() != GetInvalidLinkageVolNo());
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
private:
#endif

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Persistent layout of data within IOMan_InfoPage header
     */
    /* -----------------------------------------------------------------------*/

    // Must do this to make the source compileable
    typedef Kernel_Page74::PersistentHeader             PH1;    // page74 header

private:

    struct PersistentHeader
    {
        PH1             m_KernelPage74Header;
        SAPDB_UInt4     m_PrevLogicalDevNo;
        SAPDB_UInt4     m_NextLogicalDevNo;
        SAPDB_UInt4     m_Capacity;
        SAPDB_UInt4     m_BlockSize;
        SAPDB_UInt4     m_BadBlockNo;
        SAPDB_UInt4     m_BadResetCount;
        SAPDB_UInt4     m_RestartPageVolumeNo;
        SAPDB_UInt4     m_RestartPageBlockNo;
        SAPDB_UInt4     m_Filler1;
        SAPDB_UInt4     m_Filler2;
        SAPDB_Byte      m_DBIdent[IOMAN_DB_IDENT_LENGTH];
    };

    /*!
       @brief    Returns pointer to page header on frame
       @return  (PersistentHeader*) header pointer
     */

    PersistentHeader* PersistentHeaderPtr()
    {
        return( reinterpret_cast< PersistentHeader* >
                ( m_Frame.GetPointer( 0, sizeof( PersistentHeader ))));
    }

    const PersistentHeader* PersistentHeaderPtr() const
    {
        return( reinterpret_cast< const PersistentHeader* >
                ( m_Frame.GetPointer( 0, sizeof( PersistentHeader ))));
    }

    /*!
       @brief   Returns size of page header
       @return  (SizeType) header size
     */

    virtual Kernel_Page::SizeType GetHeaderSize() const
    {
        return( sizeof( PersistentHeader ));
    }

    /*!
       @brief   This method is used to write the block size of the volume
                into the info page.
       @param   blockSize [in] BlockSize in bytes
       @return  none
     */

    void SetBlockSize( const SAPDB_UInt4    blockSize ){
        PersistentHeaderPtr()->m_BlockSize = blockSize;
    }

    /*!
       @brief   This method is used to set the bad reset count.
       @param   resetCount [in] number of reset attemps
       @return  none
     */

    void SetBadResetCount( const SAPDB_UInt4    resetCount ){
        PersistentHeaderPtr()->m_BadResetCount = resetCount;
    }

    /*!
       @brief   This method is used to write the unique identfier of the 
                volume into the info page.
       @param   pDBIdentifier [in] Pointer to the unique identifier
       @return  none
     */

    void SetDBIdentifier( const SAPDB_Byte    *pDBIdentifier )
    {
        SAPDB_MemCopyNoCheck( PersistentHeaderPtr()->m_DBIdent,
                              pDBIdentifier, GetDBIdentifierSize() );
    }

    /*!
       @brief   Returns the invalid device number used for the volume linkage
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetInvalidLinkageVolNo() const{
        return 0; // former IOMAN_INVALID_LINKAGE = 0
    }
};


#endif  /* IOMAN_INFOPAGE_HPP */
