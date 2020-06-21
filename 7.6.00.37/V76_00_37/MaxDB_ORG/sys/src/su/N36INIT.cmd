*   *ID* N36INIT  DBCMD    changed on 1992-09-28-13.26.25 by ADMIN     *
create table t1 ( t1xc1 char(20),
                  t1xc2 fixed(10),
                  t1xc3 char(10) ascii,
                  t1xc4 fixed(10,5),
                  t1xc5 float(10),
                  t1xc6 fixed(18) ) !
insert t1 values ( 'Hans', 100, 'Muenchen', 100.100, null,
                    500000000000000000 ) !
insert t1 values ( null, null, 'Hamburg', 0.00005, 8E-30, -2000 ) !
insert t1 values ( 'Karin', -600000, null, null, -1E12,
                    800000000000000000 ) !
insert t1 values ( 'Karin', 5, 'Bonn', -700, 1000, null ) !
insert t1 values ( 'Hans', -500000, 'Bremen', 700.00005, null, 300 ) !
insert t1 values ( ' Marianne', -80, 'Koeln 1', -300, 10e-10, null ) !
select * from t1 !
fetch !
create table t2 ( t2xc1 fixed(5) key,
                  t2xc2 char(10) ascii,
                  t2xc3 char(15) ascii,
                  t2xc4 float(5),
                  t2xc5 fixed(5) range between -50000 and +50000 ) !
insert t2 values ( 10, 'Klaus', 'Mueller', 1E20, 20000 ) !
insert t2 values ( 20, 'Elena', 'Meier', -5E10, -100 ) !
insert t2 values ( 30, null, 'Lehmann', null, null ) !
insert t2 values ( 40, 'Paul', 'Anse Rauch', 8 , -50000 ) !
select  * from t2 !
fetch !
create table t3 ( t3xc1 fixed(2,1 ) key,
                  t3xc2 char(20) key ,
                  t3xc3 float( 5) ,
                  t3xc4 char(20) ascii range between 'A' and 'G' ,
                  t3xc5 fixed(18) ) !
insert t3 values ( 0.1, 'Frankreich', 80E50, 'Amsel',
                   700000000000000000 ) !
insert t3 values ( 2.5, 'Belgien', 1E-30, 'Fink', null ) !
insert t3 values ( 3.0, 'Italien', -5E30, 'Amsel', -1000 ) !
insert t3 values ( 4.1, 'VR Polen', null, 'Fink', +200 ) !
select * from t3 !
fetch !
