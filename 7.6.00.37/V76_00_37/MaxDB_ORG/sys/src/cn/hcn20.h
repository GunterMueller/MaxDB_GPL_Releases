/*! 
  -----------------------------------------------------------------------------
 
  module: hcn20.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Parameter Management
   
  description:  DBMServer Parameter Management - Specification

  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


  -----------------------------------------------------------------------------
*/

#ifndef _HCN20_H_
#define _HCN20_H_

/* ------------------------------------------------------------------
 * INCLUDES
 * ------------------------------------------------------------------
 */
#include "gcn00.h"
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

/* ------------------------------------------------------------------
 * SPECIFICATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20FreeXParamSection
 * ------------------------------------------------------------------
 * purpose:   Abort a XParam-Session and frees the memory
 *
 * parameters:
 *   vcontrol  - CServer session data
 */
void cn20FreeXParamSection (
      VControlDataT * vcontrol);

/*!
  -------------------------------------------------------------------------
  function:     cn20XParamGetValue
  -------------------------------------------------------------------------
  description:  get a param value

  arguments:    DBNameC  [IN]  - name of database
                XpKeyC   [IN]  - name of parameter
                XpValueC [OUT] - value of parameter in string representation
                                 For crypted parameters you will get an empty 
                                 string.
                XpNum    [OUT] - value of parameter in numeric representation
                                 If the value isn't a number, you will get a 0.

  return value: OK_CN00, ERR_XPNOTFOUND_CN00, ERR_RTEEXT_CN00

  prototypes:   2

  -------------------------------------------------------------------------
 */
//tcn00_Error cn20XParamGetValue
//      ( const char *               DBNameCIn,
//        const char *               XpKeyCIn,
//        tcn002_XpValueString     & XpValueC );

//tcn00_Error cn20XParamGetValue
//      ( const char *               DBNameCIn,
//        const char *               XpKeyCIn,
//        tsp00_Int4               & XpNum );

bool cn20XParamGetValue
      ( const char *               DBNameCIn,
        const char *               XpKeyCIn,
        tcn002_XpValueString     & XpValueC,
        DBMSrvMsg_Error          & oDBMError );

bool cn20XParamGetValue
      ( const char *               DBNameCIn,
        const char *               XpKeyCIn,
        tsp00_Int4               & XpNum,
        DBMSrvMsg_Error          & oDBMError );

/*!
  -------------------------------------------------------------------------
  function:     cn20XParamPutValue
  -------------------------------------------------------------------------
  description:  get a param value

  arguments:    DBNameC  [IN]  - name of database
                XpKeyC   [IN]  - name of parameter
                XpValueC [IN]  - value of parameter in string representation
                                 For crypted parameters you will get an empty 
                                 string.
                XpNum    [IN]  - value of parameter in numeric representation
                                 If the value isn't a number, you will get a 0.

  return value: OK_CN00, ERR_XPNOTFOUND_CN00, ERR_RTEEXT_CN00

  prototypes:   2

  -------------------------------------------------------------------------
 */
tcn00_Error cn20XParamPutValue
      ( const tsp00_DbNamec        & DBNameC,
        const tcn002_XpValueName   & XpKeyC,
        const tcn002_XpValueString & XpValueC );
tcn00_Error cn20XParamPutValue
      ( const tsp00_DbNamec        & DBNameC,
        const tcn002_XpValueName   & XpKeyC,
        const tsp00_Int4           & XpNum );
/*
tcn00_Error cn20XParamPutValue
      ( const tsp00_DbNamec        & DBNameCIn,
        const tcn002_XpValueName   & XpKeyCIn,
        tcn002_XPRecord            * xpRecord);
*/

/*!
  -------------------------------------------------------------------------
  function:     cn20XParamDelete
  -------------------------------------------------------------------------
  description:  delete a param value

  arguments:    DBNameC  [IN]  - name of database
                XpKeyC   [IN]  - name of parameter

  return value: OK_CN00, ERR_XPNOTFOUND_CN00, ERR_RTEEXT_CN00

  -------------------------------------------------------------------------
 */
tcn00_Error cn20XParamDelete
      ( const tsp00_DbNamec        & DBNameCIn,
        const tcn002_XpValueName   & XpKeyCIn );

/*!
  -------------------------------------------------------------------------
  function:     cn20XParamGetServiceValues
  -------------------------------------------------------------------------
  description:  get some values for the service kernel

  arguments:    szDBName   [OUT]  - name of database
                szDbRoot   [IN]   - dbroot
                szUserName [OUT]  - name of the cold user
                szPassword [OUT]  - password of the cold user

  return value: OK_CN00, ERR_XPNOTFOUND_CN00, ERR_RTEEXT_CN00

  prototypes:   2

  -------------------------------------------------------------------------
 */
tcn00_Error cn20XParamGetServiceValues
      ( tsp00_DbNamec     & szDBName,
        const tsp00_Pathc & szDbRoot,
        tcn00_UserNamec   & szUserName,
        tsp00_Namec       & szPassword );

tcn00_Error cn20XParamGetServiceValues
      ( tsp00_DbNamec     & szDBName,
        const tsp00_Pathc & szDbRoot );

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20XParamGetValue
 * ------------------------------------------------------------------
 * purpose:   get a param value. XParam-Session isn't necessary, but it
 *            will enhance speed if there more than one cn20XParamGetValue calls.
 *            (use cn20XParamStart and cn20FreeXParamSection) 
 *
 * parameters:
 *   vcontrol    - CServer session data
 *   szParameter - Name of Parameter
 *   szValue     - Buffer for the value (at the moment 65 byte are enough;
 *                 sizeof(xp_value_record.xp_type) + 1 from veo001c)
 *                 crypt values (e.g. CONTROLPASSWORD) will be crypt by
 *                 s02appencrypt and send in ASCII representation - 
 *                 sequence of 6 hex numbers consisting of 8 digits
 *   szError     - Buffer for errormessage (full cserver error message
 *                 with "ERR" line ...)
 *
 * return - the usual cserver return codes
 */
tcn00_Error cn20XParamGetValue
      ( VControlDataT * vcontrol,
        const char    * szParameter,
        char          * szValue,
        char          * szError);


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20CalcDevices
 * ------------------------------------------------------------------
 * purpose:   all the param commands of the CServer
 *
 * parameters:
 *   vcontrol    - CServer session data
 *   command     - CServer command structure
 *   replyData   - buffer for answer
 *   replyLen    - answer length
 *   replyLenMax - max. answer lengthgth
 */
tcn00_Error cn20CalcDevices
      ( VControlDataT * vcontrol,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20CheckedParamPut
 * ------------------------------------------------------------------
 * purpose:   put check and save a parameter
 *
 * parameters:
 *   vcontrol    - CServer session data
 *   command     - CServer command structure
 *   replyData   - buffer for answer
 *   replyLen    - answer length
 *   replyLenMax - max. answer lengthgth
 *   nCount      - count of parameters
 *   szNames     - Array of parameter names
 *   szValues    - Array of parameter values
 */
tcn00_Error cn20CheckedParamPut
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax,
        int             nCount,
        const char    * szNames[],
        const char    * szValues[]);

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20ParamCheckAll
 * ------------------------------------------------------------------
 * purpose:   check the parameters
 *
 * parameters:
 *   szDbName    - database name
 *   pXParamData - pointer to parameter list
 *   szGroup     - group to check
 *   replyData   - buffer for answer
 *   replyLen    - answer length
 *   replyLenMax - max. answer lengthgth
 */
tcn00_Error cn20ParamCheckAll
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        const char           * szGroup,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax);

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20ParamCommitSession
 * ------------------------------------------------------------------
 * purpose:   write parameters
 *
 * parameters:
 *   szDbName    - database name
 *   pXParamData - pointer to parameter list
 *   replyData   - buffer for answer
 *   replyLen    - answer length
 *   replyLenMax - max. answer lengthgth
 */
tcn00_Error cn20ParamCommitSession
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax);

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20Param***
 * ------------------------------------------------------------------
 * purpose:   all the param commands of the CServer
 *
 * parameters:
 *   vcontrol    - CServer session data
 *   command     - CServer command structure
 *   replyData   - buffer for answer
 *   replyLen    - answer length
 *   replyLenMax - max. answer lengthgth
 *
 * return - the usual cserver return codes
 */
tcn00_Error cn20ParamDirectGet (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamDirectPut (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamDirectGetAll (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamDirectGetAllNext (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamDirectDel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamStartSession (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamExtGet (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamExtGetAll (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamExtGetAllNext (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetType (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetDefault (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetValue (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetProperties (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetHelp (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetExplain (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetFull (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamPut (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamCheckAll (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamPutConfirm (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamCommitSession (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamAbortSession (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamInit (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamCopy (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamAddDevSpace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamAddDevSpace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax,
      long          & nReturnSize);

tcn00_Error cn20ParamDelDevSpace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetDevSpace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamGetDevSpaceAll (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamCalcDevSpace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamRmFile (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamRestore (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamVersions (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamHistory (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20ParamHistoryNext (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn20CreateSrvParam
      ( tsp00_DbNamec   szDbName );
tcn00_Error cn20CreateSrvParam
      ( tsp00_DbNamec   szDbName,
        tsp00_DbNamec   szMaster );

tcn00_Error cn20SaveParamsInfoToDB
      ( const tsp00_DbNamec & szDbName);

void cn20FreeParamHistory(VControlDataT * vcontrol);

#endif /* _HCN20_H_ */

