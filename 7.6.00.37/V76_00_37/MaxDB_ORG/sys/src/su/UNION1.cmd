*   *ID* UNION1   DBCMD    changed on 1992-09-28-13.28.44 by ADMIN     *
file connect ( kern test !
select * from tab1 union select * from tab2 !
fetch !
select * from tab1 union select * from tab3 !
fetch !
select * from tab2 union select * from tab3 !
fetch !
select * from tab1 union all select * from tab2 !
fetch !
select * from tab1 union all select * from tab3 !
fetch !
select * from tab2 union all select * from tab3 !
fetch !
select * from tab2 union all select * from tab4 !
fetch !
select * from tab2 union all select * from tab1 !
fetch !
select t1c1, t1c2 from tab1 union select t2c2, t2c1 from tab2 !
fetch !
select t2c3, t2c4 from tab2 union select t3c4, t3c3 from tab3 !
fetch !
select t1c5 from tab1
 union select t2c5 from tab2
  union select t3c5 from tab3!
fetch !
select t1c1, t1c5 from tab1 union select t2c2, t2c1 from tab2 !
fetch !
select t1c1, t1c2 from tab1
 union select ascii(t1c1), ascii(t1c2) from tab1 !
fetch !
select t1c1, t1c2 from tab1
 union all select ascii(t1c1), ascii(t1c2) from tab1 !
fetch !
select t1c3, t1c4 from tab1
 union all (select t1c3, t1c4 from tab1 where t1c3 > 120
             union select t1c3, t1c4 from tab1 ) !
fetch !
select t1c3, t1c4 from tab1
 union all select t1c3, t1c4 from tab1 where t1c3 > 120
  union select t1c3, t1c4 from tab1  !
fetch !
select t1c3, t1c4 from tab1
 union all select t1c3, t1c4 from tab1 where t1c3 > 120
  union all select t1c3, t1c4 from tab1  !
fetch !
update tab1 set t1c1 = 'abc' where t1c2 in
    ( select t1c2 from tab1 union select t2c2 from tab2) !
select * from tab1 !
fetch !
delete tab1 where t1c2 in
    ( select t1c2 from tab1 union select t2c2 from tab2) !
select * from tab1 !
fetch !
rollback work !
insert into tab3
    select * from tab2 union select * from tab3 !
select * from tab3 !
fetch !
rollback release !
