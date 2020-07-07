/*!================================================================
 module:    vpr01Con.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Connection Verwaltung

 description: Connection Verwaltung

 see:

 change history: 

    2001-11-08 D025086 Cancel session bugfix PTS 1112515 and PASCAL 
               port from p01cancel,
    2001-09-04 d025086 No implicite connect. Added OldStyle attribute.


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

#ifndef __VPR01_CON_H__
#define __VPR01_CON_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr01StmtName.h"
#include "vpr03Segment.h"
#include "vpr06ParseId.h"
#include "vpr09DynaDesc.h"
#include "vpr01ParseInfo.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef enum epr01Con_SessionType {
  SessionEmpty_epr01Con,   /* this session is currently unused */
  SessionPrimary_epr01Con, /* parameters could overwritten by SQLOPT */
  SessionDefault_epr01Con, /* normal session */
  SessionTemp_epr01Con     /* temporary session only the Condesc is used */
} epr01Con_SessionType;

typedef enum epr01Con_Status {
  Empty_epr01Con,
  Connected_epr01Con,
  Rollback_epr01Con,
  Commited_epr01Con,
  Subtrans_epr01Con,
  Closed_epr01Con,
  Timeout_epr01Con,
  Crash_epr01Con
} epr01Con_Status;

typedef enum epr01Con_Attr {
  DatabaseName_epr01Con,
  ServerNode_epr01Con,
  ServerName_epr01Con,
  UserName_epr01Con,
  UserNameUCS2_epr01Con,
  CryptPWD_epr01Con,
  PWD_epr01Con,
  PWDUCS2_epr01Con,
  ParseInfoCacheSize_epr01Con,
  HashTableLoadFactor_epr01Con,
  OldStyleConnect_epr01Con,
  ActualSession_epr01Con,
  OptimizedStreams_epr01Con
} epr01Con_Attr;


typedef struct tpr01Con_Feature {
  char feature;
  char value;
} tpr01Con_Feature;

typedef tpr01Con_Feature tpr01Con_FeatureSet[sp1f_last_feature];

extern const tpr01Con_FeatureSet pr01ConFeatureSetWanted;
extern const tpr01Con_FeatureSet pr01ConFeatureSetEmpty;

/*!
  Decleration: tpr01Con_ParseMethod
  Description: pointer to corresponding parse method

  Depending on existence of parseinfo cache different parse methods will be called. If the
  parse info cache exists the ParseInfoCont->Parse () method will invoked. If not the
  old parse from vpr01c.c will invoked.
*/
typedef tsp00_Int4 (*tpr01Con_ParseMethod) (struct tpr01_SQLDesc *SQLDesc, tsp00_Int2 *cmdfetch, tsp00_Int4 *datafound);

typedef struct tpr01_ConDesc {
  tpr01_DescTypeEnum DescType;
  struct tpr01_ConContainer *Connection;
  struct tpr01_SegContainer *Seg;
  struct tpr01_ParseInfoContainer *ParseInfoCache;
  tpr00_DatabaseName szDatabaseName;
  tsp4_xuser_record  *XUserRecord;
  epr01Con_Status Status;
  epr01Con_SessionType SessionType;
  tpr01_ConnectOptionBinary Option;
  struct tpr01_ErrorDesc    *ErrorDesc;
  tpr06_ParseIDCache *ParseIdCache;
  tpr03_SegmDesc  *SegmDesc;
  sqlxatype  *sqlxa;
  sqlcatype  *sqlca;
  sqlgaentry *ga;
  sqlkaentry *ka;
  tsp00_Int2 SessionNr;
  tpr01Con_ParseMethod Parse;  /* Connect descroptor depending parse method */
  bool IsUnicode;              /* Connection to an UNICODE enabled Kernel */
  tpr01Con_FeatureSet DBFeatures;   /* Database Features of this connection */
} tpr01_ConDesc;


typedef struct tpr01_ConContainer {
  struct tpr01_EnvDesc *EnvDesc;
  tpr09DynamicDescriptor *Desc;
  tsp00_Int2 SessionCnt;       /* number of real used sessions */
  tpr01_ParseInfoParams ParseInfoParams;       /* customizable parameters of parseinfo cache*/
  BOOLEAN OldStyle;            /* implicite connect is possible */ 
  tpr01_ConDesc *Actual;        /* pointer to actual (active) connection */ 
  void           (*InitCont)   (struct tpr01_ConContainer *Cont, struct tpr01_EnvDesc *EnvDesc);
  tsp00_Int4     (*OpenCont)   (struct tpr01_ConContainer *Cont);
  void           (*CloseCont)  (struct tpr01_ConContainer *Cont);
  void           (*EmptyCont)  (struct tpr01_ConContainer *Cont);
  tpr01_ConDesc *(*AddDesc)    (struct tpr01_ConContainer *Cont, tpr00_DatabaseName szDatabaseName, sqlxatype *sqlxa, epr01Con_SessionType SessionType);
  void           (*DeleteDesc) (tpr01_ConDesc *ConDesc);
  tsp00_Int4     (*GetCount)   (struct tpr01_ConContainer *Cont);
  tpr01_ConDesc *(*EnumDesc)   (struct tpr01_ConContainer *Cont);
  tsp00_Int4     (*SetIndex)   (struct tpr01_ConContainer *Cont, tsp00_Int4 cbIndex);
  tpr01_ConDesc *(*FindDesc)   (struct tpr01_ConContainer *Cont, tpr00_DatabaseName szDatabaseName, epr01Con_SessionType SessionType);
  tpr01_ConDesc *(*GetConDescByParseID)   (struct tpr01_ConContainer *Cont, tpr00_ParseId *ParseID);
  tpr03_SegmDesc *(*InitPacket) (tpr01_ConDesc *Desc, tpr05_StringEncoding PacketEncoding, tsp1_cmd_mess_type MessType);
  tpr03_SegmDesc *(*SegmentNew) (tpr01_ConDesc *Desc);
  void           (*SegmentDelete) (tpr01_ConDesc *Desc);
  BOOLEAN        (*SetAttr) (tpr01_ConDesc *Desc, epr01Con_Attr Attr, void *pAttr, tsp00_Int4 cbLen);
  void           *(*GetAttr) (tpr01_ConDesc *Desc, epr01Con_Attr Attr, void **pAttr, tsp00_Int4 *cbLen);
  BOOLEAN        (*IsConnected) (tpr01_ConDesc *Desc);
  void           (*AutoConnect) (tpr01_ConDesc *ConDesc);
  tsp1_sqlmode   (*SqlMode)     (tpr01_ConDesc *ConDesc);
  void           (*Connect)     (tpr01_ConDesc *ConDesc, struct tpr01_StmtNameDesc *StmtNameDesc);
  void           (*Open)     (tpr01_ConDesc *ConDesc, sqlkaentry *ka);
  void           (*Release)  (tpr01_ConDesc *ConDesc);
  void           (*Drop)     (tpr01_ConDesc *ConDesc);
  sqlcatype  *sqlca;
  tsp00_Int2 NoLog;
  char OptimizedStreams;
} tpr01_ConContainer;


/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

struct tpr01_ConContainer *pr01ConNewCont(struct tpr01_EnvDesc *EnvDesc);

void pr01ConDeleteCont(struct tpr01_ConContainer *Cont);

#endif
