*  show (invisible) user !
*  declare sel_user cursor for select * from invisible
*   where username <> 'SUT' !
*  fetch sel_user !
*  close sel_user !
declare sel_user cursor for 
 select username,groupname,owner,usermode,
        connectmode as "CONNECT",
        maxtimeout,costwarning,costlimit,
        pwcreadate as "PWCREATEDATE",
        pwcreatime as "PWCREATETIME" 
   from domain.users
  where username not in ('SUT', 'SUPERDBA')
  order by username !
fetch sel_user into :a,:b,:c,:d,:e,:f,:g,:h,:i,:j !
close sel_user !
