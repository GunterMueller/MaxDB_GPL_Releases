*   *ID* UNION6   DBCMD    changed on 1992-09-28-13.28.48 by ADMIN     *
file connect ( kern test !
select distinct t1c1 from tab1, tab2 group by t1c1!
fetch !
select t1c1 from tab1, tab2 group by t1c1
   union select t1c1 from tab2, tab1 group by t1c1!
fetch !
select t3c1 from tab2, tab3 group by t3c1 !
fetch !
select t1c1 from tab1, tab2 group by t1c1
   union all select t3c1 from tab2, tab3 group by t3c1!
fetch !
select t1c1 from tab1, tab2 
   union all select distinct t3c1 from tab2, tab3 !
fetch !
select distinct t1c1 from tab1, tab2 
   union all select distinct t3c1 from tab2, tab3 !
fetch !
select distinct t1c1 from tab1, tab2 
   union all select t3c1 from tab2, tab3 !
fetch !
select count(*) from tab1, tab2 group by t1c2 !
fetch !
select count(*) from tab3 group by t3c2 !
fetch !
select count(*) from tab1, tab2 group by t1c2
union all select count(*) from tab3 group by t3c2 !
fetch !
select count(*) from tab1, tab2 group by t1c2
union select count(*) from tab3 group by t3c2 !
fetch !
create table e ( a fixed (4), b char (8))!
insert e values (1, 'aaa')!
insert e values (1, 'aaa')!
insert e values (2, 'bbb')!
select sum(a) from e group by b!
fetch!
select sum(a) from e group by b
  union all
select sum(a) from e group by b!
fetch !
select sum(a) from e group by b
  union 
select sum(a) from e group by b!
fetch !
drop table e !
commit release !
