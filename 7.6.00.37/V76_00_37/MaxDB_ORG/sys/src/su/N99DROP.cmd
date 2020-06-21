*   *ID* N99DROP  DBCMD    changed on 1992-09-28-13.27.24 by ADMIN     *
file connect ( kern test !
create table t ( a char (7))!
insert  t set a = 'aaa'!
parsing!
declare erg cursor for
select * from t!
pars_execute!
drop parseid!
1
parsing !
declare erg cursor for
select * from t!
select * from t!
pars_execute!
drop parseid!
2
parsing !
select * from t!
executing!
select !
1
pars_execute!
drop parseid!
1
parsing !
select * from t!
close !
pars_execute !
drop parseid!
1
parsing!
select * from t!
executing!
select !
1
pars_execute!
close!
drop parseid!
1
parsing !
select * from t!
pars_execute !
drop table t !
commit work release !
