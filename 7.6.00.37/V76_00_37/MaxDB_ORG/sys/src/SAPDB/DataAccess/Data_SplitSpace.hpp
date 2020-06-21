/*!
  @file           Data_SplitSpace.hpp
  @author         UweH
  @brief          defines class Data_SplitSpace

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
#ifndef Data_SplitSpace_HPP
#define Data_SplitSpace_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"
#include "RunTime/RTE_Message.hpp"

/// initiali allocation unit
#define DATA_SPLIT_SPACE_INIT_PART_COUNT 3
/*!
   @class Data_SplitSpace
   @brief This manages space which is not located in one page.
 */
template <class T> class Data_SplitSpace : public Data_ISplitSpace
{
public:
	/// default constructor
    Data_SplitSpace (SAPDBMem_IRawAllocator& Allocator)
    : m_Parts              (Allocator),
      m_NumPagesAppended (0)
    {}
    /// The destructor deallocates all resources.
    ~Data_SplitSpace()
    {
        SAPDBErr_MessageList errlist;
        if(!Deassign(errlist))
            RTE_Crash(errlist);
        Delete();
    }
    /*!
        @brief  pre allocate for three parts.
        @return false, if the object cannot be initialized.
     */
    bool Initialize ()
    {
        return m_Parts.Initialize(DATA_SPLIT_SPACE_INIT_PART_COUNT);
    }
    /// release all pages and call Delete() for all used parts
    void Delete()
    {
        if ( m_Parts.GetCapacity() > 0 )
            m_Parts.Delete();
    }
    virtual PartNo CurrentPart () const
    {
        return m_CurrentPart;
    }
    virtual void GetPart (PartNo       Part,
                          SAPDB_UInt  &Length,
                          SAPDB_Byte* &Value) const
    {
        if ( Part > m_CurrentPart || ! IsAssigned() )
        {
			// PTS 1118382 UH 2002-10-17
            Value  = 0;
            Length = 0;
        }
        else
        {
            Value  = m_Parts [Part].Value;
            Length = m_Parts [Part].Length;
        }
    }
    virtual SAPDB_UInt Length() const
    {
        if ( ! IsAssigned() )
            return 0;

        SAPDB_UInt Result = 0;

        for (PartNo part = 0; part <= m_CurrentPart; ++part )
            Result += m_Parts[part].Length;

        return Result;
    }
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
    {
        if ( ! IsAssigned() )
        {
            errlist = errlist + Data_Exception(filename,linenumber,DATA_SPLIT_SPACE_INFO_NOT_ASSIGNED,
                                (title!=0?title:"Data_SplitSpace") );
            return;
        }
        errlist = errlist + Data_Exception(filename,linenumber,DATA_SPLIT_SPACE_INFO,
                            (title!=0?title:"Data_SplitSpace"),
                            SAPDB_ToString(m_CurrentPart),
                            SAPDB_ToString(m_NumPagesAppended));
        
        if ( m_PrevPage.IsAssigned() )
        {
            errlist = errlist + Data_Exception(filename,linenumber,DATA_SPLIT_SPACE_PART_INFO,
                                "prevPage",
                                SAPDB_ToString(0),
                                SAPDB_ToString(0));
        }

        for ( PartNo part = 0; part <= m_CurrentPart; ++part )
        {
            const SAPDB_UInt offset = m_Parts[part].Page.IsAssigned()
                                      ? m_Parts[part].Page.GetOffset(m_Parts[part].Value,m_Parts[part].Length)
                                      : SAPDB_MAX_UINT2;
            errlist = errlist + Data_Exception(filename,linenumber,DATA_SPLIT_SPACE_PART_INFO,
                                SAPDB_ToString(part),
                                SAPDB_ToString(m_Parts[part].Length),
                                SAPDB_ToString(offset));
        }
    }
    /*!
	    @return  false, if space begin is not properly aligned
	    @brief   This checks if the split space begins at an aligned position
    */
     virtual bool CheckAlignment () const
     {
         if ( ! IsAssigned() )
             return true;
             
         SAPDB_UInt8 address =
                         #ifdef BIT64
                         reinterpret_cast<SAPDB_UInt8>(m_Parts[0].Value);
                         #else
                         reinterpret_cast<SAPDB_UInt4>(m_Parts[0].Value);
                         #endif
         
         if ( address % T::Alignment() == 0 )
             return true;

         WriteToTrace("SplitSpace is not properly aligned");

         Msg_List errlist;
         AppendContextInfo (__CONTEXT__, errlist, "SplitSpace is not properly aligned");
         RTE_Message(errlist);

         for ( PartNo part = 0; part <= m_CurrentPart; ++part )
             if ( GetPage(part).IsAssigned() )
                 GetPage(part).Dump();

        return false;
    }
    /*!
        @brief       Advances the current part one element forward. In case the part didn't exist,
                     it is allocated by resizing the vector of parts. The page of the part is not accessed, but the
                     access mode is set to the same value as that of the first page of the SplitSpace
        @return      TRUE in case the next part is usable. FALSE in case of an error.
        @ATTENTION   A Resize() is done internally and therefore all pointers
                     to the current SplitSpace can be become invalid after this call !!!
     */
    bool UseNextPart(SAPDBErr_MessageList &errlist)
    {
        SAPDB_UInt newPart;

        if ( m_CurrentPart.IsInvalid() )
            m_CurrentPart = 0;
        else
            ++m_CurrentPart;
	
        newPart = m_CurrentPart;

        if ( newPart >= m_Parts.GetSize() )
        {
            bool setAccessMode = m_Parts.GetSize() > 0;

            if ( ! m_Parts.Resize(m_Parts.GetSize()+DATA_SPLIT_SPACE_INIT_PART_COUNT) )
            {
                // PTS 1117126 UH 2002-08-07 added if-clause
                errlist = Data_Exception(__CONTEXT__,DATA_SPLIT_SPACE_RESIZE,
                                         SAPDB_ToString(m_Parts.GetSize()));
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            if ( setAccessMode )
                SetAccessMode(m_Parts[0].Page.AccessMode());
        }

        SAPDBERR_ASSERT_STATE ( ! m_Parts[m_CurrentPart].Page.IsAssigned() );

        return true;
    }
    /// The current used page is returned as const reference
    const T& GetCurrentPage () const
    {
        return m_Parts[m_CurrentPart].Page;
    }
    /// The current used page is returned as reference
    T& GetCurrentPage ()
    {
        return m_Parts[m_CurrentPart].Page;
    }
    /// The reference to the page of the specified part is returned.
    T& GetPage (PartNo part)
    {
        return m_Parts[part].Page;
    }
    /// The const reference to the page of the specified part is returned.
    const T& GetPage (PartNo part) const
    {
        return m_Parts[part].Page;
    }
    /// A complete part is appended to the part list.
    bool AppendPart (T                    &Page,
                     SAPDB_UInt            Length,
                     SAPDB_Byte           *Value,
                     SAPDBErr_MessageList &errlist)
    {
        if ( ! UseNextPart(errlist) )
            return false;
        T& InternalPage = GetCurrentPage();
        InternalPage.Assign (Page);
        SetPart (Length, Value);
    }
    /// The description of a new space is set to to current part.
    void SetPart (SAPDB_UInt  Length,
                  SAPDB_Byte* Value)
    {
        SAPDBERR_ASSERT_ARGUMENT( m_Parts[m_CurrentPart].Page.ReferenceIsValid (Value) );

        m_Parts [m_CurrentPart].Value  = Value;
		m_Parts [m_CurrentPart].Length = Length;
    }
    /// return true, if first part page is assigned.
    bool IsAssigned() const
    {
        return m_CurrentPart.IsValid();
    }
    /// From the current used part the page is moved to part 0.
    /// If is not assigned, nothing changes.
    bool Reassign(SAPDBErr_MessageList &errlist,
                  bool                  bKeepFirstPart = false)
    {
        if ( ! IsAssigned() || 0 == m_CurrentPart )
            return true;

        SAPDBErr_MessageList auxErrlist;
        bool                 result = true;
        
        if ( bKeepFirstPart )
        {
            for ( PartNo part = 1; part <= m_CurrentPart; ++part )
            {
                if (! m_Parts[part].Page.Deassign(auxErrlist) )
                {
                    result  = false;
                    errlist = errlist + auxErrlist;
                }
            }
        }
        else
        {
            if( m_Parts[0].Page.Deassign(errlist) )
            {
                m_Parts[0].Page.Assign (m_Parts[m_CurrentPart].Page);
    
                for ( PartNo part = m_CurrentPart-1; part > 0; --part )
                {
                    if(!m_Parts[part].Page.Deassign(auxErrlist))
                    {
                        result  = false;
                        errlist = errlist + auxErrlist;
                    }
                }
            }
        }
        if ( ! result )
            AppendContextInfo(__CONTEXT__,errlist);
        else
            SAPDBERR_ASSERT_STATE( IsAssigned() );
        m_CurrentPart      = 0;
        m_NumPagesAppended = 0;
        return result;
    }
    /*!
        @brief All parts are released, if ok, then release and update pages.
        @param pageHasBeenChanged [in] default is true
     */
    bool Deassign (      SAPDBErr_MessageList &errlist,
                   const bool                  pageHasBeenChanged = true)
    {
        if ( ! IsAssigned() )
            return true;
        
        bool result = true;
        
        // PTS 1133123 UH 2004-12-22 completely rewritten

        // cleanup made changes to: NextPageNO, FirstFreeOffset and remove pages which were appended
        
        for ( PartNo part = m_CurrentPart; part >= 0; --part )
        {
            if ( ! pageHasBeenChanged
                 &&
                 m_Parts[part].Page.AccessMode() != Data_ForRead )
            {            
                if ( m_NumPagesAppended > 0 )
                {
                    // first remove appended new pages
                    --m_NumPagesAppended;
                    if ( part > 0 )
                        result = m_Parts[part].Page.Remove(m_Parts[part-1].Page, errlist);
                    else
                        if ( part == 0 )
                        {
                            if ( m_PrevPage.IsAssigned() )
                                result = m_Parts[part].Page.Remove(m_PrevPage, errlist);
                            else
                            {
                                result  = false;
                                errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "too many pages to remove");
                            }
                        }
                        else
                        {
                            result  = false;
                            errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "too many pages to remove");
                        }
                }
                else
                    if ( part == 0 )
                    {
                        m_Parts[0].Page.SetNextPageNo(Data_PageNo());
                        m_Parts[0].Page.UndoLastReservedSpace(m_Parts[0].Length);
                    }
                    else
                    {
                        result  = false;
                        errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "too few pages to remove");
                    }
            }
            result &= m_Parts[part].Page.Deassign(errlist, pageHasBeenChanged);
        }

        if ( ! result )
            AppendContextInfo(__CONTEXT__,errlist, "Deassign pages in split space error");

        if ( m_PrevPage.IsAssigned() )
            result &= m_PrevPage.Deassign(errlist, pageHasBeenChanged);
        
        m_CurrentPart.Invalidate();
        m_NumPagesAppended = 0;

        return result;
    }
    /// If IsAssigned , all pages are set to AccessMode.
    void SetAccessMode (Data_AccessMode AccessMode)
    {
        if ( m_Parts.GetSize() == 0 )
            (void) Initialize();
            
        for ( SAPDB_UInt i = 0; i < m_Parts.GetSize(); ++i )
			// PTS 1117126 UH 2002-08-07
            m_Parts[i].Page.SetAccessMode (AccessMode);
        // m_PrevPage is not set
    }
    /// returns true, if all pages have the same given access mode.
    bool CheckAccessMode (Data_AccessMode AccessMode) const
    {
        bool result = true;
        for ( SAPDB_UInt i = 0; i < m_Parts.GetSize(); ++i )
            result &= (m_Parts[i].Page.AccessMode() == AccessMode);
        // m_PrevPage is not checked
        return result;
    }
    /// max part is moved to 0 and vice versa
    void Reorder()
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );

        Part AuxPart;
        
        for ( PartNo src = 0, dst = m_CurrentPart;
              src < dst;
              ++src, --dst )
        {
            AuxPart.Page.Assign (m_Parts[src].Page);
            AuxPart.Length = m_Parts[src].Length;
            AuxPart.Value  = m_Parts[src].Value;
            
            m_Parts[src].Page.Assign (m_Parts[dst].Page);
            m_Parts[src].Length = m_Parts[dst].Length;
            m_Parts[src].Value  = m_Parts[dst].Value;
            
            m_Parts[dst].Page.Assign (AuxPart.Page);
            m_Parts[dst].Length = AuxPart.Length;
            m_Parts[dst].Value  = AuxPart.Value;
        }
        // m_PrevPage is not handled here
    }
    /// write important data to knltrace.
    void WriteToTrace (const char * Title) const
    {
        if ( ! IsAssigned() )
        {
            Kernel_VTrace() << Title << ": not assigned.";
            return;
        }
        Kernel_VTrace() << Title;
        for ( PartNo part = 0; part <= m_CurrentPart; ++part )
            if ( m_Parts[part].Page.IsAssigned() )
                Kernel_VTrace() << "part(" << part << ") pno:" << m_Parts[part].Page.PageNo()
                                << ", offset: " << m_Parts[part].Page.GetOffset (m_Parts[part].Value,m_Parts[part].Length,false)
                                << ", length: " << m_Parts[part].Length
                                << ", numPagesAppended: " << m_NumPagesAppended;
            else
                Kernel_VTrace() << "part(" << part << ") pno: page not assigned"
                                << ", offset: page not assigned"
                                << ", length: " << m_Parts[part].Length
                                << ", numPagesAppended: " << m_NumPagesAppended;
        if ( m_PrevPage.IsAssigned() ) // PTS 1133123 UH 2004-12-22
            Kernel_VTrace() << "prevPage pno: " << m_PrevPage.PageNo();
        else
            Kernel_VTrace() << "prevPage not assigned";
    }
    /// Increments the internal counter which counts new allocated pages.
    void SetNewPageWasAppended()
    {
        ++m_NumPagesAppended;
    }
    /// Returns true if the internal counter for new allocated pages is greater than 0.
    bool NewPageWasAppended()
    {
        return m_NumPagesAppended > 0;
    }
    /// return the number of pages added during the last write operation to the split space.
    SAPDB_UInt GetNumAdditionalPagesAppended()
    {
        SAPDB_UInt parts = m_CurrentPart.IsValid() ? m_CurrentPart+1 : 0;
        return parts - m_NumPagesAppended;
    }
    /// returns the prev page handler which can be used during reservation
    T& GetPrevPage()
    {
        // PTS 1133123 UH 2004-12-22 new
        return m_PrevPage;
    }
    /// returns the first pageno (prev or first of split space)
    Data_PageNo GetFirstPageNo() const
    {
        // PTS 1133123 UH 2004-12-22 new
        if ( IsAssigned() )
            return m_PrevPage.IsAssigned() ? m_PrevPage.PageNo() : GetPage(0).PageNo();
        else
            return Data_PageNo();
    }

private:
    /// Consists of the Page and the Length, Value within this Page
    class Part
    {
    public:
        /// the page handler
        T           Page;
        /// length of this part
        SAPDB_UInt  Length;
        /// pointer to this parts value
        SAPDB_Byte* Value;
        // PTS 1130666 UH 2004-07-20
        // changed Part from struct to class
        // added a default constructor so that newarraw() can call it
        Part() : Page(), Length(0), Value(0)
        {}
    };
    /// the currently used part
    PartNo m_CurrentPart;
    /// the list parts
    Container_Vector<Part> m_Parts;
    /// number of added pages during last write
    SAPDB_UInt m_NumPagesAppended; // PTS 1115979 UH 2002-05-29 changed from bool to int
    /*!
    @brief This is needed if the SplitSpace is used to append space to an existing page chain.
    m_PrevPage is then the last page in the chain before the append operation,
    but there is not enough space on this page and the split space begins at the next (new) page.
    The original last last must then be hold in m_PrevPage because otherwise there is no synchronization
    with the savepoint.
    */
    T m_PrevPage; // PTS 1133123 UH 2004-12-22 added
};
#endif // Data_SplitSpace_HPP
