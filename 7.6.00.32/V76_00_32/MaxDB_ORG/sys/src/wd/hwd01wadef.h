/*!**********************************************************************

  module: hwd01wadef.h -> sapdbwa_define.h

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Özgen

  special area: SAPDB WebAgent definitions

  description:  Export description of WebAgent definitions

  see also:     

  -----------------------------------------------------------------------

						Copyright (c) 2000-2005 SAP AG



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




************************************************************************/

#ifndef SAPDBWADEFINE_H
#define SAPDBWADEFINE_H

#if defined(__cplusplus)
extern "C" {
#endif


typedef unsigned int   sapdbwa_UInt4;
typedef unsigned short sapdbwa_UInt2;
typedef unsigned char  sapdbwa_UInt1;
typedef short          sapdbwa_Int2;
typedef int            sapdbwa_Int4;
typedef unsigned char  sapdbwa_Bool;

/*--------------------------------------------------------------------------
declaration: sapdbwa_{ULong | Long}
description: basic long data types for fastest execution
             (machine-dependent size)
--------------------------------------------------------------------------*/
#if defined(WIN32) && !defined(_M_IX86)
typedef unsigned __int64    sapdbwa_ULong;      /* long (fast, unsigned) */
typedef signed __int64      sapdbwa_Long;       /* long (fast, signed)   */
#else
typedef unsigned long       sapdbwa_ULong;      /* long (fast, unsigned) */
typedef signed long         sapdbwa_Long;       /* long (fast, signed)   */
#endif

#define sapdbwa_True	((sapdbwa_Bool)1)
#define sapdbwa_False	((sapdbwa_Bool)0)

/* return codes for service function WebAgentApiService */
#define sapdbwa_RTC_Ignored		-1	/* The request was ignored, let the web server	*/
									/* handle it									*/
#define sapdbwa_RTC_Error		0	/* Abort request handling						*/
#define sapdbwa_RTC_Accepted	1	/* The request was accepted, no webbserver		*/
									/* request handling								*/
#define sapdbwa_RTC_Ok			2	/* Init/Exit function return code				*/


/*
 * HTTP Status codes for reply (HttpRep):
 */
#define sapdbwa_Status_Ok				200
#define sapdbwa_Status_Created			201
#define sapdbwa_Status_Response			204

#define sapdbwa_Status_Moveto 			301
#define sapdbwa_Status_Redirect			302
#define sapdbwa_Status_Modified			304

#define sapdbwa_Status_BadRequest		400
#define sapdbwa_Status_Unauthorized		401
#define sapdbwa_Status_Forbidden		403
#define sapdbwa_Status_NotFound			404
#define sapdbwa_Status_MethodNotAllowed	405
#define sapdbwa_Status_Conflict			409

#define sapdbwa_Status_ServerError		500
#define sapdbwa_Status_NotImplemented	501

typedef sapdbwa_Bool (*sapdbwa_Destructor)( void *userData );

/*
 * HTTP Status codes for reply (HttpRep): WebDAV specific
 */
#define sapdbwa_Status_Partial_Content						206			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
#define sapdbwa_Status_Multistatus							207
#define sapdbwa_Status_PreconditionFailed					412
#define sapdbwa_Status_UnsupportedMediaType					415
#define sapdbwa_Status_Requested_Range_Not_Satisfiable		416			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
#define sapdbwa_Status_Locked								423
#define sapdbwa_Status_BadGateway							502
#define sapdbwa_Status_InsufficentStorage					507


/****************************************************************************
 ***
 *** Directories
 ***
 ****************************************************************************/

#define SAPDBWA_DOCUMENT_ROOT_HTML			"HTML"
#define SAPDBWA_DOCUMENT_ROOT_IMAGES		"Images"
#define SAPDBWA_DOCUMENT_ROOT_JAVA_SCRIPTS	"JavaScripts"
#define SAPDBWA_DOCUMENT_ROOT_STYLES		"Styles"

/*******************************
 * platform specific settings
 *******************************/

#ifndef WINAPI
#ifdef _WIN32
#define WINAPI __stdcall
#else
#define WINAPI
#endif
#endif

#if defined(__cplusplus)
}
#endif
    
#endif
