*   *ID* N99A23   DBCMD    changed on 1992-09-28-13.27.17 by ADMIN     *
file connect ( kern test !
* Fehlerprotokoll 95, gefunden von Guenter in Rel.2.4.06 !
create table t ( a char (8), b char (8), c char (8))!
create index "t.c"
	on t( c ) !
create index i1 on t (a, b, c) !
insert t values ('aaa', 'b', '123')!
insert t values ('aaa', 'b', '234')!
insert t values ('aaa', 'b', '345')!
create table t1 ( b1 char (8))!
insert t1 set b1 = 'b' !
create index "t1.b1"
	on t1( b1 ) !
explain select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c between '123' and '234') !
select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c between '123' and '234') !
fetch!
explain select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c = '123' or c = '234') !
select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c = '123' or c = '234') !
fetch!
drop table t !
drop table t1 !
commit release !
