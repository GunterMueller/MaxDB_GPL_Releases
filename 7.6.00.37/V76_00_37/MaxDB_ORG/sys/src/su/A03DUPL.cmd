*   *ID* A03DUPL  DBCMD    changed on 1992-09-28-13.22.18 by ADMIN     *
file connect ( kern test !
create table t ( k fixed (3) key,
                 a char  (8))!
insert t set k = 1, a = 'aaa'!
insert t set k = 1, a = 'bbb' !
insert t set k = 1, a = 'ccc' ignore duplicates !
insert t values (1, 'ddd') update duplicates !
insert t (k, a) values (1, 'eee') reject duplicates !
 * Fehlerprotokoll 3-76 !
create table temp.t ( a fixed (3) key)!
insert temp.t select k from t!
select * from temp.t !
fetch !
drop table temp.t !
drop table t !
commit work release !
