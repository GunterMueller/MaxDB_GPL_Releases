*   *ID* N44INDX2 DBCMD    changed on 1992-09-28-13.26.33 by ADMIN     *
file connect ( kern test !
create table tab ( spalte01 char(10),
                   spalte02 char(10),
                   spalte03 char(10),
                   spalte04 char(10),
                   spalte05 char(10),
                   spalte06 char(10),
                   spalte07 char(10),
                   spalte08 char(10),
                   spalte09 char(10),
                   spalte10 char(10),
                   spalte11 char(10),
                   spalte12 char(10),
                   spalte13 char(10),
                   spalte14 char(10),
                   spalte15 char(10),
                   spalte16 char(10),
                   spalte17 char(10),
                   spalte18 char(10),
                   spalte19 char(10),
                   spalte20 char(10) ) !
insert tab values ( 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't' ) !
insert tab values ( 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u' ) !
insert tab values ( 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't' ) !
insert tab values ( 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
                  'm', 'n', 'o', 'p', 'q', 'r', 's', 't', null, null ) !
insert tab values ( 'a', 'b', 'c', null, 'e', 'f', 'g', 'h', 'i',
              null, 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v' ) !
insert tab values ( 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                    null, 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w' ) !
insert tab set spalte01 = null !
commit work !
create index indexa on tab ( spalte01, spalte03, spalte04, spalte05,
    spalte06, spalte08, spalte09, spalte10, spalte11, spalte12, spalte13,
    spalte14, spalte15, spalte16, spalte17, spalte18, spalte19 ) !
create index index1 on tab ( spalte01, spalte03, spalte04, spalte05,
    spalte06, spalte08, spalte09, spalte10, spalte11, spalte12, spalte13,
    spalte14, spalte15, spalte16, spalte17, spalte18 ) !
commit work !
select spalte01, spalte03, spalte04 from tab order by
    spalte01, spalte03, spalte04, spalte05,
    spalte06, spalte08, spalte09, spalte10, spalte11, spalte12, spalte13,
    spalte14, spalte15, spalte16, spalte17, spalte18 !
fetch !
create index index2 on tab
    ( spalte02 desc, spalte03 desc, spalte05 desc, spalte06 desc,
      spalte07 desc, spalte08 desc, spalte09 desc, spalte10 desc,
      spalte11 desc, spalte12 desc, spalte13 desc, spalte14 desc,
      spalte15 desc, spalte17 desc, spalte18 desc, spalte20 desc ) !
create index index3 on tab
    ( spalte02 desc, spalte03 asc , spalte05 desc, spalte06 asc ,
      spalte07 desc, spalte08 asc , spalte09 desc, spalte10 asc ,
      spalte11 desc, spalte12 asc , spalte13 desc, spalte14 asc ,
      spalte15 desc, spalte17 asc , spalte18 desc, spalte20 asc  ) !
commit work !
insert tab values ( 'dd', 'aa', 'ff', 'gg', 'rr', 'dd', 'aa', 'aa',
 'jj', 'nn', 'tt', 'gg', null,null,null, 'xx', 'zz', 'ss', 'ee', 'ss' )!
create index index4 on tab ( spalte05, spalte06 ) !
create index index5 on tab ( spalte18, spalte07 desc, spalte12 asc ) !
create index index6 on tab ( spalte03 desc, spalte01 desc ) !
create index index7 on tab ( spalte14 desc,spalte15 asc,
      spalte03,spalte02 ) !
commit work !
create index index8 on tab ( spalte15 ) !
create index index9 on tab ( spalte13 desc ) !
commit work !
create index index10 on tab (spalte20 desc,spalte19 asc,spalte03 asc) !
create index index11 on tab ( spalte03 asc, spalte04 asc ) !
create index index12 on tab ( spalte04 desc, spalte05, spalte07,
     spalte08, spalte13, spalte15 desc ) !
create index index13 on tab ( spalte01 desc, spalte06 desc ) !
commit work !
create index index14 on tab ( spalte09 ) !
create index index15 on tab (spalte15 desc,spalte16 asc,spalte18 desc) !
create index index16 on tab ( spalte04 desc, spalte06 desc ) !
create index index17 on tab ( spalte08 desc, spalte07 asc ) !
commit work !
insert tab set spalte05 = null !
insert tab values ( 'sss', 'ddd', null, null, null, 'kern',
   'kern', 'aaa',
   'bbb', 'aaa', 'fff', 'ggg' , 'hhh', 'ddd', 'fff', 'eee', 'rrr' ,
   null, null, null ) !
insert tab values ( 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't' ) !
select spalte01, spalte06 from tab
    order by spalte01 desc, spalte06 desc !
fetch !
select spalte15, spalte16, spalte18 from tab
   where spalte15 = 'q' and spalte16 = 'r' and spalte18 = 't' !
fetch !
* Fehlerprotokoll 96 !
select spalte01, spalte15 from tab where spalte15 = 'o'!
fetch!
pars_then_ex !
data !
select spalte01, spalte15 from tab where spalte15 = :p!
c 10 'o'

nodata !
fetch into :a, :b!
pars_execute !
update tab set spalte10 = 'kkkk'
    where spalte05 = 'g' and spalte06 = 'h' !
select spalte10, spalte05, spalte06 from tab
    where spalte05 = 'g' and spalte06 = 'h' !
fetch !
delete tab where spalte20 = 't' and spalte19 = 's' and spalte03 = 'c' !
select spalte20, spalte01, spalte06 from tab
    order by spalte01 desc, spalte06 desc !
fetch !
commit work !
drop index index10 !
commit work !
create index index10 on tab ( spalte05 desc, spalte07 asc ) !
rollback work !
create index index10 on tab ( spalte05 desc, spalte07 asc ) !
create index index17 on tab ( spalte04 asc ) !
drop table tab !
commit work release !
