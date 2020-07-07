
import string
import xmllib

from DocDirDescription import factory, XMLTag

class DddReader (xmllib.XMLParser):
    def __init__ (self):
        xmllib.XMLParser.__init__ (self)
        self.tagStack = [XMLTag ()]
        self.currentText = ''

    def handle_data (self, data):
        self.currentText = self.currentText + data

    def unknown_starttag (self, tag, attributes):
        #print '<' + tag + '>'
        #
        # create tag
        #
        newTag = factory.create (tag)
        for key, value in attributes.items ():
            setattr (newTag, key, value)
        self.currentText = ''
        #
        #
        #
        superTag = self.tagStack [-1]
        self.tagStack.append (newTag)
        if superTag.isList:
            #print 'append', newTag, 'to', superTag
            superTag.append (newTag)
        else:
            #
            # set new tag as attribute of enclosing tag
            #
            #print 'insert', newTag, 'into', superTag
            varname = string.lower (tag [0]) + tag [1:]
            setattr (superTag, varname, newTag)

    def unknown_endtag (self, tag):
        #print '</' + tag + '>'
        currentTag = self.tagStack.pop ()
        currentTag.pcdata = self.currentText

    def getRootTag (self):
        return self.tagStack [0].directory

def parseData (data):
    reader = DddReader ()
    reader.feed (data)
    return reader.getRootTag ()

if __name__ == "__main__":
    import sys
    reader = DddReader ()
    data = open (sys.argv [1], 'rt').read ()
    rootTag = parseData (data)
    print rootTag, dir (rootTag)

