#!/usr/local/bin/python
#############################################################################
#
#  rmtestdb.py
#
#  Script for removing outdated and test databases
#
#  Author: Robert Völkel (d022860)
# 
#  Required environment:  - dbmcli must be in the PATH
#                         - x_server must be running
#                         - write permission for current working directory
#                         - see "Global variables" below for settings
#
#  Known bugs: - DBs of very old releases (approx. < 7.2.4) may appear to be
#                freshly installed because there was no parameter history
#                file back in the dark ages.
#
#  Change history:
#
#    2001-09-28 d022860: First version
#    2003-12-22 d022860: option -f added
#
#############################################################################
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



#
# Phyton modules
#


import os
import sys
import string
import time
import shutil
import glob

#
# Options
#

# if called with -f, disable grace perion and skip prompts
if len(sys.argv) > 1:
	forceOption = sys.argv[1]
else:
	forceOption = "-n"

#
# Global variables
#


# grace period for databases without _COMMENT in days
# DB will be removed if gracePeriod is exceeded and no _COMMENT is set
# to disable grace period, set to -1
gracePeriod = 60
if forceOption == "-f":
	gracePeriod = -1

# list of DBM users and passwords for dbmcli calls
users = [["dbm", "dbm"], ["control", "control"], ["superdba", "colduser"]]

# test comment used to indicate DBs created by the TestFrame
# DB will be treated as if _COMMENT was empty
testComment = "SAPDBJAVATEST"

# log file for this script
logFileName = "rmtestdb.log"


#
# Functions
#


# function for executing an OS command
#	parameter: string containing the command
#	returns: the command's output
#	note: command is logged

def execCmd(cmd):
	logFile.write("Executing: " + cmd + "\n")
	os.system(cmd + " > _tmpout")
	tmpFile = open("_tmpout", 'r')
	resultList = []
	line = tmpFile.readline()
	while line:
		resultList.append(line)
		line = tmpFile.readline()
	tmpFile.close()
	os.remove("_tmpout")
	return resultList


# function for calculating the time difference
#	parameters: strings in YYYYMMDD format
#	returns: the diff in days between the two dates
#	note: function does not account for leap years

def timeDiffDays(early, late):
	eYear = string.atoi(early[0:4])
	eMonth = string.atoi(early[4:6])
	eDay = string.atoi(early[6:])
	lYear = string.atoi(late[0:4])
	lMonth = string.atoi(late[4:6])
	lDay = string.atoi(late[6:])
	monthLen = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
	eDiff = (eYear - 1970) * 365
	mct = 0
	while mct < eMonth - 1:
		eDiff = eDiff + monthLen[mct]
		mct = mct + 1
	eDiff = eDiff + eDay
	lDiff = (lYear - 1970) * 365
	mct = 0
	while mct < lMonth - 1:
		lDiff = lDiff + monthLen[mct]
		mct = mct + 1
	lDiff = lDiff + lDay
	return lDiff - eDiff


#
# Main program
#


# say hi to user

print "rmtestdb.py - Script for removing outdated and test databases"


# init log file

logFile = open(logFileName, 'w')
logFile.write("rmtestdb.py log file\n")
logFile.write("-----------------------------------------------------\n")
logFile.write("Start time: " + time.asctime(time.localtime(time.time())) + "\n")
print "Log file is written to " + logFileName + " ..."


# determine config directory

logFile.write("-----------------------------------------------------\n")
configDir = ""
if os.name == "nt" or os.name == "NT":
	output = execCmd("dbmcli dbm_getpath INDEPDATAPATH")
	if len(output) > 1 and output[0] == "OK\n":
		configDir = os.path.join(string.strip(output[1]), "config")
		logFile.write("Config directory is " + configDir + "\n")
		print "Config directory is " + configDir
elif os.name == "posix" or os.name == "POSIX":
	configDir = "/usr/spool/sql/config"
	logFile.write("Config directory is /usr/spool/sql/config\n")
	print "Config directory is /usr/spool/sql/config"
else:
	logFile.write("Operating system " + os.name + " not covered by this script,\n")
	logFile.write("   cannot determine config directory.\n")
	print "Operating system " + os.name + " not covered by this script,"
	print "   cannot determine config directory."


# create list of databases, containing DB name and DBROOT directory for each DB

print "Creating list of databases"
print "Found:"
logFile.write("-----------------------------------------------------\n")
logFile.write("Creating list of databases\n")
logFile.write("--------------------------------\n")
output = execCmd("dbmcli db_enum")
logFile.write("Found:\n")
dbList = []
ctr = 0
while ctr < len(output):
	if ctr == 0 and output[ctr] != "OK\n":
		logFile.write("  ..." + output[0])
		print output[0]
		break
	if output[ctr] != "OK\n" and output[ctr] != "\n":
		tmpList = string.split(output[ctr])
		# don't list doubles from dbmcli db_enum
		ctr2 = 0
		found = 0
		while ctr2 < len(dbList):
			if dbList[ctr2][0] == tmpList[0]:
				found = 1
				break
			ctr2 = ctr2 + 1
		if not found:
			print "  " + tmpList[0]
			logFile.write("  " + tmpList[0] + "\n")
			dbList.append([tmpList[0], tmpList[1]])
	ctr = ctr + 1


# init junk array with default "OK"

dbState = []
ctr = 0
while ctr < len(dbList):
	dbState.append("OK")
	ctr = ctr + 1


# get database parameters 

logFile.write("-----------------------------------------------------\n")
logFile.write("Getting database parameters\n")
logFile.write("--------------------------------\n")
ctr = 0
while ctr < len(dbList):

	print "Getting parameters for " + dbList[ctr][0] + " ..."
	logFile.write("Getting parameters for " + dbList[ctr][0] + " ...\n")

	# get _COMMENT
	comment = ""
	usr = 0
	while usr < len(users):
		output = execCmd("dbmcli -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " param_getvalue _COMMENT")
		if output[0] == "OK\n":
			comment = string.strip(output[1])
			logFile.write("  ...OK\n")
			break
		else:
			logFile.write("  ..." + output[0])
			if len(output) > 1:
				logFile.write("  ..." + output[1])
			output = execCmd("dbmcli -s -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " param_getvalue _COMMENT")
			if output[0] == "OK\n":
				comment = string.strip(output[1])
				logFile.write("  ...OK\n")
				break
			else:
				logFile.write("  ..." + output[0])
				if len(output) > 1:
					logFile.write("  ..." + output[1])
		usr = usr + 1
	dbList[ctr].append(comment)
		
	# get RUNDIRECTORY, and set junk flag if the database is really trashed
	rundir = ""
	usr = 0
	incomplete = 0
	while usr < len(users):
		output = execCmd("dbmcli -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " param_getvalue RUNDIRECTORY")
		if output[0] == "OK\n":
			rundir = string.strip(output[1])
			logFile.write("  ...OK\n")
			break
		else:
			if (string.find(output[0], "database or server not found") > -1) or (string.find(output[0], "protocol error") > -1):
				dbState[ctr] = "junk"
				if incomplete == 0:
					incomplete = 1
					print "  DB configuration already incomplete."
					logFile.write("DB configuration already incomplete:\n")
			logFile.write("  ..." + output[0])
			if len(output) > 1:
				logFile.write("  ..." + output[1])
			output = execCmd("dbmcli -s -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " param_getvalue RUNDIRECTORY")
			if output[0] == "OK\n":
				rundir = string.strip(output[1])
				logFile.write("  ...OK\n")
				break
			else:
				if (string.find(output[0], "database or server not found") > -1) or (string.find(output[0], "protocol error") > -1):
					dbState[ctr] = "junk"
					if incomplete == 0:
						incomplete = 1
						print "  DB configuration already incomplete."
						logFile.write("DB configuration already incomplete:\n")
				logFile.write("  ..." + output[0])
				if len(output) > 1:
					logFile.write("  ..." + output[1])
		usr = usr + 1
	dbList[ctr].append(rundir)

	# get age of database in days 
	# default is 9999, so DBs with no files left are suggested for removal
	timeDiff = 9999

	# try to find out date of first entry in parameter hiostory
	if configDir != "" and os.path.exists(os.path.join(configDir, dbList[ctr][0] + ".pah")):
		tmpFile = open(os.path.join(configDir, dbList[ctr][0] + ".pah"), 'r')
		line = tmpFile.readline()
		while line:
			tmpList = string.split(line)
			if tmpList[0][0:3] == "200" or tmpList[0][0:3] == "199":
				timeDiff = timeDiffDays(tmpList[0][0:8], time.strftime("%Y%m%d", time.localtime(time.time())))			
				break
			line = tmpFile.readline()
		tmpFile.close()

	# if that didn't work, try to find out when INSTPRT was last modified (last INIT_CONFIG)
	if timeDiff != 9999:
		usr = 0
		while usr < len(users):
			output = execCmd("dbmcli -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " file_getlist")
			if output[0] == "OK\n":
				ctr2 = 0
				while ctr2 < len(output):
					if output[ctr2] != "OK\n" and output[ctr2] != "\n":
						tmpList = string.split(output[ctr2])
						if tmpList[0] == "INSTPRT":
							timeDiff = timeDiffDays(tmpList[3], time.strftime("%Y%m%d", time.localtime(time.time())))
					ctr2 = ctr2 + 1
				logFile.write("  ...OK\n")
				break
			else:
				logFile.write("  ..." + output[0])
				if len(output) > 1:
					logFile.write("  ..." + output[1])
				output = execCmd("dbmcli -s -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " file_getlist")
				if output[0] == "OK\n":
					ctr2 = 0
					while ctr2 < len(output):
						if output[ctr2] != "OK\n" and output[ctr2] != "\n":
							tmpList = string.split(output[ctr2])
							if tmpList[0] == "INSTPRT":
								timeDiff = timeDiffDays(tmpList[3], time.strftime("%Y%m%d", time.localtime(time.time())))
						ctr2 = ctr2 + 1
					logFile.write("  ...OK\n")
					break
				else:
					logFile.write("  ..." + output[0])
					if len(output) > 1:
						logFile.write("  ..." + output[1])
			usr = usr + 1

	dbList[ctr].append(timeDiff)
	print "  _COMMENT=" + dbList[ctr][2]
	print "  RUNDIRECTORY=" + dbList[ctr][3]
	if dbList[ctr][4] == 9999:
		print "  Could not determine age of database."
	else:
		print "  Database is " + repr(dbList[ctr][4]) + " day(s) old."
	logFile.write("  _COMMENT=" + dbList[ctr][2] + "\n")
	logFile.write("  RUNDIRECTORY=" + dbList[ctr][3] + "\n")
	if dbList[ctr][4] == 9999:
		logFile.write("  Could not determine age of database.\n")
	else:
		logFile.write("  Database is " + repr(dbList[ctr][4]) + " day(s) old.\n")
	ctr = ctr + 1

# now each element in dbList includes DB name, DBROOT, _COMMENT, RUNDIRECTORY, time diff


# remove databases if _COMMENT is empty or testComment, but only if gracePeriod is exceeded

ctr = 0
while ctr < len(dbList):

	if (dbList[ctr][4] > gracePeriod and (dbList[ctr][2] == "" or dbList[ctr][2] == testComment)) or (dbState[ctr] == "junk"):

		# this is it - wipe out database

		# prompt for approval first		

		junkRemark = ""
		if dbState[ctr] == "junk":
			junkRemark = "DB configuration already incomplete. "
		if dbList[ctr][4] == 9999:
			age = "could not determine age of DB. "
		else:
			age = `dbList[ctr][4]` + " days old. "
		if forceOption == "-f":
			ask = "y"
		else:
			ask = raw_input("\nFound: "+ dbList[ctr][0] + ", " + dbList[ctr][2] + ", " + dbList[ctr][3] + ",\n  " + age + junkRemark + "Remove? (y/n)")
		if ask == "y" or ask == "Y":

			# Go! Go! Go!
			
			print "Removing database " + dbList[ctr][0]
			logFile.write("-----------------------------------------------------\n")
			logFile.write("Removing database " + dbList[ctr][0] + "\n")
			logFile.write("--------------------------------\n")
	
			# stop database
			usr = 0
			while usr < len(users):
				output = execCmd("dbmcli -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " db_stop")
				if output[0] == "OK\n":
					logFile.write("  ...OK\n")
					break
				else:
					logFile.write("  ..." + output[0])
					if len(output) > 1:
						logFile.write("  ..." + output[1])
					output = execCmd("dbmcli -s -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " db_stop")
					if output[0] == "OK\n":
						logFile.write("  ...OK\n")
						break
					else:
						logFile.write("  ..." + output[0])
						if len(output) > 1:
							logFile.write("  ..." + output[1])
				usr = usr + 1
	
			# drop database
			usr = 0
			dropped = 0
			while usr < len(users):
				output = execCmd("dbmcli -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " db_drop")
				if output[0] == "OK\n":
					logFile.write("  ...OK\n")
					dropped = 1
					break
				else:
					logFile.write("  ..." + output[0])
					if len(output) > 1:
						logFile.write("  ..." + output[1])
					output = execCmd("dbmcli -s -d " + dbList[ctr][0] + " -u " + users[usr][0] + "," + users[usr][1] + " db_drop")
					if output[0] == "OK\n":
						logFile.write("  ...OK\n")
						dropped = 1
						break
					else:
						logFile.write("  ..." + output[0])
						if string.find(output[0], "database or server not found") > -1:
							dropped = 2
						if len(output) > 1:
							logFile.write("  ..." + output[1])
							if string.find(output[0], "database or server not found") > -1:
								dropped = 2
				usr = usr + 1
			dbList[ctr].append(dropped)
			# now each element in dbList also includes the status dropped (1), drop failed (0), or DB not found (2)
	
			# remove RUNDIRECTORY if it exists and dropping was successful
			if os.path.exists(dbList[ctr][3]) and dbList[ctr][5]:
				if forceOption == "-f":
					ask = "y"
				else:
					ask = raw_input("\nDatabase " + dbList[ctr][0] + " has been dropped, but RUNDIRECTORY " + dbList[ctr][3] + " still exists.\nRemove RUNDIRECTORY? (y/n)")
				if ask == "y" or ask == "Y":
					logFile.write("Removing directory " + dbList[ctr][3] + "\n")
					shutil.rmtree(dbList[ctr][3], 1)
					
			# remove param files DBNAME and DBNAME.* if config dir exists and dropping was successful
			if configDir != "" and dbList[ctr][5]:
				for f in glob.glob(os.path.join(configDir, dbList[ctr][0] + ".*")):
					logFile.write("Removing file " + os.path.join(configDir, f) + "\n")
					os.remove(os.path.join(configDir, f))
				if os.path.isfile(os.path.join(configDir, dbList[ctr][0])):
					logFile.write("Removing file " + os.path.join(configDir, dbList[ctr][0]) + "\n")
					os.remove(os.path.join(configDir, dbList[ctr][0]))

	ctr = ctr + 1


# complete log file

logFile.write("-----------------------------------------------------\n")
logFile.write("End time: " + time.asctime(time.localtime(time.time())) + "\n")
logFile.close()
