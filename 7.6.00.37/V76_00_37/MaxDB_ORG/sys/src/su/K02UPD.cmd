*   *ID* K02UPD   DBCMD    changed on 1992-09-28-13.24.25 by ADMIN     *
 ***************************
 ***   K 0 2 U P D       ***
 ***************************!
lock table tab in exclusive mode!
insert into tab set keycol = 'ins-ins', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'ins-del', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
insert into tab set keycol = 'ins-upd', col = 'aa',
    col2 = 'AA', varcol = 'bbbbb', varcol2 = 'BBBBB',
    inv = 'd', inv2 = 'D', varinv = 'eeee', varinv2 = 'EEEE'!
delete from tab KEY keycol = 'del-ins'!
delete from tab KEY keycol = 'del-del'!
delete from tab KEY keycol = 'del-upd'!
update of tab set
    col = 'xx', optcol = 'neu',
    varcol = 'y234567890y', inv = 'zz',
    varinv = 'xyzxyz', optinv = 'xyz'
    KEY keycol = 'upd-ins'!
update of tab set
    col = 'xx', optcol = 'neu',
    varcol = 'y234567890y', inv = 'zz',
    varinv = 'xyzxyz', optinv = 'xyz'
    KEY keycol = 'upd-del'!
update of tab set
    col = 'xx', optcol = 'neu',
    varcol = 'y234567890y', inv = 'zz',
    varinv = 'xyzxyz', optinv = 'xyz'
    KEY keycol = 'upd-upd-1'!
update of tab set
    col = 'xx', optcol = 'neu',
    varcol = 'y234567890y', inv = 'zz',
    varinv = 'xyzxyz', optinv = 'xyz'
    KEY keycol = 'upd-upd-2'!
update of tab set
    col = 'xx', col2 = 'XX',
    optcol = 'neu', optcol2 = 'NEU',
    varcol = 'y234567890y', varcol2 = 'Y23456789Y',
    inv = 'zz', inv2 = 'ZZ',
    varinv = 'xyzxyz', varinv2 = 'XYZXYZ',
    optinv = 'xyz', optinv2 = 'XYZ'
    KEY keycol = 'upd-upd-3'!
declare result cursor for
select keycol,
       col, col2, varcol, varcol2 from tab!
fetch result!
close       result!
declare result cursor for
select keycol,
       inv, inv2, varinv, varinv2 from tab!
fetch result!
close       result!
declare result cursor for
select keycol,
       optcol, optcol2, optinv, optinv2 from tab!
fetch result!
close       result!
insert into tab set keycol = 'none-ins', col = 'n',
    col2 = 'AA', varcol = 'new-col',
    varcol2 = 'BBBBB', optcol2 = 'neu', inv = 'n',
    inv2 = 'D', varinv = 'inserted',
    varinv2 = 'EEEE', optinv2 = 'new'!
insert into tab set keycol = 'ins-ins', col = 'n',
    col2 = 'AA', varcol = 'new-col',
    varcol2 = 'BBBBB', optcol2 = 'neu', inv = 'n',
    inv2 = 'D', varinv = 'inserted',
    varinv2 = 'EEEE', optinv2 = 'new'!
insert into tab set keycol = 'del-ins', col = 'n',
    col2 = 'AA', varcol = 'new-col',
    varcol2 = 'BBBBB', optcol2 = 'neu', inv = 'n',
    inv2 = 'D', varinv = 'inserted',
    varinv2 = 'EEEE', optinv2 = 'new'!
insert into tab set keycol = 'upd-ins', col = 'n',
    col2 = 'AA', varcol = 'new-col',
    varcol2 = 'BBBBB', optcol2 = 'neu', inv = 'n',
    inv2 = 'D', varinv = 'inserted',
    varinv2 = 'EEEE', optinv2 = 'new'!
delete from tab  KEY keycol = 'none-del'!
delete from tab  KEY keycol = 'ins-del'!
delete from tab  KEY keycol = 'del-del'!
delete from tab  KEY keycol = 'upd-del'!
update of tab set
    optcol = 'm', varcol = 'mmmm',
    varinv = 'n', optinv = 'nn'
    KEY keycol = 'none-upd'!
update of tab set
    optcol = 'm', varcol = 'mmmm',
    varinv = 'n', optinv = 'nn'
    KEY keycol = 'ins-upd'!
update of tab set
    optcol = 'm', varcol = 'mmmm',
    varinv = 'n', optinv = 'nn'
    KEY keycol = 'del-upd'!
update of tab set
    optcol = 'm', varcol = 'mmmm',
    varinv = 'n', optinv = 'nn'
    KEY keycol = 'upd-upd-1'!
update of tab set
    col2 = 'u', varcol2 = 'updated',
    optcol2 = 'upd', inv2 = 'up',
    varinv2 = 'upd', optinv2 = 'new'
    KEY keycol = 'upd-upd-2'!
update of tab set
    col = 'g', col2 = 'GG',
    varcol = 'hh', varcol2 = 'HH',
    optcol = 'iii', optcol2 = 'III',
    inv = 'j', inv2 = 'J',
    varinv = 'kk', varinv2 = 'KK',
    optinv = 'lll', optinv2 = 'LLL'
    KEY keycol = 'upd-upd-3'!
declare result cursor for
select keycol,
       col, col2, varcol, varcol2 from tab!
fetch result!
close       result!
declare result cursor for
select keycol,
       inv, inv2, varinv, varinv2 from tab!
fetch result!
close       result!
declare result cursor for
select keycol,
       optcol, optcol2, optinv, optinv2 from tab!
fetch result!
close       result!
commit work !
