
### NAME
HashToScarf - A perl module for writing Scarf
### SYNOPSIS
```perl
use ScarfToHash;

my $output = "/path/to/file";

my $writer = new HashToScarf($output);
my $defaultPretty = $writer->GetPretty #$defaultPretty == 1
my $defaultErrorLevel = $writer->GetErrorLevel #$defaultErrorLevel == 2
$writer->setPretty(0);
$writer->setErrorLevel(1);

$writer->AddStartTag($initial_details);
$writer->AddBugInstance($bug);
$writer->AddMetric($metric);
$writer->AddSummary;
$writer->AddEndTag;

```
### DESCRIPTION
This module provides the ability to convert Perl data structures into Scarf formatted documents. It is dependant on XML::Writer library for writing.

The writer is controlled primarily by the user through method calls which feed data in to be written. This data is to be structured as a hash containing all fields for a specific section. For details on how the data structures are formatted see below.

The user has the ability to set the error level to 0 (none), 1 (warnings), or 2 (exit). Both error levels 1 and 2 will print all error messages during writing including misformatted elements, required elements not found, wrong value type, and ordering violations. Error level 2 will also exit the program as soon as an error is found to ensure validity of results.

### METHODS
#### new(FILE [, ERRORLEVEL, PRETTYENABLE])
This is a class method used to instantiate the writer. FILE is an object blessed into an IO::Handle, an open file handle, or a string filepath or a reference to a string to be written to. ERRORLEVEL is 0, 1, or 2 and sets the error level of the writer (default 2). PRETTYENABLE will enable or disable pretty printing of output with a true or false value respectively (default true).

#### GetWriter
Returns the XML::Writer used for writing.

#### GetHandle
Returns the set handle.

#### GetErrorLevel
Returns the current error level of the writer.

#### SetErrorLevel(ERRORLEVEL)
Allows changing the error level of the writer to 0, 1, or 2. If ERRORLEVEL does not equal one of these values does nothing.

#### SetPretty 
Returns the current value set to pretty print.

#### SetPretty(PRETTYENABLE)
If PRETTYENABLE is a true value then enable pretty printing, else disable pretty printing.

#### AddStartTag(INITIALDATA)
Writes a start tag to the file based on INITIALDATA. For details on valid [data structures](#data-structures) see below.

#### AddBugInstance(BUGDATA) 
Writes a bug to the file based on BUGDATA. For details on valid data structures see below.

#### AddMetric(METRICDATA)
Writes a metric to the file based on METRICDATA. For details on valid data structures see below.

#### AddSummary
Writes a summary to the file based on all bugs and metrics already written with this writer.

#### AddEndTag
Writes an end tag to the file.


### DATA STRUCTURES


The following are the data structures used in the methods listed above. Undefined keys will not be written. 

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

