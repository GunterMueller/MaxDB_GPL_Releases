connect kern test !

CREATE TABLE PVC_VERSION
(
	"VERSIONID"           Varchar (16) BYTE,
	"OBJECTID"            Varchar (16) BYTE     NOT NULL,
	"BRANCHID"            Varchar (16) BYTE     NOT NULL,
	"BRANCHSEQNO"         Integer               NOT NULL,
	"OBJSTATE"            Integer               NOT NULL,
	PRIMARY KEY ("VERSIONID")
)!
insert into PVC_VERSION values (x'3456', x'12', x'123456', 1, 2)!
insert into PVC_VERSION values (x'FF3456', x'12', x'12345678', 1, 2)!

create index PVC_V_BRID_BRSEQNO on PVC_VERSION ( BRANCHID, BRANCHSEQNO )!
create index PVC_V_OID_VID on PVC_VERSION ( OBJECTID, VERSIONID )!

CREATE TABLE PVC_VERRELATION
(
	"RELATIONID"          Varchar (16) BYTE,
	"OWNERVSID"           Varchar (16) BYTE     NOT NULL,
	"OBJECTID"            Varchar (16) BYTE     NOT NULL,
	"PREDECESSORVID"      Varchar (16) BYTE,
	"PREDECESSORBRID"     Varchar (16) BYTE,
	"PREDECESSORSEQNO"    Integer,
	"SUCCESSORVID"        Varchar (16) BYTE     NOT NULL,
	"SUCCESSORBRID"       Varchar (16) BYTE     NOT NULL,
	"SUCCESSORSEQNO"      Integer               NOT NULL,
	"OBJSTATE"            Integer               NOT NULL,
	PRIMARY KEY ("RELATIONID")
)!

insert into PVC_VERRELATION values 
(x'123456', x'1234567890ABCDEF', X'12', 
null, null, null, x'12', x'12', 1, 1)!
insert into PVC_VERRELATION values 
(x'12345678', x'1234567890ABCDEF', X'12', 
null, null, null, x'12', x'12', 1, 1)!

create index PVC_VR_SUCCV on PVC_VERRELATION ( SUCCESSORVID )!
create index PVC_VR_O on PVC_VERRELATION ( OBJECTID )!
create index PVC_VR_SBR_SSEQNO on 
    PVC_VERRELATION ( SUCCESSORBRID, SUCCESSORSEQNO )!
create index PVC_VR_PBR_PSEQNO on 
    PVC_VERRELATION ( PREDECESSORBRID, PREDECESSORSEQNO )!
    
parsing!
select * from PVC_VerRelation 
where relationid in ( select v.branchid 
                      from PVC_Version v 
                      where v.branchid = relationid and 
                            v.branchseqno = 1 and 
                            v.objstate = 2 and 
                            v.versionid <> successorvid ) 
and ownervsid = :a and objstate = 1!
data!
executing!
select!
1
b 10 1234567890ABCDEF


pars_execute!

commit work release !
