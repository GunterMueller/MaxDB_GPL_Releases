*   *ID* N25SEL2  DBCMD    changed on 1992-09-28-13.25.46 by ADMIN     *
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
select ref1.spalte11 neu11,kern.tab2.* into :a1,:a2,:a3,:a4
   from tab1  ref1, kern.tab2
   where ref1.spalte13 = kern.tab2.spalte21 !
select kern.tab1.*, ref2.*
   into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab2 ref2
   where tab1.spalte13 = ref2.spalte21 !
select all ref1.spalte11 neu11, ref1.spalte12,
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
