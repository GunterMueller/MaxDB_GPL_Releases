/*!================================================================
 module:    vpr03m.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Memory Management
 
 description: Memory Management
              OS unabhaengige Speicherverwaltung mit Tracefunktionalitaet.
	      in der FASTen Variante. 
	      Die verwendeten Funktionen rufen die LZU Funktionen sqlallocat,
	      sqlfree und sqlreallocat oder 
	      malloc, free, reallocat wenn das MACRO USE_MALLOC gesetzt ist.
	      
	      Der Erzeugte Traceoutput ist von der Environmentvariablen 
	      SAPDBMEMCHECK=<filename> abhaengig. 
	      Ist diese gesetzt, so wird Traceoutput in die Datei "filename"
	      geschrieben.  
	      Zum Zeitpunkt des Tracefileopens wird mittels der ProcessID 
	      ein prozessspezifische Dateiname gebildet wenn der "filename"
	      einen numerischen Formatierungscharacter (%d, %p, oder %x) 
	      beinhaltet.
	      
	      Der Traceoutput hat die Form.

	      func,'name',size,addr und laesst sich fuer die weitere 
	      Bearbeitung in eine Tabelle hochladen.
	      Zu diesem Zweck enthaelt die Datei eine XLOAD Header, mittels 
	      dem die Tabellen angelegt und die Daten 
	      importiert werden koennen.
	      
	      create table mallcheck ( 
	               k char(8) byte default stamp, 
		       func char (18), 
		       name char(100), 
		       size int, 
		       addr char(10));

	      create view allocs as select * from mallcheck
	                                     where func = 'alloc';

	      create view frees  as select * from mallcheck
	                                     where func = 'free';

	      Mittels folgendem SELECT koennen die nicht freigegebenen Adressen
	      angezeigt werden.

	      select name, size, addr from allocs 
                    where k not in (SELECT distinct t1.k
		                    FROM   allocs t1, frees t2
                    		    WHERE  t1.addr = t2.addr
                      		    AND  t1.k < t2.k);

	      
	      
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

#include "gpr03m.h"
#include "hsp77.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

/*==================================================================*
 *  DEFINE                                                    *
 *==================================================================*/
#define ALIGNMENT 8
#define ALIGN(n) ((n % ALIGNMENT) == 0) ? n : (n + ALIGNMENT - (n % ALIGNMENT))

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
typedef enum tpr03m_IsTraceEnum { 
  IsInit_epr03m, IsOn_epr03m, IsOff_epr03m
} tpr03m_IsTraceEnum;

static tsp00_Pathc pr03mTraceFileName;
static tpr03m_IsTraceEnum pr03mIsTrace = IsInit_epr03m;

static char pr03mXLoadHeader[] = 
"/\n\
if $rc (exists table mallcheck) = 0\n\
then\n\
     begin\n\
/\n\
     drop table mallcheck\n\
/\n\
     end\n\
/\n\
create table mallcheck (\n\
	k char(8) byte default stamp,\n\ 
	func char (18),\n\
       	name char(100),\n\
	size int,\n\
	addr char(10));\n\
/\n\
create view allocs as select * from mallcheck\n\
                                where func = 'alloc'\n\
/\n\
create view frees  as select * from mallcheck\n\
                                where func = 'free'\n\
/\n\
DATALOAD TABLE mallcheck\n\
FUNC 1-1\n\
NAME 2-2 DEFAULT NULL\n\
SIZE 3-3 DEFAULT NULL\n\
ADDR 4-4 DEFAULT NULL\n\
INFILE *\n\
DEC '/ /./'\n\
DATE 'INTERNAL'\n\
TIME 'INTERNAL'\n\
NULL '?'\n\
COMPRESS SEPARATOR ','\n\
/\n\
";

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr03mWriteTrace(char* szFmt, ...);
static void pr03mP2C( void *cout , void *pin , int len );

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function: pr03mAllocatF 

  description: Reserviert Speicher

  arguments:
      lSize        [in]  Anzahl Byte des zu resevierenden Speichers.

  return:          Pointer auf resevierten Speicher oder NULL fuer Fehler.
 */
void* pr03mAllocatF(tsp00_Int4 lSize)
{
  void *p;
#if !defined(USE_MALLOC)
  tsp00_Bool bOk;
  sqlallocat(lSize, (tsp00_Uint1**)&p, &bOk);
  p = (bOk) ? p : NULL;
#else
  p = malloc(lSize);
#endif
  return p;
}  

/*!
  Function: pr03mCallocF 

  description: Allocates an array in memory with elements initialized to 0.

  arguments:
      num          [in]  number of elements
      lSize        [in]  length in bytes of each element

  return:          Pointer auf resevierten Speicher oder NULL fuer Fehler.
 */
void* pr03mCallocF(tsp00_Int4 num, tsp00_Int4 lSize)
{
  void *p;
#if !defined(USE_MALLOC)
  tsp00_Bool bOk;
  sqlallocat(ALIGN(lSize*num), (tsp00_Uint1**)&p, &bOk);
  memset(p,0,ALIGN(lSize*num));
  p = (bOk) ? p : NULL;
#else
  p = calloc( (size_t) num, (size_t) lSize );
#endif
  return p;
}  

/*!
  Function: pr03mAllocatT 

  description: Reserviert Speicher 

  arguments:
      lSize        [in]  Anzahl Byte des zu resevierenden Speichers.
      szStr        [in]  Tracestring fuer Memorycheck.

  return:          Pointer auf resevierten Speicher oder NULL fuer Fehler.
 */
void* pr03mAllocatT(tsp00_Int4 lSize, char *szStr)
{
  void *p;
  p = pr03mAllocatF(lSize);
  pr03mWriteTrace((char*)"alloc,'%s',%d,%#08lx\n", szStr, lSize, p);
  return p;
}  

/*!
  Function: pr03mCallocT 

  description: Allocates an array in memory with elements initialized to 0.

  arguments:
      num          [in]  number of elements
      lSize        [in]  length in bytes of each element
      szStr        [in]  Tracestring fuer Memorycheck.

  return:          Pointer auf resevierten Speicher oder NULL fuer Fehler.
 */
void* pr03mCallocT(tsp00_Int4 num, tsp00_Int4 lSize, char *szStr)
{
  void *p;
  p = pr03mCallocF(num, lSize);
  pr03mWriteTrace((char*)"calloc,'%s',%d,%d,%#08lx\n", szStr, num, lSize, p);
  return p;
}  

/*!
  Function: pr03mAllocatP

  description: Reserviert Speicher

  arguments:
      lSize        [in]  Anzahl Byte des zu resevierenden Speichers.
      Buffer       [out] Pointer auf reservierten Speicher. NULL im Falle
                         eines Fehlers.
      szStr        [in]  Pascal Tracestring fuer Memorycheck.

 */
void pr03mAllocatP(tsp00_Int4 lSize, void** Buffer, tsp00_Name VAR_ARRAY_REF szPStr)
{
  tsp00_Namec szCStr;
  pr03mP2C(szCStr, szPStr, sizeof(tsp00_Name));
  *Buffer = pr03mAllocatT(lSize, szCStr);
}

/*!
  Function: pr03mReallocatF

  description: Reallociert reservierten Speicher

  arguments:
      lSize        [in]  Anzahl Byte des zu resevierenden Speichers.
      Buffer       [in]  Pointer auf reservierten Speicher.

  return:          Pointer auf resevierten Speicher oder NULL fuer Fehler.
 */
void* pr03mReallocatF(void* Buffer, tsp00_Int4 lSize)
{
  void *p;
#if !defined(USE_MALLOC)
  tsp00_Bool bOk;
  sqlreallocat (lSize, (tsp00_Uint1**)&Buffer, &bOk);
  p = (bOk) ? Buffer : NULL;
#else
  p = realloc(Buffer, lSize);
#endif
  return p;
}

/*!
  Function: pr03mReallocatT

  description: Reallociert reservierten Speicher

  arguments:
      lSize        [in]  Anzahl Byte des zu resevierenden Speichers.
      Buffer       [in]  Pointer auf reservierten Speicher.
      szStr        [in]  Tracesting fuer Memorycheck.

  return:          Pointer auf resevierten Speicher oder NULL fuer Fehler.
 */
void* pr03mReallocatT(void* Buffer, tsp00_Int4 lSize, char *szStr)
{
  void *p;
  pr03mWriteTrace((char*)"free,'%s',%d,%#08lx\n", szStr, lSize, Buffer);
  p = pr03mReallocatF(Buffer, lSize);
  pr03mWriteTrace((char*)"alloc,'%s',%d,%#08lx\n", szStr, lSize, p);
  return p;
}

/*!
  Function: pr03mReallocatP

  description: Reallociert reservierten Speicher

  arguments:
      lSize        [in]  Anzahl Byte des zu resevierenden Speichers.
      Buffer       [in/out] Pointer auf reservierten Speicher.
      szStr        [in]  Pascal Tracesting fuer Memorycheck.

 */
void* pr03mReallocatP(void* Buffer, tsp00_Int4 lSize, tsp00_Name VAR_ARRAY_REF szPStr)
{
  tsp00_Namec szCStr;
  pr03mP2C(szCStr, szPStr, sizeof(tsp00_Name));
  return pr03mReallocatT(Buffer, lSize, szCStr);
}


/*!
  Function: pr03mFreeF 

  description: Gibt reservierten Speicher frei.

  arguments:
      Buffer       [in] Pointer auf reservierten Speicher.

 */
void pr03mFreeF(void* Buffer)
{
#if !defined(USE_MALLOC)
  sqlfree((tsp00_Uint1*)Buffer);
#else
  free(Buffer);
#endif
  return;
}

/*!
  Function: pr03mFreeT

  description: Gibt reservierten Speicher frei.

  arguments:
      Buffer       [in] Pointer auf reservierten Speicher.
      szStr        [in]  Tracestring fuer Memorycheck.

 */
void pr03mFreeT(void* Buffer, char *szStr)
{
  pr03mWriteTrace((char*)"free,'%s',0,%#08lx\n", szStr, Buffer);
  pr03mFreeF(Buffer);
  return;
}

/*!
  Function: pr03mFreeP

  description: Gibt reservierten Speicher frei.

  arguments:
      Buffer       [in] Pointer auf reservierten Speicher.
      szPStr       [in]  Pascal Tracestring fuer Memorycheck.

 */
void pr03mFreeP(void* Buffer, tsp00_Name VAR_ARRAY_REF szPStr)
{
  tsp00_Namec szCStr;
  pr03mP2C(szCStr, szPStr, sizeof(tsp00_Name));
  pr03mFreeT(Buffer, szCStr);
}

static void pr03mWriteTrace(char* szFmt, ...)
{
#ifndef SAPDB_FAST
  va_list ap;
  FILE *fd;
  if (pr03mIsTrace == IsOff_epr03m) {
    return;
  }  
  if (pr03mIsTrace != IsOn_epr03m) {
    char *p;
    p = getenv("SAPDBMEMCHECK");
    if (!p) {
      pr03mIsTrace = IsOff_epr03m;
      return;
    }
    sp77sprintf(pr03mTraceFileName, sizeof(pr03mTraceFileName), p, getpid());
    pr03mIsTrace = IsOn_epr03m;
    fd = fopen( pr03mTraceFileName, "w" );
    fprintf( fd, pr03mXLoadHeader );
  }
  else 
    fd = fopen( pr03mTraceFileName, "a" );
  va_start (ap, szFmt);     
  if (fd) {
    char szBuffer[400];
    sp77vsprintf(szBuffer, sizeof(szBuffer), szFmt, ap);
    fprintf( fd, szBuffer );
    fclose( fd );    
  }
#endif
}

/* stolen from vpr07Macro.c */
static void pr03mP2C( void *cout , void *pin , int len )
{
  char *pptr ;       /* pascal string ptr */
  char *cptr ;       /* C string ptr */
  
  pptr = (char *)pin;
  pptr += len - 1 ;
  cptr = (char *)cout;
  cptr += len ;
  
  * cptr -- = '\0' ;
  
  while ( pptr != pin && * pptr == ' ' ) {
    * cptr -- = '\0' ;
    pptr -- ;
  }
  
  if ( pptr == pin && * pptr == ' ' ) {
    * cptr -- = '\0' ;
  }
  else {
    while ( pptr != pin ) {
      * cptr -- = * pptr -- ;
    }
    * cptr = * pptr ; /* don't forget last char */
  }
}

