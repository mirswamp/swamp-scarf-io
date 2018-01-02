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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/xmlreader.h>
#include "ScarfXml.h"

/////////////////////////TypeDef for Clarity///////////////////////////////////////


typedef struct Callback {
    BugCallback bugCall;
    MetricCallback  metricCall;
    InitialCallback initialCall;
    BugSummaryCallback bugSumCall;
    MetricSummaryCallback metricSumCall;
    FinalCallback finishCallback;
    void *CallbackData;
} Callback;


struct ScarfXmlReader{
    xmlTextReaderPtr reader;
    Callback *callback;
};

///////////////Initiailize a Metric//////////////////////////////////////////////
Metric *initializeMetric()
{
    Metric *metric = calloc(1,sizeof(Metric));
    return metric;
}


///////////////Initiailize a BugInstance//////////////////////////////////////////////
BugInstance *initializeBug()
{
    BugInstance *bug = calloc(1,sizeof(BugInstance));
    return bug;
}


///////////////////////////////free summaries///////////////////////////////////
void DeleteBugSummary(BugSummary *bugSummary){
    BugSummary *cur = bugSummary;
    BugSummary *prev = NULL;
    while (cur != NULL) {
	prev = cur;
	cur = cur->next;
	xmlFree((xmlChar *) prev->code);
	xmlFree((xmlChar *) prev->group);
	free(prev);
    }
    return;
}

void DeleteMetricSummary(MetricSummary *metricSummary){
    MetricSummary *cur = metricSummary;
    MetricSummary *prev = NULL;
    while (cur != NULL) {
	prev = cur;
	cur = cur->next;
	xmlFree((xmlChar *) prev->type);
	free(prev);
    }
    return;
}


///////////////////////////////Free initial struct//////////////////////////////////
void DeleteInitial(Initial *initial){

#define XmlFree(i, a) xmlFree((xmlChar *) i->a);
    XmlFree(initial, assess_fw);
    XmlFree(initial, assess_fw_version);
    XmlFree(initial, assessment_start_ts);
    XmlFree(initial, build_fw);
    XmlFree(initial, build_fw_version);
    XmlFree(initial, build_root_dir);
    XmlFree(initial, package_name);
    XmlFree(initial, package_root_dir);
    XmlFree(initial, package_version);
    XmlFree(initial, parser_fw);
    XmlFree(initial, parser_fw_version);
    XmlFree(initial, platform_name);
    XmlFree(initial, tool_name);
    XmlFree(initial, tool_version);
    XmlFree(initial, uuid);

    free(initial);
    return;
}


///////////////////////////////Free a Metric///////////////////////////////////
void DeleteMetric(Metric *metric)
{
    free(metric->type);
    free(metric->className);
    free(metric->methodName);
    free(metric->sourceFile);
    free(metric->value);
    free(metric);
    return;
}


///////////////////////////////Free a BugInstance///////////////////////////////////
void DeleteBug(BugInstance *bug)
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
    free(bug->instanceLocation.xPath);

//    CweIds *cwe = bug->cweIds;
//    CweIds *prevCwe;
//    while (cwe != NULL) {
//	prevCwe = cwe;
//	cwe = cwe->next;
//	free(prevCwe);
//    }
    if (bug->cweIds != NULL){
	free(bug->cweIds);
    }

    if (bug->methods != NULL){
	int i;
	for ( i = 0; i < bug->methodsCount; i++ ) {
	    free(bug->methods[i].name);
	}
	free(bug->methods);
    }

    if ( bug->locations != NULL) {
	int i;
	for ( i = 0; i < bug->locationsCount; i++ ) {
	    free(bug->locations[i].explanation);
	    free(bug->locations[i].sourceFile);
	}
	free(bug->locations);
    }
//    Method *method = bug->methods;
//    Method *prevMethod;
//    while (method != NULL) {
//	prevMethod = method;
//	method = method->next;
//	free(prevMethod->name);
//	free(prevMethod);
//    }

//    Location *loc = bug->bugLocations;
//    Location *prevLoc;
//    while (loc != NULL) {
//	prevLoc = loc;
//	loc = loc->next;
//	free(prevLoc->sourceFile);
//	free(prevLoc->explanation);
//	free(prevLoc);
//    }

    free(bug);
    return;
}


////////////////////////COPY DATA//////////////////////////////////////////////
BugInstance *CopyBug(BugInstance *bug) {
    BugInstance *ret = calloc(1, sizeof(BugInstance));
    ret->bugId = bug->bugId;
    if (bug->cweIds != NULL) {
	ret->cweIdsSize = bug->cweIdsSize;
	ret->cweIdsCount = bug->cweIdsCount;
	ret->cweIds = malloc(ret->cweIdsSize * sizeof(int));
	memcpy(ret->cweIds, bug->cweIds, bug->cweIdsSize * sizeof(int));
    }

    ret->instanceLocation = bug->instanceLocation;
    if (bug->className != NULL) {
	ret->className =  malloc(strlen(bug->className) + 1);
	strcpy(ret->className, bug->className);
    }
    if (bug->bugSeverity != NULL) {
	ret->bugSeverity = malloc(strlen(bug->bugSeverity) + 1);
	strcpy(ret->bugSeverity, bug->bugSeverity);
    }
    if (bug->bugRank != NULL) {
	ret->bugRank = malloc(strlen(bug->bugRank) + 1);
	strcpy(ret->bugRank, bug->bugRank);
    }
    if (bug->resolutionSuggestion != NULL) {
	ret->resolutionSuggestion = malloc(strlen(bug->resolutionSuggestion) + 1);
	strcpy(ret->resolutionSuggestion, bug->resolutionSuggestion);
    }
    if (bug->bugMessage != NULL) {
	ret->bugMessage = malloc(strlen(bug->bugMessage) + 1);
	strcpy(ret->bugMessage, bug->bugMessage);
    }
    if (bug->bugCode != NULL) {
	ret->bugCode = malloc(strlen(bug->bugCode) + 1);
	strcpy(ret->bugCode, bug->bugCode);
    }
    if (bug->bugGroup != NULL) {
	ret->bugGroup =  malloc(strlen(bug->bugGroup) + 1 );
	strcpy(ret->bugGroup, bug->bugGroup);
    }
    if (bug->assessmentReportFile != NULL) {
	ret->assessmentReportFile = malloc(strlen(bug->assessmentReportFile) + 1);
	strcpy(ret->assessmentReportFile, bug->assessmentReportFile);
    }
    if (bug->buildId != NULL) {
	ret->buildId = malloc(strlen(bug->buildId) + 1);
	strcpy(ret->buildId, bug->buildId);
    }
    if (bug->methods != NULL) {
	ret->methodsSize = bug->methodsSize;
	ret->methodsCount = bug->methodsCount;
	ret->methods =  malloc(ret->methodsSize * sizeof(Method));
	int i;
	for ( i = 0; i < ret->methodsCount; i++ ) {
	    ret->methods[i].methodId = bug->methods[i].methodId;
	    ret->methods[i].primary = bug->methods[i].primary;
	    ret->methods[i].name =  malloc(strlen(bug->methods[i].name) + 1);
	    strcpy(ret->methods[i].name, bug->methods[i].name);
	}
    }
    if (bug->locations != NULL) {
	ret->locationsSize = bug->locationsSize;
	ret->locationsCount = bug->locationsCount;
	ret->locations =  malloc(ret->locationsSize * sizeof(Location));
	int i;
	for ( i = 0; i < ret->locationsCount; i++ ) {
	    Location * retloc = &ret->locations[i];
	    Location * bugloc = &bug->locations[i];
	    retloc->locationId = bugloc->locationId;
	    retloc->primary =  bugloc->primary;
	    retloc->startLine = bugloc->startLine;
	    retloc->endLine = bugloc->endLine;
	    retloc->startColumn = bugloc->startColumn;
	    retloc->endColumn = bugloc->endColumn;
	    retloc->explanation =  malloc(strlen(bugloc->explanation) + 1);
	    strcpy(retloc->explanation, bugloc->explanation);
	    retloc->sourceFile =  malloc(strlen(bugloc->sourceFile) + 1);
	    strcpy(retloc->sourceFile, bugloc->sourceFile);
	}
    }
    return ret;
}


Metric *CopyMetric(Metric *metr) {
    Metric *ret = calloc(1, sizeof(Metric));
    ret->id = metr->id;
    if ( metr->value != NULL) {
	ret->value = malloc(strlen(metr->value) + 1);
	strcpy(ret->value, metr->value);
    }
    if ( metr->className != NULL) {
	ret->className = malloc(strlen(metr->className) + 1);
	strcpy(ret->className, metr->className);
    }
    if (metr->methodName != NULL) {
	ret->methodName = malloc(strlen(metr->methodName) + 1);
	strcpy(ret->methodName, metr->methodName);
    }
    if ( metr->sourceFile != NULL) {
    ret->sourceFile = malloc(strlen(metr->sourceFile) + 1);
    strcpy(ret->sourceFile, metr->sourceFile);
    }
    if (metr->type != NULL){
	ret->type = malloc(strlen(metr->type) + 1);
	strcpy(ret->type, metr->type);
    }
    return ret;
}

Initial *CopyInitial(Initial *init) {
    Initial *ret = malloc(sizeof(Initial));

#define CopyAttr(dst, src, a) if (src->a != NULL)  { \
	    dst->a = malloc(strlen(src->a) + 1); \
	    strcpy(dst->a, src->a); \
	}

    CopyAttr(ret, init,  assess_fw);
    CopyAttr(ret, init,  assess_fw_version);
    CopyAttr(ret, init,  assessment_start_ts);
    CopyAttr(ret, init,  build_fw);
    CopyAttr(ret, init,  build_fw_version);
    CopyAttr(ret, init,  build_root_dir);
    CopyAttr(ret, init,  package_name);
    CopyAttr(ret, init,  package_root_dir);
    CopyAttr(ret, init,  package_version);
    CopyAttr(ret, init,  parser_fw);
    CopyAttr(ret, init,  parser_fw_version);
    CopyAttr(ret, init,  platform_name);
    CopyAttr(ret, init,  tool_name);
    CopyAttr(ret, init,  tool_version);
    CopyAttr(ret, init,  uuid);

    return ret;
}


//////////////////Generic clear leading/trailing whitespace method//////////////////////
char *trim(char *str)
{
    char *end;
    while ( isspace(*str) ) {
	str++;
    }

    end = str + strlen(str);
    while ( end > str && isspace(*(end - 1)) ) {
	--end;
    }

    int strLen = end - str;
    char *newStr = malloc(strLen + 1);
    memcpy(newStr, str, strLen);
    newStr[strLen] = '\0';

    return newStr;
}


////////////////////parse singular line of metric  file////////////////////////////////
int processMetric(xmlTextReaderPtr reader, Metric *metric)
{
    char *name = (char *) xmlTextReaderName(reader);
    int type = xmlTextReaderNodeType(reader);

    //type 1 == start tag
    if (type == 1) {
	if (strcmp(name, "Metric") == 0) {
	    char *temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    metric->id = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Value") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    metric->value = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Type") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    metric->type = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Class") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    metric->className = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Method") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    metric->methodName = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "SourceFile") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
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
int processBug(xmlTextReaderPtr reader, BugInstance *bug)
{
    char *name = (char *) xmlTextReaderName(reader);
    int type = xmlTextReaderNodeType(reader);

    //start tags
    if (type == 1) {

	if (strcmp(name, "BugInstance") == 0) {
	    char *temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    bug->bugId = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);

//	} else if (strcmp(name, "InstanceLocation") == 0) {
//	    InstanceLocation *inst = calloc(1, sizeof(InstanceLocation));
//	    bug->instanceLocation = inst;

	} else if (strcmp(name, "Xpath") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->instanceLocation.xPath = trim(temp);
	    xmlFree((xmlChar *) temp);

//	} else if (strcmp(name, "LineNum") == 0) {
//	    LineNum lineNum = {0};
//	    bug->instanceLocation->lineNum = lineNum;

	} else if (strcmp(name, "Start") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->instanceLocation.lineNum.start = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);

	} else if (strcmp(name, "End") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->instanceLocation.lineNum.end = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);

	}  else if (strcmp(name, "CweId") == 0) {

	    if ( bug->cweIds == NULL ) {
		bug->cweIdsSize = 5;
		bug->cweIdsCount = 0;
		bug->cweIds = malloc(bug->cweIdsSize * sizeof(int));
	    }
	    if ( bug->cweIdsCount >= bug->cweIdsSize ) {
		bug->cweIdsSize = bug->cweIdsSize * 2;
		int *tempArray = realloc(bug->cweIds, bug->cweIdsSize * sizeof(int));
		if (tempArray) {
		     bug->cweIds = tempArray;
		} else {
		    printf("Could not expand CweID array. Exiting parsing");
		    exit(1);
		}
	    }
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->cweIds[bug->cweIdsCount] = strtol(temp, NULL, 10);
	    bug->cweIdsCount++;
	    xmlFree((xmlChar *) temp);


    	} else if (strcmp(name, "Method") == 0){
	    if ( bug->methods == NULL ) {
		bug->methodsSize = 5;
		bug->methodsCount = 0;
		bug->methods = calloc(1, bug->methodsSize * sizeof(Method));
	    }
	    if ( bug->methodsCount >= bug->methodsSize ) {
		bug->methodsSize = bug->methodsSize * 2;
		int *tempArray = realloc(bug->methods, bug->methodsSize * sizeof(Method));
		if (tempArray) {
		     bug->methods = (Method *)tempArray;
		     memset(&bug->methods[bug->methodsCount], 0, (bug->methodsSize/2) * sizeof(Method));
		} else {
		    printf("Could not expand Methods array. Exiting parsing");
		    exit(1);
		}
	    }
	    Method *method = &bug->methods[bug->methodsCount];
	    char *temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
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
	    bug->methodsCount++;

	} else if (strcmp(name, "Location") == 0) {
	    if ( bug->locations == NULL ) {
		bug->locationsSize = 5;
		bug->locationsCount = 0;
		bug->locations = calloc(1, bug->locationsSize * sizeof(Location));
	    }
	    if ( bug->locationsCount >= bug->locationsSize ) {
		bug->locationsSize = bug->locationsSize * 2;
		int *tempArray = realloc(bug->locations, bug->locationsSize * sizeof(Location));
		if (tempArray) {
		     bug->locations = (Location *)tempArray;
		     memset(&bug->locations[bug->locationsCount], 0, (bug->locationsSize/2) * sizeof(Location));
		} else {
		    printf("Could not expand Locations  array. Exiting parsing");
		    exit(1);
		}
	    }
	    //Location *loc = calloc(1, sizeof(Location));
	    Location *loc = &bug->locations[bug->locationsCount];
	    char *temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    loc->locationId = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	    temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "primary");
	    printf("%s\n", temp);
	    if (strcmp(temp, "true") == 0) {
		loc->primary = 1;
    	    } else {
		loc->primary = 0;
	    }
	    xmlFree((xmlChar *) temp);
	    bug->locationsCount++;

	} else if (strcmp(name,"StartLine") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location * cur = &bug->locations[bug->locationsCount-1];
	    cur->startLine = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"EndLine") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location * cur = &bug->locations[bug->locationsCount-1];
	    cur->endLine = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"StartColumn") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location * cur = &bug->locations[bug->locationsCount-1];
	    cur->startColumn = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"EndColumn") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    Location * cur = &bug->locations[bug->locationsCount-1];
	    cur->endColumn = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"Explanation") == 0) {
	    Location * cur = &bug->locations[bug->locationsCount-1];
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    cur->explanation = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name,"SourceFile") == 0) {
	    Location * cur = &bug->locations[bug->locationsCount-1];
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    cur->sourceFile = trim(temp);
	    xmlFree((xmlChar *) temp);

	} else if (strcmp(name, "AssessmentReportFile") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->assessmentReportFile = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BuildId") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->buildId = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugCode") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->bugCode = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugRank") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->bugRank = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "ClassName") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->className = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugSeverity") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->bugSeverity = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugGroup") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->bugGroup = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "BugMessage") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
	    bug->bugMessage = trim(temp);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "ResolutionSuggestion") == 0) {
	    char *temp = (char *) xmlTextReaderReadInnerXml(reader);
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

ScarfXmlReader *NewScarfXmlReaderFromFilename(char *filename, char *encoding)
{
    struct Callback *calls= calloc(1, sizeof(struct Callback));
    ScarfXmlReader *reader = malloc(sizeof(ScarfXmlReader));
    reader->reader = xmlReaderForFile(filename, encoding, 0);
    reader->callback = calls;
    return reader;
}
ScarfXmlReader *NewScarfXmlReaderFromString(char *str, char *encoding)
{
    struct Callback *calls= calloc(1, sizeof(struct Callback));
    ScarfXmlReader *reader = malloc(sizeof(ScarfXmlReader));
    reader->reader = xmlReaderForDoc((xmlChar *)str, NULL, encoding, 0);
    reader->callback = calls;
    return reader;
}
ScarfXmlReader *NewScarfXmlReaderFromFd(int fd, char *encoding)
{
    struct Callback *calls= calloc(1, sizeof(struct Callback));
    ScarfXmlReader *reader = malloc(sizeof(ScarfXmlReader));
    reader->reader = xmlReaderForFd(fd, NULL, encoding, 0);
    reader->callback = calls;
    return reader;
}
ScarfXmlReader *NewScarfXmlReaderFromMemory(char *loc, int size, char *encoding)
{
    struct Callback *calls= calloc(1, sizeof(struct Callback));
    ScarfXmlReader *reader = malloc(sizeof(ScarfXmlReader));
    reader->reader = xmlReaderForMemory(loc, size, NULL, encoding, 0);
    reader->callback = calls;
    return reader;
}



void SetBugCallback(ScarfXmlReader *reader, BugCallback callback) {
    reader->callback->bugCall = callback;
}
void SetMetricCallback(ScarfXmlReader *reader, MetricCallback callback) {
    reader->callback->metricCall = callback;
}
void SetBugSummaryCallback(ScarfXmlReader *reader, BugSummaryCallback callback) {
    reader->callback->bugSumCall = callback;
}
void SetMetricSummaryCallback(ScarfXmlReader *reader, MetricSummaryCallback callback) {
    reader->callback->metricSumCall = callback;
}
void SetFinalCallback(ScarfXmlReader *reader, FinalCallback callback) {
    reader->callback->finishCallback = callback;
}
void SetInitialCallback(ScarfXmlReader *reader, InitialCallback callback) {
    reader->callback->initialCall = callback;
}
void SetCallbackData(ScarfXmlReader *reader, void *callbackData) {
    reader->callback->CallbackData = callbackData;
}


BugCallback GetBugCallback(ScarfXmlReader *reader) {
    return reader->callback->bugCall;
}
MetricCallback GetMetricCallback(ScarfXmlReader *reader) {
    return reader->callback->metricCall;
}
BugSummaryCallback GetBugSummaryCallback(ScarfXmlReader *reader) {
    return reader->callback->bugSumCall;
}
MetricSummaryCallback GetMetricSummaryCallback(ScarfXmlReader *reader) {
    return reader->callback->metricSumCall;
}
FinalCallback GetFinalCallback(ScarfXmlReader *reader) {
    return reader->callback->finishCallback;
}
InitialCallback GetInitialCallback(ScarfXmlReader *reader) {
    return reader->callback->initialCall;
}
void *GetCallbackData(ScarfXmlReader *reader) {
    return reader->callback->CallbackData;
}

int _clearBug(BugInstance * bug) {
    free(bug->assessmentReportFile);
    free(bug->buildId);
    free(bug->bugCode);
    free(bug->bugRank);
    free(bug->className);
    free(bug->bugSeverity);
    free(bug->bugGroup);
    free(bug->bugMessage);
    free(bug->resolutionSuggestion);
    free(bug->instanceLocation.xPath);
    free(bug->cweIds);


    int i;
    if ( bug->methods != NULL ) {
	for ( i = 0; i < bug->methodsCount; i++ ) {
	    free(bug->methods[i].name);
	}
	free(bug->methods);
    }
    for ( i = 0; i < bug->locationsCount; i++ ) {
        free(bug->locations[i].explanation);
        free(bug->locations[i].sourceFile);
    }
    free(bug->locations);

    memset(bug, 0, sizeof(BugInstance));
    return 0;
}

int  _clearMetric(Metric * metric) {
    free(metric->type);
    free(metric->className);
    free(metric->methodName);
    free(metric->sourceFile);
    free(metric->value);
    memset(metric, 0, sizeof(Metric));
    return 0;
}

void * Parse(ScarfXmlReader *hand)
{
    void *kill = NULL;
    xmlTextReaderPtr reader = hand->reader;
    if (reader != NULL) {
	char *name;
	int finished = 0;
	int type;
	int ret = 1;
	BugInstance *bug = initializeBug();
	Metric *metric = initializeMetric();
	Callback *callback = hand->callback;
        while (ret == 1 && kill == NULL) {
	    ret = xmlTextReaderRead(reader);
	    name = (char *) xmlTextReaderName(reader);
	    type = xmlTextReaderNodeType(reader);
	    if ( type == 1 ) {
	        if ( strcmp ( name, "AnalyzerReport" ) == 0 && callback->initialCall != NULL ) {
		    Initial *initial = calloc(1, sizeof(Initial));

#define SetXmlAttr(r, i, a) i->a = (char *) xmlTextReaderGetAttribute(r, (xmlChar *) #a)
		    SetXmlAttr(reader, initial, assess_fw);
		    SetXmlAttr(reader, initial, assess_fw_version);
		    SetXmlAttr(reader, initial, assessment_start_ts);
		    SetXmlAttr(reader, initial, build_fw);
		    SetXmlAttr(reader, initial, build_fw_version);
		    SetXmlAttr(reader, initial, build_root_dir);
		    SetXmlAttr(reader, initial, package_name);
		    SetXmlAttr(reader, initial, package_root_dir);
		    SetXmlAttr(reader, initial, package_version);
		    SetXmlAttr(reader, initial, parser_fw);
		    SetXmlAttr(reader, initial, parser_fw_version);
		    SetXmlAttr(reader, initial, platform_name);
		    SetXmlAttr(reader, initial, tool_name);
		    SetXmlAttr(reader, initial, tool_version);
		    SetXmlAttr(reader, initial, uuid);

		    kill = callback->initialCall(initial, callback->CallbackData);
		    DeleteInitial(initial);

	        } else if ( strcmp ( name, "BugInstance" ) == 0 && callback->bugCall != NULL ) {
		    int foundBug = 0;
		    //iBugInstance *bug = initializeBug();
		    while (ret == 1 && foundBug == 0) {
			foundBug = processBug(reader, bug);
			if (foundBug == 0) {
			    ret = xmlTextReaderRead(reader);
			}
		    }
		    if (foundBug == 1) {
			kill = callback->bugCall(bug, callback->CallbackData);
		    }
		    _clearBug(bug);

	        } else if ( strcmp ( name, "Metric" ) == 0 && callback->metricCall != NULL ) {
		    //Metric *metric = initializeMetric();
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
		    _clearMetric(metric);

	        } else if ( strcmp ( name, "BugSummary" ) == 0 && callback->bugSumCall !=NULL ) {
		    BugSummary *bugsum = NULL;
		    int finSummary = 0;
		    while (ret == 1 && !finSummary) {
			ret = xmlTextReaderRead(reader);
			name = (char *) xmlTextReaderName(reader);
			type = xmlTextReaderNodeType(reader);
			if (type == 1) {
			    if (strcmp(name, "BugCategory") == 0) {
				BugSummary *temp = calloc(1, sizeof(BugSummary));
				char *att = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "count");
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
				    BugSummary *curr = bugsum;
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
		    MetricSummary *metricsum = NULL;
		    MetricSummary *temp = NULL;
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
				char *text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->count = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Minimum") == 0 ) {
				char *text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->min = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Maximum") == 0 ) {
				char *text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->max = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Average") == 0 ) {
				char *text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->average = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "StandardDeviation") == 0 ) {
				char *text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->stdDeviation = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "Sum") == 0 ) {
				char *text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->sum = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    } else if ( strcmp(name, "SumOfSquares") == 0 ) {
				char *text = (char *) xmlTextReaderReadInnerXml(reader);
				temp->sumOfSquares = strtod(text, NULL);
				xmlFree((xmlChar *) text);
			    }
			} else if ( type == 15 ) {
			    if ( strcmp(name, "MetricSummary") == 0 ) {
				if ( metricsum == NULL ) {
                                    metricsum = temp;
                                } else {
                                    MetricSummary *curr = metricsum;
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
	    return (void*)-1;
	} else if ( kill != NULL && !finished ) {
	    if ( callback->finishCallback != NULL ) {
		kill = callback->finishCallback(kill, callback->CallbackData);
	    }
	}

    } else {
	printf("ScarfXmlReader set to invalid file\n");
	return (void*)-1;
    }
    return kill;
}



//////////////////Close parser////////////////////////////////////////////
int DeleteScarfXmlReader(ScarfXmlReader *reader)
{
    return xmlTextReaderClose(reader->reader);
}




