*   *ID* N99A5    DBCMD    changed on 1992-09-28-13.27.22 by ADMIN     *
file connect ( kern test !
create table tab ( s1 fixed(5,1), s2 float(5), s3 char(5),
  s4 char(7) ascii, s5 char(4) byte,
  s7 date, s8 time ) !
insert tab set s1 = 23.1,
               s2 = 2.31E+1,
               s3 = 'aaa',
               s4 = 'aaa',
               s5 = X'818181',
               s7 = '19880518',
               s8 = '00233914'  !
select * from tab !
fetch !
select * from tab where s1 = s1 !
select * from tab where s1 = s2 !
select * from tab where s2 = s2 !
select * from tab where s3 = s3 !
select * from tab where s3 = s4 !
select * from tab where s4 = s4 !
select * from tab where s3 = s5 !
select * from tab where s4 = s5 !
select * from tab where s5 = s5 !
select * from tab where s3 = s7 !
select * from tab where s4 = s7 !
select * from tab where s5 = s7 !
select * from tab where s7 = s7 !
select * from tab where s3 = s8 !
select * from tab where s4 = s8 !
select * from tab where s7 = s8 !
select * from tab where s8 = s8 !
rollback work release !
