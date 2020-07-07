/* @lastChanged: "1998-06-26  12:01"
 * @filenameheo46heo46heo46.h
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


#ifndef HEO46_H
#define HEO46_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#include "gsp00.h"
#include "gsp01.h"

externC void sqlos_errortxtc                ( char            *ErrorText,
                                              int              ErrorTextLen ) ;

externC void sqlos_errtext_by_errcode       ( int              ErrorCode,          
                                              char            *ErrorText,
                                              int              ErrorTextLen ) ;

externC void sqlos_errcode_and_errtext      ( int             *ErrorCode,          
                                              char            *ErrorText,
                                              int              ErrorTextLen ) ;

externC char *eo46_rte_errtext_with_filename( const char      *ErrTxt,
                                              const char      *Filename,
                                              char            *FullTxt,
                                              int              FullTxtLen ) ;

externC char *eo46_system_error_text        ( int              ErrorCode,
                                              char            *ErrorText,
                                              int              ErrorTextLen ) ;

externC void eo46_rte_error_init            ( tsp01_RteError  *RteError);

externC void eo46_set_rte_error             ( tsp01_RteError  *RteError,
                                              int              OsErrCode,
                                              const char      *ErrText,
                                              const char      *Argument);

externC void eo46PtoC                       ( char                          *CStr,
                                              tsp00_Byte const              *PascalStr,
                                              int                           Len );

externC void eo46CtoP                       ( tsp00_Byte                    *PascalStr,
                                              char const                    *CStr,
                                              int                           Len );

externC void  eo46BuildPascalErrorStringRC  ( tsp00_Byte                    *pcPascalErrText,
                                              char const                    *pszErrText,
                                              teo00_Int4                    rc );

externC void  eo46BuildErrorStringRC        ( tsp00_ErrTextc VAR_ARRAY_REF  pcErrText,
                                              char const                   *pszErrText,
                                              teo00_Int4                    rc );

externC void  sql46c_build_error_string     ( char                         *pcPascalErrText,
                                              char const                   *pszErrText,
                                              teo00_Int4                   rc );
#endif
