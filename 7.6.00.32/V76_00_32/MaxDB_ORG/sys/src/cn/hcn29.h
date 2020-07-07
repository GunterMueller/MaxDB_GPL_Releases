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
/* ==================================================================
 * @lastChanged: "1998-05-07  13:23"
 *
 * @filename:    hcn29.h
 * @purpose:     "param utility functions"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * @Copyright (c) 1998-2005 SAP AG"
 *
 * ==================================================================
 *
 * responsible:   BerndV
 *
 * created:       1998-03-05 by BerndV
 *
 * purpose:       utility functions for all vcn2? modules
 *                               
 * ==================================================================
 */

#ifndef _HCN29_H_
#define _HCN29_H_

/* ------------------------------------------------------------------
 * INCLUDES
 * ------------------------------------------------------------------
 */
#include "gcn00.h"
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

/* SPECIFICATION INTERN FUNCTION cn29SearchDynamic
 * ------------------------------------------------------------------
 * purpose: search a dynamic description for the specified parameter
 *
 */
DBMSrvPar_Parameter * cn29SearchDynamic 
      ( DBMSrvPar_Parameter  * pParameter,
        const char           * szName );

/* SPECIFICATION INTERN FUNCTION cn29CopyParameter
 * ------------------------------------------------------------------
 * purpose: search a parameter record for the specified parameter
 *
 */
tcn00_Error cn29CopyParameter 
      ( DBMSrvPar_Parameter * pDest,
        DBMSrvPar_Parameter * pSource );

/* SPECIFICATION INTERN FUNCTION cn29PrintProperties
 * ------------------------------------------------------------------
 * purpose: print one property with value
 */
void cn29PrintProperties 
      ( char *             pBuffer,
        tcn002_XPPropClass  nClass, 
        DBMSrvPar_Parameter::XPPropValue  nValue );

/* SPECIFICATION INTERN FUNCTION cn29PrintExtendedGet
 * ------------------------------------------------------------------
 * purpose: extended parameter print for get
 */
void cn29PrintExtendedGet
      ( char              * pBuffer,
        DBMSrvPar_Parameter * pParameter );

/* SPECIFICATION INTERN FUNCTION cn29SyntaxError
 * ------------------------------------------------------------------
 * pupose: supply a text for the specified syntax errorcode
 */
void cn29SyntaxError (
        char          * pData,
        int           * pnDataLen,
        tcn002_XPError  nSyntaxError,
        const char    * szBuffer,
        const char    * szFile,
        long            nLine);

/* SPECIFICATION INTERN FUNCTION cn29StringToParamValue
 * ------------------------------------------------------------------
 * purpose:
 *    - copy a zero terminated string in the param record
 *    - strings will be placed in the param record using pascal
 *      representation
 *
 * parameters:
 *    XP_VALUE_REC       * pValue
 *       - pointer to param record where the value will be placed
 *    const char         * szValue
 *       - pointer to a zero terminated string representation of
 *         value
 *
 * return:
 *    XP_VALUE_REC *  - the param record (equal to parameter pValue)
 */
DBMSrvPar_Parameter::XPRecord * cn29StringToParamValue
    ( DBMSrvPar_Parameter::XPRecord * pValue,
      const char                    * szValue );

/* INTERN FUNCTION cn29ParamValueToString
 * ------------------------------------------------------------------
 * purpose:
 *    - copy value form a param record to zero terminated string
 *      representaion in the provided buffer
 *    - awaits strings in the param rescord in pascal representation
 *
 * parameters:
 *    char               * szValue
 *       - pointer to Buffer for the zero terminated string
 *         representation of value
 *    const XP_VALUE_REC * pValue
 *       - pointer to Param Record where the value will be found
 *
 * return:
 *    char *  - the string buffer (equal to szValue)
 */
char * cn29ParamValueToString  
    ( char                                * szValue,
      const DBMSrvPar_Parameter::XPRecord * pValue );

/* SPECIFICATION INTERN FUNCTION cn29IntToParamValue
 * ------------------------------------------------------------------
 * purpose:
 *    - copy a zero terminated string in the param record
 *    - strings will be placed in the param record using pascal
 *      representation
 *
 * parameters:
 *    XP_VALUE_REC       * pValue
 *       - pointer to param record where the value will be placed
 *    tsp00_Int4           nValue
 *       - the value
 *
 * return:
 *    XP_VALUE_REC *  - the param record (equal to parameter pValue)
 */
DBMSrvPar_Parameter::XPRecord * cn29IntToParamValue
    ( DBMSrvPar_Parameter::XPRecord * pRecord,
      tsp00_Int4                      nValue );

/* INTERN FUNCTION cn29ParamValueToInt
 * ------------------------------------------------------------------
 * purpose:
 *    - copy value form a param record to zero terminated string
 *      representaion in the provided buffer
 *    - awaits strings in the param rescord in pascal representation
 *
 * parameters:
 *    const XP_VALUE_REC * pValue
 *       - pointer to Param Record where the value will be found
 *
 * return:
 *    tsp00_Int4  - the value
 */
tsp00_Int8 cn29ParamValueToInt  
    ( const DBMSrvPar_Parameter::XPRecord * pRecord );

/* SEPCIFICATION INTERN FUNCTION cn29ParamGetAllIntern
 * ------------------------------------------------------------------
 */
tcn00_Error cn29ParamGetAllIntern
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        const char        * szCommand,
        char              * replyData,
        int               * replyLen,
        const int           replyLenMax,
        const bool          bExtended,
        const bool          bNext);

/* SEPCIFICATION INTERN FUNCTION cn29ParamGetIntern
 * ------------------------------------------------------------------
 */
tcn00_Error cn29ParamGetIntern
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        const char        * szParameter,
        char              * replyData,
        int               * replyLen,
        int                 replyLenMax,
        tcn002_XPRequest    nRequest);

/* SPECIFICATION INTERN FUNCTION cn29SoftPutIntern
 * ------------------------------------------------------------------
 */
tcn00_Error cn29SoftPutIntern
      ( DBMSrvPar_Parameters * pXParamData,
        const char           * szParameter,
        const char           * szValue,
        const char           * szComment,
        bool                   bIgnoreReadOnly = false,
        DBMSrvPar_Parameter::XPWriteMode oWriteMode = DBMSrvPar_Parameter::XPWriteModeUnknown);

/* SPECIFICATION INTERN FUNCTION cn29ParamStartSessionDBIndependent
 *---------------------------------------------------------------------------*
 */
tcn00_Error cn29ParamStartSessionDBIndependent
      ( DBMSrvPar_Parameters * & pXParamData,
        const char           *   szDBName,
        char                 *   replyData,
        int                  *   replyLen,
        int                      replyLenMax);

/* SPECIFICATION INTERN FUNCTION cn29ParamStartSession
 *---------------------------------------------------------------------------*
 */
tcn00_Error cn29ParamStartSession
      ( tsp00_DbNamec         szDbName,
        DBMSrvPar_Parameters * & pXParamData,
        char              *   replyData,
        int               *   replyLen,
        int                   replyLenMax,
        bool                  bReadOnly = false);

/* SPECIFICATION INTERN FUNCTION cn29CheckRange
 *---------------------------------------------------------------------------*
 */
tcn00_Error cn29CheckRange
      ( DBMSrvPar_Parameter * pParameter,
        const char        * szValue );

/* SPECIFICATION INTERN FUNCTION cn29CheckInstance 
 *---------------------------------------------------------------------------*
 */
tcn00_Error cn29CheckInstance 
      ( const char        * szInstance,
        DBMSrvPar_Parameters * pXParamData);

/* SPECIFICATION INTERN FUNCTION cn29AnswerParamNotFound
 *---------------------------------------------------------------------------*
 */
tcn00_Error cn29AnswerParamNotFound 
      ( char              * replyData,
        int               * replyLen,
        const char        * szParameter,
        DBMSrvPar_Parameter * pParameter,
        bool                bDirect = false);

/* SPECIFICATION INTERN FUNCTION cn29WriteParamHistory
 *---------------------------------------------------------------------------*
 */
void cn29WriteParamHistory 
      ( tsp00_DbNamec       szDBName,
        DBMSrvPar_Parameter * pParameter, 
        char                cOperation);

#endif /* _HCN29_H_ */
