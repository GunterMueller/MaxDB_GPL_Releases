/*!***************************************************************************

  module      : LVCSim_ObjectTree.cpp

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_ObjectTree.hpp"


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

LVCSim_ObjectTree::Locker::Locker(LVCSim_ObjectTree *_tree, LVCSim_SID _sid)
	: tree(_tree), sid(_sid)
{
	if (tree->locksid == sid) {
		// already locked by us
		++(tree->lockcnt);
		return;
	}

	// locked by other thread - must wait for lock
	tree->lock.Lock();
	tree->locksid = sid;
	tree->lockcnt = 1;
}

LVCSim_ObjectTree::Locker::~Locker()
{
	// decrement lock count and alternatively unlock
	if (--(tree->lockcnt) == 0) {
		tree->locksid = NULL;
		tree->lock.Unlock();
	}
}




class LVCSim_ObjectTreeNode {

public:

	class NodeList {
	public:
		LVCSim_ObjectHandle	*m_node;
		NodeList			*next;

		NodeList(LVCSim_ObjectHandle *_node, NodeList *_next) : m_node(_node), next(_next)
		{
		}

		~NodeList()
		{
			delete next;
		}
	};

	typedef LVCSIM_STD map<LVCSim_BinaryKeyRef, LVCSim_ObjectTreeNode*>::iterator iterator;

	iterator	pos;
	NodeList	*nodes;

    LVCSim_ObjectTreeNode(LVCSim_ObjectHandle *m_node, iterator &p) : pos(p)
	{
		nodes = new NodeList(m_node, NULL);
		m_node->m_node = this;
	}

	~LVCSim_ObjectTreeNode()
	{
		delete nodes;
	}

	short newNode(LVCSim_ObjectHandle *m_node, LVCSim_SID sid)
	{
		// return true if no conflict
		NodeList *cur = nodes;
		while (cur) {
			// test if this m_node is already deleted and not visible in this TX
      short res = cur->m_node->isNewable(sid);
      if (res != e_ok){
        return res;
      }
			cur = cur->next;
		}
		nodes = new NodeList(m_node, nodes);
		m_node->m_node = this;
		return e_ok;
	}

	bool delNode(LVCSim_ObjectHandle *m_node)
	{
		NodeList *last = NULL, *cur = nodes;
		while (cur && cur->m_node != m_node) {
			last = cur;
			cur = cur->next;
		}
        if (!cur) {
            return false;
        }
        if (last) {
            last->next = cur->next;
        } else {
            nodes = cur->next;
        }
		cur->next = NULL;
		delete cur;

		return (nodes == NULL);
	}

	LVCSim_ObjectHandle *visibleNode(LVCSim_SID sid)
	{
		NodeList *cur = nodes;
		while (cur != NULL) {
			// test if this m_node is visible in TX
			NodeList *next = cur->next;
			if (cur->m_node->isVisible(sid)) return cur->m_node;
			cur = next;
		}
		return NULL;
	}

	iterator getPos() const { return pos; }
};


int LVCSim_BinaryKey::compare(const LVCSim_BinaryKey &x) const
{
    if (size < x.size) {
        return -1;
    }
    if (size > x.size) {
        return 1;
    }
    return memcmp(data, x.data, size);
}

int LVCSim_BinaryKey::compare(const void *key, size_t keySize) const
{
    if (size < keySize) {
        return -1;
    }
    if (size > keySize) {
        return 1;
    }
    return memcmp(data, key, size);
}

bool operator <(const LVCSim_BinaryKeyRef &a, const LVCSim_BinaryKeyRef &b)
{
	return a.m_ref->compare(*b.m_ref) < 0;
}



LVCSim_ObjectTree::LVCSim_ObjectTree() : locksid(NULL), lockcnt(0)
{
}

LVCSim_ObjectTree::~LVCSim_ObjectTree()
{
	// delete all items from tree
	tree_type::iterator i = tree.begin(), stop = tree.end();
	while (i != stop) {
        delete i->first.m_ref;
		delete i->second;
		++i;
	}
	tree.clear();
}


short LVCSim_ObjectTree::addKey(const void *key, size_t size, LVCSim_ObjectHandle *m_node, LVCSim_SID sid)
{
	Locker lck(this, sid);

    LVCSim_BinaryKey *k = new(size) LVCSim_BinaryKey(key, size);
	pair_type item = pair_type(LVCSim_BinaryKeyRef(k), NULL);

    LVCSIM_STD pair<tree_type::iterator, bool> result = tree.insert(item);
    if (result.second == false) {
        // already there
        short res = result.first->second->newNode(m_node, sid);
        delete item.first.m_ref;
        delete item.second;
        return res;
    } else {
        result.first->second = new LVCSim_ObjectTreeNode(m_node, result.first);
    }
    return e_ok;
}

short LVCSim_ObjectTree::addKey(const LVCSim_BinaryKey *k, LVCSim_ObjectHandle *m_node, LVCSim_SID sid)
{
	Locker lck(this, sid);

	pair_type item = pair_type(LVCSim_BinaryKeyRef(k), NULL);

    LVCSIM_STD pair<tree_type::iterator, bool> result = tree.insert(item);
    if (result.second == false) {
        // already there
        short res = result.first->second->newNode(m_node, sid);
        delete item.first.m_ref;
        delete item.second;
        return res;
    } else {
        *const_cast<LVCSim_BinaryKey**>(&result.first->first.m_ref) = 
            new(k->size) LVCSim_BinaryKey(*k);
        result.first->second = new LVCSim_ObjectTreeNode(m_node, result.first);
    }
    return e_ok;
}


void LVCSim_ObjectTree::delKey(LVCSim_ObjectHandle *m_node, LVCSim_SID sid)
{
	if (!m_node->m_node) return;	// not yet added - delete after failed insert
	Locker lck(this, sid);
	if (m_node->m_node->delNode(m_node)) {
        tree_type::iterator i = m_node->m_node->getPos();
        delete i->first.m_ref;
        if (i->second) {
            delete i->second;
        }
		tree.erase(i);
	}
	m_node->m_node = NULL;
}


LVCSim_ObjectHandle *LVCSim_ObjectTree::firstKey(LVCSim_SID sid)
{
	Locker lck(this, sid);
	tree_type::iterator i = tree.begin();
	while (i != tree.end()) {
		tree_type::iterator old = i++;
		LVCSim_ObjectHandle *m_node = old->second->visibleNode(sid);
        if (m_node) {
            return m_node;
        }
    }
	return NULL;
}

// PTS 1119480
LVCSim_ObjectHandle *LVCSim_ObjectTree::lastKey(LVCSim_SID sid)
{
	Locker lck(this, sid);
	tree_type::reverse_iterator i = tree.rbegin();
	while (i != tree.rend()) {
		tree_type::reverse_iterator old = i++;
		LVCSim_ObjectHandle *m_node = old->second->visibleNode(sid);
        if (m_node) {
            return m_node;
        }
    }
	return NULL;
}


LVCSim_ObjectHandle *LVCSim_ObjectTree::findKey(const void *key, size_t size, LVCSim_SID sid, 
                                                bool partial, OmsIterDirection direction)
{
	Locker lck(this, sid);
	tree_type::iterator i;
  LVCSim_BinaryKey *k = new(alloca(LVCSim_BinaryKey::contSize(size))) 
        LVCSim_BinaryKey(key, size);

  if (direction == OMS_ASCENDING){   // PTS 1119480
    if (partial) {
        i = tree.lower_bound(LVCSim_BinaryKeyRef(k));  // lower_bound: greaterequal
    } else {
        i = tree.find(LVCSim_BinaryKeyRef(k));
    }

	  for (;;) {
        if (i == tree.end()) {
            return NULL;
        }
	      tree_type::iterator old = i++;
        LVCSim_ObjectHandle *m_node = old->second->visibleNode(sid);
        if (m_node) {
            return m_node;
        }
        if (!partial) {
            return NULL;
        }
    }
  } else {  // PTS 1119480
    if (partial) {
      i = tree.upper_bound(LVCSim_BinaryKeyRef(k));  // upper_bound: greater
      if (i == tree.begin()){
        return NULL;
      } else {
        --i;                                         // decrement => lessequal
      }
    } else {
        i = tree.find(LVCSim_BinaryKeyRef(k));
    }

	  for (;;) {
      LVCSim_ObjectHandle *m_node = i->second->visibleNode(sid);
      if (m_node) {
        return m_node;
      }
      if (!partial) {
        return NULL;
      }
      if (i != tree.begin()){
        --i;
      } else {
        return NULL;
      }
    }
  }
}

LVCSim_ObjectHandle *LVCSim_ObjectTree::nextKey(const LVCSim_ObjectHandle *m_node, LVCSim_SID sid)
{
	Locker lck(this, sid);
	tree_type::iterator i = m_node->m_node->getPos();
	++i;
	for (;;) {
		if (i == tree.end()) return NULL;
		tree_type::iterator old = i++;
		LVCSim_ObjectHandle *m_node = old->second->visibleNode(sid);
        if (m_node) {
            return m_node;
        }
    }
}

// PTS 1119480
LVCSim_ObjectHandle *LVCSim_ObjectTree::prevKey(const LVCSim_ObjectHandle *m_node, LVCSim_SID sid)
{
	Locker lck(this, sid);
	tree_type::iterator i = m_node->m_node->getPos();
	for (;;) {
    if (i != tree.begin()){
      --i;
    }
    else {
      return NULL;
    }
		LVCSim_ObjectHandle *m_node = i->second->visibleNode(sid);
    if (m_node) {
      return m_node;
    }
  }
}

int LVCSim_ObjectTree::compareKey(const LVCSim_ObjectHandle *m_node,
	const void *key, size_t keySize)
{
	return m_node->m_node->getPos()->first.m_ref->compare(key, keySize);
}




LVCSim_ObjectTree::LVCSim_ObjectTree(LVCSim_CheckpointReader &i, LVCSim_OIDAllocator *alloc)
	: locksid(NULL), lockcnt(0)
{
	unsigned int oid;
	i.read(&oid, sizeof(oid));
	while (oid) {
        size_t size;
        i.read(&size, sizeof(size));
        LVCSim_BinaryKey *k = new(alloca(LVCSim_BinaryKey::contSize(size)))
            LVCSim_BinaryKey(i, size);
		addKey(k, (LVCSim_ObjectHandle *) alloc->map(oid), NULL);
		i.read(&oid, sizeof(oid));
	}
}

void LVCSim_ObjectTree::writeToStream(LVCSim_CheckpointWriter &o) const
{
	tree_type::const_iterator i = tree.begin();
	unsigned int oid;
	while (i != tree.end()) {
		const LVCSim_ObjectTreeNode::NodeList *nodes = i->second->nodes;
		while (nodes) {
			oid = nodes->m_node->getCurrentOid();
			o.write(&oid, sizeof(oid));
			i->first.m_ref->writeToStream(o);
			nodes = nodes->next;
		}
		++i;
	}
	oid = 0;
	o.write(&oid, sizeof(oid));
}
