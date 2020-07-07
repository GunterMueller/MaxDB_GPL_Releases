
# qaccSchema.py

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
