* show domaindef &1 !
declare sel_dom cursor for 
 select c_definition
   from sysdd.domains
  where domainname = '&1' !
fetch sel_dom into :a !
close sel_dom !
