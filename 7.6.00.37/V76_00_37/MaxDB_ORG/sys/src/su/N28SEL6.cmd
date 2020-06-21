*   *ID* N28SEL6  DBCMD    changed on 1992-09-28-13.25.55 by ADMIN     *
file connect ( kern test !
create table t ( u fixed(5), v fixed(5), w fixed(5) ) !
insert t values ( 3, 6, null ) !
select * from t where u>v and u=v !
select * from t where u>v and u=w !
select * from t where u>v and u<v !
select * from t where u=w and v<w !
select * from t where u=w and u<v !
select * from t where u<v and v=v !
select * from t where u>v or  u=v !
select * from t where u>v or  u=w !
select * from t where u>v or  u<v !
select * from t where u=w or  v<w !
select * from t where u=w or  u<v !
select * from t where u<v or  v=v !
select * from t where not ( u>v and u=v ) !
select * from t where not ( u>v and u=w ) !
select * from t where not ( u>v and u<v ) !
select * from t where not ( u=w and v<w ) !
select * from t where not ( u=w and u<v ) !
select * from t where not ( u<v and v=v ) !
select * from t where not ( u>v or  u=v ) !
select * from t where not ( u>v or  u=w ) !
select * from t where not ( u>v or  u<v ) !
select * from t where not ( u=w or  v<w ) !
select * from t where not ( u=w or  u<v ) !
select * from t where not ( u<v or  v=v ) !
select * from t where not ( u=w and u>v ) or u=v !
select * from t where not ( u=w and u>v ) or u<v !
select * from t where not ( u=w and u>v ) or u<w !
select * from t where not ( u=w or  u>v ) or u=v !
select * from t where not ( u=w or  u>v ) or u<v !
select * from t where not ( u=w or  u>v ) or u<w !
select * from t where not ( u=w and u>v ) and u=v !
select * from t where not ( u=w and u>v ) and u<v !
select * from t where not ( u=w and u>v ) and u<w !
select * from t where not ( u=w or  u>v ) and u=v !
select * from t where not ( u=w or  u>v ) and u<v !
select * from t where not ( u=w or  u>v ) and u<w !
commit work release !
