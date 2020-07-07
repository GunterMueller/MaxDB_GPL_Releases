/*!---------------------------------------------------------------------
  @file           RTE_DBRegister.hpp
  @author         JoergM, RobinW
  @brief          DBM Support: SAPDB Instance and Database Registration and Management Interfaces
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "Container/Container_List.hpp"
#include "RunTime/RTE_Installation.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

//typedef SAPDB_Char RTE_DBKey[256];

class RTE_DBRegister;
class RTE_Installation;
class RTE_ActiveInstallation;
class RTE_Database;
class RTE_ActiveDatabase;


/*!
    @brief  a list of speeds
*/
typedef Container_List<RTE_SpeedInfo> RTE_SpeedList;


/*!
  @class    RTE_DBRegister
  @brief    Register of installations and databases
            Installations are handled as instances of the classes passive RTE_Installation and RTE_ActiveInstallation.
            A passive installation allows to collect information about an installation. An active installation 
            adds the ability to register/unregister databases.

 */
class RTE_DBRegister
{
public:
    
    /*!
        @brief  The one and only instance of the RTE_DBRegister singleton
                Being created if it did not exist before
        
        @return A reference to the one and only...
     */
    static RTE_DBRegister &Instance();

    /*!
        @brief  Register a given Installation
                Registration of a given installation means to store VersionInfo and path to DbRoot in the
                register. If another installation with the same DbRoot it is overwritten and a warning is
                reported.
        
        @param  dbRoot  [in]    the root path of the database
        @param  version [in]    the version of the database
        @param  errLíst [out]   contains an error list if unsuccessful
        @return true if success, false if not
     */
    virtual bool RegisterInstallation( const RTE_Path dbRoot, 
                                       const RTE_Installation::InstVersionInfo &version,
                                       SAPDBErr_MessageList &errList);
    /*!
        @brief  Unregister a given Installation
                The registration identified by DbRoot is removed from register. Only installations, that
                contain no databases are removable.

        @param  dbRoot  [in]    the root path of the database
        @param  errList [out]   contains an error list if unsuccessful
        @return true if success, false if not
     */
    virtual bool UnregisterInstallation( const RTE_Path dbRoot,
                                         SAPDBErr_MessageList &errList);

    /*!
        @brief  Get a container filled with the passive installation instance references
                The installations are only suitable to enumerate registered installations and databases.
                They are not usefull to add databases or start/stop them. Use GetActiveInstallation()
                or GetActiveDatabase() for these purposes.

        @return a list of installations registered on the machine
     */
    virtual Container_List<RTE_Installation>* GetInstallations();

    /*!
        @brief   Get an passive installation instance reference using dbName

        @param   dbName  [in]    the name of the database
        @param   errList [out]   contains an error list if unsuccessful
        @return  a pointer to the installation the database belongs to - NULL if not found
     */
    virtual RTE_Installation * GetInstallationByDBName( const RTE_DBName dbName,
                                                        SAPDBErr_MessageList &errList);
    /*!
        @brief  Get an passive installation instance reference using dbRoot

        @param  dbName  [in]    the root path of the database
        @param  errList [out]   contains an error list if unsuccessful
        @return a pointer to the installation the database belongs to - NULL if not found
     */
    virtual RTE_Installation * GetInstallationByDBRoot( const RTE_Path   dbRoot,
                                                        SAPDBErr_MessageList &errList);

    /*!
        @brief  Get an active installation instance reference using dbName

        @param   dbName  [in]    the name of the database
        @param   errList [out]   contains an error list if unsuccessful
        @return  a pointer to the installation the database belongs to - NULL if not found
     */
    virtual RTE_ActiveInstallation * GetActiveInstallationByDBName( const RTE_DBName dbName,
                                                                    SAPDBErr_MessageList &errList);
    /*!
        @brief  Get an active installation instance reference using dbRoot

        @param  dbName  [in]    the root path of the database
        @param  errList [out]   contains an error list if unsuccessful
        @return a pointer to the installation the database belongs to - NULL if not found
     */
    virtual RTE_ActiveInstallation * GetActiveInstallationByDBRoot( const RTE_Path   dbRoot,
                                                                    SAPDBErr_MessageList &errList);

    /*!
        @brief  Get a database instance reference using dbName

        @param   dbName  [in]    the name of the database
        @param   errList [out]   contains an error list if unsuccessful
        @return  a pointer to the database - NULL if not found
     */
    RTE_Database * GetDatabase( const RTE_DBName dbName,
                                SAPDBErr_MessageList &errList);

    /*!
        @brief  Get an active database instance reference using dbName

        @param   dbName  [in]    the name of the database
        @param   errList [out]   contains an error list if unsuccessful
        @return  a pointer to the active database - NULL if not found
     */
    RTE_ActiveDatabase * GetActiveDatabase( const RTE_DBName dbName,
                                            SAPDBErr_MessageList &errList);

private:
    static RTE_DBRegister *m_Instance;
    RTE_DBRegister(){};
    RTE_DBRegister(const RTE_DBRegister &) {}
    SAPDB_Bool getInstVersionFromRegistry(
        RTE_Path instroot,
        RTE_Installation::InstVersionInfo &instversion,
        SAPDB_Int1 dbKind,
        SAPDB_Bool &inRegistry,
        SAPDBErr_MessageList &err);
#ifdef _WIN32
    SAPDB_Bool openInstRegistry (
        HKEY &keyHandle);
    void replaceCharsInString (
        SAPDB_Char* string, 
        SAPDB_Char fromChar, 
        SAPDB_Char toChar);
#endif
    void parseVersion (
        const char * versionString,
        RTE_Installation::InstVersionInfo & versionInfo);
}; /* Endclass: RTE_DBRegister */

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
#include "gsp09.h"

/*!
    @brief  convert the contents of a SAPDBErr_MessageList instance into a tsp9_rte_xerror struct
            Needed only as "glue" for old interface wrappers    

*/
void RTE_FillXErrorFromErrList(tsp9_rte_xerror* xerror,SAPDBErr_MessageList &errList);

/*!
    @brief  convert the contents of a tsp9_rte_xerror struct into a SAPDBErr_MessageList instance 
            Needed only as "glue" for old interface wrappers    

*/
void RTE_FillErrListFromXError(SAPDBErr_MessageList &errList,tsp9_rte_xerror* xerror);

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
