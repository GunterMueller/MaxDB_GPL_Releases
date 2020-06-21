/*


    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "idl/idl2xml/pms00.h"
#include "idl/idl2xml/pmsmsg.h"
#include "idl/idl2xml/pmser.h"

#define M__NAME "PMSER"

#define MSG_HEAD_LEN 39
/* total message length: header + message + line feed */
#define MSG_TOT_LEN  (MSG_HEAD_LEN + MSG_LEN + 1)

static char     sev_char [5] = {'S','I','W','E','F'}; 
static char     mon[12][4]   = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"}; 

typedef struct tm *st_tm_p;


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section of External Routines                                    */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Routine Section                                                            */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/******************************************************************************/
/* Prepare message text                                                       */
/******************************************************************************/

void PmsMsgPrep
(
  char                 *msgtxt,
  unsigned              msg_id,
  char                  modname[5],
  int                   line_no,
  severity_code         s_code
)
{
  char          *fac_p;
  char          *cod_p;
  time_t        tim_in_sec;
  st_tm_p       tim_p;


  tim_in_sec = time (0);
  tim_p = localtime (&tim_in_sec);
  fac_p = (char (*)) modname;
  cod_p = (char (*)) modname + 3;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* the header consists of:                                                    */
/*                                                                            */
/* facility code                                                              */
/* severity code                                                              */
/* message name                                                               */
/* module ID                                                                  */
/* line no. of error message in module                                        */
/* time in hours, minutes, and seconds                                        */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* with time */
  sprintf (msgtxt,"%.3s-%c-%.6s %.2s%04d %02d-%s-%02d %02d:%02d:%02d ",
                   fac_p,
                        sev_char[s_code],
                           MsgLst[msg_id],
                                cod_p,
                                    line_no,
                                         tim_p->tm_mday,
                                              mon[tim_p->tm_mon],
                                                   tim_p->tm_year-100,
                                                        tim_p->tm_hour,
                                                             tim_p->tm_min,
                                                                  tim_p->tm_sec);
  
/* without time 
  sprintf (msgtxt,"%.3s-%c-%.6s %.2s%04d ",
                   fac_p,
                        sev_char[s_code],
                           MsgLst[msg_id],
                                cod_p,
                                    line_no);
  strncpy (&msgtxt[MSG_HEAD_LEN - 8], MsgLst[msg_id]+7, MSG_LEN-7);
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* add message text                                                           */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

  strncpy (&msgtxt[MSG_HEAD_LEN], MsgLst[msg_id]+7, MSG_LEN-7);

} /* END OF PmsMsgPrep */


/******************************************************************************/
/* Print out error message                                                    */
/******************************************************************************/

void PmsMsg
(
  unsigned short        msg_id,
  char                  modname[5],
  int                   line_no,
  severity_code         s_code,
  unsigned short        arg_ct,
  char                 *msg_line,
  ...
)
{
  char           msg_text[pmc_mxerrline];

  /* FJH 29-04-1998
     makes no sense while arguments are checked against pmc_mxerrline
     char           msg_text2[pmc_mxerrline / 2]; */

  char           msg_text2[pmc_mxerrline * 10];
  va_list        ap;
  unsigned short lg;
  
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* prepare message text                                                       */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

  lg = strlen (msg_line);
  strcpy (msg_text, msg_line);
  
  PmsMsgPrep (msg_line, msg_id, modname, line_no, s_code);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* get variable parameters                                                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
  
  va_start (ap, msg_line);

  vsprintf (msg_text2, msg_line, ap);

  va_end (ap);
 
  strcpy (msg_line, msg_text);
  
  if ( (lg + strlen (msg_text2) + 1) < pmc_mxerrline)
    {
	 if (lg > 0) strcat (msg_line, "\n");
	 strcat (msg_line, msg_text2);
    }
  else
    {
     if ((lg + 40) < pmc_mxerrline)
	   {
	    strcat (msg_line, "\n");
        sprintf ( &msg_line[lg + 1], 
			     "Message '%.12s' does not fit", msg_text2);
	   }
    }

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* print out with parameters                                                  */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

}


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* End of Module                                                              */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/





