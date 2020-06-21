*   *ID* SUB1     DBCMD    changed on 1992-09-28-13.28.15 by ADMIN     *
file connect ( kern test !
create table ts ( a char (8))!
insert ts set a = 'aaa'!
* 31 subqueries !
select * from ts where
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) !
* 32 subqueries !
select * from ts where
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) !
* 33 subqueries !
select * from ts where
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) and
a in (select a from ts) !
* call null_result when kb_enough_resultsets may come !
create table x ( a char (8))!
insert x set a = 'aaa'!
create table x1 (a1 char (8))!
insert x1 set a1 = 'bbb'!
select min(a) from x, x1 where a = a1 !
select * from x where exists (select min(a1) from x1 where a1 = x.a)!
fetch!
rollback release !
