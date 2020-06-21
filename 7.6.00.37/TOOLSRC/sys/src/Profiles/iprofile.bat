@echo off
cls

set TITLE=<FORMATED_BUILD_VERSION> SAP DB Developer Studio (<LC_STATE>)
if "%LOW_PRIO%." == "." goto NOLOWTITLE
set TITLE=%TITLE% (low prio)
:NOLOWTITLE
title %TITLE%
echo Welcome to %TITLE% !
echo.

rem --------------- Variablen initialisieren -----------------------
rem OWN is in the environment!
set OWN=<OWN>

rem ------------ execute before profile ----------------
if exist %OWN%\ipreprof.bat call %OWN%\ipreprof.bat

rem set VMAKE_PATH=%OWN%,%GRP%
set VMAKE_PATH=<VMAKE_PATH>
set VMAKE_OPTION=aDISOhvWMLRgbX

set RELVER=<RELVER>
set BUILDPRAEFIX=<BUILD_PREFIX>
set CORRECTION_LEVEL=<CORRECTION_LEVEL>
set SAPDB_PURPOSE=<SAPDB_PURPOSE>
set LC_STATE=<LC_STATE>
set RELSTAT=<RELSTAT>
set PATCH_DATE=<PATCH_DATE>
set DBROOT=%OWN%\usr
set DESC=%OWN%\sys\desc
set INSTROOT=%OWN%\usr
set SAPDB_NO_INDEPPATH=y
set TEST=%OWN%\test
if %TOOL%. == . set TOOL=D:\DevTool
if %PERL%. == . set PERL=%TOOL%\Perl
rem Extention der Tools (vmake-relevant)
set TOOLEXT=.pl
set SAPDB_INTERNAL=YES
set JTEST_ROOT=%OWN%\test\jtest
set ANT_HOME=%TOOL%\ant

set BACKUP=y
if  "%INITIAL_CLASSPATH%." == "." set INITIAL_CLASSPATH=%CLASSPATH%
set CLASSPATH=%INITIAL_CLASSPATH%
set CLASSPATH=%CLASSPATH%;%DBROOT%\runtime\jar\sapdbc.jar;%OWN%\test\jtest\frame\testframe.jar;%OWN%\test\jtest\tests\lctest.jar
set CLASSPATH=%CLASSPATH%;%OWN%\test\jtest\tests\oltptest.jar;%OWN%\test\jtest\frame\Lib.jar
set CPU=i386
rem %EDITOR% wird als Standard-Editor verwendet
if %EDITOR%. ==  . set EDITOR=msdev.exe
if  "%INITIAL_INCLUDE%." == "." set INITIAL_INCLUDE=%INCLUDE%
set INCLUDE=%INITIAL_INCLUDE%
set INCLUDE=%DBROOT%\incl;%TOOL%\incl;%OWN%\sys\wrk\incl;%OWN%\sys\wrk\incl\SAPDB;%INCLUDE%
set LIB=%DBROOT%\lib;%TOOL%\lib;%LIB%
set MGET=
set MUT=%TEST%\port\mut
set OSBASE=%OS%
set CODECHECK=OFF
set OCCINCL=%DBROOT%\incl;%TOOL%\incl;%OWN%\sys\wrk\incl;
set OWNINCS=%OCCINCL%
if  not "%INITIAL_PATH%." == "." goto SETPATH
set INITIAL_PATH=%PATH%
:SETPATH
set PATH=%INITIAL_PATH%
set PATH=.;%TOOL%\bin;%TOOL%\Posix;%TOOL%\pgm;%DBROOT%\bin;%DBROOT%\pgm;%TOOL%\Perl\bin;%ANT_HOME%\bin;%PATH%
if  "%INITIAL_PATHEXT%." == "." set INITIAL_PATHEXT=%PATHEXT%
set PATHEXT=%INITIAL_PATHEXT%
set PATHEXT=%PATHEXT%;%TOOLEXT%;.py
set PERL5LIB=%INSTROOT%\misc;%TOOL%\bin;%TOOL%\Lib\perl5;%TOOL%\Lib\Perl
if %PYTHON%. ==  . set PYTHON=%TOOL%\Python
set PATH=%PATH%;%PYTHON%
set PYTHONPATH=%INSTROOT%\runtime\python;%TOOL%\Lib\Python;%OWN%\usr\misc;%INSTROOT%\lib
set POSIX=%TOOL%\Posix
rem %REMCL% (relevant für die Library ICopy)
set REMCL=%TOOL%\bin\_rem.exe
set SERVERDB=
rem SHELL= äquiv. SHELL=cmd.exe
rem Shell in der Kommandos aus Descriptions (via ! oder !!) ausgeführt werden (vmake-relevant)
set SHELL=
set SRC=%OWN%\sys\src
set SUT=%TEST%\kern\sut
set TMP=%OWN%\tmp
set TOOLSHELL=%PERL%\bin\perl.exe
set TOOLVARS=%TOOL%\bin\toolvars%TOOLEXT%
set WRK=%OWN%\sys\wrk

&if $MACH == AMD64
set MACHINE_CONFIG=NTamd64
set BIT64=1
&elif defined BIT64
set MACHINE_CONFIG=NTia64
set BIT64=1
&else
set MACHINE_CONFIG=NTintel
set BIT64=
&endif

if "%RELSTAT%." == "PROD." set LC_POOL_ROOT=\\production\SCM\MaxDB\LC_POOL\%MACHINE_CONFIG%

rem --------------- Special conventions for Itanium Cross Development
&if $MACH == I386
&if not defined BIT64
if exist %OWN%\X64.dat (
&endif
    set CPU=IA64
    set BIT64=1
    if "%PROCESSOR_ARCHITECTURE%" == "x86" set XCOMP=1
    set OSVersion=/SRV64
&if not defined BIT64    
)
&endif
&endif

set CMD_OUTPUT_LENGTH=0

if "%1" == "-l" title %TITLE% (lowprio)

rem --------------------------------------------------------------
rem -------------------- nützliche Macros ------------------------

rem MACRO ils :
rem Dieses Macro ermöglicht den Output von ils zu pipen
doskey ils=perl -S ils.pl $*

rem MACRO istat :
rem Dieses Macro ermöglicht den Output von istat zu pipen
doskey istat=perl -S istat.pl $*

rem MACRO ev : zeigt Variablen (Prozentzeichen können entfallen)
doskey ev=echo %%$1%%

doskey iv   = iview.pl $*

doskey ..   = cd ..\$1*
doskey .    = popd
doskey export = set $1
doskey unset = set $1=

if not %OWN%.==. doskey own  = cd /d %OWN%\$1*
if not %GRP%.==. doskey grp  = pushd %GRP%\$*
if not %ALL%.==. doskey all  = pushd %ALL%\$*
if not %REL%.==. doskey rel  = pushd %REL%\$*

if not %SRC%.==. doskey src  = cd /d %SRC%\$1*
if not %WRK%.==. doskey wrk  = cd /d %WRK%\$1*
if not %DESC%.==. doskey desc = cd /d %DESC%\$1*
if not %SUT%.==. doskey sut  = cd /d %SUT%\$1*
if not %MUT%.==. doskey mut  = cd /d %MUT%\$1*
if not %INSTROOT%.==. doskey instroot=cd /d %INSTROOT%\$1*
if not %TOOL%.==. doskey tool = cd /d %TOOL%\$1*
if not %TOOL%.==. doskey tbin = cd /d %TOOL%\bin\$1*

doskey fast = cd /d fast\$1*
doskey quick= cd /d quick\$1*
doskey slow = cd /d slow\$1*

rem ------------ benutzerdefinierte Konfiguration ----------------
&if defined USE_SDK
set NO_VC_NET=yes
set USE_SDK=yes
&endif

rem ------------ releasespezifische Konfiguration ----------------
if exist %OWN%\irelease.bat call %OWN%\irelease.bat

set _VMAKE_OPTION=%VMAKE_OPTION%&set _VMAKE_PATH=%VMAKE_PATH%&set _TOOLVARS=%TOOLVARS%

rem ------------ machinenspezifische Konfiguration
if "%TOOL_ROOT%." == "." set TOOL_ROOT=%TOOL%
if exist %TOOL_ROOT%\imach.bat call %TOOL_ROOT%\imach.bat

rem ------------ lokale Konfiguration
if exist %OWN%\iuser.bat call %OWN%\iuser.bat
rem --------------------------------------------------------------
&if defined USE_SDK
rem ------------ Microsoft Visual Studio .NET configuration ---------------------

if not "%NO_VC_NET%." == "." goto VS_NET_END

if not "%REQUIRED_VC_VERSION%." == "." goto VCVER%REQUIRED_VC_VERSION%

rem --- Don't use VC.NET 2003 for releases smaller than 76:
if "%RELVER%" lss "R76" goto VCVER1300
if "%RELVER%" lss "R78" goto VCVER1310

:VS_NET_START

:VCVER1400
if "%VCVER1400%" == "checked" goto VS_NET_TEST_END

if not exist "C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat" GOTO VCVER1400_NEXT1
set VCINSTALLDIR=C:\Program Files\Microsoft Visual Studio 8
GOTO VS_NET_OK

:VCVER1400_NEXT1
if not exist "D:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat" GOTO VCVER1400_NEXT2
set VCINSTALLDIR=D:\Program Files\Microsoft Visual Studio 8
GOTO VS_NET_OK

:VCVER1400_NEXT2
if not exist "E:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat" GOTO VCVER1400_END
set VCINSTALLDIR=E:\Program Files\Microsoft Visual Studio 8
GOTO VS_NET_OK

:VCVER1400_END
set VCVER1400=checked
if "%RELSTAT%" == "DEV" goto VS_NET_TEST_END
if "%RELSTAT%" == "PROD" goto VS_NET_TEST_END
echo.
echo Info: Can't find required compiler (VC 8). Look for an other compiler ...
echo.

:VCVER1310
if "%VCVER1310%" == "checked" goto VS_NET_TEST_END

if not exist "c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat" GOTO VCVER1310_NEXT1
set VCINSTALLDIR=c:\Program Files\Microsoft Visual Studio .NET 2003
GOTO VS_NET_OK

:VCVER1310_NEXT1
if not exist "d:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat" GOTO VCVER1310_NEXT2
set VCINSTALLDIR=d:\Program Files\Microsoft Visual Studio .NET 2003
GOTO VS_NET_OK

:VCVER1310_NEXT2
if not exist "e:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat" GOTO VCVER1310_END
set VCINSTALLDIR=e:\Program Files\Microsoft Visual Studio .NET 2003
GOTO VS_NET_OK

:VCVER1310_END
set VCVER1310=checked
if "%RELSTAT%" == "DEV" goto VS_NET_TEST_END
if "%RELSTAT%" == "PROD" goto VS_NET_TEST_END
echo.
echo Info: Can't find required compiler (.NET 2003). Look for an other compiler ...
echo.

:VCVER1300
if "%VCVER1300%" == "checked" goto VS_NET_TEST_END

if not exist "c:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat" GOTO VCVER1300_NEXT1
set VCINSTALLDIR=c:\Program Files\Microsoft Visual Studio .NET
GOTO VS_NET_OK

:VCVER1300_NEXT1
if not exist "d:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat" GOTO VCVER1300_NEXT2
set VCINSTALLDIR=d:\Program Files\Microsoft Visual Studio .NET
GOTO VS_NET_OK

:VCVER1300_NEXT2
if not exist "e:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat" GOTO VCVER1300_END
set VCINSTALLDIR=e:\Program Files\Microsoft Visual Studio .NET
GOTO VS_NET_OK

:VCVER1300_END
set VCVER1300=checked
if "%RELSTAT%" == "DEV" goto VS_NET_TEST_END
if "%RELSTAT%" == "PROD" goto VS_NET_TEST_END
echo.
echo Info: Can't find required compiler (.NET). Look for an other compiler ...
echo. 
goto VS_NET_START


:VS_NET_TEST_END
if not "%VCINSTALLDIR%." == "." goto VS_NET_OK
echo ---- ! No Microsoft Visual Studio .NET detected    ! ----
pause
GOTO FORCE_SDK

:VS_NET_OK
if exist "%VCINSTALLDIR%\Common7\Tools\vsvars32.bat" GOTO VS_NET_SET
    echo ---- ! vsvars32.bat was not detected in %VCINSTALLDIR%\Common7\Tools   ! ----
    pause
    exit
:VS_NET_SET
@echo Following Microsoft Compiler is used :
@echo.
call "%VCINSTALLDIR%\Common7\Tools\vsvars32.bat"
@echo.
@echo .NET root path: %VCINSTALLDIR%
:VS_NET_END

rem --------------------------------------------------------------

if "%USE_SDK%." == "." goto SDK_END

:FORCE_SDK
&endif
rem ------------ Microsoft SDK Konfiguration ---------------------
if not "%MSSdk%." == "." goto MSSDK_OK
if exist "c:\Program Files\Microsoft Platform SDK" set MSSdk=c:\Program Files\Microsoft Platform SDK
if exist "c:\Program Files\Microsoft SDK" set MSSdk=c:\Program Files\Microsoft SDK

if not "%MSSdk%." == "." goto MSSDK_OK
echo ---- ! No Microsoft SDK detected    ! ----
pause
exit

:MSSDK_OK
if not exist "%MSSdk%\SetEnv.bat" (
    echo ---- ! SetEnv.bat was not detected in %MSSdk%   ! ----
    pause
    exit
)
echo ---- ! Microsoft SDK is used   ! ----


rem echo %MSSdk%\SetEnv.bat %OSVersion% /RETAIL
set MYINCLUDE=%INCLUDE%
set MYLIB=%LIB%
call "%MSSdk%\SetEnv.bat" %OSVersion% /RETAIL
&if defined USE_SDK
rem if not "%RESTORE_INCL%. == "." goto NO_RESTORE_INCL
if  "%BIT64%." == "." GOTO NO_RESTORE_INCL
set INCLUDE=%INCLUDE%;%MYINCLUDE%
set LIB=%LIB%;%MYLIB%
:NO_RESTORE_INCL
title %TITLE%
:SDK_END
&else
set INCLUDE=%INCLUDE%;%MYINCLUDE%
set LIB=%LIB%;%MYLIB%
title %TITLE%
&endif


rem ------------ check startup parameter -------------------------
if "%_VMAKE_OPTION%" == "%VMAKE_OPTION%" goto OPTIONS_OK
echo ---- ! caution: manipulated VMAKE_OPTION  ! ----
:OPTIONS_OK
if "%_VMAKE_PATH%" == "%VMAKE_PATH%" goto PATH_OK
echo ---- ! caution: manipulated VMAKE_PATH    ! ----
:PATH_OK
if "%_TOOLVARS%" == "%TOOLVARS%" goto TOOLVARS_OK
echo ---- ! caution: manipulated TOOLVARS     ! ----
:TOOLVARS_OK
set _VMAKE_OPTION=&set _VMAKE_PATH=&set _TOOLVARS=

if not %OWN%. == . goto OWN_OK
echo ---- ! ERROR: OWN not set ! ----
pause
exit
:OWN_OK

if not %RELVER%. == . goto RELVER_OK
echo ---- ! ERROR: RELVER not set ! ----
pause
exit
:RELVER_OK

if  exist %SHELL% goto SHELL_OK
if  %SHELL%. == . goto SHELL_OK
echo ---- ! WARNING: SHELL do'nt exists ! ----
pause
exit
:SHELL_OK

rem Shell die die Tools ausführt (vmake-relevant)
if  exist %TOOLSHELL% goto TOOLSHELL_OK
if  not %TOOLSHELL%. == . goto TOOLSHELL_OK
echo ---- ! WARNING: TOOLSHELL don't exists ! ----
pause
exit
:TOOLSHELL_OK

set GLOBAL_PROFILE=1

cd /d %OWN%
@echo.
@echo %%OWN%%: %OWN%
@echo %%GRP%%: %GRP%
@echo %%ALL%%: %ALL%
@echo %%REL%%: %REL%
@echo.
@echo %%VMAKE_PATH%%: %VMAKE_PATH%
@echo.
