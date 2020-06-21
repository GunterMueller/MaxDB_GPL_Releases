*   *ID* UNION12  DBCMD    changed on 1992-09-28-13.28.45 by ADMIN     *
file connect ( kern test !
create table tab1 ( t1c2 char(5) ascii,
                    t1c3 fixed(6,2),
                    t1c4 float,
                    t1c5 char(5) byte )!
commit work !
insert tab1 set t1c2 = '121ab',
                t1c3 = 113.01,
                t1c4 = 114.01,
                t1c5 = X'f1f5f18182' !
insert tab1 set t1c2 = '122ab',
                t1c3 = 213.02,
                t1c4 = 214.02,
                t1c5 = X'f1f5f28182' !
insert tab1 set t1c2 = '123ab',
                t1c3 = 313.03,
                t1c4 = 314.03,
                t1c5 = X'f1f5f38182' !
insert tab1 set t1c2 = '124ab',
                t1c3 = 413.04,
                t1c4 = 414.04,
                t1c5 = X'f1f5f48182' !
commit work!
select * from tab1 !
fetch !
create view v1 as select * from tab1 union all select * from tab1 !
select * from v1 !
fetch !
create view v2 as select * from tab1 where t1c3 < 300 union all
                  select * from tab1 where t1c3 > 300 !
select * from v2 !
fetch !
select t1c2 from v1 except all select t1c2 from v2 !
fetch pos (1)!
fetch pos (2)!
fetch pos (3)!
fetch pos (4)!
rollback release !
