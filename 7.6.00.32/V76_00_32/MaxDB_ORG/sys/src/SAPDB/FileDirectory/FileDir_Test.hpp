/*****************************************************************************/
/*!

  @file         FileDir_Test.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for FileDir_Test.cpp.

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
/*****************************************************************************/

#ifndef FILEDIR_TEST_HPP
#define FILEDIR_TEST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;

class FileDir_IDirectory;
class FileDir_Directory;
class FileDir_Table;
class FileDir_Index;
class FileDir_Oms;
class FileDir_OmsVar;
class FileDir_OmsCont;
class FileDir_OmsFixed;
class Kernel_BlobDescription;
class Kernel_IndexDescription;
class Converter_Version;

/*!
 * @class FileDir_Test
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief This class provides some basic tests of the file directory
 *        functionality.
 */
class FileDir_Test
{
public:
    /*!
     * @brief Runs a file directory functionality test.
     *
     * @param transContext [in] transaction context of calling task
     * @param scenario     [in] determines which test is run
     */
    static void Test(
        tgg00_TransContext& transContext,
        const int scenario);

private:
    FileDir_Test() {};

    /// number of tasks working on the file directory
    static RTESync_InterlockedCounter<SAPDB_UInt> m_activeTasks;
    static bool m_crashOnFlush;   ///< true if db will crash on next savepoint
    static bool m_restarted;      ///< true if restart done
    static bool m_shuttingDown;   ///< true if shutdown phase is in progress
    static FileDir_Table m_refCounterFileEntry; ///< smart pointer used in ref counter test
    static void Assert(bool mustBeTrue);

    static void TableCreator(
        const FileDir_FileNo& fileId,
        const Converter_Version& converterVersion,
        SAPDB_Int4& rootPage,
        tgg00_FiletypeSet& fileTypeSet,
        tgg91_FileVersion& fileVersion,
        Kernel_BlobDescription& blobs);

    static void IndexCreator(
        const FileDir_FileNo fileId,
        const FileDir_FileNo parentFileId,
        const Converter_Version& converterVersion,
        SAPDB_Int4& rootPage,
        tgg00_FiletypeSet& fileTypeSet,
        tgg91_FileVersion& fileVersion,
        Kernel_IndexDescription& indexes);

    static void OmsCreator(
        const FileDir_FileNo& fileId,
        const Converter_Version& converterVersion,
        SAPDB_Int4& rootPage,
        SAPDB_UInt2& objBodySize,
        SAPDB_UInt2& objPerPageCount,
        SAPDB_UInt2& chainCount);

    static void OmsVarCreator(
        const FileDir_FileNo& fileId,
        const Converter_Version& converterVersion,
        SAPDB_Int4& rootPage,
        SAPDB_UInt2& objBodySize,
        SAPDB_UInt2& objPerPageCount,
        SAPDB_UInt2& chainCount);

    static void OmsContCreator(
        FileDir_IDirectory& fd,
        const FileDir_FileNo& fileId,
        const Converter_Version& converterVersion,
        const FileDir_FileNo& omsVarFileId,
        SAPDB_Int4& rootPage,
        SAPDB_UInt2& objBodySize,
        SAPDB_UInt2& objPerPageCount,
        SAPDB_UInt2& chainCount,
        SAPDB_UInt2& partitionNo);

    static bool VerifyTableEntry(
        SAPDBMem_IRawAllocator& alloc,
        FileDir_Table& p);

    static bool VerifyIndexEntry(
        SAPDBMem_IRawAllocator& alloc,
        FileDir_Index& p);

    static bool VerifyOmsEntry(
        SAPDBMem_IRawAllocator& alloc,
        FileDir_Oms& p);

    static bool VerifyOmsVarEntry(
        SAPDBMem_IRawAllocator& alloc,
        FileDir_OmsVar& p);

    static bool VerifyOmsContEntry(
        FileDir_IDirectory& fd,
        SAPDBMem_IRawAllocator& alloc,
        FileDir_OmsCont& p);

    static int VerifyFileDirectory(
        SAPDBMem_IRawAllocator& alloc,
        const Converter_Version& converterVersion,
        FileDir_IDirectory& fd);

    static int CreateTest(SAPDBMem_IRawAllocator& m_alloc,
                          tgg00_TransContext& transContext,
                          const Converter_Version& converterVersion,
                          FileDir_IDirectory& fd);

    static int VerifyTest(SAPDBMem_IRawAllocator& m_alloc,
                          tgg00_TransContext& transContext,
                          const Converter_Version& converterVersion,
                          FileDir_IDirectory& fd);

    static int ReaderTest(SAPDBMem_IRawAllocator& m_alloc,
                          tgg00_TransContext& transContext,
                          FileDir_IDirectory& fd);

    static int AlterTest(
        SAPDBMem_IRawAllocator& alloc,
        tgg00_TransContext& transContext,
        FileDir_IDirectory& fd,
        Converter_Version& converterVersion);

    static int AddTest(
        SAPDBMem_IRawAllocator& alloc,
        tgg00_TransContext& transContext,
        FileDir_IDirectory& fd,
        Converter_Version& converterVersion);

    static int RemoveTest(
        SAPDBMem_IRawAllocator& alloc,
        tgg00_TransContext& transContext,
        FileDir_IDirectory& fd);

    static int ClearTest(SAPDBMem_IRawAllocator& m_alloc,
                         tgg00_TransContext& transContext,
                         const Converter_Version& converterVersion,
                         FileDir_IDirectory& fd);

    static void InitRefCounterTest(
        tgg00_TransContext& transContext,
        FileDir_IDirectory& fd);

    static void RefCounterTestTask(tgg00_TransContext& transContext);

    static SAPDB_UInt EvalRefCounterTest(
        tgg00_TransContext& transContext,
        FileDir_IDirectory& fd);

};

#endif // FILEDIR_TEST_HPP
