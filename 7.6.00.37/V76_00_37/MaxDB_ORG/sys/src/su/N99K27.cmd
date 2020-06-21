*   *ID* N99K27   DBCMD    changed on 1992-09-28-13.27.27 by ADMIN     *
file connect ( kern test !
create table tab3 ( tab3_sp char(60) key,
                    tab3_sp2 fixed(4) key ) !
create table tab4 ( tab4_spalte1 fixed(4) ) !
pars_then_ex !
declare erg cursor for
select tab3.tab3_sp, tab3_sp2
    from tab3,tab4
    where tab3.tab3_sp  = 'PROJEKTLEITER'
      and tab3.tab3_sp2 = tab4.tab4_spalte1 !
pars_execute !
rollback work release !
