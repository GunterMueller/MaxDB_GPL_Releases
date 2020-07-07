/*!
  @file           Converter_IManager.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        Converter
  @brief          abstract interface class to the converter
                  containing general functions to manage the converter
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



#ifndef CONVERTER_IMANAGER_HPP
#define CONVERTER_IMANAGER_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "DataAccess/Data_Types.hpp"

class Converter_Version;
class IOMan_PackedBlockAddress;
class Kernel_DateTime;
class RTETask_ITask;

/*!
  @interface      Converter_IManager
  @brief          abstract interface class to the converter
                  containing general functions to manage the converter
 */
class Converter_IManager
{
public:

    /*!
       @brief   returns the manager interface of the converter
       @return  (Converter_IManager) instance of the converter

       - This static function returns the interface of the converter which
         allows access to all methods defined in this interface class
     */
    static  Converter_IManager & Instance();

    /*!
       @brief   Creates converter for restarting an existing database
       @param   task                        [in] own task
       @param   ConverterVersion            [in] converter version from restart record
       @param   packedRootBlockAddr         [in] i/o address of converter root page
       @param   snapShotVersion             [in] converter version which identifies a frozen
                                                 snapshot of the converter
       @param   packedSnapShotRootBlockAddr [in] i/o address of the root of the frozen
                                                 converter snapshot
       @param   snapShotCreationTimeStamp   [in] creation 'timestamp' of the snapshot
       @param   bIncConverterVersion        [in] indicates whether version must be
                                                 incremented after the restart
       @param   bLastSaveDataSuccessful     [in] indicates whether the last save data
                                                 finished successfully
       @param   MaxDynamicPageNo            [in] maximun used dynamic page number of the
                                                 restarted database
       @param   MaxStaticPageNo             [in] maximun used static page number of the
                                                 restarted database
       @param   NumAllPages                 [in] maximum number of pages which can be stored
                                                 in the data base. this corresponds to the sum
                                                 of size of all volumes
       @param   volumeNoBitCount            [in] configuration value VOLUMENO_BIT_COUNT stored
                                                 in the restart record. This will be checked
                                                 against current configuration value. Should be
                                                 always be the same.
       @return  Converter_ReturnCode


       - The internal structures of the converter are created.
       - Converter pages are read from volumes. The converter root
         page can be found at address 'rootaddr'.
       - The parameters 'MaxDynamicPageNo', 'MaxStaticPageNo' and  'NumAllPages'
         are used to determine the initial size of the  converter.
       - 'ConverterVersion' is the converter version of the database to be
         recovered. If 'bIncConverterVersion' is true, the converter version
         is incremented after a successful restart.
     */
    virtual Converter_ReturnCode Restart (
        const RTETask_ITask             &task,
        const Converter_Version         &ConverterVersion,
        const IOMan_PackedBlockAddress  &packedRootBlockAddr,
        const Converter_Version         &snapShotVersion,
        const IOMan_PackedBlockAddress  &packedSnapShotRootBlockAddr,
        const Kernel_DateTime           &snapShotCreationTimeStamp,
        const SAPDB_Bool                bIncConverterVersion,
        const SAPDB_Bool                bLastSaveDataSuccessful,
        const Data_PageNo               &MaxDynamicPageNo,
        const Data_PageNo               &MaxStaticPageNo,
        const SAPDB_UInt                NumAllPages,
        const SAPDB_UInt                volumeNoBitCount ) = 0;

    /*!
       @brief   Restart the converter for restore pages.
       @param   task                        [in] own task
       @param   ConverterVersion            [in] converter version from restart record
       @param   packedRootBlockAddr         [in] i/o address of converter root page
       @param   snapShotVersion             [in] converter version which identifies a frozen
                                                 snapshot of the converter
       @param   packedSnapShotRootBlockAddr [in] i/o address of the root of the frozen
                                                 converter snapshot
       @param   snapShotCreationTimeStamp   [in] creation 'timestamp' of the snapshot
       @param   bLastSaveDataSuccessful     [in] indicates whether the last save data
                                                 finished successfully
       @param   MaxDynamicPageNo            [in] maximun used dynamic page number of the
                                                 restarted database
       @param   MaxStaticPageNo             [in] maximun used static page number of the
                                                 restarted database
       @param   NumAllPages                 [in] maximum number of pages which can be stored
                                                 in the data base. this corresponds to the sum
                                                 of size of all volumes
       @param   volumeNoBitCount            [in] configuration value VOLUMENO_BIT_COUNT stored
                                                 in the restart record. This will be checked
                                                 against current configuration value. Should be
                                                 always be the same.
       @return  Converter_ReturnCode


       - The internal structures of the converter are restarted
       - Converter pages are read from volumes. The converter root
         page can be found at address 'rootaddr'.
       - The parameters 'MaxDynamicPageNo', 'MaxStaticPageNo' and
         'NumAllPages' are used to determine the initial size of
         the converter.
       - 'ConverterVersion' is the converter version of the database
         to be recovered. If 'bIncConverterVersion' is true, the converter
         version is incremented after a successful restart.
     */
    virtual Converter_ReturnCode RestartForRestore (
        const RTETask_ITask             &task,
        const Converter_Version         &ConverterVersion,
        const IOMan_PackedBlockAddress  &packedRootBlockAddr,
        const Converter_Version         &snapShotVersion,
        const IOMan_PackedBlockAddress  &packedSnapShotRootBlockAddr,
        const Kernel_DateTime           &snapShotCreationTimeStamp,
        const SAPDB_Bool                bLastSaveDataSuccessful,
        const Data_PageNo               &MaxDynamicPageNo,
        const Data_PageNo               &MaxStaticPageNo,
        const SAPDB_UInt                NumAllPages,
        const SAPDB_UInt                volumeNoBitCount ) = 0;

    /*!
       @brief   Creates an empty converter for a new data base (needed for activate serverdb)
       @param   task        [in] own task
       @param   numAllPages [in] maximum number of pages which can be stored in the data base.
                                 this corresponds to the sum of size of all volumes
       @return  none

       - The internal structures of the converter are created.
       - The parameter 'NumAllPages' is used to determine the initial size of the  converter.
     */

    virtual void Create(
        const RTETask_ITask &task,
        const SAPDB_UInt    numAllPages ) = 0;

    /*!
       @brief   Creates an empty converter for restoring existing database
       @param   task                    [in] own task
       @param   converterVersion        [in] converter version from restart record
       @param   bLastSaveDataSuccessful [in] indicates whether the last save data finished successfully
       @param   maxDynamicPageNo        [in] maximun used dynamic page number of the restarted database
       @param   maxStaticPageNo         [in] maximun used static page number of the restarted database
       @param   numAllPages             [in] maximum number of pages which can be stored in the data base.
                                             this corresponds to the sum of size of all data volumes
       @return  none

       - The internal structures of the converter are created and initialized.
       - The parameters 'MaxDynamicPageNo', 'MaxStaticPageNo' and 'NumAllPages'
         are used to determine the initial size of the  converter.
       - ConverterVersion' is the converter version of the database to be recovered.
     */

    virtual void  CreateForRestore(
        const RTETask_ITask     &task,
        const Converter_Version &converterVersion,
        const SAPDB_Bool        bLastSaveDataSuccessful,
        const Data_PageNo       &maxDynamicPageNo,
        const Data_PageNo       &maxStaticPageNo,
        const SAPDB_UInt        numAllPages ) = 0;

    /*!
       @brief   Shuts the converter down
       @param   task [in] own task
       @return  none

       - All resources held by the converter are released and all
         member data are reseted to their initial values
       - All tasks waiting for the converter are resumed.
     */

    virtual void Shutdown( const RTETask_ITask &task ) = 0;


    /*!
       @brief   Saves converter to data volumes (sequential writing)
       @param   task                 [in] own task
       @param   ConverterVersion    [out] new valid converter version
       @param   MaxDynamicPageNo    [out] maximun used dynamic page number. This
                                          number has to be stored in the restart page
                                          in order the calculate the right size of the
                                          converter (dynamic map) after a restart.
       @param   MaxStaticPageNo     [out] maximun used static page number. This number
                                          has to be stored in the restart page in order
                                          the calculate the right size of the converter
                                          (static map) after a restart.
                                          (see Converter_IManager::Restart)
       @param   packedRootBlockAddr [out] Block address of the converter
                                          root page (to be stored in the restart record)
       @return         SAPDB_Bool


       - All converter pages which have been changed since the last
         savepoint and which refer to permanent data pages are
         written out to data devspaces.
       - The routine returns several parameters describing the actual
         state of the converter.
       - If the flush is triggered by a SAVEPOINT the returned parameters are not
         needed since they were already inserted into the restart record by
         the Converter_ISavePoint::EndSavePoint method.
         If the converter is not active the return value is false;
     */
    virtual SAPDB_Bool FlushSerial (
        const RTETask_ITask         &task,
        Converter_Version           &ConverterVersion,
        Data_PageNo                 &MaxStaticPageNo,
        Data_PageNo                 &MaxDynamicPageNo,
        IOMan_PackedBlockAddress    &packedRootBlockAddr ) = 0;

    /*!
       @brief   Saves converter to data volumes (sequential writing)
       @param   task                 [in] own task
       @param   packedRootBlockAddr [out] Block address of the converter root
                                          page (to be stored in the restart record
       @return  SAPDB_Bool


       - All converter pages which have been changed since the last
         savepoint and which refer to permanent data pages are
         written out to data volumes in a sequential way.
         If the converter is not active the return value is false;
     */
    virtual SAPDB_Bool FlushSerialForColdSave (
        const RTETask_ITask         &task,
        IOMan_PackedBlockAddress    &packedRootBlockAddr ) = 0;

    /*!
       @brief   Expands the converter / externaly synchronized by IOManager!
       @param   task          [in] own task
       @param   numPagesAdded [in] number of pages added and usable by the data base
       @return  none

       - Expands the converter to manage NumPagesAdded more data pages
       - The internal structures of the converter are adjusted if possible
     */
    virtual SAPDB_Bool Expand (
        const RTETask_ITask &task,
        const SAPDB_UInt    numPagesAdded ) = 0;

    /*!
       @brief   shrinks the converter / externaly synchronized by IOManager!
       @param   task            [in] own task
       @param   numPagesRemoved [in] number of pages removed from the data base
       @return  none

       - Shrinks the converter to manage NumPagesRemoved less data pages
       - The internal structures of the converter are not adjusted
     */
    virtual SAPDB_Bool Shrink (
        const RTETask_ITask &task,
        const SAPDB_UInt    numPagesRemoved ) = 0;
};

#endif  /* CONVERTER_IMANAGER_HPP */
