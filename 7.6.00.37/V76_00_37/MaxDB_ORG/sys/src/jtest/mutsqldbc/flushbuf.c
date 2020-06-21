/*
 * @(#) flushbuf.c	2005-03-02



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

#include <stdio.h>
#include "RunTime/System/RTESys_MSWindows.h" /*nocheck*/
//#include <windows.h>

#define UNDEFINED_HANDLE_VALUE	(HANDLE)0
#define ESCAPE_CHARACTER	'\033'
#define ESCAPE_BRACKET		'['
#define MUT_SHARE_CONSOLE       "MUT_Share_Console"

static	CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo ;
static	HANDLE hBuffer = UNDEFINED_HANDLE_VALUE ;
static  HANDLE hMutex  = NULL ;

#include "jtest/mutsqldbc/flushbuf.h" 
/*------------------------------------------------------------------------*/

DWORD	sflushf ( char * szBuffer )
{
DWORD	dwCode , dwSend , dwSize , dwNext , dwLast = 0 ;

  if ( hBuffer == UNDEFINED_HANDLE_VALUE )
  {
    if ( hMutex == NULL )
      hMutex = CreateMutex ( NULL , FALSE , MUT_SHARE_CONSOLE );
    if ( hMutex == NULL )
      return ( GetLastError () ) ;
    hBuffer = CreateFile ( "CONOUT$" , GENERIC_READ | GENERIC_WRITE ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE , NULL ,
                           OPEN_EXISTING , 0 , NULL );
#ifdef DEBUG
    fprintf ( stderr , "Bufferhandle : %d\n" , hBuffer ) ;
#endif
    if ( hBuffer == INVALID_HANDLE_VALUE )
      return ( GetLastError () ) ;
  }

  if ( WaitForSingleObject ( hMutex , INFINITE ) == WAIT_FAILED )
    return ( GetLastError () ) ;
  dwSize = strlen ( szBuffer ) ;
  while ( dwLast < dwSize )
  {
    for ( dwNext = dwLast ; dwNext < dwSize ; dwNext ++ )
      if ( szBuffer[dwNext] == ESCAPE_CHARACTER ) break ;

    dwSend = dwNext - dwLast ;
    if ( dwSend > 0 )
    {
      if ( !WriteConsole ( hBuffer , &szBuffer[dwLast] ,
                           dwSend , &dwCode , NULL ) )
        return ( GetLastError () ) ;
#ifdef DEBUG
      fprintf ( stderr , "Write : %d , Size : %d\n" , dwSend , dwCode ) ;
#endif
      dwLast += dwCode ;
    }

    dwNext = dwLast ;
    if ( szBuffer[dwNext++] == ESCAPE_CHARACTER )
      dwLast = dwNext ;
    else continue;
    if ( szBuffer[dwNext++] == ESCAPE_BRACKET )
    switch ( szBuffer[dwNext] )
    {
      char  szText[16] ;
      COORD  xyPosition ;
    case 'K' :
      if ( !GetConsoleScreenBufferInfo ( hBuffer , &ConsoleInfo ) )
        return ( GetLastError () ) ;

      if ( !FillConsoleOutputCharacter (
                hBuffer , ' ' , (DWORD)ConsoleInfo.dwSize.X ,
                ConsoleInfo.dwCursorPosition , &dwCode ) )
        return ( GetLastError () ) ;
#ifdef DEBUG
        fprintf ( stderr , "Clear : %d , Size : %d\n" ,
                  ConsoleInfo.dwSize , dwCode ) ;
#endif
      dwLast = ++dwNext ;
      break;
    case '0' :
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
      dwCode = 0 ;
      do szText[dwCode++] = szBuffer[dwNext++] ;
      while ( szBuffer[dwNext] >= '0' &&
        szBuffer[dwNext] <= '9' ) ;
      if ( szBuffer[dwNext++] != ';' ) break ;

      szText[dwCode] = '\0' ;
      xyPosition.Y = (short)(atoi ( szText ) - 1);

      dwCode = 0 ;
      do szText[dwCode++] = szBuffer[dwNext++] ;
      while ( szBuffer[dwNext] >= '0' &&
        szBuffer[dwNext] <= '9' ) ;
      if ( szBuffer[dwNext++] != 'H' ) break ;

      szText[dwCode] = '\0' ;
      xyPosition.X = (short)(atoi ( szText ) - 1);

      if ( !SetConsoleCursorPosition ( hBuffer , xyPosition ) )
        return ( GetLastError () ) ;
      dwLast = dwNext ;
    }
  }
  if ( !ReleaseMutex ( hMutex ) )
    return ( GetLastError () ) ;
  return ( 0 ) ;
}