/*!**************************************************************************

  module      : vpr09HashTable.h

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Implements a hash table.

  This modul implements a hashtable, which maps keys to values. Any non-null
  strings can be used as a key. The hashtable stores pointer to hashed objects
  (structures).

  To successfully store and retrieve objects from a hashtable, the hashtable
  stores a reference to the equals method and hashfunction. If no user defined
  equals method and hashfunction was assigned, the default functions will be
  used.

  An instance of Hashtable has two parameters that affect its performance,
  capacity and load factor. The capacity is the number of buckets
  in the hash table.

  In the case a "hash collision", a single  bucket stores multiple entries,
  which must be searched sequentially (linked list).

  The load factor is a measure of how full the hash table is allowed to get
  before its capacity reached. When the number of entries in the hashtable exceeds
  the product of the load factor and the current capacity, no more objects can be
  inserted.

  Generally, the default load factor (.75) offers a good tradeoff between
  time and space costs. Higher values decrease the space overhead but increase
  the time cost to look up an entry (which is reflected in most Hashtable
  operations, including get and put).

  The capacity controls the maximum count of keys to value pairs, which
  can stored in the hashtable. However, setting the capacity too high can
  waste space.

  last changed: 2000-01-26
  see also    :

  -------------------------------------------------------------------------



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




*****************************************************************************/


#ifndef VPR09HashTable_H
#define VPR09HashTable_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gpr01.h"
#include "vpr09LinkedList.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define DEFAULT_HASHTABLE_SIZE 1000
#define DEFAULT_HASHTABLE_LOADFACTOR 75

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
  Decleration: tpr06_StmtCacheError
  Description: statement cache error codes
 */
typedef enum tpr09_HashTableError
{
  ok_epr09,                 /* Processing sucessfull */
  AllocMemErr_epr09,        /* failed to allocate sufficient memory */
  CacheInconsistant_epr09,  /* cache inconsistent */
  InvalidConHdl_epr09,      /* invalid connection handle*/
  HashTableFull_epr09,      /* hashtable full*/
  CollisionLstFull_epr09,   /* no more collision list items can allocated*/
  InvalidHashIndex_epr09    /* hash index is invalid*/
} tpr09_HashTableError;

/*!
  Decleration: tpr09_HTRehash
  Description: rehash flags
 */
typedef enum tpr09_HTRehash
{
  rehash_enable  = 1,
  rehash_disable = 0
} tpr09_HTRehash;

/*!
  Decleration: tpr09HashItem
  Description: hashtable element

  The implemented hashtable stores instances of tpr09HashItem`s. A hashitem
  have a data_p pointer, which should points to the hashed user data.

  NOTE, that only the adress of hashed user data will stored in hashtable. The
  user data self must be stored in a own container.
*/
typedef tpr09LinkedListItem tpr09HashItem;
/*!
  Decleration: tpr09HashKey
  Description: non-null bytearray which will be used to generate hashindex
*/
typedef void* tpr09HashKey;

/*!
  Decleration: tpr09HashIndex
  Description: index into hashtable

  The hashindex is an index into hashtable, which points direct to a bucket.
  Normally the hashfunction will be used to compute the hashindex. But if you
  know the hashindex of searched data, you can use the hashindex directly.
  This will increase the performance of searching data in hashtable
*/
typedef tsp00_Uint4 tpr09HashIndex;

/*!
  Decleration: tpr09HashValue
  Description: pointer to hashed object

  The hashtable stores only pointer to hashed objects. This type should be
  used for the handling of hashed objects.
*/
typedef void* tpr09HashValue;


/*!
  Decleration: tpr09PrintHashItem
  Description: callback to print out the hash value
*/
typedef void (*tpr09PrintHashItem) (const tpr09HashValue data_p, FILE* datei);


/*!
  Decleration: tpr09ExtractKey
  Description: pointer to extract key function

  Retrieves the key member from the data object used for compare

*/
typedef tpr09HashKey (*tpr09ExtractKey) (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen);

/*!
  Decleration: tpr09CompareMethod
  Description: pointer to callback function for comparsion of two Hashvalues

  To find data in hashtable the hashed objects must be compared. You should use
  a own compare method, that is customized to the objects you would to hash. The
  function gets two hashvalues as input. They must return zero, if hashvalues are
  not equal und a value greater zero if hashvalues are equal.
*/
typedef tsp00_Int4 (*tpr09CompareMethod) (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len);

/*!
  Decleration: tpr09HashFunc
  Description: pointer to hash function
*/
typedef tpr09HashIndex (*tpr09HashFunc) (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen);

/*!
  Decleration: tpr09CacheStat
  Description: statistics on hashtable

  Additionally a hashtable stores statistic informations for performance
  analysis that will be stored in this structure.
*/
typedef struct tpr09CacheStat
{
  tsp00_Longuint      requests;   /* number of requests against hashtable */
  tsp00_Longuint      founds;     /* number of hashvalues found in hashtable */
  tsp00_Longuint      inserts;    /* number of inserted values */
  tsp00_Longuint      deletes;    /* number of deleted values */
  tsp00_Longuint      collisions; /* number of collisopns */
  tsp00_Int4       weightedHitrate; /*weighted hitrate 33 Proz alter Wert 66 Proz neuer Wert*/
} tpr09CacheStat;

/*!
  Decleration: tpr09HashTableFreeLst
  Description: free memory list for building collision list

  The "hashtable free list" is used to extend the hash table with collision
  lists in case if necessary. It is organized as a set of HASH_ITEM chunks
  which will be allocated dynamically if needed.
*/
#define HASH_FL_CHUNK_SIZE	(4096/sizeof(tpr09HashItem))*sizeof(tpr09HashItem)
#define HASH_FL_CHUNK_INC_CNT	6
typedef struct tpr09HashTableFreeLst
{
   tpr09HashItem ** chunks; /* pointers to HASH_ITEM arrays which will be allocated dynamically      */
   tpr09HashItem * first_p; /* pointer to the first element of the hash free list */
   tsp00_Uint4	 cbChunkCnt;
} tpr09HashTableFreeLst;

typedef struct tpr09HashTableIterator
{
   tsp00_Uint4	   cbCurElem;
   tpr09HashItem * pElem;
} tpr09HashTableIterator;
/*!
  Decleration: tpr09HashTable
  Description: basis structure for hashtable

*/
typedef struct {
  tsp00_Int4 cbCLIncCount;   /*number of elements, which will be allocated
                               in one chunk. Used for collision list */
  tsp00_Uint4 cbActElem;      /*number of current used elements */
  tsp00_Uint4 cbMaxElem;      /*number of maximal elements*/
  tsp00_Uint4 cbLoadFactor;   /*measure of how full the hash table is allowed to get
                               before its capacity is reached value between 0 and 100*/
  tsp00_Uint4 cbMaxCapacity;  /*maximum capacity of hashtable*/
  tpr09_HTRehash rehash;      /*signals whether rehashing is enabled or disabled*/
  tpr09CompareMethod CompareMethod; /*method for comparsion of two Hashvalues*/
  tpr09HashFunc      HashFunc;    /*pointer to hash function*/
  tpr09ExtractKey    KeyFunc;     /*pointer to key extract function*/
  tpr09PrintHashItem PrintFunc;   /*pointer to dump function*/
  tpr09CacheStat statistic;  /*statistic data of hash table*/
  tpr09HashItem *pHashArray; /*pointer to array with hashed key value pairs */
  tpr09HashTableFreeLst HashTableFreeLst; /**/
  tpr09HashTableIterator iterator;
} tpr09HashTable;

/*!
  Function: pr09HTCreateHashTable

  see also:

  Description: Constructs a new, empty hashtable with the specified
               capacity and the specified load factor.

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       capacity       [in]     capacity of hashtable
       LoadFactor     [in]     load factor of hashtable
       CompareMethod  [in]     pointer to a compare function (for 2 hash values)
       HashFunc       [in]     pointer to a hash

  returns: tpr09_HashTableError, if an error occurred
 */
tpr09_HashTableError pr09HTCreateHashTable(tpr09HashTable*    HashTable,
                                           tsp00_Int4         capacity,
                                           tsp00_Int4         LoadFactor,
                                           tpr09_HTRehash     rehash,
                                           tpr09CompareMethod CompFunc,
                                           tpr09HashFunc      HashFunc,
                                           tpr09ExtractKey    KeyFunc,
                                           tpr09PrintHashItem PrintFunc);

/*!
  Function: pr09HTDeleteHashTable

  see also:

  Description: Deletes the hashtable

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable

  returns: void
 */
void pr09HTDeleteHashTable(tpr09HashTable * HashTable);

/*!
  Function: pr09HTClearHashTable

  see also:

  Description: Clears this hashtable so that it contains no keys

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable

  returns: void
 */
void pr09HTClearHashTable (tpr09HashTable * HashTable);


/*!
  Function: pr09HTContainsByKey

  see also:

  Description: Tests if some key/value pairs maps into the a specified entry in this hashtable

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key

  returns: 0 if hash table don`t contains the key/value pair
           1 if key/value pair founds in the table
 */
tsp00_Int4 pr09HTContainsByKey (tpr09HashTable* HashTable,
                                tpr09HashKey    HashKey,
                                tsp00_Uint4      cbHashKeyLen);


/*!
  Function: pr09HTContainsByIndex

  see also:

  Description: Tests if some Index/value pairs maps into the a specified entry in this hashtable

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashIndex      [in]     index into hashtable
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key

  returns: 0 if hash table don`t contains the key/value pair
           1 if key/value pair founds in the table
 */
tsp00_Int4 pr09HTContainsByIndex (tpr09HashTable * HashTable,
                                  tpr09HashIndex HashIndex,
                                  tpr09HashKey   HashValue,
                                  tsp00_Uint4    cbHashKeyLen);

/*!
  Function: pr09HTGetHashItemByKey

  see also:

  Description: Returns the hashitem with the specified key/value pair from hashtable

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key

  returns: NULL      - if nothing found
           HashItem  - the specified hashitem
 */
tpr09HashValue  pr09HTGetHashItemByKey (tpr09HashTable * HashTable,
                                        tpr09HashKey   HashKey,
                                        tsp00_Uint4    cbHashKeyLen);

/*!
  Function: pr09HTGetHashItemByIndex

  see also:

  Description: Returns the hashitem with the specified index/value pair from hashtable

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashIndex      [in]     index into hashtable for searching the value
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key

  returns: NULL      - if nothing found
           HashItem  - the specified hashitem
 */
tpr09HashValue  pr09HTGetHashItemByIndex (tpr09HashTable * HashTable,
                                          tpr09HashIndex HashIndex,
                                          tpr09HashKey   HashValue,
                                          tsp00_Uint4    cbHashKeyLen);

/*!
  Function: pr09HTInsertHashItemByValue

  see also:

  Description: Inserts hash value into hashtable

  The input parameters (HashKey, cbHashKeyLen, HashValue) will be
  used to compute the storing position in hashtable. When no error
  occurs the HashIndex will returned. The HashIndex gives the position
  of HashValue in hashtable.

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key
       HashValue      [in]     hash value you are looking for
       HashIndex      [out]    hash index computed by hashfunction

  returns: tpr09_HashTableError
 */
tpr09_HashTableError pr09HTInsertHashItemByValue (tpr09HashTable * HashTable,
                                                  tpr09HashKey   HashKey,
                                                  tsp00_Uint4    cbHashKeyLen,
                                                  tpr09HashValue HashValue,
                                                  tpr09HashIndex *HashIndex);


/*!
  Function: pr09HTInsertHashItemByIndex

  see also:

  Description: Inserts hash value into hashtable

  The given HashValue will inserted in hashtable
  to the position given by HashIndex.

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashIndex      [in|out] hash index computed by hashfunction
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key
       HashValue      [in]     hash value you are looking for

  returns: tpr09_HashTableError
 */
tpr09_HashTableError pr09HTInsertHashItemByIndex (tpr09HashTable * HashTable,
                                                  tpr09HashIndex *HashIndex,
                                                  tpr09HashKey   HashKey,
                                                  tsp00_Uint4    cbHashKeyLen,
                                                  tpr09HashValue HashValue);


/*!
  Function: pr09HTRemoveHashItemByKey

  see also:

  Description: Removes hash value with specified Key/Value from hashtable

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key

  returns: void
 */
void pr09HTRemoveHashItemByKey (tpr09HashTable * HashTable,
                                tpr09HashKey   HashKey,
                                tsp00_Uint4    cbHashKeyLen);

/*!
  Function: pr09HTRemoveHashItemByIndex

  see also:

  Description: Removes hash value with specified Index/Value from hashtable

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       HashIndex      [in]     index you are looking for
       HashKey        [in]     key you are looking for
       cbHashKeyLen   [in]     length of hash Key

  returns: void
 */
void pr09HTRemoveHashItemByIndex (tpr09HashTable * HashTable,
                                  tpr09HashIndex HashIndex,
                                  tpr09HashKey   HashKey,
                                  tsp00_Uint4    cbHashKeyLen);

/*!
  Function: pr09HTResetIterator

  see also:

  Description: Resets the iterator to the first item in the hashtable

  Arguments:

  returns: void
 */
void pr09HTResetIterator (tpr09HashTable * HashTable);

/*!
  Function: pr09HTResetIterator

  see also:

  Description: Returns the next item retrieved while iterating thru the hashtable

  Arguments:

  returns: void*
 */
tpr09HashValue pr09HTIteratorNextItem (tpr09HashTable * HashTable);

/*!
  Function: pr09HTPrintStatHeader

  see also:

  Description: Prints header for statistic output of hash table performance

  If "datei" is a proper file handle the output will be written into this file.
  If "datei" is NULL the output will be written to stdout

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       datei          [in]     handle to a file or NULL

  returns: void
 */
void pr09HTPrintStatHeader (tpr09HashTable * HashTable, FILE * datei );

/*!
  Function: pr09HTPrintStatValues

  see also:

  Description: Prints current statistic values for the hash table

  If "datei" is a proper file handle the output will be written into this file.
  If "datei" is NULL the output will be written to stdout

  Arguments:
       tpr09HashTable [in|out] pointer to a HashTable
       datei          [in]     handle to a file or NULL

  returns: void
 */
void pr09HTPrintStatValues (tpr09HashTable * HashTable, FILE * datei );

/*!
  Function: pr06StmtCache_errString

  see also:

  Description: Returns a string representation of a tpr06_StmtCacheError

  returns: a string representation of a tpr06_StmtCacheError
 */
const char * pr09HTerrString (tpr09_HashTableError procResult);


void pr09HTDump (tpr09HashTable * HashTable, FILE * datei );

#endif  /*VPR09HashTable_H */
