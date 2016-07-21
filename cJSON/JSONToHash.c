#include <yajl/yajl_parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filestructure.h"

typedef yajl_handle Reader;

typedef void (*BugCallback)(BugInstance * bug, void * reference); 
typedef void (*BugSummaryCallback)(BugSummary * bugSum, void * reference);
typedef void (*MetricCallback)(Metric * metr, void * reference);
typedef void (*MetricSummaryCallback)(MetricSummary * metrSum, void * reference);
typedef void (*InitialCallback)(Initial * initial, void * reference);

typedef struct Callback {
    BugCallback bugCall;
    MetricCallback  metricCall;
    InitialCallback initialCall;
    BugSummaryCallback bugSumCall;
    MetricSummaryCallback metricSumCall;
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

///////////////////////////////Free initial struct//////////////////////////////////
int freeInitial( Initial * initial ){
    free( initial->tool_name );
    free( initial->tool_version );
    free( initial->uuid );
    free( initial );
    return 1;
}


///////////////////////////////Free a Metric///////////////////////////////////
int freeMetric(Metric * metric)
{
    free( metric->type);
    free( metric->clas);
    free( metric->method);
    free( metric->sourceFile);
    free( metric->value);
    free(metric);
    return 1;
}

///////////////////////////////Free a BugInstance///////////////////////////////////
int freeBug(BugInstance * bug)
{
    int number = 0;
    free( bug->assessmentReportFile);
    free( bug->buildId);
    free( bug->bugCode);
    free( bug->bugRank);
    free( bug->className);
    free( bug->bugSeverity);
    free( bug->bugGroup);
    free( bug->bugMessage);
    free( bug->resolutionSuggestion);
    free( bug->instanceLocation);

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
        free( prevMethod->name);
        free(prevMethod);
    }

    Location * loc = bug->bugLocations;
    Location * prevLoc;
    while (loc != NULL) {
        prevLoc = loc;
        loc = loc->next;
        free( prevLoc->sourceFile);
        free( prevLoc->explanation);
        free(prevLoc);
    }
    number = number + 1;

    free(bug);
    return 0;
}

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
                    ctx->loc->primary = 1;
		}
                else {
                    ctx->loc->primary = 0;
                    //self->data[self->arrayType][self->arrayLoc]["primary"] = 0
		}
	    } else if ( strcmp("Methods",ctx->arrayType) == 0) {
                if ( boolean ) {
                    ctx->method->primary = 1;
		}
                else {
                    ctx->method->primary = 0;
                    //self->data[self->arrayType][self->arrayLoc]["primary"] = 0
		}
	    }
	}
    }
    return 1;
}

static int handle_number(void * data, const char * s, size_t l)
{
    struct State * ctx = (struct State *) data;
    char * stringNumber = malloc(l + 1);
    strncpy(stringNumber , s, l);
    stringNumber[l] = '\0';
    double number = strtod(stringNumber, NULL);
    free(stringNumber);
    if ( strcmp("bug", ctx->hashType) == 0 ) {
	if ( strncmp(ctx->curr, "Start", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation->lineNum.start = number;
	} else if ( strncmp(ctx->curr, "BugId", ctx->currLength) == 0 ) {
	    ctx->bug->bugId = number;
	} else if ( strncmp(ctx->curr, "End", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation->lineNum.end = number;
	} else if ( ctx->isArray ) {
	    if ( strncmp("BugLocations", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    ctx->loc->primary = number;
		} else if (strncmp("LocationId", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->locationId = number;
		} else if (strncmp("StartLine", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->startLine = number;
		} else if (strncmp("EndLine", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->endLine = number;
		} else if (strncmp("StartColumn", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->startColumn = number;
		} else if (strncmp("EndColumn", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->endColumn = number;
		}
    	    } else if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    ctx->method->primary = number;
		} else if (strncmp("MethodId", ctx->curr, ctx->currLength) == 0) {
                    ctx->method->methodId = number;
		}
	    } else if ( strncmp("CweIds", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
		CweIds * cwe = 	calloc(1, sizeof(CweIds));
		cwe->cweid = number;
		if (ctx->bug->cweIds == NULL) {
		    ctx->bug->cweIds = cwe;
		} else {
		    CweIds *  cur = ctx->bug->cweIds;
		    while (cur->next != NULL) {
			cur = cur->next;
		    }
		    cur->next = cwe;
		}
	    
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
		ctx->callbacks->initialCall(ctx->initial, ctx->callbacks->CallbackData);
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
	    ctx->bug->instanceLocation->xPath = stringValue; 
        } else if ( strncmp(ctx->curr, "Start", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation->lineNum.start = number;
        } else if ( strncmp(ctx->curr, "End", ctx->currLength) == 0 ) {
	    ctx->bug->instanceLocation->lineNum.end = number;
        
	} else if ( ctx->isArray ) {
	    if ( strncmp("BugLocations", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
                if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    if ( strncmp(stringVal, "true", stringLen) == 0 ) {
			ctx->loc->primary = 1;
		    } else {
			ctx->loc->primary = 0;
		    }
                } else if (strncmp("LocationId", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->locationId = number;
                } else if (strncmp("StartLine", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->startLine = number;
                } else if (strncmp("EndLine", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->endLine = number;
                } else if (strncmp("StartColumn", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->startColumn = number;
                } else if (strncmp("EndColumn", ctx->curr, ctx->currLength) == 0) {
                    ctx->loc->endColumn = number;
                } else if (strncmp("SourceFile", ctx->curr, ctx->currLength) == 0) {
		    ctx->loc->sourceFile = stringValue;
		} else if (strncmp("Explanation", ctx->curr, ctx->currLength) == 0) {
		    ctx->loc->explanation = stringValue;
                }


            } else if ( strncmp("Methods", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
                if (strncmp("primary", ctx->curr, ctx->currLength) == 0) {
		    if ( strncmp(stringVal, "true", stringLen) == 0 ) {
			ctx->method->primary = 1;
		    } else {
			ctx->method->primary = 0;
		    }
                } else if (strncmp("MethodId", ctx->curr, ctx->currLength) == 0) {
                    ctx->method->methodId = number;
                } else if (strncmp("name", ctx->curr, ctx->currLength) == 0) {
		    ctx->method->name = stringValue;
		}
            } else if ( strncmp("CweIds", ctx->arrayType, ctx->arrayTypeLength) == 0 ) {
                CweIds * cwe =  calloc(1, sizeof(CweIds));
                cwe->cweid = number;
                if (ctx->bug->cweIds == NULL) {
                    ctx->bug->cweIds = cwe;
                } else {
                    CweIds *  cur = ctx->bug->cweIds;
                    while (cur->next != NULL) {
                        cur = cur->next;
                    }
                    cur->next = cwe;
                }
	    }
	}

    } else if ( strcmp("metric", ctx->hashType) == 0 ) {
	if ( strncmp(ctx->curr, "MetricId", ctx->currLength) == 0 ) {
	    ctx->metric->id = number;	
	} else if ( strncmp(ctx->curr, "Value", ctx->currLength) == 0 ) {
            ctx->metric->value = stringValue; 
	} else if ( strncmp(ctx->curr, "Class", ctx->currLength) == 0 ) {
            ctx->metric->clas = stringValue;
	} else if ( strncmp(ctx->curr, "Method", ctx->currLength) == 0 ) {
            ctx->metric->method = stringValue;
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
//    } else if ( strncmp(stringVal, "BugLocations", stringLen) == 0 && ctx->depth == 3 ) {
//        ctx->bug->bugLocations = calloc(1, sizeof(Location));
//    } else if ( strncmp(stringVal, "Methods", stringLen) == 0 && ctx->depth == 3 ) {
//        ctx->bug->methods = calloc(1, sizeof(Method));
    } else if ( strncmp(stringVal, "InstanceLocation", stringLen) == 0 && ctx->depth == 3 ) {
        ctx->bug->instanceLocation = calloc(1, sizeof(InstanceLocation));
//    } else if ( stringVal == "LineNum" && ctx->depth == 4 ) {
//        ctx->bug->instanceLocation->LineNum = 
//    } else if ( ctx->hashType == "bugsum" && ctx->depth == 3 ) {
//        ctx->sumCode = stringVal;
//    } else if ( ctx->hashType == "bugsum" && ctx->depth == 4 ) {
//        ctx->sumGroup = stringVal;
//    } else if ( ctx->hashType == "metrsum" && ctx->depth == 3 ) {
//        ctx->sumGroup = stringVal;
    }
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
    if (ctx->isArray && ctx->depth == 3) {
	if ( strncmp( ctx->arrayType, "BugLocations", ctx->arrayTypeLength-1 ) == 0 ) {
	    ctx->loc = calloc(1, sizeof(Location)); 
	} else if  ( strncmp( ctx->arrayType, "Methods", ctx->arrayTypeLength-1 ) == 0 ) {
	    ctx->method = calloc(1, sizeof(Method)); 
    	} 
    } else if (ctx->depth == 3) {
	if ( strcmp(ctx->hashType, "bugsum") == 0 ) {
	    ctx->bugSum = calloc(1, sizeof(BugSummary));
	} else if ( strcmp(ctx->hashType, "metrsum") == 0 ) {
	    ctx->metricSum = calloc(1, sizeof(MetricSummary));
	}
    } else if (ctx->depth == 2) {
	if ( strcmp(ctx->hashType, "bug") == 0 ) {
	    ctx->bug = calloc(1, sizeof(BugInstance));
	} else if ( strcmp(ctx->hashType, "metric") == 0 ) {
	    ctx->metric = calloc(1, sizeof(Metric));
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
		ctx->callbacks->bugCall(ctx->bug, ctx->callbacks->CallbackData);
	    } else {
		freeBug(ctx->bug);
	    }
	} else if ( strcmp("metric", ctx->hashType) == 0 ) {
            if ( ctx->callbacks->metricCall != NULL ) {
		ctx->callbacks->metricCall(ctx->metric, ctx->callbacks->CallbackData);
	    } else {
		freeMetric(ctx->metric);
	    }
	} else if ( ctx->callbacks->bugSumCall != NULL && ctx->bugSummaries != NULL) {
            ctx->callbacks->bugSumCall(ctx->bugSummaries, ctx->callbacks->CallbackData);
	} else if ( ctx->callbacks->metricSumCall != NULL && ctx->metricSummaries != NULL) {
            ctx->callbacks->metricSumCall(ctx->metricSummaries, ctx->callbacks->CallbackData);
	}
    } else if (ctx->depth == 3) {
	if (ctx->isArray) {
	    if ( strncmp( ctx->arrayType, "BugLocations", ctx->arrayTypeLength ) == 0 ) {
                if (ctx->bug->bugLocations == NULL) {
                    ctx->bug->bugLocations = ctx->loc;
                } else {
                    Location *  cur = ctx->bug->bugLocations;
                    while (cur->next != NULL) {
                        cur = cur->next;
                    }
                    cur->next = ctx->loc;
                }
	        	     
	    } else if  ( strncmp( ctx->arrayType, "Methods", ctx->arrayTypeLength ) == 0 ) {
	        if (ctx->bug->methods == NULL) {
		    ctx->bug->methods = ctx->method;
	        } else {
		    Method *  cur = ctx->bug->methods;
		    while (cur->next != NULL) {
			cur = cur->next;
		    }
		    cur->next = ctx->method;
	        }
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
            ctx->callbacks->initialCall(ctx->initial, ctx->callbacks->CallbackData);
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

//static void
//usage(const char * progname)
//{
//    fprintf(stderr, "%s: handle json from stdin\n"
//            "usage:  json_handle [options]\n"
//            "    -e escape any forward slashes (for embedding in HTML)\n"
//            "    -m minimize json rather than beautify (default)\n"
//            "    -s handle a stream of multiple json entites\n"
//            "    -u allow invalid UTF8 inside strings during parsing\n",
//            progname);
//    exit(1);
//}





/////////////////Initiailize a Metric//////////////////////////////////////////////
//Metric * initializeMetric()
//{
//    Metric * metric = calloc(1,sizeof(Metric));
//    return metric;
//}
//
//
/////////////////Initiailize a BugInstance//////////////////////////////////////////////
//BugInstance * initializeBug()
//{
//    BugInstance * bug = calloc(1,sizeof(BugInstance));
//    return bug;
//}
//
//
/////////////////////////////get initial tag//////////////////////////////////////
//Initial * nextInitial(Reader reader)
//{
//}
//
////////////////////Close parser////////////////////////////////////////////
//int closeReader(Reader reader)
//{
//}
//
//////////////////////Process next bug/////////////////////////////////////////
//BugInstance * nextBug(Reader reader)
//{
//}
//
//////////////////////Process next metric/////////////////////////////////////////
//Metric * nextMetric(Reader reader)
//{
//}

//////////////////////change filename/reset parser////////////////////////////////////////
Reader newReader(BugCallback BugInstance, BugSummaryCallback BugSummary, MetricCallback Metric, MetricSummaryCallback MetricSummary, InitialCallback Initial, void * reference)
{
    struct State * status = calloc(1, sizeof(struct State));
    struct Callback * calls= malloc(sizeof(struct Callback));
    calls->bugCall = BugInstance;
    calls->metricCall = Metric;
    calls->bugSumCall = BugSummary;
    calls->initialCall = Initial;
    calls->metricSumCall = MetricSummary;
    calls->CallbackData = reference;
    status->callbacks = calls;
    Reader reader =  yajl_alloc(&callbacks, NULL, status);
    return reader;
}


int parse(Reader hand, char * filename)
{
    int retval = 0;
//    yajl_handle hand;
    static unsigned char fileData[65536];
    size_t rd;
    yajl_status stat;
    FILE * fh = fopen(filename, "r");
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
        stat = yajl_parse(hand, fileData, rd);
        if (stat != yajl_status_ok) break;
    }
    if (stat != yajl_status_ok) {
        unsigned char * str = yajl_get_error(hand, 1, fileData, rd);
        fprintf(stderr, "%s", (const char *) str);
        yajl_free_error(hand, str);
        retval = 1;
    }
    yajl_free(hand);
    return retval;
}

//int
//main(int argc, char ** argv)
//{
//    /* generator config */
//    yajl_gen g;
//    int a = 1;
//    g = yajl_gen_alloc(NULL);
//    yajl_gen_config(g, yajl_gen_beautify, 1);
//    yajl_gen_config(g, yajl_gen_validate_utf8, 1);
//    /* ok->  open file->  let's read and parse */
//    /* and let's allow comments by default */
//    yajl_config(hand, yajl_allow_comments, 1);
//    /* check arguments->*/
//    while ((a < argc) && (argv[a][0] == '-') && (strlen(argv[a]) > 1)) {
//        unsigned int i;
//        for ( i=1; i < strlen(argv[a]); i++) {
//            switch (argv[a][i]) {
//                case 'm':
//                    yajl_gen_config(g, yajl_gen_beautify, 0);
//                    break;
//                case 's':
//                    yajl_config(hand, yajl_allow_multiple_values, 1);
//                    s_streamhandle = 1;
//                    break;
//                case 'u':
//                    yajl_config(hand, yajl_dont_validate_strings, 1);
//                    break;
//                case 'e':
//                    yajl_gen_config(g, yajl_gen_escape_solidus, 1);
//                    break;
//                default:
//                    fprintf(stderr, "unrecognized option: '%c'\n\n",
//                            argv[a][i]);
//                    usage(argv[0]);
//            }
//        }
//        ++a;
//    }
//    if (a < argc) {
//        usage(argv[0]);
//    }
//    for (;;) {
//        rd = fread((void *) fileData, 1, sizeof(fileData) - 1, stdin);
//        if (rd == 0) {
//            if (!feof(stdin)) {
//                fprintf(stderr, "error on file read->\n");
//                retval = 1;
//            }
//            break;
//        }
//        fileData[rd] = 0;
//        stat = yajl_parse(hand, fileData, rd);
//        if (stat != yajl_status_ok) break;
//        {
//            const unsigned char * buf;
//            size_t len;
//            yajl_gen_get_buf(g, &buf, &len);
//            fwrite(buf, 1, len, stdout);
//            yajl_gen_clear(g);
//        }
//    }
//    stat = yajl_complete_parse(hand);
//    if (stat != yajl_status_ok) {
//        unsigned char * str = yajl_get_error(hand, 1, fileData, rd);
//        fprintf(stderr, "%s", (const char *) str);
//        yajl_free_error(hand, str);
//        retval = 1;
//    }
//    yajl_gen_free(g);
//    yajl_free(hand);
//    return retval;
//}
