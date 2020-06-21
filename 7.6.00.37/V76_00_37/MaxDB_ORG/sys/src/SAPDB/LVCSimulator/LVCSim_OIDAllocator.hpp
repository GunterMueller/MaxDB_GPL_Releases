/****************************************************************************

  module      : LVCSim_OIDAllocator.hpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : allocator for memory and OIDs

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


#ifndef LVCSIM_OIDALLOCATOR_HPP
#define LVCSIM_OIDALLOCATOR_HPP

#include "LVCSimulator/LVCSim_CheckpointIO.hpp"

#define	LVCSIM_ALLOC_MAX_MAP	4096	// allocate max. 2GB/allocator


/*!
 * \brief Base class for objects allocated by LVCSim_OIDAllocator
 */
class LVCSim_OIDObject 
{
private:
	// WARNING: This must have the same structure as OmsTypeOid
  union {
    SAPDB_UInt8  OidAs8ByteInt;
    struct {
	    teo00_Uint4		m_pagenr;		  	///< page number
	    teo00_Uint2		m_pagepos;		  ///< page position
	    teo00_Uint2		m_generation;		///< object generation
    } OidAsStruct;
  } m_oid;

public:
	/*!
	 * \brief Fill in OID structure.
   *
   * \param lpoid	[out]	space where to write OID
	 */
	void getCurrentOid(OMS_ObjectId8 *lpoid) const;

	/*!
	 * \brief Fill in PageRef structure.
   *
   * \param ref		[out]	space where to write PageRef
	 */
	void getRef(tgg91_PageRef *ref) const;

  /// Compute hash value
	inline unsigned int hash() const
	{
		return m_oid.OidAsStruct.m_pagenr ^ (m_oid.OidAsStruct.m_pagepos << 8);
	}

  /*!
   * \brief Set OID from container and position in container.
   *
   * \param container container ID.
   * \param oid index in the container's OID allocator.
   * \param _generation object generation.
   */
	inline void setOid(unsigned int container, unsigned int oid, unsigned int generation)
	{
		m_oid.OidAsStruct.m_pagenr     = oid | ((container >> 16) << LVCSIM_OADDR_BITS);
		m_oid.OidAsStruct.m_pagepos    = container & 0xffff;
		m_oid.OidAsStruct.m_generation = generation;
	}

  /// Get container handle from OID.
	inline OMS_ContainerHandle getContainer() const
	{   
        OMS_ContainerHandle contHndl;
        contHndl.SetOldContainerHandle( m_oid.OidAsStruct.m_pagepos | ((m_oid.OidAsStruct.m_pagenr >> LVCSIM_OADDR_BITS) << 16));
        return contHndl;
	}

  /// Check if the object is still valid.
  inline bool isValid() const
  {
    if ((m_oid.OidAsStruct.m_pagepos | ((m_oid.OidAsStruct.m_pagenr >> LVCSIM_OADDR_BITS) << 16)) == 0) {
      return false;
    }
    return m_oid.OidAsStruct.m_pagenr != (teo00_Uint4) -1;
  }

  /// Get index in container from OID.
	inline unsigned int getCurrentOid() const
	{
		return m_oid.OidAsStruct.m_pagenr & ((1 << LVCSIM_OADDR_BITS) - 1);
	}

  /// Get simulated page number.
	inline unsigned int getPageNr() const
	{
		return m_oid.OidAsStruct.m_pagenr;
	}

  /// Get simulated page position.
	inline unsigned int getPagePos() const
	{
		return m_oid.OidAsStruct.m_pagepos;
	}

  /// Get object generation.
	inline unsigned int getGeneration() const
	{
		return m_oid.OidAsStruct.m_generation;
	}

  /// Set object generation.   // PTS 1125361
	inline void setGeneration(unsigned int generation)
	{
		m_oid.OidAsStruct.m_generation = generation;
	}


  /// Increase generation.
  /*! Increase generation.
  ** \attention As only one byte is used for the generation (there is no more
  **    space left in the kernel) it must be assured, that the generation value
  **    stay within this corresponding interval. Furthermore 0 is no valid 
  **    generation so that the interval [1..0xff] remains as valid generations
  **    (PTS 1125361)
  */
	inline void nextGeneration()
	{
    if (m_oid.OidAsStruct.m_generation == 0xff)    
      m_oid.OidAsStruct.m_generation = 1;
    else
		  ++m_oid.OidAsStruct.m_generation;
    m_oid.OidAsStruct.m_pagepos = 0;
    m_oid.OidAsStruct.m_pagenr = 0;
	}

  /// Deactivate current object (make it invalid).
	inline void deactivateObject()
	{
    m_oid.OidAsStruct.m_pagepos = 0;
    m_oid.OidAsStruct.m_pagenr = 0;
	}
};




/*!
 * \brief This class allocates memory and physical references for objects.
 *
 * The allocator allocates objects not one-by-one, but in blocks. This saves per-object
 * memory overhead and makes it faster. Freed objects are held in a freelist.
 *
 * \see LVCSim_ContainerMeta
 * \see \ref lvcsim_tech_oid
 */
class LVCSim_OIDAllocator  
{
private:
	size_t	m_idx;				///< Allocator index (container ID)
	size_t	m_size;				///< Aligned object size
	size_t	m_objpage;	  ///< Objects per page

	int		  m_freeptr;	  ///< Free pointer
	size_t	m_endptr;			///< End pointer (maximal allocated OID)

	size_t	m_mapcnt;			///< Alocated page ranges count
	char*	m_maps[LVCSIM_ALLOC_MAX_MAP];		///< Page ranges pointers

	RTESync_Spinlock	m_lock;

public:
  /// Initialize from the stream.
	LVCSim_OIDAllocator(LVCSim_CheckpointReader &i, size_t size);

  /*!
   * \brief Create new allocator.
   *
   * \param idx container ID for which this allocator allocates objects.
   * \param sz object size (internal size).
   */
	LVCSim_OIDAllocator(unsigned int idx, size_t sz);

	~LVCSim_OIDAllocator();

	/*!
	 * \brief Allocate memory for an object.
   *
   * The object must be derived from LVCSim_OIDObject - it's structure is filled
   * in. Generation's highest bit in allocated object is set to 1, application
	 * MUST set this bit to 0 to activate the object.
	 * \return pointer to allocated memory
	 */
	void *allocate();

	/*!
	 * \brief Deallocate an object.
   *
   * \param oid	[in] index of the object in this allocator/container.
	 */
	void deallocate(unsigned int oid);

	/*!
   * \brief Map physical reference to a memory address.
   *
   * \param oid	[in] index of the object in this allocator/container.
   * \return pointer to object
	 */
	void *map(unsigned int oid) const;

	/*!
	 * \brief Get next allocated OID in allocator (used for iterators).
   *
   * \param oid [in]	last returned OID or 0 for first call.
   * \return next OID or 0 if no more objects
	 */
	unsigned int getNextOid(unsigned int oid) const;

  /// Write all objects in this allocator to the stream.
	void writeToStream(LVCSim_CheckpointWriter &o, size_t size) const;
};

#endif
