#!/usr/bin/python

#  Copyright 2016 Brandon G. Klein
# 
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

import sys
from xml.dom import minidom
from lxml.etree import ElementTree as ET
from lxml import etree


###################Handle errors#############################################################
def error(error_level, message):
    if error_level == 0:
	return
    elif error_level == 1:
	print message
    else:
	print message
	sys.exit(1)

def checkStart(initial_details):
    errors = []
    for reqAttr in ["tool_name", "tool_version", "uuid"]:
        if reqAttr not in initial_details:
	   errors.append(self.error_level, "Required attribute: %s not found when creating startTag" % reqAttr)
    return errors

def checkMetric(metricHash, metricID):
    errors = []
    for reqElt in ["Value", "Type", "SourceFile"]:
	if reqElt not in metricHash:
	    error(self.error_level, "Required element: %s could not be found for Metric: %s" % (reqElt, metricID))
    return errors

def checkBug(bugHash, bugID):
    errors = []
    for reqElt in ["BugLocations", "BugMessage", "BuildId", "AssessmentReportFile"]:
        if reqElt not in bugHash:
	   errors.append("Required element: %s could not be found in BugInstance: %s" % (reqElt, bugID))
    if "Methods" in bugHash:
        methodID = 1
        methodPrimary = 0
        for method in bugHash["Methods"]:
	    if "primary" not in method:
		errors.append("Required attribute: primary not found for Method: %s in BugInstance: %s" % (methodID, bugID))
	    elif (method["primary"]) :
		if (methodPrimary) :
		    errors.append("Misformed Element: More than one primary Method found in BugInstance: %s" % (bugID));
		else :
		    methodPrimary = 1;
	    if "name" not in method:
		error.append("Required text: name not found for Method: %s in BugInstance: %s" % (methodID, bugID))
	    methodID = methodID + 1
#	if not methodPrimary :
#       	errors.append("Misformed Element: No primary Method found in  BugInstance: %s." % (bugID));

    if "BugLocations" in bugHash:	
	locPrimary = 0
	locID = 1
	for location in bugHash["BugLocations"]:
	    if "primary" not in location:
	        errors.append("Required attribute: primary not found for Location: %s in BugInstance: %s" % (locID, bugID))
	    elif (location["primary"]) :
	        if (locPrimary) :
		   errors.append("Misformed Element: More than one primary Location found in BugInstance: %s" % (bugID));
	        else :
		    methodPrimary = 1;
	    for reqLocElt in ["SourceFile"]:
	        if reqLocElt not in location:
		    errors.append("Required Element: %s could not be found for Location: %s in BugInstance %s" % (reqLocElt, locID, bugID))
	    for optNum in ["StartLine", "EndLine", "StartColumn", "EndColumn"]:
	        if optNum in location:
		    if not location[optNum].isdigit():
			errors.append("Wrong value type: $optLocElt child of BugLocation in BugInstance %s requires a positive integer." % (bugID))
#	if not locPrimary :
#           errors.append("Misformed Element: No primary Location found in  BugInstance: %s." % (bugID));
	locID = locID + 1

    if "CweIds" in bugHash:
	for cweid in bugHash["CweIds"]:
	    if not cweid.isdigit():
		errors.append("Wrong value type: CweID expected to be a positive integer in BugInstance %s." % (bugID))

    if "InstanceLocation" in bugHash:
	if "LineNum" in bugHash["InstanceLocation"]:
	    line_num = bugHash["InstanceLocation"]["LineNum"]
	    if "Start" not in line_num :
                errors.append("Required element missing: Could not find Start child of a LineNum in BugInstance: %s." % (bugID))
            elif not line_num["Start"].isdigit() :
 	        errors.append("Wrong value type: Start child of LineNum requires a positive integer BugInstance: %s." % (bugID))
	    if "End" not in line_num:
		errors.append("Required element missing: Could not find End child of a LineNum BugInstance: %s." % (bugID))
	    elif not line_num["End"].isdigit() :
		errors.append("Wrong value type: End child of LineNum requires a positive integer BugInstance: %s." % (bugID))
	elif "Xpath" not in bugHash["InstanceLocation"]:
	    errors.append("Misformed Element: Neither LineNum or Xpath children were present in InstanceLocation BugInstance: %s" % (bugID));
	return errors;



##########################################################################################
class HashToScarf:

##################Initialize Writer##################################################
    def __init__(self, output, error_level):
#	try:
#	    self.output = open(output, "w")
#	except IOError:
#	    print('cannot open file')
#	    sys.exit(1)
	self.output  = output
	if error_level == 1 or error_level == 0:
	    self.error_level = error_level
	else:
	    self.error_level = 2
	self.pretty = 0
	self.bodyType = ""
	self.start = 0

	self.bugID = 1
	self.metricID = 1

	self.metricSummaries = {}
	self.bugSummaries = {}


#########################Returns file#######################################################
    def getFile(self):
	return self.output


######################Returns current set error level######################################################
    def getErrorLevel(self):
	return self.error_level


#######################Pretty Print Options##########################################################
    def getPrettyPrint(self):
	return self.pretty;

    def setPrettyPrint(self, pretty_enable):
	self.pretty = pretty_enable;

####################Allows change of error level############################################################
    def setErrorLevel(self, error_level):
	if error_level == 1:
	    self.error_level = 1
	elif error_level == 0:
	    self.error_level = 0
	else:
	    self.error_level = 2


#######################Write a start tag######################################


    def addStartTag(self, initial_details):

	if self.error_level != 0 :
	    if self.start:
		print("Scarf file already open\n")
		if self.error_level == 2:
		    sys.exit(1)
	    errors =  checkStart(initial_details)
	    for error in errors:
		print error
	    if errors and self.error_level == 2:
		sys.exit(1)
	self.start = 1
	self.bodyType = "body"
	self.metricSummaries = {}
	self.bugSummaries = {}

	writer = self.output
	writer.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" )
	writer.write("<AnalyzerReport tool_name=\"%s\" tool_version=\"%s\" uuid=\"%s\">\n" % (initial_details["tool_name"], initial_details["tool_version"], initial_details["uuid"]) )
	return self


####################Write a bug instance#########################################################


    def addBugInstance(self, bugHash):
	#check for req elmts
	if self.error_level != 0 :
	    if self.bodyType == "summary":
		print("Summary already written. Invalid Scarf\n")
		if self.error_level == 2:
		    sys.exit(1)
	    errors =  checkBug(bugHash, self.bugID)
	    for error in errors:
		print error
	    if errors and self.error_level == 2:
		sys.exit(1)
		
	# byte count info
	byte_count = 0
	initial_byte_count = 0;
	initial_byte_count = self.output.tell()


	#addbug	
	bug = etree.Element("BugInstance")
	bug.set("id", "%s" % self.bugID)

	if "ClassName" in bugHash:
	    className = etree.SubElement(bug, "ClassName")
	    className.text = bugHash["ClassName"]

	if "Methods" in bugHash:
	    methods = etree.SubElement(bug, "Methods")
	    methodID = 1
	    for method in bugHash["Methods"]:
		wrtMethod = etree.SubElement(methods, "Method")
		wrtMethod.text = method["name"]
		wrtMethod.set("id", "%s" % methodID)
		if method["primary"]:
		    primary = "true"
		else:
		    primary = "false"
		wrtMethod.set("primary", primary)

	if "BugLocations" in bugHash:
	    if len(bugHash["BugLocations"]) >= 1:
		bugLocations = etree.SubElement(bug, "BugLocations")
		locID = 1
		for location in bugHash["BugLocations"]:
		    if location["primary"]:
			primary = "true"
		    else:
			primary = "false"
		    wrtLocation = etree.SubElement(bugLocations, "Location")
		    wrtLocation.set("id", "%s" % locID)
		    wrtLocation.set("primary", "%s" % primary)
		    for req in ["SourceFile"]:
			locSubElement = etree.SubElement(wrtLocation, req)
			locSubElement.text = "%s" % location[req]
		    for opt in ["StartColumn", "Explantion", "EndColumn", "StartLine", "EndLine"]:
			if opt in location:
			    locSubElement = etree.SubElement(wrtLocation, opt)
			    locSubElement.text = "%s" % location[opt]
		    locID = locID + 1
	
	if "CweIds" in bugHash:
	    for cweid in bugHash["CweIds"]:
		cwe = etree.SubElement(bug, "CweId")
		cwe.text = "%s" % cweid

	for bugElt in ["BugGroup", "BugCode", "BugRank", "BugSeverity"]:
	    if bugElt in bugHash:
		eltSub = etree.SubElement(bug, bugElt)
		eltSub.text = bugHash[bugElt]
	
	bugMessage = etree.SubElement(bug, "BugMessage")
	bugMessage.text = "%s" % bugHash["BugMessage"]
	
	if "ResolutionSuggestion" in bugHash:
	    resolution = etree.SubElement(bug, "ResolutionSuggestion")
	    resolution.text = "%s" % bugHash["ResolutionSuggestion"]

	bugTrace = etree.SubElement(bug, "BugTrace")
	buildID = etree.SubElement(bugTrace, "BuildId")
	buildID.text = "%s" % bugHash["BuildId"]
	assessment = etree.SubElement(bugTrace, "AssessmentReportFile")
	assessment.text = bugHash["AssessmentReportFile"]
	if "InstanceLocation" in bugHash:
	    instanceLoc = bugHash["InstanceLocation"]
	    instance = etree.SubElement(bugTrace, "InstanceLocation")
	    if "Xpath" in instanceLoc:
		xpath = etree.SubElement(instance, "Xpath")
		xpath.text = instanceLoc["Xpath"]
	    if "LineNum" in instanceLoc:
		linenum = instanceLoc["LineNum"]
		line = etree.SubElement(instance, "LineNum")
		start = etree.SubElement(line, "Start")
		start.text = "%s" % linenum["Start"]
		end = etree.SubElement(line, "End")
		end.text = "%s" % linenum["End"]
	if self.pretty:
	    self.output.write(etree.tostring(bug, pretty_print = True))
	else:
	    self.output.write(etree.tostring(bug))
	bug.clear()
	self.bugID = self.bugID + 1

	# more byte count info
	final_byte_count = self.output.tell()
	byte_count = final_byte_count - initial_byte_count

	#group bugs for summary
	if "BugGroup" in bugHash:
	    group = bugHash["BugGroup"]
	else:
	    group = "undefined"
	if "BugCode" in bugHash:
	    code = bugHash["BugCode"]
	else:
	    code = "undefined"
	
	if code in self.bugSummaries:
	    if group in self.bugSummaries[code]:
		summary = self.bugSummaries[code][group]
		summary["count"] = summary["count"] + 1
		summary["bytes"] = summary["bytes"] + byte_count
		self.bugSummaries[code][group] = summary
	    else:
		self.bugSummaries[code][group] = {"count":1, "bytes":byte_count}
	else:
	    self.bugSummaries[code] = {}
	    self.bugSummaries[code][group] = {"count":1, "bytes":byte_count}

	bug.clear()
	return self


###########Writer a metric##################################################

    def addMetric(self, metricHash):

	if self.error_level != 0 :
	    if self.bodyType == "summary":
		print("Summary already written. Invalid Scarf\n")
		if self.error_level == 2:
		    sys.exit(1)
	    errors =  checkMetric(metricHash, self.metricID)
	    for error in errors:
		print error
	    if errors and self.error_level == 2:
		sys.exit(1)

	metric = etree.Element("Metric")
	metric.set("id", "%s" % self.metricID)
	    
	loc = etree.SubElement(metric,"Location")
	source = etree.SubElement(loc, "SourceFile")	
	source.text = metricHash["SourceFile"]

	for optMetr in ["Class", "Method"]:
	    if optMetr in metricHash:
		opt = etree.SubElement(metric, optMetr)
		opt.text = metricHash[optMetr]

	for reqMetr in ["Type", "Value"]:	
	    req = etree.SubElement(metric, reqMetr)
	    req.text = "%s" % metricHash[reqMetr]
	if self.pretty:
	    self.output.write(etree.tostring(metric, pretty_print = True))
	else:
	    self.output.write(etree.tostring(metric))
	self.metricID = self.metricID + 1

	metricType = metricHash["Type"]
	if metricType in  self.metricSummaries:
	    summary = self.metricSummaries[metricType]
	    summary["Count"] = summary["Count"] + 1
	    if metricType != "language" and "Sum" in summary:
		try:
		    value = float(metricHash["Value"])
		    summary["SumOfSquares"] = summary["SumOfSquares"] + value*value
		    summary["Sum"] = summary["Sum"] + value
		    if value > summary["Maximum"]:
			summary["Maximum"] = value
		    if value < summary["Minimum"]:
			summary["Minimum"] = value
		except ValueError:
		    try:
			del summary["SumOfSquares"]
			del summary["Sum"]
			del summary["Maximum"]
			del summary["Minimum"]
		    except KeyError:
			pass
		except TypeError:
		    try:
			del summary["SumOfSquares"]
			del summary["Sum"]
			del summary["Maximum"]
			del summary["Minimum"]
		    except KeyError:
			pass


	    self.metricSummaries[metricType] = summary

	else:
	    if metricType == "language":
		self.metricSummaries[metricType] = {"Count":1}
	    else:
		try:
		    value = float(metricHash["Value"])
		    summary = {"Count":1, "Sum":value, "Maximum":value, "Minimum":value, "SumOfSquares":value*value}
		    self.metricSummaries[metricType] = summary
		except ValueError:
		    self.metricSummaries[metricType] = {"Count":1}

	metric.clear()
	return self

############Add summary from written elements##############################################################
    def addSummary(self):
	import math

	if self.bugSummaries:
	    self.bodyType = "summary"
	    summaries = etree.Element("BugSummary")
	    for code in self.bugSummaries:
		for group in code:
		    summary = self.bugSummaries[code][group]
		    codeBranch = etree.SubElement(summaries, code)
		    groupBranch = etree.SubElement(codeBranch, group)
		    groupBranch.set("count", "%s" % summary["count"])
		    groupBranch.set("bytes", "%s" % summary["bytes"])
	    self.output.write(etree.tostring(summaries, pretty_print = True))

	if self.metricSummaries:
	    self.bodyType = "summary"
	    summaries = etree.Element("MetricSummaries")
	    for metric in self.metricSummaries:
		summary = self.metricSummaries[metric]
		metricSummary = etree.SubElement(summaries, "MetricSummary")
		metricType = etree.SubElement(metricSummary, "Type")
		metricType.text = metric
		metricCount = summary["Count"]
		count = etree.SubElement(metricSummary, "Count")
		count.text = "%s" % metricCount
		
		if "Sum" in summary:
		    metricSum = summary["Sum"]
		    metricSumofSquares = summary["SumOfSquares"]
		    average = metricSum / metricCount

		    denominator = metricCount * (metricCount - 1)
		    squareOfSum = metricSum * metricSum
		    stdDeviation = 0
		    if denominator != 0:
			stdDeviation = math.sqrt((metricSumofSquares * metricCount - squareOfSum) / denominator)

		    for sumElt in ["Sum", "SumOfSquares", "Minimum", "Maximum"]:
			element = etree.SubElement(metricSummary, sumElt)
			element.text = "%s" % summary[sumElt]

		    metricAverage = etree.SubElement(metricSummary, "Average")
		    metricAverage.text = "%s" % average

		    metricStdDeviation = etree.SubElement(metricSummary, "StandardDeviation")
		    metricStdDeviation.text = "%s" % stdDeviation
	    if self.pretty:
		self.output.write(etree.tostring(summaries, pretty_print = True))
	    else:
		self.output.write(etree.tostring(summaries))
	    summaries.clear()
	    return self


    #######################Add end tag for analyzer report###########################################
    def addEndTag(self):
	if self.error_level != 0:    
	    if !self.start:
		print("Scarf file already closed\n")
		if self.error_level == 2:
		    sys.exit(1)
	self.start = 0
	self.output.write("</AnalyzerReport>")
	return self


