*   *ID* N00T0808 DBCMD    changed on 1992-09-28-13.24.57 by ADMIN     *
 ***  8.13 <create view statement> !
file connect ( kern test !
create table t11
   ( t1xc1 char(10) key,
     t1xc2 fixed(3) not null range between -100 and +100,
     t1xc3 float(3),
     t1xc4 char(10) byte ) !
create view v1xt1 as
   select *  from t11
   where t1xc1 > 'HAMELN' or ( t11.t1xc4 is  null ) !
create view v2xt1 as select t11.t1xc1, t1xc2  from t11 !
create view v3xt1  as select  t1xc3 from t11
   where t11.t1xc3 >= 0 !
create view v4xt1 ( v4xc1, v4xc2 )  as
   select all t1xc1, k1xt1.t1xc2 from t11 k1xt1
   where ( k1xt1.t1xc2 between -50 and +50 )
   and k1xt1.t1xc1 < 'K' with check option !
create view v5xt1 (  v5xc1 ) as
   select 3 * t1xc3
   from t11
   where t1xc3*3 <3E20 !
insert t11 values ( 'MUENCHEN', 32, 2E10, x'D6E3E3D6' ) !
insert v1xt1 values ( 'BERLIN', 12, 8E-7, x'D7C1E4D3' ) !
insert v1xt1 set t1xc1 = 'HAMBURG',
                        t1xc2 = 38,
                        t1xc3 = 80E-08 !
update v2xt1 set t1xc2 = 64 where t1xc2 = 32 !
select v3xt1.*, t1xc3  from v3xt1 !
fetch !
insert v4xt1 set v4xc1 = 'BONN',
                        v4xc2 = -20 !
delete v4xt1 where v4xc1 = 'BONN' and v4xc2 in (-20) !
insert v4xt1 values ( 'BONN', -20 ) !
select * from kern.t11 !
fetch !
commit work !
create view v1xv1xt1 as
   select t1xc1, v1xt1.t1xc2, t1xc3
   from v1xt1
   where v1xt1.t1xc1 > 'KOELN'
   with check option !
create view v1xv1xv1xt1 as
   select *
   from v1xv1xt1 syn1
   where syn1.t1xc3 <1E30 !
insert v1xv1xv1xt1 values ( 'MANNHEIM', 18, -5.e12 ) !
declare erg1 cursor for
select t1xc1, v1xv1xt1.t1xc2,v1xv1xt1.t1xc3
   from v1xv1xt1 !
fetch erg1 !
declare erg2 cursor for
select distinct kern.t11.t1xc1, v1xv1xv1xt1.t1xc3,
   syn2.t1xc2
   from t11, v2xt1 syn2, v1xv1xv1xt1
   where syn2.t1xc2 + 32 <= v1xv1xv1xt1.t1xc3
   order by kern.t11.t1xc1, syn2.t1xc2 !
fetch erg2 !
commit work !
create table t21 ( t2xc1 float(5) , t2xc2 char(20) ) !
create synonym synxt2 for t21 !
create synonym synxv1xv1xt1 for v1xv1xt1 !
create view synxview
   (spalte1,spalte2,spalte3,spalte4,spalte5,spalte6,spalte7 ) as
   select synxt2.t2xc1, synxv1xv1xt1.t1xc1, t2xc2, v1xv1xt1.*,
          synxv1xv1xt1.t1xc2
   from synxt2, synxv1xv1xt1 , v1xv1xt1
   where synxt2.t2xc2 < 'K' and  ( v1xv1xt1.t1xc1 < 'NUERNBERG' )!
insert synxt2 values ( 38, 'ESSEN' ) !
create synonym synxsynxview for synxview !
select * from synxsynxview where ( spalte1 in ( 37, 38, 39 )
                             and synxsynxview.spalte2 < 'MOELLN' ) !
fetch !
create user r1 password r1xw resource !
commit release !
file connect ( sut sut !
create user d1 password d1xw resource !
commit release !
file connect ( kern test !
grant all on v1xt1 to public !
grant select on synxview to r1 !
commit work release !
file connect ( d1 d1xw !
create view view1 as
    select t1xc1, t1xc2
    from kern.v1xt1
    with check option !
insert view1 values ( 'ISERLOHN' , - 20 ) !
select * from view1 !
fetch !
commit work release !
file connect ( r1 r1xw !
select * from synxview !
fetch !
create synonym synxv1xt1 for kern.v1xt1 !
create view r1xview ( spalte1, spalte2, spalte3 ) as
   select spalte1, spalte3, t1xc1
   from synxview, synxv1xt1
   where spalte1 - 38 <= 0 and t1xc1 < 'I' !
select * from r1xview !
fetch !
commit work release !
file connect ( kern test !
drop table t11 !
drop table t21 !
rollback work release !
