/*!
  @file           vos211.cpp
  @author         RaymondR
  @brief          SAPDB - Cluster Server Manager
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include <afx.h>
#include <malloc.h>

#include "gos202.h"
#include "geo81.h"
#include "geo60.h"
#include "gos00.h"
#include "geo007_2.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif


#define OPTION_HELP                          1
#define OPTION_MOVE_DRIVE_TO_GROUP           2
#define OPTION_BUILDUP_GROUP                 3
#define OPTION_REMOVE_SAPDB_RESOURCES        4
#define OPTION_CREATE_RESOURCE_TYPES         5
#define OPTION_REMOVE_RESOURCE_TYPES         6
#define OPTION_ONLINE_XSERVER_RESOURCE       7
#define OPTION_OFFLINE_XSERVER_RESOURCE      8
#define OPTION_ONLINE_SERVERDB_RESOURCE      9
#define OPTION_OFFLINE_SERVERDB_RESOURCE     10
#define OPTION_ADD_SERVERDB_RESOURCE         11
#define OPTION_REMOVE_SERVERD_BRESOURCE      12
#define OPTION_ENABLE_CLUSTERLOG             13
#define OPTION_DISABLE_CLUSTERLOG            14
#define OPTION_REGISTER_DEBUGEX              15
#define OPTION_UNREGISTER_DEBUGEX            16
#define OPTION_ONLINE_GROUP                  17
#define OPTION_OFFLINE_GROUP                 18
#define OPTION_VERBOSE_ERROR_MESSAGES        19
#define OPTION_ERROR_MESSAGES_TO_DBOX        20



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


class t_EvalArgs : public teo81_EvalProgArgs
{
public:
  t_EvalArgs                               ( int argc, TCHAR** argv );
  ~t_EvalArgs                              () {;};

private:
  bool       eo80_vNextOptionPass1         ( const int            OptionIndex,
                                             const TCHAR*         OptionArgument );
  bool       eo80_vNextValidOption         ( const int            OptionIndex,
                                             const TCHAR*         OptionArgument );

  bool       eo80_vPass1Finished           ( const _TCHAR* const* ArgvRemaining,
                                             int                  ArgsRemaining,
                                             int                  ArgsParsed,
                                             bool                 BreakOptionFound ) 
                                           { 
                                             if ( ArgsRemaining > 0 )
                                             {
                                                eo80_vHelp(NULL);
                                                return false;
                                             }
                                             return true;
                                           };
private:
  tos202_CluManDll      *CluManDll;
  unsigned              EvtOptions;
};

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static teo81_EvalProgArgs::teo81_OptionDesc Options[] = 
       {{ OPTION_HELP                     , _T('h'),  _T("help")                  , true, true,  _T("[<Option> | LONG]"),                                      OPTION_HELP_STR },
        { OPTION_HELP                     , _T('?'),  _T("?")                     , true, true,  _T("[<Option> | LONG]"),                                      OPTION_HELP_STR },
        { OPTION_ERROR_MESSAGES_TO_DBOX   , _T('d'),  _T("DialogBox")             , false,true,  _T(""),                                                       OPTION_ERROR_MESSAGES_TO_DBOX_STR },
        { OPTION_VERBOSE_ERROR_MESSAGES   , _T('v'),  _T("Verbose")               , false,true,  _T(""),                                                       OPTION_VERBOSE_ERROR_MESSAGES_STR },
        { OPTION_MOVE_DRIVE_TO_GROUP      , _T('m'),  _T("MoveDriveToGroup")      , true, true,  _T("<DiskName>"),                                             OPTION_MOVE_DRIVE_TO_GROUP_STR },
        { OPTION_ONLINE_GROUP             , _T('G'),  _T("OnlineGroup")           , true, true,  _T("<TimeOut>,[<GroupName>]"),                                OPTION_ONLINE_GROUP_STR },
        { OPTION_OFFLINE_GROUP            , _T('g'),  _T("OfflineGroup")          , true, true,  _T("<TimeOut>,[<GroupName>]"),                                OPTION_OFFLINE_GROUP_STR },            
        { OPTION_BUILDUP_GROUP            , _T('B'),  _T("BuildUpGroup")          , true, true,  _T("\"<NetworkName>,<IPAddress>,<SubnetMask>,<DiskName>\""),  OPTION_BUILDUP_GROUP_STR },            
        { OPTION_REMOVE_SAPDB_RESOURCES   , _T('A'),  _T("RemoveAllResources")    , false,true,  _T(""),                                                       OPTION_REMOVE_SAPDB_RESOURCES_STR },
        { OPTION_CREATE_RESOURCE_TYPES    , _T('C'),  _T("CreateResourceTypes")   , false,true,  _T(""),                                                       OPTION_CREATE_RESOURCE_TYPES_STR },
        { OPTION_REMOVE_RESOURCE_TYPES    , _T('R'),  _T("RemoveResourceTypes")   , false,true,  _T(""),                                                       OPTION_REMOVE_RESOURCE_TYPES_STR },
        { OPTION_ONLINE_XSERVER_RESOURCE  , _T('O'),  _T("OnlineXSERVER")         , false,true,  _T(""),                                                       OPTION_ONLINE_XSERVER_RESOURCE_STR },
        { OPTION_OFFLINE_XSERVER_RESOURCE , _T('F'),  _T("OfflineXSERVER")        , false,true,  _T(""),                                                       OPTION_OFFLINE_XSERVER_RESOURCE_STR },
        { OPTION_ONLINE_SERVERDB_RESOURCE , _T('o'),  _T("OnlineSERVERDB")        , true, true,  _T("<ServerDB>"),                                             OPTION_ONLINE_SERVERDB_RESOURCE_STR },
        { OPTION_OFFLINE_SERVERDB_RESOURCE, _T('f'),  _T("OfflineSERVERDB")       , true, true,  _T("<ServerDB>"),                                             OPTION_OFFLINE_SERVERDB_RESOURCE_STR },
        { OPTION_ADD_SERVERDB_RESOURCE    , _T('a'),  _T("AddSERVERDBResource")   , true, true,  _T("<ServerDB>,\"[<Remark>]\""),                              OPTION_ADD_SERVERDB_RESOURCE_STR },
        { OPTION_REMOVE_SERVERD_BRESOURCE , _T('r'),  _T("RemoveSERVERDBResource"), true, true,  _T("<ServerDB>"),                                             OPTION_REMOVE_SERVERD_BRESOURCE_STR },
        { OPTION_ENABLE_CLUSTERLOG        , _T('E'),  _T("EnableClusterLog")      , true, true,  _T("<MBLogSize>,[<LogFile>]"),                                OPTION_ENABLE_CLUSTERLOG_STR },
        { OPTION_DISABLE_CLUSTERLOG       , _T('D'),  _T("DisableClusterLog")     , false,true,  _T(""),                                                       OPTION_DISABLE_CLUSTERLOG_STR },
        { OPTION_REGISTER_DEBUGEX         , _T('x'),  _T("RegisterDebugEx")       , false,true,  _T(""),                                                       OPTION_REGISTER_DEBUGEX_STR },
        { OPTION_UNREGISTER_DEBUGEX       , _T('i'),  _T("UnregisterDebugEx")     , false,true,  _T(""),                                                       OPTION_UNREGISTER_DEBUGEX_STR }
       };
                    

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/




/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

int main ( int argc, char **argv )
{
  ROUTINE_DBG_MEO60 (_T("main"));

  t_EvalArgs Args( argc, argv );

  bool Ok = Args.eo80_ParseArguments();

  return (int)!Ok;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/


/*---------------------------------------------------------------------------*/
/*---------------------------- class t_EvalArgs -----------------------------*/
/*---------------------------------------------------------------------------*/

t_EvalArgs::t_EvalArgs ( int argc, TCHAR** argv )
           : CluManDll(NULL),
             EvtOptions ( EVTOPT_PRINT_ON_OS202 ),
             teo81_EvalProgArgs( argc, argv, 
                                 Options, NUM_OF_OPTIONS_EO80(Options),
                                 OPTION_HELP )
{
  ROUTINE_DBG_MEO60 (_T("t_EvalArgs::t_EvalArgs"));
};


/*---------------------------------------------------------------------------*/

bool t_EvalArgs::eo80_vNextOptionPass1 ( const int            OptionIndex,
                                         const TCHAR*         OptionArgument )
{
  ROUTINE_DBG_MEO60 (_T("t_EvalArgs::eo80_vNextOptionPass1"));

  switch ( OptionIndex )
  {
  case OPTION_ERROR_MESSAGES_TO_DBOX   :
    EvtOptions |= EVTOPT_DIALOG_BOX_OS202;
    break;
  case OPTION_VERBOSE_ERROR_MESSAGES   :
    EvtOptions |= EVTOPT_VERBOSE_OS202;
    break;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool t_EvalArgs::eo80_vNextValidOption ( const int            OptionIndex,
                                         const TCHAR*         OptionArgument )
{
  ROUTINE_DBG_MEO60 (_T("t_EvalArgs::eo80_vNextValidOption"));

  bool Ok = true;

  if ( CluManDll == NULL )
  {
    CluManDll = new tos202_CluManDll( EvtOptions );

    if ( CluManDll == NULL )
      return false;
  }

  if ( CluManDll->os203_DLLLoaded () == false )
    return false;

  switch ( OptionIndex )
  {
  case OPTION_ERROR_MESSAGES_TO_DBOX   :  // already done during pass1
    break; 
  case OPTION_VERBOSE_ERROR_MESSAGES   :  // already done during pass1
    break;
  case OPTION_MOVE_DRIVE_TO_GROUP      :
    CluManDll->os202_MoveDriveToGroup ( OptionArgument );
        return false;
    break;
  case OPTION_ONLINE_GROUP             :
    {
      teo80_SeparateArgs    SeparateArgs(OptionArgument, _T(','));
      const TCHAR*          TimeOut     = SeparateArgs.NextPart();
      const TCHAR*          GroupName   = SeparateArgs.NextPart();

      if ( TimeOut == NULL ) 
        TimeOut = _T("0");

      if (!CluManDll->os202_BringGroupOnline( _ttoi(TimeOut) * 1000, GroupName ))
        return false;
    }
    break;
  case OPTION_OFFLINE_GROUP            :
    {
      teo80_SeparateArgs    SeparateArgs(OptionArgument, _T(','));
      const TCHAR*          TimeOut     = SeparateArgs.NextPart();
      const TCHAR*          GroupName   = SeparateArgs.NextPart();

      if ( TimeOut == NULL ) 
        TimeOut = _T("0");

      if (!CluManDll->os202_BringGroupOffline( _ttoi(TimeOut) * 1000, GroupName ))
        return false;
    }
    break;
  case OPTION_BUILDUP_GROUP            :
    {
      teo80_SeparateArgs    SeparateArgs(OptionArgument, _T(','));
      const TCHAR*          NetworkName = SeparateArgs.NextPart();
      const TCHAR*          IPAddress   = SeparateArgs.NextPart();
      const TCHAR*          SubnetMask  = SeparateArgs.NextPart();
      const TCHAR*          DriveName   = SeparateArgs.NextPart();
      
      if (( NetworkName != NULL ) &&
          ( IPAddress   != NULL ) &&
          ( SubnetMask  != NULL ) &&
          ( DriveName   != NULL ) &&
          ( SeparateArgs.NextPart() == NULL ))
      {
        if (!CluManDll->os202_BuildUpGroup ( NetworkName, IPAddress, SubnetMask, DriveName ))
            return false;
      }
      else
        eo80_vHelp ( NULL );
    }
    break;

  case OPTION_REMOVE_SAPDB_RESOURCES  :
    if (!CluManDll->os202_RemoveSAPDBResources())
        return false;
    break;
  case OPTION_CREATE_RESOURCE_TYPES    :
    if (!CluManDll->os202_CreateResourceTypes())
        return false;
    break;
  case OPTION_REMOVE_RESOURCE_TYPES    :
    if (!CluManDll->os202_RemoveResourceTypes())
        return false;
    break;
  case OPTION_ONLINE_XSERVER_RESOURCE  :
    if (!CluManDll->os202_OnlineXSERVERResource())
        return false;
    break; 
  case OPTION_OFFLINE_XSERVER_RESOURCE :
    if (!CluManDll->os202_OfflineXSERVERResource())
        return false;
    break;
  case OPTION_ONLINE_SERVERDB_RESOURCE :
    if (!CluManDll->os202_OnlineSERVERDBResource( OptionArgument ))
        return false;
    break;
  case OPTION_OFFLINE_SERVERDB_RESOURCE:
    if (!CluManDll->os202_OfflineSERVERDBResource( OptionArgument ))
        return false;
    break;
  case OPTION_ADD_SERVERDB_RESOURCE    :
    {
      teo80_SeparateArgs    SeparateArgs(OptionArgument, _T(','));
      const TCHAR*          ServerDB = SeparateArgs.NextPart();
      const TCHAR*          Remark   = SeparateArgs.NextPart();

      if (!CluManDll->os202_AddSERVERDBResource( ServerDB, Remark ))
          return false;
    }
    break;
  case OPTION_REMOVE_SERVERD_BRESOURCE :
    if (!CluManDll->os202_RemoveSERVERDBResource( OptionArgument, 60 )) // PTS 1129537
        return false;
    break;
  case OPTION_ENABLE_CLUSTERLOG        :
    {
      teo80_SeparateArgs    SeparateArgs(OptionArgument, _T(','));
      const TCHAR*          MBLogSize  = SeparateArgs.NextPart();
      const TCHAR*          LogFile    = SeparateArgs.NextPart();

      if (( MBLogSize  != NULL ) &&
          ( SeparateArgs.NextPart() == NULL ))
      {
        if (!CluManDll->os202_EnableDisableClusterLog( _ttoi(MBLogSize), LogFile ))
            return false;
      }
      else
        eo80_vHelp ( NULL );
    }
    break;
  case OPTION_DISABLE_CLUSTERLOG       :
    if (!CluManDll->os202_EnableDisableClusterLog( 0, NULL )) // PTS 1129537
        return false;
    break;
  case OPTION_REGISTER_DEBUGEX         :
    if (!CluManDll->os202_RegisterUnregisterDebugEx( true ))
        return false;
    break;
  case OPTION_UNREGISTER_DEBUGEX       :
    if (!CluManDll->os202_RegisterUnregisterDebugEx( false ))
        return false;
    break;

  default:
    LPTSTR Msg = (LPTSTR)_alloca(40);
    _stprintf( Msg, _T("INTERNAL ERROR: Unknown option index '%d'\n"), OptionIndex );
    if (( EvtOptions & EVTOPT_DIALOG_BOX_OS202 ) != 0 )
      MessageBox( HWND_DESKTOP, Msg, PRODUCT_GROUP_NAME, MB_OK | MB_ICONEXCLAMATION);
    else
      _tprintf ( Msg  );

    Ok = false;
    break;
  }

  return Ok;
}



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/