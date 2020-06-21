*   *ID* A16SEL1  DBCMD    changed on 1992-09-28-13.22.38 by ADMIN     *
file CONNECT ( KERN TEST !
create table t ( a char (6)) !
create index "t.a"
	on t( a ) !
insert t set a = 'ddd'!
create table t1 ( b char (8))!
insert t1 set b = 'ddd'!
declare erg cursor for 
select max(b) from t, t1 where t.a = 'sss'!
fetch erg!
close erg!
parsing!
declare erg cursor for 
select max(b) from t, t1 where t.a = :b!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
fetch erg into :a!
close erg!
declare erg cursor for 
select b from t,t1 where t.a = 'sss'!
close erg!
parsing!
parse declare erg cursor for 
select b from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
close erg!
pars_execute !
drop table t!
drop table t1!
commit release !
