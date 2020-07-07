/*!
  @file     Converter_TempMapControl.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Manages the concurrent access to the structure mapping
            temporary pageNo's to their physical block address
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


#ifndef CONVERTER_TEMP_MAPCONTROL_HPP
#define CONVERTER_TEMP_MAPCONTROL_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_MapControl.hpp"

/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Converter_MapSection;
class Converter_Version;
class Data_PageRecoveryMode;
class IOMan_BlockAddress;
class SAPDBMem_IRawAllocator;
class RTETask_ITask;


/*===========================================================================*
 *  DEFINES                                                                  *
 **/

/*!
  @class    Converter_TempMapControl
  @brief    Administration object to handle concurrent access to the converter
            map, which deals with temporary pages.
 */

class Converter_TempMapControl : public Converter_MapControl
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction                                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   allocator [in] allocator for memory resources
       @param   recMode   [in] recovery mode of the map control
     */

    Converter_TempMapControl(
        SAPDBMem_IRawAllocator  &allocator,
        const Data_PageRecoveryMode   &recMode )
    :
    Converter_MapControl( allocator, recMode )
    {}

    /*!
       @brief   Sets all members of the map control according to the input parameters
       @param   task                [in] own task
       @param   maxDataPnoInMap     [in] maximum page number to manage by the converter
       @param   baseRegionId        [in] start region id of for the first section
       @param   numConverterRegions [in] the converter is devided into  numConverterRegions
                                         'little' converter each protected by an own region
                                         with BaseRegionId &lt;=RegionId&lt;BaseRegionId+
                                         NumConverterRegions. The more regions are supplied
                                         the better parallel accesses can be handled
       @param   clusterSize         [in] number of subsequent map entries building a lock set
       @param   converterPageSize   [in] total size of a converter page in byte
       @return  (SAPDB_Bool) returns true if the initilization was successful
     */

    virtual SAPDB_Bool  Initialize(
        const RTETask_ITask  &task,
        const SAPDB_UInt     maxDataPnoInMap,
        const RTE_RegionId   &baseRegionId,
        const SAPDB_UInt     numConverterRegions,
        const SAPDB_UInt     clusterSize,
        const SAPDB_UInt     converterPageSize )
    {
        return( Converter_MapControl::Init( task, maxDataPnoInMap, baseRegionId,
                                            numConverterRegions, clusterSize, converterPageSize ));

    }

    /* -----------------------------------------------------------------------*/
    /*! @name PageNo mapping for temporary pages to block addresses           */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief   Get the current block address of of a page. If no block address
               is available, an invalid block address will be returned.
      @param   task   [in]  own task
      @param   pageNo [in] given page no the block address is requested for
      @param   block  [out] requested block address
      @return  (Converter_ReturnCode)
                Converter_Ok,
                Converter_NoConverterEntry,
                Converter_PageNoOutOfRange,
                Converter_InvalidBlockAddressFound
    */

    Converter_ReturnCode GetBlockAddress(
        const RTETask_ITask &task,
        const Data_PageNo   pageNo,
        IOMan_BlockAddress  &block );

    /*!
      @brief   Get an i/o adress of an empty block to write a temp page
               to a data volume.
      @param   task           [in] own task
      @param   pageNo         [in] wanted page
      @param   bReqSequential [in] request access mode of volume
      @param   newBlock      [out] new block address
      @return  (Converter_ReturnCode)
    */

    Converter_ReturnCode GetNewPositionForWrite(
        const RTETask_ITask  &task,
        const Data_PageNo    pageNo,
        const SAPDB_Bool     bReqSequential,
        IOMan_BlockAddress   &newBlock );

    /*!
      @brief   Updates the block address of a temporary page. The old block
               address is set to Free.
      @param   task                 [in] own task
      @param   pageConverterVersion [in] version of the converter when the
                                         page was written the last time
      @param   pageNo [in] page number for which the I/o-address hast to be updated
      @param   block [in] new block address which is to insert
      @return  none
    */

    void SetPositionForWriting (
        const RTETask_ITask         &task,
        const Converter_Version     &pageConverterVersion,
        const Data_PageNo           pageNo,
        const IOMan_BlockAddress    &block );

    /*!
      @brief   Releases a temporary page number. Frees the converter slot and updates
               the appropriate free chains and counters. If the converter slot is
               already empty an emergency shutdown is executed.
      @param   task   [in] own task
      @param   pageNo [in] page number to release
      @return  none
    */

    void FreePageNo(
        const RTETask_ITask &task,
        const Data_PageNo   pageNo );
};

#endif //CONVERTER_TEMP_MAPCONTROL_HPP
