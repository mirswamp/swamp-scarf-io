#ifndef GRANDFATHER_H
#define GRANDFATHER_H

/////////////////Summary structs ///////////////////////////
typedef struct BugSummary {
    int count;
    int byteCount;
    char * code;
    char * group;
    struct BugSummary * next;
} BugSummary;


typedef struct BugSummaries {
    char * code;
    struct BugSummary * codeSummary;
    struct BugSummaries * next;
} BugSummaries;


typedef struct MetricSummary {
    double count;
    double sum;
    double sumOfSquares;
    double max;
    double min;
    double stdDeviation;
    double average;
    int valid;
    char * type;
    struct MetricSummary * next;
} MetricSummary;



/////////////////////////Initial Data Struct///////////////////////////////////////
typedef struct Initial{
    char *tool_name;
    char *tool_version;
    char *uuid;
} Initial;


/////////////////////////Metric Structs////////////////////////////////////////////
typedef struct Metric{
    int id;
    char *value;
    char *className;
    char *methodName;
    char *sourceFile;
    char *type;
} Metric;


///////////////////////////Bug Structs////////////////////////////////////////////
typedef struct Method{
    int methodId;
    int primary;
    char *name;
} Method;

typedef struct Methods{
    Method *methods;
    int count;
    int size;
} Methods;

typedef struct Location{
    int primary;
    int startLine;
    int endLine;
    int startColumn;
    int endColumn;
    int locationId;
    char *explanation;
    char *sourceFile;
} Location;

typedef struct BugLocations{
    Location *locations;
    int count;
    int size;
} BugLocations;

typedef struct {
    int start;
    int end;
} LineNum;


typedef struct {
    LineNum lineNum;
    char *xPath;
} InstanceLocation;


typedef struct CweIds{
    int *cweids;
    int count;
    int size;
} CweIds;


typedef struct {
    int bugId;
    CweIds *cweIds;
    InstanceLocation instanceLocation;
    char *className;
    char *bugSeverity;
    char *bugRank;
    char *resolutionSuggestion;
    char *bugMessage;
    char *bugCode;
    char *bugGroup;
    char *assessmentReportFile;
    char *buildId;
    Methods *methods;
    BugLocations *bugLocations;
} BugInstance;



#endif

