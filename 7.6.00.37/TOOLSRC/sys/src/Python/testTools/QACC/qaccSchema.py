
# qaccSchema.py
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


import sys

import mylib
from sql import sqlHelper
import qaccDB

_schema = [
    ('sequence', 'confid', ''),
    ('table', 'Configurations', '''(
        "config-id" integer,
        "propname"  varchar (20),
        "propvalue" varchar (50),
        PRIMARY KEY ("config-id", "propname")
        )
    '''),
    ('sequence', 'runid', ''),
    ('table', 'Runs', '''(
        "run-id"    integer,
        "dir"       varchar (50),
        "when"      TIMESTAMP,
        "config-id" integer,
        PRIMARY KEY ("run-id")
        )
    '''),
    ('index', 'run-by-dir-time', '''
        on "Runs" ("dir", "when")
    '''),
    ('table', 'Data', '''(
        "dir"       varchar (50),
        "target"    varchar (20),
        "run-id"    integer,
        "success"   char (4),
        "quality"   integer,
        "prot-file" varchar (256),
        PRIMARY KEY ("dir", "target", "run-id")
        )
    '''),
    ('table', 'TargetGroup', '''(
        "name"      varchar (20),
        "target"    varchar (20),
        "index"     integer,
        PRIMARY KEY ("name", "target")
        )
    '''),
    ('table', 'Report', '''(
        "title"      varchar (20),
        "source"    LONG ASCII,
        PRIMARY KEY ("title")
        )
    '''),
    ('table', 'ReportCache', '''(
        "title"     varchar (20),
        "text"      LONG ASCII,
        PRIMARY KEY ("title")
        )
    '''),
    ('table', 'ReportConfigs', '''(
        "config-id" integer,
        "title"      varchar (20),
        PRIMARY KEY ("config-id", "title"),
        FOREIGN KEY ("title")
            REFERENCES "ReportCache" ("title")
            ON DELETE CASCADE
        )
    '''),
]


def main (options, args):
    """maintains the QACC database schema
    """
    try:
        manager = sqlHelper.SchemaManager (_schema, qaccDB.connect (),
            caseSensitive = 1)
        manager.callByName (args [0])
    except AttributeError:
        mylib.fatalError (args [0] + ' is not a valid command', 'qaccSchema')

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
