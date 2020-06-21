*   *ID* DMLPRIV0 DBCMD    changed on 1992-09-28-13.23.22 by ADMIN     *
file connect ( sut sut !
create user d0 password w dba !
create user d1 password w !
create user d2 password w !
create user d3 password w !
create user d4 password w !
create user d5 password w !
create user d6 password w !
create user d7 password w !
create user d8 password w !
create user d9 password w !
commit release !
file connect ( kern test !
create table t1 ( t1_c1 char(10)          key ,
                         t1_c2 fixed(5)   key ,
                         t1_c3 fixed(5,2) not null ) !
grant all on t1 to d0 !
grant select on t1 to d1 !
grant select (t1_c1) on t1 to d2 !
grant select (t1_c1,t1_c2) on t1 to d3 !
grant select (t1_c3) on t1 to d4 !
grant insert on t1 to d5 !
grant update on t1 to d6 !
grant update (t1_c3) on t1 to d7 !
grant delete on t1 to d8 !
grant select (t1_c2) on t1 to d9 !
grant update (t1_c3) on t1 to d9 !
insert into t1 values ( 'd0' , 1 , 1.0 ) !
insert into t1 values ( 'd0' , 2 , 1.0 ) !
insert into t1 values ( 'd0' , 3 , 1.0 ) !
insert into t1 values ( 'd0' , 4 , 1.0 ) !
insert into t1 values ( 'd0' , 5 , 1.0 ) !
insert into t1 values ( 'd0' , 6 , 1.0 ) !
commit work release !
