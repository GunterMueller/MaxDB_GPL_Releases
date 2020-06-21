file connect ( kern test !
monitor init !
pars_then_ex !
parameter !
data!
select &1 from tab where f2 = 3 + :A * 4  &2 &3 &4 &5 !
n 18

n 18 3

nodata !
no_parameter !
fetch into :A, :A, :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
parameter !
data!
select &1 from tab where f2 < 2 - :A * 4  &2 &3 &4 &5 !
n 18

n 18 12

nodata !
no_parameter !
fetch into  :A, :A, :A!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
parameter !
data!
select &1 from tab where f2 > 3 + :A * 4  &2 &3 &4 &5 !
n 18

n 18 114

nodata!
no_parameter !
fetch into :A, :A, :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
parameter !
data!
select &1 from tab where f2 <= 2 - :A * 4  &2 &3 &4 &5 !
n 18

n 18 12

nodata !
no_parameter !
fetch into  :A, :A, :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
parameter !
data!
select &1 from tab where f2 >= 3 + :A * 4  &2 &3 &4 &5 !
n 18

n 18 114

nodata!
no_parameter !
fetch into :A, :A, :A!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
parameter !
data !
select &1 from tab where f2 between 5 * :A and 8 + :B
&2 &3 &4 &5 !
n 18
n 18

n 18 3
n 18 10

nodata!
no_parameter !
fetch into :a, :a, :a!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
parameter !
data !
select &1 from tab where f2 between :A and 8 + :B
&2 &3 &4 &5 !
n 10
n 18

n 10 15
n 18 10

nodata!
no_parameter !
fetch into :a, :a, :a!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
parameter !
data!
select &1 from tab where f2 in (fixed(sqrt(:A),7), 10, 13, 8 * :B )
&2 &3 &4 &5 !
n 18
n 18

n 18 81
n 18 8

nodata!
no_parameter !
fetch into :a, :a, :a !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 like rfill(:A,'=',10) &2 &3 &4 &5 !
c 254 '%===ttt'

nodata !
fetch into :a,:a,:a!
pars_execute !
file sel_monitor ( load!
monitor init!
commit work release !
