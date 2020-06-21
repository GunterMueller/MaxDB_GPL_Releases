* none start sequence, aggregate behind sequence
explain select min(k1) from t1!
monitor init!
select min(k1) from t1!
fetch!
file MINMAX_OPTIM_RESULT!

explain select max(k1) from t1!
monitor init!
select max(k1) from t1!
fetch!
file MINMAX_OPTIM_RESULT!

explain select /*+indexaccess*/ min(k1),max(k1) from t1!
monitor init!
select /*+indexaccess*/ min(k1),max(k1) from t1!
fetch!
file MINMAX_OPTIM_RESULT!

* const start sequence, aggregate behind sequence
explain select min(k2) from t1 where k1 = 2!
monitor init!
select min(k2) from t1 where k1 = 2!
fetch!
file MINMAX_OPTIM_RESULT!

explain select max(k2) from t1 where k1 = 2!
monitor init!
select max(k2) from t1 where k1 = 2!
fetch!
file MINMAX_OPTIM_RESULT!

explain select /*+indexaccess*/ min(k2),max(k2) from t1 where k1 = 2!
monitor init!
select /*+indexaccess*/ min(k2),max(k2) from t1 where k1 = 2!
fetch!
file MINMAX_OPTIM_RESULT!

* aggregate on sequence
explain select min(k1) from t1 where k1 > 2!
monitor init!
select min(k1) from t1 where k1 > 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select min(k2) from t1 where k1 = 2 and k2 >= 2!
monitor init!
select min(k2) from t1 where k1 = 2 and k2 >= 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select min(k2) from t1 where k1 = 2 and k2 > 2!
monitor init!
select min(k2) from t1 where k1 = 2 and k2 > 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select min(k2) from t1 where k1 = 2 and k2 > 2 and k3 = 6!
monitor init!
select min(k2) from t1 where k1 = 2 and k2 > 2 and k3 = 6!
fetch!
file MINMAX_OPTIM_RESULT!
explain select min(k1) from t1 where k1 < 2!
monitor init!
select min(k1) from t1 where k1 < 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select min(k2) from t1 where k1 = 2 and k2 < 2!
monitor init!
select min(k2) from t1 where k1 = 2 and k2 < 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select min(k2) from t1 where k1 = 2 and k2 < 2 and k3 = 6!
monitor init!
select min(k2) from t1 where k1 = 2 and k2 < 2 and k3 = 6!
fetch!
file MINMAX_OPTIM_RESULT!

explain select max(k1) from t1 where k1 < 2!
monitor init!
select max(k1) from t1 where k1 < 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select max(k2) from t1 where k1 = 2 and k2 <= 2!
monitor init!
select max(k2) from t1 where k1 = 2 and k2 <= 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select max(k2) from t1 where k1 = 2 and k2 < 2!
monitor init!
select max(k2) from t1 where k1 = 2 and k2 < 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select max(k2) from t1 where k1 = 2 and k2 < 2 and k3 = 6!
monitor init!
select max(k2) from t1 where k1 = 2 and k2 < 2 and k3 = 6!
fetch!
file MINMAX_OPTIM_RESULT!
explain select max(k1) from t1 where k1 > 2!
monitor init!
select max(k1) from t1 where k1 > 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select max(k2) from t1 where k1 = 2 and k2 > 2!
monitor init!
select max(k2) from t1 where k1 = 2 and k2 > 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select max(k2) from t1 where k1 = 2 and k2 > 2 and k3 = 6!
monitor init!
select max(k2) from t1 where k1 = 2 and k2 > 2 and k3 = 6!
fetch!
file MINMAX_OPTIM_RESULT!
explain select max(k2) from t1 where k1 = 2 and k2 < 2 and k3 = 6!
monitor init!
select max(k2) from t1 where k1 = 2 and k2 < 2 and k3 = 6!
fetch!
file MINMAX_OPTIM_RESULT!

explain select /*+indexaccess*/ min(k1),max(k1) from t1 where k1 < 2!
monitor init!
select /*+indexaccess*/ min(k1),max(k1) from t1 where k1 < 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select /*+indexaccess*/ min(k1),max(k2) from t1 where k1 = 2 and 
k2 < 2!
monitor init!
select /*+indexaccess*/ min(k1),max(k2) from t1 where k1 = 2 and k2 < 2!
fetch!
file MINMAX_OPTIM_RESULT!
explain select /*+indexaccess*/ min(k1),max(k2) from t1 where k1 = 2 and 
k2 < 2 and k3 = 6!
monitor init!
select /*+indexaccess*/ min(k1),max(k2) from t1 where k1 = 2 and 
k2 < 2 and k3 = 6!
fetch!
file MINMAX_OPTIM_RESULT!

commit work!
