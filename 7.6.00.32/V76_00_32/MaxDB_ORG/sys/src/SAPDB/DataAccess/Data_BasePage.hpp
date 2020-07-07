/*!**************************************************************************

  module      : Data_BasePage.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Data Base Page
  description : This module is used to encapsulate the data page pascal
                implementaion for the I/O management without problems 
                with the cache properties of the derived class Data_Page
                
                This class offers all methods needed by the data page 
                I/O management.

  last changed: 2001-02-26  8:43
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/


#ifndef DATA_BASEPAGE_HPP
#define DATA_BASEPAGE_HPP

#include "gbd00.h"

#include "Converter/Converter_Version.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_Page72.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "SAPDBCommon/SAPDB_PersistentTypes.hpp"

class Data_PageAccessManager;
class SAPDBErr_MessageList;

/*!*****************************************************************************

   class: Data_BasePage

   description: This class is the encapsulate the data page pascal 
                implementaion for the I/O management.

                This class is needed to separate the data page I/O properties
                from the data page cache protperties, implemented by the derived
                class Data_Page.
              
*******************************************************************************/

class Data_BasePage : public Kernel_Page72
{
    /// The page access manager is a friend of this class.
    friend class Data_PageAccessManager;
public:
    /* -----------------------------------------------------------------------*/
    /*! chapter: Type definitions                                             */
    /* -----------------------------------------------------------------------*/
    /*!------------------------------------------------------------------------
    declaration : PageWriteCount
    description:  Indicates how often a page was written in the past
    --------------------------------------------------------------------------*/

    typedef SAPDB_Int4      PageWriteCount;

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Type definitions                                          */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function:     Data_BasePage()
    description:  This is the default constructor of a data base page qualified
                  for making I/O, without having problems with the cache
                  properties of the derived class Data_Page. The recovery mode 
                  is set per default to dynamic and recoverable.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    Data_BasePage ()
    : m_AccessMode     (Data_ForRead),
    m_ReleaseMode    (Data_Unchanged),
    m_LruChainInfo   (Data_LruNormal),
    m_pAccessManager (NULL)
    {}

    /*!-------------------------------------------------------------------------
    function:     Data_BasePage()
    description:  Construct a data base page with a given page image qualified
                  for making I/O, without having problems with the cache
                  properties of the derived class Data_Page.The recovery mode 
                  is set per default to dynamic and recoverable.
    arguments:    frame [in] data page
    return value: none
    --------------------------------------------------------------------------*/

    Data_BasePage( const PageFrame &frame )
    : Kernel_Page72    (frame),
    m_AccessMode     (Data_ForRead),
    m_ReleaseMode    (Data_Unchanged),
    m_LruChainInfo   (Data_LruNormal),
    m_pAccessManager (NULL)
    {}

    /*!-------------------------------------------------------------------------
    function:     Data_BasePage()
    description:  Construct a data base page with a given page image qualified
                  for making I/O, without having problems with the cache
                  properties of the derived class Data_Page.
                  The recovery mode is set to the given recovery mode.
    arguments:    frame        [in] data page
                  recoveryMode [in] recovery mode
    return value: none
    --------------------------------------------------------------------------*/

    Data_BasePage(
        const PageFrame             &frame,
        const Data_PageRecoveryMode &recoveryMode)
    : Kernel_Page72    (frame),
    m_AccessMode     (Data_ForRead),
    m_ReleaseMode    (Data_Unchanged),
    m_LruChainInfo   (Data_LruNormal),
    m_pAccessManager (NULL)
    {
        SetRecoveryMode( recoveryMode );
    }

    /*!-------------------------------------------------------------------------
    function:     Data_BasePage()
    description:  This method is used to construct a data base page with a 
                  given page image for making I/O, without having problems 
                  with the cache properties of the derived class Data_Page.
                  The recovery mode  is set per default to dynamic and recoverable.
    arguments:    pNode   [in] data page image
                  pCblock [in] data cache control block pointer for data page
    return value: none
    --------------------------------------------------------------------------*/

    Data_BasePage( tbd_nodeptr  pNode,
                   void        *pCblock = NULL )
    : m_AccessMode     (Data_ForRead),
    m_ReleaseMode    (Data_Unchanged),
    m_LruChainInfo   (Data_LruNormal),
    m_pAccessManager (NULL)
    {
        m_Frame.Assign( pNode, sizeof( *pNode ));
        SetHint( pCblock);
    }

    /*!-------------------------------------------------------------------------
    function:     Data_BasePage()
    description:  This method is used to construct a data base page with a 
                  given page image for making I/O, without having problems 
                  with the cache properties of the derived class Data_Page.
                  The recovery mode is build by the given parameters bIsDynamic
                  and bIsTemp.
    arguments:    pNode      [in] data page image
                  bIsDynamic [in] addressing mode of the given page
                  bIsTemp    [in] the given page is a temporary page
                  pCblock    [in] data cache control block pointer for data page
    return value: none
    --------------------------------------------------------------------------*/

    Data_BasePage(
        tbd_nodeptr pNode,
        SAPDB_Bool  bIsDynamic,
        SAPDB_Bool  bIsTemp,
        void *pCblock = NULL )
    : m_AccessMode     (Data_ForRead),
    m_ReleaseMode    (Data_Unchanged),
    m_LruChainInfo   (Data_LruNormal),
    m_pAccessManager (NULL)
    {
        m_Frame.Assign( pNode, sizeof( *pNode ));
        SetHint( pCblock);

        if( bIsTemp )
        {
            SAPDBERR_ASSERT_STATE( bIsDynamic );

            SetRecoveryMode( Data_PageRecoveryMode( Data_Dynamic, Data_NotRecoverable ));
        }
        else if( ! bIsDynamic )
            SetRecoveryMode( Data_PageRecoveryMode( Data_Static, Data_Recoverable ));
        else
            SetRecoveryMode( Data_PageRecoveryMode( Data_Dynamic, Data_Recoverable ));
    }

    /*!-------------------------------------------------------------------------
    function:     Assign()
    description:  This method is used to assign a data base page a given data 
                  base page for making I/O, without having problems with the cache
                  properties of the derived class Data_Page.
    arguments:    page [in] data page
    return value: none
    --------------------------------------------------------------------------*/

    void Assign( const Data_BasePage &page )
    {
        SetFrame( page.GetFrame() );
        SetHint( page.GetHint() );
        SetRecoveryMode( page.RecoveryMode() );
    }

    /*!-------------------------------------------------------------------------
    function:     Assign()
    description:  This method is used to assign a data base page a new content.
                  for making I/O, without having problems with the cache
                  properties of the derived class Data_Page. Note the the given
                  page input is no data page handler!
    arguments:    pNode      [in] data page image
                  bIsDynamic [in] addressing mode of the given page
                  bIsTemp    [in] the given page is a temporary page
                  pCblock    [in] data cache control block pointer for data page
    return value: none
    --------------------------------------------------------------------------*/

    void Assign(
        const tbd_nodeptr pNode,
        const SAPDB_Bool  bIsDynamic,
        const SAPDB_Bool  bIsTemp,
        void *pCblock = NULL )
    {
        m_Frame.Assign( pNode, sizeof( *pNode ));
        SetHint( pCblock);

        if( bIsTemp )
        {
            SAPDBERR_ASSERT_STATE( bIsDynamic );

            SetRecoveryMode( Data_PageRecoveryMode( Data_Dynamic, Data_NotRecoverable ));
        }
        else if( ! bIsDynamic )
            SetRecoveryMode( Data_PageRecoveryMode( Data_Static, Data_Recoverable ));
        else
            SetRecoveryMode( Data_PageRecoveryMode( Data_Dynamic, Data_Recoverable ));
    }

    void AssignNodePtr(
        const tbd_nodeptr pNode)
    {
        m_Frame.Assign( pNode, sizeof( *pNode ));
    }

    /// This initializes all node header fields used here.
    virtual void InitializeFrame ()
    {
        // many parts of the Kernel72_Page frame were initialized in vbd13.cpp
        /*
        NodePtr()->nd_id()     = SAPDB_MAX_INT4;
        NodePtr()->nd_root()   = SAPDB_MAX_INT4;
        NodePtr()->nd_bottom() = MinFreeOffset();
        */
    }
    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Data Base Page I/O management access                         */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function:     CheckAfterRead()
    description:  Prepares page for input from device
                  - Performs parity check on the frame contents
                  - Recalculates parity from frame contents and compares
                    the result with parity bytes kept in page header and trailer
                  - Applicable immediately after a page is read from device
    arguments:    none
    return value: true if check successful
    --------------------------------------------------------------------------*/

    virtual SAPDB_Bool CheckAfterRead() const;

    /*!-------------------------------------------------------------------------
    function:     ConverterVersion()
    description:  This method is used to get the converter version of the data 
                  page header. On the one hand this indicates the point at which
                  the page was written the last time and on the other hand it is 
                  needed by the component converter to handle the state of the 
                  corresponding block in the component free block management (FBM).
    arguments:    none
    return value: Converter_Version
    --------------------------------------------------------------------------*/

    Converter_Version ConverterVersion() const
    {
        return( PersistentHeaderPtr()->m_RemainingNodeHeader.m_ConverterVersion );
    }

    /*!-------------------------------------------------------------------------
    function:     GetWriteCount()
    description:  This method provides the write count. This indicates how
                  often the page was written in the past.
    arguments:    none
    return value: PageWriteCount
    --------------------------------------------------------------------------*/

    PageWriteCount GetWriteCount() const
    {
        return( PersistentHeaderPtr()->m_RemainingNodeHeader.m_WriteCount );
    }

    /*!-------------------------------------------------------------------------
    function:     IncrementAndSetWriteCount()
    description:  This method is used to increment the write count and make
                  him persistent within the data page header.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void IncrementAndSetWriteCount()
    {
        if( SAPDB_MAX_INT4 <= GetWriteCount() )
            PersistentHeaderPtr()->m_RemainingNodeHeader.m_WriteCount = 0;
        else
            PersistentHeaderPtr()->m_RemainingNodeHeader.m_WriteCount +=1;
    }

    /*!-------------------------------------------------------------------------
    function:     PageNo()
    description:  Reads page id from frame
    arguments:    none
    return value: (Data_PageNo) page id
    --------------------------------------------------------------------------*/

    Data_PageNo PageNo() const
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );

        return Kernel_Page72::GetPageID();
    }

    /*!-------------------------------------------------------------------------
    function:     PrepareForWrite()
    description:  Prepares page for output to device
                  - Reads and check parity information of frame contents
                  - Updates parity information with actual frame contents
                  - Applicable immediately before a page is written to device
    arguments:    none
    return:       true if check successful
    --------------------------------------------------------------------------*/

    virtual SAPDB_Bool PrepareForWrite();

    /*!-------------------------------------------------------------------------
    function:     RecoveryMode()
    description:  This method is used to get the recovery mode of the page.
    arguments:    none
    return:       Data_PageRecoveryMode
    --------------------------------------------------------------------------*/

    Data_PageRecoveryMode   RecoveryMode() const
    {
        return( m_RecoveryMode );
    }

    /*!-------------------------------------------------------------------------
    function:     SetRecoveryMode()
    description:  This method is used to set the recovery mode of the page from
                  default dynamic and recoverable to the given.
    arguments:    PageRecoveryMode [in] new recovery mode
    return:       none
    --------------------------------------------------------------------------*/

    void SetRecoveryMode( const Data_PageRecoveryMode& RecoveryMode )
    {
        m_RecoveryMode = RecoveryMode;
    }

    /*!-------------------------------------------------------------------------
    function:     VolumeMode()
    description:  This method is used to get the volume access mode of
                  the page. This question could be answered only if a
                  page is asigned, If not the return value is 
                  Data_VolumeUnknown. If the page belongs to an archive file
                  the return value is Data_VolumeSequential else Data_VolumeNormal.
    arguments:    none
    return:       volume access mode
    --------------------------------------------------------------------------*/

    Data_VolumeMode VolumeMode() const;

    /*!-------------------------------------------------------------------------
    function:     SetConverterVersion()
    description:  This method is used to set the given converter version 
                  in the data page header. On the one hand this indicated
                  the point at which the page was written the last time and
                  on the other hand it is needed by the component converter to
                  handle the state of the corresponding block in the component
                  free block management( FBM ).
    arguments:    version [in] current converter version
    return value: none
    --------------------------------------------------------------------------*/

    void SetConverterVersion( const Converter_Version version )
    {
        PersistentHeaderPtr()->m_RemainingNodeHeader.m_ConverterVersion = version;
    }

    /*!-------------------------------------------------------------------------
    function:     Verify()
    description:  Verifies consistency of page contents
    arguments:    none
    return:       true if verify successful
    --------------------------------------------------------------------------*/

    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const;

    /*!-------------------------------------------------------------------------
    function:     GetLevel()
    description:  This method returns the level of the b*tree the page belongs to.
    arguments:    none
    return value: Data_Level
    --------------------------------------------------------------------------*/

    Data_Level GetLevel() const
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );

        return( static_cast< Data_Level >
                ( PersistentHeaderPtr()->m_RemainingNodeHeader.m_Level ));
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Data Base Page I/O management access                      */
    /* -----------------------------------------------------------------------*/

    /// Writes the main characteristica of the page handle to the knldiag
    virtual void AppendContextInfo (
        SAPDB_Char  const * const  filename,
        SAPDB_UInt4 const          linenumber,
        SAPDBErr_MessageList       &errlist,
        SAPDB_Char  const * const  title = 0) const
        {}

protected:

    /*--------------------------------------------------------------------------
    function:     GetBottom()
    description:  Returns the bottom value of the data base page. Bottom is 
                  the first unused byte in the body part of a data page.
    arguments:    none
    return value: (SAPDB_Int4) Bottom
    --------------------------------------------------------------------------*/

    SAPDB_Int4  GetBottom() const
    {
        return( PersistentHeaderPtr()->m_RemainingNodeHeader.m_Bottom );
    }

    /*--------------------------------------------------------------------------
    function:     GetRecordCount()
    description:  Returns the number of records stored in the data base page.
    arguments:    none
    return value: (SAPDB_Int4) RecordCount
    --------------------------------------------------------------------------*/

    SAPDB_Int4  GetRecordCount() const
    {
        return( PersistentHeaderPtr()->m_RemainingNodeHeader.m_RecordCount );
    }

protected:
    /// This returns a pointer to the page frame.
    SAPDB_Byte * FramePtr()
    {
        return m_Frame.GetDataPtr();
    }
    /// This returns a const pointer to the page frame.
    const SAPDB_Byte * FramePtr() const
    {
        return m_Frame.GetDataPtr();
    }
    /// This returns a formatted pointer to the frame.
    tbd_node* NodePtr()
    {
        return REINTERPRET_CAST (tbd_node*, FramePtr());
    }
    /// This returns a formatted const pointer to the frame.
    const tbd_node* NodePtr() const
    {
        return REINTERPRET_CAST (const tbd_node*, FramePtr());
    }
    /// This copies a new node to the current frame.
    void SetNodePtr(tbd_node* pNode)
    {
        m_Frame.Assign (pNode, sizeof(tbd_node));
    }

public:

    /// PageId becomes the id from the page frame.
    void PageId (Data_PageId& PageId) const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        PageId.SetPageNo          (NodePtr()->nd_id());
        PageId.SetPageRecoveryMode( RecoveryMode() );
    }
    /// Give a hint to Release that there had been no changes to the page frame.
    void SetUnChanged()
    {
        m_ReleaseMode = Data_Unchanged;
    }
    /// Give a hint to Release that there had been changes to the page frame.
    void SetChanged()
    {
        m_ReleaseMode = Data_Changed;
    }
    /// Set a hint for the datacache.
    void SetLruChainInfo( const Data_LRUChainInfo   lruChainInfo )
    {
        m_LruChainInfo = lruChainInfo;
    }
    /// return the set release mode
    Data_ReleaseMode ReleaseMode () const
    {
        return m_ReleaseMode;
    }
    /// return the set access mode
    Data_AccessMode AccessMode () const
    {
        return m_AccessMode;
    }
    /// set the access mode
    void SetAccessMode (Data_AccessMode AccessMode)
    {
        m_AccessMode = AccessMode;
    }

protected:

    /*-----------------------------------------------------------------------------
    declaration: m_RecoveryMode
    description: The recovery mode defines the recoverbility as well as the addressing
                 mode, i.e. recoverbility distinguishs between perm or temp and the
                 adressing mode between dynamic or static.
    ------------------------------------------------------------------------------*/

    Data_PageRecoveryMode   m_RecoveryMode;

    /// the access mode
    Data_AccessMode m_AccessMode;
    /// the release mode
    Data_ReleaseMode m_ReleaseMode;
    /// the data cache hint
    Data_LRUChainInfo m_LruChainInfo;
    /// the used data access manager
    Data_PageAccessManager* m_pAccessManager;

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
private:
#endif

    /*--------------------------------------------------------------------------
    declaration: PersistentHeader
    description: Persistent layout of pascal data page node defined in gbd00
    --------------------------------------------------------------------------*/
    // Must do this redefinition to avoid redundancy (compiler reason):

    typedef Kernel_Page72::PersistentHeader     PH1;        // page72 header ( 8 bytes )

    typedef struct {
        SAPDB_Int4         m_Bottom;           // from byte  9 to byte 12
        SAPDB_Int4         m_RecordCount;      // from byte 13 to byte 16
        SAPDB_Int2         m_Level;            // from byte 17 to byte 18
        tsp00_C6           m_1;                // from byte 19 to byte 24
        SAPDB_Int4         m_Right;            // from byte 25 to byte 28
        tsp00_C12          m_2;                // from byte 29 to byte 40
        SAPDB_UInt4        m_ConverterVersion; // from byte 41 to byte 44
        tsp00_C32          m_3;
        PageWriteCount     m_WriteCount;       // from byte 77 to byte 80
    } PH2;        // layout of tbd_node

protected:

    struct PersistentHeader
    {
        PH1     m_KernelPage72Header;
        PH2     m_RemainingNodeHeader;
    };

private:

    /*--------------------------------------------------------------------------
    declaration: DataBasePagePersistentHeader
    description: Compile time size check for persistent header
    --------------------------------------------------------------------------*/

    typedef SAPDB_Persistent
    < PersistentHeader,
    PersistentHeader,
    sizeof( PH1 ) +
    sizeof( PH2 )
    >
    CheckedPersistentHeader;

    /*--------------------------------------------------------------------------
    function:     PersistentHeaderPtr()
    description:  Returns pointer to page header on frame
    arguments:    none
    prototypes:   2
    return value: (PersistentHeader*) header pointer
    --------------------------------------------------------------------------*/

    PersistentHeader* PersistentHeaderPtr()
    {
        return
            REINTERPRET_CAST( PersistentHeader*,
                              ( m_Frame.GetPointer( 0, sizeof( PersistentHeader ))));
    }

    const PersistentHeader* PersistentHeaderPtr() const
    {
        return
            REINTERPRET_CAST( const PersistentHeader*,
                              ( m_Frame.GetPointer( 0, sizeof( PersistentHeader ))));
    }

    /*--------------------------------------------------------------------------
    function:     IsForArchive()
    description:  This method indicates if the data page is a special archive page.
                  If the instance type archive is set archive pages will
                  be stored in data volume of access mode sequential only! 
                  For testing the state a valid page frame is needed.
    arguments:    none
    return value: SAPDB_Bool
    --------------------------------------------------------------------------*/

    SAPDB_Bool IsForArchive() const;

    /*--------------------------------------------------------------------------
    function:     GetRight()
    description:  Returns the value of the right neigbor of the data base page.
    arguments:    none
    return value: Data_PageNo
    --------------------------------------------------------------------------*/

    Data_PageNo  GetRight() const
    {
        return( PersistentHeaderPtr()->m_RemainingNodeHeader.m_Right );
    }

    /*--------------------------------------------------------------------------
    function:     CalculateChecksum()
    description:  This method provides the checksum calculated over the current
                  data base page. Note that it is dangerous to change the
                  algorithmen, because the checksum is stored in the customer
                  data bases and a new checksum algorithmen will cause errors
                  during read the old pages.
    arguments:    none
    return value: Checksum
    --------------------------------------------------------------------------*/

    PageChecksum CalculateChecksum() const;

    void CalculateBodyChecksum(
        SAPDB_UInt4     FirstPos,
        SAPDB_UInt4     LastPos,
        SAPDB_UInt      Step,
        PageChecksum   &Checksum,
        SAPDB_Bool     bCheckPointerlist = false ) const;


    SAPDB_Bool CheckPageForIO() const;

};

/*!*****************************************************************************

   endclass: Data_BasePage

*******************************************************************************/

#endif  /* DATA_BASEPAGE_HPP */
