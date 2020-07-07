/*!**************************************************************************

  module      : HashTable.c

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Implements a hash table.

  This modul implements a hashtable, which maps keys to values. Any non-null
  strings can be used as a key. The hashtable stores pointer to hashed objects.

  To successfully store and retrieve objects from a hashtable, the objects
  the equals method.

  An instance of Hashtable has two parameters that affect its performance,
  initial capacity and load factor. The capacity is the number of buckets
  in the hash table, and the initial capacity is simply the capacity of
  the hash table.

  In the case a "hash collision", a single  bucket stores multiple entries,
  which must be searched sequentially.

  The load factor is a measure of how full the hash table is allowed to get
  before its capacity reached. When the number of entries in the hashtable exceeds
  the product of the load factor and the current capacity, no more objects can be
  inserted.

  Generally, the default load factor (.75) offers a good tradeoff between
  time and space costs. Higher values decrease the space overhead but increase
  the time cost to look up an entry (which is reflected in most Hashtable
  operations, including get and put).

  The initial capacity controls the maximum count of keys to value pairs, which
  can can stored in the hashtable. However, setting the initial capacity too
  high can waste space.

  last changed: 2000-01-20  17:17
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "vpr09HashTable.h"
#include "gpr03m.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
/* array of prime numbers */
static const tsp00_Uint4	PrimeNumbers[] = {
  7UL,53UL, 97UL, 193UL, 389UL, 769UL, 1543UL, 3079UL, 6151UL, 12289UL, 24593UL, 49157UL, 98317UL,
  196613UL, 393241UL, 786433UL, 1572869UL, 3145739UL, 6291469UL, 12582917UL, 25165843UL,
  50331653UL, 100663319UL, 201326611UL, 402653189UL, 805306457UL, 1610612741UL,
  3221225473UL, 4294967291UL, 0 };


static tpr09_HashTableError pr09HTRehash(tpr09HashTable*    HashTable);
  
static void defaultPrintHashItem (const tpr09HashValue data_p, FILE* datei){
  fprintf(datei, "<hashitem addr=0x%p />", data_p);
}

/*!
  Function: defaultCompareMethod

  see also:

  Description: default compare method for comparsion of two hash values

  Arguments:
    Item1   [in] hash value
    Item2   [in] hash value

  returns: 1 if Item1 and Item2 are the same
 */
static tpr09HashValue defaultKeyFunction (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen)
{
    *HashKeyLen = (tsp00_Uint4)strlen ((char*)data_p);
    return data_p;
}

/*!
  Function: defaultCompareMethod

  see also:

  Description: default compare method for comparsion of two hash values

  Arguments:
    Item1   [in] hash value
    Item2   [in] hash value

  returns: 1 if Item1 and Item2 are the same
 */
static tsp00_Int4 defaultCompareMethod (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len)
{
    return (Item1==Item2?1:0);
}

/*!
  Function: defaultHashFunction

  see also:

  Description: default hash function

  Arguments:
    HashKey      [in] hash key
    cbHashKeyLen [in] length of hash key
    cbHashTabLen [in] length of hash table

  returns: hash index
 */
static tpr09HashIndex defaultHashFunction (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen)
{
    tsp00_Uint4 i;
    tpr09HashIndex HashVal=0;
    const char*	myKey = (char*) HashKey;
/*    printf("Hashfunc: TabLen->%u<\n", cbHashTabLen);
    printf("Hashfunc: Length->%u<\n", cbHashKeyLen);
    printf("Hashfunc: Value->%.*s<\n", cbHashKeyLen, HashKey);
*/
    for(i=0; i < cbHashKeyLen; i++)
        if (myKey[i]) HashVal = (HashVal<<5) - HashVal + myKey[i];
/*      HashVal += (tpr09HashIndex) myKey[i];
      printf("Hashfunc: %u\n", (tpr09HashIndex)HashVal%cbHashTabLen);
*/
    return ((tpr09HashIndex)HashVal%cbHashTabLen);
}

/*!
  Function: pr09addtoFreeLst

  see also:

  Description: adds a HashItem to the free-list.


  Arguments:
    tpr09HashItem   [in] pointer to the adding HashItem
    tpr09HashTable  [in] pointer to a HashTable
  returns: void
 */
static void pr09addtoFreeLst (tpr09HashItem * const HItem, tpr09HashTable * HashTable)
{
   HItem->data_p = NULL;
   HItem->next_p = HashTable->HashTableFreeLst.first_p;
   HashTable->HashTableFreeLst.first_p = HItem;
}

/*!
  Function: pr09getnewCollLstItem

  see also:

  Description: Gets a new collision list item from the free-list.

  If the free-list is empty the function tries to allocates a new bunch
  of hash items from the heap and assigns them to the free-list.
  On success, the function returns a pointer to a free collision list
  item in 'newItem'.

  Arguments:
    tpr09HashItem   [in|out] pointer to new collision list item
    tpr09HashTable  [in]     pointer to a HashTable

  returns: tpr09_HashTableError
 */
static tpr09_HashTableError pr09getnewCollLstItem (tpr09HashItem ** const newItem, tpr09HashTable * HashTable)
{
   tpr09HashTableFreeLst * HashTableFreeLst = &(HashTable->HashTableFreeLst);
   if ( HashTableFreeLst->first_p == NULL )
   {
      /* the free-list is empty; allocate a new chunk of items
         from the heap and assign them to the free-list  */
      tsp00_Uint4	  i;
      tpr09HashItem * item_p;

      /* init */
      if (! HashTableFreeLst->cbChunkCnt){
        HashTableFreeLst->cbChunkCnt = HASH_FL_CHUNK_INC_CNT;
        HashTableFreeLst->chunks = (tpr09HashItem**)pr03mAllocatT(HashTableFreeLst->cbChunkCnt*sizeof(void*), "HashTableChunkLst"); 
        memset (HashTableFreeLst->chunks,0,HashTableFreeLst->cbChunkCnt*sizeof(void*));
        if (! HashTableFreeLst->chunks) return AllocMemErr_epr09;  
      }  

      /* search for the first "unused" chunk */
      for ( i = 0; i < HashTableFreeLst->cbChunkCnt && HashTableFreeLst->chunks[i] != NULL; i++ )
      {}
      if ( i == HashTableFreeLst->cbChunkCnt &&  HashTableFreeLst->chunks[i] != NULL)
      {
           HashTableFreeLst->chunks = (tpr09HashItem**)pr03mReallocat(HashTableFreeLst->chunks, (HashTableFreeLst->cbChunkCnt+HASH_FL_CHUNK_INC_CNT)*sizeof(void*), "HashTableChunkLst");
           memset (&HashTableFreeLst->chunks[HashTableFreeLst->cbChunkCnt],0,HASH_FL_CHUNK_INC_CNT*sizeof(void*));
           HashTableFreeLst->cbChunkCnt += HASH_FL_CHUNK_INC_CNT;
           if (! HashTableFreeLst->chunks) return AllocMemErr_epr09;
           i++; 
      }
      HashTableFreeLst->chunks[i] = (tpr09HashItem*)pr03mAllocatT(HASH_FL_CHUNK_SIZE, "HashTableFreeLst");

      /* set the 'next_p' of the last free-list element to NULL and
         and all the others to the respective next element in the
         allocated array of items; this is done from "right" to
        "left";  */
      HashTableFreeLst->first_p = HashTableFreeLst->chunks[i];
      item_p = (HashTableFreeLst->first_p) + ((HASH_FL_CHUNK_SIZE/sizeof(tpr09HashItem)) - 1);
      item_p->next_p = NULL;
      while ( item_p != HashTableFreeLst->first_p )
      {
         item_p->data_p = NULL;
         item_p--;
         item_p->next_p = item_p + 1;
      }
   }

   /* return the first hash item from the free-list */
   *newItem = HashTableFreeLst->first_p;
   HashTableFreeLst->first_p = HashTableFreeLst->first_p->next_p;

   return ( ok_epr09);

} /* hash_new */

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
tpr09_HashTableError pr09HTCreateHashTable(tpr09HashTable*    HashTable, 
                                           tsp00_Int4         cbMaxElem, 
                                           tsp00_Int4         cbLoadFactor, 
                                           tpr09_HTRehash     rehash,
                                           tpr09CompareMethod CompFunc, 
                                           tpr09HashFunc      HashFunc, 
                                           tpr09ExtractKey    KeyFunc,
                                           tpr09PrintHashItem PrintFunc)
{
   /* determine size of hash vectors used by the hash indexes with
      respect to the number of cbMaxElem */
   tsp00_Int4 i;
   tsp00_Int4 LoadFactor = ((cbLoadFactor<=0||cbLoadFactor>=100) ? DEFAULT_HASHTABLE_LOADFACTOR : cbLoadFactor);

   HashTable->cbMaxCapacity = (((cbMaxElem)?cbMaxElem:DEFAULT_HASHTABLE_SIZE) *100) / LoadFactor;

   for ( i = 0; (tsp00_Uint4) HashTable->cbMaxCapacity > PrimeNumbers[i] && PrimeNumbers[i] != 0; i++ )
   {}
   /*init HashTable record*/
   HashTable->cbMaxCapacity = ( PrimeNumbers[i] == 0 ) ? PrimeNumbers[i-1] : PrimeNumbers[i];
   HashTable->cbCLIncCount  = 3;
   HashTable->cbActElem     = 0;     /*number of current used elements */
   HashTable->cbMaxElem     = HashTable->cbMaxCapacity * LoadFactor / 100; /*number of maximal elements*/
   HashTable->cbLoadFactor  = LoadFactor;  /*measure of how full the hash table is allowed to get
                                             before its capacity is reached value between 1 and 100*/
   HashTable->rehash        = rehash;
   HashTable->CompareMethod = (CompFunc  ==NULL)?defaultCompareMethod:CompFunc; /*method for comparsion of two Hashvalues*/
   HashTable->HashFunc      = (HashFunc  ==NULL)?defaultHashFunction:HashFunc;  /*method for comparsion of two Hashvalues*/
   HashTable->KeyFunc       = (KeyFunc   ==NULL)?defaultKeyFunction:KeyFunc;    /*method for comparsion of two Hashvalues*/
   HashTable->PrintFunc     = (PrintFunc ==NULL)?defaultPrintHashItem:PrintFunc;
   HashTable->statistic.requests   = 0;  /* number of requests against hashtable */
   HashTable->statistic.founds     = 0;  /* number of hashvalues found in hashtable */
   HashTable->statistic.inserts    = 0;  /* number of inserted values */
   HashTable->statistic.deletes    = 0;  /* number of deleted values */
   HashTable->statistic.collisions = 0;  /* number of collisopns */
   HashTable->statistic.weightedHitrate = 0;  /* weighted hitrate */

    /*alloc hash table*/
   HashTable->pHashArray = (tpr09HashItem*) pr03mCallocT(HashTable->cbMaxCapacity, sizeof(tpr09HashItem), "pr09HashTable");

   /*initinal free list*/
   HashTable->HashTableFreeLst.chunks  = NULL;
   HashTable->HashTableFreeLst.first_p = NULL;
   HashTable->HashTableFreeLst.cbChunkCnt = 0;

   pr09HTResetIterator(HashTable);

   return (HashTable->pHashArray==NULL?AllocMemErr_epr09:ok_epr09);
}

void pr09HTDeleteHashTable(tpr09HashTable * HashTable)
{
   tsp00_Uint4 i;
  
   /*reset HashTable record*/
   HashTable->cbMaxCapacity = 0;
   HashTable->cbCLIncCount  = 0;
   HashTable->cbActElem     = 0; /*number of current used elements */
   HashTable->cbMaxElem     = 0; /*number of maximal elements*/
   HashTable->cbLoadFactor  = 100;  /*measure of how full the hash table is allowed to get
                                      before its capacity is reached value between 1 and 100*/
   HashTable->CompareMethod = NULL; /*method for comparsion of two Hashvalues*/
   HashTable->KeyFunc       = NULL; 
   HashTable->statistic.requests   = 0;  /* number of requests against hashtable */
   HashTable->statistic.founds     = 0;  /* number of hashvalues found in hashtable */
   HashTable->statistic.inserts    = 0;  /* number of inserted values */
   HashTable->statistic.deletes    = 0;  /* number of deleted values */
   HashTable->statistic.collisions = 0;  /* number of collisopns */
   HashTable->statistic.weightedHitrate = 0;  /* weighted hitrate */

   /*free HashTableFreeLst*/
   for ( i = 0; i < HashTable->HashTableFreeLst.cbChunkCnt && HashTable->HashTableFreeLst.chunks[i] != NULL; i++ ) {
     pr03mFreeT(HashTable->HashTableFreeLst.chunks[i], "HashTableFreeLst");
     HashTable->HashTableFreeLst.chunks[i]=NULL;
   }
   pr03mFreeT(HashTable->HashTableFreeLst.chunks, "HashTableChunkLst");
   HashTable->HashTableFreeLst.chunks = NULL;
   HashTable->HashTableFreeLst.first_p = NULL;
   HashTable->HashTableFreeLst.cbChunkCnt = 0;

   /*free memory*/
   pr03mFreeT(HashTable->pHashArray, "pr09HashTable");
}

void pr09HTClearHashTable (tpr09HashTable * HashTable)
{
   tsp00_Uint i,j;
   tpr09HashItem* pElem;
   
   /*reset HashTable record*/
   HashTable->cbActElem     = 0; /*number of current used elements */
   
   /*reset statistics*/
   HashTable->statistic.requests   = 0;  /* number of requests against hashtable */
   HashTable->statistic.founds     = 0;  /* number of hashvalues found in hashtable */
   HashTable->statistic.inserts    = 0;  /* number of inserted values */
   HashTable->statistic.deletes    = 0;  /* number of deleted values */
   HashTable->statistic.collisions = 0;  /* number of collisopns */
   HashTable->statistic.weightedHitrate = 0;  /* weighted hitrate */

    HashTable->HashTableFreeLst.first_p = NULL;    
    for ( i = 0; i < HashTable->HashTableFreeLst.cbChunkCnt && HashTable->HashTableFreeLst.chunks[i] != NULL; i++ ) {
      for (j=0; j < HASH_FL_CHUNK_SIZE/sizeof(tpr09HashItem); j++){
/*        printf ("FL[%08d] CHUNK[%08d] ITEM[%p]\n", i, j, (HashTable->HashTableFreeLst.chunks[i]+j));*/
        pElem = HashTable->HashTableFreeLst.chunks[i]+j;
        pElem->data_p = NULL;
        pElem->next_p = HashTable->HashTableFreeLst.first_p;
        HashTable->HashTableFreeLst.first_p = pElem;
      } 
    }

    /*clear hash array*/
    memset(HashTable->pHashArray, 0, HashTable->cbMaxCapacity*sizeof(tpr09HashItem));
    pr09HTResetIterator(HashTable);
}

tsp00_Int4 pr09HTContainsByKey (tpr09HashTable * HashTable, 
                                tpr09HashKey HashKey, 
                                tsp00_Uint4 cbHashKeyLen)
{
    if (NULL != pr09HTGetHashItemByKey (HashTable, HashKey, cbHashKeyLen))
      return 1;
    else
      return 0;
}

tsp00_Int4 pr09HTContainsByIndex (tpr09HashTable * HashTable, 
                                  tpr09HashIndex HashIndex, 
                                  tpr09HashKey   HashValue,
                                  tsp00_Uint4    cbHashKeyLen)
{
    if (NULL != pr09HTGetHashItemByIndex (HashTable, HashIndex, HashValue, cbHashKeyLen))
      return 1;
    else
      return 0;
}

tpr09HashValue pr09HTGetHashItemByKey (tpr09HashTable * HashTable, 
                                        tpr09HashKey   HashKey, 
                                        tsp00_Uint4    cbHashKeyLen)
{
    tpr09HashIndex myHashIndex = HashTable->HashFunc(HashKey, cbHashKeyLen, HashTable->cbMaxCapacity);
    return pr09HTGetHashItemByIndex (HashTable, myHashIndex, HashKey, cbHashKeyLen);
}


tpr09HashValue pr09HTGetHashItemByIndex (tpr09HashTable * HashTable, 
                                         tpr09HashIndex HashIndex, 
                                         tpr09HashKey   HashValue,
                                         tsp00_Uint4    cbHashKeyLen)
{
    void* tmpElem;
    tsp00_Uint4 tmpElemLen;
    tpr09HashItem* myHashItem = (HashIndex >= 0 && HashIndex <= HashTable->cbMaxCapacity)?HashTable->pHashArray + HashIndex : NULL;
    HashTable->statistic.requests++;

    if (myHashItem->data_p == NULL) {
      /*nothing found*/
      return NULL;
    }

    do {
      tmpElem = HashTable->KeyFunc(myHashItem->data_p, &tmpElemLen);
      if (HashTable->CompareMethod(HashValue, 
                                   cbHashKeyLen, 
                                   tmpElem, 
                                   tmpElemLen)) {
        HashTable->statistic.founds++;
        return myHashItem->data_p;
      }
      myHashItem = myHashItem->next_p;
    } while (myHashItem != NULL);

    return NULL;
}

tpr09_HashTableError pr09HTInsertHashItemByValue (tpr09HashTable * HashTable, 
                                                  tpr09HashKey   HashKey, 
                                                  tsp00_Uint4    cbHashKeyLen, 
                                                  tpr09HashValue HashValue, 
                                                  tpr09HashIndex *HashIndex)
{
    *HashIndex = HashTable->HashFunc(HashKey, cbHashKeyLen, HashTable->cbMaxCapacity);
    return pr09HTInsertHashItemByIndex (HashTable, HashIndex, HashKey, cbHashKeyLen, HashValue);
}

tpr09_HashTableError pr09HTInsertHashItemByIndex (tpr09HashTable * HashTable, 
                                                  tpr09HashIndex *HashIndex,
                                                  tpr09HashKey   HashKey, 
                                                  tsp00_Uint4    cbHashKeyLen, 
                                                  tpr09HashValue HashValue)
{
   tpr09HashItem * hashItem;

   if (HashTable->cbActElem >= HashTable->cbMaxElem){ /*hash table full*/
     if (HashTable->rehash == rehash_enable) {
       tpr09_HashTableError erg = pr09HTRehash(HashTable);
       if (erg != ok_epr09) return erg;
       *HashIndex = HashTable->HashFunc(HashKey, cbHashKeyLen, HashTable->cbMaxCapacity);
     }    
     else
       return HashTableFull_epr09;
   }  

   hashItem = HashTable->pHashArray + *HashIndex;

   if ( NULL != pr09HTGetHashItemByIndex (HashTable, 
                                          *HashIndex, 
                                          HashKey,
                                          cbHashKeyLen)) /*Hash item already exists*/
     return ok_epr09;

   if ( hashItem->data_p == NULL )
   {
      /* still no object stored under the computed hash address */
      hashItem->data_p = HashValue;
   }
   else {
      /* there is already at least one object with the given hash
         address; get a new hash item and put it on top of the
         collision list */
      tpr09_HashTableError	  rc;
      tpr09HashItem		* new_p;


      rc = pr09getnewCollLstItem (&new_p, HashTable);

      if(rc!=ok_epr09)
         return rc;

      new_p->next_p = hashItem->next_p;
      new_p->data_p = HashValue;
      hashItem->next_p = new_p;
      HashTable->statistic.collisions++;
   }
   HashTable->cbActElem++;
   HashTable->statistic.inserts++;
   return ok_epr09;
}

void pr09HTRemoveHashItemByKey (tpr09HashTable * HashTable, 
                                tpr09HashKey   HashKey, 
                                tsp00_Uint4    cbHashKeyLen)
{
    tpr09HashIndex myHashIndex = HashTable->HashFunc(HashKey, cbHashKeyLen, HashTable->cbMaxCapacity);
    pr09HTRemoveHashItemByIndex (HashTable, myHashIndex, HashKey, cbHashKeyLen);
}

void pr09HTRemoveHashItemByIndex (tpr09HashTable * HashTable, 
                                  tpr09HashIndex HashIndex, 
                                  tpr09HashKey HashKey, 
                                  tsp00_Uint4 cbHashKeyLen)
{
    void* tmpElem;
    tsp00_Uint4 tmpElemLen;
    tpr09HashItem* myHashItem = HashTable->pHashArray + HashIndex;
    tpr09HashItem* prevHashItem = myHashItem;

    if (myHashItem->data_p == NULL) {
      /*nothing found*/
      return;
    }

    do {
      tmpElem = HashTable->KeyFunc(myHashItem->data_p, &tmpElemLen);
      if (HashTable->CompareMethod(HashKey, cbHashKeyLen, tmpElem, tmpElemLen)) {
        if (HashTable->iterator.pElem == myHashItem){
        /*move iterator to the next item*/
          pr09HTIteratorNextItem (HashTable);
        }  
        /*remove hash item*/
        if (prevHashItem == myHashItem) {
          /*first item*/
          myHashItem = myHashItem->next_p;
          prevHashItem->data_p = (myHashItem)?myHashItem->data_p:NULL;
        }
        if (myHashItem) {
           prevHashItem->next_p = myHashItem->next_p;
           /*bring back to hashtable freelist*/
           pr09addtoFreeLst(myHashItem, HashTable);
        }
        else
           prevHashItem->next_p = NULL;
        HashTable->cbActElem--;
        HashTable->statistic.deletes++;
        return;
      }
      prevHashItem = myHashItem;
      myHashItem = myHashItem->next_p;
    } while (prevHashItem->next_p != NULL);
    /*nothing found*/
   return;
}

static tpr09_HashTableError pr09HTRehash(tpr09HashTable*    HashTable) {

   tpr09_HashTableError erg = ok_epr09; 
   tsp00_Uint4 i, hashIndex, rehashKeyLen,oldMaxCapacity = HashTable->cbMaxCapacity;
   tpr09HashItem *oldElem, *tmpElem,      *oldHashArray  = HashTable->pHashArray;
   tpr09HashKey   rehashKey;
   tpr09HashValue rehashVal;
   
   HashTable->cbMaxCapacity = (200 * HashTable->cbMaxCapacity) / HashTable->cbLoadFactor;
   for ( i = 0; (tsp00_Uint4) HashTable->cbMaxCapacity > PrimeNumbers[i] && PrimeNumbers[i] != 0; i++ ){}
   HashTable->cbMaxCapacity = PrimeNumbers[i];
   HashTable->cbMaxElem     = HashTable->cbMaxCapacity * HashTable->cbLoadFactor / 100; /*number of maximal elements*/
   HashTable->statistic.collisions = 0;
   
   /*alloc new bucket array*/
   HashTable->pHashArray = (tpr09HashItem*) pr03mCallocT(HashTable->cbMaxCapacity, sizeof(tpr09HashItem), "pr09HashTable");
   if (! HashTable->pHashArray){
     HashTable->cbMaxCapacity = 0;
     HashTable->cbMaxElem     = 0;
     return AllocMemErr_epr09;
   }
   
   for (i=0; i<oldMaxCapacity; i++){
     oldElem = oldHashArray+i;
     if (oldElem->data_p == NULL) {
       continue;
     }
     while (oldElem != NULL && erg == ok_epr09){
       rehashVal = oldElem->data_p;
       rehashKey = HashTable->KeyFunc(rehashVal, &rehashKeyLen);
       hashIndex = HashTable->HashFunc(rehashKey, rehashKeyLen, HashTable->cbMaxCapacity);
       tmpElem   = HashTable->pHashArray+hashIndex;

       if ( tmpElem->data_p == NULL ){
         tmpElem->data_p = rehashVal;
       }
       else {
         tpr09HashItem		* new_p;
         if (ok_epr09!=(erg = pr09getnewCollLstItem (&new_p, HashTable))) continue; 
         new_p->next_p = tmpElem->next_p;
         new_p->data_p = rehashVal;
         tmpElem->next_p = new_p;
         HashTable->statistic.collisions++;
       }
       
       tmpElem = oldElem->next_p;
       if (tmpElem){
         oldElem->data_p = tmpElem->data_p;
         oldElem->next_p = tmpElem->next_p;
         pr09addtoFreeLst(tmpElem, HashTable);
       } else {
         oldElem = NULL;
       }
     }
   }
   
   pr03mFreeT (oldHashArray,"pr09HashTable");
   pr09HTResetIterator(HashTable);
   return erg;  
} 

void pr09HTDump (tpr09HashTable * HashTable, FILE * datei ){
  tsp00_Uint4 i, maxDeep=0, tmpMaxDeep, sumDeep=0, usedItems=0;
  tpr09HashItem *pElem;
  
  
  pr09HTPrintStatHeader (HashTable, datei);
  pr09HTPrintStatValues (HashTable, datei);
  
  fprintf (datei, "\nHASHTABLE DUMP\n");
  fprintf (datei, "==============\n\n");
  for (i=0; i<HashTable->cbMaxCapacity; i++){
    pElem = HashTable->pHashArray+i;
    if (pElem->data_p != NULL) {
      fprintf (datei, "Bucket[%08d]",i);
    } else {
      continue;
    }
    tmpMaxDeep = 0;
    usedItems ++;
    while (pElem != NULL){
      fprintf (datei, "->",pElem->data_p);
      HashTable->PrintFunc(pElem->data_p,datei);
      pElem = pElem->next_p;
      tmpMaxDeep++;
      sumDeep++;
    }
    if (maxDeep < tmpMaxDeep) maxDeep = tmpMaxDeep;
    fprintf (datei, "\n");
  }
  fprintf (datei, "\nMax deep of collision list: %d\n", maxDeep);
  fprintf (datei, "\nAvg deep of collision list: %f\n", ((double)sumDeep)/usedItems);
}

void pr09HTPrintStatHeader (tpr09HashTable * HashTable, FILE * outputdev )
{
    if (outputdev ==NULL ) {
      printf ("\nHASHTABLE STATISTIC\n");
      printf ("===================\n\n");
      printf ("Max Size   : %d entries\n", HashTable->cbMaxCapacity);
      printf ("Max Filling: %d entries\n", HashTable->cbMaxElem);
      printf ("Load Factor: %d%%\n\n"    , HashTable->cbLoadFactor);
      printf ( "| Inserts | Deletes | Filling | Requests | Founds | Collisions | Hitrate |\n");
      printf ( "|---------|---------|---------|----------|--------|------------|---------|\n");

    }
    else {
      fprintf (outputdev, "HASHTABLE STATISTIC\n");
      fprintf (outputdev, "===================\n\n");
      fprintf (outputdev, "Max Size   : %d entries\n", HashTable->cbMaxCapacity);
      fprintf (outputdev, "Max Filling: %d entries\n", HashTable->cbMaxElem);
      fprintf (outputdev, "Load Factor: %d%%\n\n"    , HashTable->cbLoadFactor);
      fprintf (outputdev, "| Inserts | Deletes | Filling | Requests | Founds | Collisions | Hitrate |\n");
      fprintf (outputdev, "|---------|---------|---------|----------|--------|------------|---------|\n");
    }
}

void pr09HTPrintStatValues (tpr09HashTable * HashTable, FILE * datei )
{
   if( datei==NULL ) {
     printf ( "|%9lu|%9lu|%8.1f%%|%10lu|%8lu|%12lu|%8.1f%%|\n" \
                    ,HashTable->statistic.inserts\
                    ,HashTable->statistic.deletes\
                    ,(float)(100*HashTable->cbActElem)/(float)HashTable->cbMaxElem\
                    ,HashTable->statistic.requests\
                    ,HashTable->statistic.founds\
                    ,HashTable->statistic.collisions\
                   ,(HashTable->statistic.requests==0)?0:(double)((tsp00_Longuint)(100*HashTable->statistic.founds))/(double)((tsp00_Longuint)HashTable->statistic.requests));
   }
   else {
     fprintf (datei, "|%9lu|%9lu|%8.1f%%|%10lu|%8lu|%12lu|%8.1f%%|\n" \
                   ,HashTable->statistic.inserts\
                   ,HashTable->statistic.deletes\
                   ,(float)(100*HashTable->cbActElem)/(float)HashTable->cbMaxElem\
                   ,HashTable->statistic.requests\
                   ,HashTable->statistic.founds\
                   ,HashTable->statistic.collisions\
                   ,(HashTable->statistic.requests==0)?0:(double)((tsp00_Longuint)(100*HashTable->statistic.founds))/(double)((tsp00_Longuint)HashTable->statistic.requests));
  }
}

/*!
  Function: pr06StmtCache_errString

  returns: a string representation of a tpr06_StmtCacheError
 */
const char * pr09HTerrString (tpr09_HashTableError procResult)
{
    switch (procResult)
    {
        case (ok_epr09):
          return        "Ok";
        case (AllocMemErr_epr09):
          return        "failed to allocate sufficient memory";
        case (CacheInconsistant_epr09):
          return        "cache inconsistent";
        case  (InvalidConHdl_epr09) :
          return        "invalid connection handle";
        case (HashTableFull_epr09):
          return        "hashtable full";
        case  (CollisionLstFull_epr09) :
          return        "impossible to allocate more collision list items";
        case  (InvalidHashIndex_epr09) :
          return        "hash index is invalid";
        default:
          return        "unknown error ocurred";
    }
}

void pr09HTResetIterator (tpr09HashTable * HashTable){
  HashTable->iterator.cbCurElem = 0;
  HashTable->iterator.pElem = HashTable->pHashArray;
  if (HashTable->iterator.pElem->data_p == NULL)
    pr09HTIteratorNextItem (HashTable);
}

static void getNextBucket (tpr09HashTable * HashTable){
  tpr09HashTableIterator *iter = &(HashTable->iterator);
  do {
    iter->cbCurElem++;
    iter->pElem = HashTable->pHashArray+iter->cbCurElem;
  } while ( iter->cbCurElem < HashTable->cbMaxCapacity-1 
            && (iter->pElem == NULL || iter->pElem->data_p == NULL));
  if (iter->cbCurElem >= HashTable->cbMaxCapacity){
    iter->pElem = NULL;
  }
}

tpr09HashValue pr09HTIteratorNextItem (tpr09HashTable * HashTable){
  tpr09HashTableIterator *iter = &(HashTable->iterator);
  tpr09HashValue hVal;
  
  if (iter->pElem) 
    hVal = iter->pElem->data_p;
  else
    return NULL;
  
  if(iter->pElem->next_p == NULL){
    getNextBucket(HashTable);
  } else {
    iter->pElem = iter->pElem->next_p;
  }
  return hVal;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
