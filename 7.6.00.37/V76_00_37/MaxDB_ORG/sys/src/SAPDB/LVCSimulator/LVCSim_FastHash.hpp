/*!***************************************************************************

  module      : LVCSim_FastHash.hpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : utility double hash list, high performance, with spinlocks

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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


#ifndef LVCSIM_FASTHASH_HPP
#define LVCSIM_FASTHASH_HPP

#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_CheckpointIO.hpp"
#include <memory.h>

#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif


/*!
  class :  LVCSim_FastHash

  description: Implements a highly concurrent double hash table. Each item has a key
    of type Key and an ID of type ID (should be an integer, must support '%' operation).
	Class Key must have a public method hash() to compute a numerical hash value.
	When doDelete is true, the referenced objects are deleted on clear() and remove().

	The hash table is indexed by both key and id. Only needed buckets are locked in
	order to insert a value.

	TODO: Maybe re-work locking? What is less expensive: spinlocks for each read/write
		on individual buckets or shared lock on entire table for read and exclusive lock
		on entire table for write?

  template arguments:
	Key		expensive key, must have method hash() to compute hash value
	ID		cheap key (object ID), must support operator % to compute hash value
	Val		value class (hashtable stores pointers to this class)
	doDelete if true, references to values will be deallocated on clear or remove
*/

template <class T, class T2, class Val>
class LVCSim_FHBucket {
public:
	T						key;			// key for bucket in this hash
	T2						ref;			// key for bucket in second hash
	Val						*data;			// pointer to data
	LVCSim_FHBucket<T, T2, Val>		*next;			// pointer to next bucket in chain
	LVCSim_FHBucket(const T &_key, const T2 &_ref, Val *_data, LVCSim_FHBucket<T, T2, Val> *_next)
		: key(_key), ref(_ref), data(_data), next(_next)		{ }
	~LVCSim_FHBucket()	{ delete next; }
};

template<class Key, class ID, class Val, bool doDelete = false>
class LVCSim_FastHash  
{
private:

	RTESync_Spinlock			*idLocks;		// locks for ID buckets
	RTESync_Spinlock			*valLocks;		// locks for key buckets

	LVCSim_FHBucket<ID, Key, Val>		**idBuckets;	// ID buckets
	LVCSim_FHBucket<Key, ID, Val>		**valBuckets;	// key buckets

	int							idBucketCnt;	// size of ID bucket array
	int							valBucketCnt;	// size of key bucket array

public:

	class Iterator {
	public:
		typedef	LVCSim_FastHash<Key, ID, Val, doDelete>	HashType;
		typedef LVCSim_FHBucket<Key, ID, Val>			BucketType;

		Iterator(HashType *hashlist, BucketType *bucket, int id)
			: parent(hashlist), curBucket(bucket), curID(id)
		{
		}

		Iterator() : parent(NULL), curBucket(NULL)
		{
		}

		Iterator &operator++()
		{
			if (curBucket) {
				if (curBucket->next) curBucket = curBucket->next;
				else curBucket = parent->nextObject(curID);
			}
			return *this;
		}

		bool isValid() const 
		{ 
			return curBucket != NULL; 
		}

		ID &currentID() const
		{
			return curBucket->ref;
		}

		Key &currentKey() const
		{
			return curBucket->key;
		}

		Val &currentItem() const
		{
			return *curBucket->data;
		}

		Val &operator*() const
		{
			return *curBucket->data;
		}

	private:
		HashType	*parent;
		BucketType	*curBucket;
		int			curID;
	};

	inline LVCSim_FastHash(int _idBucketCnt = 257, int _valBucketCnt = 67)
		: idLocks(new RTESync_Spinlock[_idBucketCnt]),
		  valLocks(new RTESync_Spinlock[_valBucketCnt]),
		  idBuckets(new LVCSim_FHBucket<ID, Key, Val>*[_idBucketCnt]),
		  valBuckets(new LVCSim_FHBucket<Key, ID, Val>*[_valBucketCnt]),
		  idBucketCnt(_idBucketCnt), valBucketCnt(_valBucketCnt)
	{
		int i;
		for (i = 0; i < _idBucketCnt; ++i) idBuckets[i] = NULL;
		for (i = 0; i < _valBucketCnt; ++i) valBuckets[i] = NULL;
	}

	inline ~LVCSim_FastHash()
	{
		clear();

		delete[] idLocks;
		delete[] valLocks;
		delete[] idBuckets;
		delete[] valBuckets;
	}


	/*!
		function: clear
		description: clear all the items in the hash table
	*/
	void clear();	// NOT multithread-safe

	/*!
		function: find
		description: find a value in the hash table, either by ID or by key (and
			return ID)
		returns: pointer to value or NULL if not found
		prototypes: 3
	*/
	Val *find(ID id);
	Val *find(ID id, Key &key_out);
	Val *find(const Key &key, ID &id_out);

	/*!
		function: remove
		description: remove a value identified by key or by ID from the hash table
		returns: pointer to the value (may point to free space if doDelete is true!)
			or NULL if value not found
		prototypes: 2
	*/
	Val *remove(ID id);
	Val *remove(const Key &key);

	/*!
		function: insert
		description: insert a new value in hashtable
		arguments:
			key [in] reference to a key class
			id [in] object ID
			data [in] pointer to data object
		returns: NULL if OK, pointer to old value if key/id already exists
	*/
	Val *insert(const Key &key, ID id, Val *data);
  Val *forceInsert(const Key &key, ID id, Val *data);

	void writeToStream(LVCSim_CheckpointWriter &o)
	{
		int bucket;
		char next = 1;
		for (bucket = 0; bucket < idBucketCnt; ++bucket) {
			RTESync_LockedScope lck(idLocks[bucket]);

			LVCSim_FHBucket<ID, Key, Val> *p = idBuckets[bucket];
			while (p) {
				o.write(&next, 1);
				o.write(&p->ref, sizeof(p->ref));
				o.write(&p->key, sizeof(p->key));
				p->data->writeToStream(o);
				p = p->next;
			}
		}
		next = 0;
		o.write(&next, 1);
	}

	void readFromStream(LVCSim_CheckpointReader &i)
	{
		clear();

		char next = 0;
		i.read(&next, 1);
		while (next) {
			Key k;
			ID id;
			i.read(&k, sizeof(k));
			i.read(&id, sizeof(id));
			Val *v = new Val(i);
			insert(k, id, v);
			i.read(&next, 1);
		}
	}

	Iterator begin()
	{
		LVCSim_FHBucket<Key, ID, Val> *b;
		int curID = 0;
		while (curID < valBucketCnt) {
			b = valBuckets[curID];
			if (b) return Iterator(this, b, curID);
			++curID;
		}
		return Iterator(this, NULL, 0);
	}

protected:
	friend	class Iterator;

	LVCSim_FHBucket<Key, ID, Val> *nextObject(int &curID)
	{
		while (++curID < valBucketCnt) {
			LVCSim_FHBucket<Key, ID, Val> *b = valBuckets[curID];
			if (b) return b;
		}
		return NULL;
	}
};

/*! endclass: LVCSim_FastHash */



template<class Key, class ID, class Val, bool doDelete>
void LVCSim_FastHash<Key, ID, Val, doDelete>::clear()
{
	int i;
	for (i = 0; i < idBucketCnt; ++i) {
		delete idBuckets[i];
		idBuckets[i] = NULL;
	}
	for (i = 0; i < valBucketCnt; ++i) {
		if (doDelete) {
			LVCSim_FHBucket<Key, ID, Val> *p = valBuckets[i];
			while (p) {
				delete p->data;
				p = p->next;
			}
		}
		delete valBuckets[i];
		valBuckets[i] = NULL;
	}
}


template<class Key, class ID, class Val, bool doDelete>
Val *LVCSim_FastHash<Key, ID, Val, doDelete>::find(ID id)
{
	unsigned int bucket = id % idBucketCnt;
	RTESync_LockedScope lck(idLocks[bucket]);

	LVCSim_FHBucket<ID, Key, Val> *p = idBuckets[bucket];
	while (p) {
		if (p->key == id) return p->data;
		p = p->next;
	}
	return NULL;
}


template<class Key, class ID, class Val, bool doDelete>
Val *LVCSim_FastHash<Key, ID, Val, doDelete>::find(ID id, Key &key_out)
{
	unsigned int bucket = id % idBucketCnt;
	RTESync_LockedScope lck(idLocks[bucket]);

	LVCSim_FHBucket<ID, Key, Val> *p = idBuckets[bucket];
	while (p) {
		if (p->key == id) {
			key_out = p->ref;
			return p->data;
		}
		p = p->next;
	}
	return NULL;
}


template<class Key, class ID, class Val, bool doDelete>
Val *LVCSim_FastHash<Key, ID, Val, doDelete>::find(const Key &key, ID &id_out)
{
	unsigned int bucket = ((unsigned int) key.hash()) % ((unsigned int) valBucketCnt);
	RTESync_LockedScope lck(valLocks[bucket]);

	LVCSim_FHBucket<Key, ID, Val> *p = valBuckets[bucket];
	while (p) {
		if (p->key == key) {
			id_out = p->ref;
			return p->data;
		}
		p = p->next;
	}
	return NULL;
}


template<class Key, class ID, class Val, bool doDelete>
Val *LVCSim_FastHash<Key, ID, Val, doDelete>::remove(const Key &key)
{
	ID id;
	{
		unsigned int bucket = ((unsigned int) key.hash()) % ((unsigned int) valBucketCnt);
		RTESync_LockedScope lck(valLocks[bucket]);

		LVCSim_FHBucket<Key, ID, Val> *p = valBuckets[bucket];
		while (p) {
			if (p->key == key) {
				id = p->ref;
				break;
			}
			p = p->next;
		}
		if (!p) return NULL;
	}
	return remove(id);
}


template<class Key, class ID, class Val, bool doDelete>
Val *LVCSim_FastHash<Key, ID, Val, doDelete>::remove(ID id)
{
	Val *val;
	{
		unsigned int id_bucket = id % idBucketCnt;
		RTESync_LockedScope lck_id(idLocks[id_bucket]);

		LVCSim_FHBucket<ID, Key, Val> *p = idBuckets[id_bucket], *last = NULL;
		while (p) {
			if (p->key == id) {
				break;
			}
			last = p;
			p = p->next;
		}
		if (!p) return NULL;

		unsigned int val_bucket = ((unsigned int) p->ref.hash()) % ((unsigned int) valBucketCnt);
		RTESync_LockedScope lck_val(valLocks[val_bucket]);
		LVCSim_FHBucket<Key, ID, Val> *p2 = valBuckets[val_bucket], *last2 = NULL;
		while (p2) {
			if (p2->ref == id) {
				break;
			}
			last2 = p2;
			p2 = p2->next;
		}
		if (!p2) return NULL;	// this shouldn't happen at all!

		// remove from buckets
		val = p->data;
		if (last) last->next = p->next;
		else idBuckets[id_bucket] = p->next;
		p->next = NULL;
		delete p;
		if (last2) last2->next = p2->next;
		else valBuckets[val_bucket] = p2->next;
		p2->next = NULL;
		delete p2;
	}

    if (doDelete) {
        delete val;
    }
	return val;
}


template<class Key, class ID, class Val, bool doDelete>
Val *LVCSim_FastHash<Key, ID, Val, doDelete>::insert(const Key &key, ID id, Val *data)
{
	unsigned int id_bucket = id % idBucketCnt;
	unsigned int val_bucket = ((unsigned int) key.hash()) % ((unsigned int) valBucketCnt);

	RTESync_LockedScope lck_id(idLocks[id_bucket]);
	RTESync_LockedScope lck_val(valLocks[val_bucket]);

	// test if already there - first by key, then by id
	{
		LVCSim_FHBucket<Key, ID, Val> *p = valBuckets[val_bucket];
		while (p) {
			if (p->key == key) return p->data;
			p = p->next;
		}
	}
	{
		LVCSim_FHBucket<ID, Key, Val> *p = idBuckets[id_bucket];
		while (p) {
			if (p->key == id) return p->data;
			p = p->next;
		}
	}

	// append new buckets
	// TODO: this is not exception-safe
  // IMPORTANT: necessary to add bucket in front, buckets have to be ordered
	valBuckets[val_bucket] = new LVCSim_FHBucket<Key, ID, Val>(key, id, data, valBuckets[val_bucket]);
	idBuckets[id_bucket] = new LVCSim_FHBucket<ID, Key, Val>(id, key, data, idBuckets[id_bucket]);
	return NULL;
}

template<class Key, class ID, class Val, bool doDelete>
Val *LVCSim_FastHash<Key, ID, Val, doDelete>::forceInsert(const Key &key, ID id, Val *data)
{
	unsigned int id_bucket = id % idBucketCnt;
	unsigned int val_bucket = ((unsigned int) key.hash()) % ((unsigned int) valBucketCnt);

	RTESync_LockedScope lck_id(idLocks[id_bucket]);
	RTESync_LockedScope lck_val(valLocks[val_bucket]);

	// test if already there - first by key, then by id
	//{
	//	LVCSim_FHBucket<Key, ID, Val> *p = valBuckets[val_bucket];
	//	while (p) {
	//		if (p->key == key) return p->data;
	//		p = p->next;
	//	}
	//}
	{
		LVCSim_FHBucket<ID, Key, Val> *p = idBuckets[id_bucket];
		while (p) {
			if (p->key == id) return p->data;
			p = p->next;
		}
	}

	// append new buckets
	// TODO: this is not exception-safe
  // IMPORTANT: necessary to add bucket in front, buckets have to be ordered
	valBuckets[val_bucket] = new LVCSim_FHBucket<Key, ID, Val>(key, id, data, valBuckets[val_bucket]);
	idBuckets[id_bucket] = new LVCSim_FHBucket<ID, Key, Val>(id, key, data, idBuckets[id_bucket]);
	return NULL;
}

/*!
	endclass: LVCSim_FastHash
*/

#endif
