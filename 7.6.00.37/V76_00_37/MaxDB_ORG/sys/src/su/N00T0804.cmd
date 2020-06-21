*   *ID* N00T0804 DBCMD    changed on 1992-09-28-13.24.55 by ADMIN     *
 *** 8.5   <rename column statement> !
file connect ( kern test !
create table t1 ( spalte1 char(10) key, spalte2 fixed(5) ) !
create view v1xt1 as select spalte1 from t1 with check option !
create view v1xv1xt1 as select v1xt1.spalte1, ref.spalte2
                        from v1xt1, t1 ref
                        where v1xt1.spalte1 > 'J'
                          and ref.spalte2 > 5 !
create index "t1.spalte2"
	on t1( spalte2 desc ) !
create index index1 on t1 ( spalte2, spalte1 desc ) !
commit work !
rename column t1.spalte1 to x !
rename column t1.x to col1 !
rename column t1.spalte2 to col2 !
file sel_cols_tab ( T1 !
declare sel_col cursor for 
 select owner,tablename,columnname,
        mode as "MOD",
        datatype,
        codetype as "CODE",
        len,"DEC",columnprivileges,"DEFAULT"
   from domain.columns
  where owner = USER
    and columnname = 'COL1' !
fetch sel_col into :a,:b,:c,:d,:e,:f,:g,:h,:i,:j !
close sel_col !
file sel_view ( V1XT1 !
file sel_view ( V1XV1XT1 !
insert t1 set col1 = 'Monika', col2 = 6 !
insert v1xt1 set col1 = 'Karin' !
select col1, sum ( col2 ) from v1xv1xt1 where 2* col2 = 12
   group by col1 having count ( col1 ) = 1 !
fetch !
commit work !
drop table t1 !
commit work release !
