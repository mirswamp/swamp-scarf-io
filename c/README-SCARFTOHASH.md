### NAME
ScarfToHash - A C tool for parsing SCARF
### SYNOPSIS
```c
#include <stdio.h>
#include "ScarfToHash.c"

int main(int argc, char **argv) {
    Reader *reader = newReader(BUGCALLBACK, BUGSUMCALLBACK, METRICCALLBACK, METRICSUMCALLBACK, INITIALCALLBACK, FINISHCALLBACK, CONTEXT);
    if(reader == NULL){
	printf("reader failed\n");
	return 1;
    }

    parse(reader, "../blast+-2.2.28---test-centos-6.7-64---gcc-warn---justparse/parsed_results.xml");
    return 0;
}
```
### DESCRIPTION
This module provides the ability to convert SCARF files into C data structures. It is dependant on libxml, which is used to handle parsing of the XML document.

The parser is controlled primarily by the callbacks the user sets prior to calling the parse method. A callback will be called once the parser is finished parsing a section of the document. These sections are the beginning AnalyzerReport tag, an entire BugInstance or Metric, a complete BugSummary or MetricSummary and the end AnalyzerReport tag.

All Callbacks except the FinishCallback receive as parameters a reference to a struct containing information on their section of parsed data and the context reference provided in the initializer. Upon reaching the end of an Analyzer Report the FinishCallback  will be called with just the context reference.

While the parser does do minor checks to ensure the inputted file is a SCARF file, if an invalid SCARF file is passed in, most behavior will be undefined.
### METHODS
#### ScarfToHash * newScarfToHash(BugCallback, BugSummaryCallback, MetricCallback, MetricSummaryCallback, InitialCallback, FinishCallback, void * context)

####

####

####

#### parse()
This method initiates the parsing of the set file. If parsing fails an exit(1) call will be thrown, otherwise will return 0 on completion.

####


### CALLBACKS
The main purpose of this module is to interpret data from libxml and assemble them into usable C data structures. When parsing, the module will call the calllbacks defined in the initializer upon completion of parsing an object of their respective type. All callbacks receive the context reference as an additional parameter. For details on the structure of each individual C struct see below.

#### InitialCallback(Initial * intitial-data, void * context)
This is called just after the AnalyzerReport start tag is parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### MetricCallback(Metric * metric-data, void * context)
This is called every time a single Metric completes parsing. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### BugCallback(BugInstance * bug-data, void * context)
This is called every time a single BugInstance completes parsing. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### BugSummaryCallback(BugSummary * bug-summary-data, void * context)
This is called after all BugSummaries have been parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### MetricSummaryCallback(MetricSummary * metric-summary-data, void * context)
This is called once all MetricSummaries have been parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### FinishCallback(void * context)*
This is called after reaching an AnalayzerReport end tag.


### DATA STRUCTURES

The following are the data structures used in the callbacks listed above. Elements not defined in the Scarf file will be NULL.

#### InitialData
```
{
    char * tool_name
    char * tool_version
    char * uuid
} 
```
#### BUGDATA
```
{
    int bugId
    char * className
    char * bugSeverity
    char * bugRank
    char * resolutionSuggestion
    char * bugMessage
    char * bugCode
    char * bugGroup
    char * assessmentReportFile
    char * buildId
    CweIds * cweIds
    InstanceLocation * instanceLocation
    Method * methods
    Location * bugLocations
}
```
##### CweIds
```
{
    int cweid
    CweIds * next
}
```
##### InstanceLocation
```
{
    LineNum lineNum
    char * xPath
}
```
###### LineNum
```
{
    int start
    int end
}
```
##### Method
```
{
    int methodId
    int primary
    char * name
    Method * next
}
```
##### Location
```
{
    int primary
    int startLine
    int endLine
    int startColumn
    int endColumn
    int locationId
    char * explanation
    char * sourceFile
    Location * next
}
```
#### Metric
```
{
    int id
    char * value
    char * clas
    char * method
    char * sourceFile
    char * type
}
```

#### BugSummary
```
{
    int count
    int byteCount
    char * code
    char * group
    BugSummary * next
}
```

#### MetricSummary
```
{
    double count
    double sum
    double sumOfSquares
    double max
    double min
    double stdDeviation
    double average
    int valid
    char * type
    MetricSummary * next
}
```

