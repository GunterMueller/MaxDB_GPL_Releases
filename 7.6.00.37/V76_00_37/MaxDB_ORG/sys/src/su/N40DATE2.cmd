*   *ID* N40DATE2 DBCMD    changed on 1995-01-17-18.30.00 by Heinz     *
file pnxdiff!
file connect ( kern test sqlmode db2 !
set format iso !
 **** DATE _ TIME _ FUNCTIONS **** !
declare c cursor for select year(a),a   from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select month(a),a  from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select day(a),a    from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select hour(a),a   from tab!
close c  !
declare c cursor for select minute(a),a from tab!
close c  !
declare c cursor for select second(a),a from tab!
close c  !
declare c cursor for select microsecond(a),a from tab !
close c  !
declare c cursor for select year(b),b   from tab!
close c  !
declare c cursor for select month(b),b  from tab!
close c  !
declare c cursor for select day(b),b    from tab!
close c  !
declare c cursor for select hour(b),b   from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select minute(b),b from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select second(b),b from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select microsecond(b),b from tab !
close c !
declare c cursor for select year(c),c   from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select month(c),c  from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select day(c),c    from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select hour(c),c   from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select minute(c),c from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select second(c),c from tab!
fetch c into :a, :b !
close c  !
declare c cursor for select microsecond(c),c from tab !
fetch c into :a, :b !
close c  !
 **** DATE ARITHMETIK **** !
declare c cursor for select a + 10 year from tab!
fetch c into :a !
close c  !
declare c cursor for select date('1988-02-29') + 1 year from tab!
fetch c into :a !
close c  !
declare c cursor for select a, a + 110 - 110 from tab !
fetch c into :a, :b !
close c  !
declare c cursor for select a + 340699 from tab!
fetch c into :a !
close c  !
declare c cursor for select a - year(12345678) years -
       month(12345678) months -
       day(12345678) days from tab!
fetch c into :a !
close c  !
declare c cursor for select a - 12345678 from tab !
fetch c into :a !
close c  !
declare c cursor for select a - current b from tab!
close c !
declare c cursor for select a + current date from tab!
close c !
declare c cursor for select '1989-03-31' - 1 month from tab!
fetch c into :a !
close c  !
declare c cursor for select '1989-03-31' + 1 month from tab!
fetch c into :a !
close c  !
declare c cursor for select date('1989-03-31') - 1 month from tab!
fetch c into :a !
close c  !
declare c cursor for select date('1989-03-31') + 1 month from tab!
fetch c into :a !
close c  !
declare c cursor for select '1989-03-31' - a  from tab!
fetch c into :a !
close c  !
declare c cursor for select date('1999-02-28') + 1 years +
       1 days - 1 days - 1 year from tab!
fetch c into :a !
close c  !
declare c cursor for select date('1999-02-28') + 1 years +
       1 days - 1 year - 1 day from tab!
fetch c into :a !
close c  !
 **** TIME ARITHMETIK **** !
declare c cursor for select b + 10 hour from tab!
fetch c into :a !
close c  !
declare c cursor for select b + 10 hour - 10 hours from tab!
fetch c into :a !
close c  !
declare c cursor for select char(time('11:02:29') + 1 hour, usa)
       from tab!
fetch c into :a !
close c  !
declare c cursor for select b, b + 110 - 110 from tab !
fetch c into :a, :b !
close c  !
declare c cursor for select b + 340699 from tab!
fetch c into :a !
close c  !
declare c cursor for select b - hour(345678) hours -
       minute(345678) minutes -
       second(345678) seconds from tab!
fetch c into :a !
close c  !
declare c cursor for select b - 345678 from tab !
fetch c into :a !
close c  !
declare c cursor for select 
(b - current time) - (b -current time) from tab!
fetch c into :a !
close c  !
declare c cursor for select b + current time from tab!
close c !
declare c cursor for select '12:03' - 1 second from tab!
fetch c into :a !
close c  !
declare c cursor for select '12:03' + 1 second from tab!
fetch c into :a !
close c  !
declare c cursor for select time('12:03') - 1 second from tab!
fetch c into :a !
close c  !
declare c cursor for select time('12:03') + 1 second from tab!
fetch c into :a !
close c  !
declare c cursor for select time('23:59:59') + 1 hours +
        1 second - 1 second- 1 hour from tab!
fetch c into :a !
close c  !
declare c cursor for select time('23:59:59') + 1 hour +
        1 second - 1 hour - 1 second from tab!
fetch c into :a !
close c  !
declare c cursor for select 
(b + (3 * minute(c) + 45 ) seconds - current time)
-
(b + (3 * minute(c) + 45 ) seconds - current time)
from tab !
fetch c into :a !
close c  !
 **** TIMESTAMP ARITHMETIK **** !
declare c cursor for select timestamp(a,b) - c from tab!
fetch c into :a !
close c !
declare c cursor for select c + 123456 from tab !
fetch c into :a !
close c !
declare c cursor for select timestamp(a,b) - 1988 year from tab !
fetch c into :a !
close c  !
declare c cursor for select c - 1989 years from tab !
fetch c into :a !
close c  !
declare c cursor for select timestamp('0001-01-01','00:00:00') +
       9998 years + 11 months
       + 30 days + 23 hours + 59 minutes + 59 seconds
       + 999999 microseconds from tab !
fetch c into :a !
close c  !
declare c cursor for select timestamp('0001-01-01','00:00:00') +
       9998 years + 11 months
       + 30 days + 23 hours + 59 minutes + 59 seconds
       + 999999 microseconds + 1 microsecond from tab !
 ***** SONSTIGES **** !
set format iso !
declare c cursor for select days(a) from tab!
fetch c into :a !
close c  !
declare c cursor for select days(c) from tab !
fetch c into :a !
close c  !
declare c cursor for select days(date(c)) from tab !
fetch c into :a !
close c  !
declare c cursor for select days('1989-02-28') from tab !
fetch c into :a !
close c  !
declare c cursor for select date(days('1989-02-28')) from tab !
fetch c into :a !
close c  !
commit release !
file connect ( kern test sqlmode internal !
set format iso !
declare c cursor for select addtime('56:59:59','56:59:59') from tab !
fetch c into :a !
close c  !
set format usa !
declare c cursor for select addtime('11:59 pm','11:59 pm') from tab !
fetch c into :a !
close c  !
commit release !
