/* @lastChanged: "1998-06-26  9:57"
 *
 * @filename:    vcn32.cpp
 * @purpose:     "backup history management"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * @Copyright (c) 1998-2005 SAP AG-2004"
 *
 * ==================================================================
 *
 * responsible:   MartinR, TiloH
 *
 * created:       1998-05-25 by MartinR
 *
 * purpose:       basic module for backup history
 *                               
 * ==================================================================


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/* ------------------------------------------------------------------
 * list of functions:
 *
 * cn32_AllocEBIDList
 * cn32_EBIDList
 *
 * cn32FreeEBIDList
 * cn32GetEBIDs
 * cn32GetEBIDsFromTool
 * cn32ListNextEBIDs
 * cn32ForgetEBIDs
 * ------------------------------------------------------------------
 */

// ------------------------------------------------------------------
// includes
// ------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDList.hpp"
#include "hcn32.h"
#include "hcn90.h"


// PRIVATE FUNCTION cn32_AllocEBIDList
// ------------------------------------------------------------------
// purpose: allocate new memory for a EBID list
//
static tcn00_Error cn32_AllocEBIDList(VControlDataT         * vcontrol,
                                      char                  * replyData,
                                      int                   * replyLen,
                                      DBMSrvBHist_EBIDList *& pEBIDList)
{
    tcn00_Error rc=OK_CN00;

    if(0!=vcontrol)
    {
        if(0==pEBIDList)
            pEBIDList=new DBMSrvBHist_EBIDList();

        if(0==pEBIDList)
            cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
        else
            vcontrol->pEBIDList=pEBIDList;
    }

    return rc;
} // end cn32_AllocEBIDList

// PRIVATE FUNCTION cn32_EBIDList
// ------------------------------------------------------------------
// purpose: get the EBID list from global struct and check
//
static tcn00_Error cn32_EBIDList(VControlDataT        *  vcontrol,
                                 char                 *  replyData,
                                 int                  *  replyLen,
                                 DBMSrvBHist_EBIDList *& pEBIDList)
{
    tcn00_Error rc=OK_CN00;

    pEBIDList=vcontrol->pEBIDList;

    // is there an active history session?
    if(0==pEBIDList)
        rc=cn90AnswerIError(replyData, replyLen, ERR_NOEBIDLIST_CN00);

    return rc;
} // end cn32_EBIDList

// PUBLIC FUNCTION cn32FreeEBIDList
// ------------------------------------------------------------------
// purpose: frees the EBID List from the VControlDataT structure
//
void cn32FreeEBIDList(VControlDataT * vcontrol)
{
    if(0!=vcontrol && 0!=vcontrol->pEBIDList)
    {
        delete vcontrol->pEBIDList;
        vcontrol->pEBIDList=0;
    }
} // end cn32FreeEBIDList

/* SPECIFICATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */

// PUBLIC FUNCTION cn32GetEBIDs
// ------------------------------------------------------------------
// purpose: queries an external backup tool for a list of external
//          backup IDs (EBIDs). The tool is determined through a
//          backup medium and the list consists of EBIDs for backups
//          which are currently available.
//
tcn00_Error cn32GetEBIDs(VControlDataT * vcontrol,
                         CommandT      * command,
                         char          * replyData,
                         int           * replyLen,
                         int             replyLenMax)
{
    tcn00_Error            rc=OK_CN00;
    DBMSrvBHist_EBIDList * pEBIDList=0;

    rc=cn32_EBIDList(vcontrol, replyData, replyLen, pEBIDList);

    if(rc==OK_CN00)
    {
        cn32FreeEBIDList(vcontrol);
        pEBIDList=0;                    // (*pEBIDList) was freed one line above, so don't reference to it anymore
    }

    rc=cn32_AllocEBIDList(vcontrol, replyData, replyLen, pEBIDList);

    if(OK_CN00==rc)
        rc=pEBIDList->GetEBIDs(vcontrol, command, replyData, replyLen, replyLenMax);

    if(OK_CN00==rc)
        sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);

    *replyLen=int(strlen(replyData));

    return rc;
} // end cn32GetEBIDs

const char * const LongHelp_GetEBIDS_cn32=
    "@command backup_ext_ids_get You inquire external backup IDs and other "
        "information about backups for a database instance that are currently "
        "stored in a backup tool. Use the command backup_ext_ids_list to "
        "display the inquired information."
    "@preconditions You have the DBM operator authorization Backup or DBInfoRead."
    "@syntax backup_ext_ids_get <medium> [<database_name>] [<database_server>]"
        "@param <medium>        Name of the backup medium; this must be defined "
            "beforehand with the help of the command medium_put"
        "@param <database_name> Name of the database instance from which the "
            "backups were created"
        "@param <database_server>   Name of the server on which the backups were created"
    "@reply OK";


// PUBLIC FUNCTION cn32GetEBIDsFromTool
// ------------------------------------------------------------------
// purpose: queries an external backup tool for a list of external
//          backup IDs (EBIDs). The tool is determined through a
//          string and the list consists of EBIDs for backups
//          which are currently available.
//
tcn00_Error cn32GetEBIDsFromTool(VControlDataT * vcontrol,
                                 CommandT      * command,
                                 char          * replyData,
                                 int           * replyLen,
                                 int             replyLenMax)
{
    tcn00_Error            rc=OK_CN00;
    DBMSrvBHist_EBIDList * pEBIDList=0;

    rc=cn32_EBIDList(vcontrol, replyData, replyLen, pEBIDList);

    if(rc==OK_CN00)
    {
        cn32FreeEBIDList(vcontrol);
        pEBIDList=0;                    // (*pEBIDList) was freed one line above, so don't reference to it anymore
    }

    rc=cn32_AllocEBIDList(vcontrol, replyData, replyLen, pEBIDList);

    if(OK_CN00==rc)
        rc=pEBIDList->GetEBIDs(vcontrol, command, replyData, replyLen, replyLenMax, 0); //0 means not from a medium

    if(OK_CN00==rc)
        sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);

    *replyLen=int(strlen(replyData));

    return rc;
} // end cn32GetEBIDs

const char * const LongHelp_GetEBIDSFromTool_cn32=
    "@command backup_ext_ids_getfromtool You inquire external backup IDs and other "
        "information about backups for a database instance that are currently "
        "stored in a backup tool. Use the command backup_ext_ids_list to "
        "display the inquired information."
    "@preconditions You have the DBM operator authorization Backup or DBInfoRead."
    "@syntax backup_ext_ids_getfromtool <tool_type> [<database_name>] [<database_server>]"
        "@param <tool_type>      The backup tool that should be used. See command medium_put "
            "(option <tool_type>) for supported backup tools and allowed values for <tooltype>."
        "@param <database_name> Name of the database instance from which the "
            "backups were created"
        "@param <database_server>   Name of the server on which the backups were created"
    "@reply OK";

// PUBLIC FUNCTION cn32ListNextEBIDs
// ------------------------------------------------------------------
// purpose: prints a list of EBIDs to the reply buffer
//
tcn00_Error cn32ListNextEBIDs(VControlDataT * vcontrol,
                              CommandT      * command,
                              char          * replyData,
                              int           * replyLen,
                              int             replyLenMax)
{
    tcn00_Error            rc;
    DBMSrvBHist_EBIDList * pEBIDList=0;

    rc=cn32_EBIDList(vcontrol, replyData, replyLen, pEBIDList);

    if(OK_CN00==rc)
        rc=pEBIDList->ListNextEBIDs(vcontrol, replyData, replyLen, replyLenMax);

    *replyLen=(int)strlen(replyData);

    return rc;
} // end cn32ListNextEBIDs

// PUBLIC FUNCTION cn32ForgetEBIDs
// ------------------------------------------------------------------
// purpose: frees EBID list
//
tcn00_Error cn32ForgetEBIDs(VControlDataT * vcontrol,
                            CommandT      * command,
                            char          * replyData,
                            int           * replyLen,
                            int             replyLenMax )
{
    tcn00_Error            rc=OK_CN00;
    DBMSrvBHist_EBIDList * pEBIDList=0;

    rc=cn32_EBIDList(vcontrol, replyData, replyLen, pEBIDList);

    if(OK_CN00==rc)
    {
        cn32FreeEBIDList(vcontrol);
        sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    }

    *replyLen = (int)strlen(replyData);

    return rc;
} // end cn32ForgetEBIDs
