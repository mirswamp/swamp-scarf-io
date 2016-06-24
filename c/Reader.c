#include <stdio.h>
#include <string.h>
//#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include "filestructure.h"

/////////////////////////TypeDef for Clarity///////////////////////////////////////

typedef xmlTextReaderPtr Reader;
/*/////////////////////////Initial Data Struct///////////////////////////////////////
typedef struct Initial{
    char *tool_name;
    char *tool_version;
    char *uuid;
} Initial;


/////////////////////////Metric Structs////////////////////////////////////////////
typedef struct Metric{
    int id;
    char *value;
    char *class;
    char *method;
    char *sourceFile;
    char *type;
} Metric;


///////////////////////////Bug Structs////////////////////////////////////////////
typedef struct Method{
    int methodId;
    int primary;
    char *name;
    struct Method *next;
} Method;


typedef struct Location{
    int primary;
    int startLine;
    int endLine;
    int startColumn;
    int endColumn;
    int locationId;
    char *explanation;
    char *sourceFile;
    struct Location *next;
} Location;


typedef struct {
    int start;
    int end;
} LineNum;


typedef struct {
    LineNum lineNum;
    char *xPath;
} InstanceLocation;


typedef struct CweIds{
    int cweid;
    struct CweIds *next;
} CweIds;


typedef struct {
    int bugId;
    CweIds *cweIds;
    char *className;
    char *bugSeverity;
    char *bugRank;
    char *resolutionSuggestion;
    char *bugMessage;
    char *bugCode;
    char *bugGroup;
    char *assessmentReportFile;
    char *buildId;
    InstanceLocation *instanceLocation;
    Method *methods;
    Location *bugLocations;
} BugInstance;

*/
///////////////Initiailize a BugInstance//////////////////////////////////////////////
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


///////////////////////////////Free a BugInstance//////////////////////////////////
int freeInitial(Initial * initial){
    xmlFree((xmlChar *) initial->tool_name);   
    xmlFree((xmlChar *) initial->tool_version);   
    xmlFree((xmlChar *) initial->uuid);   
    free(initial);
    return 1;
}


///////////////////////////////Free a BugInstance///////////////////////////////////
int freeMetric(Metric * metric) 
{
    xmlFree((xmlChar *) metric->type);
    xmlFree((xmlChar *) metric->class);
    xmlFree((xmlChar *) metric->method);
    xmlFree((xmlChar *) metric->sourceFile);
    xmlFree((xmlChar *) metric->value);
    free(metric);
    return 1;
}


///////////////////////////////Free a BugInstance///////////////////////////////////
int freeBug(BugInstance * bug) 
{
    xmlFree((xmlChar *) bug->assessmentReportFile);
    xmlFree((xmlChar *) bug->buildId);
    xmlFree((xmlChar *) bug->bugCode);
    xmlFree((xmlChar *) bug->bugRank);
    xmlFree((xmlChar *) bug->className);
    xmlFree((xmlChar *) bug->bugSeverity);
    xmlFree((xmlChar *) bug->bugGroup);
    xmlFree((xmlChar *) bug->bugMessage);
    xmlFree((xmlChar *) bug->resolutionSuggestion);
    xmlFree((xmlChar *) bug->instanceLocation);

    CweIds * cwe = bug->cweIds;
    CweIds * prevCwe;
    while (cwe != NULL) {
	prevCwe = cwe;
	cwe = cwe->next;
	xmlFree(prevCwe);
    }

    Method * method = bug->methods;
    Method * prevMethod;
    while (method != NULL) {
	prevMethod = method;
	method = method->next;
	xmlFree((xmlChar *) prevMethod->name);
	xmlFree(prevMethod);
    }

    Location * loc = bug->bugLocations;
    Location * prevLoc;
    while (loc != NULL) {
	prevLoc = loc;
	loc = loc->next;	
	xmlFree((xmlChar *) prevLoc->sourceFile);
	xmlFree((xmlChar *) prevLoc->explanation);
	xmlFree(prevLoc);
    }

    free(bug);
    return 0;
}


////////////////////parse singular line of metric  file////////////////////////////////
int processMetric(xmlTextReaderPtr reader, Metric * metric)
{ 
    char * name = (char *) xmlTextReaderName(reader);
    int type = xmlTextReaderNodeType(reader);
    printf("%d:%s\n",type,name);
    if (type == 1) { 	
	if (strcmp(name, "Metric") == 0) {
	    char * temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
	    metric->id = strtol(temp, NULL, 10);
	    xmlFree((xmlChar *) temp);
	} else if (strcmp(name, "Value") == 0) {
	    metric->value = (char *) xmlTextReaderReadInnerXml(reader); 
	} else if (strcmp(name, "Type") == 0) {
	    metric->type = (char *) xmlTextReaderReadInnerXml(reader);	
	} else if (strcmp(name, "Class") == 0) {
	    metric->class = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "Method") == 0) {
	    metric->method = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "SourceFile") == 0) {
	    metric->sourceFile = (char *) xmlTextReaderReadInnerXml(reader);
	}

    } else if (type == 15 && strcmp(name, "Metric") == 0) {
	xmlFree((xmlChar *) name);
	return 1;
    }

    xmlFree((xmlChar *) name);
    return 0;
}


////////////////////parse singular line of bug file////////////////////////////////////
int processBug(Reader reader, BugInstance * bug) 
{
    char * name = (char *) xmlTextReaderName(reader);
    int type = xmlTextReaderNodeType(reader);
    
//    printf("%s type %d\n", name, type);
    
    if (type == 1) { 

//	printf("%s type %d\n", name, type);

	if (strcmp(name, "BugInstance") == 0) {
	    char * temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "id");
//	    printf("%s\n", temp);
	    bug->bugId = strtol(temp, NULL, 10);
//	    printf("%d\n", bug->bugId);
	    xmlFree((xmlChar *) temp);

	} else if (strcmp(name, "InstanceLocation") == 0) {
	    InstanceLocation * inst = calloc(1, sizeof(InstanceLocation));
	    bug->instanceLocation = inst;

	} else if (strcmp(name, "Xpath") == 0) {
	    bug->instanceLocation->xPath = (char *) xmlTextReaderReadInnerXml(reader);

	} else if (strcmp(name, "LineNum") == 0) {
//	    LineNum * lineNum = calloc(1, sizeof(LineNum));
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
	    method->name =  (char *) xmlTextReaderReadInnerXml(reader);
	    temp = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "primary");
	    if (strcmp(temp, "true") == 0) {
		method->primary = 1;
	    } else {
		method->primary = 0;
	    }
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
	    cur->explanation = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name,"SourceFile") == 0) {
	    Location *  cur = bug->bugLocations;
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
	    cur->sourceFile = (char *) xmlTextReaderReadInnerXml(reader);

	} else if (strcmp(name, "AssessmentReportFile") == 0) {
	    bug->assessmentReportFile = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "BuildId") == 0) {
	    bug->buildId = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "BugCode") == 0) {
	    bug->bugCode = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "BugRank") == 0) {
	    bug->bugRank = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "ClassName") == 0) {
	    bug->className = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "BugSeverity") == 0) {
	    bug->bugSeverity = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "BugGroup") == 0) {
	    bug->bugGroup = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "BugMessage") == 0) {
	    bug->bugMessage = (char *) xmlTextReaderReadInnerXml(reader);
	} else if (strcmp(name, "ResolutionSuggestion") == 0) {
	    bug->resolutionSuggestion = (char *) xmlTextReaderReadInnerXml(reader);
	}

    } else if (type == 15) {	
	if (strcmp(name, "BugInstance") == 0) {
	    xmlFree((xmlChar *) name);
	    return 1;
	} 
    }

//    int count;
//    for (count = 0; count < 14; count++) {
//    	printf("Name: %s --- Content: %s\n", bug[count].key, bug[count].value);
//    }
    xmlFree((xmlChar *) name); 
    return 0;
}


//////////////////////change filename/reset parser////////////////////////////////////////
Reader newReader(char * filename)
{    
    Reader reader = xmlNewTextReaderFilename(filename);
    return reader;
}

Initial * nextInitial(Reader reader)
{
    Initial * initial = calloc(1, sizeof(Initial));
    if (reader != NULL) {
	int foundInit = 0;
	int ret = xmlTextReaderRead(reader);
	while (ret == 1 && foundInit == 0) {
	    char * name = (char *) xmlTextReaderName(reader);
	    int type = xmlTextReaderNodeType(reader);
	    if (type == 1 && strcmp(name, "AnalyzerReport") == 0) {
		foundInit = 1;
		initial->tool_name = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "tool_name");
		initial->tool_version = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "tool_version");
		initial->uuid = (char *) xmlTextReaderGetAttribute(reader, (xmlChar *) "uuid");
	    } else {
		ret = xmlTextReaderRead(reader);
	    }
	}
	if (foundInit == 1) {
	    return initial;
	}
	if (ret != 0) {
	    printf("Failed to parse set file\n");
	}
    } else {
	printf("Reader set to invalid file\n");
    }
    return NULL;
}


//////////////////Close parser////////////////////////////////////////////
int closeReader(Reader reader)
{
    return xmlTextReaderClose(reader);
}


////////////////////Process next bug/////////////////////////////////////////
BugInstance * nextBug(Reader reader)
{   
    
    BugInstance * bug = initializeBug();
    int ret;
    int foundBug = 0;
    if (reader != NULL) {
	ret = xmlTextReaderRead(reader);
	while (ret == 1 && foundBug == 0) {
	    foundBug = processBug(reader, bug);
	    if (foundBug == 0) {
		ret = xmlTextReaderRead(reader);
	    }
	}
	if (foundBug == 1) {
	    return bug;
	}
	if (ret != 0) {
	    printf("Failed to parse set file\n");
	}
    } else {
	printf("Reader set to invalid file\n");
    }
    return NULL;
}


////////////////////Process next bug/////////////////////////////////////////
Metric * nextMetric(Reader reader)
{
    Metric * metric = initializeMetric();
    int ret;
    int foundMetric = 0;
    if (reader != NULL) {
	ret = xmlTextReaderRead(reader);
	while (ret == 1 && foundMetric == 0) {
	    foundMetric = processMetric(reader, metric);
	    if (foundMetric == 0) {
		ret = xmlTextReaderRead(reader);
	    }
	}
	if (foundMetric == 1) {
	    printf("%s", metric->value);
	    return metric;
	}
	if (ret != 0) {
	    printf("Failed to parse set file\n");
	}
    } else {
	printf("Reader set to invalid file\n");
    }
    return NULL;
}



/////////////////////////////////////Main Method for tests///////////////////////
/*int main(int argc, char **argv) 
{
    
    char * filename = "../perl/parsed_results.xml";//user input eventually
//    char * filename = "out.xml";
    Reader reader = newReader(filename);
    BugInstance * bug = nextBug(reader);
  
    while (bug != NULL) {
	freeBug(bug);
	bug = nextBug(reader);
    }

//    printf("assess: %s\n Location Start: %d\n", bug->assessmentReportFile, bug->bugLocations->startLine);

//    freeBug(bug);

//    bug = nextBug();

//    printf("ID#2: %d", bug->bugId);

//    newReader(filename);

//    bug = nextBug();
    
    return 0;
}*/
