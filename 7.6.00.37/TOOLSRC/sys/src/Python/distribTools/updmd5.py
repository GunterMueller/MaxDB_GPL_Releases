
# updmd5.py

import os

import downloads

def main (options, args):
    """Update the cache of MD5 numbers for a download directory

    This is an addon to the downloads.py script which updates
    the download pages for mysql.com.
    """
    directory = downloads.Directory (args [0])
    files = os.listdir (directory.path)
    for file in files:
        directory.getFileInfo (file)
    directory.saveCache (trimFluff = True)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
