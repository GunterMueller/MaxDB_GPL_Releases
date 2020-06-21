file connect (kern test!
CREATE TABLE SYSDDMOD
  (
   OWNER        CHAR(64) ASCII KEY DEFAULT USERGROUP, 
   MODULNAM1    CHAR(18) ASCII KEY DEFAULT ' ', 
   MODULNAM2    CHAR(18) ASCII KEY DEFAULT ' ', 
   PROGLANG     CHAR(18) ASCII KEY DEFAULT ' ', 
   OBJTYPE      CHAR(18) ASCII KEY DEFAULT 'MODULE', 
   SUBTYPE      CHAR(12) ASCII KEY DEFAULT ' ', 
   VERSION      CHAR(4)  ASCII KEY DEFAULT ' ',
   LONGNAME     CHAR(40) ASCII DEFAULT ' ', 
   TYPE         CHAR(18) ASCII DEFAULT ' ',
   CREATEDATE   DATE DEFAULT DATE,
   CREATETIME   TIME DEFAULT TIME, 
   UPDDATE      DATE DEFAULT DATE, 
   UPDTIME      TIME DEFAULT TIME,
   COMMENT      LONG
  )!
COMMIT WORK!
CREATE VIEW SYSDDMODV 
  (OBJTYPE, OWNER, NAME1, NAME2, NAME3, SUBTYPE, VERSION, COMMENT)
AS SELECT
   OBJTYPE, OWNER, MODULNAM1, MODULNAM2, PROGLANG, 
   SUBTYPE, VERSION, COMMENT
FROM SYSDDMOD! 
COMMIT WORK!
GRANT SELECT ON SYSDDMODV TO PUBLIC!
CREATE VIEW SYSDD_MOD
AS SELECT *
   FROM SYSDDMOD
   WHERE owner = USERGROUP!
COMMIT WORK!
GRANT ALL ON SYSDD_MOD TO PUBLIC!
CREATE VIEW SYSDDPRG
  (OWNER,PROGRAMNAME,NAME2,NAME3,OBJTYPE,SUBTYPE,VERSION,
   LONGNAME,CREATEDATE,CREATETIME,UPDDATE,UPDTIME,COMMENT)
AS SELECT
   OWNER,MODULNAM1,' ',' ','PROGRAM',SUBTYPE,VERSION,
   ' ',CREATEDATE,CREATETIME,UPDDATE,UPDTIME,COMMENT
   FROM SYSDDMOD!
COMMIT WORK!
CREATE VIEW SYSDDPRG_ 
  (OBJTYPE, OWNER, NAME1, NAME2, NAME3, SUBTYPE, VERSION, COMMENT)
AS SELECT
   OBJTYPE, OWNER, PROGRAMNAME, NAME2, NAME3, SUBTYPE, 
   VERSION, COMMENT
   FROM SYSDDPRG!
file sel_table !
ROLLBACK WORK RELEASE!

*PTS 1111379 !
*=======================================================!
 * normal behaviour !
 
file connect ( kern test!
create table t ( ta char (8))!
insert t values ('bbb')!
create table tab ( a char (8), b char (8) primary key)!
insert tab values ('aaa', 'bbb')!
commit !
create view klein1 as select a,b from tab where 
b <> 'i' !
create view gross1 as select max(a), b from t, klein1
where b = ta group by b !
commit release !

file connect ( kern test !
create view klein2 as select a,b from tab where 
b <> 'i' !
create view gross2 as select max(a), b from t, klein2
where b = ta group by b !
commit release !

file connect ( kern test !
drop view klein1 !
commit release !

file connect ( kern test !
pars_then_ex !
select * from gross2 !
pars_execute !
drop table t !
drop table tab !
commit release !

*=======================================================!
 * create view with post-option 'NO REFERENCES' !

file connect ( kern test!
create table t ( ta char (8))!
insert t values ('bbb')!
create table tab ( a char (8), b char (8) primary key)!
insert tab values ('aaa', 'bbb')!
commit !
create view klein1 as select a,b from tab where 
b <> 'i' no references!
create view gross1 as select max(a), b from t, klein1
where b = ta group by b !
commit release !

file connect ( kern test !
create view klein2 as select a,b from tab where 
b <> 'i' no references!
create view gross2 as select max(a), b from t, klein2
where b = ta group by b !
commit release !

file connect ( kern test !
drop view klein1 !
commit release !

file connect ( kern test !
pars_then_ex !
select * from gross2 !
pars_execute !
drop view klein2 !
drop table t !
drop table tab !
commit release !

*=======================================================!
 * create AND drop view with post-option 'NO REFERENCES' !
file connect ( kern test!
create table t ( ta char (8))!
insert t values ('bbb')!
create table tab ( a char (8), b char (8) primary key)!
insert tab values ('aaa', 'bbb')!
commit !
create view klein1 as select a,b from tab where 
b <> 'i' no references!
create view gross1 as select max(a), b from t, klein1
where b = ta group by b !
commit release !

file connect ( kern test !
create view klein2 as select a,b from tab where 
b <> 'i' no references!
create view gross2 as select max(a), b from t, klein2
where b = ta group by b !
commit release !

file connect ( kern test !
drop view klein1 no references !
commit release !

file connect ( kern test !
pars_then_ex !
select * from gross2 !
pars_execute !
drop table t !
drop table tab !
commit release !
