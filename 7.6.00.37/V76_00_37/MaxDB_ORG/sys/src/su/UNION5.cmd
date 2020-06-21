*   *ID* UNION5   DBCMD    changed on 1992-09-28-13.28.48 by ADMIN     *
file connect ( kern test !
select t1c1, t1c2 from tab1 intersect
   select t1c1, t1c2 from tab1 where t1c3 > 120!
fetch !
select t1c1, t1c2 from tab1 where t1c3 < 120 intersect
   select t1c1, t1c2 from tab1 !
fetch !
select t1c3, t1c4 from tab1
  except select t1c3, t1c4 from tab1 where t1c1 > '111ab'
    intersect select t1c3, t1c4 from tab1 where t1c1 < '114ab' !
fetch !
( select t1c3, t1c4 from tab1
   except select t1c3, t1c4 from tab1 where t1c1 > '111ab' )
 intersect select t1c3, t1c4 from tab1 where t1c1 < '114ab' !
fetch !
create view v1 as
  select t1c1 from tab1
    union all select t1c2 from tab1 where t1c3 > 120 !
create view v2 as
  select t1c2 from tab1
    union all select t1c1 from tab1 where t1c3 < 400 !
select * from v1 !
fetch !
select * from v2 !
fetch !
select * from v1 intersect select * from v2 !
fetch !
* implicit added from-select causes trouble in April 2006 
* has to be changed, of course !
* select * from v1
*   except select t1c2 from tab1
*     union all select * from v2
*       intersect select t1c2 from tab1 where t1c3 > 120 !
* fetch !
* ( select * from v1
*     except select t1c2 from tab1
*       union all select * from v2 )
*  intersect select t1c2 from tab1 where t1c3 > 120 !
* fetch !
drop view v1 !
drop view v2 !
create view v1 as
  select t1c1 from tab1
    union all select t1c1 from tab1 where t1c3 > 120 !
create view v2 as
  select t1c1 from tab1
    union all select t1c1 from tab1 where t1c3 < 400 !
select * from v1 !
fetch !
select * from v2 !
fetch !
select * from v1 intersect select * from v2 !
fetch !
select * from v1 intersect all select * from v2 !
fetch !
rollback release !
