/*****************************************************************************/
/*!
  @file         Log_ActionSetFileDirCounter.hpp
  @author       MartinKi
  @ingroup      ActionHandling

  @brief        Header file for class Log_ActionSetFileDirCounter.

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

#ifndef LOG_ACTIONSETFILEDIRCOUNTER_HPP
#define LOG_ACTIONSETFILEDIRCOUNTER_HPP

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

class Msg_List;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @brief Log action to set the file directory counters of a table or
 *        index file to a given set of values.
 *
 * This action has no undo action defined, i.e. it cannot be rolled
 * back.
 *
 * @author MartinKi
 */
class Log_ActionSetFileDirCounter : public Log_ReadWriteActionImage
{
public:
    /// This constructor is designed for a further call of ReadImagePersistent
	Log_ActionSetFileDirCounter ()
        : Log_ReadWriteActionImage (Log_SetFileDirCounter),
          m_Head                   ()
    {}

    /// This constructor is designed for a further call of
    /// WriteImagePersistent.
    Log_ActionSetFileDirCounter (
        const FileDir_FileNo &fileNo,
        const SAPDB_UInt4     indexPageCount,
        const SAPDB_UInt4     leafPageCount,
        const SAPDB_UInt8     entryCount,
        const SAPDB_UInt8     lobPageCount)
        : Log_ReadWriteActionImage (Log_SetFileDirCounter),
          m_Head                   (fileNo,
                                    indexPageCount,
                                    leafPageCount,
                                    entryCount,
                                    lobPageCount)
    {}

    /// This has at least to be copied in one step.
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Header));
    }

	/// Call for rollback of action. Not implemented!
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

    /// defines the physical structure of this action in undo/redo-files
    struct Header
    {
        SAPDB_UInt4       indexPageCount;
        SAPDB_UInt4       leafPageCount;
        FileDir_FileNo    fileNo;
        SAPDB_UInt8       entryCount;
        SAPDB_UInt8       lobPageCount;

        Header ()
        {
            indexPageCount = 0;
            leafPageCount  = 0;
            entryCount     = 0;
            lobPageCount   = 0;
        }

        Header (
            const FileDir_FileNo &newFileNo,
            const SAPDB_UInt4     newIndexPageCount,
            const SAPDB_UInt4     newLeafPageCount,
            const SAPDB_UInt8     newEntryCount,
            const SAPDB_UInt8     newLobPageCount)
            : fileNo         (newFileNo),
              indexPageCount (newIndexPageCount),
              leafPageCount  (newLeafPageCount),
              entryCount     (newEntryCount),
              lobPageCount   (newLobPageCount)
        {}
    };

    void Execute (tgg00_TransContext &TransContext,
                  ExecutionType       executionType) const;

    // no copy-construction or assignment
    Log_ActionSetFileDirCounter(const Log_ActionSetFileDirCounter&);
    Log_ActionSetFileDirCounter operator=(Log_ActionSetFileDirCounter&);

    /// this is the structured persistent entry information
    Header         m_Head;
};

#endif // LOG_ACTIONSETFILEDIRCOUNTER_HPP
