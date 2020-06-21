file connect ( kern test !
diagnose vtrace default insert on!
insert into tab1 values (1, 'abc')!
insert into tab1 values (2, 'abc')!
insert into tab1 values (3, 'def')!
insert into tab1 values (4, 'def')!
insert into tab1 values (5, 'qwe')!
insert into tab2 values (0, 'abc')!
insert into tab2 values (1, 'abc')!
insert into tab2 values (2, 'def')!
insert into tab2 values (3, 'def')!
insert into tab2 values (4, 'hij')!
insert into tab2 values (6, 'hij')!
insert into tab3 values (1, 'def')!
insert into tab3 values (2, 'hij')!
insert into tab3 values (3, 'abc')!
insert into tab3 values (4, 'fed')!
insert into tab3 values (8, 'fed')!
commit work!
select * from tab1!
fetch !
diagnose vtrace default insert off!
select * from tab2!
fetch !
select * from tab3!
fetch !
declare erg cursor for select * from tab1, tab2 where a (+) = c !
fetch erg !
select a, b, c, d, e, f from erg, tab3 !
fetch !
declare erg cursor for select * from tab2, tab3 order by 3,1 !
fetch erg !
select a, b, c, d, e, f from erg, tab1 where a (+) = c !
fetch !
declare erg cursor for select * from tab1, tab2 where a (+) = c !
fetch erg !
select a, b, c, d, e, f from erg, tab3 where c = e !
fetch !
select a, b, c, d, e, f from tab1, tab2, tab3 where a (+) = c AND c = e !
fetch !
declare erg cursor for select * from tab1, tab2 where a (+) = c !
fetch erg !
select a, b, c, d, e, f from erg, tab3 !
fetch !
select a, b, c, d, e, f from <tab1, tab2, tab3> where a (+) = c !
fetch !
select a, b, c, d, e, f from <tab1, tab3, tab2> where a (+) = c !
fetch !
select a, b, c, d, e, f from <tab2, tab1, tab3> where a (+) = c !
fetch !
select a, b, c, d, e, f from <tab2, tab3, tab1> where a (+) = c !
fetch !
select a, b, c, d, e, f from <tab3, tab1, tab2> where a (+) = c !
fetch !
select a, b, c, d, e, f from <tab3, tab2, tab1> where a (+) = c !
fetch !
declare erg cursor for select * from tab2, tab3 where c = e !
fetch erg !
select a, b, c, d, e, f from tab1, erg where a (+) = c !
fetch !
drop table tab1 !
drop table tab2 !
drop table tab3 !
commit work release !
