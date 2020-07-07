
# depCount.py
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#


import vmake.dependencies

def getSpeed (options):
    result = 's'
    if options.quick:
        result = 'q'
    elif options.fast:
        result = 'f'
    return result

def main (options, args):
    """counts modules that are dependant on <args>

    Only object sources are counted, not include sources.
    The vmake information is cached and kept for a day,
    so subsequent runs should be faster

    """
    speed = 's' # getSpeed (options)
    dep = vmake.dependencies.VmakeDependencies ('all', speed)
    dependencies = dep.dependentsOf (args, options.target)
    if options.list:
        dependencies.sort ()
        for dependency in dependencies:
            print dependency
        print '----------------'
    print len (dependencies)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        #('s', 'slow', '', None, 'slow targets (Default)'),
        #('q', 'quick', '', None, 'quick targets'),
        #('f', 'fast', '', None, 'fast targets'),
        ('list', None, '', None, 'additionally lists modules'),
        ('target', None, '::', None, 'count only modules in this target'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
