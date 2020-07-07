/*!================================================================
 module:    vpr03CheckPoint.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | APO
 
 description: Kernel CheckPoint Unterstuetzung von APO
 
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
#include "vpr03CheckPoint.h"
#include "gpr03.h"
#include "vpr03sc.h"
#include "gpr08.h"
#include <limits.h>

/*==================================================================*
 *  DIRTY DECLARATIONS                                              *
 *==================================================================*/

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

static SQLCheckPointProc *pr03CheckPointCallBack = NULL;

/*==================================================================*
 *  EXPORTED FUNCTIONS                                                      *
 *==================================================================*/

externC void 
pr03CheckPointPutPart(sqlcatype *sqlca, sqlgaentry *gae, sqlratype *sqlrap);

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

static void pr03CheckPointTrace(sqlratype *sqlrap, int nCheckPoint);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function: sqlInitCheckPoint 

  description: Teilt der Interface Runtime die Adresse einer Callbackfunktion
      mit, die jeweils vor dem Execute eines SQL Kommandos aufgerufen wird.
      Die Callbackfunktion liefert einen Werte (0 | != 0) ob das schreiben 
      eines CheckPoints beim Kern veranlasst werden soll  
      
  arguments:
      sqlCheckPointCallBack  [in]  Zeiger auf Callbackfunktion

  returns: 
      Die Adresse, auf der zuletzt gesetzten Callbackfunktion.
 */

SQLCheckPointProc *
sqlInitCheckPoint(SQLCheckPointProc *sqlCheckPointCallBack)
{
  SQLCheckPointProc *pOld = pr03CheckPointCallBack;
  pr03CheckPointCallBack = sqlCheckPointCallBack;
  return(pOld);
}

/*!
  Function: pr03CheckPointPutPart 

  description: Fuellt den abap_info_part mit den mittels der Callbackfunktion
      ermittelten ABAP Namen und der Zeilennummer. Die Callbackfunktion
      muss vorher mittels Callbackfunktion sqlInitCheckPoint gesetzt worden 
      sein. 
      Der Aufruf der Callbackfunktion ist in einen Exceptionhandle gekapselt,
      so das scheitern dieses Calls zur Fortfuehrung des Programmes und dem
      setzten des sqlcode auf ABAP_EXCEPTION fuehrt.
      
  arguments:
      sqlca, sqlrap

 */

void 
pr03CheckPointPutPart(sqlcatype *sqlca, sqlgaentry *gae, sqlratype *sqlrap)
{
  if (pr03CheckPointCallBack && sqlca->sqlemp->ereturncode == 0) {
    int CheckPoint;
    CheckPoint = pr03CheckPointCallBack();
    pr03CheckPointTrace(sqlrap, CheckPoint);
    if (CheckPoint ==  sqlNoWaitCheckPoint) {
      tsp1_part_ptr part_ptr=NULL;
      struct SQLERROR *sqlemp = sqlca->sqlemp;
      tsp1_packet    *pck = (tsp1_packet *) gae->gareqptr;
      if (pck) {
        int len = sizeof(part_ptr->sp1p_buf[0]);
        if(s26size_new_part(pck, (tsp1_segment*)sqlrap->rasegptr) > len)
          s26new_part_init (pck, (tsp1_segment*)sqlrap->rasegptr, &part_ptr);
	if (part_ptr) {
	  tsp1_part_header *ph=&part_ptr->sp1p_part_header;
	  ph->sp1p_part_kind = sp1pk_checkpoint_info;
	  ph->sp1p_arg_count = 1;
	  ph->sp1p_buf_len = 0;
	  len = sizeof(part_ptr->sp1p_buf[0]);
	  if (ph->sp1p_buf_len + len > ph->sp1p_buf_size) {
	    p03cseterror (sqlemp, cpr_request_area_overflow);
	  }
	  else {
	    part_ptr->sp1p_buf[ph->sp1p_buf_len] = 0x00;
	    ph->sp1p_buf_len += len;
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
  }
  p03csqlemptosqlca (sqlca, sqlca->sqlemp);
}

/*!
  Function: pr03CheckPointWait 

  description: Returns ever sqlWaitCheckPoint.
  
  arguments: none
  
*/

int pr03CheckPointWait()
{
  return sqlWaitCheckPoint; 
}

/*!
  Function: pr03CheckPointNoWait 

  description: Returns ever sqlNoWaitCheckPoint.
  
  arguments: none
  
*/

int pr03CheckPointNoWait()
{
  sqlInitCheckPoint(NULL);
  return sqlNoWaitCheckPoint; 
}

/*!
  Function: pr03CheckPointTrace 

  description: Schreibt die von der Callbackfunktion ermittelten Werten
  in den Interface Trace.
  
  arguments:
  sqlrap      [in] Request Area;
  nCheckPoint [in] schreibt TRUE (!=0) oder FALSE in den Trace 
  
*/

static void pr03CheckPointTrace(sqlratype *sqlrap, int nCheckPoint)
{
  sqltatype *tap = (sqlrap) ? sqlrap->rasqltap : NULL;
  tsp00_Int2 *ll = (tap) ? &tap->tastr80l : NULL;
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  {
    char *szStr = "CHECKPOINT INFO  : ";
    strcpy(tap->tastr80, szStr);
    if (nCheckPoint == sqlNoWaitCheckPoint) {
      strcat(tap->tastr80, "NO WAIT");
    }
    else {
      strcat(tap->tastr80, "WAIT");
    }
    tap->tastr80l=strlen(tap->tastr80);
    p08vfwritetrace (sqlrap);
  }
}


