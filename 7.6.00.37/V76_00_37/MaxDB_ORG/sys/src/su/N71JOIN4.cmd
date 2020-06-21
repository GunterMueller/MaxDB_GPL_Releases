*   *ID* N71JOIN4 DBCMD    changed on 1992-09-28-13.27.08 by ADMIN     *
file CONNECT ( KERN TEST !
create table t (k char (8) key, a char (6))!
insert t set k = 'first' , a = 'test'!
insert t set k = 'second', a = 'test'!
insert t set k = 'aa    ', a = 'test'!
create index "t.a"
	on t( a ) !
create table t1 (b char (6) )!
commit !
insert t1 set b = 'second' !
select * from <t1, t> where
    t1.b = t.k and
    t.k < 'rest' !
fetch !
rollback !
insert t1 set b = 'test' !
select * from <t1, t> where
    t1.b = t.a and
    t.k < 'rest' and t.k >'anfang' !
fetch !
rollback !
drop table t !
drop table t1 !
commit release !
