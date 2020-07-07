/*!
  @file           IOMan_IDataInfo.hpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          Interface for data area information
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



#ifndef IOMAN_IDATAINFO_HPP
#define IOMAN_IDATAINFO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

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

/*!
  @interface    IOMan_IDataInfo
  @brief        This interface is used for data area information.
 */


class IOMan_IDataInfo
{
public:

    /*!
       @brief   This method returns the access to the IOMan_IDataInfo interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the IOMan_IDataInfo interface
     */
    static IOMan_IDataInfo& GetInstance();
    /*!
         @brief  This method returns the io blocking factor in pages for read and write access 
                 The value is only available when the IoManager singleton is available
         @return (SAPDB_UInt)
     */
    virtual SAPDB_UInt GetDataIOBlockCount() const = 0;
    
    /*!
        @brief  Returns the io blocking factor too, but this call will read the value from
                the RTE resp. config file. This is not so fast, but it is assured in contrast
                to GetDataIOBlockCount that the value is available.
        @return (SAPDB_UInt)
    */
    static SAPDB_UInt ReadDataIOBlockCount();
    
    /*!
       @brief   This method indicates whether a add data devspace is possible.
       @return  true means that add data devspace is possible
     */
    virtual SAPDB_Bool AddDataVolumePossible() const = 0;

    /*!
       @brief   This method provides the number of configured data devspaces.
       @return  SAPDB_Int
     */
    virtual SAPDB_Int ConfiguredDataVolumes() const = 0;

    /*!
       @brief   This method provides the maximum number of configurable 
                log volumes.
       @return  SAPDB_Int
     */
    virtual SAPDB_Int MaxConfigurableDataVolumes() const = 0;

    /*!
       @brief   This method checks if the volume is listed in the config file
       @return  bool
     */
    virtual SAPDB_Bool IsConfigured(IOMan_DeviceNo) const = 0;

    /*!
       @brief   This method returns the logical identifier of for the given
                data devspace name. If the devspace name doesn't correspond to
                a valid devspace the return value is false.
       @param   devName [in] Name of a data devspace
       @param   devNo   [out] Logical devspace identifier
       @return  true if devspace name was found
     */
    virtual bool GetDataVolumeNo(
        tsp00_VFilename &devName,
        IOMan_DeviceNo  &devNo ) const = 0;
    /*!
       @brief   This method returns the name of the data devspace identified
                by the given logical identifier.
       @param   devNo   [in ] Logical devspace identifier
       @param   devName [out] Name of a data devspace
       @return  true if devspace name was found
     */
    virtual bool GetDataVolumeName(
        IOMan_DeviceNo    devNo,
        tsp00_VFilename   &devName ) const = 0;

    /*!
       @brief   This method provides the configured size in pages of the 
                specified data devspace. It is assumed that the devspace 
                identifier is a valid.
       @param   devNo [in] Logical devspace identifier
       @return  IOMan_BlockCount
     */
    virtual IOMan_BlockCount GetDataVolumeSize( const IOMan_DeviceNo devNo ) const = 0;

    /*!
       @brief   This method provides the usable size in pages of the 
                specified data devspace. It is assumed that the devspace 
                identifier is valid.
       @param   devNo [in] Logical devspace identifier
       @return  IOMan_BlockCount
     */
    virtual IOMan_BlockCount GetDataVolumeUsableSize( const IOMan_DeviceNo devNo ) const = 0;

    /*!
       @brief   This method returns the sum of all pages storable within all 
                configured data devspaces. This value doesn't include the pages
                needed for administrativ purpose, i.e IOMan_InfoPage.
                This value is only available if all
                data devspaces are in mode open.
       @return  SAPDB_Int4
     */
    virtual SAPDB_Int4 TotalUsableDataPages() const = 0;

    /*!
       @brief   This method returns the sum of all pages storable within all 
                configured data devspaces. This value includes all pages of
                all data devspaces without taking amount of pages reserved 
                for administrative purposes. This value is only available if 
                all data devspaces are in mode open.
       @return  SAPDB_Int4
     */
    virtual SAPDB_Int4 TotalDataPages() const = 0;

    /*!
       @brief   This method provides the number of active data devspaces. 
                If the I/O management isn't restarted the initial value of 
                one is returned.
       @return  SAPDB_Int
     */
    virtual SAPDB_Int UsedDataVolumes() const = 0;
};


#endif  /* IOMAN_IDATAINFO_HPP */
