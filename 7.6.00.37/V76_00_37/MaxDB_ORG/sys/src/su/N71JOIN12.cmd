file CONNECT ( KERN TEST !
create table ta (a date,
                  b time,
                  c timestamp )!
commit work !
insert into ta values ( '19990301','00143008', '19990303090000123456') !
insert into ta values ( '19990312','00131313', '19990403090000234567') !
insert into ta values ( '19990229','00235959', '19990503090000456789') !
insert into ta values ( '19990331','00900000', '19990503090000456700') !
commit work !
set format eur !
SELECT * FROM TA, TA TB WHERE ta.a=tb.a and ta.b=tb.b and ta.c=tb.c !
fetch!
SELECT * FROM TA, TA TB WHERE ta.a=tb.a and ta.b=tb.b and ta.c=tb.c 
order by ta.a, tb.a, ta.c, tb.b, tb.c, ta.b  !
fetch!
set format internal !
SELECT * FROM TA, TA TB WHERE ta.a=tb.a and ta.b=tb.b and ta.c=tb.c !
fetch!
SELECT * FROM TA, TA TB WHERE ta.a=tb.a and ta.b=tb.b and ta.c=tb.c 
order by ta.a, tb.a, ta.c, tb.b, tb.c, ta.b  !
fetch!
drop table ta!
commit work release !
* PTS 1632 !
file connect ( kern test !
create table dick (a char (8), a300 char (300))!
insert dick values ('aaa', 'lange Spalte')!
create table zweite ( a2 char (8), a280 char (280))!
insert zweite values ('xxx', 'nicht lang')!
select * from dick where a300 = 'lange Spalte'!
fetch!
create view v as
select * from dick where a300 = 'lange Spalte'!
select * from v !
fetch!
select v.*, zweite.a280 from zweite, v !
fetch !
insert zweite values ('aaa', 'lange Spalte')!
select v.*, zweite.a280 from zweite, v !
fetch !
rollback release !
* PTS 3670 !
file connect ( kern test !
CREATE TABLE tab (ak1  fixed (10) key,
                 ak2  fixed (10) key,
                 a1   fixed (10),
                 a2   fixed (10),
                 a3   fixed (10)) !
select max (t1.a1), 1, 2, '1234567890', sum (t1.a1) 
from <tab t1, tab t2> !
fetch !
select max (t1.a1), 1, 2, '1234567890', sum (t1.a1) 
from <tab t2, tab t1> !
fetch !
rollback release !
