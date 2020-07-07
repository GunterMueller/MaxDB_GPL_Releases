/*!
  @file           RTEIO_VolumeInfo.hpp
  @author         JoergM
  @ingroup        RunTime
  @brief          class collecting volume information needed for I/O operations

  The RTEIO_VolumeInfo instances provides the configuration informated used in formatting volumes and 
  task specific synchronous I/O operations. They also store the internal used device identification used
  by the runtime to access volume statistic, serialize access etc.

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
#ifndef RTEIO_VOLUMEINFO_HPP
#define RTEIO_VOLUMEINFO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/InputOutput/RTEIO_Info.hpp"
#include "Container/Container_Vector.hpp"
class SAPDBErr_MessageList;

#include "vsp002.h" // PASCAL: tsp2_dev_type_Param

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;

/*!
  @class RTEIO_VolumeInfo
  @brief This class collects the volume specific information 

  This class is introduced to allow the collection of volume information used during 
  formatting of volumes.

  Its current primary purpose is to provide a C++ accessability of a new parallel 
  formatting code (see RTEIO_FormatVolumeVector below).

  THIS CLASS IS SUBJECT TO IMMEDIATE CHANGE WITH NEW IO CONCEPTS OF RUN TIME ENVIRONMENT
  */
class RTEIO_VolumeInfo : public RTEIO_Info
{
public:
    /*!
      While volume is detached, the volume identifier is set to RTEIO_DetachedId
     */
    const static SAPDB_Int4 RTEIO_DetachedId;

    /*!
    @brief ctor of volume info

    If the 'capacityInPages' is not known, is is possible to set it to zero. After formatting the volume this information
    is filled.

    @param logicalVolNo        [in]    type specific index of volume (First == 1)
    @param type                [in]    Type identifier 
    @param capacityInPages     [in]    If != 0 size of volume in pages, otherwise capacity is retrieved (volume must exist)
    @param formattingPageImage [in]    If given, Formatting of volume is enforced using this image on each page. The image
                                       must be I/O suitable aligned.
   */
    RTEIO_VolumeInfo( SAPDB_Int4           logicalVolNo,
                      tsp2_dev_type_Param  type,
                      SAPDB_UInt4          capacityInPages=0,
                      const SAPDB_Byte   * formattingPageImage=(const SAPDB_Byte *)0 )
     : m_LogicalVolNo(logicalVolNo)
     , m_Type(type)
     , m_CapacityInPages(capacityInPages)
     , m_FormattingPageImage(formattingPageImage)
     , m_AccessMode(RTE_VolumeAccessModeUnknown)
     , m_FileSystemAccessType(RTE_FileSystemAccessTypeUnknown)
     , m_AssignedVolumeId(RTEIO_DetachedId)
        {}

  /*!
    @brief Default ctor of volume info

    Use Initialize afterwards
   */
    RTEIO_VolumeInfo()
     :  m_LogicalVolNo(0),
        m_Type((tsp2_dev_type_Param)0),
        m_CapacityInPages(0),
        m_FormattingPageImage((const SAPDB_Byte *)0)
        {}

  /*!
    @brief Initialize values for combination with default constructor

    @param logicalVolNo        [in]    type specific index of volume (First == 1)
    @param type                [in]    Type identifier 
    @param capacityInPages     [in]    If != 0 size of volume in pages, otherwise capacity is retrieved (volume must exist)
    @param formattingPageImage [in]    If given, Formatting of volume is enforced using this image on each page. The image
                                       must be I/O suitable aligned.
   */
  void Initialize( SAPDB_Int4           logicalVolNo,
                   tsp2_dev_type_Param  type,
                   SAPDB_UInt4          capacityInPages=0,
                   const SAPDB_Byte   * formattingPageImage=(const SAPDB_Byte *)0 )
  {
      m_LogicalVolNo        = logicalVolNo;
      m_Type                = type;
      m_CapacityInPages     = capacityInPages;
      m_FormattingPageImage = formattingPageImage;
  }

  /*!
  
    @brief get the logical volume number

    @return the preconfigured type specific volume index ( First == 1 )
   */
  SAPDB_Int4 GetLogicalVolNo() const { return m_LogicalVolNo; }

  /*!
    @brief get the device type

    @return The preconfigured volume type
   */
  tsp2_dev_type_Param GetType() const { return m_Type; }

  /*!
    @brief Get the attached volume number

    The value attached volume number is depending on the sequence in which the volumes have been attached.
    This call allows to retrieve that device number using the logical device number and the type.
    
    @param attachedDevNo [out] on success the attached devno is returned
    @returns true if volume is attached and returned number is valid, false if volume is not attached
   */
  SAPDB_Bool GetAttachedDevNo(SAPDB_Int4 &attachedDevNo);

  /*!
    @brief get the volume access mode
    @return The preconfigured volume access mode
   */
  RTE_VolumeAccessMode GetAccessMode();

  /*!
    @brief get the file system access type
    @return The preconfigured file system access type
   */
  RTE_FileSystemAccessType GetFileSystemAccessType();

  /*!
    @brief get the capacity in pages
    @return the preconfigured or set volume capacity in pages
   */
  SAPDB_UInt4 GetCapacityInPages() const { return m_CapacityInPages; }

  /*!
    @brief set the capacity in pages
    @param newCapacityInPages [in] the capacity in pages if this information is available
   */
  void SetCapacityInPages(SAPDB_UInt4 newCapacityInPages) { m_CapacityInPages = newCapacityInPages; }

  /*!
      @brief get current physical size of volume in bytes

      To be used to compare found size with expected size after formatting...
      Long time ago there was an off by 1 error in formatting code...

      @param sizeInBytes [out] current size of volume in bytes
      @param sizeInPages [out] current size of volume in pages
      @return true if size is returned, false if size unknown (i.e. raw device...)
   */
  SAPDB_Bool GetPhysicalCapacity(SAPDB_UInt8 &sizeInBytes, SAPDB_UInt4 &sizeInPages) const;

  /*!
    @brief get the pointer to the formatting page image
    @return the preconfigured image used during formatting volume or NULL if formatting not enforced
   */
  const SAPDB_Byte * GetFormattingPageImage() const { return m_FormattingPageImage; }

private:
  /*!
  
    @brief get the assigned physical volume number

    The physical volume number is assigned during RTETask_ITask::Attach() operation and reset to RTEIO_DetachedId
    in RTETask_ITask::Detach() operation,

    @return the assigned physical volume number ( First == 1 , RTEIO_DetachedId == 0 )
   */
  SAPDB_Int4 GetAssignedVolumeId() const { return m_AssignedVolumeId; }

  /*!
  
    @brief set the physical volume number

    The physical volume number is assigned during RTETask_ITask::Attach() operation and reset to RTEIO_DetachedId
    in RTETask_ITask::Detach() operation,

    @return the assigned physical volume number ( First == 1 , RTEIO_DetachedId == 0 )
   */
  void SetAssignedVolumeId(SAPDB_Int4 assignedVolumeId) { m_AssignedVolumeId = assignedVolumeId; }

  friend struct RTETask_Task;

private:
    SAPDB_Int4           m_LogicalVolNo;     //!< the type specific volume index
    tsp2_dev_type_Param  m_Type;             //!< the volume type
    RTE_VolumeAccessMode m_AccessMode;       //!< the volume access mode
    SAPDB_UInt4          m_CapacityInPages;  //!< the capacity in pages
    const SAPDB_Byte   * m_FormattingPageImage; //!< the formatting page pointer or NULL if formatting not enforced
    RTE_FileSystemAccessType m_FileSystemAccessType; //!< the volume file system access type
    SAPDB_Int4           m_AssignedVolumeId; //!< the assigned physical volume id (former devno)
}; /*! @endclass RTEIO_VolumeInfo */

/*!---------------------------------------------------------------------------
  @class RTEIO_LogVolumeInfo
  @brief This class collects the log volume specific information 

  This class is introduced to allow the collection of volume information used during 
  formatting of volumes.
  */
class RTEIO_LogVolumeInfo : public RTEIO_VolumeInfo
{
public:
  /*!
    @brief ctor of volume info

    If the 'capacityInPages' is not known, is is possible to set it to zero. After formatting the volume this information
    is filled.

    @param logicalVolNo        [in]    type specific index of volume (First == 1)
    @param capacityInPages     [in]    If != 0 size of volume in pages, otherwise capacity is retrieved (volume must exist)
    @param formattingPageImage [in]    If given, Formatting of volume is enforced using this image on each page. The image
                                       must be I/O suitable aligned.
   */
    RTEIO_LogVolumeInfo( SAPDB_Int4           logicalVolNo,
                         SAPDB_UInt4          capacityInPages,
                         const SAPDB_Byte   * formattingPageImage=(const SAPDB_Byte *)0 )
     :  RTEIO_VolumeInfo(logicalVolNo, sp2dt_log, capacityInPages, formattingPageImage)
        {}
};

/*!
  @class RTEIO_MirrorLogVolumeInfo
  @brief This class collects the log volume specific information 

  This class is introduced to allow the collection of volume information used during 
  formatting of volumes.
  */
class RTEIO_MirrorLogVolumeInfo : public RTEIO_VolumeInfo
{
public:
  /*!
    @brief ctor of volume info

    If the 'capacityInPages' is not known, is is possible to set it to zero. After formatting the volume this information
    is filled.

    @param logicalVolNo        [in]    type specific index of volume (First == 1)
    @param capacityInPages     [in]    If != 0 size of volume in pages, otherwise capacity is retrieved (volume must exist)
    @param formattingPageImage [in]    If given, Formatting of volume is enforced using this image on each page. The image
                                       must be I/O suitable aligned.
   */
    RTEIO_MirrorLogVolumeInfo( SAPDB_Int4           logicalVolNo,
                               SAPDB_UInt4          capacityInPages,
                               const SAPDB_Byte   * formattingPageImage=(const SAPDB_Byte *)0 )
     :  RTEIO_VolumeInfo(logicalVolNo, sp2dt_mirr_log, capacityInPages, formattingPageImage)
        {}
};

/*!
  @class RTEIO_DataVolumeInfo
  @brief This class collects the log volume specific information 

  This class is introduced to allow the collection of volume information used during 
  formatting of volumes.
  */
class RTEIO_DataVolumeInfo : public RTEIO_VolumeInfo
{
public:
  /*!
    @brief ctor of volume info

    If the 'capacityInPages' is not known, is is possible to set it to zero. After formatting the volume this information
    is filled. Data volume currently are not formatting with a special formatting image.

    @param logicalVolNo        [in]    type specific index of volume (First == 1)
    @param capacityInPages     [in]    If != 0 size of volume in pages, otherwise capacity is retrieved (volume must exist)
   */
    RTEIO_DataVolumeInfo( SAPDB_Int4  logicalVolNo,
                          SAPDB_UInt4 capacityInPages )
    : RTEIO_VolumeInfo(logicalVolNo, sp2dt_data, capacityInPages, (const SAPDB_Byte *)0)
    {}
};

/*!
  @brief A Container_Vector of RTEIO_VolumeInfo references

  This type definition defines the vector of VolumeInfo needed to format volumes in parallel
 */
typedef Container_Vector<RTEIO_VolumeInfo *> RTEIO_VolumeInfoVector;

/*!
  @brief Format all volumes given in the vector in parallel

  The vector is inspected and for each volume group that is formatted in parallel a worker thread
  is started. After all worker threads finished their work, the call returns. During formatting
  the volume capacity information may be updated.

  @param taskId [in]            the task id of the caller
  @param infoVector [inout]     vector of volumes to format
  @param errList [out]          message list to fill
  @return true on success for all, false on any error (errList is filled)
 */
bool RTEIO_FormatVolumeVector( SAPDB_Int4             taskId,
                               RTEIO_VolumeInfoVector &infoVector,
                               SAPDBErr_MessageList   &errList);

/*!
  @brief Format a single volumes given

  During formatting the volume capacity information may be updated.

  @param taskId [in] the task id of the caller
  @param singlevolume [inout] volume to format
  @param errList [out] message list to fill
  @return true on success for all, false on any error (errList is filled)
 */
bool RTEIO_FormatSingleVolume( SAPDB_Int4           taskId,
                               RTEIO_VolumeInfo     &singlevolume,
                               SAPDBErr_MessageList &errList);

#endif  /* RTEIO_VOLUMEINFO_HPP */
