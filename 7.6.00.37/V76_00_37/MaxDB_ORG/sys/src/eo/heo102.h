/*!
  @file           heo102.h
  @author         JoergM
  @special area   User LZU Function Prototypes
  @brief          User LZU Function Prototypes
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


#ifndef HEO102_H
#define HEO102_H

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "geo102.h"
/* storage management
sqlallocat, 
sqlfree,
sqlreallocat,
*/
#include "geo572.h"

/*
    veo01.c

sqlgetenv
*/
#include "heo01.h"

/*
    veo03c:component kernel communication
sqlaconnect,
sqlarelease,
sqlarequest,
sqlareplyavailable,
sqlareceive,
sqlacancel,
sqlhostname,
sqlnodename
 */
#include "heo03.h"

/*
	veo06c:Virtual File (System independent) 
sqlfopenc,sqlfopenp,
sqlfsaveopenc,
sqlfclosec,sqlfclosep,
sqlfreadc,sqlfreadp,
sqlfwritec,sqlfwritep,
sqlfseekc,sqlfseekp,
sqlflongseekc,sqlflongseekp,
sqlftellc,sqlftellp,
sqlferasec,sqlferasep,
sqlfinfoc,sqlfinfop,
sqlfilecopyc,
sqlfopendirc,
sqlfclosedirc,
sqlfreaddirc,
sqljoindirc,
sqlcreate_dirc,
sqlgetcwdc,
sqlchdirc,
sqlspecialenum,
sqlspecialnext,
sqlspecialclose,
sqlffinishc,sqlffinishp,
sqlfunlockc,sqlfunlockp
*/
#include "heo06.h"

/*
   veo13uc:xuser API
sqlxuopenuser,
sqlxucloseuser,
sqlputuser,
sqlgetuser,
sqlindexuser,
sqlclearuser,
sqlinfouser
 */
#include "heo13.h"

/*===========================================================================*
*  FUNCTION PROTOTYPES                                                      *
*===========================================================================*/

/*
=O S=========================================================
*/

/*	vos01uc:Initialization (User Part) */

externC void sqlclock (tsp00_Int4   *pSeconds,
					   tsp00_Int4   *pMilliSeconds);

externC void sqldattime (tsp00_Date  pDate,
						 tsp00_Time  pTime);

externC void sqlinit (tsp00_CompName acComponent,
					  tsp00_BoolAddr  pbCancelAddress);

externC void sqlfinish (tsp00_Bool bTerminate);

externC void sqlget_config_path (tsp00_C256   ConfigPath);

externC void sqlget_wrk_path (tsp00_C256   WrkPath);

externC void sqlos (tsp00_Os *pfOS);

externC void sqlresult (tsp00_Uint1 fResult);

externC void sqlsleep (tsp00_Int2 limit);

externC void sqluid (tsp00_TaskId* pUserID);


/*	vos04u:Virtual Terminal (Pascal Code) */

externC void sqlton (tsp00_Bool multi_task, 
						   tsp00_Int4 *term_ref, 
						   tsp00_TerminalDescription *desc,
						   tsp00_BoolAddr ok);

externC void sqltio (tsp00_Int4 term_ref, 
						   tsp00_ScreenBufAddr *screen_buf,
						   tsp00_ScreenBufAddr *attributes_buf,
						   tsp00_Bool refresh, 
						   tsp00_Int2 *cursorline, 
						   tsp00_Int2 *cursorcolumn,
						   tsp00_HifParms *hif,
						   tsp00_VtOptions *options,
						   tsp00_VtResult *result,
						   tsp00_BoolAddr ok);

externC void sqlttable (tsp00_Int2 i,
							  tsp00_VtAttrib ptoc_ptr_att,
							  tsp00_VtColor foreground,
							  tsp00_VtColor background);

externC void sqlwindow (tsp00_Int4 parent_ref,
							  tsp00_VtRectangle *position,
							  tsp00_TerminalDescription *desc,
							  tsp00_BoolAddr ok);

externC void sqlwsplit (tsp00_Int4 term_ref,
							  tsp00_Int2 nr_screens,
							  tsp00_TerminalDescription *desc,
							  tsp00_Int2 *offset,
							  tsp00_BoolAddr ok);

externC void sqlwoff (tsp00_Int4 term_ref,
							tsp00_ScreenBufAddr *screen_buf,
							tsp00_ScreenBufAddr *attribute_buf,
							tsp00_ScreenBufAddr *color_buf);

externC void sqlwdgswitch (tsp00_Int2 box_id,
								 tsp00_VtKeys ptoc_ptr_trigger,
								 tsp00_VtKey retrn, 
								 tsp00_BoolAddr ok);

externC void sqlwdgclear (tsp00_Int2 box_id);

externC void sqlwdgset (tsp00_Int2 box_id,
							  tsp00_Int2 item_id,
							  tsp00_C100 text,
							  tsp00_Int2 text_len,
							  tsp00_BoolAddr ok);

externC void sqlwdgget (tsp00_Int2 box_id,
							  tsp00_Int2 item_id,
							  tsp00_C100 text,
							  tsp00_Int2 *text_len,
							  tsp00_BoolAddr ok);

externC void sqlwtitle (tsp00_Int4 term_ref,
							  tsp00_Int2 screen_nr,
							  tsp00_C100 text,
							  tsp00_Int2 text_len,
							  tsp00_BoolAddr ok);

externC void sqlwscroll (tsp00_Int4 term_ref,
							   tsp00_VtRectangle *rect,
							   tsp00_Int2 displ_x,
							   tsp00_Int2 displ_y,
							   tsp00_BoolAddr ok);

externC void sqlwscbar (tsp00_Int4 term_ref,
							  tsp00_Int2 screen_nr,
							  tsp00_VtScroll t,
							  tsp00_Int2 range_beg,
							  tsp00_Int2 range_end,
							  tsp00_Int2 pos,
							  tsp00_BoolAddr ok);

/*	vos04uc:Virtual Terminal */

externC void sqltermvar (tsp00_Uint1 *psTerminalType);

externC void sqltoff (tsp00_Int4 lTermRef,
					  tsp00_ScreenBufAddr *ppsScreenBuffer,
					  tsp00_ScreenBufAddr *ppsAttributeBuffer,
					  tsp00_C80                psMsg);

externC void sqlcton (tsp00_VtKey *psReturnKeys,
					  tsp00_Int2 *pkReturnKeysSize,
					  tsp00_VtAttrib *psAttributes,
					  tsp00_Int2 *pkAttributesSize,
					  tsp00_VtColor *psColors,
					  tsp00_Int2 *pkColorSize,
					  tsp00_BoolAddr pbHasSysLine,                           			    			    
					  tsp00_BoolAddr pbLabel,
					  tsp00_Int2 *pkMaxLines,
					  tsp00_Int2 *pkMaxCols,
					  tsp00_ScreenBufAddr *ppsScreenBuffer,
					  tsp00_ScreenBufAddr *ppsAttributeBuffer,
					  tsp00_BoolAddr pbGraphic,
					  tsp00_BoolAddr pbMark,
					  tsp00_BoolAddr pbWindows,
					  tsp00_BoolAddr pbOk);

externC void sqlctio (tsp00_ScreenBufAddr *ppsScreenBuffer,
					  tsp00_ScreenBufAddr *ppsAttributeBuffer,
					  tsp00_Int2 *pkCursorLine,
					  tsp00_Int2 *pkCursorColumn,
					  tsp00_Addr              psSkText,
					  tsp00_Line              psSysText,
					  tsp00_VtAttrib *psSysAttr,
					  tsp00_C8 pszInsertText,
					  teo04_InsertLabelPos akInsertPos,
					  tsp00_C8c *pcInsertAttr,
					  tsp00_VtOptions *psOptions,
					  tsp00_VtKey *psReturnKeys,
					  tsp00_VtKey *psRejectKeys,
					  tsp00_Int2 *pkNumberOfLines,
					  tsp00_Int2 *pkNumberOfColumns,
					  tsp00_Int2 *pkMarkBegin,
					  tsp00_Int2 *pkMarkEnd,
					  tsp00_BoolAddr pbScreenChanged,
					  teo04_KeyStroke* psKeystroke,
					  tsp00_Bool  bRefresh,
					  tsp00_BoolAddr pbOk);

externC void sqlcttable (tsp00_Int2       kTableIndex,
						 tsp00_VtAttrib *psAttributes,
						 tsp00_VtColor   ucForegroundColor,
						 tsp00_VtColor   ucBackgroundColor);

/*	vos05uc:Virtual Printer */

externC void sqlpon (tsp00_PrtName printer,
					 tsp00_BoolAddr       vpok);

externC void sqlprint (tsp00_PrtLine     line,
					   tsp00_Int2        length,
					   tsp00_VpLinefeeds lfeeds,
					   tsp00_ErrText     errtext,
					   tsp00_BoolAddr vpok);

externC void sqlpoff (tsp00_BoolAddr  print,
					  tsp00_ErrText   errtext,
					  tsp00_BoolAddr  vpok);

/*	vos06uc:Virtual File */

externC void sqldevsize (tsp00_DevName       dev_name,
						 tsp00_Int4         *devcapacity,
						 tsp00_ErrText       errtext,
						 tsp00_BoolAddr      ok);

externC void sqlfinit (tsp00_Int2     buf_pool_size,
					   tsp00_Int4 *poolptr,
					   tsp00_BoolAddr ok);

externC void sqlfopen (tsp00_VFilename     vf_filename,
					   tsp00_VFileOpCodes  direction,
					   tsp00_VfResource    resource,
					   tsp00_Int4         *hostfileno,
					   tsp00_VfFormat     *format,
					   tsp00_Int4         *rec_len,
					   tsp00_Int4          poolptr,
					   tsp00_Int2          buf_count,
					   char              **block,
					   tsp00_VfReturn     *error,
					   tsp00_ErrText       errtext);

externC void sqlfclose_next_tape (tsp00_Int4         *hostfileno,
								  tsp00_Bool                erase,
								  tsp00_Int4          poolptr,
								  tsp00_Int2          buf_count,
								  char *     block,
								  tsp00_VfReturn     *error,
								  tsp00_VfReturn     *next_tape_error,
								  tsp00_ErrText       errtext);


externC void sqlfclose (tsp00_Int4        *hostfileno,
						tsp00_Bool               erase,
						tsp00_Int4         poolptr,
						tsp00_Int2         buf_count,
						char *    block,
						tsp00_VfReturn    *error,
						tsp00_ErrText      errtext);

externC void sqlfread (tsp00_Int4*               hostfileno,
					   char *    block,
					   tsp00_Int4*               length,
					   tsp00_VfReturn*          error,
					   tsp00_ErrText           errtext);

externC void sqlfwrite (tsp00_Int4*               hostfileno,
						char *    block,
						tsp00_Int4                length,
						tsp00_VfReturn*          error,
						tsp00_ErrText           errtext);

externC void sqlfseek (tsp00_Int4       plHostFileNo,
					   tsp00_Int4        lFilePos,
					   tsp00_VfReturn*  peError,
					   tsp00_ErrText   acErrorText);

externC void sqlftextseek (tsp00_Int4       plHostFileNo,
						   tsp00_Int4        lFilePos,
						   tsp00_VfReturn*  peError,
						   tsp00_ErrText   acErrorText);

externC void sqlftell (tsp00_Int4       plHostFileNo,
					   tsp00_Int4*       lFilePos,
					   tsp00_VfReturn*  peError,
					   tsp00_ErrText   acErrorText);

externC void sqlferase (tsp00_VFilename acFileName,
						tsp00_VfReturn*      peError,
						tsp00_ErrText       acErrorText);

/*	vos08uc:sqlexec */

externC void sqlexec (tsp00_ExecArgLine   pszCommand,
					  tsp00_ExecMode      eMode,
					  tsp00_ExecReturn   *peResult,
					  tsp00_ErrText       pszErrMsg,
					  tsp00_Int2         *pfProgResult);


/*	vos10uc:sqlread/sqlwrite */

externC void sqlread (tsp00_Line  acLine, tsp00_BoolAddr pbOk);

externC void sqlwrite (tsp00_Line acLine);

/*	vos14uc:sqltermid */

externC void sqltermid (tsp00_TermId acTerminalId);

/*	vos18uc:charactger set mapping */

externC void sqlcharsetname (tsp00_KnlIdentifier acCharSetName);


/*	vos50kc:kernel exception handling/exit */

externC void sqlabort (void);

externC void sqlarg3     (tsp04_XuserRecord  *prcUserParams,
                          tsp00_Pw            acPassword,
                          tsp4_args_options  *prcArgsOptions,
                          tsp4_xuserset       aucXuserType,
                          tsp00_ErrText       acErrorText,
                          tsp00_BoolAddr      pbOk            );

externC void sqlversion ( tsp00_Version      RteVersion);

externC void sqlgetpid (  tsp00_Int4         *pid);

#endif
