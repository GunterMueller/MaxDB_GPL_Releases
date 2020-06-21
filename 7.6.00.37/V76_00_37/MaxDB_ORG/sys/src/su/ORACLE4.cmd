file connect ( kern test sqlmode oracle !
data !
usage on !
c 8 'PROGC'
c 18 'TESTPROG'
c 18 'TEST'
c 18 ' '

parsing !
create table t1 (s1 char(10)) !
insert into t1 values (:a) !
create table t2 as select * from t1 !
select * from t2 !
pars_execute !
rollback !
usage off !
file sel_table !
commit release !
