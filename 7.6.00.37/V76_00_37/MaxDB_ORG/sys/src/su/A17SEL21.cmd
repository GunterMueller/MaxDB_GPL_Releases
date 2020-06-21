file connect ( kern test !
create table a (k1 fixed (6) key, f1 fixed (6) key, a1 char (10))!
insert a values (50000, 0, 'SATZ 0')!
insert a values (50001, 1, 'SATZ 1')!
insert a values (50002, 2, 'SATZ 2')!
insert a values (50003, 3, 'SATZ 3')!
insert a values (50004, 4, 'SATZ 4')!
insert a values (50005, 5, 'SATZ 5')!
insert a values (50006, 6, 'SATZ 6')!
commit!
explain select * from a where (k1=50001 or k1=50002) and
(f1=1 or f1=2)!
select * from a where (k1=50001 or k1=50002) and
(f1=1 or f1=2)!
fetch!
explain select * from a where ((k1=50001 or k1=50002)
and (f1=1 or f1=2)) or (k1=50004) or (k1=50005)!
select * from a where ((k1=50001 or k1=50002)
and (f1=1 or f1=2)) or (k1=50004) or (k1=50005)!
fetch!
explain select * from a where (k1=50004) or (k1=50005) or
((k1=50001 or k1=50002) and (f1=1 or f1=2)) !
select * from a where (k1=50004) or (k1=50005) or
((k1=50001 or k1=50002) and (f1=1 or f1=2)) !
fetch!
drop table a!
commit release!
