/*!
  @file           veo112c.cpp
  @author         JoergM
  @special area   tool
  @brief          get first / last logpno from save
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


#include <stdlib.h>
#include <stdio.h>

#include "gsp00.h"
#include "ggg00.h"
#include "geo60.h"
#include "heo06.h"
#include "gkb03.h"

#ifdef WIN32
#include <windows.h>
#endif

#define MOD__ "veo112c.cpp:"
#undef MF__

#ifndef TRUE
#define TRUE   1
#define FALSE  0
#endif /* FALSE */

externC void sqlinit             ( char *                  psComponent,
                                   tsp00_BoolAddr          pbCancelAddress );

externC void sqlfinish           ( tsp00_Bool              bTerminate );


#define IS_FIRST_INFO_PAGE(_page )  ( _page->inf_pt2 == pt2VolumeFirstInfo_egg00 && \
                                      _page->inf_pt  == ptSave_egg00 )

#define IS_DATA_PAGE(_page )  ( _page->inf_pt == ptData_egg00 || _page->inf_pt == ptLog_egg00 || _page->inf_pt == ptFileDir_egg00 )
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#define ERRTEXT(v,t)  { DBG3((MF__,t));\
		      v.rawAssign(t); }

static char BAD_BLOCKSIZE_GIVEN []            = "bad blocksize given";
static char BAD_BLOCKSIZE_READ  []            = "bad blocksize read";



static int	sqlextract_logpno ( tsp00_VFilename                   SaveFile ,
                                tsp00_Int2                        BufCnt ,
                                char                            FirstLast,
                                tsp00_VfReturn_Enum           * error ,
                                tsp00_ErrText                 & errtext ) ;


int	main ( int argc , char **argv )
{
  #undef  MF__
  #define MF__ MOD__"main"
    tsp00_ErrTextc              errtextc;
    tsp00_VFilename               SaveFile ;
    tsp00_VfReturn_Enum         error ;
    tsp00_ErrText               errtext ;
    tsp00_Int2                    BufCnt ;
	int                         LogPno ;
    char                        WhichLpno ;


    if (  argc != 4 || 
		 ( (WhichLpno = *(argv[3])) != 'f' && WhichLpno != 'l') )
    {
	(void) fputs ( "usage: g_fl_lpno savefile blck_cnt f|l\n", stderr );
	return ( 1 );
    }

    strcpy(SaveFile,argv[1]) ;

    BufCnt = atoi ( argv[2] ) ;

    sqlinit ( (char *)"g_fl_lpno" , (tsp00_BoolAddr)0 );
    LogPno = sqlextract_logpno ( SaveFile, BufCnt, WhichLpno, &error, errtext ) ;
    if ( error != vf_ok )
      { errtextc.p2c( errtext );
        fprintf ( stderr, "g_fl_lpno: %s \n" , (char *) errtextc ) ;
      }
    else
       fprintf ( stdout, "%d\n", LogPno );

    sqlfinish ( FALSE );
    return ( error != vf_ok );
}




static int	sqlextract_logpno ( tsp00_VFilename                   SaveFile ,
                                tsp00_Int2                        BufCnt ,
                                char                            FirstLast,
                                tsp00_VfReturn_Enum           * error ,
                                tsp00_ErrText                 & errtext )
{
  #undef  MF__
  #define MF__ MOD__"sqlextract_logpno"

  tsp00_Int4             PoolName = 0;
  tsp00_Int4             SaveFileNo,
                       RecLen = BufCnt * sizeof(tsp00_Page); /* XXX rdependend on tsp00_Page. Better retry sqlfsaveopenc() with the returned RecLen... */

  tsp00_Longint          BytesRead ;
  char                 *Buf = NULL ;
  tsp05_RteFileError   RteError ;
  int                  LogPno =-1 ;
  tkb3_info_stuff      *InfoPage ;
  
  DBGIN;

  DBG3 ((MF__, "SaveFile = %s"  , SaveFile )) ;
  DBG3 ((MF__, "BufCnt   = %d"  , BufCnt   )) ;
  DBG3 ((MF__, "FirstLast= %c\n", FirstLast   )) ;

  if ( BufCnt < 1 )
    { *error = vf_notok  ;
       ERRTEXT( errtext, BAD_BLOCKSIZE_GIVEN );
    }
  else
      *error = vf_ok  ;

  if ( *error == vf_ok )
    { sqlfsaveopenc ( SaveFile, sp5vf_read, &RecLen, &SaveFileNo, (void**) &Buf, &RteError ) ;
      if ( ( *error = RteError.sp5fe_result ) != vf_ok )
        ERRTEXT( errtext, RteError.sp5fe_text )
    }

  if ( *error == vf_ok )
    { 
      do
      {
        sqlfreadc ( SaveFileNo, Buf, RecLen, &BytesRead , &RteError ) ;

        DBG3 ((MF__, " %8d.sqlfread : %d Bytes", ++TotalReads, BytesRead )) ;

        if ( ( *error = RteError.sp5fe_result ) == vf_ok && BytesRead > 0 )
        { InfoPage = (tkb3_info_stuff *) Buf ;
          if ( IS_FIRST_INFO_PAGE ( InfoPage ) ) 
            {
		      LogPno = FirstLast == 'f' ? InfoPage->inf_first_iosequence 
			                            : InfoPage->inf_last_iosequence ;
            }
	    }
        else
          if ( *error != vf_ok )
            ERRTEXT( errtext, RteError.sp5fe_text )
          else
            { *error = vf_notok ;
              ERRTEXT( errtext, BAD_BLOCKSIZE_READ );
            }
        }
      while ( *error == vf_ok && !IS_DATA_PAGE( InfoPage ) && LogPno == -1 ) ;
    }

  sqlfclosec ( SaveFileNo, sp5vf_close_normal, &RteError);

  DBGOUT ;
  return( LogPno ) ;
}

