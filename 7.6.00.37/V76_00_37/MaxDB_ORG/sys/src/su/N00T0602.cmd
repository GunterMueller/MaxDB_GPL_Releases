*   *ID* N00T0602 DBCMD    changed on 1992-09-28-13.24.52 by ADMIN     *
 *** 6.2  <rollback statement> !
file connect ( sut sut !
create user r5 password r5xw resource !
rollback work !
create user r5 password r5xw resource !
commit work release !
file connect ( r5 r5xw !
create table r5_t1 ( spalte1 char(5) key, spalte2 float(3) ) !
insert r5_t1 set  spalte2= 3, spalte1 = 'aa' !
insert r5_t1 set  spalte1 = 'cc', spalte2= 5 !
update r5_t1 set spalte2 = 6 where spalte2 = 5 !
delete r5_t1 where spalte2>5 !
commit work !
insert r5_t1 set  spalte1 = 'bb', spalte2 = 4 !
rollback !
select spalte1, spalte2 from r5_t1 !
insert r5_t1  set spalte1 = 'bb', spalte2 = 4 !
rollback !
insert r5_t1 set spalte1 = 'bb', spalte2 = 4 !
rollback !
select spalte1 from r5_t1 !
fetch !
rollback work release !
