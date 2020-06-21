*   *ID* N22UPD7  DBCMD    changed on 1992-09-28-13.25.34 by ADMIN     *
file connect ( kern test !
drop table tab !
create table tab ( s1 char(10) key, s2 char(10) key,
                   s3 fixed(5) key , s4 fixed(5) ) !
insert tab values ( 'aaaa', 'bbb', 5, 4 ) !
insert tab values ( 'cccc', 'aaa', 1, 1 ) !
insert tab values ( 'bbbb', 'bbb', 6, 5 ) !
update of tab
   set s4 = 1
   key s1 = 'cccc', s2 = 'aaa', s3 = 1
   where s4 <> 1 !
drop table tab !
 * KEYUPDATE          !
create table t (
k1 char (8) key,
k2 char (8) key,
k3 char (8) key,
k4 char (8) key,
k5 char (8) key,
k6 char (8) key,
k7 char (8) key,
k8 char (8) key,
f1 char (3)) !
insert t values (
'11111111',
'22222222',
'33333333',
'44444444',
'55555555',
'66666666',
'77777777',
'88888888', 'abc') !
create index i on t (k5, k6, k7, k8)!
commit !
pars_then_ex !
select k1, k2, k3, k4, k5, k6, k7, k8 from t !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i !
update t set k3 = 'neu' !
select k1, k2, k3, k4, k5, k6, k7, k8 from t !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i !
update t set k7 = 'auchneu' !
select k1, k2, k3, k4, k5, k6, k7, k8 from t !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i !
pars_execute !
drop table t !
commit !
CREATE TABLE TAB (SCHL1 CHAR (10) KEY,
SCHL2 CHAR (10) KEY, FELD CHAR (10))!
INSERT TAB SET SCHL1 = 'abc', SCHL2 = 'xyz', FELD = '123'!
UPDATE TAB SET SCHL1 = 'cba'!
UPDATE TAB SET SCHL2 = 'zyx' key schl1 = 'cba', schl2 = 'xyz'!
SELECT * FROM TAB!
FETCH!
CREATE INDEX "TAB.SCHL2"
	on TAB( SCHL2 ) !
UPDATE TAB SET SCHL2 = 'falsch' key schl1 = 'cba', schl2 = 'zyx'!
SELECT * FROM TAB!
FETCH!
ROLLBACK RELEASE!
file connect ( kern test !
create table t ( a char (8) key, b char (8) key,
f1 char (8),
f2 char (8),
f3 char (8),
f4 char (8),
f5 char (8))!
insert t values ('aaa', 'bbb', '111', '222', '333', '444', '555')!
create index "t.f1"
	on t( f1 ) !
create index "t.f2"
	on t( f2 desc ) !
create unique index "t.f3"
	on t( f3 ) !
create unique index "t.f4"
	on t( f4 desc ) !
 * Fehler 250 darf NICHT kommen !
update t set b = 'ttt' key a = 'aaa', b ='bbb' !
update t set b = 'neu' where f4 = '444'!
update t set b = 'qqq' where f5 = '555'!
rollback release !
 * konsequentes Durchtesten von unique Indizes bei ggf. Keyupdate !
file connect ( kern test !
create table t ( a char (8) key,
                 b char (8) key,
                 c char (8) key,
f1 char (8),
f2 char (8))!
insert t values ('aaa', 'bbb', 'ccc', 'anfang', '222')!
create unique index "t.b"
	on t( b ) !
commit !
 * keyupdate with unique index on keycolumn !
update t set a = 'neu' where f2 = '222'!
rollback !
update t set b = 'neu' where f2 = '222'!
rollback !
 * no real keyupdate !
update t set a = 'aaa' where f2 = '222'!
rollback !
update t set b = 'bbb' where f2 = '222'!
rollback !
 * no real keyupdate, but other column is updated !
update t set a = 'aaa', f1 = 'auchneu' where f2 = '222'!
rollback !
update t set b = 'bbb', f1 = 'auchneu' where f2 = '222'!
rollback !
 * sometimes error 250 has to come, sometimes not !
insert t values ('aaa', 'a1', 'a2', 'anfang1', '222')!
create unique index "t.f1"
	on t( f1 ) !
commit !
 * keyupdate with unique index on keycolumn !
update t set a = 'neu' where f2 = '222'!
select * from t !
fetch!
rollback !
 *  Error 250 !
update t set b = 'neu' where f2 = '222'!
select * from t !
fetch !
rollback !
 * no real keyupdate for first update!
update t set a = 'aaa' where f2 = '222'!
select * from t !
fetch !
rollback !
update t set b = 'bbb' where f2 = '222'!
select * from t !
fetch!
rollback !
 * no real keyupdate, but other column is updated !
update t set a = 'aaa', f1 = 'auchneu' where f2 = '222'!
select * from t !
fetch!
rollback !
 * no real keyupdate, but other column is updated !
update t set a = 'aaa', f1 = 'auchneu' where f2 = '222' and c = 'a2'!
select * from t !
fetch!
rollback !
 * keyupdate for one row, but other column is updated in both rows!
update t set b = 'bbb', f1 = 'auchneu' where f2 = '222'!
select * from t !
fetch !
rollback !
drop table t !
commit release !
* PTS 1106468 !
file connect ( kern test !
create table t ( a fixed (17,5))!
insert t set a = 1234.5678!
pars_then_ex !
data!
update t set a = a + :a!
n 38,4 765.4322

select * from t where rowno < :a !
n 10 6

pars_execute !
nodata !
select * into :a from t !
drop table t!
commit release !
