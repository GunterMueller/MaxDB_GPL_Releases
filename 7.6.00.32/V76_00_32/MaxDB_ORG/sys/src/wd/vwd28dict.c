/*!**********************************************************************

  module: vwd28dict.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  ADT Dictionary

  see also:

  -----------------------------------------------------------------------

  copyright:              Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include <string.h>

#include "hwd28dict.h"

/*!**********************************************************************

  Chapter: prototypes for internal functions

************************************************************************/

typedef struct st_wa_dict_entry {
    char       *key;
    char       *value;
    sapdbwa_Bool  copied;
} twd28DictEntry;


sapdbwa_Bool wd28_ReallocEntries( twd28DictionaryP dict );

void wd28_FreeEntry( twd28DictEntry *entry );

/*!**********************************************************************

  Chapter: twd28DictionaryP

************************************************************************/

#define MAX_KEY_LEN 255
#define MAX_VALUE_LEN 1024

struct st_wa_dictionary {
    twd28DictEntry*  entries;
    sapdbwa_UInt4      usedEntries;
    sapdbwa_UInt4      allocEntries;
}; /* struct st_dictionary */


twd28DictionaryP wd28CreateDictionary()
{
    sapdbwa_Bool        allocatOK;
    twd28DictionaryP  newDict;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_dictionary ),
                (sapdbwa_UInt1**) &newDict,
                &allocatOK );
    if (allocatOK) {
        newDict->entries = NULL;
        newDict->usedEntries = 0;
        newDict->allocEntries = 0;
    } else {
        newDict = NULL;
    }; /* else */

    return newDict;
} /* wd28CreateDictionary */


void wd28DestroyDictionary( twd28DictionaryP dict )
{
    wd28FreeAllEntries( dict );
    if (dict->entries != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) dict->entries );
    }; /* if */

    sapdbwa_SQLFREE( (sapdbwa_UInt1*) dict );
} /* wd28DestroyDictionary */


sapdbwa_Bool wd28AddCopyEntry( twd28DictionaryP  dict,
                             const char       *key,
                             const char       *value )
{
    sapdbwa_Bool retcode = true;
    sapdbwa_Bool allocatOK = true;
    
    if (dict->usedEntries+1 > dict->allocEntries) {
        retcode = wd28_ReallocEntries( dict );
    }; /* if */
    
    if (retcode) {
        sapdbwa_SQLALLOCAT( strlen( key )+1,
                       (sapdbwa_UInt1**) &dict->entries[ dict->usedEntries ].key,
                       &allocatOK );
        if (allocatOK) {
            sapdbwa_SQLALLOCAT( strlen( value )+1,
                           (sapdbwa_UInt1**)
                           &dict->entries[ dict->usedEntries ].value,
                           &allocatOK );
        }; /* if */
        if (allocatOK) {
            strcpy( (char*) dict->entries[ dict->usedEntries ].key,
                    (char*) key );
            strcpy( (char*) dict->entries[ dict->usedEntries ].value,
                    (char*) value );
            dict->entries[ dict->usedEntries ].copied = true;
            dict->usedEntries++;
        } else {
            retcode = false;
        }; /* else */
    }; /* if */

    return retcode;
} /* wd28AddCopyEntry */

sapdbwa_Bool wd28ReplaceCopyEntry( twd28DictionaryP  dict,
                                   const char       *key,
                                   const char       *oldvalue,
                                   const char       *value )
{
    sapdbwa_Bool  retcode   = true;
    sapdbwa_Bool  allocatOK = true;
    sapdbwa_UInt4 iEntry;

    /* search entry by key _and_ value */
    for (iEntry = 0; iEntry < dict->usedEntries; iEntry++ ) {
        if (dict->entries[iEntry].key != NULL) {
            if (strcmp( dict->entries[iEntry].key, key ) == 0) {
                if (dict->entries[iEntry].value != NULL) {
                    if (strcmp( dict->entries[iEntry].value, oldvalue ) == 0) {
                        break;
                    }; /* if */
                }; /* if */
            }; /* if */
        }; /* if */
    }; /* for */

    if (iEntry == dict->usedEntries) {
        /* entry not found */
        retcode = false;
    } else {
        if (dict->entries[ iEntry ].copied) {
            /* free old value buffer */
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) dict->entries[ iEntry ].value );

            /* allocat new value buffer */
            sapdbwa_SQLALLOCAT( strlen( value )+1,
                                (sapdbwa_UInt1**)
                                &dict->entries[ iEntry ].value,
                                &allocatOK );
            if (allocatOK) {
                strcpy( (char*) dict->entries[ iEntry ].value,
                        (char*) value );
            } else {
              retcode = false;
            }; /* else */
        } else {
            retcode = false;
        } /* else */
    } /* else */

    return retcode;
} /* wd28ReplaceCopyEntry */

sapdbwa_Bool wd28AddEntry( twd28DictionaryP  dict,
                         const char       *key,
                         char             *value )
{
    sapdbwa_Bool retcode = true;

    if (dict->usedEntries+1 > dict->allocEntries) {
        retcode = wd28_ReallocEntries( dict );
    }; /* if */
    
    if (retcode) {
        dict->entries[ dict->usedEntries ].key   = (char*) key;
        dict->entries[ dict->usedEntries ].value = value;
        dict->entries[ dict->usedEntries ].copied = false;
        dict->usedEntries++;
    }; /* if */

    return retcode;
} /* wd28AddEntry */

void wd28FreeAllEntries( twd28DictionaryP  dict )
{
    sapdbwa_UInt4 iEntry;

    for (iEntry = 0; iEntry < dict->usedEntries; iEntry++ ) {
        if (dict->entries[iEntry].key != NULL) {
            wd28_FreeEntry( &dict->entries[iEntry] );
        };
    }; /* for */
    dict->usedEntries = 0;
} /* wd28FreeAllEntries */


sapdbwa_Bool wd28GetKeys( twd28DictionaryP  dict,
                        sapdbwa_StringSeqP     strSeq )
{
    sapdbwa_UInt4 iEntry;

    for (iEntry = 0; iEntry < dict->usedEntries; iEntry++ ) {
        /* no duplicates */
        if (dict->entries[iEntry].key != NULL) {
            if (!wd11ContainsString( strSeq, dict->entries[iEntry].key )) {
                wd11InsertString( strSeq, dict->entries[iEntry].key );
            }; /* if */
        }; /* if */
    }; /* for */

    return true;
} /* wd28GetKeys */


sapdbwa_Bool wd28GetEntryByIndex( twd28DictionaryP  dict,
                                sapdbwa_UInt4       index,
                                const char      **key,
                                char            **value )
{
    if (index < dict->usedEntries) {
        *key = dict->entries[index].key;
        *value = dict->entries[index].value;
        return true;
    } else {
        return false;
    }; /* else */
} /* wd28GetValueByIndex */


char *wd28GetValueByKey( twd28DictionaryP dict, const char *key )
{
    sapdbwa_UInt4 iEntry;

    for (iEntry = 0; iEntry < dict->usedEntries; iEntry++ ) {
        if (dict->entries[iEntry].key != NULL) {
            if (strcmp( dict->entries[iEntry].key, key ) == 0) {
                return dict->entries[iEntry].value;
            }; /* if */
        }; /* if */
    }; /* for */

    return NULL;
} /* wd28GetValueByKey */


sapdbwa_Bool wd28GetValuesByKey( twd28DictionaryP  dict,
                               const char       *key,
                               sapdbwa_StringSeqP     strSeq )
{
    sapdbwa_UInt4 iEntry;

    for (iEntry = 0; iEntry < dict->usedEntries; iEntry++ ) {
        if ( dict->entries[iEntry].key != NULL
             && strcmp( dict->entries[iEntry].key, key ) == 0) {
            wd11InsertString( strSeq, dict->entries[iEntry].value );
        }; /* if */
    }; /* for */

    return true;
} /* wd28GetValuesByKey */


sapdbwa_UInt4 wd28NumEntries( twd28DictionaryP dict )
{
    return dict->usedEntries;
} /* wd28NumEntries */


sapdbwa_Bool wd28RemoveEntryByKey( twd28DictionaryP  dict,
                                 const char       *key )
{
    sapdbwa_UInt4 iEntry;
    sapdbwa_Bool  found = false;
    
    for (iEntry = 0; iEntry < dict->usedEntries; iEntry++ ) {
        if ( dict->entries[iEntry].key != NULL ) {
            if (strcmp( dict->entries[iEntry].key, key ) == 0) {
                found = true;
                wd28_FreeEntry( &dict->entries[iEntry] );
                if (dict->usedEntries == iEntry+1) {
                    dict->usedEntries--;
                };
            }; /* if */
        }; /* if */
    }; /* for */

    return found;
} /* wd28RemoveEntryByKey */


sapdbwa_Bool wd28RemoveEntryByIndex( twd28DictionaryP  dict,
                                   sapdbwa_UInt4       index )
{
    sapdbwa_Bool retcode = true;
    
    if (index < dict->usedEntries) {
        wd28_FreeEntry( &dict->entries[index] );
        if (dict->usedEntries == index+1) {
            dict->usedEntries--;
        };
    } else {
        retcode = false;
    }; /* else */

    return retcode;
} /* wd28RemoveEntryByIndex */


sapdbwa_Bool wd28Reorganize( twd28DictionaryP  dict )
{
    sapdbwa_UInt4 iEntry;
    sapdbwa_UInt4 jEntry;
    
    for (iEntry = 0; iEntry < dict->usedEntries; iEntry++ ) {
        if ( dict->entries[iEntry].key == NULL ) {
            /* find entry != NULL starting at the end */
            jEntry = dict->usedEntries-1;
            while (dict->entries[jEntry].key == NULL && jEntry > iEntry) {
                jEntry--;
            };
            /* copy entry from the end into the "hole" */
            dict->entries[iEntry] = dict->entries[jEntry];
            dict->entries[jEntry].key    = NULL;
            dict->entries[jEntry].value  = NULL;
            dict->entries[jEntry].copied = false;
            dict->usedEntries = jEntry;
        };
    }; /* for */

    return true;
} /* wd28Reorganize */

/*!**********************************************************************

  EndChapter: twd28DictionaryP

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

void wd28_FreeEntry( twd28DictEntry *entry )
{
    if (entry->copied) {    
        if (entry->key != NULL) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) entry->key );
        }
        if (entry->value != NULL) {        
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) entry->value );
        };
    };
    entry->copied = false;
    entry->key = NULL;
    entry->value = NULL;
} /* wd28_FreeEntry */

#define ALLOC_INC 4

sapdbwa_Bool wd28_ReallocEntries( twd28DictionaryP dict )
{
    sapdbwa_Bool allocatOK;

    if (dict->entries == NULL) {
        sapdbwa_SQLALLOCAT( ALLOC_INC * sizeof( twd28DictEntry ),
                       (sapdbwa_UInt1**) &dict->entries,
                       &allocatOK );
        if (allocatOK) {
            dict->allocEntries = ALLOC_INC;
        }; /* if */
    } else {
        sapdbwa_SQLREALLOCAT( (dict->allocEntries+ALLOC_INC)
                         * sizeof( twd28DictEntry ),
                         (sapdbwa_UInt1**) &dict->entries,
                         &allocatOK );
        if (allocatOK) {
            dict->allocEntries += ALLOC_INC;
        }; /* if */
    }; /* else */

    return allocatOK;
} /* wd28ReallocEntries */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
