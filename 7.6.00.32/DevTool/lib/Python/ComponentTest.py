
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
import os

#---------------------------------------------------------
# Append a message string to a log file
#

def AppendLogFile(logfile, msg):
  f = open(logfile, "a")
  f.write(msg+"\n")
  f.close

#---------------------------------------------------------
# Compile and execute a component test
#   logfile: file where to write log information
#   macfile: mac description of component test
#   platformlist: a list of one ore more entries of <local>, <sun32>, <sun64>, <linux> ...
#

def DoCompTest(logfile, macfile, platformlist):
  AppendLogFile(logfile, "Testing program "+macfile)
  for p in platformlist:
    AppendLogFile(logfile, "Compiling on "+p)
    if p == "local":
      cmd = "perl -S ims.pl -u " + macfile + " >> " + logfile
      os.system(cmd)
    elif (p == "aix32" or
          p == "linux" or
          p == "hp32" or
          p == "sun32" or
          p == "sni32" or
          p == "aix64" or
          p == "dec" or
          p == "hp64" or
          p == "sun64" or
          p == "sni64" or
          p == "ia64"):
      cmd = "perl -S remims.pl " + p + " -u " + macfile + " >> " + logfile
      os.system(cmd)
    else:
      print "Unknown platform <" + p + ">"

#---------------------------------------------------------
# Print the status of a component test
#   TestStarted: The component test started at least on one platform
#   Platform: The name of the platform
#   Compiling: The compilation started on that platform
#   Executing: The compilation finished succesful on tht platform
#   PlatformOK: The execution of the test finished successful on that platform
#

def PrintStatus(TestStarted, Platform, Compiling, Executing, PlatformOK):
  if TestStarted:
    if not PlatformOK:
      print Platform,
      if Compiling:
        print ": COMPILE ERROR !!!"
      else:
        if Executing:
          print ": NOT SUCCESSFUL !!!"
        else:
          print ": NOT TESTED"

import re, string, sys

#---------------------------------------------------------
# Evaluate the content of a log file
#

def EvaluateLogFile(Filename):
  TestStarted = 0
  PlatformOK = 0
  Compiling = 0
  Executing = 0
  Platform = ""

  LogFile = open(Filename, 'r')
  for Line in LogFile.readlines():
    LineList = string.split(Line)
    if re.compile("Testing program").search(Line, 0):
      PrintStatus(TestStarted, Platform, Compiling, Executing, PlatformOK)
      print "---------------------------------------------------"
      print "Program", LineList[2]
      TestStarted = 0
      Compiling = 0
      Executing = 0
      PlatformOK = 0

    elif re.compile("Compiling on").search(Line, 0):
      PrintStatus(TestStarted, Platform, Compiling, Executing, PlatformOK)
      Platform = LineList[2]
      TestStarted = 1
      Compiling = 0
      Executing = 0
      PlatformOK = 0

    elif re.compile("start  of.+slow").search(Line, 0):
      Compiling = 1

    elif re.compile("!.TEST").search(Line, 0):
      Compiling = 0
      Executing = 1

    elif re.compile("Test successful").search(Line, 0):
      Executing = 0
      PlatformOK = 1

  PrintStatus(TestStarted, Platform, Compiling, Executing, PlatformOK)
  LogFile.close()


