
import string

import qaccLib

class Report:
    def __init__ (self, session, confSelect, confOrder):
        self.session = session
        self.confSelect = confSelect
        self.confOrder = confOrder
        self.confCache = {}

    def executeSelect (self, when):
        cmd = 'SELECT ' + self.getSelectList () + '\n FROM %(Data)s, %(Runs)s\n '\
            + 'WHERE %(when)s = ?'
        cmd = self.pasteCommand (cmd, ' AND ', self.confSelectSQL ())
        cmd = self.pasteCommand (cmd, ' AND ', self.additionalWhereSQL ())
        cmd = self.pasteCommand (cmd, ' ORDER BY ', self.orderBySQL ())
        # print cmd
        return self.session.sqlX (cmd, [when])

    def executeSelect (self):
        cmd = 'SELECT ' + self.getSelectList () + '\n FROM %(Data)s, %(Runs)s\n ' \
            + 'where %(Runs)s.%(run-id)s in (' + self.runidSelectSQL () + ')\n'
        cmd = self.pasteCommand (cmd, ' AND ', self.additionalWhereSQL ())
        cmd = self.pasteCommand (cmd, ' ORDER BY ', self.orderBySQL ())
        # print cmd
        # self.session.explainSQL (cmd)
        return self.session.sqlX (cmd)

    def pasteCommand (self, cmd, syntaxGlue, sqlClause):
        if sqlClause:
            cmd = cmd + syntaxGlue + sqlClause + '\n'
        return cmd

    def getSelectList (self):
        cols = ('dir', 'target', 'run-id', 'success', 'quality', 'prot-file')
        result = '%(Runs)s.%(config-id)s'
        for col in cols:
            result = result + ',%(Data)s.%(' + col + ')s'
        return result

    def runidSelectSQL (self):
        selectList = 'select %(run-id)s from %(Runs)s'
        configSelect = self.configidSelectSQL ()
        latestSelect = '''(%(when)s, %(config-id)s) in
            (select max (%(when)s), %(config-id)s from %(Runs)s group by %(config-id)s)'''
        if configSelect:
            sql = selectList + ' where %(config-id)s in (' + self.configidSelectSQL ()\
                + ') and ' + latestSelect
        else:
            sql = selectList + ' where ' + latestSelect
        sql = self.pasteCommand (sql, ' AND ', self.dirPredicateSQL ())
        return sql

    def configidSelectSQL (self):
        if not self.confSelect:
            return None
        cmd = " Select %(config-id)s from %(Configurations)s "
        delimiter = 'WHERE '
        for name, value in self.confSelect:
            if type (value) == type (''):
                valueExpr = " = '" + value + "'"
            else:
                valueExpr = " IN ('" + value [0] + "'"
                for subValue in value [1:]:
                    valueExpr = valueExpr + ", '" + subValue + "'"
                valueExpr = valueExpr + ')'
            cmd = cmd + ("%s %%(propname)s = '%s' AND %%(propvalue)s " % (delimiter, name))\
                + valueExpr
        return cmd

    def confSelectSQL (self):
        if not self.confSelect:
            return None
        cmd = " %(config-id)s in (Select %(config-id)s from %(Configurations)s "
        delimiter = 'WHERE '
        for name, value in self.confSelect:
            if type (value) == type (''):
                valueExpr = " = '" + value + "'"
            else:
                valueExpr = " IN ('" + value [0] + "'"
                for subValue in value [1:]:
                    valueExpr = valueExpr + ", '" + subValue + "'"
                valueExpr = valueExpr + ')'
            cmd = cmd + ("%s %%(propname)s = '%s' AND %%(propvalue)s " % (delimiter, name))\
                + valueExpr
        return cmd + ')'

    def additionalWhereSQL (self):
        return None

    def orderBySQL (self):
        return None

    def dirPredicateSQL (self):
        return None

    def displayPlain (self, when):
        cursor = self.executeSelect (when)
        confSet = qaccLib.ConfigurationSet (self.session)
        for dir, target, ordinal, configID, when, success, quality, protfile in cursor:
            config = confSet.getConfigById (configID)
            print dir, target, config, when, success, quality, protfile

    def createConfigIndex (self, configSet):
        list = []
        for config in configSet.values ():
            row = []
            list.append (row)
            for name in self.confOrder:
                row.append (config [name])
            row.append (config)
        list.sort ()
        index = {}
        for i, row in indexIterator (list):
            config = row[-1]
            index [config.id] = i
        return list, index

    def createTableTop (self, stream, title, confList, includeQuality):
        if includeQuality:
            spread = 2
        else:
            spread = 1
        headerLines = len (confList [0]) - 1
        frontCell = '<th>' + title + '</th>'
        for i in xrange (headerLines):
            stream.write ('<tr>' + frontCell)
            frontCell = '<td>&nbsp;</td>'
            lastEntry = None
            tagOpened = None
            for row in confList:
                entry = row [i]
                if entry != lastEntry:
                    if tagOpened:
                        stream.write (self.headerCol (lastEntry, colCount * spread))
                    colCount = 0
                    lastEntry = entry
                    tagOpened = 1
                colCount = colCount + 1
            stream.write (self.headerCol (lastEntry, colCount * spread))
            stream.write ('</tr>\n')

    def headerCol (self, content, colCount):
        if colCount > 1:
            result = '<th colspan="%d">%s</th>' % (colCount, content)
        else:
            result = '<th>%s</th>' % content
        return result


    def displayHTML (self, title, when, stream,
            includeQuality = None, totalsOnly = None, configIDs = None):
        cursor = self.executeSelect ()
        configurationSet = qaccLib.ConfigurationSet (self.session)
        configSet = {}
        dataList = []
        currentTarget = None
        #
        # gather all configurations
        #
        for configID, dir, target, runID, success, quality, protfile in cursor:
            configSet [configID] = configurationSet.getConfigById (configID)
            if target != currentTarget:
                mapping = {}
                dataList.append ((target, mapping))
                currentTarget = target
            mapping [configID] = (when, success, quality, protfile)
        configList, configIndex = self.createConfigIndex (configSet)
        if len (configList) == 0:
            stream.write ('<P>no data found for %s</P>\n' % title)
            return
        if configIDs != None:
            configIDs.extend (configSet.keys ())
        #
        # create HTML table
        #
        stream.write ('<table border="2">\n')
        self.createTableTop (stream, title, configList, includeQuality)
        if includeQuality:
            dataFormat = '<td width="20">%s</td><td width="60" align="right">%s</td>' * len (configList)
        else:
            dataFormat = '<td width="20">%s</td>' * len (configList)
        totalErrors = [0] * len (configList)
        totalQuality = [0] * len (configList)
        for target, mapping in dataList:
            #
            # down
            #
            if not totalsOnly:
                stream.write ('<tr>')
                stream.write ('<td>' + target + '</td>')
            parameters = ['?'] * len (configList)
            qualities = ['&nbsp;'] * len (configList)
            for configID, (when, success, quality, protfile) in mapping.items ():
                #
                # across
                #
                parameterIndex = configIndex [configID]
                if success == 'OK':
                    parameters [parameterIndex] = '&nbsp;'
                else:
                    parameters [parameterIndex] = success
                    totalErrors [parameterIndex] = totalErrors [parameterIndex] + 1
                if quality:
                    qualities [parameterIndex] = quality
                    totalQuality [parameterIndex] = totalQuality [parameterIndex] + quality
            if includeQuality:
                dataTuple = mergeTuples (parameters, qualities)
            else:
                dataTuple = tuple (parameters)
            if not totalsOnly:
                stream.write (dataFormat % dataTuple)
                stream.write ('</tr>\n')
        #
        # totals
        #
        stream.write ('<tr><td><b>Total</b></td>')
        if includeQuality:
            dataTuple = mergeTuples (totalErrors, totalQuality)
        else:
            dataTuple = tuple (totalErrors)
        stream.write (dataFormat % dataTuple)
        stream.write ('</tr>\n')
        stream.write ('</table>\n')

def mergeTuples (list0, list1):
    result = []
    for row in map (None, list0, list1):
        result.extend (list (row))
    return tuple (result)


class DirReport (Report):
    def __init__ (self, session, dirName, confSelect, confOrder):
        Report.__init__ (self, session, confSelect, confOrder)
        self.dirName = dirName

    def dirPredicateSQL (self):
        return " %(Runs)s.%(dir)s LIKE '" + self.dirName + "%%'"

    def orderBySQL (self):
        return ' UPPER (%(target)s) '

class TargetGroupReport (Report):
    def __init__ (self, session, targetGroup, dir, confSelect, confOrder):
        Report.__init__ (self, session, confSelect, confOrder)
        self.dirName = dir
        self.targetGroup = targetGroup

    def dirPredicateSQL (self):
        return " %(Runs)s.%(dir)s LIKE '" + self.dirName + "%%'"

    def additionalWhereSQL (self):
        return """ %(target)s in
            (select %(target)s from %(TargetGroup)s
            where %(name)s = '""" + self.targetGroup + "')"


def indexIterator (list):
    return map (None, xrange (len (list)), list)

def releaseOverview (session, stream):
    confSelect = [('release', 'R74')]
    confOrder = ['platform', 'bits']
    report = DirReport (session, 'imf', confSelect, confOrder)
    report.displayHTML ('imf', 'CURRENT', stream)

def platformOverview (session, stream):
    confSelect = [('platform', ('aix', 'sun'))]
    confOrder = ['release', 'platform', 'bits']
    report = DirReport (session, 'imf', confSelect, confOrder)
    report.displayHTML ('imf', 'CURRENT', stream)

def scriptingOverview (session, stream):
    confSelect = [('bits', '32')]
    confOrder = ['release', 'platform']
    report = TargetGroupReport (session, 'Scripting', 'imf', confSelect, confOrder)
    report.displayHTML ('Scripting', 'CURRENT', stream)

def sutOverview (session, stream):
    confSelect = []
    confOrder = ['release', 'platform', 'bits', 'speed', 'encoding']
    report = DirReport (session, 'SUT', confSelect, confOrder)
    report.displayHTML ('SUT', 'CURRENT', stream, 1, 1)
    report.displayHTML ('SUT', 'CURRENT', stream, 1)

def main ():
    import sys
    import qaccDB
    from sql import sqlHelper
    session = qaccDB.connect ()
    session = sqlHelper.ExtendedConnection (session, sqlHelper.SelfDict ())
    print '<html><body>\n'
    #releaseOverview (session, sys.stdout)
    #platformOverview (session, sys.stdout)
    #scriptingOverview (session, sys.stdout)
    sutOverview (session, sys.stdout)
    print '</body></html>\n'

if __name__ == "__main__":
    main ()
