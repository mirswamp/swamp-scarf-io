### NAME
ScarfToHash - A perl module for parsing SCARF
### SYNOPSIS
```perl
use ScarfToHash;

my $input = "/path/to/file";
my $reader = new ScarfToHash($input);

$reader->SetInitialCallback(\&initial);
$reader->SetBugCallback(\&bugcall);
$reader->SetMetricCallback(\&metriccall);
$reader->SetBugSummaryCallback(\&bugsummary);
$reader->SetMetricSummaryCallback(\&metricsummary);
$reader->SetFinishCallback(\&finishcall);

$data = DATA;
$reader->SetCallbackData(\$data);

$reader->parse;
```
### DESCRIPTION
This module provides the ability to convert SCARF files into Perl data structures. It is dependant on XML::Parser library for parsing of the XML document.

The parser is controlled primarily by the callbacks the user sets prior to calling the parse method. A callback will be called once the parser is finished parsing a section of the document. These sections are the beginning AnalyzerReport tag, an entire BugInstance or Metric, a complete BugSummary or MetricSummary and the end AnalyzerReport tag.  

All Callbacks except the FinishCallback receive as parameters a reference to a hash containing information on their section of parsed data and the data stored in the CallbackData key of the callback hash. Upon reaching the end of an Analyzer Report the FinishCallback  will be called with just the CallbackData key.

While the parser does do minor checks to ensure the input file is a SCARF file, if an invalid SCARF file is passed in, the behavior is undefined.
### METHODS
#### new(FILE)
This is a class method used to instantiate the parser. FILE can be an open handle, a string containing the filename of a file, or a reference to a string containing the entire file.

#### Parse
This method initiates the parsing of the set file. If parsing fails an exception is thrown with an error message detailing where in the file it failed. The return value of parse will be the return value of the FinishCallback if it is defined. Otherwise the return value will be the same as the last callback executed.

#### SetInitialCallback(INITIALCALLBACK)
Sets the InitialCallback to be called after each start AnalyzerReport start tag is parsed.

#### SetBugCallback(BUGCALLBACK)
Sets the BugCallback to be called after each full BugInstance is parsed.

#### SetMetricCallback(METRICCALLBACK)
Sets the BugCallback to be called after each full BugInstance is parsed.

#### SetMetricSummaryCallback(METRICSUMMARYCALLBACK)
Sets the MetricSummaryCallback to be called after all metric summaries are parsed.

#### SetBugSummaryCallback(BUGSUMMARYCALLBACK)
Sets the BugSummaryCallback to be called after all metric summaries are parsed.

#### SetFinishCallback(FINISHCALLBACK)
Sets the FinishCallback to be called after each AnalyzerReport end tag is parsed.

#### SetCallbackData(CALLBACKDATA)
Sets the CallbackData to be used as an additional parameter to callbacks.

#### GetInitialCallback
Access current value set to InitialCallback.

#### GetBugCallback
Access current value set to BugCallback.

#### GetMetricCallback
Access current value set to MetricCallback.

#### GetMetricSummaryCallback
Access current value set to MetricSummaryCallback

#### GetBugSummaryCallback
Access current value set to BugSummaryCallback.

#### GetFinishCallback
Access current value set to FinishCallback.

#### GetCallbackData
Access current value of CallbackData.

### CALLBACKS
The main purpose of this module is to interpret the events generated from XML::Parser and assemble them into a usable Perl data structures. When parsing, the module will call the pre-defined callbacks upon completion of parsing an object of their respective type. If defined, all callbacks will  receive the data contained in the optional key "CallbackData" as a parameter. For details on the structure of each individual Perl data structure see below. 

####InitialCallback(INITIALDATA[, CALLBACKDATA])
This is called just after the opening AnalyzerReport tag is parsed. Any defined return value will terminate parsing and skip to FinishCallback.

#### MetricCallback(METRICDATA[, CALLBACKDATA])
This is called every time a single Metric completes parsing. Any defined return value will terminate parsing and skip to FinishCallback.

#### BugCallback(BUGDATA[, CALLBACKDATA])
This is called every time a single BugInstance completes parsing. Any defined return value will terminate parsing and skip to FinishCallback.

#### BugSummaryCallback(BUGSUMMARYDATA[, CALLBACKDATA])
This is called after all BugSummaries have been parsed. Any defined return value will terminate parsing and skip to FinishCallback.

#### MetricSummaryCallback(METRICSUMMARYDATA[, CALLBACKDATA])
This is called once all MetricSummaries have been parsed. Any defined return value will terminate parsing and skip to FinishCallback.

#### FinishCallback(RETURNVALUE[, CALLBACKDATA])
This is called after reaching an AnalayzerReport end tag. If one of the above callbacks terminates parsing with a defined return value, RETURNVALUE will equal that value, otherwise RETURNVALUE will be undef.


### DATA STRUCTURES


The following are the data structures used in the callbacks listed above. If a keys value is not defined in the SCARF file, then the corresponding key will not exist in the data structures.

#### INITIALDATA
```
{
    tool_name => TOOLNAMEVALUE,
    tool_verison => TOOLVERSIONVALUE 
    uuid => UUIDVALUE,
} 
```

#### BUGDATA
```
{
    BugId => BUGIDVALUE,                          
    BugGroup => GROUPVALUE,        
    BugCode => CODEVALUE,                     
    BugMessage => BUGMESSAGEVALUE,
    BugRank => BUGRANKVALUE,                       
    BugSeverity => SEVERITYVALUE,                 
    ResolutionSuggestion => RESOLUTIONSUGGESTIONVALUE
    AssessmentReportFile => ASSESSREPORTVALUE, 
    BuildId => BUILDIDVALUE,                        
    InstanceLocation => {                  
        Xpath => XPATHVALUE,
	LineNum => { 
            Start = STARTVALUE
            End = ENDVALUE
        }            
    },                                        
    CweIds => [                            
        CWEIDVALUE, CWEIDVALUE, CWEIDVALUE                         
        ],                                    
    ClassName => CLASSVALUE,                  
    Methods => [                           
        {                                    
            MethodId => METHODIDVALUE,               
            name => METHODNAMEVALUE,             
            primary => PRIMARYVALUE                   
        },                                   
        {                                    
            MethodId => METHODIDVALUE,               
            name => METHODNAMEVALUE,             
            primary => PRIMARYVALUE                   
        }                                    
    ],                                       
    BugLocations => [                      
        {                                    
            LocationId => LOCIDVALUE,             
            SourceFile => SOURCEVALUE         
            StartLine => STARTLINEVALUE,              
            EndLine => ENDLINEVALUE,                
            primary => PRIMARYVALUE,                  
        },                                   
        {                                    
            LocationId => LOCIDVALUE,             
            SourceFile => SOURCEVALUE,        
            StartLine => STARTLINEVALUE,            
            EndLine => ENDLINEVALUE,              
            StartColumn => STARTCOLUMNVALUE             
            EndColumn => ENDCOLUMNVALUE,              
            primary => PRIMARYVALUE,                  
            Explanation => EXPLANATIONVALUE,    
        }                                    
    ],                                       
}
```

#### METRICDATA
```
{
    Value => VALUE,          
    Type => TYPEVALUE,       
    Method => METHODVALUE,   
    Class => CLASSVALUE,     
    SourceFile => SOURCEVALUE,
    MetricId => METRICIDVALUE 
}
```

#### BUGSUMMARYDATA
```
{
BugGroup => {
    BugCode => {
        bytes => BYTESVALUE,
        count => COUNTVALUE
        }
    BugCode => {
        bytes => BYTESVALUE,
        count => COUNTVALUE
        }
    }
BugGroup => {
    BugCode => {
        bytes => BYTESVALUE,
        count => COUNTVALUE
        }
    }
}
```

#### METRICSUMMARYDATA
```
{
MetricSummaries => [{
    Type => TYPEVALUE,
    Count => COUNTVALUE,
    Sum => SUMVALUE,
    SumOfSquares => SUMOFSQVALUE
    Maximum => MAXVALUE,
    Minimum => MINIMUMVALUE,
    Average => AVERAGEVALUE,
    StandardDeviation => STDDEVIATIONVALUE,
    },
    {
    Type => TYPEVALUE,
    Count => COUNTVALUE,
    Sum => SUMVALUE,
    SumOfSquares => SUMOFSQVALUE
    Maximum => MAXVALUE,
    Minimum => MINIMUMVALUE,
    Average => AVERAGEVALUE,
    StandardDeviation => STDDEVIATIONVALUE,
    }
}
```
