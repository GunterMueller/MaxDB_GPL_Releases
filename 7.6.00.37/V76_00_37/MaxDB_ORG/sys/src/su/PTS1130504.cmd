connect kern test !

create table kitest ( k1 int, k2 int, k3 int, primary key (k1, k2, k3))!

insert into kitest values( 1, 2,  1 )!
insert into kitest values( 1, 2,  2 )!
insert into kitest values( 1, 2,  3 )!
insert into kitest values( 1, 2,  4 )!
insert into kitest values( 1, 2,  5 )!
insert into kitest values( 1, 2,  6 )!
insert into kitest values( 1, 2,  7 )!
insert into kitest values( 1, 2,  8 )!
insert into kitest values( 1, 2,  9 )!
insert into kitest values( 1, 2, 10 )!
insert into kitest values( 1, 2, 11 )!
insert into kitest values( 1, 2, 12 )!

select /*+L2_range_strategy*/ * from kitest where 
k1 = 1 and k2 = 2 and 
( k3 in ( 1, 2, 3)
or k3 in ( 10, 11, 12)
or k3 in ( 5, 6, 7)
)!
fetch!
commit work release !
