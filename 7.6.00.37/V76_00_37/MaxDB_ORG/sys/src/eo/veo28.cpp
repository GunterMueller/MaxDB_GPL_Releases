  /*!
  @file           veo28.cpp
  @author         RaymondR
  @special area   Profile Container Class
  @brief          This container class is used to handle the storage of
              profile data.
  @see            http://p26326/Tools/Autorisierung-Ergebnis981013.html


                Profile data has arbitrary length, but can
                be identified by a userid/password/DBName combination.
                Each profile consists of length + bytestream.
                The profile data is only accessable via this interface.
                This is acting not on a ASCII-file....

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"
#include "heo01.h"
#include "geo001.h"
#include "geo200.h"
#include "geo007_1.h"
#define  ERR_CANT_GET_FILE_OWNER  N(152), ERR_TYPE,_T("MEMORY  "),_T("Could not get file '%s' owner, rc = %d")
#define  ERR_CANT_GET_FILE_GROUP  N(152), ERR_TYPE,_T("MEMORY  "),_T("Could not get file '%s' group, rc = %d")
#define  ERR_CANT_SET_FILE_GROUP  N(152), ERR_TYPE,_T("MEMORY  "),_T("Could not set file '%s' group '%s', rc = %d")
#include "geo28.h"
#include "geo60.h"
#include "geo570.h"
#include "geo49.h"   // Security specific stuff

#include <stddef.h>

#if defined(_WIN32)
# include <crtdbg.h>
#else /* UNIX */
# include "geo47.h"
# include <sys/types.h>
# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <assert.h>
# include <dirent.h>
# include <sys/stat.h>

# if defined(_DEBUG)
#  define _ASSERT      assert
# else
#  define _ASSERT(_assertion)
# endif
#endif

#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
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

#define MIN_LEN(_val1, _val2)    ((_val1) < (_val2)) ? (_val1) : (_val2)
#define OFFSET_OFF(_strct,_elem)   ((size_t)&(((_strct *)0)->_elem))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


//-----------------------------------------------------------------------------
//---------------------------- class t_Container ------------------------------
//-----------------------------------------------------------------------------
//
//
//  Container contents:
//
//          +==========================+
//          | <HeaderPage>             |   Comment: if there is no next page 'NextPageNo' is zero.
//          |--------------------------|            if 'FirstUnusedPageNo' is zero, add a new page
//      +---|    FirstUnusedPageNo     |            to the end of the container.
//   +--|---|    LastUnusedPageNo      |       
//   |  |   |    ----------------------|
//   |  |   |    FirstDirPageNo        |------+
//   |  |   +==========================+      |
//   |  |   | <DirectoryPage>          |<-----+
//   |  |   |--------------------------|
//   |  |   |    NextPageNo            |->0 
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
//   |  +---|    NextPageNo            |       |  |  |
//   |  |   +==========================+       |  |  |
//   |  |   .                          .       |  |  |
//   |  |   +==========================+       |  |  |
//   |  |   | <ProfileDataPage>        |<------+  |  |
//   |  |   |--------------------------|          |  |
//   |  |   |    NextPageNo            |----+     |  |
//   |  |   +==========================+    |     |  |
//   |  |   .                          .    |     |  |
//   |  |   +==========================+    |     |  |
//   +--+-->| <Unused Page>            |    |     |  |
//          |--------------------------|    |     |  |
//          |    NextPageNo            |->0 |     |  |
//          +==========================+    |     |  |
//          .                          .    |     |  |
//          +==========================+    |     |  |
//          | <ProfileDataPage>        |<---+-----+  |
//          |--------------------------|             |
//          |    NextPageNo            |->0          |
//          +==========================+             |
//          .                          .             |
//          +==========================+             |
//          | <ProfileDataPage>        |<------------+
//          |--------------------------|       
//          |    NextPageNo            |->0    
//          +==========================+
//
class t_Container : public teo570_GlobalAlloc
{
PRIVATE_NESTED_STRUCTS:

  typedef tsp00_Uint4   t_PageNo;


  //-------------------------------------------------
  //--------------- enum t_PageType -----------------
  //-------------------------------------------------

  typedef enum 
  {
    HeaderPage_e, 
    DirPage_e,
    ProfileDataPage_e,
    UnusedPage_e
  } t_PageType;


  //-------------------------------------------------
  //--------------- struct t_Header -----------------
  //-------------------------------------------------

  struct t_Header
  {
    t_PageType        PageType;         // - must be the first page entry!!!!
    t_PageNo          CurrPageNo;
    tsp00_Uint4       ContainerVersion;
    
    // --- unused pages
    t_PageNo          FirstUnusedPageNo;
    t_PageNo          LastUnusedPageNo;
      
    // --- directory pages
    tsp00_Uint4       NumOfDirEntries;
    tsp00_Uint4       NumOfDirPages;
     
    t_PageNo          FirstDirPageNo;
    
    t_PageNo          LastPageNo;
  };
 

  //-------------------------------------------------
  //------------ struct t_ProfileData ---------------
  //-------------------------------------------------

  struct t_ProfileData
  {
    t_PageType        PageType;         // - must be the first page entry!!!!
    t_PageNo          CurrPageNo;
    t_PageNo          NextPageNo;

    tsp00_Uint4       DataLen;
    char Data[1];
  };

# define MAX_PROFILE_DATA_PER_PAGE (tsp00_Int4)(CONTAINER_PAGE_SIZE  -  \
                                                OFFSET_OFF(t_ProfileData, Data[0]))

  //-------------------------------------------------
  //------------- struct t_Directory ----------------
  //-------------------------------------------------

  struct t_Directory
  {
    struct t_Entry
    {
      tsp00_C18c             UserName;
      tsp00_Int4             ProfileLength;
      tsp00_Uint4            NumOfProfileDataPages;
      t_PageNo               FirstProfileDataPage;
      t_PageNo               LastProfileDataPage;
    };


    t_PageType        PageType;         // - must be the first page entry!!!!
    t_PageNo          CurrPageNo;
    t_PageNo          NextPageNo;
    t_Entry           Entries[1];
  };

# define ENTRIES_PER_DIRPAGE   ((CONTAINER_PAGE_SIZE  - OFFSET_OFF(t_Directory, Entries[0])) \
                                 / sizeof(t_Directory::t_Entry))

  //-------------------------------------------------
  //---------------- union t_Page -------------------
  //-------------------------------------------------

  typedef union
  {
    struct {                       // - must be the first page entry!!!!   
      t_PageType        PageType;
      t_PageNo          CurrPageNo;
      t_PageNo          NextPageNo;
    } Common;

    t_Header           Header;
    t_Directory        Dir;
    t_ProfileData      ProfileData;
    unsigned char      Binary[CONTAINER_PAGE_SIZE];
  } t_Page;


public:
  bool GetProfile             ( tsp00_C18c const        UserName,               // - IN
                                tsp00_Int4              &Length,                // - IN
                                void * const            pProfile,               // - OUT
                                teo200_EventList *      pEventList );           
  bool SetProfile             ( tsp00_C18c const        UserName,               // - IN
                                tsp00_Int4              Length,                 // - IN
                                void * const            pProfile,               // - IN
                                teo200_EventList *      pEventList );
  bool ScanProfiles           ( tsp00_Uint4             &Token,                 // - IN/OUT
                                tsp00_Int4              &Length,                // - IN/OUT
                                void * const            pProfile,               // - OUT
                                tsp00_C18c              &UserName,              // - OUT
                                teo200_EventList *      pEventList );
  bool DelProfile             ( tsp00_C18c const        UserName,               // - IN
                                teo200_EventList *      pEventList );
protected:
  t_Container();
  ~t_Container();

  bool Initialize             ( bool                    NewContainer,           // - IN
                                teo200_EventList *      pEventList = NULL );

  virtual bool vLoadPage      ( t_PageNo const          PageNo,                 // - IN
                                t_Page                  &Page,                  // - OUT
                                teo200_EventList *      pEventList = NULL ) = 0;
  virtual bool vSavePage      ( t_PageNo const          PageNo,                 // - IN 
                                t_Page const            Page,                   // - IN
                                teo200_EventList *      pEventList = NULL ) = 0;

private:
  void InitHeaderPage         ();
  bool LoadHeaderPage         ( teo200_EventList *      pEventList );
  bool SaveHeaderPage         ( teo200_EventList *      pEventList ) 
                                { return vSavePage( 0, m_HeaderPage, pEventList ); };

  bool LoadDirectoryPages     ( teo200_EventList *      pEventList );
  bool SaveDirectoryPage      ( tsp00_Uint4 const       DirPage,                // - IN
                                teo200_EventList *      pEventList );
  bool NewDirectoryPage       ( teo200_EventList *      pEventList );
  bool FindDirEntry           ( tsp00_C18c const        UserName,               // - IN
                                tsp00_Uint4             &DirEntry );            // - OUT

  void GetDirEntryValues      ( tsp00_Uint4 const       DirEntry,               // - IN
                                tsp00_C18c *            pUserName,              // - OUT
                                tsp00_Int4 *            pProfileLength,         // - OUT
                                tsp00_Uint4 *           pNumOfProfileDataPages, // - OUT
                                t_PageNo *              pFirstProfileDataPage,  // - OUT
                                t_PageNo *              pLastProfileDataPage ); // - OUT

  bool SetDirEntryValues      ( tsp00_C18c const        UserName,               // - IN
                                tsp00_Int4 const        ProfileLength,          // - IN
                                tsp00_Uint4 const       NumOfProfileDataPages,  // - IN
                                t_PageNo const          FirstProfileDataPage,   // - IN
                                t_PageNo const          LastProfileDataPage,    // - IN
                                tsp00_Uint4             &DirEntry,              // - IN/OUT
                                teo200_EventList *      pEventList );
  
  bool ReleaseDirEntry        ( tsp00_Uint4 const       DirEntry,               // - IN
                                teo200_EventList *      pEventList );

  bool GetNextProfileDataPage ( t_PageNo const          PageNo,                 // - IN
                                t_Page                  &Page,                  // - OUT
                                teo200_EventList *      pEventList );

  bool GetNextUnusedPage      ( t_PageNo                &PageNo,                // - OUT
                                t_Page                  Page,                   // - OUT
                                teo200_EventList *      pEventList );

  bool ReleaseProfileData     ( t_PageNo const          FirstPageNo,            // - IN
                                t_PageNo const          LastPageNo,             // - IN
                                teo200_EventList *      pEventList );

  bool SetProfileData         ( tsp00_Int4 const        Length,                 // - IN
                                void const * const      pProfile,               // - IN
                                t_PageNo                &FirstPageNo,           // - IN/OUT
                                t_PageNo                &LastPageNo,            // - IN/OUT
                                tsp00_Uint4             &NumOfPages,            // - OUT
                                teo200_EventList *      pEventList );

  bool GetProfileData         ( tsp00_Uint4 const       NumOfPages,             // - IN
                                t_PageNo const          FirstPageNo,            // - IN
                                tsp00_Int4 const        DataLength,             // - IN
                                tsp00_Int4              &Length,                // - OUT
                                void * const            pProfile,               // - OUT
                                teo200_EventList *      pEventList );
private:
  t_Page              m_HeaderPage;
  t_Header *          m_pHeader;

  t_Page *            m_pDirPages;
  bool                m_Modified;
};

//-----------------------------------------------------------------------------
//------------------------- class t_FileContainer ----------------------------
//-----------------------------------------------------------------------------

class t_FileContainer : public t_Container
{
public: 
  t_FileContainer( tsp00_DbNamec const     ServerDB,
                   bool                    ReadOnly   = false,
                   teo200_EventList *      pEventList = NULL );
  ~t_FileContainer() {;};

  bool OpenContainer          ( teo200_EventList *      pEventList = NULL );
  bool CloseContainer         ( teo200_EventList *      pEventList = NULL );
  bool DeleteContainer        ( teo200_EventList *      pEventList = NULL );

  bool GetOwnerName           ( tsp00_Namec &  Username,               // - OUT
                                teo200_EventList *      pEventList );
  bool GetGroupName           ( tsp00_Namec &  Groupname,               // - OUT
                                teo200_EventList *      pEventList );
  bool SetGroupName           ( tsp00_Namec const & Groupname,
                                teo200_EventList *      pEventList );
  bool GetFileHandle          ( int & fileHandle,         // - OUT
                                teo200_EventList * );

private:
  bool vLoadPage              ( t_PageNo const          PageNo,                 // - IN
                                t_Page                  &Page,                  // - OUT
                                teo200_EventList *      pEventList = NULL );
  bool vSavePage              ( t_PageNo const          PageNo,                 // - IN 
                                t_Page const            Page,                   // - IN
                                teo200_EventList *      pEventList = NULL );

private:
  bool               m_ContainerIsEmpty;
  tsp00_Pathc        m_FileName;
  bool               m_ReadOnly;
  char *             m_pEndOfPathName;

# if defined(_WIN32)

private:
 HANDLE              m_FileHdl;

# else /* UNIX */
 int                 m_FileDesc;
private:

# endif

};


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


//-----------------------------------------------------------------------------
//---------------------------- class t_Container ------------------------------
//-----------------------------------------------------------------------------

t_Container::t_Container()
{
  ROUTINE_DBG_MEO60 (_T("t_Container::t_Container"));

  _ASSERT(CONTAINER_PAGE_SIZE == sizeof(t_Page));

  m_Modified  = false;
  m_pDirPages = NULL;
  m_pHeader   = &m_HeaderPage.Header;

  InitHeaderPage();
}

//-----------------------------------------------------------------------------

t_Container::~t_Container()
{
  ROUTINE_DBG_MEO60 (_T("t_Container::~t_Container"));

  teo200_EventList EvtLst;

  DEALLOCATE_EO570 (m_pDirPages, &EvtLst );

# if 0
   if ( EvtLst == true )
   {
     MSGCD_EVTLST (EvtLst);
   }
# endif
}

//-----------------------------------------------------------------------------


bool t_Container::Initialize ( bool               NewContainer,
                               teo200_EventList * pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::Initialize"));

  DEALLOCATE_EO570 (m_pDirPages, pEventList);

  m_Modified  = false;
  m_pDirPages = NULL;
  m_pHeader   = &m_HeaderPage.Header;

  if ( NewContainer == true )
    InitHeaderPage();
  else
  {
    if ( LoadHeaderPage( pEventList ) == false )
      return false;

    if ( LoadDirectoryPages( pEventList ) == false )
    {
      InitHeaderPage();
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

void t_Container::InitHeaderPage()
{
  ROUTINE_DBG_MEO60 (_T("t_Container::InitHeaderPage"));

  SAPDB_MemFillNoCheck ( &m_HeaderPage, 0, sizeof(m_HeaderPage) );

  m_pHeader->PageType         = HeaderPage_e;
  m_pHeader->ContainerVersion = CONTAINER_VERSION;

  return;
}

//-----------------------------------------------------------------------------

bool t_Container::LoadHeaderPage( teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::LoadHeaderPage"));

  // --- Load header page
  if ( vLoadPage( 0, m_HeaderPage, pEventList ) == false )
    return false;

  // --- check page type
  if ( m_HeaderPage.Common.PageType != HeaderPage_e )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_WRNG_CONTAINER_PAGE_TYPE, 
                             m_HeaderPage.Common.PageType, HeaderPage_e );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- check page no
  if ( m_HeaderPage.Common.CurrPageNo != 0 )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_CURR_PAGE, 
                             m_HeaderPage.Common.CurrPageNo, 0 );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- check container version
  if ( m_pHeader->ContainerVersion > CONTAINER_VERSION )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_WRNG_CONTAINER_VERSION, 
                             m_pHeader->ContainerVersion, CONTAINER_VERSION );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::LoadDirectoryPages( teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::LoadDirectoryPages"));

  tsp00_Uint4        PageCnt;
  t_PageNo           CurrPageNo;

  if ( m_pHeader->NumOfDirPages == 0 )
    return true;

  m_pDirPages = (t_Page*) ALLOCATE_EO570 ( sizeof(t_Page) * m_pHeader->NumOfDirPages, 
                                           pEventList );

  if ( m_pDirPages == NULL )
    return false;

  // --- Load all directory pages
  for ( PageCnt = 0, CurrPageNo = m_pHeader->FirstDirPageNo; 
        ( PageCnt < m_pHeader->NumOfDirPages ) && ( CurrPageNo != 0 );
        PageCnt++ )
  {
    // --- Load a single directory page
    if ( vLoadPage( CurrPageNo, m_pDirPages[PageCnt], pEventList ) == false )
      return false;

    // --- check page type
    if ( m_pDirPages[PageCnt].Common.PageType != DirPage_e )
    {
      teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_WRNG_CONTAINER_PAGE_TYPE, 
                               m_pDirPages[PageCnt].Common.PageType, DirPage_e );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    // --- check page no
    if ( m_pDirPages[PageCnt].Common.CurrPageNo != CurrPageNo )
    {
      teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_CURR_PAGE, 
                               m_pDirPages[PageCnt].Common.CurrPageNo, CurrPageNo );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    CurrPageNo = m_pDirPages[PageCnt].Dir.NextPageNo;
  }

  // --- surprising 'next page' 
  if (( m_pDirPages[PageCnt - 1].Common.NextPageNo != 0 ) || 
      ( PageCnt < m_pHeader->NumOfDirPages ))
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_NEXT_PAGE, 
                             m_pDirPages[PageCnt].Common.NextPageNo );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::SaveDirectoryPage( tsp00_Uint4 const    DirPage,
                                     teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::SaveDirectoryPage"));

  _ASSERT ( m_pDirPages                       != NULL );
  _ASSERT ( m_pDirPages[DirPage].Dir.PageType == DirPage_e );

  // --- Save a single directory page
  return vSavePage( m_pDirPages[DirPage].Dir.CurrPageNo, 
                    m_pDirPages[DirPage], pEventList );
}

//-----------------------------------------------------------------------------

bool t_Container::NewDirectoryPage( teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::NewDirectoryPage"));

  t_Page *                pDirPages;
  t_PageNo                NewDirPageNo;

  pDirPages = (t_Page*) ALLOCATE_EO570( sizeof(t_Page) * (m_pHeader->NumOfDirPages + 1), 
                                        pEventList );

  if ( pDirPages == NULL )
    return false;

  // --- get a new directory 'page no' from end of container
  m_pHeader->LastPageNo++;
  NewDirPageNo = m_pHeader->LastPageNo;
  
  if ( m_pHeader->NumOfDirPages == 0 )
    m_pHeader->FirstDirPageNo = NewDirPageNo;
  else
  {
    // --- copy old pages into the new buffer
    SAPDB_MemCopyNoCheck ( pDirPages, m_pDirPages, sizeof(t_Page) * m_pHeader->NumOfDirPages );

    // --- set 'NextPageNo' of previous page
    pDirPages[m_pHeader->NumOfDirPages - 1].Dir.NextPageNo = NewDirPageNo;   
  }
  
  // --- initialize the new page!
  SAPDB_MemFillNoCheck ( &pDirPages[m_pHeader->NumOfDirPages], 0, sizeof(t_Page) );
  pDirPages[m_pHeader->NumOfDirPages].Dir.PageType   = DirPage_e;
  pDirPages[m_pHeader->NumOfDirPages].Dir.CurrPageNo = NewDirPageNo;
  
  DEALLOCATE_EO570 (m_pDirPages, pEventList);
  m_pDirPages = pDirPages;

  
  m_pHeader->NumOfDirPages++;

 if ( SaveDirectoryPage( m_pHeader->NumOfDirPages - 1, pEventList ) == false )
   return false;

 if ( m_pHeader->NumOfDirPages > 1 )
 {
   if ( SaveDirectoryPage( m_pHeader->NumOfDirPages - 2, pEventList ) == false ) 
     return false;
 }

 if ( SaveHeaderPage( pEventList ) == false )
   return false;
 

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::FindDirEntry( tsp00_C18c const       UserName,
                                tsp00_Uint4            &DirEntry )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::GetDirEntry"));

  tsp00_Uint4        PageCnt;
  tsp00_Uint4        DirPageEntry;

  if ( m_pDirPages == NULL )
    return false;

  for ( PageCnt = 0, DirEntry = 0; 
        ( PageCnt  < m_pHeader->NumOfDirPages ) &&
        ( DirEntry < m_pHeader->NumOfDirEntries );
        PageCnt++ )
  {
    for ( DirPageEntry = 0; 
          ( DirPageEntry < ENTRIES_PER_DIRPAGE ) &&
          ( DirEntry     < m_pHeader->NumOfDirEntries );
          DirPageEntry++, DirEntry++ )
    {
      if ( m_pDirPages[PageCnt].Dir.Entries[DirPageEntry].UserName[0] == _T('\0') )
        continue;

      if ( _tcsicmp ( m_pDirPages[PageCnt].Dir.Entries[DirPageEntry].UserName, UserName  ) == 0 )
        return true;
    }
  }
  
  return false;
}

//-----------------------------------------------------------------------------

void t_Container::GetDirEntryValues( tsp00_Uint4 const       DirEntry,
                                     tsp00_C18c *            pUserName,
                                     tsp00_Int4 *            pProfileLength,
                                     tsp00_Uint4 *           pNumOfProfileDataPages,
                                     t_PageNo *              pFirstProfileDataPage,
                                     t_PageNo *              pLastProfileDataPage )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::GetDirEntryValues"));

  tsp00_Uint4             DirPage;
  tsp00_Uint4             DirPageEntry;
  t_Directory::t_Entry *  pEntry;

  DirPage        = (tsp00_Uint4)(DirEntry / ENTRIES_PER_DIRPAGE);
  DirPageEntry   = (tsp00_Uint4)(DirEntry % ENTRIES_PER_DIRPAGE);
  pEntry         = &(m_pDirPages[DirPage].Dir.Entries[DirPageEntry]);

  if ( pUserName != NULL )
    *pUserName = pEntry->UserName;

  if ( pProfileLength != NULL )
    *pProfileLength = pEntry->ProfileLength;

  if ( pNumOfProfileDataPages != NULL )
    *pNumOfProfileDataPages = pEntry->NumOfProfileDataPages;

  if ( pFirstProfileDataPage != NULL )
    *pFirstProfileDataPage = pEntry->FirstProfileDataPage;

  if ( pLastProfileDataPage != NULL )
    *pLastProfileDataPage = pEntry->LastProfileDataPage;

  return;
}

//-----------------------------------------------------------------------------

bool t_Container::SetDirEntryValues( tsp00_C18c const            UserName,
                                     tsp00_Int4 const            ProfileLength,
                                     tsp00_Uint4 const           NumOfProfileDataPages,
                                     t_PageNo const              FirstProfileDataPage,
                                     t_PageNo const              LastProfileDataPage,
                                     tsp00_Uint4                 &DirEntry,
                                     teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::SetDirEntryValues"));

  tsp00_Uint4             DirPage;
  tsp00_Uint4             DirPageEntry;
  t_Directory::t_Entry *  pEntry;
  bool                    HeaderUpdated = false;

  // --- append a new entry ?
  //
  if ( DirEntry == LAST_TOKEN_EO28 )
  {
    DirEntry = m_pHeader->NumOfDirEntries;

    // --- new directory page required?
    if ( m_pHeader->NumOfDirPages < ((m_pHeader->NumOfDirEntries + 1) / ENTRIES_PER_DIRPAGE) + 1 )
    { 
      if ( NewDirectoryPage( pEventList ) == false )
        return false;
    }
    m_pHeader->NumOfDirEntries++;
    HeaderUpdated = true;
  }

  DirPage      = (tsp00_Uint4)(DirEntry / ENTRIES_PER_DIRPAGE);
  DirPageEntry = (tsp00_Uint4)(DirEntry % ENTRIES_PER_DIRPAGE);
  pEntry       = &(m_pDirPages[DirPage].Dir.Entries[DirPageEntry]);

  if (( _tcsncmp ( pEntry->UserName, UserName, sizeof(UserName)-1 )) ||
      ( pEntry->ProfileLength          != ProfileLength )            ||
      ( pEntry->NumOfProfileDataPages  != NumOfProfileDataPages )    ||
      ( pEntry->FirstProfileDataPage   != FirstProfileDataPage )     ||
      ( pEntry->LastProfileDataPage    != LastProfileDataPage ))        
  {
    m_Modified = true;

    _tcscpy ( pEntry->UserName, UserName );
    pEntry->ProfileLength         = ProfileLength;
    pEntry->NumOfProfileDataPages = NumOfProfileDataPages;
    pEntry->FirstProfileDataPage  = FirstProfileDataPage;
    pEntry->LastProfileDataPage   = LastProfileDataPage;

    if ( SaveDirectoryPage( DirPage, pEventList )  == false )
      return false;
  }

  if ( HeaderUpdated == true )
  {
    // --- save the modified header
    if ( SaveHeaderPage( pEventList ) == false )
      return false;     
  }

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::ReleaseDirEntry( tsp00_Uint4 const           DirEntry,
                                   teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::ReleaseDirEntry"));

  tsp00_Uint4             DirPage;
  tsp00_Uint4             DirPageEntry;
  tsp00_Uint4             LastDirPage;
  tsp00_Uint4             LastDirPageEntry;
  t_Directory::t_Entry *  pEntry;
  t_Directory::t_Entry *  pLastDirEntry;

  // --- get pointer to the directory entry which has to be released
  DirPage      = (tsp00_Uint4)(DirEntry / ENTRIES_PER_DIRPAGE);
  DirPageEntry = (tsp00_Uint4)(DirEntry % ENTRIES_PER_DIRPAGE);
  pEntry       = &(m_pDirPages[DirPage].Dir.Entries[DirPageEntry]);

  // --- get pointer to the last directory entry
  LastDirPage      = (tsp00_Uint4)((m_pHeader->NumOfDirEntries - 1) / ENTRIES_PER_DIRPAGE);
  LastDirPageEntry = (tsp00_Uint4)((m_pHeader->NumOfDirEntries - 1) % ENTRIES_PER_DIRPAGE);
  pLastDirEntry    = &(m_pDirPages[LastDirPage].Dir.Entries[LastDirPageEntry]);

  // --- copy the last directory entry into the gap
  _tcscpy( pEntry->UserName, pLastDirEntry->UserName );
  pEntry->ProfileLength          = pLastDirEntry->ProfileLength;
  pEntry->NumOfProfileDataPages  = pLastDirEntry->NumOfProfileDataPages;
  pEntry->FirstProfileDataPage   = pLastDirEntry->FirstProfileDataPage;
  pEntry->LastProfileDataPage    = pLastDirEntry->LastProfileDataPage;
  pLastDirEntry->UserName[0]     = _T('\0');

  m_pHeader->NumOfDirEntries--;

  m_Modified = true;

  // --- save the modified directory pages 
  //
  if ( DirPage != LastDirPage )
  {
    if (( SaveDirectoryPage( DirPage, pEventList )     == false ) ||
        ( SaveDirectoryPage( LastDirPage, pEventList ) == false ))
    {
      return false;
    }
  }
  else
  {
    if ( SaveDirectoryPage( DirPage, pEventList ) == false )
      return false;
  }
    
  // --- save the modified header
  if ( SaveHeaderPage( pEventList ) == false )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::GetNextProfileDataPage( t_PageNo const      PageNo, 
                                          t_Page              &Page,
                                          teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::GetNextProfileDataPage"));

  // --- Load a single data page
  if ( vLoadPage( PageNo, Page, pEventList ) == false )
    return false;

  // --- check page type
  if ( Page.Common.PageType != ProfileDataPage_e )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_WRNG_CONTAINER_PAGE_TYPE, 
                             Page.Common.PageType, ProfileDataPage_e );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  
  // --- check page no
  if ( Page.Common.CurrPageNo != PageNo )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_CURR_PAGE, 
                             Page.Common.CurrPageNo, PageNo );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  
  // --- check profile data length
  if ( Page.ProfileData.DataLen > MAX_PROFILE_DATA_PER_PAGE )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_PROF_DATA_LEN, 
                             Page.ProfileData.DataLen, MAX_PROFILE_DATA_PER_PAGE );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::GetNextUnusedPage( t_PageNo            &PageNo,
                                     t_Page              Page,
                                     teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::GetNextUnusedPage"));

  // --- get the next page from unused page list
  PageNo = m_pHeader->FirstUnusedPageNo;

  // --- unused page list empty?
  if ( PageNo == 0 )
  {
    return true;
  }

  if ( vLoadPage( PageNo, Page, pEventList ) == false ) 
    return false;
    
  // --- check page type
  if ( Page.Common.PageType != UnusedPage_e )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_WRNG_CONTAINER_PAGE_TYPE, 
                             Page.Common.PageType, UnusedPage_e );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- check page no
  if ( Page.Common.CurrPageNo != PageNo )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_CURR_PAGE, 
                             Page.Common.CurrPageNo, PageNo );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  m_pHeader->FirstUnusedPageNo = Page.Common.NextPageNo;
  
  // --- no more pages?
  if ( m_pHeader->FirstUnusedPageNo == 0 ) 
    m_pHeader->LastUnusedPageNo  = 0;

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::SetProfileData ( tsp00_Int4 const        Length,
                                   void const * const      pProfile,
                                   t_PageNo                &FirstPageNo,
                                   t_PageNo                &LastPageNo,
                                   tsp00_Uint4             &NumOfPages, 
                                   teo200_EventList *      pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::SetProfileData"));

  enum  { ExistingPages_e, UnusedPages_e, NewPages_e }  PageSource;
  tsp00_Int4                                            LengthCopied = 0;
  tsp00_Uint4                                           PageCnt      = 0;
  tsp00_Uint4                                           CurrPageIdx  = 0;
  tsp00_Uint4                                           PrevPageIdx  = 0;
  t_PageNo                                              CurrPageNo;
  t_PageNo                                              NextPageNo;
  t_Page                                                Page[2];

  _ASSERT( (!( FirstPageNo != 0 ) && ( LastPageNo == 0 )) ||
           (!( FirstPageNo == 0 ) && ( LastPageNo != 0 )));
  
  for ( NextPageNo = FirstPageNo, PageSource = (FirstPageNo != 0) ? ExistingPages_e : UnusedPages_e;
        (LengthCopied < Length); )
  {
    switch ( PageSource )
    {
    case ExistingPages_e:

      if ( GetNextProfileDataPage( NextPageNo, Page[CurrPageIdx], pEventList ) == false )
        return false;

      CurrPageNo = NextPageNo; 
      NextPageNo = Page[CurrPageIdx].ProfileData.NextPageNo;
      PageCnt++;

      if ( NextPageNo == 0 ) // --- if there are no pages left, proceed with unused pages!
      {
        PageSource = UnusedPages_e;
      }
      // --- is this the last page we need? ....
      else if ((LengthCopied + MAX_PROFILE_DATA_PER_PAGE) >= Length ) 
      {
        // --- ... but there are some pages we don't need!
        if ( ReleaseProfileData( NextPageNo, LastPageNo, pEventList ) == false )
          return false;
      }
      break;
      
    case UnusedPages_e:
      if ( GetNextUnusedPage( CurrPageNo, Page[CurrPageIdx], pEventList ) == false )
        return false;

      if ( CurrPageNo == 0 ) // --- no unused pages left? ....
      {
        PageSource = NewPages_e; // ---  ....proceed with new pages!
        continue;
      }

      PageCnt++;
      break;
      
    case NewPages_e:
      //  --- get new pages from end of container 
      CurrPageNo = ++m_pHeader->LastPageNo;
      PageCnt++;
      break;
    }

    // --- The first time 'CurrPageIdx' and 'PrevPageIdx' are identically
    if ( CurrPageIdx == PrevPageIdx )  
      FirstPageNo = CurrPageNo;
    else
    { // --- update 'NextPageNo' of the previous page
      Page[PrevPageIdx].ProfileData.NextPageNo = CurrPageNo;

      // --- save previous page
      if ( vSavePage( Page[PrevPageIdx].ProfileData.CurrPageNo, Page[PrevPageIdx], pEventList ) == false )
        return false;
    }

    Page[CurrPageIdx].ProfileData.CurrPageNo = CurrPageNo;
    Page[CurrPageIdx].ProfileData.PageType   = ProfileDataPage_e;
    Page[CurrPageIdx].ProfileData.DataLen    = MIN_LEN(MAX_PROFILE_DATA_PER_PAGE, Length - LengthCopied);
    
    SAPDB_MemCopyNoCheck ( Page[CurrPageIdx].ProfileData.Data, (char*)pProfile + LengthCopied, 
             Page[CurrPageIdx].ProfileData.DataLen );
    
    LengthCopied += Page[CurrPageIdx].ProfileData.DataLen;

    PrevPageIdx  = CurrPageIdx;
    CurrPageIdx ^= 1;
  }

  NumOfPages                               = PageCnt;
  LastPageNo                               = Page[PrevPageIdx].ProfileData.CurrPageNo;
  Page[PrevPageIdx].ProfileData.NextPageNo = 0;

  // --- save last page
  if ( vSavePage( Page[PrevPageIdx].ProfileData.CurrPageNo, Page[PrevPageIdx], pEventList ) == false )
    return false;

  if ( SaveHeaderPage( pEventList ) == false )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::GetProfileData( tsp00_Uint4 const           NumOfPages,
                                  t_PageNo const              FirstPageNo,
                                  tsp00_Int4 const            DataLength,
                                  tsp00_Int4                  &Length,
                                  void * const                pProfile,
                                  teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::GetProfileData"));

  tsp00_Int4         LengthCopied = 0;
  tsp00_Uint4        PageCnt;
  t_Page             Page;
  t_PageNo           CurrPageNo;

  _ASSERT(pProfile != NULL);

  // --- insufficent profile buffer?
  if ( Length < DataLength )
  {
    // --- always return the right length!
    Length = DataLength;
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_INSUFFICIENT_BUFFER );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- Load all data pages
  for ( PageCnt= 0, CurrPageNo = FirstPageNo; 
        ( PageCnt      <  NumOfPages ) &&
        ( CurrPageNo   != 0 )          &&
        ( LengthCopied <  DataLength );
        PageCnt++ )
  {
    // --- Load a single data page
    if ( GetNextProfileDataPage( CurrPageNo, Page, pEventList ) == false )
      return false;

    SAPDB_MemCopyNoCheck ( (char*)pProfile + LengthCopied, Page.ProfileData.Data, Page.ProfileData.DataLen );
    LengthCopied += Page.ProfileData.DataLen;
    CurrPageNo    = Page.ProfileData.NextPageNo;
  }

  // --- surprising 'next page' 
  if (( Page.ProfileData.NextPageNo != 0 ) || ( PageCnt < NumOfPages ))
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_NEXT_PAGE, 
                             Page.ProfileData.NextPageNo );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- check profile data length
  if ( LengthCopied != DataLength )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_SURPRISING_PROF_DATA_LEN, 
                             LengthCopied, DataLength );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  Length = DataLength;

  return true;
}

//-----------------------------------------------------------------------------

bool t_Container::ReleaseProfileData ( t_PageNo const          FirstPageNo,
                                       t_PageNo const          LastPageNo,
                                       teo200_EventList *      pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::ReleaseProfileData"));

  t_Page             Page;
  t_PageNo           NextPageNo;

  // --- Mark all pages as unused
  //
  for ( NextPageNo = FirstPageNo; NextPageNo != 0;  )
  {
    // --- Load the next data page
    if ( vLoadPage( NextPageNo, Page, pEventList ) == false ) 
      return false;

    _ASSERT( Page.Common.PageType   == ProfileDataPage_e );
    _ASSERT( Page.Common.CurrPageNo == NextPageNo );

    Page.Common.PageType = UnusedPage_e;
    NextPageNo    = Page.ProfileData.NextPageNo;

    // --- Save a unused page
    if ( vSavePage( Page.Common.CurrPageNo, Page, pEventList ) == false )
      return false;
  }

  _ASSERT( NextPageNo == 0 );

  //
  // --- Append the pages to the chain of unused pages.
  //
  if ( m_pHeader->FirstUnusedPageNo == 0 )
  {
    m_pHeader->FirstUnusedPageNo = FirstPageNo;
    m_pHeader->LastUnusedPageNo  = LastPageNo;
  }
  else
  {
    if ( vLoadPage( m_pHeader->LastUnusedPageNo, Page, pEventList ) == false )
      return false;

    _ASSERT( Page.Common.PageType   == UnusedPage_e );
    _ASSERT( Page.Common.NextPageNo == NULL );

    Page.Common.NextPageNo = FirstPageNo; 

    if ( vSavePage( m_pHeader->LastUnusedPageNo, Page, pEventList ) == false )
      return false;

    m_pHeader->LastUnusedPageNo = LastPageNo;
  }

  if ( SaveHeaderPage( pEventList ) == false )
    return false;

  return true;
}


//-----------------------------------------------------------------------------

bool t_Container::GetProfile( tsp00_C18c const      UserName, 
                              tsp00_Int4            &Length,
                              void * const          pProfile,
                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::GetProfile"));

  tsp00_Uint4        DirEntry;
  tsp00_Int4         DataLength;
  tsp00_Uint4        NumOfPages;
  t_PageNo           FirstPageNo;

  if ( FindDirEntry( UserName, DirEntry ) == false )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_PROFILE_NOT_FOUND );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  GetDirEntryValues( DirEntry, NULL, &DataLength, &NumOfPages, &FirstPageNo, NULL );

  if ( pProfile != NULL )
  {
    if ( GetProfileData ( NumOfPages, FirstPageNo, DataLength, 
                          Length, pProfile, pEventList ) == false )
    {
      return false;
    }
  }
  else
    Length = DataLength;

  return true;
}


//-----------------------------------------------------------------------------

bool t_Container::SetProfile( tsp00_C18c const      UserName, 
                              tsp00_Int4            Length,
                              void * const          pProfile,
                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::GetProfile"));

  tsp00_Uint4        DirEntry;
  tsp00_Uint4        NumOfPages;
  t_PageNo           FirstPageNo;
  t_PageNo           LastPageNo;

  if ( FindDirEntry( UserName, DirEntry ) == false )
  {
    DirEntry    = LAST_TOKEN_EO28;
    FirstPageNo = 0;
    LastPageNo  = 0;
  }
  else
    GetDirEntryValues( DirEntry, NULL, NULL, NULL, &FirstPageNo, &LastPageNo );

  if (( SetProfileData   ( Length, pProfile, FirstPageNo, LastPageNo, 
                           NumOfPages,  pEventList )          == false ) ||
      ( SetDirEntryValues( UserName, Length, NumOfPages, FirstPageNo, 
                           LastPageNo, DirEntry, pEventList ) == false ))
  {
    return false;
  }


  return true;
}


//-----------------------------------------------------------------------------

bool t_Container::DelProfile( tsp00_C18c const      UserName, 
                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::DelProfile"));

  tsp00_Uint4        DirEntry;
  t_PageNo           FirstPageNo;
  t_PageNo           LastPageNo;

  if ( FindDirEntry( UserName, DirEntry ) == false )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_PROFILE_NOT_FOUND );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  GetDirEntryValues( DirEntry, NULL, NULL, NULL, &FirstPageNo, &LastPageNo );

  if (( ReleaseDirEntry( DirEntry, pEventList )                   == false ) || 
      ( ReleaseProfileData( FirstPageNo, LastPageNo, pEventList ) == false ))
  {
    return false;
  }


  return true;
}


//-----------------------------------------------------------------------------

bool t_Container::ScanProfiles( tsp00_Uint4         &Token,
                                tsp00_Int4          &Length,
                                void * const        pProfile,
                                tsp00_C18c          &UserName, 
                                teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("t_Container::ScanProfiles"));

  tsp00_Uint4        DirEntry;
  tsp00_Int4         DataLength;
  tsp00_Uint4        NumOfPages;
  t_PageNo           FirstPageNo;

  if (( Token != FIRST_TOKEN_EO28 ) && ( Token >= m_pHeader->NumOfDirEntries ))
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_INVALID_PROFILE_TOKEN, Token );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if (( m_pHeader->NumOfDirEntries == 0 ) || ( m_pDirPages == NULL ))
  {
    Token = NO_TOKEN_EO28;
    return true;
  }

  if ( Token == FIRST_TOKEN_EO28 )
  {
    m_Modified = false;
    DirEntry   = 0;
  }
  else if ( Token == LAST_TOKEN_EO28 )
    DirEntry = m_pHeader->NumOfDirEntries - 1;
  else 
    DirEntry = Token;

  GetDirEntryValues( DirEntry, &UserName, &DataLength, &NumOfPages, &FirstPageNo, NULL );

  if ( pProfile != NULL )
  {
    if ( GetProfileData ( NumOfPages, FirstPageNo, DataLength, 
                          Length, pProfile, pEventList ) == false )
    {
      return false;
    }
  }
  else
    Length = DataLength;

  Token++;

  if ( Token >= m_pHeader->NumOfDirEntries )
    Token = LAST_TOKEN_EO28;

  if ( m_Modified == true ) // - container modified during repeated scan activities ?
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, WRN_PROF_CONTAINER_MODIFIED );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  
  return true;
}

//-----------------------------------------------------------------------------
//------------------------- class t_FileContainer -----------------------------
//-----------------------------------------------------------------------------

t_FileContainer::t_FileContainer( tsp00_DbNamec const     ServerDB,
                                  bool                    ReadOnly,
                                  teo200_EventList *      pEventList ) 
                : m_ContainerIsEmpty ( false ),
                  m_ReadOnly(ReadOnly),
                  m_pEndOfPathName(NULL)
{
  ROUTINE_DBG_MEO60 (_T("t_FileContainer::t_FileContainer"));
  tsp01_RteError      RteError;

# if defined(_WIN32)
  m_FileHdl  = INVALID_HANDLE_VALUE;
# else /* UNIX */
  m_FileDesc = -1;
# endif
  
  if ( sqlGetIndependentConfigPath( m_FileName, 1,  &RteError ) == false )
  {
    teo200_EventList EvtLst( RteError.OsErrCode, FUNCTION_NAME_MEO60, ERR_ERROR_TEXT, RteError.RteErrText );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return;
  }
 
  if ( m_FileName[0] != '\0' )
    m_pEndOfPathName = m_FileName + strlen(m_FileName.asCharp()) - 1;

  _tcscat( m_FileName, ServerDB );
  _tcscat( m_FileName, ".upc" );

  return;
 }
 

#if defined(_WIN32)

 //-----------------------------------------------------------------------------
 //-------------------- class t_FileContainer  ( WIN32 ) -----------------------
 //-----------------------------------------------------------------------------

 bool t_FileContainer::OpenContainer( teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::OpenContainer"));
   DWORD               fdwCreate;
   DWORD               fdwAccess;
   DWORD               fdwShare;
   DWORD               fdwFlags;
   bool                NewContainer = false;
   teo001_SystemRc     rc;
   char                DirSepChar;

   m_ContainerIsEmpty = false;

   // --- Does the file name contain a path?
   if ( m_pEndOfPathName != NULL )
   {
     DirSepChar = m_pEndOfPathName[0];
     m_pEndOfPathName[0] = '\0';
     
     // --- create directory
     CreateDirectory (m_FileName, NULL );
     
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
       teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_OPEN_FILE, m_FileName.asCharp(), rc );
       COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
       return false;
     }
   }

  return Initialize ( m_ContainerIsEmpty, pEventList );
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::CloseContainer( teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::CloseContainer"));

   if ( m_FileHdl != INVALID_HANDLE_VALUE )
   {
     CloseHandle( m_FileHdl );

     m_FileHdl = INVALID_HANDLE_VALUE;

     if ( m_ContainerIsEmpty == true )
       DeleteContainer( pEventList );
   }
   return true;
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::DeleteContainer( teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::DeleteContainer"));

   if ( DeleteFile( m_FileName ) == false )
   {
     teo001_SystemRc  rc = GetLastError();

     if ( rc != ERROR_FILE_NOT_FOUND_EO001 )
     {
       teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_DELETE, m_FileName.asCharp() );
       COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
       return false;
     }
   }

   return true;
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::GetOwnerName( tsp00_Namec &  OwnerName,              // - OUT
                                     teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::GetOwnerName"));

   if ( eo49GetFileOwner(m_FileHdl, OwnerName) == false )
   {
     teo001_SystemRc  rc = GetLastError();

     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_GET_FILE_OWNER, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }
 //-----------------------------------------------------------------------------

 bool t_FileContainer::GetGroupName( tsp00_Namec & GroupName,       // - OUT
                                     teo200_EventList *     pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::GetGroupName"));

   if ( eo49GetFileGroup(m_FileHdl, GroupName) == false )
   {
     teo001_SystemRc  rc = errno;

     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_GET_FILE_GROUP, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }
 //-----------------------------------------------------------------------------

 bool t_FileContainer::SetGroupName( tsp00_Namec const & GroupName,   // - IN
                                     teo200_EventList *     pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::GetGroupName"));

   if ( eo49SetFileGroup(m_FileHdl, GroupName) == false )
   {
     teo001_SystemRc  rc = errno;

     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_SET_FILE_GROUP, m_FileName.asCharp(), GroupName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }
 //-----------------------------------------------------------------------------

 bool t_FileContainer::vLoadPage ( t_PageNo const          PageNo,
                                   t_Page                  &Page,
                                   teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::vLoadPage"));

   teo001_SystemRc     rc;
   DWORD               LengthWanted = sizeof(Page);
   DWORD               Length;

   if ( SetFilePointer( m_FileHdl, PageNo * sizeof(Page), NULL, FILE_BEGIN ) == -1 )
   {
     rc = GetLastError();
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_SEEK_FILE_DEV_PIPE, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( ReadFile(m_FileHdl, &Page, LengthWanted, &Length, NULL) == false )
   {
     rc = GetLastError();
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_READ_FILE_DEV_PIPE, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( Length != LengthWanted )
   {
     teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_NOT_ALL_BYTES_READ, 
                              m_FileName.asCharp(), Length, LengthWanted );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::vSavePage ( t_PageNo const          PageNo,
                                   t_Page const            Page,
                                   teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::vSavePage"));

   teo001_SystemRc     rc;
   DWORD               LengthWanted = sizeof(Page);
   DWORD               Length;

   if ( SetFilePointer( m_FileHdl, PageNo * sizeof(Page), NULL, FILE_BEGIN ) == -1 )
   {
     rc = GetLastError();
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_SEEK_FILE_DEV_PIPE, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( WriteFile(m_FileHdl, &Page, LengthWanted, &Length, NULL) == false )
   {
     rc = GetLastError();
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_WRITE_FILE_DEV_PIPE, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( Length != LengthWanted )
   {
     teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_NOT_ALL_BYTES_WRITTEN, 
                              m_FileName.asCharp(), Length, LengthWanted );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   m_ContainerIsEmpty = false;

   return true;
 }


#else /* UNIX */
 //-----------------------------------------------------------------------------
 //--------------------- class t_FileContainer  ( UNIX ) -----------------------
 //-----------------------------------------------------------------------------
                                   
 bool t_FileContainer::OpenContainer( teo200_EventList *      pEventList )
 {
   int                 OpenMode;
   bool                DoCreate;
   char                DirSepChar;

   ROUTINE_DBG_MEO60 (_T("t_FileContainer::OpenContainer"));

   m_ContainerIsEmpty = false;

   // --- Does the file name contain a path?
   if ( m_pEndOfPathName != NULL )
   {
     DirSepChar = m_pEndOfPathName[0];
     m_pEndOfPathName[0] = '\0';

     // --- create directory
     mkdir (m_FileName, 0777);
     
     m_pEndOfPathName[0] =  DirSepChar;
   }

   if ( m_ReadOnly )
   {
     OpenMode = O_RDONLY;
     DoCreate = false;
   }
   else
   {
     OpenMode = O_RDWR;
     DoCreate = true;
   }

   m_FileDesc = open ( m_FileName, OpenMode, 0660 );

   if ( m_FileDesc == -1 )
   {
     if ( errno == ENOENT )
     {
       m_FileDesc = open ( m_FileName, OpenMode | ( DoCreate ? O_CREAT : 0 ), 0660 );

       if ( m_FileDesc != -1 )
         m_ContainerIsEmpty = true;
     }

     if ( m_FileDesc == -1 )
     {
       teo200_EventList EvtLst( errno, FUNCTION_NAME_MEO60, ERR_CANT_OPEN_FILE, m_FileName.asCharp(), errno );
       COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
       return false;
     }
   }

  return Initialize ( m_ContainerIsEmpty, pEventList );
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::CloseContainer( teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::CloseContainer"));

   if ( m_FileDesc != -1 )
   {
     close ( m_FileDesc );

     m_FileDesc = -1;

     if ( m_ContainerIsEmpty == true )
       DeleteContainer( pEventList );
   }

   return true;
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::DeleteContainer( teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::DeleteContainer"));

   if ( unlink( m_FileName ) == -1 )
   {
     if ( errno != ERROR_FILE_NOT_FOUND_EO001 )
     {
       teo200_EventList EvtLst( errno, FUNCTION_NAME_MEO60, ERR_DELETE, m_FileName.asCharp() );
       COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
       return false;
     }
   }

   return true;
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::GetOwnerName( tsp00_Namec &  OwnerName,              // - OUT
                                     teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::GetOwnerName"));

   if ( eo49GetFileOwner(m_FileDesc, OwnerName) == false )
   {
     teo001_SystemRc  rc = errno;

     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_GET_FILE_OWNER, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }
 //-----------------------------------------------------------------------------

 bool t_FileContainer::GetGroupName( tsp00_Namec & GroupName,       // - OUT
                                     teo200_EventList *     pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::GetGroupName"));

   if ( eo49GetFileGroup(m_FileDesc, GroupName) == false )
   {
     teo001_SystemRc  rc = errno;

     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_GET_FILE_GROUP, m_FileName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }
 //-----------------------------------------------------------------------------

 bool t_FileContainer::SetGroupName( tsp00_Namec const & GroupName,   // - IN
                                     teo200_EventList *     pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::GetGroupName"));

   if ( eo49SetFileGroup(m_FileDesc, GroupName) == false )
   {
     teo001_SystemRc  rc = errno;

     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CANT_SET_FILE_GROUP, m_FileName.asCharp(), GroupName.asCharp(), rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }
 //-----------------------------------------------------------------------------

 bool t_FileContainer::GetFileHandle( int & fileHandle,         // - OUT
                                      teo200_EventList * )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::GetFileHandle"));

   fileHandle = m_FileDesc;
   return true;
 }
 //-----------------------------------------------------------------------------

 bool t_FileContainer::vLoadPage ( t_PageNo const          PageNo,
                                   t_Page                  &Page,
                                   teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::vLoadPage"));

   size_t     LengthWanted = sizeof(Page);
   off_t      Pos          = PageNo * sizeof(Page);
   size_t     Length;

   if ( lseek( m_FileDesc, Pos, 0 ) == -1 )
   {
     teo200_EventList EvtLst( errno, FUNCTION_NAME_MEO60, ERR_CANT_SEEK_FILE_DEV_PIPE, m_FileName.asCharp(), errno );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( (Length = read(m_FileDesc, &Page, LengthWanted )) == (size_t)-1 )
   {
     teo200_EventList EvtLst( errno, FUNCTION_NAME_MEO60, ERR_CANT_READ_FILE_DEV_PIPE, m_FileName.asCharp(), errno );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( Length != LengthWanted )
   {
     teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_NOT_ALL_BYTES_READ, 
                              m_FileName.asCharp(), Length, LengthWanted );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   return true;
 }

 //-----------------------------------------------------------------------------

 bool t_FileContainer::vSavePage ( t_PageNo const          PageNo,
                                   t_Page const            Page,
                                   teo200_EventList *      pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_FileContainer::vSavePage"));

   size_t     LengthWanted = sizeof(Page);
   off_t      Pos          = PageNo * sizeof(Page);
   size_t     Length;


   if ( lseek( m_FileDesc, Pos, 0 ) == -1 )
   {
     teo200_EventList EvtLst( errno, FUNCTION_NAME_MEO60, ERR_CANT_SEEK_FILE_DEV_PIPE, m_FileName.asCharp(), errno );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( (Length = write(m_FileDesc, &Page, LengthWanted)) == (size_t)-1 )
   {
     teo200_EventList EvtLst( errno, FUNCTION_NAME_MEO60, ERR_CANT_WRITE_FILE_DEV_PIPE, m_FileName.asCharp(), errno );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   if ( Length != LengthWanted )
   {
     teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_NOT_ALL_BYTES_WRITTEN, 
                              m_FileName.asCharp(), Length, LengthWanted );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
     return false;
   }

   m_ContainerIsEmpty = false;

   return true;
 }



#endif


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

//-----------------------------------------------------------------------------
//---------------------- class teo28_ProfileContainer -------------------------
//-----------------------------------------------------------------------------

teo28_ProfileContainer::teo28_ProfileContainer( tsp00_DbNamec const ServerDB,
                                                teo200_EventList *  pEventList )
                       : m_Open(false)
{
  CLEAR_EVENTLIST_EO200(pEventList);

  m_pContainer = NULL;

  try 
  { 
    m_pContainer = new t_FileContainer( ServerDB, false, pEventList ); 
  }
  catch ( teo200_EventList EvtLst )
  {
    if ( pEventList != NULL )
      *pEventList = EvtLst;
    else
      throw EvtLst;
  }
}

//-----------------------------------------------------------------------------
 
teo28_ProfileContainer::teo28_ProfileContainer( tsp00_DbNamec const ServerDB,
                                                bool                ReadOnly,
                                                teo200_EventList *  pEventList )
                       : m_Open(false)
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::teo28_ProfileContainer"));

  CLEAR_EVENTLIST_EO200(pEventList);

  m_pContainer = NULL;

  try 
  { 
    m_pContainer = new t_FileContainer( ServerDB, ReadOnly, pEventList ); 
  }
  catch ( teo200_EventList EvtLst )
  {
    if ( pEventList != NULL )
      *pEventList = EvtLst;
    else
      throw EvtLst;
  }
}

//-----------------------------------------------------------------------------

teo28_ProfileContainer::~teo28_ProfileContainer()
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::~teo28_ProfileContainer"));

  if ( m_pContainer != NULL )
  {
    if ( m_Open == true )
    {
      teo200_EventList EvtLst;
      m_pContainer->CloseContainer(&EvtLst);
    }
    delete m_pContainer;
  }
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_GetOwnerName( tsp00_Namec & Username,
                                                teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_GetOwnerName"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == false )
  {
    if ( (m_Open = m_pContainer->OpenContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->GetOwnerName( Username, pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_GetGroupName( tsp00_Namec & Groupname,
                                                teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_GetGroupName"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == false )
  {
    if ( (m_Open = m_pContainer->OpenContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->GetGroupName( Groupname, pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_SetGroupName( tsp00_Namec const & Groupname,
                                                teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_SetGroupName"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == false )
  {
    if ( (m_Open = m_pContainer->OpenContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->SetGroupName( Groupname, pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_SetProfile( tsp00_C18c const      UserName,
                                              tsp00_Int4 const      Length,
                                              void * const          pProfile,
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_SetProfile"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == false )
  {
    if ( (m_Open = m_pContainer->OpenContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->SetProfile( UserName, Length, pProfile, pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_DelProfile( tsp00_C18c const      UserName, 
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_DelProfile"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == false )
  {
    if ( (m_Open = m_pContainer->OpenContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->DelProfile( UserName, pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_GetProfile( tsp00_C18c const      UserName, 
                                              tsp00_Int4            &Length,
                                              void * const          pProfile,
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_GetProfile"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == false )
  {
    if ( (m_Open = m_pContainer->OpenContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->GetProfile( UserName, Length, pProfile, pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_ScanProfiles( tsp00_Uint4         &Token,
                                                tsp00_Int4          &Length,
                                                void * const        pProfile,
                                                tsp00_C18c          &UserName, 
                                                teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_ScanProfiles"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == false )
  {
    if ( (m_Open = m_pContainer->OpenContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->ScanProfiles( Token, Length, pProfile, UserName, pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_DeleteProfileContainer( teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_DeleteProfileContainer"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == true )
  {
    m_Open = false;

    if ( (m_pContainer->CloseContainer ( pEventList )) == false )
      return false;
  }

  return m_pContainer->DeleteContainer( pEventList );
}

//-----------------------------------------------------------------------------

bool teo28_ProfileContainer::eo28_CloseProfileContainer( teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo28_ProfileContainer::eo28_CloseProfileContainer"));

  CLEAR_EVENTLIST_EO200(pEventList);

  if ( m_Open == true )
  {
    m_Open = false;

    return m_pContainer->CloseContainer ( pEventList );
  }

  return true;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
