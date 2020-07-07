/*!
  @file           SAPDB_Singleton.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Common singleton base class
  
  After several bad problems with unwanted copy actions on singletons, this
  base class is introduced. It has protected copy constructor and assignment
  operator. Therefore with a singleton that is derived from this class, the
  compiler will detect all attempts to copy a singleton...


\if EMIT_LICENCE

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



\endif
*/
#ifndef SAPDB_SINGLETON_HPP
#define SAPDB_SINGLETON_HPP

#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*!

   @class SAPDB_Singleton

   @brief Base class for singletons

   Usage of this class as base class prevents copy or assignment of singletons

  */
class SAPDB_Singleton
{
public:
    /*!-------------------------------------------------------------------------
    @brief Public default ctor 
    
    Needed if copy constructor is defined for a class
    --------------------------------------------------------------------------*/
    SAPDB_Singleton() {}
protected:
    /*!-------------------------------------------------------------------------
    @brief Protected copy ctor
    prevents unwanted copy of singleton
    --------------------------------------------------------------------------*/
    SAPDB_Singleton(const SAPDB_Singleton &) {}
    /*!-------------------------------------------------------------------------
    @brief Protected assignment operator
    prevents unwanted assignments of singleton
    --------------------------------------------------------------------------*/
    SAPDB_Singleton & operator=(const SAPDB_Singleton &) { return *this; }
}; /*! @endclass SAPDB_Singleton */

/*!
   @class SAPDB_SingletonTemplate
   @brief Alternative for class SAPDB_Singleton including Instance method

   Needs a default constructor for the class Singleton
 */
template <class Singleton>
class SAPDB_SingletonTemplate
{
public :
    /*!
      @brief yields the singleton instance
      @return reference to the singleton instance
    */
    inline static Singleton& Instance();
protected :
    /*!-------------------------------------------------------------------------
    @brief Protected default ctor 
    Needed if copy constructor is defined for a class
    --------------------------------------------------------------------------*/
    SAPDB_SingletonTemplate() {}
    /*!-------------------------------------------------------------------------
    @brief Protected copy ctor
    prevents unwanted copy of singleton
    --------------------------------------------------------------------------*/
    SAPDB_SingletonTemplate(const SAPDB_SingletonTemplate &) {}
    /*!-------------------------------------------------------------------------
    @brief Protected assignment operator
    prevents unwanted assignments of singleton
    --------------------------------------------------------------------------*/
    SAPDB_SingletonTemplate & operator=(const SAPDB_SingletonTemplate &) { return *this; }
}; /*! @endclass SAPDB_SingletonTemplate */

/*!
  @brief yields the singleton instance
  @return reference to the singleton instance
*/
template <class Singleton>
inline Singleton& SAPDB_SingletonTemplate<Singleton>::Instance()
{
  // Static memory for the m_Instance member
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(Singleton));
  static Singleton* Instance = 0;

  if (Instance == 0)
  {
    static RTESync_Spinlock lock;
    RTESync_LockedScope scope(lock);
    if (Instance == 0)
    {
      Instance = new(Space) Singleton;
    }
  }
  return *Instance;
}

/// obsolete... to be removeed by ThomasA ;-)
#define SINGLETON_INIT_MEMBER(X) // class X;template<> X* SAPDB_SingletonTemplate<X>::m_Instance = 0; 

#endif /* SAPDB_SINGLETON_HPP */

