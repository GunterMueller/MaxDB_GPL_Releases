/*!================================================================
 module:    vpr01Error.h

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

#ifndef __VPR01_ERROR_H__
#define __VPR01_ERROR_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef tsp00_C70  tpr01_ErrorText;
typedef tsp00_Int4 tpr01_ErrorCode;

typedef struct tpr01_ErrorMethodes {
  struct tpr01_ErrorDesc *(*OpenDesc)   (struct tpr01_ErrorDesc *Desc);
  struct tpr01_ErrorDesc *(*NextDesc)   (struct tpr01_ErrorDesc *Desc);
  struct tpr01_ErrorDesc *(*PrevDesc)   (struct tpr01_ErrorDesc *Desc);
  struct tpr01_ErrorDesc *(*AddDesc)    (struct tpr01_ErrorDesc *Desc);
  BOOLEAN                 (*SetError)   (struct tpr01_ErrorDesc *Desc, tsp00_Int4 ErrorCode, char *szErrorText, tsp00_Int4 cbLenErrorText);
  tpr01_ErrorCode         (*GetError)   (struct tpr01_ErrorDesc *Desc, tpr01_ErrorCode *ErrorCode, char *szErrorText, tsp00_Int4 *cbLenErrorText, tsp00_Int4 cbLenErrorMax);
  void                    (*CloseDesc)  (struct tpr01_ErrorDesc *Desc);
  void                    (*DeleteDesc) (struct tpr01_ErrorDesc *Desc);
  void                    (*MapError)   (struct tpr01_ErrorDesc *Desc, tpr01_DescTypeEnum DescType, tpr00_ErrorKeyEnum ErrorKey, char *szErrorText, tpr01_ErrorCode *cbErrorCode);
} tpr01_ErrorMethod;

typedef struct tpr01_ErrorDesc {
  tpr01_DescTypeEnum DescType;
  tsp00_Int4         ErrorCode;
  char              *szErrorText;
  tsp00_Int4         cbLenErrorText;
  tpr09DynamicDescriptor *Desc;
  tpr01_ErrorMethod *Error;
} tpr01_ErrorDesc;

typedef struct tpr01_ErrorMessage {
  tpr00_ErrorKeyEnum Key;
  tpr01_ErrorText Text;
  tpr01_ErrorCode Code;  
}tpr01_ErrorMessage;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

tpr01_ErrorDesc *pr01NewErrorDesc();

void pr01DeleteErrorDesc(tpr01_ErrorDesc *Desc);

tsp00_Int4 pr01ErrorGetErrorText(tpr_runtime_errors_Enum ErrorEnum, tsp00_Int4 *ErrorCode, tpr01_ErrorText szErrorText);

#endif
