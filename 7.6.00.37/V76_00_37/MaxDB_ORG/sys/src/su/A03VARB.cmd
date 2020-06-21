*   *ID* A03VARB  DBCMD    changed on 1992-09-28-13.22.23 by ADMIN     *
* checkt das Verhalten beim insert betr. varcolcount-Byte !
file connect ( kern test !
create table t ( a char (8) key,
                 c char (35) )!
insert t values ('aaa1', 'ccc')!
pars_then_ex !
insert t values ('aaa2', 'ccc')!
data !
insert t values (:a, 'ccc')!
c 8 'aaa3'

insert t values (:a, :c)!
c 8 'aaa4' c 35 'ccc'

insert t values ('aaa5', :c)!
c 35 'ccc'

pars_execute !
nodata !
select * from t !
fetch!
drop table t!
commit release !
