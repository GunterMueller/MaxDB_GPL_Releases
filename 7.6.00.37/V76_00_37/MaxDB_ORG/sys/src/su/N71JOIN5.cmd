*   *ID* N71JOIN5 DBCMD    changed on 1992-09-28-13.27.09 by ADMIN     *
file CONNECT ( KERN TEST !
* select .. where t.f between t1.f1 and t1.f2 !
create table t ( nr fixed (8) key, no1 char (7) key , birthdate date)!
insert t values (1, 'a', '19560708')!
insert t values (1, 'b', '19751212')!
create table t1 ( nr fixed (8) key, no1 char(7) key,
  low_date date, high_date date)!
insert t1 values (1, 'a', '19600102', '19890315')!
select birthdate, low_date, high_date from t, t1
      where (birthdate between low_date and high_date) and
             t.nr = 1 and t1.nr = 1!
fetch !
drop table t !
drop table t1 !
commit release !
