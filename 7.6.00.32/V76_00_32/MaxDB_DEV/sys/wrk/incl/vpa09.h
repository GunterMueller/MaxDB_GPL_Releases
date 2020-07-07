/*



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
#ifndef VPA09_H
#define VPA09_H
/*!**********************************************************************

  module: vpa09.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  suppport for asynchron functions

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa40DBC.h"
#include "vpa60Stmt.h"

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

#define PA09_WAIT_FOR_SEMAPHORE    1
#define PA09_RELEASE_SEMAPHORE     2
#define PA09_SEMAPHORE_SQLCANCEL   10
#define PA09_SEMAPHORE_WRITETRACE  11
#define PA09_SEMAPHORE_MTHANDLER   12
#define PA09_SEMAPHORE_ADD         13
#define PA09_SEMAPHORE_CONNECT     14
#define PA09_ADD                   100
#define PA09_SUB                   101
#define PA09_CONNECT_BEGIN         102
#define PA09_WAIT_FOR_BEGIN        103
#define PA09_CONNECT_END           104

#define API_ASYNC_INIT		(0)
#define API_ASYNC_START		(1)
#define API_ASYNC_READY   	(2)
#define API_ASYNC_INFLIGHT	(3)
#define API_ASYNC_END   	(4)

VOID  pa09PrepareAsyncCall( SQLHSTMT lParam, UWORD function_code);
VOID  pa09ExecuteAsyncCall( SQLHSTMT hstmt);
SWORD pa09CreateThread( SQLHSTMT hstmt);
SWORD pa09TerminateThread( SQLHSTMT hstmt);
VOID pa09LeaveAsyncFunction( VOID );
void pa09SaveStack( API_HANDLE *hand, PTR lpValues, DWORD cbSize);
VOID pa09RestoreStack( API_HANDLE hand, PTR lpValues);
DWORD pa09IsCanceled( DWORD reference);
void pa09Semaphore (int sema, int action);       /* PTS 1120833 */
int  pa09MTHandler (int action);
#ifdef ASYNCENABLED
VOID  pa09AsyncRetcode( SQLHSTMT hstmt, RETCODE retcode);
RETCODE SQL_API pa09ThreadProc( SQLHSTMT hstmt);
VOID pa09SuspendThread( teo07_Thread ThreadId);
VOID pa09ResumeThread( teo07_Thread ThreadId);

VOID  pa09ProtectThread ();
VOID  pa09UnProtectThread ();
SWORD pa09WaitEndOfThread (SQLHSTMT);
#endif


#define PA09ENTERASYNCFUNCTION(d, s) pa09EnterAsyncFunction(d, s)

#define PA09LEAVEASYNCFUNCTION() pa09LeaveAsyncFunction()

VOID pa09SetAsyncLocals( tpa40DBC  *dbc_block_ptr,
                         tpa60Stmt *stmt_block_ptr );

VOID pa09EnterAsyncFunction( tpa40DBC  *dbc_block_ptr,
                             tpa60Stmt *stmt_block_ptr );

BOOL pa09IsConnectionBusy( tpa60Stmt *stmt_block_ptr,
                           tpa40DBC  *dbc_block_ptr );

DWORD pa09IsAsync( tpa60Stmt *stmt_block_ptr,
                   tpa40DBC  *dbc_block_ptr,
                   RETCODE   *retcode );

void pa09DeleteCriticalSection ();

/* initialize multithread function in LZU
   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124459 */
void pa09InitMultiThreading (tsp00_ErrTextc errTxt, teo07_ThreadErr *ok);


/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif
