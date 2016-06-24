#!/usr/bin/perl -w

#  Copyright 2016 Brandon G. Klein
# 
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

package HashToScarf;
use XML::Writer;
use strict;
use Data::Dumper;

my $byteCountHash;
my $count_hash;
my $metric_count_hash;
my $metric_sum_hash;
my $metric_sumofsquares_hash;
my $metric_max_hash;
my $metric_min_hash;
my $metricID;
my $bugID;

#constructer
sub new
{
    my ($class, $output_file, $error_level) = @_;
    my $self = {};
    open $self->{output}, ">", $output_file or die "invalid output file";
    $self->{writer} = new XML::Writer(OUTPUT => $self->{output}, DATA_MODE => 'true', DATA_INDENT => 2, ENCODING => 'utf-8' );
    $self->{writer}->xmlDecl('UTF-8' );
    
    if (defined $error_level && $error_level == 0 || $error_level == 1) {
	$self->{error_level} = $error_level;
    } else {
	$self->{error_level} = 2;
    }

    $bugID = 1;
    $metricID = 1;
    
    $self->{MetricSummaries};
    $self->{BugSummaries};
    
    bless $self, $class;
    return $self;
}


#return writer
sub getWriter
{
    my ($self) = @_;
    return $self->{_writer};
}


#return output file
sub getFile
{
    my ($self) = @_;
    return $self->{output};
}


sub getErrorLevel
{
    my ($self) = @_;
    return $self->{error_level};
}


sub setErrorLevel
{
    my ($self, $error_level) = @_;
    if (defined $error_level) {
	if ($error_level == 1 or $error_level == 2 or $error_level == 0) {
	    $self->{error_level} = $error_level;
	} else {
	    print "Invalid error level";
	}
    }
}


sub error
{
    my ($error_level, $error_message) = @_;
    if ($error_level == 0) {
	return;
    } elsif ($error_level == 1) {
	print "$error_message\n";
    } else {
	die $error_message;
    }
}

#start analyzer report
sub addStartTag
{   
    my ($self, $initial_details) = @_;
    for my $reqAttr (qw/tool_name tool_version uuid/) {
	if (!(defined $initial_details->{$reqAttr})) {
	    error($self->{error_level}, "Required attribute: $reqAttr not found when creating startTag");	    
	}
    }

    $self->{writer}->startTag('AnalyzerReport', tool_name => $initial_details->{tool_name}, 
	    tool_version => $initial_details->{tool_version}, uuid => $initial_details->{uuid});
    return $self;
}


#close	analyzer report
sub addEndTag
{
    my ($self) = @_;
    $self->{writer}->endTag();
    return $self;
}

#add a single bug instance to file
sub addBugInstance
{
    my($self, $bugInstance) = @_;

    #check for req elements existence
    for my $bugReqElt (qw/BugLocations BugMessage BuildId AssessmentReportFile/) {
	if (!(defined $bugInstance->{$bugReqElt})) {
	    error($self->{error_level}, "Required element: $bugReqElt could not be found in BugInstance:$bugID");     
	}
    }
    my $locID = 1;
    my $methodID = 1;
    foreach my $method (@{$bugInstance->{Methods}}) {
	if (!(defined $method->{primary})) {
	    error($self->{error_level}, "Required attribute: primary not found for Method:$methodID in BugInstance:$bugID");
	}
	if (!(defined $method->{name})) {
	    error($self->{error_level}, "Required text: name not found for Method:$methodID in BugInstance:$bugID");
	}
	$methodID++;
    }
    foreach my $location (@{$bugInstance->{BugLocations}}) {
	if (!(defined $location->{primary})) {
	    error($self->{error_level}, "Required attribute: primary could not be found for Location:$locID in BugInstance:$bugID");	    
	}
	for my $locElt (qw/SourceFile/) {
	    if (!(defined $location->{$locElt})) {
		error($self->{error_level}, "Required element: $locElt could not be found for Location:$locID in BugInstance:$bugID");	    
	    }
	}
	$locID++;
    }
#    if ($locID == 1) {
#	error($self->{error_level}, "No BugLocations found for BugInstance:$bugID");	    
#    }

    my $writer = $self->{writer};   

    #byte count info	
    my $byte_count = 0;
    my $initial_byte_count = 0;
    my $final_byte_count = tell($self->{output});
    $initial_byte_count = $final_byte_count;
    
    #add bug
    #print $bugID;
    $writer->startTag('BugInstance', id => "$bugID");
    
    if (defined $bugInstance->{ClassName}) {
	$writer->startTag('ClassName');
	$writer->characters($bugInstance->{ClassName});
	$writer->endTag();
    }

    if (defined $bugInstance->{Methods}) {
	$methodID=1;
	$writer->startTag('Methods');
	foreach my $method (@{$bugInstance->{Methods}}) {
	    my $primary;
	    if ($method->{primary}) {
		$primary = "true";
	    } else {
		$primary ="false";
	    }

	    $writer->startTag('Method', id => $methodID, primary => $primary);
	    
	    $writer->characters($method->{name});
	    $writer->endTag();
	    $methodID++;
	}
	$writer->endTag();
    }
    
    $writer->startTag('BugLocations');	
    $locID = 1;

    foreach my $location (@{$bugInstance->{BugLocations}}) {	
	my $primary;
	if ($location->{primary}) {
	    $primary = "true";
	} else {
	    $primary = "false";
	}
	$writer->startTag('Location', id => $locID, primary => $primary);#$location->{primary});
	
	for my $locElt (qw/SourceFile/) {
	    $writer->startTag($locElt);
	    $writer->characters($location->{$locElt});
	    $writer->endTag();
	}

	for my $optLocElt (qw/StartColumn Explanation EndColumn StartLine EndLine/) {
	    if (defined $location->{$optLocElt}  ) {
		$writer->startTag($optLocElt);
		$writer->characters($location->{$optLocElt});
		$writer->endTag();  
	    }
	}

	$writer->endTag();
	$locID++;
    }

    $writer->endTag();
    
    if (defined $bugInstance->{CweIds}) {
	foreach my $cweid (@{$bugInstance->{CweIds}}) {
	    $writer->startTag('CweId');
	    $writer->characters($cweid);
	    $writer->endTag();
	}
    }

    for my $bugElts (qw/BugGroup BugCode BugRank BugSeverity/) {
	if (defined $bugInstance->{$bugElts}){
	    $writer->startTag($bugElts);
	    $writer->characters($bugInstance->{$bugElts});
	    $writer->endTag();
	}
    }

    $writer->startTag('BugMessage');
    $writer->characters($bugInstance->{BugMessage});
    $writer->endTag();

    if (defined $bugInstance->{ResolutionSuggestion}) {
	$writer->startTag('ResolutionSuggestion');
	$writer->characters($bugInstance->{ResolutionSuggestion});
	$writer->endTag();
    }

    $writer->startTag('BugTrace');

    $writer->startTag('BuildId');
    $writer->characters($bugInstance->{BuildId});
    $writer->endTag();

    $writer->startTag('AssessmentReportFile');
    $writer->characters($bugInstance->{AssessmentReportFile});
    $writer->endTag();

    if (defined $bugInstance->{InstanceLocation}) {
	$writer->startTag('InstanceLocation');

	    if (defined $bugInstance->{InstanceLocation}->{Xpath}) {
		$writer->startTag('Xpath');
		$writer->characters($bugInstance->{InstanceLocation}->{Xpath});
		$writer->endTag();
	    } 
	    if (defined $bugInstance->{InstanceLocation}->{LineNum}) {
		if (!(defined $bugInstance->{InstanceLocation}->{LineNum}->{Start})) {
		    error($self->{error_level}, "Required element: Start could not be found for LineNum in InstanceLocation for BugInstance:$bugID");	    
		}
		if (!(defined $bugInstance->{InstanceLocation}->{LineNum}->{End})) {
		    error($self->{error_level}, "Required element: End could not be found for LineNum in InstanceLocation for BugInstance:$bugID");	
		}
		$writer->startTag('LineNum');
		$writer->startTag('Start');
		$writer->characters($bugInstance->{InstanceLocation}->{LineNum}->{Start});
		$writer->endTag();		  
		$writer->startTag('End');
		$writer->characters($bugInstance->{InstanceLocation}->{LineNum}->{End});
		$writer->endTag();						   
		$writer->endTag();
	    }
	$writer->endTag();
    }	
    $writer->endTag();

    $bugID++;		
    $writer->endTag();

    #byte count info
    $final_byte_count = tell $self->{output};
    $byte_count = $final_byte_count - $initial_byte_count;

    #group bugs
    my $code = $bugInstance->{BugCode};
    my $group = $bugInstance->{BugGroup};
    if (!(defined $code) || $code eq "") {
	$code = "undefined";
    }
    if (!(defined $group) || $group eq "") {
	$group = "undefined";
    }

    my $type_hash = $self->{BugSummaries}->{$code}->{$group};
    if (defined $type_hash) {
	$type_hash->{count}++;
	$type_hash->{bytes} += $byte_count;
    } else {
	my $bugSummary = {};
	$bugSummary->{count} = 1;
	$bugSummary->{bytes} = $byte_count;
	$self->{BugSummaries}->{$code}->{$group} = $bugSummary;
    }
    
}

sub addMetric
{
    my($self, $metric) = @_;
 
    for my $reqMetrElt (qw/SourceFile Type Value/) {
	if (!(defined $metric->{$reqMetrElt})) {
	   error($self->{error_level}, "Required element: $reqMetrElt could not be found for Metric:$metricID");	    
	}
    }

    my $writer = $self->{writer};
    $writer->startTag('Metric', id => $metricID);   

    $writer->startTag('Location');
    $writer->startTag('SourceFile');
    $writer->characters($metric->{SourceFile});
    $writer->endTag();
    $writer->endTag();
 
    for my $optMetr (qw/Class Method/) {
	if (defined $metric->{$optMetr}) {
	    $writer->startTag($optMetr);
	    $writer->characters($metric->{$optMetr});
	    $writer->endTag();
	}
    }
    
    for my $reqMetr (qw/Type Value/) {
	$writer->startTag($reqMetr);
        $writer->characters($metric->{$reqMetr});
	$writer->endTag();
    }	

    $metricID++;
    my $value = $metric->{Value};
    my $type = $metric->{Type};
    if (!(defined $type) ) {
	die "no metric type listed";
    } else {
	my $type_hash = $self->{MetricSummaries}->{$type};
	if ( defined $type_hash ) {
	    $type_hash->{Count}++;
	    if (defined $type_hash->{Sum} && $type ne "language") {
		if ($metric->{Value} =~ /^([+-]?[0-9]*\.?[0-9]+([eE][0-9]+)?)$/) {
		    $type_hash->{SumOfSquares} += $value * $value;
		    $type_hash->{Sum} += $value;
		    if ($type_hash->{Maximum} < $value) {
			$type_hash->{Maximum} = $value;
		    }
		    if ($type_hash->{Minimum} > $value) {
			$type_hash->{Minimum} = $value;
		    }
		} else {
		    for my $summaries (qw/SumOfSquares Sum Maximum Minimum/) {
			undef $type_hash->{$summaries};
		    }
		}
	    }

	} else {
	    my $type_hash_data->{Count} = 1;
	    if ($type ne "language" && $metric->{Value} =~ /^([+-]?[0-9]*\.?[0-9]+([eE][0-9]+)?)$/) {
		$type_hash_data->{SumOfSquares} = $value*$value;
		$type_hash_data->{Sum} = $value;
		$type_hash_data->{Maximum} = $value;
		$type_hash_data->{Minimum} = $value;
	    }
	    $self->{MetricSummaries}->{$type} = $type_hash_data;
	}
    }
    $writer->endTag();
}

#Add Summary to file
sub addSummary
{
    my ($self) = @_ ;
    my $writer = $self->{writer};
    if (defined $self->{BugSummaries}) {
	$writer->startTag('BugSummary');
	foreach my $code (keys %{$self->{BugSummaries}}) {
	    foreach my $group (keys %{$self->{BugSummaries}->{$code}}) {
		my $hashes = $self->{BugSummaries}->{$code}->{$group};
		$writer->emptyTag('BugCategory', group => $group, code => $code, count => $hashes->{count}, bytes => $hashes->{bytes});	  
	    }
	}
	$writer->endTag();
    }	
    
    if ($self->{MetricSummaries}) {
	$writer->startTag('MetricSummaries');
	foreach my $summaryName (keys(%{$self->{MetricSummaries}})) {
	    my $summary = $self->{MetricSummaries}->{$summaryName};
	    $writer->startTag('MetricSummary');

	    $writer->startTag('Type');
	    $writer->characters($summaryName);
	    $writer->endTag();
	    
	    my $count = $summary->{Count};
	    $writer->startTag('Count');
	    $writer->characters($count);
	    $writer->endTag();

	    if (defined $summary->{Sum}) {

		for my $sumElt (qw/Sum SumOfSquares Minimum Maximum/) {
		    $writer->startTag($sumElt);
		    $writer->characters($summary->{$sumElt});
		    $writer->endTag();
		}
    
		my $average = 0;
		if ($count != 0) {
		    $average = $summary->{Sum}/$count;
		}

		$writer->startTag('Average');
		$writer->characters(sprintf("%.2f", $average));
		$writer->endTag();

    
		my $denominator = $count * ($count - 1);
		my $square_of_sum = $summary->{Sum} * $summary->{Sum};
		my $standard_dev = 0;
		if ($denominator != 0) {
		    $standard_dev = sqrt(($summary->{SumOfSquares} * $count - $square_of_sum ) / $denominator);
		}

		$writer->startTag('StandardDeviation');
		$writer->characters(sprintf("%.2f", $standard_dev));
		$writer->endTag();
	    }	    
	    $writer->endTag();
	}
	$writer->endTag();
    }
}

1;
