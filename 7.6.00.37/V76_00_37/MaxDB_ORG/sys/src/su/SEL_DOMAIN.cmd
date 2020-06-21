* show domain !
declare sel_dom cursor for 
 select owner,domainname,datatype,codetype,len,"DEC",
        "DEFAULT"
   from domain.domains 
  where owner <> 'SUT' !
fetch sel_dom into :a,:b,:c,:d,:e,:f,:g !
close sel_dom !
