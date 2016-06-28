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

class ScarfToHash:

    # initialize information
    def __init__(self, inputFile, callback):
        self.inputFile = inputFile
        self.callback = callback

    # parse file
    def parse(self):
        
        import xml.etree.ElementTree as etree
    
        #setup parser
        callback = self.callback
        for event, elem in etree.iterparse(self.inputFile, events = ("start", "end")):

            # parse bug instance
            if elem.tag == "BugInstance" and "BugCallback" in callback and event == "end":
                bug = {"BugId":elem.get("id")}
                cweids = []
                methods = []
                locations = []

                for subelement in elem:
                    tag = subelement.tag

                    if tag in ["ClassName", "BugGroup", "BugCode", "BugRank", "BugSeverity", "BugMessage", "ResolutionSuggestion"]:
                        bug[tag] = subelement.text
            
                    elif tag == "BugTrace":
                        for children in subelement:
                            if children.tag in ["AssessmentReportFile", "BuildId"]:
                                bug[children.tag] = children.text
                            elif children.tag == "InstanceLocation":
                                instanceLocation = {}
                                for inst in children:
                                    if inst.tag == "Xpath":
                                        instanceLocation["Xpath"] = inst.text
                                    elif inst.tag == "LineNum":
                                        lineInfo = {}
                                        for lineNum in inst:
                                            lineInfo[lineNum.tag] = lineNum.text
                                        instanceLocation["LineNum"] = lineInfo
                                bug["InstanceLocation"] = instanceLocation

                    elif tag == "CweId":
                        cweids.append(subelement.text)

                    elif tag == "Methods":
                        for method in subelement:
                            methodMap = {"MethodId":method.get("id")}
                            if method.get("primary") == "true":
                                methodMap["primary"] = 1
                            else:
                                methodMap["primary"] = 0
                            methodMap["name"] = method.text
                            methods.append(methodMap)

                    elif tag == "BugLocations":
                        for location in subelement:
                            locationMap = {"LocationId":location.get("id")}
                            if location.get("primary") == "true":
                                locationMap["primary"] = 1
                            else:
                                locationMap["primary"] = 0
                            for locationElem in location:
                                locationMap[locationElem.tag] = locationElem.text
                            locations.append(locationMap)
        
                if len(locations) != 0:
                    bug["BugLocations"] = locations
                if len(methods) != 0:
                    bug["Methods"] = methods
                if len(cweids) != 0:
                    bug["CweIds"] = cweids
        
                callback["BugCallback"](bug)
                
                parent.clear()

            #parse metric
            elif elem.tag == "Metric" and "MetricCallback" in callback and event == "end":
                metric = {"MetricId":elem.get("id")}

                for subelement in elem:
                    if subelement.tag == "Location":
                        metric["SourceFile"] = subelement[0].text
                    else:
                        metric[subelement.tag] = subelement.text
        
                callback["MetricCallback"](metric)
                parent.clear()
        
            #parse initial information
            elif elem.tag == "AnalyzerReport" and event == "start":
                initialDetails = {"tool_name":elem.get("tool_name"), "tool_version":elem.get("tool_version"), "uuid":elem.get("uuid")}

                parent = elem

                if "InitialCallback" in callback:
                    callback["InitialCallback"](initialDetails)
                parent.clear()

            #parse summaries
            elif elem.tag == "MetricSummaries" and "SummaryCallback" in callback and event == "end":
                summary = {}
                for metricSum in elem:
                    sum_hash = {}
                    for values in metricSum:
                        if values.tag != "Type":
                            sum_hash[values.tag] = values.text
                        else:
                            metricType = values.text
                    summary[metricType] = sum_hash

                callback["SummaryCallback"](summary)
                parent.clear()


            elif elem.tag == "BugSummary" and "SummaryCallback" in callback and event == "end":
                summary = {}
                for category in elem:
                    sum_hash = {"bytes":category.get("bytes"), "count":category.get("count")}
                    summary[category.get("code"), category.get("group")] = sum_hash

                callback["SummaryCallback"](summary)
                parent.clear()


