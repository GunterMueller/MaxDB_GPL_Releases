/*!
  @file           Converter_IndexPage.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Handler class for converter index pages
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


#ifndef CONVERTER_INDEXPAGE_HPP
#define CONVERTER_INDEXPAGE_HPP

#include "Converter/Converter_Page.hpp"


/*!
   @interface      Converter_IndexPage
   @brief          Specialized converter page class for index pages (concrete class)

   Call static method 'SetDevNoBitCount()' once prior to first usage
   of this class.
 */
class Converter_IndexPage : public Converter_Page
{
public:

    /* -----------------------------------------------------------------------*/

    /*! @name Constructors and initialization */

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_IndexPage();

    /*!
       @brief          Constructor; assigns frame
       @param          frame [in] page frame
       @return         none

                     Does not change frame contents.
     */
    Converter_IndexPage
        (const PageFrame&               frame);

    /*!
       @brief          Constructor; assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @return         none
     */
    Converter_IndexPage
        (const PageFrame&               frame,
         const Version&                 version,
         const Converter_ParentReference&         parent);

    /*!
       @brief          Assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @return         none
     */
    void SetAndInitializeFrame
        (const PageFrame&               frame,
         const Version&                 version,
         const Converter_ParentReference&         parent);

    /*!
       @brief          Initializes frame contents
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @return         none
     */
    void InitializeFrame
        (const Version&                 version,
         const Converter_ParentReference&         parent);

    /* -----------------------------------------------------------------------*/

    /*! @name Frame access */

    /*!
       @brief          Returns total count of entries on converter index page.
       @return         (Converter_PageEntryCount) entry count
     */
    virtual PageEntryCount GetEntryCount() const;

    /* -----------------------------------------------------------------------*/


    /*!
        @brief  Checks page after reading from volume
        @return (SAPDB_Bool) true if check successful
    */
    virtual SAPDB_Bool CheckAfterRead() const;

protected:

    /* -----------------------------------------------------------------------*/

    /*! @name Protected global functions */

    /*!
       @brief          Initializes all entry flags on the page with 0's.
       @return         none
     */
    virtual void InitializeFlagInfo();

    /* -----------------------------------------------------------------------*/
private:

    /*!
    @brief         Returns offset on page where entire in-use flags block begins
    @return        (OffsetType) offset
    */
    virtual OffsetType InUseFlagsOffset() const;

    /*!
      @brief   Returns total count of entries on converter index page.
      @return  (PageEntryCount) entry count

                  - Preconditions: as = 4 or 8, hs = multiple of 4
                  - usablesize (us) = pagesize(ps) - headersize(hs) - trailersize(ts)
                  - entrysize(es)   = addrsize(as) + flagsize(fs) (no off-cut)
                  - as              = 4 bytes
                  - fs              = 1/8 (1 bit)
                  - es              = as + 1/8 = 33/8
    */
    PageEntryCount CalcEntryCount() const;

};


//------------------------------------------------------------------------------

inline Converter_IndexPage::Converter_IndexPage()
{}

//------------------------------------------------------------------------------

inline Converter_IndexPage::Converter_IndexPage
    (const PageFrame&               frame)
    :
     Converter_Page(frame)
{}

//------------------------------------------------------------------------------

inline Converter_IndexPage::Converter_IndexPage
    (const PageFrame&               frame,
     const Version&                 version,
     const Converter_ParentReference&         parent)
    :
     Converter_Page(frame)
{
    InitializeFrame(version, parent);
}

//------------------------------------------------------------------------------

inline void
    Converter_IndexPage::SetAndInitializeFrame
    (const PageFrame&               frame,
     const Version&                 version,
     const Converter_ParentReference&         parent)
{
    SetFrame(frame);
    InitializeFrame(version, parent);
}

//------------------------------------------------------------------------------

inline void
    Converter_IndexPage::InitializeFrame
    (const Version&                 version,
     const Converter_ParentReference&         parent)
{
    Converter_Page::InitializeFrame(version, parent);
    WriteConverterPageType(Converter_IndexPageType);
    #ifdef SAPDB_QUICK
    const bool           dummyIsAdmin = true;
    SAPDBErr_MessageList errlist;
    SAPDBERR_ASSERT_ARGUMENT ( Verify(dummyIsAdmin,errlist) );
    #endif
}

//------------------------------------------------------------------------------

inline Converter_IndexPage::PageEntryCount
    Converter_IndexPage::GetEntryCount() const
{
    static PageEntryCount count = CalcEntryCount();
    return count;
}

//------------------------------------------------------------------------------

inline void
    Converter_IndexPage::InitializeFlagInfo()
{

    Converter_FlagInfo::FlagInfoData* flaginfoptr    = GetFlagInfo(InUseFlagsOffset(), 0).FlagInfoDataPtr();
    Converter_FlagInfo::FlagInfoData* flaginfoendptr = GetFlagInfo(InUseFlagsOffset(), GetEntryCount() -1).FlagInfoDataPtr();

    while (flaginfoptr <= flaginfoendptr)
    {
        *flaginfoptr = 0;
        ++flaginfoptr;
    }
}
//------------------------------------------------------------------------------

inline Converter_IndexPage::OffsetType
    Converter_IndexPage::InUseFlagsOffset() const
{
    static OffsetType offs
           = sizeof(PersistentHeader) + GetEntryCount() * sizeof(PersistentEntry);
    return offs;
}

//------------------------------------------------------------------------------


#endif // CONVERTER_INDEXPAGE_HPP
