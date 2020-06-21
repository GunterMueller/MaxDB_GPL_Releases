/*!================================================================
 module:    gmt90.h

 responsible: BurkhardD
 
 special area:   Traceoutput 
 description: 
 
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

#ifndef __GMT90_H__
#define __GMT90_H__
#if !defined M90_INC   /* Don't reinclude */
#define M90_INC
#ifndef RC_INVOKED
#if defined (__cplusplus)
extern "C"
{
#endif
extern int m90_trace_status;
#if defined (__cplusplus)
}
#endif

#define M90_TR_STRING  (1)
#define M90_TR_UCHAR   (2)
#define M90_TR_SCHAR   (3)
#define M90_TR_DWORD   (4)
#define M90_TR_SDWORD  (5)
#define M90_TR_UDWORD  (6)
#define M90_TR_WORD    (7)
#define M90_TR_SWORD   (8)
#define M90_TR_UWORD   (9)
#define M90_TR_SDOUBLE (10)
#define M90_TR_LDOUBLE (11)
#define M90_TR_SFLOAT  (12)
#define M90_TR_BOOL    M90_TR_WORD
#define M90_TR_BUFFER  (13)
#define M90_TR_HANDLE  (14)
#define M90_TR_PTR     (15)
#define M90_TR_INT     (16)
#define M90_TR_ENUM    (17)

#define M90_TR_ENTRY   (50)
#define M90_TR_EXIT    (51)
#define M90_TR_MSG     (52)

#define M90_TR_STRING_LEN (53)

#define M90_TRACERESET 	(0)
#define M90_TRACEON    	(1)
#define M90_TRACEOFF	(2)
#define M90_TRACEINIT   (3)
#define M90_TRACEDIALOG	(88)


/* special defines for M90_TR_STRING_LEN */ 

#define M90_NTS -3
#define M90_NULL_DATA -1

#define M90_TRACE_PREFIX "      "
#define M90_TRACE_MAXCHAR 20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if !defined WIN32
#ifndef SAPDB_FAST
#define M90_TRACE_OUTPUT(buffer)\
   if (mt90level <= m90maxlevel) {\
     fprintf(file_ptr,"%*c",(int) (mt90level > 20) ? 20 : mt90level,' ');\
     fprintf(file_ptr,"%s", buffer);\
     fflush(file_ptr);\
   }
#define M90_EXIT(x) exit(x)
#define M90_EXIT_MSG(msg) {printf(" %s\n", msg); exit(16);}
#else
#define M90_TRACE_OUTPUT(buffer)
#define M90_EXIT(x)
#define M90_EXIT_MSG(msg)
#endif

#define M90CRLF "\x0A"
   
#else

#include <windows.h>

typedef struct {
   int version;
   HWND hwnd;
   int status;
   char FileName[260];
   int TraceLevel;
} mt90TraceStatus;


#ifndef SAPDB_FAST
#define M90_TRACE_OUTPUT(xbuffer) mt90TraceOutPut(& file_handle, xbuffer)
#define M90_EXIT(x) FatalAppExit(0,"API_EXIT Called.") 
#define M90_EXIT_MSG(msg) FatalAppExit(0,msg);
#else
#define M90_TRACE_OUTPUT(buffer)
#define M90_EXIT(x)
#define M90_EXIT_MSG(msg)
#endif
   
#define M90CRLF "\x0D\x0A"

unsigned int  m90ctropt();
BOOL PASCAL mt90ProcDebug(HWND, unsigned, WORD, LONG );
void mt90TraceSwitch(int Level);

#endif /* WIN32 */

#define M90_TR_INIFILE   "sql-db.ini"
#define M90_TR_FILENAME_ENTRY "LogFile"
#define M90_TR_LEVEL_ENTRY "TraceLevel"
#define M90_TR_BUFFERSIZE_ENTRY "BufferSize"
#define M90_TR_PROFENTRY "m90trace"
#define M90_TR_APPNAME "SAPDB Tech Trace Interface"

#define M90_MEMCPY memcpy
#define M90_MEMSET memset
#define M90_MEMCMP memcmp
#define M90_STRCPY strcpy
#define M90_STRNCPY strncpy
#define M90_STRCMP strcmp
#define M90_STRLEN strlen
#define M90_STRCAT strcat
#define M90_STRNCAT strncat
#define M90_STRCHR strchr
#define M90_STRRCHR strrchr
#define M90_SPRINTF sprintf
#define M90_NEAR(p) p


#define M90_TR_FILENAME "trace.log"

#if defined (__cplusplus)
extern "C"
{
#endif
void m90ctrac( unsigned short int type,
char * name,
void * ptr,
long int len,
long int pos 
);

#ifndef SAPDB_FAST  
#define M90TRACEINIT(status);\
	  {m90_trace_status = status;\
	     m90ctrac(0, 0, NULL, (long int) 0, (long int) 0);}

#define M90TRACE(type,name,ptr);\
	  {if (m90_trace_status == 1)\
	      m90ctrac(type, name, (void *) ptr, (long int) 0, (long int) 0);}
#define M90TRACELEN(type,name,ptr,len);\
	  {if (m90_trace_status == 1)\
	      m90ctrac(type, name, (void *) ptr, (long int) len, (long int) 0);}
#define M90TRACEBUF(name, ptr, pos, len);\
	  {if (m90_trace_status == 1)\
	      m90ctrac(M90_TR_BUFFER, name, (void *) ptr, (long int) len, (long int) pos);}
#define M90TRACEIF(type,name,expr,ptr);\
	  {if (m90_trace_status == 1 && expr)\
	      m90ctrac(type, name, (void *) ptr, (long int) 0, (long int) 0);}
	  
#define M90TRACELENIF(type,name,expr, ptr,len);\
	  {if (m90_trace_status == 1 && expr)\
	      m90ctrac(type, name, (void *) ptr, (long int) len, (long int) 0);}
#else
#define M90TRACEINIT(status);
#define M90TRACE(type,name,ptr);
#define M90TRACELEN(type,name,ptr,len);
#define M90TRACEBUF(name,ptr,pos,len);
#define M90TRACEIF(type,name,expr,ptr);
#define M90TRACELENIF(type,name,expr,ptr,len);
#endif
#if defined (__cplusplus)
}
#endif
#endif 

#define M90_TRACEFILE               101
#define M90_TRACELEVEL              102
#define M90_APPLICATION             103
#define IDC_STATIC                  -1
#define DEBUGBOX                    9001
#endif
#endif /* __GMT90_H__ */
