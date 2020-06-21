*   *ID* N71JOIN1 DBCMD    changed on 1992-09-28-13.27.07 by ADMIN     *
file CONNECT ( KERN TEST !
create table t1 ( t1c1 char(251),
                  t1c2 char(250),
                  t1c3 char(250),
                  t1c4 char(249),
                  t1c5 char(151),
                  t1c6 char(150),
                  t1c7 char(100),
                  t1c8 char( 99),
                  t1b1 char(191),
                  t1b2 char(191),
                  t1b3 char(191),
                  t1b4 char(192) ) !
create table t2 ( t2c1 char(251),
                  t2c2 char(250),
                  t2c3 char(250),
                  t2c4 char(249),
                  t2c5 char(151),
                  t2c6 char(150),
                  t2c7 char(100),
                  t2c8 char( 99),
                  t2b1 char(191),
                  t2b2 char(191),
                  t2b3 char(191),
                  t2b4 char(192) ) !
create table t3 ( t3c1 char(251),
                  t3c2 char(250),
                  t3c3 char(250),
                  t3c4 char(249),
                  t3c5 char(151),
                  t3c6 char(150),
                  t3c7 char(100),
                  t3c8 char( 99),
                  t3b1 char(191),
                  t3b2 char(191),
                  t3b3 char(191),
                  t3b4 char(192) ) !
create table t4 ( t4c1 char(251),
                  t4c2 char(250),
                  t4c3 char(250),
                  t4c4 char(249),
                  t4c5 char(151),
                  t4c6 char(150),
                  t4c7 char(100),
                  t4c8 char( 99),
                  t3b1 char(191),
                  t3b2 char(191),
                  t3b3 char(191),
                  t3b4 char(192) ) !
select t1c3, t2c4 from t1,t2 where t1c1 = t2c1 
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c3, t2c4 from t1,t2 where t1c1 < t2c1  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c3, t2c4 from t1,t2 where t1c2 = t2c2  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c3, t2c4 from t1,t2 where t1c5 = t2c5 and t1c7 = t2c7  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c3, t2c4 from t1,t2 where t1c6 = t2c6 and t1c7 = t2c7  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c3, t2c4 from t1,t2 where t1c6 = t2c6 and t1c8 = t2c8 !
select t1c1 from t1,t2,t3 where t1c2 = t2c2  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c1 from t1,t2,t3 where t1c4 = t2c4  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c1 from t1,t2,t3 where t1c4 = t2c4 and t1c7 = t2c7  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
select t1c1 from t1,t2,t3,t4 where t1c4 = t2c4 and t3c4 = t4c4  
 and t1b1=t2b1 and t1b2=t2b2 and t1b3=t2b3 and t1b4=t2b4 !
 
insert into t1 values ('ACol01', 'ACol02', 'ACol03', 'ACol04',
                       'ACol05', 'ACol06', 'ACol07', 'ACol08',
                       'ACol09', 'ACol10', 'ACol11', 'ACol12')!

insert into t1 values ('BCol01', 'BCol02', 'BCol03', 'BCol04',
                       'BCol05', 'BCol06', 'BCol07', 'BCol08',
                       'BCol09', 'BCol10', 'BCol11', 'BCol12')!

insert into t1 values ('CCol01', 'CCol02', 'CCol03', 'CCol04',
                       'CCol05', 'CCol06', 'CCol07', 'CCol08',
                       'CCol09', 'CCol10', 'CCol11', 'CCol12')!

insert into t1 values ('DCol01', 'DCol02', 'DCol03', 'DCol04',
                       'DCol05', 'DCol06', 'DCol07', 'DCol08',
                       'DCol09', 'DCol10', 'DCol11', 'DCol12')!

select count(*) from t1 a, t1 b, t1 c, t1 d 
                   group by a.t1c1, b.t1c1, c.t1c1, d.t1c1,		
                            a.t1c2, b.t1c2, c.t1c2, d.t1c2,			
                            a.t1c3, b.t1c3, c.t1c3, d.t1c3!	 
 
select a.t1c1, count(*) from t1 a, t1 b, t1 c, t1 d 
                   group by a.t1c1, b.t1c1, c.t1c1, d.t1c1,		
                            a.t1c2, b.t1c2, c.t1c2, d.t1c2,			
                            a.t1c3, b.t1c3, c.t1c3, d.t1c3!		
 
select max(a.t1c1), max(b.t1c1), max(c.t1c1), max(d.t1c1),		
       max(a.t1c2), max(b.t1c2), max(c.t1c2), max(d.t1c2),			
       max(a.t1c3), max(b.t1c3), max(c.t1c3), max(d.t1c3),			
       max(a.t1c4), max(b.t1c4), max(c.t1c4), max(d.t1c4),			
       max(a.t1c5), max(b.t1c5), max(c.t1c5), max(d.t1c5),			
       max(a.t1c6), max(b.t1c6), max(c.t1c6), max(d.t1c6),			
       max(a.t1c7), max(b.t1c7), max(c.t1c7), max(d.t1c7),			
       max(a.t1c8), max(b.t1c8), max(c.t1c8), max(d.t1c8),			
       max(a.t1b1), max(b.t1b1), max(c.t1b1), max(d.t1b1),			
       max(a.t1b2), max(b.t1b2), max(c.t1b2), max(d.t1b2),			
       max(a.t1b3), max(b.t1b3), max(c.t1b3), max(d.t1b3),			
       max(a.t1b4), max(b.t1b4), max(c.t1b4), max(d.t1b4)
       from t1 a, t1 b, t1 c, t1 d! 	
 
select max(a.t1c1), max(b.t1c1), max(c.t1c1), max(d.t1c1),		
       max(a.t1c2), max(b.t1c2), max(c.t1c2), max(d.t1c2),			
       max(a.t1c3), max(b.t1c3), max(c.t1c3), max(d.t1c3),			
       max(a.t1c4), max(b.t1c4), max(c.t1c4), max(d.t1c4),			
       max(a.t1c5), max(b.t1c5), max(c.t1c5), max(d.t1c5),			
       max(a.t1c6), max(b.t1c6), max(c.t1c6), max(d.t1c6),			
       max(a.t1c7), max(b.t1c7), max(c.t1c7), max(d.t1c7),			
       max(a.t1c8), max(b.t1c8), max(c.t1c8), max(d.t1c8),			
       max(a.t1b1), max(b.t1b1), max(c.t1b1), max(d.t1b1),			
       max(a.t1b2), max(b.t1b2), max(c.t1b2), max(d.t1b2),			
       max(a.t1b3), max(b.t1b3), max(c.t1b3), max(d.t1b3),			
       max(a.t1b4), max(b.t1b4), max(c.t1b4), max(d.t1b4)
       from t1 a, t1 b, t1 c, t1 d
       order by 1,2,3,4,5,6,7,8,9,10,11,12!

rollback work release !
