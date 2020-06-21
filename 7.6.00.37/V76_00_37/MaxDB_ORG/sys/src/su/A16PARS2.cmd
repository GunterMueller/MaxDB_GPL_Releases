*   *ID* A16PARS2 DBCMD    changed on 1992-09-28-13.22.37 by ADMIN     *
file CONNECT ( KERN TEST !
create table tab1 ( spalte1 char(5) key,
                    spalte2 char(5),
                    spalte3 char(5),
                    spalte4 fixed(10),
                    spalte5 fixed(10) ) !
create table tab2 ( col1 char(5) key,
                    col2 char(5),
                    col3 char(5),
                    col4 fixed(10),
                    col5 fixed(10) ) !
create index "tab2.col2"
	on tab2( col2 ) !
pars_then_ex !
data !
select col2, col4, spalte4 from tab2, tab1
    where col2 between :a and :b and col4 = spalte4!
c 5 '3' c 5 '4'

nodata !
pars_execute !
rollback release !
