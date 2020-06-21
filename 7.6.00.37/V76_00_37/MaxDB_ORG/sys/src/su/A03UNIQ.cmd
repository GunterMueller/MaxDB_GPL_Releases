file connect ( kern test !
create table t ( a char (8) key, b fixed (1), c fixed (5))!
create unique index "t.b"
	on t( b ) !
create table t1 ( a1 char (8) key, b1 fixed (1), c fixed (5))!
insert t values ('aaa', 1, 1)!
insert t values ('bbb', 1, 2)!
insert t1 values ('bbb', 1, 3)!
select * from t !
fetch!
insert t select * from t1 update duplicates !
select * from t !
fetch!
delete t1 !
insert t1 values ('aaa', 1, 3)!
insert t select * from t1 update duplicates !
select * from t !
fetch!
drop table t!
drop table t1!
commit release!
