*   *ID* N25SEL7  DBCMD    changed on 1992-09-28-13.25.48 by ADMIN     *
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
