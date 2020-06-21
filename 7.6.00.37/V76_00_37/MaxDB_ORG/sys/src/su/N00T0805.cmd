*   *ID* N00T0805 DBCMD    changed on 1992-09-28-13.24.55 by ADMIN     *
 ***  8.7  <create domain statement> !
file connect ( sut sut !
create user d2 password d2xw dba !
commit release !
file connect ( kern test !
create domain dom_1 char(15)
   default 'defaultwert' range in ( 'defaultwert', 'Otto', 'Karl' ) !
commit work release !
file connect ( d2 d2xw !
create domain        dom_2 date !
create  domain  dom_3  time   default  '185901' !
create domain dom_4 fixed (5,2) default 0 !
create domain  dom_5  float(18) range between -2e12 and +2e12 !
create domain dom_6 character(10) ascii  range not in ( 'a') !
create user d2xr1 password d2xr1xw resource !
commit work release !
file connect ( d2xr1 d2xr1xw !
create table domaintab1
      ( spalte1 dom_2, spalte2 kern.dom_1 not null, spalte3 dom_5 ) !
file sel_cols_1tab ( D2XR1 DOMAINTAB1 !
insert domaintab1 values ( '19850706', 'Otto' , 0 ) !
insert domaintab1 set spalte2 = 'defaultwert' !
insert domaintab1 set spalte1 = date , spalte3 = 1E10 !
update domaintab1 set spalte3 = spalte3 + 1000 !
select spalte2, spalte1 from domaintab1 !
fetch !
commit work release !
file connect ( d2 d2xw !
create table domaintab2 ( spalte1  dom_6  key,
                          spalte2 fixed(5),
                          spalte3 dom_3 not null,
                          spalte4 dom_4 ) !
insert domaintab2 values ( 'aa' , null,time, 323.01 ) !
insert domaintab2 set spalte1 = 'bbb', spalte2 = 876 !
select * from domaintab2 where spalte1 > 'aaa' !
fetch !
commit work release !
