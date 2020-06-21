REM STEP 1 : sdb loader package

REM variant 1: make sdb loader package

imf -u ls
imf -u srvinst
buildpackage -profile "Loader" -format sar -ignore_errors
cp   %WRK%\fast\install\maxdbld7600_02.sar     %TEST%\sdb\maxdbld7600_02.sar
sapcar  -xvf %TEST%\sdb\maxdbld7600_02.sar -R  %TEST%\sdb\

REM variant 2: import sdb loader package from lcpool

REM packdistrib -profile "Loader"  -format sar -version 7600 -state DEV -os WIN32   -dir %TEST%\sdb\
REM sapcar  -xvf %TEST%\sdb\maxdbld7600_01.sar -R   %TEST%\sdb\

REM STEP 2 : uninstall sdb loader

SET SAPDB_NO_INDEPPATH=
SET DBROOT=
sdbuninst -package "Base"   -autoresolve -silent 

REM STEP 3 : install sdb loader
%TEST%\sdb\maxdb-loader-win-32bit-i386-7_6_0_1\sdbinst -profile "Loader" -silent -indep_prog "%ProgramFiles%\sdb\programs" -indep_data "%ALLUSERSPROFILE%\Application Data\sdb\data"

REM STEP 4 : test    sdb loader
%HOMEDRIVE%\Program" "Files\sdb\programs\bin\x_python %HOMEDRIVE%\Program" "Files\sdb\programs\lib\test\sdb\loader\test\test_loader.py -T 16