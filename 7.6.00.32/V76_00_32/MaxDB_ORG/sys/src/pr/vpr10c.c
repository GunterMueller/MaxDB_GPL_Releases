/*!================================================================
 module:    vpr10c.c

 responsible: BurkhardD
 
 special area:   
 
 description: 
 
 last changed: 
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
#include "vpr10c.h"
#include "vpa40DBC.h"
#include "vpr01Env.h"
#include "vpr01SQL.h"
#include "vpr01StmtName.h"
#include "vpr03Packet.h"
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define PR10_CONNECT_DEFAULT  "DB_ODBC"

static tpr01_SQLContainer *pr10SQL = NULL;
static tpr01_StmtNameContainer *pr10StmtName = NULL;

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  EXPORTED FUNCTIONS                                              *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/* stolen from veo131.c and modified */
static bool Ascii8Copy (unsigned char *destAscii8, int destLen, 
			const unsigned short *srcUCS2, int srcLen)
{
  int i;

  memset (destAscii8, ' ', destLen);
  for (i = 0; i < srcLen/2; i++) {
    if (srcUCS2[i] < 256) {
      destAscii8[i] = (unsigned char) srcUCS2[i];
    } else {
      memset (destAscii8, ' ', destLen);
      return false;
    }
  }
  return true;
}

tpr01_ConDesc *pr10Connect (tpa40DBC *api_dbc_ptr, tpr05_String *ConStr,
			   sqldbnode sqcdbnode, sqldbname sqcdbname,
			   tpr05_String *UID, tpr05_String *PWD)
{
  tpr01_StmtNameDesc *StmtNameDesc;
  tpr01_EnvDesc *EnvDesc;
  tpr01_ConContainer *Connection;
  tpr01_ConDesc *ConDesc;
  tsp00_Name passw;
  sqlcatype *sqlca = &(api_dbc_ptr->esqblk.sqlca);
      
  /*  
  if (!pr10Env) {
    tpr01_EnvContainer *Env;
    pr10Env = pr01EnvNewCont();
  }
  EnvDesc = pr10Env->AddDesc(pr10Env, sqlca);
  */

  if (!pr10SQL) {
    pr10SQL = pr01SQLNewCont(api_dbc_ptr->EnvDesc);
  }
  
  if (!sqlca->sqlcxap->xaSQLDesc) 
    sqlca->sqlcxap->xaSQLDesc = pr10SQL->AddDesc(pr10SQL);
  pr10SQL->InitDesc(sqlca->sqlcxap->xaSQLDesc, (sqlxatype*)sqlca->sqlcxap);
  if (!pr10StmtName) {
    pr10StmtName = pr01StmtNameNewCont();
    StmtNameDesc = pr10StmtName->AddDesc(pr10StmtName, NULL, ODBCConStmtID_cpr01, NULL);
  }
  else {
    StmtNameDesc = pr10StmtName->FindDesc(pr10StmtName, NULL, ODBCConStmtID_cpr01);    
  }
  if (!api_dbc_ptr->ConDesc) {
    tpr01_EnvContainer *Env = api_dbc_ptr->EnvDesc->Env;
    Connection = Env->GetConnectionCont (api_dbc_ptr->EnvDesc);
    api_dbc_ptr->ConDesc = Connection->AddDesc (Connection, PR10_CONNECT_DEFAULT, (sqlxatype*) api_dbc_ptr->esqblk.sqlca.sqlcxap, SessionDefault_epr01Con);    
  }
  ConDesc    = api_dbc_ptr->ConDesc;
  Connection = ConDesc->Connection;      /* determine container of ConDesc */
  sqlca->sqlcxap->xaSQLDesc->ConDesc = ConDesc;
  sqlca->sqlcxap->xalang = CPR_LA_CALL;
  sqlca->sqlrap->radbmode = cpr_kind_internal;
  p03csqlcaareainit(sqlca);              /* PTS 1120970 */
  p01xtracefilecheck (sqlca, (sqlxatype*)sqlca->sqlcxap);
  sqlca->sqlcxap->xakano = cpr_com_sql_conn;
  pr10StmtName->PutSQL(StmtNameDesc, ConStr, First_epr01);
  Connection->SetAttr(ConDesc, ServerNode_epr01Con, sqcdbnode, sizeof(sqldbnode));
  sqlca->sqlcxap->xaSQLDesc->ConDesc->ga->gaServerNode = (char*)sqcdbnode;
  Connection->SetAttr(ConDesc, ServerName_epr01Con, sqcdbname, sizeof(sqldbname));
  {
    tsp00_Pw password;
    char tmpPasswordUCS2[2*PW_MXSP00];
    tsp00_Pw tmpPasswordAscii;
    
    tsp00_Uint4 cbWrite;
    tsp00_Uint4 cbParsed;
    tsp00_Uint4 fillLength;
    void *fillPointer;
    bool bpureAsciiUser = true;
    bool bpureAsciiPassw = true;

    fillLength  = sizeof (ConDesc->XUserRecord->xu_userUCS2);
    fillPointer = &ConDesc->XUserRecord->xu_userUCS2[0];
    sp77encodingUCS2Native->fillString (&fillPointer, 
                                        &fillLength, 
                                        fillLength / 2, 
                                        ' ');

    sp78convertString (sp77encodingUCS2Native, 
                       &ConDesc->XUserRecord->xu_userUCS2, 
                       sizeof (ConDesc->XUserRecord->xu_userUCS2),
                       &cbWrite, 
                       FALSE, 
                       UID->encodingType, 
                       UID->rawString, 
                       UID->cbLen, 
                       &cbParsed);

    memset (ConDesc->XUserRecord->xu_user, ' ', sizeof (ConDesc->XUserRecord->xu_user));  /* PTS 1113399 */
    bpureAsciiUser = Ascii8Copy ((unsigned char *) &ConDesc->XUserRecord->xu_user[0], 
				 sizeof (ConDesc->XUserRecord->xu_user),
                                 (const unsigned short *) &ConDesc->XUserRecord->xu_userUCS2[0],
                                 cbWrite);
    if (!bpureAsciiUser) {
      memset (&ConDesc->XUserRecord->xu_user[0], ' ', 
              sizeof (ConDesc->XUserRecord->xu_user));
    }

    sp78convertString (sp77encodingUCS2Native, 
                       &tmpPasswordUCS2[0], 
                       sizeof (tsp00_Pw) * sizeof (tsp81_UCS2Char),
                       &cbWrite, 
                       FALSE, 
                       PWD->encodingType, 
                       PWD->rawString, 
                       PWD->cbLen, 
                       &cbParsed);

    bpureAsciiPassw = Ascii8Copy ((unsigned char *) &tmpPasswordAscii[0], 
				  sizeof (tmpPasswordAscii),
                                  (const unsigned short *) &tmpPasswordUCS2[0],
                                  cbWrite);
    if (!bpureAsciiPassw || !bpureAsciiUser) {
      fillLength  = sizeof (tsp00_Pw);
      fillPointer = password;
      sp77encodingUCS2Native->fillString (&fillPointer, &fillLength, fillLength/2, ' ');
      memset (&ConDesc->XUserRecord->xu_user[0], ' ', sizeof (tsp00_KnlIdentifier));
      memcpy (password, tmpPasswordUCS2, cbWrite);
      Connection->SetAttr (ConDesc, PWDUCS2_epr01Con, password, sizeof (tsp00_Pw));
      sqlca->sqlcxap->xaSQLDesc->PacketEncoding = sp77encodingUCS2Native;
    } else {
      memset (password, ' ', sizeof (tsp00_Pw));
      memcpy (password, tmpPasswordAscii, cbWrite/2);
      Connection->SetAttr (ConDesc, PWD_epr01Con, password, sizeof (tsp00_Pw));
      sqlca->sqlcxap->xaSQLDesc->PacketEncoding = sp77encodingAscii;
    }
  }
    
  Connection->Connect (ConDesc, StmtNameDesc);

  if (sqlca->sqlcode == 0) {
    sqlca->sqldbmode = cpr_kind_internal;
    sqlca->sqlrap->rakamode = cpr_kind_internal;
#ifdef _UNICODE_ODBC
    /* the ASCII ODBC driver uses always ASCII packets for communication.
       Only the Unicode driver looks at the kernel parameter and uses
       than always (!) Unicode packets, if kernel is Unicode (cmp vpa30.c). 
       http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122580 */
     {
       char IsUnicode = 0;      
       pr03PacketGetKernelParameters( (tsp1_segment*)
                                      sqlca->sqlrap->rasegptr,
                                      UnicodeCatalog_epr03,
                                      &IsUnicode);
       if (IsUnicode)
         sqlca->sqlcxap->xaSQLDesc->PacketEncoding = sp77encodingUCS2Native;
       else
         sqlca->sqlcxap->xaSQLDesc->PacketEncoding = sp77encodingAscii;
     }
#endif
  }
  return ConDesc;
}

static const sqlint2 pr10DBNo_dummy = cpr_se_primary;

void pr10Disconnect (tpa40DBC *api_dbc_ptr)
{
  tpr01_ConDesc *ConDesc = api_dbc_ptr->ConDesc;
  sqlcatype *sqlca = &(api_dbc_ptr->esqblk.sqlca);
  p03csqlcaareainit (sqlca);
  p10release  (sqlca, ConDesc->ga, &pr10DBNo_dummy);
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131844 */
  {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    Connection->Release(ConDesc);
    api_dbc_ptr->ConDesc = NULL;    
  }
  /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1128067 */
  if (pr10SQL) {
    if (sqlca->sqlcxap->xaSQLDesc) {
      pr10SQL->DeleteDesc (sqlca->sqlcxap->xaSQLDesc);
      sqlca->sqlcxap->xaSQLDesc = 0;
    }
  }

}


void pr10DropParseID (tpa40DBC *api_dbc_ptr, const sqlparsid sqcparsid)
{
  tpr01_ConDesc *ConDesc = api_dbc_ptr->ConDesc;
  sqlcatype *sqlca = &(api_dbc_ptr->esqblk.sqlca);
  sqlgaentry *gae = (ConDesc) ? ConDesc->ga : NULL;
  p10dropparsid(sqlca, gae, &pr10DBNo_dummy, sqcparsid);
}


tpr01_SQLDesc *pr10MakeSQLDesc(tpr01_ConDesc *ConDesc, tpr01_StmtNameDesc *StmtNameDesc, sqldatype *sqlda)
{
  sqlcatype *sqlca = ConDesc->sqlca;
  tpr01_SQLDesc *SQLDesc = sqlca->sqlcxap->xaSQLDesc;
  sqlkaentry *ka = StmtNameDesc->ka;
  ka->kamode = sqlca->sqlrap->rakamode;
  ka->kapacount = -1;
  pr10SQL->InitDesc(SQLDesc, (sqlxatype*)sqlca->sqlcxap);
  SQLDesc->ConDesc = ConDesc;
  SQLDesc->StmtNameDesc = StmtNameDesc;
  SQLDesc->ore = StmtNameDesc->ore;
  SQLDesc->ka = StmtNameDesc->ka;
  SQLDesc->ComType = cpr_com_sql;
  sqlca->sqlcxap->xasqldap = sqlda;
  return SQLDesc;
}

