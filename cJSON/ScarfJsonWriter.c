#include <yajl/yajl_gen.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ScarfJson.h"
////////////////////structs///////////////////////////

#define YAJL_GEN_STRING(handler, string) yajl_gen_string(handler, (YAJL_UStr)string, strlen(string))

struct ScarfJSONWriter {
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
};


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
    writerInfo->file = fopen(filename, "w");
	if (writerInfo->file == NULL){
        printf("Error - open: %s failed - errno: %d.\n", filename, errno);
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
void ScarfJSONWriterSetPretty(ScarfJSONWriter * writerInfo, int pretty_level) {
    yajl_gen_config(writerInfo->writer, yajl_gen_beautify, pretty_level);
    writerInfo->pretty = pretty_level;
}

void ScarfJSONWriterSetUTF8(ScarfJSONWriter * writerInfo, int utf8){
    yajl_gen_config(writerInfo->writer, yajl_gen_validate_utf8 , 1);
    writerInfo->utf8 = utf8;
}

int ScarfJSONWriterGetPretty(ScarfJSONWriter * writerInfo) {
    return writerInfo->pretty;
}

int ScarfJSONWriterGetUTF8 (ScarfJSONWriter * writerInfo) {
    return writerInfo->utf8;
}

yajl_gen  getScarfJSONWriter (ScarfJSONWriter * writerInfo)
{
    if (writerInfo != NULL){
	return writerInfo->writer;
    } else return NULL;
}


int ScarfJSONWriterGetErrorLevel(ScarfJSONWriter * writerInfo)
{
    if (writerInfo != NULL){
	return writerInfo->errorLevel;
    } else return -1;
}


int ScarfJSONWriterSetErrorLevel(ScarfJSONWriter * writerInfo, int errorLevel)
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
    char  temp[140];

    if (bug->locations == NULL) {
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
        Method * methods = bug->methods;
	int i;
	for ( i = 0 ; i < bug->methodsCount ; i++ ) {
	    Method *method = &methods[i];
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

    Location * buglocs = bug->locations;
    int locID = 1;
    int locPrimary = 0;
    if (buglocs != NULL) {
	int i;
	for ( i = 0 ; i < bug->locationsCount ; i++ ) {
	    Location *loc = &buglocs[i];
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

    return errors;
}


typedef const unsigned char *YAJL_UStr;


int ScarfJSONWriterAddBug(ScarfJSONWriter * writerInfo, BugInstance * bug)
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
        YAJL_GEN_STRING(writer, "BugInstances");
        yajl_gen_array_open(writer);
        strcpy (writerInfo->curr, "bug");
	writerInfo->openBody = 1;
    }

    yajl_gen_map_open(writer);


    YAJL_GEN_STRING(writer, "BugId");

    yajl_gen_integer(writer, writerInfo->bugId);

    if ( bug->buildId != NULL ) {
	YAJL_GEN_STRING(writer, "BuildId");
	YAJL_GEN_STRING(writer, bug->buildId);
    }
    if ( bug->bugCode != NULL ) {
	YAJL_GEN_STRING(writer, "BugCode");
	YAJL_GEN_STRING(writer, bug->bugCode);
    }
    if ( bug->bugRank != NULL ) {
	YAJL_GEN_STRING(writer, "BugRank");
	YAJL_GEN_STRING(writer, bug->bugRank);
    }
    if ( bug->className != NULL ) {
	YAJL_GEN_STRING(writer, "ClassName");
	YAJL_GEN_STRING(writer, bug->className);
    }
    if ( bug->bugSeverity != NULL ) {
	YAJL_GEN_STRING(writer, "BugSeverity");
	YAJL_GEN_STRING(writer, bug->bugSeverity);
    }
    if ( bug->bugGroup != NULL ) {
	YAJL_GEN_STRING(writer, "BugGroup");
	YAJL_GEN_STRING(writer, bug->bugGroup);
    }
    if ( bug->bugMessage != NULL ) {
	YAJL_GEN_STRING(writer, "BugMessage");
	YAJL_GEN_STRING(writer, bug->bugMessage);
    }
    if ( bug->resolutionSuggestion != NULL ) {
	YAJL_GEN_STRING(writer, "ResolutionSuggestion");
	YAJL_GEN_STRING(writer, bug->resolutionSuggestion);
    }

    if ( bug->cweIds != NULL ) {
	YAJL_GEN_STRING(writer, "CweIds");
	yajl_gen_array_open(writer);
	int i;
	for ( i = 0 ; i < bug->cweIdsCount ; i++ ) {
	    yajl_gen_integer(writer, bug->cweIds[i]);
	}
        yajl_gen_array_close(writer);
    }

    if ( bug->methods != NULL ) {
        int methodId = 1;
	YAJL_GEN_STRING(writer, "Methods");
        yajl_gen_array_open(writer);
	Method * methods = bug->methods;
	int i;
	for ( i = 0 ; i < bug->methodsCount; i++ ) {
	    Method *method = &methods[i];
	    yajl_gen_map_open(writer);
            YAJL_GEN_STRING(writer, "name");
            YAJL_GEN_STRING(writer, method->name);
            YAJL_GEN_STRING(writer, "primary");
            if ( method->primary ) {
                yajl_gen_bool(writer, 1);
            } else {
                yajl_gen_bool(writer, 0);
	    }
            YAJL_GEN_STRING(writer, "MethodId");
            yajl_gen_integer(writer, methodId);
            methodId = methodId + 1;
            yajl_gen_map_close(writer);
	}
        yajl_gen_array_close(writer);
    }

    if ( bug->locations != NULL ) {
	int locID = 1;
        YAJL_GEN_STRING(writer, "BugLocations");
        yajl_gen_array_open(writer);
	Location * bugloc = bug->locations;
	int i;
	for ( i = 0; i < bug->locationsCount; i++){
	    Location *location = &bugloc[i];
            yajl_gen_map_open(writer);

            if ( location->startLine != 0 ) {
                YAJL_GEN_STRING(writer, "StartLine");
                yajl_gen_integer(writer, location->startLine);
	    }
            if ( location->endLine != 0 ) {
                YAJL_GEN_STRING(writer, "EndLine");
                yajl_gen_integer(writer, location->endLine);
	    }
            if ( location->startColumn != 0 ) {
                YAJL_GEN_STRING(writer, "StartColumn");
                yajl_gen_integer(writer, location->startColumn);
	    }
            if ( location->endColumn != 0 ) {
                YAJL_GEN_STRING(writer, "EndColumn");
                yajl_gen_integer(writer, location->endColumn);
	    }


            if ( location->sourceFile != NULL ) {
                YAJL_GEN_STRING(writer, "SourceFile");
                YAJL_GEN_STRING(writer, location->sourceFile);
	    }
            if ( location->explanation != NULL ) {
                YAJL_GEN_STRING(writer, "Explanation");
                YAJL_GEN_STRING(writer, location->explanation);
	    }

	    YAJL_GEN_STRING(writer, "primary");
            if ( location->primary ) {
                yajl_gen_bool(writer, 1);
            } else {
                yajl_gen_bool(writer, 0);
	    }

            YAJL_GEN_STRING(writer, "LocationId");
            yajl_gen_integer(writer, locID);
            locID = locID + 1;
            yajl_gen_map_close(writer);
	}
        yajl_gen_array_close(writer);
    }

    InstanceLocation inst = bug->instanceLocation;
    if ( (inst.xPath != NULL || inst.lineNum.start != 0 || inst.lineNum.end != 0) ) {
	YAJL_GEN_STRING(writer, "InstanceLocation");
        yajl_gen_map_open(writer);
	if ( inst.xPath != NULL ) {
	    YAJL_GEN_STRING(writer, "Xpath");
	    YAJL_GEN_STRING(writer, inst.xPath);
	}
	if ( inst.lineNum.start != 0 || inst.lineNum.end != 0 ) {
	    YAJL_GEN_STRING(writer, "LineNum");
	    yajl_gen_map_open(writer);
	    if ( inst.lineNum.start != 0 ) {
		YAJL_GEN_STRING(writer, "Start");
                yajl_gen_integer(writer, inst.lineNum.start);
	    }
	    if ( inst.lineNum.end != 0 ) {
		YAJL_GEN_STRING(writer, "End");
                yajl_gen_integer(writer, inst.lineNum.end);
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
    //printf("summaries\n");
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
        summary->byteCount = bytes;
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
            summary->byteCount = bytes;
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
            curGroup->byteCount += bytes;
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
    char temp[140];
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


int ScarfJSONWriterAddMetric(ScarfJSONWriter *  writerInfo, Metric * metric)
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
        YAJL_GEN_STRING(writer, "Metrics");
        yajl_gen_array_open(writer);
        strcpy(writerInfo->curr, "metric");
	writerInfo->openBody = 1;
    }

    yajl_gen_map_open(writer);
    YAJL_GEN_STRING(writer, "MetricId");
    yajl_gen_integer(writer, writerInfo->metricId);
    if ( metric->value != NULL ) {
        YAJL_GEN_STRING(writer, "Value");
        YAJL_GEN_STRING(writer, metric->value);
    }
    if ( metric->className != NULL ) {
        YAJL_GEN_STRING(writer, "Class");
        YAJL_GEN_STRING(writer, metric->className);
    }
    if ( metric->methodName != NULL ) {
        YAJL_GEN_STRING(writer, "Method");
        YAJL_GEN_STRING(writer, metric->methodName);
    }
    if ( metric->sourceFile != NULL ) {
        YAJL_GEN_STRING(writer, "SourceFile");
        YAJL_GEN_STRING(writer, metric->sourceFile);
    }
    if ( metric->type != NULL ) {
        YAJL_GEN_STRING(writer, "Type");
        YAJL_GEN_STRING(writer, metric->type);
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

int ScarfJSONWriterAddStartTag(ScarfJSONWriter * writerInfo, Initial * initial)
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
    YAJL_GEN_STRING(writer, "AnalyzerReport");
    yajl_gen_map_open(writer);

    if (initial->uuid != NULL ){
	YAJL_GEN_STRING(writer, "uuid");
	YAJL_GEN_STRING(writer, initial->uuid);
    }
    if (initial->tool_name != NULL ){
	YAJL_GEN_STRING(writer, "tool_name");
	YAJL_GEN_STRING(writer, initial->tool_name);
    }
    if (initial->tool_version != NULL ){
	YAJL_GEN_STRING(writer, "tool_version");
	YAJL_GEN_STRING(writer, initial->tool_version);
    }
	/*
    if (initial->assess_fw != NULL ){
	YAJL_GEN_STRING(writer, "assess_fw");
	YAJL_GEN_STRING(writer, initial->assess_fw);
    }
    if (initial->assess_fw_version != NULL ){
	YAJL_GEN_STRING(writer, "assess_fw_version");
	YAJL_GEN_STRING(writer, initial->assess_fw_version);
    }
    if (initial->assessment_start_ts != NULL ){
	YAJL_GEN_STRING(writer, "assessment_start_ts");
	YAJL_GEN_STRING(writer, initial->assessment_start_ts);
    }
    if (initial->assessment_end_ts != NULL ){
	YAJL_GEN_STRING(writer, "assessment_end_ts");
	YAJL_GEN_STRING(writer, initial->assessment_end_ts);
    }
    if (initial->report_generation_ts != NULL ){
	YAJL_GEN_STRING(writer, "report_generation_ts");
	YAJL_GEN_STRING(writer, initial->report_generation_ts);
    }
    if (initial->build_fw != NULL ){
	YAJL_GEN_STRING(writer, "build_fw");
	YAJL_GEN_STRING(writer, initial->build_fw);
    }
    if (initial->build_fw_version != NULL ){
	YAJL_GEN_STRING(writer, "build_fw_version");
	YAJL_GEN_STRING(writer, initial->build_fw_version);
    }    
    if (initial->build_root_dir != NULL ){
	YAJL_GEN_STRING(writer, "build_root_dir");
	YAJL_GEN_STRING(writer, initial->build_root_dir);
    }
    if (initial->package_name != NULL ){
	YAJL_GEN_STRING(writer, "package_name");
	YAJL_GEN_STRING(writer, initial->package_name);
    }
    if (initial->package_root_dir != NULL ){
	YAJL_GEN_STRING(writer, "package_root_dir");
	YAJL_GEN_STRING(writer, initial->package_root_dir);
    }
    if (initial->package_version != NULL ){
	YAJL_GEN_STRING(writer, "package_version");
	YAJL_GEN_STRING(writer, initial->package_version);
    }
    if (initial->platform_name != NULL ){
	YAJL_GEN_STRING(writer, "platform_name");
	YAJL_GEN_STRING(writer, initial->platform_name);
    }
    if (initial->platform_version != NULL ){
	YAJL_GEN_STRING(writer, "platform_version");
	YAJL_GEN_STRING(writer, initial->platform_version);
    }
    if (initial->parser_fw != NULL ){
	YAJL_GEN_STRING(writer, "parser_fw");
	YAJL_GEN_STRING(writer, initial->parser_fw);
    }
    if (initial->parser_fw_version != NULL ){
	YAJL_GEN_STRING(writer, "parser_fw_version");
	YAJL_GEN_STRING(writer, initial->parser_fw_version);
    }	
	*/

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

    return 0;
}




//////////////////////End initialtag/////////////////////////////////////////////
int ScarfJSONWriterAddEndTag(ScarfJSONWriter * writerInfo)
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

    return 0;
}

//////////////Add summary generated from instances//////////////////////////////////
int ScarfJSONWriterAddSummary(ScarfJSONWriter * writerInfo)
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

    if (curBugSummary != NULL) {
	YAJL_GEN_STRING(writer, "BugSummaries");
	yajl_gen_map_open(writer);
    }
    while (curBugSummary != NULL) {
	YAJL_GEN_STRING(writer, curBugSummary->code);
	yajl_gen_map_open(writer);
        curBugSummaryGroup = curBugSummary->codeSummary;
	while (curBugSummaryGroup != NULL) {
	    YAJL_GEN_STRING(writer, curBugSummaryGroup->group);
	    yajl_gen_map_open(writer);
	    YAJL_GEN_STRING(writer, "count");
	    yajl_gen_integer(writer, curBugSummaryGroup->count);
	    YAJL_GEN_STRING(writer, "bytes");
	    yajl_gen_integer(writer, curBugSummaryGroup->byteCount);
	    yajl_gen_map_close(writer);
	    curBugSummaryGroup = curBugSummaryGroup->next;
        }
	yajl_gen_map_close(writer);
        curBugSummary = curBugSummary->next;
    }

    if (curMetricSummary != NULL) {
	yajl_gen_map_close(writer);
	YAJL_GEN_STRING(writer, "MetricSummaries");
	yajl_gen_map_open(writer);
    }
    while (curMetricSummary != NULL){
        int count = curMetricSummary->count;
        double sum = curMetricSummary->sum;
        double sumOfSquares = curMetricSummary->sumOfSquares;
	YAJL_GEN_STRING(writer, curMetricSummary->type);
	yajl_gen_map_open(writer);
	YAJL_GEN_STRING(writer, "Count");
        yajl_gen_integer(writer, count);

	if (curMetricSummary->valid != 0) {
	    char temp[1024];
	    
	    YAJL_GEN_STRING(writer, "Sum");
	    sprintf(temp, "%.2f", sum);
            YAJL_GEN_STRING(writer, temp);
	    YAJL_GEN_STRING(writer, "SumOfSquares");
	    sprintf(temp, "%.2f", sumOfSquares);
            YAJL_GEN_STRING(writer, temp);
	    YAJL_GEN_STRING(writer, "Maximum");
	    sprintf(temp, "%.2f", curMetricSummary->max);
            YAJL_GEN_STRING(writer, temp);
	    YAJL_GEN_STRING(writer, "Minimum");
	    sprintf(temp, "%.2f", curMetricSummary->min);
            YAJL_GEN_STRING(writer, temp);

	    double average = ((double) sum) / count;
	    YAJL_GEN_STRING(writer, "Average");
	    sprintf(temp, "%.2f", average);
            YAJL_GEN_STRING(writer, temp);

	    int denominator = count * (count - 1);
            double standard_dev = 0;
            if (denominator != 0) {
                standard_dev = sqrt(((sumOfSquares * count) - (sum * sum) ) / denominator);
            }
	    YAJL_GEN_STRING(writer, "StandardDeviation");
	    sprintf(temp, "%.2f", standard_dev);
            YAJL_GEN_STRING(writer, temp);
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

    return 0;
}
