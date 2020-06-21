*   *ID* N28SEL2  DBCMD    changed on 1992-09-28-13.25.53 by ADMIN     *
file connect ( kern test !
declare selecttab1 cursor for 
select tab1.spalte11, kern.tab1.spalte13,
       ref2.spalte22 neu22,
       ref2.spalte23  neu23, kern.tab3.spalte33
   from kern.tab1, tab2  ref2 , tab3
   where tab1.spalte13 <= kern.tab3.spalte33
     and ref2.spalte22 <> tab1.spalte11
     and not ( ref2.spalte23 is null ) !
fetch selecttab1 !
declare selecttab3 cursor for 
select distinct tab1.spalte11, kern.tab1.spalte13,
       ref2.spalte22 neu22,
       ref2.spalte23 neu23, kern.tab3.spalte33
   from kern.tab1, tab2  ref2 , tab3
   where tab1.spalte13 <= kern.tab3.spalte33
     and ref2.spalte22 <> tab1.spalte11
     and not ( ref2.spalte23 is null )
        order by tab1.spalte11, neu22, kern.tab3.spalte33 desc !
fetch selecttab3 !
declare selecttab3 cursor for 
select tab1.spalte11, kern.tab1.spalte13,
       ref2.spalte22 neu22,
       ref2.spalte23 neu23, kern.tab3.spalte33
   from kern.tab1, tab2  ref2 , tab3
   where tab1.spalte13 <= kern.tab3.spalte33
     and ref2.spalte22 <> tab1.spalte11
     and not ( ref2.spalte23 is null )
   order by kern.tab1.spalte13 asc, ref2.spalte22 desc !
fetch selecttab3 !
commit work release !
