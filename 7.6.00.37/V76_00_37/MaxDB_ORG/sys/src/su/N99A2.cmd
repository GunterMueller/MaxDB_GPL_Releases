*   *ID* N99A2    DBCMD    changed on 1992-09-28-13.27.15 by ADMIN     *
file connect ( sut sut !
create user elke password os resource!
commit release !
file connect ( kern test !
create table x (x1 char(1) key, x2 char (2))!
insert x values ('a', 'b')!
grant select on x to elke !
commit release !
file connect ( elke os !
create table a (a1 char (1) key, a2 char(1), a3 char (3))!
insert a values ('a', 'd', 'e')!
create view m (a1, a2, a3, x1, x2) as select
   a1, a2, a3, x1, x2 from a, kern.x
   where a1 = x1 !
file sel_cols_tab ( M !
file sel_cols_1tab ( KERN X !
file sel_cols_tab ( A !
update m set a2 = 'q' key a1 = 'a', x1 = 'a'!
rollback work release !
