#include <yajl/yajl_parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filestructure.h"

typedef void * (*BugCallback)(BugInstance * bug, void * reference); 
typedef void * (*BugSummaryCallback)(BugSummary * bugSum, void * reference);
typedef void * (*MetricCallback)(Metric * metr, void * reference);
typedef void * (*MetricSummaryCallback)(MetricSummary * metrSum, void * reference);
typedef void * (*InitialCallback)(Initial * initial, void * reference);
typedef void * (*FinalCallback)(void * returnValue, void * reference);

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
} ScarfJSONReader;


////////////////////////COPY DATA//////////////////////////////////////////////
BugInstance *CopyBug(BugInstance *bug) {
    BugInstance *ret = calloc(1, sizeof(BugInstance));
    ret->bugId = bug->bugId;
    if (bug->cweIds != NULL) {
        ret->cweIds = malloc(sizeof(CweIds));
        ret->cweIds->size = bug->cweIds->size;
        ret->cweIds->count = bug->cweIds->count;
        ret->cweIds->cweids = malloc(ret->cweIds->size * sizeof(int));
        memcpy(ret->cweIds->cweids, bug->cweIds->cweids, bug->cweIds->size * sizeof(int));
    }

    ret->instanceLocation = bug->instanceLocation;
    if (bug->className != NULL) {
        ret->className =  malloc(strlen(bug->className) + 1);
        strcpy(ret->className, bug->className);
    }
    if (bug->cweIds != NULL) {
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
    if (bug->cweIds != NULL) {
        ret->bugMessage = malloc(strlen(bug->bugMessage) + 1);
        strcpy(ret->bugMessage, bug->bugMessage);
    }
    if (bug->cweIds != NULL) {
        ret->bugCode = malloc(strlen(bug->bugCode) + 1);
        strcpy(ret->bugCode, bug->bugCode);
    }
    if (bug->cweIds != NULL) {
        ret->bugGroup =  malloc(strlen(bug->bugGroup) + 1 );
        strcpy(ret->bugGroup, bug->bugGroup);
    }
    if (bug->cweIds != NULL) {
        ret->assessmentReportFile = malloc(strlen(bug->assessmentReportFile) + 1);
        strcpy(ret->assessmentReportFile, bug->assessmentReportFile);
    }
    if (bug->cweIds != NULL) {
        ret->buildId = malloc(strlen(bug->buildId) + 1);
        strcpy(ret->buildId, bug->buildId);
    }
    if (bug->cweIds != NULL) {
        ret->methods = malloc(sizeof(Methods));
        ret->methods->size = bug->methods->size;
        ret->methods->count = bug->methods->count;
        ret->methods->methods =  malloc(ret->methods->size * sizeof(Method));
        int i;
        for ( i = 0; i < ret->methods->count; i++ ) {
            ret->methods->methods[i].methodId = bug->methods->methods[i].methodId;
            ret->methods->methods[i].primary = bug->methods->methods[i].primary;
            ret->methods->methods[i].name =  malloc(strlen(bug->methods->methods[i].name) + 1);
            strcpy(ret->methods->methods[i].name, bug->methods->methods[i].name);
        }
    }
    if (bug->cweIds != NULL) {
        ret->bugLocations =  malloc(sizeof(BugLocations));
        ret->bugLocations->size = bug->bugLocations->size;
        ret->bugLocations->count = bug->bugLocations->count;
        ret->bugLocations->locations =  malloc(ret->bugLocations->size * sizeof(Location));
        for ( i = 0; i < ret->bugLocations->count; i++ ) {
            Location * retloc = &ret->bugLocations->locations[i];
            Location * bugloc = &bug->bugLocations->locations[i];
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
int DeleteInitial( Initial * initial ){
    free( initial->tool_name );
    free( initial->tool_version );
    free( initial->uuid );
    free( initial );
    return 0;
}


///////////////////////////////Delete a Metric///////////////////////////////////
int DeleteMetric(Metric * metric)
{
    free( metric->type);
    free( metric->className);
    free( metric->methodName);
    free( metric->sourceFile);
    free( metric->value);
    free(metric);
    return 0;
}

///////////////////////////////Delete a BugInstance///////////////////////////////////
int DeleteBug(BugInstance * bug)
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

    CweIds * cwe = bug->cweIds;
    if (cwe !=  NULL){
	free(cwe->cweids);
	free(cwe);
    }

    Methods * method = bug->methods;
    if ( method != NULL ) {
	int i;
	for ( i < 0; i < method->count; i++){
	    free(method->methods[i].name);
	}
	free(method->methods);
	free(method);
    }

    BugLocations * bugloc = bug->bugLocations;
    if (bugloc != NULL) {
	int i;
	for(i = 0; i < bugloc->count; i++) {
	    free(bugloc->locations[i].sourceFile);
	    free(bugloc->locations[i].explanation);
	}
	free(bugloc->locations);
	free(bugloc);
    }

    free(bug);
    return 0;
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

    CweIds * cwe = bug->cweIds;
    if (cwe !=  NULL){
	free(cwe->cweids);
	free(cwe);
    }

    Methods * method = bug->methods;
    if ( method != NULL ) {
	int i;
	for ( i < 0; i < method->count; i++){
	    free(method->methods[i].name);
	}
	free(method->methods);
	free(method);
    }

    BugLocations * bugloc = bug->bugLocations;
    if (bugloc != NULL) {
	int i;
	for(i = 0; i < bugloc->count; i++) {
	    free(bugloc->locations[i].sourceFile);
	    free(bugloc->locations[i].explanation);
	}
	free(bugloc->locations);
	free(bugloc);
    }

    memset(bug, 0, sizeof(BugInstance));
    return 0;
}

////////////////////////handlers//////////////////////////////
static int handle_null(void * ctx)
{
    return 1;
}

static int handle_boolean(void * data, int boolean)
{
    struct State * ctx = (struct State *) data;
    if ( strcmp("bug", ctx->hashType) == 0 ) {
        if ( ctx->isArray ) {
            if ( strcmp("BugLocations", ctx->arrayType) == 0 ) {
                if ( boolean ) {
                    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].primary = 1;
		}
                else {
                    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].primary = 0;
                    //self->data[self->arrayType][self->arrayLoc]["primary"] = 0
		}
	    } else if ( strcmp("Methods",ctx->arrayType) == 0) {
                if ( boolean ) {
                    ctx->bug->methods->methods[ctx->bug->methods->count].primary = 1;
		}
                else {
                    ctx->bug->methods->methods[ctx->bug->methods->count].primary = 0;
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
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].primary = number;
		} else if (strncmp("LocationId", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->locationId = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].locationId = number;
		} else if (strncmp("StartLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startLine = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].startLine = number;
		} else if (strncmp("EndLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endLine = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].endLine = number;
		} else if (strncmp("StartColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startColumn = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].startColumn = number;
		} else if (strncmp("EndColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endColumn = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].endColumn = number;
		}
    	    } else if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    //ctx->method->primary = number;
		    ctx->bug->methods->methods[ctx->bug->methods->count].primary = number;
		} else if (strncmp("MethodId", ctx->curr, ctx->currLength) == 0) {
                    //ctx->method->methodId = number;
		    ctx->bug->methods->methods[ctx->bug->methods->count].methodId = number;
		}
	    } else if ( strncmp("CweIds", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		
		if ( ctx->bug->cweIds == NULL ) {
		    ctx->bug->cweIds = malloc(sizeof(CweIds));
		    ctx->bug->cweIds->size = 5;
		    ctx->bug->cweIds->count = 0;
		    ctx->bug->cweIds->cweids = malloc(ctx->bug->cweIds->size * sizeof(int));
		}
		if ( ctx->bug->cweIds->count >= ctx->bug->cweIds->size ) {
		    ctx->bug->cweIds->size = ctx->bug->cweIds->size * 2;
		    int *tempArray = realloc(ctx->bug->cweIds->cweids, ctx->bug->cweIds->size * sizeof(int));
		    if (tempArray) {
			ctx->bug->cweIds->cweids = tempArray;
		    } else {
			printf("Could not expand CweID array. Exiting parsing");
			exit(1);
		    }
		}	
		
		ctx->bug->cweIds->cweids[ctx->bug->cweIds->count] = number;
		ctx->bug->cweIds->count++;
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
	    ctx->bugSum->bytes = number;
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
			ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].primary = 1;
		    } else {
			//ctx->loc->primary = 0;
			ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].primary = 0;
		    }
                } else if (strncmp("LocationId", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->locationId = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].locationId = number;
                } else if (strncmp("StartLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startLine = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].startLine = number;
                } else if (strncmp("EndLine", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endLine = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].endLine = number;
                } else if (strncmp("StartColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->startColumn = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].startColumn = number;
                } else if (strncmp("EndColumn", ctx->curr, ctx->currLength) == 0) {
                    //ctx->loc->endColumn = number;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].endColumn = number;
                } else if (strncmp("SourceFile", ctx->curr, ctx->currLength) == 0) {
		    //ctx->loc->sourceFile = stringValue;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].sourceFile = stringValue;
		} else if (strncmp("Explanation", ctx->curr, ctx->currLength) == 0) {
		    //ctx->loc->explanation = stringValue;
		    ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count].explanation = stringValue;
                }


            } else if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
                if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    if ( strncmp(stringVal, "true", stringLen) == 0 ) {
//			ctx->method->primary = 1;
			ctx->bug->methods->methods[ctx->bug->methods->count].primary = 1;
		    } else {
//			ctx->method->primary = 0;
			ctx->bug->methods->methods[ctx->bug->methods->count].primary = 0;
		    }
                } else if (strncmp("MethodId", ctx->curr, ctx->currLength) == 0) {
//                    ctx->method->methodId = number;
		    ctx->bug->methods->methods[ctx->bug->methods->count].methodId = number;
                } else if (strncmp("name", ctx->curr, ctx->currLength) == 0) {
//		    ctx->method->name = stringValue;
		    ctx->bug->methods->methods[ctx->bug->methods->count].name = stringValue;
		}
            } else if ( strncmp("CweIds", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		if ( ctx->bug->cweIds == NULL ) {
		    ctx->bug->cweIds = malloc(sizeof(CweIds));
		    ctx->bug->cweIds->size = 5;
		    ctx->bug->cweIds->count = 0;
		    ctx->bug->cweIds->cweids = malloc(ctx->bug->cweIds->size * sizeof(int));
		}
		if ( ctx->bug->cweIds->count >= ctx->bug->cweIds->size ) {
		    ctx->bug->cweIds->size = ctx->bug->cweIds->size * 2;
		    int *tempArray = realloc(ctx->bug->cweIds->cweids, ctx->bug->cweIds->size * sizeof(int));
		    if (tempArray) {
			ctx->bug->cweIds->cweids = tempArray;
		    } else {
			printf("Could not expand CweID array. Exiting parsing");
			exit(1);
		    }
		}	
		
		ctx->bug->cweIds->cweids[ctx->bug->cweIds->count] = number;
		ctx->bug->cweIds->count++;
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
            ctx->bugSum->bytes = number;
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
		ctx->bug->bugLocations->count++;
		if ( ctx->bug->bugLocations->count >= ctx->bug->bugLocations->size ) {
		    ctx->bug->bugLocations->size = ctx->bug->bugLocations->size * 2;
		    int *tempArray = realloc(ctx->bug->bugLocations->locations, ctx->bug->bugLocations->size * sizeof(Location));
		    if (tempArray) {
			 ctx->bug->bugLocations->locations = (Location *)tempArray;
			 memset(&ctx->bug->bugLocations->locations[ctx->bug->bugLocations->count], 0, (ctx->bug->bugLocations->size/2) * sizeof(Location));
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
		ctx->bug->methods->count++;
		if ( ctx->bug->methods->count >= ctx->bug->methods->size ) {
		    ctx->bug->methods->size = ctx->bug->methods->size * 2;
		    int *tempArray = realloc(ctx->bug->methods->methods, ctx->bug->methods->size * sizeof(Methods));
		    if (tempArray) {
			ctx->bug->methods->methods = (Method *)tempArray;
			memset(&ctx->bug->methods->methods[ctx->bug->methods->count], 0, (ctx->bug->methods->size/2) * sizeof(Method));
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
		    int i = 0;
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
	    if ( ctx->bug->bugLocations == NULL ) {
                ctx->bug->bugLocations = malloc(sizeof(BugLocations));
                ctx->bug->bugLocations->size = 5;
                ctx->bug->bugLocations->count = 0;
                ctx->bug->bugLocations->locations = calloc(1, ctx->bug->bugLocations->size * sizeof(Location));
            }
	}
	if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
            if ( ctx->bug->methods == NULL ) {
                ctx->bug->methods = malloc(sizeof(Methods));
                ctx->bug->methods->size = 5;
                ctx->bug->methods->count = 0;
                ctx->bug->methods->methods = calloc(1, ctx->bug->methods->size * sizeof(Method));
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

void SetBugCallback(ScarfJSONReader * reader, BugCallback callback) {
    reader->state->callbacks->bugCall = callback;
}
void SetMetricCallback(ScarfJSONReader * reader, MetricCallback callback) {
    reader->state->callbacks->metricCall = callback;
}
void SetBugSummaryCallback(ScarfJSONReader * reader, BugSummaryCallback callback) {
    reader->state->callbacks->bugSumCall = callback;
}
void SetMetricSummaryCallback(ScarfJSONReader * reader, MetricSummaryCallback callback) {
    reader->state->callbacks->metricSumCall = callback;
}
void SetFinalCallback(ScarfJSONReader * reader, FinalCallback callback) {
    reader->state->callbacks->finalCallback = callback;
}
void SetInitialCallback(ScarfJSONReader * reader, InitialCallback callback) {
    reader->state->callbacks->initialCall = callback;
}
void SetCallbackData(ScarfJSONReader * reader, void * callbackData) {
    reader->state->callbacks->CallbackData = callbackData;
}


BugCallback GetBugCallback(ScarfJSONReader * reader, BugCallback callback) {
    return reader->state->callbacks->bugCall;
}
MetricCallback GetMetricCallback(ScarfJSONReader * reader, MetricCallback callback) {
    return reader->state->callbacks->metricCall;
}
BugSummaryCallback GetBugSummaryCallback(ScarfJSONReader * reader, BugSummaryCallback callback) {
    return reader->state->callbacks->bugSumCall;
}
MetricSummaryCallback GetMetricSummaryCallback(ScarfJSONReader * reader, MetricSummaryCallback callback) {
    return reader->state->callbacks->metricSumCall;
}
FinalCallback GetFinalCallback(ScarfJSONReader * reader, FinalCallback callback) {
    return reader->state->callbacks->finalCallback;
}
InitialCallback GetInitialCallback(ScarfJSONReader * reader, InitialCallback callback) {
    return reader->state->callbacks->initialCall;
}
void * GetCallbackData(ScarfJSONReader * reader, void * callbackData) {
    return reader->state->callbacks->CallbackData;
}


void * Parse(ScarfJSONReader * hand)
{
    hand->reader =  yajl_alloc(&callbacks, NULL, hand->state);
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

