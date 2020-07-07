/*!
  \infile  DBMSrvBHist_EBIDList.hpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   a class for managing a list of external backup ID's (including
           type and date of the backup)

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#if !defined(DBMSRVBHIST_EBIDLIST_HPP)
#define DBMSRVBHIST_EBIDLIST_HPP


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp"


//-----------------------------------------------------------------------------
// class DBMSrvBHist_EBIDList
//-----------------------------------------------------------------------------

class DBMSrvBHist_EBIDList: public DBMSrvBHist_ListProducer
{
  public:
    DBMSrvBHist_EBIDList();
    ~DBMSrvBHist_EBIDList();

    tcn00_Error GetEBIDs(VControlDataT * vcontrol,
                         CommandT      * command,
                         char          * replyData,
                         int           * replyLen,
                         int             replyLenMax,
                         int             FromMedium=1);

    tcn00_Error ListNextEBIDs(VControlDataT * vcontrol,
                              char          * replyData,
                              int           * replyLen,
                              int             replyLenMax);

    tcn00_Error AddToEnd(char       * replyData, 
                         int        * replyLen,
                         DBMSrvBHist_Part::EBIDStatus Status,
                         const char * Key,
                         size_t       KeyLength,
                         const char * Label,
                         size_t       LabelLength,
                         const char * EBID,
                         size_t       EBIDLength,
                         const char * Type,
                         const char * DateTime);

    void Free();

    Tools_List<DBMSrvBHist_Part *> & GiveList();

  private:
    Tools_List<DBMSrvBHist_Part *> List;
    Tools_List<DBMSrvBHist_Part *>::iterator Current;

    void RemoveDoublequotes(char * String);
};

#endif
