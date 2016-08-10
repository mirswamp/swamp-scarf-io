//  Copyright 2016 Brandon G. Klein
// 
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//


#include <stdio.h>
#include <string.h>
#include <libxml/xmlreader.h>
#include "filestructure.h"

/////////////////////////TypeDef for Clarity///////////////////////////////////////


typedef void * (*BugCallback)(BugInstance * bug, void * reference);
typedef void * (*BugSummaryCallback)(BugSummary * bugSum, void * reference);
typedef void * (*MetricCallback)(Metric * metr, void * reference);
typedef void * (*MetricSummaryCallback)(MetricSummary * metrSum, void * reference);
typedef void * (*InitialCallback)(Initial * initial, void * reference);
typedef void * (*FinishCallback)(void * killValue, void * reference);

typedef struct Callback {
    BugCallback bugCall;
    MetricCallback  metricCall;
    InitialCallback initialCall;
    BugSummaryCallback bugSumCall;
    MetricSummaryCallback metricSumCall;
    FinishCallback finishCallback;
    void * CallbackData;
} Callback;


typedef struct ScarfToHash{
    char * filename;
    xmlTextReaderPtr reader;
    Callback * callback;
} ScarfToHash;

///////////////Initiailize a Metric//////////////////////////////////////////////
Metric * initializeMetric()
{
    Metric * metric = calloc(1,sizeof(Metric));
    return metric;
}


///////////////Initiailize a BugInstance//////////////////////////////////////////////
BugInstance * initializeBug()
{
    BugInstance * bug = calloc(1,sizeof(BugInstance));
    return bug;
}


///////////////////////////////free summaries///////////////////////////////////
int freeBugSummary(BugSummary * bugSummary){
    xmlFree((xmlChar *) bugSummary->code);
    xmlFree((xmlChar *) bugSummary->group);
    free(bugSummary);
    return 0;
}

int freeMetricSummary(MetricSummary * metricSummary){
    xmlFree((xmlChar *) metricSummary->type);
    free(metricSummary);
    return 0;
}


///////////////////////////////Free initial struct//////////////////////////////////
int freeInitial(Initial * initial){
    xmlFree((xmlChar *) initial->tool_name);   
    xmlFree((xmlChar *) initial->tool_version);   
    xmlFree((xmlChar *) initial->uuid);   
    free(initial);
    return 0;
}


///////////////////////////////Free a Metric///////////////////////////////////
int freeMetric(Metric * metric) 
{
    free(metric->type);
    free(metric->clas);
    free(metric->method);
    free(metric->sourceFile);
    free(metric->value);
    free(metric);
    return 0;
}


///////////////////////////////Free a BugInstance///////////////////////////////////
int freeBug(BugInstance * bug) 
{
    free(bug->assessmentReportFile);
    free(bug->buildId);
    free(bug->bugCode);
    free(bug->bugRank);
    free(bug->className);
    free(bug->bugSeverity);
    free(bug->bugGroup);
    free(bug->bugMessage);
    free(bug->resolutionSuggestion);
    free(bug->instanceLocation);

    CweIds * cwe = bug->cweIds;
    CweIds * prevCwe;
    while (cwe != NULL) {
	prevCwe = cwe;
	cwe = cwe->next;
	free(prevCwe);
    }

    Method * method = bug->methods;
    Method * prevMethod;
    while (method != NULL) {
	prevMethod = method;
	method = method->next;
	free(prevMethod->name);
	free(prevMethod);
    }

    Location * loc = bug->bugLocations;
    Location * prevLoc;
    while (loc != NULL) {
	prevLoc = loc;
	loc = loc->next;	
	free(prevLoc->sourceFile);
	free(prevLoc->explanation);
	free(prevLoc);
    }

    free(bug);
    return 0;
}

//////////////////Generic clear leading/trailing whitespace method//////////////////////
char * trim(char *str)
{
    char *end;
    while ( isspace(*str) ) {
	str++;
    }
    if ( *str == 0 ) {
	return str;
    }
    end = str + strlen(str) - 1;
    while ( end > str && isspace(*end) ) {
	end--;
    }
    *(end+1) = '\0';
    char * newStr = malloc(strLen(str) + 1);
    strcpy(newStr, str);
    return newStr;
}


////////////////////parse singular line of metric  file////////////////////////////////
int processMetric(xmlTextReaderPtr reader, Metric * metric)
{ 
    char * name = (char *) xmlTextReaderName(reader);
    int type = xmlTextReaderNodeType(reader);
    
    //type 1 == start tag
    if (type == 1) { 	
	if (strcmp(name, "Metric") == 0) {
	    char * temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    metric->id = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Value") == 0) { 
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    metric->value = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Type") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    metric->type = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Class") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    metric->clas = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Method") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    metric->method = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "SourceFile") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    metric->sourceFile = trim(temp);
	    xmlFree((xmlChar *) temp);
	}

    //type 15 == end tag
    } else if (type == 15 && strcmp(name, "Metric") == 0) {
	xmlFree((xmlChar *) name);
	return 1;
    }

    xmlFree((xmlChar *) name);
    return 0;
}


////////////////////parse singular line of bug file////////////////////////////////////
int processBug(xmlTextReaderPtr reader, BugInstance * bug) 
{
    char * name = (char *) xmlTextReaderName(reader);
    int type = xmlTextReaderNodeType(reader);
    
    //start tags    
    if (type == 1) { 

	if (strcmp(name, "BugInstance") == 0) {
	    char * temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    bug->bugId = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);

	} else if (strcmp(name, "InstanceLocation") == 0) {
	    InstanceLocation * inst = calloc(1, sizeof(InstanceLocation));
	    bug->instanceLocation = inst;

	} else if (strcmp(name, "Xpath") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->instanceLocation->xPath = trim(temp);
	    xmlFree((xmlChar *) temp);

	} else if (strcmp(name, "LineNum") == 0) {
	    LineNum lineNum = {0};
	    bug->instanceLocation->lineNum = lineNum;

	} else if (strcmp(name, "Start") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->instanceLocation->lineNum.start = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);

	} else if (strcmp(name, "End") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->instanceLocation->lineNum.end = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);

	}  else if (strcmp(name, "CweId") == 0) {
	    CweIds * cweid = calloc(1, sizeof(CweIds));
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader);
	    cweid->cweid = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	    if (bug->cweIds == NULL) {
		bug->cweIds = cweid;
	    } else {
		CweIds *  cur = bug->cweIds;
		while (cur->next != NULL) {
		    cur = cur->next;
		}
		cur->next = cweid;
	    }

    	} else if (strcmp(name, "Method") == 0){
	    Method * method = calloc(1, sizeof(Method));
	    char * temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    method->methodId = strtol(temp, NULL, 10);  
	    xmlFree((xmlChar *) temp);
	    temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "primary");
	    if (strcmp(temp, "true") == 0) {
		method->primary = 1;
	    } else {
		method->primary = 0;
	    }
	    xmlFree((xmlChar *) temp);
	    temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    method->name = trim(temp);
	    xmlFree((xmlChar *) temp);
	    if (bug->methods == NULL) {
		bug->methods = method;
	    } else {
		Method *  cur = bug->methods;
		while (cur->next != NULL) {
		    cur = cur->next;
		}
		cur->next = method;
	    }

	} else if (strcmp(name, "Location") == 0) {
	    Location * loc = calloc(1, sizeof(Location));
	    char * temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    loc->locationId = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	    temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "primary");
	    if (strcmp(temp, "true") == 0) {
		loc->primary = 1;
    	    } else {
		loc->primary = 0;
	    }
	    xmlFree((xmlChar *) temp);
	    if (bug->bugLocations == NULL) {
		bug->bugLocations = loc;
	    } else {
		Location *  cur = bug->bugLocations;
		while (cur->next != NULL) {
		    cur = cur->next;
		}
		cur->next = loc;
	    }

	} else if (strcmp(name,"StartLine") == 0) {   
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location *  cur = bug->bugLocations;
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
	    cur->startLine = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"EndLine") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location *  cur = bug->bugLocations;
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
	    cur->endLine = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"StartColumn") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location *  cur = bug->bugLocations;
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
	    cur->startColumn = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"EndColumn") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location *  cur = bug->bugLocations;
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
	    cur->endColumn = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"Explanation") == 0) {
	    Location *  cur = bug->bugLocations;
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    cur->explanation = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"SourceFile") == 0) {
	    Location *  cur = bug->bugLocations;
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    cur->sourceFile = trim(temp);
	    xmlFree((xmlChar *) temp);

	} else if (strcmp(name, "AssessmentReportFile") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->assessmentReportFile = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BuildId") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->buildId = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugCode") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->bugCode = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugRank") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->bugRank = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "ClassName") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->className = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugSeverity") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->bugSeverity = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugGroup") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->bugGroup = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugMessage") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->bugMessage = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "ResolutionSuggestion") == 0) {
	    char * temp = (char *) xmlTextReaderReadInnerXml(reader); 
	    bug->resolutionSuggestion = trim(temp);
	    xmlFree((xmlChar *) temp);
	}
    
    //end tags
    } else if (type == 15) {	
	if (strcmp(name, "BugInstance") == 0) {
	    xmlFree((xmlChar *) name);
	    return 1;
	} 
    }

    xmlFree((xmlChar *) name); 
    return 0;
}


//////////////////////change filename/reset parser////////////////////////////////////////

ScarfToHash * newScarfToHash(char * filename)
{
    struct Callback * calls= calloc(1, sizeof(struct Callback));
    ScarfToHash * reader = malloc(sizeof(ScarfToHash));
    reader->filename = malloc(strlen(filename) + 1 ) ;
    strcpy(reader->filename, filename);
    reader->reader = NULL;
    reader->callback = calls;
    return reader;
}

void setBugCallback(ScarfToHash * reader, BugCallback callback) {
    reader->callback->bugCall = callback;
}
void setMetricCallback(ScarfToHash * reader, MetricCallback callback) {
    reader->callback->metricCall = callback;
}
void setBugSummaryCallback(ScarfToHash * reader, BugSummaryCallback callback) {
    reader->callback->bugSumCall = callback;
}
void setMetricSummaryCallback(ScarfToHash * reader, MetricSummaryCallback callback) {
    reader->callback->metricSumCall = callback;
}
void setFinishCallback(ScarfToHash * reader, FinishCallback callback) {
    reader->callback->finishCall = callback;
}
void setInitialCallback(ScarfToHash * reader, InitialCallback callback) {
    reader->callback->initialCall = callback;
}
void setCallbackData(ScarfToHash * reader, void * callbackData) {
    reader->callback->CallbackData = callbackData;
}


BugCallback getBugCallback(ScarfToHash * reader, BugCallback callback) {
    return reader->callback->bugCall;
}
MetricCallback getMetricCallback(ScarfToHash * reader, MetricCallback callback) {
    return reader->callback->metricCall;
}
BugSummaryCallback getBugSummaryCallback(ScarfToHash * reader, BugSummaryCallback callback) {
    returnreader->callback->bugSumCall;
}
MetricSummaryCallback getMetricSummaryCallback(ScarfToHash * reader, MetricSummaryCallback callback) {
    return reader->callback->metricSumCall;
}
FinishCallback getFinishCallback(ScarfToHash * reader, FinishCallback callback) {
    return reader->callback->finishCall;
}
InitialCallback getInitialCallback(ScarfToHash * reader, InitialCallback callback) {
    return reader->callback->initialCall;
}
void * getCallbackData(ScarfToHash * reader, void * callbackData) {
    return reader->callback->CallbackData;
}


int parse(ScarfToHash * hand)
{
    hand->reader = xmlNewTextReaderFilename(hand->filename);
    xmlTextReaderPtr reader = hand->reader;
    if (reader != NULL) {
	char * name;
	int finished = 0;
	int type;
	void * kill = NULL;
	int ret = 1;
	Callback * callback = hand->callback;
        while (ret == 1 && kill == NULL) {
	    ret = xmlTextReaderRead(reader);
	    name = (char *) xmlTextReaderName(reader);
	    type = xmlTextReaderNodeType(reader); 
	    if ( type == 1 ) {
	        if ( strcmp ( name, "AnalyzerReport" ) == 0 && callback->initialCall != NULL ) {
		    Initial * initial = calloc(1, sizeof(Initial));
		    initial->tool_name = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "tool_name");
		    initial->tool_version = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "tool_version");
		    initial->uuid = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "uuid");
		    kill = callback->initialCall(initial, callback->CallbackData);

	        } else if ( strcmp ( name, "BugInstance" ) == 0 && callback->bugCall != NULL ) {
		    int foundBug = 0;
		    BugInstance * bug = initializeBug();
		    while (ret == 1 && foundBug == 0) {
			foundBug = processBug(reader, bug);
			if (foundBug == 0) {
			    ret = xmlTextReaderRead(reader);
			}
		    }
		    if (foundBug == 1) {
			kill = callback->bugCall(bug, callback->CallbackData);
		    }

	        } else if ( strcmp ( name, "Metric" ) == 0 && callback->metricCall != NULL ) {
		    Metric * metric = initializeMetric();
		    int foundMetric = 0;
		    while (ret == 1 && foundMetric == 0) {
			foundMetric = processMetric(reader, metric);
			if (foundMetric == 0) {
			    ret = xmlTextReaderRead(reader);
			}	
		    }
		    if (foundMetric == 1) {
			kill = callback->metricCall(metric, callback->CallbackData);
		    }

	        } else if ( strcmp ( name, "BugSummary" ) == 0 && callback->bugSumCall !=NULL ) {
		    BugSummary * bugsum = NULL;
		    int finSummary = 0;
		    while (ret == 1 && !finSummary) {
			ret = xmlTextReaderRead(reader);
			name = (char *) xmlTextReaderName(reader);
			type = xmlTextReaderNodeType(reader); 
			if (type == 1) {
			    if (strcmp(name, "BugCategory") == 0) {
				BugSummary * temp = malloc(sizeof(BugSummary));
				char * att = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "count");
				temp->count = strtol(att, NULL, 10);
				xmlFree((xmlChar *) att);
				att = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "bytes");
				temp->byteCount = strtol(att, NULL, 10);
				xmlFree((xmlChar *) att);
				temp->code = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "code");
				temp->group = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "group");
				if ( bugsum == NULL ) {
				    bugsum = temp;
				} else {
				    BugSummary * curr = bugsum;
				    while (curr->next != NULL) {
					curr = curr->next;
				    }
				    curr->next = temp;
				}
			    }
			} else if (type == 15 && strcmp(name, "BugSummary") == 0) {
			    finSummary = 1;
			}
		    }
		    if ( finSummary == 1 ) {
			kill = callback->bugSumCall(bugsum, callback->CallbackData);
		    }

	        } else if ( strcmp ( name, "MetricSummaries") == 0 && callback->metricSumCall != NULL ) {
		    MetricSummary * metricsum = NULL;
		    MetricSummary * temp = NULL;
		    int finSummary = 0;
		    while (ret == 1 && !finSummary) {
			ret = xmlTextReaderRead(reader);
			name = (char *) xmlTextReaderName(reader);
			type = xmlTextReaderNodeType(reader); 
			if (type == 1) {
			    if ( strcmp(name, "MetricSummary") == 0 ) {
				temp = calloc(1, sizeof(MetricSummary));
			    } else if ( strcmp(name, "Type") == 0 ) {
				temp->type = (char *) xmlTextReaderReadInnerXml(reader);
			    } else if ( strcmp(name, "Count") == 0 ) {
				char * text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->count = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Minimum") == 0 ) {
				char * text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->min = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Maximum") == 0 ) {
				char * text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->max = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Average") == 0 ) {
				char * text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->average = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "StandardDeviation") == 0 ) {
				char * text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->stdDeviation = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Sum") == 0 ) {
				char * text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->sum = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "SumOfSquares") == 0 ) {
				char * text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->sumOfSquares = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    }
			} else if ( type == 15 ) {
			    if ( strcmp(name, "MetricSummary") == 0 ) {
				if ( metricsum == NULL ) {
                                    metricsum = temp;
                                } else {
                                    MetricSummary * curr = metricsum;
                                    while (curr->next != NULL) {
                                        curr = curr->next;
                                    }
                                    curr->next = temp;
				}
			    } else if ( strcmp(name, "MetricSummaries") == 0 ) {
				kill = callback->metricSumCall(metricsum, callback->CallbackData);
				finSummary = 1;
			    } else if ( strcmp(name, "AnalyzerReport") == 0 ) {
				if ( callback->finishCallback != NULL ) {
				    kill = callback->finishCallback(kill, callback->CallbackData);
				    finished = 1;
				}

			    }
			}
		    }
	        }
	    }
	}
	if (ret != 0) {
	    printf("Failed to parse set file\n");
	    return NULL;
	} else if ( kill != NULL and !finished ) {
	    if ( callback->finishCallback != NULL ) {
		kill = callback->finishCallback(kill, callback->CallbackData);
	    }
	}
	
    } else {
	printf("ScarfToHash set to invalid file\n");
	return NULL;
    } 
    return kill;
}



//////////////////Close parser////////////////////////////////////////////
int closeScarfToHash(ScarfToHash * reader)
{
    return xmlTextReaderClose(reader->reader);
}




