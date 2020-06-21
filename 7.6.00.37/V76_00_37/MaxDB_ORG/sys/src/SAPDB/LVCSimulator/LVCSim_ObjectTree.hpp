/****************************************************************************

  module      : LVCSim_ObjectTree.hpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : Tree for storing keys for keyed objects.

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG




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




*****************************************************************************/


#ifndef LVCSIM_OBJECTTREE_HPP
#define LVCSIM_OBJECTTREE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "LVCSimulator/LVCSim_ObjectHandle.hpp"
#include "LVCSimulator/LVCSim_CheckpointIO.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif

#include <map>

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/// Helper class for tree keys.
class LVCSim_BinaryKey {
public:
    size_t	size;
    char	data[1];

    void *operator new(size_t sz1, size_t sz2)
    {
        return new char[sz1 + sz2 - 1];
    }

    void operator delete(void *p, size_t sz2)
    {
        delete[] (char*) p;
    }

    void *operator new(size_t sz, void *ptr)
    {
        return ptr;
    }

    void operator delete(void *ptr, void *ptr2)
    {
    }

    void operator delete(void *ptr)
    {
        delete[] (char*) ptr;
    }

    static inline size_t contSize(size_t datasz);

    LVCSim_BinaryKey(LVCSim_CheckpointReader &i, size_t _size) : size(_size)
	{
        i.read(&data[0], size);
	}

    LVCSim_BinaryKey(const void *_data, size_t _size) : size(_size)
	{
		SAPDB_MemCopyNoCheck(data, _data, _size);
	}

	LVCSim_BinaryKey(const LVCSim_BinaryKey &x)
	{
        size = x.size;
        SAPDB_MemCopyNoCheck(data, x.data, size);
	}

	~LVCSim_BinaryKey()
	{
	}

	int compare(const LVCSim_BinaryKey &x) const;
	int compare(const void *key, size_t keySize) const;

	void writeToStream(LVCSim_CheckpointWriter &o) const
	{
		o.write(&size, sizeof(size));
		o.write(&data[0], size);
	}
};

inline size_t LVCSim_BinaryKey::contSize(size_t datasz)
{
    return sizeof(LVCSim_BinaryKey) + datasz - 1;
}

class LVCSim_BinaryKeyRef {
public:
    LVCSim_BinaryKeyRef() : m_ref(NULL) {}
    LVCSim_BinaryKeyRef(const LVCSim_BinaryKey *k) : m_ref(k) {}
    LVCSim_BinaryKeyRef(const LVCSim_BinaryKey &k) : m_ref(&k) {}

    const LVCSim_BinaryKey *m_ref;
};

/*! 
 * \brief Object tree for storing object keys. Built on top of STL map container.
 */
class LVCSim_ObjectTree  
{
protected:
  /// Helper class for locking a tree.
	class Locker {
	private:
		LVCSim_ObjectTree	*tree;
		LVCSim_SID			sid;
	public:
		Locker(LVCSim_ObjectTree *tree, LVCSim_SID sid);
		~Locker();
	};

	friend class LVCSim_ObjectTreeNode;
	friend class Locker;

	RTESync_Spinlock			lock;     ///< Tree lock.
	volatile LVCSim_SID		locksid;  ///< Lock owner (transaction).
	int							      lockcnt;  ///< Lock count (for reentrant locks).

	typedef LVCSIM_STD map<LVCSim_BinaryKeyRef, class LVCSim_ObjectTreeNode*>	tree_type;
	typedef LVCSIM_STD pair<const LVCSim_BinaryKeyRef, class LVCSim_ObjectTreeNode*>	pair_type;

	tree_type					tree; ///< Underlying STL tree.

public:
	LVCSim_ObjectTree();
	LVCSim_ObjectTree(LVCSim_CheckpointReader &i, LVCSim_OIDAllocator *alloc);
	~LVCSim_ObjectTree();

	/*!
	 * \brief Adds a new key into the tree, if possible.
   *
   * \param key	 [in]	pointer to key data
   * \param size [in]	key size
   * \param node [in]	object handle
   * \param sid	 [in]	sid - sink/transaction ID
   *
   * \return e_ok if no error occurred; otherwise error-code
   */
	short addKey(const void *key, size_t size, LVCSim_ObjectHandle *node, LVCSim_SID sid);

	/*!
	 * \brief Adds a new key into the tree, if possible.
   *
   * \param key	 [in]	key data
   * \param node [in]	object handle
   * \param sid	 [in]	sid - sink/transaction ID
   *
   * \return e_ok if no error occurred; otherwise error-code
   */
	short addKey(const LVCSim_BinaryKey *key, LVCSim_ObjectHandle *node, LVCSim_SID sid);

	/*!
   * \brief Removes a key from the tree that points to given object node.
   *
   * \param node [in]	object handle to be removed from the tree
   * \param sid	 [in]	sink ID of current TX
	 */
	void delKey(LVCSim_ObjectHandle *node, LVCSim_SID sid);

	/*!
   * \brief Return the handle of first visible object in the tree.
   *
   * \param sid	[in]	sink ID
   * \return	pointer to the first object handle in the tree or NULL
	 */
	LVCSim_ObjectHandle *firstKey(LVCSim_SID sid);

	/*!
   * \brief Return the handle of last visible object in the tree.
   *
   * \param sid	[in]	sink ID
   * \return	pointer to the last object handle in the tree or NULL
   * \since PTS 1119480
	 */
	LVCSim_ObjectHandle *lastKey(LVCSim_SID sid);

	/*!
   * \brief Return the first matching key.
   *
   * \param key		[in]	key data
   * \param size	[in]	key size
   * \param sid		[in]	sink ID
   * \param partial	[in]	if true, partial match will be returned (>= key)
   * \return	pointer to the first matching visible object or NULL
	 */
	LVCSim_ObjectHandle *findKey(const void *key, size_t size, LVCSim_SID sid, bool partial = true, 
                               OmsIterDirection direction = OMS_ASCENDING);

	/*!
	 * \brief Return the next matching key.
   *
   * \param node		[in]	pointer to an object in the tree
   * \param sid			[in]	sink ID
   * \return pointer to the next matching visible object or NULL
	 */
	LVCSim_ObjectHandle *nextKey(const LVCSim_ObjectHandle *node, LVCSim_SID sid);

	/*!
   * \brief Return the previous matching key.
   * 
   * \param node		[in]	pointer to an object in the tree
   * \param sid			[in]	sink ID
   * \return	pointer to the next matching visible object or NULL
   * \since PTS 1119480
	 */
	LVCSim_ObjectHandle *prevKey(const LVCSim_ObjectHandle *node, LVCSim_SID sid);

	/*!
   * \brief Compare node key with supplied key.
   *
   * \param node	[in]	pointer to an object in the tree
   * \param key		[in]	pointer to the key
   * \param keySize	[in]	size of the key
   * \returns:	-1, 0 or 1 when node key is <, == or > as given key
	 */
	int compareKey(const LVCSim_ObjectHandle *node, const void *key, size_t keySize);

  /// Dump tree to the saved state.
	void writeToStream(LVCSim_CheckpointWriter &o) const;
};


#endif
