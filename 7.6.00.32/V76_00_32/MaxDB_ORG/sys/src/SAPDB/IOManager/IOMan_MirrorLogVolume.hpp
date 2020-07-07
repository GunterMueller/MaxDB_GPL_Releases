/*!
  @file     IOMan_MirrorLogVolume.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle a mirror log volume.
            Mirrored log volumes are available in log mode dual only.
            Note that both volumes of a log device in log mode dual
            are mirrored log volumes! In contrast to this the class
            IOMan_LogVolume is exclusive available in non dual log modes.
  @see      IOMan_LogVolume, IOMan_SingleLogDevice and IOMan_DualLogDevice

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



#ifndef IOMAN_MIRROR_LOG_VOLUME_HPP
#define IOMAN_MIRROR_LOG_VOLUME_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "IOManager/IOMan_KernelPages.hpp"
#include "IOManager/IOMan_LogVolume.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
   @class   IOMan_MirrorLogVolume
   @brief   This class is used to handle a mirror log volume. A mirror
            log volume is a special kind of a IOMan_LogVolume which
            offers a couple of methods more to realize a log mode DUAL.
 */
/* -----------------------------------------------------------------------*/

class IOMan_MirrorLogVolume : public IOMan_LogVolume
{
public:


    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor. 
                This method is used to set the member data to an initial state, 
                in particular the type to mlog. Additional the constructor of 
                the derived IOMan_LogVolume class is called. Note that the 
                logical devo won't be set.
       @param   bIsSecondaryVolume [in] specifies whether this log volume is
                                       the secondary volume
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_MirrorLogVolume( const SAPDB_Bool bIsSecondaryVolume )
    :
    IOMan_LogVolume( bIsSecondaryVolume )
    {}

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the individual logical log volume
                identifier.
       @param   devNo [in] Logical volume identifier
       @param   frame [in] Frame for the IOManInfoPage of the log volume
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void Initialize(
        const IOMan_DeviceNo            devNo,
        const Data_PageFrame   &frame)
    {
        IOMan_LogVolume::Initialize( devNo, frame );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/
    
    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring a log volume from close mode into
                open mode, presumed it isn't in open mode already. Note that
                no security check is made and the IOMan_InfoPage is NOT read!
       @param   taskId [in] Identification of the calling task.
       @return  (IOMan_ReturnCode) IOMan_Okay means open was successful
     */
    /* -----------------------------------------------------------------------*/

    IOMan_ReturnCode OpenForReIntegration( const tsp00_TaskId taskId );

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to prepare the log volume for the subsequent
                intergration.
       @param   taskId        [in] Identification of the calling task.
       @param   pDBIdentifier [in] Unique data base identifier
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void PrepareForReIntergration( 
        const tsp00_TaskId  taskId,
        const SAPDB_Byte    *pDBIdentifier );

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used remove the bad marker after successful 
                reintegration.
       @param   taskId  [in] Identification of the calling task.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void EnableAfterReIntegration( const tsp00_TaskId  taskId );

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to mark an open volume as bad. If the 
                given block number is valid it is written into the zero page 
                of the volume. Note that the method won't close the volume. 
                Each following open of the volume will cause an error.
       @param   taskId [in] Identification of the calling task.
       @param   blockNo [in] BlockNo to be inserted into the zero page.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void SetMirroredLogToBad(
        const tsp00_TaskId  taskId,
        const IOMan_BlockNo blockNo )
    {
        IOMan_Volume::SetBad( taskId, blockNo );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write the logical device number of 
                the next logical volume into the info page of the
                current volume.
       @param   taskId    [in] Identification of the calling task.
       @param   nextDevNo [in] Logical volume identifier
       @return  (SAPDB_Bool) linkage is created successfully
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool SetSuccessor(
        const tsp00_TaskId      taskId,
        const IOMan_DeviceNo    nextDevNo );

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write the logical device number of 
                the previous logical volume into the info page of the
                current volume.
       @param   taskId    [in] Identification of the calling task.
       @param   prevDevNo [in] Logical volume identifier
       @return  (SAPDB_Bool) linkage is created successfully
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool SetPredecessor(
        const tsp00_TaskId      taskId,
        const IOMan_DeviceNo    prevDevNo );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Common information */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get the physical identifier of the
                volume. This is needed by the RTE Interface to identify a 
                volume.
       @return  physical volume id
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Int4 GetPhysicalDevNoOfMirroredLog() const
    {
        return( IOMan_Volume::GetPhysicalDevNo() );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to determine whether a given block number
                is addressable via the volume. i.e. the block number is in
                the valid number range of the volume.
       @param   blockNo [in] BlockNo to be examined
       @return  true means is in volume scope; else false
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool IsBlockNoValid( const IOMan_BlockNo blockNo ) const
    {
        return(  IOMan_Volume::IsBlockNoMapable( blockNo ));
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    // Assignment operator is not supported

    IOMan_MirrorLogVolume& operator=( const IOMan_MirrorLogVolume &volume );

    // copy constructor is needed for newarray operation during initialization
    //IOMan_MirrorLogVolume( const IOMan_MirrorLogVolume &volume ); 
    
};


#endif  /* IOMAN_MIRROR_LOG_VOLUME_HPP */
