*   *ID* SQL1     DBCMD    changed on 1992-09-28-13.28.11 by ADMIN     *
file connect ( kern test !
*select first laufzeit = time-zeit from z !
select max(qty) - min(qty)
   from p px, spj spjx
   where px.pno = spjx.pno
     and spjx.sno =
   any ( select s.sno
         from p, spj, s, j
         where p.pno = spjx.pno
           and s.city = j.city
           and px.city = s.city
         group by s.sno )
   group by qty !
fetch !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
select spjx.sno, spjx.pno, spjx.jno, s.sname
  from spj spjx, s, j jx
  where s.status <
    all ( select max(qty) - min(qty)
          from spj spjy, j
          where jx.jno = spjy.jno
            and spjy.qty <
                  all ( select max ( spj.qty)
                        from spj
                        where spj.jno < spjx.jno
                        group by sno )
          group by qty ) !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
select spjx.sno, spjx.pno, spjx.jno, s.sname
  from spj spjx, s, j jx
  where s.status <>
    all ( select max(qty) - min(qty)
          from spj spjy, j
          where jx.jno = spjy.jno
            and spjy.qty <
                  all ( select max ( spj.qty)
                        from spj
                        where spj.jno < spjx.jno
                        group by sno )
          group by qty ) !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
select city from j jx
  where jx.city in ( 'Paris', 'Oslo' )
    and jx.city in ( select sx.city from s sx
                      where sx.city <
                       any ( select p.city from p
                             where p.weight + sx.status < 50 ) ) !
fetch !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
select distinct max(qty) - min(qty)
   from p px, spj spjx
   where px.pno = spjx.pno
     and spjx.sno =
   any ( select distinct s.sno
         from p, spj, s, j
         where p.pno = spjx.pno
           and s.city = j.city
           and px.city = s.city
         group by s.sno )
   group by qty !
fetch !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
select distinct spjx.sno, spjx.pno, spjx.jno, s.sname
  from spj spjx, s, j jx
  where s.status <
    all ( select distinct max(qty) - min(qty)
          from spj spjy, j
          where jx.jno = spjy.jno
            and spjy.qty <
                  all ( select distinct max ( spj.qty)
                        from spj
                        where spj.jno < spjx.jno
                        group by sno )
          group by qty ) !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
select distinct city from j jx
  where jx.city in ( 'Paris', 'Oslo' )
    and jx.city in ( select distinct sx.city from s sx
                      where sx.city <
                       any ( select distinct p.city from p
                             where p.weight + sx.status < 50 ) ) !
fetch !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
select spjx.sno, px.city
  from spj spjx, p px
  where spjx.sno in
        ( select s.sno from s
          where px.pno = spjx.pno )
          and spjx.sno = 'S3'
           or spjx.sno = 'S4' !
fetch !
* select first zeit=time from p !
*select first laufzeit = time-zeit from z !
commit work release !
