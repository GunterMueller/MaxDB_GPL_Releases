
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

import string

def path2release (path, delimiter):
    list = string.split (path, delimiter)
    #
    # path is one of:
    # [..., 'V73', 'develop' ]
    # [..., 'V73', 'c_00', 'develop']
    # [..., 'V73', 'c_00', 'b_17']
    corrEntry = list [-2]
    if corrEntry [:2] == 'c_':
        list = list [-3:]
    else:
        list = list [-2:] + ['']
    majorVPart, corrLevelPart, buildPart = list
    if majorVPart == 'sapdb':
        return (corrLevelPart, '', '', '')
    if majorVPart in ['TOOLSRC', '_tools']:
        return (majorVPart, '', '', buildPart)
    assert majorVPart [0] == 'V'
    majorPart = majorVPart [1:-1]
    minorPart = majorVPart [-1:]
    if corrLevelPart [:2] == 'c_':
        corrLevelPart = corrLevelPart [2:]
    if buildPart [:2] == 'b_':
        buildPart = buildPart [2:]
    return majorPart, minorPart, corrLevelPart, buildPart

def release2filename (releaseInfo):
    majorPart, minorPart, corrLevelPart, buildPart = releaseInfo
    result = 'V' + majorPart + minorPart + '_' + corrLevelPart
    if buildPart:
        result = result + '_' + buildPart
    return result

def releasePath2filename (path, delimiter):
    return release2filename (path2release (path, delimiter))

def release2string (releaseInfo):
    majorPart, minorPart, corrLevelPart, buildPart = releaseInfo
    result = majorPart + minorPart + '.' + corrLevelPart + '.' + buildPart
    return result

