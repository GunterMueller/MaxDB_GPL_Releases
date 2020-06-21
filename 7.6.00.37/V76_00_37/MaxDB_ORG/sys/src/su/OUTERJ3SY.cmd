file connect ( kern test !
select a, b, c, d, e, f from tab1, tab2, tab3 
where a (+) = c AND b (+) = d !
select a, b, c, d, e, f from tab1, tab2, tab3 
where a (+) = c AND b (+) = d AND b = f !
select a, b, c, d, e, f from tab1, tab2, tab3
where a (+) = c AND b     = d !
select a, b, c, d, e, f from tab1, tab2, tab3
where a (+) = c AND b     = d (+) !
select a, b, c, d from tab1, tab2
where a (+) = c AND b     = d !
select a, b, c, d from tab1, tab2
where a (+) = c AND b     = d (+) !
select a, b, c, d, e, f from tab1, tab2, tab3
where a (+) = c AND b (+) = b !
select a, b, c, d, e, f from tab1, tab2, tab3
where a (+) = c AND b (+) = b !
select a, b, c, d from tab1, tab2
where a (+) = c AND b (+) = b !
select a, b, c, d, e, f from tab1, tab2, tab3
where a (+) = c AND b = d (+) !
select a, b, c, d, e, f from tab1, tab2, tab3
where a (+) = c AND c = e (+) !
commit work release !
