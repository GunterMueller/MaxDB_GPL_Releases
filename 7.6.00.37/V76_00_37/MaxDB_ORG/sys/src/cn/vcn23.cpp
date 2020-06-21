/* @lastChanged: "1998-06-19  10:33"
 *
 * @filename:    vcn23c.cpp
 * @purpose:     "param calc"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * @Copyright (c) 1998-2005 SAP AG"
 *
 * ==================================================================
 *
 * responsible:   Bernd Vorsprach (D025588 - Bernd.Vorsprach@SAP-AG.de)
 *
 * created:       1998-02-10 by Bernd Vorsprach
 *
 * purpose:       calculation module for parameter management
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
#include <ctype.h>
#include <stdlib.h>

#include "hcn23.h"
#include "hcn29.h"
#include "hcn90.h"


/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn23_CalculateExpression
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_CalculateExpression (
      DBMSrvPar_Parameters * pXParamData,
      const char        * szLine,
      tcn002_XPOperand  * pOut,
      const char        * szCurrent,
      bool                bConstraint);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn23_DoOperation
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_DoOperation (
      tcn002_XPOperand *  pInOut,
      tcn002_XPOperand *  pIn,
      tcn002_XPOperators  nOperator );

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn23_GetParameterOperand
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_GetParameterOperand (
      DBMSrvPar_Parameter * pParameter,
      char              * pId,
      tcn002_XPOperand  * pOperand,
      const char        * szCurrent,
      bool                bConstraint);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn23_AssignValue 
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_AssignValue (
      DBMSrvPar_Parameter * pParameter,
      tcn002_XPOperand  * pOperand);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn23_PrintCalcResult
 * ------------------------------------------------------------------
 */
static void cn23_PrintCalcResult (
      DBMSrvPar_Parameter * pParameter,
      char             * pBuffer);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn23_PrintParamDep
 * ------------------------------------------------------------------
 */
static void cn23_PrintParamDep (
      DBMSrvPar_Parameter * pFirst,
      DBMSrvPar_Parameter * pCurrent,
      char             * szIndent,
      char             * szDep);

/* ------------------------------------------------------------------
 * START OF IMPLEMENTATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn23InitializeChecking
 * ------------------------------------------------------------------
 */
void cn23InitializeChecking (
      DBMSrvPar_Parameter * pParameter)
{

  while (pParameter != NULL) {
    pParameter->nCalcState = DBMSrvPar_Parameter::XPCalcStateUnknown;
    pParameter = pParameter->pNext;
  } /* end while */

} /* end cn23InitializeChecking */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn23PrintDependencies
 * ------------------------------------------------------------------
 */
void cn23PrintDependencies (
      DBMSrvPar_Parameter * pFirst)
{
  char               szIndent[XP_STRING_LENGTH_MXCN002 + 1];
  char               szDep[10000];
  DBMSrvPar_Parameter * pParameter = pFirst;

  while (pParameter != NULL) {
    SAPDB_strcpy(szIndent, "");
    SAPDB_strcpy(szDep, "");
    cn23_PrintParamDep(pFirst, pParameter, szIndent, szDep);
    printf("\n");
    pParameter = pParameter->pNext;
  } /* end while */

} /* end cn23PrintDependencies */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC cn23CheckConstraint
 * ------------------------------------------------------------------
 */
tcn00_Error cn23CheckConstraint (
      DBMSrvPar_Parameters * pXParamData,
      DBMSrvPar_Parameter * pParameter,
      char              * replyData,
      int               * replyLen)
{
  tcn00_Error       nFuncStatus  = OK_CN00;
  tcn002_XPError    nSyntaxError = XPOK_CN002;
  tcn002_XPOperand  opResult;

  if (pParameter->pConstraint != NULL) {
    nSyntaxError = cn23_CalculateExpression(pXParamData, 
                                            pParameter->pConstraint->Line(), 
                                            &opResult, 
                                            pParameter->szParameterName,
                                            true);

    /* evaluate result */
    if (nSyntaxError == XPOK_CN002) {
      /* check condition */
      if (opResult.nOpType == XPOpTypebool   ) {
        if (!opResult.opValue.bValue) {
          pParameter->nCalcState = DBMSrvPar_Parameter::XPCalcStateConstraint;      
        } /* end if */
      } else {
        nSyntaxError = XPERR_INVEXPT_CN002;
      } /* end if */
    } /* end if */
  } // end if

  if (nSyntaxError != XPOK_CN002) {
    nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_XPSYNTAX_CN00);
    /* add information about syntax error*/
    cn29SyntaxError(replyData, replyLen, nSyntaxError, pParameter->pConstraint->Line(), 
                    "???", pParameter->pConstraint->Number());
  } else {
    nFuncStatus = cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncStatus;
} /* end cn23CheckConstraint */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC cn23CalculateParameter
 * ------------------------------------------------------------------
 */
tcn00_Error cn23CalculateParameter (
      DBMSrvPar_Parameters * pXParamData,
      DBMSrvPar_Parameter * pParameter,
      char              * replyData,
      int               * replyLen)
{
  tcn00_Error       nFuncStatus  = OK_CN00;
  tcn002_XPError    nSyntaxError = XPOK_CN002;
  DBMSrvPar_XPLine* pLine        = pParameter->pCode;
  DBMSrvPar_XPLine::LineType   nLastLine    = DBMSrvPar_XPLine::XPLineUnknown;
  tcn002_XPOperand  opResult;
  int               nCompare     = 0;

  // loop the code section
  while ((pLine != NULL) && (nSyntaxError == XPOK_CN002)) {
    switch(pLine->Type()) {
      case DBMSrvPar_XPLine::XPLineCodeIF:
      case DBMSrvPar_XPLine::XPLineCodeELSIF:
        /* calculate expression and then next line for interpretation */
        nSyntaxError = cn23_CalculateExpression(pXParamData, pLine->Line(), &opResult, pParameter->szParameterName, false);
        if (nSyntaxError == XPOK_CN002) {
          if (opResult.nOpType == XPOpTypebool   ) {
            if (!opResult.opValue.bValue) {
              /* ignore next line */
              nLastLine = pLine->Type();
              pLine = pLine->Next();
            } /* end if */
          } else {
            nSyntaxError = XPERR_INVEXPT_CN002;
          } /* end if */
        } /* end if */
        break;
      case DBMSrvPar_XPLine::XPLineCodeELSE:
        /* do nothing */
        break;
      case DBMSrvPar_XPLine::XPLineCodeCALC:
        /* calculate value */
        nSyntaxError = cn23_CalculateExpression(pXParamData, pLine->Line(), &opResult, pParameter->szParameterName, false);

        /* evaluate result */
        if (nSyntaxError == XPOK_CN002) {
            /* assign value */
            nSyntaxError = cn23_AssignValue(pParameter, &opResult);
        } /* end if */

        /* calculate next line */
        if (nSyntaxError == XPOK_CN002) {
          if ((nLastLine == DBMSrvPar_XPLine::XPLineCodeIF) || (nLastLine == DBMSrvPar_XPLine::XPLineCodeELSIF)) {
            while (pLine->Next() != NULL) {
              if ((pLine->Next()->Type() == DBMSrvPar_XPLine::XPLineCodeELSE) || (pLine->Next()->Type() == DBMSrvPar_XPLine::XPLineCodeELSIF)) {
                /* ignore next two lines */
                nLastLine = pLine->Next()->Type();
                pLine = pLine->Next()->Next();
              } else {
                break;
              } /* end if */
            } /* end while */
          } /* end if */
        } /* end if */
        break;

      default:
        nSyntaxError = XPERR_INVSTMT_CN002;
        break;
    } /* end switch */

    if ((pLine != NULL) && (nSyntaxError == XPOK_CN002)) {
      nLastLine = pLine->Type();
      pLine = pLine->Next();
    } /* end if */
  } /* end while */

  nFuncStatus = (nSyntaxError == XPOK_CN002) ? nFuncStatus : ERR_XPSYNTAX_CN00;

  /* set checkstate */
  if (nFuncStatus == OK_CN00) {
    if ((strlen(pParameter->szNewValue) == 0) && (pParameter->xpProperties[XPPropClassMandatory] == DBMSrvPar_Parameter::XPPropValueYes)) {
      pParameter->nCalcState = DBMSrvPar_Parameter::XPCalcStateMandatory;
    } else {
      pParameter->nCalcState = DBMSrvPar_Parameter::XPCalcStateOK;
    } /* end if */
  } /* end if */

  if (nFuncStatus != OK_CN00) {
    /* internal error */
    cn90AnswerIError(replyData, replyLen, nFuncStatus);

    if (nFuncStatus == ERR_XPSYNTAX_CN00) {
      /* add information about syntax error*/
      cn29SyntaxError(replyData, replyLen, nSyntaxError, pLine->Line(), "???", pLine->Number());
    } /* end if */
  } /* end if */

  return nFuncStatus;
} /* end cn23CalculateParameter */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn23GetCalcState
 * ------------------------------------------------------------------
 */
tcn00_Error cn23GetCalcState (
      DBMSrvPar_Parameter             * pParameter,
      char                            * pBuffer,
      DBMSrvPar_Parameter::XPPropValue  nGroup)
{
  tcn00_Error nFuncStatus = OK_CN00;
  int         nTemp       = 0;

  switch (pParameter->nCalcState) {
    case DBMSrvPar_Parameter::XPCalcStateMandatory:
    case DBMSrvPar_Parameter::XPCalcStateConstraint:
    case DBMSrvPar_Parameter::XPCalcStateUnknown:
      nFuncStatus = ERR_XPCHECK_CN00;
      cn90AnswerIError(pBuffer, &nTemp, nFuncStatus);
      cn23_PrintCalcResult (pParameter, pBuffer + strlen(pBuffer));
      break;
  } /* end switch */

  return nFuncStatus;
} /* end cn23GetCalcState */

/* ------------------------------------------------------------------
 * START OF IMPLEMENTATION PRIVATE FUNCTIONS
 * ------------------------------------------------------------------
 */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn23_CalculateExpression
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_CalculateExpression (
      DBMSrvPar_Parameters * pXParamData,
      const char        * szLine,
      tcn002_XPOperand  * pOut,
      const char        * szCurrent,
      bool                bConstraint)
{
  tcn002_XPError          nSyntaxError  = XPOK_CN002;
  tcn002_XPOperand        opStack[XP_OPERANDS_MXCN002];
  long                   nStackPointer = -1;
  long                   nLineLength   = 0;
  tcn002_XPOperators      nOperator     = XPOperatorUnknown;
  long                   i             = 0;
  struct {
    tcn002_XPOperators    nOperator;
    const char        *  szOperator;
  }                      aTable[]      = XP_OPERATOR_TABLE_CN002;   
  char                   szToken[XP_STRING_LENGTH_MXCN002 + 1];
  long                   nToken        = 2;
  long                   bToken        = true;

  /* start at the second token */
  bToken = cn90GetToken(szLine, szToken, nToken++, XP_STRING_LENGTH_MXCN002 + 1);

  while (bToken && (nSyntaxError == XPOK_CN002)) {
    /* reset operator */
    nOperator    = XPOperatorUnknown;

    if (isdigit(szToken[0]) || (( szToken[0] == '-') && isdigit(szToken[1]))) {
      /* operand numeric constant */
      nStackPointer++;
      if (nStackPointer >= XP_OPERANDS_MXCN002) {
        nSyntaxError = XPERR_MANYOP_CN002;
      } else {
        if (!cn90AnalyzeNumber(szToken, &(opStack[nStackPointer].opValue.nValue), true)) {
          nSyntaxError = XPERR_INVCONST_CN002;
        } else {
         opStack[nStackPointer].nOpType = XPOpTypeArith;
        } /* end if */
      } /* end if */
    } else if (szToken [0] == XP_CHAR_STRING_CN002) {
      /* operand string constant */
      nStackPointer++;
      if (nStackPointer >= XP_OPERANDS_MXCN002) {
        nSyntaxError = XPERR_MANYOP_CN002;
      } else {
        if (!cn90AnalyzeString(szToken, opStack[nStackPointer].opValue.szValue, XP_STRING_LENGTH_MXCN002)) {
          nSyntaxError = XPERR_INVCONST_CN002;
        } else {
         opStack[nStackPointer].nOpType = XPOpTypeString;
        } /* end if */
      } /* end if */
    } else {
      /* check for operator */
      i = 0;
      while( (aTable[i].nOperator != XPOperatorUnknown) && (nOperator == XPOperatorUnknown) ) {
        if (strcmp(aTable[i].szOperator, szToken) == 0) {
          nOperator = aTable[i].nOperator;
        } /* end if */
        i++;
      } /* end while */
      if (nOperator == XPOperatorUnknown) {
        /* operand parameter */
        if (isalpha(szToken[0]) || (szToken[0] == XP_CHAR_UNDERLINE_CN002) ) {
          nStackPointer++;
          if (nStackPointer >= XP_OPERANDS_MXCN002) {
            nSyntaxError = XPERR_MANYOP_CN002;
          } else {
            nSyntaxError = cn23_GetParameterOperand(pXParamData->pFirst, szToken, &opStack[nStackPointer], szCurrent, bConstraint);         
          } /* end if */
        } else {
          nSyntaxError = XPERR_INVID_CN002;
        } /* end if */
      } /* end if */
    } /* end if */

    if ((nSyntaxError == XPOK_CN002) && (nOperator != XPOperatorUnknown)) {
      nStackPointer--;
      if (nStackPointer < 0) {
        nSyntaxError = XPERR_FEWOP_CN002;
      } else {
        nSyntaxError = cn23_DoOperation(&opStack[nStackPointer], &opStack[nStackPointer + 1], nOperator);
      } /* end if */
    } /* end if */

    /* next token */
    bToken = cn90GetToken(szLine, szToken, nToken++, XP_STRING_LENGTH_MXCN002 + 1);
  } /* end while */

  if (nSyntaxError == XPOK_CN002) {
    if (nStackPointer > 0) {
      nSyntaxError = XPERR_MANYOP_CN002;
    } else if (nStackPointer < 0) {
      nSyntaxError = XPERR_FEWOP_CN002;
    } else {
      SAPDB_memcpy(pOut, &opStack[0], sizeof(tcn002_XPOperand));
    } /* end if */
  } /* end if */

  return nSyntaxError;
} /* end cn23_CalculateExpression */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn23_DoOperation
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_DoOperation (
      tcn002_XPOperand *  pInOut,
      tcn002_XPOperand *  pIn,
      tcn002_XPOperators  nOperator )
{
  tcn002_XPError          nSyntaxError = XPOK_CN002;
  tcn002_XPOpType         xpOutType    = XPOpTypeUnknown;
  long                   i            = 0;
  long                   nTemp        = 0;
  struct {
    tcn002_XPOperators nOperator;
    tcn002_XPOpType    nInType1;
    tcn002_XPOpType    nInType2;
    tcn002_XPOpType    nOutType;
  }             aTable[]    = XP_OPERATOR_TO_TYPE_TABLE_CN002;
  
//  /* check types */
//  if (pInOut->nOpType != pIn->nOpType) {
//    nSyntaxError = XPERR_INVTYPE_CN002;
//  } /* end if */

  /* check operation */
  if (nSyntaxError == XPOK_CN002) {
    while( (aTable[i].nOperator != XPOperatorUnknown) && (xpOutType == XPOpTypeUnknown) ) {
      if ((aTable[i].nOperator == nOperator) && (aTable[i].nInType1 == pInOut->nOpType) &&
          (aTable[i].nInType2 == pIn->nOpType)) {
        xpOutType = aTable[i].nOutType;
      } /* end if */
      i++;
    } /* end while */
    if  (xpOutType == XPOpTypeUnknown) {
      nSyntaxError = XPERR_INVTYPE_CN002;
    } /* end if */
  } /* end if */

  /* do the operation */
  if (nSyntaxError == XPOK_CN002) {
    switch (nOperator) {
      case XPOperatorAND:
        pInOut->opValue.bValue = pInOut->opValue.bValue && pIn->opValue.bValue;
        break;
      case XPOperatorOR:
        pInOut->opValue.bValue = pInOut->opValue.bValue || pIn->opValue.bValue;
        break;
      case XPOperatorXOR:
        pInOut->opValue.bValue = (  pInOut->opValue.bValue && !pIn->opValue.bValue ) ||
                                 ( !pInOut->opValue.bValue &&  pIn->opValue.bValue );
        break;
      case XPOperatorPLUS:
        if (xpOutType == XPOpTypeString) {
          if (pInOut->nOpType == XPOpTypeArith) {
            nTemp = pInOut->opValue.nValue;
            sprintf ( pInOut->opValue.szValue , "%ld" , nTemp);
          } // end if
          if (pIn->nOpType == XPOpTypeArith) {
            nTemp = pIn->opValue.nValue;
            sprintf ( pIn->opValue.szValue , "%ld" , nTemp);
          } // end if
          if ((strlen(pInOut->opValue.szValue) + strlen(pIn->opValue.szValue)) < XP_STRING_LENGTH_MXCN002) {
            strcat(pInOut->opValue.szValue, pIn->opValue.szValue);
          } else {
            nSyntaxError = XPERR_OVERFL_CN002;
          } /* end if */ 
        } else {
          pInOut->opValue.nValue = pInOut->opValue.nValue + pIn->opValue.nValue;
        } /* end if */
        break;
      case XPOperatorMINUS:
        pInOut->opValue.nValue = pInOut->opValue.nValue - pIn->opValue.nValue;
        break;
      case XPOperatorMULTIPLY:
        pInOut->opValue.nValue = pInOut->opValue.nValue * pIn->opValue.nValue;
        break;
      case XPOperatorDIVIDE:
        if (pIn->opValue.nValue == 0) {
          nSyntaxError = XPERR_DIVBYZERO_CN002;
        } else {
          pInOut->opValue.nValue = pInOut->opValue.nValue / pIn->opValue.nValue;
        } // end if
        break;
      case XPOperatorMODULO:
        if (pIn->opValue.nValue == 0) {
          nSyntaxError = XPERR_DIVBYZERO_CN002;
        } else {
          pInOut->opValue.nValue = pInOut->opValue.nValue % pIn->opValue.nValue;
        } // end if
        break;
      case XPOperatorALIGN:
        if (pIn->opValue.nValue == 0) {
          nSyntaxError = XPERR_DIVBYZERO_CN002;
        } else {
          if ((pInOut->opValue.nValue % pIn->opValue.nValue) > 0) {
            pInOut->opValue.nValue = pInOut->opValue.nValue + pIn->opValue.nValue 
                                     - (pInOut->opValue.nValue % pIn->opValue.nValue);
          } // end if
        } // end if
        break;
      case XPOperatorPOWER:
        if (pIn->opValue.nValue < 0) {
          pInOut->opValue.nValue = 0;
        } else {
          nTemp = 1;
          for (i = 0; i < pIn->opValue.nValue; ++i) {
            nTemp = nTemp * pInOut->opValue.nValue;
          } // end for
          pInOut->opValue.nValue = nTemp;
        } // end if
        break;
      case XPOperatorEQUAL:
        if (pInOut->nOpType == XPOpTypeString) {
          pInOut->opValue.bValue = strcmp(pInOut->opValue.szValue, pIn->opValue.szValue) == 0;
        } else {
          pInOut->opValue.bValue = pInOut->opValue.nValue == pIn->opValue.nValue;
        } /* end if */
        break;
      case XPOperatorLESS:
        if (pInOut->nOpType == XPOpTypeString) {
          pInOut->opValue.bValue = strcmp(pInOut->opValue.szValue, pIn->opValue.szValue) < 0;
        } else {
          pInOut->opValue.bValue = pInOut->opValue.nValue < pIn->opValue.nValue;
        } /* end if */
        break;
      case XPOperatorGREATER:
        if (pInOut->nOpType == XPOpTypeString) {
          pInOut->opValue.bValue = strcmp(pInOut->opValue.szValue, pIn->opValue.szValue) > 0;
        } else {
          pInOut->opValue.bValue = pInOut->opValue.nValue > pIn->opValue.nValue;
        } /* end if */
        break;
      case XPOperatorLESSEQUAL:
        if (pInOut->nOpType == XPOpTypeString) {
          pInOut->opValue.bValue = strcmp(pInOut->opValue.szValue, pIn->opValue.szValue) <= 0;
        } else {
          pInOut->opValue.bValue = pInOut->opValue.nValue <= pIn->opValue.nValue;
        } /* end if */
        break;
      case XPOperatorGREATEREQUAL:
        if (pInOut->nOpType == XPOpTypeString) {
          pInOut->opValue.bValue = strcmp(pInOut->opValue.szValue, pIn->opValue.szValue) >= 0;
        } else {
          pInOut->opValue.bValue = pInOut->opValue.nValue >= pIn->opValue.nValue;
        } /* end if */
        break;
      case XPOperatorNOTEQUAL:
        if (pInOut->nOpType == XPOpTypeString) {
          pInOut->opValue.bValue = strcmp(pInOut->opValue.szValue, pIn->opValue.szValue) != 0;
        } else {
          pInOut->opValue.bValue = pInOut->opValue.nValue != pIn->opValue.nValue;
        } /* end if */
        break;
      default:
        nSyntaxError = XPERR_INVOP_CN002;
        break;
    } /* end switch */
  } /* end if */

  if (nSyntaxError == XPOK_CN002) {
    pInOut->nOpType = xpOutType;
  } /* end if */

  return nSyntaxError;
} /* end cn23_DoOperation */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn23_GetParameterOperand
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_GetParameterOperand (
      DBMSrvPar_Parameter * pParameter,
      char              * pId,
      tcn002_XPOperand  * pOperand,
      const char        * szCurrent,
      bool                bConstraint)
{
  tcn002_XPError         nSyntaxError = XPOK_CN002;
  bool                   bAfter       = false;
  bool                   bFound       = false;
  bool                   bWildcard    = false;
  tcn002_XPOperand       aTmpOp;
  char                 * pFirst       = NULL;
  char                 * pSecond      = NULL;
  const char           * pEndParam    = NULL;
  int                    nFirstLen    = 0;
  int                    nSecondLen   = 0;

  // init Operand
  pOperand->nOpType = XPOpTypeUnknown;
  pOperand->opValue.nValue = 0;
  
  // replace "dynamic" (?) parameter with the current
  if (strstr(pId, XP_KEY_DYNAMIC_CN002) != NULL) {
    char * pEndDyn = strstr(pId, XP_KEY_DYNAMIC_CN002);
    pEndDyn[0] = CHAR_STRINGTERM_CN90;
    pEndDyn++;
    pEndParam = &szCurrent[strlen(szCurrent) - strlen(pEndDyn)];
    if ( (strncmp(szCurrent, pId, strlen(pId)) == 0) &&
         (strncmp(pEndDyn, pEndParam , strlen(pEndDyn))  == 0)    ) {
      SAPDB_strcpy(pId, szCurrent);
    } /* end if */
  } // end if

  // check for wildcards
  if (strstr(pId, XP_KEY_WILDCARD_CN002) != NULL) {
    pSecond = strstr(pId, XP_KEY_WILDCARD_CN002);

    pFirst = pId;
    nFirstLen = (int) (pSecond - pFirst);

    pSecond++;
    nSecondLen = (int) strlen(pSecond);

    bWildcard = true;
  } // end if

  do {
    bFound = false;

    while (pParameter != NULL) {

      // check for match
      if (!bWildcard) {
        // without wildcards
        if (strcmp(pParameter->szParameterName, pId) == 0) {
          bFound = true;
          break;
        } // end if
      } else {
        // with wildcards
        pEndParam = &pParameter->szParameterName[strlen(pParameter->szParameterName) - nSecondLen];
        if ( (strncmp(pParameter->szParameterName, pFirst,  nFirstLen ) == 0) &&
             (strncmp(pEndParam,                   pSecond, nSecondLen) == 0)    ) {
          bFound = true;
          break;
        } // end if
      } // end if

      // check for correct order
      bAfter = ((strcmp(pParameter->szParameterName, szCurrent) == 0) && !bConstraint)  || bAfter;

      pParameter = pParameter->pNext;
    } /* end while */

    if (bFound && !bAfter) {
      switch (pParameter->xpRecord.xpType) {
        case DBMSrvPar_RTEConfParameter::t_Integer:
          pOperand->nOpType = (pOperand->nOpType == XPOpTypeUnknown) ? XPOpTypeArith : pOperand->nOpType;
          aTmpOp.nOpType    = XPOpTypeArith;
          aTmpOp.opValue.nValue = atol(pParameter->szNewValue);
          break;
        case DBMSrvPar_RTEConfParameter::t_String:
          pOperand->nOpType = (pOperand->nOpType == XPOpTypeUnknown) ? XPOpTypeString : pOperand->nOpType;
          aTmpOp.nOpType = XPOpTypeString;
          SAPDB_strcpy(aTmpOp.opValue.szValue, pParameter->szNewValue);
          break;
       case DBMSrvPar_RTEConfParameter::t_CryptInfo:
          nSyntaxError = XPERR_CRYPTUSE_CN002;
          break;
        default:
          nSyntaxError = XPERR_INVTYPE_CN002;
          break;
      } /* end switch */

      nSyntaxError = cn23_DoOperation(pOperand, &aTmpOp, XPOperatorPLUS);

      pParameter = pParameter->pNext;
    } else {
      if (!bFound && (pOperand->nOpType == XPOpTypeUnknown)) {
        nSyntaxError = XPERR_UNKNOWNID_CN002;
      } else if (bFound && bAfter) {
        nSyntaxError = XPERR_DECLAFTER_CN002;
      } // end if
    } /* end if */

  } while ((pParameter != NULL) && (bWildcard) && (nSyntaxError == XPOK_CN002));

  return nSyntaxError;
} /* end cn23_GetParameterOperand */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn23_AssignValue
 * ------------------------------------------------------------------
 */
static tcn002_XPError cn23_AssignValue (
      DBMSrvPar_Parameter * pParameter,
      tcn002_XPOperand  * pOperand)
{
  tcn002_XPError          nSyntaxError = XPOK_CN002;

  switch (pParameter->xpRecord.xpType) {
    case DBMSrvPar_RTEConfParameter::t_Integer:
      if (pOperand->nOpType == XPOpTypeArith) {
        sprintf(pParameter->szNewValue, "%ld", pOperand->opValue.nValue);
      } else {
        nSyntaxError = XPERR_INVEXPT_CN002;
      } /* end if */
      break;
    case DBMSrvPar_RTEConfParameter::t_String:
      if (pOperand->nOpType == XPOpTypeString) {
        SAPDB_strcpy(pParameter->szNewValue, pOperand->opValue.szValue);
      } else if (pOperand->nOpType == XPOpTypeArith) {
        sprintf(pParameter->szNewValue, "%ld", pOperand->opValue.nValue);
      } else {
        nSyntaxError = XPERR_INVEXPT_CN002;
      } /* end if */
      break;
    case DBMSrvPar_RTEConfParameter::t_CryptInfo:
      nSyntaxError = XPERR_CRYPTUSE_CN002;
      break;
    default:
      nSyntaxError = XPERR_INVTYPE_CN002;
      break;
  } /* end switch */

  return nSyntaxError;
} /* end cn23_AssignValue */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn23_PrintCalcResult
 * ------------------------------------------------------------------
 * purpose: get the calc result after calculation for one parameter
 */
static void cn23_PrintCalcResult (
      DBMSrvPar_Parameter * pParameter,
      char             * pBuffer)
{
  long               i           = 0;
  struct {
    DBMSrvPar_Parameter::CalcState    nValue;
    const char       * szText;
  }                  aTable[]    = XP_CALC_STATES_TABLE_CN002;

  if (pParameter != NULL) {

    /* get description of check state */
    while ((aTable[i].nValue != DBMSrvPar_Parameter::XPCalcStateUnknown) && (aTable[i].nValue != pParameter->nCalcState)) {
      i++;
    } /* end while */
    sprintf(pBuffer, "%s%s%s%s%s%s%s%s", pParameter->szParameterName.asCharp(), VALUE_SEPSTRING_CN00, 
                                         aTable[i].szText,                      LINE_SEPSTRING_CN00,
                                         pParameter->szNewValue.asCharp(),      LINE_SEPSTRING_CN00,
                                         "",                                    LINE_SEPSTRING_CN00);
  } /* end if */

} /* end cn23_PrintCalcResult */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn23_PrintParamDep
 * ------------------------------------------------------------------
 */
static void cn23_PrintParamDep (
      DBMSrvPar_Parameter * pFirst,
      DBMSrvPar_Parameter * pCurrent,
      char             * szIndent,
      char             * szDep)
{
  struct {
    tcn002_XPOperators    nOperator;
    const char        *  szOperator;
  }                      aTable[]      = XP_OPERATOR_TABLE_CN002;   
  DBMSrvPar_XPLine *     pLine         = pCurrent->pCode;
  long                   i             = 0;
  tcn002_XPOperators      nOperator     = XPOperatorUnknown;
  char                   szToken[XP_STRING_LENGTH_MXCN002 + 1];
  char                   szTemp[XP_STRING_LENGTH_MXCN002 + 1];
  long                   nToken        = 0;
  long                   bToken        = true;
  DBMSrvPar_Parameter     * pParameter    = NULL;
  char                   szCurrent[10000];

  printf("%s%s\n", szIndent, pCurrent->szParameterName.asCharp());
  SAPDB_strcpy(szCurrent, "");

  while (pLine != NULL) {
    switch(pLine->Type()) {
//      case XPLineCodeCONSTRAINT:
      case DBMSrvPar_XPLine::XPLineCodeIF:
      case DBMSrvPar_XPLine::XPLineCodeELSIF:
      case DBMSrvPar_XPLine::XPLineCodeCALC:
        /* start at the second token */
        nToken = 2;
        bToken = cn90GetToken(pLine->Line(), szToken, nToken++, XP_STRING_LENGTH_MXCN002 + 1);

        while (bToken) {
          /* reset operator */
          nOperator    = XPOperatorUnknown;

          if (!isdigit(szToken[0]) && (szToken [0] != XP_CHAR_STRING_CN002)) {
            /* check for operator */
            i = 0;
            while( (aTable[i].nOperator != XPOperatorUnknown) && (nOperator == XPOperatorUnknown) ) {
              if (strcmp(aTable[i].szOperator, szToken) == 0) {
                nOperator = aTable[i].nOperator;
              } /* end if */
              i++;
            } /* end while */
            if (nOperator == XPOperatorUnknown) {
              /* exclude self dependencies */
              if (strcmp(pCurrent->szParameterName, szToken) != 0) {
                sprintf(szTemp, "*%s*", szToken);
                /* exclude duplicates in direct dependencies */
                if (strstr(szCurrent, szTemp) == NULL) {
                  /* maintain list of direct dependencies of the current parameter */
                  strcat(szCurrent, szTemp);
                  pParameter = pFirst;
                  while (pParameter != NULL) {
                    if (strcmp(pParameter->szParameterName, szToken) == 0) {
                      /* exclude duplicates in recursive dependencies */
                      if (strstr(szDep, szTemp) != NULL) {
                        printf("  %s%s *** Recursion found! No check for %s!\n", szIndent, szToken, szToken);
                      } else {
                        /* maintain list of recursive dependencies of the current parameter */
                        strcat(szDep, szTemp);
                        /* maintain indent for nice printing */
                        strcat(szIndent, "  ");
                        /* recursive analysis */
                        cn23_PrintParamDep(pFirst, pParameter, szIndent, szDep);
                        /* maintain indent for nice printing */
                        szIndent[strlen(szIndent) - 2] = CHAR_STRINGTERM_CN90;
                        /* remove parameter  from list */
                        szDep[strlen(szDep) - strlen(szTemp)] = CHAR_STRINGTERM_CN90;
                      } /* end if */
                    } /* end if */
                    pParameter = pParameter->pNext;
                  } /* end while */
                } /* end if */
              } /* end if */
            } /* end if */
          } /* end if */

          /* next token */
          bToken = cn90GetToken(pLine->Line(), szToken, nToken++, XP_STRING_LENGTH_MXCN002 + 1);
        } /* end while */

        break;
    } /* end switch */

    pLine = pLine->Next();
  } /* end while */

} /* end cn23_PrintParamDep */
