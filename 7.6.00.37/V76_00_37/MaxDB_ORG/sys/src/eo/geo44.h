/*!
  @file           geo44.h
  @author         RaymondR
  @special area   communication
  @brief          This module contains the communication request and 
              response classes
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



#ifndef GEO44_H  
#define GEO44_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo42.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

// --- request/response types ( 1 - 32 ) reserved by base class in geo42.h
#define REQ_RESP_TYPE_BUFFER_SIZE_REQUEST           33
#define REQ_RESP_TYPE_BUFFER_SIZE_RESPONSE          34

#define REQ_RESP_TYPE_SET_TRANSFER_BUFFER_REQUEST   35
#define REQ_RESP_TYPE_SET_TRANSFER_BUFFER_RESPONSE  36

#define REQ_RESP_TYPE_CANCEL_REQUEST                125
#define REQ_RESP_TYPE_DISCONNECT_REQUEST            127
#define REQ_RESP_TYPE_DISCONNECT_RESPONSE           128
// --- request/response types ( 129 - 255 ) reserved for application
//     specific connections like DBMServer...

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/





/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                class teo44_QueryTransferBufferSizeReq                     */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo44_QueryTransferBufferSizeReq
   @Description    Build up and/or extract a transfer buffer size request packet.


                This class is used to build up and/or extract a packet to
                request the peer transfer buffer size and its minmum
                response packet size.
                teo44_QueryTransferBufferSizeReq is derived from 
                teo42_RTEReqRespBase

 */

class teo44_QueryTransferBufferSizeReq : public teo42_RTEReqRespBase
{
public:
  struct  teo44_Params : public teo42_RTEReqRespBase::teo42_FixedPartParams
  {
    tsp00_Uint4                     MaxTransferBufferSize_eo44; // - optional
  };

public:
                      teo44_QueryTransferBufferSizeReq () { eo44_InitParams();};
                      ~teo44_QueryTransferBufferSizeReq() {;};

  teo44_Params&       eo44_Params                    () { return m_Params; };
  void                eo44_InitParams                ();

  static tsp00_Uint1  eo42_ReqRespType               () { return REQ_RESP_TYPE_BUFFER_SIZE_REQUEST; };

protected:
  virtual teo42_FixedPartParams& eo42_FixedPartParams() { return m_Params; };

  virtual bool        eo42_FillVarPart               ( teo42_RTEReqRespVarPart             &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );
                                                                                         
  virtual bool        eo42_ExtractVarPart            ( teo42_RTEReqRespVarPart const       &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );
                                                                                         
private:
  teo44_Params  m_Params;
}; 


/*---------------------------------------------------------------------------*/

inline void  teo44_QueryTransferBufferSizeReq::eo44_InitParams ()
{
  ROUTINE_DBG_MEO60 (_T("teo44_QueryTransferBufferSizeReq::eo44_InitParams"));

  eo42_InitFixedPartParams();

  m_Params.MaxTransferBufferSize_eo44 = (tsp00_Uint4) -1; // max. uint4
}


/*---------------------------------------------------------------------------*/


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                  class teo44_QueryTransferBufferSizeResp                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo44_QueryTransferBufferSizeResp
   @Description    Build up and/or extract a transfer buffer size response packet.


                This class is used to build up and/or extract a packet to
                respond a transfer buffer size request.
                teo44_QueryTransferBufferSizeResp is derived from 
                teo42_RTEReqRespBase

 */

class teo44_QueryTransferBufferSizeResp : public teo42_RTEReqRespBase
{
public:
  struct  teo44_Params : public teo42_RTEReqRespBase::teo42_FixedPartParams
  {
    tsp00_Uint4                     TransferBufferSize_eo44; 
    tsp00_Uint4                     MinResponsePacketSize_eo44;       
  };

public:
                      teo44_QueryTransferBufferSizeResp () { eo44_InitParams();};
                      ~teo44_QueryTransferBufferSizeResp() {;};

  teo44_Params&       eo44_Params                    () { return m_Params; };
  void                eo44_InitParams                ();

  static tsp00_Uint1  eo42_ReqRespType               () { return REQ_RESP_TYPE_BUFFER_SIZE_RESPONSE; };

protected:
  virtual teo42_FixedPartParams& eo42_FixedPartParams() { return m_Params; };

  virtual bool        eo42_FillVarPart               ( teo42_RTEReqRespVarPart             &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );
                                                                                         
  virtual bool        eo42_ExtractVarPart            ( teo42_RTEReqRespVarPart const       &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );
                                                                                         
private:
  teo44_Params      m_Params;
}; 


/*---------------------------------------------------------------------------*/

inline void  teo44_QueryTransferBufferSizeResp::eo44_InitParams ()
{
  ROUTINE_DBG_MEO60 (_T("teo44_QueryTransferBufferSizeResp::eo44_InitParams"));

  eo42_InitFixedPartParams();

  m_Params.TransferBufferSize_eo44    = (tsp00_Uint4) UNDEF_SP00;
  m_Params.MinResponsePacketSize_eo44 = 0;
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                   class teo44_SetTransferBufferSizeReq                    */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo44_SetTransferBufferSizeReq
   @Description    Build up and/or extract a set transfer buffer size request 
              packet.


                This class is used to build up and/or extract a packet to
                set the peer transfer buffer size.
                teo44_SetTransferBufferSizeReq is derived from 
                teo42_RTEReqRespBase

 */

class teo44_SetTransferBufferSizeReq : public teo42_RTEReqRespBase
{
public:
  struct  teo44_Params : public teo42_RTEReqRespBase::teo42_FixedPartParams
  {
    tsp00_Uint4                     TransferBufferSize_eo44;      // - in/out
  };

public:
                      teo44_SetTransferBufferSizeReq () { eo44_InitParams();};
                      ~teo44_SetTransferBufferSizeReq() {;}

  teo44_Params&       eo44_Params                    () { return m_Params; };
  void                eo44_InitParams                ();

  static tsp00_Uint1  eo42_ReqRespType               () { return REQ_RESP_TYPE_SET_TRANSFER_BUFFER_REQUEST; };

protected:
  virtual teo42_FixedPartParams& eo42_FixedPartParams() { return m_Params; };

  virtual bool        eo42_FillVarPart               ( teo42_RTEReqRespVarPart             &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );
                                                                                          
  virtual bool        eo42_ExtractVarPart            ( teo42_RTEReqRespVarPart const       &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );
private:
  teo44_Params      m_Params;
}; 


/*---------------------------------------------------------------------------*/

inline void  teo44_SetTransferBufferSizeReq::eo44_InitParams ()
{
  ROUTINE_DBG_MEO60 (_T("teo44_SetTransferBufferSizeReq::eo44_InitParams"));

  eo42_InitFixedPartParams();

  m_Params.TransferBufferSize_eo44 = (tsp00_Uint4) UNDEF_SP00;
}



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                   class teo44_SetTransferBufferSizeResp                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo44_SetTransferBufferSizeResp
   @Description    Build up and/or extract a set transfer buffer size response 
              packet.


                This class is used to build up and/or extract a packet to
                respond a set transfer buffer size request.
                teo44_SetTransferBufferSizeResp is derived from 
                teo42_EmptyReqResp

 */

class teo44_SetTransferBufferSizeResp : public teo42_EmptyReqResp
{
public:
  static tsp00_Uint1  eo42_ReqRespType                () { return REQ_RESP_TYPE_SET_TRANSFER_BUFFER_RESPONSE; };
}; 


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         class teo44_DisconnectReq                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo44_DisconnectReq
   @Description    Build up and/or extract disconnect request packet.


                teo44_DisconnectReq is derived from 
                teo42_EmptyReqResp

 */

class teo44_DisconnectReq : public teo42_EmptyReqResp
{
public:
  static tsp00_Uint1  eo42_ReqRespType                () { return REQ_RESP_TYPE_DISCONNECT_REQUEST; };
}; 




/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                        class teo44_DisconnectResp                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo44_DisconnectResp
   @Description    Build up and/or extract disconnect response packet.


                teo44_DisconnectResp is derived from 
                teo42_EmptyReqResp

 */

class teo44_DisconnectResp : public teo42_EmptyReqResp
{
public:
  static tsp00_Uint1  eo42_ReqRespType                () { return REQ_RESP_TYPE_DISCONNECT_RESPONSE; };
}; 



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class teo44_CancelReq                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo44_CancelReq
   @Description    Build up and/or extract cancel request packet.


                teo44_CancelReq is derived from 
                teo42_EmptyReqResp

 */

class teo44_CancelReq : public teo42_EmptyReqResp
{
public:
  static tsp00_Uint1  eo42_ReqRespType                () { return REQ_RESP_TYPE_CANCEL_REQUEST; };
}; 




/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO44_H */
