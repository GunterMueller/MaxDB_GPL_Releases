/*****************************************************************************/
/*!
  @file   Kernel_Dump.hpp

  @author TorstenS

  @brief This class is used to write main memory structures into
                the kernel dump file.

  special area: Kernel dump file handling

  last changed: 2001-06-07  18:54
  see also    :

\if EMIT_LICENCE
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

\endif
*/
/****************************************************************************/


#ifndef KERNEL_DUMP_HPP
#define KERNEL_DUMP_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_DumpPage.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "DataAccess/Data_PageFrame.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 *
 * @class Kernel_Dump
 *
 * @brief This class is used to write main memory structures into the
 * kernel dump file.
 */
class Kernel_Dump
{
public:
    /// The different dump types.
    enum Kernel_DumpEntryId  // changes will affect Kernel_DumpInfo too!!
    {
        DmpNil = 0,
        DmpConverterManager,
        DmpConverterMapControl,
        DmpConverterMapSection,
        DmpConverterMapEntry,
        DmpConverterIndex,
        DmpFBMManager,
        DmpFBMDataDevice,
        DmpPagerController,
        DmpFileDirectory,
        DmpSQLTempFileCollection,
		DmpEnd
    };

    /*!
     * @name Constructors and initialization
     */
    /*@{*/

    /*!
     * The standard constructor of the kernel dump file.
     *
     * @param FileName  [in] Name of the kernel dump file
     * @param PageFrame [in] I/O capable buffer for the dump entries
     */
    Kernel_Dump(
        const tsp00_VFilename  &FileName,
        const Data_PageFrame   &PageFrame )
        :
        m_File( FileName ),
        m_Page( PageFrame ),
        m_PageNo( 0 ),
        m_LastEntryId( DmpNil )
        {}

    /*!
     * This constructor of the kernel dump file should be used to
     * access a kernel dump file that is already opened (e.g. a dump
     * file that has been opened using Pascal).
     *
     * @param FileName   [in] Name of the kernel dump file
     * @param PageFrame  [in] I/O capable buffer for the dump entries
     * @param dumpFileNo [in] fileNo of the already opened dump file
     * @param dumpPageNo [in] current pageNo of the dump
     */
    Kernel_Dump(
        const tsp00_VFilename  &FileName,
        const Data_PageFrame   &PageFrame,
        const SAPDB_Int4        dumpFileNo,
        const SAPDB_Int4        dumpPageNo);

    /*!
     * This is the destructor of the kernel dump file. The last
     * updated dump page is filled and written into the kernel dump
     * file.
     */
    ~Kernel_Dump()
    {
        Flush();
    }

    /*@}*/

    /*! @name Update operations */
    /*@{*/

    /*!
     * This method inserts a given dump entry in the kernel dump
     * file. Each dump entry belongs to a group of dump informations
     * identified by a DumpEntryId. Each dump page contains only the
     * dump entries of one DumpEntryId. If a new DumpEntryId is given
     * the old dump page is written into the file and the new entry is
     * stored on a fresh dump page together with the internal dump
     * label which is build by the given DumpEntryId. It is assumed
     * that dump entries of the same group are written stepwise, for
     * instance each entry of a data cache control block is written
     * with a single InsertEntry call! Dump entries are limited in
     * size.  The upper bound is delimited by the page size less than
     * about 20 bytes.
     *
     * @param DumpEntryId [in] identification of the given entry
     * @param DumpEntry   [in] given dump entry
     * @return true if insert was successfully; else false
     */
    SAPDB_Bool  InsertEntry(
        Kernel_DumpEntryId              DumpEntryId,
        const Kernel_DumpPage::Entry    &DumpEntry );

    /*!
     * This method inserts a given page in the kernel dump file. If
     * the current dump page is not completely filled with dump
     * entries this dump page will be filled with up with a special
     * token and then the page is flushed before the given page is
     * written into the kernel dump file.
     *
     * @param Page [in] kernel page handler
     * @return true if insert was successfully; else false
     */
    SAPDB_Bool InsertPage( const Kernel_Page  &Page );

    /*@}*/

    /*! @name Miscellaneous operations */
    /*@{*/

    /*!
     * The current dump page is filled with a special token and
     * than it is written into the kernel dump file.
     */
    void Flush();

    /*!
     * This method is used to get the kernel dump file error state.
     *
     * @return true if a error has occured
     */
    SAPDB_Bool  ErrorOccured()
    {
        return( m_File.ErrorOccured() );
    }

    /*!
     * This method returns the current dump page number. It is
     * only used by the old pascal dump.
     *
     * @return the current dump page number
     */
    SAPDB_UInt4 GetPageNo() const
    {
        return( m_PageNo );
    }

    /*!
     * This method returns the kernel dump file identifier. It is
     * only used by the old pascal dump.
     *
     * @return the current dump file identifier
     */
    SAPDB_UInt4 GetFileNo() const
    {
        return( m_File.GetFileNo() );
    }
    /*@}*/
private:

    /// This is the handler for the kernel dump file.
    Kernel_FileIO   m_File;

    /// This is the current dump page on which the dump entries will
    /// be stored.
    Kernel_DumpPage m_Page;

    /// This is the page number of the current dump page.
    SAPDB_UInt4 m_PageNo;

    /// This is the unique identifier for the dump entries stored at
    /// this time.
    Kernel_DumpEntryId  m_LastEntryId;
};

#endif  /* KERNEL_DUMP_HPP */
