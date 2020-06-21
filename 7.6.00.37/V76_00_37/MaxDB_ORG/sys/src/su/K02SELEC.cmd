*   *ID* K02SELEC DBCMD    changed on 1992-09-28-13.24.25 by ADMIN     *
 ***************************
 ***   K 0 2 S E L E C   ***
 ***************************!
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
