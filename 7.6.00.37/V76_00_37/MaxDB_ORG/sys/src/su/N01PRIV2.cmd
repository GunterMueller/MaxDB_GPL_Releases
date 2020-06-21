*   *ID* N01PRIV2 DBCMD    changed on 1992-09-28-13.24.59 by ADMIN     *
 *** GRANT und REVOKE wird getestet.
 *** Der SUT vergibt das ALL-Privileg fuer genau
 *** eine Basis-Tabelle mit genau einem Feld, welches ein
 *** Schluesselfeld ist, an einen DBA.
 *** GRANT  <table priv list> ON <table name> TO   <auth id>
 *** REVOKE <table priv list> ON <table name> FROM <auth id>
 *** Die    <table priv list>  enthaelt genau einen Wert.!
CREATE USER dba1 PASSWORD passdba1 DBA !
CREATE USER dba2 PASSWORD passdba2 DBA !
commit release !
file connect ( kern test !
CREATE TABLE kern.tabelle1 ( spalte1 CHAR(3) KEY ) !
CREATE TABLE kern.tabelle2 ( spalte1 CHAR(3) KEY ) !
GRANT ALL          ON tabelle1 TO dba1 !
GRANT ALL          ON tabelle1 TO dba1 !
GRANT SELECT       ON tabelle1 TO dba1 !
GRANT INSERT       ON tabelle1 TO dba1 !
GRANT DELETE       ON tabelle1 TO dba1 !
GRANT UPDATE       ON tabelle1 TO dba1 !
GRANT SELUPD ON tabelle1 TO dba1 !
GRANT CREATE       ON tabelle1 TO dba1 !
file sel_priv_granted ( PUBLIC !
file sel_priv_granted ( SUT !
file sel_priv_granted ( DBA3 !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
COMMIT WORK RELEASE !
file CONNECT ( dba1 passdba1 !
file sel_priv_granted ( PUBLIC !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
ROLLBACK WORK RELEASE !
file CONNECT ( kern test !
REVOKE SELECT       ON tabelle2 FROM dba1 !
REVOKE SELECT       ON tabelle1 FROM dba1 !
REVOKE SELECT       ON tabelle1 FROM dba1 !
REVOKE INSERT       ON tabelle1 FROM dba1 !
REVOKE DELETE       ON tabelle1 FROM dba1 !
REVOKE UPDATE       ON tabelle1 FROM dba1 !
REVOKE SELUPD ON tabelle1 FROM dba1 !
REVOKE ALL          ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
REVOKE ALL          ON tabelle1 FROM dba1 !
file sel_priv_granted ( PUBLIC !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
COMMIT WORK !
file sel_priv_granted ( PUBLIC !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
GRANT ALL          ON tabelle1 TO dba1 !
file sel_priv_granted ( DBA1 !
REVOKE ALL         ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
GRANT SELECT, INSERT ON tabelle1 TO dba1 !
file sel_priv_granted ( DBA1 !
REVOKE ALL           ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
GRANT ALL          ON tabelle1 TO dba1 !
file sel_priv_granted ( DBA1 !
REVOKE ALL         ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
REVOKE ALL ON tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
DROP TABLE tabelle1 !
DROP TABLE tabelle2 !
COMMIT WORK release!
file connect ( sut sut !
DROP USER dba1 !
DROP USER dba2 !
COMMIT WORK release!
