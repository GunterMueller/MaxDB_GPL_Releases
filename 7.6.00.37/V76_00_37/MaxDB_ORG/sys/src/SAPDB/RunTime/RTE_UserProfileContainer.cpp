/*!========================================================================

  @file         RTE_UserProfileContainer.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        This container class is used to handle the storage of
                user related profile data.

                Profile data has arbitrary length, but can
                be identified by a userid/SERVERDB combination.
                Each profile consists of length + bytestream.
                The profile data is only accessable via this interface.
                This is acting not on a ASCII-file....
                SERVERDB is used to identify a container file on per
                database base.

  @since        2005-05-30  11:03
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stddef.h>

#if defined(_WIN32)
# include <crtdbg.h>
#else /* UNIX */
//# include "geo47.h"
# include <sys/types.h>
# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <assert.h>
# include <dirent.h>
# include <sys/stat.h>
#endif

#include "gsp01.h"
#include "heo01.h"
#include "geo49.h"   // Security specific stuff
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "RunTime/RTE_UserProfileContainer.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define CONTAINER_PAGE_SIZE         512U
#define CONTAINER_VERSION           1     

#if defined(_WIN32)
#else /* UNIX */
# if !defined _tcscat
#  define _tcscat  strcat
# endif
#endif

// --- Use the following define for nested private structs or unions declared in classes.
//     It's a workaround for the AIX scope error "private member ... cannot be accessed."
#if defined (_WIN32)    
# define  PRIVATE_NESTED_STRUCTS protected
#else
# define  PRIVATE_NESTED_STRUCTS public
#endif



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define MIN_LEN(_val1, _val2)       ((_val1) < (_val2)) ? (_val1) : (_val2)
#define OFFSET_OFF(_strct,_elem)    ((size_t)&(((_strct *)0)->_elem))


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

//-----------------------------------------------------------------------------
//------------------------- class CommonContainer -----------------------------
//-----------------------------------------------------------------------------
//
//
//  Container contents:
//
//          +==========================+
//          | <HeaderPage>             |   Comment: if there is no next page, 'nextPageNo' is zero.
//          |--------------------------|            if 'firstUnusedPageNo' is zero, add a new page
//      +---|    firstUnusedPageNo     |            to the end of the container.
//   +--|---|    lastUnusedPageNo      |       
//   |  |   |    ----------------------|
//   |  |   |    firstDirPageNo        |------+
//   |  |   +==========================+      |
//   |  |   | <DirectoryPage>          |<-----+
//   |  |   |--------------------------|
//   |  |   |    nextPageNo            |->0 
//   |  |   |    ----------------------|    
//   |  |   |    1 .FirstProfilePage   |-------+   
//   |  |   |       LastProfilePage    |-------|--+
//   |  |   |       -------------------|       |  |
//   |  |   |         .          .     |       |  |
//   |  |   |         .          .     |       |  |
//   |  |   |       -------------------|       |  |
//   |  |   |    n. FirstProfilePage   |-------|--|--+ 
//   |  |   |       LastProfilePage    |-------|--|--+ 
//   |  |   +==========================+       |  |  |
//   |  +-->| <Unused Page>            |       |  |  |
//   |      |--------------------------|       |  |  |
//   |  +---|    nextPageNo            |       |  |  |
//   |  |   +==========================+       |  |  |
//   |  |   .                          .       |  |  |
//   |  |   +==========================+       |  |  |
//   |  |   | <ProfileDataPage>        |<------+  |  |
//   |  |   |--------------------------|          |  |
//   |  |   |    nextPageNo            |----+     |  |
//   |  |   +==========================+    |     |  |
//   |  |   .                          .    |     |  |
//   |  |   +==========================+    |     |  |
//   +--+-->| <Unused Page>            |    |     |  |
//          |--------------------------|    |     |  |
//          |    nextPageNo            |->0 |     |  |
//          +==========================+    |     |  |
//          .                          .    |     |  |
//          +==========================+    |     |  |
//          | <ProfileDataPage>        |<---+-----+  |
//          |--------------------------|             |
//          |    nextPageNo            |->0          |
//          +==========================+             |
//          .                          .             |
//          +==========================+             |
//          | <ProfileDataPage>        |<------------+
//          |--------------------------|       
//          |    nextPageNo            |->0    
//          +==========================+
//
class CommonContainer
{
PRIVATE_NESTED_STRUCTS:
    typedef SAPDB_UInt4   PageNoType;


    //-------------------------------------------------
    //---------------- enum PageType ------------------
    //-------------------------------------------------

    typedef enum 
    {
        HeaderPage, 
        DirectoryPage,
        ProfileDataPage,
        UnusedPage
    } PageType;


    //-------------------------------------------------
    //--------------- struct PageHeader -----------------
    //-------------------------------------------------

    struct PageHeader
    {
        PageType            pageType;         // - must be the first page entry!!!!
        PageNoType          currPageNo;
        SAPDB_UInt4         containerVersion;

        // --- unused pages
        PageNoType          firstUnusedPageNo;
        PageNoType          lastUnusedPageNo;

        // --- directory pages
        SAPDB_UInt4         numOfDirEntries;
        SAPDB_UInt4         numOfDirPages;

        PageNoType          firstDirPageNo;
        PageNoType          lastPageNo;
    };


    //-------------------------------------------------
    //------------- struct ProfileData ----------------
    //-------------------------------------------------

    struct ProfileData
    {
        PageType            pageType;         // - must be the first page entry!!!!
        PageNoType          currPageNo;
        PageNoType          nextPageNo;

        SAPDB_UInt4         dataLen;
        char                data[1];
    };

#   define MAX_PROFILE_DATA_PER_PAGE (SAPDB_Int4)(CONTAINER_PAGE_SIZE  -  \
                                                  OFFSET_OFF(ProfileData, data[0]))

    //-------------------------------------------------
    //------------- struct ProfleDirectory ----------------
    //-------------------------------------------------
    struct ProfleDirectory
    {
        struct DirEntry
        {
            tsp00_C18c        userName;
            SAPDB_Int4        profileLength;
            SAPDB_UInt4       numOfProfileDataPages;
            PageNoType        firstProfileDataPage;
            PageNoType        lastProfileDataPage;
        };


        PageType            pageType;         // - must be the first page entry!!!!
        PageNoType          currPageNo;
        PageNoType          nextPageNo;
        DirEntry            entries[1];
    };

#   define ENTRIES_PER_DIRPAGE   ((CONTAINER_PAGE_SIZE  - OFFSET_OFF(ProfleDirectory, entries[0])) \
                                   / sizeof(ProfleDirectory::DirEntry))

    /*!
     union describing the PageEntry
     */

    typedef union
    {
        /*! common first part of every union member */
        struct {                       // - must be the first page entry!!!!   
            PageType          pageType;
            PageNoType        currPageNo;
            PageNoType        nextPageNo;
        } Common;

        PageHeader          header;
        ProfleDirectory     dir;
        ProfileData         profileData;
        unsigned char       binary[CONTAINER_PAGE_SIZE];
    } PageEntry;


public:
    bool GetProfile             ( tsp00_C18c const        userName,               // - IN
                                  SAPDB_Int4              &length,                // - IN
                                  void * const            pProfile,               // - OUT
                                  Msg_List    &messageList );           
    bool SetProfile             ( tsp00_C18c const        userName,               // - IN
                                  SAPDB_Int4              length,                 // - IN
                                  void * const            pProfile,               // - IN
                                  Msg_List    &messageList );
    bool ScanProfiles           ( SAPDB_UInt4             &token,                 // - IN/OUT
                                  SAPDB_Int4              &length,                // - IN/OUT
                                  void * const            pProfile,               // - OUT
                                  tsp00_C18c              &userName,              // - OUT
                                  Msg_List    &messageList );
    bool DelProfile             ( tsp00_C18c const        userName,               // - IN
                                  Msg_List    &messageList );
protected:
    CommonContainer();
    ~CommonContainer();

    bool Initialize             ( bool                    newContainer,           // - IN
                                  Msg_List    &messageList );

    /*!
      @brief interface to load a page
     */
    virtual bool vLoadPage      ( PageNoType const        pageNo,                 // - IN
                                  PageEntry               &page,                  // - OUT
                                  Msg_List    &messageList ) = 0;
    /*!
      @brief interface to save a page
     */
    virtual bool vSavePage      ( PageNoType const        pageNo,                 // - IN 
                                  PageEntry const         page,                   // - IN
                                  Msg_List    &messageList ) = 0;

private:
    void InitHeaderPage         ();
    bool LoadHeaderPage         ( Msg_List    &messageList );
    bool SaveHeaderPage         ( Msg_List    &messageList ) 
         { 
             return vSavePage( 0, m_HeaderPage, messageList ); 
         };

    bool LoadDirectoryPages     ( Msg_List    &messageList );
    bool SaveDirectoryPage      ( SAPDB_UInt4 const       dirPage,                // - IN
                                  Msg_List    &messageList );
    bool NewDirectoryPage       ( Msg_List    &messageList );
    bool FindDirEntry           ( tsp00_C18c const        userName,               // - IN
                                  SAPDB_UInt4             &dirEntry );            // - OUT

    void GetDirEntryValues      ( SAPDB_UInt4 const       dirEntry,               // - IN
                                  tsp00_C18c *            pUserName,              // - OUT
                                  SAPDB_Int4 *            pProfileLength,         // - OUT
                                  SAPDB_UInt4 *           pNumOfProfileDataPages, // - OUT
                                  PageNoType *            pFirstProfileDataPage,  // - OUT
                                  PageNoType *            pLastProfileDataPage ); // - OUT

    bool SetDirEntryValues      ( tsp00_C18c const        userName,               // - IN
                                  SAPDB_Int4 const        profileLength,          // - IN
                                  SAPDB_UInt4 const       numOfProfileDataPages,  // - IN
                                  PageNoType const        firstProfileDataPage,   // - IN
                                  PageNoType const        lastProfileDataPage,    // - IN
                                  SAPDB_UInt4             &dirEntry,              // - IN/OUT
                                  Msg_List    &messageList );

    bool ReleaseDirEntry        ( SAPDB_UInt4 const       dirEntry,               // - IN
                                  Msg_List    &messageList );

    bool GetNextProfileDataPage ( PageNoType const        pageNo,                 // - IN
                                  PageEntry               &page,                  // - OUT
                                  Msg_List    &messageList );

    bool GetNextUnusedPage      ( PageNoType              &pageNo,                // - OUT
                                  PageEntry               page,                   // - OUT
                                  Msg_List    &messageList );

    bool ReleaseProfileData     ( PageNoType const        firstPageNo,            // - IN
                                  PageNoType const        lastPageNo,             // - IN
                                  Msg_List    &messageList );

    bool SetProfileData         ( SAPDB_Int4 const        length,                 // - IN
                                  void const * const      pProfile,               // - IN
                                  PageNoType              &firstPageNo,           // - IN/OUT
                                  PageNoType              &lastPageNo,            // - IN/OUT
                                  SAPDB_UInt4             &numOfPages,            // - OUT
                                  Msg_List    &messageList );

    bool GetProfileData         ( SAPDB_UInt4 const       numOfPages,             // - IN
                                  PageNoType const        firstPageNo,            // - IN
                                  SAPDB_Int4 const        dataLength,             // - IN
                                  SAPDB_Int4              &length,                // - OUT
                                  void * const            pProfile,               // - OUT
                                  Msg_List    &messageList );
private:
    PageEntry                 m_HeaderPage;
    PageHeader *              m_pHeader;

    PageEntry *               m_pDirPages;
    bool                      m_Modified;
};

//-----------------------------------------------------------------------------
//----------------------- class RTE_ProfileContainer --------------------------
//-----------------------------------------------------------------------------

class RTE_ProfileContainer : public CommonContainer
{
public: 
    RTE_ProfileContainer        ( tsp00_DbNamec const     serverDB,
                                  bool                    readOnly,
                                  Msg_List    &messageList );
    ~RTE_ProfileContainer() {;};

    bool OpenContainer          ( Msg_List    &messageList );
    bool CloseContainer         ( Msg_List    &messageList );
    bool DeleteContainer        ( Msg_List    &messageList );

    bool GetOwnerName           ( tsp00_Namec             &username,                  // - OUT
                                  Msg_List    &messageList );
    bool GetGroupName           ( tsp00_Namec             &groupname,                 // - OUT
                                  Msg_List    &messageList );
    bool SetGroupName           ( tsp00_Namec const       &groupname,
                                  Msg_List    &messageList );
    bool GetFileHandle          ( int                     &fileHandle,                // - OUT
                                  Msg_List    &messageList );

private:
    bool vLoadPage              ( PageNoType const        pageNo,                     // - IN
                                  PageEntry               &page,                      // - OUT
                                  Msg_List    &messageList );
    bool vSavePage              ( PageNoType const        pageNo,                     // - IN 
                                  PageEntry const         page,                       // - IN
                                  Msg_List    &messageList );

private:
    bool               m_ContainerIsEmpty;
    tsp00_Pathc        m_FileName;
    bool               m_ReadOnly;
    char *             m_pEndOfPathName;

# if defined(_WIN32)
    HANDLE             m_FileHdl;
# else /* UNIX */
    int                m_FileDesc;
# endif

};

//-----------------------------------------------------------------------------
//---------------------------- class CommonContainer ------------------------------
//-----------------------------------------------------------------------------

CommonContainer::CommonContainer()
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::CommonContainer", RTE_Trace, 9);

    SAPDBERR_ASSERT_STATE(CONTAINER_PAGE_SIZE == sizeof(PageEntry));

    m_Modified  = false;
    m_pDirPages = 0;
    m_pHeader   = &m_HeaderPage.header;

    InitHeaderPage();
}

//-----------------------------------------------------------------------------

CommonContainer::~CommonContainer()
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::~CommonContainer", RTE_Trace, 9);

    if ( 0 != m_pDirPages )
    {
        RTEMem_RteAllocator::Instance().Deallocate(m_pDirPages);
        m_pDirPages = 0;
    }
}

//-----------------------------------------------------------------------------


bool CommonContainer::Initialize ( bool                    newContainer,
                                   Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::Initialize", RTE_Trace, 9);

    if ( m_pDirPages )
    {
        RTEMem_RteAllocator::Instance().Deallocate(m_pDirPages);
    }

    m_Modified  = false;
    m_pDirPages = 0;
    m_pHeader   = &m_HeaderPage.header;

    if ( newContainer == true )
        InitHeaderPage();
    else
    {
        if ( LoadHeaderPage( messageList ) == false )
            return false;

        if ( LoadDirectoryPages( messageList ) == false )
        {
            InitHeaderPage();
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

void CommonContainer::InitHeaderPage()
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::InitHeaderPage", RTE_Trace, 9);

    SAPDB_MemFillNoCheck ( &m_HeaderPage, 0, sizeof(m_HeaderPage) );

    m_pHeader->pageType         = HeaderPage;
    m_pHeader->containerVersion = CONTAINER_VERSION;

    return;
}

//-----------------------------------------------------------------------------

bool CommonContainer::LoadHeaderPage( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::LoadHeaderPage", RTE_Trace, 9);

    // --- Load header page
    if ( vLoadPage( 0, m_HeaderPage, messageList ) == false )
        return false;

    // --- check page type
    if ( m_HeaderPage.Common.pageType != HeaderPage )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_WRNG_CONTAINER_PAGE_TYPE,
                                            SAPDB_ToString(m_HeaderPage.Common.pageType), 
                                            SAPDB_ToString(HeaderPage) );
        return false;
    }

    // --- check page no
    if ( m_HeaderPage.Common.currPageNo != 0 )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_CURR_PAGE,
                                            SAPDB_ToString(m_HeaderPage.Common.currPageNo), 
                                            SAPDB_ToString(0) );
        return false;
    }

    // --- check container version
    if ( m_pHeader->containerVersion > CONTAINER_VERSION )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_WRNG_CONTAINER_VERSION,
                                            SAPDB_ToString(m_pHeader->containerVersion), 
                                            SAPDB_ToString(CONTAINER_VERSION) );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::LoadDirectoryPages( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::LoadDirectoryPages", RTE_Trace, 9);

    SAPDB_UInt4        pageCnt;
    PageNoType         currPageNo;

    if ( m_pHeader->numOfDirPages == 0 )
        return true;

    m_pDirPages = (PageEntry*) RTEMem_RteAllocator::Instance().Allocate(sizeof(PageEntry) * m_pHeader->numOfDirPages);

    if ( m_pDirPages == 0 )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_OUT_OF_RTE_MEMORY  );
        return false;
    }

    // --- Load all directory pages
    for ( pageCnt = 0, currPageNo = m_pHeader->firstDirPageNo; 
          ( pageCnt < m_pHeader->numOfDirPages ) && ( currPageNo != 0 );
          pageCnt++ )
    {
        // --- Load a single directory page
        if ( vLoadPage( currPageNo, m_pDirPages[pageCnt], messageList ) == false )
            return false;

        // --- check page type
        if ( m_pDirPages[pageCnt].Common.pageType != DirectoryPage )
        {
            messageList = Msg_List( RTE_CONTEXT, RTEERR_WRNG_CONTAINER_PAGE_TYPE,
                                                SAPDB_ToString(m_pDirPages[pageCnt].Common.pageType), 
                                                SAPDB_ToString(DirectoryPage) );
            return false;
        }

        // --- check page no
        if ( m_pDirPages[pageCnt].Common.currPageNo != currPageNo )
        {
            messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_CURR_PAGE,
                                                SAPDB_ToString(m_pDirPages[pageCnt].Common.currPageNo), 
                                                SAPDB_ToString(currPageNo) );
            return false;
        }

        currPageNo = m_pDirPages[pageCnt].dir.nextPageNo;
    }

    // --- surprising 'next page' 
    if (( m_pDirPages[pageCnt - 1].Common.nextPageNo != 0 ) || 
        ( pageCnt < m_pHeader->numOfDirPages ))
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_NEXT_PAGE,
                                            SAPDB_ToString(m_pDirPages[pageCnt].Common.nextPageNo) );
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::SaveDirectoryPage( SAPDB_UInt4 const       dirPage,
                                        Msg_List     &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::SaveDirectoryPage", RTE_Trace, 9);

    SAPDBERR_ASSERT_STATE ( m_pDirPages                       != 0 );
    SAPDBERR_ASSERT_STATE ( m_pDirPages[dirPage].dir.pageType == DirectoryPage );

    // --- Save a single directory page
    return vSavePage( m_pDirPages[dirPage].dir.currPageNo, 
                      m_pDirPages[dirPage], messageList );
}

//-----------------------------------------------------------------------------

bool CommonContainer::NewDirectoryPage( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::NewDirectoryPage", RTE_Trace, 9);

    PageEntry *                pDirPages;
    PageNoType                 newDirPageNo;

    pDirPages = (PageEntry*) RTEMem_RteAllocator::Instance().Allocate(sizeof(PageEntry) * (m_pHeader->numOfDirPages + 1));

    if ( pDirPages == 0 )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_OUT_OF_RTE_MEMORY  );
        return false;
    }

    // --- get a new directory 'page no' from end of container
    m_pHeader->lastPageNo++;
    newDirPageNo = m_pHeader->lastPageNo;

    if ( m_pHeader->numOfDirPages == 0 )
        m_pHeader->firstDirPageNo = newDirPageNo;
    else
    {
        // --- copy old pages into the new buffer
        SAPDB_MemCopyNoCheck ( pDirPages, m_pDirPages, sizeof(PageEntry) * m_pHeader->numOfDirPages );

        // --- set 'nextPageNo' of previous page
        pDirPages[m_pHeader->numOfDirPages - 1].dir.nextPageNo = newDirPageNo;   
    }

    // --- initialize the new page!
    SAPDB_MemFillNoCheck ( &pDirPages[m_pHeader->numOfDirPages], 0, sizeof(PageEntry) );
    pDirPages[m_pHeader->numOfDirPages].dir.pageType   = DirectoryPage;
    pDirPages[m_pHeader->numOfDirPages].dir.currPageNo = newDirPageNo;

    RTEMem_RteAllocator::Instance().Deallocate(m_pDirPages);
    m_pDirPages = pDirPages;

    m_pHeader->numOfDirPages++;

    if ( SaveDirectoryPage( m_pHeader->numOfDirPages - 1, messageList ) == false )
        return false;

    if ( m_pHeader->numOfDirPages > 1 )
    {
        if ( SaveDirectoryPage( m_pHeader->numOfDirPages - 2, messageList ) == false ) 
            return false;
    }

    if ( SaveHeaderPage( messageList ) == false )
        return false;


    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::FindDirEntry( tsp00_C18c const       userName,
                                   SAPDB_UInt4            &dirEntry )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::GetDirEntry", RTE_Trace, 9);

    SAPDB_UInt4        pageCnt;
    SAPDB_UInt4        dirPageEntry;

    if ( m_pDirPages == 0 )
        return false;

    for ( pageCnt = 0, dirEntry = 0; 
          ( pageCnt  < m_pHeader->numOfDirPages ) &&
          ( dirEntry < m_pHeader->numOfDirEntries );
          pageCnt++ )
    {
        for ( dirPageEntry = 0; 
              ( dirPageEntry < ENTRIES_PER_DIRPAGE ) &&
              ( dirEntry     < m_pHeader->numOfDirEntries );
              dirPageEntry++, dirEntry++ )
        {
            if ( m_pDirPages[pageCnt].dir.entries[dirPageEntry].userName[0] == _T('\0') )
                continue;

            if ( _tcsicmp ( m_pDirPages[pageCnt].dir.entries[dirPageEntry].userName, userName  ) == 0 )
                return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------

void CommonContainer::GetDirEntryValues( SAPDB_UInt4 const       dirEntry,
                                         tsp00_C18c *            pUserName,
                                         SAPDB_Int4 *            pProfileLength,
                                         SAPDB_UInt4 *           pNumOfProfileDataPages,
                                         PageNoType *            pFirstProfileDataPage,
                                         PageNoType *            pLastProfileDataPage )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::GetDirEntryValues", RTE_Trace, 9);

    SAPDB_UInt4                  dirPage;
    SAPDB_UInt4                  dirPageEntry;
    ProfleDirectory::DirEntry *  pEntry;

    dirPage        = (SAPDB_UInt4)(dirEntry / ENTRIES_PER_DIRPAGE);
    dirPageEntry   = (SAPDB_UInt4)(dirEntry % ENTRIES_PER_DIRPAGE);
    pEntry         = &(m_pDirPages[dirPage].dir.entries[dirPageEntry]);

    if ( pUserName != 0 )
        *pUserName = pEntry->userName;

    if ( pProfileLength != 0 )
        *pProfileLength = pEntry->profileLength;

    if ( pNumOfProfileDataPages != 0 )
        *pNumOfProfileDataPages = pEntry->numOfProfileDataPages;

    if ( pFirstProfileDataPage != 0 )
        *pFirstProfileDataPage = pEntry->firstProfileDataPage;

    if ( pLastProfileDataPage != 0 )
        *pLastProfileDataPage = pEntry->lastProfileDataPage;

    return;
}

//-----------------------------------------------------------------------------

bool CommonContainer::SetDirEntryValues( tsp00_C18c const            userName,
                                         SAPDB_Int4 const            profileLength,
                                         SAPDB_UInt4 const           numOfProfileDataPages,
                                         PageNoType const            firstProfileDataPage,
                                         PageNoType const            lastProfileDataPage,
                                         SAPDB_UInt4                 &dirEntry,
                                         Msg_List        &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::SetDirEntryValues", RTE_Trace, 9);

    SAPDB_UInt4                  dirPage;
    SAPDB_UInt4                  dirPageEntry;
    ProfleDirectory::DirEntry *  pEntry;
    bool                         headerUpdated = false;

    // --- append a new entry ?
    //
    if ( dirEntry == LAST_TOKEN )
    {
        dirEntry = m_pHeader->numOfDirEntries;

        // --- new directory page required?
        if ( m_pHeader->numOfDirPages < ((m_pHeader->numOfDirEntries + 1) / ENTRIES_PER_DIRPAGE) + 1 )
        { 
            if ( NewDirectoryPage( messageList ) == false )
                return false;
        }
        m_pHeader->numOfDirEntries++;
        headerUpdated = true;
    }

    dirPage      = (SAPDB_UInt4)(dirEntry / ENTRIES_PER_DIRPAGE);
    dirPageEntry = (SAPDB_UInt4)(dirEntry % ENTRIES_PER_DIRPAGE);
    pEntry       = &(m_pDirPages[dirPage].dir.entries[dirPageEntry]);

    if (( _tcsncmp ( pEntry->userName, userName, sizeof(userName)-1 )) ||
        ( pEntry->profileLength          != profileLength )            ||
        ( pEntry->numOfProfileDataPages  != numOfProfileDataPages )    ||
        ( pEntry->firstProfileDataPage   != firstProfileDataPage )     ||
        ( pEntry->lastProfileDataPage    != lastProfileDataPage ))        
    {
        m_Modified = true;

        _tcscpy ( pEntry->userName, userName );
        pEntry->profileLength         = profileLength;
        pEntry->numOfProfileDataPages = numOfProfileDataPages;
        pEntry->firstProfileDataPage  = firstProfileDataPage;
        pEntry->lastProfileDataPage   = lastProfileDataPage;

        if ( SaveDirectoryPage( dirPage, messageList )  == false )
            return false;
    }

    if ( headerUpdated == true )
    {
        // --- save the modified header
        if ( SaveHeaderPage( messageList ) == false )
            return false;     
    }

    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::ReleaseDirEntry( SAPDB_UInt4 const           dirEntry,
                                       Msg_List        &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::ReleaseDirEntry", RTE_Trace, 9);

    SAPDB_UInt4                  dirPage;
    SAPDB_UInt4                  dirPageEntry;
    SAPDB_UInt4                  lastDirPage;
    SAPDB_UInt4                  lastDirPageEntry;
    ProfleDirectory::DirEntry *  pEntry;
    ProfleDirectory::DirEntry *  pLastDirEntry;

    // --- get pointer to the directory entry which has to be released
    dirPage      = (SAPDB_UInt4)(dirEntry / ENTRIES_PER_DIRPAGE);
    dirPageEntry = (SAPDB_UInt4)(dirEntry % ENTRIES_PER_DIRPAGE);
    pEntry       = &(m_pDirPages[dirPage].dir.entries[dirPageEntry]);

    // --- get pointer to the last directory entry
    lastDirPage      = (SAPDB_UInt4)((m_pHeader->numOfDirEntries - 1) / ENTRIES_PER_DIRPAGE);
    lastDirPageEntry = (SAPDB_UInt4)((m_pHeader->numOfDirEntries - 1) % ENTRIES_PER_DIRPAGE);
    pLastDirEntry    = &(m_pDirPages[lastDirPage].dir.entries[lastDirPageEntry]);

    // --- copy the last directory entry into the gap
    _tcscpy( pEntry->userName, pLastDirEntry->userName );
    pEntry->profileLength          = pLastDirEntry->profileLength;
    pEntry->numOfProfileDataPages  = pLastDirEntry->numOfProfileDataPages;
    pEntry->firstProfileDataPage   = pLastDirEntry->firstProfileDataPage;
    pEntry->lastProfileDataPage    = pLastDirEntry->lastProfileDataPage;
    pLastDirEntry->userName[0]     = _T('\0');

    m_pHeader->numOfDirEntries--;

    m_Modified = true;

    // --- save the modified directory pages 
    //
    if ( dirPage != lastDirPage )
    {
        if (( SaveDirectoryPage( dirPage, messageList )     == false ) ||
            ( SaveDirectoryPage( lastDirPage, messageList ) == false ))
        {
            return false;
        }
    }
    else
    {
        if ( SaveDirectoryPage( dirPage, messageList ) == false )
            return false;
    }

    // --- save the modified header
    if ( SaveHeaderPage( messageList ) == false )
        return false;

    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::GetNextProfileDataPage( PageNoType const        pageNo, 
                                              PageEntry               &page,
                                              Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::GetNextProfileDataPage", RTE_Trace, 9);

    // --- Load a single data page
    if ( vLoadPage( pageNo, page, messageList ) == false )
        return false;

    // --- check page type
    if ( page.Common.pageType != ProfileDataPage )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_WRNG_CONTAINER_PAGE_TYPE,
                                            SAPDB_ToString(page.Common.pageType), 
                                            SAPDB_ToString(ProfileDataPage) );
        return false;
    }

    // --- check page no
    if ( page.Common.currPageNo != pageNo )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_CURR_PAGE,
                                            SAPDB_ToString(page.Common.currPageNo), 
                                            SAPDB_ToString(pageNo) );
        return false;
    }

    // --- check profile data length
    if ( page.profileData.dataLen > MAX_PROFILE_DATA_PER_PAGE )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_PROF_DATA_LEN,
                                            SAPDB_ToString(page.profileData.dataLen), 
                                            SAPDB_ToString(MAX_PROFILE_DATA_PER_PAGE) );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::GetNextUnusedPage( PageNoType              &pageNo,
                                         PageEntry               page,
                                         Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::GetNextUnusedPage", RTE_Trace, 9);

    // --- get the next page from unused page list
    pageNo = m_pHeader->firstUnusedPageNo;

    // --- unused page list empty?
    if ( pageNo == 0 )
    {
        return true;
    }

    if ( vLoadPage( pageNo, page, messageList ) == false ) 
        return false;

    // --- check page type
    if ( page.Common.pageType != UnusedPage )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_WRNG_CONTAINER_PAGE_TYPE,
                                            SAPDB_ToString(page.Common.pageType), 
                                            SAPDB_ToString(UnusedPage) );
        return false;
    }

    // --- check page no
    if ( page.Common.currPageNo != pageNo )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_CURR_PAGE,
                                            SAPDB_ToString(page.Common.currPageNo), 
                                            SAPDB_ToString(pageNo) );
        return false;
    }

    m_pHeader->firstUnusedPageNo = page.Common.nextPageNo;

    // --- no more pages?
    if ( m_pHeader->firstUnusedPageNo == 0 ) 
        m_pHeader->lastUnusedPageNo  = 0;

    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::SetProfileData ( SAPDB_Int4 const        length,
                                       void const * const      pProfile,
                                       PageNoType              &firstPageNo,
                                       PageNoType              &lastPageNo,
                                       SAPDB_UInt4             &numOfPages, 
                                       Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::SetProfileData", RTE_Trace, 9);

    enum  { ExistingPages, UnusedPages, NewPages }        pageSource;
    SAPDB_Int4                                            lengthCopied = 0;
    SAPDB_UInt4                                           pageCnt      = 0;
    SAPDB_UInt4                                           currPageIdx  = 0;
    SAPDB_UInt4                                           prevPageIdx  = 0;
    PageNoType                                            currPageNo;
    PageNoType                                            nextPageNo;
    PageEntry                                             page[2];

    SAPDBERR_ASSERT_ARGUMENT( (!( firstPageNo != 0 ) && ( lastPageNo == 0 )) ||
                              (!( firstPageNo == 0 ) && ( lastPageNo != 0 )));

    for ( nextPageNo = firstPageNo, pageSource = (firstPageNo != 0) ? ExistingPages : UnusedPages;
        (lengthCopied < length); )
    {
        switch ( pageSource )
        {
        case ExistingPages:

            if ( GetNextProfileDataPage( nextPageNo, page[currPageIdx], messageList ) == false )
                return false;

            currPageNo = nextPageNo; 
            nextPageNo = page[currPageIdx].profileData.nextPageNo;
            pageCnt++;

            if ( nextPageNo == 0 ) // --- if there are no pages left, proceed with unused pages!
            {
                pageSource = UnusedPages;
            }
            // --- is this the last page we need? ....
            else if ((lengthCopied + MAX_PROFILE_DATA_PER_PAGE) >= length ) 
            {
                // --- ... but there are some pages we don't need!
                if ( ReleaseProfileData( nextPageNo, lastPageNo, messageList ) == false )
                    return false;
            }
            break;

        case UnusedPages:
            if ( GetNextUnusedPage( currPageNo, page[currPageIdx], messageList ) == false )
                return false;

            if ( currPageNo == 0 ) // --- no unused pages left? ....
            {
                pageSource = NewPages; // ---  ....proceed with new pages!
                continue;
            }

            pageCnt++;
            break;

        case NewPages:
            //  --- get new pages from end of container 
            currPageNo = ++m_pHeader->lastPageNo;
            pageCnt++;
            break;
        }

        // --- The first time 'currPageIdx' and 'prevPageIdx' are identically
        if ( currPageIdx == prevPageIdx )  
            firstPageNo = currPageNo;
        else
        { // --- update 'nextPageNo' of the previous page
            page[prevPageIdx].profileData.nextPageNo = currPageNo;

            // --- save previous page
            if ( vSavePage( page[prevPageIdx].profileData.currPageNo, page[prevPageIdx], messageList ) == false )
                return false;
        }

        page[currPageIdx].profileData.currPageNo = currPageNo;
        page[currPageIdx].profileData.pageType   = ProfileDataPage;
        page[currPageIdx].profileData.dataLen    = MIN_LEN(MAX_PROFILE_DATA_PER_PAGE, length - lengthCopied);

        SAPDB_MemCopyNoCheck ( page[currPageIdx].profileData.data, (char*)pProfile + lengthCopied, 
                               page[currPageIdx].profileData.dataLen );

        lengthCopied += page[currPageIdx].profileData.dataLen;

        prevPageIdx  = currPageIdx;
        currPageIdx ^= 1;
    }

    numOfPages                               = pageCnt;
    lastPageNo                               = page[prevPageIdx].profileData.currPageNo;
    page[prevPageIdx].profileData.nextPageNo = 0;

    // --- save last page
    if ( vSavePage( page[prevPageIdx].profileData.currPageNo, page[prevPageIdx], messageList ) == false )
        return false;

    if ( SaveHeaderPage( messageList ) == false )
        return false;

    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::GetProfileData( SAPDB_UInt4 const           numOfPages,
                                      PageNoType const            firstPageNo,
                                      SAPDB_Int4 const            dataLength,
                                      SAPDB_Int4                  &length,
                                      void * const                pProfile,
                                      Msg_List        &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::GetProfileData", RTE_Trace, 9);

    SAPDB_Int4         lengthCopied = 0;
    SAPDB_UInt4        pageCnt;
    PageEntry          page;
    PageNoType         currPageNo;

    SAPDBERR_ASSERT_ARGUMENT(pProfile != 0);

    // --- insufficent profile buffer?
    if ( length < dataLength )
    {
        // --- always return the right length!
        length = dataLength;
        messageList = Msg_List( RTE_CONTEXT, RTEERR_INSUFFICIENT_BUFFER_SPACE );
        return false;
    }

    // --- Load all data pages
    for ( pageCnt= 0, currPageNo = firstPageNo; 
        ( pageCnt      <  numOfPages ) &&
        ( currPageNo   != 0 )          &&
        ( lengthCopied <  dataLength );
          pageCnt++ )
    {
        // --- Load a single data page
        if ( GetNextProfileDataPage( currPageNo, page, messageList ) == false )
            return false;

        SAPDB_MemCopyNoCheck ( (char*)pProfile + lengthCopied, page.profileData.data, page.profileData.dataLen );
        lengthCopied += page.profileData.dataLen;
        currPageNo    = page.profileData.nextPageNo;
    }

    // --- surprising 'next page' 
    if (( page.profileData.nextPageNo != 0 ) || ( pageCnt < numOfPages ))
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_NEXT_PAGE,
                                            SAPDB_ToString(page.profileData.nextPageNo) );
        return false;
    }

    // --- check profile data length
    if ( lengthCopied != dataLength )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_SURPRISING_PROF_DATA_LEN,
                                            SAPDB_ToString(lengthCopied), 
                                            SAPDB_ToString(dataLength) );
        return false;
    }

    length = dataLength;

    return true;
}

//-----------------------------------------------------------------------------

bool CommonContainer::ReleaseProfileData ( PageNoType const          firstPageNo,
                                           PageNoType const          lastPageNo,
                                           Msg_List      &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::ReleaseProfileData", RTE_Trace, 9);

    PageEntry             page;
    PageNoType           nextPageNo;

    // --- Mark all pages as unused
    //
    for ( nextPageNo = firstPageNo; nextPageNo != 0;  )
    {
        // --- Load the next data page
        if ( vLoadPage( nextPageNo, page, messageList ) == false ) 
            return false;

        SAPDBERR_ASSERT_STATE( page.Common.pageType   == ProfileDataPage );
        SAPDBERR_ASSERT_STATE( page.Common.currPageNo == nextPageNo );

        page.Common.pageType = UnusedPage;
        nextPageNo           = page.profileData.nextPageNo;

        // --- Save a unused page
        if ( vSavePage( page.Common.currPageNo, page, messageList ) == false )
            return false;
    }

    SAPDBERR_ASSERT_STATE( nextPageNo == 0 );

    //
    // --- Append the pages to the chain of unused pages.
    //
    if ( m_pHeader->firstUnusedPageNo == 0 )
    {
        m_pHeader->firstUnusedPageNo = firstPageNo;
        m_pHeader->lastUnusedPageNo  = lastPageNo;
    }
    else
    {
        if ( vLoadPage( m_pHeader->lastUnusedPageNo, page, messageList ) == false )
            return false;

        SAPDBERR_ASSERT_STATE( page.Common.pageType   == UnusedPage );
        SAPDBERR_ASSERT_STATE( page.Common.nextPageNo == 0 );

        page.Common.nextPageNo = firstPageNo; 

        if ( vSavePage( m_pHeader->lastUnusedPageNo, page, messageList ) == false )
            return false;

        m_pHeader->lastUnusedPageNo = lastPageNo;
    }

    if ( SaveHeaderPage( messageList ) == false )
        return false;

    return true;
}


//-----------------------------------------------------------------------------

bool CommonContainer::GetProfile( tsp00_C18c const        userName, 
                                  SAPDB_Int4              &length,
                                  void * const            pProfile,
                                  Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::GetProfile", RTE_Trace, 9);

    SAPDB_UInt4        dirEntry;
    SAPDB_Int4         dataLength;
    SAPDB_UInt4        numOfPages;
    PageNoType         firstPageNo;

    if ( FindDirEntry( userName, dirEntry ) == false )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_PROFILE_NOT_FOUND,
                                            SAPDB_ToString(userName) );
        return false;
    }

    GetDirEntryValues( dirEntry, 0, &dataLength, &numOfPages, &firstPageNo, 0 );

    if ( pProfile != 0 )
    {
        if ( GetProfileData ( numOfPages, firstPageNo, dataLength, 
                              length, pProfile, messageList ) == false )
        {
            return false;
        }
    }
    else
        length = dataLength;

    return true;
}


//-----------------------------------------------------------------------------

bool CommonContainer::SetProfile( tsp00_C18c const      userName, 
                                  SAPDB_Int4            length,
                                  void * const          pProfile,
                                  Msg_List  &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::GetProfile", RTE_Trace, 9);

    SAPDB_UInt4        dirEntry;
    SAPDB_UInt4        numOfPages;
    PageNoType         firstPageNo;
    PageNoType         lastPageNo;

    if ( FindDirEntry( userName, dirEntry ) == false )
    {
        dirEntry    = LAST_TOKEN;
        firstPageNo = 0;
        lastPageNo  = 0;
    }
    else
        GetDirEntryValues( dirEntry, 0, 0, 0, &firstPageNo, &lastPageNo );

    if (( SetProfileData   ( length, pProfile, firstPageNo, lastPageNo, 
                             numOfPages,  messageList )          == false ) ||
        ( SetDirEntryValues( userName, length, numOfPages, firstPageNo, 
                             lastPageNo, dirEntry, messageList ) == false ))
    {
        return false;
    }


    return true;
}


//-----------------------------------------------------------------------------

bool CommonContainer::DelProfile( tsp00_C18c const        userName, 
                                 Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::DelProfile", RTE_Trace, 9);

    SAPDB_UInt4        dirEntry;
    PageNoType           firstPageNo;
    PageNoType           lastPageNo;

    if ( FindDirEntry( userName, dirEntry ) == false )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_PROFILE_NOT_FOUND,
                                            SAPDB_ToString(userName) );
        return false;
    }

    GetDirEntryValues( dirEntry, 0, 0, 0, &firstPageNo, &lastPageNo );

    if (( ReleaseDirEntry( dirEntry, messageList )                   == false ) || 
        ( ReleaseProfileData( firstPageNo, lastPageNo, messageList ) == false ))
    {
        return false;
    }


    return true;
}


//-----------------------------------------------------------------------------

bool CommonContainer::ScanProfiles( SAPDB_UInt4             &token,
                                   SAPDB_Int4              &length,
                                   void * const            pProfile,
                                   tsp00_C18c              &userName, 
                                   Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("CommonContainer::ScanProfiles", RTE_Trace, 9);

    SAPDB_UInt4        dirEntry;
    SAPDB_Int4         dataLength;
    SAPDB_UInt4        numOfPages;
    PageNoType         firstPageNo;

    if (( token != FIRST_TOKEN ) && ( token >= m_pHeader->numOfDirEntries ))
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_INVALID_PROFILE_TOKEN,
                                            SAPDB_ToString(token));
        return false;
    }

    if (( m_pHeader->numOfDirEntries == 0 ) || ( m_pDirPages == 0 ))
    {
        token = NO_TOKEN;
        return true;
    }

    if ( token == FIRST_TOKEN )
    {
        m_Modified = false;
        dirEntry   = 0;
    }
    else if ( token == LAST_TOKEN )
        dirEntry = m_pHeader->numOfDirEntries - 1;
    else 
        dirEntry = token;

    GetDirEntryValues( dirEntry, &userName, &dataLength, &numOfPages, &firstPageNo, 0 );

    if ( pProfile != 0 )
    {
        if ( GetProfileData ( numOfPages, firstPageNo, dataLength, 
                              length, pProfile, messageList ) == false )
        {
            return false;
        }
    }
    else
        length = dataLength;

    token++;

    if ( token >= m_pHeader->numOfDirEntries )
        token = LAST_TOKEN;

    if ( m_Modified == true ) // - container modified during repeated scan activities ?
    {
        messageList = Msg_List( RTE_CONTEXT, RTEWARN_PROF_CONTAINER_MODIFIED );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//------------------------- class RTE_ProfileContainer -----------------------------
//-----------------------------------------------------------------------------

RTE_ProfileContainer::RTE_ProfileContainer( tsp00_DbNamec const     serverDB,
                                            bool                    readOnly,
                                            Msg_List    &messageList ) 
                      : m_ContainerIsEmpty ( false ),
                        m_ReadOnly(readOnly),
                        m_pEndOfPathName(0)
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::RTE_ProfileContainer", RTE_Trace, 9);
    tsp01_RteError      RteError;

# if defined(_WIN32)
    m_FileHdl  = INVALID_HANDLE_VALUE;
# else /* UNIX */
    m_FileDesc = -1;
# endif

    if ( sqlGetIndependentConfigPath( m_FileName, 1,  &RteError ) == false )
    {
        messageList = Msg_List( RTE_CONTEXT, RteError.OsErrCode, RTEINFO_ANY_ERROR_TEXT,
                                            SAPDB_ToString(RteError.RteErrText));
        return;
    }

    if ( m_FileName[0] != '\0' )
        m_pEndOfPathName = m_FileName + strlen(m_FileName.asCharp()) - 1;

    _tcscat( m_FileName, serverDB );
    _tcscat( m_FileName, ".upc" );

    return;
}


#if defined(_WIN32)

//-----------------------------------------------------------------------------
//-------------------- class RTE_ProfileContainer  ( WIN32 ) -----------------------
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::OpenContainer( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::OpenContainer", RTE_Trace, 9);
    DWORD               fdwCreate;
    DWORD               fdwAccess;
    DWORD               fdwShare;
    DWORD               fdwFlags;
    bool                newContainer = false;
    RTE_SystemRc        rc;
    char                DirSepChar;

    m_ContainerIsEmpty = false;

    // --- Does the file name contain a path?
    if ( m_pEndOfPathName != 0 )
    {
        DirSepChar = m_pEndOfPathName[0];
        m_pEndOfPathName[0] = '\0';

        // --- create directory
        CreateDirectory (m_FileName, 0 );

        m_pEndOfPathName[0] =  DirSepChar;
    }

    if ( m_ReadOnly )
        fdwAccess = GENERIC_READ;
    else
        fdwAccess = GENERIC_READ | GENERIC_WRITE;

    fdwFlags  = FILE_ATTRIBUTE_NORMAL;
    fdwShare  = FILE_SHARE_READ;
    fdwCreate = OPEN_EXISTING;

    m_FileHdl = CreateFile( m_FileName, fdwAccess, fdwShare, NULL, fdwCreate, fdwFlags, NULL );

    if ( m_FileHdl == INVALID_HANDLE_VALUE )
    {
        if ( GetLastError() == ERROR_FILE_NOT_FOUND )
        {
            fdwCreate = OPEN_ALWAYS;

            m_FileHdl = CreateFile( m_FileName, fdwAccess, fdwShare, NULL, fdwCreate, fdwFlags, NULL );

            if ( m_FileHdl != INVALID_HANDLE_VALUE )
                m_ContainerIsEmpty = true;
        }

        if ( m_FileHdl == INVALID_HANDLE_VALUE )
        {
            rc = GetLastError();
            messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_OPEN_PROFILE_CONTAINER,
                                                m_FileName.asCharp(), SAPDB_ToString(rc));
            return false;
        }
    }

    return Initialize ( m_ContainerIsEmpty, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::CloseContainer( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::CloseContainer", RTE_Trace, 9);

    if ( m_FileHdl != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_FileHdl );

        m_FileHdl = INVALID_HANDLE_VALUE;

        if ( m_ContainerIsEmpty == true )
            DeleteContainer( messageList );
    }
    return true;
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::DeleteContainer( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::DeleteContainer", RTE_Trace, 9);

    if ( DeleteFile( m_FileName ) == false )
    {
        RTE_SystemRc  rc = GetLastError();

        if ( rc != RTE_SYSTEMRC_FILE_NOT_FOUND )
        {
            messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_DELETE_PROFILE_CONTAINER,
                                                m_FileName.asCharp(), SAPDB_ToString(rc));
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::GetOwnerName( tsp00_Namec             &ownerName,              // - OUT
                                         Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::GetOwnerName", RTE_Trace, 9);

    if ( eo49GetFileOwner(m_FileHdl, ownerName) == false )
    {
        RTE_SystemRc  rc = GetLastError();

        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_GET_PROFILE_CONTAINER_OWNER,
                                            m_FileName.asCharp());
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::GetGroupName( tsp00_Namec             &groupName,       // - OUT
                                         Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::GetGroupName", RTE_Trace, 9);

    if ( eo49GetFileGroup(m_FileHdl, groupName) == false )
    {
        RTE_SystemRc  rc = errno;

        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_GET_PROFILE_CONTAINER_GROUP,
                                            m_FileName.asCharp());
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::SetGroupName( tsp00_Namec const       &groupName,   // - IN
                                         Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::SetGroupName", RTE_Trace, 9);

    if ( eo49SetFileGroup(m_FileHdl, groupName) == false )
    {
        RTE_SystemRc  rc = errno;

        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_SET_PROFILE_CONTAINER_GROUP,
                                            m_FileName.asCharp());
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::vLoadPage ( PageNoType const          pageNo,
                                       PageEntry                 &page,
                                       Msg_List      &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::vLoadPage", RTE_Trace, 9);

    RTE_SystemRc     rc;
    DWORD               LengthWanted = sizeof(page);
    DWORD               length;

    if ( SetFilePointer( m_FileHdl, pageNo * sizeof(page), NULL, FILE_BEGIN ) == -1 )
    {
        rc          = GetLastError();
        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_CANT_SEEK_ON_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(rc));
        return false;
    }

    if ( ReadFile(m_FileHdl, &page, LengthWanted, &length, NULL) == false )
    {
        rc = GetLastError();
        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_CANT_READ_FROM_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(rc));
        return false;
    }

    if ( length != LengthWanted )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_NOT_ALL_BYTES_READ_FROM_PROF_CONTAINER, 
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(length), 
                                            SAPDB_ToString(LengthWanted) );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::vSavePage ( PageNoType const          pageNo,
                                       PageEntry const           page,
                                       Msg_List      &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::vSavePage", RTE_Trace, 9);

    RTE_SystemRc     rc;
    DWORD               LengthWanted = sizeof(page);
    DWORD               length;

    if ( SetFilePointer( m_FileHdl, pageNo * sizeof(page), NULL, FILE_BEGIN ) == -1 )
    {
        rc = GetLastError();
        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_CANT_SEEK_ON_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(rc) );
        return false;
    }

    if ( WriteFile(m_FileHdl, &page, LengthWanted, &length, NULL) == false )
    {
        rc = GetLastError();
        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_CANT_WRITE_TO_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(rc) );
        return false;
    }

    if ( length != LengthWanted )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_NOT_ALL_BYTES_WRITTEN_TO_PROF_CONTAINER, 
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(length), 
                                            SAPDB_ToString(LengthWanted) );
        return false;
    }

    m_ContainerIsEmpty = false;

    return true;
}


#else /* UNIX */
//-----------------------------------------------------------------------------
//--------------------- class RTE_ProfileContainer  ( UNIX ) -----------------------
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::OpenContainer( Msg_List    &messageList )
{
    int                 OpenMode;
    char                DirSepChar;

    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::OpenContainer", RTE_Trace, 9);

    m_ContainerIsEmpty = false;

    // --- Does the file name contain a path?
    if ( m_pEndOfPathName != 0 )
    {
        DirSepChar = m_pEndOfPathName[0];
        m_pEndOfPathName[0] = '\0';

        // --- create directory
        mkdir (m_FileName, 0777);

        m_pEndOfPathName[0] =  DirSepChar;
    }

    if ( m_ReadOnly )
        OpenMode = O_RDONLY;
    else
        OpenMode = O_RDWR;

    m_FileDesc = open ( m_FileName, OpenMode, 0660 );

    if ( m_FileDesc == -1 )
    {
        if ( errno == ENOENT )
        {
            m_FileDesc = open ( m_FileName, OpenMode | O_CREAT, 0660 );

            if ( m_FileDesc != -1 )
                m_ContainerIsEmpty = true;
        }

        if ( m_FileDesc == -1 )
        {
            messageList = Msg_List( RTE_CONTEXT, errno, RTEERR_OPEN_PROFILE_CONTAINER,
                                                m_FileName.asCharp(),
                                                SAPDB_ToString(errno) );
            return false;
        }
    }

    return Initialize ( m_ContainerIsEmpty, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::CloseContainer( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::CloseContainer", RTE_Trace, 9);

    if ( m_FileDesc != -1 )
    {
        close ( m_FileDesc );

        m_FileDesc = -1;

        if ( m_ContainerIsEmpty == true )
            DeleteContainer( messageList );
    }

    return true;
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::DeleteContainer( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::DeleteContainer", RTE_Trace, 9);

    if ( unlink( m_FileName ) == -1 )
    {
        if ( errno != RTE_SYSTEMRC_FILE_NOT_FOUND )
        {
            messageList = Msg_List( RTE_CONTEXT, errno, RTEERR_DELETE_PROFILE_CONTAINER,
                                                m_FileName.asCharp(), SAPDB_ToString(errno) );
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::GetOwnerName( tsp00_Namec             &ownerName,              // - OUT
                                         Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::GetOwnerName", RTE_Trace, 9);

    if ( eo49GetFileOwner(m_FileDesc, ownerName) == false )
    {
        RTE_SystemRc  rc = errno;

        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_GET_PROFILE_CONTAINER_OWNER,
                                            m_FileName.asCharp() );
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::GetGroupName( tsp00_Namec             &groupName,       // - OUT
                                         Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::GetGroupName", RTE_Trace, 9);

    if ( eo49GetFileGroup(m_FileDesc, groupName) == false )
    {
        RTE_SystemRc  rc = errno;

        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_GET_PROFILE_CONTAINER_GROUP,
                                            m_FileName.asCharp() );
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::SetGroupName( tsp00_Namec const       &groupName,   // - IN
                                         Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::GetGroupName", RTE_Trace, 9);

    if ( eo49SetFileGroup(m_FileDesc, groupName) == false )
    {
        RTE_SystemRc  rc = errno;

        messageList = Msg_List( RTE_CONTEXT, rc, RTEERR_SET_PROFILE_CONTAINER_GROUP,
                                            m_FileName.asCharp());
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::GetFileHandle( int                     &fileHandle,         // - OUT
                                          Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::GetFileHandle", RTE_Trace, 9);

    fileHandle = m_FileDesc;
    return true;
}
//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::vLoadPage ( PageNoType const          pageNo,
                                       PageEntry                 &page,
                                       Msg_List      &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::vLoadPage", RTE_Trace, 9);

    size_t     LengthWanted = sizeof(page);
    off_t      Pos          = pageNo * sizeof(page);
    size_t     length;

    if ( lseek( m_FileDesc, Pos, 0 ) == -1 )
    {
        messageList = Msg_List( RTE_CONTEXT, errno, RTEERR_CANT_SEEK_ON_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(errno) );
        return false;
    }

    if ( (length = read(m_FileDesc, &page, LengthWanted )) == (size_t)-1 )
    {
        messageList = Msg_List( RTE_CONTEXT, errno, RTEERR_CANT_READ_FROM_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(errno) );
        return false;
    }

    if ( length != LengthWanted )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_NOT_ALL_BYTES_READ_FROM_PROF_CONTAINER, 
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(length), 
                                            SAPDB_ToString(LengthWanted) );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool RTE_ProfileContainer::vSavePage ( PageNoType const          pageNo,
                                       PageEntry const           page,
                                       Msg_List      &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ProfileContainer::vSavePage", RTE_Trace, 9);

    size_t     LengthWanted = sizeof(page);
    off_t      Pos          = pageNo * sizeof(page);
    size_t     length;


    if ( lseek( m_FileDesc, Pos, 0 ) == -1 )
    {
        messageList = Msg_List( RTE_CONTEXT, errno, RTEERR_CANT_SEEK_ON_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(errno) );
        return false;
    }

    if ( (length = write(m_FileDesc, &page, LengthWanted)) == (size_t)-1 )
    {
        messageList = Msg_List( RTE_CONTEXT, errno, RTEERR_CANT_WRITE_TO_PROFILE_CONTAINER,
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(errno) );
        return false;
    }

    if ( length != LengthWanted )
    {
        messageList = Msg_List( RTE_CONTEXT, RTEERR_NOT_ALL_BYTES_WRITTEN_TO_PROF_CONTAINER, 
                                            m_FileName.asCharp(), 
                                            SAPDB_ToString(length), 
                                            SAPDB_ToString(LengthWanted) );
        return false;
    }

    m_ContainerIsEmpty = false;

    return true;
}
#endif


/*===========================================================================*
*  STATIC/INLINE FUNCTION PROTOTYPES                                        *
*===========================================================================*/



/*===========================================================================*
*  METHODS                                                                  *
*===========================================================================*/

//-----------------------------------------------------------------------------
//---------------------- class RTE_UserProfileContainer -------------------------
//-----------------------------------------------------------------------------

RTE_UserProfileContainer::RTE_UserProfileContainer()
                         : m_Open(false)
{
}

//-----------------------------------------------------------------------------

RTE_UserProfileContainer::~RTE_UserProfileContainer()
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::~RTE_UserProfileContainer", RTE_Trace, 9);

    if ( m_pContainer != 0 )
    {
        if ( m_Open == true )
        {
            Msg_List    messageList;
            m_pContainer->CloseContainer(messageList);
        }
        destroy ( m_pContainer, RTEMem_RteAllocator::Instance() );
    }
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::Open( tsp00_DbNamec const     serverDB,
                                     Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::Open", RTE_Trace, 9);

    m_pContainer = new(RTEMem_RteAllocator::Instance()) RTE_ProfileContainer( serverDB, false, messageList ); 

    return 0 != m_pContainer;
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::Open( tsp00_DbNamec const     serverDB,
                                     bool                    readOnly,
                                     Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::Open", RTE_Trace, 9);

    m_pContainer = new(RTEMem_RteAllocator::Instance()) RTE_ProfileContainer( serverDB, readOnly, messageList ); 

    return 0 != m_pContainer;
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::GetOwnerName( tsp00_Namec             &username,
                                             Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::GetOwnerName", RTE_Trace, 9);

    if ( m_Open == false )
    {
        if ( (m_Open = m_pContainer->OpenContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->GetOwnerName( username, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::GetGroupName( tsp00_Namec             &groupname,
                                             Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::GetGroupName", RTE_Trace, 9);

    if ( m_Open == false )
    {
        if ( (m_Open = m_pContainer->OpenContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->GetGroupName( groupname, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::SetGroupName( tsp00_Namec const       &groupname,
                                             Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::SetGroupName", RTE_Trace, 9);

    if ( m_Open == false )
    {
        if ( (m_Open = m_pContainer->OpenContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->SetGroupName( groupname, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::SetProfile( tsp00_Namec const       userName,
                                           SAPDB_Int4 const        length,
                                           void * const            pProfile,
                                           Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::SetProfile", RTE_Trace, 9);

    if ( m_Open == false )
    {
        if ( (m_Open = m_pContainer->OpenContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->SetProfile( userName, length, pProfile, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::DelProfile( tsp00_Namec const        userName, 
                                           Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::DelProfile", RTE_Trace, 9);

    if ( m_Open == false )
    {
        if ( (m_Open = m_pContainer->OpenContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->DelProfile( userName, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::GetProfile( tsp00_Namec const        userName, 
                                           SAPDB_Int4              &length,
                                           void * const            pProfile,
                                           Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::GetProfile", RTE_Trace, 9);

    if ( m_Open == false )
    {
        if ( (m_Open = m_pContainer->OpenContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->GetProfile( userName, length, pProfile, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::ScanProfiles( SAPDB_UInt4             &token,
                                             SAPDB_Int4              &length,
                                             void * const            pProfile,
                                             tsp00_Namec              &userName, 
                                             Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::ScanProfiles", RTE_Trace, 9);

    if ( m_Open == false )
    {
        if ( (m_Open = m_pContainer->OpenContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->ScanProfiles( token, length, pProfile, userName, messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::DeleteContainer( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::DeleteContainer", RTE_Trace, 9);

    if ( m_Open == true )
    {
        m_Open = false;

        if ( (m_pContainer->CloseContainer ( messageList )) == false )
            return false;
    }

    return m_pContainer->DeleteContainer( messageList );
}

//-----------------------------------------------------------------------------

bool RTE_UserProfileContainer::CloseContainer( Msg_List    &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_UserProfileContainer::CloseContainer", RTE_Trace, 9);

    if ( m_Open == true )
    {
        m_Open = false;

        return m_pContainer->CloseContainer ( messageList );
    }

    return true;
}




/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/