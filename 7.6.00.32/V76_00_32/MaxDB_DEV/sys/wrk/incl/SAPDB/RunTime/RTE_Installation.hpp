/*!
  @file           RTE_Installation.hpp
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
*/


#ifndef RTE_INSTALLATION_HPP
#define RTE_INSTALLATION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "Container/Container_List.hpp"
#include "RunTime/RTE_SpeedInfo.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"

#ifdef _WIN32
#include "gos44.h"      /* nocheck */
#else
#include <unistd.h>     /* nocheck */
#include <sys/stat.h>   /* nocheck */
#include "RunTime/RTE_PidListForExecutable.hpp" /* nocheck */
#endif
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
class RTE_Database;
class PidList;


typedef SAPDB_Char RTE_DBKey[256]; ///< DBKey

/*!
  @class RTE_Installation
  @brief installation access
 */
class RTE_Installation
{
public:
    /*!
       installation version info
     */
    struct InstVersionInfo
    {
        RTE_DBKey   key; ///< DBKey
        SAPDB_Byte  release; ///< release byte
        SAPDB_Byte  major; ///< major number byte
        SAPDB_Byte  minor; ///< minor number byte
        SAPDB_Byte  build; ///< build number byte
    };

    /*!
      @brief Return current dbroot
      @return Reference to dbroot of installation
     */
    const RTE_Path & GetDBRoot() const { return m_dbRoot; }

    /*!
      @brief Return installation version
      @return Reference to dbroot of installation
     */
    const InstVersionInfo & GetVersion() const { return m_Version; }

    /*!
      @brief Get a list of all available speeds
      @return speed list
     */
    RTE_SpeedList& GetSpeedList()
    {
        return m_speedList;
    };

    /*!
      @brief Get a container of registered databases
      
      This container allows to iterate all registered databases for this
      installation.
      @param errList [out] Filled with message in case of trouble
      @return The database list (may be empty)
     */
    Container_List<RTE_Database>* GetDatabases(SAPDBErr_MessageList &errList);

    /*!
       @brief ctor
       @param templateInst [in]
     */
    RTE_Installation(const RTE_Installation &templateInst)
        :
        m_speedList(RTEMem_Allocator::Instance()),
        m_Version(templateInst.m_Version)
        , m_inRegistry(templateInst.m_inRegistry)
    {
        strncpy(m_dbRoot,templateInst.m_dbRoot,sizeof(RTE_Path));
        m_speedList.Initialize(templateInst.m_speedList);
    };
    /// dtor
    ~RTE_Installation()
    {
        ;
    };

protected:
    /*! 
      @brief ctor
      @param dbRoot [in] given root
      @param version [in] given version
     */
    RTE_Installation(RTE_Path &dbRoot, InstVersionInfo &version,SAPDB_Bool inRegistry = false):
        m_speedList(RTEMem_Allocator::Instance())
        , m_inRegistry(inRegistry)
    {
        strncpy(m_dbRoot,dbRoot,sizeof(RTE_Path));
        m_Version = version;

        AddSpeed(speedFast);
        AddSpeed(speedQuick);
        AddSpeed(speedSlow);

        if((version.release == 7 && version.major >= 5  )   // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124053
            ||(version.release == 7 && version.major == 4 && version.minor >= 2 )
            ||(version.release == 7 && version.major == 2 && version.minor == 5 && version.build >= 16)
            ||(version.release == 7 && version.major == 2 && version.minor >= 5 ))
        {
            AddSpeed(speedTest);
        }
    }

    RTE_SpeedList   m_speedList; ///< speed list

    SAPDB_Bool m_inRegistry;

private:
    RTE_Path        m_dbRoot;
    InstVersionInfo m_Version;
    friend class RTE_DBRegister;

    SAPDB_Bool dbHasExecutable (
        const SAPDB_Char *kernelName )
    {
        RTE_Path fullKernelPath;  // PTS 1105631
        SAPDB_Int		    	rc = 0;
#ifdef _WIN32
        SAPDB_sprintf ( fullKernelPath, sizeof(RTE_Path), "%s\\%s", m_dbRoot, kernelName ) ;

        SAPDB_UInt                 ulTmp       = 0;
        SAPDB_Int                  lAtt        = 0;
        rc = sql44c_get_file_info ( fullKernelPath, &lAtt, &ulTmp, &ulTmp );
#else
        SAPDB_sprintf ( fullKernelPath, sizeof(RTE_Path), "%s/pgm/%s", m_dbRoot, kernelName);
        struct stat		statInfo;

        rc = stat ((SAPDB_Char *)fullKernelPath, &statInfo);
#endif
        return (rc == 0);
    }

    void AddSpeed(RTE_SpeedInfo &speed)
    {
        RTE_SpeedInfo newSpeed(speed);
        if(!dbHasExecutable(speed.ExecutableName()))
        {
            newSpeed.SetExists(false);
        }
        m_speedList.InsertEnd(newSpeed);
    }

/*----------------------------------------*/


    SAPDB_Bool dbHasRunningProcess ( RTE_DBName &dbname,RTE_SpeedInfo speed );

#ifndef _WIN32
    SAPDB_Bool dbHasSpecificProcess (
        SAPDB_Char *dbname,
        const SAPDB_Char *kernelpgm);

    SAPDB_Bool MatchInProcFS(RTE_DBName &dbname,RTE_SpeedInfo speed,tsp00_TaskId pid);

    RTE_PidListForExecutable    m_pidListFast;
    RTE_PidListForExecutable    m_pidListQuick;
    RTE_PidListForExecutable    m_pidListSlow;
    RTE_PidListForExecutable    m_pidListTest;

    RTE_PidListForExecutable &getPidList(RTE_SpeedInfo speed)
    {
        if( speed == speedQuick )
            return m_pidListQuick;
        if( speed == speedSlow )
            return m_pidListSlow;
        if( speed == speedTest )
            return m_pidListTest;
        else
            return m_pidListFast;
    }

#endif
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif
