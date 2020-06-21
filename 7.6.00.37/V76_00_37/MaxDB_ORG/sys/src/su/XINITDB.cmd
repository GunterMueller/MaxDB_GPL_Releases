*   *ID* XINITDB  DBCMD    changed on 1992-09-28-13.22.00 by ADMIN     *
*
*  Das Initialisieren ist ausgelagert worden, damit es auch ohne das
*  Anlegen des Users SUT SUT aus einer Datei heraus erfolgen
*  kann.
*
file xinitall (sut sut!
DBMCLI SUPERDBA,COLDUSER!
LOAD_SYSTAB -U SUT,SUT !
BYE!
*
&nodisplay
connect sut identified by sut !
select * from domain.versions !
fetch !
* check if installation worked. check last view from XDD !
select * from domain.parsinfos !
IF &rc <> 0 THEN BEGIN
commit release !
END
ELSE BEGIN
commit release !
dbmcli superdba,colduser!
util_connect sut,sut!
util_execute check catalog !
db_admin!
util_connect superdba,colduser!
backup_save XINITDB DATA!
util_release!
bye!
if $rc <> 0 then stop (99)
&display
END
