*   *ID* A03DEF   DBCMD    changed on 1992-09-28-13.22.17 by ADMIN     *
file connect ( kern test !
create table t ( a char (8) default 'kern',
                 b char (8),
                 c char (25) default 'longkern')!
insert t values ('a1', default, 'long')!
insert t values (default, 'b2', 'long')!
insert t set a = 'a3', b = default!
insert t set a = default, b = 'b4'!
insert t set b = 'b5'!
pars_then_ex !
insert t values (default, :b)!
pars_execute !
select * from t!
fetch !
update t set a = 'new' !
select * from t!
fetch !
commit !
update t set a = default !
select * from t !
fetch !
rollback !
update t set b = default !
select * from t !
fetch !
rollback !
update t set c = default !
select * from t !
fetch !
rollback !
pars_then_ex !
data !
update t set b = :b, a = default !
c 8 'b7'

pars_execute !
nodata !
select * from t!
fetch !
diagnose share parse drop !
drop table t!
commit release !
file connect ( kern test !
* PTS 1122484 !
create table t ( k int,
c_date     date        default date,
c_utcdate  timestamp   default utcdate,
c_utcdiff  fixed (4,2) default utcdiff,
c_timezone fixed(6)    default timezone,
c_sysdba   char (32)   default sysdba)!
select COLUMNNAME, "DEFAULT", DEFAULTFUNCTION from columns 
 where owner = 'KERN' and tablename = 'T' !
fetch!
insert into t (k) values (1)!
pars_then_ex !
insert into t (k) values (2)!
pars_execute!
select * from t !
fetch!
create table t1 (c_utcdate  time        default utcdate)!
create table t2 (c_utcdiff  fixed (4,3) default utcdiff)!
create table t3 (c_timezone fixed(5)    default timezone)!
create table t4 (c_sysdba   char (30)   default sysdba)!
rollback release !