/*!---------------------------------------------------------------------
  @file           RTE_FileUser.hpp
  @author         Christoph
  @brief          Find any process who is using a file as executable or module
  @see            

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
---------------------------------------------------------------------*/

#ifndef RTE_FILEUSER_H
#define RTE_FILEUSER_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

#ifndef WIN32
/*! if not windows, it must be UNIX */
#define UNIX UNIX
#endif

#ifdef _WIN64
/*! windows compiler do not automatically defined ptr_t */
typedef __int64 ptr_t;
#else
/*! windows compiler do not automatically defined ptr_t */
typedef long ptr_t;
#endif

#ifdef UNIX
#include <sys/types.h>
#endif

/*! linked list of module infos */
struct modinfo {
	char *name; ///< module name
	ptr_t pid;  ///< associated process id
	modinfo *next; ///< next modinfo
};

/*! linked list of process infos */
struct procinfo {
	ptr_t pid;   ///< process id
	ptr_t ppid;  ///< parent process id
	ptr_t uid;   ///< user id
	char *cmd;   ///< command
	procinfo *next; ///< next procinfo
};
/*!
 @class RTE_ProcInfo
 @brief process info chain acess class
 */
class RTE_ProcInfo {
public:
    /*! @brief ctor */
	RTE_ProcInfo (procinfo *);
    /*! @brief dtor */
	~RTE_ProcInfo ();
    /*!
      @brief next process info in chain
      @param current [in] current procinfo
      @return next in procinfo chain
     */
	procinfo *Next (procinfo *current);
    /*!
      @brief first process info in chain
      @return first entry in procinfo chain
     */
	procinfo *First ();
    /*!
      @brief terminate process
      @param toTerminate [in] process to terminate
      @return exit code
     */
	int Terminate (procinfo *toTerminate);
private:
	procinfo *pi; ///< contain hook to procinfo
}; /* endclass RTE_ProcInfo */

/*!
 @class RTE_FileUser
 @brief process info chain acess class
 */
class RTE_FileUser {
public:
    /*! @brief ctor */
	RTE_FileUser ();
    /*! @brief dtor */
	~RTE_FileUser ();

    /*! 
    @brief get process chain using given module
    @param moduleName [in] the module name
    @return pointer to process info chain
     */
	RTE_ProcInfo WhoUsesModule (const char *moduleName);

private:
	modinfo  *mi; ///< moduile info
	procinfo *ps; ///< process info
}; /* endclass RTE_FileUser */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
#endif

