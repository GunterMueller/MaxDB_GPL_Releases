/*!================================================================
 module:    vpr01Cursor.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Cursorverwaltung

 description: Cursor Verwaltung

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

#ifndef __VPR01_CURSOR_H__
#define __VPR01_CURSOR_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaHash.h"
#include "vpr09DynaDesc.h"
#include "vpr01SQL.h"
#include "vpr01StmtName.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef enum epr01Cursor_Attr {
  openkeep_epr01Cursor,
  useNewParseInfo_epr01Cursor,
  lastDataReceived_epr01Cursor
} epr01Cursor_Attr;

typedef struct tpr01_CursorAttribute {
  BOOLEAN openkeep;
  BOOLEAN useNewParseInfo;
  BOOLEAN lastDataReceived;  /*true, if last data flag in datapart of received order packet found*/
} tpr01_CursorAttribute;

typedef struct tpr01_CursorDesc_CursorID
{
  tpr05_String * cursorName;
  tpr01_UniqueID UniqueID;
}tpr01_CursorDesc_CursorID;

typedef struct tpr01_CursorDesc {
  tpr01_DescTypeEnum DescType;
  sqlcuentry            *cue;
  tpr01_CursorDesc_CursorID CursorID;
  tpr00_CursorNameDesc CursorNameDesc;    /*tpr05_String containing cursor name*/
  struct tpr01_ErrorDesc       *ErrorDesc;
  struct tpr01_CursorContainer *Cursor;
  struct tpr01_StmtNameDesc    *StmtNameDesc;
  struct tpr01_ConDesc         *ConDesc;
  sqlkaentry *OpenKa;
  struct tpr01_StmtNameDesc    *FetchStmtNameDesc;
  sqlkaentry *FetchKa;
  sqlkaentry *CloseKa;
  tpr01_CursorAttribute Attribute;
  BOOLEAN (*SetAttr) (struct tpr01_CursorDesc *CursorDesc, epr01Cursor_Attr Attr,
		      void *pAttr, tsp00_Int4 cbLen);
  void    (*GetAttr) (struct tpr01_CursorDesc *CursorDesc, epr01Cursor_Attr Attr,
		      void *pAttr, tsp00_Int4 *cbLen);
} tpr01_CursorDesc;

typedef struct tpr01_CursorContainer {
  struct tpr01_ErrorDesc       *ErrorDesc;
#ifdef DYNAHASH
  tpr09DynamicHashDescriptor *Desc;
#else
  tpr09DynamicDescriptor *Desc;
#endif
    tpr01_CursorDesc_CursorID cachedCursorID; /*use for temporary stmt ID'S to avaid malloc/frees*/
  void              (*InitCont)  (struct tpr01_CursorContainer *Cursor);
  tsp00_Int4        (*OpenCont)  (struct tpr01_CursorContainer *Cursor);
  void              (*CloseCont) (struct tpr01_CursorContainer *Cursor);
  tpr01_CursorDesc *(*AddDesc)   (struct tpr01_CursorContainer *Cursor,  tpr05_String *CursorName, tpr01_UniqueID UniqueID, struct tpr01_ConDesc *ConDesc);
  void              (*DeleteDesc)         (tpr01_CursorDesc *CursorDesc);
  tpr01_CursorDesc *(*EnumDesc)  (struct tpr01_CursorContainer *Cursor);
  void           (*ResetIterator)(struct tpr01_CursorContainer *Cursor);
  tpr01_CursorDesc *(*FindDesc)  (struct tpr01_CursorContainer *Cursor,  tpr05_String* CursorName, tpr01_UniqueID UniqueID);
  void              (*Execute)   (tpr01_CursorDesc *CursorDesc);
  tpr01_CursorDesc *(*Declare)   (struct tpr01_CursorContainer *Cursor, struct tpr01_StmtNameDesc *StmtNameDesc, struct tpr01_ConDesc *ConDesc, tpr05_String *CursorName, tpr01_UniqueID UniqueID);
  void              (*PrepareOpen)        (tpr01_CursorDesc *CursorDesc, sqlkaentry *ka);
  void              (*PrepareFetch)       (tpr01_CursorDesc *CursorDesc, struct tpr01_StmtNameDesc *StmtNameDesc, boolean tryexec);
  void              (*PrepareClose)       (tpr01_CursorDesc *CursorDesc, sqlkaentry *ka);
  void              (*Close)              (tpr01_CursorDesc *CursorDesc);
  void              (*AlterCursorName)    (tpr01_CursorDesc *CursorDesc,  tpr05_String* CursorName);
  struct tpr01_ConDesc * (*GetConnection) (tpr01_CursorDesc *CursorDesc);
  void              (*InitCursorName)     (tpr00_CursorNameDesc *CursorNameDesc);
} tpr01_CursorContainer;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC tpr01_CursorContainer *pr01CursorNewCont();

externC void pr01CursorDeleteCont(tpr01_CursorContainer *Cursor);

externC void pr01CursorAlterParseid (tpr01_SQLDesc *SQLDesc);
#endif
