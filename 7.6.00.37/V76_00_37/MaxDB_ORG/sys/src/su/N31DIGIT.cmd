*   *ID* N31DIGIT DBCMD    changed on 1992-09-28-13.26.06 by ADMIN     *
create table t (a fixed (10), b fixed (10,10)) !
create table tab ( a fixed(8,4),
                   b fixed(3),
                   c char(4),
                   d float(7) )!
commit work release !
file connect ( kern test sqlmode db2 !
insert into t values ( 1234567891,  0.1234567891 )!
insert into t values (  123000000,  0.000000123 )!
insert into t values (        123,  0.123 )!
insert into t values (          0,  0 )!
insert into t values (-       123, -0.123 )!
insert into t values (-  12300000, -0.00000123 )!
insert into t values (-1234567891, -0.1234567891 )!
declare c cursor for select digits(a), digits (b) from t !
fetch c into :a, :b !
close c  !
drop table t !
insert into tab values( 1234.56, 987, 'abcd', 24e8)!
insert into tab values (-987.65, 0, 'aaa', 0)!
insert into tab values (4567, -3, 'ttt', 34)!
declare c cursor for select * from tab !
fetch c into :a, :b, :c, :d !
close c  !
commit work !
declare c cursor for select digits(a), a from tab !
fetch c into :a, :b !
close c  !
declare c cursor for select digits(b) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(c), c from tab !
close c !
declare c cursor for select digits(d), d from tab !
close c !
declare c cursor for select digits(date), digits(time) from tab !
close c !
declare c cursor for select digits('1234') from tab !
close c !
declare c cursor for select digits(substr('1234',2,2)) from tab !
close c !
declare c cursor for select sum(digits(a)) from tab !
close c !
declare c cursor for select digits(sum(a)), a from tab !
close c !
declare c cursor for select digits(min(a)) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(max(a+b)) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(-123) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(0.123456) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(8e3) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(-8e3) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(8e-3) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits(-8e-3) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits( a * 0.1236 + 10 * b ) from tab !
fetch c into :a !
close c  !
declare c cursor for select digits('19890124' - date('19890309')) from tab !
fetch c into :a !
close c  !
commit work release !
file connect ( kern test !
