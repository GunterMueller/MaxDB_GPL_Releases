*   *ID* A05USER1 DBCMD    changed on 1992-09-28-13.22.27 by ADMIN     *
 ***************************
 ***   A 0 5 U S E R 1   ***
 ***************************!
file connect ( freier w !
create table  p5
       (f1 char (12) key, f2 char (4), f3 fixed (6,2)) !
create table  freier1
       (f1 char (12), f2 char (4), f3 fixed (6,2)) !
grant ALL on freier1 to publik !
create table  freier2
      (f1 char (12), f2 char (4), f3 fixed (6,2)) !
grant ALL on freier2 to publik !
alter table publik.p2 add  s1 char(3), s2 char (12)  !
create user  fred password w !
create index  "publik.p1.f1"
	on publik.p1( f1 ) !
create index  "publik.p2.f1"
	on publik.p2( f1 ) !
commit  work !
file sel_tab_owner ( PUBLIK !
file sel_user !
file sel_priv_on !
file sel_priv_granted ( FREIER !
file sel_priv_granted ( MUNZ !
file sel_priv_granted ( LAMB !
file sel_priv_granted ( FRED !
lock ( nowait ) table publik.p2, publik.p1  in exclusive mode !
insert into publik.p2 set f1 = 'aa' !
insert      publik.p1 set f1 = 'aaa' !
insert into publik.t1 set f1 = 'aa' !
insert into publik.p2 set f1 = 'bb' !
insert  publik.p3  values ('fr', 'satz', 10 ) !
insert  publik.p1  values ('freier' ) !
insert  publik.p2  values ('freier' ,'f2', 13, 's1', 's2' ) !
insert  publik.p2  values ('freier' ,'f2' ) !
delete from publik.p1  key  f1 = 'aa' !
delete from publik.p2  key  f1 = 'aa' !
delete from publik.t1  where (  f1 = 'aa')  !
commit work !
declare ergp1 cursor for 
select f1 from publik.p1 !
fetch  ergp1 !
declare ergp2 cursor for 
select f1, f2, f3, s1, s2  from publik.p2 !
fetch  ergp2 !
close   ergp2 !
alter table  publik.p2 drop f3 !
commit work !
declare ergp2 cursor for 
select f1, f2, f3, s1, s2  from publik.p2 !
fetch  ergp2 !
close   ergp2 !
declare ergp2 cursor for 
select f1, f2, s1, s2  from publik.p2 !
fetch  ergp2 !
close   ergp2 !
insert publik.p2 values ('drop', 'df2', 'ds1', 'ds2' ) !
commit work !
declare ergp2 cursor for 
select f1, f2, s1, s2  from publik.p2 !
fetch  ergp2 !
close   ergp2 !
declare ergt1 cursor for 
select f1, f2, f3  from publik.t1 !
fetch  ergt1 !
declare ergp3 cursor for 
select f1, f2, f3 from publik.p3 !
fetch  ergp3 !
commit work release !
file connect ( munz rudi !
lock (nowait) table publik.p2 in exclusive mode !
insert into publik.p2 values ('munz', 'sat1' )!
insert into publik.p2 values ('munz', 'satz', 's1', 's2')!
insert into publik.p2 values ('munz', 'satz', 10, 's1', 's2')!
declare ergp2 cursor for 
select f1, f2,  s1, s2  from publik.p2 !
fetch  ergp2 !
commit work release !
file connect ( publik pw !
declare ergp2 cursor for 
select f1, f2, s1, s2  from publik.p2 !
fetch  ergp2 !
commit work release !
