*   *ID* N01PRIV3 DBCMD    changed on 1992-09-28-13.25.00 by ADMIN     *
 *** GRANT und REVOKE wird getestet.
 *** Der SUT vergibt das ALL-Privileg fuer genau
 *** eine Basis-Tabelle mit genau einem Feld, welches ein
 *** Schluesselfeld ist, an genau einen DBA.
 *** Test, ob ALL und ALL PRIVILEGES austauschbar sind. !
file connect ( sut sut !
CREATE USER dba1 password passdba1 DBA !
commit release !
file connect ( kern test !
CREATE TABLE tabelle1  ( spalte1 CHAR(3) KEY ) !
GRANT ALL            ON tabelle1 TO dba1 !
GRANT ALL PRIVILEGES ON tabelle1 TO dba1 !
file sel_priv_granted ( DBA1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
REVOKE ALL PRIVILEGES ON tabelle1 FROM dba1 !
REVOKE ALL            ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
GRANT ALL PRIVILEGES ON tabelle1 TO dba1 !
GRANT ALL            ON tabelle1 TO dba1 !
COMMIT WORK !
file sel_priv_granted ( SUT !
file sel_priv_granted ( DBA1 !
REVOKE ALL            ON tabelle1 FROM dba1 !
REVOKE ALL PRIVILEGES ON tabelle1 FROM dba1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
DROP TABLE tabelle1 !
COMMIT WORK release !
file connect ( sut sut !
DROP USER dba1 !
commit release !
