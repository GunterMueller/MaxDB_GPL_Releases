*   *ID* XINITALL DBCMD    changed on 1992-09-28-13.21.59 by ADMIN     *
*
*   Diese Datei sollte moeglichst immer fuer das Initialisieren einer DB
*   werden, wenn nicht, wie ansonsten jetzt ueblich, das XRESTORE DBCMD
*   benutzt werden kann.
*
*   &1      &2
*   sysdba  syspwd
*
dbmcli superdba,colduser!
db_activate &1,&2 !
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
bye!
&nodisplay

*
*---------- fuer stackmessung freischalten:
** switch! 
** xx
** st
*----------
*
&display
