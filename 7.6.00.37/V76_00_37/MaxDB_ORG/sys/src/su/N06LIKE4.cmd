*   *ID* N06LIKE4 DBCMD    changed on 1992-09-28-13.25.09 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Falsche 'LIKE' !
create table kunde    ( name char(10) key ) !
insert into kunde set name = 'GIAG' !
commit work !
select name from kunde where name like ' '        !
fetch !
select name from kunde where name like ' GIAG'    !
select name from kunde where name like '_'        !
select name from kunde where name like '_'        !
select name from kunde where name like 'GI'       !
select name from kunde where name like 'G_G'      !
select name from kunde where name like 'GIAG_'    !
select name from kunde where name like 'GI_AG'    !
select name from kunde where name like '_GI_'     !
select name from kunde where name like 'GI+G'     !
select name from kunde where name like 'GI__G'    !
select name from kunde where substr(name,5) like '_' !
select name from kunde where substr(name,5) like '__' !
* PTS 1388 !
select name from kunde where name like 'GIAGZULANGERWERT%' !
