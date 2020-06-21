file connect ( kern test !
monitor init !
pars_then_ex !
data!
select k1 from tab where k1 = substr(:A,1,10) &1 &2 &3 &4 !
c 254 'abcdefghijklnmopqrstuvwxyz'

nodata !
fetch into :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 < substr(:A,5,9) &1 &2 &3 &4 !
c 254 'wert000044500bbaaaaccc'

nodata !
fetch into  :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 > substr(:A,5,10) &1 &2 &3 &4 !
c 254 'wertzzzzzdddyyyyyddddd'

nodata!
fetch into :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 <= substr(:A,5,9) &1 &2 &3 &4 !
c 254 'wert000044500bbaaaaccc'

nodata !
fetch into  :A !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 >= substr(:A,5,10) &1 &2 &3 &4 !
c 254 'wertzzzzzdddyyyyyddddd'

nodata!
fetch into :A!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data !
select k1 from tab where k1 between substr(:A,5,6) and rfill (:B,'=',11)
&1 &2 &3 &4 !
c 254 'dsedaaacccbdsfad'
c 254 'ab===ttt'

nodata!
fetch into :a!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 in (substr(:A,4,6),'werner', 'dirk',
          rfill (:B,'=',11)) &1 &2 &3 &4 !
c 254 'dsedaaacccbdsfad'
c 254 'ab===ttt'

nodata!
fetch into :a !
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data!
select k1 from tab where k1 like rfill(:A,'=',10) &1 &2 &3 &4 !
c 254 '%===ttt'

nodata !
fetch into :a!
pars_execute !
file sel_monitor ( load!
monitor init!
pars_then_ex!
data !
select k1 from tab where k1 like rfill(:A,'%',9) &1 &2 &3 &4 !
c 254 'ab===ttt'

nodata!
fetch into :a!
pars_execute !
file sel_monitor ( load!
monitor init!
commit work release !
