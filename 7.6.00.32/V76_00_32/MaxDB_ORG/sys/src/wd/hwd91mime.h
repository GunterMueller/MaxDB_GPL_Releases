/*!**********************************************************************

  module: hwd91mime.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Mime type management

  see also:

  -----------------------------------------------------------------------

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




************************************************************************/

#ifndef HWD91MIME_H
#define HWD91MIME_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

/*!**********************************************************************

  Chapter: twd91MimeTypesP

  description:
  
************************************************************************/

typedef struct st_wa_mime_types *twd91MimeTypesP;

twd91MimeTypesP wd91CreateMimeTypes( const char *mimeTypeFile );

void wd91DestroyMimeTypes( twd91MimeTypesP mimeTypes );

const char* wd91GetMimeType( twd91MimeTypesP  mimeTypes,
                             const char      *extension );

/*!**********************************************************************

  EndChapter: twd91MimeTypesP

************************************************************************/
#endif
