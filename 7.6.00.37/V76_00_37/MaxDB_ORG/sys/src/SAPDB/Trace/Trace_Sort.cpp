/*!**************************************************************************

  module       : Trace_Sort.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Kernel Trace
  description : Tracefile Converter

  Knltrace files are written from several UKTs in parallel. 

  last changed: 2001-01-04  10:24
  see also: PTS 1108768
  first created:2000-11-28  10:56

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG



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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#  include "Trace/Trace_Types.hpp"
#  include "Trace/Trace_InfoPage.hpp"
#  include "Trace/Trace_Page.hpp"
#  include "Trace/Trace_Entry.hpp"
#  include "geo54.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define INVALID_PAGE_NO 0
#define KNLTRACE_PAGE_SIZE 8192

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static void Usage();
static void Page0IO ( FILE *fp, bool doRead );
static SAPDB_Byte *AlignToPageSize(SAPDB_Byte *ptr);
static SAPDB_Char *SAPDB_UInt8ToAscii ( SAPDB_UInt8 arg, SAPDB_Int4 base );
static void ParseArguments(int argc, char *argv[], char * & unsorted, char * & sorted);


//---------------------------------------------------------------------------*/

Trace_Page unassignedPage;
const char      *NoMessage = 0;
const SAPDB_Int4 magicKnltrace = TRACE_INFOPAGE_MAGIC;
const SAPDB_Int4 firstEntryOffset = 4;
const SAPDB_Int4 ioBlockingFactor = 4;
SAPDB_Int4       verboseLevel = 0;

//--------------------------------------------------------
class ceo957traceSorter
{
public:
  /*
    Function: ceo957traceSorter
    Description: ctor Read the info page of the given trace file
    Arguments: fp [in] unsorted file
   */
  ceo957traceSorter(FILE *fp);

  /*
    Function: Setup
    Description: Setup the internal structures for accessing the unsorted file
   */
  void Setup(FILE *fp);

  /*
    Function: Scan
    Description: Scan the unsorted file
    
    In each UKT section of the unsorted file the oldest page is searched.
    The offset to the begin of the UKT section (as page number), is put
    into m_UktOldestPageOffset array. Beginning with this page, the entries
    are ordered.
   */
  void Scan();

  /*
    Function: MergeSort
    Description: MergeSort the unsorted file
    
    In each UKT section of the unsorted file the oldest page is searched.
    The offset to the begin of the UKT section (as page number), is put
    into oldestPageIndex array. Beginning with this page, the entries
    are ordered.
   */
  void MergeSort(FILE *resultFile);

  /*
    Function: CopyBack
    Description: CopyBack the sorted file to the unsorted file
    Arguments: dest [out] Destination file
               src  [in]  Source file
   */
  void CopyBack(FILE *dest, FILE *src);

private:
  /*
    Function: FillActivArray
    Description: Fill UKT index array
    
    Each UKT segement is allowed to be empty. In such a case, it is not
    part of the merging. During the merge some UKTs may be preempted. With
    the indirection of the activ array only those UKTs are merged, that still
    contain entries to be merged.

    Arguments: activArray [out] Array of UKT index containing entries
               totalActiv [out] Total number of elements setup in array
   */
  void FillActivArray(SAPDB_Int4 * &activArray, SAPDB_Int4 & totalActiv);

  /*
    Function: FindLowestSequenceNo
    Description: Find the lowest sequence number of the given UKT segments

    With each call the sequence number will be modified. 

    Arguments: activArray [inout] Array of UKT index containing entries
               totalActiv [inout] Total number of elements in activArray
               validLowestSequence [in] flag is true if first call (lastLowestSequence invalid)
               lastLowestSequence [in] the last result (or 0 if first call)
               lowestSequence [out] The choosen sequence number
               lowestIndex [out] The index of the choosen UKT
    Return value: true if lowestSequence found, false if no more entries
   */
  bool FindLowestSequenceNo(SAPDB_Int4 *activArray, 
                            SAPDB_Int4 & totalActiv,
                            bool        validLowestSequence,
                            Trace_SeqNo lastLowestSequence,
                            Trace_SeqNo &lowestSequence,
                            SAPDB_Int4 &lowestIndex);

  /*
    Class: uktTraceInfo
    Description: Class the contains internal status of an single UKT segment in input file

    The class handles a single UKT segment and allows to scan it. It is used in two
    calling seqeunces. During Scan() the usedPages, totalPages, oldestPageIndex and
    firstPageOffset are initialized. 
   */
  struct uktTraceInfo
  {
    // Initialized during 'Scan'
    SAPDB_Int4   usedPages;
    SAPDB_Int4   totalPages; 
    SAPDB_Int4   oldestPageIndex;
    SAPDB_Int4   firstPageOffset;

    // Initialized during 'Sort'
    SAPDB_Int4              readPages;
    Trace_SeqNo             currentSequence;
    SAPDB_Byte            * pageBuffer;
    Trace_PageEntryIterator currentEntry;
    Trace_Page              pageWrapper;

    /*
      Function: InitializeSort
      Description: Initialize internal variables
      
      If no used pages, only reset values. Otherwise allocate an aligned page buffer,
      reset counter and let PrepareNextEntry() do the rest.

      Arguments: fp [in] The input file
     */
    void InitializeSort(FILE *fp)
    {
      if ( usedPages > 0 )
      {
        pageBuffer = new SAPDB_Byte [KNLTRACE_PAGE_SIZE];
        if ( !pageBuffer )
        {
          throw "Not enough space for UKT pageBuffer";
        }

        readPages = 0;
        currentEntry.Assign(unassignedPage);
        pageWrapper.AssignPage((tsp00_PageAddr)0);
        PrepareNextEntry(fp);
      }
      else
      {
        readPages = usedPages;
        currentSequence = 0;
        pageBuffer = 0;
        currentEntry.Assign(unassignedPage);
        pageWrapper.AssignPage((tsp00_PageAddr)0);
      }
    }

    /*
      Function: PrepareNextEntry
      Description: Update internal variables to next trace entry

      If pageWrapper has still entry, a call to NextEntry() does the job.
      If not, the next page is read in and pageWrapper is Setup() to
      first entry. First entry is checked to be valid., that means empty
      pages are skipped.

      Arguments: fp [in] The input file
     */
    void PrepareNextEntry(FILE *fp)
    {
      // if entry is setup, move to next entry
      if ( !currentEntry.End() )
      {
        ++currentEntry;
      }

      // if no current entrys, try to read next page
      while ( currentEntry.End() && readPages < usedPages )
      {
        if ( fseek(fp, ( firstPageOffset + ((oldestPageIndex+readPages)%totalPages) ) * KNLTRACE_PAGE_SIZE, SEEK_SET) != 0 )
        {
          perror("Bad seek on input file:");
          printf("target position %d\n", ( firstPageOffset + ((oldestPageIndex+readPages)%totalPages) ) * KNLTRACE_PAGE_SIZE );
          throw "Bad seek on input file";
        }
        if ( fread(pageBuffer, KNLTRACE_PAGE_SIZE, 1, fp) != 1 )
        {
          printf("Failed to read page %d\n", (oldestPageIndex+readPages)%totalPages );
          throw "Bad read on file to sort";
        }
        ++readPages;

        pageWrapper.AssignPage((tsp00_PageAddr)pageBuffer);
        currentEntry.Assign(pageWrapper);
      }
    }

  };

  uktTraceInfo *m_uktTraceInfo;

  FILE *m_FileToSort;
  FILE *m_ResultFile;
  Trace_InfoPage m_InfoPage;

  SAPDB_Int4 *m_UktOldestPageOffset; // array storing oldest page offset of UKT
  SAPDB_Int4 *m_UktUsedPages; // array storing number of used pages of UKT
  SAPDB_Int4 m_FileSize; // Size of File to be sorted
};

/*---------------------------------------------------------------------------*/

/*
  Class: ceo957resultFile
  Description: This class provides all means to fill the result file

  Once initialized with an already open file, a loop over all trace entries
  to fill in is done. Each entry is first size checked with CheckSize(), 
  allowing the class to reposition its internal write marker correctly. 
  With 'CheckInitialized' the current page is initalized (pageNo is setup
  and increased), if necessary. With the FillEntry() call the given entry
  is copied. Automatically flushing of ioBlockFactor pages is done during the
  loop.

  Last must should be Flush() to save the last pages.
 */

class ceo957resultFile
{
public:
  /*
    Function: ceo957resultFile
    Description: Create the result buffer
    
    Allocate memory for output and aligned it.
   */
  ceo957resultFile(FILE *fp):
      m_resultPageNo(1),
      m_resultPagesFilled(0),
      m_resultPageInitialized(false),
      m_resultBuffer(new SAPDB_Byte[KNLTRACE_PAGE_SIZE * (ioBlockingFactor+1)]),
      m_resultFile(fp),
      m_totalSaved(0)
  {
    m_resultBuffer = AlignToPageSize(m_resultBuffer);
    memset(m_resultBuffer, 0, KNLTRACE_PAGE_SIZE * ioBlockingFactor);

    Page0IO(fp, false);
  }

  /*
    Function: FillEntry
    Description: Fill the given content of the source entry into current result page
    Arguments: sourceEntry [in] source entry
   */
  void FillEntry( Trace_IFullEntry &sourceEntry)
  {
    CheckInitialized();

    if ( false == m_resultPage.Insert(sourceEntry) )
    {
      NextPage();

      if ( verboseLevel > 1 )
      {
        printf("Result page %d filled\n", m_totalSaved + m_resultPagesFilled );
      }

      if ( false == m_resultPage.Insert(sourceEntry) )
      {
        throw "Insert on fresh page failed";
      }
    }

  }

  /*
    Function: Flush
    Description: Flush the remaining pages to result file
   */
  void Flush()
  {
    if ( true == m_resultPageInitialized )
    {
      ++m_resultPagesFilled;
    }
    if ( m_resultPagesFilled > 0 )
    {
      fwrite(m_resultBuffer, KNLTRACE_PAGE_SIZE, m_resultPagesFilled, m_resultFile);
      m_totalSaved += m_resultPagesFilled;
      m_resultPagesFilled = 0;
    }
  }

  /*
    Function: GetTotalSaved
    Description: Get total number of pages saved to result file
   */
  SAPDB_Int4 GetTotalSaved() const { return m_totalSaved; }

private:
  SAPDB_Int4              m_resultPageNo;
  Trace_Page              m_resultPage;
  SAPDB_Int4              m_resultPagesFilled;     // Number of pages completely filled
  bool                    m_resultPageInitialized; // Current result initialized if true
  SAPDB_Byte            * m_resultBuffer;
  FILE                  * m_resultFile;
  SAPDB_Int4              m_totalSaved;

  /*
    Function: NextPage
    Description: Setup next page to fill

    If the space is not sufficient, the next page in the buffer is prepared.
    A new prepared result buffer page should always have sufficient space. If the new
    page does not exist in the resultBuffer, the result buffer is flushed first.
   */
  void NextPage()
  {
    ++m_resultPagesFilled;
    if ( m_resultPagesFilled == ioBlockingFactor )
    {
      if ( fwrite(m_resultBuffer, KNLTRACE_PAGE_SIZE, ioBlockingFactor, m_resultFile) != ioBlockingFactor )
      {
        printf("Saving to result file failed after %d pages", m_totalSaved);
        throw "Saving result failed";
      }
      m_resultPagesFilled = 0;
      m_totalSaved += ioBlockingFactor;
    }
    m_resultPageInitialized = false;

    CheckInitialized();
  }

  /*
    Function: CheckInitialized
    Description: Check that current resultPage is properly initialized

    Initialization means to clear the content to zero, increase result page number
    and setup result trace entry to first entry in page.
   */
  void CheckInitialized()
  {
    if ( false == m_resultPageInitialized )
    {
      memset( &m_resultBuffer[m_resultPagesFilled*KNLTRACE_PAGE_SIZE],
              0, KNLTRACE_PAGE_SIZE );
      m_resultPage.AssignNewPage( (tsp00_PageAddr)&m_resultBuffer[m_resultPagesFilled*KNLTRACE_PAGE_SIZE], m_resultPageNo );
      ++m_resultPageNo;
      m_resultPageInitialized = true;
    }
  }

};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

int main(int argc, char *argv[])
{
  FILE *unsortedFile;
  FILE *sortedResultFile;
  setbuf(stdout, 0);
  setbuf(stderr, 0);
  char *unsortedFileName;
  char *sortedFileName;
  try {

    ParseArguments(argc, argv, unsortedFileName, sortedFileName);

    /* Open file readonly if both files are given, but read+write if only one given */
    // read+write if copied back after allows to check permissions on open
    // see calling of theSorter.CopyBack() below
    unsortedFile = fopen( unsortedFileName, sortedFileName == (char *)0 ? "rb" : "r+b");

    if ( NULL == unsortedFile )
    {
      printf("Failed to open unsorted file %s\n", unsortedFileName);
      throw NoMessage;
    }

    ceo957traceSorter theSorter(unsortedFile);

    theSorter.Scan();

    if ( verboseLevel )
    {
      printf("Scan completed\n\n");
    }

    if ( sortedFileName != (char *)0 )
    {
      sortedResultFile = fopen(sortedFileName, "wb");
      if ( NULL == sortedResultFile )
      {
        printf("Failed to open sorted file %s\n", sortedFileName);
        throw NoMessage;
      }

      theSorter.MergeSort(sortedResultFile);

      if ( verboseLevel )
      {
        printf("MergeSort completed\n\n");
      }

    }
    else
    {
      /* first open a temporary file, then sort into it and later copy it back */
      /* it is automatically deleted on close */
      sortedResultFile = tmpfile();
      if ( NULL == sortedResultFile )
      {
        throw "Failed to open temporary result file";
      }

      theSorter.MergeSort(sortedResultFile);

      fclose(unsortedFile);

      if ( verboseLevel )
      {
        printf("\nCopy back temporary file to unsorted file\n");
      }

      unsortedFile = fopen( unsortedFileName, "wb" );
      theSorter.CopyBack(unsortedFile, sortedResultFile);
    }

    fclose(sortedResultFile);

    fclose(unsortedFile);

    if ( verboseLevel )
    {
      printf("\ntracesort completed\n");
    }
  }

  catch(const char *msg)
  {
    if ( msg != 0 ) printf("ERROR:%s\n", msg);
    Usage();
  }
  return 0;
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

static void Usage()
{
  printf("Syntax: tracesort [<option>] knltrace_unsorted [knltrace_sorted]\n");
  printf("Usage: Sort the knltrace entries given in file knltrace_unsorted\n");
  printf("       and put the sorted result into knltrace_sorted. If second file\n");
  printf("       is not given, the sort is done in place.\n");
  printf("Option: -vN Verbosity level (default only warnings and errors are reported)\n");
  printf("        -v or -v1 show summary of scan and sort\n");
  printf("        -v2 additionally show all completed result pages\n");
  printf("        -v3 additionally show each 100th parsed entry\n");
  printf("        -v3 additionally show all found entries\n");

  exit(1);
}

/*---------------------------------------------------------------------------*/

static void
Page0IO ( FILE       *fp,
          bool        doRead )
{
  bool  ok = true;
  SAPDB_Byte *page0buffer = new SAPDB_Byte[KNLTRACE_PAGE_SIZE * 2];
  SAPDB_Byte *page0ptr = AlignToPageSize(page0buffer);

  SAPDB_Int4 expectedBufferSize = KNLTRACE_PAGE_SIZE;

  if ( doRead )
  {
    if ( fread(page0ptr, KNLTRACE_PAGE_SIZE, 1, fp) != 1 )
    {
      throw "Reading page zero failed";
    }

    SAPDB_Int4 bufferSizeRead;
#if defined(_WIN32)
    bufferSizeRead = ((unsigned char)page0ptr [4] << 24) 
                   + ((unsigned char)page0ptr [5] << 16)
                   + ((unsigned char)page0ptr [2] << 8) 
                   +  (unsigned char)page0ptr [3];
#else
    teo54Page0Info *pInfo = (teo54Page0Info *)(page0ptr + PAGE_0_RECORD_OFFSET_EO54);

    /*
     * Compatibility with old tapes must be enforced...
     */
    if ( !strcmp(pInfo->magicHeader_eo54, MAGIC_HEADER_EO54) )
    {
        /* New Page0 Layout */
        bufferSizeRead = eo54RestoredInt4(pInfo->blockSize_eo54);
    }
    else
    {
        /* Old Page0 Layout contained only two bytes for bufferSize */
        expectedBufferSize &= 0x0000FFFF;
        bufferSizeRead =
          ((unsigned char)(((teo54OldUnixPage0Info *)page0ptr)->blockSizeByte1_Ueo54) << 8)
         | (unsigned char)(((teo54OldUnixPage0Info *)page0ptr)->blockSizeByte0_Ueo54);
    }
#endif /* _WIN32 */
    if (bufferSizeRead == expectedBufferSize)
    {
      ok = true;
    }
    else 
    {
      printf("Found unexpected page size on page zero:%d\n", bufferSizeRead);
      throw "Unexpected page size";
    }
  }
  else
  {
    SAPDB_Int4 bufferSize = KNLTRACE_PAGE_SIZE;

    memset (page0ptr, '\0', KNLTRACE_PAGE_SIZE);
#if defined(_WIN32)
    page0ptr [3] = (unsigned char) (  bufferSize        & 0xff);
    page0ptr [2] = (unsigned char) ( (bufferSize >> 8)  & 0xff);
    page0ptr [5] = (unsigned char) ( (bufferSize >> 16) & 0xff);
    page0ptr [4] = (unsigned char) ( (bufferSize >> 24) & 0xff);
#else
    {
      teo54Page0Info *pInfo = (teo54Page0Info *)(page0ptr + PAGE_0_RECORD_OFFSET_EO54);
      pInfo->blockSize_eo54 = eo54SavableInt4(bufferSize);
    }
#endif /* _WIN32 */
    if ( fwrite(page0ptr, KNLTRACE_PAGE_SIZE, 1, fp) != 1 )
    {
      throw "Saving page zero failed";
    }

    // Add an empty page...
    memset (page0ptr, '\0', KNLTRACE_PAGE_SIZE);
    if ( fwrite(page0ptr, KNLTRACE_PAGE_SIZE, 1, fp) != 1 )
    {
      throw "Saving empty page failed";
    }
    // should be removable...
  }
}    

/*---------------------------------------------------------------------------*/

void ceo957traceSorter::Setup(FILE *fp)
{
  if ( fseek(fp, 0, SEEK_END) != 0 )
  {
    perror("fseek to end of input file:");
    throw "Bad seek to end of input file";
  }

  m_FileSize = ftell(fp);

  if ( -1 == m_FileSize )
  {
    perror("ftell offset of end of input file:");
    throw "Bad ftell on input file";
  }

  if ( 0 == m_FileSize )
  {
    throw "Inputfile empty";
  }

  if ( fseek(fp, 0, SEEK_SET) != 0 )
  {
    perror("fseek to begin of input file:");
    throw "Bad seek to begin of file";
  }

  Page0IO(fp, true);

  Trace_InfoPage::Layout header;

  if ( fread(&header, sizeof(Trace_InfoPage::Layout), 1, fp) != 1 )
  {
    throw "Failed to read header";
  }

  m_InfoPage.AssignPage(REINTERPRET_CAST(tsp00_PageAddr,&header));

  if ( !m_InfoPage.CheckMagic() )
  {
    throw "Invalid magic for unsorted knltrace";
  }

  if ( !m_InfoPage.CheckPageSize() )
  {
    printf("Invalid page size: %d\n", header.pageSize );
    throw "Bad page size";
  }

  if ( ( m_FileSize % header.pageSize ) != 0 )
  {
    printf("File size %d not multiple of pageSize %d\n",
      m_FileSize, header.pageSize );
    throw "Bad file size";
  }

  m_FileSize /= header.pageSize;

  if ( header.uktCount <= 0 )
  {
    printf("Invalid header: UKT count %d\n", header.uktCount );
    throw "Invalid header";
  }

  if ( fseek(fp, KNLTRACE_PAGE_SIZE, SEEK_SET) != 0 )
  {
    perror("fseek back to begin of input file:");
    throw "Bad seek back to begin of file";
  }

  SAPDB_Byte *infoPage = new SAPDB_Byte[KNLTRACE_PAGE_SIZE];

  if ( !infoPage )
  {
    perror("Allocation of info page wrapper failed:");
    throw "Bad alloc";
  }

  if ( fread(infoPage, KNLTRACE_PAGE_SIZE, 1, fp) != 1 )
  {
    throw "Bad read of first page";
  }

  m_InfoPage.AssignPage((tsp00_PageAddr)infoPage);

  SAPDB_Int4 totalPages = 2; /* First page is LZU page, second is info page */

  for ( int uktIndex = 0; uktIndex < header.uktCount; uktIndex++ )
  {
    if ( m_InfoPage.GetUktTraceSize(uktIndex) < 0 )
    {
      printf("Illegal header: Size entry for UKT%d %d pages\n", 
                uktIndex+1, m_InfoPage.GetUktTraceSize(uktIndex) );
      throw "Illegal header";
    }

    totalPages += m_InfoPage.GetUktTraceSize(uktIndex);
    
    if ( totalPages > m_FileSize )
    {
      printf("Illegal header: Total size larger than file for UKT%d total %d pages > %d pages\n", 
                uktIndex+1, totalPages, m_FileSize );
      throw "Illegal header";
    }
  }

  if ( totalPages != m_FileSize )
  {
      printf("Illegal header or file too large: Total size %d page File size %d pages)\n",
		  totalPages, m_FileSize );
      throw "File corrupted";
  }
}

/*---------------------------------------------------------------------------*/

ceo957traceSorter::ceo957traceSorter(FILE *fp)
{
  if ( !fp )
  {
    throw "No input file descriptor given";
  }

  m_FileToSort = fp;
  m_ResultFile = 0;
  Setup(fp);
  m_uktTraceInfo = new uktTraceInfo[m_InfoPage.GetUktCount()];
  if ( !m_uktTraceInfo )
  {
    throw "Allocation of offsets failed";
  }
  memset(m_uktTraceInfo, 0, m_InfoPage.GetUktCount() * sizeof(uktTraceInfo));
}

/*---------------------------------------------------------------------------*/

void ceo957traceSorter::Scan()
{
  SAPDB_Byte *pageBuffer = new SAPDB_Byte[KNLTRACE_PAGE_SIZE*2];
  pageBuffer = AlignToPageSize(pageBuffer);
  SAPDB_Int4 firstPageOffset;
  Trace_Page currentPage;

  if ( fseek(m_FileToSort, 2*KNLTRACE_PAGE_SIZE, SEEK_SET) != 0 )
  {
    perror("fseek to first UKT segement:");
    throw "Bad seek on input file";
  }

  firstPageOffset = 2;

  if ( verboseLevel )
  {
    printf("Scan: Input file contains %d UKT Slots\n", m_InfoPage.GetUktCount() );
  }

  for ( SAPDB_Int4 uktIndex = 0; 
        uktIndex < m_InfoPage.GetUktCount(); 
	++uktIndex )
  {
    SAPDB_Int4 uktSize = m_InfoPage.GetUktTraceSize(uktIndex);
    SAPDB_Int4 usedPages = -1;
    SAPDB_Int4 oldestPageIndex = -1;
    SAPDB_Int4 firstPageNo;
    SAPDB_Int4 lastPageNo;
    SAPDB_Int4 currentPageNo;

    if ( uktSize > 0 )
    {
        if ( fread(pageBuffer, KNLTRACE_PAGE_SIZE, 1, m_FileToSort) != 1 )
        {
          printf("Failed to first page on UKT%d\n", uktIndex+1);
          throw "Bad read on file to sort";
        }
        currentPage.AssignPage((tsp00_PageAddr)pageBuffer);
        firstPageNo = lastPageNo = currentPage.GetPageNo();

        for ( SAPDB_Int4 pageIndex = 1; pageIndex < uktSize; ++pageIndex )
        {
          if ( fread(pageBuffer, KNLTRACE_PAGE_SIZE, 1, m_FileToSort) != 1 )
          {
            printf("Failed to read page %d on UKT%d\n", pageIndex, uktIndex+1);
            throw "Bad read on file to sort";
          }
          currentPage.AssignPage((tsp00_PageAddr)pageBuffer);
          currentPageNo = currentPage.GetPageNo();

          if ( usedPages != -1 )
          {
            /* If we think we know all valid pages, rest must be 0 */
            if ( currentPageNo != INVALID_PAGE_NO )
              {
                printf("Corrupted PageNo for UKT%d PageNo %d Expected 0 (ignored)\n", 
                  uktIndex+1, currentPageNo );
              }
          }
          else if ( INVALID_PAGE_NO == currentPageNo )
          {
        if ( -1 == oldestPageIndex )
        {
          /* Now we think we know all valid pages */
          usedPages = pageIndex;
          oldestPageIndex = 0;
        }
        else
        {
          /* We found an oldest, but now we found 0, so we discard all oldest... */
          usedPages = oldestPageIndex;
          oldestPageIndex = 0;
          printf("Corrupted PageNo for UKT%d PageNo 0 Expected %d (ignored)\n", 
            uktIndex+1, lastPageNo+1 );
        }
          }
          else if ( currentPageNo == (lastPageNo+1) )
          {
        /* normal during scan */
        lastPageNo = currentPageNo;
          }
          else if ( currentPageNo == (firstPageNo + pageIndex - uktSize) )
          {
        /* We found the oldest */
            oldestPageIndex = pageIndex;
        lastPageNo = currentPageNo;
          }
          else 
          {
        /* a corrupted is handled as a 0 pageNo */
        usedPages = pageIndex;
        oldestPageIndex = 0;
        printf("Corrupted PageNo for UKT%d PageNo %d Expected %d (ignored)\n", 
            uktIndex+1, currentPageNo, lastPageNo+1 );
          }
        }
    }

    m_uktTraceInfo[uktIndex].usedPages = 
         ( -1 == usedPages ? uktSize : usedPages );
    m_uktTraceInfo[uktIndex].totalPages =  uktSize;

    m_uktTraceInfo[uktIndex].oldestPageIndex = 
         ( -1 == oldestPageIndex ? 0 : oldestPageIndex );
    m_uktTraceInfo[uktIndex].firstPageOffset =  firstPageOffset;

    if ( verboseLevel > 1 )
    {
      printf("Scan: UKT Slot %d used %d of %d page%s oldest page %d file offset page %d\n",
        uktIndex,
        m_uktTraceInfo[uktIndex].usedPages,
        m_uktTraceInfo[uktIndex].totalPages,
        m_uktTraceInfo[uktIndex].totalPages == 1 ? "" : "s",
        m_uktTraceInfo[uktIndex].oldestPageIndex,
        m_uktTraceInfo[uktIndex].firstPageOffset );
    }

    firstPageOffset += uktSize;
  }
}

/*---------------------------------------------------------------------------*/

void ceo957traceSorter::FillActivArray(SAPDB_Int4 * &activArray, SAPDB_Int4 & totalActiv)
{
  SAPDB_Int4 uktIndex;   // Index of UKT
  SAPDB_Int4 uktCount    = m_InfoPage.GetUktCount();
  SAPDB_Int4 activIndex; // Index in activeArray
  activArray = new SAPDB_Int4[uktCount];
  memset(activArray, 0, sizeof(SAPDB_Int4) * uktCount);

  for ( activIndex = uktIndex = 0; uktIndex < uktCount; uktIndex++ )
  {
    m_uktTraceInfo[uktIndex].InitializeSort(m_FileToSort);
    if ( !m_uktTraceInfo[uktIndex].currentEntry.End() )
    {
      activArray[activIndex] = uktIndex;
      ++activIndex;
    }
    else
    {
      if ( verboseLevel > 1 )
      {
        printf( "UKT Slot %d empty\n", uktIndex);
      }
    }
  }
  totalActiv = activIndex;
  if ( verboseLevel )
  {
    printf( "%d UKT Slot%s entries\n", totalActiv, totalActiv == 1 ? " contains" : "s contain" );
  }
}

/*---------------------------------------------------------------------------
  Function: FindLowestSequenceNo
  Description: Find the lowest sequence number of the given UKT segments

  With each call the sequence number will be modified. 

  Arguments: activArray [inout] Array of UKT index containing entries
             totalActiv [inout] Total number of elements in activArray
             validLowestSequence [in] flag is true if first call (lastLowestSequence invalid)
             lastLowestSequence [in] the last result (or 0 if first call)
             lowestSequence [out] The choosen sequence number
             lowestIndex [out] The index of the choosen UKT
  Return value: true if lowestSequence found, false if no more entries
 */
bool ceo957traceSorter::FindLowestSequenceNo(SAPDB_Int4 *activArray, 
                                             SAPDB_Int4 & totalActiv,
                                             bool        validLowestSequence,
                                             Trace_SeqNo lastLowestSequence,
                                             Trace_SeqNo &lowestSequence,
                                             SAPDB_Int4 &lowestIndex )
{
  bool foundLowSequence = false;

  // search for lowest sequence number in activ slots
  for ( SAPDB_Int4 activIndex = 0; activIndex < totalActiv; ++activIndex )
  {
    Trace_SeqNo testSequence;

    while ( (totalActiv > 0) 
        && !m_uktTraceInfo[activArray[activIndex]].currentEntry.End() )
    {
      uktTraceInfo *currentTraceInfo = &m_uktTraceInfo[activArray[activIndex]];

      testSequence = (*(currentTraceInfo->currentEntry)).GetSequenceNo();

      if ( validLowestSequence && (testSequence <= lastLowestSequence) )
      {
        if ( testSequence != 0 )
        {
          printf( "Corrupted SequenceNumber %s in UKT%d page %d file page %d (ignored)\n",
            SAPDB_UInt8ToAscii((SAPDB_UInt8)testSequence,10),
            activArray[activIndex]+1,
            currentTraceInfo->readPages, 
            ( currentTraceInfo->firstPageOffset 
             + ((currentTraceInfo->oldestPageIndex
                + currentTraceInfo->readPages)
                 % currentTraceInfo->totalPages) )
          );
        }
        currentTraceInfo->PrepareNextEntry(m_FileToSort);

        if ( currentTraceInfo->currentEntry.End() )
        {
          --totalActiv;
          if ( verboseLevel > 1 )
          {
            printf( "UKT Slot %d now emptied, %d slot left\n", 
              activArray[activIndex], totalActiv);
          }
          activArray[activIndex] = activArray[totalActiv];
        }
      }
      else
      {
        break;
      }
    }

    if ( !m_uktTraceInfo[activArray[activIndex]].currentEntry.End() )
    {
      if ( false == foundLowSequence )
      {
        // first is always lowest
        lowestSequence = testSequence;
        lowestIndex = activIndex;
        foundLowSequence = true;
      }
      else if ( testSequence < lowestSequence )
      {
        // use only if lower
        lowestSequence = testSequence;
        lowestIndex = activIndex;
      }
    }
  }

  return foundLowSequence;
}

/*---------------------------------------------------------------------------
  Function: MergeSort
  Description: Merge the UKT slots of the unsorted input file into result

  Find non empty UKT slots and fill result sequence by sequence beginning with lowest
  sequence number.

  Arguments: resultFile [in] already opend result file
 */
void ceo957traceSorter::MergeSort(FILE *resultFile)
{
  SAPDB_Int4 *activArray = 0;
  SAPDB_Int4 totalActiv = 0;
  Trace_SeqNo           totalSequenceNumbers = 0;
  Trace_SeqNo           totalResultPages = 0;

  FillActivArray(activArray, totalActiv);

  bool                  validLowestSequence = false;
  Trace_SeqNo           lastLowestSequence = 0;
  Trace_SeqNo           lowestSequence = 0;
  SAPDB_Int4            lowestIndex = 0;

  ceo957resultFile      result(resultFile);

  while( totalActiv > 0 )
  {
    if ( true == FindLowestSequenceNo(activArray, 
                                      totalActiv, 
                                      validLowestSequence,
                                      lastLowestSequence, 
                                      lowestSequence,
                                      lowestIndex) )
    {
      validLowestSequence = true;
      lastLowestSequence = lowestSequence;

      ++totalSequenceNumbers;

      if ( verboseLevel >= 4 )
      {
        printf("Copy SeqNo %s UKT%d\n", 
          SAPDB_UInt8ToAscii((SAPDB_UInt8)lowestSequence,10), 
          activArray[lowestIndex]+1 );
      }
      else if ( verboseLevel == 3 && ((totalSequenceNumbers%100) == 0) )
      {
        printf("%s entries copied, last SeqNo %s UKT%d\n", 
          SAPDB_UInt8ToAscii((SAPDB_UInt8)totalSequenceNumbers,10),
          SAPDB_UInt8ToAscii((SAPDB_UInt8)lowestSequence,10), 
          activArray[lowestIndex]+1 );
      }

      // A valid entry is found, now we move it into result page
      uktTraceInfo & source = m_uktTraceInfo[activArray[lowestIndex]];

      Trace_PageEntry sourceEntry = *source.currentEntry;

      result.FillEntry(sourceEntry);

      source.PrepareNextEntry(m_FileToSort);

      if ( source.currentEntry.End() )
      {
        --totalActiv;
        if ( verboseLevel > 1 )
        {
          printf( "UKT Slot %d completed\n", activArray[lowestIndex] );
        }
        activArray[lowestIndex] = activArray[totalActiv];
      }
    }

  } /* totalActiv */

  result.Flush();

  if ( verboseLevel )
  {
    printf("Result file total: %s entr%s in %d page%s\n", 
      SAPDB_UInt8ToAscii((SAPDB_UInt8)totalSequenceNumbers, 10),
      totalSequenceNumbers == 1 ? "y" : "ies",
      result.GetTotalSaved(),
      result.GetTotalSaved() == 1 ? "" : "s" );
  }

}

/*---------------------------------------------------------------------------
  Function: CopyBack
  Description: Copy sorted temp file back into unsorted file
  Arguments: dest [in] destination (former unsorted file)
             source [in] sorted temp file
 */
void ceo957traceSorter::CopyBack(FILE *dest, FILE *source)
{
  SAPDB_Byte * copyBuffer = new SAPDB_Byte[KNLTRACE_PAGE_SIZE * (ioBlockingFactor+1)];
  copyBuffer = AlignToPageSize(copyBuffer);

  if ( fseek(dest, 0, SEEK_SET) != 0 )
  {
    throw "Bad seek to begin of destination during copy back";
  }

  if ( fseek(source, 0, SEEK_SET) != 0 )
  {
    throw "Bad seek to begin of tempfile during copy back";
  }

  SAPDB_Int4 readCount;
  SAPDB_Int4 writeCount;
  SAPDB_Int4 totalWritten = 0;
  do // loop with reading/writing ioBlockingFactor pages until all pages are copied
  {
    readCount = (SAPDB_Int4)fread(copyBuffer, KNLTRACE_PAGE_SIZE, ioBlockingFactor, source);
    if ( readCount > 0 )
    {
      // something was read, so we copy it back
      writeCount = (SAPDB_Int4)fwrite(copyBuffer, KNLTRACE_PAGE_SIZE, readCount, dest);
      if ( writeCount != readCount )
      {
        printf("Bad write (%d) after %d pages written\n", 
                ferror(dest), totalWritten + writeCount);
        throw "Bad write on copy back";
      }
      totalWritten += writeCount;
    }

    if ( readCount != ioBlockingFactor )
    { // if we are at end of file, 
      if ( feof(source) )
      {
        readCount = 0;
      }
      else if ( ferror(source) != 0 )
      {
        printf("Bad read (%d) after %d pages written\n", 
                ferror(source), totalWritten );
      }
    }

  } while ( readCount > 0 );
}

/*---------------------------------------------------------------------------*/

static SAPDB_Byte *AlignToPageSize(SAPDB_Byte *ptr)
{
  return ptr = ( (SAPDB_Byte *)0 ) 
             + ( ((ptr - (SAPDB_Byte *)0)+KNLTRACE_PAGE_SIZE-1)&~(KNLTRACE_PAGE_SIZE-1) );
}

/*---------------------------------------------------------------------------*/

static SAPDB_Char *SAPDB_UInt8ToAscii ( SAPDB_UInt8 arg, SAPDB_Int4 base )
{
    static SAPDB_Char tmpBuf[40];
    SAPDB_Int4        count = 1;

    if ( base == 0 ) base = 10;

    if ( base < 0 || base > 16 )
    {
      throw "Bad base for Uint8 conversion";
    }

    const SAPDB_Char *       digits = "0123456789ABCDEF";

    memset(tmpBuf, 0, sizeof(tmpBuf));

    while (arg > 0) 
    {
        tmpBuf[sizeof (tmpBuf) - count - 1] = digits[arg % base];
        ++count;
        arg /= base;
    }

    return &tmpBuf[sizeof (tmpBuf) - count];
}

/*---------------------------------------------------------------------------*/

#define OPT_LEADIN_CHAR '-'
static void ParseArguments(int argc, char *argv[], char * & unsorted, char * & sorted )
{
  bool gotUnsorted = false;
  bool gotSorted = false;
  int argIndex = 1;

  if ( argc < 2 )
  {
    throw NoMessage;
  }

  unsorted = (char *)0;
  sorted = (char *)0;
  while ( argIndex < argc )
  {
    char *currentArg = argv[argIndex++];
    if ( currentArg[0] == OPT_LEADIN_CHAR )
    {
      switch( currentArg[1] )
      {
      case 'v':
        if ( currentArg[2] )
          verboseLevel = currentArg[2] - '0';
        else
          verboseLevel = 1;
        break;
      default:
        throw NoMessage;
      }
    }
    else
    {
      if ( !gotUnsorted )
      {
        gotUnsorted = true;
        unsorted = currentArg;
      }
      else if ( !gotSorted )
      {
        gotSorted = true;
        sorted = currentArg;
      }
      else
      {
        throw "Only two filename allowed";
      }
    }
  }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
