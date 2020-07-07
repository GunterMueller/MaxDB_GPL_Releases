/*!================================================================
 module:    vpr03AbapInfo.c

 responsible: BurkhardD

 special area:   CPCDrv | Client | VTRACE | Diagnose | ABAP

 description: Kernel VTRACE Unterstuetzung von ABAP Programmen

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

#include "hsp26.h"
#include "gpr00.h"
#include "precom/sqlinfo.h"
#include "gpr03.h"
#include "vpr03sc.h"
#include "gpr08.h"
void p08runtimeerror (sqlcatype*, sqlxatype*, int);
void p08vfwritetrace (sqlratype *sqlra);

#include <limits.h>

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

static SQLABAPInfoProc *pr03ABAPInfoCallBack = NULL;

/*==================================================================*
 *  EXPORTED FUNCTIONS                                                      *
 *==================================================================*/

void pr03AbapInfoPutPart(sqlcatype *sqlca, sqlratype *sqlra, sqlgaentry * gaen);

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

void pr03AbapInfoTrace(sqlratype *sqlrap, char *szABAPInfo, int nElem);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function: sqlInitABAPInfo

  description: Teilt der Interface Runtime die Adresse einer Callbackfunktion
      mit, die jeweils vor dem Parsen eines SQL Kommandos aufgerufen wird.
      Die Callbackfunktion liefert den ABAP Namen und die Zeilennummer als
      Nullterminierte Strings.

  arguments:
      sqlABAPInfoCallBack  [in]  Zeiger auf Callbackfunktion

  returns:
      Die Adresse, auf der zuletzt gesetzten Callbackfunktion.
 */

SQLABAPInfoProc *
sqlInitABAPInfo(SQLABAPInfoProc *sqlABAPInfoCallBack)
{
  SQLABAPInfoProc *pOld = pr03ABAPInfoCallBack;
  pr03ABAPInfoCallBack = sqlABAPInfoCallBack;
  return(pOld);
}

/*!
  Function: pr03AbapInfoPutPart

  description: Fuellt den abap_info_part mit den mittels der Callbackfunktion
      ermittelten ABAP Namen und der Zeilennummer. Die Callbackfunktion
      muss vorher mittels Callbackfunktion sqlInitABAPInfo gesetzt worden
      sein.
      Der Aufruf der Callbackfunktion ist in einen Exceptionhandle gekapselt,
      so das scheitern dieses Calls zur Fortfuehrung des Programmes und dem
      setzten des sqlcode auf ABAP_EXCEPTION fuehrt.

  arguments:
      sqlca, sqlra

 */

void
pr03AbapInfoPutPart(sqlcatype *sqlca, sqlratype *sqlra, sqlgaentry *gae)
{
  if (pr03ABAPInfoCallBack && sqlca->sqlemp->ereturncode == 0) {
    char szABAPInfo[2][UCHAR_MAX];
    const int cbNumElem = ((sizeof(szABAPInfo) / sizeof(szABAPInfo[0])));
    int ret;
    ret = pr03ABAPInfoCallBack(szABAPInfo[0], szABAPInfo[1]);
    if (ret > 0) {
      tsp1_part_ptr part_ptr=NULL;
      struct SQLERROR *sqlemp = sqlca->sqlemp;
      tsp1_packet    *pck = (tsp1_packet *) gae->gareqptr;
      if (pck) {
        int i;
        unsigned char len=0;
        for(i=0; i < cbNumElem; i++)
          len+=(unsigned char)strlen(szABAPInfo[i])+1;
        if(s26size_new_part(pck, sqlra->rasegptr) > len)
          s26new_part_init (pck, sqlra->rasegptr, &part_ptr);
	if (part_ptr) {
	  int buf_len = 0;
	  tsp1_part_header *ph=&part_ptr->sp1p_part_header;
	  ph->sp1p_part_kind = sp1pk_abap_info;
	  ph->sp1p_arg_count = 0;
	  for(i=0; i < cbNumElem; i++) {
	    len = (unsigned char)strlen(szABAPInfo[i]);
	    if (len > 0) {
	      pr03AbapInfoTrace(sqlra, szABAPInfo[i], i);
	      if (ph->sp1p_buf_len + len > ph->sp1p_buf_size) {
		p03cseterror (sqlemp, cpr_request_area_overflow);
		break;
	      }
	      part_ptr->sp1p_buf[ph->sp1p_buf_len] = len;
	      ph->sp1p_buf_len++;
	      memcpy(&part_ptr->sp1p_buf[ph->sp1p_buf_len], szABAPInfo[i], len);
	      ph->sp1p_buf_len += len;
	      ph->sp1p_arg_count++;
	    }
	  }
	  s26finish_part (pck, part_ptr);
	}
	else
	  if (sqlemp->eprerr != cpr_p_ok)
	    p03cseterror (sqlemp, cpr_reflex_start_required);
      }
      else
	if (sqlemp->eprerr != cpr_p_ok)
	  p03cseterror (sqlemp, cpr_reflex_start_required);
    }
    p03csqlemptosqlca (sqlca, sqlca->sqlemp);
  }
}

/*!
  Function: pr03AbapInfoPutPart

  description: Schreibt die von der Callbackfunktio ermittelten Werten
      in den Interface Trace.

  arguments:
      sqlra      [in] Request Area;
      szABAPInfo [in] Nullterminierter String mit AbapInfo
      nElem      [in] Index der ABAPInfo.

 */

void pr03AbapInfoTrace(sqlratype *sqlra, char *szABAPInfo, int nElem)
{
  sqltatype *tap = (sqlra) ? sqlra->rasqltap : NULL;
  tsp00_Int2 *ll = (tap) ? &tap->tastr80l : NULL;
  const char szStr[][sizeof(tsp00_Name)]={"DIAGNOSE INFO  : ",
					"         LINE  : ",
					"    UNKNOWN    : "};
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  strcpy(tap->tastr80, szStr[((nElem > 1) ? 2 : nElem)]);
  strncat(tap->tastr80, szABAPInfo, sizeof(tap->tastr80)-sizeof(tsp00_Name));
  tap->tastr80l=strlen(tap->tastr80);
  p08vfwritetrace (sqlra);
}


