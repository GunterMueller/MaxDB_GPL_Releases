* test key MIN/MAX optimization on index
explain 
select /*+indexaccess*/ min(k1), max(k1) from t2 where c5=3 and c6=5!
monitor init!
select /*+indexaccess*/ min(k1), max(k1) from t2 where c5=3 and c6=5!
fetch!
file MINMAX_OPTIM_RESULT!

* w/o optimization because of addnl. qualification
explain 
select /*+indexaccess*/ min(k1), max(k1) from t2 where c5=3 and c6=5 and k2=3!
explain 
select /*+indexaccess*/ min(k1), max(k1) from t2 
where c5=3 and c6=5 and k1>1 and k1<3 !
* optimization because of key start sequence
explain 
select /*+indexaccess*/ min(k2), max(k2) from t2 where c5=3 and c6=5 and k1=1!
monitor init!
select /*+indexaccess*/ min(k2), max(k2) from t2 where c5=3 and c6=5 and k1=1!
fetch!
file MINMAX_OPTIM_RESULT!

