/*!========================================================================
  
  @ingroup      IPC
  @file         RTEIPC_NamedSharedMemory.hpp
  @author       StefanP

  @brief        Primitives for Named Shared Memory Handling

  @since        2004-07-14  14:46
  @sa           

  =======================================================================

  \if EMIT_LICENCE
    
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
  ----------------------------------------------------------------------------*/


#ifndef RTEIPC_NAMEDSHAREDMEMORY_HPP
#define RTEIPC_NAMEDSHAREDMEMORY_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(AIX)// || defined(LINUX)
#define RTE_WELL_KNOWN_RAMDISK_PATH         "/rel/ramdisk/SAPDB/"
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



/*!
    @class  RTEIPC_NamedSharedMemory
    @brief	Basic class for Named Shared Memory handling

            Named shared memory objects can be shared between different processes. 
            This class offers the basic methods for named shared memory handling. 

            One can create a new object as well as open an existing object. The constructor does not 
			create/open the shared memory object. This is first done by a call to the 'Open' method. 

            The base address for the shared memory object can either be determined by the system or
            can be assigned to a given, fixed address.
*/

class   RTEIPC_NamedSharedMemory
{
public:

    enum SHMRet {NoError = 0,		///<No error
                 Error,				///<Error not specified
                 ResourceMissing,	///<Shared Memory object is missing
                 BadAddress,		///<Shared memory object could not be mapped at the given address
                 AlreadyExist		///<Shared memory object already exists
                };

  /*!
     @brief			Constructor

                    The constructor does not open or create the shared memory. This is done by a call 
                    to the open method.
     
     @return        none
  */
    
    RTEIPC_NamedSharedMemory        ();

  /*!
     @brief			Destructor
     @return        none
  */

    ~RTEIPC_NamedSharedMemory      ();

  /*!
     @brief			Open or create a shared memory object (obsolet)
                     
     @param         pSharedMemoryName [in]	- Name of the shared memory object (e.g. SDBTech-COMMSEG-\<UniqueID\>).
											  Special conventions regarding the name have not to be noticed. This is done
											  by the open method itself (e.g. 
												SUN: "SDBTech-COMMSEG-\<UniqueID\>" ->  "/SDBTech-COMMSEG-\<UniqueID\>"
												SOLARIS: "SDBTech-COMMSEG-\<UniqueID\>" ->  "/SDBTech-COMMSEG-\<UniqueID\>"
												NT:  "SDBTech-COMMSEG-\<UniqueID\>" ->  "Global\\SAPDBTech-COMMSEG-\<UniqueID\>"

                                              There is no limitation to the length of the name.
     @param         pBaseAddress [in]		- Address at which the shared memory should be mapped to.
                                              The address has to be a multiple of the system page size. 
                                              If the shared memory can not be assigned to this address the
                                              functions returns 'BadAddress'.
     @param         openMode [in]			- RTE_OpenExisting or RTE_OpenNew
     @param         secAttribute [in]		- Reference of the security attribute for creating the shared memory.
  										      Is not relevant for opening. In that case 
													RTESys_SecurityAttributes::Instance().GetNoSA () can be 
											  given.	
											  !!!Caution: For UNIX the security attribute (file permission) has to be given
														  absolut, i.e. an existing umask is not considered.

     @param         shmSize [in]			- Size of the shared memory to be created. The size has to be a
                                              multiple of system page size!
     @param         ppSharedMemory [out]	- Pointer to the address of the shared memory
     @param         messageList [out]		- Message list

     @return        NoError, Error, ResourceMissing, BadAddress, AlreadyExist 

  */

    SHMRet           Open               (SAPDB_Char const * const           pSharedMemoryName,
                                         void *                             pBaseAddress,
                                         RTE_OpenMode                       openMode,
                                         RTE_SECURITY_ATTRIBUTES &          secAttribute,   
                                         SAPDB_ULong                        shmSize,
                                         void **                            ppSharedMemory,                   
                                         SAPDBErr_MessageList &             messageList);


  /*!
     @brief			Open or create a shared memory object 
                     
     @param         pSharedMemoryName [in]	- Name of the shared memory object (e.g. SDBTech-COMMSEG-\<UniqueID\>).
											  Special conventions regarding the name have not to be noticed. This is done
											  by the open method itself (e.g. 
												SUN: "SDBTech-COMMSEG-\<UniqueID\>" ->  "/SDBTech-COMMSEG-\<UniqueID\>"
												SOLARIS: "SDBTech-COMMSEG-\<UniqueID\>" ->  "/SDBTech-COMMSEG-\<UniqueID\>"
												NT:  "SDBTech-COMMSEG-\<UniqueID\>" ->  "Global\\SAPDBTech-COMMSEG-\<UniqueID\>"

                                              There is no limitation to the length of the name.
     @param         pBaseAddress [in]		- Address at which the shared memory should be mapped to.
                                              The address has to be a multiple of the system page size. 
                                              If the shared memory can not be assigned to this address the
                                              functions returns 'BadAddress'.
     @param         openMode [in]			- RTE_OpenExisting or RTE_OpenNew
     @param         secAttribute [in]		- Reference of the security attribute for creating the shared memory.
  										      Is not relevant for opening. In that case 
													RTESys_SecurityAttributes::Instance().GetNoSA () can be 
											  given.	
											  !!!Caution: For UNIX the security attribute (file permission) has to be given
														  absolut, i.e. an existing umask is not considered.

     @param         shmSize [in]			- Size of the shared memory to be created. The size has to be a
                                              multiple of system page size!
     @param         messageList [out]		- Message list

     @return        NoError, Error, ResourceMissing, BadAddress, AlreadyExist 

  */

    SHMRet           Open               (SAPDB_Char const * const           pSharedMemoryName,
                                         void *                             pBaseAddress,
                                         RTE_OpenMode                       openMode,
                                         RTE_SECURITY_ATTRIBUTES &          secAttribute,   
                                         SAPDB_ULong                        shmSize,
                                         SAPDBErr_MessageList &             messageList)
    {
        return     Open ( pSharedMemoryName, pBaseAddress, openMode, secAttribute, shmSize, 0, messageList);
    }


 /*!
     @brief			Get address of the shared memory
                     
     @return        Address of the shared memory
 */
    void*           GetSharedMemoryAddress() { return m_pMem; };


 /*!
     @brief			Get size of the shared memory
                     
     @return        Size of the shared memory
 */
    SAPDB_ULong     GetSharedMemorySize() { return m_Size; };

  /*!
     @brief			Get the allocation granularity of shared memory segment
  */
    static SAPDB_UInt4     AllocGranularity();

 /*!
     @brief			Close a shared memory object
                     
     @param         messageList [out]		- Message list
     @return        Error, NoError
 */


    SHMRet          Close               (SAPDBErr_MessageList &         messageList);

  /*!
     @brief			Lock the shared memory object, i.e. the shared memory object can not further be attached.
					(May be done for security reasons)
  */

    SAPDB_Bool      Lock                (SAPDBErr_MessageList &         messageList);


#if !defined (_WIN32)
  /*!
     @brief			Change the owner of the shared memory object to the owner of the calling process
  */

    SAPDB_Bool      ChangeOwner         (SAPDB_Int4                     groupId,
                                         SAPDBErr_MessageList &         messageList);
#endif

private:
  /*!
     @brief			Prevent calling the copy constructor from outside
  */

	inline          RTEIPC_NamedSharedMemory  (const RTEIPC_NamedSharedMemory & shm) {;};

#if !defined (_WIN32)
  /*!
     @brief			Unlink the shared memory object
  */

    SAPDB_Int       UnlinkSHMObject     ();

  /*!
     @brief			Open respectively create a shared memory object
  */

    SAPDB_Int       OpenSHMObject       (SAPDB_Int const                openFlags, 
                                         mode_t const                   secAttribute = (mode_t)0);
#endif

    SAPDB_ULong                         m_Size;

    RTE_FileHandle                      m_Handle;
    void *                              m_pMem;
    SAPDB_Char *                        m_pName;
    RTE_OpenMode                        m_OpenMode;
};

 /*!
	@brief Convert return codes from methods of the named shared memory class into readable strings

	@param			rc [in]			- return code to convert
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTEIPC_NamedSharedMemory::SHMRet     rc)
{
    switch (rc)
    {
    case RTEIPC_NamedSharedMemory::NoError: 
        return SAPDB_ToStringClass("NOERROR");
    case RTEIPC_NamedSharedMemory::ResourceMissing: 
        return SAPDB_ToStringClass("RESOURCEMISSING");
    case RTEIPC_NamedSharedMemory::BadAddress: 
        return SAPDB_ToStringClass("BADADDRESS");
    case RTEIPC_NamedSharedMemory::AlreadyExist: 
        return SAPDB_ToStringClass("ALREADYEXIST");
    case RTEIPC_NamedSharedMemory::Error: 
        return SAPDB_ToStringClass("ERROR");
    default:                          
        break;
    }

    return SAPDB_ToStringClass((SAPDB_Int4)rc); 
}


#endif  /* RTEIPC_NAMEDSHAREDMEMORY_HPP */
