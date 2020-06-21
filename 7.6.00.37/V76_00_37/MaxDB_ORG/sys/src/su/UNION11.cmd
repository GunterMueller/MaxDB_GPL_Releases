*   *ID* UNION11  DBCMD    changed on 1992-09-28-13.28.45 by ADMIN     *
file connect ( kern test !
create table tab1 ( t1c1 char(5) ,
                    t1c2 char(5) ascii,
                    t1c3 fixed(6,2),
                    t1c4 float,
                    t1c5 char(5) byte )!
create table tab4 ( t4c1 char(50) ,
                    t4c2 char(50) ascii,
                    t4c3 fixed(10,2),
                    t4c4 float,
                    t4c5 char(50) byte )!
insert tab1 set t1c1 = '111ab',
                t1c2 = '121ab',
                t1c3 = 113.01,
                t1c4 = 114.01,
                t1c5 = X'f1f5f18182' !
insert tab1 set t1c1 = '112ab',
                t1c2 = '122ab',
                t1c3 = 213.02,
                t1c4 = 214.02,
                t1c5 = X'f1f5f28182' !
insert tab1 set t1c1 = '113ab',
                t1c2 = '123ab',
                t1c3 = 313.03,
                t1c4 = 314.03,
                t1c5 = X'f1f5f38182' !
insert tab1 set t1c1 = '114ab',
                t1c2 = '124ab',
                t1c3 = 413.04,
                t1c4 = 414.04,
                t1c5 = X'f1f5f48182' !
insert tab4 set t4c1 = '411abcdefghij',
                t4c2 = '421abcdefghij',
                t4c3 = 4020123.01,
                t4c4 = 4020124.01,
                t4c5 = X'f4f5f18182' !
insert tab4 set t4c1 = '412abcdefghij',
                t4c2 = '422abcdefghij',
                t4c3 = 4020223.02,
                t4c4 = 4020224.02,
                t4c5 = X'f4f5f28182' !
insert tab4 set t4c1 = '413abcdefghij',
                t4c2 = '423abcdefghij',
                t4c3 = 4020323.03,
                t4c4 = 4020324.03,
                t4c5 = X'f4f5f38182' !
insert tab4 set t4c1 = '414abcdefghij',
                t4c2 = '424abcdefghij',
                t4c3 = 4020433.04,
                t4c4 = 4020434.04,
                t4c5 = X'f4f5f48182' !
select t4c1, t4c2, t4c1|| t4c2, t4c5, t4c4, t4c3 from tab4
  union select t1c1, t1c2, t1c2 || t1c1, t1c5, t1c3, t1c4 from tab1 !
fetch !
select t4c1, t4c2, t4c1|| t4c2, t4c5, t4c3, t4c4 from tab4
  union select t4c1, t4c2, t4c1 || t4c2, t4c5, t4c3, t4c4 from tab4 !
fetch !
declare erg1 cursor for select expand (t4c1, 200) || t4c2 from tab4
  union all select t1c1 from tab1 !
fetch erg1 !
declare erg2 cursor for select * from erg1
  union all select * from erg1 !
fetch erg2 !
declare erg1 cursor for select * from erg2
  union all select * from erg2 !
fetch erg1 !
declare erg2 cursor for select * from erg1
  union all select * from erg1 !
fetch erg2 !
declare erg1 cursor for select * from erg2
  union all select * from erg2 !
fetch erg1 !
rollback release !
