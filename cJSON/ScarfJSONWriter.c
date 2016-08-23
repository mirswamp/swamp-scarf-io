#include <yajl/yajl_gen.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "filestructure.h"



////////////////////structs///////////////////////////

typedef struct ScarfJSONWriter {
    int bugId;
    int metricId;
    int errorLevel;
    int openBody;
    int start;
    int filetype;
    int pretty;
    int utf8;
//    char * filename;
    FILE * file;
    char curr[20];
    struct BugSummaries * bugSums;
    struct MetricSummary * metricSum;
    yajl_gen writer;
} ScarfJSONWriter;


//////////////constructor////////////////////
ScarfJSONWriter * NewScarfJSONWriterFromFile(FILE * file)
{
    ScarfJSONWriter * writerInfo = calloc(1, sizeof(ScarfJSONWriter));
    writerInfo->writer = yajl_gen_alloc(NULL);
    yajl_gen_config(writerInfo->writer, yajl_gen_beautify, 1);
    yajl_gen_config(writerInfo->writer, yajl_gen_validate_utf8 , 1);
    writerInfo->pretty = 1;
    writerInfo->utf8 = 1;
//    writerInfo->filename = malloc(strlen(filename) + 1);
    writerInfo->file = file;//fopen(filename, "w");
    writerInfo->filetype = 0;
    writerInfo->bugId = 1;
    writerInfo->metricId = 1;
    writerInfo->errorLevel = 1;
//    strcpy(writerInfo->filename, filename);
    return writerInfo;
}
ScarfJSONWriter * NewScarfJSONWriterFromFilename(char * filename)
{
    ScarfJSONWriter * writerInfo = calloc(1, sizeof(ScarfJSONWriter));
    writerInfo->writer = yajl_gen_alloc(NULL);
    yajl_gen_config(writerInfo->writer, yajl_gen_beautify, 1);
    yajl_gen_config(writerInfo->writer, yajl_gen_validate_utf8 , 1);
//    writerInfo->filename = malloc(strlen(filename) + 1);
    writerInfo->file = fopen(filename, "w");
    if (writerInfo->file == NULL){
        printf("File could not open\n");
        free(writerInfo);
        return NULL;
    }
    writerInfo->bugId = 1;
    writerInfo->metricId = 1;
    writerInfo->errorLevel = 1;
//    strcpy(writerInfo->filename, filename);
    writerInfo->filetype = 1;
    return writerInfo;
}
ScarfJSONWriter * NewScarfJSONWriterFromString(char * str, size_t *size)
{
    ScarfJSONWriter * writerInfo = calloc(1, sizeof(ScarfJSONWriter));
    writerInfo->writer = yajl_gen_alloc(NULL);
    yajl_gen_config(writerInfo->writer, yajl_gen_beautify, 1);
    yajl_gen_config(writerInfo->writer, yajl_gen_validate_utf8 , 1);
//    writerInfo->filename = malloc(strlen(filename) + 1);
    writerInfo->file = open_memstream (&str, size);
    if (writerInfo->file == NULL){
        printf("File could not open\n");
        free(writerInfo);
        return NULL;
    }
    writerInfo->bugId = 1;
    writerInfo->metricId = 1;
    writerInfo->errorLevel = 1;
//    strcpy(writerInfo->filename, filename);
    writerInfo->filetype = 2;
    return writerInfo;
}


void DeleteScarfJSONWriter (ScarfJSONWriter * writerInfo)
{
    yajl_gen_free(writerInfo->writer);
    free(writerInfo->bugSums);
    free(writerInfo->metricSum);
    if (writerInfo->filetype == 1 || writerInfo->filetype == 2) {
	fclose(writerInfo->file);
    }
//    free(writerInfo->filename);
    free(writerInfo);
}

////////////////////////change options////////////////////////////////
void SetPretty ( ScarfJSONWriter * writerInfo, int pretty_level ) {
    yajl_gen_config(writerInfo->writer, yajl_gen_beautify, pretty_level); 
    writerInfo->pretty = pretty_level;   
}

void SetUTF8 (ScarfJSONWriter * writerInfo, int utf8){
    yajl_gen_config(writerInfo->writer, yajl_gen_validate_utf8 , 1);
    writerInfo->utf8 = utf8;
}

int GetPretty (ScarfJSONWriter * writerInfo) {
    return writerInfo->pretty;
}

int GetUTF8 (ScarfJSONWriter * writerInfo) {
    return writerInfo->utf8;
}

yajl_gen  getScarfJSONWriter (ScarfJSONWriter * writerInfo)
{
    if (writerInfo != NULL){
	return writerInfo->writer;
    } else return NULL;
}


int getErrorLevel(ScarfJSONWriter * writerInfo)
{
    if (writerInfo != NULL){
	return writerInfo->errorLevel;
    } else return -1; 
}


int SetErrorLevel(ScarfJSONWriter * writerInfo, int errorLevel)
{
    if (writerInfo != NULL){
	if ( errorLevel == 0 || errorLevel == 1 || errorLevel == 2 ) { 
	    writerInfo->errorLevel = errorLevel;
	} else return 1;
    } else return 1;
    return 0;
}


////////////////////Write a bug/////////////////////////////////////////////
char * CheckBug(BugInstance * bug)
{
    char * errors = malloc(strlen("\0") + 1);
    errors[0] = '\0';
    char * temp = malloc(140);

//    printf("errors: %p::%s\n", errors, errors);
    if (bug->bugLocations == NULL) {
        sprintf(temp, "Required element: BugLocations could not be found in BugInstance\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    if (bug->bugMessage == NULL) {
        sprintf(temp, "Required element: BugMessage could not be found in BugInstance\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    if (bug->buildId == NULL) {
        sprintf(temp, "Required element: BuildId could not be found in BugInstance\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    if (bug->assessmentReportFile == NULL) {
        sprintf(temp, "Required element: AssessmentReportFile could not be found in BugInstance\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    if (bug->methods != NULL) {
        int methodID = 1;
        int methodPrimary = 0;
        Methods * methods = bug->methods;
	int i;
	for ( i = 0 ; i < methods->count ; i++ ) {
	    Method *method = &methods->methods[i];
	    if (method->primary != 0 && method->primary != 1) {
	        sprintf(temp, "Invalid primary attribute for Method:%d in BugInstance\n", methodID);
	        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
	        errors = strcat(errors, temp);
	    } else if (method->primary) {
	        if (methodPrimary) {
		    sprintf(temp, "Multiple primary methods in BugInstance\n");
		    errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
		    errors = strcat(errors, temp);
	        } else {
		    methodPrimary = 1;
	        }
	    }
	    if (method->name == NULL) {
	        sprintf(temp, "Required text not found: name of Method: %d in BugInstance\n", methodID);
	        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
	        errors = strcat(errors, temp);
	    }
	    methodID++;
	}
	
        if (methodPrimary == 0 && methodID != 1) {
            sprintf(temp, "Misformed Element: No primary Method found in  BugInstance\n");
            errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
            errors = strcat(errors, temp);
        }
    }

    BugLocations * buglocs = bug->bugLocations;
    int locID = 1;
    int locPrimary = 0;
    if (buglocs != NULL) {
	int i;
	for ( i = 0 ; i < buglocs->count ; i++ ) {
	    Location *loc = &buglocs->locations[i];
	    if (loc->primary != 0 && loc->primary != 1) {
		sprintf(temp, "Invalid primary attribute for a Location:%d in BugInstance\n", locID);
		errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
		errors = strcat(errors, temp);
	    } else if (loc->primary) {
		/* if (locPrimary) {
		    sprintf(temp, "Multiple primary Locations in BugInstance: %d\n", bugID);
		    errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
		    errors = strcat(errors, temp);
		} else { */
		    locPrimary = 1;
		// }
	    }
	    if (loc->sourceFile == NULL) {
		sprintf(temp, "Required Element: SourceFile of Location:%d in BugInstance\n", locID);
		errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
		errors = strcat(errors, temp);
	    }
	locID++;
	}
    }
    if (locPrimary == 0) {
        sprintf(temp, "Misformed Element: No primary Location found in  BugInstance\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }

    if (bug->instanceLocation.lineNum.start == 0 && bug->instanceLocation.lineNum.end == 0 && bug->instanceLocation.xPath == NULL) {
        sprintf(temp, "Misformed Element: Neither LineNum or Xpath children were present in InstanceLocation BugInstance\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    
    free(temp);
    return errors;
}


int AddBug(ScarfJSONWriter * writerInfo, BugInstance * bug)
{
    if (writerInfo->errorLevel != 0) {
        if (strcmp(writerInfo->curr, "summary") == 0) {
            printf("Summary already written. Invalid Scarf.\n");
            if (writerInfo->errorLevel == 2) {
                exit(1);
            }
        }
        if (writerInfo->start == 0) {
            printf("Scarf file closed.\n");
            if (writerInfo->errorLevel == 2) {
                exit(1);
            }
        }
        char * errors = NULL;
        errors = CheckBug(bug);
        if ( strcmp(errors,"") != 0 ) {
            printf("%s", errors);
            if ( writerInfo->errorLevel == 2 ) {
                exit(1);
            }
        }
        free(errors);
    }

    yajl_gen writer = writerInfo->writer;


    if ( strcmp (writerInfo->curr, "metric") == 0 ) {
        yajl_gen_array_close(writer);
    } else if (strcmp(writerInfo->curr, "bug") != 0) {
        yajl_gen_string(writer, "BugInstances", 12);
        yajl_gen_array_open(writer);
        strcpy (writerInfo->curr, "bug");
	writerInfo->openBody = 1;
    }
    char temp [1024];
    int tempLen;
    
    yajl_gen_map_open(writer);
    
    
    yajl_gen_string(writer, "BugId", 5);
    
    tempLen = sprintf(temp, "%d", writerInfo->bugId);
    yajl_gen_number(writer, temp, tempLen);

    if ( bug->buildId != NULL ) {
	yajl_gen_string(writer, "BuildId", 7);
	tempLen = sprintf(temp, "%s", bug->buildId);
	yajl_gen_string(writer, temp, tempLen);
    }
    if ( bug->bugCode != NULL ) {
	yajl_gen_string(writer, "BugCode", 7);
	tempLen = sprintf(temp, "%s", bug->bugCode);
	yajl_gen_string(writer, temp, tempLen);
    }
    if ( bug->bugRank != NULL ) {
	yajl_gen_string(writer, "BugRank", 7);
	tempLen = sprintf(temp, "%s", bug->bugRank);
	yajl_gen_string(writer, temp, tempLen);
    }
    if ( bug->className != NULL ) {
	yajl_gen_string(writer, "ClassName", 9);
	tempLen = sprintf(temp, "%s", bug->className);
	yajl_gen_string(writer, temp, tempLen);
    }
    if ( bug->bugSeverity != NULL ) {
	yajl_gen_string(writer, "BugSeverity", 11);
	tempLen = sprintf(temp, "%s", bug->bugSeverity);
	yajl_gen_string(writer, temp, tempLen);
    }
    if ( bug->bugGroup != NULL ) {
	yajl_gen_string(writer, "BugGroup", 8);
	tempLen = sprintf(temp, "%s", bug->bugGroup);
	yajl_gen_string(writer, temp, tempLen);
    }
    if ( bug->bugMessage != NULL ) {
	yajl_gen_string(writer, "BugMessage", 10);
	tempLen = sprintf(temp, "%s", bug->bugMessage);
	yajl_gen_string(writer, temp, tempLen);
    }
    if ( bug->resolutionSuggestion != NULL ) {
	yajl_gen_string(writer, "ResolutionSuggestion", 20);
	tempLen = sprintf(temp, "%s", bug->resolutionSuggestion);
	yajl_gen_string(writer, temp, tempLen);
    }

    if ( bug->cweIds != NULL ) {
	yajl_gen_string(writer, "CweIds", 6);
	yajl_gen_array_open(writer);
	int i;
	for ( i = 0 ; i < bug->cweIdsCount ; i++ ) {
	    tempLen = sprintf(temp, "%d", bug->cweIds[i]);
	    yajl_gen_number(writer, temp, tempLen);
	}
        yajl_gen_array_close(writer);
    }

    if ( bug->methods != NULL ) {
        int methodId = 1;
	yajl_gen_string(writer, "Methods", 7);
        yajl_gen_array_open(writer);
	Method * methods = bug->methods;
	int i;
	for ( i = 0 ; i < bug->methodsCount; i++ ) { 
	    Method *method = &methods[i];
	    yajl_gen_map_open(writer);	    
            yajl_gen_string(writer, "name", 4);
	    tempLen = sprintf(temp, "%s", method->name);
            yajl_gen_string(writer, temp, tempLen);
            yajl_gen_string(writer, "primary", 7);
            if ( method->primary ) {
                yajl_gen_bool(writer, 1);
            } else {
                yajl_gen_bool(writer, 0);
	    }
            yajl_gen_string(writer, "MethodId", 8);
	    tempLen = sprintf(temp, "%d", methodId);
            yajl_gen_number(writer, temp, tempLen);
            methodId = methodId + 1;
            yajl_gen_map_close(writer);
	}
        yajl_gen_array_close(writer);
    }

    if ( bug->bugLocations != NULL ) {
	int locID = 1;
        yajl_gen_string(writer, "BugLocations", 12);
        yajl_gen_array_open(writer);
	Location * bugloc = bug->locations;
	int i;
	for ( i = 0; i < bug->locationsCount; i++){
	    Location *location = &bugloc[i];
            yajl_gen_map_open(writer);

            if ( location->startLine != 0 ) {
                yajl_gen_string(writer, "StartLine", 9);
		tempLen = sprintf(temp, "%d", location->startLine);
                yajl_gen_number(writer, temp, tempLen);
	    }
            if ( location->endLine != 0 ) {
                yajl_gen_string(writer, "EndLine", 7);
		tempLen = sprintf(temp, "%d", location->endLine);
                yajl_gen_number(writer, temp, tempLen);
	    }
            if ( location->startColumn != 0 ) {
                yajl_gen_string(writer, "StartColumn", 11);
		tempLen = sprintf(temp, "%d", location->startColumn);
                yajl_gen_number(writer, temp, tempLen);
	    }
            if ( location->endColumn != 0 ) {
                yajl_gen_string(writer, "EndColumn", 9);
		tempLen = sprintf(temp, "%d", location->endColumn);
                yajl_gen_number(writer, temp, tempLen);
	    }

	    
            if ( location->sourceFile != NULL ) {
                yajl_gen_string(writer, "SourceFile", 10);
		tempLen = sprintf(temp, "%s", location->sourceFile);
                yajl_gen_string(writer, temp, tempLen);
	    }
            if ( location->explanation != NULL ) {
                yajl_gen_string(writer, "Explanation", 9);
		tempLen = sprintf(temp, "%s", location->explanation);
                yajl_gen_string(writer, temp, tempLen);
	    }
            
	    yajl_gen_string(writer, "primary", 7);
            if ( location->primary ) {
                yajl_gen_bool(writer, 1);
            } else {
                yajl_gen_bool(writer, 0);
	    }
	    
            yajl_gen_string(writer, "LocationId", 10);
	    tempLen = sprintf(temp, "%d", locID);
            yajl_gen_number(writer, temp, tempLen);
            locID = locID + 1;
            yajl_gen_map_close(writer);
	}
        yajl_gen_array_close(writer);
    }
    
    InstanceLocation inst = bug->instanceLocation;
    if ( inst.xPath != NULL || inst.lineNum.start != 0 || inst.lineNum.end != 0 ) {
	yajl_gen_string(writer, "InstanceLocation", 16);
        yajl_gen_map_open(writer);
	if ( inst.xPath != NULL ) {
	    yajl_gen_string(writer, "Xpath", 5);
	    tempLen = sprintf(temp, "%s", inst.xPath);
	    yajl_gen_string(writer, temp, tempLen);
	}
	if ( inst.lineNum.start != 0 || inst.lineNum.end != 0 ) {
	    yajl_gen_string(writer, "LineNum", 7);
	    yajl_gen_map_open(writer);
	    if ( inst.lineNum.start != 0 ) {
		yajl_gen_string(writer, "Start", 5);
		tempLen = sprintf(temp, "%d", inst.lineNum.start);
                yajl_gen_number(writer, temp, tempLen);
	    }
	    if ( inst.lineNum.end != 0 ) {
		yajl_gen_string(writer, "End", 3);
		tempLen = sprintf(temp, "%d", inst.lineNum.end);
                yajl_gen_number(writer, temp, tempLen);
	    }
	    yajl_gen_map_close(writer);
	}
        yajl_gen_map_close(writer);
    }
    yajl_gen_map_close(writer);

    int initBytes = ftell(writerInfo->file);
    const unsigned char * buf;
    size_t bufLen;
    yajl_gen_get_buf(writer, &buf, &bufLen);
    fwrite(buf, 1, bufLen, writerInfo->file);
    yajl_gen_clear(writer);
    int finalBytes = ftell(writerInfo->file);
    int bytes = finalBytes - initBytes;

    ///////////////////////////////Group bugs/////////////////////
    printf("summaries\n");
    char * code = bug->bugCode;
    if (code == NULL) {
        code = "undefined";
    }
    char * group = bug->bugGroup;
    if (group == NULL) {
        group = "undefined";
    }

    BugSummaries * prev = NULL;
    BugSummaries * cur = writerInfo->bugSums;
    while (cur != NULL && strcmp(cur->code, code) != 0){
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL) {
        BugSummaries * summaries = malloc(sizeof(BugSummaries));
        summaries->code = malloc(strlen(code) + 1);
        strcpy(summaries->code, code);

        BugSummary * summary = malloc(sizeof(BugSummary));
        summary->count = 1;
        summary->bytes = bytes;
        summary->next = NULL;
        summary->code = malloc(strlen(code) + 1);
        strcpy(summary->code, code);
        summary->group = malloc(strlen(group) + 1);
        strcpy(summary->group, group);
        summaries->codeSummary = summary;
        summaries->next = NULL;

        if (prev == NULL) {
            writerInfo->bugSums = summaries;
        } else {
            prev->next = summaries;
        }
    } else {
        BugSummary * prevGroup = NULL;
        BugSummary * curGroup = cur->codeSummary;
        while (curGroup != NULL && strcmp(curGroup->group, group) != 0){
            prevGroup = curGroup;
            curGroup = curGroup->next;
        }
        if (curGroup == NULL) {
            BugSummary * summary = malloc(sizeof(BugSummary));
            summary->count = 1;
            summary->bytes = bytes;
            summary->next = NULL;
            summary->code = cur->code;
            summary->group = malloc(strlen(group) + 1);
            strcpy(summary->group, group);

            if (prevGroup == NULL) {
                cur->codeSummary = summary;
            } else {
                prevGroup->next = summary;
            }

        } else {
            curGroup->count++;
            curGroup->bytes += bytes;
        }
    }


    writerInfo->bugId++;
    return 0;
}

////////////////////////Add a metric/////////////////////////////////////////////////////////////
char * CheckMetric(Metric * metric)
{
    char * errors = malloc(strlen("\0") + 1);
    errors[0] = '\0';
    char * temp = malloc(140);
    if (metric->value == NULL){
        sprintf(temp, "Required Element: Value not found in Metric\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    if ( metric->type == NULL ) {
        sprintf(temp, "Required Element: Type not found in Metric\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) +  1);
        errors = strcat(errors, temp);
    }
    if ( metric->sourceFile == NULL ) {
        sprintf(temp, "Required Element: SourceFile not found in Metric\n");
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    return errors;


}


int AddMetric(ScarfJSONWriter *  writerInfo, Metric * metric)
{
    yajl_gen writer = writerInfo->writer;
    if (writerInfo->errorLevel != 0) {
        if (strcmp(writerInfo->curr, "summary") == 0) {
            printf("Summary already written. Invalid Scarf.\n");
            if (writerInfo->errorLevel == 2) {
                exit(1);
            }
        }
        if (writerInfo->start == 0) {
            printf("Scarf file closed.\n");
            if (writerInfo->errorLevel == 2) {
                exit(1);
            }
        }
        char * errors = NULL;
        errors = CheckMetric(metric);
        if ( strcmp(errors,"") != 0 ) {
            printf("%s", errors);
            if ( writerInfo->errorLevel == 2 ) {
                exit(1);
            }
        }
        free(errors);
    }

    if ( strcmp ( writerInfo->curr, "bug") == 0 ) {
        yajl_gen_array_close(writer);
    } else if (strcmp(writerInfo->curr, "metric") != 0) {
        yajl_gen_string(writer, "Metrics", 7);
        yajl_gen_array_open(writer);
        strcpy(writerInfo->curr, "metric");
	writerInfo->openBody = 1;
    }

    
    char temp [1024];
    int tempLen;

    yajl_gen_map_open(writer);
    yajl_gen_string(writer, "MetricId", 8);
    tempLen = sprintf(temp, "%d", writerInfo->metricId);
    yajl_gen_number(writer, temp, tempLen);
    if ( metric->value != NULL ) {
        yajl_gen_string(writer, "Value", 5);
	tempLen = sprintf(temp, "%s", metric->value);
        yajl_gen_string(writer, temp, tempLen);
    }
    if ( metric->className != NULL ) {
        yajl_gen_string(writer, "Class", 5);
	tempLen = sprintf(temp, "%s", metric->className);
        yajl_gen_string(writer, temp, tempLen);
    }
    if ( metric->methodName != NULL ) {
        yajl_gen_string(writer, "Method", 6);
	tempLen = sprintf(temp, "%s", metric->methodName);
        yajl_gen_string(writer, temp, tempLen);
    }
    if ( metric->sourceFile != NULL ) {
        yajl_gen_string(writer, "SourceFile", 10);
	tempLen = sprintf(temp, "%s", metric->sourceFile);
        yajl_gen_string(writer, temp, tempLen);
    }
    if ( metric->type != NULL ) {
        yajl_gen_string(writer, "Type", 4);
	tempLen = sprintf(temp, "%s", metric->type);
        yajl_gen_string(writer, temp, tempLen);
    }

    yajl_gen_map_close(writer);

    writerInfo->metricId = writerInfo->metricId + 1;

    /////////////////group metrics ////////////////////////////////
    char * type = metric->type;
    if (type == NULL) {
        type = "undefined";
    }

    double value;
    char * buffer = NULL;
    MetricSummary * prev = NULL;
    MetricSummary * cur = writerInfo->metricSum;
    while (cur != NULL && strcmp(cur->type, type) != 0){
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL) {
        MetricSummary * summary = malloc(sizeof(MetricSummary));
        summary->type = malloc(strlen(type) + 1);
        strcpy(summary->type, type);
        summary->count = 1;
        value = strtod(metric->value, &buffer);
        if (buffer != metric->value) {
            summary->sum = value;
            summary->sumOfSquares = value * value;
            summary->max = value;
            summary->min = value;
            summary->valid = 1;
        }
        summary->next = NULL;

        if (prev == NULL) {
            writerInfo->metricSum = summary;
        } else {
            prev->next = summary;
        }
    } else {
        cur->count++;
        if (cur->valid != 0) {
            value = strtod(metric->value, &buffer);
            if (buffer != metric->value) {
                cur->sum += value;
                cur->sumOfSquares += value * value;
                if (value > cur->max){
                    cur->max = value;
                }
                if (value < cur->min){
                    cur->min = value;
                }
            } else {
                cur->valid = 0;
            }
        }
    }
    writerInfo->metricId++;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////

char * CheckStart(Initial * initial){
    char * errors = malloc(strlen("\0") + 1);
    errors[0] = '\0';
    if (initial->tool_name == NULL){
        char * temp = "Required Attribute: tool_name not found in Initial";
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    if ( initial->tool_version == NULL ) {
        char * temp = "Required Attribute: tool_version not found in Initial";
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    if ( initial->uuid == NULL ) {
        char * temp = "Required Attribute: uuid not found in Initial";
        errors = realloc(errors, strlen(errors) + strlen(temp) + 1);
        errors = strcat(errors, temp);
    }
    return errors;
}

int AddStartTag(ScarfJSONWriter * writerInfo, Initial * initial)
{
    strcpy(writerInfo->curr, "Init");
    if (writerInfo->errorLevel != 0) {
        if ( writerInfo->start == 1 ) {
            printf("Scarf file already open\n");
            if (writerInfo->errorLevel == 2) {
                exit(1);
            }
        }
        char * errors = NULL;
        errors = CheckStart(initial);
        if ( strcmp(errors,"") != 0) {
            printf("%s", errors);
            if ( writerInfo->errorLevel == 2 ) {
                exit(1);
            }
        }
        free(errors);
    }
    
    yajl_gen writer = writerInfo->writer;
    yajl_gen_map_open(writer);
    yajl_gen_string(writer, "AnalyzerReport", 14);
    yajl_gen_map_open(writer);
    
    char temp[1024];
    int tempLen;
    if (initial->uuid != NULL ){
	yajl_gen_string(writer, "uuid", 4);
	tempLen = sprintf(temp, "%s", initial->uuid);
	yajl_gen_string(writer, temp, tempLen);		
    }
    if (initial->tool_name != NULL ){
	yajl_gen_string(writer, "tool_name", 9);
	tempLen = sprintf(temp, "%s", initial->tool_name);
	yajl_gen_string(writer, temp, tempLen);
    }
    if (initial->tool_version != NULL ){
	yajl_gen_string(writer, "tool_version", 12);
	tempLen = sprintf(temp, "%s", initial->tool_version);
	printf("%s : %d \n", initial->tool_version, tempLen);
	yajl_gen_string(writer, temp, tempLen);		
    }


    const unsigned char * buf;
    size_t bufLen;
    yajl_gen_get_buf(writer, &buf, &bufLen);
    fwrite(buf, 1, bufLen, writerInfo->file);
    yajl_gen_clear(writer);

    BugSummaries *freeBugSum = writerInfo->bugSums;
    BugSummaries *prevBugSum = NULL;
    while ( freeBugSum != NULL ) {
        prevBugSum = freeBugSum;
        freeBugSum = freeBugSum->next;
        free(prevBugSum->code);
        BugSummary *codeBugSum = prevBugSum->codeSummary;
        BugSummary *prevCodeBugSum = NULL;
        while ( codeBugSum != NULL ) {
            prevCodeBugSum = codeBugSum;
            codeBugSum = codeBugSum->next;
            free(prevCodeBugSum->code);
            free(prevCodeBugSum->group);
            free(prevCodeBugSum);
        }
        free(prevBugSum->codeSummary);
        free(prevBugSum);
    }
    MetricSummary *metrSum = writerInfo->metricSum;
    MetricSummary *prev = NULL;
    while ( metrSum != NULL ) {
        prev = metrSum;
        metrSum = metrSum->next;
        free(prev->type);
        free(prev);
    }
    
    writerInfo->start = 1;
    writerInfo->metricSum = NULL;
    writerInfo->bugSums = NULL;

}




//////////////////////End initialtag/////////////////////////////////////////////
int AddEndTag(ScarfJSONWriter * writerInfo)
{
    strcpy(writerInfo->curr, "end");
    if (writerInfo->errorLevel != 0) {
        if ( writerInfo->start == 0 ) {
            printf("Scarf file already closed\n");
            if (writerInfo->errorLevel == 2) {
                exit(1);
            }
        }
    }
    writerInfo->start = 0;
    yajl_gen writer = writerInfo->writer;
    if (writerInfo->openBody) {
	yajl_gen_array_close(writer);
	writerInfo->openBody = 0;
    }
    yajl_gen_map_close(writer);
    yajl_gen_map_close(writer);
    const unsigned char * buf;
    size_t bufLen;
    yajl_gen_get_buf(writer, &buf, &bufLen);
    fwrite(buf, 1, bufLen, writerInfo->file);
    yajl_gen_clear(writer);
}

//////////////Add summary generated from instances//////////////////////////////////
int AddSummary(ScarfJSONWriter * writerInfo)
{
    strcpy(writerInfo->curr, "summary");
    yajl_gen writer = writerInfo->writer;
    if (writerInfo->openBody) {
	yajl_gen_array_close(writer);
	writerInfo->openBody = 0;
    }

    BugSummaries * curBugSummary = writerInfo->bugSums;
    MetricSummary * curMetricSummary = writerInfo->metricSum;
    BugSummary * curBugSummaryGroup;
    int hasSummary = 0;
    if ( curBugSummary != NULL || curMetricSummary != NULL ) {
	hasSummary = 1;
    } else {
	if (writerInfo->errorLevel != 0) {
	    if ( writerInfo->start == 0 ) {
		printf("Scarf file closed\n");
		if (writerInfo->errorLevel == 2) {
		    exit(1);
		}
	    }
	}
    }

    char temp [1024];
    int tempLen;

    if (curBugSummary != NULL) {
	yajl_gen_string(writer, "BugSummaries", 12);
	yajl_gen_map_open(writer);
    }
    while (curBugSummary != NULL) {
	tempLen = sprintf(temp, "%s", curBugSummary->code);
	yajl_gen_string(writer, temp, tempLen);
	yajl_gen_map_open(writer);
        curBugSummaryGroup = curBugSummary->codeSummary;
	while (curBugSummaryGroup != NULL) {
	    tempLen = sprintf(temp, "%s", curBugSummaryGroup->group);
	    yajl_gen_string(writer, temp, tempLen);
	    yajl_gen_map_open(writer);
            tempLen = sprintf(temp, "%d", curBugSummaryGroup->count);
	    yajl_gen_string(writer, "count", 5);
	    yajl_gen_string(writer, temp, tempLen);
	    tempLen = sprintf(temp, "%d", curBugSummaryGroup->bytes);
	    yajl_gen_string(writer, "bytes", 5);
	    yajl_gen_string(writer, temp, tempLen);
	    yajl_gen_map_close(writer); 
	    curBugSummaryGroup = curBugSummaryGroup->next;
        }
	yajl_gen_map_close(writer); 
        curBugSummary = curBugSummary->next;
    }

    if (curMetricSummary != NULL) {
	yajl_gen_map_close(writer);  
	yajl_gen_string(writer, "MetricSummaries", 15);
	yajl_gen_map_open(writer);
    }
    while (curMetricSummary != NULL){
        int count = curMetricSummary->count;
        int sum = curMetricSummary->sum;
        int sumOfSquares = curMetricSummary->sumOfSquares;
	tempLen = sprintf(temp, "%s", curMetricSummary->type);
	yajl_gen_string(writer, temp, tempLen);
	yajl_gen_map_open(writer);
	tempLen = sprintf(temp, "%d", count);
	yajl_gen_string(writer, "Count", 5);
        yajl_gen_string(writer, temp, tempLen);
	
	if (curMetricSummary->valid != 0) {
            tempLen = sprintf(temp, "%d", sum);
	    yajl_gen_string(writer, "Sum", 3);
            yajl_gen_string(writer, temp, tempLen);
	    tempLen = sprintf(temp, "%d", sumOfSquares);	    
	    yajl_gen_string(writer, "SumOfSquares", 12);
            yajl_gen_string(writer, temp, tempLen);
	    sprintf(temp, "%d", curMetricSummary->max);
	    yajl_gen_string(writer, "Maximum", 7);
            yajl_gen_string(writer, temp, tempLen);
	    sprintf(temp, "%d", curMetricSummary->min);
	    yajl_gen_string(writer, "Minimum", 7);
            yajl_gen_string(writer, temp, tempLen);

	    double average = ((double) sum) / count;
            sprintf(temp, "%.2f", average);
	    yajl_gen_string(writer, "Average", 7);
            yajl_gen_string(writer, temp, tempLen);

	    int denominator = count * (count - 1);
            double standard_dev = 0;
            if (denominator != 0) {
                standard_dev = sqrt(((sumOfSquares * count) - (sum * sum) ) / denominator);
            }
            sprintf(temp, "%.2f", standard_dev);
	    yajl_gen_string(writer, "StandardDeviation", 17);
            yajl_gen_string(writer, temp, tempLen);
	}
	curMetricSummary = curMetricSummary->next;
	yajl_gen_map_open(writer);
    }
    if ( hasSummary ) {
	yajl_gen_map_close(writer);
    }
	
    const unsigned char * buf;
    size_t bufLen;
    yajl_gen_get_buf(writer, &buf, &bufLen);
    fwrite(buf, 1, bufLen, writerInfo->file);
    yajl_gen_clear(writer);

}




