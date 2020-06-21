*   *ID* N00T0811 DBCMD    changed on 1992-09-28-13.24.58 by ADMIN     *
 ***  8.17  <save catalog statement>, <restore catalog statement> !
file connect ( sut sut !
create user r password w resource !
create user p password w !
commit release !
file connect ( kern test !
create table tab ( spalte1 fixed(5) key,
                   spalte2 char(5) range between 'a' and 'f',
                   spalte3 char(5) ascii default 'ddd',
                   spalte4 float(3) not null ) !
create view w as select * from tab !
grant all on kern.tab to r !
grant insert, select on w to p !
commit work release !
file connect ( r w !
create view v as select * from kern.tab
   where spalte2 > 'd' and spalte3 <> 'ddd' with check option !
create view vv as select spalte1, spalte3, spalte4 from v !
create synonym syn1 for kern.tab !
create synonym syn2 for v !
create synonym syn3 for r.vv !
save catalog of syn2 into schema1 !
drop view v !
create view v as select spalte1, spalte2, spalte3, spalte4
   from kern.tab where spalte1 < 10 and spalte3 > 'a'
   with check option !
create synonym syn4 for schema1 !
restore catalog from schema1 !
insert syn3 values ( 4, 'ccc', 1E12 ) !
commit work release !
file connect ( kern test !
save catalog of kern.tab into schema2 !
drop table tab !
create table tab ( spalte1 fixed(5) key,
                   spalte2 char(10) ascii range between 'd' and 'h',
                   spalte3 char(5) ascii not null,
                   spalte4 fixed(5,2),
                   spalte5 char(5) ) !
grant all on tab to r !
restore catalog from schema2 !
commit work release !
file connect ( r w !
insert vv values ( 8, 'aaa', 23.2 ) !
insert syn1 values ( 4, 'e', 'cc', 1, null ) !
commit work release !
file connect ( p w !
insert kern.w set spalte1 = 5, spalte2 = 'f',
                      spalte3 = 'hhh', spalte5 = 'a' !
commit work release !
