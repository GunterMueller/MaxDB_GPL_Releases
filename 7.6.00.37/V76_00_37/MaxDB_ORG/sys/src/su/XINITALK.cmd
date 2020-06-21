*   *ID* XINITALK DBCMD    changed on 1992-09-28-13.21.58 by ADMIN     *
*
*               sysdba syspwd logsize logmode dev1size code
*
file xinitall ( &1 &2 &3 &4 &5 &6 !
connect &1 identified by &2 !
create user kern password test dba not exclusive !
commit work release !
