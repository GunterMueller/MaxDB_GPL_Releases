*   *ID* A07FLOA2 DBCMD    changed on 1992-09-28-13.22.28 by ADMIN     *
 ***************************
 ***   a 0 7 f l o a 2   ***
 *** grenzen test        ***
 ***************************!
file connect ( kern test !
create table kern.t1 (f1 float (8) range in (-0.1e-64) )!
create table kern.t11 (f1 float (8) range in (+0.1e-64) )!
create table kern.t12 (f1 float (8) range in (+0.1e+64) )!
create table kern.t13 (f1 float (8) range in (-0.1e+64) )!
create table kern.t14 (f1 float (8) range in (-0.1e-63) )!
create table kern.t2 (f1 float (8) range in (+0.1e-63) )!
create table kern.t3
   (f1 float(8) range in (+0.1e+63,+0.99999999e+63))!
create table kern.t4
   (f1 float(8) range in (-0.00001e+63,-0.99999999e+63))!
create table kern.t5 (f1 float (8) range in (-0.00001e-63) )!
create table kern.t6 (f1 float (8) range in (+0.00001e-63) )!
create table kern.t7
   (f1 float (8) range in (+0.00001e+63, +0.99999999e+63) )!
create table kern.t8
   (f1 float (8) range in (-0.00001e+63, -0.99999999e+63) )!
create table kern.t15 (f1 float (8) range in (-100001e-63) )!
create table kern.t16 (f1 float (8) range in (+100001e-63) )!
create table kern.t17(f1 float (8) range in (+100001e+63) )!
create table kern.t18(f1 float (8) range in (-100001e+63) )!
commit work !
file sel_table !
file sel_cols_1tab ( KERN T1 !
file sel_cols_1tab ( KERN T11 !
file sel_cols_1tab ( KERN T12 !
file sel_cols_1tab ( KERN T13 !
file sel_cols_1tab ( KERN T14 !
file sel_cols_1tab ( KERN T2 !
file sel_cols_1tab ( KERN T3 !
file sel_cols_1tab ( KERN T4 !
file sel_cols_1tab ( KERN T5 !
file sel_cols_1tab ( KERN T6 !
file sel_cols_1tab ( KERN T7 !
file sel_cols_1tab ( KERN T8 !
file sel_cols_1tab ( KERN T15 !
file sel_cols_1tab ( KERN T16 !
file sel_cols_1tab ( KERN T17 !
file sel_cols_1tab ( KERN T18 !
commit work release !
