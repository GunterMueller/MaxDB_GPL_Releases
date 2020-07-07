/*!
    \file    DBMSrvBLabel_Label.hpp
    \author  TiloH
    \ingroup backup label handling for DBM Server
    \brief   a class handling labels of backups

\if EMIT_LICENCE

    ========== licence begin LGPL
    Copyright (c) 1998-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

\endif
*/

#if !defined(DBMSRVBLABEL_LABEL_HPP)
#define DBMSRVBLABEL_LABEL_HPP

#include "hcn36.h"
#include "DBM/Srv/Backup/Media/DBMSrvBMedia_DeviceType.hpp"

/*! \brief for storing and reading labels block information of backups */
class DBMSrvBLabel_Label
{
  public:
    /*! \brief constructor, initializing to an "empty" label block */
    DBMSrvBLabel_Label();

    /*!  \brief destructor*/
    ~DBMSrvBLabel_Label();

    /*! \brief function for reading a label from a certain backup

        For file backups with a block size smaller than 32 MB, the parameter
        blocksize is just a hint. For tapes and pipes the blocksize parameter
        must be correct, as those can only be read once.
    
        \param deviceName  [IN] name of the file/tape/pipe within the file
            system
        \param blockSize   [IN] the block size that is tried first
        \param deviceType  [IN] the type of the device, that is expected
        \param replyData   [IN/OUT] buffer containing any error messages
        \param replyLen    [IN/OUT] length of the constructed reply
        \param replyLenMax [IN]    maximal allowed length for reply (\TODO:
            honor this parameter)
        \return true if function was successful, otherwise false is returned and
            replyData is filled with an error message */
    bool readLabelFrom(
        const char                    * deviceName,
        size_t                          blockSize,
        const DBMSrvBMedia_DeviceType & deviceType,
        char                          * replyData,
        int                           * replyLen,
        int                             replyLenMax);

    /*! \brief get label information from the label block
        \returns label if available, empty string otherwise */
    const char * getLabel() const;

    /*! \brief get start date of backup from the label block
        \returns start date of backup if available, empty string otherwise */
    const char * getStartDate() const;

    /*! \brief get start time of backup from the label block
        \returns start time of backup if available, empty string otherwise */
    const char * getStartTime() const;

    /*! \brief get end date of backup from the label block
        \returns end date of backup if available, empty string otherwise */
    const char * getStopDate() const;

    /*! \brief get end time of backup from the label block
        \returns end time of backup if available, empty string otherwise */
    const char * getStopTime() const;

    /*! \brief get machine name from the label block
        \returns machine name if available, empty string otherwise */
    const char * getServer() const;

    /*! \brief get db name from the label block
        \returns db name if available, empty string otherwise */
    const char * getDB() const;

    /*! \brief get kernel version from the label block
        \return kernel version, if available, empty string otherwise */
    const char * getKernelVersion() const;

    /*! \brief get consistency flag from the label block
        \return consistency flag, if available, empty string otherwise */
    const char * getIsConsistent() const;

    /*! \brief get the number of the first log page from the label block
        \return first log page number, if available, empty string otherwise */
    const char * getFirstLogPage() const;

    /*! \brief get the number of the last log page from the label block
        \return last log page number, if available, empty string otherwise */
    const char * getLastLogPage() const;

    /*! \brief get date of dbstamp1 from the label block
        
        dbstamp1 is the log io-sequence of the savepoint(data, pages backup)
        or first commit contained in the backup
         
        \returns get date of dbstamp1 if available, empty string otherwise */
    const char * getDBStamp1Date() const;

    /*! \brief get time of dbstamp1 from the label block

        dbstamp1 is the log io-sequence of the savepoint(data, pages backup)
        or first commit contained in the backup

        \returns time of dbstamp1 if available, empty string otherwise */
    const char * getDBStamp1Time() const;

    /*! \brief get date of dbstamp2 from the label block
        
        dbstamp2 is the log io-sequence last commit contained in a log backup
         
        \returns get date of dbstamp2 if available, empty string otherwise */
    const char * getDBStamp2Date() const;

    /*! \brief get time of dbstamp1 from the label block

        dbstamp2 is the log io-sequence last commit contained in a log backup

        \returns time of dbstamp2 if available, empty string otherwise */
    const char * getDBStamp2Time() const;

    /*! \brief get page count from the label block
        \returns page count, if available, empty string otherwise */
    const char * getPageCount() const;

    /*! \brief get number of used devices from the label block
        \returns number of used devices, if available, empty string otherwise */
    const char * getNumberDevices() const;

    /*! \brief get database ID from the label block
        \returns database ID, if available, empty string otherwise */
    const char * getDatabaseID() const;

    /*! \brief get maximal used page data page number from the label block
        \returns maximal used page data page number, if available, empty string otherwise */
    const char * getMaxUsedDataPage() const;

    /*! \brief get number of used converter pages from the label block
        \returns number of used converter pages, if available, empty string otherwise */
    const char * getConverterPageCount() const;

    /*! \brief get start time (including date) of the backup according to the label block
    
        \param time [IN/OUT] time object to contain the start time
        \return true if successful, false otherwise */
    bool getStartDateTime(tcn36_DateTimeString & time) const;

    /*! \brief compare function for the label class

        \param otherLabel [IN] label that is to be compared against the object
        \return true if label are equal, false otherwise */
    bool equals(const DBMSrvBLabel_Label & otherLabel) const;

  private:
    char * m_Label;
    char * m_StartDate;
    char * m_StartTime;
    char * m_Server;
    char * m_StopDate;
    char * m_StopTime;
    char * m_DB;
    char * m_KernelVersion;
    char * m_IsConsistent;
    char * m_FirstLogPage;
    char * m_LastLogPage;
    char * m_DBStamp1Date;
    char * m_DBStamp1Time;
    char * m_DBStamp2Date;
    char * m_DBStamp2Time;
    char * m_PageCount;
    char * m_NumberDevices;
    char * m_DatabaseID;
    char * m_MaxUsedDataPage;
    char * m_ConverterPageCount;

    bool createCopy(
        char       *& dest,
        const char *  src,
        size_t        maxLengthSrc,
        char       *  replyData,
        int        *  replyLen);

    static int m_MaxAllowedBlockSize;
};

#endif
