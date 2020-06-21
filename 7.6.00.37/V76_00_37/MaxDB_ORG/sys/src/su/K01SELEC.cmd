*   *ID* K01SELEC DBCMD    changed on 1992-09-28-13.24.23 by ADMIN     *
 ***************************
 ***   K 0 1 S E L E C   ***
 ***************************!
declare result cursor for
select keycol, a02, a10, a15, a20, a25, a30, a35,
     a40, a45, a50, a55, a60, a65, a70, a84, a85, a86, a87
     from hypertab!
fetch result!
close   result!
