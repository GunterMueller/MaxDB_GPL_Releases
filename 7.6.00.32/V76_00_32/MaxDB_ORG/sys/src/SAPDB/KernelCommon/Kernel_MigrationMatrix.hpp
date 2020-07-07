/**************************************************************************/
/*!

  @file         Kernel_MigrationMatrix.hpp
  @author       UweH
  @brief        This defines the Kernel_MigrationMatrix
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
#ifndef KERNEL_MIGRATION_MATRIX_HPP
#define KERNEL_MIGRATION_MATRIX_HPP

/* --------------------------------------------------------------------------- */
/// This constant says that a migration is only necessary for livecache instances.
#define LIVECACHE_ONLY true
/* --------------------------------------------------------------------------- */
Kernel_Migration::ChangeInfo Kernel_Migration::m_ChangeInfos[] =
{
    // #0
    Kernel_Migration::ChangeInfo( "HISTORY ROOT ID CHECK",
                                  Kernel_Migration::data,
                                  LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:historyRootIdCheck:COMMA_FORMATED>) ),
    // #1
    Kernel_Migration::ChangeInfo( "REPEATED_REDO_FLAG_IN_OPEN_TRANS_FILE",
                                  Kernel_Migration::data,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:RepeatedRedoFlagInOpenTransFile:COMMA_FORMATED>)  ),
    // #2
    Kernel_Migration::ChangeInfo( "CONVERTER VOLUME NO BIT COUNT",     
                                  Kernel_Migration::converter,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:converterVolumeNoBitCount:COMMA_FORMATED>) ),    
    // #3
    Kernel_Migration::ChangeInfo( "OPEN TRANS FILE WITH MINIMAL EOT SEQUENCE",
                                  Kernel_Migration::data,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:openTransFileWithMinEotSeq:COMMA_FORMATED>)  ),
    // #4
    Kernel_Migration::ChangeInfo( "PHYSICAL AND CONFIGURED VOLUME SIZE",
                                  Kernel_Migration::data,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:physicalAndConfiguredVolumeSize:COMMA_FORMATED>)  ),
    // #5
    Kernel_Migration::ChangeInfo( "MULTIPLE ACTIONS",
                                  Kernel_Migration::log,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:multipleActions:COMMA_FORMATED>)  ),
    // #6
    Kernel_Migration::ChangeInfo("NEW FILE DIRECTORY LAYOUT",
                                  Kernel_Migration::filedir,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:newFileDirectoryLayout:COMMA_FORMATED>)  ),
    // #7
    Kernel_Migration::ChangeInfo("FLOATING RESTART PAGE",
                                  Kernel_Migration::data,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:floatingRestartPage:COMMA_FORMATED>)  ),
    // #8
    Kernel_Migration::ChangeInfo("LOB SIZE INFORMATION IN FILE DIRECTORY",
                                 Kernel_Migration::filedir,
                                 ! LIVECACHE_ONLY,
                                 Kernel_Version(<PERSINFO:lobSizeInfoInFileDirectory:COMMA_FORMATED>)),
    // #9
    Kernel_Migration::ChangeInfo( "RecreateIndexForOmsKeyedObjects",
                                  Kernel_Migration::data,
                                  LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:RecreateIndexForOmsKeyedObjects:COMMA_FORMATED>)  ),
    // #10
    Kernel_Migration::ChangeInfo( "FILE ATTRIBUTES IN FILE DIRECTORY",
                                  Kernel_Migration::filedir,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:AttributesInFileDirectory:COMMA_FORMATED>)  ),
    // #11
    Kernel_Migration::ChangeInfo( "COMPRESSED SUBTREE LEAVES",
                                  Kernel_Migration::data,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:CompressedSubTreeLeaves:COMMA_FORMATED>)  ),
    // #12
    Kernel_Migration::ChangeInfo( "OPEN TRANS FILE WITH QUEUE ID",
                                  Kernel_Migration::data,
                                  ! LIVECACHE_ONLY,
                                  Kernel_Version(<PERSINFO:OpenTransFileWithQueueId:COMMA_FORMATED>)  ),
    // #############################################################################################
	// if you need more entries you have to update #define MAX_CHANGE_INFO 14 in Kernel_Migration.hpp
    // #############################################################################################
    // #13 (INVALID_CHANGE_INFO = MAX_CHANGE_INFO -1)
    Kernel_Migration::ChangeInfo()  // invalid value => DO NOT CHANGE THE LAST VALUE
};
/* --------------------------------------------------------------------------- */
#endif
