/*!
  @file     Converter_MapControl.hpp
  @author   TorstenS
  @author   AlexanderK
  @ingroup  Converter
  @brief    Base class for fundamental functionality of handling
            concurrent access to the converter map, which is used
            to transform pageNo's into block addresses.
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


#ifndef CONVERTER_MAPCONTROL_HPP
#define CONVERTER_MAPCONTROL_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Map.hpp"
#include "Converter/Converter_MapSectionData.hpp"
#include "Container/Container_Vector.hpp"
#include "KernelCommon/Kernel_Common.hpp"

/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Data_PageRecoveryMode;
class Kernel_Dump;
class SAPDBMem_IRawAllocator;
class Converter_MapClusterIterator;
class Converter_MapPeriodIterator;
class Converter_MapSectionIterator;
class RTETask_ITask;

/*===========================================================================*
 *  DEFINES                                                                  *
 **/

/// suspend reason
#define MAX_CONVERTER_SECTIONS  64

/*!
  @class    Converter_MapControl
  @brief    Administration object to handle concurrent access to the converter
            map. This class offers base functionality and it's not possible
            to create an instance of it.
 */
class Converter_MapControl
{
public:

    /// Defines abbreviation for a section data

    typedef Container_Vector<Converter_MapSectionData>  SectionData;

    /// Defines abbreviation for a section data iterator

    typedef SectionData::Iterator                       SectionDataIterator;

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Manager                                                         */
    /* -----------------------------------------------------------------------*/

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

    virtual SAPDB_Bool  Init(
        const RTETask_ITask  &task,
        const SAPDB_UInt     maxDataPnoInMap,
        const RTE_RegionId   &baseRegionId,
        const SAPDB_UInt     numConverterRegions,
        const SAPDB_UInt     clusterSize,
        const SAPDB_UInt     converterPageSize);

    /*!
      @brief   All resources held by members of the map control are returned.
      @param   task [in] own task
      @return  none
    */

    virtual void Shutdown( const RTETask_ITask  &task );

    /*!
    @brief  Expand the map control to be able to handle NewNumAllPages more.
    @param  task            [in] own task
    @param  newNumAllPages  [in] after this command succeded the map control
                                 should manage the minimum number of NewNumAllPages
                                 pages
    @return (Converter_ReturnCode)
            Converter_Ok                     : Expand finished successfully
            Converter_MemoryAllocationFailed : Expand failed due to a lack of memory
    */

    Converter_ReturnCode Expand(
        const RTETask_ITask &task,
        const SAPDB_UInt    newNumAllPages );

    /*!
       @brief   Reserves a new page number.
       @param   task   [in]  own task
       @param   pageNo [out] requested new page number
       @return  (SAPDB_Bool) returns true if  a page could be returned if the data base is
                             already full and no page could be supplied false is returned
     */

    SAPDB_Bool  RequestNewPageNo(
        const RTETask_ITask &task,
        Data_PageNo         &pageNo );


    /*!
      @brief   Returns the maximum possible page number. Note that
               it's not said that this page no is in use.
      @return  (Data_PageNo) maximum possible dynamic page number
    */

    Data_PageNo MaxPageNo() const{
        return( m_Map.GetHighestPossiblePageNo());
    }

    /*!
      @brief   Returns the recovery mode of the data pages
            managed by this control strucure.
      @return  (Data_PageRecoveryMode&)
    */

    const Data_PageRecoveryMode& GetRecoveryMode() const{
        return( m_RecoveryMode );
    }

    /*!
      @brief   returns cluster iterator for a section in a period of a converter map
      @param   section[in] section number
      @param   period[in] period number
      @param   volNo[in] volume number of pages return by iterator
      @return  Converter_MapClusterIterator 
    */
    Converter_MapClusterIterator ClusterBegin(const SAPDB_UInt section, const SAPDB_UInt period, const IOMan_DeviceNo volNo);

    /*!
      @brief   returns end of cluster iterator for a section in a period of a converter map
      @param   section[in] section number
      @param   period[in] period number
      @param   volNo[in] volume number of pages return by iterator
      @return  Converter_MapClusterIterator 
    */
    Converter_MapClusterIterator ClusterEnd(const SAPDB_UInt section, const SAPDB_UInt period, const IOMan_DeviceNo volNo);

    /*!
      @brief   returns period iterator for a converter map
      @param   section[in] section number
      @param   volNo[in] volume number of pages return by iterator
      @return  Converter_MapPeriodIterator 
    */
    Converter_MapPeriodIterator PeriodBegin(const SAPDB_UInt section, const IOMan_DeviceNo volNo);

    /*!
      @brief   returns end of period iterator for a converter map
      @param   section[in] section number
      @param   volNo[in] volume number of pages return by iterator
      @return  Converter_MapPeriodIterator 
    */
    Converter_MapPeriodIterator PeriodEnd(const SAPDB_UInt section, const IOMan_DeviceNo volNo);

    /*!
      @brief   returns section iterator for a converter map
      @param   volNo[in] volume number of pages return by iterator
      @param   task[in] calling task 
      @return  Converter_MapSectionIterator 
    */
    Converter_MapSectionIterator SectionBegin(const IOMan_DeviceNo volNo, const RTETask_ITask &task);

    /*!
      @brief   returns end of section iterator for a converter map
      @param   volNo[in] volume number of pages return by iterator
      @param   task[in] calling task 
      @return  Converter_MapSectionIterator 
    */
    Converter_MapSectionIterator SectionEnd(const IOMan_DeviceNo volNo, const RTETask_ITask &task);


    /*!
      @brief   returns a locked section of the converter
      @param   task[in] calling task 
      @param   sectopm[in] section number 
    */
    Converter_MapSection GetSection(const RTETask_ITask &task,
                                    const SAPDB_UInt sectionNo)
    {
        return Converter_MapSection( task, m_SectionData[ sectionNo ] );
    }


    /* -----------------------------------------------------------------------*/
    /*! @name Dump                                                            */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Dumps MapControl and the Map into the kernel dump file
       @param   task         [in] own task
       @param   dump     [in|out] kernel dump file
       @param   bEnterRegion [in] use converter regions
       @return  none
     */

    virtual void Dump(
        const RTETask_ITask &task,
        Kernel_Dump         &dump,
        const SAPDB_Bool    bEnterRegion );

    SAPDB_Bool operator==(const Converter_MapControl &comp)
    {
        return (m_RecoveryMode == comp.m_RecoveryMode);
    }

protected:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction                                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   allocator [in] allocator for memory resources
       @param   recMode   [in] recovery mode of the map control
     */

    Converter_MapControl(
        SAPDBMem_IRawAllocator        &allocator,
        const Data_PageRecoveryMode   &recMode )
    :
    m_NextSectionToSearchForNewPageNo( 0 ),
    m_RecoveryMode( recMode ),
    m_Map( allocator ),
    m_SectionData( allocator )
    {}

    /* -----------------------------------------------------------------------*/
    /*! @name  Miscellaneous                                                  */
    /* -----------------------------------------------------------------------*/

    /// Determine section id for given page no
    SAPDB_UInt  CalculateSectionNo( const Data_PageNo pageNo ) const{
        return( m_Map.CalcSectionNo( pageNo ));
    }

    /// Return sectionId of the section pointed by the given iterator.
    /// It's assumed that the iterator is valid.

    SAPDB_UInt  GetSectionNo( const SectionDataIterator &iter ) const{
        return( m_SectionData.GetIndex( iter ));
    }

    /// Number of used sections, i.e. number of configured converter latches.
    SAPDB_UInt GetSectionCount() const{
        return( m_SectionData.GetSize());
    }

    /// Check if the given pageNo is addressable by the current map
    bool IsPageNoOutOfRange( const Data_PageNo pageNo ) const{
        return(( pageNo > this->MaxPageNo()) || ( pageNo < 0 ));
    }

protected:

    /// Defines the sections number which is used next to get a free page no.

    SAPDB_UInt  m_NextSectionToSearchForNewPageNo;

    /// Defines the recovery mode of this map control. Generally needed
    /// for trace and diagnostic output.

    Data_PageRecoveryMode   m_RecoveryMode;

    //// handle for the converter map which contains the converter leaf pages

    Converter_Map   m_Map;

    /// list of section item containing the section specific informations

    SectionData m_SectionData;

};

#endif  /* CONVERTER_MAPCONTROL_HPP */
