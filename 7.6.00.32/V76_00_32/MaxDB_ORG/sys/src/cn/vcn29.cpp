/* @lastChanged: "1998-06-05  15:29"
 * @filename:    vcn29.cpp
 * @filename:    vcn29.cpp
 * @purpose:     "param access service functions"
 * @release:     7.1. / 6.2
 * @see:         "-.-"
 *
 * @Copyright (c) 1998-2005 SAP AG"
 *
 * ==================================================================
 *
 * responsible:   BerndV
 *
 * created:       1998-03-04 by BerndV
 *
 * purpose:       service functions for all vcn2*c.cpp modules
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
 * IMPORT
 * ------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>


#include "heo02.h"
#include "heo01.h"
#include "hsp02.h"
#include "heo06.h"
#include "hsp100.h"

#include "hcn21.h"
#include "hcn23.h"
#include "hcn28.h"
#include "hcn50.h"
#include "hcn52.h"
#include "hcn90.h"

#include "DBM/Srv/Deprecated/DBMSrvDep_DirtyReply.hpp"
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

#include "hcn29.h"

/* SPECIFICATION PRIVATE FUNCTION cn29_CopyLines
 * ------------------------------------------------------------------
 * purpose: copy a line section from source to dest
 */
static tcn00_Error cn29_CopyLines
      ( DBMSrvPar_XPLine ** pDest,
        DBMSrvPar_XPLine *  pSource );

/* SPECIFICATION PRIVATE FUNCTION cn29_RequestedProperties
 * ------------------------------------------------------------------
 */
typedef DBMSrvPar_Parameter::XPPropValue tReqPropArray[XPPropClassMAX][XP_MAX_VALUES_PER_CLASS_CN002];

static tcn00_Error cn29_RequestedProperties
      ( const char         * szLine,
        tReqPropArray        xpProperties);

/* ------------------------------------------------------------------
 * START OF IMPLEMENTATION
 * ------------------------------------------------------------------
 */

/* ------------------------------------------------------------------
 * IMPLEMENTATION INTERN FUNCTION cn29SearchDynamic
 * ------------------------------------------------------------------
 */
DBMSrvPar_Parameter * cn29SearchDynamic
      ( DBMSrvPar_Parameter * pDynHelp,
        const char        * szName )
{
  tcn002_XpValueName   szDynamic;
  tcn002_XpValueName   szCheck;
  char               * pEndDyn     = NULL;
  const char         * pEndParam   = NULL;
  bool                 bFound = false;
  tcn00_DevspaceParams aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
  int                  nIndex  = 0;
  int                  nNumber = 0;

  /* search for dynamic match */
  bFound = false;
  while ((pDynHelp != NULL) && (bFound == false)) {
    if (pDynHelp->xpProperties[XPPropClassDynamic] == DBMSrvPar_Parameter::XPPropValueYes) {
      /* special check for dynamics */
      SAPDB_strcpy(szDynamic, pDynHelp->szParameterName);
      pEndDyn = strstr(szDynamic, XP_KEY_DYNAMIC_CN002);
      pEndDyn[0] = CHAR_STRINGTERM_CN90;
      pEndDyn++;
      pEndParam = &szName[strlen(szName) - strlen(pEndDyn)];
      if ( (strncmp(szName, szDynamic, strlen(szDynamic)) == 0) &&
           (strncmp(pEndDyn, pEndParam , strlen(pEndDyn))  == 0)    ) {

        bFound = true;

        // check for right number format of volumes
        nIndex = 0;
        while (aParamTable[nIndex].nDevSpace != XPDevSpaceUnknown_ecn00) {
          if ( strncmp(aParamTable[nIndex].szName, szDynamic, strlen(szDynamic)) == 0) {
            nNumber = atoi(&szName[strlen(szDynamic)]);
            sprintf(szCheck, aParamTable[nIndex].szName, nNumber);
            bFound = (strcmp(szCheck, szName) == 0);
            break;
          } else if ( strncmp(aParamTable[nIndex].szType, szDynamic, strlen(szDynamic)) == 0) {
            nNumber = atoi(&szName[strlen(szDynamic)]);
            sprintf(szCheck, aParamTable[nIndex].szType, nNumber);
            bFound = (strcmp(szCheck, szName) == 0);
            break;
          } else if ( strncmp(aParamTable[nIndex].szSize, szDynamic, strlen(szDynamic)) == 0) {
            nNumber = atoi(&szName[strlen(szDynamic)]);
            sprintf(szCheck, aParamTable[nIndex].szSize, nNumber);
            bFound = (strcmp(szCheck, szName) == 0);
            break;
          } // end if
          nIndex++;
        } // end while

      } /* end if */
    } /* end if */
    
    pDynHelp = (bFound) ? pDynHelp : pDynHelp->pNext;
  } /* end while */

  return pDynHelp;
} /* end cn29SearchDynamic */

/* IMPLEMENTATION INTERN FUNCTION cn29CopyParameter
 * ------------------------------------------------------------------
 */
tcn00_Error cn29CopyParameter
      ( DBMSrvPar_Parameter * pDest,
        DBMSrvPar_Parameter * pSource )
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  long                 i            = 0;

  SAPDB_strcpy(pDest-> szParameterName, pSource->szParameterName);
  SAPDB_strcpy(pDest->szDefValue,       pSource->szDefValue);
  SAPDB_memcpy(&(pDest->xpRecord), &(pSource->xpRecord), sizeof(DBMSrvPar_Parameter::XPRecord));
  pDest->bHelpExist     = pSource->bHelpExist;
  pDest->bParamExist    = pSource->bParamExist;

  for (i = 0; i < XPPropClassMAX; i++) {
    pDest->xpProperties[i] = pSource->xpProperties[i];
  } /* end for */

  pDest->pNext          = NULL;

  /* lines */
  nFuncStatus = cn29_CopyLines(&(pDest->pCode), pSource->pCode);
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn29_CopyLines(&(pDest->pHelp), pSource->pHelp);
  } /* end if */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn29_CopyLines(&(pDest->pConstraint), pSource->pConstraint);
  } /* end if */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn29_CopyLines(&(pDest->pExplain), pSource->pExplain);
  } /* end if */

  return nFuncStatus;
} /* end cn29CopyParameter */

/* IMPLEMENTATION INTERN FUNCTION cn29_RequestedProperties
 * ------------------------------------------------------------------
 */
static tcn00_Error cn29_RequestedProperties
      ( const char         * szLine,
        tReqPropArray        xpProperties)
{
  tcn00_Error           nFuncStatus   = OK_CN00;
  int                   nClassIndex   = 0;
  int                   nValueIndex   = 0;
  int                   nOrIndex      = 0;
  char                * pClass        = NULL;
  char                * pValue        = NULL;
  char                * pOneValue     = NULL;
  long                  nToken        = 1;
  char                  szProperty[XP_PARAMLINE_LENGTH_MXCN002];
  struct {
    DBMSrvPar_Parameter::XPPropValue   nValue;
    const char        * szText;
  }                     aValueTable[] = XP_PROP_VALUES_CN002;
  struct tcn002_PropClassTable {
    tcn002_XPPropClass   nClass;
    DBMSrvPar_Parameter::XPPropValue   nDefault;
    const char        * szText;
  }                     aClassTable[] = XP_PROP_CLASSES_CN002;

  /* init array */
  for (nClassIndex = 0; nClassIndex < XPPropClassMAX; nClassIndex ++) {
    for (nOrIndex = 0; nOrIndex < XP_MAX_VALUES_PER_CLASS_CN002; nOrIndex ++) {
      xpProperties[nClassIndex][nOrIndex] = DBMSrvPar_Parameter::XPPropValueUnknown;
    } /* end for */
  } /* end for */

  while (cn90GetToken(szLine, szProperty, nToken++, XP_PARAMLINE_LENGTH_MXCN002) && (nFuncStatus == OK_CN00)) {
    pClass = strtok(szProperty, "=");
    pValue = strtok(NULL, "=");
    pOneValue = strtok(pValue, ",");

    if ((pClass != NULL) && (pOneValue != NULL) ) {

      nOrIndex = 0;
      while (pOneValue != NULL && nFuncStatus == OK_CN00) {
        nClassIndex=0;
        nValueIndex=0;
        /* search class */
        while ( (aClassTable[nClassIndex].nClass != XPPropClassUnknown) && (strcmp(aClassTable[nClassIndex].szText, pClass) != 0) ) {
          nClassIndex++;
        } /* end while */
        /* search Value */
        while ( (aValueTable[nValueIndex].nValue != DBMSrvPar_Parameter::XPPropValueUnknown) && (strcmp(aValueTable[nValueIndex].szText, pOneValue) != 0) ) {
          nValueIndex++;
        } /* end while */

        if ((aValueTable[nValueIndex].nValue != DBMSrvPar_Parameter::XPPropValueUnknown) && (aClassTable[nClassIndex].nClass != XPPropClassUnknown) &&  nOrIndex < XP_MAX_VALUES_PER_CLASS_CN002) {
          xpProperties[aClassTable[nClassIndex].nClass][nOrIndex] = aValueTable[nValueIndex].nValue;
        } else {
          nFuncStatus =  ERR_PARAM_CN00;
        } /* end if */

        nOrIndex++;
        pOneValue = strtok(NULL, ",");
      } // end while

    } else {
      nFuncStatus =   ERR_PARAM_CN00;
    } /* end if */

  } /* end while */

  return nFuncStatus;
} /* end cn29_RequestedProperties */

/* IMPLEMENTATION INTERN FUNCTION cn29PrintProperties
 * ------------------------------------------------------------------
 */
void cn29PrintProperties
      ( char               * pBuffer,
        tcn002_XPPropClass   nClass,
        DBMSrvPar_Parameter::XPPropValue   nValue,
        tsp00_C512c          szValue)
{
  struct {
    DBMSrvPar_Parameter::XPPropValue   nValue;
    const char         * szText;
  }                     aValueTable[] = XP_PROP_VALUES_CN002;
  struct tcn002_PropClassTable {
    tcn002_XPPropClass   nClass;
    DBMSrvPar_Parameter::XPPropValue   nDefault;
    const char         * szText;
  }                     aClassTable[] = XP_PROP_CLASSES_CN002;
  int                   j                 = 0;

  if (nClass != XPPropClassUnknown) {
    /* search value */
    while ( (aValueTable[j].nValue != DBMSrvPar_Parameter::XPPropValueUnknown) && (aValueTable[j].nValue != nValue) ) {
      j++;
    } /* end while */
    if (aValueTable[j].nValue == DBMSrvPar_Parameter::XPPropValueUnknown) {
      sprintf(pBuffer, "%s%s%s%s", aClassTable[nClass].szText, VALUE_SEPSTRING_CN00, szValue.asCharp(), LINE_SEPSTRING_CN00);
    } else {
      sprintf(pBuffer, "%s%s%s%s", aClassTable[nClass].szText, VALUE_SEPSTRING_CN00, aValueTable[j].szText, LINE_SEPSTRING_CN00);
    } // end if
  } /* end if */

} /* end  cn29PrintProperties */

/* IMPLEMENTATION INTERN FUNCTION cn29PrintExtendedGet
 * ------------------------------------------------------------------
 */
void cn29PrintExtendedGet
      ( char              * pBuffer,
        DBMSrvPar_Parameter * pParameter )
{
  tcn00_ValueToText  aTable[]    = XP_TYPES_CN002;
  long               i           = 0;
  tcn002_XpValueName aDummy;

  /* search typ */
  while ( (strlen(aTable[i].szText) > 0) && (pParameter->xpRecord.xpType != aTable[i].nValue) ) {
    i++;
  } /* end while */

  // print param
  sprintf(pBuffer,
          "%-*s%s%s%s%s%s",
          aDummy.size() + 1,
          pParameter->szParameterName.asCharp(),
          VALUE_SEPSTRING_CN00,
          aTable[i].szText,
          VALUE_SEPSTRING_CN00,
          (pParameter->xpRecord.xpType != DBMSrvPar_RTEConfParameter::t_CryptInfo) ?
           pParameter->szNewValue.asCharp() : "",
          LINE_SEPSTRING_CN00);

} /* end cn29PrintExtendedGet */

/* IMPLEMENTATION PRIVATE FUNCTION cn29_CopyLines
 * ------------------------------------------------------------------
 */
static tcn00_Error cn29_CopyLines
      ( DBMSrvPar_XPLine ** pDest,
        DBMSrvPar_XPLine *  pSource )
{
  tcn00_Error           nFuncStatus  = OK_CN00;
  DBMSrvPar_XPLine  *   pNewLine    = NULL;
  DBMSrvPar_XPLine  *   pLastLine   = NULL;

  /* code lines */
  while (pSource && (nFuncStatus == OK_CN00)) {
    pNewLine = new DBMSrvPar_XPLine(pSource->Line(), pSource->Type(), pSource->Number());

    if ((pNewLine == NULL) || (pNewLine->Line() == NULL)) {
      nFuncStatus = ERR_MEM_CN00;
    } else {

      if (*pDest == NULL) {
        *pDest = pNewLine;
      } else {
        pLastLine->SetNext(pNewLine);
      } /* end if */
      pLastLine = pNewLine;

      pSource = pSource->Next();

    } /* end if */
  } /* end while */

  return nFuncStatus;
} /* end cn29_CopyLines */

/* IMPLEMENATION INTERN FUNCTION cn29SyntaxError
 * ------------------------------------------------------------------
 */
void cn29SyntaxError(
        char          * pData,
        int           * pnDataLen,
        tcn002_XPError   nSyntaxError,
        const char    * szBuffer,
        const char    * szFile,
        long            nLine)
{
  int i = 0;
  tcn00_ValueToText errorArray[] = XPERROR_TO_TEXT_CN002;

  while ((errorArray[i].nValue !=  XPERR_UNKNOWN_CN002) && (errorArray[i].nValue !=  nSyntaxError)) {
    i++;
  } /* end while */

  sprintf (&pData[strlen(pData)], "%d,%s\n%s(%ld) : %s\n", nSyntaxError,
                                                           errorArray[i].szText,
                                                           szFile,
                                                           nLine,
                                                           szBuffer);
  *pnDataLen = (int)strlen(pData);

} /* end cn29SyntaxError */

/* IMPLEMENTATION INTERN FUNCTION cn29StringToParamValue
 * ------------------------------------------------------------------
 * purpose: copy a zero terminated string in the param value record
 *          (for strings use pascal representation)
 */
DBMSrvPar_Parameter::XPRecord * cn29StringToParamValue
    ( DBMSrvPar_Parameter::XPRecord * pRecord,
      const char                    * szValue )
{
  char          szTemp[PARAMETER_MAXLEN_CN90];
  tsp00_Name    szName;       /* char [18] */
  tsp00_CryptPw cryptPwTemp;  /* char [24] */

  /* initialize the whole memory of the union with spaces */
  memset(&(pRecord->xpValue), CHAR_PASCALFILL_CN90, sizeof(pRecord->xpValue));

  switch ( pRecord->xpType ) {
    case DBMSrvPar_RTEConfParameter::t_Integer:
      pRecord->xpValue.xpValueInt = atol ( szValue );
      break;
    case DBMSrvPar_RTEConfParameter::t_Real:
      pRecord->xpValue.xpValueReal = atof ( szValue );
      break;
    case DBMSrvPar_RTEConfParameter::t_String:
      SAPDB_strcpy( pRecord->xpValue.xpValueString, szValue);
      break;
    case DBMSrvPar_RTEConfParameter::t_CryptInfo:
    case xp_crypt_type  :
      SAPDB_strcpy(szTemp, szValue);
      if (strlen(szTemp) == sizeof(tsp00_CryptName) * 2) {
        cn90Uncrypt(szTemp, false);
      } /* end if */
      cn90StringCtoP(szName, szTemp, sizeof(tsp00_Name));
      s02encrypt(szName, cryptPwTemp);
      SAPDB_memcpy(&(pRecord->xpValue.xpValueCrypt[0]), &(cryptPwTemp[0]), sizeof(pRecord->xpValue.xpValueCrypt));
      break;
  } /* end switch */

  return pRecord;
} /* end cn90StringToParamValue */

/* IMPLEMENTATION INTERN FUNCTION cn29ParamValueToString
 * ------------------------------------------------------------------
 */
char * cn29ParamValueToString
    ( char                                * szValue,
      const DBMSrvPar_Parameter::XPRecord * pRecord )
{

  switch ( pRecord->xpType ) {
    case DBMSrvPar_RTEConfParameter::t_Integer:
      sprintf ( szValue , "%ld" , (long) pRecord->xpValue.xpValueInt );
      break;
    case DBMSrvPar_RTEConfParameter::t_Real:
      sprintf ( szValue , "%f" , (double) pRecord->xpValue.xpValueReal );
      break;
    case DBMSrvPar_RTEConfParameter::t_String:
      SAPDB_strcpy(szValue, pRecord->xpValue.xpValueString.asCharp());
      break;
    case DBMSrvPar_RTEConfParameter::t_CryptInfo:
      {
      tsp00_Namec    szClearPwd;
      tsp00_CryptName  oTemp;
      SAPDB_memcpy(&(oTemp[0]), &(pRecord->xpValue.xpValueCrypt[0]), sizeof(oTemp));
      cn90CalculateUncrypt(oTemp, szClearPwd, true);
      SAPDB_strcpy(szValue, szClearPwd);
      }
      break;
  } /* end switch */

  return szValue;
} /* end cn29ParamValueToString */

/* IMPLEMENTATION INTERN FUNCTION cn29IntToParamValue
 * ------------------------------------------------------------------
 */
DBMSrvPar_Parameter::XPRecord * cn29IntToParamValue
    ( DBMSrvPar_Parameter::XPRecord * pRecord,
      tsp00_Int4                      nValue )
{
  switch ( pRecord->xpType ) {
    case DBMSrvPar_RTEConfParameter::t_Integer:
      pRecord->xpValue.xpValueInt = nValue;
      break ;
  } // end switch

  return pRecord;
} // end cn29IntToParamValue

/* IMPLEMENTATION INTERN FUNCTION cn29ParamValueToInt
 * ------------------------------------------------------------------
 */
tsp00_Int8 cn29ParamValueToInt
    ( const DBMSrvPar_Parameter::XPRecord * pRecord )
{
  tsp00_Int8 nValue = -1;
  switch ( pRecord->xpType ) {
    case DBMSrvPar_RTEConfParameter::t_Integer:
      nValue = pRecord->xpValue.xpValueInt;
      break ;
  } // end switch

  return nValue;
} // end cn29ParamValueToInt


/* IMPLEMENTATION INTERN FUNCTION cn29ParamGetAllIntern
 * ------------------------------------------------------------------
 * purpose: get all parameters with values from param file
 *
 */
tcn00_Error cn29ParamGetAllIntern
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        const char        * szCommand,
        char              * replyData,
        int               * replyLen,
        const int           replyLenMax,
        const bool          bExtended,
        const bool          bNext)

{
  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter  * pParameter  = NULL;
  bool                 bInit = false;
  char               * pCurrPos;
  tcn002_XpValueString szValue;
  tReqPropArray        xpProperties;
  bool                 bMatch      = true;
  bool                 bOrMatch    = false;
  long                 nClassIndex = 0;
  long                 nOrIndex    = 0;
  tcn002_XpValueName   aDummy;

  /* init param management for commandline use or tcontrol or rcontrol */
  if (pXParamData == NULL) {
    nFuncReturn = cn29ParamStartSession( szDbName, pXParamData, replyData, replyLen, replyLenMax, true);
    bInit = true;
  } /* end if */

  if (nFuncReturn == OK_CN00) {

    if (bExtended) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, cn29_RequestedProperties(szCommand, xpProperties));
    } /* end if */

    if (nFuncReturn == OK_CN00) {
      /* Output */
      sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
      pCurrPos = replyData + strlen (replyData);

      pParameter = pXParamData->pFirst;

      if (bNext) {
        if (cn00DBMServerData::vcontrol()->szLastParameter.length() > 0) {
          while (pParameter != NULL) {
            if (strcmp(cn00DBMServerData::vcontrol()->szLastParameter, pParameter->szParameterName) == 0) {
              break;
            } // end if
            pParameter = pParameter->pNext;
          } // end while
        } else {
          pParameter = NULL;
        } // end if
      } // end if
    } /* end if */

    while ((pParameter != NULL) && (nFuncReturn == OK_CN00)) {

      if (bExtended) {
        /* check propeties */
        bMatch = true;
        for (nClassIndex = 0; nClassIndex < XPPropClassMAX; nClassIndex ++) {
          if (xpProperties[nClassIndex][0] != DBMSrvPar_Parameter::XPPropValueUnknown) {
            bOrMatch = false;
            for (nOrIndex = 0; nOrIndex < XP_MAX_VALUES_PER_CLASS_CN002; nOrIndex++) {
              if (xpProperties[nClassIndex][nOrIndex] != DBMSrvPar_Parameter::XPPropValueUnknown) {
                bOrMatch = bOrMatch || xpProperties[nClassIndex][nOrIndex] == pParameter->xpProperties[nClassIndex];
              } // end if
            } // end for
            bMatch = bMatch && bOrMatch;
          } // end if
        } // end for

        /* print only requested parameters */
        if ( bMatch) {
          if ((int)(strlen(replyData) + (XP_PARAMLINE_LENGTH_MXCN002 * 2)) > replyLenMax) {
            cn00DBMServerData::vcontrol()->szNextCommand.rawAssign("param_extgetallnext");
            cn00DBMServerData::vcontrol()->nNextCommandSkip = 0;
            cn00DBMServerData::vcontrol()->szLastParameter.rawAssign(pParameter->szParameterName);
            break;
          } else {
            cn29PrintExtendedGet(pCurrPos, pParameter);
            pCurrPos = replyData + strlen (replyData);
          } /* end if */
        } /* end if */

      } else {
        /* in normal mode print only param parameter */
        if (pParameter->bParamExist) {
          if ((int)(strlen(replyData) + XP_PARAMLINE_LENGTH_MXCN002) > replyLenMax) {
            cn00DBMServerData::vcontrol()->szNextCommand.rawAssign("param_directgetallnext");
            cn00DBMServerData::vcontrol()->nNextCommandSkip = 0;
            cn00DBMServerData::vcontrol()->szLastParameter.rawAssign(pParameter->szParameterName);
            break;
          } else {
            sprintf(pCurrPos,
                    "%-*s%s%s%s",
                    aDummy.size() + 1,
                    pParameter->szParameterName.asCharp(),
                    VALUE_SEPSTRING_CN00,
                    (pParameter->xpRecord.xpType != DBMSrvPar_RTEConfParameter::t_CryptInfo) ?
                      cn29ParamValueToString(szValue, &(pParameter->xpRecord)) :
                      "",
                    LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);
          } /* end if */
        } /* end if */
      } /* end if */

      pParameter = pParameter->pNext;

    } /* end while */
    if (pParameter == NULL) {
      cn00DBMServerData::vcontrol()->szLastParameter.rawAssign("");
      cn00DBMServerData::vcontrol()->szNextCommand.rawAssign("");
      cn00DBMServerData::vcontrol()->nNextCommandSkip = 0;
    } // end if

    *replyLen = (int)strlen(replyData);
  } /* end if */

  /* if init then stop param management */
  if (bInit && (pXParamData != NULL)) {
    delete pXParamData;
    pXParamData = NULL;
  } /* end if */

  return nFuncReturn;
} /* end cn29ParamGetAllIntern */

/* IMPLEMENTATION INTERN FUNCTION cn29ParamGetIntern
 * ------------------------------------------------------------------
 * purpose: get the specified data about the parameter
 */
tcn00_Error cn29ParamGetIntern
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        const char        * szParameter,
        char              * replyData,
        int               * replyLen,
        int                 replyLenMax,
        tcn002_XPRequest nRequest )
{
  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter   * pParameter  = NULL;
  DBMSrvPar_Parameter   * pDynamic    = NULL;
  DBMSrvPar_Parameter     aTmpParam;
  bool                 bInit  = false;
  tcn002_XpValueString szValue;
  char               * pCurrPos;
  tcn00_ValueToText    aTable[]    = XP_TYPES_CN002;
  long                 i           = 0;
  DBMSrvPar_XPLine   * pLine       = NULL;

  /* init param management for commandline use or tcontrol or rcontrol */
  if (pXParamData == NULL) {
    nFuncReturn = cn29ParamStartSession( szDbName, pXParamData, replyData, replyLen, replyLenMax, true);
    bInit = true;
  } /* end if */

  if (nFuncReturn == OK_CN00) {

    if (strlen(szParameter) > 0) {

      /* search parameter in List */
      pParameter = pXParamData->Get(szParameter);

      if (pParameter == NULL) {
        pDynamic = cn29SearchDynamic(pXParamData->pFirst, szParameter);

        /* is there an dynamic then generate tmp param */
        if (pDynamic != NULL) {
          pParameter = &aTmpParam;

          /* copy data from dynamic record */
          nFuncReturn = cn29CopyParameter(pParameter, pDynamic);
          cn90AnswerIError(replyData, replyLen, nFuncReturn);

          /* clear the dynamic/intern property */
          pParameter->xpProperties[XPPropClassDynamic] = DBMSrvPar_Parameter::XPPropValueNo;
          pParameter->xpProperties[XPPropClassIntern] = DBMSrvPar_Parameter::XPPropValueNo;
          /* set the real name */
          cn90SaveStrcpy(pParameter->szParameterName, szParameter, sizeof(tcn002_XpValueName));
        } /* end if */
      } /* end if */

      /* copy match to reply buffer */
      if (pParameter != NULL) {
        /* Output OK */
        sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
        pCurrPos = replyData + strlen (replyData);

        /* Output Requested Data */
        switch (nRequest) {
          case XPRequestDirectGet:
            /* in normal mode print only param-parameter */
            if (pParameter->bParamExist) {
              sprintf(pCurrPos,
                      "%s%s%s%s",
                      pParameter->szParameterName.asCharp(),
                      VALUE_SEPSTRING_CN00,
                      (pParameter->xpRecord.xpType != DBMSrvPar_RTEConfParameter::t_CryptInfo) ?
                        cn29ParamValueToString(szValue, &(pParameter->xpRecord)) :
                        "",
                      LINE_SEPSTRING_CN00);
            } else {
              nFuncReturn = cn29AnswerParamNotFound(replyData, replyLen, szParameter, pXParamData->pFirst, true);
            } /* end if */
            break;

          case XPRequestExtGet:
            /* in extended */
            cn29PrintExtendedGet(pCurrPos, pParameter);
            break;

          case XPRequestType:
            /* type */
            while ( (strlen(aTable[i].szText) > 0) && (pParameter->xpRecord.xpType != aTable[i].nValue) ) {
              i++;
            } /* end while */
            sprintf(pCurrPos, "%s%s", aTable[i].szText, LINE_SEPSTRING_CN00);
            break;

          case XPRequestDefault:
            /* Default */
            sprintf(pCurrPos, "%s%s", pParameter->szDefValue.asCharp(), LINE_SEPSTRING_CN00);
            break;

          case XPRequestValue:
            /* value */
            if (pParameter->xpRecord.xpType != DBMSrvPar_RTEConfParameter::t_CryptInfo) {
              sprintf(pCurrPos, "%s%s", pParameter->szNewValue.asCharp(), LINE_SEPSTRING_CN00);
            } else {
              sprintf(pCurrPos, "%s", LINE_SEPSTRING_CN00);
            } /* end if */
            break;

          case XPRequestValueIntern:
            /* value */
            sprintf(pCurrPos, "%s", pParameter->szNewValue.asCharp(), LINE_SEPSTRING_CN00);
            if (pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_CryptInfo) {
              cn90Uncrypt(pCurrPos, false);
            } /* end if */
            sprintf(pCurrPos, "%s%s", pCurrPos, LINE_SEPSTRING_CN00);
            break;

          case XPRequestProperties:
            /* properties */
            for (i = 0; i < XPPropClassMAX; i++) {
              if (i != XPPropClassClass) {
                cn29PrintProperties(pCurrPos, (tcn002_XPPropClass) i, pParameter->xpProperties[i], pParameter->szProperties[i]);
                pCurrPos = replyData + strlen (replyData);
              } /* end if */
            } // end for
            break;

          case XPRequestClass:
            /* class */
            {
              sprintf(pCurrPos, "%s%s", XP_KEY_CLASS_CN002, VALUE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
              i = 0;
              struct {
                DBMSrvPar_Parameter::XPClassValue   nValue;
                const char        * szText;
              }                     aClassValueTable[] = XP_CLASS_VALUES_CN002;
              while (aClassValueTable[i].nValue != DBMSrvPar_Parameter::XPClassValueUnknown) {
                if (aClassValueTable[i].nValue & pParameter->nClasses) {
                  sprintf(pCurrPos, "%s ", aClassValueTable[i].szText);
                  pCurrPos = replyData + strlen (replyData);
                } // end if
                ++i;
              } // end while
              sprintf(pCurrPos, "%s", LINE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
            } // end block
            break;

          case  XPRequestGood:
            /* good value */
            sprintf(pCurrPos, "%s%s%s%s", XP_KEY_GOOD_CN002, VALUE_SEPSTRING_CN00, pParameter->szGoodValue.asCharp(), LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);
            break;

          case  XPRequestActive:
            /* active value */
            sprintf(pCurrPos, "%s%s%s%s", XP_KEY_ACTIVE_CN002, VALUE_SEPSTRING_CN00, pParameter->szActiveValue.asCharp(), LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);
            break;

          case XPRequestHelp:
            /* help */
            pLine = pParameter->pHelp;
            while (pLine != NULL) {
              sprintf(pCurrPos, "%s%s", pLine->Line(), LINE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
              pLine = pLine->Next();
            } /* end while */
            break;

          case XPRequestExplain:
            /* explain */
            pLine = pParameter->pExplain;
            while (pLine != NULL) {
              sprintf(pCurrPos, "%s%s", pLine->Line(), LINE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
              pLine = pLine->Next();
            } /* end while */
            break;

          case XPRequestFull:
            /* type */
            while ( (strlen(aTable[i].szText) > 0) && (pParameter->xpRecord.xpType != aTable[i].nValue) ) {
              i++;
            } /* end while */
            sprintf(pCurrPos, "%s%s", aTable[i].szText, LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);

            /* Default */
            sprintf(pCurrPos, "%s%s", pParameter->szDefValue.asCharp(), LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);

            /* value */
            if (pParameter->xpRecord.xpType != DBMSrvPar_RTEConfParameter::t_CryptInfo) {
              sprintf(pCurrPos, "%s%s", pParameter->szNewValue.asCharp(), LINE_SEPSTRING_CN00);
            } else {
              sprintf(pCurrPos, "%s", LINE_SEPSTRING_CN00);
            } /* end if */
            pCurrPos = replyData + strlen (replyData);

            /* properties */
            for (i = 0; i < XPPropClassMAX; i++) {
              if (i != XPPropClassClass) {
                cn29PrintProperties(pCurrPos, (tcn002_XPPropClass) i, pParameter->xpProperties[i], pParameter->szProperties[i]);
                pCurrPos = replyData + strlen (replyData);
              } /* end if */
            } /* end for */
            pCurrPos = replyData + strlen (replyData);

            /* class */
            {
              sprintf(pCurrPos, "%s%s", XP_KEY_CLASS_CN002, VALUE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
              i = 0;
              struct {
                DBMSrvPar_Parameter::XPClassValue   nValue;
                const char        * szText;
              }                     aClassValueTable[] = XP_CLASS_VALUES_CN002;
              while (aClassValueTable[i].nValue != DBMSrvPar_Parameter::XPClassValueUnknown) {
                if (aClassValueTable[i].nValue & pParameter->nClasses) {
                  sprintf(pCurrPos, "%s ", aClassValueTable[i].szText);
                  pCurrPos = replyData + strlen (replyData);
                } // end if
                ++i;
              } // end while
              sprintf(pCurrPos, "%s", LINE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
            } // end block

            /* good value */
            sprintf(pCurrPos, "%s%s%s%s", XP_KEY_GOOD_CN002, VALUE_SEPSTRING_CN00, pParameter->szGoodValue.asCharp(), LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);

            /* active value */
            sprintf(pCurrPos, "%s%s%s%s", XP_KEY_ACTIVE_CN002, VALUE_SEPSTRING_CN00, pParameter->szActiveValue.asCharp(), LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);

            /* help */
            sprintf(pCurrPos, "%s%s", XP_KEY_HELP_CN002, LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);

            pLine = pParameter->pHelp;
            while (pLine != NULL) {
              sprintf(pCurrPos, "%s%s", pLine->Line(), LINE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
              pLine = pLine->Next();
            } /* end while */
            pCurrPos = replyData + strlen (replyData);

            /* explain */
            sprintf(pCurrPos, "%s%s", XP_KEY_EXPLAIN_CN002, LINE_SEPSTRING_CN00);
            pCurrPos = replyData + strlen (replyData);

            pLine = pParameter->pExplain;
            while (pLine != NULL) {
              sprintf(pCurrPos, "%s%s", pLine->Line(), LINE_SEPSTRING_CN00);
              pCurrPos = replyData + strlen (replyData);
              pLine = pLine->Next();
            } /* end while */
            pCurrPos = replyData + strlen (replyData);
            break;

         } /* end switch */
      } else {
        nFuncReturn = cn29AnswerParamNotFound(replyData, replyLen, szParameter, pXParamData->pFirst, (nRequest == XPRequestDirectGet));
      } /* end if */

    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } /* end if */

  } /* end if */

  /* if init then stop param management */
  if (bInit && (pXParamData != NULL)) {
    delete pXParamData;
    pXParamData = NULL;
  } /* end if */

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end cn29ParamGetIntern */

/* IMPLEMENTATION INTERN FUNCTION cn29CheckRange
 * ------------------------------------------------------------------
 */
tcn00_Error cn29CheckRange
      ( DBMSrvPar_Parameter * pParameter,
        const char        * szValue )
{
  tcn00_Error        nFuncReturn   = OK_CN00;
  char               szBuffer[XP_STRING_LENGTH_MXCN002];
  char               szToken[XP_STRING_LENGTH_MXCN002];
  int                nToken;
  int                nIndex;

  // check Valueset
  if (nFuncReturn == OK_CN00) {
    if (strlen(pParameter->szProperties[XPPropClassValueset]) > 0) {

      nFuncReturn = ERR_XPVALUESET_CN00;
      nToken = 1;
      while (cn90GetToken(pParameter->szProperties[XPPropClassValueset], szToken, nToken, XP_STRING_LENGTH_MXCN002)) {
        if (pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Integer ||
            pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Real       ) {
          SAPDB_strcpy(szBuffer, szToken);
        } else {
          cn90AnalyzeString(szToken, szBuffer, XP_STRING_LENGTH_MXCN002);
        } // end if
        if (pParameter->xpProperties[XPPropClassCasesensitive] == DBMSrvPar_Parameter::XPPropValueYes) {
          if (strcmp(szBuffer, szValue) == 0) {
            nFuncReturn = OK_CN00;
          } // end if
        } else {
          if (stricmp(szBuffer, szValue) == 0) {
            nFuncReturn = OK_CN00;
          } // end if
        } // end if

        nToken++;
      } // end while
    } // end if
  } // end if

  // check maximum
  if (nFuncReturn == OK_CN00) {
    if (strlen(pParameter->szProperties[XPPropClassMax]) > 0) {
      switch ( pParameter->xpRecord.xpType ) {
        case DBMSrvPar_RTEConfParameter::t_Integer:
          nFuncReturn = atol(szValue) > atol(pParameter->szProperties[XPPropClassMax]) ? ERR_XPMAX_CN00 : OK_CN00;
          break;
        case DBMSrvPar_RTEConfParameter::t_Real:
          nFuncReturn = atof(szValue) > atof(pParameter->szProperties[XPPropClassMax]) ? ERR_XPMAX_CN00 : OK_CN00;
          break;
        case DBMSrvPar_RTEConfParameter::t_String:
          cn90AnalyzeString(pParameter->szProperties[XPPropClassMax], szBuffer, XP_STRING_LENGTH_MXCN002);
          if (pParameter->xpProperties[XPPropClassCasesensitive] == DBMSrvPar_Parameter::XPPropValueYes) {
            nFuncReturn = strcmp(szValue, szBuffer) > 0 ? ERR_XPMAX_CN00 : OK_CN00;
          } else {
            nFuncReturn = stricmp(szValue, szBuffer) > 0 ? ERR_XPMAX_CN00 : OK_CN00;
          } // end if
          break;
      } /* end switch */
    } // end if
  } // end if

  // check minimum
  if (nFuncReturn == OK_CN00) {
    if (strlen(pParameter->szProperties[XPPropClassMin]) > 0) {
      switch ( pParameter->xpRecord.xpType ) {
        case DBMSrvPar_RTEConfParameter::t_Integer:
          nFuncReturn = atol(szValue) < atol(pParameter->szProperties[XPPropClassMin]) ? ERR_XPMIN_CN00 : OK_CN00;
          break;
        case DBMSrvPar_RTEConfParameter::t_Real:
          nFuncReturn = atof(szValue) < atof(pParameter->szProperties[XPPropClassMin]) ? ERR_XPMIN_CN00 : OK_CN00;
          break;
        case DBMSrvPar_RTEConfParameter::t_String:
          cn90AnalyzeString(pParameter->szProperties[XPPropClassMin], szBuffer, XP_STRING_LENGTH_MXCN002);
          if (pParameter->xpProperties[XPPropClassCasesensitive] == DBMSrvPar_Parameter::XPPropValueYes) {
            nFuncReturn = strcmp(szValue, szBuffer) < 0 ? ERR_XPMIN_CN00 : OK_CN00;
          } else {
            nFuncReturn = stricmp(szValue, szBuffer) < 0 ? ERR_XPMIN_CN00 : OK_CN00;
          } // end if
          break;
      } /* end switch */
    } // end if
  } // end if

  // check range of datatype
  if (nFuncReturn == OK_CN00) {
    switch ( pParameter->xpRecord.xpType ) {
      case DBMSrvPar_RTEConfParameter::t_Integer:
        break;
      case DBMSrvPar_RTEConfParameter::t_Real:
        break;
      case DBMSrvPar_RTEConfParameter::t_String:
        nFuncReturn = (RTECONF_MAXSTRINGLENGTH < strlen(szValue)) ? ERR_XPOVERFLOW_CN00 : nFuncReturn;
        break;
    } /* end switch */
  } // end if

  // check 7-Bit ASCII
  if (nFuncReturn == OK_CN00) {
    switch ( pParameter->xpRecord.xpType ) {
      case DBMSrvPar_RTEConfParameter::t_String:
      case DBMSrvPar_RTEConfParameter::t_CryptInfo:
        for (nIndex = 0; szValue[nIndex] != 0; ++nIndex) {
          nFuncReturn = (((unsigned char) szValue[nIndex]) < 128) ?  nFuncReturn : ERR_ASCII_CN00;
        } // end if

        break;
    } /* end switch */
  } // end if

  return nFuncReturn;
} /* cn29CheckRange */

/* IMPLEMENTATION INTERN FUNCTION cn29SoftPutIntern
 * ------------------------------------------------------------------
 * purpose: save the new value internal param session structure
 *          - no check
 *          - no persitent write to param file
 */
tcn00_Error cn29SoftPutIntern
      ( DBMSrvPar_Parameters           * pXParamData,
        const char                     * szParameter,
        const char                     * szValueIn,
        const char                     * szComment,
        bool                             bIgnoreReadOnly,
        DBMSrvPar_Parameter::XPWriteMode oWriteMode)
{
  tcn00_Error        nFuncReturn   = OK_CN00;
  DBMSrvPar_Parameter * pParameter    = NULL;
  DBMSrvPar_Parameter * pParamChanged = NULL;
  DBMSrvPar_Parameter * pParamVerified= NULL;
  DBMSrvPar_Parameter * pDynHelp      = NULL;
  tcn002_XpValueString szValue;

  oWriteMode = (oWriteMode == DBMSrvPar_Parameter::XPWriteModeUnknown)?DBMSrvPar_Parameter::XPWriteModePermanent:oWriteMode; 

  if (strlen(szParameter) > 0) {

    /* search parameter in List */
    pParameter = pXParamData->Get(szParameter);

    if (pParameter == NULL) {
      pDynHelp = cn29SearchDynamic(pXParamData->pFirst, szParameter);

      /* is there an dynamic match then create new instance */
      if (pDynHelp != NULL) {
        pParameter = new DBMSrvPar_Parameter;
        if (pParameter == NULL) {
          nFuncReturn = ERR_MEM_CN00;
        } else {
          /* copy data from dynamic record */
          nFuncReturn = cn29CopyParameter(pParameter, pDynHelp);
          /* clear the dynamic/intern property */
          pParameter->xpProperties[XPPropClassDynamic] = DBMSrvPar_Parameter::XPPropValueNo;
          pParameter->xpProperties[XPPropClassIntern] = DBMSrvPar_Parameter::XPPropValueNo;
          /* set the real name */
          cn90SaveStrcpy(pParameter->szParameterName, szParameter, sizeof(tcn002_XpValueName));

          /* list management */
          pParameter->pNext = pDynHelp->pNext;
          pDynHelp->pNext   = pParameter;
          pXParamData->Put(pParameter);
        } /* end if */
      } /* end if */
    } /* end if */

    /* parameter found? */
    if (pParameter != NULL) {

      if (pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Integer ||
          pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Real      ) {
        SAPDB_strcpy(szValue, szValueIn);
      } else {
        if (!cn90AnalyzeString(szValueIn, szValue, XP_STRING_LENGTH_MXCN002)) {
          SAPDB_strcpy(szValue, szValueIn);
        } // end if
      } // end if

      /* check if write allowed */
      if (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueNo && !bIgnoreReadOnly) {
        /* never writes read-only */
        nFuncReturn = ERR_XPREADONLY_CN00;
      } else if ((pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueOffline) && (oWriteMode & DBMSrvPar_Parameter::XPWriteModeOnline)) {
        /* do write online if only offline allowed */
        nFuncReturn = ERR_XPREADONLY_CN00;
      } // end if
      if (nFuncReturn == OK_CN00) {
        if ((oWriteMode & DBMSrvPar_Parameter::XPWriteModeOnline) && (DBMSrvKnl_State::State(cn00DBMServerData::vcontrol()->dbname) == DBMSrvKnl_State::StateOffline) && !bIgnoreReadOnly) {
          nFuncReturn = ERR_DBNORUN_CN00;
        } // end if
      } // end if
      if (nFuncReturn == OK_CN00) {
        pParamVerified = pXParamData->Get(PAN_PARAM_VERSION_TIME);
        if (pParamVerified != NULL) {
          if ((pParamVerified->bParamExist)                                &&
            (pParameter->xpProperties[XPPropClassModify] == DBMSrvPar_Parameter::XPPropValueNo) &&
            !bIgnoreReadOnly) {
            nFuncReturn = ERR_XPREADONLY_CN00;
          } // end if
        } // end if
      } // end if

      if (nFuncReturn == OK_CN00) {
        pParamChanged = pXParamData->Get(PAN_XPARAM_CHANGED);
        if (pParamChanged != NULL) {
          if (pParamChanged->bParamExist) {
            if (pParameter->xpProperties[XPPropClassDevspace] == DBMSrvPar_Parameter::XPPropValueYes) {
//              /* don't change devspace parameters if kernel parameters dirty */
//              if ( (atol(pParamChanged->szInput) != 0) || pXParamData->bKernelDirty) {
//                nFuncReturn = ERR_XPDIRTY_CN00;
//              } /* end if */
            } else if (pParameter->xpProperties[XPPropClassDevspace] == DBMSrvPar_Parameter::XPPropValueNo) {
              /* don't change kernel parameters if devspace parameters dirty */
              if (pXParamData->bDevspaceDirty) {
                nFuncReturn = ERR_XPDIRTY_CN00;
              } /* end if */
            } /* end if */
          } /* end if */
        } /* end if */
      } /* end if */

      /* check the value */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn29CheckRange(pParameter, szValue);
      } /* end if */

      /* "write" the parameter */
      if (nFuncReturn == OK_CN00) {
        cn90StrUpperCopy (pParameter->szNewValue, szValue,
                          (bool   ) (pParameter->xpProperties[XPPropClassCasesensitive] == DBMSrvPar_Parameter::XPPropValueYes));
        pParameter->oWriteMode = oWriteMode;
        pParameter->szComment.rawAssign(szComment);
        if (pParameter->xpProperties[XPPropClassDevspace] == DBMSrvPar_Parameter::XPPropValueYes) {
          pXParamData->bDevspaceDirty = true;
        } else {
          pXParamData->bKernelDirty = true;
        } /* end if */
      } /* end if */
    } else {
      nFuncReturn = ERR_XPNOTFOUND_CN00;
    } /* end if */

  } else {
    nFuncReturn = ERR_PARAM_CN00;
  } /* end if */

  return nFuncReturn;
} /* end cn29SoftPutIntern */

/* IMPLEMENTATION INTERN FUNCTION cn29ReadParamFileContent
 * ------------------------------------------------------------------
 * purpose: read and analyze the contents of XParam help file
 *
 */
static tcn00_Error cn29ReadParamFileContent (
      tsp00_DbNamec       szDbName,
      DBMSrvPar_Parameters * pXParamData,
      char             * replyData,
      int              * replyLen,
      bool               bReadOnly)
{
  tcn00_Error           nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter     aParameter;
  DBMSrvPar_Parameter   * pParameter  = NULL;
  DBMSrvPar_Parameter   * pDynHelp    = NULL;

  DBMSrvPar_Parameter::XPRecord            xpRecord;
  DBMSrvMsg_Error                          oMsg;
  DBMSrvPar_RTEConfParameter::Name    sName;
  SAPDB_Bool                 bEnd       = false;
  SAPDB_Bool                 bOK        = true;
  SAPDB_Bool                 bVersion   = true;

  DBMSrvPar_RTEConfParameter          oConfigVersion (szDbName.asCharp(),
                                             1,
                                             RTECONF_MAXNAMELENGTH,
                                             RTECONF_MAXSTRINGLENGTH);
  DBMSrvPar_RTEConfParameter          oConfig        (szDbName.asCharp(),
                                             0,
                                             RTECONF_MAXNAMELENGTH,
                                             RTECONF_MAXSTRINGLENGTH);

  /* open Param file of db */
  if  ( oConfig.Read(oMsg)) {
    if  (!oConfigVersion.Read(oMsg)) {
      bVersion = false;
      oMsg.ClearMessageList();
    } // end if

    RTEConf_ParameterIterator   oIterator = oConfig.GetIterator();

    do {

      bOK = oIterator.Next(sName, xpRecord.xpType, bEnd, oMsg);

      if (bOK) {
        switch (xpRecord.xpType) {
          case DBMSrvPar_RTEConfParameter::t_Integer:
            bOK = oConfig.GetValue (sName, xpRecord.xpValue.xpValueInt, oMsg);
            break;
          case DBMSrvPar_RTEConfParameter::t_Real:
            bOK = oConfig.GetValue (sName, xpRecord.xpValue.xpValueReal, oMsg);
            break;
          case DBMSrvPar_RTEConfParameter::t_String:
            bOK = oConfig.GetValue (sName, (DBMSrvPar_RTEConfParameter::String) xpRecord.xpValue.xpValueString.asCharp(), oMsg);
            break;
          case DBMSrvPar_RTEConfParameter::t_CryptInfo:
            bOK = oConfig.GetValue (sName, xpRecord.xpValue.xpValueCrypt, oMsg);
            break;
        } // end switch
      } // end if

      if (bOK) {

        /* search parameter in List */
        pParameter = pXParamData->Get((char *) sName);

        /* No match -> check for dynamic */
        if (pParameter == NULL) {

          pDynHelp = cn29SearchDynamic(pXParamData->pFirst, (char *)  sName);
          if (pDynHelp != NULL) {

            pParameter = new DBMSrvPar_Parameter;
            if (pParameter == NULL) {
              nFuncReturn = ERR_MEM_CN00;
              cn90AnswerIError(replyData, replyLen, nFuncReturn);
            } else {
              /* copy data from dynamic record */
              nFuncReturn = cn29CopyParameter(pParameter, pDynHelp);
              /* clear the dynamic/intern property */
              pParameter->xpProperties[XPPropClassDynamic] = DBMSrvPar_Parameter::XPPropValueNo;
              pParameter->xpProperties[XPPropClassIntern] = DBMSrvPar_Parameter::XPPropValueNo;

              /* list management (insert after dynamic) */
              pParameter->pNext = pDynHelp->pNext;
              pDynHelp->pNext   = pParameter;
              SAPDB_strcpy(pParameter->szParameterName, (char *) sName);
              pXParamData->Put(pParameter);

            } // end if

          } // end if

        } // end if

        if (pParameter != NULL) {
          /* Copy Data */
          SAPDB_memcpy(&pParameter->xpRecord, &xpRecord, sizeof(xpRecord));
          SAPDB_strcpy(pParameter->szParameterName, (char *) sName);

          cn29ParamValueToString(pParameter->szNewValue, &pParameter->xpRecord);
          SAPDB_strcpy(pParameter->szOldValue, pParameter->szNewValue);
          SAPDB_strcpy(pParameter->szActiveValue, pParameter->szNewValue);

          /* set flags */
          pParameter->bParamExist = true;
          pXParamData->bParamFileExist = true;
          /* an version value */
          if (bVersion) {
            switch (xpRecord.xpType) {
              case DBMSrvPar_RTEConfParameter::t_Integer:
                bOK = oConfigVersion.GetValue (sName, xpRecord.xpValue.xpValueInt, oMsg);
                break;
              case DBMSrvPar_RTEConfParameter::t_Real:
                bOK = oConfigVersion.GetValue (sName, xpRecord.xpValue.xpValueReal, oMsg);
                break;
              case DBMSrvPar_RTEConfParameter::t_String:
                bOK = oConfigVersion.GetValue (sName, (DBMSrvPar_RTEConfParameter::String) xpRecord.xpValue.xpValueString.asCharp(), oMsg);
                break;
              case DBMSrvPar_RTEConfParameter::t_CryptInfo:
                bOK = oConfigVersion.GetValue (sName, xpRecord.xpValue.xpValueCrypt, oMsg);
                break;
            } // end switch
            if (bOK) {
              cn29ParamValueToString(pParameter->szGoodValue, &xpRecord);
            } else {
              oMsg.ClearMessageList();
            } // end if
          } // end if

        } else {
          // do not delete the systemdevspace parameter!!!
          if ((strcmp((char *) sName, "SYSDEV_001")        != 0) &&
              (strcmp((char *) sName, PAN_MAXSYSDEVSPACES) != 0) &&
              !bReadOnly) {
            // parameter not known in cserv.pcf -> delete it
            SAPDB_strcpy(aParameter.szParameterName, (char *)  sName);
            SAPDB_memcpy(&aParameter.xpRecord, &xpRecord, sizeof(xpRecord));
            cn29ParamValueToString(aParameter.szNewValue, &aParameter.xpRecord);
            SAPDB_strcpy(aParameter.szOldValue, aParameter.szNewValue);
            oConfig.Delete(sName, oMsg);
            oConfig.Write(oMsg);
            cn29WriteParamHistory(szDbName, &aParameter, PAHI_DELETE_CN002);
          } // end if
        } /* end if */
      } /* end if */

    }  while ( (oMsg.IsEmpty()) && (nFuncReturn == OK_CN00) && !bEnd);

    if  ( oMsg.IsEmpty() ) {

      // simulate param_changed and param_verified
      DBMSrvPar_Parameter            * pParameter = NULL;
      DBMSrvPar_RTEConfParameter::TimeStamp   aTime;
      SAPDB_Bool                     bChanged   = !oConfig.GetLastVerifyTime(aTime);
      SAPDB_Bool                     bCrashed   = oConfig.GetOriginalCrashFlag();

      pParameter = pXParamData->Get(PAN_XPARAM_CHANGED);
      if ((pParameter != NULL) && (aTime.Year > 0)) {
        pParameter->bParamExist = true;
        pParameter->xpRecord.xpValue.xpValueInt = bChanged ? 1 : 0;
        cn29ParamValueToString(pParameter->szNewValue, &pParameter->xpRecord);
        SAPDB_strcpy(pParameter->szOldValue, pParameter->szNewValue);
      } // end if

      pParameter = pXParamData->Get(PAN_PARAM_VERSION_TIME);
      if (pParameter != NULL && (aTime.Year > 0)) {
        pParameter->bParamExist = true;
        sprintf((char *) pParameter->xpRecord.xpValue.xpValueString,
                "%04d-%02d-%02d %02d:%02d:%02d",
                (int) aTime.Year,
                (int) aTime.Month,
                (int) aTime.Day,
                (int) aTime.Hour,
                (int) aTime.Minute,
                (int) aTime.Second);
        cn29ParamValueToString(pParameter->szNewValue, &pParameter->xpRecord);
        SAPDB_strcpy(pParameter->szOldValue, pParameter->szNewValue);
      } // end if

      pParameter = pXParamData->Get(PAN_CRASH_SEM);
      if (pParameter != NULL) {
        pParameter->bParamExist = true;
        pParameter->xpRecord.xpValue.xpValueInt = bCrashed ? 1 : 0;
        cn29ParamValueToString(pParameter->szNewValue, &pParameter->xpRecord);
        SAPDB_strcpy(pParameter->szOldValue, pParameter->szNewValue);
      } // end if


    } // end if

  } else {
	if (oMsg == DBMSrvMsg_Error::DBMSrv_FILEOPEN) {
		oMsg.ClearMessageList();
	} // end if
  } // end if

  if  ( !oMsg.IsEmpty() ) {
    nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
  } // end if

  return nFuncReturn;
} /* end cn29ReadParamFileContent */

/* IMPLEMENTATION INTERN FUNCTION cn29ReadParamsFromDB
 * ------------------------------------------------------------------
 * purpose: read the active configuration from the database
 *
 */
static tcn00_Error cn29ReadParamsFromDB (
      tsp00_DbNamec          szDbName,
      DBMSrvPar_Parameters * pXParamData,
      char                 * replyData,
      int                  * replyLen,
      int                    replyLenMax)
{
  DBMSrvKnl_Session::ResultType oResultType = DBMSrvKnl_Session::resultUnknown;
  DBMSrvKnl_Session             oSession;
  cn50DBMUser                   usrDBM    (szDbName, cn50DBMUser::getColdUser(szDbName) );
  DBMSrvMsg_Error               oError;
  DBMSrvKnl_State :: StateValue state=DBMSrvKnl_State::State(szDbName);

  if (state == DBMSrvKnl_State::StateOnline || state == DBMSrvKnl_State::StateStandby) {
    // get utility session
    if (oSession.NormalConnect(szDbName.asCharp(), usrDBM.getUserName().asCharp(), usrDBM.getClearMasterPwd().asCharp(), oError)) {
      if (oSession.Execute("select * from sysdd.activeconfiguration", oResultType, oError) &&
          (oResultType == DBMSrvKnl_Session::resultSQL)) {
        DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
        bool bEnd = false;

        DBMSrvPar_Parameter   * pParameter = NULL;
        Tools_DynamicUTF8String szName;
        int                     nName = oRecord.fieldNumber("PARAMETERNAME");
        Tools_DynamicUTF8String szValue;
        int                     nValue = oRecord.fieldNumber("VALUE");

        while (!bEnd && oError.IsEmpty()) {
          pParameter = pXParamData->Get(oRecord.fieldAsChar(nName).CharPtr());
          if (pParameter != NULL) {
            pParameter->szActiveValue.rawAssign(oRecord.fieldAsChar(nValue).CharPtr() );
          } // end if
          oRecord.getNextRecord(bEnd, oError);
        } // end while
      } // end if
    } // end if

    oSession.Release(oError);

  } // end if
 
  return OK_CN00;
} // end cn29ReadParamsFromDB

tcn00_Error cn29ParamStartSessionDBIndependent
      ( DBMSrvPar_Parameters * & pXParamData,
        const char           *   szDBName,
        char                 *   replyData,
        int                  *   replyLen,
        int                      replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp00_Pathc         szNewFile;
  tsp01_RteError      RteError;

  if (!sqlGetDbrootEnvPath ( szNewFile, TERM_WITH_DELIMITER_EO01, &RteError)) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(RteError));
  } else {
    strcat(szNewFile, XP_NEWFILE_CN002);
  } // end if

  if (nFuncReturn == OK_CN00) {

    /* free existing XParam section */
    if (pXParamData != NULL) {
      delete pXParamData;
      pXParamData = NULL;
    } /* end if */

    /* allocate new XParam section */
    if (nFuncReturn == OK_CN00) {
      pXParamData = new DBMSrvPar_Parameters(szDBName?szDBName:"");
      if (pXParamData == NULL) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
      } /* end if */
    } /* end if */

    /* read the help file */
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn21ReadHelpFile(pXParamData, replyData, replyLen, szNewFile, true);
    } /* end if */
  }

  return nFuncReturn;
}

/* IMPLEMENTATION INTERN FUNCTION cn29ParamStartSession
 *---------------------------------------------------------------------------*
 * purpose: initialize a XParam-Session
 *
 */
tcn00_Error cn29ParamStartSession
      ( tsp00_DbNamec         szDbName,
        DBMSrvPar_Parameters * & pXParamData,
        char              *   replyData,
        int               *   replyLen,
        int                   replyLenMax,
        bool                  bReadOnly)
{
  FUNCTION_DBG_MCN00_1(_T("vcn29::cn29ParamStartSession"));

  tcn00_Error         nFuncReturn = OK_CN00;

  nFuncReturn=cn29ParamStartSessionDBIndependent(
                pXParamData,
                szDbName.asCharp(),
                replyData,
                replyLen,
                replyLenMax);

  if (nFuncReturn == OK_CN00) {

    // check the paramfile version
    if (nFuncReturn == OK_CN00) {
      DBMSrvPar_Parameter * pParameter = NULL;
      tsp00_Versionc      VersionString;

      pParameter = pXParamData->Get(PAN_PARAM_KERNELVERSION);
      if (pParameter != NULL) {
        if (strlen(pParameter->szDefValue) > 0) {
          sp100_GetVersionString(COMP_NAME_DBMSERVER_SP100, s100buildnumber, VersionString);
          if (strncmp(&VersionString[BEG_OF_1ST_VERS_SUBSTR_SP100],
                      &pParameter->szDefValue[BEG_OF_1ST_VERS_SUBSTR_SP100],
                      END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100) != 0) {

            DBMSrvMsg_Error aEvent(SDBMSG_DBMSRV_PARAMVER,
                                   Msg_Arg(SDBMSGTAG_DBMSRV_PARAMVER__DBMSRV_COMP1,VersionString.asCharp()),
	                                 Msg_Arg(SDBMSGTAG_DBMSRV_PARAMVER__DBMSRV_COMP2,pParameter->szDefValue.asCharp()));
            nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
          } // end if
        } // end if
      } // end if
    } // end if

    /* check for paramfile migration */
    if (nFuncReturn == OK_CN00 && !bReadOnly) {
      nFuncReturn = cn28MigrateParamFile(szDbName, pXParamData, replyData, replyLen, replyLenMax);
    } /* end if */

    /* reading the param file  !!! after the help file */
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29ReadParamFileContent(szDbName, pXParamData, replyData, replyLen, bReadOnly);
    } /* end if */

    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29ReadParamsFromDB(szDbName, pXParamData, replyData, replyLen, replyLenMax);
    } /* end if */

    if (nFuncReturn == OK_CN00) {
      DBMSrvPar_Parameter * pParameter = NULL;

      /* initialize the checking  */
      cn23InitializeChecking(pXParamData->pFirst);

      pParameter = pXParamData->Get(PAN_INSTANCE_TYPE);
      if ( pParameter != NULL ) {
        if ( strlen(pParameter->szNewValue) > 0 ) {
          nFuncReturn = cn29CheckInstance(pParameter->szNewValue, pXParamData);
        } // end if
      } // end if
    } // end if

    if (nFuncReturn == OK_CN00) {
#ifdef XPCHECKDEP
      /* print dependencies to stdout */
      cn23PrintDependencies (pXParamData->pFirst);
#endif
      /* generate Answer */
      cn90AnswerOK(replyData, replyLen, NULL);

      pXParamData->bKernelDirty = false;
      pXParamData->bDevspaceDirty = false;

    } else {
      /* free the XParam section */
      if (pXParamData != NULL) {
        delete pXParamData;
        pXParamData = NULL;
      } /* end if */
    } /* end if */
  } /* end if */

  return nFuncReturn;

} /* end cn29ParamStartSession */

/* IMPLEMENTATION PRIVATE FUNCTION cn29_InvalidForInstance
 *---------------------------------------------------------------------------*
 */
bool cn29_InvalidForInstance
      ( const _TCHAR  *szInstance,
        const _TCHAR  *szPossible )
{
  bool    bInvalid = false;
  _TCHAR  szToken[PARAMETER_MAXLEN_CN90];
  int     nToken = 1;

  if ((_tcslen(szInstance) > 0) && (_tcslen(szPossible) > 0)) {
    bInvalid = true;
    while (bInvalid && cn90GetToken(szPossible, szToken, nToken, PARAMETER_MAXLEN_CN90) ) {
      cn90StripQuotes(szToken);
      if (_tcsicmp(szToken, szInstance) == 0) {
        bInvalid = false;
      } // end if
      ++nToken;
    } // end while
  } // end if

  return bInvalid;
} // end cn29_InvalidForInstance

/* IMPLEMENTATION INTERN FUNCTION cn29CheckInstance
 *---------------------------------------------------------------------------*
 */
tcn00_Error cn29CheckInstance
      ( const _TCHAR      * szInstance,
        DBMSrvPar_Parameters * pXParamData)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter * pParameter  = NULL;
  DBMSrvPar_Parameter * pNext       = NULL;

  if (pXParamData != NULL) {

    // maintain the list
    pParameter = pXParamData->pFirst;
    while (pParameter != NULL) {

      pNext = pParameter->pNext;

      if (cn29_InvalidForInstance(szInstance, pParameter->szProperties[XPPropClassInstance])) {
        pParameter->xpProperties[XPPropClassGroup] = DBMSrvPar_Parameter::XPPropValueNo;
      } // end if

      // set defaults
      if (strlen(pParameter->szDefaultFor[0][XP_DEFAULTFORINSTANCE_CN002]) > 0 ) {
        int nIndex = 0;

        for (nIndex = 0; nIndex < XP_DEFAULTFOR_MXCN002; ++nIndex) {
          if (_tcsicmp(szInstance, pParameter->szDefaultFor[nIndex][XP_DEFAULTFORINSTANCE_CN002]) == 0) {
            if (!pParameter->bParamExist) {
              SAPDB_strcpy(pParameter->szNewValue,   pParameter->szDefaultFor[nIndex][XP_DEFAULTFORVALUE_CN002]);
            } // end if
            SAPDB_strcpy(pParameter->szDefValue, pParameter->szDefaultFor[nIndex][XP_DEFAULTFORVALUE_CN002]);
            break;
          } // end if
        } // end for
      } // end if

      pParameter = pNext;
    } // end while

  } // end if

  return nFuncReturn;
} // end cn29CheckInstance

/* IMPLEMENTATION INTERN FUNCTION cn29AnswerParamNotFound
 *---------------------------------------------------------------------------*
 */
tcn00_Error cn29AnswerParamNotFound
      ( _TCHAR            * replyData,
        int               * replyLen,
        const _TCHAR      * szParameter,
        DBMSrvPar_Parameter * pParameter,
        bool                bDirect)
{
  tcn00_Error         nFuncReturn = ERR_XPNOTFOUND_CN00;
  _TCHAR            * pData;
  int                 nHits = 0;

  cn90AnswerIError(replyData, replyLen, nFuncReturn);

  if (_tcslen(szParameter) > 2) {

    pData = replyData + _tcslen(replyData);

    while (pParameter != NULL) {

      if ((strstr(pParameter->szParameterName, szParameter) != NULL) &&
          (!bDirect || (bDirect && (pParameter->bParamExist)))) {
        if (nHits == 0) {
          _stprintf(pData, "\n");
          pData = pData + _tcslen(pData);
        } // end if

        ++nHits;

        _stprintf(pData, "%-20s", pParameter->szParameterName.asCharp());
        pData = pData + _tcslen(pData);

        if ((nHits % 3) == 0) {
          _stprintf(pData, "\n");
          pData = pData + _tcslen(pData);
        } // end if

      } // end if

      pParameter = pParameter->pNext;
    } // end while

    if ((nHits % 3) != 0) {
      _stprintf(pData, "\n");
      pData = pData + _tcslen(pData);
    } // end if

  } // end if

  *replyLen = (int)_tcslen(replyData);

  return nFuncReturn;
} // end cn29AnswerParamNotFound

/* IMPLEMENTATION INTERN FUNCTION cn29WriteParamHistory
 *---------------------------------------------------------------------------*
 */
void cn29WriteParamHistory
      ( tsp00_DbNamec       szDBName,
        DBMSrvPar_Parameter * pParameter,
        char                cOperation)
{
  if ( ( (strcmp(pParameter->szNewValue, pParameter->szOldValue)!=0 ) ||
         (strlen(pParameter->szComment)                         !=0 ) ||
         (cOperation == PAHI_DELETE_CN002                           )    ) &&
       (strcmp(pParameter->szParameterName, PAN_XPARAM_CHANGED    )  != 0 ) &&
       (strcmp(pParameter->szParameterName, PAN_PARAM_VERSION_TIME)  != 0 ) &&
       (pParameter->xpRecord.xpType != DBMSrvPar_RTEConfParameter::t_CryptInfo     )    ) {

    tcn002_XpValueName   szName;
    tcn002_XpValueString szNewValue;
    tcn002_XpValueString szOldValue;
    DBMSrvMsg_Error      oError;
    tsp00_C8c            szGroup;


    szName.rawAssign(pParameter->szParameterName);
    szNewValue.rawAssign(pParameter->szNewValue);
    szOldValue.rawAssign(pParameter->szOldValue);

    struct {
      DBMSrvPar_Parameter::XPPropValue   nValue;
      const char         * szText;
    }                     aValueTable[] = XP_PROP_VALUES_CN002;
    int                   j                 = 0;
    /* search value */
    while ( (aValueTable[j].nValue != DBMSrvPar_Parameter::XPPropValueUnknown) && (aValueTable[j].nValue != pParameter->xpProperties[XPPropClassGroup]) ) {
      j++;
    } /* end while */
    if (aValueTable[j].szText != NULL) {
      szGroup.rawAssign(aValueTable[j].szText);
    } else {
      szGroup.Init();
    } // end if

    cn52ParamHistoryRecord aRecord(szDBName,
                                   szName,
                                   szNewValue,
                                   szOldValue,
                                   szGroup,
                                   pParameter->szComment,
                                   (cOperation == PAHI_DELETE_CN002)? cn52ParamHistoryRecord::StDeleted : cn52ParamHistoryRecord::StChanged);

    aRecord.appendToFile (oError);

  } // end if

} // end cn29WriteParamHistory
