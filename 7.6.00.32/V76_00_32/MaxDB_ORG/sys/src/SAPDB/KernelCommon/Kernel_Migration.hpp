/**************************************************************************/
/*!

  @file         Kernel_Migration.hpp
  @author       UweH
  @brief        This implements the class Kernel_Migration.
  @since        2003-06-30
*/
/*!
\if EMIT_LICENCE

  copyright:    Copyright (c) 2001-2005 SAP AG


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
#ifndef KERNEL_MIGRATION_HPP
#define KERNEL_MIGRATION_HPP

#include "KernelCommon/Kernel_Version.hpp"

class Msg_List;
struct Kernel_MigrationInfoList;

/// maximum of persistent change list
#define MAX_CHANGE_INFO     14
#define INVALID_CHANGE_INFO MAX_CHANGE_INFO - 1

/*!
    @brief        This class bundles the migration of persistent structures within the kernel.
    @ingroup      KernelCommon
*/
class Kernel_Migration
{
    public:
        
        /// These components independently check migration needs and compatibility
        enum PersistentComponent
        {
            all         = 0,
            data        = 1,
            log         = 2,
            catalog     = 3,
            filedir     = 4,
            converter   = 5,
            MaxPersistentComponent_do_not_use = 6
        };
        
        /// one element of change list
        struct ChangeInfo
        {
            const SAPDB_Char*         changeTitle;       ///< readable title/description of defined migration
            const PersistentComponent component;         ///< component which needs migration
            const bool                livecacheOnly;     ///< true if it is relevant for livecache instances
            const Kernel_Version      changedInVersion;  ///< the software version which contains the migration
            bool                      migrationEnabled;  ///< true, if the current migration is enabled.
            
            /// constructor used for static initialization
            ChangeInfo(const SAPDB_Char*         newChangeTitle,
                       const PersistentComponent newComponent,
                       const bool                newLivecacheOnly,
                       const Kernel_Version      newChangedInVersion)
            : changeTitle      (newChangeTitle),
              component        (newComponent),
              livecacheOnly    (newLivecacheOnly),
              changedInVersion (newChangedInVersion),
              migrationEnabled (false)
            {}

            /// default construtor sets all to null
            ChangeInfo()
            : changeTitle      (0),
              component        (data),
              livecacheOnly    (false),
              changedInVersion (Kernel_Version()),
              migrationEnabled (false)
            {}
            
            /// returns true if the migration element is used
            bool IsValid() const
            {
                return changeTitle != 0;
            }

            /// returns true if the migration element is enabled.
            bool IsEnabled() const
            {
                return IsValid() && migrationEnabled;
            }
        };
        
        /// Compatibility of persistent data is checked.
        /// false is returned if the software version is older then the checkVersion.
        bool CheckCompatibility(PersistentComponent   checkComponent,
                                const Kernel_Version &checkVersion,
                                Msg_List             &errlist);

        /// stores information about an executed migration. This information will be stores into
        /// table SYSDBA.UPGRADEHISTORY at the end of the restart
        void CollectMigrationInfo (PersistentComponent Component, const SAPDB_Char* Comment);
        /// yields the next migration info
        bool NextMigrationInfo (bool First, const SAPDB_Char* &pComponent, const SAPDB_Char* &pComment, const SAPDB_Char* &pTimeStamp);
        /// Search for a named persistent change.
        /// If it is not found the returns ChangeInfo is invalid.
        /// changeTitle must be given as a 0 terminated ascii string.
        ChangeInfo & SearchForChangeInfo (const SAPDB_Char * changeTitle);
        /// the constan variant
        const ChangeInfo & SearchForChangeInfo (const SAPDB_Char * changeTitle) const;
        
        /// This compares all changes and returns the latest change version.
        static Kernel_Version GetOldestCompatibleVersion(PersistentComponent checkComponent = all);
        
        static bool Test(Msg_List &errlist); ///< Self Test

    private:
       
        static SAPDB_Char* PersistentComponentStrings [MaxPersistentComponent_do_not_use];
        static ChangeInfo                m_ChangeInfos[MAX_CHANGE_INFO];
        static Kernel_MigrationInfoList& m_migrationInfoList; 
};
#endif
