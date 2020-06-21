file connect ( kern test !
create table t ( a char (8), b fixed (8))!
insert t set a = 'aaa', b = 4!
create table t1 (a1 char (12), b1 fixed (7))!
pars_then_ex !
data !
insert into t1 values (:a, 123) !
c 12 'xyz'

insert into t1 select substr (:a, 2), 567 from t!
c 254 'rst' 

insert into t1 select (substr(:a,2), 678) from t !

insert into t1 select substr(:a,2), 789 from dual!
c 254 'rrr'

insert into t1 values (substr(:a, 2), 789) !
c 254 'sst'

insert into t1 values (substr(:a,2), :a)!
c 254 'ttt'
n 20 789

insert into t1 select :a , 567 from t! 
c 254 'uuu'

 * Fehler 100, weil das (select 'der'...) einen Satz liefert und damit
 * ... where not exists scheitert. !
insert into t1 select :a, 567 from t where not exists
    (select 'der' from t)!
c254 'vvv'

 * Fehler -7016, weil das (select :b ...) scheitert !
insert into t1 select :a, 567 from t where not exists (select :b from t)!

nodata !
pars_execute! 
select * from t1 !
fetch !
rollback release !
