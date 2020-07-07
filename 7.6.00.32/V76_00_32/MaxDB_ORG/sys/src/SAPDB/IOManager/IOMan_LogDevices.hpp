/*!
  @file     IOMan_LogDevices.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle a collecton of log devices
            in particular single and dual logs.

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



#ifndef IOMAN_LOG_DEVICES_HPP
#define IOMAN_LOG_DEVICES_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "IOManager/IOMan_LogDevice.hpp"
#include "IOManager/IOMan_DualLogDevice.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class IOMan_LogDeviceIterator; // forward declaraion

/* -----------------------------------------------------------------------*/
/*!
  @class    IOMan_LogDevices
  @brief    This class is used to handle a collecton of log devices
            in particular single or dual logs. Note  that a dual log
            consists of original and mirrored log volumes whereby a
            single log contains only of a single log volume.
 */
/* -----------------------------------------------------------------------*/


class IOMan_LogDevices
{
public:

    /// Declare IOMan_LogDeviceIterator to be a friend
    friend class IOMan_LogDeviceIterator;

    /// Abbrivation for Container_Vector< IOMan_ILogDevice* >
    typedef Container_Vector< IOMan_ILogDevice* >   LogDevices;

    /// Abbrivation for LogDevices::Iterator
    typedef LogDevices::Iterator    Iterator;
    
    /// Abbrivation for LogDevices::ConstIterator
    typedef LogDevices::ConstIterator               ConstIterator;

    
    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor. Creates a collection of log devices.
       @param   allocator [in] Allocator
       @param   bIsLogMirrored [in] Is log mode dual configured
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogDevices(
        SAPDBMem_IRawAllocator  &allocator,
        const SAPDB_Bool        bIsLogMirrored )
    :
    m_LogDevices( allocator ),
    m_bIsLogMirrored( bIsLogMirrored )
    {}

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to initialize all log devices.
       @param   taskId [in] Identification of the calling task
       @param   maxLogVolumes [in] Maximum number of addressable log volumes.
       @return  (SAPDB_Bool) in case of an error false is returned else true
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool Initialize(
        const tsp00_TaskId  taskId,
        const SAPDB_Int     maxLogVolumes );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name direct access */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get access to a log device which is 
´               identified by the given number.
       @param   devNo [in] logical identifier of a log device
       @return  IOMan_ILogDevice
     */
    /* -----------------------------------------------------------------------*/

    IOMan_ILogDevice& Get( const IOMan_DeviceNo devNo )
    {
        return( *( m_LogDevices[ devNo - IOMan_LogDevices::FirstDeviceNo() ] ));
    }

    const IOMan_ILogDevice& Get( const IOMan_DeviceNo devNo ) const
    {
        return( *( m_LogDevices[ devNo - IOMan_LogDevices::FirstDeviceNo() ] ));
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get access to a dual log device 
                which is  identified by the given number. This operation
                is available in log mode dual only.
       @param   devNo [in] logical identifier of a dual log device
       @return  IOMan_DualLogDevice
     */
    /* -----------------------------------------------------------------------*/

    IOMan_DualLogDevice& GetDualDevice( const IOMan_DeviceNo devNo )
    {
        SAPDBERR_ASSERT_STATE( m_bIsLogMirrored );

        return( *( reinterpret_cast< IOMan_DualLogDevice* >
                   (m_LogDevices[ devNo - IOMan_LogDevices::FirstDeviceNo() ])));
    }

    const IOMan_DualLogDevice& GetDualDevice( const IOMan_DeviceNo devNo ) const
    {
        SAPDBERR_ASSERT_STATE( m_bIsLogMirrored );

        return( *( reinterpret_cast< IOMan_DualLogDevice* >
                   (m_LogDevices[ devNo - IOMan_LogDevices::FirstDeviceNo() ])));
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Configuration and statistic information */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This static method returns the number of the first logical
                log device.
       @return  (IOMan_DeviceNo)
     */
    /* -----------------------------------------------------------------------*/

    static IOMan_DeviceNo FirstDeviceNo()
    {
        return( 1 ); // IOMAN_FIRST_LOG_VOLUME
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method checks the validity of the given log device number.
       @param   devNo [in] Logical volume identifier
       @return  (SAPDB_Bool) true if valid
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool IsDeviceNoValid( const IOMan_DeviceNo devNo ) const
    {
        if(
            ( IOMan_LogDevices::FirstDeviceNo() > devNo )
            ||
            ( SAPDB_UInt( devNo ) > MaxDevices())
        )
            return( SAPDB_FALSE );
        else
            return( SAPDB_TRUE );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the maximun number of configurable log 
                devices. This value is stored within the data base configuration
                file.
       @return  (SAPDB_Int) Maximun number of configurable log devices.
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_UInt MaxDevices() const
    {
        return( m_LogDevices.GetCapacity() );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Contains a collection of pointers referencing either single
                or dual log devices. Note that one log device consists of at 
                least one log volume. If log mode dual is defined each log 
                device consists of two log volumes (original and mirror).
     */
    /* -----------------------------------------------------------------------*/

    LogDevices  m_LogDevices;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  specifies whether the log is mirrored
     */
    /* -----------------------------------------------------------------------*/

    const SAPDB_Bool  m_bIsLogMirrored;
};

/* -----------------------------------------------------------------------*/
/*!
  @class    IOMan_LogDeviceIterator
  @brief    This class to get access to all log device via an iterator.
 */
/* -----------------------------------------------------------------------*/


class IOMan_LogDeviceIterator
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor
       @param   logDevices [in] Collection of log devices
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogDeviceIterator( IOMan_LogDevices   &logDevices )
    :m_LogDevices( logDevices )
    {}

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Iterator access */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the iterator to the first log
                device. There is no check whether this device is valid.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void Begin()
    {
        m_DevNo = IOMan_LogDevices::FirstDeviceNo();
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to report whether the current device 
                pointed by the iterator is valid.
       @return  (SAPDB_Bool) true means that the current device is not valid and 
                that the end of the log device collection is reached; else false
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool End() const
    {
        return( SAPDB_UInt( m_DevNo ) > m_LogDevices.MaxDevices() );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the iterator to the next log device.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void operator++()
    {
        ++m_DevNo;
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the iterator to the next log device.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogDeviceIterator& operator=( const IOMan_LogDeviceIterator &iter )
    {
        m_DevNo = iter.m_DevNo;

        return( *this );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get access to a log device pointed 
                by the iterator.
       @return  IOMan_ILogDevice&amp;
     */
    /* -----------------------------------------------------------------------*/

    IOMan_ILogDevice& operator*()
    {
        return( m_LogDevices.Get( m_DevNo ));
    }

    const IOMan_ILogDevice& operator*() const
    {
        return( m_LogDevices.Get( m_DevNo ));
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get access to a dual log device 
                pointed by the iterator. This operation is available in 
                log mode dual only.
       @return  IOMan_DualLogDevice&amp;
     */
    /* -----------------------------------------------------------------------*/

    IOMan_DualLogDevice& GetDualDevice()
    {
        return( m_LogDevices.GetDualDevice( m_DevNo ));
    }

    const IOMan_DualLogDevice& GetDualDevice( const IOMan_DeviceNo devNo ) const
    {
        return( m_LogDevices.GetDualDevice( m_DevNo ));
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   The copy operator is not supported to avoid not allowed
                duplicates.
       @param   iter [in] iterator 
       @return  IOMan_LogDeviceIterator
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogDeviceIterator( const IOMan_LogDeviceIterator &iter );

private:

     /* -----------------------------------------------------------------------*/
    /*!
        @brief  Reference of the log device collection this iterator works on.
     */
    /* -----------------------------------------------------------------------*/


    IOMan_LogDevices  &m_LogDevices;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Identifier of the current log device the iterator works on.
     */
    /* -----------------------------------------------------------------------*/

    IOMan_DeviceNo  m_DevNo;
};



#endif  /* IOMAN_LOG_DEVICES_HPP */
