/*****************************************************************************/
/*!
  @file     Kernel_FileIO.hpp
  @author   TorstenS

  @brief    This module is used to write a kernel page into the
            filesystem of the operating system.


  last changed: 2001-05-30  15:43
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


*/
/*****************************************************************************/


#ifndef KERNEL_FILEIO_HPP
#define KERNEL_FILEIO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "KernelCommon/Kernel_Common.hpp"

class IOMan_IPage;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

 /*!----------------------------------------------------------------------
 declarations: Predefined labels and extensions
-----------------------------------------------------------------------*/

#define KERNEL_CONV_FILE            "Conv"
#define KERNEL_DATA_FILE            "Data"
#define KERNEL_LOG_FILE             "Log"

#define KERNEL_BAD_FILE_EXTENSION   "bad"
#define KERNEL_COR_FILE_EXTENSION   "cor"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 *
 * @class  Kernel_FileIO
 * @author TorstenS
 *
 * This module is used to write a kernel page into the filesystem of
 * the operating system.
 *
 */

class Kernel_FileIO
{
public:

    /*!
     * This constructor is used to generate a file handle for kernel
     * page write operations into a OS file. The given file name
     * determines the name of the generated file. The contructor opens
     * implizit the given file. If no file is present it will be
     * generated, in other cases the file is overridden.
     *
     * @param  FileName [in] Name for the OS file
     */
    Kernel_FileIO( const tsp00_VFilename  &FileName );

    /*!
     * This constructor is used to generate a file handle for kernel
     * page write operations into a OS file. The filename is given in
     * parts, so that the call is able to build an individual filename
     * very effectively. The generated filename looks like
     * this "Data14888.bad" or "Conv-1-20876.bad". (See the predefined
     * labels and extensions in this module) The contructor opens
     * the given file implicitely. If no file is present it will be
     * generated, in other cases the file is overridden.
     *
     * @param Label      [in] Suffix of the filename
     * @param Extension  [in] Prefix of the filename
     * @param FirstName  [in] First part of the filename
     * @param SecondName [in] If given(optional) second part of the filename
     */
    Kernel_FileIO(
        const SAPDB_Char    *Label,
        const SAPDB_Char    *Extension,
        const SAPDB_Char    *FirstName,
        const SAPDB_Char    *SecondName = NULL);

    /*!
     * This constructor should be used to access a file that has
     * already been opened (e.g. in Pascal). No attempt will be made
     * to open the file and it will not be closed during destruction
     * of this object. Also, this object will not try to lock the
     * file.
     *
     * @param  FileName   [in] Name for the OS file
     * @param  dumpFileNo [in] fileNo of the already opened dump file
     */
    Kernel_FileIO(
        const tsp00_VFilename  &FileName,
        const SAPDB_Int4        dumpFileNo);

    /*!
     * The destructor closes the current file if necessary.
     */
    ~Kernel_FileIO()
    {
        Close();
    }

    /*!
     * This method is used to close a file if it is in open mode.
     */

    void Close();

    /*!
     * This method is used to write the memory block of a kernel
     *           page into the run directtory of the kernel.
     *
     * @param Page [in] Kernel page
     * @return true on success
     */
    SAPDB_Bool  Write( IOMan_IPage    &Page );

    /*!
     * This method is used to get the file internal error state.
     *
     * @return true if no error has occured
     */
    SAPDB_Bool  ErrorOccured()
    {
        return( vf_ok != m_FileError );
    }

    /*!
     * This method returns the OS file identifier.
     * @return FileNo
     */
    SAPDB_UInt4 GetFileNo() const
    {
        return( m_FileNo );
    }

private:

    /*!
     * This method is used to create or open the current file. If
     * the file is already open nothing happens.
     *
     * @return  true if file is in mode open.
     */
    SAPDB_Bool  Open();

    /*!
     * This method returns true if the file is in mode open.
     *
     * @return true if the file is in mode open
     */
    SAPDB_Bool  IsOpen() const
    {
        return( m_IsOpen );
    }

    SAPDB_Bool IsLocked() const
    {
        return( 0 != m_TaskId );
    }

    /*!
     * Lock resource, if it isn't locked yet.
     */
    void Lock();

    /*
     * Unlock resource, if it is locked
     */
    void Unlock();

private:

    /// current taskId for locking.
    tsp00_TaskId m_TaskId;

    /// file state. true is file is in mode open.
    SAPDB_Bool  m_IsOpen;

    /// contains the error if one has occured.
    tsp00_VfReturn  m_FileError;

    ///contains the internal Id of the OS file.
    SAPDB_Int4  m_FileNo;

    /// contains the given OS file name.
    tsp00_VFilename m_FileName;

    /// true, if file should neither be opened or locked
    SAPDB_Bool m_doNotOpen;
};

#endif  /* KERNEL_FILEIO_HPP */
