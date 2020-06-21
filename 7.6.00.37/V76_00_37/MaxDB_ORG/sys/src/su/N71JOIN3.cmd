*   *ID* N71JOIN3 DBCMD    changed on 1992-09-28-13.27.08 by ADMIN     *
file CONNECT ( KERN TEST !
create table t1 ( t1xc1 fixed(5), t1xc2 fixed(5), t1xc3 fixed(5) ) !
create table t2 ( t2xc1 fixed(5), t2xc2 fixed(5), t2xc3 fixed(5) ) !
insert t1 values ( 2,2,2 ) !
insert t1 values ( 4,4,4 ) !
insert t1 values ( 2,2,2 ) !
insert t1 values ( 3,3,3 ) !
insert t1 values ( 2,2,2 ) !
insert t2 values ( 2,2,2 ) !
insert t2 values ( 4,4,4 ) !
insert t2 values ( 2,2,2 ) !
insert t2 values ( 3,3,3 ) !
insert t2 values ( 2,2,2 ) !
select sum ( t1xc1 ) from t1, t2
  where t1xc1 = t1xc2
    and t1xc2 > 1
    and ( t1xc2 < 7
     or ( t1xc3 < t2xc3
    and t1xc2 + t1xc1 > 0 )
     or t1xc3 between 2 and 4 ) !
select sum ( t1xc1 ) from t1, t2
   where t1xc1 = t1xc2
     and ( t1xc1 < 7
     and ( t1xc1 < 7
     and ( t1xc1 < 7
     and ( t1xc1 < 7
     and ( t1xc1 < 7
     and ( t1xc1 < 7
     and ( t1xc1 < 7
      or ( t1xc1 < 7 )))))))) !
select sum ( t1xc2 ) from t1, t2
   where t1xc1 = t2xc1
     and t1xc2 < 7
     and ( t1xc2 < 7
      or ((( t1xc3 < 7
     and t1xc3 < 7 )
      or t1xc1 < 7 )
     and t1xc3 < 7 )
      or t1xc1 < 7 ) !
select sum ( t1xc2 ) from t1, t2
   where t1xc1 = t2xc1
     and t1xc2 < 7
     and ( not ( t1xc2 < 1
      or ((( t1xc3 < 7
     and t1xc3 < 7 ))
      or ( not ( t1xc1 < 1 ))
     and t1xc3 < 7 )
      or t1xc1 < 7 )) !
rollback work release !
