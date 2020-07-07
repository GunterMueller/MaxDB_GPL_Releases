/* @lastChanged: "1998-05-19  16:25"
 * @filename:    heo02.h
 * @filename:   heo02.h
 * @purpose:    "Interface for RTE functions, used by c++ modules"
 * @release:    7.1.0.0
 * @see:        "http://www.bea.sap-ag.de/doku/rel-7.1/eo/heo02.html"
 *
 * @Copyright (c) 1997-2005 SAP AG"


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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


#ifndef HEO02
#define HEO02


/******************************************
 *            I N C L U D E S             *
 ******************************************/

#if !defined(__cplusplus)
#ERROR This header file is used by C++ modules only. Use heo102.h instead
#endif

#include "gsp00.h"
#include "vsp004.h"
#include "geo572.h" /* sqlfree(), sqlallocat() */

/******************************************
 *        D E K L A R A T I O N S         * 
 ******************************************/

externC void sqlfinish (
	char					terminate);



externC void sqlabort ();

 
 
/*------------------------------*/ 
 
/* deleted by Bernd Vorsprach - prototype now in geo572.h
externC void sqlallocat (
	tsp00_Int4				length,
	tsp00_ObjAddr*			p,
	tsp00_Bool*  		    ok); */

 
/*------------------------------*/ 
 

/* deleted by Bernd Vorsprach - prototype now in geo572.h
externC void sqlfree (
	tsp00_BufAddr				p); */

 
/*------------------------------*/ 
 

externC void sqlargl (
	tsp00_Line&				args);				/* tsp00_Array<char, mxsp_c132> */

 
/*------------------------------*/ 
 

externC void sqlarg3 (
	tsp4_xuser_record*		user_params,
	tsp00_Pw&					password,			/* tsp00_Array<char, mxsp_c18> */
	tsp4_args_options*		options,
	tsp4_xuserset&			xusertype,			/* tsp00_Set<tsp4_xuser_Enum, ...> */
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlcharsetname (
	tsp00_KnlIdentifier&		charsetname);		/* tsp00_Array<char, mxsp_c64> */

 
/*------------------------------*/ 

externC void sqlclock (
	tsp00_Int4*				sec,
	tsp00_Int4*				microsec);

 
/*------------------------------*/ 
 

externC void sqlconcat (
	tsp00_VFilename&			first_part,			/* tsp00_Array<char, mxsp_c64> */
	tsp00_VFilename&			second_part,		/* tsp00_Array<char, mxsp_c64> */
	tsp00_VFilename&			resconcat,			/* tsp00_Array<char, mxsp_c64> */
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqldattime (
	tsp00_Date&				d,					/* tsp00_Array<char, mxsp_c8> */
	tsp00_Time&				t);					/* tsp00_Array<char, mxsp_c8> */

 
/*------------------------------*/ 
 

externC void SqlDevSize (
	const char             *dev_name,			/* tsp00_Array<char, mxsp_c64> */
	tsp00_Int4*				devcapacity,
	tsp00_ErrTextc&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlexec (
	tsp00_ExecLine&			command,			/* tsp00_Array<char, mxsp_c512> */
	tsp00_ExecMode_Enum		mode,				
	tsp00_ExecReturn*		error,				/* tsp00_Enum<tsp_exec_return_Enum, ...> */
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_Int2*				commandresult);

 
/*------------------------------*/ 
 

externC void sqlgetpid (
	tsp00_Int4*				pid);

 
/*------------------------------*/ 

externC void sqlinit (
	tsp00_CompName&			component,			/* tsp00_Array<char, mxsp_c64> */
	tsp00_Bool*             ok);

 
/*------------------------------*/ 
 

externC void sqlos (
	tsp00_Os*					os);				/* tsp00_Enum<tsp_os_Enum, ...> */

 
/*------------------------------*/ 
 

externC void sqlpoff (
	char					print,
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlpon (
	tsp00_PrtName&			printer,			/* tsp00_Array<char, mxsp_c64> */
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlprint (
	tsp00_PrtLine&			line,				/* tsp00_Array<char, mxsp_c256> */
	tsp00_Int2				length,
	tsp00_VpLinefeeds_Enum	lfeeds,
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 
externC void sqlread (
	tsp00_Line&				text,				/* tsp00_Array<char, mxsp_c132> */
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 
/* deleted by Bernd Vorsprach - prototype now in geo572.h
externC void sqlreallocat (
	tsp00_Int4				length,
	tsp00_ObjAddr*			p,
	tsp00_Bool*				ok); */

 
/*------------------------------*/ 
 

externC void sqlresult (
	tsp00_Uint1				result);

 
/*------------------------------*/ 
 

externC void sqlsleep (
	tsp00_Int2				duration);

 
/*------------------------------*/ 
 

externC void sqltermid (
	tsp00_TermId&				terminalid);		/* tsp00_Array<char, mxsp_c18> */			

 
/*------------------------------*/ 
 

externC void sqltermvar (
	tsp00_C18&				term_type);			/* tsp00_Array<char, mxsp_c18> */

 
/*------------------------------*/ 
 

externC void sqlthold (
	tsp00_Int4				term_ref,
	char					manual_input);

 
/*------------------------------*/ 
 

externC void sqltio (
	tsp00_Int4				term_ref,
	tsp00_ScreenBufAddr*		screen_buf,
	tsp00_ScreenBufAddr*		attribute_buf,
	char					refresh,
	tsp00_Int2*				cursorline,
	tsp00_Int2*				cursorcolumn,
	tsp00_HifParms*			hif,
	tsp00_VtOptions*			options,
	tsp00_VtResult*			result,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqltoff (
	tsp00_Int4				term_ref,
	tsp00_ScreenBufAddr*		screen_buf,
	tsp00_ScreenBufAddr*		attribute_buf,
	tsp00_VtMsg&				msg);				/* tsp00_Array<char, mxsp_c80> */

 
/*------------------------------*/ 
 

externC void sqlton (
	char					multi_task,
	tsp00_Int4*				term_ref,
	tsp00_TerminalDescription*	desc,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqltrelease (
	tsp00_Int4				term_ref);

 
/*------------------------------*/ 
 

externC void sqlttable (
	tsp00_Int2				i,
	tsp00_VtAttrib&			attrib,				/* tsp00_Set<tsp_vt_mode_Enum, ...> */
	tsp00_VtColor&			foreground,			/* tsp00_Set<tsp_vt_color_Enum, ...> */
	tsp00_VtColor&			background);		/* tsp00_Set<tsp_vt_color_Enum, ...> */

 
/*------------------------------*/ 
 

externC void sqlversion (
	tsp00_C40&				rte_version);		/* tsp00_Array<char, mxsp_c40> */

 
/*------------------------------*/ 
 

externC void sqluid (
	tsp00_TaskId*			uid);

 
/*------------------------------*/ 
 

externC void sqlwdgclear (
	tsp00_Int2				box_id);

 
/*------------------------------*/ 
 

externC void sqlwdgget (
	tsp00_Int2				box_id,
	tsp00_Int2				item_id,
	tsp00_C256&				text,				/* tsp00_Array<char, mxsp_c256> */
	tsp00_Int2*				text_len,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlwdgset (
	tsp00_Int2				box_id,
	tsp00_Int2				item_id,
	tsp00_C256&				text,				/* tsp00_Array<char, mxsp_c256> */
	tsp00_Int2				text_len,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlwdgswitch (
	tsp00_Int2				box_id,
	tsp00_VtKeys&			trigger,			/* tsp00_Set<tsp_vt_key_Enum, ...> */
	tsp00_VtKey_Enum		return_key,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlwindow (
	tsp00_Int4				parent_ref,
	tsp00_VtRectangle*			position,
	tsp00_TerminalDescription*	desc,
	tsp00_Int4*				term_ref,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlwoff (
	tsp00_Int4				term_ref,
	tsp00_ScreenBufAddr*		screen_buf,
	tsp00_ScreenBufAddr*		attribute_buf);

 
/*------------------------------*/ 
 

externC void sqlwrite (
	tsp00_Line&				text);				/* tsp00_Array<char, mxsp_c132> */

 
/*------------------------------*/ 
 

externC void sqlwscbar (
	tsp00_Int4				term_ref,
	tsp00_Int2				screen_nr,
	tsp00_VtScroll_Enum		t,
	tsp00_Int2				range_beg,
	tsp00_Int2				range_end,
	tsp00_Int2				pos,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlwscroll (
	tsp00_Int4				term_ref,
	tsp00_VtRectangle*			rect,
	tsp00_Int2				displ_x,
	tsp00_Int2				displ_y,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlwsplit (
	tsp00_Int4				term_ref,
	tsp00_Int2				nr_screens,
	tsp00_TerminalDescription*	desc,
	tsp00_Int2*				offset,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 
 

externC void sqlwtitle (
	tsp00_Int4				term_ref,
	tsp00_Int2				screen_nr,
	tsp00_C256&				text,				/* tsp00_Array<char, mxsp_c256> */
	tsp00_Int2				text_len,
	tsp00_Bool*				ok);

 
/*------------------------------*/ 

externC void sqlget_config_path (  
   tsp00_C256&       configpath  ); /* tsp00_Array<char, mxsp_c256> */

/*------------------------------*/ 

externC void sqlget_wrk_path (  
   tsp00_C256&       wrkpath  );    /* tsp00_Array<char, mxsp_c256> */

#endif /*HEO02*/	
