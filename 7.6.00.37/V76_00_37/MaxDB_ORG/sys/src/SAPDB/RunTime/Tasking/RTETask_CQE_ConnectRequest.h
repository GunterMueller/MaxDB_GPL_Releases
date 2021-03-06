/*!========================================================================

  @file         RTETask_CQE_ConnectRequest.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Common queue element part (connect request)

                The following class provides methodes to access
                connect request part. This object is part of  the 
                common queue element. 

  @since        2003-10-23  18:08
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef RTETASK_CQE_CONNECTREQUEST_H
#define RTETASK_CQE_CONNECTREQUEST_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined (__cplusplus)
  struct RTETask_TaskClientConnection;
#endif

/*!---------------------------------------------------------------------------
  @class   RTETask_CQE_ConnectRequest
  @brief   Common queue element part (connect request)

           This element part is used to transfer the connect request
           to a task that is waiting for an incomming connect.

           This struct is designed to be used in C and C++ coding.
           It has no constructor and must be initialized via Init() methode.

           It is used as part of a union in conjunction with the legacy 
           part of the common queue element (RTETask_CommonQueueElement).
 ----------------------------------------------------------------------------*/
struct RTETask_CQE_ConnectRequest
{
#if defined (__cplusplus)
public:
    /*!
    @brief         Initialize the this object.

                   The values must be set via set methodes.

    @param         pTaskClientConnection      [in] - Pointer to a task client
                                                     connection object
    */
    void Init( RTETask_TaskClientConnection *  pTaskClientConnection )
    {
        m_pTaskClientConnection = pTaskClientConnection;
    }



    /*!
    @brief         Get task client object pointer.
    @return        Pointer to a task client connection object
    */
    RTETask_TaskClientConnection*  TaskClientConnection()
    {
        return m_pTaskClientConnection;
    }
private:
#endif
    /*  
     *  The C and C++ member data goes here....
     */
    struct RTETask_TaskClientConnection *    m_pTaskClientConnection;
};



/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/



#endif  /* RTETASK_CQE_CONNECTREQUEST_H */
