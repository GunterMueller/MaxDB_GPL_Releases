*   *ID* CORR2    DBCMD    changed on 1992-09-28-13.23.00 by ADMIN     *
file connect ( kern test !
create table t2 ( spalte1 char(245) ) !
create table t3 ( spalte1 char(245) ) !
insert t2 values ( 'a' ) !
insert t3 values ( 'a' ) !
select * from t2 where spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
               and exists ( select * from t3 where
                                spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
               and exists ( select * from t3 where
                                spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
               and exists ( select * from t3 where
                                spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
               and exists ( select * from t3 where
                                spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
               and exists ( select * from t3 where
                                spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
               and exists ( select * from t3 where
                                spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
                            and spalte1 = substr ( 'a' , 1 )
               and exists ( select * from t3 where
                                spalte1 = substr ( 'a' , 1 ) ))))))) !
drop table t2 !
drop table t3 !
commit work release !
