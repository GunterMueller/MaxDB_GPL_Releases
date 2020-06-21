*   *ID* N01PRIV1 DBCMD    changed on 1992-09-28-13.24.59 by ADMIN     *
 *** GRANT wird getestet.
 *** Der SUT vergibt Privilegien fuer eine nicht
 *** definierte Tabelle !
CREATE USER dba1 password passdba1 DBA!
GRANT INSERT ON tabelle TO sut !
GRANT INSERT ON tabelle TO dba1     !
GRANT ALL    ON tabelle TO sut !
GRANT ALL    ON tabelle TO dba1     !
GRANT OWNER  ON tabelle TO sut !
GRANT OWNER  ON tabelle TO dba1     !
DROP USER dba1 !
