*** STAVAS.cmd ***
* Aufruf in STAVAA.cmd und STAVAB.cmd
*
* Selektieren auf den VIEWs
pars_then_ex !
SELECT * FROM VE&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VW&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VS&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VX&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VY&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VZ&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VA&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VB&1!
IF $RC = 0 THEN FETCH!
*
pars_execute !
monitor init!
pars_then_ex !
SELECT * FROM VC&1!
pars_execute !
file sel_monitor ( trans !
pars_then_ex !
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VD&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VF&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VG&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VH&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VI&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VJ&1!
IF $RC = 0 THEN FETCH!
*
SELECT * FROM VK&1!
IF $RC = 0 THEN FETCH!
*
pars_execute !
