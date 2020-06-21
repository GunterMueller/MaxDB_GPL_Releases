create view v as 
       select * from t1, t2 where b1 = 'value' and b2 = s2 and sl = bl !
explain select * from v !
        select * from v !
fetch !
drop view v ! 
create view v as 
        select * from t1, t2 where b2 = s2 and b1 = 'value' and sl = bl !
explain select * from v !
        select * from v !
fetch !
drop view v ! 
create view v as 
        select * from t1, t2 where b1 = s1 and b2 = 'value' and sl = bl !
explain select * from v !
        select * from v !
fetch !
drop view v ! 
create view v as 
        select * from t1, t2 where b1 = s1 and bl = 'value' and s2 = b2 !
explain select * from v !
        select * from v !
fetch !
drop view v ! 
create view v as 
        select * from t1, t2 where b1 = s1 and 'value' = b2 and sl = bl !
explain select * from v !
        select * from v !
fetch !
drop view v ! 
