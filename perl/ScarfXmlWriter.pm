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
        open($self->{output}, ">", $handle) or die "open > $handle: $!\n";
	$self->{filetype} = 1;
	$self->{filename} = $handle;
    }

    if ($encoding) {
	$self->{enc} = $encoding;
    } else {
	$self->{enc} = "utf-8";
    }

    $self->{error_level} = 2;
    $self->{bugId} = 0;
    $self->{metricId} = 0;

    $self->{bodyType} = undef;
    $self->{open} = undef;
    $self->{MetricSummaries} = {};
    $self->{BugSummaries} = {};

    bless $self, $class;
    return $self;
}

sub GetWriterAttrs
{
    my ($self, $attrs) = @_;

    my $filename;
    if (exists $self->{filename})  {
	$filename = $self->{filename};
    }  else  {
	$filename = "<unknown>";
    }
    $attrs->{'scarf-file'} = $filename;
    $attrs->{'parsed-results-file'} = $filename;
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
    if ($self->{error_level} >= 1) {
	if ($self->{open}) {
            my $errorMsg = "SCARF file alread open";
            print "$errorMsg\n";
            if ( $self->{error_level} >= 2 ) {
                die $errorMsg;
            }
	}
	my $errors = CheckStart($initial_details);
        if (@$errors)  {
            my $errorMsg = join "\n", @{$errors};
            print "$errorMsg\n";
            if ($self->{error_level} >= 2) {
                die $errorMsg;
            }
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
    if ($self->{error_level} >= 1) {
        if (!$self->{open}) {
            my $errorMsg = "SCARF file already closed";
            print "$errorMsg\n";
            if ( $self->{error_level} >= 2 ) {
                die $errorMsg;
            }
        }
    }
    $self->{writer}->endTag();
    $self->{writer}->end();
    $self->{open} = 0;
    return $self;
}


#Check bug for required elements
sub CheckBug
{
    my ($bugInstance, $bugId) = @_;
    my @errors = ();
    if (not defined $bugId){
	$bugId = "";
    }
    for my $bugReqElt (qw/BugLocations BugMessage BuildId AssessmentReportFile/) {
	if (!(defined $bugInstance->{$bugReqElt})) {
	    push @errors, "Required element $bugReqElt could not be found in BugInstance $bugId";
	}
    }
    my $locID = 0;
    my $methodID = 0;
    if (defined $bugInstance->{Methods}) {
        my $methodprimary = 0;
	foreach my $method (@{$bugInstance->{Methods}}) {
	    if (!(defined $method->{primary})) {
		push @errors, "Required attribute primary not found for Method $methodID in BugInstance $bugId";
	    } elsif ($method->{primary} eq 'true') {
		if ($methodprimary) {
		    push @errors, "More than one primary Method found in BugInstance $bugId";
		} else {
		    $methodprimary = 1;
		}
	    }

	    if (!(defined $method->{name})) {
		push @errors, "Method name not found for Method$methodID in BugInstance $bugId";
	    }
	}
	if (!($methodprimary)) {
	    # push @errors, "No primary Method found in  BugInstance $bugId";
	}
	$methodID++;
    }

    my $locprimary = 0;
    foreach my $location (@{$bugInstance->{BugLocations}}) {
	if (!(defined $location->{primary})) {
	    push @errors, "Primary attribute not found for Location$locID in BugInstance $bugId";
	} elsif ($location->{primary}) {
	    if ($locprimary) {
#		push @errors, "More than one primary BugLocation found in  BugInstance $bugId";
	    } else {
		$locprimary = 1;
	    }
	}
	for my $locElt (qw/SourceFile/) {
	    if (!(defined $location->{$locElt})) {
		push @errors, "Element $locElt could not be found for Location$locID in BugInstance $bugId";
	    }
	}
	for my $optLocElt (qw/StartColumn EndColumn StartLine EndLine/) {
	    if (defined $location->{$optLocElt}  ) {
		if ($location->{$optLocElt} !~ /[0-9]+/) {
		    push @errors, "Not a positive integer: for $optLocElt child of BugLocation in BugInstance: $bugId";
		}
	    }
	}
	$locID++;
    }
    if (@{$bugInstance->{BugLocations}} > 0 && !($locprimary)) {
        push @errors, "No primary BugLocation found in BugInstance $bugId";
    }

    # some tools emit ids for non-cwe taxonomies, but they are put in the CweIds list with a prefix
    # XXX: for now disable this check
    #
    #if (defined $bugInstance->{CweIds}) {
    #	for my $cweid (@{$bugInstance->{CweIds}}) {
    #       if ($cweid !~ /[0-9]+/) {
    #		push @errors, "Not a positive integer for CweID in BugInstance: $bugId";
    #	    }
    #	}
    #}

    if (defined $bugInstance->{InstanceLocation}) {
        if (defined $bugInstance->{InstanceLocation}->{LineNum}) {
            my $line_num = $bugInstance->{InstanceLocation}->{LineNum};
            if (!(defined $line_num->{Start})) {
                push @errors, "Start child missing in LineNum in BugInstance $bugId";
            } elsif ($line_num->{Start} !~ /[0-9]+/ ) {
               push @errors, "Not a positive integer for Start child of LineNum in BugInstance $bugId";
            }
            if (!(defined $line_num->{End})) {
                push @errors, "End child missing in LineNum BugInstance: $bugId";
	    } elsif ($line_num->{End} !~ /[0-9]+/) {
                push @errors, "Not a positive integer for End child of LineNum in BugInstance $bugId";
	    }
	}
	elsif (!(defined $bugInstance->{InstanceLocation}->{Xpath})) {
	    push @errors, "Neither LineNum or Xpath children were present in InstanceLocation BugInstance $bugId";
	}
    }

    return \@errors;
}

#Add a single bug instance to file
sub AddResult
{
    my($self, $bugInstance) = @_;

    #Check for req elements existence
    if ($self->{error_level} >= 1) {
        if ($self->{bodyType} eq "summary") {
            my $errorMsg = "Adding bug after summary written";
            print "$errorMsg\n";
            if ( $self->{error_level} >= 2) {
                die $errorMsg;
            }
        }
        my $errors = CheckBug($bugInstance, $self->{bugId});
        if (@$errors)  {
            my $errorMsg = join "\n", @$errors;
            print "$errorMsg\n";
            if ($self->{error_level} >= 2) {
                die $errorMsg;
            }
        }
    }

    my $writer = $self->{writer};

    #byte count info
    my $byte_count = 0;
    my $initial_byte_count = 0;
    my $final_byte_count = tell($self->{output});
    $initial_byte_count = $final_byte_count;

    #Add bug
    $writer->startTag('BugInstance', id => "$self->{bugId}");

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

    $self->{bugId}++;
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
	   push @errors, "Required element $reqMetrElt not found for Metric $metricId";
	}
    }
    return \@errors;
}


#write a single metric
sub AddMetric
{
    my($self, $metric) = @_;

    ++$self->{metricId};

    if ($self->{error_level} >= 1) {
        if ($self->{bodyType} eq "summary") {
            my $errorMsg = "Adding metric after summary written";
            print "$errorMsg\n";
            if ( $self->{error_level} >= 2) {
                die $errorMsg;
            }
        }
        my $errors = CheckMetric($metric, $self->{metricId});
        if (@$errors)  { 
            my $errorMsg = join "\n", @$errors;
            print "$errorMsg\n";
            if ($self->{error_level} >= 2) {
                die $errorMsg;
            }
        }
    }

    my $writer = $self->{writer};
    $writer->startTag('Metric', id => $self->{metricId});

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

    my $value = $metric->{Value};
    my $type = $metric->{Type};
    if (!(defined $type) ) {
	die "AddMetric requires Type attribute";
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
    if ($self->{error_level} >= 1)  {
	if ($self->{bodyType} eq "summary") {
	    my $errorMsg = "Adding summary after summary written";
	    print "$errorMsg\n";
	    if ( $self->{error_level} == 2) {
		die $errorMsg;
	    }
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

# Return the number of bugs
sub GetNumBugs {
    my ($self) = @_;

    return $self->{bugId};
}

# Return the number of metrics
sub GetNumMetrics {
    my ($self) = @_;

    return $self->{metricId};
}

1;
