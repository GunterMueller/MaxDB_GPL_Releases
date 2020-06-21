*   *ID* DEMOK9   DBCMD    changed on 1992-09-28-13.21.37 by ADMIN     *
file connect ( travel10 travel10 sqlmode db2 !
* April 1992, Folie 9.010 !
*
*CS*1992-08-17:  frueher: select <expression> <resultcolumnname>
*                jetzt:   ... ohne res_col_name
*
declare c cursor for
select          price*1.115,
       decimal (price*1.115, 8, 2),
       integer (price*1.115),
       float   (price*1.115)
  from room !
fetch c into :a, :b, :c, :d !
commit release !
file connect ( travel10 travel10 !
* April 1992, Folie 9.020 !
select        price*1.115 x,
       round (price*1.115, 2) r2,
       trunc (price*1.115, -1) r1
  from room !
fetch !
* April 1992, Folie 9.030 !
select        price*1.115-100 x,
       ceil  (price*1.115-100) c,
       floor (price*1.115-100) f,
       round (price*1.115-100) r,
       trunc (price*1.115-100) t
  from room !
fetch !
* April 1992, Folie 9.040 !
select       price-70 x,
       sign (price-70) s,
       abs  (price-70) a
  from room !
fetch !
* April 1992, Folie 9.050 !
select        hno x,
       power (hno, 3) p,
       sqrt  (hno) s
  from hotel !
fetch !
commit release !
file connect ( travel10 travel10 sqlmode oracle !
* April 1992, Folie 9.060 !
declare c cursor for
select      price-70 x,
       mod (price-70, 12) m,
       mod (price-70, 0) n
  from room !
fetch c into :a, :b, :c !
commit release !
file connect ( travel10 travel10 sqlmode internal !
* April 1992, Folie 9.070 !
select        name x,
       index (name, 'en') i1,
       index (name, 'n', 4) i2,
       index (name, 'e', 3, 2) i3
  from hotel !
fetch !
commit release !
file connect ( travel10 travel10 sqlmode internal !
* April 1992, Folie 9.080 !
select lpad  (name, 2, '.') lp,
       lfill (name, '.', 16) lf
  from hotel !
fetch !
* April 1992, Folie 9.090 !
select               name x,
       ltrim (       name, 'L') lt,
       rtrim (       name, 's') rt,
       trim  (upper (name),'E') t
  from hotel !
fetch !
commit release !
file connect ( travel10 travel10 sqlmode internal !
* April 1992, Folie 9.100 !
create table testtable (x char (30)) !
insert testtable set x = 'THIS is a Test' !
select          x,
       initcap (x) i
  from testtable !
fetch !
rollback !
* April 1992, Folie 9.110 !
select           name x,
       replace  (name, 'Long', 'Small') rep,
       replace  (name, 'Beach') rem
  from city !
fetch !
select            name x,
       translate (name, 'el', 'ir') tr
  from city !
fetch !
commit release !
file connect ( travel00 travel00 sqlmode internal !
* April 1992, Folie 9.120 !
select        firstname x,
       decode (firstname,
                             '-', NULL,
                             firstname) d
  from customer !
fetch !
select        d x,
       value (d, 'Company') v
  from (
        select decode (firstname,
                                 '-', NULL,
                                 firstname) d
          from customer)
       !
fetch !
commit release !
file connect ( travel10 travel10 sqlmode internal !
* April 1992, Folie 9.130 !
select                price x,
       least    ( 80, price) l,
       greatest (100, price) g
  from room !
fetch !
* April 1992, Folie 9.140 !
select state,
       decode (state,
                      'CA',  5.7,
                      'DC',  6.5,
                      'FL',  9  ,
                      'IL', 11.5,
                      'MI',  8  ,
                             0 )
  from city !
fetch !
commit release !
file connect ( travel20 travel20 sqlmode internal !
* April 1992, Folie 9.150 !
select year(arrival), month(arrival), day(arrival)
    from booking !
fetch!
commit release !
file connect ( travel20 travel20 sqlmode db2 !
* Februar 1993, Folie 9.155 !
declare c cursor for
select        (max_free*price/100)-50,
substr(digits((max_free*price/100)-50), 34)
    from room !
fetch c into :a, :b, :c !
close c !
commit !
declare c cursor for
select substr(digits((max_free*price/100)-50), 34),
        char(decimal((max_free*price/100)-50, 18, 12))
    from room !
fetch c into :a, :b, :c !
commit release !
file connect ( travel20 travel20 sqlmode internal !
* April 1992, Folie 9.160 !
declare c cursor for
select       arrival,
       char (arrival, iso),
       char (arrival, usa)
  from booking !
fetch c into :a, :b, :c !
* April 1992, Folie 9.180 !
select      hno x1,
       hex (hno) h1
  from hotel !
fetch !
select      state x2,
       hex (state) h2
  from city !
fetch !
commit release !
file connect ( travel20 travel20 sqlmode oracle !
* April 1992, Folie 9.190 !
create table r_ora (arrival date, departure date)!
insert into r_ora values ('14-NOV-92', '18-NOV-92')!
insert into r_ora values ('03-MAR-92', '03-APR-92')!
insert into r_ora values ('14-NOV-92', '24-DEC-92')!
declare c cursor for
select                 arrival a,
       add_months     (arrival, 4) am,
       last_day       (arrival) l,
       months_between (departure, arrival) mb
  from r_ora !
fetch c into :a, :b, :c, :d !
close c !
declare c cursor for
select to_number ('123') + 4
  from booking
 where rno = 100 !
fetch c into :a !
close c !
* The following call of a user defined function is only possible in a
* database with that function installed. So here we better ignore that!
* declare c cursor for select to_char (arrival, 'YYYYMMDDHHMISS') from r_ora!
declare c cursor for select arrival from r_ora!
fetch c into :a !
close c !
declare c cursor for
select      rno r,
       chr (rno) c
  from booking
 where rno <= 128 !
fetch c into :a, :b !
close c !
declare c cursor for
select rawtohex ( hextoraw ( '796A' ) )
  from booking
 where rno = 100 !
fetch c into :a !
commit release !
file connect ( travel20 travel20 sqlmode internal !
drop table r_ora !
commit release !
file connect ( travel20 travel20 sqlmode db2 !
* April 1992, Folie 9.200 !
declare c cursor for
select              arrival,
       days        (arrival),
       timestamp   (arrival, '00083500'),
       date        (arrival),
       time        ('00075523'),
       microsecond (timestamp(arrival, '00083500'))
  from booking
 where rno = 100 !
fetch c into :a, :b, :c, :d, :e, :f, :g, :h, :i, :j, :k, :l !
close c !
commit release !
file connect ( travel10 travel10 sqlmode internal !
* April 1992, Folie 9.210 !
create table testtable (x fixed (2)) !
insert testtable set x = 10 !
insert testtable set x = 20 !
insert testtable set x = 30 !
insert testtable set x = 15 !
insert testtable set x = 27 !
select variance (x) v,
       stddev   (x) s
from testtable !
fetch !
rollback release !
