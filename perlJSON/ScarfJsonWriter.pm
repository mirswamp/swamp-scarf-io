#!/usr/bin/perl -w

package ScarfJSONWriter;
use JSON::MaybeXS;
use Scalar::Util qw[openhandle];
use IO qw[Handle Seekable File Pipe];
use strict;

sub new
{
    my ($class, $handle, $error_level,) = @_;
    my $self = {};
    if ( openhandle($handle) or ref $handle eq "IO" or ref $handle eq "SCALAR") {
	$self->{output} = $handle;
	$self->{filetype} = 0;
    } else {
	open($self->{output}, ">", $handle) or die "open > $handle: $!";
	$self->{filetype} = 1;
	$self->{filename} = $handle;
    }
    $self->{writer} = JSON::MaybeXS->new(utf8 => 1, pretty => 1);

    if (defined $error_level && $error_level == 0 || $error_level == 1) {
        $self->{error_level} = $error_level;
    } else {
	$self->{error_level} = 2;
    }

    $self->{bugId} = 0;
    $self->{metricId} = 0;
    $self->{utf8} = 1;

    $self->{open};
    $self->{bodyType} = undef;
    $self->{openBody} = 0;
    $self->{openStart} = 0;
    $self->{MetricSummaries} = undef;
    $self->{BugSummaries} = undef;

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
    $attrs->{'scarf-json-file'} = $filename;
}

sub close
{
    my ($self) = @_;
    if ( $self->{filetype} == 1 ){
	close $self->{output};
    }
    $self = undef;
    return $self;
}

#encoding options
sub getEncoding
{
    my ($self) = @_;
    return $self->{utf8};
}

sub setEncoding
{
    my ($self, $encode) = @_;
    $self->{utf8} = $encode;
    $self->{writer}->utf8($encode);
}



#pretty printing options
sub getPretty
{
    my ($self) = @_;
    return $self->{writer}->get_space_before;
}

sub setPretty
{
    my ($self, $pretty_enable) = @_;
    $self->{writer}->pretty([$pretty_enable]);
}


#return writer
sub getWriter
{
    my ($self) = @_;
    return $self->{writer};
}


#return output file
sub getFile
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
        } else {
            print "Invalid error level";
        }
    }
}


#check required start elements
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


sub QuoteString
{
    my ($s) = @_;

    $s =~ s/(["\\])/\\$1/g;

    return "\"$s\"";
}


sub MakeField
{
    my ($name, $values) = @_;

    return "" unless exists $values->{$name};

    return QuoteString($name) . ": " . QuoteString($value->{$name})
}


#start analyzer report
sub AddStartTag
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

    my $jsonw = $self->{writer};

    if ( $self->{openStart} == 0 ) {
#	my @attrs = qw/ assess_fw assess_fw_version assessment_start_ts build_fw
#			build_fw_version build_root_dir package_name
#			package_root_dir package_version parser_fw
#			parser_fw_version platform_name tool_name tool_version
#			uuid /;
	my $header = "{\n  \"AnalyzerReport\": {\n    \"assessmentAttrs\" :\n";
	$header .= $self->{writer}->encode($initial_details);
	$header .= "\n";
#	$header .= join ",\n      ", map {MakeField($_, $initial_details)} @attrs;
	$self->{output}->print($header));

	$self->{openStart} = 1;
    }
    $self->{MetricSummaries} = {};
    $self->{BugSummaries} = {};
    return $self;
}


sub AddEndTag
{
    my ($self) = @_;
    if ($self->{error_level} >= 1) {
        if ( $self->{open} ) {
	    my $errorMsg = "SCARF file already closed";
            print "$errorMsg\n";
            if ( $self->{error_level} >= 2 ) {
                die $errorMsg; 
            }
        }
    }
    my ($self) = @_;
    my $jsonw = $self->{writer};

    if ( $self->{openBody} == 1 ) {
	$self->{output}->print("]\n");
	$self->{openBody} = 0;
	$self->{bodyType} = undef;
    }
    if ( $self->{openStart} == 1 ) {
	$self->{output}->print("}\n}\n");
	$self->{openStart} = 0;
    }
    return $self;
}


#validate bug
sub CheckBug
{
    my ($bugInstance, $bugId) = @_;
    my @errors = ();
    for my $bugReqElt (qw/BugLocations BugMessage BuildId AssessmentReportFile/) {
        if (!(defined $bugInstance->{$bugReqElt})) {
            push @errors, "Required element $bugReqElt could not be found in BugInstance:$bugId";
        }
    }
    my $locID = 1;
    my $methodID = 1;
    if (defined $bugInstance->{Methods}) {
        my $methodprimary = 0;
        foreach my $method (@{$bugInstance->{Methods}}) {
            if (!(defined $method->{primary})) {
                push @errors, "Required attribute primary not found for Method:$methodID in BugInstance:$bugId";
            } elsif ($method->{primary}) {
                if ($methodprimary) {
                    push @errors, "More than one primary Method found in BugInstance $bugId";
                } else {
                    $methodprimary = 1;
                }
            }
            if (!(defined $method->{name})) {
                push @errors, "Method name not found for Method:$methodID in BugInstance:$bugId";
            }
        }
        if (!($methodprimary)) {
#           push @errors, "No primary Method found in  BugInstance: $bugId.";
        }
        $methodID++;
    }

    my $locprimary = 0;
    foreach my $location (@{$bugInstance->{BugLocations}}) {
        if (!(defined $location->{primary})) {
            push @errors, "Primary attribute not found for Location:$locID in BugInstance:$bugId";
        } elsif ($location->{primary}) {
            if ($locprimary) {
#               push @errors, "More than one primary BugLocation found in  BugInstance: $bugId";
            } else {
                $locprimary = 1;
            }
        }
        for my $locElt (qw/SourceFile/) {
            if (!(defined $location->{$locElt})) {
                push @errors, "Element $locElt could not be found for Location:$locID in BugInstance:$bugId";
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
    if (!($locprimary)) {
        push @errors, "No primary BugLocation found in BugInstance: $bugId.";
    }

    if (defined $bugInstance->{CweIds}) {
        for my $cweid (@{$bugInstance->{CweIds}}) {
            if ($cweid !~ /[0-9]+/) {
                push @errors, "Not a positive integer for CweID in BugInstance: $bugId";
            }
        }
    }

    if (defined $bugInstance->{InstanceLocation}) {
        if (defined $bugInstance->{InstanceLocation}->{LineNum}) {
            my $line_num = $bugInstance->{InstanceLocation}->{LineNum};
            if (!(defined $line_num->{Start})) {
                push @errors, "Start child missing in LineNum in BugInstance: $bugId";
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
            push @errors, "Neither LineNum or Xpath children were present in InstanceLocation BugInstance: $bugId.";
        }
    }

    return \@errors;
}


#Add a single bug instance to file
sub AddBugInstance
{
    my($self, $bugInstance) = @_;

    ++$self->{bugId};

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

    if ( $self->{bodyType} eq "metric" ) {
	$self->{output}->print("    ],\n    \"BugInstances\" : [\n    ");
    } elsif ( $self->{bodyType} eq "bug") {
	$self->{output}->print(", ");
    } else {
	$self->{output}->print(",\n    \"BugInstances\" : [\n    ");
	$self->{openBody} = 1;
    }

    my $true = JSON->true;
    my $false = JSON->false;

    $self->{bodyType} = "bug";
    $bugInstance->{BugId} = $self->{bugId};
    my $locID = 0;
    foreach my $location (@{$bugInstance->{BugLocations}}) {
        my $primary;
        if ($location->{primary}) {
            $location->{primary} = $true;
        } else {
            $location->{primary} = $false;
        }
	$location->{LocationId} = $locID;
	$locID = $locID + 1;
    }
    my $methodID = 0;
    if (defined $bugInstance->{Methods}) {
        foreach my $method (@{$bugInstance->{Methods}}) {
            my $primary;
            if ($method->{primary}) {
                $method->{primary} = $true;
            } else {
                $method->{primary} = $false;
            }
	    $method->{MethodId} = $methodID;
	    $methodID = $methodID + 1;
	}
    }
    my $json = $self->{writer}->encode($bugInstance);
    $json =~ s/\n/\n    /g;
    $self->{output}->print("$json");


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

    return $self;
}


#check for metrics required elements
sub CheckMetric
{
    my ($metric, $metricId) = @_;
    my @errors = ();
    for my $reqMetrElt (qw/SourceFile Type Value/) {
        if (!(defined $metric->{$reqMetrElt})) {
           push @errors, "Required element $reqMetrElt not found for Metric:$metricId";
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

    if ( $self->{bodyType} eq "bug" ) {
	$self->{output}->print("    ],\n    \"Metrics\" : [\n    ");
    } elsif ( $self->{bodyType} eq "metric") {
	$self->{output}->print(", ");
    } else {
	$self->{output}->print(",\n    \"Metrics\" : [\n    ");
	$self->{openBody} = 1;
    }
    $self->{bodyType} = "metric";
    $metric->{MetricId} = $self->{metricId};

    my $json = $self->{writer}->encode($metric);
    $json =~ s/\n/\n    /g;
    $self->{output}->print("$json");

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
#    $writer->endTag();
    return $self;
}


#Add Summary to file
sub AddSummary
{
    my ($self) = @_ ;
    my $out = $self->{output};
    if ($self->{error_level} >= 1)  {
        if ($self->{bodyType} eq "summary") {
            my $errorMsg = "Adding summary after summary written";
            print "$errorMsg\n";
            if ( $self->{error_level} == 2) {
                die $errorMsg;
            }
        }
    }

    if ($self->{openBody}) {
	$out->print("],\n ");
	$self->{openBody} = 0;
	$self->{bodyType} = "summary";
        if (defined $self->{BugSummaries}) {

	    $out->print("  \"BugSummaries\" : ");
	    $out->print($self->{writer}->encode($self->{BugSummaries}));
	    if ($self->{MetricSummaries}) {
		$out->print(", ");
	    }
        }

        if ($self->{MetricSummaries}) {
	    foreach my $summaryName (keys(%{$self->{MetricSummaries}})) {
                my $summary = $self->{MetricSummaries}->{$summaryName};

		if ( defined $summary->{Sum} ) {
		    my $count = $summary->{Count};
		    my $average = 0;
		    if ($count != 0) {
			$average = $summary->{Sum}/$count;
		    }
		    $summary->{Average} = sprintf("%.2f", $average);
		    my $denominator = $count * ($count - 1);
		    my $square_of_sum = $summary->{Sum} * $summary->{Sum};
		    my $standard_dev = 0;
		    if ($denominator != 0) {
			$standard_dev = sqrt(($summary->{SumOfSquares} * $count - $square_of_sum ) / $denominator);
		    }
		    $summary->{StandardDeviation} = (sprintf("%.2f", $standard_dev));
		}
	    }
	    $out->print(" \"MetricSummaries\" : ");
	    $out->print($self->{writer}->encode($self->{MetricSummaries}));


	}
    }
    return $self;
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
