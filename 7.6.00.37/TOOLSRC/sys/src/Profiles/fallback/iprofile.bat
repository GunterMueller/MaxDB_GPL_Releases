@echo off
cls

set TITLE=<FORMATED_BUILD_VERSION> SAP DB Developer Studio (<LC_STATE>)
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

rem release special settings
&if $RELVER = R72
&   if $CLEVEL = 4
        set GRP=\\P30883\V7204\develop
&   else
        set DEPOT=//sapdb/V72/develop
        set GRP=\\P30883\V72\develop
&   endif   	
&elif $RELVER = R73
&   if $CLEVEL = 0
        set GRP=\\P64529\V73\c_00\develop
        set DEPOT=//sapdb/V73/c_00/develop
&   else
        set DEPOT=//sapdb/V73/develop
        set GRP=\\P26327\V73\develop
&   endif
&endif

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

set BACKUP=y
set CLASSPATH=%CLASSPATH%;%DBROOT%\misc\sapdbc.jar;%OWN%\test\jtest\frame\testframe.jar;%OWN%\test\jtest\tests\lctest.jar
set CLASSPATH=%CLASSPATH%;%OWN%\test\jtest\tests\oltptest.jar;%OWN%\test\jtest\frame\Lib.jar
set CPU=i386
rem %EDITOR% wird als Standard-Editor verwendet
if %EDITOR%. ==  . set EDITOR=msdev.exe
set INCLUDE=%DBROOT%\incl;%TOOL%\incl;%TOOL%\incl\Cluster;%OWN%\sys\wrk\incl;%INCLUDE%
set LIB=%DBROOT%\lib;%TOOL%\lib;%LIB%
set MGET=
set MUT=%TEST%\port\mut
set OSBASE=%OS%
set CODECHECK=OFF
set OCCINCL=%DBROOT%\incl;%TOOL%\incl;%OWN%\sys\wrk\incl;
set OWNINCS=%OCCINCL%
set PATH=.;%TOOL%\bin;%TOOL%\Posix;%TOOL%\pgm;%DBROOT%\bin;%DBROOT%\pgm;%TOOL%\Perl\bin;%PATH%
set PATHEXT=%PATHEXT%;%TOOLEXT%;.py
set PERL5LIB=%INSTROOT%\misc;%TOOL%\bin;%TOOL%\Lib\Perl;%TOOL%\Lib\perl5
if %PYTHON%. ==  . set PYTHON=%TOOL%\Python
set PATH=%PATH%;%TOOL%\Python
set PYTHONPATH=%TOOL%\Lib\Python;%OWN%\usr\misc;%INSTROOT%\lib
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

if not %OWN%.==. doskey own  = cd /d %OWN%\$1*
if not %GRP%.==. doskey grp  = pushd %GRP%\$1*
if not %ALL%.==. doskey all  = pushd %ALL%\$1*
if not %REL%.==. doskey rel  = pushd %REL%\$1*

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
set _VMAKE_OPTION=%VMAKE_OPTION%&set _VMAKE_PATH=%VMAKE_PATH%&set _TOOLVARS=%TOOLVARS%

rem ------------ machinenspezifische Konfiguration
if exist %TOOL%\imach.bat call %TOOL%\imach.bat

rem ------------ lokale Konfiguration
if exist %OWN%\iuser.bat call %OWN%\iuser.bat

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
echo ---- ! ERROR: %OWN% not set ! ----
pause
exit
:OWN_OK

if not %RELVER%. == . goto RELVER_OK
echo ---- ! ERROR: %RELVER% not set ! ----
pause
exit
:RELVER_OK

if exist %REMCL% goto REMCL_OK
echo ---- ! WARNING: %REMCL% do'nt exists ! ----
pause
:REMCL_OK

if  exist %SHELL% goto SHELL_OK
if  %SHELL%. == . goto SHELL_OK
echo ---- ! WARNING: %SHELL% do'nt exists ! ----
pause
exit
:SHELL_OK

rem Shell die die Tools ausführt (vmake-relevant)
if  exist %TOOLSHELL% goto TOOLSHELL_OK
if  not %TOOLSHELL%. == . goto TOOLSHELL_OK
echo ---- ! WARNING: %TOOLSHELL% don't exists ! ----
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
