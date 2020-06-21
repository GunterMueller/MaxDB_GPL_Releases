*   *ID* T04SEL1  DBCMD    changed on 1992-09-28-13.28.38 by ADMIN     *
 ***************************
 ***   T 0 4 S E L 1     ***
 ***************************!
 *** Testfaelle fuer :                         ***
 *** Leere Menge                               ***
 *** Suche in 1 Tabelle                        ***
 *** Select mit Views                          ***
 *** Select mit UNDEF-Werten                   ***
 *** asc und desc                              ***
 *** nicht distinct und distinct               ***
 *** Select mit Views und OR                   ***
 *** Join mit 2 Tabellen  in beiden Richtungen ***
 *** alle Joinoperatoren ueber Nicht-Key-Feld  ***
 *** alle Joinoperatoren ueber Key-Feld        ***
 *** Join mit 3 Tabellen  in allen Richtungen  ***
 *** Leere Menge !
declare erg1 cursor for select k1,f1
       from p2 !
close erg1 !
commit work !
lock table p2 in exclusive mode !
insert into p2 set k1 = 'a', f1 = 'c' !
delete from p2 KEY k1 = 'a' !
declare erg1 cursor for select k1,f1
       from p2 !
close erg1 !
commit work !
declare erg1 cursor for select k1,f1
       from p2 !
close erg1 !
 *** Suche in 1 Tabelle !
select k1, f1
       from p3 !
fetch !
close !
declare erg1 cursor for select k1 e1,f1 e2
       from p3 for reuse !
fetch erg1 !
again erg1 !
fetch erg1 !
declare erg11 cursor for select e1
     from erg1
     where erg1.e1 = 'x' !
fetch erg11 !
fetch first erg1 !
declare erg12 cursor for select e1
     from erg1
     where erg1.e1 = 'r' !
fetch erg12 !
declare erg13 cursor for select 
  erg1.e1 f1,erg1.e2 f2,p4.k1 f3,p4.f1 f4
     from erg1, p4
     where erg1.e2 = p4.f1 !
fetch erg13 !
declare erg131 cursor for select f1,f2,f3,f4
     from erg13
     where erg13.f1 = f2 and
           erg13.f2 = f3 and
           erg13.f3 = f4 !
fetch erg131 !
close erg131 !
close erg13 !
close erg11 !
close erg12 !
fetch erg12 !
fetch erg11 !
close erg1 !
fetch erg1 !
declare erg2 cursor for select k1,f1
     from p3
     order by f1 desc !
fetch erg2 !
declare erg3 cursor for select k1,f1
     from p3
     where p3.f1 = 'c' !
fetch erg3 !
again erg2 !
fetch erg2 !
fetch erg1 !
close erg1 !
close erg3 !
declare erg3 cursor for select k1 e1,f1 e2
       from p3
       where p3.k1 = 'c' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1 e1,f1 e2
       from p3
       where p3.k1 > 'c' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1 e1,f1 e2
       from p3
       where p3.k1 >= 'c' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1 e1,f1 e2
       from p3
       where p3.k1 < 'x' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1 e1,f1 e2
       from p3
       where p3.k1 <= 'x' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1 e1,f1 e2, f1 e3
       from p3
       where p3.k1 = 'c' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1,f1
       from p3
       where p3.k1 < 'b' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1,f1
       from p3
       where p3.k1 > 'b' and
       p3.k1 < 'u' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1,f1
       from p3
       where p3.f1 <> 'c' and
       p3.k1 < 'x' !
fetch erg3 !
close erg3 !
declare erg3 cursor for select k1,f1
       from p3
       where p3.k1 = f1 !
fetch erg3 !
close erg3 !
 *** Select mit Views !
declare erg11 cursor for select k1,f1
       from p3 !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
       from p31 !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
       from p31
       where p31.k1 < 'b' !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
       from p31
       where p31.k1 = 'x' !
fetch erg11 !
close erg11 !
 *** Select mit UNDEF-Werten !
declare erg1 cursor for select k1 e1,f1 e2
     from p6 !
fetch erg1 !
close erg1 !
declare erg1 cursor for select k1,f1
     from p6
     where p6.f1  is  NULL !
fetch erg1 !
close erg1 !
declare erg1 cursor for select p3.k1 f1,p3.f1 f2,p6.k1 f3,p6.f1 f4
     from p3, p6
     where p3.f1 = p6.f1 !
fetch erg1 !
close erg1 !
declare erg1 cursor for select p6.k1 f1,p6.f1 f2,pa.k1 f3,pa.f1 f4
     from p6, p6  pa
     where p6.f1 = pa.f1 !
fetch erg1 !
close erg1 !
 *** asc und desc !
declare erg1 cursor for select k1,f1
       from p3
       order by k1 asc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select k1,f1
       from p3
       order by k1 desc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select k1,f1
       from p3
       order by f1 asc, k1 asc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select k1,f1
       from p3
       order by f1 asc, k1 desc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select k1,f1
       from p3
       order by f1 desc, k1 asc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select k1,f1
       from p3
       order by f1 desc, k1 desc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
       from p3, p4
       order by p4.k1 asc, p3.k1 desc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
       from p3, p4
       where p3.f1 = p4.f1
       order by p4.f1 asc, p3.k1 asc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
       from p3, p4
       where p3.f1 = p4.f1
       order by p4.f1 asc, p3.k1 desc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
       from p3, p4
       where p3.f1 = p4.f1
       order by p4.f1 desc, p3.k1 asc !
fetch erg1 !
close erg1 !
declare erg1 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
       from p3, p4
       where p3.f1 = p4.f1
       order by p4.f1 desc, p3.k1 desc !
fetch erg1 !
close erg1 !
 *** nicht distinct und distinct !
declare erg4 cursor for select f1
     from p3 !
fetch erg4 !
close erg4 !
declare erg4 cursor for select distinct f1
     from p3
     order by f1 asc !
fetch erg4 !
close erg4 !
declare erg4 cursor for select distinct f1
     from p3 !
fetch erg4 !
close erg4 !
 *** Select mit Views und OR !
declare erg11 cursor for select k1,f1
     from p3 !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p31 !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p32 !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p33 !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p34 !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p31
     where p31.k1 < 'b' !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p31
     where p31.k1 = 'x' !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p32
     where p32.k1 = 'c' or p32.k1 = 'x' !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p33
     where p33.k1 <> 'c' !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p33
     where not (p33.k1 = 'c') !
fetch erg11 !
close erg11 !
declare erg11 cursor for select k1,f1
     from p34
     where (p34.k1 = 'a') or
           (p34.k1 = 'c') or
           (p34.k1 = 'x') !
fetch erg11 !
close erg11 !
declare erg21 cursor for select k1,f1
     from p5 !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where p5.k1 = 'l' !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where (p5.k1 = 'l') or (p5.k1 = 'm') !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where (p5.k1 = 'l') or (p5.k1 = 'x') !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where ((p5.k1 = 'l') or (p5.k1 = 'm')) and
             not (p5.k1 = 'n') !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where ((p5.k1 = 'l') or (p5.k1 = 'm')) and
             (p5.k1 <> 'n') !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where ((p5.k1 = 'l') and (p5.k1 <> 'n')) or
             ((p5.k1 = 'm') and (p5.k1 <> 'n')) !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where ((not (p5.k1 <> 'l')) and (not (p5.k1 = 'n'))) or
             ((not (p5.k1 <> 'm')) and (not (p5.k1 = 'n'))) !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where not ((p5.k1 <> 'l') or (p5.k1 = 'n')) or
             not ((p5.k1 <> 'm') or (p5.k1 = 'n')) !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where not (((p5.k1 <> 'l') or (p5.k1 = 'n')) and
                  ((p5.k1 <> 'm') or (p5.k1 = 'n'))) !
fetch erg21 !
close erg21 !
declare erg21 cursor for select k1,f1
       from p5
       where not (((p5.k1 <> 'l') and (p5.k1 <> 'm')) or
                 (p5.k1 = 'n')) !
fetch erg21 !
close erg21 !
 *** Join mit 2 Tabellen in beiden Richtungen !
declare erg4 cursor for select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.f1 = p4.f1 !
fetch erg4 !
declare erg5 cursor for select p3.k1
     from p3, p4
     order by p3.k1 desc
     where p3.f1 = p4.f1 !
fetch erg5 !
close erg5 !
declare erg5 cursor for select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p4.f1 = p3.f1 !
fetch erg5 !
close erg5 !
declare erg5 cursor for select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
       from p3, p4
       where p3.k1 = p4.f1 and
             p3.k1 = p4.k1 !
fetch erg5 !
close erg5 !
declare erg5 cursor for select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
       from p3, p4
       where p3.f1 = p4.f1 and
             p3.k1 <> p4.k1 !
fetch erg5 !
close erg5 !
declare erg5 cursor for select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
       from p3, p4
       where p3.f1 = p4.f1 and
             p3.k1 = 'x' !
fetch erg5 !
close erg5 !
declare erg5 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
       from p3, p4
       where p3.f1 = p4.f1 and
             p3.k1 = 'b' !
fetch erg5 !
close erg5 !
 *** alle Joinoperatoren ueber Nicht-Key-Feld !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.f1 = p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.f1 > p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.f1 < p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.f1 >= p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.f1 <= p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.f1 <> p4.f1 !
fetch erg6 !
close erg6 !
 *** alle Joinoperatoren ueber Key-Feld !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.k1 = p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.k1 > p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.k1 < p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.k1 >= p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.k1 <= p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.k1 <> p4.k1 !
fetch erg6 !
close erg6 !
 *** Join mit 3 Tabellen in allen Richtungen !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p3.f1 = p4.f1 and
           p4.f1 = p5.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p5.f1 = p4.f1 and
           p4.f1 = p3.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p5.f1 = p4.f1 and
           p3.f1 = p4.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p3.f1 = p4.f1 and
           p5.f1 = p4.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p4.f1 = p3.f1 and
           p4.f1 = p5.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p4.f1 = p5.f1 and
           p4.f1 = p3.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
       from p3, p4, p5
       where p3.f1 = p4.f1 and
             p4.k1 = p5.k1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for select
 p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
       from p3, p4, p5
       where p3.f1 = p4.f1 and
             p4.k1 < p5.k1 !
fetch erg7 !
close erg7 !
