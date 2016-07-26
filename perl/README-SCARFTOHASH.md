### NAME
ScarfToHash - A perl module for parsing Scarf
### SYNOPSIS
```perl
use ScarfToHash;

my $input = "/path/to/file";

my $callbackHash ={};
$callbackHash->{'InitialCallback'} = \&init;
$callbackHash->{'MetricCallback'} = \&metric;
$callbackHash->{'BugCallback'} = \&bug;
$callbackHash->{'BugSummaryCallback'} = \&summary;
$callbackHash->{'MetricSummaryCallback'} = \&summary;
$callbackHash->{'FinishCallback'} = \&finished;

my $test_reader = new ScarfToHash($input, $callbackHash);
$test_reader->parse;
```
### DESCRIPTION
This module provides the ability to convert Scarf files into Perl data structures. It is dependant on XML::Parser, which is used to handle parsing of the XML document.

The parser is controlled primarily by the callbacks the user sets prior to calling the parse method. Each callback receives as parameters a reference to a hash containing information on one hash of parsed data and the data stored in the CallbackData key of the callback hash. Upon completion of the parse the FinishCallback  will be called with just the CallbackData key.

The parser assumes that a valid Scarf file is inputted, but does do minor checks to ensure the file is Scarf. 
### METHODS
*new(FILENAME, CALLBACKS)* - This is a class method used to instatiate the parser. Filename is the path to the file this parser should interpret. Callbacks is a hash containing specified callback functions for parsed data to be sent to. For additional information on Callbacks see below.

*parse()* - This method initiates the parsing of the set file. If parsing fails a die call will be thrown, otherwise will return 0 on completion.


### CALLBACKS
The main purpose of this module is to interpret the events generated from XML::Parser and assemble them into a usable Perl data structures. When parsing the module will call the pre-defined callbacks upon completion of parsing an object of that type. All callbacks receive the data contained in the optional key "CallbackData" as a parameter. For details on the structure of each individual Perl data structure see below. 

*InitialCallback(INITIALDATA[, CALLBACKDATA])* - This is called just after the AnalyzerReport tag is parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

*MetricCallback(METRICDATA[, CALLBACKDATA])* - This is called every time a single Metric completes parsing. Any non-zero return value will terminate parsing and skip to FinishCallback.

*BugCallback(BUGDATA[, CALLBACKDATA])* - This is called every time a single BugInstance completes parsing. Any non-zero return value will terminate parsing and skip to FinishCallback.

*BugSummaryCallback(BUGSUMMARYDATA[, CALLBACKDATA])* - This is called after all BugSummaries have been parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

*MetricSummaryCallback(METRICSUMMARYDATA[, CALLBACKDATA])* - This is called once all MetricSummaries have been parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

*FinishCallback([CALLBACKDATA])* -  This is called once parsing has been completed.


### DATA STRUCTURES

The following are the data structures used in the callbacks listed above. If a keys value is not defined in the Scarf file, then the corresponding key will not exist in the data structures.

#### INITIALDATA
```
{
    'uuid' => UUIDVALUE,
    'tool_name' => TOOLNAMEVALUE,
    'tool_verison' => TOOLVERSIONVALUE 
} 
```

#### BUGDATA
```
{
    'AssessmentReportFile' => ASSESSREPORTVALUE, 
    'BuildId' => BUILDIDVALUE,                        
    'BugCode' => CODEVALUE,                     
    'BugId' => BUGIDVALUE,                          
    'BugRank' => BUGRANKVALUE,                       
    'CweIds' => [                            
        CWEIDVALUE, CWEIDVALUE, CWEIDVALUE                         
        ]                                    
    'Methods' => [                           
        {                                    
            'MethodId' => METHODIDVALUE,               
            'name' => METHODNAMEVALUE,             
            'primary' => PRIMARYVALUE                   
        },                                   
        {                                    
            'MethodId' => METHODIDVALUE,               
            'name' => METHODNAMEVALUE,             
            'primary' => PRIMARYVALUE                   
        }                                    
    ],                                       
    'ClassName' => CLASSVALUE,                  
    'BugSeverity' => SEVERITYVALUE,                 
    'BugGroup' => GROUPVALUE,                   
    'BugLocations' => [                      
        {                                    
            'EndLine' => ENDLINEVALUE,                
            'LocationId' => LOCIDVALUE,             
            'StartLine' => STARTLINEVALUE,              
            'primary' => PRIMARYVALUE,                  
            'SourceFile' => SOURCEVALUE         
        },                                   
        {                                    
            'primary' => PRIMARYVALUE,                  
            'EndColumn' => ENDCOLUMNVALUE,              
            'EndLine' => ENDLINEVALUE,              
            'Explanation' => EXPLANATIONVALUE,    
            'StartLine' => STARTLINEVALUE,            
            'LocationId' => LOCIDVALUE,             
            'SourceFile' => SOURCEVALUE,        
            'StartColumn' => STARTCOLUMNVALUE             
        }                                    
    ],                                       
    'InstanceLocation' => {                  
        'Xpath' => XPATHVALUE
	'LineNum' => { 
            'Start' = STARTVALUE
            'End' = ENDVALUE
        }            
    }                                        
    'BugMessage' => BUGMESSAGEVALUE
    'ResolutionSuggestion' => RESOLUTIONSUGGESTIONVALUE
}
```

#### METRICDATA
```
{
    'Value' => VALUE,          
    'Type' => TYPEVALUE,       
    'Method' => METHODVALUE,   
    'Class' => CLASSVALUE,     
    'SourceFile' => SOURCEVALUE,
    'MetricId' => METRICIDVALUE 
}
```

#### BUGSUMMARYDATA
```
{
CODEVALUE => {
    GROUPVALUE => {
        'bytes' => BYTESVALUE,
        'count' => COUNTVALUE
        }
    GROUPVALUE => {
        'bytes' => BYTESVALUE,
        'count' => COUNTVALUE
        }
    }
CODEVALUE => {
    GROUPVALUE => {
        'bytes' => BYTESVALUE,
        'count' => COUNTVALUE
        }
    }
}
```

#### METRICSUMMARYDATA
```
{
'MetricSummaries' => [{
    'Type' => TYPEVALUE,
    'Maximum' => MAXVALUE,
    'Sum' => SUMVALUE,
    'Average' => AVERAGEVALUE,
    'StandardDeviation' => STDDEVIATIONVALUE,
    'Count' => COUNTVALUE,
    'Minimum' => MINIMUMVALUE,
    'SumOfSquares' => SUMOFSQVALUE
    },
    {
    'Type' => TYPEVALUE,
    'Maximum' => MAXVALUE,
    'Sum' => SUMVALUE,
    'Average' => AVERAGEVALUE,
    'StandardDeviation' => STDDEVIATIONVALUE,
    'Count' => COUNTVALUE,
    'Minimum' => MINVALUE,
    'SumOfSquares' => SUMOFSQVALUE
    }
}
```
