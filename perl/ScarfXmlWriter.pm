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

package ScarfXmlWriter;
use XML::Writer;
use Scalar::Util qw[openhandle];
use IO qw[Handle Seekable File Pipe];
use strict;
use Data::Dumper;

use Exporter qw(import);
my @EXPORT_OK = qw(CheckStart CheckBug CheckMetric);


my $byteCountHash;
my $count_hash;
my $metric_count_hash;
my $metric_sum_hash;
my $metric_sumofsquares_hash;
my $metric_max_hash;
my $metric_min_hash;

#constructer
sub new
{
    my ($class, $handle, $encoding) = @_;
#    my ($class, $output_file, $error_level, $pretty_enable) = @_;
    my $self = {};
    if (openhandle($handle) or ref $handle eq "IO" or ref $handle eq "SCALAR") {
	$self->{output} = $handle;
	$self->{filetype} = 0;
    } else {
        open($self->{output}, ">", $handle) or die "invalid output file\n";
	$self->{filetype} = 1;
    }

    if ($encoding) {
	$self->{enc} = $encoding;
    } else {
	$self->{enc} = "utf-8";
    }

    $self->{error_level} = 2;
    $self->{bugID} = 0;
    $self->{metricID} = 1;
    
    $self->{bodyType} = undef;
    $self->{open} = undef;
    $self->{MetricSummaries} = {};
    $self->{BugSummaries} = {};
    
    bless $self, $class;
    return $self;
}

sub SetOptions {
    my ($self, $options) = @_;

    if (defined $options->{pretty} && $options->{pretty}) {
        $self->{writer} = new XML::Writer(OUTPUT => $self->{output}, DATA_MODE => 'true', DATA_INDENT => 2, ENCODING => $self->{enc} );
    } else {
        $self->{writer} = new XML::Writer(OUTPUT => $self->{output}, ENCODING => $self->{enc} );
    }
    $self->{writer}->xmlDecl('UTF-8' );

    if (defined $options->{error_level}) {
        if ($options->{error_level} >= 0 && $options->{error_level} <= 2) {
            $self->{error_level} = $options->{error_level};
        }
    }

    if (defined $options->{sortKeys}) {
        $self->{sortKeys} = $options->{sortKeys} ? 1 : 0;
    }
}

sub Close
{
    my ($self) = @_;
    $self->{writer}->end();
    if ( $self->{filetype} == 1 ) {
	close $self->{output};
    }
    $self = undef;
    return $self;
}


#return writer
sub getWriter
{
    my ($self) = @_;
    return $self->{writer};
}


#return output file
sub getHandle
{
    my ($self) = @_;
    return $self->{output};
}


# error level accessor/mutator
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
	}
    }
}

#enable or disable pretty printing
sub getPretty
{
    my ($self) = @_;
    return $self->{writer}->getDataMode();
}

sub setPretty
{
    my ($self, $pretty_enable) = @_;
    if ( $pretty_enable ) {
	$self->{writer}->setDataMode('true');
	$self->{writer}->setDataIndent(2);
    } else {
	$self->{writer}->setDataMode(0);
	$self->{writer}->setDataIndent(0);
    }
}


#Check required start elements
sub CheckStart
{
    my ($initial) = @_;
    my @errors = ();

    for my $reqAttr (qw/tool_name tool_version uuid/) {
	if (!(defined $initial->{$reqAttr})) {
	    push @errors, "Required attribute: $reqAttr not found when creating startTag";	    
	}
    }

    return \@errors;

}


# invalid XML 1.0 characters,
# should also include U+D800 - U+DFFF and U+10000 - U+10FFFF
my $badXmlCharRe = qr/([\x00-\x08\x0b\x0c\x0e-\x1f])/;

sub ConvertBadXmlChar
{
    my ($c) = @_;

    my $fixedChar = sprintf("\\u%04X",ord($c));

    print STDERR "WARNING: bad XML char in output converting to '$fixedChar'\n";

    return $fixedChar;
}


sub WriteSimpleElement
{
    my ($writer, $data, @tagAndAttrs) = @_;

    for (my $i = 0; $i < @tagAndAttrs; ++$i)  {
	$tagAndAttrs[$i] =~  s/$badXmlCharRe/ConvertBadXmlChar($1)/eg;
    }

    if (defined $data)  {
	$data =~  s/$badXmlCharRe/ConvertBadXmlChar($1)/eg;

	$writer->startTag(@tagAndAttrs);
	$writer->characters($data);
	$writer->endTag();
    }  else  {
	$writer->emptyTag(@tagAndAttrs);
    }
}


# Dummy function to match SarifJsonWriter's API
sub BeginFile {

}

#start analyzer report
sub BeginRun
{   
    my ($self, $initial_details) = @_;
    if ($self->{error_level} != 0) {
	if ( $self->{open} ) {
	    print "Scarf file already open";
	    if ( $self->{error_level} == 2 ) {
		die "Exiting";
	    }
	}
	my $errors = CheckStart($initial_details);
	print "$_\n" for @{$errors};
	if (@{$errors} and $self->{error_level} == 2) {
	    die "Exiting";
	}
    }
    $self->{bodyType} = "body";

    my %copy = %{$initial_details};
    delete $copy{assessments};
    $self->{writer}->startTag('AnalyzerReport', %copy);
    $self->{open} = 1;
    $self->{MetricSummaries} = {};
    $self->{BugSummaries} = {};
    return $self;
}

# Dummy function to match SarifJsonWriter's API
sub AddOriginalUriBaseIds {

}

# Dummy function to match SarifJsonWriter's API
sub AddToolData {

}

# Dummy function to match SarifJsonWriter's API
sub AddInvocations {

}

# Dummy function to match SarifJsonWriter's API
sub BeginResults {

}

# Dummy function to match SarifJsonWriter's API
sub EndResults {

}

# Dummy function to match SarifJsonWriter's API
sub EndFile {

}

#close	analyzer report
sub EndRun
{
    my ($self) = @_;
    if ($self->{error_level} > 0) {
        if (!$self->{open}) {
            print "Scarf file already closed";
            if ( $self->{error_level} == 2 ) {
                die "Exiting";
            }
        }
    }
    $self->{writer}->endTag();
    return $self;
}


#Check bug for required elements
sub CheckBug
{
    my ($bugInstance, $bugID) = @_;
    my @errors = ();
    if (not defined $bugID){
	$bugID = "";
    }
    for my $bugReqElt (qw/BugLocations BugMessage BuildId AssessmentReportFile/) {
	if (!(defined $bugInstance->{$bugReqElt})) {
	    push @errors, "Required element: $bugReqElt could not be found in BugInstance $bugID";
	}
    }
    my $locID = 0;
    my $methodID = 0;
    if (defined $bugInstance->{Methods}) {
        my $methodprimary = 0;
	foreach my $method (@{$bugInstance->{Methods}}) {
	    if (!(defined $method->{primary})) {
		push @errors, "Required attribute: primary not found for Method $methodID in BugInstance $bugID";
	    } elsif ($method->{primary} eq 'true') {
		if ($methodprimary) {
		    push @errors, "Misformed Element: More than one primary Method found in BugInstance $bugID";
		} else {
		    $methodprimary = 1;
		}
	    }

	    if (!(defined $method->{name})) {
		push @errors, "Required text: name not found for Method$methodID in BugInstance $bugID";
	    }
	}
	if (!($methodprimary)) {
	    # push @errors, "Misformed Element: No primary Method found in  BugInstance $bugID";
	}
	$methodID++;
    }

    my $locprimary = 0;
    foreach my $location (@{$bugInstance->{BugLocations}}) {
	if (!(defined $location->{primary})) {
	    push @errors, "Required attribute: primary could not be found for Location$locID in BugInstance $bugID";	    
	} elsif ($location->{primary}) {
	    if ($locprimary) {
#		push @errors, "Misformed Element: More than one primary BugLocation found in  BugInstance $bugID";
	    } else {
		$locprimary = 1;
	    }
	}
	for my $locElt (qw/SourceFile/) {
	    if (!(defined $location->{$locElt})) {
		push @errors, "Required element: $locElt could not be found for Location$locID in BugInstance $bugID";	    
	    }
	}
	for my $optLocElt (qw/StartColumn EndColumn StartLine EndLine/) {
	    if (defined $location->{$optLocElt}  ) {
		if ($location->{$optLocElt} !~ /[0-9]+/) {
		    push @errors, "Wrong value type: $optLocElt child of BugLocation in BugInstance$bugID requires a positive integer";
		}
	    }
	}
	$locID++;
    }
    if (!($locprimary)) {
        push @errors, "Misformed Element: No primary BugLocation found in BugInstance $bugID";
    }

    if (defined $bugInstance->{CweIds}) {
	for my $cweid (@{$bugInstance->{CweIds}}) {
            if ($cweid !~ /[0-9]+/) {
		push @errors, "Wrong value type: CweID expected to be a positive integer in BugInstance $bugID";
	    }
	}
    }

    if (defined $bugInstance->{InstanceLocation}) {
        if (defined $bugInstance->{InstanceLocation}->{LineNum}) {
            my $line_num = $bugInstance->{InstanceLocation}->{LineNum};
            if (!(defined $line_num->{Start})) {
                push @errors, "Required element missing: Could not find Start child of a LineNum in BugInstance $bugID";
            } elsif ($line_num->{Start} !~ /[0-9]+/ ) {
               push @errors, "Wrong value type: Start child of LineNum requires a positive integer BugInstance $bugID";
            }
            if (!(defined $line_num->{End})) {
                push @errors, "Required element missing: Could not find End child of a LineNum BugInstance $bugID";
	    } elsif ($line_num->{End} !~ /[0-9]+/) {
                push @errors, "Wrong value type: End child of LineNum requires a positive integer BugInstance $bugID";
	    }
	}
	elsif (!(defined $bugInstance->{InstanceLocation}->{Xpath})) {
	    push @errors, "Misformed Element: Neither LineNum or Xpath children were present in InstanceLocation BugInstance $bugID";
	}
    }

    return \@errors;
}

#Add a single bug instance to file
sub AddResult
{
    my($self, $bugInstance) = @_;

    #Check for req elements existence
    if ($self->{error_level} != 0) {
        if ($self->{bodyType} eq "summary") {
            print "Summary already written. Invalid Scarf";
            if ( $self->{error_level} == 2) {
                die "Exiting";
            }
        }
	my $errors = CheckBug($bugInstance, $self->{bugID});
	print "$_\n" for @$errors;
	if (@$errors and $self->{error_level} == 2) {
	    die "Exiting";
	}
    }

    my $writer = $self->{writer};   

    #byte count info	
    my $byte_count = 0;
    my $initial_byte_count = 0;
    my $final_byte_count = tell($self->{output});
    $initial_byte_count = $final_byte_count;
    
    #Add bug
    $writer->startTag('BugInstance', id => "$self->{bugID}");
    
    if (defined $bugInstance->{ClassName}) {
	WriteSimpleElement($writer, $bugInstance->{ClassName}, 'ClassName');
    }

	$writer->startTag('Methods');
    if (defined $bugInstance->{Methods}) {
	my $methodID=0;
	
	foreach my $method (@{$bugInstance->{Methods}}) {
	    WriteSimpleElement($writer, $method->{name}, 'Method', id => $methodID, primary => $method->{primary});
	    
	    $methodID++;
	}
    }
	$writer->endTag();
    
    $writer->startTag('BugLocations');	
    my $locID = 0;

    foreach my $location (@{$bugInstance->{BugLocations}}) {
		$writer->startTag('Location', id => $locID, primary => $location->{primary});
	
	for my $locElt (qw/SourceFile/) {
	    WriteSimpleElement($writer, $location->{$locElt}, $locElt);
	}

	for my $optLocElt (qw/StartLine EndLine StartColumn EndColumn Explanation/) {
	    if (defined $location->{$optLocElt}  ) {
		WriteSimpleElement($writer, $location->{$optLocElt}, $optLocElt);
		}
	}

	$writer->endTag();
	$locID++;
    }

    $writer->endTag();
    
    if (defined $bugInstance->{CweIds}) {
	foreach my $cweId (@{$bugInstance->{CweIds}}) {
	    WriteSimpleElement($writer, $cweId, 'CweId');
	}
    }

    for my $bugElts (qw/BugGroup BugCode BugRank BugSeverity/) {
	if (defined $bugInstance->{$bugElts}){
	    WriteSimpleElement($writer, $bugInstance->{$bugElts}, $bugElts);
	}
    }

    WriteSimpleElement($writer, $bugInstance->{BugMessage}, 'BugMessage');

    if (defined $bugInstance->{ResolutionSuggestion}) {
	WriteSimpleElement($writer, $bugInstance->{ResolutionSuggestion}, 'ResolutionSuggestion');
    }

    $writer->startTag('BugTrace');

    WriteSimpleElement($writer, $bugInstance->{BuildId}, 'BuildId');

    WriteSimpleElement($writer, $bugInstance->{AssessmentReportFile}, 'AssessmentReportFile');

    if (defined $bugInstance->{InstanceLocation}) {
	$writer->startTag('InstanceLocation');

	    if (defined $bugInstance->{InstanceLocation}->{Xpath}) {
		WriteSimpleElement($writer, $bugInstance->{InstanceLocation}->{Xpath}, 'Xpath');
	    } 
	    if (defined $bugInstance->{InstanceLocation}->{LineNum}) {
		$writer->startTag('LineNum');
		WriteSimpleElement($writer, $bugInstance->{InstanceLocation}->{LineNum}->{Start}, 'Start');
		WriteSimpleElement($writer, $bugInstance->{InstanceLocation}->{LineNum}->{End}, 'End');
		$writer->endTag();
	    }
	$writer->endTag();
    }	
    $writer->endTag();

    $self->{bugID}++;		
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

    my $type_hash = $self->{BugSummaries}->{$group}->{$code};
    if (defined $type_hash) {
	$type_hash->{count}++;
	$type_hash->{bytes} += $byte_count;
    } else {
	my $bugSummary = {};
	$bugSummary->{count} = 1;
	$bugSummary->{bytes} = $byte_count;
	$self->{BugSummaries}->{$group}->{$code} = $bugSummary;
    }
    
}


#Check for metrics required elements
sub CheckMetric
{
    my ($metric, $metricId) = @_;
    my @errors = ();
    for my $reqMetrElt (qw/SourceFile Type Value/) {
	if (!(defined $metric->{$reqMetrElt})) {
	   push @errors, "Required element: $reqMetrElt could not be found for Metric $metricId";	    
	}
    }
    return \@errors;
}


#write a single metric
sub AddMetric
{
    my($self, $metric) = @_;
 
    if ($self->{error_level} != 0) {
        if ($self->{bodyType} eq "summary") {
            print "Summary already written. Invalid Scarf";
            if ( $self->{error_level} == 2) {
                die "Exiting";
            }
        }
	my $errors = CheckMetric($metric, $self->{metricID});
	print "$_\n" for @$errors;
	if (@$errors and $self->{error_level} == 2) {
	    die "Exiting";
	}
    }

    my $writer = $self->{writer};
    $writer->startTag('Metric', id => $self->{metricID});   

    $writer->startTag('Location');
    WriteSimpleElement($writer, $metric->{SourceFile}, 'SourceFile');
    $writer->endTag();
 
    for my $optMetr (qw/Class Method/) {
	if (defined $metric->{$optMetr}) {
	    WriteSimpleElement($writer, $metric->{$optMetr}, $optMetr);
	}
    }
    
    for my $reqMetr (qw/Type Value/) {
        WriteSimpleElement($writer, $metric->{$reqMetr}, $reqMetr);
    }	

    $self->{metricID}++;
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
sub AddSummary
{
    my ($self) = @_ ;
    my $writer = $self->{writer};
    if ($self->{bodyType} eq "summary") {
        print "Summary already written Invalid Scarf";
        if ( $self->{error_level} == 2) {
            die "Exiting";
        }
    }
    if (%{$self->{BugSummaries}}) {
	$writer->startTag('BugSummary');
	if ($self->{sortKeys}) {
	    foreach my $group (sort keys %{$self->{BugSummaries}}) {
		foreach my $code (sort keys %{$self->{BugSummaries}->{$group}}) {
		    my $hashes = $self->{BugSummaries}->{$group}->{$code};
		    WriteSimpleElement($writer, undef, 'BugCategory', group => $group, code => $code, count => $hashes->{count}, bytes => $hashes->{bytes});
		}
	    }
	} else {
	    foreach my $group (keys %{$self->{BugSummaries}}) {
		foreach my $code (keys %{$self->{BugSummaries}->{$group}}) {
		    my $hashes = $self->{BugSummaries}->{$group}->{$code};
		    WriteSimpleElement($writer, undef, 'BugCategory', group => $group, code => $code, count => $hashes->{count}, bytes => $hashes->{bytes});
		}
	    }
	}
	$writer->endTag();
	$self->{bodyType} = "summary";
    }
    
    if (%{$self->{MetricSummaries}}) {
	$writer->startTag('MetricSummaries');
	foreach my $summaryName (keys(%{$self->{MetricSummaries}})) {
	    my $summary = $self->{MetricSummaries}->{$summaryName};
	    $writer->startTag('MetricSummary');

	    WriteSimpleElement($writer, $summaryName, 'Type');
	    
	    my $count = $summary->{Count};
	    WriteSimpleElement($writer, $count, 'Count');

	    if (defined $summary->{Sum}) {

		for my $sumElt (qw/Sum SumOfSquares Minimum Maximum/) {
		    WriteSimpleElement($writer, $summary->{$sumElt}, $sumElt);
		}
    
		my $average = 0;
		if ($count != 0) {
		    $average = $summary->{Sum}/$count;
		}

		WriteSimpleElement($writer, sprintf("%.2f", $average), 'Average');

    
		my $denominator = $count * ($count - 1);
		my $square_of_sum = $summary->{Sum} * $summary->{Sum};
		my $standard_dev = 0;
		if ($denominator != 0) {
		    $standard_dev = sqrt(($summary->{SumOfSquares} * $count - $square_of_sum ) / $denominator);
		}

		WriteSimpleElement($writer, sprintf("%.2f", $standard_dev), 'StandardDeviation');
	    }	    
	    $writer->endTag();
	}
	$writer->endTag();
	$self->{bodyType} = "summary";
    }
}

# Return the number of bugs in total
sub GetNumBugs {
    my ($self) = @_;

    return $self->{bugID};
}

1;
