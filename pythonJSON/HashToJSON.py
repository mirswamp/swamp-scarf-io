from yajl import YajlGen

###################Handle errors############################################################
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
#       if not methodPrimary :
#               errors.append("Misformed Element: No primary Method found in  BugInstance: %s." % (bugID));

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
#       if not locPrimary :
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


class HashToJSON:
    
##################Initialize Writer##################################################
    def __init__(self, output, error_level):
        try:
            self.output = open(output, "w")
        except IOError:
            print('cannot open file')
            sys.exit(1)
        if error_level == 1 or error_level == 0:
            self.error_level = error_level
        else:
            self.error_level = 2
        self.pretty = 0;
	self.writer = YajlGen(beautify=False)
	self.curr = "initial";

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
	if self.pretty:
	    self.writer = YajlGen(beautify=True)
	else:
	    self.writer = YajlGen(beautify=False)

#######################Write a start tag######################################


    def addStartTag(self, initial_details):
#       for reqAttr in ["tool_name", "tool_version", "uuid"]:
#           if reqAttr not in initial_details:
#               error(self.error_level, "Required attribute: %s not found when creating startTag" % reqAttr)

        if self.error_level != 0 :
            errors =  checkStart(initial_details)
            for error in errors:
                print error
            if errors and self.error_level == 2:
                sys.exit(1)

        writer = self.writer
        writer.yajl_gen_map_open()
        writer.yajl_gen_string("AnalyzerReport")
        writer.yajl_gen_map_open()
        writer.yajl_gen_string("tool_name")
        writer.yajl_gen_string(initial_details["tool_name"])
        writer.yajl_gen_string("tool_version")
        writer.yajl_gen_string(initial_details["tool_version"])
        writer.yajl_gen_string("uuid")
        writer.yajl_gen_string(initial_details["uuid"])

        self.output.write(writer.yajl_gen_get_buf())

        return self


####################Write a bug instance#########################################################


    def addBugInstance(self, bugHash):

	writer = self.writer
        #check for req elmts
        if self.error_level != 0 :
            errors =  checkBug(bugHash, self.bugID)
            for error in errors:
                print error
            if errors and self.error_level == 2:
                sys.exit(1)

        # byte count info
        byte_count = 0
        initial_byte_count = 0;
        initial_byte_count = self.output.tell()

	if self.curr == "metric":
	    writer.yajl_gen_array_close()
	if self.curr == "summary":
	    print("Summary already written")
	    return
	if self.curr != "bug":
	    writer.yajl_gen_string("BugInstances")
	    writer.yajl_gen_array_open()
	    self.curr = "bug"

	writer.yajl_gen_map_open()

	writer.yajl_gen_string("BugId")
	writer.yajl_gen_number(str(self.bugID))

	for elmt in ["BuildId", "BugCode", "BugRank", "ClassName", "BugSeverity", "BugGroup", "BugMessage", "ResolutionSuggestion"]:
	    if elmt in bugHash:
		writer.yajl_gen_string(elmt)
		writer.yajl_gen_string(bugHash[elmt])

	if "CweIds" in bugHash:
	    writer.yajl_gen_string("CweIds")
	    writer.yajl_gen_array_open()
	    for cwe in bugHash["CweIds"]:
		writer.yajl_gen_number(str(cwe))
	    writer.yajl_gen_array_close()

	if "Methods" in bugHash:
            methodId = 1
            writer.yajl_gen_string("Methods")
            writer.yajl_gen_array_open()
            for method in bugHash["Methods"]:
                writer.yajl_gen_map_open()
                writer.yajl_gen_string("name")
                writer.yajl_gen_string(str(method["name"]))
                writer.yajl_gen_string("primary")
                if method["primary"]:
                    writer.yajl_gen_string("true")
                else:
                    writer.yajl_gen_string("false")
        
                writer.yajl_gen_string("MethodId")
                writer.yajl_gen_number(str(methodId))
                methodId = methodId + 1
                writer.yajl_gen_map_close()
            writer.yajl_gen_array_close()
        
        if "BugLocations" in bugHash:
            locId = 1
            writer.yajl_gen_string("BugLocations")
            writer.yajl_gen_array_open()
            for location in bugHash["BugLocations"]:
                writer.yajl_gen_map_open()
                for numLocElt in ["StartLine", "EndLine", "StartColumn", "EndColumn"]:
                    if numLocElt in location:
			writer.yajl_gen_string(numLocElt)
			writer.yajl_gen_number(str(location[numLocElt]))
                for strLocElt in ["SourceFile", "Explanation"]:
                    if strLocElt in location:
			writer.yajl_gen_string(strLocElt)
			writer.yajl_gen_string(location[strLocElt])
        
                writer.yajl_gen_string("primary")
                if location["primary"]:
                    writer.yajl_gen_string("true")
                else:
                    writer.yajl_gen_string("false")
                writer.yajl_gen_string("LocationId")
                writer.yajl_gen_number(str(locId))
                locId = locId + 1
                writer.yajl_gen_map_close()
            writer.yajl_gen_array_close()

        if "InstanceLocation" in bugHash:
            writer.yajl_gen_string("InstanceLocation")
            writer.yajl_gen_map_open()
            if "Xpath" in bugHash["InstanceLocation"]:
		writer.yajl_gen_string("Xpath")
		writer.yajl_gen_string(location["InstanceLocation"]["Xpath"])
            if "LineNum" in bugHash["InstanceLocation"]:
		writer.yajl_gen_string("LineNum")
		writer.yajl_gen_map_open()
		if "Start" in bugHash["InstanceLocation"]["LineNum"]:
		    writer.yajl_gen_string("Start")
		    writer.yajl_gen_number(str(bugHash["InstanceLocation"]["LineNum"]["Start"]))
		if "End" in bugHash["InstanceLocation"]["LineNum"]:
		    writer.yajl_gen_string("End")
		    writer.yajl_gen_number(str(bugHash["InstanceLocation"]["LineNum"]["End"]))
		writer.yajl_gen_map_close()
            writer.yajl_gen_map_close()

	writer.yajl_gen_map_close()

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

	self.output.write(writer.yajl_gen_get_buf())

        return self


###########Writer a metric##################################################

    def addMetric(self, metricHash):

        writer = self.writer
        if self.error_level != 0 :
            errors =  checkMetric(metricHash, self.metricID)
            for error in errors:
		print error
            if errors and self.error_level == 2:
                sys.exit(1)
    
        if self.curr == "bug":
            writer.yajl_gen_array_close()
        if self.curr == "summary":
            print("Summary already written")
            return
        if self.curr != "metric":
            writer.yajl_gen_string("Metrics")
            writer.yajl_gen_array_open()        
            self.curr = "metric"
    
        writer.yajl_gen_map_open()
    
        writer.yajl_gen_string("MetricId")
        writer.yajl_gen_number(str(self.metricID))
    
        for elmt in ["Value", "Class", "Method", "SourceFile", "Type"]:
            if elmt in metricHash:
		writer.yajl_gen_string(elmt)
		writer.yajl_gen_string(metricHash[elmt])
        
        writer.yajl_gen_map_close()

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

	self.output.write(writer.yajl_gen_get_buf())

        return self

############Add summary from written elements##############################################################
    def addSummary(self):
        import math

	writer = self.writer
	if self.curr == "bug" or self.curr == "metric":
	    writer.yajl_gen_array_close()        
	self.curr = "summary"

	if self.bugSummaries:

            writer.yajl_gen_string("BugSummaries")
            writer.yajl_gen_array_open()
    
            for code in  self.bugSummaries:
                writer.yajl_gen_string(code)
                writer.yajl_gen_map_open()
		for group in self.bugSummaries[code]:
		    writer.yajl_gen_string(group)
		    writer.yajl_gen_map_open()        
		    summary = self.bugSummaries[code][group]
		    writer.yajl_gen_string("count")
		    writer.yajl_gen_number(str(summary["count"]))
		    writer.yajl_gen_string("bytes")
		    writer.yajl_gen_number(str(summary["bytes"]))
		    writer.yajl_gen_map_close()        
            writer.yajl_gen_map_close()        
            writer.yajl_gen_array_close()        
            self.output.write(writer.yajl_gen_get_buf())


        if self.metricSummaries:
            writer.yajl_gen_string("MetricSummaries")
            writer.yajl_gen_map_open()        
            for metric in self.metricSummaries:
            
                summary = self.metricSummaries[metric]
    
		writer.yajl_gen_string(metric)
		writer.yajl_gen_map_open()        
                metricCount = summary["Count"]
		writer.yajl_gen_string("Count")
		writer.yajl_gen_number(str(metricCount))
    
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
			writer.yajl_gen_string(sumElt)
			writer.yajl_gen_number(str(summary[sumElt]))
    
			writer.yajl_gen_string("Average")
			writer.yajl_gen_number(str(average))
			writer.yajl_gen_string("StandardDeviation")
			writer.yajl_gen_number(str(stdDeviation))
		writer.yajl_gen_map_close()        
            
            writer.yajl_gen_map_close()        
    
            self.output.write(writer.yajl_gen_get_buf())
            
	return self


    #######################Add end tag for analyzer report###########################################
    def addEndTag(self):
        
        writer = self.writer
        if self.curr == "bug" or self.curr == "metric":
            print("closing")
            writer.yajl_gen_array_close()     
        self.curr = "summary"
        writer.yajl_gen_map_close()     
        writer.yajl_gen_map_close()     
        
        self.output.write(writer.yajl_gen_get_buf())
    
        return self












1
