@echo off
rem Release
set RELVER=R§MAJ§MIN
set CORRECTION_LEVEL=§COR
set BUILDPRAEFIX=§BLDPRFX
set OWN=§DEV
set GRP=§ORG


set TOOL=§DEVTOOL
set PERL=§PERL
set PYTHON=§PYTHON
rem SHELL= äquiv. SHELL=cmd.exe
set SHELL=
set EDITOR=§EDITOR
set LC_STATE=§LCSTATE
set SAPDB_NO_INDEPPATH=yes
set TEST=%OWN%\test
set JTEST_ROOT=%OWN%\test\jtest

rem #statisch
set PYTHONPATH=%TOOL%\lib\Python;%OWN%\usr\runtime\python;%OWN%\usr\misc
set DBROOT=%OWN%\usr
set INSTROOT=%OWN%\usr
set SRC=%OWN%\sys\src
set DESC=%OWN%\sys\desc
set WRK=%OWN%\sys\wrk

set VMAKE_PATH=%OWN%,%GRP%
set TOOLEXT=.pl
set VMAKE_OPTION=aDISOhvWMLRgbX
set INCLUDE=%DBROOT%\incl;%TOOL%\incl;%OWN%\sys\wrk\incl;%OWN%\sys\wrk\incl\SAPDB;%INCLUDE%
set LIB=%DBROOT%\lib;%TOOL%\lib;%LIB%
set PATHEXT=%PATHEXT%;%TOOLEXT%;.py
set POSIX=%TOOL%\Posix
set TMP=%OWN%\tmp
set TOOLVARS=%TOOL%\bin\toolvars%TOOLEXT%
set WRK=%OWN%\sys\wrk
set PYTHONPATH=%TOOL%\Lib\Python;%OWN%\usr\misc;%INSTROOT%\lib;%PYTHONPATH%
set PERL5LIB=%INSTROOT%\misc;%TOOL%\bin;%TOOL%\Lib\Perl,%PERL5LIB%
set PATH=.;%TOOL%\bin;%TOOL%\Posix;%TOOL%\pgm;%DBROOT%\bin;%DBROOT%\pgm;%PERL%\bin;%PYTHON%;%PATH%
set TOOLSHELL=%PERL%\bin\perl.exe

echo #
echo #
echo #
echo #	Welcome To MaxDB Development Environment
echo #	*****************************************
echo #
echo #	Version:			%RELVER%
echo # 	Correction Level:  		%CORRECTION_LEVEL%
echo #	Buildprefix:			%BUILDPRAEFIX%
echo #	Tools:				%TOOL%
echo #	Perl:				%PERL%
echo #	Python:				%PYTHON%
echo #	Editor:				%EDITOR%
echo #	VMAKE_PATH:			%VMAKE_PATH%
echo #
echo #
echo #
echo #

cd %OWN%
if exist %OWN%\iuser.bat call %OWN%\iuser.bat

