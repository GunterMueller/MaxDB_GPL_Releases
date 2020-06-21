*   *ID* XRESTORE DBCMD    changed on 1992-09-28-13.28.59 by ADMIN     *
&nodisplay
dbmcli superdba,colduser!
util_connect superdba,colduser!
backup_restore XINITDB DATA!
util_execute clear log!
db_online!
util_release!
util_connect sut,sut!
*
*---------- Bitte den Anfang der folgenden Zeilen NICHT veraendern, denn
* in 'sqlkern:/u/admin/_sut-stack' wird darauf mit dem 'sed' gearbeitet.
* Kennzeichen dafuer sind die beiden '*' am Zeilenanfang.
*---------- fuer stackmessung freischalten:
** util_execute diagnose vtrace default off !
** util_execute diagnose vtrace order standard on !
*----------
*
util_release!
bye!
*
*---------- fuer stackmessung freischalten:
** switch! 
** xx
** st
**
*----------
*
&display
