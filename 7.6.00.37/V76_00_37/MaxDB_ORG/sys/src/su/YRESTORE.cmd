*   *ID* YRESTORE DBCMD    changed on 1992-09-28-13.28.59 by ADMIN     *
&nodisplay
dbmcli superdba,colduser!
util_connect superdba,colduser!
backup_restore YINITDB DATA!
util_execute clear log!
db_online!
util_release!
util_connect sut,sut!
*
*---------- Bitte den Anfang der folgenden Zeilen NICHT veraendern, denn
* in 'sqlkern:/u/admin/_sut-stack' wird darauf mit dem 'sed' gearbeitet.
* Kennzeichen dafuer sind die beiden '*' am Zeilenanfang.
*---------- fuer stackmessung freischalten:
** connect sut identified by sut
** diagnose vtrace default off
** diagnose vtrace order standard on
*----------
*
util_release!
bye!
if $rc <> 0 then stop (99)
*
*---------- fuer stackmessung freischalten:
** switch! 
** xx
** st
*----------
*
&display
