file connect ( kern test !
monitor init !
pars_then_ex !
data!
select k1 from tab where k1 = substr('abcdefghijklnmopqrstuvwxyz',1,:A)
 &1 &2 &3 &4 !
n 38 14 

nodata !
fetch into :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 < substr('wert000044500bbaaaaccc',5,:A) 
&1 &2 &3 &4 !
n 38 14

nodata !
fetch into  :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 > substr('wertzzzzzdddyyyyyddddd',5,:A) 
&1 &2 &3 &4 !
n 38 14

nodata!
fetch into :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 <= substr('wert000044500bbaaaaccc',5,:A) 
&1 &2 &3 &4 !
n 38 14

nodata !
fetch into  :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 >= substr('wertzzzzzdddyyyyyddddd',5,:A)
 &1 &2 &3 &4 !
n 38 14

nodata!
fetch into :A!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data !
select k1 from tab where k1 between substr('dsedaaacccbdsfadwergfsdgrth',5,:A)
and rfill (:B,'=',11)
&1 &2 &3 &4 !
n 38 14
c 254 'ab===ttt'

nodata!
fetch into :a!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data !
select k1 from tab where k1 between substr ('ab===ttt========',1,:B)
and substr('dsedabacccbdsfadwergfsdgrth',5,:A) 
&1 &2 &3 &4 !
n 38 14
n 38 14

nodata!
fetch into :a!
pars_execute!
file sel_monitor ( load!
monitor init!
pars_then_ex!
data !
select k1 from tab where k1 between rfill (:B,'=',11)
and substr('dsedacacccbdsfadwergfsdgrth',5,:A) 
&1 &2 &3 &4 !
c 254 'ab===ttt'
n 38 14

nodata!
fetch into :a!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 in (substr('dsedaaacccbdsfadeeeeeeeeeee',4,:A),
'werner', 'dirk', rfill (:B,'=',11)) &1 &2 &3 &4 !
n 38 14
c 254 'ab===ttt'

nodata!
fetch into :a !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 in ( 'werner', 'dirk',
substr('dsedaaacccbdsfadeeeeeeeeeee',4,:A),
rfill (:B,'=',11)) &1 &2 &3 &4 !
n 38 14
c 254 'ab===ttt'

nodata!
fetch into :a !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex !
data!
select k1 from tab where k1 in ( 'werner', 'dirk',
substr('dsedaaacccbdsfadeeeeeeeeeee',4,:A),
substr ('ab===ttt========',1,:B)) &1 &2 &3 &4 !
n 38 14
n 38 12

nodata!
fetch into :a !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex !
data !
select k1 from tab where k1 in ( 
substr('dsedaaacccbdsfadeeeeeeeeeee',4,:A),
substr('dsedaaacccbdsfadeeeeeeeeeee',4,:A),
substr('dsedaaacccbdsfadeeeeeeeeeee',4,:A),
substr ('ab===ttt========',1,:B)) &1 &2 &3 &4 !
n 38 14
n 38 13
n 38 12
n 38 12

nodata!
fetch into :a !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 like substr('dsedaaacccbdsfadeeeeeeeeeee',4,:A)
&1 &2 &3 &4 !
n 38 14

nodata !
fetch into :a!
pars_execute !
file sel_monitor ( load!
monitor init!
commit work release !
