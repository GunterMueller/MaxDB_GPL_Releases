
# parcel.py
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


from vmake.parcelLib import _versionString, Parcel, \
    defaultPlatformString, defaultReleaseString, defaultPacker

def main (options, args):
    for arg in args:
        parcel = Parcel (arg)
        parcel.packParcel (options)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('nomake', None, '', None, 'don\'t use vmake to generate file list'),
        ('o', None, ':', None, 'output file or directory'),
        ('platform', None, ':', defaultPlatformString (), 'platform string'),
        ('release', None, ':', defaultReleaseString (), 'release string'),
        ('packer', None, ':', defaultPacker (), 'packer [tgz, zip]'),
        ('parcelName', None, ':', None, 'parcel in filename'),
        ('vmake', None, ':', None, 'execute vmake command or alias before packing'),
        ('env', None, '::', None, 'add environment variables before make (name=value)'),
        ('version', None, '', _versionString, 'version information'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
