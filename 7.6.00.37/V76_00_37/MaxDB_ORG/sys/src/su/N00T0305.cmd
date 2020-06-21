*   *ID* N00T0305 DBCMD    changed on 1992-09-28-13.24.45 by ADMIN     *
 *** 3.12.5  <join predicate> !
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
select * from tab1 !
fetch !
select * from tab2 !
fetch !
select * from tab3 !
fetch !
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
