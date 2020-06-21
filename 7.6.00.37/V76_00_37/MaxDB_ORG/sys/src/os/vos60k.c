/*!
  @file           vos60k.c
  @author         RaymondR
  @brief          kernel message output
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

\endif
*/




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "gos44.h"

# include "Messages/Msg_OutputKnlDiag.h"
/*
 *  DEFINES
 */
#define MOD__  "VOS60KC : "
#define MF__   MOD__"UNDEFINED"


/*
 *  MACROS
 */

/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

/*
 * EXTERNAL VARIABLES
 */

/*
 *  EXPORTED VARIABLES
 */

/*
 * LOCAL VARIABLES
 */

/*
 * LOCAL FUNCTION PROTOTYPES
 */

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */


VOID  vopmsg ( INT2 prio,
               INT2 msg_id,
               INT4 npar1,
               INT4 npar2,
               C64  spar,
               C64  msgline )
  {
  #undef  MF__
  #define MF__ MOD__"vopmsg"
  C64C            szSPar;
  CHAR            szMsgLine  [MAX_MSG_LINE_LEN];
  CHAR            szMsgPara  [sizeof (C64C)];
  ULONG           uMsgParaLen;
  PUKT_CTRL_REC   pUKT = THIS_UKT_CTRL;

  DBGIN;


  pUKT->pCTask->TaskState = TSK_VOPMSG_EO00 ;

  // --- output is the formatted message line.
  eo46PtoC (szMsgLine , msgline , sizeof (C64));
  eo46PtoC (szSPar, spar, sizeof (C64));

  *szMsgPara  = '\0';
  uMsgParaLen = 0;

  if (npar1)
    {
    ltoa (npar1, szMsgPara, 10);
    uMsgParaLen += (ULONG)strlen(szMsgPara);
    szMsgPara [uMsgParaLen++] = ' ';
    szMsgPara [uMsgParaLen]   = '\0';
    }
  if (npar2)
    {
    ltoa (npar2, szMsgPara + uMsgParaLen, 10);
    uMsgParaLen += (ULONG)strlen(szMsgPara + uMsgParaLen);
    szMsgPara [uMsgParaLen++] = ' ';
    szMsgPara [uMsgParaLen]   = '\0';
    }

  if ((*szSPar) && ( strcmp ( szMsgLine, szSPar ) ))
    {
    strcat (szMsgPara, szSPar);
    }

  strcat (szMsgLine, " ");
  strcat (szMsgLine, szMsgPara);
#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msg_id,INFO_TYPE,"KERNEL",szMsgLine);
#endif
#ifndef NO_OLD_DIAG_FILES
  sql60_msg_prio_x (prio, msg_id, INFO_TYPE, "", szMsgLine);
#endif

  pUKT->pCTask->TaskState = TSK_RUNNING_EO00 ;

  DBGOUT;
  return;
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

/*
 * =============================== END ========================================
 */
