*   *ID* N17CHAN3 DBCMD    changed on 1992-09-28-13.25.26 by ADMIN     *
 *** 'CHANGE USER' wird getestet.
 *** Vor diesem File muss 'N17CHAN1 DBCMD' und
 *** 'N17CHAN2 DBCMD' aufgerufen werden.
 ***********************************************************!
file connect (d1 d1_w1 !
alter user d1_r1 resource !
commit work release !
file connect (d1_r1 d1_r1_w1 !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
rollback work release !
file connect (d1 d1_w1 !
alter user d1_r1 standard !
commit work release !
file connect (d1_r1 d1_r1_w1 !
file sel_table !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
rollback work release !
file connect (d1_r2 d1_r2_w1 !
select * from d1_r1.d1r1t1 !
rollback work release !
 ***********************************************************!
file connect (d2 d2_w1 !
alter user d2_p1 resource !
commit work release !
file connect (d2_p1 w !
file sel_table !
delete  t2 key t2_c1 = 'Paul' !
insert into r2.r2_t1 set r2_t1_c1 = -  05.0 , r2_t1_c2=88.!
rollback work release !
 ***********************************************************!
file connect ( sut sut !
alter user d1 resource !
commit work release !
file connect (d1 d1_w1 !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
select * from p3.t1 where t1_c1 = 'Meier' !
fetch !
drop table t1 !
insert into d1_pt1 set d1_pt1_c2 = 'Holger' !
alter table d1_pt1 add d1_pt1_c3 fixed (18,12) !
rollback work release !
file connect ( sut sut !
revoke insert on d1_pt1 from d1 !
alter user d1 dba !
commit work release !
file connect (d1 d1_w1 !
insert into d1_pt1 values  ( 'Hans',  'Frieda' ) !
select d1_pt1_c1 from d1_pt1 where d1_pt1_c1='Hans' !
fetch !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !

file sel_user !
rollback work release !
 ***********************************************************!
file connect ( sut sut !
alter user d2 standard !
commit work release !
file connect (d2 d2_w1 !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
delete d1_pt1 where d1_pt1_c1 = ' Hans ' !
insert into r2.r2_t1 values (+ 05, -.220 ) !
select * from r2.r2_t1 !
rollback work release !
file connect (r2 r2_w1 !
insert into d2.d2_t1 set d2_t1_c1 = 888,d2_t1_c2 =-888 !
file sel_table !
rollback work release !
 ***********************************************************!
file connect ( sut sut !
alter user r1 resource !
commit work release !
file connect (r1 r1_w1 !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
rollback work release !
file connect ( sut sut !
alter user r1 dba !
commit work release !
file connect (r1 r1_w1 !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
update d1_pt1 set d1_pt1_c2 = 'Friedel' !
select * from d1_pt1 !
fetch !
file sel_table !
rollback work release !
 ***********************************************************!
file connect ( sut sut !
alter user r2 standard !
commit work release !
file connect (r2 r2_w1 !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
file sel_table !
select * from d2.d2_t1 !
rollback work release !
file connect (d1 d1_w1 !
insert into r2.r2_t1 values ( 0.0, +333.03 ) !
rollback work release !
file connect (d1_r2 d1_r2_w1 !
delete r2.r2_t1 key r2_t1_c1 = 5 !
rollback  work release !
file connect (d2_p1 w !
delete from r2.r2_t1 where r2_t1_c1 =+5 !
rollback work release !
 ***********************************************************!
file connect ( sut sut !
alter user p1 resource !
commit work release !
file connect (p1 p1_w1 !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
update of d1_pt1 set d1_pt1_c2 = 'Lisa ' !
insert into d1_r2.d1r2t1 values ( +12345678.0, 0.000430 ) !
rollback work release !
 ***********************************************************!
file connect ( sut sut !
alter user p2 dba !
commit work release !
file connect (p2 p2_w1 !
file sel_table !
select d1r2t2_c2, d1r2t2_c1 from d1_r2.d1r2t2 !
fetch !
file sel_priv_granted ( P3 !
file sel_priv_granted ( D1 !
file sel_priv_granted ( D2 !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( P1 !
file sel_priv_granted ( P2 !
file sel_priv_granted ( D1_R1 !
file sel_priv_granted ( D1_R2 !
file sel_priv_granted ( D1_P1 !
file sel_priv_granted ( D1_P2 !
file sel_priv_granted ( D2_R1 !
file sel_priv_granted ( D2_R2 !
file sel_priv_granted ( D2_P1 !
file sel_priv_granted ( D2_P2 !
rollback work release !
 ***********************************************************!
