*   *ID* A16JOIN1 DBCMD    changed on 1992-09-28-13.22.36 by ADMIN     *
file CONNECT ( KERN TEST !
create table t ( a char (12)) !
create index "t.a"
	on t( a ) !
insert t set a = 'ddd'!
create table t1 ( b char (16))!
insert t1 set b = 'ddd'!
declare erg cursor for 
select max(b) from t, t1 where t.a = 'sss'!
fetch erg!
close erg!
parsing!
declare erg cursor for 
select max(b) from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
fetch erg into :a!
close erg!
insert t set a = 'sss'!
declare erg cursor for 
select max(b) from t, t1 where t.a = 'sss'!
fetch erg!
close erg!
parsing!
declare erg cursor for 
select max(b) from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
fetch erg into :a!
close erg!
delete t where a = 'sss'!
declare erg cursor for 
select b from t, t1 where t.a = 'sss'!
close erg!
parsing!
declare erg cursor for 
select b from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
close erg!
rollback release !
pars_execute !
