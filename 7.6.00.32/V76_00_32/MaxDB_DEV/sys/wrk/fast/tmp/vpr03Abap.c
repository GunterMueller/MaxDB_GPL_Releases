/*!================================================================

  module:       vpr03Abap.c

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area: PR Layer

  description:  Abap Exception Handler Wrapper.

  see also:

 -------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#define LC_STREAMS_ENABLED
#include "livecachetypes.h" /* no check */

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

/* callbackfunctions for handling stream IO prior to 7.4.3. Build 33 */
typedef int (*SQLStreamReadProc_743)(SQL_LC_StreamParm *ABAPTabParm, void *rgbOutStream, int cbStreamMax, int *pcbStreamLen, int *pcbNoOfRowsRead);

typedef int (*SQLStreamWriteProc_743)(SQL_LC_StreamParm *ABAPTabParm, void *rgbInStream, int cbNoOfRows);

typedef int (*SQLStreamWriteProc_new)(SQL_LC_StreamParm *ABAPTabParm, void *rgbInStream, int cbNoOfRows, int cbBufSize, void*);

int pr03WriteProc(SQLABAPWriteProc *WriteProc,
                  SQL_LC_ABAPTabParm *StreamParam,
                  char *pOutputStream,
                  int cbRowCount,
                  int cbBufSize,
                  void* pMask,
                  tsp00_Bool old);

int pr03ReadProc(SQLABAPReadProc *ReadProc,
                 SQL_LC_ABAPTabParm *StreamParam,
                 char *pInputStream,
                 int cbPartLenMax,
                 int *cbBufSize,
                 int *cbRowCount,
                 void* pMask,
                 tsp00_Bool old);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

int pr03WriteProc(SQLABAPWriteProc *WriteProc,
	      SQL_LC_ABAPTabParm *StreamParam,
	      char *pOutputStream,
	      int cbRowCount,
	      int cbBufSize,
          void* pMask,
          tsp00_Bool old)
{
  int retcode = SQL_ABAP_OK;
  if (old)
    retcode = ((SQLStreamWriteProc_743)WriteProc)(StreamParam, pOutputStream, cbRowCount);
  else
    retcode = ((SQLStreamWriteProc_new)WriteProc)(StreamParam, pOutputStream, cbRowCount, cbBufSize, pMask);
  return retcode;
}

int pr03ReadProc(SQLABAPReadProc *ReadProc,
	      SQL_LC_ABAPTabParm *StreamParam,
	      char *pInputStream,
	      int cbPartLenMax,
	      int *cbBufSize,
	      int *cbRowCount,
          void* pMask,
          tsp00_Bool old)
{
  int retcode = SQL_ABAP_OK;
  if (old)
    retcode = ((SQLStreamReadProc_743)ReadProc)(StreamParam, pInputStream, cbPartLenMax, cbBufSize, cbRowCount);
  else
    retcode = ReadProc(StreamParam, pInputStream, cbPartLenMax, cbBufSize, cbRowCount, pMask);
  /* 1106904 */
  return retcode;
}

static char ident_sr[] = "@(#)vpr03Abap.c dated 2006-07-17 05:56:31 used 2008-05-09 01:03:41";
extern char *vpr03Abap_force_data () { return( ident_sr ); }
static char ident_mf[] = "@(#)vpr03Abap FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF";
static char ident_cc[] = "@(#)vpr03Abap cc -DREL30 -DLINUX -DI386 -DSAG -DSDB_RELEASE_070600 -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/usr/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl/SAPDB -DSYSV -D_SVID -I/usr/include/ncurses -w -Wall -D_FILE_OFFSET_BITS=64 -fPIC -DDYNAHASH -DHAS_NPTL -fno-strict-aliasing -DSAPDB_FAST -D_REENTRANT -O3 -march=pentium -mcpu=pentiumpro -I/home/gunter/SANDBOX/MaxDB/DevTool/incl ";
extern char *vpr03Abap_force_whatinfo () { return( ident_cc ); }
