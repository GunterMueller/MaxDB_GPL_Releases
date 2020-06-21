*   *ID* N40DATE1 DBCMD    changed on 1995-01-17-18.30.00 by Heinz     *
file pnxdiff!
file connect ( kern test sqlmode db2 !
create table tab (a date,
                  b time,
                  c timestamp )!
commit work !
 **** DATE TIME TIMESTAMP INSERT ****
 **** CURRENT VALUE **** !
insert into tab values (current date, current time, current timestamp)!
insert into tab values (current date, current time, current timestamp)!
insert into tab values (current date, current time, current timestamp)!
insert into tab values (current time, current time, current timestamp)!
insert into tab values (current date, current date, current timestamp)!
insert into tab values (current date, current time, current time)!
insert into tab values (current timestamp, current time, current timestamp)!
insert into tab values (current date, current timestamp, current timestamp)!
insert into tab values (current date, current time, current date)!
insert into tab values (current date, current time, current timestamp)!
insert into tab values (current date, current time, current timestamp)!
insert into tab values (current date, current time, current timestamp)!
declare c cursor for select * from tab !
fetch c into :a, :b, :c !
close c !
rollback work !
 **** FORMAT INSERT **** !
set format iso !
insert into tab values ('19890124', current time, current timestamp) !
insert into tab values (current date, '00184605', current timestamp) !
insert into tab values (current date, current time,
        '19890124184605476321') !
insert into tab values('1989-01-24','18:46:05','1989-01-24 18:46:05.476321')!
declare c cursor for select * from tab !
fetch c into :a, :b, :c !
close c !
set format usa !
insert into tab values ('19890124', current time, current timestamp) !
insert into tab values (current date, '00184605', current timestamp) !
insert into tab values (current date, current time,
        '19890124184605476321') !
insert into tab values('01/24/1989','6:46 pm','1989-01-24-18.46.00.000000')!
declare c cursor for select * from tab !
fetch c into :a, :b, :c !
close c !
set format eur !
insert into tab values ('19890124', current time, current timestamp) !
insert into tab values (current date, '00184605', current timestamp) !
insert into tab values (current date, current time,
        '19890124184605476321') !
insert into tab values('25.01.1989','18.46.05','1989-01-25-18.46.05.476321')!
declare c cursor for select * from tab !
fetch c into :a, :b, :c  !
close c !
set format jis !
insert into tab values ('19890124', current time, current timestamp) !
insert into tab values (current date, '00184605', current timestamp) !
insert into tab values (current date, current time,
        '19890124184605476321') !
insert into tab values('1989-01-26','18:46:05','1989-01-26-18.46.05.476321')!
declare c cursor for select * from tab !
fetch c into :a, :b, :c  !
close c !
commit work!
 **** CHAR **** !
declare c cursor for select char(a,iso), char(a,usa),
       char(a,eur), char(a,jis),
       char(a, internal), a from tab !
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
declare c cursor for select char(b,iso), char(b,usa),
       char(b,eur), char(b,jis),
       char(b, internal), b from tab !
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
declare c cursor for select char(c,iso), char(c,usa),
       char(c,eur), char(c,jis),
       char(c, internal), c from tab !
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
set format iso !
declare c cursor for select char(a), a, char(b), b,
       char(c), c from tab!
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
declare c cursor for select * from tab where char(b) =
           any ( select char(b,eur) from tab
         where c < timestamp) !
fetch c into :a, :b, :c  !
close c !
declare c cursor for select * from tab where b =
           any ( select char(b,iso) from tab)
         order by c desc !
close c !
declare c cursor for select * from tab where b =
           any ( select char(b,internal) from tab)
         order by c desc !
fetch c into :a, :b, :c  !
close c !
commit release !
file connect ( kern test sqlmode internal !
set format iso !
 **** SUBQUERY !
create table t1 ( a date) !
insert into t1 values('1989-01-24')!
declare c cursor for select a, b from tab where a =
 ( select a from t1 ) !
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
declare c cursor for select a, b from tab where a =
 ( select * from t1 ) !
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
declare c cursor for select a, b from tab where a =
 ( select a from t1 where tab.a = t1.a ) !
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
declare c cursor for select a, b from tab where a =
 ( select * from t1 where tab.a = t1.a ) !
fetch c into :a, :b, :c, :d, :e, :f  !
close c !
rollback !
select char(adddate(a,10), internal) from tab!
fetch !
select char('12341212', internal) from tab!
select char(timediff('00:00:00','12:14:12'),usa) from tab!
fetch !
select char(timediff('24:00:00','12:14:12'),usa) from tab!
commit release !
file connect ( kern test sqlmode db2 !
set format iso !
declare c cursor for select char(c - 1 microseconds, eur) from tab !
fetch c into :a !
close c !
declare c cursor for select * from tab where c = timestamp(a,b) order by a !
fetch c into :a, :b, :c !
close c !
 **** DATE TIME TIMESTAMP VALUES WITHIN A SELECT STATEMENT **** !
set format iso !
declare c cursor for select * from tab
         where a =
         substr(a,1,4) ||'-'|| substr(a,5,2) ||'-'|| substr(a,7,2) !
close c !
declare c cursor for select * from tab
         where a =
         substr(a,1,4) ||'-'|| substr(a,6,2) ||'-'|| substr(a,9,2) !
fetch c into :a, :b, :c  !
close c !
declare c cursor for select * from tab
         where a =
         date(substr(a,1,4) ||'-'|| substr(a,6,2) ||'-'|| substr(a,9,2)) !
fetch c into :a, :b, :c  !
close c !
declare c cursor for select * from tab where b = '18:46:05' !
close c !
declare c cursor for select * from tab where b = time('18:46:05') !
fetch c into :a, :b, :c  !
close c !
declare c cursor for select * from tab where c = '1989-01-24 18:46:05' !
close c !
declare c cursor for select * from tab
    where c = timestamp('1989-01-24 18:46:00') !
fetch c into :a, :b, :c  !
close c !
commit work !
 **** TIME IN USA FORMAT **** !
set format usa !
create table tab1 ( a time ) !
insert into tab1 values ( '12 am') !
insert into tab1 values ( '12:00 am') !
insert into tab1 values ( '12:34 am') !
insert into tab1 values ( '12:65 am') !
insert into tab1 values ( '12:234 am') !
insert into tab1 values ( '0:00 am') !
insert into tab1 values ( '0:23 am') !
insert into tab1 values ( '6 am') !
insert into tab1 values ( '6:12 am') !
insert into tab1 values ( '11:12 am') !
insert into tab1 values ( '11:12 pm') !
insert into tab1 values ( '12:12 pm') !
insert into tab1 values ( '0 pm') !
insert into tab1 values ( '0:12 pm') !
insert into tab1 values ( '12 pm') !
insert into tab1 values ( '11:59 pm') !
insert into tab1 values ( '13 pm') !
insert into tab1 values ( '13 am') !
declare c cursor for select * from tab1 !
fetch c into :a !
close c !
set format internal !
declare c cursor for select * from tab1 !
fetch c into :a !
close c !
set format usa !
declare c cursor for select * from tab1 where a > '6 AM'!
close c !
declare c cursor for select * from tab1 where a > time('6 AM')!
fetch c into :a !
close c !
declare c cursor for select * from tab1 where a < time('24.00.00')!
close c !
declare c cursor for select * from tab1 where a < time('12 AM')!
fetch c into :a !
close c !
drop table tab1 !
commit release !
file connect ( kern test !
diagnose share parse drop !
create table t (a char (8) key, d date)!
insert t values ('aaa', '19930313')!
insert t values ('bbb', '19921224')!
create table t1 like t !
declare erg cursor for 
select * from t for reuse !
insert t1 select * from erg !
select * from t1 !
fetch !
delete t1 !
diagnose share parse drop !
insert t1 select * from t !
select * from t1 !
fetch !
delete t1 !
diagnose share parse drop !
close erg !
set format iso !
declare erg cursor for 
select * from t for reuse !
insert t1 select a||'ex', adddate (d, 3) from erg!
insert t1 select * from erg !
select * from t1 !
fetch !
delete t1 !
diagnose share parse drop !
insert t1 select a||'ex', adddate (d, 3) from t!
insert t1 select * from t !
select * from t1 !
fetch !
delete t1 !
diagnose share parse drop !
close erg !
rollback release !
