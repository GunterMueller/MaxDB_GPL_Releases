/*!
  @file           geo573.h
  @author         RaymondR
  @special area   Global scope new and delete.
  @brief          Global scope new and delete.
  @see


                This operator are using the 'teo570_GlobalAlloc' class.

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/



#ifndef GEO573_H
#define GEO573_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(WIN32)
#include <new>
#else
#include <new.h>
#endif

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
 * \brief Bad_alloc handler calling debug functionality, before bad_alloc, if registered.
 *
 * This is used only from DbpError implementation and throw of bad_alloc.
 */
class geo573_BadAllocHandler {
public:
  /// Callback function type
  typedef void (*Callback)();
  /// Set a callback to call on bad_alloc
  inline static void setCallback(Callback cb) { m_cb = cb; }
  /// Call the bad_alloc callback
  inline static void callCallback()
  {
    if (m_cb)
      m_cb();
  }
private:
  /// Registered callback or NULL
  static Callback m_cb;
};

#if defined(HPUX)
#define BAD_ALLOC_GEO573 bad_alloc
#else
#define BAD_ALLOC_GEO573 std::bad_alloc
#endif

#if defined(AIX) && __IBMCPP__ < 500
#define THROW_NEW_GEO573
#endif

#if defined(SUN) || defined(SOLARIS) || defined(OSF1) 
#define THROW_NEW_GEO573 throw(std::bad_alloc)
#endif

#if defined (HPUX)
#define THROW_NEW_GEO573 throw(bad_alloc)
#endif

#if defined (_WIN32)
#define THROW_NEW_GEO573 _THROW1(std::bad_alloc)
#endif

#if !defined (THROW_NEW_GEO573)
#define THROW_NEW_GEO573 throw(std::bad_alloc)
#endif

#if defined(SUN) || defined(SOLARIS) || defined (HPUX) || defined(OSF1) || (defined(AIX) && __IBMCPP__ >= 500) || defined (FREEBSD)
#define THROW_DELETE_GEO573 throw()
#endif

#if defined (_WIN32)
#define THROW_DELETE_GEO573 _THROW0()
#endif

#if !defined (THROW_DELETE_GEO573)
#define THROW_DELETE_GEO573
#endif

#if defined(OSF1) || defined(SUN) || defined(SOLARIS) || defined(LINUX) || defined(NMP) || defined (FREEBSD)
#define THROW_BAD_ALLOC_GEO573 geo573_BadAllocHandler::callCallback(); throw std::bad_alloc()
#else
#if defined(HPUX)
#define THROW_BAD_ALLOC_GEO573 geo573_BadAllocHandler::callCallback(); throw bad_alloc()
#else
#if defined(AIX) && __IBMCPP__ < 500
#define THROW_BAD_ALLOC_GEO573 geo573_BadAllocHandler::callCallback()
#else
#define THROW_BAD_ALLOC_GEO573 geo573_BadAllocHandler::callCallback(); throw std::bad_alloc("")
#endif
#endif
#endif

void *operator new ( size_t    Size ) THROW_NEW_GEO573;

void operator delete ( void * Buffer ) THROW_DELETE_GEO573;

void *operator new[] ( size_t Size) THROW_NEW_GEO573;

void operator delete[]  ( void * Buffer ) THROW_DELETE_GEO573;

#if defined (_WIN32)
// allocate or return null pointer
void *__cdecl operator new  (size_t Size, const std::nothrow_t&) throw();
void *__cdecl operator new[](size_t Size, const std::nothrow_t&) throw();

void __cdecl operator delete  (void *, const std::nothrow_t&)    throw();
void __cdecl operator delete[](void *, const std::nothrow_t&)    throw();
#endif

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO573_H */
