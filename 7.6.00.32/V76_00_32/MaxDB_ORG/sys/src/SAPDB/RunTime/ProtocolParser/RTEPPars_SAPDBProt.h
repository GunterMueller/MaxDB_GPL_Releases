/*!**************************************************************************

  module      : RTEPPars_SAPDBProt.h

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: SAPDB Protocol Parser

  description : Implements a protocol parser which can be used be the Microsoft
                Network Monitor


  last changed: 2002-08-16  10:51
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/


#ifndef RTEPPARS_SAPDBPROT_H
#define RTEPPARS_SAPDBPROT_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <winsock2.h>
#include <netmon.h>
#include <windows.h>
#include "geo003.h"
#include "SAPDBCommon/SAPDB_Singleton.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTEPPars_SAPDBPropEntry
{
  PROPERTYINFO        Info;
  DWORD               IdentLevel;
  SAPDB_Int4          Length;
  DWORD               DataOffset;
  DWORD               HelpID;
  DWORD               IFlags;
};



/*!---------------------------------------------------------------------------
  Class:        RTEPPars_SAPDBProt
  Description:  SAPDB Protocol Parser

                Implements a protocol parser which can be used be the Microsoft
                Network Monitor
*/
class RTEPPars_SAPDBProt 
{
public:
  RTEPPars_SAPDBProt ();
  ~RTEPPars_SAPDBProt() {;}

   /*!---------------------------------------------------------------------------
    Function:       CreateProtocol
    Description:    CreateProtocol notifies Network Monitor that the SAPDB protocol parser exists
    Arguments:      none
    Return value:   none
   */
   void CreateProtocol();

   /*!---------------------------------------------------------------------------
    Function:       DestroyProtocol
    Description:    DestroyProtocol destroys the protocol that the CreateProtocol function creates.
    Arguments:      none
    Return value:   none
   */
   void DestroyProtocol();

  /*!
    Function:       Instance
    Description:    Return reference to single instance of RTEPPars_SAPDBProt
                    The Initialize() function must have been called before.
    Arguments:      none
    Return value:   Reference to RTEMem_AWEAllocator instance
  */
  static RTEPPars_SAPDBProt & Instance();

  PPF_PARSERDLLINFO ParserAutoInstallInfo();

private:
  //
  // registered entry points called directly by the network monitor 
  //
  static void BHAPI _Register            ( HPROTOCOL hProtocol )
    { Instance().Register( hProtocol ); }

  static  void WINAPI _Deregister        ( HPROTOCOL hProtocol )
    { Instance().Deregister( hProtocol ); }

  static  LPBYTE BHAPI _AttachProperties ( HFRAME     hFrame,
                                           LPBYTE     pMacFrame,
                                           LPBYTE     pFrame,
                                           DWORD      MacType,
                                           DWORD      Bytesleft,
                                           HPROTOCOL  hPreviousProtocol,
                                           DWORD      nPrevProtocolOffset,
                                           DWORD      InstData )
    { return Instance().AttachProperties( hFrame, pMacFrame, pFrame, MacType, Bytesleft, 
                                          hPreviousProtocol, nPrevProtocolOffset, InstData ); }

  static DWORD BHAPI _FormatProperties  ( HFRAME         hFrame,
                                          LPBYTE         pMacFrame,
                                          LPBYTE         pFrame,
                                          DWORD          nPropertyInsts,
                                          PROPERTYINST   *p )
    { return Instance().FormatProperties( hFrame,  pMacFrame, pFrame, nPropertyInsts, p ); }

  static LPBYTE BHAPI _RecognizeFrame   ( HFRAME     hFrame,
                                          LPBYTE     pMacFrame,
                                          LPBYTE     pProtocol,
                                          DWORD      MacType,
                                          DWORD      BytesLeft,
                                          HPROTOCOL  hPrevProtocol,
                                          DWORD      nPreviuosProtOffset,
                                          LPDWORD    pProtocolStatus,
                                          HPROTOCOL  *phNextProtocol,
                                          LPDWORD    InstData )
    { return Instance().RecognizeFrame( hFrame, pMacFrame, pProtocol, MacType,
                                         BytesLeft, hPrevProtocol, nPreviuosProtOffset,
                                         pProtocolStatus, phNextProtocol, InstData ); }

private:
  DWORD BHAPI   Register        ( HPROTOCOL hProtocol );

  DWORD WINAPI  Deregister      ( HPROTOCOL hProtocol );

  LPBYTE BHAPI AttachProperties( HFRAME        hFrame,
                                 LPBYTE        pMacFrame,
                                 LPBYTE        pFrame,
                                 DWORD         MacType,
                                 DWORD         Bytesleft,
                                 HPROTOCOL     hPreviousProtocol,
                                 DWORD         nPrevProtocolOffset,
                                 DWORD         InstData );
  DWORD BHAPI FormatProperties ( HFRAME        hFrame,
                                 LPBYTE        pMacFrame,
                                 LPBYTE        pFrame,
                                 DWORD         nPropertyInsts,
                                 PROPERTYINST  *p );

  void   FormatSummaryProperty ( LPBYTE         pFrame,
                                 LPBYTE         pMacFrame,
                                 PROPERTYINST   *p );

  LPBYTE BHAPI RecognizeFrame  ( HFRAME        hFrame,
                                 LPBYTE        pMacFrame,
                                 LPBYTE        pProtocol,
                                 DWORD         MacType,
                                 DWORD         BytesLeft,
                                 HPROTOCOL     hPrevProtocol,
                                 DWORD         nPreviuosProtOffset,
                                 LPDWORD       pProtocolStatus,
                                 HPROTOCOL     *phNextProtocol,
                                 LPDWORD       InstData );

  DWORD       AddProperty      ( DWORD                     EntryNo,
                                 DWORD                     IdentLevel,
                                 HPROTOCOL                 hProtocol,
                                 SAPDB_Int4                Length,
                                 DWORD                     DataOffset,
                                 DWORD                     HelpID,
                                 DWORD                     IFlags,
                                 LPSTR const               Label,
                                 LPSTR const               Comment,
                                 BYTE                      DataType,
                                 BYTE                      DataQualifier );

  PCHAR const GetMessClassName ( SAPDB_UInt1               MessClass );
  SAPDB_UInt4 TCPAcknowledgeNum( LPBYTE                    pFrame, 
                                 LPBYTE                    pMacFrame );

private:
  LPHANDOFFTABLE            m_hTable;
  HPROTOCOL                 m_hProtocol;
  DWORD                     m_Attached; 
  PCHAR                     m_ProtocolName;
  PCHAR                     m_ProtocolInfo;
  ENTRYPOINTS               m_EntryPoints;
  DWORD                     m_Port;
  DWORD                     m_NumParsers;
  DWORD                     m_NumFollows;
  DWORD                     m_NumHandoffs;
  RTEPPars_SAPDBPropEntry*  m_PropertyTable;
  DWORD                     m_NumProperties;

private:
  SAPDB_Singleton           m_ThisIsASingleton; // prevents copy and assignment
  static RTEPPars_SAPDBProt *m_Instance;
};
/*! EndClass: RTEPPars_SAPDBProt */

#endif  /* RTEPPARS_SAPDBPROT_H */
