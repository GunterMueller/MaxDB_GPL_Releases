file connect ( kern test sqlmode &1 !
create table t ( a decimal(5), b char (8)) !
insert      t        values (1, 'abc')!
insert into t        values (2, 'abc')!
insert into t (b, a) values ('abc', 3)!
insert into t set a = 4, b = 'abc' !
insert into t (b, a) values (substr('aabc', 2) , 5)!
insert into t (b, a) values (substr('aabc', 2) , default)!
insert into t (b, a) values (substr('aabc', 2) , null)!
insert into t (b, a) values (substr('aabc', 2) , stamp)!
create table t1 (a decimal (5), b char (8))!
insert into t1 select * from t !
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
declare ct1 cursor for select * from t1!
fetch ct1 into :a, :b !
close ct1 !
update    t1 set b = 'xyz' where a = 3 !
update of t set b = 'def'!
update    t set b = 'def'!
update    t set b = 'ghi' where a < 3 !
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
commit work !
update    t set b = (select b from t1 )!
update    t set b = (select b from t1 where a = 2)!
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
update    t set b = (select b from t1 where a = t.a)!
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
rollback work !
update    t set  b  =      ('jkl') where a = 2 !
update    t     (b) values ('mno') where a = 3 !
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
commit work !
update    t set (a, b) = (select a,b from t1 where a = 2) !
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
rollback work !
update    t set  a, b  = (22, 'pqr') where a = 2 !
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
rollback work !
delete from t where a <  3 !
delete      t where a >= 3 !
declare ct cursor for select * from t!
fetch ct into :a, :b !
close ct !
commit work release !
file connect (kern test !
drop table t !
drop table t1!
commit work release !
