*   *ID* N16CHAN4 DBCMD    changed on 1992-09-28-13.25.24 by ADMIN     *
 *** Getestet wird 'ALTER USER'.
 *** Alle 'ALTER USER'-Befehle sind richtig.
 *** Initialisierung durch 'N16CHAN1 VDNMAC'.
 **********************************************************!
file connect (d1 d1_w1 !
alter user d1_r1 resource !
file sel_user1 ( D1_R1 !
create user d1_r1 password w !
commit work release !
file connect (d1_r1 d1_r1_w1 !
insert into d1r1t2 values ( -0.0e+00, + 0.e-0 ) !
create table d1_r1_falsch (falsch_c1 float(6)) !
commit work release !
file connect (d1 d1_w1 !
alter user d1_r1 standard !
file sel_user1 ( D1_R1 !
commit work release !
file connect (d1_r1 d1_r1_w1 !
file sel_table !
insert into d1_r1.d1r1t1 values ( 88, 'Monika' ) !
rollback work release !
file connect (d1 d1_w1 !
alter user d1_r1 resource !
commit work release !
file connect (d1_r1 d1_r1_w1 !
file sel_table !
insert into d1_r1.d1r1t1 values ( 88 , 'Monika' ) !
rollback work release !
 ***********************************************************!
file connect (d2 d2_w1 !
alter user d2_p2 resource !
alter password d2_p2 to d2_p2_w2 !
commit work release !
file connect (d2_p2 d2_p2_w1 !

file sel_user !
create table d2p2t1 ( d2p2t1_c1 fixed ( 5,3 ) key,
                        d2p2t1_c2 float (3) not null ) !
insert into d2p2t1 values ( 22.123 , 111 ) !
grant selupd on d2p2t1 to d1_p1 !
commit work release !
file connect (d2 d2_w1 !
alter user d2_p2 standard !
file sel_user1 ( D2_P2 !
commit work release !
file connect (d2_p2 d2_p2_w1 !
*file connect (d2_p2 d2_p2_w2 !
file sel_priv_granted ( PUBLIC !
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
select * from d2p2t1 !
rollback work release !
 **********************************************************!
file connect ( sut sut !
alter user d1_p1 dba !
file sel_user1 ( D1_P1 !
commit work release !
file connect (d1_p1 d1_p1_w1 !
create user d1_p1_p1 password w resource !
rollback work release !
file connect (d1 d1_w1 !

file sel_user !
alter user d1_p1 standard !
rollback work release !
 **********************************************************!
file connect ( sut sut !
alter user d2_r1 standard !
file sel_user1 ( D2_R1 !
commit work release !
file connect (d2_r1 d2_r1_w1 !
file sel_table !

file sel_user !
rollback work release !
file connect (d2 d2_w1 !
alter user d2_r1 resource !

file sel_user !
rollback work release !
 **********************************************************!
file connect ( sut sut !
alter user d1 dba !
file sel_user1 ( D1 !
commit work release !
file connect (d1 d1_w1 !
file sel_table !
rollback work release !
 **********************************************************!
file connect ( sut sut !
alter user d1 resource !
file sel_user1 ( D1 !
commit work release !
file connect ( d1 d1_w1 !
create user d1_falsch password w !

file sel_user !
file sel_priv_granted ( PUBLIC !
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
insert into d1_pt1 values ( 'a','b' ) !
insert into d1_t1 set d1_t1_c1 = 0.0, d1_t1_c2=-5 !
grant select on d1_pt1 to d1_r1 !
rollback work release !
file connect ( sut sut !
alter user d1 dba !
file sel_priv_granted ( PUBLIC !
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
revoke all on d1_pt1 from d1 !
grant all on d1_pt1 to d1 !
commit work release !
 *****************************************************!
file connect ( sut sut !
alter user d2 standard !
file sel_user1 ( D2 !
commit work release !
file connect (d2 d2_w1 !
create table d2_falsch ( spalte char(3)) !
select * from d2_pt2 !
fetch !
delete d2_t1 where d2_t1_c2 = 3.5 !
rollback work release !
file connect ( sut sut !
alter user d2 dba !

file sel_user !
commit work release !
file connect (d2 d2_w1 !

file sel_user !
rollback work release !
 ****************************************************!
file connect ( sut sut !
alter user r1 resource !

file sel_user !
commit work release !
file connect (r1 r1_w1 !
create user r1_falsch password w !
create table r1_t3 ( r1_t3_c1 char(200) key ) !
file sel_table !
rollback work release !
file connect ( sut sut !
alter user r1 dba !
file sel_user1 ( R1 !
commit work release !
file connect (r1 r1_w1 !
file sel_table !
create table r1_pt1 ( r1_pt1_c1 fixed (5,5)) !
rollback work release !
 ****************************************************!
file connect ( sut sut !
alter user r2 standard !
file sel_user1 ( R2 !
commit work release !
file connect (r2 r2_w1 !
drop table r2_t1 !
rollback work release !
 ****************************************************!
file connect ( sut sut !
alter user p1 dba !
commit work release !
file connect (p1 p1_w1 !
file sel_table !
create user p1_p1 password w !

file sel_user !
create table p1_t1 ( p1_t1_c1 char (5)  key ,
                        p1_t1_c2 fixed(1,1)) !
insert into p1_t1 set p1_t1_c1 = 'ok_ko' !
rollback work release !
 *****************************************************!
file connect ( sut sut !
alter user p2 resource !
commit release !
file connect ( kern test !
file sel_user1 ( P2 !
commit work release !
file connect (p2 p2_w1 !

file sel_user !
create table p2_t1 ( p2_t1 float (1) key ) !
file sel_table !
rollback work release !
 ******************************************************!
file connect (d1_r2 d1_r2_w1 !
file sel_table !
insert into d1r2t1 values ( 30 , +00045 ) !
rollback work release !
 ******************************************************!
file connect ( d2_p1 w !
file sel_table !
file sel_priv_granted ( PUBLIC !
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
