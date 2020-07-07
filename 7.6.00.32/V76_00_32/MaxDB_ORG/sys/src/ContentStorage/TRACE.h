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
#ifndef XTRACE_H
#define XTRACE_H

#define NOXTRACE
#ifdef NOXTRACE
//***************************************************************************

#define XTRACE(str) 
#define XTRACE2(str1,str2) 
#define XTRACE3(str1,str2,str3) 
#define XTRACE4(str1,str2,str3,str4)
#define XTRACEI1(str1,i1)
#define XTRACEI2(str1,i1,i2)
#define XTRACEI3(str1,i1,i2,i3) 
#define XTRACEI4(str1,i1,i2,i3,i4) 
#define XTRACEI5(str1,i1,i2,i3,i4,i5) 
#define XTRACEFMT1(str,fmt,a1) 
#define XTRACEFMT2(str,fmt,a1,a2)
#define XTRACEFMT3(str,fmt,a1,a2,a3)
#define XTRACEFMT4(str,fmt,a1,a2,a3,a4) 

#define XTRACE_REPDEFINE() 
#define XTRACE_MEMORYLEAKS()	
#define XTRACE_CHECKMEMORY()

//***************************************************************************

#else
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>

//***************************************************************************
#ifdef WIN32
#include <crtdbg.h>
#endif
#define XTRACEC(s)	((s)?(s):"(null)")
#define XTRACESTMP(buf) \
{\
	SYSTEMTIME st;\
	::GetSystemTime(&st);\
	sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d ",(int)st.wYear,(int)st.wMonth,(int)st.wDay,(int)st.wHour,(int)st.wMinute,(int)st.wSecond);\
}

#ifndef XTRACE_FILE
#define XTRACE_FILE __FILE__
#endif

#define XTRACEFILE(s,f) \
				{	FILE* _trace_fh=fopen("ContentStorageTrace.txt","a+");if(_trace_fh){\
					char _trace_tstmp[60]; XTRACESTMP(_trace_tstmp);\
					fwrite(_trace_tstmp,strlen(_trace_tstmp),1,_trace_fh);\
					fwrite(XTRACEC(f),strlen(XTRACEC(f)),1,_trace_fh);\
					fwrite("\t\t",2,1,_trace_fh);\
					fwrite(XTRACEC(s),strlen(XTRACEC(s)),1,_trace_fh);\
					fwrite("\n",1,1,_trace_fh);\
					fclose(_trace_fh);};\
				}

#define XTRACE(str) \
				{	XTRACEFILE(XTRACEC(str),XTRACE_FILE);}
#define XTRACE2(str1,str2) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+strlen(XTRACEC(str2))+100); \
					sprintf(_trace_buf,"%s %s",XTRACEC(XTRACEC(str1)),XTRACEC(str2)); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACE3(str1,str2,str3) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+strlen(XTRACEC(str2))+strlen(XTRACEC(str3))+100); \
					sprintf(_trace_buf,"%s %s %s",XTRACEC(str1),XTRACEC(str2),XTRACEC(str3)); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACE4(str1,str2,str3,str4) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+strlen(XTRACEC(str2))+strlen(XTRACEC(str3))+strlen(XTRACEC(str4))+100); \
					sprintf(_trace_buf,"%s %s %s %s",XTRACEC(str1),XTRACEC(str2),XTRACEC(str3),XTRACEC(str4)); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACEI1(str1,i1) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+100); \
					sprintf(_trace_buf,"%s %d",XTRACEC(str1),i1); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACEI2(str1,i1,i2) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+100); \
					sprintf(_trace_buf,"%s %d %d",XTRACEC(str1),i1,i2); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACEI3(str1,i1,i2,i3) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+100); \
					sprintf(_trace_buf,"%s %d %d %d",XTRACEC(str1),i1,i2,i3); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACEI4(str1,i1,i2,i3,i4) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+100); \
					sprintf(_trace_buf,"%s %d %d %d %d",XTRACEC(str1),i1,i2,i3,i4); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACEI5(str1,i1,i2,i3,i4,i5) \
				{ 	char* _trace_buf=(char*)malloc(strlen(XTRACEC(str1))+100); \
					sprintf(_trace_buf,"%s %d %d %d %d %d",XTRACEC(str1),i1,i2,i3,i4,i5); \
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);}
#define XTRACEFMT1(str,fmt,a1) \
				{ 	char* _trace_buf=(char*)malloc(50000),_trace_buf2=(char*)malloc(50000); \
					sprintf(_trace_buf2,fmt,a1); sprintf(_trace_buf,"%s %s",str,_trace_buf2);\
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);free(_trace_buf2);}
#define XTRACEFMT2(str,fmt,a1,a2) \
				{ 	char* _trace_buf=(char*)malloc(50000),_trace_buf2=(char*)malloc(50000); \
					sprintf(_trace_buf2,fmt,a1,a2); sprintf(_trace_buf,"%s %s",str,_trace_buf2);\
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);free(_trace_buf2);}
#define XTRACEFMT3(str,fmt,a1,a2,a3) \
				{ 	char* _trace_buf=(char*)malloc(50000),_trace_buf2=(char*)malloc(50000); \
					sprintf(_trace_buf2,fmt,a1,a2,a3); sprintf(_trace_buf,"%s %s",str,_trace_buf2);\
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);free(_trace_buf2);}
#define XTRACEFMT4(str,fmt,a1,a2,a3,a4) \
				{ 	char* _trace_buf=(char*)malloc(50000),_trace_buf2=(char*)malloc(50000); \
					sprintf(_trace_buf2,fmt,a1,a2,a3,a4); sprintf(_trace_buf,"%s %s",str,_trace_buf2);\
					XTRACEFILE(_trace_buf,XTRACE_FILE);free(_trace_buf);free(_trace_buf2);}


#define XTRACE_REPDEFINE() \
				{  _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );\
				   _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );\
				   _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );\
				   _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );\
				   _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );\
				   _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );\
				   _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)\
					   | _CRTDBG_ALLOC_MEM_DF \
					   | _CRTDBG_DELAY_FREE_MEM_DF \
					  /* | _CRTDBG_CHECK_ALWAYS_DF*/); \
				   XTRACE_SETHOOK();\
				}

int XTRACE_REPORTHOOK( int reportType, char *userMessage, int *retVal );
inline int XTRACE_REPORTHOOK( int reportType, char *userMessage, int *retVal )
{
	XTRACEFILE(userMessage,XTRACE_FILE);
	retVal = 0;
	return false;
}

#define XTRACE_CHECKMEMORY()	{ XTRACE_REPDEFINE();_CrtCheckMemory( );}
#define XTRACE_MEMORYLEAKS()	{ XTRACE_REPDEFINE();_CrtDumpMemoryLeaks( );}
#define	XTRACE_SETHOOK()		{ _CrtSetReportHook( XTRACE_REPORTHOOK );}

//***************************************************************************
#endif

#endif
