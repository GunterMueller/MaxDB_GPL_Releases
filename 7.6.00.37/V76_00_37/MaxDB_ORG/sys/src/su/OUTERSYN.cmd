*   *ID* OUTERSYN DBCMD    changed on 1992-09-28-13.27.37 by ADMIN     *
file connect ( kern test !
create table t1 ( a char(5) ) !
insert t1 set a = '111ab' !
create table t2 ( b char(5) ) !
insert t2 set b = '111ab' !
select * from t1 where a (+) = '111ab' !
select * from t1 where a (+) like '1%' !
select * from t1 where a (+) is not null !
select * from t1, t2 where a (+) = a and a = b !
select * from t1, t2 where a (+) = b and a (+) like '1%' !
select * from t1, t2 where a = b (+) and a (+) like '1%' !
select * from t1, t2 where a (+) = b and a like '1%' !
fetch !
select * from t1, t2 where a = b and a like '1%' !
fetch !
select * from t1, t2 where a = (+) b and a like '1%' !
fetch !
select * from t1, t2 where a (+) = b and a like '1%' !
fetch !
select * from t1, t2 where b (+) = a and a like '1%' !
fetch !
select * from t1, t2 where a (+) = b and a (+) is not null !
select * from t1, t2 where a = (+) b and a (+) is not null !
select * from t1, t2 where a = b (+) and a (+) is not null !
select * from t1, t2 where a (+) = b and a is not null !
fetch !
select * from t1, t2 where a = b and a is not null !
fetch !
select * from t1, t2 where b (+) = a and a is not null !
fetch !
select * from t1, t2 where a = b (+) and a is not null !
fetch !
commit work release !
file connect ( kern test !
CREATE TABLE BUCH                                     (
"ONR               " CHAR      (1) ASCII    ,
"HNR               " CHAR      (1) ASCII    ,
"BNR               " CHAR      (1) ASCII    ,
"GAST              " CHAR      (20) ASCII    
, PRIMARY KEY ("ONR               "
,"HNR               "
,"BNR               "
)) !
insert buch (onr, hnr, bnr, gast) values ('1' , '1' , '1' , 'Renz') !
insert buch (onr, hnr, bnr, gast) values ('1' , '1' , '2' , 'Berg') !
insert buch (onr, hnr, bnr, gast) values ('2' , '1' , '1' , 'Lutz') !
insert buch (onr, hnr, bnr, gast) values ('2' , '1' , '2' , 'Wind') !
insert buch (onr, hnr, bnr, gast) values ('2' , '1' , '9' , 'Buen') !
CREATE TABLE HOTEL                                    (
"ONR               " CHAR      (1) ASCII    ,
"HNR               " CHAR      (1) ASCII    ,
"HOTEL             " CHAR      (20) ASCII    
, PRIMARY KEY ("ONR               "
,"HNR               "
)) !
insert hotel (onr, hnr, hotel) values ('1' , '1' , 'Kaiser') !              
insert hotel (onr, hnr, hotel) values ('1' , '2' , 'Koenig') !              
insert hotel (onr, hnr, hotel) values ('1' , '3' , 'Berlin') !              
insert hotel (onr, hnr, hotel) values ('2' , '1' , 'Adler') !               
insert hotel (onr, hnr, hotel) values ('2' , '2' , 'Lamm') !                
commit work !
select hotel.onr, hotel.hnr, hotel, bnr, gast
from hotel, buch
where hotel.onr = '1'
and hotel.onr = buch.onr (+)
and hotel.hnr = buch.hnr (+) !
fetch !
select hotel.onr, hotel.hnr, hotel, bnr, gast
from <hotel, buch>
where hotel.onr = '1'
and hotel.onr = buch.onr (+)
and hotel.hnr = buch.hnr (+) !
fetch !
select hotel.onr, hotel.hnr, hotel, bnr, gast
from <buch, hotel>
where hotel.onr = '1'
and hotel.onr = buch.onr (+)
and hotel.hnr = buch.hnr (+) !
fetch !
rollback work release !
