*   *ID* K02INIT  DBCMD    changed on 1992-09-28-13.24.24 by ADMIN     *
 ***************************
 ***   K 0 2 I N I T     ***
 ***************************!
commit work !
create table tab
  (keycol  char (15) key,
          col  char  (2) not null,
       varcol  char (11) not null,
       optcol  char  (3) ,
          col2 char  (2) not null,
       varcol2 char (11) not null,
       optcol2 char  (3) ,
          inv  char  (2) not null,
       varinv  char (11) not null,
       optinv  char  (3) ,
          inv2 char  (2) not null,
       varinv2 char (11) not null,
       optinv2 char  (3) )!
commit work !
create index "tab.inv"
	on tab( inv ) !
commit work !
create index "tab.inv2"
	on tab( inv2 ) !
create index "tab.varinv"
	on tab( varinv ) !
create index "tab.varinv2"
	on tab( varinv2 ) !
create index "tab.optinv"
	on tab( optinv ) !
create index "tab.optinv2"
	on tab( optinv2 ) !
commit work !
lock table tab in exclusive mode!
insert into tab set keycol = 'del-ins', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'del-del', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'del-upd', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'upd-ins', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'upd-del', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'upd-upd-1', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'upd-upd-2', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'upd-upd-3', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
commit work !
