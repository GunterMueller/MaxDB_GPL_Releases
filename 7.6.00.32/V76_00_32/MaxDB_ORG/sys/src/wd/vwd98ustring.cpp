/*



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
///////////////////////////////////////////////////////////////////////////////
//
// ustring.cpp - Copyright 1997, Don Box
//
// This file contains the implementation of several conversion routines 
// that are used by the String816 class for duplicating/converting strings
// on the fly.
//
//     uxdup(const char *psz) - returns a new-ed wchar_t string based on psz
//     uxdup(const wchar_t *psz) - returns a new-ed char string based on psz
//     

#include <stdlib.h>

#include "gwd00.h"

#ifndef _USTRING_CPP
#define _USTRING_CPP

wchar_t *uxdup(const char *psz)
{
    size_t cch = strlen(psz) + 1;
    wchar_t *pwszResult = NULL;
    tsp00_Bool allocatOk;

    sapdbwa_SQLALLOCAT((tsp00_Int4)(cch * sizeof(wchar_t)),
                   (SAPDB_UInt1**) &pwszResult,
                   &allocatOk );

    if (allocatOk)
        mbstowcs(pwszResult, psz, cch);
    return pwszResult;
}

char *uxdup(const wchar_t *pwsz)
{
    size_t cch = wcslen(pwsz) + 1;
    char *pszResult = NULL;
    tsp00_Bool allocatOk;

    sapdbwa_SQLALLOCAT((tsp00_Int4)(cch * sizeof(char)),
                   (SAPDB_UInt1**) &pszResult,
                   &allocatOk );
    
    if (allocatOk)
        wcstombs(pszResult, pwsz, cch);
    return pszResult;
}

#endif
