/*!**************************************************************************

  module      : RTEPPars_DLLMain.cpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: SAPDB Protocol Parser Dll Main 
  description : 


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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/ProtocolParser/RTEPPars_SAPDBProt.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

extern "C"  BOOL WINAPI DllMain( HANDLE hInstance, ULONG Command, LPVOID Reserved )
{
  switch(Command)
  {
  case DLL_PROCESS_ATTACH:
    // Loading parser DLL.
    RTEPPars_SAPDBProt::Instance().CreateProtocol();
    break;
    // Call DestroyProtocol.
  case DLL_PROCESS_DETACH:
    // Unloading parser DLL.
    RTEPPars_SAPDBProt::Instance().DestroyProtocol();
    break;
  }
  return TRUE;
}

/*---------------------------------------------------------------------------*/

extern "C" __declspec(dllexport) PPF_PARSERDLLINFO ParserAutoInstallInfo() 
{
  return RTEPPars_SAPDBProt::Instance().ParserAutoInstallInfo();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/