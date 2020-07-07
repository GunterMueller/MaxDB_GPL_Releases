/*!
  @file           veo600.c
  @author         JoegM
  @special area   Unicode Kernel LZU Extensions
  @brief          Contains all unicode extensions of kernel LZU needed
  @List of implemented functionsvmessageUnicode Unicode version of vmessage (veo60k.c)

  vopmsgUnicode Unicode version of vopmsg (vos60kc,ven60c)
  @first created  000-08-04  10:25
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00_2.h"
#include "geo00_0.h"
#include "geo50_0.h"
#include "heo60_2.h"
#include "heo600.h"
#include "heo602.h"
#include "Messages/Msg_OutputKnlDiag.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef KERNEL_LZU
#  if defined(_WIN32)
#  define CURR_TASK_STATE (THIS_UKT_CTRL)->pCTask->TaskState
#  else
#  define CURR_TASK_STATE (THIS_UKT_CTRL)->curr_task->state
#  endif
#endif /* KERNEL_LZU */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

void vopmsgUnicode( 
              tsp00_Int4    msgEncoding,
              tsp00_Int2    msgPrio ,
              tsp00_Int2    msgNo ,
              tsp00_C132    msgText ) /* doubled size to allow UCS2 'C64' strings */
                          /* since C128 was not available, Elke said use C132 ;-) */
{
#undef  MF__
#define MF__ MOD__"vopmsgUnicode"

  tsp00_Byte msgTextUTF8[ sizeof ( tsp00_C80 ) + 4 ] ; /* UTF8 buffer */

  DBGIN;

  CURR_TASK_STATE = TSK_VOPMSG_EO00;

  eo602_ConvertPascalStringToUTF8Unicode ( msgEncoding , 
                                           msgText ,
                                           sizeof(tsp00_C80),
                                           msgTextUTF8,
                                           sizeof(msgTextUTF8) );

#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msgNo,sp3m_info,"KERNEL",(const char *)msgTextUTF8);
#endif
#ifndef NO_OLD_DIAG_FILES
  eo60k_Message( (tsp3_priority)msgPrio, sp3m_info, msgNo, "", (const char *)msgTextUTF8 );
#endif

  CURR_TASK_STATE = TSK_RUNNING_EO00;

  DBGOUT;
}

/*---------------------------------------------------------------------------*/

void vmessageUnicode( 
              tsp00_Int4    msgEncoding,
              tsp3_priority msgPrio ,
              tsp3_msg_type msgType ,
              tsp00_Int4    msgNo ,
              tsp00_C8      msgLabel , /* Normal size (always 7-Bit ASCII == UTF8) */
              tsp00_C80     msgText ) /* doubled size to allow UCS2 'C40' strings */
{
#undef  MF__
#define MF__ MOD__"vmessageUnicode"

  tsp00_Byte msgLabelUTF8[ sizeof ( tsp00_C8 ) + 4 ] ; /* UTF8 buffer */
  tsp00_Byte msgTextUTF8[ sizeof ( tsp00_C80 ) + 4 ] ; /* UTF8 buffer */

  DBGIN;

  CURR_TASK_STATE = TSK_VOPMSG_EO00;

  eo602_ConvertPascalStringToUTF8Unicode ( msgEncoding , 
                                           msgText ,
                                           sizeof(tsp00_C80),
                                           msgTextUTF8,
                                           sizeof(msgTextUTF8) );

  /* ASCII-7 is identical to UTF8, so no conversion is enforced... */
  /* If the kernel gave us bad characters here, it is not detected... */
  eo602_ConvertPascalStringToUTF8Unicode ( csp_utf8 ,
                                           msgLabel ,
                                           sizeof(tsp00_C8) ,
                                           msgLabelUTF8 ,
                                           sizeof(msgLabelUTF8) );

#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msgNo,msgType,(char *)msgLabelUTF8,(char *)msgTextUTF8);
#endif
#ifndef NO_OLD_DIAG_FILES
  eo60k_Message( msgPrio, msgType, msgNo, (const char *)msgLabelUTF8, (const char *)msgTextUTF8 );
#endif
  CURR_TASK_STATE = TSK_RUNNING_EO00;

  DBGOUT;
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
