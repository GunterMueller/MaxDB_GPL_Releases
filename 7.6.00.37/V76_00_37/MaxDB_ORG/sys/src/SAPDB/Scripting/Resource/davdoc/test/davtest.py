#
# DAV testing hack
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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

#sys
import sys
import string
import StringIO
#davtest
import davlib
import qp_xml

HOST = 'P76209'
PORT = 85

BASE = 'http://%s:%s' % (HOST, PORT)

class mydav(davlib.DAV):
  def _request(self, method, url, *rest, **kw):
    print 'REQUEST:', method, url
    response = apply(davlib.DAV._request, (self, method, url) + rest, kw)
    print "STATUS:", response.status
    print "REASON:", response.reason
    for hdr in response.msg.headers:
      print string.strip(hdr)
    print '-'*70
    if response.status == 207:
      #print response.doc.dump()
      #print response.doc.toxml()
      response.parse_multistatus()
      davlib.qp_xml.dump(sys.stdout, response.root)
    elif method == 'LOCK' and response.status == 200:
      response.parse_lock_response()
      davlib.qp_xml.dump(sys.stdout, response.root)
    else:
      print response.read()
    print '-'*70
    return response
  def get_lock(self, url, owner='', timeout=None, depth=None):
    return self.lock(url, owner, timeout, depth).locktoken


def _dav():
  return mydav(HOST, PORT)

def getvalue(url, ns, prop):
  response = _dav().getprops(url, prop, ns=ns)
  resp = response.msr.responses[0]
  if resp.status and resp.status[0] != 200:
    raise 'error retrieving property', response.status
  propstat = resp.propstat[0]
  if propstat.status and propstat.status[0] != 200:
    raise 'error retrieving property', propstat.status
  return propstat.prop[(ns, prop)]

  #s = StringIO.StringIO()
  #davlib.qp_xml.dump(s, propstat.prop[(ns, prop)])
  #return s.getvalue()

def gen_test_data():
  # some property values
  pv1 = '<pv1>pv1</pv1>'
  pv2 = '<pv2/>'
  pv3 = '<foo:pv3 xmlns:foo="namespace">pv3</foo:pv3>'
  pv4 = '<foo:pv4 xmlns:foo="namespace"/>'
  pv5 = '<bar:pv5 xmlns:bar="namespace">pv5</bar:pv5>'
  pv6 = '<bar:pv6 xmlns:bar="namespace"/>'
  pv7 = '<pv7 xmlns="namespace">pv7</pv7>'
  pv8 = '<pv8 xmlns="namespace"/>'
  pv9 = '<foo:pv9 xmlns:foo="other">pv9</foo:pv9>'
  pv10 = '<foo:pv10 xmlns:foo="other"/>'
  pv11 = '<bar:pv11 xmlns:bar="other">pv11</bar:pv11>'
  pv12 = '<bar:pv12 xmlns:bar="other"/>'
  pv13 = '<pv13 xmlns="other">pv13</pv13>'
  pv14 = '<pv14 xmlns="other"/>'

  # prep the file structure
  _dav().mkcol('/webdav/testdata')
  _dav().put('/webdav/testdata/file1', 'file1 contents\n')
  _dav().put('/webdav/testdata/file2', 'file2 contents\n')
  _dav().put('/webdav/testdata/file3', 'file3 contents\n')
  _dav().mkcol('/webdav/testdata/sub')
  _dav().put('/webdav/testdata/sub/file1', 'sub/file1 contents\n')
  _dav().put('/webdav/testdata/sub/file2', 'sub/file1 contents\n')
  _dav().put('/webdav/testdata/sub/file3', 'sub/file1 contents\n')

  # attach a bunch of properties
  _dav().setprops('/webdav/testdata',
                  pv1, pv2,
                  pv3, pv4, pv5, pv6, pv7, pv8,
                  pv9, pv10, pv11, pv12, pv13, pv14)
  _dav().setprops('/webdav/testdata/file1',
                  pv1, pv2,
                  pv3, pv4, pv5, pv6, pv7, pv8,
                  pv9, pv10, pv11, pv12, pv13, pv14)
  _dav().setprops('/webdav/testdata/file2',
                  pv9, pv10, pv11, pv12, pv13, pv14)
  _dav().setprops('/webdav/testdata/file3',
                  pv1, pv2,
                  pv3, pv4, pv5, pv6, pv7, pv8)
  _dav().setprops('/webdav/testdata/sub',
                  pv1, pv2)
  _dav().setprops('/webdav/testdata/sub/file1',
                  pv1, pv2,
                  pv9, pv10, pv11, pv12, pv13, pv14)
  _dav().setprops('/webdav/testdata/sub/file2',
                  pv3, pv4, pv5, pv6, pv7, pv8,
                  pv9, pv10, pv11, pv12, pv13, pv14)
  _dav().setprops('/webdav/testdata/sub/file3',
                  pv1, pv2,
                  pv3, pv4, pv5, pv6, pv7, pv8)

  # do some moves and copies
  _dav().move('/webdav/testdata/file1', BASE + '/webdav/testdata/newfile1')
  _dav().move('/webdav/testdata/sub', BASE + '/webdav/testdata/newsub')
  _dav().move('/webdav/testdata/newsub/file2', BASE + '/webdav/testdata/newsubfile2')
  _dav().copy('/webdav/testdata/newsub/file3', BASE + '/webdav/testdata/newsub/file3copy')
  _dav().copy('/webdav/testdata/newsub/file1', BASE + '/webdav/testdata/subfile1copy')
  _dav().copy('/webdav/testdata/newsub', BASE + '/webdav/testdata/subcopy')

  # dump all the data
  _dav().allprops('/webdav/testdata')


def del_test_data():
  _dav().delete('/webdav/testdata')

def gettest():
  _dav()._request('GET', '/webdav/test.html')

def if_test():
  _dav().put('/webdav/foo.html', 'foo.html contents\n')
  etag = qp_xml.textof(getvalue('/webdav/foo.html', 'DAV:', 'getetag'))
  print 'ETAG:', etag
  _dav()._request('DELETE', '/webdav/foo.html', extra_hdrs={
    'If' : '(["abc"])',
    })
  _dav()._request('DELETE', '/webdav/foo.html', extra_hdrs={
    'If' : '([' + etag + '])',
    })

def lock_test():
  _dav().delete('/webdav/locktest')
  _dav().mkcol('/webdav/locktest')
  _dav().mkcol('/webdav/locktest/sub')

  # test a locknull resource
  r = _dav().lock('/webdav/locktest/locknull')

def test():
  _dav().put('/webdav/foo.html', 'foo.html contents\n')
  _dav().options('/webdav/foo.html')
  _dav().delete('/webdav/foo.html')
  _dav().delete('/webdav/newdir')
  _dav().mkcol('/webdav/newdir')
  _dav().mkcol('/webdav/newdir/another')
  _dav().allprops('/webdav', 1)
  _dav().propnames('/webdav', 1)
  _dav().getprops('/webdav', 'author', 'foober', 'title')
  _dav().propfind('/webdav',
                  '<?xml version="1.0"?><propfind xmlns="DAV:"><prop>'
                  '<author/><foobar/><title/>'
                  '</prop></propfind>',
                  'infinity')
  _dav().delprops('/webdav', 'author')
  _dav().setprops('/webdav', '''<author>
        <complex oops="hi" two="three" > stuff goes in here
        </complex>
          <more foo="bar"/>
          stuff
      </author>''')
  _dav().put('/webdav/blah.cgi', 'body of blah.cgi\n')
  _dav().put('/webdav/file1', 'body of file1\n')
  _dav().move('/webdav/blah.cgi', BASE + '/webdav/foo.cgi')
  _dav().copy('/webdav/subdir', BASE + '/webdav/subdir3')
  _dav().setprops('/webdav/file1','<woo>bar</woo>')

  _dav().propfind('/webdav/foo.cgi', '''<?xml version="1.0"?>
  <propfind xmlns="DAV:"><propname/>
  <foo:bar xmlns:foo="xyz" xmlns:bar="abc">
  <foo:bar bar:xxx="hello"/>
  <bar:ddd what:aaa="hi" xmlns:what="ha"/>
  <geez xmlns="empty"/>
  <davtag/>
  <rep:davtag2 xmlns:rep="DAV:"/>
  </foo:bar>
  </propfind>
  ''')

  del_test_data()
  gen_test_data()


if __name__ == '__main__':
  if HOST == 'FILL THIS IN':
    import sys
    sys.stdout = sys.stderr
    print 'ERROR: you must edit davtest.py to set the HOST/PORT values'
    print '       at the top of the script.'
    sys.exit(1)

  test()
