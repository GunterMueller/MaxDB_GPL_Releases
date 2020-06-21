*   *ID* N01PRIV4 DBCMD    changed on 1992-09-28-13.25.00 by ADMIN     *
 *** GRANT und REVOKE wird getestet.
 *** Der SUT vergibt mehrere Privilegien in einem
 *** GRANT-Befehl fuer genau eine Basis-Tabelle mit
 *** genau einem Feld, welches ein Schluesselfeld ist,
 *** an genau einen DBA. !
file connect ( sut sut !
CREATE USER dba1 password passdba1 DBA !
CREATE USER dba2 password passdba2 DBA !
commit release !
file connect ( kern test !
CREATE TABLE tabelle1  ( spalte1 CHAR(3) KEY ) !
GRANT ALl, SELECT ON tabelle1 TO dba1 !
GRANT SELECT, ALL ON tabelle1 TO dba1 !
GRANT SELECT ON tabelle1 TO dba1 !
GRANT SELECT ON tabelle1 TO dba1 !
GRANT SELECT ( spalte1 ) ON tabelle1 TO dba1 !
GRANT SELECT ( spalte2 ) ON tabelle2 TO dba1 !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
GRANT INSERT ON tabelle1, DELETE ON tabelle1 TO dba1 !
GRANT INSERT, SELECT ON tabelle1 TO dba1 !
GRANT SELUPD ON tabelle1 TO dba1 !
GRANT ALL ON tabelle1 TO dba1 !
GRANT OWNER ON tabelle1 TO dba1 !
GRANT INSERT, INSERT ON tabelle1 TO dba1 !
GRANT INSERT, DELETE ON tabelle1 TO dba1 !
GRANT UPDATE ON tabelle1 TO dba1 !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
COMMIT WORK RELEASE !
file CONNECT ( dba1 passdba1 !
file sel_priv_granted ( DBA3 !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
ROLLBACK WORK RELEASE !
file CONNECT ( kern test !
REVOKE ALL ON tabelle1 FROM dba1 !
REVOKE SELUPD ON tabelle1 FROM dba1 !
REVOKE OWNER ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
REVOKE INSERT ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
REVOKE SELECT, INSERT ON tabelle1 FROM dba1 !
REVOKE SELECT, DELETE, UPDATE ON tabelle1 FROM dba1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
DROP TABLE  tabelle1 !
COMMIT WORK release !
file connect ( sut sut !
DROP USER dba1 !
DROP USER dba2 !
commit release !
