/*


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
#if defined(CN72) && defined(_WIN32)
  #line 3 "d:\\v72\\sys\\src\\cn\\hcn30.h"
#endif 

/* @lastChanged: "1998-05-08  8:23"
 *
 * @filename:    hcn30.h
 * @purpose:     "media management"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * @copyright:   "Copyright by SAP AG, 1998-2004"
 *
 * ==================================================================
 *
 * responsible:   MartinR
 *
 * created:       1998-03-12 by MartinR
 *
 * purpose:       basic module for media management 
 *                               
 * ==================================================================
 */

#ifndef _HCN30_H_
#define _HCN30_H_

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#include "gcn00.h"
#include "gcn003.h"

#include "SAPDB/DBM/Srv/DBMSrv_Reply.hpp"

/* ------------------------------------------------------------------
 * specification public functions
 * ------------------------------------------------------------------
 */

tcn00_Error cn30BackupMediaGet (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn30BackupMediaPut (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn30BackupMediaGetAll (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn30BackupMediaMigrate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn30BackupMediaDelete (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn30BackupMediaDate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

/* ------------------------------------------------------------------
 * specification intern functions
 * ------------------------------------------------------------------
 */

tcn00_Error cn30NewMedium(tcn003_Medium *& pMedium);
tcn00_Error cn30NewMediaGroup(tcn003_MediaGroup *& pMediaGroup);

void cn30InitMedia(tcn003_Media * pMedia);

tcn00_Error cn30AddMedium (
      char           * replyData,
      int            * replyLen,
      tcn003_Media   * pMedia,
      tcn003_Medium  * pMedium );

tcn00_Error cn30ReadMedia (
      const tsp00_DbNamec   szDbName,
      char                * replyData,
      int                 * replyLen,
      int                   replyLenMax,
      tcn003_Media        * pMedia );

tcn00_Error cn30WriteMedia (
      const tsp00_DbNamec & szDbName,
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia );

void cn30FreeMedia (
      tcn003_Media  * pMedia );

tcn00_Error cn30FindMediumByName (
      char              * replyData,
      int               * replyLen,
      tcn003_Media      * pMedia,
      const char        * szMediumName,
      bool                 CheckMediaGroups,
      tcn003_MediaGroup ** pMediaGroupFound,
      tcn003_Medium     ** pMediumFound,
      bool                 forbidMatchingGroups=false);

tcn00_Error cn30FindMediaGroupByName (
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia,
      const char          * szGroupName,
      tcn003_MediaGroup   ** pMediaGroupFound );

tcn00_Error cn30FindMediumInGroupByLocation (
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia,
      const char          * szGroupName,
      const char          * szLocation,
      tcn003_MediaGroup  ** pMediaGroupFound,
      tcn003_Medium      ** pMediumFound );

tcn00_Error cn30SetBackupBlockCountInMediaFile(const char * backupBlockCount, DBMSrv_Reply & reply);

#endif /* _HCN30_H_ */
