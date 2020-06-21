
import string
import os

class Tel:
    def __init__ (self):
        self.entries = map (self.parseLine, self.readTelFile ())
        self.buildByDnumber ()

    def readTelFile (self):
        telFile = os.path.join (os.environ ['TOOL'], 'bin', 'tel.pl')
        stream = open (telFile, 'r')
        line = stream.readline ()
        while line != '__DATA__\n':
            line = stream.readline ()
        result = stream.readlines ()
        stream.close ()
        return result

    def parseLine (self, line):
        fields = string.split (line, ',')
        fields = map (string.strip, fields)
        return fields

    def buildByDnumber (self):
        dict = self.byDDict = {}
        for fields in self.entries:
            number = fields [2]
            dict [string.lower (number)] = fields

    def byDnumber (self, number):
        return self.byDDict [string.lower (number)]

if __name__ == "__main__":
    tel = Tel ()


