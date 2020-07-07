/*!
  @file           vos80kn.c
  @author         RaymondR
  @brief          NT Kernel Main
  @see            

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




//
// INCLUDE FILES
//
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo002.h"
#include "gos003.h"
#include "geo007_2.h"
#include "gos00k.h"
#include "RunTime/RTE_DiagPipe.h"

#include <windowsx.h>
#include <shellapi.h>

//
//  DEFINES
//

#define MOD__  "VOS80KNC : "
#define MF__   MOD__"UNDEFINED"

#define DLG_KERNEL          100
#define ID_DIALOG_BASE      101
#define ID_CONSOLE_INFO     102
#define ID_CONSOLE          103
#define ID_CLEAR_ALL        104
#define ID_DBSTATE_FRAME    105
#define ID_DBSTATE          106
#define ID_DBSTATE_ICON     107

#define ICON_KERNEL         130
#define ICON_WARM           131
#define ICON_COLD           132
#define ICON_STOPPED        133
#define ICON_TRAFFIC        134

#define IDD_MENU_TRAYPOPUP  200
#define IDM_RESTORE         201
#define IDM_HIDE            202
#define IDM_CLOSE           203

#define DLG_SHUTDOWN        300


#define SM_ADDENTRY         (WM_USER + 100)
#define SM_SET_DBSTATE      (WM_USER + 101)
#define SM_TRAY_CALLBACK    (WM_USER + 102)
#define SM_TRAY_POPUP       (WM_USER + 103)

#define MAX_CONS_MESS       100

#if defined (_WIN64)
# undef  GetWindowLong
# define GetWindowLong      GetWindowLongPtr

# undef  SetClassLong
# define SetClassLong       SetClassLongPtr

# undef  SetWindowLong
# define SetWindowLong      SetWindowLongPtr


# define  NEW_VALUE         LONG_PTR
#else
# define  NEW_VALUE         LONG
#endif

#if !defined (GWLP_HINSTANCE)
# define GWLP_HINSTANCE  GWL_HINSTANCE
#endif
#if !defined (GWLP_WNDPROC)
# define GWLP_WNDPROC    GWL_WNDPROC
#endif
#if !defined (GCLP_HICON)
# define GCLP_HICON      GWLP_WNDPROC
#endif

//
//  MACROS
//
#define HANDLE_SM_ADDENTRY(hwnd, wParam, lParam, fn)                          \
             ((fn)((hwnd), (PSZ)(wParam)), TRUE)
#define HANDLE_SM_SET_DBSTATE(hwnd, wParam, lParam, fn)                       \
             ((fn)(hwnd), TRUE)
#define HANDLE_SM_TRAY_CALLBACK(hwnd, wParam, lParam, fn)                     \
             ((fn)((hwnd), (UINT)(wParam), (UINT)(lParam)), TRUE)
#define HANDLE_SM_TRAY_POPUP(hwnd, wParam, lParam, fn)                        \
             ((fn)(hwnd), TRUE)


#define EXIT_DELAY        3000

//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//

typedef struct
          {
          HWND           hwndDialog;
          HWND           hwndDialogBase;
          HWND           hwndConsole;
          HWND           hwndConsoleInfo;
          HWND           hwndClearAll;
          HWND           hwndDBStateFrame;
          HWND           hwndDBState;
          HWND           hwndDBStateIcon;
          HDC            hDC;
          } DLG_HWNDS;


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//
static PPROG_ARGS   pProgArgs;
static ULONG        ulArgc;
static DLG_HWNDS    DlgHandles;
static HWND         hwndConsole;
static RECT         rectConsoleDelta;
static INT          cxOld;
static INT          cyOld;
static ULONG        ulConsMessages;
static WNDPROC      wpOrigIconProc;
static WNDPROC      wpOrigDlgProc;
static HICON        hColdIcon;
static HICON        hWarmIcon;
static HICON        hStoppedIcon;
static HICON        hTrafficIcon;

//
//  FUNCTION PROTOTYPES
//



//
//  LOCAL FUNCTION PROTOTYPES
//
LRESULT  CALLBACK sql80kn_dlg_proc ( HWND   hDlg,
                                     UINT   uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam );

LONG      sql80kn_write_msg        ( PSZ   pszMsg );
//
// ========================== GLOBAL FUNCTIONS ================================
//

INT sql80kn_CreateWindow ( HINSTANCE   hInstance,
                           HINSTANCE   hPrevInstance,
                           PPROG_ARGS  pArgs,
                           INT         nCmdShow )
  {
  pProgArgs = pArgs;

  sql60_enable_message_box ();
  sql60_enable_GUI_console ( sql80kn_write_msg );

  if ( DialogBox( hInstance, MAKEINTRESOURCE(DLG_KERNEL),
                  HWND_DESKTOP, sql80kn_dlg_proc ) == -1 )
    {
    MSGBOX(( ERR_CREATE_CONSOLE_WINDOW, GetLastError() ));
    EXITPROCESS( GetLastError() );
    }

  sql60_disable_GUI_console();

  // --- wake up coordinator thread
  if (sql81k_wakeup_corrdinator ())
    SleepEx ( 10000, FALSE );


  return(0);
  }

/*------------------------------*/

LONG sql80kn_write_msg ( PSZ pszMsg )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_write_msg"
  PCHAR   pchBuffer = NULL;
  APIRET  rc;
  PCHAR   pchTmp1;
  PCHAR   pchTmp2;

  DBGPAS;

  rc = ALLOC_MEM( (PVOID*)&pchBuffer, (DWORD)strlen(pszMsg) );

  if ( rc != NO_ERROR )
    {
    MSGBOX(( ERR_ALLOC_MEMORY, rc ));
    }
  else
    {
    pchTmp1      = pszMsg;
    pchBuffer[0] = '\0';

    while ( (pchTmp2 = strchr(pchTmp1, '\r')) != NULL )
      {
      pchTmp2[0] = '\0';
      strcat ( pchBuffer, pchTmp1 );
      pchTmp2[0] = '\r';

      if ( pchTmp2[1] == '\n' )
        pchTmp1 = pchTmp2 + 2;
      else
        pchTmp1 = pchTmp2 + 1;
      }

    strcat ( pchBuffer, pchTmp1 );

    pchTmp1 = pchBuffer;

    while ( (pchTmp2 = strchr(pchTmp1, '\t')) != NULL )
      {
      pchTmp2[0] = ' ';
      pchTmp1    = pchTmp2 + 1;
      }

    PostMessage( DlgHandles.hwndDialog, SM_ADDENTRY, (WPARAM)pchBuffer,
                 (LPARAM)strlen(pchBuffer) );
    }


  return ( NO_ERROR );
  }

/*------------------------------*/

VOID sql80kn_set_db_state ( SAPDB_UInt4 ulState )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_set_db_state"

  DBGPAS;

  PostMessage( DlgHandles.hwndDialog, SM_SET_DBSTATE, (WPARAM)0,
               (LPARAM)0);

  return;
  }

//
// ========================== LOCAL FUNCTIONS =================================
//

static VOID sql80kn_dlg_tray_message( HWND hWnd, UINT message)
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_tray_message"

  NOTIFYICONDATA tnd;
  HICON          hIcon;

  DBGIN;


  // --- Change the state of the small icon in the task bar
  switch ( *kgs.pDBState )
    {
    case 0 :
      hIcon = hTrafficIcon;
      if ( kgs.szServerDB[0] != '\0' )
        wsprintf(tnd.szTip, KERNEL_HINT_SERVERDB_STARTING, kgs.szServerDB );
      else
        wsprintf(tnd.szTip, KERNEL_HINT_STARTING );
      break;
    case SERVER_COLD:
      hIcon = hColdIcon;
      if ( kgs.szServerDB[0] != '\0' )
        wsprintf(tnd.szTip, KERNEL_HINT_SERVERDB_COLD,
                 kgs.szServerDB, kgs.ulActiveUserTasks );
      else
        wsprintf(tnd.szTip, KERNEL_HINT_COLD,
                 kgs.ulActiveUserTasks );
      break;
    case SERVER_WARM:
      hIcon = hWarmIcon;
      if ( kgs.szServerDB[0] != '\0' )
        wsprintf(tnd.szTip, KERNEL_HINT_SERVERDB_WARM,
                 kgs.szServerDB, kgs.ulActiveUserTasks );
      else
        wsprintf(tnd.szTip, KERNEL_HINT_WARM,
                 kgs.ulActiveUserTasks );
      break;
    default:
      hIcon = hStoppedIcon;
      if ( kgs.szServerDB[0] != '\0' )
        wsprintf(tnd.szTip, KERNEL_HINT_SERVERDB,
                 kgs.szServerDB, kgs.ulActiveUserTasks );
      break;
    }

  switch (message)
    {
    case NIM_ADD:
      tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
      break;

    case NIM_MODIFY:
      tnd.uFlags = NIF_TIP|NIF_ICON;
      break;

    case NIM_DELETE:
      tnd.uFlags = 0;
      break;
    }

  tnd.uID              = (UINT)ICON_KERNEL;
  tnd.cbSize           = sizeof(NOTIFYICONDATA);
  tnd.hWnd             = hWnd;
  tnd.uCallbackMessage = SM_TRAY_CALLBACK;
  tnd.hIcon            = hIcon;
  strcpy(tnd.szTip, "ServerDB");
  if ( kgs.szServerDB[0] != '\0' )
  {
      strcat(tnd.szTip, ": ");
      strcat(tnd.szTip, kgs.szServerDB );
  }

  Shell_NotifyIcon(message, &tnd);

  DBGOUT
  return;
  }


/*------------------------------*/

static VOID sql80kn_dlg_tray_callback( HWND  hwnd,
                                       UINT  uID,
                                       UINT  uMouseMsg )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_tray_callback"

  DBGIN;

  if (uID == (UINT)ICON_KERNEL)
    {
    if (uMouseMsg == WM_LBUTTONDOWN)
      {
      if(IsIconic(hwnd))
        SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE | HTCLIENT, 0L);
      else
        {
        ShowWindow( GetParent(hwnd), SW_HIDE );
        SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE | HTCLIENT, 0L);
        }
      }
    else if (uMouseMsg == WM_RBUTTONDOWN)
      SendMessage(hwnd, SM_TRAY_POPUP, 0L, 0L);
    }

  DBGOUT
  return;
  }


/*------------------------------*/

static VOID sql80kn_dlg_tray_popup( HWND  hwnd )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_tray_popup"
  HMENU           hMenu;
  HMENU           hSubMenu;
  POINT           ptScreen;

  DBGIN;


  if(hMenu = LoadMenu((HINSTANCE) GetWindowLong(hwnd, GWLP_HINSTANCE),
                      MAKEINTRESOURCE(IDD_MENU_TRAYPOPUP)))
    {
    GetCursorPos(&ptScreen);
    hSubMenu = GetSubMenu(hMenu, 0);

    if(IsIconic(hwnd))
      {
      EnableMenuItem(hSubMenu, IDM_HIDE,    MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hSubMenu, IDM_RESTORE, MF_BYCOMMAND | MF_ENABLED);
      }
    else
      {
      EnableMenuItem(hSubMenu, IDM_HIDE,    MF_BYCOMMAND | MF_ENABLED);
      EnableMenuItem(hSubMenu, IDM_RESTORE, MF_BYCOMMAND | MF_GRAYED);
      }

    // Pop up a menu
    SetForegroundWindow( hwnd );
    TrackPopupMenuEx( hSubMenu, TPM_VERTICAL,
                      ptScreen.x, ptScreen.y, hwnd, NULL );
    DestroyMenu(hMenu);
    }
  else
    MessageBeep(0);

  DBGOUT
  return;
  }

/*------------------------------*/

static VOID _System sql80kn_coordinator_thread ( PPROG_ARGS  pArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_coordinator_thread"
  APIRET   rc = NO_ERROR;
  HANDLE   hProcess;

  DBGPAS;

  strcpy( kgs.szServiceName,     SERVICE_ID);
  strcat( kgs.szServiceName,     pArgs->ppszVal[0] );

  RTE_ConnectToDiagPipe(pArgs->ppszVal[0]);
  rc = sql81k_Coordinator ( pArgs->ppszVal[0],
                            pArgs->ulCount - 1,
                            pArgs->ppszVal + 1 );


  sql80kn_set_db_state ( *kgs.pDBState );

  sql80kn_dlg_tray_message(DlgHandles.hwndDialog, NIM_DELETE);

  SleepEx(EXIT_DELAY, FALSE); 

  if ( DuplicateHandle( GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(),
                        &hProcess, DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS ))
    TerminateProcess ( hProcess, rc );
  else
    ExitProcess ( rc );
  }

/*------------------------------*/

static LRESULT APIENTRY  sql80kn_icon_subclass_proc ( HWND   hwnd,
                                                      UINT   uMsg,
                                                      WPARAM wParam,
                                                      LPARAM lParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_icon_subclass_proc"
  HICON          hIcon;
  HDC            hdc;
  LRESULT        rc;

  DBGIN;

  switch (uMsg)
    {
    case WM_PAINT:
      rc = CallWindowProc( wpOrigIconProc, hwnd, uMsg,
                           wParam, lParam );

      switch ( *kgs.pDBState )
        {
        case SERVER_COLD:
          hIcon = hColdIcon;
          break;
        case SERVER_WARM:
          hIcon = hWarmIcon;
          break;
        case 0 :
        case SERVER_STARTING :
          hIcon = hTrafficIcon;
        default:
          hIcon = hStoppedIcon;
          break;
        }

      hdc = GetDC(hwnd);
      DrawIcon( hdc, 0,0, hIcon );
      ReleaseDC(hwnd, hdc);

      DBGOUT;
      return ( rc );
    }

  DBGOUT;
  return CallWindowProc( wpOrigIconProc, hwnd, uMsg,
                         wParam, lParam );
  }

/*------------------------------*/

static LRESULT APIENTRY  sql80kn_dlg_subclass_proc ( HWND   hwnd,
                                                     UINT   uMsg,
                                                     WPARAM wParam,
                                                     LPARAM lParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_subclass_proc"
  LRESULT        rc;
  ULONG          ulShutTimeOut = 600;

  DBGIN;

  switch (uMsg)
    {
    case WM_SIZE:
      if ( wParam == SIZE_MINIMIZED )
        ShowWindow( hwnd, SW_HIDE );
      else if ( wParam == SIZE_RESTORED )
        {
        ShowWindow( hwnd, SW_SHOWNORMAL );
        SetForegroundWindow( hwnd );
        }

      rc = CallWindowProc( wpOrigDlgProc, hwnd, uMsg,
                           wParam, lParam );

      DBGOUT;
      return ( rc );

    case WM_PAINT:

      if(IsIconic(hwnd))
        ShowWindow( hwnd, SW_HIDE );

      rc = CallWindowProc( wpOrigDlgProc, hwnd, uMsg,
                           wParam, lParam );

      DBGOUT;
      return ( rc );

    case WM_QUERYENDSESSION :

      rc = CallWindowProc( wpOrigDlgProc, hwnd, uMsg,
                           wParam, lParam );

      sql81k_wakeup_corrdinator ();
 
      do {
          SleepEx(1000, FALSE); 
      } while (( --ulShutTimeOut ) && ( *kgs.pDBState != SERVER_STOPPED ));

      SleepEx(EXIT_DELAY, FALSE); 

      DBGOUT;
      return ( 1 );

    }

  DBGOUT;
  return CallWindowProc( wpOrigDlgProc, hwnd, uMsg,
                         wParam, lParam );
  }

/*------------------------------*/

static BOOL sql80kn_dlg_init ( HWND   hwnd,
                               HWND   hwndFocus,
                               LPARAM lParam)
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_init"
  APIRET rc;
  CHAR   szTitle[30];
  RECT   rectTmp;
  RECT   rectDialog;
  RECT   rectConsole;
  RECT   rectDialogBase;
  HFONT  Font;

  DBGIN;

  if ( hColdIcon == 0 )
    {
    hColdIcon    = LoadIcon( (HINSTANCE) GetWindowLong(hwnd, GWLP_HINSTANCE),
                              MAKEINTRESOURCE(ICON_COLD) );
    hWarmIcon    = LoadIcon( (HINSTANCE) GetWindowLong(hwnd, GWLP_HINSTANCE),
                              MAKEINTRESOURCE(ICON_WARM) );
    hStoppedIcon = LoadIcon( (HINSTANCE) GetWindowLong(hwnd, GWLP_HINSTANCE),
                              MAKEINTRESOURCE(ICON_STOPPED) );

    hTrafficIcon = LoadIcon( (HINSTANCE) GetWindowLong(hwnd, GWLP_HINSTANCE),
                              MAKEINTRESOURCE(ICON_TRAFFIC) );
    }

  DlgHandles.hwndDialog        = hwnd;
  DlgHandles.hwndDialogBase    = GetDlgItem(hwnd, ID_DIALOG_BASE);
  DlgHandles.hwndConsole       = GetDlgItem(hwnd, ID_CONSOLE);
  DlgHandles.hwndConsoleInfo   = GetDlgItem(hwnd, ID_CONSOLE_INFO );
  DlgHandles.hwndClearAll      = GetDlgItem(hwnd, ID_CLEAR_ALL);
  DlgHandles.hwndDBStateFrame  = GetDlgItem(hwnd, ID_DBSTATE_FRAME);
  DlgHandles.hwndDBState       = GetDlgItem(hwnd, ID_DBSTATE);
  DlgHandles.hwndDBStateIcon   = GetDlgItem(hwnd, ID_DBSTATE_ICON);
  DlgHandles.hDC               = CreateCompatibleDC(NULL);

  Font = SelectObject( DlgHandles.hDC,
                       (HFONT)SendMessage(DlgHandles.hwndConsole,
                                          WM_GETFONT, 0, 0));

  ListBox_ResetContent( DlgHandles.hwndConsole );
  EnableWindow ( DlgHandles.hwndClearAll, FALSE );

  strcpy ( szTitle, KERNEL_APPLIC_NAME );
  strcat ( szTitle, pProgArgs->ppszVal[0] );

  SetWindowText ( hwnd, szTitle );

  // Associate an icon with the dialog box.
  SetClassLong( hwnd, GCLP_HICON, (NEW_VALUE)
                LoadIcon( (HINSTANCE) GetWindowLong(hwnd, GWLP_HINSTANCE),
                          MAKEINTRESOURCE(ICON_KERNEL) ));

  wpOrigIconProc = (WNDPROC)SetWindowLong(DlgHandles.hwndDBStateIcon,
                                          GWLP_WNDPROC,
                                          (NEW_VALUE) sql80kn_icon_subclass_proc );

  wpOrigDlgProc  = (WNDPROC)SetWindowLong(DlgHandles.hwndDialog,
                                          GWLP_WNDPROC,
                                          (NEW_VALUE) sql80kn_dlg_subclass_proc );

  kgs.Coord.ThrdCtrlHeader.hThrd = CreateThread( NULL, KT_COORDINATOR_STACK_SIZE,
                                                 (LPTHREAD_START_ROUTINE)sql80kn_coordinator_thread,
                                                 (LPVOID)pProgArgs, CREATE_THRD_SUSPENDED, 
                                                 &kgs.Coord.ThrdCtrlHeader.Tid );

  if ( kgs.Coord.ThrdCtrlHeader.hThrd )
    {
    if ( ResumeThread(kgs.Coord.ThrdCtrlHeader.hThrd) == (DWORD)-1)
      rc = GetLastError();
    else
      rc = NO_ERROR;
    }
  else
    {
    kgs.Coord.ThrdCtrlHeader.hThrd = INVALID_HANDLE_VALUE;
    rc = GetLastError();
    }

  if (rc != NO_ERROR)
    {
    MSGBOX(( ERR_CREATING_THREAD, "Coordinator", rc ));
    EndDialog( hwnd, IDCANCEL );
    EXITPROCESS( rc );
    }

  GetClientRect ( hwnd, &rectTmp );

  cxOld = rectTmp.right;
  cyOld = rectTmp.bottom;

  GetWindowRect ( hwnd, &rectDialog );
  GetWindowRect ( DlgHandles.hwndConsole, &rectConsole );
  GetWindowRect ( DlgHandles.hwndDialogBase, &rectDialogBase );

  rectConsoleDelta.left   = rectConsole.left  - rectDialogBase.left;
  rectConsoleDelta.top    = rectConsole.top   - rectDialogBase.top;
  rectConsoleDelta.right  = rectDialog.right  - rectConsole.right;
  rectConsoleDelta.bottom = rectDialog.bottom - rectConsole.bottom;

  sql80kn_dlg_tray_message(hwnd, NIM_ADD);
  SetTimer(hwnd, 1, 2000, NULL);

  DBGOUT;
  return(TRUE);
  }

/*------------------------------*/

static VOID sql80kn_dlg_size ( HWND hwnd, UINT state, INT cx, INT cy)
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_size"
  RECT           rectDialogBase;
  RECT           rectTmp;
  static INT     xButtonPos     = -1;
  static BOOL    fConsoleHidden =  FALSE;

  DBGIN;

  if (( cx == 0 ) || ( cy == 0 ))
    {
    DBGOUT
    return;
    }

  // - get dialog base position
  GetWindowRect ( DlgHandles.hwndDialogBase, &rectDialogBase );

  //
  // - set console listbox
  //
  if ( cy - rectConsoleDelta.bottom - rectConsoleDelta.top <= 0 )
    {
    ShowWindow ( DlgHandles.hwndConsole, SW_HIDE );
    ShowWindow ( DlgHandles.hwndConsoleInfo, SW_HIDE );
    fConsoleHidden = TRUE;
    }
  else
    {
    SetWindowPos( DlgHandles.hwndConsole, NULL, 0, 0,
                  cx - rectConsoleDelta.right  - rectConsoleDelta.left,
                  cy - rectConsoleDelta.bottom - rectConsoleDelta.top,
                  SWP_NOZORDER | SWP_NOMOVE );

    if ( fConsoleHidden == TRUE )
      {
      ShowWindow  ( DlgHandles.hwndConsole, SW_SHOW );
      ShowWindow  ( DlgHandles.hwndConsoleInfo, SW_SHOW );
      fConsoleHidden = FALSE;
      }
    }

  //
  // - set state information objects
  //
  GetWindowRect ( DlgHandles.hwndDBStateFrame, &rectTmp );

  if ( xButtonPos == -1 )
    xButtonPos = rectTmp.right - rectDialogBase.left + 15;

  SetWindowPos  ( DlgHandles.hwndDBStateFrame, NULL,
                  rectTmp.left - rectDialogBase.left,
                  rectTmp.top  - rectDialogBase.top + (cy - cyOld),
                  0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_HIDEWINDOW );

  GetWindowRect ( DlgHandles.hwndDBState, &rectTmp );
  SetWindowPos  ( DlgHandles.hwndDBState, NULL,
                  rectTmp.left - rectDialogBase.left,
                  rectTmp.top  - rectDialogBase.top + (cy - cyOld),
                  0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_HIDEWINDOW );

  GetWindowRect ( DlgHandles.hwndDBStateIcon, &rectTmp );
  SetWindowPos  ( DlgHandles.hwndDBStateIcon, NULL,
                  rectTmp.left - rectDialogBase.left,
                  rectTmp.top  - rectDialogBase.top + (cy - cyOld),
                  0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_HIDEWINDOW );

  ShowWindow ( DlgHandles.hwndDBStateFrame, SW_SHOW );
  ShowWindow ( DlgHandles.hwndDBState,      SW_SHOW );
  ShowWindow ( DlgHandles.hwndDBStateIcon,  SW_SHOW );

  //
  // - set clear button
  //
  if ( cy - rectConsoleDelta.bottom - rectConsoleDelta.top <= 0 )
    {
    GetWindowRect ( DlgHandles.hwndClearAll, &rectTmp );

    SetWindowPos  ( DlgHandles.hwndClearAll, NULL,
                    max ( xButtonPos,
                          cx - rectConsoleDelta.right -
                          ( rectTmp.right - rectTmp.left )),
                    rectTmp.top  - rectDialogBase.top + (cy - cyOld),
                    0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_HIDEWINDOW  );
    }
  else
    {
    GetWindowRect ( DlgHandles.hwndClearAll, &rectTmp );

    SetWindowPos  ( DlgHandles.hwndClearAll, NULL,
                    max ( xButtonPos,
                          cx - rectConsoleDelta.right -
                          ( rectTmp.right - rectTmp.left )),
                    rectTmp.top  - rectDialogBase.top + (cy - cyOld),
                    0, 0, SWP_NOZORDER | SWP_NOSIZE );

    ShowWindow ( DlgHandles.hwndClearAll, SW_SHOW );
    }

  cyOld = cy;
  cxOld = cx;

  DBGOUT
  return;
  }

/*------------------------------*/

static VOID sql80kn_dlg_command ( HWND hwnd,    INT id,
                                  HWND hwndCtl, UINT codeNotify )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_command"

  DBGIN;

  switch (id)
    {
    case IDM_RESTORE:
      SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE | HTCLIENT, 0L);
      break;

    case IDM_HIDE:
      ShowWindow( GetParent(hwnd), SW_HIDE );
      SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE | HTCLIENT, 0L);
      break;

    case IDM_CLOSE:
    case IDCANCEL:
        if (( *kgs.pDBState <= SERVER_WARM ) &&
               ( MessageBox( hwnd, "\n"KERNEL_EXIT_CONSOLE,
                             KERNEL_QUESTION,
                             MB_YESNO | MB_ICONQUESTION ) == IDNO ))
        {
            break;
        }

        if (!sql81k_wakeup_corrdinator ())
        {
            EndDialog( hwnd,id );           // - coordinator not active!
            sql80kn_dlg_tray_message(hwnd, NIM_DELETE);
        }

      break;

    case ID_CLEAR_ALL:
      ListBox_ResetContent( DlgHandles.hwndConsole );
      EnableWindow ( DlgHandles.hwndClearAll, FALSE );
      UpdateWindow( DlgHandles.hwndConsole );
      ulConsMessages = 0;
      break;

    default:
      break;
    }

  DBGOUT
  return;
  }

/*------------------------------*/

static VOID sql80kn_dlg_set_dbstate ( HWND hwnd )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_set_dbstate"
  static BOOL fColdModeCaught = FALSE;

  DBGIN;

  PostMessage( DlgHandles.hwndDBStateIcon, WM_PAINT,
               (WPARAM)0, (LPARAM)0);

  if (( fColdModeCaught  == FALSE ) &&
      ( *kgs.pDBState    == SERVER_COLD ))
    {
    // - the database has switched to the cold mode,
    //   and it's doing it the first time
    fColdModeCaught = TRUE;
    ShowWindow( GetParent(hwnd), SW_HIDE );
    SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE | HTCLIENT, 0L);
    }
  if ( *kgs.pDBState >= SERVER_ABORT )
    {
    MessageBeep( MB_ICONEXCLAMATION );
    ShowWindow   ( hwnd, SW_RESTORE );
    SetWindowPos ( hwnd, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE );
    }

  sql80kn_dlg_tray_message(hwnd, NIM_MODIFY);

  DBGOUT
  return;
  }

/*------------------------------*/

static VOID sql80kn_dlg_add_entry ( HWND hwnd, PSZ pszMessage )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_add_entry"
  APIRET         rc;
  ULONG          ulCnt;
  ULONG          ulDelCnt;
  SIZE           Size;
  ULONG          ulMessageLen = (ULONG)strlen ( pszMessage );
  static LONG    lMaxHorExt   = 0;
  static LONG    lSpace       = 0;

  DBGIN;

  if ( lSpace == 0 )
    {
    GetTextExtentPoint( DlgHandles.hDC, " ", 1, &Size );
    lSpace = Size.cx;
    }

  if ( ulConsMessages >= MAX_CONS_MESS )
    {
    ulDelCnt        = ulConsMessages / 10;
    ulConsMessages -= ulDelCnt;

    for ( ulCnt = 0; ulCnt < ulDelCnt; ulCnt++ )
      ListBox_DeleteString( DlgHandles.hwndConsole, 0 );
    }

  ulConsMessages ++;
  if ( ulConsMessages == 1 )
    EnableWindow ( DlgHandles.hwndClearAll, TRUE );

  ListBox_AddString ( DlgHandles.hwndConsole, pszMessage );
  ListBox_SetCurSel ( DlgHandles.hwndConsole, ulConsMessages - 1 );
  ListBox_SetSel    ( DlgHandles.hwndConsole, FALSE, ulConsMessages - 1 );

  if ( GetTextExtentPoint( DlgHandles.hDC, pszMessage, ulMessageLen, &Size ))
    {
    if ( lMaxHorExt < Size.cx )
      {
      ListBox_SetHorizontalExtent( DlgHandles.hwndConsole,
                                   Size.cx + (3 * lSpace) );
      lMaxHorExt = Size.cx;
      }
    }

  rc = FREE_MEM( (PVOID)pszMessage );

  if ( rc != NO_ERROR )
    MessageBeep( MB_ICONHAND );

  DBGOUT
  return;
  }

/*------------------------------*/

LRESULT CALLBACK sql80kn_dlg_proc ( HWND   hDlg,
                                    UINT   uMsg,
                                    WPARAM wParam,
                                    LPARAM lParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql80kn_dlg_proc"
  BOOL fProcessed = TRUE;

  DBGIN;

  switch (uMsg)
    {
    HANDLE_MSG( hDlg, WM_INITDIALOG,       sql80kn_dlg_init );
    HANDLE_MSG( hDlg, WM_SIZE,             sql80kn_dlg_size );
    HANDLE_MSG( hDlg, WM_COMMAND,          sql80kn_dlg_command );
    HANDLE_MSG( hDlg, SM_ADDENTRY,         sql80kn_dlg_add_entry );
    HANDLE_MSG( hDlg, SM_SET_DBSTATE,      sql80kn_dlg_set_dbstate );
    HANDLE_MSG( hDlg, SM_TRAY_CALLBACK,    sql80kn_dlg_tray_callback );
    HANDLE_MSG( hDlg, SM_TRAY_POPUP,       sql80kn_dlg_tray_popup );
    case WM_TIMER:
      sql80kn_dlg_tray_message(hDlg, NIM_MODIFY);
      break;

    default:
      fProcessed = FALSE;
      break;
    }

  DBGOUT;
  return(fProcessed);
  }

//
// =============================== END ========================================
//
