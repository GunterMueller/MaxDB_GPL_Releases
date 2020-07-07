/*****************************************************************************/
/*!
  @file         Data_UpdStatRequestFile.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class Data_UpdStatRequestFile.

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

#ifndef DATA_UPDSTATREQUESTFILE_HPP
#define DATA_UPDSTATREQUESTFILE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_Types.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class Msg_List;
class Data_UpdStatRequestFile;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class  Data_UpdStatRequestFile
 * @author MartinKi
 *
 * This class provides a temporary file to store request and result
 * information for tables on which statistics need to be computed.
 *
 * @sa SQLMan_UpdateStatistics Data_PrimTreeStatisticsCoordinator
 *     SrvTasks_JobCollectTableStatistics
 */
class Data_UpdStatRequestFile
{
private:
    // no copy-construction or assignment
    Data_UpdStatRequestFile(const Data_UpdStatRequestFile&);
    Data_UpdStatRequestFile operator=(const Data_UpdStatRequestFile&);

    FileDir_Table m_file;
    tgg00_FileId  m_fileId;

    struct RequestRecord {
        SAPDB_Int2  recLen;
        SAPDB_Int2  keyLen;
        SAPDB_Int2  varColOffset;
        SAPDB_Int2  varColCount;
        //
        SAPDB_Byte  tableFileSize[ 6 ];
        SAPDB_Byte  tableFileNo[ 8 ];
        //
        SAPDB_Byte      pageCntToSample[ 4 ];
        SAPDB_Byte      isDone;
    };

public:
    /// Possible return codes.
    enum ReturnCode {
        Okay,           // Finished successfully
        FileNotFound,   // The file could not be found during open.
        UpdateFailed,
        SystemError
    };

    /*!
     * @name Internal public classes
     */
    /*@{*/

    class Iterator;
    friend class Iterator;

    /*!
     * @class TableKey
     *
     * @brief Key used to retrieve information for a specific table.
     *
     * It is not intended that the key can be set by users of this
     * class. It should only be used in the Iterator::GetTableKey and
     * Iterator::Find methods.
     */
    class TableKey {
    private:
        friend class Iterator;

        SAPDB_Byte     tableFileSize[ 6 ];
        FileDir_FileNo tableFileNo;
    public:
        /// Constructor.
        TableKey();

        /*!
         * @brief Returns the fileno of the table this key is pointing to.
         */
        const FileDir_FileNo& GetTableFileNo() const;

        /*!
         * @brief Returns a pointer to the start of the key. Should
         * only be used when trying to dump key contents.
         */
        const SAPDB_Byte* GetKeyPointer() const;

        /*!
         * @brief Returns the length of the key in bytes.
         */
        SAPDB_UInt GetLength() const;

        /*!
         * @brief Returns true if this key is valid.
         *
         * This does not imply that there is an entry corresponding to
         * this key, but only that the fileno contained in the key is valid.
         */
        SAPDB_Bool IsValid() const;

        /*!
         * @brief Returns true if this key is not valid.
         */
        SAPDB_Bool IsInvalid() const;
    };

    /*!
     * @class Iterator
     *
     * Provides an iterator over Data_UpdStatRequestFile.
     */
    class Iterator
    {
    public:
        /// Possible return codes.
        enum ReturnCode {
            Okay,
            EndOfFile,
            KeyNotFound,
            SystemError
        };

        /// Copy constructor.
        Iterator(const Iterator& it);

        /*!
         * @name Navigation
         */
        /*@{*/

        /*!
         * @brief Positions the iterator before the first entry.
         *
         * The iterator content is undefined until the next call of
         * the Next method.
         */
        void SetFirst();

        /*!
         * @brief Advances the iterator by one.
         *
         * @return
         *   - Okay, if the iterator content is valid
         *   - EndOfFile, if the iterator passed the end of the file and
         *     its content is undefined; further calls to Next will also
         *     result in EndOfFile until SetFirst is called
         *   - SystemError, if an error occured during execution; in this
         *     case errMsg will be set
         */
        ReturnCode Next(
            tgg00_TransContext& trans,
            Msg_List&           errMsg);

        /*!
         * @brief Deletes the entry at the iterator's position from the file.
         *
         * If the delete operation completes successfully the iterator
         * will point to the entry behind the one that was deleted.
         *
         * @return
         *   - Okay, on success
         *   - KeyNotFound, if the iterator is not at a valid position
         *     or if the entry at the iterator's position could not be
         *     found any more (e.g. it has been deleted already)
         *   - EndOfFile, if the entry deleted was the last in the file; the
         *     iterator content is undefined in this case
         *   - SystemError, if an error occured during execution; in this
         *     case errMsg will be set
         */
        ReturnCode Delete(
            tgg00_TransContext& trans,
            Msg_List&           errMsg);

        /*!
         * @brief Returns true if the iterator is positioned at a valid entry.
         *
         * The iterator buffers the content of the current entry, so
         * it remains valid even if the entry itself has been deleted
         * by somebody else.
         */
        SAPDB_Bool IsValid() const;

        /*@}*/

        /*!
         * @name Query/Manipulation
         */
        /*@{*/

        /*!
         * @brief Returns the fileNo of the table the iterator is
         * pointing at.
         */
        FileDir_FileNo GetTableFileNo() const;


        /*!
         * @brief Returns the number of pages that should be sampled
         * for this entry.
         *
         * @return the number of pages to sample or 0, if the whole
         * file should be evaluated.
         */
        SAPDB_Int4 GetNoOfPagesToSample() const;

        /*!
         * @brief Returns true if the statistics for the current table
         * are available in the file. In other words, the request has
         * been processed successfully.
         */
        SAPDB_Bool IsRequestProcessed() const;

        /*!
         * @brief Sets whether a request is processed or not and data
         * available.
         */
        ReturnCode SetRequestProcessed(
            tgg00_TransContext& trans,
            const SAPDB_Bool    isProcessed,
            Msg_List&           errMsg);
        /*@}*/
    private:
        friend class Data_UpdStatRequestFile;

        Data_UpdStatRequestFile& m_file;
        RequestRecord            m_requestRec;
        tgg00_Lkey               m_curKey;
        SAPDB_Int4               m_pageCntToSample;
        TableKey                 m_tableKey;
        SAPDB_Bool               m_isDone;

        /// Constructor. The iterator will point before the first entry.
        Iterator(
            tgg00_TransContext&      trans,
            Data_UpdStatRequestFile& file);

        SAPDB_Bool ParseRecord(Msg_List& errMsg);
        void SetStartKey();
        void SetEndKey();
        ReturnCode UpdateRecord(
            tgg00_TransContext& trans,
            Msg_List&           errMsg);
    };
    /*@}*/


    /// Constructor.
    Data_UpdStatRequestFile();

    /*!
     * @brief Creates the request file if it does not exist.
     *
     * If the file already exists, its content will <em>not</em> be deleted!
     *
     * @return a valid FileDir_FileNo, if the file could be created or
     * already existed, otherwise the fileno will be invalid
     */
    FileDir_FileNo Create(
        tgg00_TransContext& trans,
        Msg_List&           errMsg);

    /*!
     * @brief Opens an existing request file.
     *
     * @return
     *   - Okay on success
     *   - FileNotFound, if the file given fileNo is unknown
     *   - SystemError, if an error occured during execution; in this
     *     case errMsg will be set
     */
    ReturnCode Open(
        tgg00_TransContext&   trans,
        const FileDir_FileNo& fileNo,
        Msg_List&             errMsg);

    /*!
     * @brief Destroys the request file on disk.
     *
     * @return
     *   - Okay, on success
     *   - FileNotFound, if the object is not assigned to a request file
     *   - SystemError, if an error occured during execution; in this
     *     case errMsg will be set
     */
    ReturnCode Destroy(
        tgg00_TransContext&   trans,
        Msg_List&             errMsg);

    /*!
     * @brief Return the file no of the request file on disk.
     */
    FileDir_FileNo GetFileNo() const;

    /*!
     * @brief Adds a statistics request entry.
     *
     * @return
     *   - Okay, on success
     *   - FileNotFound, if tableFileNo could not be found in the
     *     file directory
     *   - SystemError, if an error occured during execution; in this
     *     case errMsg will be set
     */
    ReturnCode AddTable(
        tgg00_TransContext&     transContext,
        const FileDir_FileNo&   tableFileNo,
        const SAPDB_Int4        noOfPagesToSample,
        Msg_List&               errMsg);

    /*!
     * @brief Returns an iterator over the request file.
     */
    Iterator GetIterator(tgg00_TransContext& trans);
};

/**************************************************************************/

inline Data_UpdStatRequestFile::TableKey::TableKey()
{
    // m_tableFileNo is initialized to invalid automatically
}

/**************************************************************************/

inline const FileDir_FileNo&
Data_UpdStatRequestFile::TableKey::GetTableFileNo() const
{
    return tableFileNo;
}

/**************************************************************************/

inline SAPDB_Bool
Data_UpdStatRequestFile::TableKey::IsValid() const
{
    return tableFileNo.IsValid();
}

/**************************************************************************/

inline SAPDB_Bool
Data_UpdStatRequestFile::TableKey::IsInvalid() const
{
    return tableFileNo.IsInvalid();
}

/**************************************************************************/

inline const SAPDB_Byte*
Data_UpdStatRequestFile::TableKey::GetKeyPointer() const
{
    return tableFileSize;
}

/**************************************************************************/

inline SAPDB_UInt
Data_UpdStatRequestFile::TableKey::GetLength() const
{
    return sizeof( *this );
}

/**************************************************************************/

inline SAPDB_Bool
Data_UpdStatRequestFile::Iterator::IsRequestProcessed() const
{
    return m_isDone;
}

/**************************************************************************/

inline FileDir_FileNo
Data_UpdStatRequestFile::Iterator::GetTableFileNo() const
{
    return m_tableKey.GetTableFileNo();
}

#endif // DATA_UPDSTATREQUESTFILE_HPP
