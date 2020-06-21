file connect ( kern test !
create table t ( a char (8) key)!
insert t set a = 'aaa'!
insert t set a = 'bbb'!
insert t set a = 'ccc'!
commit!
insert t set a = 'ddd'!
connect kern identified by test isolation level  0 !
create table temp.thistime (nr int primary key, t time)!
insert into temp.thistime values (1, time)!
select * from t where a = 'ddd'!
insert into temp.thistime values (2, time)!
set isolation level 3 !
insert into temp.thistime values (3, time)!
select * from t where a = 'ddd'!
* around midnight the select will not be ok without 1. qualification !
select timediff (time, t), nr, t, time
from temp.thistime
where time > '00001500' and
         (timediff (time, t) < '00000230' or
          timediff (time, t) > '00000400' )!
fetch !
drop table temp.thistime!
commit release !
* dieses Theater, um mit 2 usertasks auszukommen.
* Ansonsten muessten Warteschleifen rein, bis session timeout(900)
* abgelaufen !
dbmcli superdba,colduser!
util_connect sut,sut!
db_admin !
bye!
file restart !
connect kern identified by test !
create table thistime (nr int primary key, t time)!
insert into thistime values (1, time)!
commit !
insert t set a = 'ddd'!
connect kern identified by test isolation level  0 !
insert into thistime values (2, time)!
parsing !
select * from t where a = 'ddd' with lock isolation level 0 !
executing !
select * !
1
pars_execute !
insert into thistime values (3, time)!
set isolation level 3 !
insert into thistime values (4, time)!
executing !
select * !
1
pars_execute !
select timediff (time, t), nr, t, time
from thistime
where timediff (time, t) > '00000030' !
fetch !
drop table thistime !
commit release !
* dieses Theater, um mit 2 usertasks auszukommen.
* Ansonsten muessten Warteschleifen rein, bis session timeout(900)
* abgelaufen !
dbmcli superdba,colduser!
util_connect sut,sut!
db_admin !
util_connect superdba,COLDUSER!
util_release!
bye!
file restart !
connect kern identified by test !
create table thistime (nr int primary key, t time)!
insert into thistime values (1, time)!
commit !
insert t set a = 'ddd'!
connect kern identified by test isolation level  0 !
insert into thistime values (2, time)!
parsing !
select * from t where a = 'ddd' !
executing !
select * !
1
pars_execute !
insert into thistime values (3, time)!
pars_then_ex !
set isolation level 3 !
pars_execute !
insert into thistime values (4, time)!
executing !
select * !
1
pars_execute !
select timediff (time, t), nr, t, time
from thistime
where timediff (time, t) > '00000030' !
fetch !
drop table thistime !
commit release!
dbmcli superdba,colduser!
util_connect sut,sut!
db_admin!
util_connect superdba,COLDUSER!
util_release!
bye!
file restart !
file shutdwnk!
vdnexit!
