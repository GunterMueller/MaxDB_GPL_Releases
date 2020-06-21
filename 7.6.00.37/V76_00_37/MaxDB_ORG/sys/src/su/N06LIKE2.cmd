*   *ID* N06LIKE2 DBCMD    changed on 1992-09-28-13.25.08 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Wahre 'LIKE' mit '?' und '_' , keine '*' und '%'. !
create table kunde  ( name char(10) key ) !
insert into kunde set name = 'GIAG' !
commit work !
select name from kunde where name like '____'  !
fetch !
select name from kunde where name like '_IAG'  !
fetch !
select name from kunde where name like '__AG'  !
fetch !
select name from kunde where name like '_I_G'  !
fetch !
select name from kunde where name like 'GIA_'  !
fetch !
select name from kunde where name like '_I__'  !
fetch !
select name from kunde where name like '_IA_'  !
fetch !
select name from kunde where name like 'GI__'  !
fetch !
select name from kunde where name like 'G__G'  !
fetch !
