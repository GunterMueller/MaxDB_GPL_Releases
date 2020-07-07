/* @lastChanged: "1998-06-02  18:09"
 * @filename:    veo66.c
 * @purpose:     "vbackup_info, vbackup_medium_def, vfopen, vfwrite, vfclose "
 * @release:     M.m.c.p
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

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



\endif

 */

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gsp00.h"
#include "geo00_0.h"
#include "heo00.h"
#include "geo002.h"
#include "geo50_0.h"
#include "heo66.h"
#include "heo46.h"
#include "heo01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#define MOD__ "VEO66C : "

#define BACKUP_HISTORY_EXT "knl"
#define BACKUP_MED_DEF_EXT "mdf"


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*----------------------------------------------------------------------*/
#if defined(_WIN32)
/*----------------------------------------------------------------------*/
#define DEFINE_THIS_UKT 
#define SET_TASK_STATE(_State)
#define SERVERDB_NAME KGS->szServerDB
#define LINE_END_CHRS "\r\n"
/*----------------------------------------------------------------------*/
#else
/*----------------------------------------------------------------------*/
#define DEFINE_THIS_UKT ten50_UKT_Control *this_ukt = THIS_UKT_CTRL
#define SET_TASK_STATE(_State) ( this_ukt->curr_task->state = _State )
#define SERVERDB_NAME ((char *)KGS->serverdb)
#define LINE_END_CHRS "\n"
/*----------------------------------------------------------------------*/
#endif
/*----------------------------------------------------------------------*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static  void sql66kc_backup_info ( char                 *filename ,
                                   char                 *backup_info, 
                                   tsp00_Int4            info_len,
 		                           tsp00_VfReturn	    *error ,
		                           tsp00_ErrText         errtext ) ;


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC void vbackup_info ( char                *backup_info, 
                            tsp00_Int4           info_len,
 		                    tsp00_VfReturn      *error ,
		                    tsp00_ErrText        errtext )
{

  #undef  MF__
  #define MF__ MOD__"vbackup_info"

  tsp00_Pathc wrkPath;
  tsp01_RteError rteError;

  DBGIN;

  if ( !sqlGetIndependentWrkPath ( wrkPath, 1, &rteError ) ) /* PTS 1107206 */
  {
      *error = vf_notok;
      eo46CtoP( errtext, rteError.RteErrText, sizeof(tsp00_ErrText) );
      return;
  }

  sprintf(wrkPath + strlen(wrkPath), "%s.%s", SERVERDB_NAME, BACKUP_HISTORY_EXT ) ;

  sql66kc_backup_info ( wrkPath, (char *)backup_info, 
                        info_len, error ,errtext ) ;

  sql66kc_backup_info ( XPARAM(szBackupHistFile), (char *)backup_info, 
                        info_len, error ,errtext ) ;

  DBGOUT;
}

/*==========================================================================*/

externC void vbackup_medium_def ( char                *backup_medium_def, 
                                  tsp00_Int4           info_len,
 		                          tsp00_VfReturn	  *error ,
		                          tsp00_ErrText        errtext )
{

  #undef  MF__
  #define MF__ MOD__"vbackup_medium_def"

  tsp00_Pathc wrkPath;
  tsp01_RteError rteError;

  DBGIN;

  if ( !sqlGetIndependentWrkPath ( wrkPath, 1, &rteError ) ) /* PTS 1107206 */
  {
      *error = vf_notok;
      eo46CtoP( errtext, rteError.RteErrText, sizeof(tsp00_ErrText) );
      return;
  }

  sprintf(wrkPath + strlen(wrkPath), "%s.%s", SERVERDB_NAME, BACKUP_MED_DEF_EXT ) ;

  sql66kc_backup_info ( wrkPath, (char *)backup_medium_def, 
                        info_len, error ,errtext ) ;

  sql66kc_backup_info ( XPARAM(szBackupMedDefFile), (char *)backup_medium_def, 
                        info_len, error ,errtext ) ;

  DBGOUT;
}


/*==========================================================================*/

externC void    vfrawopen ( tsp00_VFilename    	hostfile ,
           		            tsp00_Int4		  	*hostfileno ,
 	        	            tsp00_VfReturn		*error ,
		                    tsp00_ErrText       errtext )

{
  #undef  MF__
  #define MF__ MOD__"vfrawopen"

  tsp00_VFilenamec filename ;
  DEFINE_THIS_UKT;

  DBGIN;

    SET_TASK_STATE( TSK_VFOPEN_EO00 ) ;

    eo46PtoC ( filename , hostfile , sizeof(tsp00_VFilename) );

    sql66kc_open ( filename, true, hostfileno, error, errtext );

    SET_TASK_STATE( TSK_RUNNING_EO00 ) ;

  DBGOUT;
}

/*==========================================================================*/

externC void    vfopen ( tsp00_VFilename    	hostfile ,
        		         tsp00_Int4		  	   *hostfileno ,
 	        	         tsp00_VfReturn		   *error ,
		                 tsp00_ErrText          errtext )

{
  #undef  MF__
  #define MF__ MOD__"vfopen"

  tsp00_VFilenamec filename ;
  DEFINE_THIS_UKT;

  DBGIN;

    SET_TASK_STATE( TSK_VFOPEN_EO00 ) ;

    eo46PtoC ( filename , hostfile , sizeof(tsp00_VFilename) );

    sql66kc_open ( filename, false, hostfileno, error, errtext );

    DBG3 ((MF__, "hostfileno = %d, error = %d" , *hostfileno, *error ));
    SET_TASK_STATE( TSK_RUNNING_EO00 ) ;

  DBGOUT;
}

/*==========================================================================*/

externC void    vfclose ( tsp00_Int4	    hostfileno ,
		                  tsp00_VfReturn   *error ,
		                  tsp00_ErrText	    errtext )

{

  #undef  MF__
  #define MF__ MOD__"vfclose"
  DEFINE_THIS_UKT;

  DBGIN;

    SET_TASK_STATE( TSK_VFCLOSE_EO00 ) ;

    DBG3 ((MF__, "hostfileno = %d" , hostfileno ));

    sql66kc_close ( (int) hostfileno , error, errtext );

    SET_TASK_STATE( TSK_RUNNING_EO00 ) ;

  DBGOUT;

}

/*==========================================================================*/

externC void    vfwrite ( tsp00_Int4	    hostfileno ,
                          char*             block ,
                          tsp00_VfReturn   *error ,
                          tsp00_ErrText	    errtext )

{
  #undef  MF__
  #define MF__ MOD__"vfwrite"
  DEFINE_THIS_UKT;

  DBGIN;

    SET_TASK_STATE( TSK_VFWRITE_EO00 ) ;

    DBG3 ((MF__, "hostfileno = %d" , hostfileno ));
    sql66kc_write ( (int) hostfileno , block , 
                    XPARAM(ulDBPageSize), error, errtext );

    SET_TASK_STATE( TSK_RUNNING_EO00 ) ;

  DBGOUT;

}


/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static void sql66kc_backup_info ( char                 *filename ,
                                  char                 *backup_info, 
                                  tsp00_Int4            info_len,
 			                      tsp00_VfReturn	   *error ,
		                          tsp00_ErrText         errtext )
{

  #undef  MF__
  #define MF__ MOD__"sql66kc_backup_info"

  tsp00_Int4			hostfileno ;
  DEFINE_THIS_UKT;

  DBGIN;

  SET_TASK_STATE( TSK_VFOPEN_EO00 ) ;

  sql66kc_append_open ( filename, &hostfileno, error, errtext );

  DBG3 ((MF__, "hostfileno = %d, error = %d" , hostfileno, *error ));

  if ( *error == vf_ok )
    {
      SET_TASK_STATE( TSK_VFWRITE_EO00 ) ;

      sql66kc_write ( (int) hostfileno , (char*) backup_info, info_len, 
                      error, errtext );

   
      if ( *error == vf_ok )
        sql66kc_write ( (int) hostfileno , (char*) LINE_END_CHRS, 
                        sizeof ( LINE_END_CHRS) - 1 , error, errtext );

      SET_TASK_STATE( TSK_VFCLOSE_EO00 ) ;

      if ( *error == vf_ok )
        sql66kc_close ( (int) hostfileno , error, errtext );
      else
        {
          tsp00_VfReturn  dum_error ;
          tsp00_ErrText   dum_errtext ;
          sql66kc_close ( (int) hostfileno , &dum_error, dum_errtext );
        }
    }

  SET_TASK_STATE( TSK_RUNNING_EO00 ) ;

  DBGOUT;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
