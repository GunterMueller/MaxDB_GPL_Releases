/*!
  @file           Converter_ISavePoint.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          abstract savepoint interface class to the converter
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

#ifndef CONVERTER_ISAVEPOINT_HPP
#define CONVERTER_ISAVEPOINT_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Version.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"


class RTETask_ITask;


/*!
  @interface      Converter_ISavePoint
  @brief          abstract savepoint interface class to the converter
 */
class Converter_ISavePoint
{
public:

    /*!
       @brief          returns the savepoint interface of the converter
       @return         (Converter_ISavePoint) instance of the converter

       - This static function returns the interface of the converter which
         allows access to all methods defined in this interface class
     */
    static Converter_ISavePoint & Instance();

    /*!
       @brief          Sets all savepoint activ flags in map control
       @param          task [in] own task
       @return         none
       @exception      none

       - After all save point activ flags are set no more blocks can be set to the state
         free_after_save_point. Instead pages wich become free are stored in a FreePageNoList.
         After the save point is finished successfully all page numbers which are in the
         FreePageNoList are released and the corresponding blocks are set to free or
         free_after_savepoint (see method) FreePageNosAfterSavepoint.
     */
    virtual void  BeginSavepoint ( const RTETask_ITask &task ) = 0;

    /*!
       @brief          Increases the converter version and resets all savepoint activ flags
       @param          task              [in] own task
       @param          converterVersion [out] new valid converter version
       @return         none
     */
    virtual void EndSavepoint( const RTETask_ITask  &task,
                               Converter_Version    &converterVersion ) = 0;

    /*!
       @brief          Release all PageNos stored in the FreePageNoList
       @param          taskId [in] current task id
       @return         none

       - After the save point is finished successfully all page numbers which are in the
         FreePageNoList are released and the corresponding blocks are set to free or free_after_savepoint
     */
    virtual void FreePageNosAfterSavepoint( const RTETask_ITask &task  ) = 0;

    /*!
       @brief   Saves converter to devspace (parallel writing)
       @param   task                 [in] own task
       @param   ConverterVersion    [out] new valid converter version
       @param   packedRootBlockAddr [out] Block address of the converter root page
                                          (to be stored in the restart record)
       @param   MaxDynamicPageNo    [out] maximun used dynamic page number.
                                          This number has to be stored in the restart
                                          page in order the calculate the right size of
                                          the converter (perm map)  after a restart.
       @param   MaxStaticPageNo     [out] maximun used static page number. This
                                          number has to be stored in the restart page in
                                          order the calculate the right size of the
                                          converter (static map) after a restart. (see
                                          Converter_IManager::Restart)
       @param   NumPages            [out] number of pages written
       @param   NumIO               [out] number of IO transaktions
       @return  none


       - All converter pages which have been changed since the last
         savepoint and which refer to permanent data pages are written
         out to data devspaces.
       - The routine returns several parameters describing the actual
         state of the converter.
     */
     virtual void FlushParallel(
        const RTETask_ITask         &task,
        Converter_Version           &ConverterVersion,
        Data_PageNo                 &MaxStaticPageNo,
        Data_PageNo                 &MaxDynamicPageNo,
        IOMan_PackedBlockAddress    &packedRootBlockAddr,
        SAPDB_UInt                  &NumPages,
        SAPDB_UInt                  &NumIO ) = 0;

    /*!
       @brief   prepares the converter for dropping a volume
       
       - Waits until a running savepoint has finished
         marks all permanent converter pages as changed to force a rewrite of all
         converter pages during the next savepoint
       @param   task [in] own task
       @return  none
     */
    virtual void PrepareForDropVolume( const RTETask_ITask &task ) = 0;
};


#endif  /* CONVERTER_ISAVEPOINT_HPP */
