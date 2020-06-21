/*!**********************************************************************

  module: vwd91mime.c

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <stdio.h>

#include "hwd91mime.h"
#include "hwd28dict.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: prototypes for internal functions

************************************************************************/

sapdbwa_Bool wd91_LoadMimeTypes( twd91MimeTypesP mimeTypes );

/*!**********************************************************************

  EndChapter: prototypes for internal functions

************************************************************************/

/*!**********************************************************************

  Chapter: twd91MimeTypesP

************************************************************************/

struct st_wa_mime_types {
    char              fileName[MAX_FILENAME_LEN_WD00+1];
    twd28DictionaryP  mimeTypesDict;
}; /* struct st_wa_mime_types */


twd91MimeTypesP wd91CreateMimeTypes( const char *mimeTypeFile )
{
    twd91MimeTypesP  newMimeTypes;
    sapdbwa_Bool       allocatOK;

    if (strlen(mimeTypeFile)>=MAX_FILENAME_LEN_WD00) {
        /* log-filename too long */
        return NULL;
    }; /* if */
    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_mime_types),
                (sapdbwa_UInt1**) &newMimeTypes,
                &allocatOK );
    if (!allocatOK) {
        /* memory allocation error */
        return NULL;
    }; /* if */
    strcpy( newMimeTypes->fileName, mimeTypeFile );
    newMimeTypes->mimeTypesDict = wd28CreateDictionary();
    if (newMimeTypes->mimeTypesDict == NULL) {
        /* memory allocation failure */
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) newMimeTypes );
        return NULL;
    }; /* if */

    if (!wd91_LoadMimeTypes( newMimeTypes )) {
        wd91DestroyMimeTypes( newMimeTypes );
        return NULL;
    }; /* if */
    
    return newMimeTypes;
} /* wd32CreateMimeTypes */


void wd91DestroyMimeTypes( twd91MimeTypesP mimeTypes )
{
    /* free dictionary */
    wd28DestroyDictionary( mimeTypes->mimeTypesDict );
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) mimeTypes );
} /* wd91DestroyMimeTypes */


const char* wd91GetMimeType( twd91MimeTypesP  mimeTypes,
                             const char      *extension )
{
    return wd28GetValueByKey( mimeTypes->mimeTypesDict, extension );
} /* wd91GetMimeType */

/*!**********************************************************************

  EndChapter: twd91MimeTypesP

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

void wd91_SkipSpaces( char **pos )
{
    while (**pos != '\0' && isspace( **pos ))
        (*pos)++;
} /* wd91_SkipSpaces */


sapdbwa_Bool wd91_GetWord( char **pos, char *word )
{
    sapdbwa_UInt2 wordPos = 0;
    
    while (**pos != '\0' && !isspace( **pos )) {
        word[wordPos++] = **pos;
        (*pos)++;
    }; /* while */
    word[wordPos] = '\0';
    
    return (wordPos>0);
} /* wd91_GetWord */


#define MAX_EXT_LEN 255

sapdbwa_Bool wd91_InsertMimeTypes( twd91MimeTypesP  mimeTypes,
                                 char            *mimeType,
                                 char            *extensions )
{
    sapdbwa_Bool   allocatOK;
    char         newExtension[MAX_EXT_LEN+1];
    char        *extensionPos;
    char        *oldExtPos;
    sapdbwa_UInt2  extensionLen;
    
    oldExtPos = extensions;
    while ((extensionPos = strchr( oldExtPos, ',')) != NULL
           || (extensionPos = strchr( oldExtPos, '\0')) != NULL) {
        extensionLen = extensionPos - oldExtPos;
        if (extensionLen > 0 && extensionLen < MAX_EXT_LEN) {
            SAPDB_memcpy( newExtension, oldExtPos, extensionLen );
            newExtension[extensionLen] = '\0';
            if (!wd28AddCopyEntry( mimeTypes->mimeTypesDict,
                                   newExtension,
                                   mimeType )) {
                return false;
            }; /* if */
        } else {
            return false;
        }; /* else */
        if (*extensionPos == ',') {
            oldExtPos = extensionPos;
            oldExtPos++; /* skip ',' */
        } else { /* *extensionPos == '\0' */
            break;
        }; /* else */
    }; /* while */

    return true;
} /* wd91_InsertMimeTypes */

            
#define ENDOFLINE(pc)   (*pc=='\0')
#define COMMENT_SYMBOL  ('#')
#define MAX_MIME_LEN    255

sapdbwa_Bool wd91_LoadMimeTypes( twd91MimeTypesP mimeTypes )
{
    FILE *fd;
    char  line[MAX_MIME_LEN];
    char  mimeType[MAX_MIME_LEN];
    char  extensions[MAX_MIME_LEN];
    char *pos;
    
    fd = fopen( mimeTypes->fileName, "r" );
    if (fd == NULL) {
        /* file open error */
        return false;
    }; /* if */

    while ((pos = fgets( line, sizeof(line), fd ))) {
        wd91_SkipSpaces( &pos );
        if (!ENDOFLINE( pos ) && *pos != COMMENT_SYMBOL) {
            if (wd91_GetWord( &pos, mimeType )) {
                wd91_SkipSpaces( &pos );
                if (!ENDOFLINE( pos )) {
                    if (wd91_GetWord( &pos, extensions )) {
                        if (!wd91_InsertMimeTypes( mimeTypes,
                                                   mimeType,
                                                   extensions )) {
                            return false;
                        }; /* if */
                    }; /* if */
                }; /* if */
            }; /* if */
        }; /* if */
    }; /* while */
    fclose( fd );
    
    return true;
} /* wd91_LoadMimeTypes */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
