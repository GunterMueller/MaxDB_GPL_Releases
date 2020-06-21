*   *ID* N16CHAN1 DBCMD    changed on 1992-09-28-13.25.22 by ADMIN     *
 *** 'ALTER USER'
 ****************************************************!
file connect ( sut sut !
create user d1 password d1_w1 dba !
create user d2 password d2_w1 dba !
create user r1 password r1_w1 resource !
create user r2 password r2_w1 resource !
create user p1 password p1_w1 !
create user p2 password p2_w1 !
commit work release !
file connect ( kern test !
create table t1     ( t1_c1 char(10) key,
                             t1_c2 fixed(3)        ) !
create table t2     ( t2_c1 char(12) key,
                             t2_c2 float(5)        ) !
insert into t1 values ( 'Mueller' , 18 ) !
insert into t1 values ( 'Meier' , 235 )  !
insert into t2 values ( 'Otto', 256 )    !
insert into t2 set t2_c1 = 'Paul'        !
commit work  release !
 *****************************************************!
file connect ( d1 d1_w1 !
create user d1_r1 password d1_r1_w1 resource !
create user d1_r2 password d1_r2_w1 resource !
create user d1_p1 password d1_p1_w1 !
create user d1_p2 password d1_p2_w1 !
create table d1_pt1 ( d1_pt1_c1 char(10),
                             d1_pt1_c2 char(10) ) !
create table d1_pt2     ( d1_pt2_c1 float(3) key,
                                 d1_pt2_c2 fixed(5,2) ) !
create table d1.d1_t1     (d1_t1_c1 fixed (5,3) key,
                           d1_t1_c2 fixed (5,3) key ) !
create table d1_t2 (d1_t2_c1 float (3),
                    d1_t2_c2 character(10) ) !
insert into d1_pt1 values ( 'Hans','Friedrich' ) !
insert into d1_pt2 values ( 111, - 0.2 ) !
insert into d1_t1 values ( 0.0, -4.) !
insert into d1_t2 set d1_t2_c2 = 'Ottokar' !
commit work release !
 ****************************************************!
file connect ( d2 d2_w1 !
create  user d2_r1 password d2_r1_w1 resource !
create user  d2_r2 password d2_r2_w1 resource !
create user  d2_p1 password w !
create user  d2_p2 password d2_p2_w1 !
create table d2_pt1
          ( d2_pt1_c1 char(14)  key,
            d2_pt1_c2 char(14) not null ) !
create table d2_pt2 ( d2_pt2_c1 fixed (5,0) not null ,
                             d2_pt2_c2 float(9) ) !
create table d2_t1     (d2_t1_c1 float (3) key,
                        d2_t1_c2 float (3) key ) !
create table d2_t2     ( d2_t2_c1 fixed (5,5) key,
                         d2_t2_c2 fixed (5,2) ) !
insert into d2_pt1 values ( 'Paris','Aphrodite' ) !
insert into d2_pt2 values ( 0022.0 , null ) !
insert into d2_t1 set d2_t1_c1 = -.00045 ,d2_t1_c2=+ 3.500 !
insert into d2_t2 values (-00.0123 , +87. ) !
commit work release !
 ****************************************************!
file connect ( r1 r1_w1 !
create table r1_t1 (r1_t1_c1 char(9) not null,
                    r1_t1_c2 float(2) not null ) !
create table r1_t2     (r1_t2_c1 float(4) key,
                        r1_t2_c2 float(4)) !
insert into r1_t1 values ( 'Wilhelm', 2E19 ) !
insert into r1_t1 values ( ' Annette' , 2.2e17) !
insert into r1_t2 values ( - 0.09 , -  .67e00 ) !
insert into r1_t2 set r1_t2_c1=null,r1_t2_c2 =+0.0003  !
commit work release !
 *****************************************************!
file connect ( r2 r2_w1 !
create table r2_t1     (r2_t1_c1 fixed(1) key range in (-0,-5,+5),
                        r2_t1_c2 fixed(5,2) ) !
create table r2_t2
      ( r2_t2_c1 char(5)  key range between 'A' and 'Z' ,
        r2_t2_c2 float(2) key range not between -0.99e-63 and 0 )!
insert into r2_t1 values ( .0 , 333.33 ) !
insert into r2_t1 set r2_t1_c1=+00005.   !
insert into r2_t2 values ('ABCD' , 8.8 ) !
insert into r2_t2 values ( 'B',.55e-34 ) !
commit work release !
 *****************************************************!
file connect ( d1_r1 d1_r1_w1 !
create table d1r1t1     ( d1r1t1_c1 fixed(5) key,
                            d1r1t1_c2 char(9) ) !
create table d1_r1.d1r1t2 ( d1r1t2_c1 float (10),
                        d1r1t2_c2 float (10) not null) !
insert into d1r1t1 values ( 12345.00, 'Juergen' ) !
insert into d1r1t1 values ( +1 , 'Egon ' ) !
insert into d1_r1.d1r1t2 values ( +3e+05 , - 3e+22 ) !
insert into       d1r1t2 set d1r1t2_c2 = -.22E-13  !
commit work release !
 *****************************************************!
file connect ( d1_r2 d1_r2_w1 !
create table d1r2t1     ( d1r2t1_c1 fixed(8) key,
                            d1r2t1_c2 float(8) ) !
create table d1r2t2
          ( d1r2t2_c1 character(12) not null,
            d1r2t2_c2 fixed(18,0) not null ) !
insert into d1_r2.d1r2t1 values (-  123, +.0e+00 ) !
insert into       d1r2t1 values ( -234 , 000.03e-2 ) !
insert into d1r2t2 set d1r2t2_c1 = ' Joachim ',
                         d1r2t2_c2 = -1234 !
insert into d1r2t2 values ( 'Viktor', +12) !
commit  work release !
 *****************************************************!
