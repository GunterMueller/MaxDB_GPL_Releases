select description, value from SYSDBM.info_sysdd_monitor where
description like 'Table scans, row%' OR
description like 'Primary key accesses, row%' OR
description like 'Primary key range accesses, row%' OR
description like 'Isolated index accesses, row%' OR
description like 'Isolated index range accesses, row%' OR
description like 'Index accesses, row%' OR
description like 'Index range accesses, row%'
!
fetch!
