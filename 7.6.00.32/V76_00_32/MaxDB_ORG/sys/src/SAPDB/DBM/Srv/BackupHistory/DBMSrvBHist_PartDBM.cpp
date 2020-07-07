/*!
  \file    DBMSrvBHist_PartDBM.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   implementing DBMServer specific part of a class for handling one
           part of a copy of a backup

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp"
#include "hcn30.h"
#include "hcn36.h"


//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

extern const char * DBMSrvBHist_Part_MEDIUMTYPE_UNKNOWN;
extern const char * DBMSrvBHist_Part_MEDIUMTYPE_FILE;
extern const char * DBMSrvBHist_Part_MEDIUMTYPE_TAPE;
extern const char * DBMSrvBHist_Part_MEDIUMTYPE_PIPE;

extern const char * DBMSrvBHist_Part_MEDIUMTYPE_HIST_FILE;
extern const char * DBMSrvBHist_Part_MEDIUMTYPE_HIST_TAPE;
extern const char * DBMSrvBHist_Part_MEDIUMTYPE_HIST_PIPE;


//-----------------------------------------------------------------------------
// member functions of class DBMSrvBHist_Part (DBMServer specific part)
//-----------------------------------------------------------------------------

tcn00_Error DBMSrvBHist_Part::ConstructMedium(tcn003_Medium *& Medium, const char * TheBackupType) const
{
    tcn00_Error rc=cn30NewMedium(Medium);

    if(OK_CN00==rc)
    {
        const char * h=cn36_FirstNonWhiteSpaceOf(MediumName);

        cn36_StrNCpy(Medium->szName, h, cn36_StartOfTrailingWhiteSpaces(h)-h);

        h=cn36_FirstNonWhiteSpaceOf(MediumLocation);
        cn36_StrNCpy(Medium->szLocation, h, cn36_StartOfTrailingWhiteSpaces(h)-h);

        h=cn36_FirstNonWhiteSpaceOf(MediumType);

        if(0==cn36_StrStrNCmp(DBMSrvBHist_Part_MEDIUMTYPE_HIST_FILE, h, cn36_StartOfTrailingWhiteSpaces(h)-h))
            SAPDB_strcpy(Medium->szType, DBMSrvBHist_Part_MEDIUMTYPE_FILE);
        else
            if(0==cn36_StrStrNCmp(DBMSrvBHist_Part_MEDIUMTYPE_HIST_TAPE, h, cn36_StartOfTrailingWhiteSpaces(h)-h))
                SAPDB_strcpy(Medium->szType, DBMSrvBHist_Part_MEDIUMTYPE_TAPE);
            else
                if(0==cn36_StrStrNCmp(DBMSrvBHist_Part_MEDIUMTYPE_HIST_PIPE, h, cn36_StartOfTrailingWhiteSpaces(h)-h))
                    SAPDB_strcpy(Medium->szType, DBMSrvBHist_Part_MEDIUMTYPE_PIPE);
                else
                    SAPDB_strcpy(Medium->szType, DBMSrvBHist_Part_MEDIUMTYPE_UNKNOWN);

        h=cn36_FirstNonWhiteSpaceOf(TheBackupType);
        cn36_StrNCpy(Medium->szSaveType, h, cn36_StartOfTrailingWhiteSpaces(h)-h);

        h=cn36_FirstNonWhiteSpaceOf(MediumSize);
        cn36_StrNCpy(Medium->szSize, h, cn36_StartOfTrailingWhiteSpaces(h)-h);

        h=cn36_FirstNonWhiteSpaceOf(MediumOverwrite);
        cn36_StrNCpy(Medium->szOverwrite, h, cn36_StartOfTrailingWhiteSpaces(h)-h);

        h=cn36_FirstNonWhiteSpaceOf(MediumBlockSize);
        cn36_StrNCpy(Medium->szBlocksize, h, cn36_StartOfTrailingWhiteSpaces(h)-h);

        SAPDB_strcpy(Medium->szAutoLoader, ""); //no autoloader command supported yet
        SAPDB_strcpy(Medium->szOSCommand, "");  //no OS-command supported yet
        SAPDB_strcpy(Medium->szCreated, "");
        SAPDB_strcpy(Medium->szModified, "");

        h=cn36_FirstNonWhiteSpaceOf(BackupToolString);
        cn36_StrNCpy(Medium->szExternal, h, cn36_StartOfTrailingWhiteSpaces(h)-h);

        Medium->bOld=false; //nobody knows where the original medium was defined

        Medium->pNext=0;
        Medium->pPrev=0;
    }

    return rc;
}
