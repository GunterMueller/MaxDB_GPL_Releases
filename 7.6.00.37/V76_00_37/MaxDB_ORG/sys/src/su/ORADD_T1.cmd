*   *ID* ORADD-T1 CMD      changed on 1994-02-08-12.14.57 by KRISCHAN  *

file connect ( kern test SQLMODE ORACLE!
CREATE SEQUENCE seq1 INCREMENT BY 2 START WITH 1!
COMMIT WORK RELEASE!

file connect ( sut sut!
CREATE USER dba1    PASSWORD dba1 DBA!
CREATE USER dba2    PASSWORD dba2 DBA!
CREATE USER dba3    PASSWORD dba3 DBA!
CREATE USER normalo PASSWORD pet  STANDARD!
COMMIT WORK RELEASE!

file connect ( dba1 dba1 SQLMODE ORACLE!
CREATE TABLE dba1t1 (s1 INT, s2 INT)!
CREATE SYNONYM dba1s1 FOR dba1t1!
COMMENT ON TABLE  dba1t1    IS 'This is table dbat1'!
COMMENT ON COLUMN dba1t1.s1 IS 'This is column s1'!
SELECT * FROM user_col_comments!
FETCH INTO :a, :b, :c!
SELECT * FROM user_tab_comments!
FETCH INTO :a, :b, :c!
COMMIT WORK RELEASE!

file connect ( dba2 dba2 SQLMODE ORACLE!
CREATE TABLE dba2t1 (s1 INT, s2 INT)!
CREATE SYNONYM dba2s1 FOR dba2t1!
COMMIT WORK RELEASE!

file connect ( dba3 dba3 SQLMODE ORACLE!
CREATE TABLE dba3t1 (s1 INT, s2 INT)!
CREATE SYNONYM dba3s1 FOR dba3t1!
COMMIT WORK RELEASE!

file connect ( kern test SQLMODE ORACLE!
SELECT owner, synonym_name, table_owner, table_name FROM dba_synonyms!
FETCH INTO :a, :b, :c, :d!
SELECT SUBSTR (owner,      1, 28) owner,
       SUBSTR (table_name, 1, 32) table_name
  FROM dba_tables
  WHERE owner <> 'DUMMY'
    AND table_name NOT LIKE 'SYS%CAT_'!
FETCH INTO :a, :b!
SELECT SUBSTR (owner,      1, 28) owner,
       SUBSTR (table_name, 1, 32) table_name,
       table_type
  FROM all_catalog WHERE owner <> 'DUMMY'
  order by 1,3,2!
FETCH INTO :a, :b, :c!
COMMIT WORK RELEASE!

* stuff with name dba_* isn't visible for normal users!
file connect ( normalo pet SQLMODE ORACLE!
SELECT * FROM     dba_users!
SELECT * FROM sys.dba_users!
SELECT * FROM dictionary WHERE table_name LIKE 'DBA_%'!
ROLLBACK WORK RELEASE!

* All these fine views are only visible in sqlmode oracle!
FILE connect (kern test!
SELECT owner FROM sys.accessible_columns!
SELECT owner FROM sys.all_catalog!
ROLLBACK WORK RELEASE!

* THE GRANT OPTION on these views works only for USER_* and ALL_*,
* but not for DBA_* views!
FILE connect (kern test SQLMODE ORACLE!
CREATE VIEW tabledef (table_name, column_name, data_type) AS
  SELECT SUBSTR (table_name,  1, 32) table_name,
         SUBSTR (column_name, 1, 32) column_name,
         data_type
    FROM all_tab_columns WHERE owner NOT IN ('SYS', 'DUMMY')!
GRANT ALL ON tabledef TO PUBLIC WITH GRANT OPTION!
CREATE VIEW dba_tabledef AS
  SELECT table_name, column_name, data_type
    FROM dba_tab_columns WHERE owner NOT IN ('SYS', 'DUMMY')!
GRANT ALL ON dba_tabledef TO PUBLIC WITH GRANT OPTION!
COMMIT WORK RELEASE!

FILE connect (normalo pet SQLMODE ORACLE!
SELECT * from kern.tabledef!
FETCH INTO :A,:B,:C!
COMMIT WORK RELEASE!

FILE connect (kern test SQLMODE ORACLE!
DROP VIEW tabledef!
DROP VIEW dba_tabledef!
COMMIT WORK RELEASE!
