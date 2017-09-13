#include <yajl/yajl_parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ScarfJson.h"

typedef struct Callback {
    BugCallback bugCall;
    MetricCallback  metricCall;
    InitialCallback initialCall;
    BugSummaryCallback bugSumCall;
    MetricSummaryCallback metricSumCall;
    FinalCallback finalCallback;
    void * CallbackData;
} Callback;

typedef struct State {
    int validStart;
    int validBody;
    int isArray;
    char * arrayType;
    size_t arrayTypeLength;
    int arrayLoc;
    char hashType[12];
    int depth;
    int requiredStart;
    char * curr;
    size_t currLength;
    void * returnValue;
    Method * method;
    Location * loc;
    BugSummary * bugSum;
    MetricSummary * metricSum;
    BugSummary * bugSummaries;
    MetricSummary * metricSummaries;
    BugInstance * bug;
    Metric * metric;
    Initial * initial;
    Callback * callbacks;
} State;

typedef struct ScarfJSONReader {
    FILE * file;
    yajl_handle reader;
    State * state;
    int filetype;
    int utf8;
} ScarfJSONReader;


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
    Initial *ret = calloc(1, sizeof(Initial));
    if (init->tool_version != NULL) {
        ret->tool_version =  malloc(strlen(init->tool_version) + 1);
        strcpy(ret->tool_version, init->tool_version);
    }
    if ( init->tool_name != NULL ) {
        ret->tool_name =  malloc(strlen(init->tool_name) + 1);
        strcpy(ret->tool_name, init->tool_name);
    }
    if (init->uuid != NULL) {
        ret->uuid = malloc(strlen(init->uuid) + 1);
        strcpy(ret->uuid, init->uuid);
    }
    return ret;
}




///////////////////////////////Delete initial struct//////////////////////////////////
void  DeleteInitial( Initial * initial ){
    free( initial->tool_name );
    free( initial->tool_version );
    free( initial->uuid );
    free( initial );
    return;
}


///////////////////////////////Delete a Metric///////////////////////////////////
void  DeleteMetric(Metric * metric)
{
    free( metric->type);
    free( metric->className);
    free( metric->methodName);
    free( metric->sourceFile);
    free( metric->value);
    free(metric);
    return;
}

///////////////////////////////Delete a BugInstance///////////////////////////////////
void  DeleteBug(BugInstance * bug)
{
    free( bug->assessmentReportFile);
    free( bug->buildId);
    free( bug->bugCode);
    free( bug->bugRank);
    free( bug->className);
    free( bug->bugSeverity);
    free( bug->bugGroup);
    free( bug->bugMessage);
    free( bug->resolutionSuggestion);
//    free( bug->instanceLocation);

    free(bug->cweIds);

    Method * method = bug->methods;
    if ( method != NULL ) {
	int i;
	for ( i = 0; i < bug->methodsCount; i++){
	    free(method[i].name);
	}
	free(method);
    }

    Location * bugloc = bug->locations;
    if (bugloc != NULL) {
	int i;
	for(i = 0; i < bug->locationsCount; i++) {
	    free(bugloc[i].sourceFile);
	    free(bugloc[i].explanation);
	}
	free(bugloc);
    }

    free(bug);
    return;
}


///////////////////////////////free summaries///////////////////////////////////
void DeleteBugSummary(BugSummary *bugSummary){
    BugSummary *cur = bugSummary;
    BugSummary *prev = NULL;
    while (cur != NULL) {
        prev = cur;
        cur = cur->next;
        free(prev->code);
        free(prev->group);
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
        free(prev->type);
        free(prev);
    }
    return;
}



///////////////////////////////Clear initial struct//////////////////////////////////
int _clearInitial( Initial * initial ){
    free( initial->tool_name );
    free( initial->tool_version );
    free( initial->uuid );
    memset(initial, 0 , sizeof(Initial));
    return 0;
}


///////////////////////////////Delete a Metric///////////////////////////////////
int _clearMetric(Metric * metric)
{
    free( metric->type);
    free( metric->className);
    free( metric->methodName);
    free( metric->sourceFile);
    free( metric->value);
    memset(metric, 0, sizeof(Metric));
    return 0;
}

///////////////////////////////Delete a BugInstance///////////////////////////////////
int _clearBug(BugInstance * bug)
{
    free( bug->assessmentReportFile);
    free( bug->buildId);
    free( bug->bugCode);
    free( bug->bugRank);
    free( bug->className);
    free( bug->bugSeverity);
    free( bug->bugGroup);
    free( bug->bugMessage);
    free( bug->resolutionSuggestion);
//    free( bug->instanceLocation);

    int * cwe = bug->cweIds;
    if (cwe !=  NULL){
	free(cwe);
    }

    Method * method = bug->methods;
    if ( method != NULL ) {
	int i;
	for ( i = 0; i < bug->methodsCount; i++){
	    free(method[i].name);
	}
	free(method);
    }

    Location * bugloc = bug->locations;
    if (bugloc != NULL) {
	int i;
	for(i = 0; i < bug->locationsCount; i++) {
	    free(bugloc[i].sourceFile);
	    free(bugloc[i].explanation);
	}
	free(bugloc);
    }

    memset(bug, 0, sizeof(BugInstance));
    return 0;
}

////////////////////////handlers//////////////////////////////
static int handle_null(void * ctx)
{
    (void)ctx; /* unused parameter */

    return 1;
}

static int handle_boolean(void * data, int boolean)
{
    struct State * ctx = (struct State *) data;
    if ( strcmp("bug", ctx->hashType) == 0 ) {
        if ( ctx->isArray ) {
            if ( strcmp("BugLocations", ctx->arrayType) == 0 ) {
                if ( boolean ) {
                    ctx->bug->locations[ctx->bug->locationsCount].primary = 1;
		}
                else {
                    ctx->bug->locations[ctx->bug->locationsCount].primary = 0;
                    //self->data[self->arrayType][self->arrayLoc]["primary"] = 0
		}
	    } else if ( strcmp("Methods",ctx->arrayType) == 0) {
                if ( boolean ) {
                    ctx->bug->methods[ctx->bug->methodsCount].primary = 1;
		}
                else {
                    ctx->bug->methods[ctx->bug->methodsCount].primary = 0;
		}
	    }
	}
    }
    return 1;
}

static int handle_number(void * data, const char * s, size_t l)
{
    printf("handleNumber\n");
    struct State * ctx = (struct State *) data;
    char * stringNumber = malloc(l + 1);
    strncpy(stringNumber , s, l);
    stringNumber[l] = '\0';
    double number = strtod(stringNumber, NULL);
    free(stringNumber);
    if ( strcmp("bug", ctx->hashType) == 0 ) {
	if ( strncmp(ctx->curr, "Start", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation.lineNum.start = number;
	} else if ( strncmp(ctx->curr, "BugId", ctx->currLength) == 0 ) {
	    ctx->bug->bugId = number;
	} else if ( strncmp(ctx->curr, "End", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation.lineNum.end = number;
	} else if ( ctx->isArray ) {
	    if ( strncmp("BugLocations", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    //ctx->loc->primary = number;
		    ctx->bug->locations[ctx->bug->locationsCount].primary = number;
		} else if (strncmp("LocationId", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->locationId = number;
		    ctx->bug->locations[ctx->bug->locationsCount].locationId = number;
		} else if (strncmp("StartLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startLine = number;
		    ctx->bug->locations[ctx->bug->locationsCount].startLine = number;
		} else if (strncmp("EndLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endLine = number;
		    ctx->bug->locations[ctx->bug->locationsCount].endLine = number;
		} else if (strncmp("StartColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startColumn = number;
		    ctx->bug->locations[ctx->bug->locationsCount].startColumn = number;
		} else if (strncmp("EndColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endColumn = number;
		    ctx->bug->locations[ctx->bug->locationsCount].endColumn = number;
		}
    	    } else if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    //ctx->method->primary = number;
		    ctx->bug->methods[ctx->bug->methodsCount].primary = number;
		} else if (strncmp("MethodId", ctx->curr, ctx->currLength) == 0) {
                    //ctx->method->methodId = number;
		    ctx->bug->methods[ctx->bug->methodsCount].methodId = number;
		}
	    } else if ( strncmp("CweIds", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {

		if ( ctx->bug->cweIds == NULL ) {
		    ctx->bug->cweIdsSize = 5;
		    ctx->bug->cweIdsCount = 0;
		    ctx->bug->cweIds = malloc(ctx->bug->cweIdsSize * sizeof(int));
		}
		if ( ctx->bug->cweIdsCount >= ctx->bug->cweIdsSize ) {
		    ctx->bug->cweIdsSize = ctx->bug->cweIdsSize * 2;
		    int *tempArray = realloc(ctx->bug->cweIds, ctx->bug->cweIdsSize * sizeof(int));
		    if (tempArray) {
			ctx->bug->cweIds = tempArray;
		    } else {
			printf("Could not expand CweID array. Exiting parsing");
			exit(1);
		    }
		}

		ctx->bug->cweIds[ctx->bug->cweIdsCount] = number;
		ctx->bug->cweIdsCount++;
            }
	}
    } else if ( strcmp("metric", ctx->hashType) == 0) {
	if ( strncmp(ctx->curr, "MetricId", ctx->currLength) == 0 ) {
	    ctx->metric->id = number;
	}

    } else if ( strcmp("bugsum", ctx->hashType) == 0) {
	if ( strncmp(ctx->curr, "count", ctx->currLength) == 0 ) {
	    ctx->bugSum->count = number;
	} else if ( strncmp(ctx->curr, "bytes", ctx->currLength) == 0 ) {
	    ctx->bugSum->byteCount = number;
	}

    } else if  ( strcmp("metrsum", ctx->hashType) == 0) {
	if ( strncmp(ctx->curr, "Sum", ctx->currLength) == 0 ) {
	    ctx->metricSum->sum = number;
	} else if ( strncmp(ctx->curr, "SumOfSquares", ctx->currLength) == 0 ) {
	    ctx->metricSum->sumOfSquares = number;
	} else if ( strncmp(ctx->curr, "Maximum", ctx->currLength) == 0 ) {
	    ctx->metricSum->max = number;
	} else if ( strncmp(ctx->curr, "Minimum", ctx->currLength) == 0 ) {
	    ctx->metricSum->min = number;
	} else if ( strncmp(ctx->curr, "Average", ctx->currLength) == 0 ) {
	    ctx->metricSum->average = number;
	} else if ( strncmp(ctx->curr, "StandardDeviation", ctx->currLength) == 0 ) {
	    ctx->metricSum->stdDeviation = number;
	} else if ( strncmp(ctx->curr, "Count", ctx->currLength) == 0 ) {
	    ctx->metricSum->count = number;
	}
    }
    return 1;
}

static int handle_string(void * data, const unsigned char * string,
                           size_t stringLen)
{
    printf("handlestr\n");
    char * stringVal = (char *) string;
    struct State * ctx = (struct State *) data;
    char * stringValue = malloc(stringLen + 1);
    strncpy( stringValue , stringVal, stringLen);
    stringValue[stringLen] = '\0';
    double number = strtod(stringVal , NULL);
    if (!ctx->requiredStart) {
	if ( strncmp(ctx->curr, "uuid", ctx->currLength) == 0 ) {
	    ctx->initial->uuid = stringValue;
	} else if ( strncmp(ctx->curr, "tool_name", ctx->currLength) == 0 ) {
	    ctx->initial->tool_name = stringValue;

	} else if ( strncmp(ctx->curr, "tool_version", ctx->currLength) == 0 ) {
	    ctx->initial->tool_version = stringValue;
	}
	if ( ctx->initial->tool_version != NULL && ctx->initial->tool_name != NULL && ctx->initial->uuid != NULL ) {
	    ctx->requiredStart = 1;
	    if ( ctx->callbacks->initialCall != NULL ) {
		ctx->returnValue = ctx->callbacks->initialCall(ctx->initial, ctx->callbacks->CallbackData);
		DeleteInitial(ctx->initial);
		if ( ctx->returnValue != NULL ) {
		    return 0;
		}
	    }
	}
    } else if ( strcmp("bug", ctx->hashType) == 0 ) {
	if ( strncmp(ctx->curr, "BugId", ctx->currLength) == 0 ) {
	    ctx->bug->bugId = number;
	} else if ( strncmp(ctx->curr, "AssessmentReportFile", ctx->currLength) == 0 ) {
	    ctx->bug->assessmentReportFile = stringValue ;
        } else if ( strncmp(ctx->curr, "BuildId", ctx->currLength) == 0 ) {
	    ctx->bug->buildId = stringValue;
        } else if ( strncmp(ctx->curr, "ClassName", ctx->currLength) == 0 ) {
	    ctx->bug->className = stringValue;

        } else if ( strncmp(ctx->curr, "BugGroup", ctx->currLength) == 0 ) {
	    ctx->bug->bugGroup = stringValue;

        } else if ( strncmp(ctx->curr, "BugCode", ctx->currLength) == 0 ) {
	    ctx->bug->bugCode = stringValue;

        } else if ( strncmp(ctx->curr, "BugRank", ctx->currLength) == 0 ) {
	    ctx->bug->bugRank = stringValue;

        } else if ( strncmp(ctx->curr, "BugSeverity", ctx->currLength) == 0 ) {
	    ctx->bug->bugSeverity = stringValue;

        } else if ( strncmp(ctx->curr, "BugMessage", ctx->currLength) == 0 ) {
	    ctx->bug->bugMessage = stringValue;

        } else if ( strncmp(ctx->curr, "ResolutionSuggestion", ctx->currLength) == 0 ) {
	    ctx->bug->resolutionSuggestion = stringValue;

        } else if ( strncmp(ctx->curr, "Xpath", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation.xPath = stringValue;
        } else if ( strncmp(ctx->curr, "Start", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation.lineNum.start = number;
        } else if ( strncmp(ctx->curr, "End", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation.lineNum.end = number;

	} else if ( ctx->isArray ) {
	    if ( strncmp("BugLocations", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
                if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    if ( strncmp(stringVal, "true", stringLen) == 0 ) {
			//ctx->loc->primary = 1;
			ctx->bug->locations[ctx->bug->locationsCount].primary = 1;
		    } else {
			//ctx->loc->primary = 0;
			ctx->bug->locations[ctx->bug->locationsCount].primary = 0;
		    }
                } else if (strncmp("LocationId", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->locationId = number;
		    ctx->bug->locations[ctx->bug->locationsCount].locationId = number;
                } else if (strncmp("StartLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startLine = number;
		    ctx->bug->locations[ctx->bug->locationsCount].startLine = number;
                } else if (strncmp("EndLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endLine = number;
		    ctx->bug->locations[ctx->bug->locationsCount].endLine = number;
                } else if (strncmp("StartColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startColumn = number;
		    ctx->bug->locations[ctx->bug->locationsCount].startColumn = number;
                } else if (strncmp("EndColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endColumn = number;
		    ctx->bug->locations[ctx->bug->locationsCount].endColumn = number;
                } else if (strncmp("SourceFile", ctx->curr, ctx->currLength) == 0) {
		    //ctx->loc->sourceFile = stringValue;
		    ctx->bug->locations[ctx->bug->locationsCount].sourceFile = stringValue;
		} else if (strncmp("Explanation", ctx->curr, ctx->currLength) == 0) {
		    //ctx->loc->explanation = stringValue;
		    ctx->bug->locations[ctx->bug->locationsCount].explanation = stringValue;
                }


            } else if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
                if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    if ( strncmp(stringVal, "true", stringLen) == 0 ) {
//			ctx->method->primary = 1;
			ctx->bug->methods[ctx->bug->methodsCount].primary = 1;
		    } else {
//			ctx->method->primary = 0;
			ctx->bug->methods[ctx->bug->methodsCount].primary = 0;
		    }
                } else if (strncmp("MethodId", ctx->curr, ctx->currLength) == 0) {
//                    ctx->method->methodId = number;
		    ctx->bug->methods[ctx->bug->methodsCount].methodId = number;
                } else if (strncmp("name", ctx->curr, ctx->currLength) == 0) {
//		    ctx->method->name = stringValue;
		    ctx->bug->methods[ctx->bug->methodsCount].name = stringValue;
		}
            } else if ( strncmp("CweIds", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		if ( ctx->bug->cweIds == NULL ) {
		    ctx->bug->cweIdsSize = 5;
		    ctx->bug->cweIdsCount = 0;
		    ctx->bug->cweIds = malloc(ctx->bug->cweIdsSize * sizeof(int));
		}
		if ( ctx->bug->cweIdsCount >= ctx->bug->cweIdsSize ) {
		    ctx->bug->cweIdsSize = ctx->bug->cweIdsSize * 2;
		    int *tempArray = realloc(ctx->bug->cweIds, ctx->bug->cweIdsSize * sizeof(int));
		    if (tempArray) {
			ctx->bug->cweIds = tempArray;
		    } else {
			printf("Could not expand CweID array. Exiting parsing");
			exit(1);
		    }
		}

		ctx->bug->cweIds[ctx->bug->cweIdsCount] = number;
		ctx->bug->cweIdsCount++;
	    }
	}

    } else if ( strcmp("metric", ctx->hashType) == 0 ) {
	if ( strncmp(ctx->curr, "MetricId", ctx->currLength) == 0 ) {
	    ctx->metric->id = number;
	} else if ( strncmp(ctx->curr, "Value", ctx->currLength) == 0 ) {
            ctx->metric->value = stringValue;
	} else if ( strncmp(ctx->curr, "Class", ctx->currLength) == 0 ) {
            ctx->metric->className = stringValue;
	} else if ( strncmp(ctx->curr, "Method", ctx->currLength) == 0 ) {
            ctx->metric->methodName = stringValue;
	} else if ( strncmp(ctx->curr, "SourceFile", ctx->currLength) == 0 ) {
            ctx->metric->sourceFile = stringValue;
	} else if ( strncmp(ctx->curr, "Type", ctx->currLength) == 0 ) {
            ctx->metric->type = stringValue;
	}
    } else if ( strcmp("bugsum", ctx->hashType) == 0 ) {
	if ( strncmp(ctx->curr, "bytes", ctx->currLength) == 0 ) {
            ctx->bugSum->byteCount = number;
	} else if ( strncmp(ctx->curr, "count", ctx->currLength) == 0 ) {
            ctx->bugSum->count = number;
	}
    } else if  ( strcmp("metrsum", ctx->hashType) == 0) {
        if ( strncmp(ctx->curr, "Sum", ctx->currLength) == 0 ) {
            ctx->metricSum->sum = number;
        } else if ( strncmp(ctx->curr, "SumOfSquares", ctx->currLength) == 0 ) {
            ctx->metricSum->sumOfSquares = number;
        } else if ( strncmp(ctx->curr, "Maximum", ctx->currLength) == 0 ) {
            ctx->metricSum->max = number;
        } else if ( strncmp(ctx->curr, "Minimum", ctx->currLength) == 0 ) {
            ctx->metricSum->min = number;
        } else if ( strncmp(ctx->curr, "Average", ctx->currLength) == 0 ) {
            ctx->metricSum->average = number;
        } else if ( strncmp(ctx->curr, "StandardDeviation", ctx->currLength) == 0 ) {
            ctx->metricSum->stdDeviation = number;
        } else if ( strncmp(ctx->curr, "Count", ctx->currLength) == 0 ) {
            ctx->metricSum->count = number;
        } else if ( strncmp(ctx->curr, "Type", ctx->currLength) == 0 ) {
	    ctx->metricSum->type = stringValue;
	}
    }
    return 1;
}

static int handle_map_key(void * data, const  unsigned char * string,
                            size_t stringLen)
{
    char * stringVal = (char *) string;
    struct State * ctx = (struct State *) data;
    if ( ctx->depth == 1 && strncmp(stringVal, "AnalyzerReport", stringLen) == 0) {
        ctx->validStart = 1;
	ctx->initial = calloc(1,sizeof(Initial));
    } else if ( strncmp(stringVal, "BugInstances", stringLen) == 0 && ctx->depth == 2 ) {
        if (  ! ctx->validStart  ) {
            printf("Misformed SCARF File: No AnalyzerReport Tag before first element");
        }
	strcpy(ctx->hashType, "bug");
        ctx->validBody = 1;
    } else if ( strncmp(stringVal, "Metrics", stringLen) == 0 && ctx->depth == 2 ) {
        if (  ! ctx->validStart  ) {
           printf("Misformed SCARF File: No AnalyzerReport Tag before first element");
        }
	strcpy(ctx->hashType, "metric");
        ctx->validBody = 1;
    } else if ( strncmp(stringVal, "BugSummaries", stringLen) == 0 && ctx->depth == 2 ) {
        if (  ! ctx->validStart  ) {
            printf("Misformed SCARF File: No AnalyzerReport Tag before first element");
        }
	if ( ! ctx->validBody ) {
            printf("No BugInstances or Metrics present");
        }
	strcpy(ctx->hashType, "bugsum");
    } else if ( strncmp(stringVal, "MetricSummaries", stringLen) == 0 && ctx->depth == 2 ) {
        if (  ! ctx->validStart  ) {
            printf("Misformed SCARF File: No AnalyzerReport Tag before first element");
        }
	if ( ! ctx->validBody ) {
            printf("No BugInstances or Metrics present");
        }
	strcpy(ctx->hashType, "metrsum");
    }// else if ( strncmp(stringVal, "InstanceLocation", stringLen) == 0 && ctx->depth == 3 ) {
//        ctx->bug->instanceLocation = calloc(1, sizeof(InstanceLocation));
//    }
    free(ctx->curr);
    ctx->curr = malloc(stringLen + 1);
    strncpy(ctx->curr, stringVal, stringLen);
    ctx->curr[stringLen] = '\0';
    ctx->currLength = stringLen + 1;
    return 1;
}

static int handle_start_map(void * data)
{
    struct State * ctx = (struct State *) data;
/*    if (ctx->isArray && ctx->depth == 3) {
	if ( strncmp( ctx->arrayType, "BugLocations", ctx->arrayTypeLength-1 ) == 0 ) {
	    ctx->loc = calloc(1, sizeof(Location));
	} else if  ( strncmp( ctx->arrayType, "Methods", ctx->arrayTypeLength-1 ) == 0 ) {
	    ctx->method = calloc(1, sizeof(Method));
    	}
    } else*/ if (ctx->depth == 3) {
	if ( strcmp(ctx->hashType, "bugsum") == 0 ) {
	    ctx->bugSum = calloc(1, sizeof(BugSummary));
	} else if ( strcmp(ctx->hashType, "metrsum") == 0 ) {
	    ctx->metricSum = calloc(1, sizeof(MetricSummary));
	}
    }
    ctx->depth = ctx->depth + 1;
    return 1;
}

static int handle_end_map(void * data)
{
    struct State * ctx = (struct State *) data;
    ctx->depth = ctx->depth - 1;
    if ( ctx->depth == 2 ) {
        if ( strcmp("bug", ctx->hashType) == 0 ) {
	    if ( ctx->callbacks->bugCall != NULL ) {
		ctx->returnValue = ctx->callbacks->bugCall(ctx->bug, ctx->callbacks->CallbackData);
		_clearBug(ctx->bug);
		if ( ctx->returnValue != NULL ) {
		    return 0;
		}
	    } else {
		DeleteBug(ctx->bug);
	    }
	} else if ( strcmp("metric", ctx->hashType) == 0 ) {
            if ( ctx->callbacks->metricCall != NULL ) {
		ctx->returnValue = ctx->callbacks->metricCall(ctx->metric, ctx->callbacks->CallbackData);
		_clearMetric(ctx->metric);
		if ( ctx->returnValue != NULL ) {
		    return 0;
		}
	    } else {
		DeleteMetric(ctx->metric);
	    }
	} else if ( ctx->callbacks->bugSumCall != NULL && ctx->bugSummaries != NULL) {
            ctx->returnValue = ctx->callbacks->bugSumCall(ctx->bugSummaries, ctx->callbacks->CallbackData);
	    //DeleteBugSummary(ctx->bugSummaries);
	    if ( ctx->returnValue != NULL ) {
		return 0;
	    }
	} else if ( ctx->callbacks->metricSumCall != NULL && ctx->metricSummaries != NULL) {
            ctx->returnValue = ctx->callbacks->metricSumCall(ctx->metricSummaries, ctx->callbacks->CallbackData);
	    //DeleteMetricSummary(ctx->metricSummaries);
	    if ( ctx->returnValue != NULL ) {
		return 0;
	    }
	}
    } else if (ctx->depth == 3) {
	if (ctx->isArray) {
	    if ( strncmp( ctx->arrayType, "BugLocations", ctx->arrayTypeLength ) == 0 ) {
		ctx->bug->locationsCount++;
		if ( ctx->bug->locationsCount >= ctx->bug->locationsSize ) {
		    ctx->bug->locationsSize = ctx->bug->locationsSize * 2;
		    int *tempArray = realloc(ctx->bug->locations, ctx->bug->locationsSize * sizeof(Location));
		    if (tempArray) {
			 ctx->bug->locations = (Location *)tempArray;
			 memset(&ctx->bug->locations[ctx->bug->locationsCount], 0, (ctx->bug->locationsSize/2) * sizeof(Location));
		    } else {
		        printf("Could not expand Locations  array. Exiting parsing");
	                exit(1);
	            }
                }

//                if (ctx->bug->bugLocations == NULL) {
//                    ctx->bug->bugLocations = ctx->loc;
//                } else {
//                    Location *  cur = ctx->bug->bugLocations;
//                    while (cur->next != NULL) {
//                        cur = cur->next;
//                    }
//                    cur->next = ctx->loc;
//                }

	    } else if  ( strncmp( ctx->arrayType, "Methods", ctx->arrayTypeLength ) == 0 ) {
		ctx->bug->methodsCount++;
		if ( ctx->bug->methodsCount >= ctx->bug->methodsSize ) {
		    ctx->bug->methodsSize = ctx->bug->methodsSize * 2;
		    int *tempArray = realloc(ctx->bug->methods, ctx->bug->methodsSize * sizeof(Method));
		    if (tempArray) {
			ctx->bug->methods = (Method *)tempArray;
			memset(&ctx->bug->methods[ctx->bug->methodsCount], 0, (ctx->bug->methodsSize/2) * sizeof(Method));
		    } else {
		        printf("Could not expand Methods array. Exiting parsing");
	                exit(1);
	            }
                }

//	        if (ctx->bug->methods == NULL) {
//		    ctx->bug->methods = ctx->method;
//	        } else {
//		    Method *  cur = ctx->bug->methods;
//		    while (cur->next != NULL) {
//			cur = cur->next;
//		    }
//		    cur->next = ctx->method;
//	        }
	    }
	} else {
            if ( strcmp(ctx->hashType, "bugsum") == 0 ) {
                if (ctx->bugSummaries == NULL) {
                    ctx->bugSummaries = ctx->bugSum;
                } else {
                    BugSummary *  cur = ctx->bugSummaries;
                    while (cur->next != NULL) {
                        cur = cur->next;
                    }
                    cur->next = ctx->bugSum;
                }
	    } else if ( strcmp(ctx->hashType, "metrsum") == 0 ) {
                if (ctx->metricSummaries == NULL) {
                    ctx->metricSummaries = ctx->metricSum;
                } else {
                    MetricSummary *  cur = ctx->metricSummaries;
                    while (cur->next != NULL) {
			cur = cur->next;
                    }
                    cur->next = ctx->metricSum;
                }
	    }
        }
    } else if ( ctx->depth == 0 ) {
        if ( ctx->callbacks->initialCall != NULL && ! ctx->requiredStart ) {
            ctx->returnValue = ctx->callbacks->initialCall(ctx->initial, ctx->callbacks->CallbackData);
	    DeleteInitial(ctx->initial);
	    if ( ctx->returnValue != NULL ) {
		return 0;
	    }
	}
    }
    if ( ctx->isArray ) {
        ctx->arrayLoc = ctx->arrayLoc + 1;
    }
    return 1;
}

static int handle_start_array(void * data)
{
    struct State * ctx = (struct State *) data;
    if ( ctx->depth > 2 ) {
        ctx->isArray = 1;
	ctx->arrayType = malloc(ctx->currLength);
        strcpy(ctx->arrayType, ctx->curr);
	ctx->arrayTypeLength = ctx->currLength;
	if ( strncmp("BugLocations", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
	    if ( ctx->bug->locations == NULL ) {
                ctx->bug->locationsSize = 5;
                ctx->bug->locationsCount = 0;
                ctx->bug->locations = calloc(1, ctx->bug->locationsSize * sizeof(Location));
            }
	}
	if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
            if ( ctx->bug->methods == NULL ) {
                ctx->bug->methodsSize = 5;
                ctx->bug->methodsCount = 0;
                ctx->bug->methods = calloc(1, ctx->bug->methodsSize * sizeof(Method));
            }
	}
    }
    return 1;
}

static int handle_end_array(void * data)
{
    struct State * ctx = (struct State *) data;
    ctx->isArray = 0;
    if (ctx->depth == 3) {
	free(ctx->arrayType);
	ctx->arrayType = NULL;
    }
    return 1;
}

static yajl_callbacks callbacks = {
    handle_null,
    handle_boolean,
    NULL,
    NULL,
    handle_number,
    handle_string,
    handle_start_map,
    handle_map_key,
    handle_end_map,
    handle_start_array,
    handle_end_array
};


//////////////////////initializer and parser////////////////////////////////////////
ScarfJSONReader * NewScarfJSONReaderFromFilename(char * filename)
{
    struct State * status = calloc(1, sizeof(struct State));
    struct Callback * calls= calloc(1, sizeof(struct Callback));
    status->callbacks = calls;
    ScarfJSONReader * reader = calloc(1, sizeof(ScarfJSONReader));
    reader->file = fopen(filename, "r");
    if (reader->file == NULL){
        printf("File could not open\n");
        return NULL;
    }
    reader->filetype = 0;
    reader->utf8 = 1;
    status->bug = calloc(1, sizeof(BugInstance));
    status->metric = calloc(1, sizeof(Metric));
    reader->state = status;
    return reader;
}

ScarfJSONReader * NewScarfJSONReaderFromFd(int fd)
{
    struct State * status = calloc(1, sizeof(struct State));
    struct Callback * calls= calloc(1, sizeof(struct Callback));
    status->callbacks = calls;
    ScarfJSONReader * reader = calloc(1, sizeof(ScarfJSONReader));
    reader->filetype = 1;
    reader->utf8 = 1;
    reader->file = fdopen(fd, "w");;
    status->bug = calloc(1, sizeof(BugInstance));
    status->metric = calloc(1, sizeof(Metric));
    reader->state = status;
    return reader;
}

ScarfJSONReader * NewScarfJSONReaderFromFile(FILE * file)
{
    struct State * status = calloc(1, sizeof(struct State));
    struct Callback * calls= calloc(1, sizeof(struct Callback));
    status->callbacks = calls;
    ScarfJSONReader * reader = calloc(1, sizeof(ScarfJSONReader));
    reader->filetype = 1;
    reader->utf8 = 1;
    reader->file = file;
    status->bug = calloc(1, sizeof(BugInstance));
    status->metric = calloc(1, sizeof(Metric));
    reader->state = status;
    return reader;
}
ScarfJSONReader * NewScarfJSONReaderFromString(char * str, size_t * size)
{
    struct State * status = calloc(1, sizeof(struct State));
    struct Callback * calls= calloc(1, sizeof(struct Callback));
    status->callbacks = calls;
    ScarfJSONReader * reader = calloc(1, sizeof(ScarfJSONReader));
    reader->file = open_memstream (&str, size);
    if (reader->file == NULL){
        printf("File could not open\n");
        return NULL;
    }
    reader->filetype = 2;
    reader->utf8 = 1;
    status->bug = calloc(1, sizeof(BugInstance));
    status->metric = calloc(1, sizeof(Metric));
    reader->state = status;
    return reader;
}

void DeleteScarfJSONReader(ScarfJSONReader * reader) {
    yajl_free(reader->reader);
    free(reader->state->arrayType);
    free(reader->state->curr);
    free(reader->state->callbacks);
    free(reader->state);
    if(reader->filetype == 1 || reader->filetype == 2) {
	fclose(reader->file);
    }
    free(reader);
}

void ScarfJSONReaderSetUTF8(ScarfJSONReader * reader, int value){
    reader->utf8 = value;
}

void ScarfJSONReaderSetBugCallback(ScarfJSONReader * reader, BugCallback callback) {
    reader->state->callbacks->bugCall = callback;
}
void ScarfJSONReaderSetMetricCallback(ScarfJSONReader * reader, MetricCallback callback) {
    reader->state->callbacks->metricCall = callback;
}
void ScarfJSONReaderSetBugSummaryCallback(ScarfJSONReader * reader, BugSummaryCallback callback) {
    reader->state->callbacks->bugSumCall = callback;
}
void ScarfJSONReaderSetMetricSummaryCallback(ScarfJSONReader * reader, MetricSummaryCallback callback) {
    reader->state->callbacks->metricSumCall = callback;
}
void ScarfJSONReaderSetFinalCallback(ScarfJSONReader * reader, FinalCallback callback) {
    reader->state->callbacks->finalCallback = callback;
}
void ScarfJSONReaderSetInitialCallback(ScarfJSONReader * reader, InitialCallback callback) {
    reader->state->callbacks->initialCall = callback;
}
void ScarfJSONReaderSetCallbackData(ScarfJSONReader * reader, void * callbackData) {
    reader->state->callbacks->CallbackData = callbackData;
}


int ScarfJSONReaderGetUTF8(ScarfJSONReader * reader) {
    return reader->utf8;
}

BugCallback ScarfJSONReaderGetBugCallback(ScarfJSONReader * reader) {
    return reader->state->callbacks->bugCall;
}
MetricCallback ScarfJSONReaderGetMetricCallback(ScarfJSONReader * reader) {
    return reader->state->callbacks->metricCall;
}
BugSummaryCallback ScarfJSONReaderGetBugSummaryCallback(ScarfJSONReader * reader) {
    return reader->state->callbacks->bugSumCall;
}
MetricSummaryCallback ScarfJSONReaderGetMetricSummaryCallback(ScarfJSONReader * reader) {
    return reader->state->callbacks->metricSumCall;
}
FinalCallback ScarfJSONReaderGetFinalCallback(ScarfJSONReader * reader) {
    return reader->state->callbacks->finalCallback;
}
InitialCallback ScarfJSONReaderGetInitialCallback(ScarfJSONReader * reader) {
    return reader->state->callbacks->initialCall;
}
void * ScarfJSONReaderGetCallbackData(ScarfJSONReader * reader) {
    return reader->state->callbacks->CallbackData;
}


void * ScarfJSONReaderParse(ScarfJSONReader * hand)
{
    hand->reader =  yajl_alloc(&callbacks, NULL, hand->state);
    yajl_config(hand->reader, yajl_dont_validate_strings, hand->utf8);
    int retval = 0;
//    yajl_handle hand;
    static unsigned char fileData[65536];
    size_t rd;
    yajl_status stat;
    FILE * fh = hand->file;
    for (;;) {
        rd = fread((void *) fileData, 1, sizeof(fileData) - 1, fh);
        if (rd == 0) {
            if (!feof(fh)) {
                fprintf(stderr, "error on file read->\n");
                retval = 1;
            }
            break;
        }
        fileData[rd] = 0;
        stat = yajl_parse(hand->reader, fileData, rd);
        if (stat != yajl_status_ok) break;
    }
/*    if (stat != yajl_status_ok) {
        unsigned char * str = yajl_Get_error(hand->reader, 1, fileData, rd);
        fprintf(stderr, "%s", (const char *) str);
        yajl_free_error(hand->reader, str);
        retval = 1;
    }*/
    if ( hand->state->callbacks->finalCallback != NULL ) {
	hand->state->returnValue = hand->state->callbacks->finalCallback(hand->state->returnValue, hand->state->callbacks->CallbackData);
    }
    return hand->state->returnValue;
}

