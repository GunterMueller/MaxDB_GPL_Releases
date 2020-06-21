*   *ID* A17SEL8  DBCMD    changed on 1992-09-28-13.22.48 by ADMIN     *
file connect ( kern test !
create table t (  a char (5) )!
insert t set a = 'aaaa'!
insert t set a = 'wwww'!
close erg0 !
fetch erg0 into :a!
declare erg0 cursor for 
select * from t !
fetch erg0 into :a!
declare erg0 cursor for 
select * from t !
fetch erg0 into :a!
close erg0 !
fetch erg0 into :a!
close erg0 !
declare erg0 cursor for 
select * from t!
fetch erg0 into :a!
 *******************************!
data !
usage on !
c 8 'PROGRAM'
c 18 'TESTPROG'
c 18 'PARSETEST'
c 18 ' '

nodata !
parsing !
close erg1 !
fetch erg1 into :a!
declare erg1 cursor for 
select * from t !
fetch erg1 into :a!
declare erg1 cursor for 
select * from t !
fetch erg1 into :a!
close erg1 !
fetch erg1 into :a!
close erg1 !
declare erg1 cursor for 
select * from t!
fetch erg1 into :a!
 *******************************!
pars_execute !
usage off !
pars_then_ex !
close erg2 !
fetch erg2 into :a!
declare erg2 cursor for 
select * from t!
fetch erg2 into :a!
declare erg2 cursor for 
select * from t !
fetch1 last erg2 into :a!
close erg2 !
fetch erg2 into :a!
close erg2 !
declare erg2 cursor for 
select * from t !
fetch erg2 into :a!
close erg2 !
select * from erg2 !
declare erg2 cursor for select * from t for reuse !
close erg2 !
select * from erg2 !
declare erg3 cursor for select * from t!
fetch erg3 into :a!
close erg3 !
pars_execute!
drop table t !
commit release !
 ********************************
 ********************************!
file connect ( kern test !
create table t ( a char (5) )!
insert t set a = 'aaaa'!
insert t set a = 'wwww'!
close !
fetch into :a!
select * from t !
fetch into :a!
select * from t !
fetch into :a!
close !
fetch into :a!
close !
select * from t!
fetch into :a!
commit release !
 *******************************!
file connect ( kern test !
data !
usage on !
c 8 'PROGRAM'
c 18 'TESTPROG'
c 18 'PARSETEST'
c 18 ' '

nodata !
parsing !
close !
fetch into :a!
select * from t !
fetch into :a!
select * from t !
fetch into :a!
close !
fetch into :a!
close !
select * from t!
fetch into :a!
pars_execute!
usage off !
commit release !
 *******************************!
file connect ( kern test !
pars_then_ex !
close !
fetch into :a!
select * from t!
fetch into :a!
select * from t !
fetch into :a!
close !
fetch into :a!
close !
select * from t !
fetch into :a!
close !
select * from t!
fetch into :a!
close !
pars_execute!
drop table t !
commit release !
