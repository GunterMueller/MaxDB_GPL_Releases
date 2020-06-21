*   *ID* N99INDEX DBCMD    changed on 1992-09-28-13.27.25 by ADMIN     *
file connect ( kern test !
create table t ( a char (7) key, b char (5))!
create index "t.b"
	on t( b ) !
insert t values ('aaa', 'bbb')!
insert t values ('a1', 'bbb')!
create view v as select a from t!
delete v key a = 'aaa'!
select * from t where b = 'bbb'!
fetch!
rollback work release !
