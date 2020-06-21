*   *ID* N25SEL3  DBCMD    changed on 1992-09-28-13.25.46 by ADMIN     *
 *** <single select statement> wird getestet.
 *** Mit JOINS ueber drei Tabellen. !
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
*----------------------------------------------------
* das folgende SELECT ist stillgelegt, bis der Fehler
* (Loop) im schnellen System beseitigt ist.
*----------------------------------------------------
*!
*EXPLAIN
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
       ref1_6789012345678.spalte12 neu12,
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
