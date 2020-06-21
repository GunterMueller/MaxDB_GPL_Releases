/*!**********************************************************************

  module: hwd28dict.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  ADT Dictionary. Stores key value pairs.

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

#ifndef HWD28DICT_H
#define HWD28DICT_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd11strseq.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: twd28DictionaryP

  description: A dictionary is used to store key/value pairs, where
               keys and values are strings.
  
************************************************************************/

typedef struct st_wa_dictionary *twd28DictionaryP;


/*!**********************************************************************
  Function: wd28CreateDictionary

  description: Create a new dictionary.

  arguments:
      
  return value: A pointer to the created dictionary object or NULL if
                memory allocation failed. */

twd28DictionaryP wd28CreateDictionary();


/*!**********************************************************************
  Function: wd28DestroyDictionary

  description: Destroy the dictionary (not the key/value pairs !!!).

  arguments:
    dict  [inout]  The dictionary to be destroyed.
    
  return value: void

  This function destroys the dictionary container and the key, value
  strings that where allocated in wd28AddCopyEntry. The key and value
  strings inserted by wd28AddEntry are not freed. */

void wd28DestroyDictionary( twd28DictionaryP dict );

/*!**********************************************************************
  Function: wd28AddCopyEntry

  description: Add one key/value pair to the dictionary. The key and
               value strings are copied.

  arguments:
    dict  [inout]  The dictionary.
    key   [in]     Pointer to key-string.
    value [in]     Pointer to value-string.
    
  return value: true for success. false if memory allocation failed.

  wd28AddEntry does not check for duplicated keys! So the caller has to
  check the existence of the key with wd28GetValue before inserting a
  new key/value pair.
  */

sapdbwa_Bool wd28AddCopyEntry( twd28DictionaryP  dict,
                             const char       *key,
                             const char       *value );

/*!**********************************************************************
  Function: wd28ReplaceCopyEntry

  description: Replace the value for a given key _and_ a given old value
               in the dictionary. The value string is copied. The entry
               must be created by wd28AddCopyEntry (because memory handling).

  arguments:
    dict     [inout]  The dictionary.
    key      [in]     Pointer to key-string.
    oldvalue [in]     Pointer to old value-string.
    value    [in]     Pointer to value-string.
    
  return value: true for success. false if entry not exists, the entry isn't
                a copied entry (created with wd28AddCopyEntry) or memory 
                allocation failed.

  */

sapdbwa_Bool wd28ReplaceCopyEntry( twd28DictionaryP  dict,
                                   const char       *key,
                                   const char       *oldvalue,
                                   const char       *value );

/*!**********************************************************************
  Function: wd28AddEntry

  description: Add one key/value pair to the dictionary. Only the
               string pointers are copied.

  arguments:
    dict  [inout]  The dictionary.
    key   [in]     Pointer to key-string.
    value [in]     Pointer to value-string.
    
  return value: true for success. false if memory allocation failed.

  wd28AddEntry does not check for duplicated keys! So the caller has to
  check the existence of the key with wd28GetValue before inserting a
  new key/value pair.
  */

sapdbwa_Bool wd28AddEntry( twd28DictionaryP  dict,
                         const char       *key,
                         char             *value );

/*!**********************************************************************
  Function: wd28FreeAllEntries

  description: All key/value pairs inserted with wd28AddCopyEntry OR
               AddEntry are freed, and removed from the dictionary.

  arguments:
    dict  [inout]  The dictionary.

  return-value: void */    

void wd28FreeAllEntries( twd28DictionaryP  dict );


/*!**********************************************************************
  Function: wd28GetValueByIndex

  description: Return the name and value for a given index.

  arguments:
    dict  [inout]  The dictionary.
    index [in]     Index of entry.
    key   [out]    Pointer to return key of entry
    value [out]    Pointer to return value of entry
    
  return value: true if entry withindex was found. If an entry
                with the given index does not exist in the dictionary
                false is returned and name and value are unchanged. */

sapdbwa_Bool wd28GetEntryByIndex( twd28DictionaryP  dict,
                                sapdbwa_UInt4       index,
                                const char      **key,
                                char            **value );


/*!**********************************************************************
  Function: wd28GetKeys

  description: Return all keys in the dictionary.

  arguments:
    dict   [inout]  The dictionary.
    strSeq [out]    String sequence in which the keys are returned.
    
  return value: Always true. */

sapdbwa_Bool wd28GetKeys( twd28DictionaryP  dict,
                        sapdbwa_StringSeqP      strSeq );


/*!**********************************************************************
  Function: wd28GetValueByKey

  description: Return the value for a given key.

  arguments:
    dict  [inout]  The dictionary.
    key   [in]     Pointer to key-string.
    
  return value: Pointer to value string if key was found. If key does not
                exist in the dictionary NULL is returned. */

char *wd28GetValueByKey( twd28DictionaryP dict, const char *key );


/*!**********************************************************************
  Function: wd28GetValuesByKey

  description: Return the value for a given key.

  arguments:
    dict   [inout]  The dictionary.
    key    [in]     Pointer to key-string.
    strSeq [out]    String sequence in which the values are returned.
    
  return value: Always true. */

sapdbwa_Bool wd28GetValuesByKey( twd28DictionaryP  dict,
                               const char       *key,
                               sapdbwa_StringSeqP      strSeq );


/*!**********************************************************************
  Function: wd28NumEntries

  description: Return the number of entries in the dictionary.

  arguments:
    dict  [inout]  The dictionary.
    
  return value: The number of entries. This is the lowest index for which
                wd28GetValueByIndex return NULL. */

sapdbwa_UInt4 wd28NumEntries( twd28DictionaryP dict );


/*!**********************************************************************
  Function: wd28RemoveEntryByKey

  description: Removes an entry with the given key.

  arguments:
    dict  [inout]  The dictionary.
    key   [in]     The key of the entry to delete.
    
  return value: true for success, false if no entry was found. */
    
sapdbwa_Bool wd28RemoveEntryByKey( twd28DictionaryP  dict,
                                 const char       *key );


/*!**********************************************************************
  Function: wd28RemoveEntryByIndex

  description: Removes an entry with the given key.

  arguments:
    dict  [inout]  The dictionary.
    index [in]     The index of the entry to delete.
    
  return value: true for success, false if no entry was found. */
    
sapdbwa_Bool wd28RemoveEntryByIndex( twd28DictionaryP  dict,
                                   sapdbwa_UInt4       index );

    
/*!**********************************************************************
  Function: wd28Reorganize

  description: Make dictionary compact (eliminate empty entries).

  arguments:
    dict  [inout]  The dictionary.
    
  return value: true for success, false if the dict is corrupted. */
    
sapdbwa_Bool wd28Reorganize( twd28DictionaryP  dict );
    
/*!**********************************************************************

  EndChapter: twd28DictionaryP

************************************************************************/
#if defined(__cplusplus)
}
#endif

#endif






