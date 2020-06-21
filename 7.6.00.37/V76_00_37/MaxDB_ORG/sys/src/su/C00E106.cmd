*   *ID* C00E106  DBCMD    changed on 1992-09-28-13.23.04 by ADMIN     *
*
*  create link ist auch bei nicht identischen definitionen moeglich
*
file cleark !
file connect ( kern test !
create table person   (pnr fixed(3) key, pname char(20) )!
create table mitglied (gnr fixed(4) key, pnr fixed(4) key )!
create table gruppe   (gnr fixed(5) key, gname char(20) )!
insert person values (1, 'pino') !
insert mitglied values (101, 1) !
insert gruppe values (101, 'O.v.W.') !
commit work!
alter table mitglied foreign key p_mitglied (pnr) references person !
alter table mitglied foreign key g_mitglied (gnr) references gruppe !
commit   release!
