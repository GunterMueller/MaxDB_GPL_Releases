/*!---------------------------------------------------------------------
  @file           RTE_ActiveInstallation.hpp
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


#ifndef RTE_ACTIVEINSTALLATION_HPP
#define RTE_ACTIVEINSTALLATION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
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

class RTE_DBRegister;
class RTE_ActiveDatabase;


/*!
  @class RTE_ActiveInstallation
  @brief Active installation information class

  It allows to register and unregister databases and to retrieve a list of active databases
 */
class RTE_ActiveInstallation: public RTE_Installation
{
public:
    /*!
      @brief    static instance that represents an invalid element
    */
    static RTE_ActiveInstallation InvalidActiveInstallation;

    /*!
      @brief        Get a container of registered databases
                    This container allows to iterate all registered databases for this
                    installation.

      @param        errList     [out]   Filled with message in case of trouble
      @return value The database list (may be empty)
     */
    Container_List<RTE_ActiveDatabase *>& GetActiveDatabases(SAPDBErr_MessageList &errList);

    /*!
      @brief        Get the service database of this installation

      @param        errList     [out] Filled with message in case of trouble
      @return value true if call suceeded, false if not (errList filled)
    */
    RTE_Database * GetServiceDB(SAPDBErr_MessageList &errList);

    /*!
      @brief        register a database. Update version, autostart flag and user.
                    If the database already exists, the values are overwritten!
      @param        dbName      [in]    Database name
      @param        autoStart   [in]    flag that indicates that the database is started automatically (valid on Windows only)
      @param        account     [in]    operating system user to register the service for (Windows only)
                                        Specify NULL for LocalSystem.
      @param        password    [in]    password of the operating system user to register the service for (Windows only)
                                        Is ignored if the database is to be registered for LocalSystem.
      @param        errList     [out]   Filled with message in case of trouble
      @return value true if call suceeded, false if not (errList filled)
    */
    bool RegisterDatabase( const RTE_DBName dbName, SAPDB_Bool autoStart,
                           SAPDB_Char *account,SAPDB_Char *password,
                           SAPDBErr_MessageList &errList)
    {
        return RegisterDatabase( dbName, autoStart ? RTE_StartAutomatic : RTE_StartOnDemand, account, password, errList);
    }

    /*!
      @brief        register a database. Update version only, leave all other values untouched.

      @param        dbName      [in]    Database name
      @param        errList     [out]   Filled with message in case of trouble
      @return value true if call suceeded, false if not (errList filled)
    */
    bool RegisterDatabase( const RTE_DBName dbName,
                           SAPDBErr_MessageList &errList)
    {
        return RegisterDatabase( dbName, RTE_StartUnchanged, (SAPDB_Char *)RTE_SERVICE_ENTRY_UNCHANGED, (SAPDB_Char *)RTE_SERVICE_ENTRY_UNCHANGED, errList);
    }
    /*!
      @brief        Unregister database from this installation

      @param        dbName      [in]    Name of database to unregister
      @param        errList     [out]   Filled with message in case of trouble
      @return value true if call suceeded, false if not (errList filled)
    */
    bool UnregisterDatabase( const RTE_DBName dbName, 
                             SAPDBErr_MessageList &errList);

private:
    bool RegisterDatabase( const RTE_DBName dbName, 
                           RTE_StartOption autoStart, 
                           SAPDB_Char *account, SAPDB_Char *password,
                           SAPDBErr_MessageList &errList);
 
    RTE_ActiveInstallation(RTE_Path &dbRoot, InstVersionInfo &version,SAPDB_Bool inRegistry)
        : RTE_Installation(dbRoot,version,inRegistry)
    {}

    friend class RTE_DBRegister;    // these friend declarations are necessary to be able to create
                                    // an ActiveInstallation (and where present for Installation)
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif
