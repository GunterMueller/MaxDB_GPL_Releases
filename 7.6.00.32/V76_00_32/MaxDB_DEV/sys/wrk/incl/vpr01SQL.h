/****************************************************************************

 module:    vpr01SQL.h

 -------------------------------------------------------------------------

 responsible: BurkhardD

 special area:   CPCDrv | Client | SQLStatment Verwaltung
 description:    SQL Verwaltung

 last changed: 2000-08-07  17:17 Marco Paskamp  Unicode Erweiterungen
 see also    :

 -------------------------------------------------------------------------





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




*****************************************************************************/

#ifndef __VPR01_SQL_H__
#define __VPR01_SQL_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
typedef struct tpr01_SQLAttribute {
  BOOLEAN massopen;
  BOOLEAN parspre;
} tpr01_SQLAttribute;

typedef struct tpr01_SQLDesc {
  tpr01_DescTypeEnum      DescType;
  struct tpr01_SQLContainer *SQL;
  struct tpr01_StmtNameDesc *StmtNameDesc;
  struct tpr01_CursorDesc   *CursorDesc;
  struct tpr01_ErrorDesc    *ErrorDesc;
  struct tpr01_ConDesc      *ConDesc;
  tpr00_ComTypeEnum ComType;
  tsp1_cmd_mess_type  MessType;
  tpr05_StringEncoding PacketEncoding;
  sqlprentry *pr;
  sqlkaentry *ka;
  sqlorentry *ore;
  sqlcuentry *cu;
  sqlxatype  *sqlxa;
  tpr01_SQLAttribute Attribute;
} tpr01_SQLDesc;


typedef struct tpr01_SQLContainer {
  struct tpr01_EnvDesc *EnvDesc;
  tpr09DynamicDescriptor *Desc;
  void                    (*InitCont)  (struct tpr01_SQLContainer *Cont, struct tpr01_EnvDesc *EnvDesc);
  tsp00_Int4              (*OpenCont)  (struct tpr01_SQLContainer *Cont);
  void                    (*CloseCont) (struct tpr01_SQLContainer *Cont);
  tpr01_SQLDesc *(*AddDesc)   (struct tpr01_SQLContainer *Cont);
  void           (*DeleteDesc) (tpr01_SQLDesc *SQLDesc);
  tsp00_Int4              (*GetCount)  (struct tpr01_SQLContainer *Cont);
  tpr01_SQLDesc *(*EnumDesc)  (struct tpr01_SQLContainer *Cont);
  tsp00_Int4              (*SetIndex)  (struct tpr01_SQLContainer *Cont, tsp00_Int4);
  tpr01_SQLStateBinary (*State ) (struct tpr01_SQLDesc *Desc);

  void        (*Prepare)  (struct tpr01_SQLDesc *Desc);
  boolean     (*TryExecute) (struct tpr01_SQLDesc *Desc);
  boolean     (*Execute)  (struct tpr01_SQLDesc *Desc);
  void        (*Open)     (struct tpr01_SQLDesc *Desc);
  void        (*Describe) (struct tpr01_SQLDesc *Desc);
  void        (*Close)    (struct tpr01_SQLDesc *Desc);
  void        (*Release)  (struct tpr01_SQLDesc *Desc);

  tsp00_Int4  (*SetError) (struct tpr01_SQLDesc *Desc, tpr_runtime_errors_Enum ErrorEnum);

  sqlcatype * (*GetSqlca) (struct tpr01_SQLDesc *Desc);
  sqlxatype * (*GetSqlxa) (struct tpr01_SQLDesc *Desc);
  int         (*PutSQL)   (struct tpr01_SQLDesc *Desc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

  int         (*GetSQL)   (struct tpr01_SQLDesc *Desc, tpr05_String **pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

  void        (*InitDesc) (struct tpr01_SQLDesc *SQLDesc, sqlxatype *sqlxa);

} tpr01_SQLContainer;


/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC tpr01_SQLContainer *pr01SQLNewCont(struct tpr01_EnvDesc *EnvDesc);

externC void pr01SQLDeleteCont(tpr01_SQLContainer *Cont);

externC void pr01SQLgetAllAttributes(struct tpr01_SQLDesc *SQLDesc);

#endif
