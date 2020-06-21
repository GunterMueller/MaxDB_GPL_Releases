*   *ID* N99A29   DBCMD    changed on 1992-09-28-13.27.20 by ADMIN     *
file connect ( kern test !
* Fehlerprotokoll 183 !
create table t_reis (
s1 char (1),
s2 char (1),
s3 char (1),
s4 char (1),
s5 char (1),
s6 char (1),
s7 char (1),
s8 char (1))!
create view v_reis as select s1 from t_reis
where
   s2 = 'J'
and (s3 = 'N' and not (s4 is null or s5 is null))
and (s3 = 'F' and not (s6 is null or s7 is null))
with check option !
 * next cmd was inkorrekt with 2.4.07 cl 4 !
create view v_reis1 as select s1 from t_reis
where
    (s3 = 'N' and not (s4 is null or s5 is null))
and (s3 = 'F' and not (s6 is null or s7 is null))
and  s2 = 'J'
with check option !
create view v_reis2 as select s1 from t_reis
where
    s3 = 'N' and not (s4 is null or s5 is null)
and s3 = 'F' and not (s6 is null or s7 is null)
and   s2 = 'J'
with check option !
create view v_reis3 as select s1 from t_reis
where
    s3 = 'N' and s4 is not null and s5 is not null
and s3 = 'F' and s6 is not null and s7 is not null
and   s2 = 'J'
with check option !
drop table t_reis !
commit release !
