/*!
  @file           FBM_Exception.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        FBM
  @brief          Error messages

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



#ifndef FBM_EXCEPTION_HPP
#define FBM_EXCEPTION_HPP


#include "FreeBlockManagement/FBM_Messages.hpp"
#include "FreeBlockManagement/FBM_Types.hpp"

/*!
   @brief          defines common exception class of this component

   - class FBM_Exception
 */
SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( FBM );

/*! enddeclaration */


/*!
  \interface      FBM_IllegalTransition
  \brief          FBM exception for illegal state transistions
 */

class FBM_IllegalTransition : public FBM_Exception
{
public:
    /*!
       @brief          Constructor of an error message displaying a wrong transition between block states
       @param          filename [in] filename (to be generated)
       @param          linenumber [in] linenumber (to be generated)
       @param          DevNo [in] device number of the block address
       @param          BlockNo [in] block number of the block address
       @param          OldBlockState [in] current state of the block
       @param          OldBackUpBlockState [in] current back up state
       @param          NewBlockState [in] wanted state of the block
       @param          NewBackUpBlockState [in] wanted back up state
       @return         none

       - Constructor of an error message. The error occurs when the FBM is ask to
         change the state of a block in a way which is not permitted
     */
    FBM_IllegalTransition
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const SAPDB_UInt               DevNo,
     const SAPDB_UInt               BlockNo,
     const FBM_BlockState           OldBlockState,
     const FBM_BlockState           OldBackUpBlockState,
     const FBM_BlockState           NewBlockState,
     const FBM_BlockState           NewBackUpBlockState
    )
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ILLEGAL_TRANSITION,
     SAPDB_ToString(DevNo, _T_d),
     SAPDB_ToString(BlockNo, _T_d),
     SAPDB_ToString( OldBlockState ),
     SAPDB_ToString( OldBackUpBlockState ),
     SAPDB_ToString( NewBlockState ),
     SAPDB_ToString( NewBackUpBlockState ))
    {}
};


/*!
  \interface      FBM_IllegalRestoreAfterCompression

  \brief          illegal restore after compression

 */

class FBM_IllegalRestoreAfterCompression : public FBM_Exception
{
public:
    /*!
       @brief          Constructor of an error message displaying a wrong transition between block states
       @param          filename [in] filename (to be generated)
       @param          linenumber [in] linenumber (to be generated)
       @param          DevNo [in] device number of the block address
       @param          BlockNo [in] block number of the block address
       @param          BlockState [in] current state of the block
       @param          BackUpBlockState [in] current back up state
       @return         none


       - Constructor of an error message. The error  occurs when the FBM is ask to
         restore the original state of a block marked for compression, but the block
         is not at all marked for compression
     */
    FBM_IllegalRestoreAfterCompression
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const SAPDB_UInt               DevNo,
     const SAPDB_UInt               BlockNo,
     const FBM_BlockState           BlockState,
     const FBM_BlockState           BackUpBlockState
    )
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ILLEGAL_RESTORE_AFTER_COMPRESSION,
     SAPDB_ToString( DevNo, _T_d ),
     SAPDB_ToString( BlockNo, _T_d ),
     SAPDB_ToString( BlockState ),
     SAPDB_ToString( BackUpBlockState ))
    {}
};



/*!
  \brief  FBM_IllegalTransitionToFreeAfterSVP

 */

class FBM_IllegalTransitionToFreeAfterSVP : public FBM_Exception
{
public:
    /*!
       @brief          Constructor of an error message displaying a wrong transition between block states
       @param          filename [in] filename (to be generated)
       @param          linenumber [in] linenumber (to be generated)
       @param          DevNo [in] device number of the block address
       @param          BlockNo [in] block number of the block address
       @param          BlockState [in] current state of the block
       @param          BackUpBlockState [in] current back up state
       @return         none

       - Constructor of an error message. The error  occurs when the FBM is ask to
         change the state of a block to 'free after savepoint' but the current state
         does not allow this transition
     */
    FBM_IllegalTransitionToFreeAfterSVP
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const SAPDB_UInt               DevNo,
     const SAPDB_UInt               BlockNo,
     const FBM_BlockState           BlockState,
     const FBM_BlockState           BackUpBlockState
    )
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ILLEGAL_TRANS_TO_FREE_AFTER_SVP,
     SAPDB_ToString( DevNo, _T_d ),
     SAPDB_ToString( BlockNo, _T_d ),
     SAPDB_ToString( BlockState ),
     SAPDB_ToString( BackUpBlockState ))
    {}
};


/*!
  \interface      FBM_IllegalRestoreAfterBackup
  \brief          illegal restore after backup

 */
class FBM_IllegalRestoreAfterBackup : public FBM_Exception
{
public:
    /*!
       @brief          Constructor of an error message displaying a wrong transition between block states
       @param          filename [in] filename (to be generated)
       @param          linenumber [in] linenumber (to be generated)
       @param          DevNo [in] device number of the block address
       @param          BlockNo [in] block number of the block address
       @param          BlockState [in] current state of the block
       @param          BackUpBlockState [in] current back up state
       @return         none


       - Constructor of an error message. The error occurs when the FBM is ask to
         restore the original state of a block marked for backup, but the block
         is not at all marked for backup
     */
    FBM_IllegalRestoreAfterBackup
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const SAPDB_UInt               DevNo,
     const SAPDB_UInt               BlockNo,
     const FBM_BlockState           BlockState,
     const FBM_BlockState           BackUpBlockState
    )
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ILLEGAL_RESTORE_AFTER_BACKUP,
     SAPDB_ToString( DevNo, _T_d ),
     SAPDB_ToString( BlockNo, _T_d ),
     SAPDB_ToString( BlockState ),
     SAPDB_ToString( BackUpBlockState ))
    {}
};


/*!
  \brief          FBM_NoMoreMemoryForNewDevice

 */

class FBM_NoMoreMemoryForNewDevice : public FBM_Exception
{
public:
    /*!
       @brief          Constructor of an error message needed if there is no more meory to register a new device
       @param          filename [in] filename (to be generated)
       @param          linenumber [in] linenumber (to be generated)
       @param          DevNo [in] number of the device which could not be registered
       @param          DevSize [in] size of the device  which could not be registered
       @return         none

     */

    FBM_NoMoreMemoryForNewDevice
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const SAPDB_UInt               DevNo,
     const SAPDB_UInt               DevSize)
    :
    FBM_Exception
    (filename, linenumber,
     FBM_NO_MORE_MEMORY_FOR_NEW_DEVICE,
     SAPDB_ToString(DevNo, _T_d),
     SAPDB_ToString(DevSize, _T_d))
    {}
};

/*!
  \brief   FBM_VolumeNoOutOfRange

 */

class FBM_VolumeNoOutOfRange : public FBM_Exception
{
public:
    /*!
       @brief   Constructor of an error message needed if the number of a volume is not in the correct range
       @param   filename   [in] filename (to be generated)
       @param   linenumber [in] linenumber (to be generated)
       @param   volNo      [in] wrong volume number
       @param   minVolNo   [in] minVolNo and maxVolNo define the range a valid devno can be in
       @param   maxVolNo   [in] minVolNo &lt;= devNo &lt; maxVolNo
       @return  none

     */

    FBM_VolumeNoOutOfRange
    (const SAPDB_Char*  filename,
     const SAPDB_UInt4  linenumber,
     const SAPDB_UInt   volNo,
     const SAPDB_UInt   minVolNo,
     const SAPDB_UInt   maxVolNo)
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ERR_VOLNO_OUT_OF_RANGE,
     SAPDB_ToString( volNo, _T_d ),
     SAPDB_ToString( minVolNo, _T_d ),
     SAPDB_ToString( maxVolNo, _T_d ))
    {}
};

/// FBM_VolumeNotRegistered 
class FBM_VolumeNotRegistered : public FBM_Exception
{
public:
    /*!
       @brief   Constructor of an error message needed if the number of a volume 
                belongs to a not registered data volume
       @param   filename   [in] filename (to be generated)
       @param   linenumber [in] linenumber (to be generated)
       @param   volNo      [in] wrong volume number
       @return  none

     */

    FBM_VolumeNotRegistered
    (const SAPDB_Char*  filename,
     const SAPDB_UInt4  linenumber,
     const SAPDB_UInt   volNo)
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ERR_VOLNO_NOT_REGISTERED,
     SAPDB_ToString( volNo, _T_d ))
    {}
};

/*!
  \brief      FBM_VolumeTooSmall 

 */
class FBM_VolumeTooSmall : public FBM_Exception
{
public:
    /*!
       @brief   Constructor of an error message needed if the blockNo of a given
                volume is greater than the capacity of the volume.
       @param   filename   [in] filename (to be generated)
       @param   linenumber [in] linenumber (to be generated)
       @param   volNo      [in] volume number
       @param   blockNo    [in] wrong block number
       @param   capacity   [in] volume capacity
       @return  none

     */

    FBM_VolumeTooSmall
    (const SAPDB_Char*  filename,
     const SAPDB_UInt4  linenumber,
     const SAPDB_UInt   volNo,
     const SAPDB_UInt   blockNo,
     const SAPDB_UInt   capacity )
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ERR_VOLUME_TOO_SMALL,
     SAPDB_ToString( blockNo, _T_d ),
     SAPDB_ToString( volNo, _T_d ),
     SAPDB_ToString( capacity, _T_d ))
    {}
};
/*!
  \brief      FBM_IllegalState

 */
class FBM_IllegalState : public FBM_Exception
{
public:
    /*!
       @brief          Constructor of an error message needed if the number of a new device is not inthe correct range
       @param          filename [in] filename (to be generated)
       @param          linenumber [in] linenumber (to be generated)
       @param          BlockNo [in] number of the blocjk which is in the illegal state
       @param          BlockState [in] current state of the block
       @param          BackUpBlockState [in] current back up state
       @return         none

     */

    FBM_IllegalState
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const SAPDB_UInt               BlockNo,
     const FBM_BlockState           BlockState,
     const FBM_BlockState           BackUpBlockState)
    :
    FBM_Exception
    (filename, linenumber,
     FBM_ILLEGAL_STATE,
     SAPDB_ToString( BlockNo, _T_d ),
     SAPDB_ToString( BlockState ),
     SAPDB_ToString( BackUpBlockState ))
    {}
};

/*!
  \brief      FBM_IllegalState

 */
class FBM_CantReserveClusterSpace : public FBM_Exception
{
public:
	/*!
	   @brief          Constructor of an error message needed if we cant reserve cluster space 
	   @param          filename [in] filename (to be generated)
	   @param          linenumber [in] linenumber (to be generated)
	   @param          numCluster [in] number of clusters to reserve
	   @param          volSize [in] size of the volume in blocks
	   @param          maxUnclusteredBlock [in] highest unclustered block number
	   @return         none

	 */

	FBM_CantReserveClusterSpace
	(const SAPDB_Char*              filename,
	 const SAPDB_UInt4              linenumber,
	 const SAPDB_UInt4              numCluster,
	 const IOMan_BlockNo            volSize,
	 const IOMan_BlockNo            maxUnclusteredBlock)
	:
	FBM_Exception
	(filename, linenumber,
	 FBM_ERR_CANT_RESERVE_CLUSTER,
	 SAPDB_ToString( numCluster, _T_d ),
	 SAPDB_ToString( volSize ),
	 SAPDB_ToString( maxUnclusteredBlock ))
	{}
};

/// block info message
class FBM_BlockInfo : public FBM_Exception
{
public:
    /*!
       @brief          Constructor of an info message
       @param          filename [in] filename (to be generated)
       @param          linenumber [in] linenumber (to be generated)
       @param          BlockNo [in] number of the blocjk which is in the illegal state
       @param          BlockState [in] current state of the block
       @return         none

     */

    FBM_BlockInfo
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const SAPDB_UInt               BlockNo,
     const FBM_BlockState           BlockState)
    :
    FBM_Exception
    (filename, linenumber,
     FBM_INFO_USED_BLOCK,
     SAPDB_ToString( BlockNo, _T_d ),
     SAPDB_ToString( BlockState ))
    {}
};

#endif  /* FBM_EXCEPTION_HPP */
