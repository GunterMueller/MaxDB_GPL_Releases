*   *ID* ORACLE1  DBCMD    changed on 1992-10-02-14.23.52 by KERN      *
file connect ( kern test sqlmode oracle !
create table tab1 (s1 integer, s2 integer) !
insert into tab1 values (1,1) !
insert into tab1 values (1,2) !
insert into tab1 values (1,3) !
insert into tab1 values (1,4) !
insert into tab1 values (1,5) !
insert into tab1 values (1,6) !
insert into tab1 values (1,7) !
create table tab2 as select * from tab1 !
pars_then_ex !
declare c cursor for select * from tab1 union select * from tab2 !
mfetch c into :a, :b !
1
mfetch c into :a, :b !
2
mfetch c into :a, :b !
4
declare c cursor for select * from tab1 x where s1 in
    (select s1 from tab2 where s1 = x.s1) !
mfetch c into :a, :b !
10
declare c cursor for select * from tab1 !
mfetch c into :a, :b !
2
fetch c into :a, :b !
pars_execute !
declare d cursor for select * from tab1 where s2 = any 1*1,2+3,4*2 !
declare d cursor for select * from tab1 where s2 = any (1*1,2+3,4*2) !
fetch d into :a, :b !
declare e cursor for select * from tab1
     where s2 = any (select s2 from tab2) !
fetch e into :a, :b !
create table tab3 (a char (8), b decimal(3), c char (20), 
                   d decimal (5), e char (40))  !
insert into tab3 values ('aaa', 3, 'cccccccccc', 5,
                        'eeeeeeeeeeeeeeeeeeeeeeee')!        
update tab3 set a = 'neu1', b,c = (30, 'rrr'),                
       d = (select 12345 from dual) !
select * into :a, :b, :c, :d, :e from tab3 !
update tab3 set e,c,a = ('neues-e', 'neu-c', 'neu-a')!
select * into :a, :b, :c, :d, :e from tab3 !
update tab3 set e = (select 'abcdefghijklmn' from dual),
                d = (select 1 from dual) !
select * into :a, :b, :c, :d, :e from tab3 !
commit work!
insert into tab1 values ( uid,3)!
declare c cursor for select *,uid from tab1 !
fetch c into :a, :b, :c!
close c !
declare c cursor for select s1 from tab1 where s1 = uid !
fetch c into :a!
close c !
create view tab4 as select * from tab1 !
declare c cursor for select *,uid from tab4 !
fetch c into :a, :b, :c!
close c !
declare c cursor for select s1 from tab4 where s1 = uid !
fetch c into :a!
close c!
* PTS 1141225-7 !
create table t1 ( a int constraint ta_c check (a < uid))!
drop table t1 !
rollback work release !
