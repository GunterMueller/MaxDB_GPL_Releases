/*!
  \file           FBM_IDataVolume.hpp
  \author         Henrik
  \ingroup        FBM

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef FBM_IDATAVOLUME_H
#define FBM_IDATAVOLUME_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FreeBlockManagement/FBM_IVolume.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
// forward
class IOMan_ComplexClusterAddress;

/*!
  \class          FBM_IDataVolume
  \brief          representation of one data volume
 */
class FBM_IDataVolume : public FBM_IVolume
{
public:

    /*!
       \brief          returns the number of blocks to add to a volume until it reaches it's desired filling level
       \return         number of blocks
     */

    virtual IOMan_BlockCount GetNumBlocksToAddUntilOptIsReached() const = 0;

    /*!
       \brief          sets the number of blocks to add to a volume until it reaches it's desired filling level
       \param          value [in] number of blocks
     */

    virtual void SetNumBlocksToAddUntilOptIsReached(IOMan_BlockCount value) = 0;

    /*!
       \brief          checks if a backup is running
       \return         state of backup
     */
    virtual SAPDB_Bool IsBackupRunning() const = 0;            

    /*!
       \brief          resets the blockaddress of the next block to check for a backup flag
                       InitActBlockNoForBackup is used during initialization of a volume for backup
     */
    virtual void InitActBlockNoForBackup() = 0;            

    /*!
       \brief          returns the number of blocks marked for backup
       \return         number of blocks marked for backup
     */
    virtual IOMan_BlockCount GetNumBlocksMarkedForBackup() const = 0;

    /*!
       \brief          returns the number of blocks marked as 'free after savepoint'
       \return         number of blocks marked as 'free after savepoint'
     */
    virtual IOMan_BlockCount GetNumBlocksFreeAfterSVP() const = 0;

    /*!
       \brief          returns the number of used blocks
       \return         number of used blocks 
     */
    virtual IOMan_BlockCount GetNumBlocksUsed() const = 0;

    /*!
       \brief          returns the number of blocks on a volume
       \return         number of blocks 
     */
    virtual IOMan_BlockCount GetNumBlocks() const = 0;


    /*!
       \brief          prepares dropping of a volume 

                       a volume in drop mode prevents writing of new pages to this volume
     */
    virtual void  PrepareForDrop() = 0;

    /*!
       \brief          restores normal operation of a volume after an aborted drop volume

                       a volume in drop mode prevents writing of new pages to this volume
     */
    virtual void  RestoreForDrop() = 0;

    /*!
       \brief          checks the 'drop state' of the volume

                       a volume in drop mode prevents writing of new pages to this volume

       \returns        'drop state' of the volume
     */
    virtual SAPDB_Bool  IsToDrop() = 0;

    /*!
       \brief          return the FBM state of a block after the backup has finished
       \param          BlockNo [in] block number 

       \returns        FBM_BlockState
     */
    virtual FBM_BlockState GetBlockStateAfterBackup(IOMan_BlockNo BlockNo) const = 0;

    /*!
       \brief          modifies the FBM state of a block for the time after the backup has finished

       \param          BlockNo [in] block address of the block to modify
       \param          BlockState [in] new block state of the block
     */
    virtual void SetBlockStateAfterBackup(IOMan_BlockNo BlockNo, FBM_BlockState BlockState) = 0;

    /*!
       \brief          sets the FBM state of a block to backup and preserves the current state for later use

       \param          BlockNo [in] block address of the block to backup
     */
    virtual void SetBlockStateToBackUp (IOMan_BlockNo BlockNo) = 0;

    /*!
       \brief          sets the FBM state of a block to 'free after savepoint' 

       \param          BlockNo [in] block address of the block to modify
     */
    virtual void SetBlockStateToFreeAfterSVP (IOMan_BlockNo BlockNo) = 0;

    /*!
       \brief          sets the FBM state of all blocks from 'free after savepoint'  to 'free'

                       .. is called after a savepoint

     */
    virtual void SetAllBlocksMarkedAsFreeAfterSVPToFree () = 0;

    virtual IOMan_BlockCount GetNumberOfClusteredBlocksToBackup() const = 0;

    /*!
        @brief        returns a number of blocks marked for backup
        @param        maxBlocksWanted [in] maximum number of blocks to return
        @param        searchClustered [in] return result from cluster area
        @param        cluster [out] resulting complex cluster
        @return       none
     */
   virtual void GetNextBlocksForBackUp (
        const IOMan_BlockCount          maxBlocksWanted,
        const SAPDB_Bool                searchClustered,
        IOMan_ComplexClusterAddress&    cluster ) = 0;

   /*!
      \brief          Removes the flags indicating that a block must be saved
      \param          BlockNo [in] address of the block to restore
      \param          bRestoredStateIsFreeAfterSVP [out] flag if the restored state of the block is 'free after savepoint'

      - The original state of the block is restored.
    */
    virtual void RestoreBlockStateMarkedForBackup (
        IOMan_BlockNo  BlockNo,
        SAPDB_Bool &bRestoredStateIsFreeAfterSVP) = 0;

   /*!
      \brief          Removes for all blocks the flags indicating that this block must be saved
      \param          NumBlocksRestoredToFreeAfterSVP [out] number of blocks restored to 'free after savepoint'

      - The original state of the blocks is restored.
    */

    virtual void RestoreAllBlockStatesMarkedForBackup (IOMan_BlockCount &NumBlocksRestoredToFreeAfterSVP) = 0;

};

#endif //FBM_IDATAVOLUME_H
