/*



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





*/
#/*!================================================================
 module:    vpi10ConSync.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Connection Syncronisation
 
 description: Connect Verwaltung
 
 see:  
 
 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpi10ConnSync.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static boolean pi10IsSessionBusy(sqlgapointer sqlgap);
static enum tpr_cmd_state_Enum pi10SetConnectionState(sqlgapointer sqlgap, enum tpr_cmd_state_Enum state);
static enum tpr_cmd_state_Enum pi10GetConnectionState(sqlgapointer sqlgap);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

void 
pi10EnterSession(
sqlgapointer sqlgap)
{
  PR07_HEAPCHECK(NULL);
  if (!pi10IsSessionBusy(sqlgap))
    pi10SetConnectionState(sqlgap, CPR_CMD_STATE_START);
}

boolean 
pi10TryEnterSession(
sqlgapointer sqlgap)
{
  PR07_HEAPCHECK(NULL);
  if (pi10IsSessionBusy(sqlgap))
    return FALSE;
  else
    pi10SetConnectionState(sqlgap, CPR_CMD_STATE_START);
  return TRUE;
}

void 
pi10LeaveSession(
sqlgapointer sqlgap)
{
  if (pi10IsSessionBusy(sqlgap))
    pi10SetConnectionState(sqlgap, CPR_CMD_STATE_READY);
  PR07_HEAPCHECK(NULL);
}

static boolean 
pi10IsSessionBusy(
sqlgapointer sqlgap)
{
  switch(pi10GetConnectionState(sqlgap)) {
  case (CPR_CMD_STATE_START):
    return TRUE;
  }
  return FALSE;
}

static enum tpr_cmd_state_Enum 
pi10SetConnectionState(
sqlgapointer sqlgap,
enum tpr_cmd_state_Enum state)
{
  if (sqlgap) {
    enum tpr_cmd_state_Enum oldstate = CPR_CMD_STATE_INIT;
    oldstate = sqlgap->gaCmdState;
    sqlgap->gaCmdState = state;
    return oldstate;
  }    
  return CPR_CMD_STATE_INIT;  
}

static enum tpr_cmd_state_Enum 
pi10GetConnectionState(
sqlgapointer sqlgap)
{
  if (sqlgap) {
    return sqlgap->gaCmdState;
  }    
  return CPR_CMD_STATE_INIT;  
}
