*   *ID* T01DDL   DBCMD    changed on 1992-09-28-13.28.28 by ADMIN     *
 ***************************
 ***   T 0 1 D D L       ***
 ***************************!
file sel_cols_1tab ( KERN DEPT !
ALTER TABLE dept ADD budget FIXED (8,2)  !
commit work !
file sel_cols_1tab ( KERN DEPT !
SELECT deptno, deptname, locname, budget
  FROM dept !
FETCH !
close !
CREATE INDEX "dept.locname"
	on dept( locname ) !
commit work !
file sel_index_tab ( DEPT !
SELECT deptname, locname
  FROM dept
  WHERE ( locname = 'San Francisco' ) !
FETCH !
close !
DROP INDEX "dept.locname" !
commit work !
SELECT deptname, locname
  FROM dept
  WHERE locname = 'San Francisco' !
FETCH !
close !
ALTER TABLE dept DROP budget !
commit work !
create table t1 ( begin fixed (8))!
insert t1 set begin = 3 !
select * from t1 where -begin = -3 !
create table t2 ( nokeyword fixed (8))!
insert t2 set nokeyword = 3 !
select * from t2 where -nokeyword = -3 !
create domain begin char(8) default 'aaa'!
create table t ( a begin)!
alter table t column a begin !
rollback !
create domain deftest char(8) default 'aaa'!
create table t ( a char (7) default 'ddd')!
alter table t column a deftest !
rollback !
