/*****************************************************************************/
/*!
  @file         Log_ActionSetFileAttributes.hpp
  @author       MartinKi
  @ingroup      ActionHandling

  @brief        Header file for class Log_ActionSetFileAttributes.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2006 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

#ifndef LOG_ACTIONSETFILEATTRIBUTES_HPP
#define LOG_ACTIONSETFILEATTRIBUTES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_FileNo.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class FileDir_Generic;
class Msg_List;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @brief Log action to set (boolean) file attributes of file
 *        directory entries.
 *
 * @author MartinKi
 */
class Log_ActionSetFileAttributes : public Log_ReadWriteActionImage
{
public:
    /// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionSetFileAttributes ()
        : Log_ReadWriteActionImage (Log_SetFileAttributes),
          m_Head                   ()
    {}

    /*!
     * @brief this constructor is designed for a further call of
     * WriteImagePersistent
     *
     * Don't forget to set which attributes you want to change before
     * calling WriteImagePersistent!
     *
     * @param fileNo [in] file no of file whose attributes are to be
     *                    changed
     */
	Log_ActionSetFileAttributes (const FileDir_FileNo &fileNo)
        : Log_ReadWriteActionImage (Log_SetFileAttributes),
          m_Head                   (fileNo)
    {}

    /*!
     * @brief Sets attribute whether the pages of a file are clustered
     * or not.
     *
     * Does not set the new attribute value immediately. Use the
     * Execute() method to actually change the attribute value.  This
     * method queries the file directory to store the current value of
     * the PagesClustered attribute for the undo operation.
     *
     * @param TransContext [in] transaction context
     * @param bPagesClustered [in] new state of pages clustered file
     *                            attribute
     *
     * @return
     *  - true, if the new and the old state of the attribute could be set
     *  - false, if the file directory entry for the file could not be found
     */
    SAPDB_Bool SetPagesClustered(
        const Trans_Context &TransContext,
        const SAPDB_Bool     bPagesClustered);

    /// This has at least to be copied in one step.
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Header));
    }

	/// Call for rollback of action.
    virtual void Undo (tgg00_TransContext &TransContext,
                       Log_IImage         &Image) const;

	/// Call for redo of action.
    virtual void Redo (tgg00_TransContext   &TransContext,
                       Log_IImage           &Image,
                       Msg_List &Errlist) const;

    virtual void Execute (tgg00_TransContext &TransContext) const;

    virtual SAPDB_UInt GetPersistentLength () const;

    virtual SAPDB_UInt GetMinimalPersistentLength () const;

    /// Writes state info to trace.
    virtual void WriteToTrace (const char * Title) const;

    /// Adds state info to errList.
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    Msg_List                  &errList,
                                    SAPDB_Char  const * const  title = 0) const;

    /// @copydoc Log_ReadWriteActionImage::ReadImagePersistent
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      Msg_List              &errlist);

    /// @copydoc Log_ReadWriteActionImage::WriteImagePersistent
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       Msg_List              &errlist) const;

private:
    enum ExecutionType
    {
        OnlineExecution,
        UndoExecution,
        RedoExecution
    };

    /// Bitpos values used to encode which attribute was changed with
    /// this action.
    enum ChangeMaskBytes {
        PagesClusteredChanged = 1
    };

    /// Bitpos values used to store new and old value of a single
    /// attribute.
    enum OldNewValueMaskBytes {
        OldValueMask = 1,
        NewValueMask = 2
    };

    /// defines the physical structure of this action in undo/redo-files
    struct Header
    {
        FileDir_FileNo fileNo;
        SAPDB_Byte     changeMask;
        SAPDB_Byte     pagesClustered;

        Header ()
            : changeMask( 0 ),
              pagesClustered( 0 )
        {}

        Header (const FileDir_FileNo& fileNo)
            : fileNo( fileNo ),
              changeMask( 0 ),
              pagesClustered( 0 )
        {}
    };

    void Execute (tgg00_TransContext &TransContext,
                  ExecutionType       executionType) const;

    // no copy-construction or assignment
    Log_ActionSetFileAttributes(const Log_ActionSetFileAttributes&);
    Log_ActionSetFileAttributes operator=(Log_ActionSetFileAttributes&);

    /// this is the structured persistent entry information
    Header         m_Head;

    void StoreAttributeValues(
        SAPDB_Byte &attribute,
        SAPDB_Bool  bOldValue,
        SAPDB_Bool  bNewValue);

    SAPDB_Bool GetAttributeValue(
        const SAPDB_Byte &attribute,
        const SAPDB_Bool  bGetNewValue) const;

    SAPDB_Bool GetFileEntry(
        const Trans_Context &TransContext,
        FileDir_Generic     &fileEntry) const;
};

#endif // LOG_ACTIONSETFILEATTRIBUTES_HPP
