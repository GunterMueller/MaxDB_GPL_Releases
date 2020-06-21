*   *ID* N01PRIV5 DBCMD    changed on 1992-09-28-13.25.01 by ADMIN     *
 *** GRANT und REVOKE wird getestet.
 *** Der SUT vergibt das SELECT-, UPDATE-, SELUPD-Privileg mit
 *** Angabe einer <column list> fuer genau eine Basis-Tabelle mit genau
 *** einem Feld, welches ein Schluesselfeld ist, an genau einen DBA.
 *** Getestet wird:
 *** GRANT SELECT       ( <column list> ) ON <table name> TO <auth id>
 *** GRANT UPDATE       ( <column list> ) ON <table name> TO <auth id>
 *** GRANT SELUPD ( <column list> ) ON <table name> TO <auth id> !
file connect ( sut sut !
CREATE USER dba1 password passdba1 DBA !
CREATE USER dba2 password passdba2 DBA !
commit release !
file connect ( kern test !
CREATE TABLE kern.tabelle1  ( spalte1 CHAR(3) KEY ) !
GRANT SELECT ( spalte1, spalte2 ) ON kern.tabelle1 TO dba1 !
GRANT SELECT   spalte1            ON kern.tabelle1 TO dba1 !
GRANT SELECT ( spalte1 )          ON kern.tabelle1 TO dba1 !
GRANT SELECT                      ON kern.tabelle1 TO dba1 !
file sel_priv_granted ( DBA1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
GRANT UPDATE ( spalte1 )          ON kern.tabelle1 TO dba1 !
GRANT UPDATE                      ON kern.tabelle1 TO dba1 !
file sel_priv_granted ( DBA1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
GRANT SELUPD ON kern.tabelle1 TO dba1 !
ROLLBACK WORK RELEASE !
file connect ( dba1 passdba1 !
file sel_priv_granted ( DBA1 !
ROLLBACK WORK RELEASE !
file connect ( kern test !
REVOKE SELECT ( spalte1, spalte2 ) ON kern.tabelle1 FROM dba1 !
REVOKE SELECT   spalte1            ON kern.tabelle1 FROM dba1 !
REVOKE SELECT                      ON kern.tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
REVOKE SELECT ( spalte1 )          ON kern.tabelle1 FROM dba1 !
REVOKE SELECT ( spalte1 )          ON kern.tabelle1 FROM dba1 !
file sel_priv_granted ( DBA2 !
file sel_priv_granted ( DBA1 !
REVOKE UPDATE ON kern.tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
REVOKE UPDATE ( spalte1 ) ON kern.tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
COMMIT WORK !
file sel_priv_granted ( DBA1 !
GRANT SELUPD ( spalte1, spalte2 ) ON kern.tabelle1 TO dba1 !
GRANT SELUPD   spalte1            ON kern.tabelle1 TO dba1 !
GRANT SELUPD ( spalte1 )          ON kern.tabelle1 TO dba1 !
GRANT SELUPD                      ON kern.tabelle1 TO dba1 !
file sel_priv_granted ( DBA1 !
COMMIT WORK RELEASE !
file connect ( dba1 passdba1 !
file sel_priv_granted ( DBA1 !
ROLLBACK WORK RELEASE !
file connect ( kern test !
REVOKE SELUPD             ON kern.tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
REVOKE SELUPD ( spalte1 ) ON kern.tabelle1 FROM dba1 !
file sel_priv_granted ( DBA1 !
DROP TABLE kern.tabelle1 !
COMMIT WORK RELEASE !
file connect ( sut sut !
DROP USER dba1 !
DROP USER dba2 !
commit release !
