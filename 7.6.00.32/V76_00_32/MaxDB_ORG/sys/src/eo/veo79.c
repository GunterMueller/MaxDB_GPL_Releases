/*!
  @file           veo79.c
  @special area   kernel / LZU
  @brief          xparamfile access (configuration)
  @author         JoergM/RobinW
  @see            example.html ...
  @Original AuthorFrankH

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

#define MOD__ "veo79.c: "
#include "geo50_0.h"
#include "geo002.h"
#include "geo007_1.h"
#include "heo00.h"
#include "heo11_1.h"
#include "heo46.h"
#include "heo79.h"

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"
#include "SAPDBCommon/SAPDB_string.h"

/***************************************************************/
#if defined(_WIN32)
/***************************************************************/
#include <io.h>

/***************************************************************/
#endif
/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static void   eo79OpenXparamBinary      ( tsp00_DbNamec      ServerDB,
                                          BOOLEAN            ReadOnly,
                                          XP_PHFILE          xp_fd ,
                                          tsp00_ErrText      errtext ,
                                          tsp00_XpReturnCode *xp_ret ) ;

static tsp00_Bool  eo79XparamDelete     (tsp11_XpKeyTypec    acXParamKeyC,
                                         tsp00_ErrText       errtext);

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

void    vxparam_save_good_config ( BOOLEAN             is_restart,        
                                   tsp00_ErrText         errtext ,
                                   tsp00_XpReturnCode *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"vxparam_save_good_config"


  DBGIN;
#ifdef VERSION_TEST
{
      int test_copy_param();
/*      vxparam_save_good_config(1, errtext, xp_ret ) ; */
      test_copy_param();
}
  vxparam_rec_put( "SCHNUDEL", "DUDEL", 0 ,errtext, xp_ret, errtext );
  vxparam_rec_put( "_PRIO_BASE_COM", "33", 1 ,errtext, xp_ret, errtext );
  vxparam_rec_put( "_TREE_RGNS", "10", 1 ,errtext, xp_ret, errtext );
  vxparam_rec_put( "_PRIO_BASE_COM", "10", 0 ,errtext, xp_ret, errtext );
  vxparam_rec_put( "_PRIO_BASE_COM", "10", 1 ,errtext, xp_ret, errtext );
#endif

  *xp_ret = XP_OK  ;

  if ( !is_restart || XPARAM(ulXparamChanged) || !RTEConf_ParameterAccessIsVerified())
  {
    tsp01_RteError RteError ;
    eo46_rte_error_init ( &RteError ) ;

    if ( !is_restart  ) /* !!! config changed (add devspace) !!! */
      sqlXParamRemoveAllVersions( KERNEL_GLOBAL_DBNAME, &RteError ) ;

    if ( RteError.RteErrCode == RTE_NO_ERROR_SP01 )
    {
      /* PTS 1103607: 1. Reset Changedflag / 2. save param as version*/
      SAPDB_Char *errText;
      if(RTEConf_ParameterAccessResetParamChangedFlag(&errText))
      {
        eo11SaveActParamFileAsVersion ( KERNEL_GLOBAL_DBNAME, &RteError ) ;
      }
      else
      {
        RteError.RteErrCode = RTE_ERROR_SP01 ;
        strncpy ( RteError.RteErrText, errText , sizeof (RteError.RteErrText) );
      }
      if ( (RteError.RteErrCode == RTE_NO_ERROR_SP01) ) 
        XPARAM(ulXparamChanged) = 0;
      else
      {
        MSGCD (( ERR_XPARAM_VERSIONING, RteError.OsErrText ));
        SAPDB_memcpy(errtext, RteError.OsErrText, sizeof( tsp00_ErrText) ) ;
        *xp_ret = !XP_OK ;
      }
    }
  }
  DBGOUT;
  return ;
}

/*
 * ===========================================================================
 */

void    vxparam_open_binary ( tsp00_DbName       ServerDB,
                              BOOLEAN            ReadOnly,
                              XP_PHFILE          xp_fd ,
                              tsp00_ErrText      errtext ,
                              tsp00_XpReturnCode *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"vxparam_open_binary"

  tsp00_DbNamec     ServerDBc ;

  DBGIN;

  eo46PtoC ( ServerDBc, ServerDB, sizeof (tsp00_DbName) );

  eo79OpenXparamBinary (ServerDBc, ReadOnly, xp_fd, errtext,xp_ret) ;

  DBGOUT;
  return ;
}

/*
 * ===========================================================================
 */

void    vxparam_open ( XP_PHFILE            xp_fd ,
                       tsp00_ErrText        errtext ,
                       tsp00_XpReturnCode  *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"vxparam_open"

  DBGIN;
  eo79OpenXparamBinary ( KERNEL_GLOBAL_DBNAME, TRUE /* ReadOnly */,
                         xp_fd, errtext, xp_ret ) ;

  DBGOUT;
  return ;
}

/*
 * ===========================================================================
 */

void    vxparam_close ( XP_HFILE            xp_fd ,
                        tsp00_ErrText         errtext ,
                        tsp00_XpReturnCode *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"vxparam_close"

  tsp01_RteError    RteError ;
  DBGIN;

  eo46_rte_error_init ( &RteError ) ;

  sqlXparamClose ( xp_fd, XP_CLOSE_NO_FLUSH_SP11, &RteError );
  if ( RteError.RteErrCode != RTE_NO_ERROR_SP01 )
    {
      *xp_ret = !XP_OK ;
      eo46CtoP ( errtext, RteError.RteErrText, sizeof (tsp00_ErrText) );
    }
  else
    *xp_ret = XP_OK ;

  DBGOUT;
  return ;
}

/*
 * ===========================================================================
 */

void    vxparam_read_binary ( XP_HFILE            xp_fd ,
                              char               *buf ,
                              tsp00_Int4            bytes_to_read ,
                              tsp00_Int4           *bytes_read ,
                              tsp00_ErrText         errtext ,
                              tsp00_XpReturnCode *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"vxparam_read_binary"

  DBGIN;

  sql11c_SeqBinReadXParam ( xp_fd , (char *)buf, bytes_to_read ,
		           bytes_read, errtext , xp_ret ) ;

  DBGOUT;
  return ;
}

/*
 * ===========================================================================
 */

void    vxparam_write_binary ( XP_HFILE            xp_fd ,
                               char               *buf ,
                               tsp00_Int4            bytes_to_write ,
                               tsp00_Int4           *bytes_written ,
                               tsp00_ErrText         errtext ,
                               tsp00_XpReturnCode *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"vxparam_write_binary"

  DBGIN;

  sql11c_SeqBinWriteXParam ( xp_fd , (char *)buf, bytes_to_write ,
		                     bytes_written, errtext , xp_ret ) ;

  DBGOUT;
  return ;
}

/*
 * ===========================================================================
 */

void    vxparam_get_filesize ( tsp00_Int4           *xp_fsize ,
                               tsp00_ErrText         errtext ,
                               tsp00_XpReturnCode *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"vxparam_get_filesize"
  tsp00_Bool       fOk ;
  tsp01_RteError   RteError;

  DBGIN;

  eo46_rte_error_init ( &RteError ) ;
  fOk = sqlXparamFsize ( KERNEL_GLOBAL_DBNAME, xp_fsize, &RteError ) ;

  if ( !fOk )
  {
    *xp_ret = !XP_OK ;
    eo46CtoP ( errtext, RteError.OsErrText, sizeof (tsp00_ErrText) );
  }
  else
    *xp_ret = XP_OK ;

  DBGOUT;
  return ;
}


/*
 * Special call for migration 7.X.X -> 7.4.3 ====================================
 */


externC tsp00_Bool  vxparam_delete_sysdev (tsp00_ErrText       errtext)
{
    #undef  MF__
    #define MF__ MOD__"vxparam_delete_sysdev"

    tsp00_Bool            rc = true;
    tsp00_Int4            idx;
    tsp00_Int4            ulNumDevsp;


    DBGIN;

    if (rc = eo79XparamDelete ("SYSDEV_001", errtext))
    {
        if (rc = eo79XparamDelete (PAN_MAXSYSDEVSPACES, errtext))
        {
            ulNumDevsp = KGS->XParam->DevspDescr[RTE_VolumeTypeSystem].ulNumDevs;
            KGS->XParam->DevspDescr[RTE_VolumeTypeSystem].ulNumDevs = 0;
            for ( idx=0; idx < ulNumDevsp; idx++ )
            {
                KGS->XParam->DevspDescr[RTE_VolumeTypeSystem].Devspaces[idx].lDevspaceType = DEVSPACE_UNUSED;
                KGS->XParam->DevspDescr[RTE_VolumeTypeSystem].Devspaces[idx].physicalDevID = RTE_UNDEF_ID;
            }
        }
    }

    DBGOUT;
    return (rc);
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/
static void    
eo79OpenXparamBinary ( tsp00_DbNamec      ServerDB,
                       BOOLEAN            ReadOnly,
                       XP_PHFILE          xp_fd ,
                       tsp00_ErrText      errtext ,
                       tsp00_XpReturnCode *xp_ret )
{
  #undef  MF__
  #define MF__ MOD__"eo79OpenXparamBinary"

  tsp01_RteError    RteError ;
  tsp00_Int4        DefaultMode = XP_OPEN_BIN_SP11 ;

  DBGIN;

  eo46_rte_error_init ( &RteError ) ;

  if ( ReadOnly ) 
    DefaultMode |= XP_OPEN_READONLY_SP11 ;
  else
    DefaultMode |= XP_OPEN_CREATE_SP11 ;

  *xp_ret = XP_OK ;

  sqlXparamOpen (ServerDB, xp_fd, DefaultMode, &RteError) ;

  if ( RteError.RteErrCode != RTE_NO_ERROR_SP01 )
    {
      *xp_ret = !XP_OK ;
      *xp_fd = -1 ;
      eo46CtoP ( errtext, RteError.RteErrText, sizeof (tsp00_ErrText) );
    }

  DBGOUT;
  return ;
}

/*
 * ===========================================================================
 */


static tsp00_Bool  eo79XparamDelete (tsp11_XpKeyTypec    acXParamKeyC,
                                      tsp00_ErrText       errtext)
{
    #undef  MF__
    #define MF__ MOD__"vxparam_delete_param"
    tsp00_Bool            rc = true;

    DBGIN;

    rc =  RTEConf_ParamaterAccessDeleteParam (KERNEL_GLOBAL_DBNAME , acXParamKeyC, errtext);
    
    DBGOUT;
    return (rc);
}


/*
 * ===========================================================================
 */
