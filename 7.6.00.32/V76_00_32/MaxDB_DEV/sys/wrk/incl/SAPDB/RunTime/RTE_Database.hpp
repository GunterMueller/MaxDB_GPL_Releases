/*!---------------------------------------------------------------------
  @file           RTE_Database.hpp
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


#ifndef RTE_DATABASE_HPP
#define RTE_DATABASE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_SpeedInfo.hpp"
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


/*!
  @class RTE_Database
  Description: Passive database information class

  It allows to get status information of registered databases.
  Status information is separated in registration information
  and the active database status. Registration information is
  root directory (GetDbRoot), database name (GetDbName), 
  corresponding installation (GetInstallation) and speed list
  (GetDbSpeeds). Active database status is summarized in 
  DbSpeedInfo (speedName,speedId and flag that shows speed is
  registered for this database) via (GetDbStatus).  
 */

class RTE_Database
{
public:
    /*! 
        @struct     DBStatusInfo
        @brief      a struct able to contain a databases status info represented as a unique id and a printable name
                    
    */
    struct DBStatusInfo
    {
        const SAPDB_UTF8 *statusName;   //!< printable status name (zero terminated)
        SAPDB_Int4        statusId;     //!< unique status id
        /*!
          @brief ctor
          @param id [in] unique status id
          @param name [in] printable status name
         */
        DBStatusInfo(SAPDB_Int4 id, const SAPDB_UTF8 *name) //!< ctor
         : statusId(id), statusName(name) {};

        /*!
         @brief comparison operator
         compare two instances of DBStatusInfo

         @param other [in] the other status info to compare
         @return true if identical status, false if different
         */
        inline bool operator == (const DBStatusInfo &other) const
        {
            return statusId == other.statusId;
        }
    };

    /* registered information */
    /*!
    @brief          

    @return         the root path of the database

    */
    const RTE_Path&     GetDBRoot() const       { return m_dbRoot; }

    /*!
    @brief          

    @return         the name of the database
    */
    const RTE_DBName&   GetDBName() const       { return m_dbName; }

    /*!
    @brief          

    @return         the list of speeds that are registered for the database

    */
    RTE_SpeedList&  GetDBSpeeds()               { return m_speedList; }

    /*!
    @brief          

    @param          speedInfo   [in] -  the speed to add to the databases list of speeds
    */
    void AddDBSpeed(RTE_SpeedInfo &speedInfo)
    {
        m_speedList.InsertEnd(speedInfo);
    };

    /*!
    @brief          

    @param          speed   [in] -  the speed to set as active speed

    */
    void SetActiveSpeed(const RTE_SpeedInfo &speed)
    {
        m_activeSpeed = speed;
    };

    /*!
    @brief          

    @return         the active speed set for the database

    */
    const RTE_SpeedInfo &GetActiveSpeed()
    {
        return m_activeSpeed;
    };

    /*!
    @brief          Change the user account a service is running for
                    
    @param          newAccount  [in] -  the name of the new user account of the service
                                            If 0, the system user (LocalSystem) is used
    @param          newPassword [in] -  password for the user account given (ignored if newAccount is 0)
    @param          errList     [out]-  filled if something went wrong. 

    @return         false if something went wrong, true if everything is allright

    */
    SAPDB_Bool UpdateUserAccount( SAPDB_Char *newAccount, SAPDB_Char *newPassword, SAPDBErr_MessageList &errList );

    /*!
    @brief          Change the autostart flag of a service entry
                    If the service entry does not exist, it is created.

    @param          autoStart   [in] -  true:  the service is to be started automatically on reboot
                                        false: the service must be started manually
    @param          errList     [out]-  filled if something went wrong. 

    @return         false if something went wrong, true if everything is allright

    */
    SAPDB_Bool UpdateAutoStartFlag( SAPDB_Bool autoStart, SAPDBErr_MessageList &errList );

    /*!
    @brief          copy ctor

    @param          templateDB  [in]    the template database to make a copy of
    */
    RTE_Database(const RTE_Database &templateDB)
        :
        m_currentState(templateDB.m_currentState.statusId,(const SAPDB_UTF8 *)templateDB.m_currentState.statusName),
        m_speedList(RTEMem_Allocator::Instance()),
        m_activeSpeed(speedNone)
    {
        strcpy(m_dbRoot , templateDB.m_dbRoot);
        strcpy(m_dbName , templateDB.m_dbName);
        m_speedList.Initialize(templateDB.m_speedList);
        m_activeSpeed = templateDB.m_activeSpeed;
    };

    /*!
    @brief          empty dtor
    */
    ~RTE_Database()
    {
    };
protected:
    /*!
    @brief          constructor

    @param          installation    [in]    the installation the database belongs to
    @param          dbRoot          [in]    the root path of the database
    @param          dbName          [in]    the name of the database
    */
    RTE_Database(RTE_Installation& installation, 
                 const RTE_Path dbRoot, 
                 RTE_DBName dbName )  
                 : 
                   m_currentState(0,(const SAPDB_UTF8 *)"UNKNOWN"),
                   m_speedList(RTEMem_Allocator::Instance()),
                   m_activeSpeed(speedNone)
    {
        strcpy(m_dbRoot , dbRoot);
        strcpy(m_dbName , dbName);
        m_speedList.Initialize(installation.GetSpeedList());
    }

    RTE_Path           m_dbRoot;        //!< the root path of the database
    RTE_DBName         m_dbName;        //!< the name of the database
    DBStatusInfo       m_currentState;  //!< the current state of the database
    RTE_SpeedList      m_speedList;     //!< list of speeds registered for the database
    RTE_SpeedInfo      m_activeSpeed;   //!< the active speed of the database (==speedNone if none has been set)
private:
    friend class RTE_Installation;
    friend class RTE_ActiveInstallation;
    friend class RTE_DBRegister;
};

#endif
