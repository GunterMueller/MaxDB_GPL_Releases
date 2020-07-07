/*!---------------------------------------------------------------------
  @file           RTE_NTService.hpp
  @author         RobinW, RaymondR
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

#ifndef RTE_NTSERVICE_HPP
#define RTE_NTSERVICE_HPP

#include "RunTime/RTE_SpeedInfo.hpp"

/*!---------------------------------------------------------------------------
  @struct  SERVERDB_PARAM_REC
  @brief   internally used only
 ----------------------------------------------------------------------------*/
struct SERVERDB_PARAM_REC;

/*!---------------------------------------------------------------------------
  @class   RTE_NTService
  @brief   Windows service entry management

           This class is designed for queues dealing with
           common tasking queue elements.


 ----------------------------------------------------------------------------*/
class RTE_NTService
{
public:
    /*!
    @brief          Constructor.

                    Constructor to create a service entry object.
                    Only internal data structures are filled

    @param          dbName          [in] -  the name of the database
                    dbRoot          [in] -  the dbroot of the database
                    PgmName         [in] -  the name of the executable.
                                            If 0, the standard executable name is chosen
                    NodeName        [in] -  the name of the node to manage the service on
                                            If 0, the local node is chosen
                    speed           [in] -  The speed of the service. 
                                            Currently speedFast, speedQuick, speedSlow and SpeedTest are avaiable
    */
    RTE_NTService(  const SAPDB_Char *dbName,
                    const SAPDB_Char *dbRoot,
                    const SAPDB_Char *PgmName,  
                    const SAPDB_Char *NodeName,
                    RTE_SpeedInfo &speed);

    /*!
    @brief          Destructor.

                    Destructor to delete a service entry object.
                    Only internal data structures are deleted

    */
    ~RTE_NTService();

    /*!
    @brief          Write new values for a service entry to the registry. 
                    If the service entry does not exist, it is created.

    @param          Start           [in] -  automatic: the service is to be started automatically on reboot
                                            demand: the service must be started manually
                                            unchanged: do not change the current setting
                    pszAccountName  [in] -  the name of the user account the service is to be created for
                                            If 0, the service is created for the system user
                                            If RTE_SERVICE_ENTRY_UNCHANGED, the current setting is not changed
                    pszPassword     [in] -  password for the user account given (ignored if no user account is given)
                    pszParameter    [in] -  the parameter of the service that is to be changed
                                            If 0, no parameter is changed
                    pszValue        [in] -  the new value of the parameter
                    errList         [out]-  filled if something went wrong. 

    @return         false if something went wrong, true if everything is allright

    */
    SAPDB_Bool UpdateServiceEntry ( RTE_StartOption        Start,
                                    SAPDB_Char             *pszAccountName,
                                    SAPDB_Char             *pszPassword,
                                    SAPDB_Char             *pszParameter,
                                    SAPDB_Char             *pszValue,
                                    SAPDBErr_MessageList   &errList);

    /*!
    @brief          Remove a service entry from registry

    @param          errList         [out]-  filled if something went wrong. 

    @return         false if something went wrong, true if everything is allright

    */
    SAPDB_Bool RemoveServiceEntry ( SAPDBErr_MessageList   &errList);

    /*!
    @brief          Read the name of the executable from registry

    @param          executable      [out]-  filled with the executable name

    @return         system error code

    */
    APIRET GetExecutableName(RTE_Path &executable);

    /*!
    @brief          Read the the autostart flag from registry

    @param          autoStart       [out]-  true if the autostart flag is set, false if not

    @return         system error code

    */
    APIRET GetAutoStartFlag(SAPDB_Bool &autoStart);

    /*!
    @brief          Start the service

    @param          options         [out]-  command line that is passed to the service. May be 0

    @return         system error code

    */
    APIRET Start(SAPDB_Char *options);

    /*!
    @brief          Stop the service

    @return         system error code

    */
    APIRET Stop(void);

    /*!
    @brief          Get the state of the service

    @param          dwCurrentState  [out]-  filled with the state of the service

    @return         system error code

    */
    APIRET GetStatus(DWORD &dwCurrentState);

private:
    SAPDB_Bool      makeErrListFromApiRet(APIRET rc,SAPDBErr_MessageList &errList);

    SAPDB_Char      *versionFromParam   ( SAPDB_Char  *KernelVersion );
    APIRET          getAccountName      ( SAPDB_Char *pszAccountName,SAPDB_Int ulMaxAccountNameLen );
    APIRET          readParameters      ( SERVERDB_PARAM_REC *Params );
    APIRET          alterParameter      ( SERVERDB_PARAM_REC *Params,SAPDB_Char *pszParameter,SAPDB_Char *pszValue );
    APIRET          updateParameters    ( SERVERDB_PARAM_REC *Params );

    RTE_DBName      m_dbName; 
    RTE_Path        m_dbRoot;
    RTE_SpeedInfo   m_speed;
    SC_HANDLE       m_hScM;
    SAPDB_Char      *m_serviceName;
    SAPDB_Char      *m_serviceDisplayString;
    SAPDB_Char      *m_executableNameWithPath;
    SAPDB_Char      *m_NodeName;
    APIRET          m_errorInConstructor;
};


#endif
