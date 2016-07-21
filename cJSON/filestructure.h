#ifndef GRANDFATHER_H
#define GRANDFATHER_H


/////////////////////////Summary Data Struct///////////////////////////////////////
typedef struct BugSummary{
    char * code;
    char * group;
    int count;
    int bytes;
    struct BugSummary * next;
} BugSummary;


typedef struct MetricSummary{
    char * type;
    int valid;
    double max;
    double min;
    double sum;
    double sumOfSquares;
    double stdDeviation;
    double average;
    double count;
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
    char *clas;
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



#endif

