/*!
  @file           Data_Page.hpp
  @author         UweH
  @brief          defines class Data_Page

\if EMIT_LICENCE
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

\endif
*/
#ifndef DATA_PAGE_HPP
#define DATA_PAGE_HPP
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "gbd00.h" // tbd_node
/*!
   @class Data_Page
   @brief Access is offered to a basic database page.and can be accessed by the datacache.
 */
class Data_Page : public Data_BasePage
{
public:
    /// default ctor
    Data_Page ()
    {}
    /*!
        @brief copy constructor
        @param page [in/out] 
        This is used by container_vector, which is used by Data_SplitSpace.
        Assign() is used internally.
     */
    Data_Page (Data_Page& page)
    {
        Assign(page);
    }
    /// The default destructor calls Deassign() to release whatever is still allocated.
    virtual ~Data_Page();

    /// Assign root page, without making the page access in the data cache
    /// Note that the root page could not be released by the 'page.Release'
    /// method, because no PAM is available.
    void AssignRoot( tbd_node_ptrs &pNodes )
    {
        m_AccessMode     = Data_ForRead;
        m_ReleaseMode    = Data_Unchanged;
        m_LruChainInfo   = Data_LruNormal;
        m_pAccessManager = NULL;

        SetNodePtr( pNodes.np_ptr());
        SetHint( pNodes.np_cbptr());
        SAPDBERR_ASSERT_STATE( PageNo() == RootPageNo());
    }

    /// Release root page, which was not accessed by PAM interface
    void DiscardRoot()
    {
        SAPDBERR_ASSERT_STATE( NULL == m_pAccessManager );
        SAPDBERR_ASSERT_STATE( PageNo() == RootPageNo());
        SetHint (NULL);
        ReleaseFrame();
    }

    /*!
        @brief OldPage moves to current page.

        OldPage must be assigned. This-> must not be assigned.
        After operation the assertions changed with each other.
     */
    virtual void Assign (Data_Page& OldPage)
    {
        SAPDBERR_ASSERT_STATE(  OldPage.IsAssigned());
        SAPDBERR_ASSERT_STATE(! IsAssigned()        );

        m_AccessMode     = OldPage.m_AccessMode;
        m_ReleaseMode    = OldPage.m_ReleaseMode;
        m_LruChainInfo   = OldPage.m_LruChainInfo;
        m_pAccessManager = OldPage.m_pAccessManager;

        SetRecoveryMode(OldPage.RecoveryMode());
        SetHint (OldPage.GetHint());
        SetFrame(OldPage.GetFrame());

        OldPage.m_pAccessManager = NULL;
        OldPage.SetHint (NULL);
        OldPage.ReleaseFrame();

        SAPDBERR_ASSERT_STATE(! OldPage.IsAssigned());
        SAPDBERR_ASSERT_STATE(  IsAssigned()        );
    }
    /// Both page handler swap their members.
    virtual void Swap (Data_Page& Other)
    {
        Data_Page AuxPage;
        AuxPage.Assign (Other);
        Other.Assign   (*this);
        Assign         (AuxPage);
    }
    /*!
        @brief Both page handler point to the same page.

        A cloned page must be destroyed with DeClone()
        A cloned page has no PAM, so a DeAssign will not release it.
        Instead of it saves (!)
     */
    virtual void Clone (Data_Page& Other)
    {
        SAPDBERR_ASSERT_STATE(  Other.IsAssigned() );
        SAPDBERR_ASSERT_STATE(! IsAssigned()       );

        m_AccessMode     = Other.m_AccessMode;
        m_ReleaseMode    = Other.m_ReleaseMode;
        m_LruChainInfo   = Other.m_LruChainInfo;
        m_pAccessManager = Other.m_pAccessManager;

        SetRecoveryMode( Other.RecoveryMode() );
        SetFrame       ( Other.GetFrame()     );
        SetHint        ( NULL                 ); // this indicates no action on release

        SAPDBERR_ASSERT_STATE( Other.PageNo() == PageNo() );
        SAPDBERR_ASSERT_STATE( Other.IsAssigned()         );
        SAPDBERR_ASSERT_STATE( IsAssigned()               );
    }
    /// Writes the main characteristica of the page handle to the knltrace
    virtual void WriteToTrace (const SAPDB_Char *title = NULL) const;
	/// Writes the main characteristica of the page handle to the knldiag
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
	/// returns true if page frame is assigned and consistent
    virtual bool Verify (bool                  isAdmin,
                         SAPDBErr_MessageList &errlist) const;
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
    /*!
        @brief The referenced page frame is released in the cache.
        @param errlist [out]
        @param pageHasBeenChanged [in] default true. When set to false, a corresponding hint is set.
        @return false if any error occured.
        If isOK is true, then release for update, else release without
        changes become persistent. Precondition: NOT IsAssigned
     */
    bool Deassign (      SAPDBErr_MessageList &errlist,
                   const bool                  pageHasBeenChanged = true)
    {
        if ( IsAssigned()
             &&
             m_pAccessManager != NULL
             &&
             GetHint() != NULL )
        {
            if ( ! pageHasBeenChanged )
                SetUnChanged();
            return m_pAccessManager->ReleasePage (*this, errlist);
        }
        
        m_pAccessManager = NULL;
        SetHint (NULL);
        ReleaseFrame();
        return true;
    }
    /// The root page no is returned.
    Data_PageNo RootPageNo () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return STATIC_CAST(Data_PageNo,NodePtr()->nd_root());
    }
    /// The frames page no is returned.
    Data_PageNo PageNo () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());

        return STATIC_CAST(Data_PageNo, NodePtr()->nd_id());
    }
    /// The converter version of the frame is returned
    SAPDB_Int4 PageConverterVersion () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());

        return STATIC_CAST(Data_PageNo, NodePtr()->nd_conv_version());
    }
    /// The frames next page no is returned.
    Data_PageNo Successor () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());

        return STATIC_CAST(Data_PageNo, NodePtr()->nd_right());
    }
    /// The root page no is set with in the frame.
    void SetRootPageNo (const Data_PageNo& RootPageNo)
    {
        SAPDBERR_ASSERT_STATE( IsAssigned()                            );
        SAPDBERR_ASSERT_STATE( Data_ForUpdate          == m_AccessMode ||
                               Data_ForStructureChange == m_AccessMode );

        NodePtr()->nd_root() = (tsp00_PageNo)RootPageNo;
    }
    /*!
        @brief  The given pointer is checked if it points into the frame.
        @return true, if the reference points within the page frame.
     */
    bool ReferenceIsValid (SAPDB_Byte* Reference) const
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );

        return Reference >= FramePtr()
               &&
               Reference <= FramePtr() + FrameSize();
    }
    /*!
        @brief  The offset in the page is determinded from the given pointer.
        @return offset in page
        If the reference is not valid mac-unit4 is returned.
     */
    Data_PageOffset GetOffset (SAPDB_Byte* Reference) const
    {
        if ( ReferenceIsValid(Reference) )
            return Data_PageOffset(Reference - FramePtr());
        else
            return SAPDB_MAX_UINT4;
    }
    /// returns true if nothing was written into the page.
    bool IsEmpty ()
    {
        return NodePtr()->nd_bottom() == sizeof (tbd_node_header)+1;
    }
    /// The size of freely usable space is returned.
    Data_RecordLength FreeSpace ()
    {
        // This method uses nd_bottom to calculate the size.
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        return MaxSpaceOffset() - FirstFreeOffset(FramePtr());
    }
    /// The methods returns the occupancy in bytes of the page
    Data_RecordLength SpaceUsed () const
    {
        // This method uses nd_bottom to calculate the occupancy.
        SAPDBERR_ASSERT_STATE( IsAssigned() );

        return( NodePtr()->nd_bottom() - BODY_BEG_BD00 );
    }
    /*!
        @brief Reserve the given Size amount of space in the page body.
        
        PRE: page is assigned
        PRE: AccessMode is Data_ForUpdate or Data_ForStructureChange
        PRE: Size is less or equal to the free space size.
        This method uses nd_bottom to reserve the size.
     */

    SAPDB_Byte* ReserveSpace (Data_RecordLength Size)
    {
        SAPDBERR_ASSERT_STATE    ( IsAssigned()                               );
        SAPDBERR_ASSERT_STATE    ( Data_ForUpdate          == m_AccessMode || \
                                   Data_ForStructureChange == m_AccessMode );
        SAPDBERR_ASSERT_ARGUMENT ( Size <= FreeSpace()                        );

        NodePtr()->nd_bottom() += Size;
        return FramePtr() + (NodePtr()->nd_bottom()-1) - Size;
    }
    /*!
        @param offset [in] 
        @brief The first free offset in the page is set to a new value.

        PRE: page is assigned
        PRE: AccessMode is Data_ForUpdate or Data_ForStructureChange
        This method uses nd_bottom.
     */
    void SetFirstFreeOffset (Data_PageOffset offset)
    {
        SAPDBERR_ASSERT_STATE    ( IsAssigned()                            );
        SAPDBERR_ASSERT_STATE    ( Data_ForUpdate          == m_AccessMode || \
                                   Data_ForStructureChange == m_AccessMode );

        NodePtr()->nd_bottom() = offset+1; // this is a position not an offset
    }
    /// return the size in bytes of a frame
    static SAPDB_UInt FrameSize ()
    {
        return sizeof (tbd_node);
    }
    /// Returns the maximum of usable space in this page.
    static SAPDB_UInt MaxSpaceSize ()
    {
        // PTS 1121659 UH 2003-04-30 new
        return   sizeof (tbd_node) /*really dirty, but "m_Frame.Capacity()" cannot be used now */
               - sizeof (tbd_node_header)
               - sizeof (PersistentChecksumTrailer);
    }
    /// Returns the offset, of the first free usable byte of space.
    static Data_PageOffset FirstFreeOffset (const SAPDB_Byte* pPage)
    {
        const SAPDB_Int4 &bottom = (reinterpret_cast <const tbd_node*>(pPage))->nd_bottom();
        SAPDBERR_ASSERT_STATE ( bottom >= 1 );
        return static_cast <Data_PageOffset> (bottom-1);
    }
    /// The minimum offset from page begin, which can be used as free space.
    static Data_PageOffset MinSpaceOffset ()
    {
        return sizeof (tbd_node_header);
    }
    /// The maximum offset from page begin, which can be used as free space.
    static Data_PageOffset MaxSpaceOffset ()
    {
        return sizeof(tbd_node) /*really dirty, but "m_Frame.Capacity()" cannot be used now */
               - sizeof(PersistentChecksumTrailer);
    }
	/// dumps the current page as a file "p<pagenumber>.cor" into the rundirectory 
    void Dump() const;

private:
    /// FORBIDDEN TO USE BECAUSE THERE SHOULD ONLY EXIST ONE HANDLE
    Data_Page& operator = (Data_Page& old)
    {
        // FORBIDDEN TO USE BECAUSE THERE SHOULD ONLY EXIST ONE HANDLE
        // FOR ONE DATA PAGE IN DATA CACHE
        // EXCEPTION: Clone() for Lock()
        return *this;
    }
};
#endif // DATA_PAGE_HPP
