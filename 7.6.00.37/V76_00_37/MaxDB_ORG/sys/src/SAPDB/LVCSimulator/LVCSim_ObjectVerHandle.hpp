/*!***************************************************************************

  module      : LVCSim_ObjectVerHandle.hpp

  -------------------------------------------------------------------------

  responsible : ChristophR

  special area: liveCache Simulator
  description : Internal object version handle 

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


#ifndef LVCSIM_OBJECTVERHANDLE_HPP
#define LVCSIM_OBJECTVERHANDLE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

//#include "SAPDBCommon/SAPDB_Types.hpp"
#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_OIDAllocator.hpp"
#include "LVCSimulator/LVCSim_CheckpointIO.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include <memory.h>

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



/*! 
 * \brief Handle for an object version.
 *
 * This class defines the control block of one version of the persistent
 * object in memory. This handle is allocated for each version of the
 * persistent object. It also contains the object data itself.
 */
class LVCSim_ObjectVerHandle
{
public:
    /// Create the object version from a checkpoint reader.
	LVCSim_ObjectVerHandle(LVCSim_CheckpointReader &r, size_t objsize);

    /*!
     * \brief Create new object version.
     *
     * \param tmin creation transaction time,
     * \param bodydata pointer to body data,
     * \param bodylen object body length,
     * \param varsize \c true, if variable-sized, \c false otherwise,
     * \param contObj \c true, if variable-sized continuation object, \c false otherwise
     * \param generation generation of this object version (for reused oids)
     * \param nextobj next object version in chain.
     */
	LVCSim_ObjectVerHandle(
		LVCSim_TID tmin,
		const void *bodydata,
		size_t bodylen, 
		bool varsize,
    bool isShared,
		bool contObj,
    OMS_ObjectId8 oid,
		LVCSim_ObjectVerHandle *nextobj)
	:
		m_tid_min(tmin),
		m_next(nextobj),
    m_isShared(isShared),
		m_contObj(contObj),
    m_oid(oid)
	{
		if (varsize) {
			m_data.var.size = bodylen;
			SAPDB_MemCopyNoCheck(m_data.var.data, bodydata, bodylen);
      
		} else {
			SAPDB_MemCopyNoCheck(m_data.fix, bodydata, bodylen);
		}
	}

    /// Destroy the object version and all older versions.
	~LVCSim_ObjectVerHandle();

    /// Operator new allocating extra space for data.
	static void *operator new(size_t sz, size_t datasize);
    /// Operator delete deallocating extra space for data.
	static void operator delete(void *ptr, size_t datasize);
    /// Normal operator delete.
	static void operator delete(void *ptr);
    
    /// Return generation of object version
  unsigned int getGeneration(){
    return m_oid.getGeneration();
  }
    /// Return oid of object version
  void getOid( OMS_ObjectId8 *oid){
    oid->m_oid.OidAs8ByteInt = m_oid.m_oid.OidAs8ByteInt;
  }

    /// Dump the version to checkpoint stream.
	void writeToStream(LVCSim_CheckpointWriter &o, size_t objsize) const;

private:
    friend class LVCSim_ObjectHandle;
    friend class LVCSim_liveCacheSink;
    friend class LVCSim_ObjectLockChainItem;

    LVCSim_TID            m_tid_min;    ///< Creating transaction ID.
    LVCSim_ObjectVerHandle	*m_next;    ///< Next older version in chain.
    bool                  m_isShared;    ///< True if object was share locked
    bool                  m_contObj;    ///< True if object is variable-sized continuation object
    OMS_ObjectId8         m_oid;        ///< Oid of object version (for reused oids)

    union {
        char				fix[1];     ///< First byte of fixed-sized object data.
        struct {
            size_t			size;       ///< Size of variable-sized object data.
            char			data[1];    ///< First byte of variable-sized object data.
        } var;                          ///< Variable-sized object data.
    } m_data;                           ///< Object data.
};

#endif
