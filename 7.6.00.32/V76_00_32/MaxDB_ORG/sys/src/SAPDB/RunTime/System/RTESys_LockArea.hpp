/*!
  @ingroup      Sys
  @file         RTESys_LockArea.hpp
  @author       StefanP

  @brief        Lock Area (Tru64 (ALPHA only)
				The ALPHA processor requires that not more than one lock is located
				in a single cacheline. Therefore this class offers some classes for
				a lock area to fulfill this requirement.

  @since        2003-11-14  12:24
  @sa     
  

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/
#ifndef RTESYS_LOCKAREA_HPP
#define RTESYS_LOCKAREA_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/System/RTESys_Spinlock.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class	RTESys_LockArea
  @brief    HP and ALPHA:    
            This lock area guaranties that only a single lock value is inside a cacheline.   
			It also guaranties that the lock value is located cacheline aligned. 
   			Of course the lock area is space consuming because it consumes 
            twice as many space as a cacheline.

            OTHERS:
            The lock area equals the RTE_Lock space. No alignment is taking place.
            Therewith this class is useable for all platforms.
 */


#if defined(_WIN32) || defined (FREEBSD) || defined(AIX) || defined(SUN) || defined(SOLARIS) || defined(LINUX) || (defined (HPUX) && defined (HP_IA64))
#define RTE_LOCK_AREA_SIZE      (sizeof (RTE_Lock))
#elif defined(PA20W) || defined(PA11) || defined (ALPHA)
#define RTE_LOCK_AREA_SIZE      SAPDB_CACHELINE_SIZE
#endif

class RTESys_LockArea
{
public:
 
  /*!
    @brief		Get the lock value which is located in the lock area.
    
	@return		Pointer to the lock value is returned.
  */

	RTE_Lock *	GetLockValue    ()
	{
#if defined(_WIN32) || defined (FREEBSD) || defined(AIX) || defined(SUN) || defined(SOLARIS) || defined(LINUX) || (defined (HPUX) && defined (HP_IA64))
		return ((RTE_Lock *) m_Lock);
#elif defined(PA20W) || defined(PA11) || defined (ALPHA)
        /* Allign the lock value to the RTE_LOCK_AREA_SIZE (cacheline alligned) */
		return ((RTE_Lock *) 
				 (((char *)0) +
				  ( ((char *)m_Lock - (char *)0 + SAPDB_CACHELINE_SIZE - 1) & 
				    (~(SAPDB_CACHELINE_SIZE - 1)) 
				  )
			     )
			   );
#endif
	}

private:
#if defined(_WIN32) || defined (FREEBSD) || defined(AIX) || defined(SUN) || defined(SOLARIS) || defined(LINUX) || (defined (HPUX) && defined (HP_IA64))
	SAPDB_Byte	m_Lock[RTE_LOCK_AREA_SIZE];
#elif defined(PA20W) || defined(PA11) || defined (ALPHA)
	SAPDB_Byte	m_Lock[RTE_LOCK_AREA_SIZE * 2];
#endif
};

/*
class RTESys_LockAreaWithAlignedLockValue
{
public:
 
  *!
    @brief		Get the lock value which is located in the lock area.
    
	@return		Pointer to the lock value is returned.
  *

	RTE_Lock *	GetLockValue ()
	{
		return ( (RTE_Lock *) 
				 (((char *)0) +
				  ( ((char *)m_Lock - (char *)0 + RTE_LOCK_ALLIGNMENT - 1) & 
				    (~(SAPDB_CACHELINE_SIZE - 1)) 
				  )
			     )
			   );
	}

private:
	SAPDB_Byte	m_Lock[RTE_LOCK_ALLIGNMENT *2];
};


*!
  @class	RTESys_AlignedLockArea (ALPHA processor only)
  @brief	This lock area requires that an instance of this class is instantiated cacheline aligned.
			
			Then it guaranties as the class 'RTESys_LockAreaWithAlignedLockValue' above that only 
			a single lock value is inside a cacheline and that a lock value is located cacheline 
			aligned. 

			Thus this kind of lock area is not as space consuming as the 
			'RTESys_LockAreaWithAlignedLockValue'. Therefor more preconditions of the memory location
			where an instance of this class is instantiated are made.
 *

class RTESys_AlignedLockArea
{
public:
  *!
    @brief		Constructor
				The constructor checks for the slow kernel if the instance 
				is instantiated cacheline aligned.  
   *

  RTESys_AlignedLockArea()
  {
	  SAPDBERR_ASSERT_STATE ((((char *)this-(char *)0) % SAPDB_CACHELINE_SIZE) == 0);
  }
	

  *!
    @brief		Get the lock value which is located in the lock area.
    
	@return		Pointer to the lock value is returned.
  *

	RTE_Lock *	GetLockValue ()
	{
		return ((RTE_Lock *) m_Lock);
	}

private:
	SAPDB_Long	m_Lock[SAPDB_CACHELINE_SIZE/sizeof (SAPDB_Long)];
};
*/
#endif  /* RTESYS_LOCKAREA_HPP */
