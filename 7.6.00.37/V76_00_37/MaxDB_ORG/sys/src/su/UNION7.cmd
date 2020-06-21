*   *ID* UNION7   DBCMD    changed on 1992-09-28-13.28.48 by ADMIN     *
file connect ( kern test !
select t1c1 from tab1 where t1c3 > 500
  union ( select t1c2 from tab1
            except select t1c2 from tab1 where t1c3 > 120 )!
fetch !
select t1c1 from tab1 where t1c3 > 500
  union select t1c2 from tab1 !
fetch !
select t1c1 from tab1
  union select t1c2 from tab1 where t1c3 > 500!
fetch !
select t1c1 from tab1 where t1c3 > 120
  except select t1c1 from tab1 where t1c3 > 500 !
fetch !
select t1c1 from tab1 where t1c3 > 500
  except select t1c1 from tab1 where t1c3 > 100 !
fetch !
select t1c1 from tab1 where t1c3 > 500
  intersect select t1c1 from tab1
    union all select t1c1 from tab1 !
fetch !
select t1c1 from tab1
  intersect select t1c1 from tab1 where t1c3 > 500 !
fetch !
commit release !
