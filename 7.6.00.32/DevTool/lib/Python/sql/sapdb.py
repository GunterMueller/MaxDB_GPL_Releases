
import sys

_pythonVersion = sys.version [:3]

if _pythonVersion == '1.5':
    from python15.sapdb import *
elif _pythonVersion == '2.1':
    from python21.sapdb import *

