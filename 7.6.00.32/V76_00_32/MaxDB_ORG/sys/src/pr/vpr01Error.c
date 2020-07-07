/*!================================================================
 module:    vpr01Error.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Error 
 description: Verwaltung von Runtime Errors
 
 see:  
  



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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vsp001.h"
#include "gpr03.h"
#include "vpr01Error.h"
#include "vpr07Macro.h"
#include "gpr03m.h"
#include <stdio.h>
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
#define PR01DEFERROR(ErrorKey, ErrorCode, ErrorStr) { ErrorKey , ErrorStr, ErrorCode }

static tpr01_ErrorMessage pr01ErrorTable[] = {
  PR01DEFERROR(ErrNoError, 0, ""),
  PR01DEFERROR(ErrRowNotFound, 100, "Row not found"),
  PR01DEFERROR(ErrDescNotFound, -790, "Descriptor not found"),
  PR01DEFERROR(ErrSqlCancelled, -102, "SQL statement cancelled"),
  PR01DEFERROR(ErrInpDefaultNotAllowed, -822, "No default value available"),
  PR01DEFERROR(ErrInpNullNotAllowed, -801, "Input parameter cannot be NULL value"),
  PR01DEFERROR(ErrInpNumberInvalid, -802, "Invalid numeric input parameter value"),
  PR01DEFERROR(ErrInpNumberOverflow, -751, "Numeric input parameter overflow"),
  PR01DEFERROR(ErrInvalidCommandstate, -9801, "Invalid command state"),
  PR01DEFERROR(ErrInvalidProgram, -805, "Program must be precompiled"),
  PR01DEFERROR(ErrDBSessionNotAllowed, -806, "DB session number not allowed"),
  PR01DEFERROR(ErrNoSessionConnected, -807, "Connection down, session released"),
  PR01DEFERROR(ErrNotImplemented, -808, "Not yet implemented"),
  PR01DEFERROR(ErrOutNullNotAllowed, -809, "Missing indicator, output param with NULL value"),
  PR01DEFERROR(ErrOutNumberInvalid, -9806, "Invalid output value"),
  PR01DEFERROR(ErrOutNumberOverflow, -811, "Numeric output parameter overflow"),
  PR01DEFERROR(ErrParamListTooLong, -804, "Parameter list too long"),
  PR01DEFERROR(ErrParamListTooShort, -812, "Parameter list too short"),
  PR01DEFERROR(ErrParamListWrong, -883, "Invalid runtime options"),
  PR01DEFERROR(ErrMissingIndicatorvariable, -884, "Missing indicator variable"),
  PR01DEFERROR(ErrReflexStartRequired, -813 , "SERVERDB must be restarted"),
  PR01DEFERROR(ErrReflexTimeout, +700, "Command inactivity timeout (work rolled back)"),
  PR01DEFERROR(ErrReflexCrash, +800, "Implicit SERVERDB restart"),
  PR01DEFERROR(ErrRequestAreaOverflow, -706, "SQL statement too long"),
  PR01DEFERROR(ErrTraceFileClose, -814, "Trace file close error"),
  PR01DEFERROR(ErrTraceFileOpen, -815, "Trace file open error"),
  PR01DEFERROR(ErrTraceFileWrite, -816, "Trace file write error"),
  PR01DEFERROR(ErrIncompatibleDatatype, -817, "Incompatible data types"),
  PR01DEFERROR(ErrUnknownDatatype, -9803, "Unknown data type"),
  PR01DEFERROR(ErrUnknownStatementName, -880, "Unknown statement name"),
  PR01DEFERROR(ErrUnknownCursorName, -874, "Unknown cursor name"),
  PR01DEFERROR(ErrAreaOverflow, -761, "AREA SIZE IS TOO SMALL"),
  PR01DEFERROR(ErrMemoryAllocationFaild, -760, "Memory allocation failed"),
  PR01DEFERROR(ErrTooManyHostvar, -893, "Too many hostvariables"),
  PR01DEFERROR(ErrTooManyHostvarCall, -712, "Too many parameters, SQLMAX too small"),
  PR01DEFERROR(ErrMacroNotInit, -818, "Missing macro parameter initialization"),
  PR01DEFERROR(ErrParamlistNotAllowed, -819, "Parameter in SQL statement not allowed"),
  PR01DEFERROR(ErrInvalidDeclareCommand, -820, "Invalid declare statement"),
  PR01DEFERROR(ErrLoopInitWrong, -856, "Invalid loop parameter value"),
  PR01DEFERROR(ErrMissingVariableAddr, -702, "Missing variable address in SQLDA"),
  PR01DEFERROR(ErrDuplicateFetch, -703, "Duplicate FETCH statem.with MFETCH-opt not allowed"),
  PR01DEFERROR(ErrDuplicateFetchCall, -703, "Duplicate FFETCH statement for same SELECT"),
  PR01DEFERROR(ErrTooManyFetchStatements, -705, "Too many FFETCH statements"),
  PR01DEFERROR(ErrCmdTooLong, -706, "SQL statement too long (buffer overflow)"),
  PR01DEFERROR(ErrConnectSyntaxWrong, -707, "Invalid CONNECT statement syntax"),
  PR01DEFERROR(ErrReflexShutdown, -708, "SERVERDB system not available"),
  PR01DEFERROR(ErrReflexConnectNotOk, -709, "CONNECT:"),
  PR01DEFERROR(ErrSelectParsidMissing, -710, "Missing SELECT PARSID"),
  PR01DEFERROR(ErrImplicitConnectWrong, -711, "Implicit CONNECT : missing user or SERVERDB"),
  PR01DEFERROR(ErrMissingUserPassw, -715, "Missing username or password for CONNECT"),
  PR01DEFERROR(ErrUnknownSysprofile, -719, "Unknown LOCALSYSUSER.SYSPROFILE table"),
  PR01DEFERROR(ErrErrorNoFromSysprofile, -720, "SQLCODE         comes from SYSPROFILE command"),
  PR01DEFERROR(ErrInpStringTruncated, -743, "Input string too long"),
  PR01DEFERROR(ErrUnterminatedCString, -767, "Unterminated C string"),
  PR01DEFERROR(ErrInvalidEscape, -768, "Invalid escape value"),
  PR01DEFERROR(ErrMultiByteSetNotAllowed, -772, "Multibyte char set not allowed"),
  PR01DEFERROR(ErrDataFileOpen, -773, "Datafile open error"),
  PR01DEFERROR(ErrDataFileWrite, -774, "Datafile write error"),
  PR01DEFERROR(ErrDataFileRead, -775, "Datafile read error"),
  PR01DEFERROR(ErrDataFileClose, -776, "Datafile close error"),
  PR01DEFERROR(ErrConvFromUnicodeImpossible, -777, "Conversion from UNICODE impossible"),
  PR01DEFERROR(ErrConvToUnicodeImpossible, -778, "Conversion to UNICODE impossible"),
  PR01DEFERROR(ErrNoLongColumnsFound, -779, "PUTVAL: No long columns found"),
  PR01DEFERROR(ErrUseCallStm, -744, "Use procedure sqcstatement"),
  PR01DEFERROR(ErrValbufTooShort, -747, "Value buffer is too short"),
  PR01DEFERROR(ErrFfetchNotAllowed, -749, "FFETCH not allowed for this select"),
  PR01DEFERROR(ErrFetchMixNotAllowed, -750, "ARRAY-FETCH and FETCH mix not allowed"),
  PR01DEFERROR(ErrSessionAlreadyConnected, -752, "Session already connected"),
  PR01DEFERROR(ErrNoOutputVariable, -753, "Output parameter with CONST type"),
  PR01DEFERROR(ErrDifferSqlmodeNotAllowed, -756, "Different SQLMODEs not allowed"),
  PR01DEFERROR(ErrMissingPreviousFetch, -7026, "SQL statement not allowed without prev. FETCH"),
  PR01DEFERROR(ErrTooManyStatements, -868, "Too many SQL statements"),
  PR01DEFERROR(ErrTooManyUsers, -9807, "Too many users"),
  PR01DEFERROR(ErrSetFormatError, -9808, "SET FORMAT error"),
  PR01DEFERROR(ErrXuserFileOpen, -757, "XUSER file open error"),
  PR01DEFERROR(ErrCmdNotAvailable, -101, "Command not available in this mode"),
  PR01DEFERROR(ErrAbapDescNotSet, -861, "ABAP descriptor not set"),
  PR01DEFERROR(ErrAbapException, -862, "ABAP exception"),
  PR01DEFERROR(ErrConnectionBusy, -810, "Connection already in use"),
  PR01DEFERROR(ErrorUnknown, -899, "Error unknown")
};

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static tpr01_ErrorDesc *pr01ErrorOpenDesc(tpr01_ErrorDesc *Desc);

static void pr01ErrorCloseDesc(tpr01_ErrorDesc *Desc);

static tpr01_ErrorDesc *pr01ErrorNextDesc(tpr01_ErrorDesc *Desc);

static tpr01_ErrorDesc *pr01ErrorPrevDesc(tpr01_ErrorDesc *Desc);

static tpr01_ErrorDesc *pr01ErrorAddDesc(tpr01_ErrorDesc *Desc);

static void pr01ErrorDeleteDesc(tpr01_ErrorDesc *Desc);

static BOOLEAN pr01ErrorSetError (tpr01_ErrorDesc *Desc, tpr01_ErrorCode ErrorCode, char *szErrorText, tsp00_Int4 cbLenErrorText);

static tsp00_Int4 pr01ErrorGetError (tpr01_ErrorDesc *Desc, tpr01_ErrorCode *ErrorCode, char *szErrorText, tsp00_Int4 *cbLenErrorText, tsp00_Int4 cbLenErrorMax);

static void pr01ErrorMapError (struct tpr01_ErrorDesc *Desc, tpr01_DescTypeEnum DescType, tpr00_ErrorKeyEnum ErrorKey, char *szErrorText, tpr01_ErrorCode *ErrorCode);
/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static void pr01InitDesc(tpr01_ErrorDesc *Err, tpr01_ErrorMethod *Error, tpr09DynamicDescriptor *Desc)
{
  Err->ErrorCode = 0;
  Err->szErrorText = NULL;
  Err->cbLenErrorText = 0;
  Err->Error = Error;
  Err->Desc = Desc;
  Err->DescType = ErrorDesc_epr01;
}

tpr01_ErrorDesc *pr01NewErrorDesc()
{
  tpr01_ErrorDesc *ErrorDesc=NULL;
  tpr09DynamicDescriptor *Desc = pr09NewDescriptor(1, sizeof(tpr01_ErrorDesc));
  if (Desc) {
    ErrorDesc = pr09AddItem(Desc);
    ErrorDesc->Error = pr03mAllocat(sizeof(tpr01_ErrorMethod), "ErrorDesc->Error");
    if (!ErrorDesc->Error)
      return NULL;    
    ErrorDesc->Error->OpenDesc   = pr01ErrorOpenDesc;
    ErrorDesc->Error->NextDesc   = pr01ErrorNextDesc;
    ErrorDesc->Error->PrevDesc   = pr01ErrorPrevDesc;
    ErrorDesc->Error->AddDesc    = pr01ErrorAddDesc;
    ErrorDesc->Error->SetError   = pr01ErrorSetError;
    ErrorDesc->Error->GetError   = pr01ErrorGetError;
    ErrorDesc->Error->CloseDesc  = pr01ErrorCloseDesc;
    ErrorDesc->Error->DeleteDesc = pr01ErrorDeleteDesc;
    ErrorDesc->Error->MapError   = pr01ErrorMapError;
    pr01InitDesc(ErrorDesc, ErrorDesc->Error, Desc);
  }
  return ErrorDesc;
}

void pr01DeleteErrorDesc(tpr01_ErrorDesc *ErrorDesc)
{
  if (ErrorDesc) {
    if (ErrorDesc->szErrorText)	
      pr03mFree((tsp00_Uint1*)ErrorDesc->szErrorText, "ErrorDesc->szErrorText");  
    if (ErrorDesc->Error) {
      pr03mFree((tsp00_Uint1*)ErrorDesc->Error, "ErrorDesc->Error");
    }
    pr09DeleteDescriptor(ErrorDesc->Desc);
  }
}

static tpr01_ErrorDesc *pr01ErrorOpenDesc(tpr01_ErrorDesc *Desc)
{
  pr09SetIndex(Desc->Desc, 0);
  return(pr09GetItem(Desc->Desc));  
}

static void pr01ErrorCloseDesc(tpr01_ErrorDesc *Desc)
{
  pr09CloseAllItems(Desc->Desc);
}

static tpr01_ErrorDesc *pr01ErrorNextDesc(tpr01_ErrorDesc *Desc)
{
  return (tpr01_ErrorDesc *)pr09GetNextItem(Desc->Desc);
}

static tpr01_ErrorDesc *pr01ErrorPrevDesc(tpr01_ErrorDesc *Desc)
{
  return (tpr01_ErrorDesc *)pr09GetPrevItem(Desc->Desc);
}

static tpr01_ErrorDesc *pr01ErrorAddDesc(tpr01_ErrorDesc *Desc)
{
  tpr01_ErrorDesc *NewErr = pr09AddItem(Desc->Desc);
  if (NewErr) {
    tpr01_ErrorDesc *Err = pr09GetItem(Desc->Desc);
    pr01InitDesc(NewErr, Err->Error, Desc->Desc);
  }
  return NewErr;
}

static void pr01ErrorDeleteDesc(tpr01_ErrorDesc *Desc)
{
  pr09CloseItem(Desc->Desc);  
}

static BOOLEAN pr01ErrorSetError (tpr01_ErrorDesc *ErrorDesc, tpr01_ErrorCode ErrorCode, char *szErrorText, tsp00_Int4 cbLenErrorText)
{
  BOOLEAN bOk = TRUE;

  if (szErrorText && cbLenErrorText == CPR_NTS)
    cbLenErrorText = (tsp00_Int4)strlen(szErrorText);

  if (szErrorText && cbLenErrorText > 0) {
    if (ErrorDesc->szErrorText) {
      if (cbLenErrorText > ErrorDesc->cbLenErrorText) {
	ErrorDesc->szErrorText = pr03mReallocat(ErrorDesc->szErrorText, cbLenErrorText, "ErrorDesc->szErrorText");
      }
    }
    else
      ErrorDesc->szErrorText = pr03mAllocat(cbLenErrorText, "ErrorDesc->szErrorText");
    if (ErrorDesc->szErrorText) {
      memcpy(ErrorDesc->szErrorText, szErrorText, cbLenErrorText);
      ErrorDesc->cbLenErrorText = cbLenErrorText;
      ErrorDesc->ErrorCode = ErrorCode;
    }
    else
      bOk = FALSE; 
  }
  else 
    bOk = FALSE;
  return bOk;
}

static tpr01_ErrorCode pr01ErrorGetError (tpr01_ErrorDesc *Desc, tpr01_ErrorCode *ErrorCode, char *szErrorText, tsp00_Int4 *cbLenErrorText, tsp00_Int4 cbLenErrorMax)
{
  PR07_CHECKDESC(Desc, ErrorDesc_epr01);    
  {
    tsp00_Int4 cbLen = PR07MIN(cbLenErrorMax, Desc->cbLenErrorText);
    if (szErrorText) {
      if (cbLen > 0) {
	memcpy(szErrorText, Desc->szErrorText, cbLen);
      }
      szErrorText[cbLen];
    }
    if (cbLenErrorText)
      *cbLenErrorText = cbLen;
    if (ErrorCode) {
      *ErrorCode = Desc->ErrorCode;
    }
    return Desc->ErrorCode;
  }
}

tsp00_Int4 pr01ErrorGetErrorText(tpr_runtime_errors_Enum ErrorEnum, tpr01_ErrorCode *ErrorCode, tpr01_ErrorText szErrorText)
{
  struct SQLERROR sqlemp;
  sqlemp.ereturncode = 0;
  sqlemp.elzu = sp1ce_ok;
  p03cseterror (&sqlemp, ErrorEnum);
  memcpy(szErrorText, sqlemp.etext, sqlemp.etextlen);
  szErrorText[sqlemp.etextlen] = '\0';  
  *ErrorCode = sqlemp.ereturncode;
  return sqlemp.etextlen;
}

typedef struct {
  tpr01_DescTypeEnum DescType;
  char szDescTypeName[18];
} tpr01_DescTypeName;

static tpr01_DescTypeName pr01ErrorDescTypeName[] = {
  { UnknownDesc_epr01, "UnknownDesc" },
  { EnvDesc__epr01, "EnvDesc" },
  { ModuleDesc_epr01, "ModuleDesc" },
  { ConDesc_epr01, "ConDesc" },
  { PrecomDesc_epr01, "PrecomDesc" },
  { StmtNameDesc_epr01, "StmtNameDesc" },
  { SQLDesc_epr01, "SQLDesc" },
  { CursorDesc_epr01, "CursorDesc" },
  { CommandDesc_epr01, "CommandDesc" },
  { ErrorDesc_epr01, "ErrorDesc" },
  { SegDesc_epr01, "SegDesc" }
};

static char *pr01ErrorGetDescTypeName(tpr01_DescTypeEnum DescType)
{
  static char *szDescTypeName="UNKNOWN";
  tpr01_DescTypeName *DescTypeName;
  DescTypeName=pr01ErrorDescTypeName;
  while (DescTypeName != &pr01ErrorDescTypeName[PR07NUMELEM(pr01ErrorDescTypeName)-1]) {
    if (DescTypeName->DescType == DescType) {
      szDescTypeName = DescTypeName->szDescTypeName;
      break;
    }
    DescTypeName++;
  }  
  return szDescTypeName;
}

static void pr01ErrorMapError (struct tpr01_ErrorDesc *Desc, tpr01_DescTypeEnum DescType, tpr00_ErrorKeyEnum ErrorKey, char *szErrorText, tpr01_ErrorCode *ErrorCode)
{
  PR07_CHECKDESC(Desc, ErrorDesc_epr01);
  {
    char *szDescTypeName=pr01ErrorGetDescTypeName(DescType);
    tpr01_ErrorMessage *ErrorTabEntry;
    
    ErrorTabEntry=pr01ErrorTable;
    while (ErrorTabEntry != &pr01ErrorTable[PR07NUMELEM(pr01ErrorTable)-1]) {
      if (ErrorTabEntry->Key == ErrorKey) {
	break;
      }
      ErrorTabEntry++;
    }
    if (szErrorText) {
      szErrorText = (char*)ErrorTabEntry->Text;
    }
    if (ErrorCode) {
      *ErrorCode = ErrorTabEntry->Code;
    }
  }
}
