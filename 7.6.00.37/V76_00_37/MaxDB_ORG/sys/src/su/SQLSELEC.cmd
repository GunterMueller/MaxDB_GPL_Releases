file connect ( kern test sqlmode &1 !
create table t ( a char (8) )!
insert into t values ('aaa')!
insert into t values ('bbb')!
insert into t values ('ccc')!
select * from t !
fetch1 into :a !
commit work !
fetch1 into :a !
close !
select * from t !
fetch1 into :a !
rollback work !
fetch1 into :a !
close !
select * from t !
select * from t order by a desc !
fetch into :a !
close !
*******************!
declare erg cursor for select * from t !
declare erg cursor for select * from t order by a desc !
fetch erg into :a !
close erg !
*******************!
declare erg cursor for select * from t !
fetch1 erg into :a !
commit work !
fetch1 erg into :a !
close erg !
declare erg cursor for select * from t !
fetch1 erg into :a !
rollback work !
fetch1 erg into :a !
close erg !
declare erg cursor for select * from t !
declare erg cursor for select * from t order by a desc !
fetch erg into :a !
close erg !
*******************!
declare erg cursor with hold for select * from t !
fetch1 erg into :a !
commit work !
fetch1 erg into :a !
close erg !
declare erg cursor with hold for select * from t !
fetch1 erg into :a !
rollback work !
fetch1 erg into :a !
close erg !
declare erg cursor with hold for select * from t !
declare erg cursor with hold for select * from t order by a desc !
fetch erg into :a !
close erg !
commit work release !
file connect (kern test !
drop table t !
commit work release !
