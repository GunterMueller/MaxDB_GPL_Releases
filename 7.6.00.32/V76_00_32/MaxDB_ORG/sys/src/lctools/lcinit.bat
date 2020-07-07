@echo off
setlocal

rem **********************************************************
rem *         liveCache  Initialization                      *
rem **********************************************************
rem *                                                        *
rem *         lcinit <lc_name> [ <parameter> [debug]]        *
rem *                          [ -e unicode | ascii ]        *
rem *                          [ -uDBM <user>,<pwd> ]        *
rem *                          [ -uDBA <user>,<pwd> ]        *
rem *                          [ -uSQL <user>,<pwd> ]        *
rem *                                                        *
rem *                                                        *
rem *                                                        *
rem *   lc_name   : database name                            *
rem *                                                        *
rem *   parameter : additional parameter                     *
rem *                                                        *
rem *               init                                     *
rem *               register                                 *
rem *               restart                                  *
rem *               slow                                     *
rem *               stop                                     *
rem *               shutdown                                 *
rem *                                                        *
rem *   userflags :                                          *
rem *                                                        *
rem *               -e    : Encoding (default : ascii)       *
rem *               -uDBM : DBM User (default : control)     *
rem *               -uDBA : DBA User (default : superdba)    *
rem *               -uSQL : SQL User (default : sapr3)       *
rem *                                                        *
rem **********************************************************

rem --------------------- input parameters -------------------------------
set LC=%1
set MODE=restart
set DEBUG=opt

if "%LC%" == "" (
  echo.
  echo USAGE : LCINIT db_name [parameter] [userflags]                                 
  echo.
  echo         parameter :
  echo                      init     [debug]
  echo                      restart  [debug]
  echo                      register [debug]
  echo                      slow             
  echo                      shutdown
  echo                      stop
  echo         userflags :
  echo                      -uDBM user,pwd
  echo                      -uDBA user,pwd
  echo                      -uSQL user,pwd
  echo                      -e    unicode|ascii
  echo.
  goto end_err
)

rem --------------------- internal user/pwd ------------------------------
set DB_USER=control
set DB_PWD=control
set DBA_USER=superdba
set DBA_PWD=admin
set SAP_USER=sapr3
set SAP_PWD=sap
set SAP_ORGPWD=sap

rem --------------------- check input parameters -------------------------
:check_input
if "%2" == "" goto end_input
if "%2" == "-uDBM" (
  set DB_USER=%3
  set DB_PWD=%4
  shift
  shift  
) else if "%2" == "-uDBA" (
  set DBA_USER=%3
  set DBA_PWD=%4
  shift
  shift  
) else if "%2" == "-uSQL" (
  set SAP_USER=%3
  set SAP_PWD=%4
  set SAP_ORGPWD=%4
  shift
  shift  
) else if "%2" == "-e" (
  set ENCODING=%3
  shift  
) else if "%2" == "debug" (
  set DEBUG=%2
) else (
  set MODE=%2
)
shift
goto check_input
:end_input

rem ------------------------ define dbmcli -------------------------------
set DBMCLI=dbmcli -d %LC% -u %DB_USER%,%DB_PWD%

rem --------------------- internal variables -----------------------------
set STATE=OFFLINE
set VERS=0
set DBTYPE=DB
set ERRFLAG=0
set DONETXT=restarted

if "%MODE%" == "INIT"     set MODE=init
if "%MODE%" == "STOP"     set MODE=stop
if "%MODE%" == "SHUTDOWN" set MODE=shutdowm
if "%MODE%" == "SLOW"     set MODE=slow
if "%MODE%" == "RESTART"  set MODE=restart
if "%MODE%" == "REGISTER" set MODE=register

if "%MODE%" == "slow" (
  set DB_ONLINE=db_online -slow
  set DB_ADMIN=db_admin   -slow
  set DB_ONLINETEXT=starting %LC% SLOW KERNEL into ONLINE
  set DB_ADMINTEXT=starting %LC% SLOW KERNEL into ADMIN
) else (
  if "%DEBUG%" == "debug" (
    set DB_ONLINE=db_online -test
    set DB_ADMIN=db_admin   -test
    set DB_ONLINETEXT=starting %LC% TEST KERNEL into ONLINE
    set DB_ADMINTEXT=starting %LC% TEST KERNEL into ADMIN
  ) else (
    set DB_ONLINE=db_online
    set DB_ADMIN=db_admin  
    set DB_ONLINETEXT=starting %LC% into ONLINE
    set DB_ADMINTEXT=starting %LC% into ADMIN
  )
)

rem --------------------- check database state ---------------------------

%DBMCLI% db_state > sdbtmp
if errorlevel 1 (
  echo.
  echo ERROR : cannot get database state
  type sdbtmp
  echo.
  goto end_err
)

findstr "WARM ONLINE" sdbtmp > sdbtmp2
if not errorlevel 1 (
  set STATE=ONLINE
) else (
  findstr "COLD ADMIN" sdbtmp > sdbtmp2
  if not errorlevel 1 (
    set STATE=ADMIN
  )
)

rem --------------------- get installation directory ---------------------

%DBMCLI% dbm_version INSTROOT > sdbtmp

if errorlevel 1 (
  echo.
  echo ERROR : cannot get INSTROOT
  type sdbtmp
  echo.
  goto end_err
) 

rem build a command script to set INSTROOT

echo @echo off > instroot.bat
echo (set INSTROOT=^^>> instroot.bat
findstr -v "OK" sdbtmp >> instroot.bat
echo ) >> instroot.bat
call instroot
del instroot.bat

if not exist "%INSTROOT%\bin" (
  echo.
  echo ERROR : %INSTROOT% does not exist
  echo.
  goto end_err
)

rem --------------------- get rundirectory ---------------------

%DBMCLI% param_directget RUNDIRECTORY > sdbtmp
if errorlevel 1 (
  echo.
  echo ERROR : cannot get RUNDIRECTORY
  type sdbtmp
  echo.
  goto end_err
) 

rem build a command script to set RUNDIRECTORY

echo @echo off > rundir.bat
echo (set RUNDIRECTORY=^^>> rundir.bat
findstr "RUNDIRECTORY" sdbtmp >> rundir.bat
echo ) >> rundir.bat
call rundir
del rundir.bat
set RUNDIRECTORY=%RUNDIRECTORY:~13%

if not exist "%RUNDIRECTORY%" (
  echo.
  echo ERROR : RUNDIRECTORY %RUNDIRECTORY% does not exist
  echo.
  goto end_err
)

rem --------------------- get MCOD parameter ---------------------

%DBMCLI% param_directget MCOD > sdbtmp
if errorlevel 1 (
  set MCOD=NO
  goto mcod_default
) 

rem build a command script to set MCOD

echo @echo off > MCOD.bat
echo (set MCOD=^^>> MCOD.bat
findstr "MCOD" sdbtmp >> MCOD.bat
echo ) >> MCOD.bat
call MCOD
del MCOD.bat
set MCOD=%MCOD:~5%

:mcod_default

rem --------------------- check database instance ------------------------

%DBMCLI% param_directget INSTANCE_TYPE > sdbtmp

if not errorlevel 1 (
  findstr "LVC" sdbtmp > sdbtmp2
  if not errorlevel 1 (
    rem liveCache, INSTANCE_TYPE is LVC
    set DBTYPE=LVC
  )
)

if "%DBTYPE%" == "LVC" (
  set DBTEXT=liveCache
)
if "%DBTYPE%" == "DB" (
  set DBTEXT=Database
)

rem -------------------------- set location of his and log file -----------------

set LOGFILE="%RUNDIRECTORY%\lcinit.log"
set HISFILE="%RUNDIRECTORY%\lcinit.his"
set INST_PATH="%INSTROOT%\sap"

rem --------------------- print out the header  --------------------------

echo.
echo %DBTEXT% %LC% (%MODE%)
echo ************************ START ***************************** > %LOGFILE%
echo %DBTEXT% %LC% (%MODE%) >> %LOGFILE%
date /t  >> %LOGFILE%
time /t  >> %LOGFILE%
echo ------------------------------------------------------------ >> %LOGFILE%
echo installation path = %INSTROOT% >> %LOGFILE%
echo.
echo The %DBTEXT% state is %STATE%
echo.

rem ------------------- check version of the database --------------------
if "%STATE%" == "ADMIN" (
  %DBMCLI% db_offline >> %LOGFILE%
  if errorlevel 1 goto err
  set STATE=OFFLINE
)

if "%STATE%"=="OFFLINE" (
  %DBMCLI% %DB_ADMIN% >> %LOGFILE%
  if errorlevel 1 goto err
  set STATE=ADMIN
)

%DBMCLI% dbm_version BUILD > sdbtmp
%DBMCLI% dbm_version BUILD >> %LOGFILE%
findstr Build sdbtmp
echo.

rem -------------------- parameter switch --------------------------------

if "%MODE%" == "init"     set DONETXT=initialized
if "%MODE%" == "init"     goto init_lc

if "%MODE%" == "stop"     set DONETXT=stopped
if "%MODE%" == "stop"     goto stop_lc

if "%MODE%" == "shutdown" set DONETXT=stopped
if "%MODE%" == "shutdown" goto stop_lc

if "%MODE%" == "slow"     set DONETXT=started
if "%MODE%" == "slow"     goto restart_lc

if "%MODE%" == "restart"  set DONETXT=started
if "%MODE%" == "restart"  goto restart_lc

if "%MODE%" == "register" set DONETXT=registered
if "%MODE%" == "register" goto restart_lc

echo.
echo ERROR : parameter '%MODE%' is not allowed!
echo.
goto end_err

rem ----------------------------------------------------------------------
rem --- restart of the database (restart, slow and register) -------------
rem ----------------------------------------------------------------------

:restart_lc

if "%STATE%" == "ONLINE" (

  if "%MODE%" == "register" (
    goto loading_data
  ) else (
    goto nothing_todo
  )

) else (

  echo %DB_ONLINETEXT%
  echo %DB_ONLINETEXT% >> %LOGFILE%
  %DBMCLI% %DB_ONLINE% >> %LOGFILE%

  if errorlevel 1 (
    echo.
    echo ERROR : restart not possible [please check knldiag!!]
    echo ERROR : restart not possible [please check knldiag] >> %LOGFILE%
    echo.
    goto err
  )
)

goto loading_data

rem ----------------------------------------------------------------------
rem --- stop the database (shutdown and stop) ----------------------------
rem ----------------------------------------------------------------------

:stop_lc

if "%STATE%" == "OFFLINE" goto nothing_todo
  
if "%MCOD%" == "YES" (
  if "%STATE%" == "ONLINE" (
    echo ERROR : Action not allowed for ALL-IN-ONE system
    echo ERROR : Action not allowed for ALL-IN-ONE system >> %LOGFILE%
    goto err
  )
)

echo stopping %LC% 
echo stopping %LC% >> %LOGFILE%
if "%MODE%" == "stop" (
  %DBMCLI% db_stop >> %LOGFILE%
) else (
  %DBMCLI% db_offline >> %LOGFILE%
)
  
if errorlevel 1 goto err

goto ok

rem --------------------   nothing TODO ----------------------------------
:nothing_todo

echo.
echo %DBTEXT% %LC% is already %STATE%
echo %DBTEXT% %LC% is already %STATE% >> %LOGFILE%
goto history

rem ----------------------------------------------------------------------
rem --- init of the database ---------------------------------------------
rem ----------------------------------------------------------------------

:init_lc

rem --------------------- check for MCOD ---------------------------------

if "%MCOD%" == "YES" goto mcodinit

rem -------------------- start DB into ADMIN mode -------------------------

if "%STATE%" == "ONLINE" (
  echo %DB_ADMINTEXT%
  echo %DB_ADMINTEXT% >> %LOGFILE%
  %DBMCLI% db_stop >> %LOGFILE%
  if errorlevel 1 goto err
  %DBMCLI% %DB_ADMIN% >> %LOGFILE%
  if errorlevel 1 goto err
)

rem --------------------- ACTIVATE SERVERDB ------------------------------

echo starting ACTIVATE SERVERDB
echo starting ACTIVATE SERVERDB >> %LOGFILE%
%DBMCLI% -uUTL -c db_activate %DBA_USER%,%DBA_PWD% >> %LOGFILE%
if errorlevel 1 goto err
set STATE=ONLINE

rem --------------------- loading system tables --------------------------

echo loading SYSTEM TABLES
echo loading SYSTEM TABLES >> %LOGFILE%
%DBMCLI% -c load_systab >> %LOGFILE%
if errorlevel 1 goto err

goto createuser

rem --------------------- MCOD -------------------------------------------

:mcodinit

rem -------------------- start DB into ONLINE mode ------------------------

if not "%STATE%" == "ONLINE" (
  echo %DB_ONLINETEXT%
  echo %DB_ONLINETEXT% >> %LOGFILE%
  %DBMCLI% %DB_ONLINE% >> %LOGFILE%
  if errorlevel 1 goto err
  set STATE=ONLINE
)

rem --------------------- check user --------------------------------------

echo Checking user %SAP_USER%
echo Checking user %SAP_USER% >> %LOGFILE%
%DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute select * from users where Upper(username) = Upper('%SAP_USER%')  >> %LOGFILE%
if errorlevel 1 goto user_not_exist

rem --------------------- disable user --------------------------------------

echo Disable user %SAP_USER%
echo Disable user %SAP_USER% >> %LOGFILE%
set SAP_PWD=temporary
%DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute alter password %SAP_USER% %SAP_PWD% >> %LOGFILE%
if errorlevel 1 goto err

rem --------------------- cancel all tasks -------------------------------

echo Disconnecting user %SAP_USER%
echo Disconnecting user %SAP_USER% >> %LOGFILE%
%DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute select process from transactions where upper(username) = upper('%SAP_USER%') > processes.txt
if errorlevel 1 goto no_users
FOR /F "skip=2" %%K IN (processes.txt) DO %DBMCLI%  db_cons kill %%K >> %LOGFILE%
if errorlevel 1 goto err
:no_users
del processes.txt 

rem --------------------- check for cleanup procedure ------------------------
if "%DBTYPE%" == "LVC" ( 
  %DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute select * from dbprocedures where DBPROCNAME = 'OMS_CLEANUP_AFTER_INIT'  >> %LOGFILE%
  if errorlevel 1 goto no_cleanup_procedure
)

rem --------------------- cleanup global oms-structures ------------------- 

if "%DBTYPE%" == "LVC" ( 
  echo Cleanup OMS structures
  echo Cleanup OMS structures >> %LOGFILE% 
  %DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute call OMS_CLEANUP_AFTER_INIT >> %LOGFILE% 
  if errorlevel 1 goto err 
) 

:no_cleanup_procedure

rem --------------------- drop schemas -----------------------------------

if "%DBTYPE%" == "LVC" (
  echo Dropping schemas
  echo Dropping schemas >> %LOGFILE%
  %DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute call drop_schemas  >> %LOGFILE%
  if errorlevel 1 goto err
)

rem --------------------- drop user --------------------------------------

echo Dropping user %SAP_USER%
echo Dropping user %SAP_USER% >> %LOGFILE%
%DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute drop user %SAP_USER%  >> %LOGFILE%
if errorlevel 1 goto err
:user_not_exist

rem --------------------- creating SAP user ------------------------------

:createuser

set SAP_PWD=temporary

if "%ENCODING%" == "" goto noencoding

echo Creating user %SAP_USER% with DEFAULTCODE %ENCODING%
echo Creating user %SAP_USER% with DEFAULTCODE %ENCODING% >> %LOGFILE%
%DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute create user %SAP_USER% password %SAP_PWD% dba not exclusive defaultcode %ENCODING% >> %LOGFILE%
if errorlevel 1 goto err
goto endencoding

:noencoding
echo Creating user %SAP_USER%
echo Creating user %SAP_USER% >> %LOGFILE%
%DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute create user %SAP_USER% password %SAP_PWD% dba not exclusive >> %LOGFILE%
if errorlevel 1 goto err

:endencoding

if "%MCOD%" == "YES" goto loading_data

rem --------------------- restarting the database ------------------------

%DBMCLI% db_offline >> %LOGFILE%
if errorlevel 1 goto err
echo %DB_ONLINETEXT%
echo %DB_ONLINETEXT% >> %LOGFILE%
%DBMCLI% %DB_ONLINE% >> %LOGFILE%
if errorlevel 1 goto err
set STATE=ONLINE

rem ----------------------------------------------------------------------
rem ------------------- END : init of the database -----------------------
rem ----------------------------------------------------------------------

rem ----------------------------------------------------------------------
rem --------------- definition of the application COM routines -----------
rem ----------------------------------------------------------------------

rem --------------------- create pocedures ------------------------

:loading_data

echo Creating liveCache application procedures
echo Creating liveCache application procedures >> %LOGFILE%
%DBMCLI% load_lcapps %SAP_USER%,%SAP_PWD% >> %LOGFILE%
if errorlevel 1 goto err

rem --------------------- check for reset procedure ------------------------
if "%MODE%" == "init" (
  %DBMCLI% -uSQL %SAP_USER%,%SAP_PWD% sql_execute select * from dbprocedures where DBPROCNAME = 'SAPAPO_RESET_GLOBALS'  >> %LOGFILE%
  if errorlevel 1 goto no_reset_procedure
)

rem --------------------- call reset procedure ------------------------

if "%MODE%" == "init" (
  echo Calling reset procedure 
  echo Calling reset procedure >> %LOGFILE%
  %DBMCLI% -uSQL %SAP_USER%,%SAP_PWD% sql_execute call SAPAPO_RESET_GLOBALS  >> %LOGFILE%
  if errorlevel 1 goto err
)

rem --------------------- enable user ------------------------

:no_reset_procedure

echo Enable user %SAP_USER%
echo Enable user %SAP_USER% >> %LOGFILE%

set SAP_PWD=%SAP_ORGPWD%
%DBMCLI% -uSQL %DBA_USER%,%DBA_PWD% sql_execute alter password %SAP_USER% %SAP_PWD% >> %LOGFILE%
if errorlevel 1 goto err

%DBMCLI% user_sap %SAP_USER%,%SAP_PWD% >> %LOGFILE%
if errorlevel 1 goto err

rem ----------------------------------------------------------------------

rem ----------------------------------------------------------------------
rem --------------- end handling -----------------------------------------
rem ----------------------------------------------------------------------

:ok 
echo.
echo %DBTEXT% %LC% successfully %DONETXT%.
echo %DBTEXT% %LC% successfully %DONETXT%. >> %LOGFILE%
goto history

:err
echo.
set ERRFLAG=1
echo ERROR : %DBTEXT% %LC% not %DONETXT% (see %LOGFILE%).
echo ERROR : %DBTEXT% %LC% not %DONETXT% >> %LOGFILE%

:history
echo ------------------------------------------------------------ >> %LOGFILE%
date /t  >> %LOGFILE%
time /t  >> %LOGFILE%
echo ************************* END ****************************** >> %LOGFILE%
type %LOGFILE% >> %HISFILE%

if exist sdbtmp (
  del sdbtmp
)
if exist sdbtmp2 (
  del sdbtmp2
)

if %ERRFLAG% == 0 goto end

rem ------------------ provocate error ----------------------------------
:end_err
@verify error >NUL 2>&1

rem ---------------------------------------------------------------------
:end
