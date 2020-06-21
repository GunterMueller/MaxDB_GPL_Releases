*   *ID* H01INIT  DBCMD    changed on 1992-09-28-13.23.51 by ADMIN     *
connect sut identified  by sut !
create user  d1 password w dba !
create user r1 password w resource !
create table sut.t1 ( t1xc1 fixed(16,2) key,
                         t1xc2 char(15) ) !
insert sut.t1 values ( 812.7 , 'BERLIN' ) !
insert sut.t1 values ( 1.12  , 'HAMBURG' ) !
grant all on sut.t1 to d1 !
commit work release !
 ***********************************************************!
connect d1 identified by w !
create table d1xt1 ( d1xt1xc1 char(15 ),
                     d1xt1xc2 char( 15) not null
                         range between 'A' and 'M' ) !
insert d1xt1 values ( 'HANS', 'AAA' ) !
insert d1xt1 values ( 'OTTO',  'BBB' ) !
commit work release !
 ***********************************************************!
connect r1 identified by w !
create table r1xt1 ( r1xt1xc1 float ( 5 ) ) !
insert r1xt1 values ( 88E15 ) !
insert r1xt1 values ( -82   ) !
grant all on r1xt1 to public !
commit work release !
