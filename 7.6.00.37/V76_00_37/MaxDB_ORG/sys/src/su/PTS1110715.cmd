connect kern test !

create table Tab1 ( k00  int, k01 int) !
create table Tab2 ( k00  int, k01 int) !

insert into Tab1 values ( 1, 1 )!
insert into Tab1 values ( 2, 2 )!
insert into Tab2 values ( 1, 1 )!
insert into Tab2 values ( 2, 2 )!

* earlier error was kb-stack type illegal 
select * from Tab1, Tab2 
where not(Tab1.k01 = 1 and Tab2.k00 = 1) !

commit work release !
