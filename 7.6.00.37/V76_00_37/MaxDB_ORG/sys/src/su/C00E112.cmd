*   *ID* C00E112  DBCMD    changed on 1992-09-28-13.23.05 by ADMIN     *
****** PRISM C104446 **************************************
file cleark !
file connect ( kern test !
CREATE TABLE t01 (s01 CHAR(8)  KEY, s02 CHAR(10), s03 CHAR(2))!
CREATE TABLE t02 (s01 CHAR(10) KEY, s02 CHAR(32))!
COMMIT!
INSERT t01 VALUES ('BENGALS','Cincinnati','si')!
INSERT t01 VALUES ('BROWNS','Cleveland','no')!
INSERT t02 VALUES ('Cincinnati','BENGALS')!
INSERT t02 VALUES ('Cleveland','BROWNS')!
COMMIT!
*
alter table t01 FOREIGN KEY l01 (s02) REFERENCES t02!
*
CREATE VIEW v01 AS SELECT s03, s02, s01 FROM t01!
COMMIT!
***********************************************************
declare r01 cursor for 
select * FROM v01 FOR UPDATE!
FETCH FIRST r01 INTO :v01, :v02, :v03!
UPDATE v01 SET s03 = 'no' WHERE CURRENT OF r01 !
select * from v01 !
fetch !
*
* RC = 100
*
***********************************************************
COMMIT!
*
alter table t01 drop foreign key l01 !
*
COMMIT!
declare r01 cursor for 
select * FROM v01 FOR UPDATE!
FETCH FIRST r01 INTO :v01, :v02, :v03!
UPDATE v01 SET s03 = 'no' WHERE CURRENT OF r01!
*
* RC = 0
*
***********************************************************
COMMIT RELEASE!
