senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* N00TEST1.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !


file connect ( kern test !
create table tab1 ( spalte1 fixed(5),
                    spalte2 fixed(11,6),
                    spalte3 fixed(3,3),
                    spalte4 float(7) ) !
create table tab2 ( spalte1 float(12), col2 fixed(5,2) ) !
insert tab1 values ( 348  , 76543.21, - 0.623, -12.3E12 ) !
insert tab1 values ( -9999,        0,   0.003, 12345e-7 ) !
insert tab1 values (  null,  -5.1234,       0,     null ) !
insert tab1 values (     0,     null,    null,    3E-32 ) !
insert tab2 values ( 1.34E+1,   22.33 ) !
insert tab2 values (   -0.18, -123.07 ) !
select *  from tab1, tab2 ref
   where +abs(tab1.spalte1 + ref.spalte1 )
       > trunc(ref.spalte1 -  ABS( -ref.col2 )) !
fetch !
select  3*trunc(  spalte3 ) / abs(ref.col2 * abs(-1.0)),
        abs ( col2 ) neu,
        kern.tab1.spalte4 + trunc(-32.7 )
  from tab1,tab2 ref where tab1.spalte3 is not null !
fetch !
rollback work release!

file connect ( kern test !
create table tab1 ( spalte11 fixed(10) key,
                    spalte12 char(8),
                    spalte13 float(8) range between -1E30 and +1E30 ) !
create table tab2 ( spalte21 float(10),
                    spalte22 fixed(12,2),
                    spalte23 char(6) ) !
create table tab3 ( spalte31 float(8) range between -1E15 and +1E15,
                    spalte32 float(8),
                    spalte33 char(12) ) !
insert tab1 values ( 12345678      , 'HANS'   , 1.2345678E7 ) !
insert tab1 values ( 22            , 'OTTO'   , 12345678    ) !
insert tab1 values ( -888          , 'HANS'   , 1E28        ) !
insert tab2 values ( -3827.32E11   , -888     , 'PAUL'      ) !
insert tab2 values ( 1E28          , 12345678 , 'HANS'      ) !
insert tab2 values ( 12.3456780E+06, -0       , 'HANS'      ) !
insert tab3 values ( 12345678      , 12345678 , 'HANS'      ) !
insert tab3 values ( -5E14         , 1E28     , 'OTTO'      ) !
insert tab3 values ( -5E14         , 0        , null        ) !
select spalte11, spalte12, spalte22, spalte23, spalte33
   from tab1, tab2 ref2, tab3
   where ref2.spalte23 = tab1.spalte12 !
fetch !
select spalte11, spalte22,spalte32 from tab1 ref1, tab2 ref2, tab3
   where ( 5E+06 *  ref2.spalte22 ) <=(tab3.spalte32) +  48 !
fetch !
select tab1.spalte11 , spalte13, spalte31, ref3.spalte32, spalte22
  from tab1, tab2  , tab3 ref3
  where tab1.spalte13/tab1.spalte11>(.7E10*ref3.spalte31)+ref3.spalte32 !
fetch !
select spalte21, spalte13 from tab1, tab2
   where (( tab2.spalte21 = 12345678)
     and 1E10 > tab1.spalte13  ) !
fetch !
select ref2.*, tab3.*  from tab2 ref2, tab3
   where ( 10E3*ref2.spalte22)  <(  ( tab3.spalte31) )
     and ( ref2.spalte23 <= tab3.spalte33
     and tab3.spalte32 / ( 3*tab3.spalte31)
         >ref2.spalte22 * (-ref2.spalte21) ) !
fetch !
select * from tab1,tab2,tab3
   where tab1.spalte13 +88.E-12= (- 1)*( - tab2.spalte21)
     and 6587- tab1.spalte11  >  (tab3.spalte32 - tab3.spalte31) *5. !
fetch !
select spalte11, tab1.spalte12,spalte21,spalte22, spalte31  ,spalte33
   from tab1,tab2,tab3
   where tab1.spalte11>=tab2.spalte22
     and tab2.spalte21 <> tab3.spalte31
     and tab3.spalte33 = tab1.spalte12 !
fetch !
select ref11.spalte11,ref11.spalte12,ref11.spalte13,ref12.*
   from tab1 ref11, tab1 ref12
   where ref12.spalte11 = ref11.spalte13 !
fetch !
select tab1.spalte11, spalte21,spalte22, ref1.spalte13
   from tab1,tab2,tab1  ref1
   where - ref1.spalte13 < -tab1.spalte11
     and (48 + 5 )*ref1.spalte13 < 10E30/tab2.spalte21 * tab2.spalte22 !
fetch !
commit work release !

file connect ( kern test !
create table t1 ( t1xc1 char(5) ascii key,
                  t1xc2 char(5) key,
                  t1xc3 fixed(5) ) !
create table t2 ( t2xc1 char(5) ascii key,
                  t2xc2 fixed(5),
                  t2xc3 char(8),
                  t2xc4 fixed(6,1) not null ) !
create index i1 on t2 ( t2xc4 asc, t2xc3 desc ) !
create index i2 on t2 ( t2xc2 asc ) !
create index "t1.t1xc3"
	on t1( t1xc3 desc ) !
insert t1 values ( 'cc', 'ffff', 2 ) !
insert t1 values ( 'aa', 'ssss', null ) !
insert t2 values ( 'aa', 5, 'dddd', 3 ) !
insert t2 values ( 'dd', 3, 'ffff', 2 ) !
explain select * from t1, t2 where t1xc1 = t2xc1 and t1xc3 < t2xc4 !
select * from t1, t2 where t1xc1 = t2xc1 and t1xc3 < t2xc4 !
fetch!
rollback work release !
file connect ( sut sut !
commit release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N00TEST2.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

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
insert v4xt1 set v4xc1 = 'BONN',
                        v4xc2 = -20 !
delete v4xt1 where v4xc1 = 'BONN' and v4xc2 in (-20) !
insert v4xt1 values ( 'BONN', -20 ) !
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
file connect ( sut sut !
drop user d1 !
drop user r1 !
commit release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N25SEL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

file connect ( kern test !
create table tab1 ( spalte11 char(10) key ,
                    spalte12 fixed(5,2),
                    spalte13 float(5) ) !
insert tab1 values ( 'HANS'    , 22.52 , null ) !
insert tab1 values ( 'PAUL'    ,   0   ,  3E2 ) !
insert tab1 values ( 'JOACHIM' , 17.1  ,   0  ) !
create table tab2 ( spalte21 fixed(4) not null,
                    spalte22 char(8) ,
                    spalte23 float(4) ) !
insert tab2 values ( 300   , 'HANS'   , null ) !
insert tab2 values ( 22.00 , 'FRIEDA' , 0    ) !
insert tab2 values ( 12    , 'JOACHIM', 0    ) !
create table tab3 ( spalte31 fixed(3),
                    spalte32 char(10),
                    spalte33 fixed(3) ) !
insert tab3 values ( 22  , 'HANS'    , null ) !
insert tab3 values ( 300 , 'ANDREAS' , 0    ) !
insert tab3 values ( 300 , 'ANDREAS' , 0    ) !
insert tab3 values ( null, 'JOACHIM' , 300  ) !
commit work release !

file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6 from tab1, tab2
   where tab1.spalte13 = tab2.spalte21 !
select * into :a1,:a2,:a3,:a4,:a5,:a6 from tab1, tab2
   where tab2.spalte21  =  tab1.spalte13 !
select  * into :a1,:a2,:a3,:a4,:a5,:a6
   from kern.tab1, kern.tab2
   where  kern.tab2.spalte21=kern.tab1.spalte13 !
select tab1.spalte13 neu13, tab1.spalte11,
       kern.tab2.spalte21 neu21
   into :a1,:a2,:a3 from tab1, kern.tab2
   where tab1.spalte11 > 'KARIN'
      or tab1.spalte13=  tab2.spalte21 !
select tab1.spalte13 neu13, tab1.spalte11,
       kern.tab2.spalte21 neu21
   into :a1,:a2,:a3 from tab1, kern.tab2
   where ( tab1.spalte11 > 'KARIN'
      or tab1.spalte13=  tab2.spalte21 ) !
select  kern.tab2.spalte22,kern.tab1.spalte13 neu13,
        tab2.spalte23
   into :a1, :a2,:a3
   from kern.tab2, tab1
   where kern.tab1.spalte13 = tab2.spalte23
     and (( tab2.spalte21 <= 20 )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6 from tab1 , tab2
   where    tab1.spalte12  = tab2.spalte21
      or  kern.tab2.spalte22 in ( 'KLAUS' , 'FRIEDA' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6 from tab1 , tab2
   where  ( tab1.spalte12  = tab2.spalte21 )
      or  kern.tab2.spalte22 in ( 'KLAUS' , 'FRIEDA' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6 from tab1,tab2
   where tab2.spalte23 <0
      or  kern.tab1.spalte11= kern.tab2.spalte22
     and kern.tab1.spalte12 <= tab2.spalte21 !
select * into :a1,:a2,:a3,:a4,:a5,:a6 from tab1,tab2
   where  kern.tab1.spalte11= kern.tab2.spalte22
     and kern.tab1.spalte12 <= tab2.spalte21
      or tab2.spalte23 <0 !
select tab2.* into :a1,:a2,:a3 from kern.tab1,tab2
   where ( kern.tab1.spalte11 like 'PA_L' )
     and tab1.spalte13 <> kern.tab2.spalte23
     and ( tab2.spalte22 < 'KARIN' ) !
select tab2.* into :a1,:a2,:a3 from kern.tab1,tab2
   where ( kern.tab1.spalte11 like 'PA_L' )
     and tab1.spalte13 <> kern.tab2.spalte23
     and ( tab2.spalte22 < 'INGE' ) !
select tab2.* into :a1,:a2,:a3 from kern.tab1,tab2
   where  tab1.spalte13 <> kern.tab2.spalte23
     and kern.tab1.spalte11 like 'PA_L'
     and tab2.spalte22 < 'INGE' !
select tab2.* into :a1,:a2,:a3 from kern.tab1,tab2
   where (tab1.spalte13 <> kern.tab2.spalte23
     and kern.tab1.spalte11 like 'PA_L' )
     and tab2.spalte22 < 'INGE' !
select tab2.* into :a1,:a2,:a3 from kern.tab1,tab2
   where  (not ( tab1.spalte13 = kern.tab2.spalte23 )
     and kern.tab1.spalte11 like 'PA_L' )
     and tab2.spalte22 < 'INGE' !
select tab1.*, kern.tab2.*, kern.tab1.*
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9 from tab1,tab2
   where kern.tab2.spalte21 = tab1.spalte12
      or tab1.spalte13 is null
     and tab2.spalte21 > 100.00 !
select all  * into :a1,:a2,:a3,:a4,:a5,:a6 from tab2,tab1
   where tab1.spalte13 = tab2.spalte21 !
select distinct  * into :a1,:a2,:a3,:a4,:a5,:a6
   from  tab2 , tab1
   where tab1.spalte13  =  tab2.spalte21 !
select  *   into :a1,:a2,:a3,  :a4,:a5,:a6
   from tab1  ref1, tab2
   where ref1.spalte13=tab2.spalte21 !
select ref1.spalte11 neu11, kern.tab2.* into :a1,:a2,:a3,:a4
   from tab1  ref1, kern.tab2
   where ref1.spalte13 = kern.tab2.spalte21 !
select kern.tab1.*, ref2.*
   into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab2 ref2
   where tab1.spalte13 = ref2.spalte21 !
select all ref1.spalte11 neu11 , ref1.spalte12,
    ref1.spalte13,ref2.spalte21 neu21,ref2.spalte21 neu21 
   into :a1, :a2 ,:a3,:a4,:a5
   from tab1  ref1, tab2 ref2
   where ref1.spalte13 = ref2.spalte21 !
select distinct kern.tab1.* , ref3.*
   into :a1,:a2,:a3,:a4,:a5,:a6
   from kern.tab1 , kern.tab3  ref3 , tab1 ref1
   where ref1.spalte13 < ref3.spalte31
     and ref3.spalte33 = kern.tab1.spalte13 !
select distinct kern.tab1.* , ref3.*
   into :a1,:a2,:a3,:a4,:a5,:a6
   from kern.tab3  ref3 , kern.tab1
   where tab1.spalte13 < ref3.spalte31
     and ref3.spalte33 = kern.tab1.spalte13
     and tab1.spalte12 between 0 and 100 !
select distinct kern.tab1.* , ref3.*
   into :a1,:a2,:a3,:a4,:a5,:a6
   from kern.tab3  ref3 , kern.tab1
   where tab1.spalte13 < ref3.spalte31
     and kern.tab1.spalte13 = ref3.spalte33
     and tab1.spalte12 between 0 and 100 !
commit work release !

file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
   from tab1, tab2, tab3
   where (tab1.spalte13, tab1.spalte11) in ((tab2.spalte23, tab2.spalte22))
     and tab2.spalte23 < tab3.spalte33 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
   from kern.tab1 , tab2, kern.tab3
   where (tab1.spalte13, kern.tab2.spalte22, tab1.spalte11)
     in ((tab2.spalte23, tab1.spalte11,      kern.tab3.spalte32)) !
select tab1.*, tab2.* , tab3.*
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
   from tab2, kern.tab1 , tab3
   where (kern.tab1.spalte13, tab2.spalte22)
     in ((kern.tab2.spalte23, tab3.spalte32)) !
select tab1.* , tab2.* , tab3.*
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
   from tab2, kern.tab1, tab3
   where (tab1.spalte13, tab2.spalte22) in ((tab2.spalte23, tab3.spalte32))
     and tab3.spalte33>= tab1.spalte12 !
select ref2.*, tab1.*, tab3.*
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
   from kern.tab2  ref2, kern.tab3, tab1
   where tab3.spalte33  is  null
     and  (kern.tab3.spalte32, ref2.spalte22)
      in ((ref2.spalte22,      kern.tab1.spalte11))
     and ( tab1.spalte13 is null
     and   tab1.spalte13 is null ) !
select all * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
   from tab2  ref2,tab3 ref3, kern.tab1 ref1
   where ref3.spalte33 is null
     and (ref2.spalte22, ref2.spalte22) in ((ref3.spalte32, ref1.spalte11))
     and ( ref1.spalte13 is null )
     and   ref1.spalte13 is null
      or ref2.spalte21 >100 !
select distinct ref1_6789012345678.spalte11,
       ref1_6789012345678.spalte12 neu12 ,
       ref1_6789012345678.spalte13 neu13,
       kern.tab2.spalte22 neu22, tab3.spalte31 neu31,
       kern.tab3.spalte32
    into :a1, :a2,:a3,:a4,:a5,:a6
    from kern.tab3, tab2 , tab1  ref1_6789012345678
    where ref1_6789012345678.spalte13 > tab2.spalte23
      and (tab2.spalte23, tab2.spalte22) in
          ((tab3.spalte33, 'FRIEDA'), (tab3.spalte33, 'HANS')) !
select distinct ref3.*, tab1.*, tab3.*
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
   from tab1 , tab3 ref3 , tab3
   where ref3.spalte31 > tab1.spalte13
     and not ( ref3.spalte33 is null )
     and tab3.spalte33 = ref3.spalte33 !
commit work release !

file connect ( kern test !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref21.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab3
   where tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31 !
select distinct * into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab3
   where tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31 !
select all  * into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab2
   where tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21 !
select distinct * into :a1, :a2, :a3, :a4, :a5, :a6
   from tab1, tab3
   where tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref12.spalte13 < ref11.spalte13
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref13.spalte13 < ref33.spalte31
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref33.spalte31
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
                                   tab3 ref33
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref31.spalte31 < ref33.spalte31
       and ref32.spalte31 = ref33.spalte31 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref32.spalte31
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref12.spalte13 < ref31.spalte31
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref32.spalte31
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref11.spalte11 = ref13.spalte11
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref22.spalte21 = ref31.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref32.spalte31
       and ref23.spalte21 = ref24.spalte21 !
commit work release !

file connect ( kern test !
CREATE TABLE APPL_TABLE
  ( NAME  CHAR(18) KEY,
    TYPE CHAR(5) NOT NULL,
    LEN FIXED(3) NOT NULL ) !
INSERT INTO APPL_TABLE VALUES ('A1','CHAR',20) !
INSERT INTO APPL_TABLE VALUES ('A2','CHAR',20) !
SELECT A1.TYPE, A1.LEN, A1.NAME
  INTO :A1, :A2, :A3
  FROM APPL_TABLE A1, APPL_TABLE A2
  WHERE A1.TYPE = A2.TYPE
    AND A1.LEN  = A2.LEN
    AND A1.NAME <> a2.NAME !
COMMIT WORK RELEASE !

file connect ( kern test !
CREATE TABLE  relship
(  defobjtype     CHAR (8)  KEY,
   defuser        CHAR (8)  KEY,
   defobjn1       CHAR (18) KEY,
   defobjn2       CHAR (18) KEY,
   refobjtype     CHAR (8)  KEY,
   refuser        CHAR (8)  KEY,
   refobjn1       CHAR (18) KEY,
   refobjn2       CHAR (18) KEY,
   reltype        CHAR (18) RANGE IN ( 'CONTAINS', 'CALLS', 'OWNER',
                                       'DERIVED_FROM', 'USES', 'ALIAS',
                                       'PERMISSION_FOR', 'COMMENTED' ),
   createdate     DATE,
   createtime     TIME,
   privilegs      CHAR( 19) ) !
CREATE VIEW entity_contains ( defobjtype, defuser, defobjn1,
                      defobjn2, refobjtype, refuser,
                      refobjn1, refobjn2, reltype )
           AS  SELECT defobjtype, defuser, defobjn1,
                      defobjn2, refobjtype, refuser,
                      refobjn1, refobjn2, reltype
               FROM   relship
               WHERE  (reltype,    defobjtype, refobjtype)
                  in (('CONTAINS', 'ENTITY',   'ATTR'))
           WITH CHECK OPTION !
CREATE TABLE  attribute_type
(  etype_name     CHAR (18) KEY,
   desc_name      CHAR (32),
   authid         CHAR (8),
   createdate     DATE,
   upd_author     CHAR (8),
   last_upd       DATE,
   COMMENTS       CHAR (80) ) !
insert entity_contains values
   ( 'ENTITY','BARBARA','TABLE', ' ',
     'ATTR','BARBARA','AUTHID',' ','CONTAINS' ) !
insert attribute_type values
  ( 'AUTHID', 'Author des Eintrags', 'BARBARA',null,null,null,'aa' ) !
select defobjn1, refobjn1, desc_name
  from attribute_type, entity_contains
  where refobjn1 = etype_name !
fetch !
select defobjn1, refobjn1, desc_name
  from entity_contains, attribute_type
  where refobjn1 = etype_name !
fetch !
commit work release !

file connect ( kern test !
create table t1 (
       s1         char(19),
       s2         fixed(10)
       ) !
insert t1 set s1 = 't1 *        ', s2 = 1 !
insert t1 set s1 = 't1   *      ', s2 = 1 !
insert t1 set s1 = 't1  *       ', s2 = 1 !
create table t2 (
       s1         char(19),
       s2         fixed(10)
        ) !
insert t2 set s1 = 't2       ***', s2 = 11111 !
insert t2 set s1 = 't2      ****', s2 = 11111 !
insert t2 set s1 = 't2        **', s2 = 44444 !
commit work !
select t1.s1, t2.s1, t1.s2, t2.s2
  from <t1, t2>
 where (t1.s2 = 1 or
       (t1.s2, t2.s2) in ((2, 44444)))
  order by t2.s1, t1.s1
  !
fetch !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N28SEL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

file connect ( kern test !
create table kern.tab1 ( spalte11 char(10) key ,
                           spalte12 fixed(5,2),
                           spalte13 float(5) ) !
insert tab1 values ( 'HANS'    , 22.52 , null ) !
insert tab1 values ( 'PAUL'    ,   0   ,  3E2 ) !
insert tab1 values ( 'JOACHIM' , 17.1  ,   0  ) !
create table kern.tab2 ( spalte21 fixed(4) not null,
                           spalte22 char(8) ,
                           spalte23 float(4) ) !
insert tab2 values ( 300   , 'HANS'   , null ) !
insert tab2 values ( 22.00 , 'FRIEDA' , 0    ) !
insert tab2 values ( 12    , 'JOACHIM', 0    ) !
create table kern.tab3 ( spalte31 fixed(3),
                           spalte32 char(10) ,
                           spalte33 fixed(3) ) !
insert tab3 values ( 22  , 'HANS'    , null ) !
insert tab3 values ( 300 , 'ANDREAS' , 0    ) !
insert tab3 values ( 300 , 'ANDREAS' , 0    ) !
insert tab3 values ( null, 'JOACHIM' , 300  ) !
commit work !
commit work release !

file connect ( kern test !
declare selecttab1 cursor for 
select tab1.spalte11, kern.tab1.spalte13,
       ref2.spalte22 neu22 ,
       ref2.spalte23 neu23, kern.tab3.spalte33
   from kern.tab1, tab2  ref2 , tab3
   where tab1.spalte13 <= kern.tab3.spalte33
     and ref2.spalte22 <> tab1.spalte11
     and not ( ref2.spalte23 is null ) !
fetch selecttab1 !
declare selecttab2 cursor for   
select distinct tab1.spalte11, kern.tab1.spalte13,
       ref2.spalte22 neu22 ,
       ref2.spalte23 neu23, kern.tab3.spalte33
   from kern.tab1, tab2  ref2 , tab3
   where tab1.spalte13 <= kern.tab3.spalte33
     and ref2.spalte22 <> tab1.spalte11
     and not ( ref2.spalte23 is null )
        order by tab1.spalte11, neu22, kern.tab3.spalte33 desc !
fetch selecttab2 !
declare selecttab3 cursor for 
select tab1.spalte11, kern.tab1.spalte13,
       ref2.spalte22 neu22 ,
       ref2.spalte23 neu23, kern.tab3.spalte33
   from kern.tab1, tab2  ref2 , tab3
   where tab1.spalte13 <= kern.tab3.spalte33
     and ref2.spalte22 <> tab1.spalte11
     and not ( ref2.spalte23 is null )
   order by kern.tab1.spalte13 asc, ref2.spalte22 desc !
fetch selecttab3 !
commit work release !

file connect ( kern test !
CREATE TABLE TAB11
   ( col1 CHAR(12) KEY,
     col2 CHAR(12) KEY ) !
CREATE TABLE TAB41
   ( spalte1 CHAR(18) KEY,
     spalte2 CHAR(18) KEY ) !
insert tab11 values ( 'Y', 'M' ) !
insert tab41 values ( 'Y', 'M' ) !
select tab11.col1, tab11.col2, ref.spalte1, ref.spalte2
    from tab41 ref , tab11
    where (tab11.col1, tab11.col2) in ((ref.spalte1, ref.spalte2)) !
fetch !
rollback work release !
file connect ( kern test !
create table eins ( wf char (12), wnr fixed (4))!
create index "eins.wf"
	on eins( wf ) !
insert eins values ('computer', 1)!
insert eins values ('software', 2)!
create table zwei (wnr fixed(4))!
create index "zwei.wnr"
	on zwei( wnr ) !
insert zwei set wnr = 1!
insert zwei set wnr = 2!
select eins.wnr, wf from eins, zwei
  where (eins.wnr, wf) in ((zwei.wnr, 'computer'), (zwei.wnr, 'software')) !
fetch!
rollback release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N31STRIN.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( c1 char(10) ) !
create table t2 ( c2 fixed(2) ) !
insert t1 set c1 = 'a' !
insert t2 set c2 = 1 !
select * from t1, t2 where t2.c2 = length (t1.c1 ) !
fetch !
rollback work !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N33COUNT.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file CONNECT ( KERN TEST !
CREATE TABLE T ( A CHAR (1) , B CHAR (10))!
INSERT T VALUES ('a', 'bbbbbbbbbb')!
INSERT T VALUES ('b', 'cccccccccc')!
CREATE TABLE T1 ( C CHAR (1), D CHAR (10))!
INSERT T1 VALUES ('d', 'eeeeeeeeee')!
INSERT T1 VALUES ('e', 'ffffffffff')!
create table t2 ( e char (11))!
insert t2 set e = 'eeeeeeeeeee' !
SELECT COUNT (DISTINCT A||B), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
SELECT COUNT (DISTINCT A||D), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
pars_then_ex !
SELECT COUNT (DISTINCT A||B), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
select count (distinct e), count (e) into :a, :b from t2 !
SELECT COUNT (DISTINCT A||D), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
pars_execute !
drop table t !
drop table t1 !
rollback work !
commit work release !

file connect ( kern test !
create table t1 ( id int,
CLIENT char (12),
PRGUID char (16) byte,
proctype char (20) byte)!
insert into t1 values (1, '001', x'ABCDEF11FE334455', 
   x 'A9972913B959B545926BFB193FABFE24')!
insert into t1 values (2, '001', x'ABCDEF11FE33', 
   x 'A9972913B959B545926BFB193FABFE24')!
create table t2 (
CLIENT char (12),
PRGUID char (16) byte,
PRSTEPID int,
error char (1))!
insert into t2 values ('001', x'ABCDEF11FE334455', 1, ' ')!
insert into t2 values ('001', x'ABCDEF11FE33', 1, ' ')!

CREATE VIEW "VSPICSPRTR" ("CLIENT", "PROCTYPE", "PRGUID", "PRSTEPID",	
       "SERROR")	
  AS SELECT T1."CLIENT", T1."PROCTYPE", T1."PRGUID", T2."PRSTEPID",	
        T2."ERROR"
FROM T1,  T2	
WHERE T1."CLIENT" = T2."CLIENT" AND T1."PRGUID" = T2."PRGUID"!
commit release !
file connect ( kern test sqlmode oracle !
pars_then_ex !
SELECT COUNT( DISTINCT  "PRGUID" ) "PCOUNT" , "PRSTEPID" "STEPID" , "SERROR" 
"ERROR" FROM "VSPICSPRTR" WHERE "CLIENT" = '001' AND "PROCTYPE" = 
'A9972913B959B545926BFB193FABFE24' GROUP BY "PRSTEPID" , "SERROR"
!
fetch into :a, :b, :c !
pars_execute !
drop table t1 !
drop table t2 !
commit release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N35GROUP.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t (woche fixed (4) key, monat fixed (2))!
insert t values (5000, 12)!
insert t values ( 100,  4)!
create table t1 (a1 fixed(8)key)!
insert t1 set a1 = 100!
create table t2 (a2 fixed(8)key)!
insert t2 set a2 = 100!
create view v1 as select t1.*, t2.* from t1, t2
    where t1.a1 = t2.a2 !
declare mon cursor for 
select  100* trunc(woche/100) + monat jjmm, woche
from t where woche > 2 for reuse!
fetch mon!
declare auf cursor for 
select jjmm, sum(trunc(a2)) aw, sum(trunc(a1)) rs
from v1 a, mon b
where a.a2 > 1 and a.a1 = b.woche
group by jjmm for reuse !
fetch auf!
declare auf1 cursor for 
select jjmm, sum(trunc(a2)) aw, sum(trunc(a1)) rs
from v1 a, mon b
where a.a1 = b.woche and a.a2 > 1
group by jjmm for reuse !
fetch auf1!
drop table t!
drop table t1!
drop table t2!
create table a (salary fixed (6,2))!
create view v (sal) as select sum(salary) from a !
insert a values (100)!
insert a values (500)!
select a.salary / sum(a1.salary) from a, a a1 group by a.salary!
fetch !
drop table a!
commit release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N36GROUP.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( t1xc1 char(20),
                  t1xc2 fixed(10),
                  t1xc3 char(10) ascii,
                  t1xc4 fixed(10,5),
                  t1xc5 float(10),
                  t1xc6 fixed(18) ) !
insert t1 values ( 'Hans', 100, 'Muenchen', 100.100, null,
                    500000000000000000 ) !
insert t1 values ( null, null, 'Hamburg', 0.00005, 8E-30, -2000 ) !
insert t1 values ( 'Karin', -600000, null, null, -1E12,
                    800000000000000000 ) !
insert t1 values ( 'Karin', 5, 'Bonn', -700, 1000, null ) !
insert t1 values ( 'Hans', -500000, 'Bremen', 700.00005, null, 300 ) !
insert t1 values ( ' Marianne', -80, 'Koeln 1', -300, 10e-10, null ) !
create table t2 ( t2xc1 fixed(5) key,
                  t2xc2 char(10) ascii,
                  t2xc3 char(15) ascii,
                  t2xc4 float(5),
                  t2xc5 fixed(5) range between -50000 and +50000 ) !
insert t2 values ( 10, 'Klaus', 'Mueller', 1E20, 20000 ) !
insert t2 values ( 20, 'Elena', 'Meier', -5E10, -100 ) !
insert t2 values ( 30, null, 'Lehmann', null, null ) !
insert t2 values ( 40, 'Paul', 'Anse Rauch', 8 , -50000 ) !
create table t3 ( t3xc1 fixed(2,1 ) key,
                  t3xc2 char(20) key ,
                  t3xc3 float( 5) ,
                  t3xc4 char(20) ascii range between 'A' and 'G' ,
                  t3xc5 fixed(18) ) !
insert t3 values ( 0.1, 'Frankreich', 80E50, 'Amsel',
                   700000000000000000 ) !
insert t3 values ( 2.5, 'Belgien', 1E-30, 'Fink', null ) !
insert t3 values ( 3.0, 'Italien', -5E30, 'Amsel', -1000 ) !
insert t3 values ( 4.1, 'VR Polen', null, 'Fink', +200 ) !
select sum ( t1.t1xc2 ) , sum ( t2.t2xc1 ) from t1, t2 !
fetch !
select sum ( t1xc2 ) , sum ( t2xc1 ) from t1, t2 !
fetch !
select sum( t1.t1xc2 ), sum ( t2.t2xc1 ),sum ( t3.t3xc3 ) from t1,t2,t3 !
fetch !
select sum ( t1xc2 ) from t1, t2 !
fetch !
select count (*), count ( distinct kern.t2.t2xc2 ),
       count ( all ( t3xc5)) from t1,t2,t3 !
fetch !
select avg ( distinct t2.t2xc1 ) ,
       count ( distinct t2.t2xc1 ) ,
       avg ( all kern.t1.t1xc5 ) ,
       sum ( all t1xc5 ) ,
       count ( all ( t1xc5 ) ) from t1,t2 !
fetch !
select sum ( ref.t1xc4 ) , AVG ( ref.t1xc4) from t1,t1 ref !
fetch !
select sum ( t1xc4 ), avg ( t1xc4), t1xc1, count(t1xc1)
    from t1,t2 group by t1xc1 !
fetch !
select sum ( t1xc4 ), sum ( t2xc4)
    from t1, t2 ref
    group by ref.t2xc1
    having ref.t2xc1 > 10
       and sum ( ref.t2xc4) is not null !
fetch !
select avg ( t1xc2), avg ( distinct t1xc5 ) , t1.t1xc5
    from t1, t2
    group by t1xc1 , kern.t1.t1xc5
    having sum ( t1xc5 ) > 0
    order by t1xc5, 2 !
fetch !
select sum( t1xc2) , count ( t1xc2) , avg (t1xc2)
    from t1,t3 group by t1xc1, t3.t3xc4 !
fetch !
select sum ( t1.t1xc2 + t2.t2xc1 ),
       avg ( t1xc4 * 5 * t2xc5 ),
       ( sum ( distinct kern.t1.t1xc4 / t2xc5 ) )
    from t1, t2
    group by t2xc1
    order by 1 desc, 3 asc !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !
file connect ( kern test !
create table MyTable(TableID varchar(100) not null,
IndexID int not null DEFAULT 0, primary key (TableID) )!
insert into MyTable Values ('Test1',1)!
insert into MyTable Values ('Test2',1)!
insert into MyTable Values ('Test3',2)!
insert into MyTable Values ('Test4',2)!
SELECT TableID FROM MyTable GROUP BY TableID
ORDER BY MAX(IndexID) DESC, TableID ASC!
fetch!
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N38ERGJO.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table tab1 ( spalte1 char(15) key , spalte2 fixed(5) ) !
create table tab2 ( col1 fixed(5), col2 fixed(5) ) !
insert tab1 values ( 'Hans', 1 ) !
insert tab1 values ( 'Paul', 3 ) !
insert tab1 values ( 'Ilse', 2 ) !
insert tab1 values ( 'John', null ) !
insert tab1 values ( 'Otto', 6 ) !
insert tab2 values ( 1, 3 ) !
insert tab2 values ( 5, 7 ) !
insert tab2 values ( null, null ) !
insert tab2 values ( 6, 3 ) !
insert tab2 values ( 1, null ) !
insert tab2 values ( 7, 6 ) !
declare tab1res1 cursor for 
select * from tab1 where spalte2 > 2 for reuse !
fetch tab1res1 !
declare res1 cursor for 
select tab1.spalte1 c1,
              tab1.spalte2 c2 ,
              tab1res1.spalte2 c3 
   from tab1, tab1res1 where tab1.spalte2 < tab1res1.spalte2 for reuse !
fetch res1 !
declare res2 cursor for 
select res1.c2 + ref1.spalte2 sp1, res1.c2 * 3 sp2
   from res1, tab1res1 ref1
   where spalte1 = c1 for reuse !
fetch res2 !
declare res3 cursor for 
select sum ( c2 ) s1, spalte1
   from tab1res1, res1
   where res1.c3 = spalte2
   group by spalte1
   order by 1 !
fetch res3 !
declare res4 cursor for 
select col2,c1,sum( col1 ),avg( c2 ) average,count( * )
   from tab1res1, tab2, res1
   where c3 <> col2
     and col1 > tab1res1.spalte2
   group by col2, res1.c1 !
fetch res4 !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N39SUB.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file CONNECT ( KERN TEST !
create table t1 ( t1xc1 char(10) ascii key,
                  t1xc2 char(20),
                  t1xc3 fixed(5,2) ) !
create table t2 ( t2xc1 fixed(7,2) key,
                  t2xc2 char(10) key,
                  t2xc3 char(20) ascii ) !
create table t3 ( t3xc1 char(15) ascii,
                  t3xc2 char(20),
                  t3xc3 fixed(6,3) ) !
insert t1 values ( 'Hans', 'Bremen', 1 ) !
insert t1 values ( 'Karl', 'Bremen', 100 ) !
insert t1 values ( 'Karin', 'Hamburg', null ) !
insert t1 values ( 'Lisa', null, null ) !
insert t2 values ( 20, 'Paul', 'Muenchen' ) !
insert t2 values ( 100, 'Emil', 'Hannover' ) !
insert t2 values ( 100, 'Emma', null ) !
insert t3 values ( 'Karl', 'Hannover', 1 ) !
insert t3 values ( 'Otto', 'Dortmund', 300 ) !
insert t3 values ( null, null, null ) !
insert t3 values ( 'Ilse', 'Hamm', 200 ) !
commit work !
select * from t1, t3
  where t1xc2 < all ( select t3xc2 from t3 where t3xc2 is not null )
    and ( t3xc2 in ( select t2xc3 from t2 where t2xc3 <> 'Muenchen' )
     or t3xc2 < 'F' ) 
  order by 4,1 !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N42ROWNO.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table tab1 ( spalte1 fixed(5) key,
                    spalte2 char(10) key,
                    spalte3 fixed(5) ) !
create table tab2 ( col1 fixed(5),
                    col2 fixed(5),
                    col3 char(10) ,
                    col4 char(10) ) !
insert tab1 values ( 1 , 'Hans' , 1 ) !
insert tab1 values ( 2 , 'Otto' , null ) !
insert tab1 values ( 2 , 'Hans' , 3 ) !
insert tab1 values ( 9 , 'Paul', 1  ) !
insert tab1 values ( 4 , 'Ilse', 1 ) !
insert tab1 values ( 9 , 'Ilse', null ) !
insert tab2 values ( null, null, null, null ) !
insert tab2 values ( 4 , 5, 'Hans' , 'Paul' ) !
insert tab2 values ( 1, 8 , 'Paul', 'ILse' ) !
insert tab2 values ( 3, 4, 'Hans' , 'Hans' ) !
insert tab2 values ( 3, 9 , 'Otto' , null ) !
insert tab2 values ( 1 , null , 'Karin', 'Paul' ) !
insert tab2 values ( 7, 1, 'Ilse', 'Hans' ) !
commit work !
select tab1.spalte1, tab1.spalte2, col4 from tab1, tab2
    where col4 = spalte2 !
fetch!
select tab1.spalte1, tab1.spalte2, col4 from tab1, tab2
    where col4 = spalte2 and rowno <= 3 !
fetch !
select distinct spalte2 , col4 from tab1 , tab2
    where col4 =spalte2 
    order by tab1.spalte2 !
fetch !
select distinct spalte2 , col4 from tab1 , tab2
    where col4 =spalte2 and rowno <= 3
    order by tab1.spalte2 !
fetch !
select spalte3, col3 from tab1, tab2
  where spalte3 is null and col3 is null and rowno < 3 !
fetch !
select distinct spalte3, col3 from tab1, tab2
  where spalte3 is null and col3 is null and rowno < 3 !
fetch !
select spalte1 + col1, col1, rowno from tab1 , tab2
    where spalte2 = col3 !
fetch !
select sum ( spalte1 + col1 ) , rowno from tab1 , tab2
    where spalte2 = col3 and rowno < 4 group by col1 !
fetch !
select spalte2, tab2.col3 from tab1, tab2
    where col3 is not null and rowno < 6 !
fetch !
select distinct spalte2, tab2.col3 from tab1, tab2
    where col3 is not null and rowno < 6
    order by tab1.spalte2 desc, tab2.col3 !
fetch !
select  spalte1,  spalte2, tab2.col1
    from tab1, tab2 where tab1.spalte2 < tab2.col4 and ( rowno <=5 )!
fetch !
select  distinct sum ( distinct spalte1 ),  spalte2,
    sum ( distinct tab2.col1 )
    from tab1, tab2 where tab1.spalte2 < tab2.col4 and ( rowno <=5 )
    group by spalte2 order by spalte2 !
fetch !
select tab1.spalte1, ref.spalte3, tab2.col1 from tab1, tab1 ref, tab2
    where ( tab1.spalte1 < ref.spalte3 ) and
    tab1.spalte1 < tab2.col1 !
fetch !
select tab1.spalte1, ref.spalte3, tab2.col1 from tab1, tab1 ref, tab2
    where ( tab1.spalte1 < ref.spalte3 and rowno < 4 ) and
    tab1.spalte1 < tab2.col1 !
fetch !
select tab1.spalte1, rowno from tab1 t, tab1 !
fetch!
select tab1.spalte1, rowno, 
       tab1.spalte2, rowno 
   from tab1 t , tab1 !
fetch!
select tab1.spalte1, rowno from tab1 t , tab1 group by tab1.spalte1!
fetch!
select spalte1, col1 from tab1, tab2 !
fetch !
select rowno, spalte1, col1 from tab1, tab2 !
fetch !
select distinct rowno, spalte1, col1 from tab1, tab2 !
fetch !
select distinct rowno, spalte1, col1 from tab1, tab2 where spalte1=col1!
fetch !
drop table tab1 !
drop table tab2 !
commit work release !

file connect ( kern test !
create table t ( a char (8) key, b char (4))!
insert t values ('aaa', 'aaa')!
insert t values ('a1a', 'a1a')!
create table t1 (c char (8) key, d fixed (6))!
insert t1 values ('aaa', 1)!
insert t1 values ('bbb', 5)!
insert t1 values ('ccc', 1)!
create table t2 ( e fixed (6)) !
create index i2 on t2 (e)!
insert t2 set e = 1!
insert t2 set e = 5!
select count(*) from t,t1!
fetch!
select * from t,t1 where rowno <= 1 !
fetch!
create view v as select * from t,t1 !
select * from v !
fetch!
select * from v order by c, a !
fetch!
select count(*) from v !
fetch!
select max(c) from v!
fetch!
select max(c) from v where rowno <= 1!
fetch!
select max(c) from v where rowno <= 3!
fetch!
select * from v where rowno <= 1!
fetch!
select count(*) from v where rowno <= 1!
fetch!
select count(*) from v where b = 'aaa' and rowno <= 1!
fetch!
select count(*), c from v group by c !
fetch!
select count(*), c from v where rowno <= 1 group by c !
fetch!
create view vv as select t.* from t,t tt where t.a=tt.a!
select * from vv !
fetch!
select count(*) from vv !
fetch!
select * from vv where rowno <= 1!
fetch!
select count(*) from vv where rowno <= 1!
fetch!
select count(*) from vv where rowno <= 1 and b = 'aaa' !
fetch!
select count(*) from vv where b = 'aaa' and rowno <= 1!
fetch!
create view v1 as select * from t,t1,t2
  where a=c and d=e !
commit !
select * from v1 !
fetch!
select * from v1 where b = 'aaa' and e = 1!
fetch!
select * from v1 where b = 'aaa' and e = 1 and rowno <= 1!
fetch!
select count(*) from v1!
fetch !
select count(*) from v1 where b = 'aaa' and e = 1!
fetch!
select count(*) from v1 where b = 'aaa' and e = 1 and rowno <= 1!
select max(a) from v1 where b = 'aaa' and e = 1 and rowno <= 1!
fetch!
drop table t!
drop table t1!
drop table t2!
commit release !

file connect ( kern test sqlmode sapr3 !
create table vbap (mandt char (3), vbeln char (10),
posnr integer, FAKSP char (1), primary key (mandt, vbeln,posnr))!
insert into vbap values ('204', '0070000147', 1, 'y')!
insert into vbap values ('204', '0070000147', 2, 'y')!
insert into vbap values ('204', '0070000147', 3, 'y')!
insert into vbap values ('204', '0070000147', 4, 'y')!
insert into vbap values ('204', '0070000147', 5, 'y')!
insert into vbap values ('204', '0070000147', 6, 'y')!
insert into vbap values ('204', '0070000147', 7, 'y')!
insert into vbap values ('204', '0070000147', 9, 'y')!
insert into vbap values ('204', '0070000250', 1, 'y')!
analyze table vbap statistics !
create table vbup (mandt char (3), vbeln char (10),
posnr integer, FKSTA char (1), FKSAA char(1),
primary key (mandt, vbeln,posnr))!
insert into vbup values ('204', '0070000147', 2, 'y', 'y')!
insert into vbup values ('204', '0070000147', 3, 'y', 'y')!
insert into vbup values ('204', '0070000147', 4, 'y', 'y')!
insert into vbup values ('204', '0070000147', 5, 'y', 'y')!
insert into vbup values ('204', '0070000300', 1, 'y', 'y')!
declare c cursor for 
SELECT T1."MANDT", T1."VBELN", T1."POSNR", T1."FAKSP", T2."FKSTA", 
    T2."FKSAA"                                                                 
  FROM <"VBAP" T1, "VBUP" T2>                                                      
 WHERE T2."MANDT" = T1."MANDT" AND T2."VBELN" = T1."VBELN" AND 
       T2."POSNR" = T1."POSNR" AND T2."FKSTA" <> 'C' AND 
       T2."FKSAA" <> 'C' and t1.mandt = '204' and 
       t1.vbeln = '0070000147' !
fetch c into :a, :b, :c, :d, :e, :f, :g, :h, :i ! 

pars_then_ex !
data !
declare c cursor for 
SELECT T1."MANDT", T1."VBELN", T1."POSNR", T1."FAKSP", T2."FKSTA", 
    T2."FKSAA"                                                                 
  FROM <"VBAP" T1, "VBUP" T2>                                                      
 WHERE T2."MANDT" = T1."MANDT" AND T2."VBELN" = T1."VBELN" AND 
       T2."POSNR" = T1."POSNR" AND T2."FKSTA" <> 'C' AND 
       T2."FKSAA" <> 'C' 
       and T1.mandt = :q and T1.vbeln = :r and rownum <= 1 !
c 3 '204' c 10 '0070000147'

nodata !
fetch c into :a, :b, :c, :d, :e, :f, :g, :h, :i! 
pars_execute !
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N43MAX.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file CONNECT ( KERN TEST !
create table tab ( spalte1 char(10) ,
                   spalte2 fixed(5,2),
                   spalte3 float(3),
                   spalte4 fixed(18) ) !
insert tab values ( 'a', 100 , -12E12, 400000 ) !
insert tab values ( 'b', 100 , 1e-36, -700000000000000000 ) !
insert tab values ( 'a', null , 50, 900000000000000000 ) !
insert tab values ( null, 200, null, -800000 ) !
insert tab values ( 'a', 100, +1E-36 , 800000000000000000 ) !
insert tab values ( null, null, null, null ) !
insert tab values ( 'c', -600, -12E12, null ) !
insert tab values ( 'a', 100, +1E-36 , 800000000000000000 ) !
insert tab values ( 'b', null, null, + 900000000000000000 ) !
select * from tab !
fetch !
create index "tab.spalte2" on tab( spalte2 ) !
create index "tab.spalte4" on tab( spalte4 ) !
commit work !
select tab.spalte1, ref.spalte1,max( tab.spalte4 ), max( ref.spalte4 )
    from tab, tab ref
    where tab.spalte4 > 0 group by tab.spalte1, ref.spalte1
    having max ( tab.spalte4 ) = min ( ref.spalte4 ) order by 2 !
select tab.spalte1, ref.spalte1,max( tab.spalte4 ), max( ref.spalte4 )
    from tab, tab ref
    where tab.spalte4 > 0 group by tab.spalte1, ref.spalte1
    having min ( tab.spalte4 ) = max ( ref.spalte4 ) order by 2 !
fetch !
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N44INDEX.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

file connect ( kern test !
create table aa (
aadstnr    char (5 ) key,
aadat      char (6 ) key,
aalfdnr    char (5 ) key,
aaswa      char (14) ,
aaadat     char (6 ) ,
aabkza     char (7 ) ,
aaregdst   char (2 ) ,
aawvdat    char (6 ) ,
aaabeg     char (6 ) ,
aabewart   char (1 ) ,
aabewul1   char (1 ) ,
aavab      char (6 ) ,
aavbis     char (6 ) ,
aaschart   char (4 ) ,
aabeh      char (1 ) ,
aanat      char (3 ) ,
aadauf     char (3 ) ,
aasopro    char (1 ) ,
aastatkn   char (1 ) ,
aastatdt   char (6 ) ,
aavstat    char (1 ) ,
aabesr1    char (1 ) ,
aabesr2    char (3 ) ,
aalevv     char (6 ) ,
aamitf1    char (5 ) ,
aamitf2    char (5 ) ,
aamitf3    char (5 ) ,
aamitf4    char (5 ) ,
aaevor1    char (24) ,
aaevor2    char (30) ,
aaevor3    char (30) ,
aaevor4    char (30) ,
aaevor5    char (16) ,
aabewul2   char (1 ) ,
aabewul3   char (1 ) ,
aaanzvv    char (3 ) ,
aagst      char (3 ) ,
aaost      char (3 ) ) !
create table ab (
abswb      char (14) key,
abbkzb     char (7 ) ,
abwzw      char (3 ) ,
abstrb     char (32) ,
abplzb     char (4 ) ,
abortb     char (32) ,
abaparb    char (32) ,
abzust     char (3 ) ,
ab18aab    char (6 ) ,
ab18aan    char (6 ) ,
ab18ale    char (1 ) ,
abtrart    char (32) ,
abegro     char (3 ) ,
abangam    char (6 ) ,
absvkn1    char (1 ) ,
absvdt1    char (6 ) ,
absvkn2    char (1 ) ,
absvdt2    char (6 ) ,
absvkn3    char (1 ) ,
absvdt3    char (6 ) ,
absvkn4    char (1 ) ,
absvdt4    char (6 ) ,
absvkn5    char (1 ) ,
absvdt5    char (6 ) ,
absvkn6    char (1 ) ,
absvdt6    char (6 ) ,
abbemk1    char (18) ,
abbemk2    char (31) ,
abbemk3    char (31) ,
abbemk4    char (31) ,
abbemk5    char (31) ,
abbemk6    char (31) ,
abbemk7    char (31) ,
abbemk8    char (31) ,
abbemk9    char (31) ,
ablekon    char (1 ) ,
ablekdt    char (6 ) ,
ablebar    char (6 ) ) !
create index "aa.aabkza" on aa( aabkza ) !
create index "aa.aaswa" on aa( aaswa ) !
insert aa set aadstnr = '12345', aadat = '050687', aalfdnr = '1',
aaswa = 'abcdef', aabkza = '52301', aastatkn = 'O' !
insert ab set abswb = 'abcdef', abbkzb = '52301',
abortb = 'A13' !
select ab.abortb, aa.aabkza, ab.abswb, aa.aastatkn, aa.aaost, aa.aaadat
from aa, ab where
aa.aabkza = ab.abbkzb and
ab.abswb = aa.aaswa and
aa.aabkza between '0110' and '53120' and
ab.abortb like 'A13'
order by ab.abortb, aa.aabkza, ab.abswb !
fetch !
select ab.abortb, aa.aabkza, ab.abswb, aa.aastatkn, aa.aaost, aa.aaadat
from aa, ab where
aa.aastatkn = 'O' and
aa.aabkza = ab.abbkzb and
ab.abswb = aa.aaswa and
aa.aabkza between '0110' and '53120' and
ab.abortb like 'A13'
order by ab.abortb, aa.aabkza, ab.abswb !
fetch !
commit work release !

file connect ( kern test !
create table t ( a fixed (3), b fixed (6,0))!
insert t set a = 3, b = 6!
create index i1 on t (a desc, b)!
create table t1 ( c fixed (10))!
insert t1 set c = 10!
select * from t, t1 where a = 3 and b = 6!
fetch!
pars_then_ex !
data!
select * from t, t1 where a = :a and b = :b!
n 3 3 n 6 6

nodata!
fetch!
pars_execute!
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N53FIXED.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table tab ( spalte1 float(3), spalte2 float(18), spalte3 fixed(5),
   spalte4 fixed(8,3), spalte5 char(10) ) !
insert tab values ( 5E13, 9E-12, 88, 4444.555, 'aaa' ) !
commit work !
create table t1 (a fixed (7))!
insert t1 set a = 3!
select fixed ( spalte1, 15, 1 ) + fixed ( a , 15, 1 ) from tab, t1!
fetch!
select fixed (spalte1 + a, 12, 4) from tab, t1!
fetch!
commit work release !
file cleark !
file connect ( kern test sqlmode db2 !
create table tab ( spalte1 float(3), spalte2 float(18), spalte3 decimal(5),
   spalte4 decimal(8,3), spalte5 char(10) ) !
insert into tab values ( 5E13, 9E-12, 88, 4444.555, 'aaa' ) !
commit work !
create table t1 (a decimal (7))!
insert into t1 values ( 3 )!
declare c cursor for select decimal ( spalte1, 15, 1 ) +
   decimal ( a , 15, 1 ) from tab, t1!
fetch c into :a !
close c!
declare c cursor for select decimal (spalte1 + a, 12, 4) from tab, t1!
fetch c into :a !
close c!
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N54VIEW.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file CONNECT ( KERN TEST !
create table t1 ( t1xc1 fixed(2) key, t1xc2 fixed(2) key,
                  t1xc3 fixed(2) ) !
create table t2 ( t2xc1 fixed(2) key, t2xc2 fixed(2), t2xc3 fixed(2) ) !
create table t3 ( t3xc1 fixed(2), t3xc2 fixed(2), t3xc3 fixed(2) ) !
create view v1 as select * from t1, t2, t3
   where t1xc1 = t2xc1 and t2xc2 = t3xc1 !
insert t1 values ( 2, 3, 5 ) !
insert t1 values ( 4, 1, 3 ) !
insert t1 values ( 8, 3, 1 ) !
insert t2 values ( 4, 2, 6 ) !
insert t2 values ( 8, 1, 5 ) !
insert t2 values ( 2, 5, 5 ) !
insert t3 values ( 2, 3, 3 ) !
insert t3 values ( 1, 6, 6 ) !
insert t3 values ( 1, 2, 4 ) !
insert t3 values ( 5, 2, 3 ) !
insert t3 values ( 3, 1, 7 ) !
commit work !
select * from v1 !
fetch !
select * from v1 where t1xc1 < t3xc1 !
fetch !
select * from v1 where t1xc1 + t2xc3 < 7 !
select * from v1 where t1xc2 > t2xc2 and t1xc3 + t3xc1 = t3xc2 !
fetch !
select * from v1 where t1xc2 > t2xc2 and t2xc3 > t3xc3 !
fetch !
declare erg1 cursor for 
select * from v1 where t1xc2 = t3xc3 or t1xc2 < t2xc2 !
select * from erg1 !
fetch !
declare erg2 cursor for 
select * from v1 where t1xc2 = t3xc3 and t1xc3 = t2xc2 !
fetch erg2!
select distinct t1xc1, t2xc3 from v1 !
fetch !
select * from v1 k where k.t2xc2 = k.t1xc3 !
fetch !
select k.t1xc1, t1xc2 from v1 k where t1xc1 < k.t1xc2 !
fetch !
select t1xc1, t2xc2 neu1 from v1 where t1xc1 < 3 !
fetch !
select v1.* from v1 !
fetch !
select k.* from v1 k !
fetch !
select t1.* from v1 !
select t1xc1 + t2xc2 + t3xc1 neu1 from v1 where t1xc1 / 2 = t3xc3 !
fetch !
select all v1.t1xc1, t1xc2, kern.v1.t2xc2 from kern.v1
   where v1.t1xc1 + t1xc2 > kern.v1.t2xc2  order by t1xc2 !
fetch !
select t1xc2, t2xc1, t3xc3 from v1
   where t1xc2 < t3xc3 and t1xc2 < t2xc1
   order by 3 asc !
fetch !
select t1xc1, t1xc2, t2xc3 from v1
   where t1xc1 + t1xc2 <= t2xc3
   order by 2 asc, v1.t2xc3 desc !
fetch !
select t1xc1, sum ( t1xc1), min (t3xc3) from v1 group by t1xc1 !
fetch !
select count (*), t1xc2 from v1 where t3xc2 <> 6
   group by t1xc2 having sum ( t1xc2 ) > 5 !
fetch !
create view v2 as select * from v1 where t1xc2 < t3xc3 !
select * from v2 !
fetch !
select * from v2 where 2 * t1xc3 = t2xc3 !
fetch !
declare erg3 cursor for
select distinct t1xc1, t1xc2, t2xc1, t2xc2  from v2 !
fetch erg3!
create view v3 as select t1xc2, t1xc3, t2xc1, t2xc2, t3xc2 from v2
   where  ( t1xc3 = t2xc2 and ( t1xc2 > t3xc2 )) !
declare erg4 cursor for 
select * from v3 !
fetch erg4!
select t1xc3 neu1, 5 from v3 !
fetch !
select * from v3 where t1xc3 < t2xc1 !
fetch !
commit work release !

file connect ( kern test !
CREATE TABLE "COMPANY"
(
	"ID"   Float (20)  ,
	"NAME"     Varchar (100) ASCII  ,
	"TOKEN"     Varchar (10) ASCII  ,
	"STREET"     Varchar (50) ASCII,
	"STREET_NR"     Varchar (5) ASCII,
	"POSTCODE"     Varchar (10) ASCII,
	"CITY"     Varchar (50) ASCII,
	"ZONE"     Varchar (5) ASCII,
	"TYPE"     Char (1) ASCII    DEFAULT '0',
	"STATUS"     Char (1) ASCII  ,
	"ORDER_SEQUENCE"     Fixed (3,0)    DEFAULT  0,
	"INSERTED"     Timestamp    DEFAULT TIMESTAMP,
	"INSERTED_BY"     Varchar (50) ASCII    DEFAULT 'SYSTEM',
	"UPDATED"     Timestamp    DEFAULT TIMESTAMP,
	"UPDATED_BY"     Varchar (50) ASCII    DEFAULT 'SYSTEM',
	PRIMARY KEY ("ID"),
	CONSTRAINT "ZONE" CHECK	"ZONE" IN ('','A','B','C','D','E','F','G')   OR
					"ZONE" IS NULL,
	CONSTRAINT "STATUS" CHECK	"STATUS" IN ('0','1'),
	CONSTRAINT "TYPE" CHECK	"TYPE" IN ('0','1')
)
!
insert company (ID, NAME, STATUS) values (1, 'SAP', '1')!

CREATE TABLE "POWER_PRICE"
(
	"ID_COMPANY"     Float (20)  ,
	"ZONE"     Varchar (5) ASCII  ,
	"VALUE_DATE"     Timestamp  ,
	"POWER1"     Varchar (7) ASCII    DEFAULT '0',
	"POWER2"     Varchar (7) ASCII    DEFAULT '0',
	"POWER3"     Varchar (7) ASCII    DEFAULT '0',
	"POWER4"     Varchar (7) ASCII    DEFAULT '0',
	"POWER5"     Varchar (7) ASCII    DEFAULT '0',
	"POWER6"     Varchar (7) ASCII    DEFAULT '0',
	"POWER7"     Varchar (7) ASCII    DEFAULT '0',
	"POWER8"     Varchar (7) ASCII    DEFAULT '0',
	"POWER9"     Varchar (7) ASCII    DEFAULT '0',
	"POWER10"     Varchar (7) ASCII    DEFAULT '0',
	"POWER11"     Varchar (7) ASCII    DEFAULT '0',
	"POWER12"     Varchar (7) ASCII    DEFAULT '0',
	"POWER13"     Varchar (7) ASCII    DEFAULT '0',
	"POWER14"     Varchar (7) ASCII    DEFAULT '0',
	"POWER15"     Varchar (7) ASCII    DEFAULT '0',
	"POWER16"     Varchar (7) ASCII    DEFAULT '0',
	"POWER17"     Varchar (7) ASCII    DEFAULT '0',
	"POWER18"     Varchar (7) ASCII    DEFAULT '0',
	"POWER19"     Varchar (7) ASCII    DEFAULT '0',
	"POWER20"     Varchar (7) ASCII    DEFAULT '0',
	"POWER21"     Varchar (7) ASCII    DEFAULT '0',
	"POWER22"     Varchar (7) ASCII    DEFAULT '0',
	"POWER23"     Varchar (7) ASCII    DEFAULT '0',
	"POWER24"     Varchar (7) ASCII    DEFAULT '0',
	"POWER25"     Varchar (7) ASCII    DEFAULT '0',
	"POWER26"     Varchar (7) ASCII    DEFAULT '0',
	"POWER27"     Varchar (7) ASCII    DEFAULT '0',
	"POWER28"     Varchar (7) ASCII    DEFAULT '0',
	"POWER29"     Varchar (7) ASCII    DEFAULT '0',
	"POWER30"     Varchar (7) ASCII    DEFAULT '0',
	"POWER31"     Varchar (7) ASCII    DEFAULT '0',
	"POWER32"     Varchar (7) ASCII    DEFAULT '0',
	"POWER33"     Varchar (7) ASCII    DEFAULT '0',
	"POWER34"     Varchar (7) ASCII    DEFAULT '0',
	"POWER35"     Varchar (7) ASCII    DEFAULT '0',
	"POWER36"     Varchar (7) ASCII    DEFAULT '0',
	"POWER37"     Varchar (7) ASCII    DEFAULT '0',
	"POWER38"     Varchar (7) ASCII    DEFAULT '0',
	"POWER39"     Varchar (7) ASCII    DEFAULT '0',
	"POWER40"     Varchar (7) ASCII    DEFAULT '0',
	"POWER41"     Varchar (7) ASCII    DEFAULT '0',
	"POWER42"     Varchar (7) ASCII    DEFAULT '0',
	"POWER43"     Varchar (7) ASCII    DEFAULT '0',
	"POWER44"     Varchar (7) ASCII    DEFAULT '0',
	"POWER45"     Varchar (7) ASCII    DEFAULT '0',
	"POWER46"     Varchar (7) ASCII    DEFAULT '0',
	"POWER47"     Varchar (7) ASCII    DEFAULT '0',
	"POWER48"     Varchar (7) ASCII    DEFAULT '0',
	"POWER49"     Varchar (7) ASCII    DEFAULT '0',
	"POWER50"     Varchar (7) ASCII    DEFAULT '0',
	"POWER51"     Varchar (7) ASCII    DEFAULT '0',
	"POWER52"     Varchar (7) ASCII    DEFAULT '0',
	"POWER53"     Varchar (7) ASCII    DEFAULT '0',
	"POWER54"     Varchar (7) ASCII    DEFAULT '0',
	"POWER55"     Varchar (7) ASCII    DEFAULT '0',
	"POWER56"     Varchar (7) ASCII    DEFAULT '0',
	"POWER57"     Varchar (7) ASCII    DEFAULT '0',
	"POWER58"     Varchar (7) ASCII    DEFAULT '0',
	"POWER59"     Varchar (7) ASCII    DEFAULT '0',
	"POWER60"     Varchar (7) ASCII    DEFAULT '0',
	"POWER61"     Varchar (7) ASCII    DEFAULT '0',
	"POWER62"     Varchar (7) ASCII    DEFAULT '0',
	"POWER63"     Varchar (7) ASCII    DEFAULT '0',
	"POWER64"     Varchar (7) ASCII    DEFAULT '0',
	"POWER65"     Varchar (7) ASCII    DEFAULT '0',
	"POWER66"     Varchar (7) ASCII    DEFAULT '0',
	"POWER67"     Varchar (7) ASCII    DEFAULT '0',
	"POWER68"     Varchar (7) ASCII    DEFAULT '0',
	"POWER69"     Varchar (7) ASCII    DEFAULT '0',
	"POWER70"     Varchar (7) ASCII    DEFAULT '0',
	"POWER71"     Varchar (7) ASCII    DEFAULT '0',
	"POWER72"     Varchar (7) ASCII    DEFAULT '0',
	"POWER73"     Varchar (7) ASCII    DEFAULT '0',
	"POWER74"     Varchar (7) ASCII    DEFAULT '0',
	"POWER75"     Varchar (7) ASCII    DEFAULT '0',
	"POWER76"     Varchar (7) ASCII    DEFAULT '0',
	"POWER77"     Varchar (7) ASCII    DEFAULT '0',
	"POWER78"     Varchar (7) ASCII    DEFAULT '0',
	"POWER79"     Varchar (7) ASCII    DEFAULT '0',
	"POWER80"     Varchar (7) ASCII    DEFAULT '0',
	"POWER81"     Varchar (7) ASCII    DEFAULT '0',
	"POWER82"     Varchar (7) ASCII    DEFAULT '0',
	"POWER83"     Varchar (7) ASCII    DEFAULT '0',
	"POWER84"     Varchar (7) ASCII    DEFAULT '0',
	"POWER85"     Varchar (7) ASCII    DEFAULT '0',
	"POWER86"     Varchar (7) ASCII    DEFAULT '0',
	"POWER87"     Varchar (7) ASCII    DEFAULT '0',
	"POWER88"     Varchar (7) ASCII    DEFAULT '0',
	"POWER89"     Varchar (7) ASCII    DEFAULT '0',
	"POWER90"     Varchar (7) ASCII    DEFAULT '0',
	"POWER91"     Varchar (7) ASCII    DEFAULT '0',
	"POWER92"     Varchar (7) ASCII    DEFAULT '0',
	"POWER93"     Varchar (7) ASCII    DEFAULT '0',
	"POWER94"     Varchar (7) ASCII    DEFAULT '0',
	"POWER95"     Varchar (7) ASCII    DEFAULT '0',
	"POWER96"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE1"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE2"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE3"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE4"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE5"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE6"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE7"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE8"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE9"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE10"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE11"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE12"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE13"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE14"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE15"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE16"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE17"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE18"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE19"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE20"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE21"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE22"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE23"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE24"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE25"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE26"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE27"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE28"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE29"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE30"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE31"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE32"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE33"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE34"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE35"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE36"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE37"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE38"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE39"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE40"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE41"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE42"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE43"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE44"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE45"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE46"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE47"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE48"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE49"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE50"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE51"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE52"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE53"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE54"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE55"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE56"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE57"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE58"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE59"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE60"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE61"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE62"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE63"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE64"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE65"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE66"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE67"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE68"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE69"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE70"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE71"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE72"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE73"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE74"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE75"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE76"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE77"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE78"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE79"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE80"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE81"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE82"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE83"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE84"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE85"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE86"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE87"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE88"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE89"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE90"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE91"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE92"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE93"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE94"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE95"     Varchar (7) ASCII    DEFAULT '0',
	"PRICE96"     Varchar (7) ASCII    DEFAULT '0',
	"INSERTED"     Timestamp    DEFAULT TIMESTAMP,
	"INSERTED_BY"     Varchar (50) ASCII    DEFAULT 'SYSTEM',
	"UPDATED"     Timestamp    DEFAULT TIMESTAMP,
	"UPDATED_BY"     Varchar (50) ASCII    DEFAULT 'SYSTEM',
	PRIMARY KEY ("ID_COMPANY", "ZONE", "VALUE_DATE"),
	CONSTRAINT "ZONE" CHECK	"ZONE" IN ('','A','B','C','D','E','F','G'),
	FOREIGN KEY "FK_REF_4511" ("ID_COMPANY") 
    REFERENCES "COMPANY" ("ID") ON DELETE CASCADE
)
!
insert POWER_PRICE (ID_COMPANY, POWER41,"ZONE", "VALUE_DATE") 
 values (1, '41', 'E', timestamp)!

CREATE INDEX "IX01_POWER_PRICE" ON "POWER_PRICE"("ID_COMPANY" ASC)!
CREATE INDEX "IX02_POWER_PRICE" ON "POWER_PRICE"("VALUE_DATE" ASC)!

CREATE TABLE "CONTACT"
(
	"ID"     Float (20)  ,
	"ID_COMPANY"     Float (20)  ,
	"TYPE"     Float (20)  ,
	"TITLE"     Varchar (20) ASCII,
	"FIRSTNAME"     Varchar (50) ASCII,
	"SURNAME"     Varchar (50) ASCII,
	"PHONE_PREFIX"     Varchar (6) ASCII,
	"PHONE"     Varchar (25) ASCII,
	"PHONE_SUFFIX"     Varchar (4) ASCII,
	"FAX_PREFIX"     Varchar (6) ASCII,
	"FAX"     Varchar (25) ASCII,
	"FAX_SUFFIX"     Varchar (4) ASCII,
	"EMAIL"     Varchar (50) ASCII,
	"ORDER_SEQUENCE"     Fixed (3,0)    DEFAULT  0,
	"INSERTED"     Timestamp    DEFAULT TIMESTAMP,
	"INSERTED_BY"     Varchar (50) ASCII    DEFAULT 'SYSTEM',
	"UPDATED"     Timestamp    DEFAULT TIMESTAMP,
	"UPDATED_BY"     Varchar (50) ASCII    DEFAULT 'SYSTEM',
	PRIMARY KEY ("ID", "ID_COMPANY"),
	CONSTRAINT "TYPE" CHECK	"TYPE" IN (10,20,30,40,50,60),
	FOREIGN KEY "FK_REF_2866"	("ID_COMPANY") 
    REFERENCES "COMPANY" ("ID") ON DELETE CASCADE
)!
insert contact (id, type, ID_COMPANY) values (4711,40,1)!

CREATE VIEW "VW_COMPANY_CONTACT"
 (
	"ID_COMPANY", 
	"TOKEN", 
	"ZONE", 
	"COMPANY_STATUS", 
	"COMPANY_ORDER_SEQUENCE", 
	"PHONE_PREFIX", 
	"PHONE", 
	"PHONE_SUFFIX"
)
AS 
SELECT A.ID ID_COMPANY, A.TOKEN, A.ZONE, A.STATUS COMPANY_STATUS, 
A.ORDER_SEQUENCE COMPANY_ORDER_SEQUENCE,
	 B.PHONE_PREFIX, B.PHONE, B.PHONE_SUFFIX
FROM COMPANY A, CONTACT B 
WHERE A.ID = B.ID_COMPANY(+) AND B.TYPE(+) = 40 !

* here is the problem: the toooo long select, which has to be
* stored intermediately !
pars_then_ex !
SELECT A.ID_COMPANY, A.VALUE_DATE, B.TOKEN COMPANY_TOKEN, A.ZONE, 
 		DECODE(DECODE(B.PHONE_PREFIX, 
					NULL, 
						B.PHONE, 
						DECODE(B.PHONE, 
							NULL, 
							B.PHONE_PREFIX, 
							
B.PHONE_PREFIX || '/' || B.PHONE)), 
				NULL, 
				B.PHONE_SUFFIX, 
				DECODE(B.PHONE_PREFIX, 
					NULL, 
						B.PHONE, 
						DECODE(B.PHONE, 
							NULL, 
							B.PHONE_PREFIX, 
							
B.PHONE_PREFIX || '/' || B.PHONE)) || DECODE(B.PHONE_SUFFIX, 
								
					NULL, 
								
					B.PHONE_SUFFIX, 	
'-' || B.PHONE_SUFFIX)) PHONE, 
	 num(A.POWER1)  POWER1 , num(A.POWER2)  POWER2 , 
num(A.POWER3)  POWER3 , num(A.POWER4)  POWER4 , num(A.POWER5) 
 POWER5 , num(A.POWER6)  POWER6 , 
	 num(A.POWER7)  POWER7 , num(A.POWER8)  POWER8 , 
num(A.POWER9)  POWER9 , num(A.POWER10) POWER10, 
num(A.POWER11) POWER11, num(A.POWER12) POWER12, 
	 num(A.POWER13) POWER13, num(A.POWER14) POWER14, 
num(A.POWER15) POWER15, num(A.POWER16) POWER16, 
num(A.POWER17) POWER17, num(A.POWER18) POWER18, 
	 num(A.POWER19) POWER19, num(A.POWER20) POWER20, 
num(A.POWER21) POWER21, num(A.POWER22) POWER22, 
num(A.POWER23) POWER23, num(A.POWER24) POWER24, 
	 num(A.POWER25) POWER25, num(A.POWER26) POWER26, 
num(A.POWER27) POWER27, num(A.POWER28) POWER28, 
num(A.POWER29) POWER29, num(A.POWER30) POWER30, 
	 num(A.POWER31) POWER31, num(A.POWER32) POWER32, 
num(A.POWER33) POWER33, num(A.POWER34) POWER34, 
num(A.POWER35) POWER35, num(A.POWER36) POWER36, 
	 num(A.POWER37) POWER37, num(A.POWER38) POWER38, 
num(A.POWER39) POWER39, num(A.POWER40) POWER40, 
num(A.POWER41) POWER41, num(A.POWER42) POWER42, 
	 num(A.POWER43) POWER43, num(A.POWER44) POWER44, 
num(A.POWER45) POWER45, num(A.POWER46) POWER46, 
num(A.POWER47) POWER47, num(A.POWER48) POWER48, 
	 num(A.POWER49) POWER49, num(A.POWER50) POWER50, 
num(A.POWER51) POWER51, num(A.POWER52) POWER52, 
num(A.POWER53) POWER53, num(A.POWER54) POWER54, 
	 num(A.POWER55) POWER55, num(A.POWER56) POWER56, 
num(A.POWER57) POWER57, num(A.POWER58) POWER58, 
num(A.POWER59) POWER59, num(A.POWER60) POWER60, 
	 num(A.POWER61) POWER61, num(A.POWER62) POWER62, 
num(A.POWER63) POWER63, num(A.POWER64) POWER64, 
num(A.POWER65) POWER65, num(A.POWER66) POWER66, 
	 num(A.POWER67) POWER67, num(A.POWER68) POWER68, 
num(A.POWER69) POWER69, num(A.POWER70) POWER70, 
num(A.POWER71) POWER71, num(A.POWER72) POWER72, 
	 num(A.POWER73) POWER73, num(A.POWER74) POWER74, 
num(A.POWER75) POWER75, num(A.POWER76) POWER76, 
num(A.POWER77) POWER77, num(A.POWER78) POWER78, 
	 num(A.POWER79) POWER79, num(A.POWER80) POWER80, 
num(A.POWER81) POWER81, num(A.POWER82) POWER82, 
num(A.POWER83) POWER83, num(A.POWER84) POWER84, 
	 num(A.POWER85) POWER85, num(A.POWER86) POWER86, 
num(A.POWER87) POWER87, num(A.POWER88) POWER88, 
num(A.POWER89) POWER89, num(A.POWER90) POWER90, 
	 num(A.POWER91) POWER91, num(A.POWER92) POWER92, 
num(A.POWER93) POWER93, num(A.POWER94) POWER94, 
num(A.POWER95) POWER95, num(A.POWER96) POWER96, 
	 num(A.PRICE1)  PRICE1 , num(A.PRICE2)  PRICE2 , 
num(A.PRICE3)  PRICE3 , num(A.PRICE4)  PRICE4 , num(A.PRICE5) 
 PRICE5 , num(A.PRICE6)  PRICE6 , 
	 num(A.PRICE7)  PRICE7 , num(A.PRICE8)  PRICE8 , 
num(A.PRICE9)  PRICE9 , num(A.PRICE10) PRICE10, 
num(A.PRICE11) PRICE11, num(A.PRICE12) PRICE12, 
	 num(A.PRICE13) PRICE13, num(A.PRICE14) PRICE14, 
num(A.PRICE15) PRICE15, num(A.PRICE16) PRICE16, 
num(A.PRICE17) PRICE17, num(A.PRICE18) PRICE18, 
	 num(A.PRICE19) PRICE19, num(A.PRICE20) PRICE20, 
num(A.PRICE21) PRICE21, num(A.PRICE22) PRICE22, 
num(A.PRICE23) PRICE23, num(A.PRICE24) PRICE24, 
	 num(A.PRICE25) PRICE25, num(A.PRICE26) PRICE26, 
num(A.PRICE27) PRICE27, num(A.PRICE28) PRICE28, 
num(A.PRICE29) PRICE29, num(A.PRICE30) PRICE30, 
	 num(A.PRICE31) PRICE31, num(A.PRICE32) PRICE32, 
num(A.PRICE33) PRICE33, num(A.PRICE34) PRICE34, 
num(A.PRICE35) PRICE35, num(A.PRICE36) PRICE36, 
	 num(A.PRICE37) PRICE37, num(A.PRICE38) PRICE38, 
num(A.PRICE39) PRICE39, num(A.PRICE40) PRICE40, 
num(A.PRICE41) PRICE41, num(A.PRICE42) PRICE42, 
	 num(A.PRICE43) PRICE43, num(A.PRICE44) PRICE44, 
num(A.PRICE45) PRICE45, num(A.PRICE46) PRICE46, 
num(A.PRICE47) PRICE47, num(A.PRICE48) PRICE48, 
	 num(A.PRICE49) PRICE49, num(A.PRICE50) PRICE50, 
num(A.PRICE51) PRICE51, num(A.PRICE52) PRICE52, 
num(A.PRICE53) PRICE53, num(A.PRICE54) PRICE54, 
	 num(A.PRICE55) PRICE55, num(A.PRICE56) PRICE56, 
num(A.PRICE57) PRICE57, num(A.PRICE58) PRICE58, 
num(A.PRICE59) PRICE59, num(A.PRICE60) PRICE60, 
	 num(A.PRICE61) PRICE61, num(A.PRICE62) PRICE62, 
num(A.PRICE63) PRICE63, num(A.PRICE64) PRICE64, 
num(A.PRICE65) PRICE65, num(A.PRICE66) PRICE66, 
	 num(A.PRICE67) PRICE67, num(A.PRICE68) PRICE68, 
num(A.PRICE69) PRICE69, num(A.PRICE70) PRICE70, 
num(A.PRICE71) PRICE71, num(A.PRICE72) PRICE72, 
	 num(A.PRICE73) PRICE73, num(A.PRICE74) PRICE74, 
num(A.PRICE75) PRICE75, num(A.PRICE76) PRICE76, 
num(A.PRICE77) PRICE77, num(A.PRICE78) PRICE78, 
	 num(A.PRICE79) PRICE79, num(A.PRICE80) PRICE80, 
num(A.PRICE81) PRICE81, num(A.PRICE82) PRICE82, 
num(A.PRICE83) PRICE83, num(A.PRICE84) PRICE84, 
	 num(A.PRICE85) PRICE85, num(A.PRICE86) PRICE86, 
num(A.PRICE87) PRICE87, num(A.PRICE88) PRICE88, 
num(A.PRICE89) PRICE89, num(A.PRICE90) PRICE90, 
	 num(A.PRICE91) PRICE91, num(A.PRICE92) PRICE92, 
num(A.PRICE93) PRICE93, num(A.PRICE94) PRICE94, 
num(A.PRICE95) PRICE95, num(A.PRICE96) PRICE96 
FROM POWER_PRICE A, VW_COMPANY_CONTACT B 
WHERE A.ID_COMPANY 		 = B.ID_COMPANY 
AND	  B.COMPANY_STATUS	 = '1' 
AND  (num(A.POWER1)  <> 0 OR num(A.POWER2)  <> 0 OR 
num(A.POWER3)  <> 0 OR num(A.POWER4)  <> 0 OR num(A.POWER5)  
<> 0 OR num(A.POWER6)  <> 0 OR num(A.POWER7)  <> 0 
OR  num(A.POWER8)  <> 0 OR num(A.POWER9)  <> 0 OR 
num(A.POWER10) <> 0 OR num(A.POWER11) <> 0 OR num(A.POWER12) 
<> 0 OR num(A.POWER13) <> 0 OR num(A.POWER14) <> 0 
OR  num(A.POWER15) <> 0 OR num(A.POWER16) <> 0 OR 
num(A.POWER17) <> 0 OR num(A.POWER18) <> 0 OR num(A.POWER19) 
<> 0 OR num(A.POWER20) <> 0 OR num(A.POWER21) <> 0 
OR  num(A.POWER22) <> 0 OR num(A.POWER23) <> 0 OR 
num(A.POWER24) <> 0 OR num(A.POWER25) <> 0 OR num(A.POWER26) 
<> 0 OR num(A.POWER27) <> 0 OR num(A.POWER28) <> 0 
OR  num(A.POWER29) <> 0 OR num(A.POWER30) <> 0 OR 
num(A.POWER31) <> 0 OR num(A.POWER32) <> 0 OR num(A.POWER33) 
<> 0 OR num(A.POWER34) <> 0 OR num(A.POWER35) <> 0 
OR  num(A.POWER36) <> 0 OR num(A.POWER37) <> 0 OR 
num(A.POWER38) <> 0 OR num(A.POWER39) <> 0 OR num(A.POWER40) 
<> 0 OR num(A.POWER41) <> 0 OR num(A.POWER42) <> 0 
OR  num(A.POWER43) <> 0 OR num(A.POWER44) <> 0 OR 
num(A.POWER45) <> 0 OR num(A.POWER46) <> 0 OR num(A.POWER47) 
<> 0 OR num(A.POWER48) <> 0 OR num(A.POWER49) <> 0 
OR  num(A.POWER50) <> 0 OR num(A.POWER51) <> 0 OR 
num(A.POWER52) <> 0 OR num(A.POWER53) <> 0 OR num(A.POWER54) 
<> 0 OR num(A.POWER55) <> 0 OR num(A.POWER56) <> 0
OR  num(A.POWER57) <> 0 OR num(A.POWER58) <> 0 OR 
num(A.POWER59) <> 0 OR num(A.POWER60) <> 0 OR num(A.POWER61) 
<> 0 OR num(A.POWER62) <> 0 OR num(A.POWER63) <> 0 
OR  num(A.POWER64) <> 0 OR num(A.POWER65) <> 0 OR 
num(A.POWER66) <> 0 OR num(A.POWER67) <> 0 OR num(A.POWER68) 
<> 0 OR num(A.POWER69) <> 0 OR num(A.POWER70) <> 0
OR  num(A.POWER71) <> 0 OR num(A.POWER72) <> 0 OR 
num(A.POWER73) <> 0 OR num(A.POWER74) <> 0 OR num(A.POWER75) 
<> 0 OR num(A.POWER76) <> 0 OR num(A.POWER77) <> 0
OR  num(A.POWER78) <> 0 OR num(A.POWER79) <> 0 OR 
num(A.POWER80) <> 0 OR num(A.POWER81) <> 0 OR num(A.POWER82) 
<> 0 OR num(A.POWER83) <> 0 OR num(A.POWER84) <> 0
OR  num(A.POWER85) <> 0 OR num(A.POWER86) <> 0 OR 
num(A.POWER87) <> 0 OR num(A.POWER88) <> 0 OR num(A.POWER89) 
<> 0 OR num(A.POWER90) <> 0 OR num(A.POWER91) <> 0 
OR  num(A.POWER92) <> 0 OR num(A.POWER93) <> 0 OR 
num(A.POWER94) <> 0 OR num(A.POWER95) <> 0 OR num(A.POWER96) <> 0)
!
fetch!
pars_execute!
rollback release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N55VIEW.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( t1xc1 fixed(2) key, t1xc2 fixed(2) key,
                  t1xc3 fixed(2) ) !
create table t2 ( t2xc1 fixed(2) key, t2xc2 fixed(2), t2xc3 fixed(2) ) !
create table t3 ( t3xc1 fixed(2), t3xc2 fixed(2), t3xc3 fixed(2) ) !
create view v1 as select * from t1, t2, t3
   where t1xc1 = t2xc1 and t2xc2 = t3xc1 !
create view v2 as select * from t1,t2,t3
   where t1.t1xc3 = kern.t2.t2xc2 and (( t3xc1 + 1 ) < t2xc3 ) !
insert t1 values ( 2, 3, 5 ) !
insert t1 values ( 4, 1, 3 ) !
insert t1 values ( 8, 3, 1 ) !
insert t2 values ( 4, 2, 6 ) !
insert t2 values ( 8, 1, 5 ) !
insert t2 values ( 2, 5, 5 ) !
insert t3 values ( 2, 3, 3 ) !
insert t3 values ( 1, 6, 6 ) !
insert t3 values ( 1, 2, 4 ) !
insert t3 values ( 5, 2, 3 ) !
insert t3 values ( 3, 1, 7 ) !
commit work !
select * from v1 !
fetch !
select * from v2 !
fetch !
commit work !
create view v3 as select
   v1.t1xc1 v1t11, v1.t1xc2 v1t12, v1.t2xc1 v1t21,
   v1.t3xc1 v1t31, v1.t3xc3 v1t33,
   v2.t1xc1 v2t11, v2.t1xc2 v2t12, v2.t2xc1 v2t21,
   v2.t3xc1 v2t31, v2.t3xc3 v2t33
   from v1,v2
   where v1.t1xc1 = v2.t1xc1 and v1.t1xc2 = v2.t3xc1 !
select * from v3 !
fetch !
select * from v3 where v1t33 < v2t21 !
fetch !
select k.v1t12, v1t33, v2t21 neu from v3 k
   where k.v1t12 < v1t33 and ( v1t33 * 2 = k.v2t21 ) !
fetch !
declare erg1 cursor for 
select v3.v1t11, kern.v3.v1t33 , k.v2t33 
   from v3, v3 k
   where v3.v1t11 < k.v2t33 !
select * from erg1 !
fetch !
select v3.v1t11, sum (v1t33) from v3 group by v3.v1t11 !
fetch !
select v1t33 + v2t21 from v3 !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !

file connect ( kern test !
create table t1 ( t1xc1 fixed(2) key, t1xc2 fixed(2) key,
                  t1xc3 fixed(2) ) !
create table t2 ( t2xc1 fixed(2) key, t2xc2 fixed(2), t2xc3 fixed(2) ) !
insert t1 values ( 2, 3, 5 ) !
insert t1 values ( 4, 1, 3 ) !
insert t1 values ( 8, 3, 1 ) !
insert t2 values ( 4, 2, 6 ) !
insert t2 values ( 8, 1, 5 ) !
insert t2 values ( 2, 5, 5 ) !
commit work !
create view v1 as select 3 * t1xc2 neu1, t2xc3, t2xc1 + t2xc2
   from t1,t2 !
select * from v1 !
fetch !
declare erg1 cursor for 
select neu1 neu2 , expression1 + 1 
   from v1 where neu1 < t2xc3 !
fetch erg1 !
create view v2 as select  t1xc3 + t2.t2xc1 neu1 from t1,t2
   where t1xc1 < t2xc3 !
select * from v2 !
fetch !
select neu1 from v2 !
fetch !
create view v3 ( s1, s2, s3, s4, s5 )
   as select t1xc2 + t2xc3, t1xc2, t2.t2xc3, t1xc1 spalte, t2xc1
   from t1,t2 where t1xc1 = t2xc1 !
select * from v3 !
fetch !
select s1, s2, s3, v3.s4, kern.v3.s5 from v3 !
fetch !
select s1 + s2, s4 + s5 neu from v3 !
fetch !
select s3, count(s3), sum ( s4 ) from v3 group by s3 order by 3 asc !
fetch !
select sum ( s2+s5), sum ( s1 +s5 ) from v3
   group by s3 having avg ( s4) = 5 !
fetch !
create view v4 ( c1, c2 ) as select sum ( t1xc2 ), count ( t2xc1 )
   from t1,t2 !
create view v5 ( c1,c2,c3,c4,c5,c6 ) as select * from t1,t2
   where t1xc2 + 2 = t2xc3 !
select * from v5 !
fetch !
select distinct c4, c5 , 7 from v5 !
fetch !
drop table t1 !
drop table t2 !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N61VIEWG.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( t1xc1 fixed(3) key, t1xc2 fixed(3) ) !
create table t2 ( t2xc1 fixed(3), t2xc2 fixed(3) ) !
create view v1 as select * from t1,t2 where t1xc1 = t2xc1 !
insert t1 values ( 3, 11 ) !
insert t1 values ( 5, 44 ) !
insert t1 values ( 7, 11 ) !
insert t1 values ( 1, 11 ) !
insert t2 values ( 5, 66 ) !
insert t2 values ( 5, 66 ) !
insert t2 values ( 1, 11 ) !
insert t2 values ( 5, 11 ) !
insert t2 values ( 7, 22 ) !
insert t2 values ( 7, 11 ) !
select * from v1 !
fetch !
create view v3 as
select count(*) * 3, ((max(t1xc1 + 6) + sum(t2xc1)) * t1xc2) mod 4 col
   from v1 where t2xc2 + t1xc1 > t1xc2
   group by t1xc2, t2xc1
   having count(*) + 30 < t1xc2 + sum(t2xc2) !
select * from v3 !
fetch !
rollback work release !

file connect ( kern test !
create table t1 ( t1xc1 char(20),
                  t1xc2 fixed(10),
                  t1xc3 char(10) ascii,
                  t1xc4 fixed(10,5),
                  t1xc5 float(10),
                  t1xc6 fixed(18) ) !
insert t1 values ( 'Hans', 100, 'Muenchen', 100.100, null,
                    500000000000000000 ) !
insert t1 values ( null, null, 'Hamburg', 0.00005, 8E-30, -2000 ) !
insert t1 values ( 'Karin', -600000, null, null, -1E12,
                    800000000000000000 ) !
insert t1 values ( 'Karin', 5, 'Bonn', -700, 1000, null ) !
insert t1 values ( 'Hans', -500000, 'Bremen', 700.00005, null, 300 ) !
insert t1 values ( ' Marianne', -80, 'Koeln 1', -300, 10e-10, null ) !
create table t2 ( t2xc1 fixed(5) key,
                  t2xc2 char(10) ascii,
                  t2xc3 char(15) ascii,
                  t2xc4 float(5),
                  t2xc5 fixed(5) range between -50000 and +50000 ) !
insert t2 values ( 10, 'Klaus', 'Mueller', 1E20, 20000 ) !
insert t2 values ( 20, 'Elena', 'Meier', -5E10, -100 ) !
insert t2 values ( 30, null, 'Lehmann', null, null ) !
insert t2 values ( 40, 'Paul', 'Anse Rauch', 8 , -50000 ) !
create table t3 ( t3xc1 fixed(2,1 ) key,
                  t3xc2 char(20) key ,
                  t3xc3 float( 5) ,
                  t3xc4 char(20) ascii range between 'A' and 'G' ,
                  t3xc5 fixed(18) ) !
insert t3 values ( 0.1, 'Frankreich', 80E50, 'Amsel',
                   700000000000000000 ) !
insert t3 values ( 2.5, 'Belgien', 1E-30, 'Fink', null ) !
insert t3 values ( 3.0, 'Italien', -5E30, 'Amsel', -1000 ) !
insert t3 values ( 4.1, 'VR Polen', null, 'Fink', +200 ) !
create view v1 as
   select sum ( t1.t1xc2 ) as col1 , sum ( t2.t2xc1 ) as col2 from t1, t2 !
select 'BLABLA', col1, 'BLABLA', col2 from v1 !
fetch !
create view v2 as
select count (*), count ( distinct kern.t2.t2xc2 ),
       count ( all ( t3xc5)) from t1,t2,t3 !
select * from v2 !
fetch !
create view v3 as
select avg ( distinct t2.t2xc1 ) ,
       count ( distinct t2.t2xc1 ) ,
       avg ( all kern.t1.t1xc5 ) ,
       sum ( all t1xc5 ) ,
       count ( all ( t1xc5 ) ) from t1,t2 !
select * from v3 !
fetch !
create view v4 as
select sum ( ref.t1xc6 ) , AVG ( ref.t1xc4) from t1,t1 ref !
select * from v4 !
fetch !
create view v5 as
select sum ( t1xc4 ), avg ( t1xc4), t1xc1, count(t1xc1)
    from t1,t2 group by t1xc1 !
select expression1, expression2, t1xc1 || ' aa', expression3 + 1
   from v5 !
fetch !
create view v6 as
select sum ( t1xc4 ), sum ( t2xc4)
    from t1, t2 ref
    group by ref.t2xc1
    having ref.t2xc1 > 10
       and sum ( ref.t2xc4) is not null !
declare erg1 cursor for 
select expression1, max ( expression2 ) exp1 from v6
   where expression1 <> v6.expression2
   group by expression1
   having count(*) = 2 !
fetch erg1 !
create view v7 as
select avg ( t1xc2), avg ( distinct t1xc5 ) , t1.t1xc5
    from t1, t2
    group by t1xc1 , kern.t1.t1xc5
    having sum ( t1xc5 ) > 0 !
select * from v7 !
fetch !
create view v8 as
select sum( t1xc2) , count ( t1xc2) , avg (t1xc2)
    from t1,t3 group by t1xc1, t3.t3xc4 !
select * from v8 where expression2 < 3 order by 1 !
fetch !
create view v9 as
select sum ( t1.t1xc2 + t2.t2xc1 ),
       avg ( t1xc4 * 5 * t2xc5 ),
       ( sum ( distinct kern.t1.t1xc4 / t2xc5 ) )
    from t1, t2 group by t2xc1 !
select * from v9 !
fetch !
create table i (i1 float (14), i2 float (14), i3 fixed(18))!
create view v10 as
select sum ( all t1.t1xc5 - t2xc4*t3xc1),
       avg ( all  (t1.t1xc5  )- t2xc4 * t3xc1 ) ,
       count (all ( t2xc4 * t3xc1 *((-1))+ t1.t1xc5 )) col3
    from t1,t2, t3
    group by t3xc4 !
select * from v10 !
fetch !
insert i select * from v10 !
select * from i !
fetch !
declare erg3 cursor for 
select min (v9.expression1) n1, v10.expression2 n2 
   from v9, v10
   where v9.expression2 is not null or v9.expression3 < 0
   group by v10.expression1, v10.expression2
   having sum (v9.expression2) - sum (v10.expression2) > 0
   order by 2 desc !
fetch erg3 !
declare erg4 cursor for 
select n1 from erg3 !
fetch erg4 !
select * from v10 !
fetch!
select v10.expression1, t1.t1xc1, t1.t1xc2 from v10, t1 !
fetch !
pars_then_ex !
select v10.expression1, t1.t1xc1, t1.t1xc2 from v10, t1 !
fetch into :a, :b, :c!
pars_execute !
rollback work release !

file connect ( kern test !
create table t1 ( t1xc1 fixed(5) key,
                  t1xc2 char(5),
                  t1xc3 char(5) ) !
create table t2 ( t2xc1 fixed(5),
                  t2xc2 fixed(5),
                  t2xc3 char(5) ) !
insert t1 values ( 2, 'aaa' , 'sss' ) !
insert t1 values ( 3, 'nnn' , 'nnn' ) !
insert t1 values ( 4, 'qqq' , 'aaa' ) !
insert t1 values ( 9, 'aaa' , 'bbb' ) !
insert t1 values ( 6, 'ccc' , 'bbb' ) !
insert t1 values ( 8, 'aaa' , 'bbb' ) !
insert t1 values ( 5, null  , null  ) !
insert t1 values ( 1, 'aaa' , 'sss' ) !
insert t2 values ( 3, 2, 'aaa' ) !
insert t2 values ( 4, 1, 'nnn' ) !
insert t2 values ( 4, 2, 'nnn' ) !
insert t2 values ( 8, null, 'bbb' ) !
insert t2 values ( 1, 4, 'sss' ) !
insert t2 values ( null, 2, 'bbb' ) !
insert t2 values ( 5, 1, null ) !
insert t2 values ( 1, 8, 'aaa' ) !
insert t2 values ( 1, 4, 'bbb' ) !
insert t2 values ( 3, 2, 'nnn' ) !
insert t2 values ( 2, 1, 'sss' ) !
insert t2 values ( 3, 2, 'bbb' ) !
insert t2 values ( 2, 1, 'bbb' ) !
create view v1 as
  select distinct sum ( distinct 5 + t2xc1 ) + 2 * sum ( t1xc1 ),
                  count ( t2xc1 ),
                  max ( t1xc2 ),
                  t2xc2
  from t1, t2
  where t1xc3 = t2.t2xc3
    and t1xc1 > t2xc1 - 1
  group by t2xc2, t1xc2
  having min ( t2xc1 ) < max ( all t1xc1 )
     and avg ( distinct t1.t1xc1 ) + min ( t1xc1 ) <= kern.t2.t2xc2 !
select * from v1 !
fetch !
select v1.t2xc2, ref.t2xc2, v1.t2xc2 + ref.t2xc2 from v1, v1 ref
   where v1.t2xc2 = ref.t2xc2 !
fetch !
rollback work !
create table t ( a char (8))!
insert t set a = 'aaa'!
create table t1 (b char (8))!
create view v1 as select * from t,t1
   where t.a = t1.b(+) !
select * from v1 !
fetch!
select * into :a, :b from v1 where v1.a = 'aaa'!
pars_then_ex !
select * from v1 !
fetch!
select * into :a, :b from v1 where a = 'aaa'!
pars_execute!
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N63EXCEP.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( a1 fixed(5), b1 fixed(5),
                  c1 fixed(18), d1 fixed(18),
                  e1 fixed(7,2), f1 fixed(7,2),
                  g1 float(3), h1 float(3), i1 float(18), j1 float(18),
                  k1 char(5), l1 char(5),
                  m1 time, n1 time, o1 date, p1 date ) !
create table t2 ( a2 fixed(5), b2 fixed(5),
                  c2 fixed(18), d2 fixed(18),
                  e2 fixed(7,2), f2 fixed(7,2),
                  g2 float(3), h2 float(3), i2 float(18), j2 float(18),
                  k2 char(5), l2 char(5),
                  m2 time, n2 time, o2 date, p2 date ) !
insert t1 values ( 55555, 33333, 666666666666666666, 777777777777777777,
   22222.22, 99999.99, 8.88E+62, 7.77E+62, 8.88888888888888888E+62,
   9.99999999999999999E+62, 'aaaaa', 'bbbbb', '00214738', '23453612',
   '19841224', '20000712' ) !
insert t2 values ( 55555, 33333, 666666666666666666, 777777777777777777,
   22222.22, 99999.99, 8.88E+62, 7.77E+62, 8.88888888888888888E+62,
   9.99999999999999999E+62, 'aaaaa', 'bbbbb', '00214738', '23453612',
   '19841224', '20000712' ) !
insert t1 values ( null, null, null, null, null, null, null, null, null,
   null, null, null, null, null, null, null ) !
insert t2 values ( null, null, null, null, null, null, null, null, null,
   null, null, null, null, null, null, null ) !
insert t1 values ( 0,0,0,0,0,0,0,0,0,0, 'a' , 'a' ,
   '11111111', '11111111' , '11111111', '11111111' ) !
insert t2 values ( 0,0,0,0,0,0,0,0,0,0, 'a' , 'a' ,
   '11111111', '11111111' , '11111111', '11111111' ) !
commit work !
create view v1 (a1,i1,c1,c2) as select a1, i1, a1/b1, i1*j1 from t1 !
commit work !
create view v2 ( y1, y2 ) as select a1, i1+j1 from t1 !
select c1 * y2 from v1, v2 !
fetch !
select * from v1, v2 where y1 > 0 !
fetch !
select * from v1, v2 where a1 > 0 !
fetch !
create view v3 (v31, v32 ) as select c1, y2 from v1,v2 !
select * from v3 !
fetch !
create view v4 (v41 ) as select c2*y2 from v1, v2 !
select * from v4 !
fetch !
create view v5 ( v51) as select a1 from v1 where c1 > 0 !
select * from v5 !
fetch !
commit work !
create view v6 (v61, v62) as select i1*j1, i2*j2 from t1,t2 !
create view v7 ( v71 ) as select v61*v62 from v6 !
select * from v7 !
fetch !
select * from v7 where v71 > 0 !
fetch !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N64VALUE.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( s1 fixed(5), s2 fixed(5), s3 fixed(5) ) !
insert t1 values ( null, null, null ) !
insert t1 values ( null, null, 1    ) !
insert t1 values ( null, 2   , null ) !
insert t1 values ( null, null, 3    ) !
insert t1 values ( null, 4   , 4    ) !
insert t1 values ( 5   , null, 5    ) !
insert t1 values ( 6   , 6   , null ) !
commit work !
create table t2 ( c1 char(8) , c2 char(8) ,
                  c3 char(8) ascii, c4 char(8) ascii,
                  date1 date, date2 date, time1 time, time2 time ) !
insert t2 values ( null, null, null, null, null, null, null, null ) !
insert t2 values
   ( 'a', null, 'b', null, '19831212', null, '22223625', null ) !
insert t2 values
   ( null, 'c', null, 'd', null, '19870904', null, '33334213' ) !
commit work !
create table t4 ( spalte float(18) ) !
create table t5 ( col    float(18) ) !
insert t4 values ( 9E+62 ) !
insert t5 values ( 9E+62 ) !
select value(spalte+col, 12 ) from t4,t5 !
select * from t4,t5 where value( spalte+col,spalte ) > 0 !
select value( spalte, col) from t4,t5 !
fetch !
create view v1(neu) as select t4.spalte + t5.col from t4,t5 !
select * from v1 !
fetch !
select value ( neu, neu) from v1 !
fetch !
select value ( 'aaa',neu) from v1 !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N65FROM.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( c fixed(1) ) !
create view v1 (col1,col2) as select * from t, t ref !
create view v2
  ( a1,a2,a3,a4,a5,a6,a7,a8,a9,
  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,
  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,
  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,
  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,
  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,
  a60,a61,a62,a63,a64)
  as select 
         t001.c,t002.c,t003.c,t004.c,t005.c,t006.c,t007.c,t008.c,t009.c,
  t010.c,t011.c,t012.c,t013.c,t014.c,t015.c,t016.c,t017.c,t018.c,t019.c,
  t020.c,t021.c,t022.c,t023.c,t024.c,t025.c,t026.c,t027.c,t028.c,t029.c,
  t030.c,t031.c,t032.c,t033.c,t034.c,t035.c,t036.c,t037.c,t038.c,t039.c,
  t040.c,t041.c,t042.c,t043.c,t044.c,t045.c,t046.c,t047.c,t048.c,t049.c,
  t050.c,t051.c,t052.c,t053.c,t054.c,t055.c,t056.c,t057.c,t058.c,t059.c,
  t060.c,t061.c,t062.c,t063.c,t064.c
  from 
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181,t t182,t t183 !
create view v3
  ( a1,a2,a3,a4,a5,a6,a7,a8,a9,
  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,
  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,
  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,
  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,
  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,
  a60,a61,a62,a63,a64,a65)
  as select
         t001.c,t002.c,t003.c,t004.c,t005.c,t006.c,t007.c,t008.c,t009.c,
  t010.c,t011.c,t012.c,t013.c,t014.c,t015.c,t016.c,t017.c,t018.c,t019.c,
  t020.c,t021.c,t022.c,t023.c,t024.c,t025.c,t026.c,t027.c,t028.c,t029.c,
  t030.c,t031.c,t032.c,t033.c,t034.c,t035.c,t036.c,t037.c,t038.c,t039.c,
  t040.c,t041.c,t042.c,t043.c,t044.c,t045.c,t046.c,t047.c,t048.c,t049.c,
  t050.c,t051.c,t052.c,t053.c,t054.c,t055.c,t056.c,t057.c,t058.c,t059.c,
  t060.c,t061.c,t062.c,t063.c,t064.c,t065.c
  from
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181,t t182,t t183,t t184 !
create view v4
  ( a1,a2,a3,a4,a5,a6,a7,a8,a9,
  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,
  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,
  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,
  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,
  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,
  a60,a61,a62,a63,a64)
  as select
         t001.c,t002.c,t003.c,t004.c,t005.c,t006.c,t007.c,t008.c,t009.c,
  t010.c,t011.c,t012.c,t013.c,t014.c,t015.c,t016.c,t017.c,t018.c,t019.c,
  t020.c,t021.c,t022.c,t023.c,t024.c,t025.c,t026.c,t027.c,t028.c,t029.c,
  t030.c,t031.c,t032.c,t033.c,t034.c,t035.c,t036.c,t037.c,t038.c,t039.c,
  t040.c,t041.c,t042.c,t043.c,t044.c,t045.c,t046.c,t047.c,t048.c,t049.c,
  t050.c,t051.c,t052.c,t053.c,t054.c,t055.c,t056.c,t057.c,t058.c,t059.c,
  t060.c,t061.c,t062.c,t063.c,t064.c
  from
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181, v1 !
create view v5
  ( a1,a2,a3,a4,a5,a6,a7,a8,a9,
  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,
  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,
  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,
  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,
  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,
  a60,a61,a62)
  as select 
         t001.c,t002.c,t003.c,t004.c,t005.c,t006.c,t007.c,t008.c,t009.c,
  t010.c,t011.c,t012.c,t013.c,t014.c,t015.c,t016.c,t017.c,t018.c,t019.c,
  t020.c,t021.c,t022.c,t023.c,t024.c,t025.c,t026.c,t027.c,t028.c,t029.c,
  t030.c,t031.c,t032.c,t033.c,t034.c,t035.c,t036.c,t037.c,t038.c,t039.c,
  t040.c,t041.c,t042.c,t043.c,t044.c,t045.c,t046.c,t047.c,t048.c,t049.c,
  t050.c,t051.c,t052.c,t053.c,t054.c,t055.c,t056.c,t057.c,t058.c,t059.c,
  t060.c,t061.c,t062.c
  from
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181 !
create view v6
  ( a1,a2,a3,a4,a5,a6,a7,a8,a9,
  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,
  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,
  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,
  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,
  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,
  a60,a61,a62,a63,a64)
  as select * from v5, v1 !
create view v7
  ( a1,a2,a3,a4,a5,a6,a7,a8,a9,
  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,
  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,
  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,
  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,
  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,
  a60,a61,a62,a63)
  as select * from v5, t !
*create view v8
*  ( a1,a2,a3,a4,a5,a6,a7,a8,a9,
*  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,
*  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,
*  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,
*  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,
*  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,
*  a60,a61,a62,a63)
*  as select * from v5, v1, t !
insert t values ( 1 ) !
select
         t001.c,t002.c,t003.c,t004.c,t005.c,t006.c,t007.c,t008.c,t009.c,
  t010.c,t011.c,t012.c,t013.c,t014.c,t015.c,t016.c,t017.c,t018.c,t019.c,
  t020.c,t021.c,t022.c,t023.c,t024.c,t025.c,t026.c,t027.c,t028.c,t029.c,
  t030.c,t031.c,t032.c,t033.c,t034.c,t035.c,t036.c,t037.c,t038.c,t039.c,
  t040.c,t041.c,t042.c,t043.c,t044.c,t045.c,t046.c,t047.c,t048.c,t049.c,
  t050.c,t051.c,t052.c,t053.c,t054.c,t055.c,t056.c,t057.c,t058.c,t059.c,
  t060.c,t061.c,t062.c,t063.c,t064.c
  from
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181,t t182,t t183,t t184,t t185,t t186,t t187,t t188,t t189,
  t t190,t t191,t t192,t t193,t t194,t t195,t t196,t t197,t t198,t t199,
  t t200,t t201,t t202,t t203,t t204,t t205,t t206,t t207,t t208,t t209,
  t t210,t t211,t t212,t t213,t t214,t t215,t t216,t t217,t t218,t t219,
  t t220,t t221,t t222,t t223,t t224,t t225,t t226,t t227,t t228,t t229,
  t t230,t t231,t t232,t t233,t t234,t t235,t t236,t t237,t t238,t t239,
  t t240,t t241,t t242,t t243,t t244,t t245,t t246,t t247,t t248,t t249,
  t t250,t t251,t t252,t t253,t t254 !
select * from 
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181,t t182,t t183,t t184,t t185,t t186,t t187,t t188,t t189,
  t t190,t t191,t t192,t t193,t t194,t t195,t t196,t t197,t t198,t t199,
  t t200,t t201,t t202,t t203,t t204,t t205,t t206,t t207,t t208,t t209,
  t t210,t t211,t t212,t t213,t t214,t t215,t t216,t t217,t t218,t t219,
  t t220,t t221,t t222,t t223,t t224,t t225,t t226,t t227,t t228,t t229,
  t t230,t t231,t t232,t t233,t t234,t t235,t t236,t t237,t t238,t t239,
  t t240,t t241,t t242,t t243,t t244,t t245,t t246,t t247,t t248,t t249,
  t t250,t t251,t t252,t t253,t t254,t t255 !
select * from 
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181,t t182,t t183,t t184,t t185,t t186,t t187,t t188,t t189,
  t t190,t t191,t t192,t t193,t t194,t t195,t t196,t t197,t t198,t t199,
  t t200,t t201,t t202,t t203,t t204,t t205,t t206,t t207,t t208,t t209,
  t t210,t t211,t t212,t t213,t t214,t t215,t t216,t t217,t t218,t t219,
  t t220,t t221,t t222,t t223,t t224,t t225,t t226,t t227,t t228,t t229,
  t t230,t t231,t t232,t t233,t t234,t t235,t t236,t t237,t t238,t t239,
  t t240,t t241,t t242,t t243,t t244,t t245,t t246,t t247,t t248,t t249,
  t t250,t t251,t t252,t t253, v1 !
select 
         t001.c,t002.c,t003.c,t004.c,t005.c,t006.c,t007.c,t008.c,t009.c,
  t010.c,t011.c,t012.c,t013.c,t014.c,t015.c,t016.c,t017.c,t018.c,t019.c,
  t020.c,t021.c,t022.c,t023.c,t024.c,t025.c,t026.c,t027.c,t028.c,t029.c,
  t030.c,t031.c,t032.c,t033.c,t034.c,t035.c,t036.c,t037.c,t038.c,t039.c,
  t040.c,t041.c,t042.c,t043.c,t044.c,t045.c,t046.c,t047.c,t048.c,t049.c,
  t050.c,t051.c,t052.c,t053.c,t054.c,t055.c,t056.c,t057.c,t058.c,t059.c,
  t060.c,t061.c,t062.c,t063.c,t064.c
  from
  t t001,t t002,t t003,t t004,t t005,t t006,t t007,t t008,t t009,
  t t010,t t011,t t012,t t013,t t014,t t015,t t016,t t017,t t018,t t019,
  t t020,t t021,t t022,t t023,t t024,t t025,t t026,t t027,t t028,t t029,
  t t030,t t031,t t032,t t033,t t034,t t035,t t036,t t037,t t038,t t039,
  t t040,t t041,t t042,t t043,t t044,t t045,t t046,t t047,t t048,t t049,
  t t050,t t051,t t052,t t053,t t054,t t055,t t056,t t057,t t058,t t059,
  t t060,t t061,t t062,t t063,t t064,t t065,t t066,t t067,t t068,t t069,
  t t070,t t071,t t072,t t073,t t074,t t075,t t076,t t077,t t078,t t079,
  t t080,t t081,t t082,t t083,t t084,t t085,t t086,t t087,t t088,t t089,
  t t090,t t091,t t092,t t093,t t094,t t095,t t096,t t097,t t098,t t099,
  t t100,t t101,t t102,t t103,t t104,t t105,t t106,t t107,t t108,t t109,
  t t110,t t111,t t112,t t113,t t114,t t115,t t116,t t117,t t118,t t119,
  t t120,t t121,t t122,t t123,t t124,t t125,t t126,t t127,t t128,t t129,
  t t130,t t131,t t132,t t133,t t134,t t135,t t136,t t137,t t138,t t139,
  t t140,t t141,t t142,t t143,t t144,t t145,t t146,t t147,t t148,t t149,
  t t150,t t151,t t152,t t153,t t154,t t155,t t156,t t157,t t158,t t159,
  t t160,t t161,t t162,t t163,t t164,t t165,t t166,t t167,t t168,t t169,
  t t170,t t171,t t172,t t173,t t174,t t175,t t176,t t177,t t178,t t179,
  t t180,t t181,t t182,t t183,t t184,t t185,t t186,t t187,t t188,t t189,
  t t190,t t191,t t192,t t193,t t194,t t195,t t196,t t197,t t198,t t199,
  t t200,t t201,t t202,t t203,t t204,t t205,t t206,t t207,t t208,t t209,
  t t210,t t211,t t212,t t213,t t214,t t215,t t216,t t217,t t218,t t219,
  t t220,t t221,t t222,t t223,t t224,t t225,t t226,t t227,t t228,t t229,
  t t230,t t231,t t232,t t233,t t234,t t235,t t236,t t237,t t238,t t239,
  t t240,t t241,t t242,t t243,t t244,t t245,t t246,t t247,t t248,t t249,
  t t250,t t251,t t252, v1 !
select * from v2 !
select * from v2, t !
select * from v4 !
select * from v4, t !
select * from v5 !
select * from v5, v1, t !
select * from v6 !
select * from v6, v1 !
select * from v6, t !
select * from v7, t !
select * from v7, t, t t1 !
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N72SEL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test!
create table t0 ( a fixed(5) )!
create table t1 ( a fixed(5) )!
insert t1 set a = 1 !
insert t1 set a = 2 !
insert t1 set a = 3 !
insert t1 set a = 4 !
insert t1 set a = 5 !
insert t1 set a = 2 !
insert t1 set a = 3 !
insert t1 set a = 4 !
create table t2 ( b fixed(5) )!
insert t2 set b = 3 !
insert t2 set b = 4 !
insert t2 set b = 5 !
insert t2 set b = 6 !
insert t2 set b = 7 !
insert t2 set b = 4 !
insert t2 set b = 5 !
insert t2 set b = 6 !
commit work !
select * from t1, t2 where a (+)= b
union
select * from t1, t2 where a =b (+)!
FETCH !
select * from t1, t2 where a (+)=b (+)!
FETCH !
select distinct * from t1, t2 where not a =b (+)!
FETCH !
SELECT A, B FROM T1, T2 WHERE NOT A = B
UNION
SELECT A, NULL  FROM T1
    WHERE NOT EXISTS
         (SELECT B FROM T2 WHERE NOT T1.A = B) !
FETCH !
select * from t1, t2 where not a = b (+) order by a, b!
FETCH !
SELECT A, B FROM T1, T2 WHERE NOT A = B
UNION all
SELECT A, NULL  FROM T1
    WHERE NOT EXISTS
         (SELECT B FROM T2 WHERE NOT T1.A = B)
order by 1, 2 !
FETCH !
drop table t0 !
drop table t1 !
drop table t2 !
commit !
create table t ( a char (8) key)!
insert t set a = 'aaa'!
create table t1 (a1 char (8))!
insert t1 set a1 = 'aa1'!
create table t2 (a2 char (8))!
insert t2 set a2 = 'aa2'!
select * from t where a in (select a from t )
  union
  select a1 from t1,t2 !
fetch!
create view v1 as
  select * from t where a in (select a from t )
  union
  select a1 from t1,t2 !
select * from v1 !
fetch !
pars_then_ex !
select * from v1 !
fetch!
pars_execute !
drop table t !
drop table t1 !
drop table t2 !
commit release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N99FEHL1.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table tab1 ( spalte11 char(10) key ,
                    spalte12 fixed(5,2),
                    spalte13 float(5) ) !
insert tab1 values ( 'HANS'    , 22.52 , null ) !
insert tab1 values ( 'PAUL'    ,   0   ,  3E2 ) !
insert tab1 values ( 'JOACHIM' , 17.1  ,   0  ) !
create table tab2 ( spalte21 fixed(4),
                    spalte22 char(32) ,
                    spalte23 float(4) ) !
insert tab2 values ( 300   , 'HANS'   , null ) !
insert tab2 values ( 22.00 , 'FRIEDA' , 0    ) !
insert tab2 values ( 12    , 'JOACHIM', 0    ) !
create table tab3 ( spalte31 fixed(4) ,
                    spalte32 char(32) ,
                    spalte33 float(4) ) !
insert tab3 values ( 1000, 'HANS'    , null ) !
insert tab3 values ( 2000, 'VOLKER' , 100  ) !
insert tab3 values ( 3000, 'ANDREAS' , 200  ) !
insert tab3 values ( 4000, 'SIEGFRIED' , 300  ) !
commit!
(SELECT * FROM tab3, (SELECT * FROM TAB2, ((SELECT * FROM TAB3))),
 ((SELECT * FROM TAB3))) !
declare erg cursor for
(SELECT * FROM tab3, (SELECT * FROM TAB2, ((SELECT * FROM TAB3))),
 ((SELECT * FROM TAB3))) !
close erg !

declare erg cursor for
SELECT tab2.spalte21, tab2.spalte22, tab2.spalte23 FROM TAB2,
(SELECT * FROM tab2) !
close erg !
declare erg cursor for
SELECT * FROM tab2,
(SELECT spalte21, spalte22, spalte23 FROM TAB2) !
close erg !
declare erg cursor for
SELECT * FROM tab2,
(SELECT * FROM TAB2) !
close erg !
declare erg cursor for
SELECT tab2.spalte21, tab2.spalte22, tab2.spalte23 FROM TAB2,
(SELECT spalte21, spalte22, spalte23 FROM TAB2) !
close erg !
drop table tab1 !
drop table tab2 !
drop table tab3 !
drop table tab4 !
commit work release !
file cleark !
file connect ( kern test !
CREATE TABLE T1 ( A CHAR (18))!
INSERT T1 SET A = 'aaa'!
CREATE TABLE T2 ( B CHAR (5)) !
INSERT T2 SET B = 'aaa'!
CREATE INDEX "T2.B"
	on T2( B DESC ) !
EXPLAIN SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
fetch !
drop index "T2.B" !
create index "t2.b" on t2( b ) !
SELECT * FROM T1, T2 WHERE T1.A = T2.B !
fetch !
rollback !
CREATE TABLE T1 ( A CHAR (38))!
INSERT T1 SET A = 'aaa'!
CREATE TABLE T2 ( B CHAR (25)) !
INSERT T2 SET B = 'aaa'!
CREATE INDEX "T2.B" on T2( B DESC ) !
EXPLAIN SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
SELECT * FROM <T1, T2> WHERE T1.A = T2.B !
fetch !
drop index "T2.B" !
create index "t2.b" on t2( b ) !
SELECT * FROM T1, T2 WHERE T1.A = T2.B !
fetch !
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* N99FEHL2.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t (s1 fixed (5), s2 fixed(5))!
insert t values (1,1)!
insert t values (2,2)!
create view v1 as select distinct s1, s2 from t!
create table t1 ( a fixed (6))!
insert t1 set a = 3 !
select * from v1, t1 !
fetch !
select distinct * from v1, t1 !
fetch !
create view v2 as select distinct * from v1, t1 !
select * from v2, t1 !
fetch !
select distinct * from v2, t1 !
fetch !
drop table t !
drop table t1 !
commit release !

file connect ( kern test !
* Fehlerprotokoll 89 !
create table t ( a char (30), b fixed(18) )!
insert t set a = 'abcdefghijklmnopqrstuvwxyzabcd',
       b = 333333333333333333!
create table t1 ( b1 fixed(18))!
insert t1 set b1 = 333333333333333333 !
parsing !
select * from t, t1 where b = b1 !
pars_execute !
select * from t !
fetch1 !
executing !
select * !
1
pars_execute !
rollback release !

file connect ( kern test !
* Fehlerprotokoll 95, gefunden von Guenter in Rel.2.4.06 !
create table t ( a char (8), b char (8), c char (8))!
create index "t.c"
	on t( c ) !
create index i1 on t (a, b, c) !
insert t values ('aaa', 'b', '123')!
insert t values ('aaa', 'b', '234')!
insert t values ('aaa', 'b', '345')!
create table t1 ( b1 char (8))!
insert t1 set b1 = 'b' !
create index "t1.b1" on t1( b1 ) !
explain select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c between '123' and '234') !
select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c between '123' and '234') !
fetch!
explain select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c = '123' or c = '234') !
select distinct a, c, b1
  from t, t1
  where a = 'aaa' and
        b = b1    and
        (c = '123' or c = '234') !
fetch!
drop table t !
drop table t1 !
commit release !

file connect ( kern test !
create table t ( a char ( 8),
b char (8),
c char (8),
d char (8),
e char (254),
f char (254),
g char (254),
h char (254),
i char (254),
j char (127),
k char (127),
l char (127),
m char (127))!
insert t values ('aa', 'bb', 'cc', 'dd',
'eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee',
'fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff',
'ggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg',
'hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh hhhh',
'iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii',
'jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj',
'kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk',
'lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll',
'mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm')!
create unique index i1 on t (a, b, c, d) !
create table t1 (a1 char (8), b1 char (8))!
create unique index i1 on t1 (a1, b1) !
insert t1 set a1 = 'aaa', b1 = 'dd'!
pars_then_ex !
data !
select * from t, t1 where a = :a and b = :b and c = :c and d > :d and
a1 = :a1 and d = b1 !
c 8 'aa' c 8 'bb' c 8 'cc' c 8 'a' c 8 'aaa'

nodata !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i, :j, :k, :l, :m, :a1, :b1 !
pars_execute !
drop table t !
drop table t1 !
commit release !

file connect ( kern test !
create table t1 (a1 char(5) key, a2 char(5) key) !
insert t1 values ('aaaaa','bbbbb') !
insert t1 values ('bbbbb','xxxxx') !
insert t1 values ('ccccc','zzzzz') !
declare erg cursor for select * from t1 for reuse !
explain select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a2 = 'xxxxx' and r3.a2 = 'xxxxx' and
r4.a2 = 'xxxxx' and r5.a2 = 'xxxxx' and r6.a2 = 'xxxxx' and
r7.a2 = 'xxxxx' and erg.a2 = 'xxxxx' !
select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a2 = 'xxxxx' and r3.a2 = 'xxxxx' and
r4.a2 = 'xxxxx' and r5.a2 = 'xxxxx' and r6.a2 = 'xxxxx' and
r7.a2 = 'xxxxx' and erg.a2 = 'xxxxx' !
fetch !
explain select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a1 = 'xxxxx' and r3.a1 = 'xxxxx' and
r4.a1 = 'xxxxx' and r5.a1 = 'xxxxx' and r6.a1 = 'xxxxx' and
r7.a1 = 'xxxxx' !
select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a1 = 'xxxxx' and r3.a1 = 'xxxxx' and
r4.a1 = 'xxxxx' and r5.a1 = 'xxxxx' and r6.a1 = 'xxxxx' and
r7.a1 = 'xxxxx' !
fetch !
rollback work !
create table t1 (a1 char(5) key, a2 char(5) key) !
insert t1 values ('aaaaa','bbbbb') !
insert t1 values ('bbbbb','xxxxx') !
insert t1 values ('ccccc','zzzzz') !
declare erg cursor for select * from t1 for reuse !
select * from erg, t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1 !
fetch !
rollback work !
create table t1 (a1 char(5) key, a2 char(5) key) !
insert t1 values ('aaaaa','bbbbb') !
insert t1 values ('bbbbb','xxxxx') !
insert t1 values ('ccccc','zzzzz') !
declare erg cursor for select * from t1 for reuse !
explain select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a2 = erg.a2 and r2.a2 = erg.a2 and r3.a2 = erg.a2 and
r4.a2 = erg.a2 and r5.a2 = erg.a2 and r6.a2 = erg.a2 and r7.a2 = erg.a2
and  r1.a1 = 'bbbbb' and r2.a1 = 'bbbbb' and r3.a1 = 'bbbbb' and
r4.a1 = 'bbbbb' and r5.a1 = 'bbbbb' and r6.a1 = 'bbbbb' and
r7.a1 = 'bbbbb' !
select * from erg, t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a2 = erg.a2 and r2.a2 = erg.a2 and r3.a2 = erg.a2 and
r4.a2 = erg.a2 and r5.a2 = erg.a2 and r6.a2 = erg.a2 and r7.a2 = erg.a2
and  r1.a1 = 'bbbbb' and r2.a1 = 'bbbbb' and r3.a1 = 'bbbbb' and
r4.a1 = 'bbbbb' and r5.a1 = 'bbbbb' and r6.a1 = 'bbbbb' and
r7.a1 = 'bbbbb' !
fetch !
rollback work release !
file connect ( sut sut !
drop user d !
drop user p !
drop user r !
commit release !
file cleark !

file connect ( kern test !
create table t1 (a1 char(5), a2 char(5)) !
insert t1 values ('2aaaa','2bbbb') !
insert t1 values ('1aaaa','1bbbb') !
insert t1 values ('4aaaa','4bbbb') !
insert t1 values ('3aaaa','3bbbb') !
insert t1 values ('5aaaa','5bbbb') !
insert t1 values ('7aaaa','7bbbb') !
insert t1 values ('6aaaa','6bbbb') !
insert t1 values ('9aaaa','9bbbb') !
insert t1 values ('8aaaa','8bbbb') !
create table t2 (b1 char(5), b2 char(5)) !
insert t2 values ('aaaaa','bbbbb') !
insert t2 values ('aaaaa','xxxxx') !
create view tv1 (a1, "a2") as select a1, a2 from t1 !
create view tv2 (b1, "b2") as select b1, b2 from t2 !
declare erg cursor for 
select a1, b1, "b2" from <tv1, tv2> order by "a2" !
FETCH erg!
declare erg cursor for 
select a1, b1, "b2" from <tv2, tv1> order by "a2" !
FETCH erg!
declare erg cursor for 
select a1, b1, "b2" from <tv1, tv2> order by "a2" desc !
FETCH erg!
declare erg cursor for 
select a1, b1, "b2" from <tv2, tv1> order by "a2" desc !
FETCH erg!
declare erg cursor for 
select a1, "a2", b1, "b2" from <tv1, tv2> order by "a2" !
FETCH erg!
declare erg cursor for 
select a1, "a2", b1, "b2" from <tv2, tv1> order by "a2" !
FETCH erg!
declare erg cursor for 
select a1, "a2", b1, "b2" from <tv1, tv2> order by "a2" desc !
FETCH erg!
declare erg cursor for 
select a1, "a2", b1, "b2" from <tv2, tv1> order by "a2" desc !
FETCH erg!
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* NEW_OPTIMIZER.vdnts
senderid DBM !
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table a(a1 int, a2 int, a3 int , primary key(a1, a2))!
create table b(b1 int, b2 int, b3 int , primary key(b1, b2))!
create table c(c1 int, c2 int, c3 int , primary key(c1, c2))!
create table d(d1 int, d2 int, d3 int , primary key(d1, d2))!
create table e(e1 int, e2 int, e3 int , primary key(e1, e2))!
create index ai1 on a(a1)!
create index ai2 on a(a1,a3)!
create index bi1 on b(b1)!
create index bi2 on b(b1,b3)!
create index ci1 on c(c1)!
create index ci2 on c(c1,c3)!
create index di1 on d(d1)!
create index di2 on d(d1,d3)!
create index ei1 on e(e1)!
create index ei2 on e(e1,e3)!

explain select * 
from a, b, c, d where
a.a1=b.b1 and b.b1=c.c1 and c.c1=d.d1 !
!* keyaccess for a, b, c; d automatic
explain select /*+join(keyaccess;keyaccess;keyaccess)*/ * 
from a, b, c, d where
a.a1=b.b1 and b.b1=c.c1 and c.c1=d.d1 !
!* keyaccess for a, b, c; d automatic
explain select /*+join(keyaccess;keyaccess;;keyaccess)*/ * 
from a, b, d, c where
a.a1=b.b1 and b.b1=c.c1 and c.c1=d.d1 !
!* keyaccess for a, c; indexaccess for b; d automatic
explain select /*+join(keyaccess;indexaccess;;keyaccess)*/ * 
from a, b, d, c where
a.a1=b.b1 and b.b1=c.c1 and c.c1=d.d1 !
!* keyaccess for a, c; indexaccess for b; dedicated indexaccess for d
explain 
select /*+ordered,join(keyaccess;indexaccess;indexaccess(di2);keyaccess)*/ * 
from a, b, d, c where
a.a1=b.b1 and b.b1=c.c1 and c.c1=d.d1 !
!* no access path for b; a,b,d automatic
explain 
select /*+ordered,join(;noaccesspath)*/ * 
from a, b, d, c where
a.a1=b.b1 and b.b1=c.c1 and c.c1=d.d1 !
!* no access path for b; a,b,d automatic; special access for a
explain 
select /*+ordered,join(access=indexaccess,indexscan;noaccesspath)*/ * 
from a, b, d, c where
a.a1=b.b1 and b.b1=c.c1 and c.c1=d.d1 !
rollback work !
* ==========================================================

!* test one phase handling and outer join
create table A (c1 char(3))!
insert into A values ('aaa')!
insert into A values ('bbb')!

create table B (c1 char(3))!
insert into B values ('bbb')!

create table C (c1 char(3) key)!
insert into C values ('aaa')!
insert into C values ('bbb')!

create table D (c1 char(3) key)!
insert into D values ('bbb')!

commit release !
senderid DBM !
file connect ( kern test !
SET PARAMETER OPTIMIZE_JOIN_ONEPHASE = 'NO'!
commit release !
senderid XCI !
file connect ( kern test !
explain 
select upper(A.c1), upper(B.c1) from <A, B> where A.c1 = B.c1(+)!
select upper(A.c1), upper(B.c1) from <A, B> where A.c1 = B.c1(+)!
fetch!
explain 
select upper(C.c1), upper(D.c1) from <C, D> where C.c1 = D.c1(+)!
select upper(C.c1), upper(D.c1) from <C, D> where C.c1 = D.c1(+)!
fetch!

explain 
select sum(1) from <A, B> where A.c1 = B.c1(+)!
select sum(1) from <A, B> where A.c1 = B.c1(+)!
fetch!
explain 
select sum(1) from <C, D> where C.c1 = D.c1(+)!
select sum(1) from <C, D> where C.c1 = D.c1(+)!
fetch!

* test if SET-function leads to NULL value for empty result sets 
explain
select min(A.c1) from <A, B> where A.c1 = B.c1 and A.c1 = 'ddd'!
select min(A.c1) from <A, B> where A.c1 = B.c1 and A.c1 = 'ddd'!
fetch!

explain
select min(C.c1) from <C, D> where C.c1 = D.c1 and C.c1 = 'ddd'!
select min(C.c1) from <C, D> where C.c1 = D.c1 and C.c1 = 'ddd'!
fetch!
commit release !
senderid DBM !
file connect ( kern test !
SET PARAMETER OPTIMIZE_JOIN_ONEPHASE = 'YES'!
commit release !
senderid XCI !
file connect ( kern test !
explain 
select upper(A.c1), upper(B.c1) from <A, B> where A.c1 = B.c1(+)!
select upper(A.c1), upper(B.c1) from <A, B> where A.c1 = B.c1(+)!
fetch!
explain 
select upper(C.c1), upper(D.c1) from <C, D> where C.c1 = D.c1(+)!
select upper(C.c1), upper(D.c1) from <C, D> where C.c1 = D.c1(+)!
fetch!

explain 
select sum(1) from <A, B> where A.c1 = B.c1(+)!
select sum(1) from <A, B> where A.c1 = B.c1(+)!
fetch!
explain 
select sum(1) from <C, D> where C.c1 = D.c1(+)!
select sum(1) from <C, D> where C.c1 = D.c1(+)!
fetch!

* test if SET-function leads to NULL value for empty result sets 
explain
select min(A.c1) from <A, B> where A.c1 = B.c1 and A.c1 = 'ddd'!
select min(A.c1) from <A, B> where A.c1 = B.c1 and A.c1 = 'ddd'!
fetch!

explain
select min(C.c1) from <C, D> where C.c1 = D.c1 and C.c1 = 'ddd'!
select min(C.c1) from <C, D> where C.c1 = D.c1 and C.c1 = 'ddd'!
fetch!
rollback work!
* ==========================================================
commit work release!
senderid XCI !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
