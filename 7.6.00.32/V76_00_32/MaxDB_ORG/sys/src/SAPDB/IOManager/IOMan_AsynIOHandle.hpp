/*!
    @file       IOMan_AsynIOHandle.hpp
    @author     TorstenS
    @ingroup    IOManagement
    @brief      This class is used to get the asynchron id of the backup chancel
                from which a backup cluster has to be read.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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

#ifndef IOMAN_ASYNIOHANDLE_HPP
#define IOMAN_ASYNIOHANDLE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "KernelCommon/Kernel_Common.hpp"
#include "gkb03.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  DECLARATION OF CLASSES, STRUCTURES, TYPES, UNIONS ...                    *
 *===========================================================================*/

/*!
    @class  IOMan_AsynIOHandle
    @brief  This class is used to get the asynchron id of the backup chancel
            from which a backup cluster has to be read.
 */

class IOMan_AsynIOHandle
{
public:

     /*!
       @brief   Constructor
       @param   pAsynIOHandle [in] pointer onto the backup/restore administration structure
                                   the encapsulate the asynchron volume id for asyn io
    */
 
    IOMan_AsynIOHandle( const tkb3_save_static_ptr  pAsynIOHandle )
    :
    m_pAsynIOHandle( pAsynIOHandle )
    {}

    // return id for asynchron volume io
    SAPDB_Int4 GetId( const IOMan_VolumeNo volNo ) const{

        const SAPDB_Int2        volIdx  = (*(m_pAsynIOHandle->sta_vol_idx()))[ volNo -1 ];
        const tkb3_devsp_desc&  volDesc = (*(m_pAsynIOHandle->sta_devsp()))[ volIdx -1 ]; 
        SAPDBERR_ASSERT_STATE( volDesc.dvd_volno == volNo );
        return volDesc.dvd_fno; 
    }
  
private:

    const tkb3_save_static_ptr    m_pAsynIOHandle;
};

/*===========================================================================*
 *  END CODE                                                                 *
 *===========================================================================*/

#endif // IOMAN_ASYNIOHANDLE_HPP

