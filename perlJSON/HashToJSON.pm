#!/usr/bin/perl

package HashToJSON;
use JSON::Streaming::Writer;
use JSON::MaybeXS;

sub new
{
    my ($class, $output_file, $error_level) = @_;
    my $self = {};
    open $self->{output}, ">", $output_file or die "invalid output file";
#    $self->{output} = $output_file;

#    $self->{writer} = JSON::Streaming::Writer->for_file($self->{output});
#    $self->{writer}->pretty_output(1);

    $self->{writer} = JSON::MaybeXS->new(utf8 => 1, pretty => 1);
    $self->{writer} = $self->{writer}->allow_nonref ([$enable]);

    if (defined $error_level && $error_level == 0 || $error_level == 1) {
        $self->{error_level} = $error_level;
    } else {
	$self->{error_level} = 2;
    }

    $bugID = 1;
    $metricID = 1;
  
    $self->{bodyType} = undef;
    $self->{openBody} = 0;
    $self->{openStart} = 0;
    $self->{MetricSummaries} = undef;
    $self->{BugSummaries} = undef;

    bless $self, $class;
    return $self;
}

#pretty printing options
sub getPretty
{
    my ($self) = @_;
    return $self->{_writer}->get_space_before;
}

sub setPretty
{
    my ($self, $pretty_enable) = @_;
    $self->{_writer}->pretty([$pretty_enable]);
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
sub checkStart
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


#start analyzer report
sub addStartTag
{
    my ($self, $initial_details) = @_;

    if ($self->{error_level} != 0) {
        my $errors = checkStart($initial_details);
        print "$_\n" for @{$errors};
        if (@{$errors} and $self->{error_level} == 2) {
            die "Exiting";
        }
    }
   
    $jsonw = $self->{writer};

    if ( $self->{openStart} == 0 ) {
	$self->{output}->print("{\n  \"AnalyzerReport\" : {\n    \"tool_name\" : \"$initial_details->{tool_name}\",\n    \"tool_version\" : \"$initial_details->{tool_version}\",\n    \"uuid\" : \"$initial_details->{uuid}\",\n");	


#	$jsonw->start_object();
#	$jsonw->start_property("AnalyzerReport");
#        $jsonw->start_object();
#        $jsonw->add_property("tool_name" => $initial_details->{tool_name});
#        $jsonw->add_property("tool_version" => $initial_details->{tool_version});
#	$jsonw->add_property("uuid" => $initial_details->{uuid});
	$self->{openStart} = 1;
    }
    return $self;
}

sub addEndTag
{
    my ($self) = @_;
    $jsonw = $self->{writer};
    
    if ( $self->{openBody} == 1 ) {
	$self->{output}->print("]\n");
#	$jsonw->end_array();    
#	$jsonw->end_property();
	$self->{openBody} = 0;
	$self->{bodyType} = undef;
    }
    if ( $self->{openStart} == 1 ) {
	$self->{output}->print("}\n}\n");
#	$jsonw->end_object();
#	$jsonw->end_property();
#	$jsonw->end_object();
	$self->{openStart} = 0;
    }
    return $self;
}

sub checkBug
{
    my ($bugInstance) = @_;
    my @errors = ();
    for my $bugReqElt (qw/BugLocations BugMessage BuildId AssessmentReportFile/) {
        if (!(defined $bugInstance->{$bugReqElt})) {
            push @errors, "Required element: $bugReqElt could not be found in BugInstance:$bugID";
        }
    }
    my $locID = 1;
    my $methodID = 1;
    if (defined $bugInstance->{Methods}) {
        my $methodprimary = 0;
        foreach my $method (@{$bugInstance->{Methods}}) {
            if (!(defined $method->{primary})) {
                push @errors, "Required attribute: primary not found for Method:$methodID in BugInstance:$bugID";
            } elsif ($method->{primary}) {
                if ($methodprimary) {
                    push @errors, "Misformed Element: More than one primary Method found";
                } else {
                    $methodprimary = 1;
                }
            }
            if (!(defined $method->{name})) {
                push @errors, "Required text: name not found for Method:$methodID in BugInstance:$bugID";
            }
        }
        if (!($methodprimary)) {
#           push @errors, "Misformed Element: No primary Method found in  BugInstance: $bugID.";
        }
        $methodID++;
    }

    my $locprimary = 0;
    foreach my $location (@{$bugInstance->{BugLocations}}) {
        if (!(defined $location->{primary})) {
            push @errors, "Required attribute: primary could not be found for Location:$locID in BugInstance:$bugID";
        } elsif ($location->{primary}) {
            if ($locprimary) {
#               push @errors, "Misformed Element: More than one primary BugLocation found in  BugInstance: $bugID";
            } else {
                $locprimary = 1;
            }
        }
        for my $locElt (qw/SourceFile/) {
            if (!(defined $location->{$locElt})) {
                push @errors, "Required element: $locElt could not be found for Location:$locID in BugInstance:$bugID";
            }
        }
	for my $optLocElt (qw/StartColumn EndColumn StartLine EndLine/) {
            if (defined $location->{$optLocElt}  ) {
                if ($location->{$optLocElt} !~ /[0-9]+/) {
                    push @errors, "Wrong value type: $optLocElt child of BugLocation in BugInstance: $bugID requires a positive integer.";
                }
            }
        }
        $locID++;
    }
    if (!($locprimary)) {
        push @errors, "Misformed Element: No primary BugLocation found in BugInstance: $bugID.";
    }

    if (defined $bugInstance->{CweIds}) {
        for my $cweid (@{$bugInstance->{CweIds}}) {
            if ($cweid !~ /[0-9]+/) {
                push @errors, "Wrong value type: CweID expected to be a positive integer in BugInstance: $bugID";
            }
        }
    }

    if (defined $bugInstance->{InstanceLocation}) {
        if (defined $bugInstance->{InstanceLocation}->{LineNum}) {
            my $line_num = $bugInstance->{InstanceLocation}->{LineNum};
            if (!(defined $line_num->{Start})) {
                push @errors, "Required element missing: Could not find Start child of a LineNum in BugInstance: $bugID.";
            } elsif ($line_num->{Start} !~ /[0-9]+/ ) {
               push @errors, "Wrong value type: Start child of LineNum requires a positive integer BugInstance: $bugID.";
            }
            if (!(defined $line_num->{End})) {
                push @errors, "Required element missing: Could not find End child of a LineNum BugInstance: $bugID.";
            } elsif ($line_num->{End} !~ /[0-9]+/) {
                push @errors, "Wrong value type: End child of LineNum requires a positive integer BugInstance: $bugID.";
            }
        }
        elsif (!(defined $bugInstance->{InstanceLocation}->{Xpath})) {
            push @errors, "Misformed Element: Neither LineNum or Xpath children were present in InstanceLocation BugInstance: $bugID.";
        }
    }

    return \@errors;
}


#add a single bug instance to file
sub addBugInstance
{
    my($self, $bugInstance) = @_;
    if ($self->{error_level} != 0) {
        my $errors = checkBug($bugInstance);
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

    if ( $self->{bodyType} eq "metric" ) {
	$self->{output}->print("    ],\n    \"BugInstances\" : [\n    ");
    } elsif ( $self->{bodyType} eq "bug") {
	$self->{output}->print(", ");	
    } else {
	$self->{output}->print("    \"BugInstances\" : [\n    ");
	$self->{openBody} = 1;
    } 
    $self->{bodyType} = "bug";
    $bugInstance->{BugId} = $bugID;
    foreach my $location (@{$bugInstance->{BugLocations}}) {
        my $primary;
        if ($location->{primary}) {
            $location->{primary} = "true";
        } else {
            $location->{primary} = "false";
        }
    if (defined $bugInstance->{Methods}) {
        foreach my $method (@{$bugInstance->{Methods}}) {
            my $primary;
            if ($method->{primary}) {
                $method->{primary} = "true";
            } else {
                $method->{primary} ="false";
            }
	}
    }
    my $json = $self->{writer}->encode($bugInstance);
    $json =~ s/\n/\n    /g;
    $self->{output}->print("$json");

#   if ( defined $self->{MetricSummaries} ) {
#	if ($self->{error_level} != 0) {
#	    print  "Only one of BugInstances or Metrics allowed";
#	}
#	if ( $self->{error_level} == 2 ) {
#	    die "Exiting";
#	}
#	$writer->end_array;
#    } elsif ( ! ( defined $self->{BugSummaries} ) ) {
#	$writer->start_property("BugInstance");
#	$writer->start_array;
#	$self->{openBody} = 1;
#    } 
#    $writer->add_value($bugInstance);
#
#    $writer->start_object();
#    $writer->add_property("id", $bugID);
#
#    for my $baseElts( qw/ClassName BugGroup BugCode BugRank BugSeverity BugMessage ResolutionSuggestion/ ) {    
#	if (defined $bugInstance->{$baseElts}) {
#	    $writer->add_property($baseElts, $bugInstance->{$baseElts});
#	}
#    }
#
#    my $locID = 1;
#    $writer->start_property("BugLocations");
#    $writer->start_object();
#    $writer->start_property("Location");
#    $writer->start_array();
#    foreach my $location (@{$bugInstance->{BugLocations}}) {
#	$writer->start_object();
#        my $primary;
#        if ($location->{primary}) {
#            $primary = "true";
#        } else {
#            $primary = "false";
#        }
#	$writer->add_property("id", $locID);
#	$writer->add_property("primary", $primary);
#        for my $locElt (qw/SourceFile StartLine EndLine StartColumn EndColumn Explanation/) {
#            if ( defined $location->{$locElt}  ) {
#		$writer->add_property($locElt, $location->{$locElt});
#            }
#        }
#        $locID++;
#	$writer->end_object();
#    }
#    $writer->end_array();
#    $writer->end_property();
#    $writer->end_object();
#    $writer->end_property();
#
#    if (defined $bugInstance->{CweIds}) {
#	$writer->start_property("CweId");
#	$writer->add_value($bugInstance->{CweIds});
#	$writer->end_property;
##        foreach my $cweid (@{$bugInstance->{CweIds}}) {
##            $writer->add_value($cweid);
##        }
#    }
#
#    if (defined $bugInstance->{Methods}) {
#        my $methodID=1;
#        $writer->start_property("Methods");
#	$writer->start_array();
#        foreach my $method (@{$bugInstance->{Methods}}) {
#            my $primary;
#            if ($method->{primary}) {
#                $primary = "true";
#            } else {
#                $primary ="false";
#            }
#	    $writer->start_object();
#	    $writer->add_property("name", $method->{name});
#	    $writer->add_property("id", $methodID);
#	    $writer->add_property("primary", $primary);
#	    $writer->end_object();
#            $methodID++;
#        }
#	$writer->end_array();
#	$writer->end_property();
#    }
#
#    $writer->start_property("BugTrace");
#    $writer->start_object();
#    if ( defined $bugInstance->{BuildId} ) {
#	$writer->add_property("BuildId", $bugInstance->{BuildId});
#    }
#    if ( defined $bugInstance->{AssessmentReportFile} ) {
#	$writer->add_property("AssessmentReportFile", $bugInstance->{AssessmentReportFile});
#    }
#    if ( defined $bugInstance->{InstanceLocation} ) {
#	$writer->start_property("InstanceLocation");
#        $writer->start_object();
#        if ( defined $bugInstance->{InstanceLocation}->{Xpath} ) {
#            $writer->add_property("Xpath", $bugInstance->{InstanceLocation}->{Xpath});
#        }
#        if ( defined $bugInstance->{InstanceLocation}->{LineNum} ) {
#            $writer->add_property("LineNum", $bugInstance->{InstanceLocation}->{LineNum});
#        }
#        $writer->end_object();
#        $writer->end_property();
#    }
#    $writer->end_object();
#    $writer->end_property();
#    $writer->end_object();

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
    
    $bugID++;
    return $self;
}

#check for metrics required elements
sub checkMetric
{
    my ($metric) = @_;
    my @errors = ();
    for my $reqMetrElt (qw/SourceFile Type Value/) {
        if (!(defined $metric->{$reqMetrElt})) {
           push @errors, "Required element: $reqMetrElt could not be found for Metric:$metricID";
        }
    }
    return \@errors;
}


#write a single metric
sub addMetric
{
    my($self, $metric) = @_;
    if ($self->{error_level} != 0) {
        my $errors = checkMetric($metric);
        print "$_\n" for @$errors;
        if (@$errors and $self->{error_level} == 2) {
            die "Exiting";
        }
    }

    my $writer = $self->{writer};

    if ( $self->{bodyType} eq "bug" ) {
	$self->{output}->print("    ],\n    \"Metrics\" : [\n    ");
    } elsif ( $self->{bodyType} eq "metric") {
	$self->{output}->print(", ");	
    } else {
	$self->{output}->print("    \"Metrics\" : [\n    ");
	$self->{openBody} = 1;
    } 
    $self->{bodyType} = "metric";
    $bugInstance->{MetricId} = $metricID;

    my $json = $self->{writer}->encode($metric);
    $json =~ s/\n/\n    /g;
    $self->{output}->print("$json");

#    $writer->start_object();
#
#    $writer->add_property("id", $metricID);
#    $writer->start_property("Location");
#    $writer->start_object();
#    $writer->add_property("SourceFile", $metric->{SourceFile});
#    $writer->end_object();
#    $writer->end_property();
#
#    for my $metrElmt (qw/Class Method Type Value/) {
#        if (defined $metric->{$metrElmt}) {
#	    $writer->add_property($metrElmt, $metric->{$metrElmt});
#        }
#    }
#
#    $writer->end_object();
#
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
#    $writer->endTag();
    return $self;
}


#Add Summary to file
sub addSummary
{
    my ($self) = @_ ;
    my $out = $self->{output};
    if ($self->{openBody}) {
	$out->print("],\n ");
#	$writer->end_array();
#	$writer->end_property();
	$self->{openBody} = 0;
	$self->{bodyType} = undef; 
        if (defined $self->{BugSummaries}) {

	    $out->print("  \"BugSummaries\" : ");
	    $out->print($self->{writer}->encode($self->{BugSummaries}));
#	    $writer->start_property("BugSummary");
#	    $writer->start_object();
#	    $writer->start_property("BugCategory");
#	    $writer->start_array();
#	    foreach my $code (keys %{$self->{BugSummaries}}) {
#		foreach my $group (keys %{$self->{BugSummaries}->{$code}}) {
#                    my $hashes = $self->{BugSummaries}->{$code}->{$group};
#		    $writer->start_object();
#		    $writer->add_property("group", $group);
#		    $writer->add_property("code", $code);
#		    $writer->add_property("count", $hashes->{count});
#		    $writer->add_property("bytes", $hashes->{bytes});
#		    $writer->end_object();
#                }
#            }
#	    $writer->end_array();
#	    $writer->end_property();
#	    $writer->end_object();
#	    $writer->end_property();
	    if ($self->{MetricSummaries}) {
		$out->print(", ");		
	    }
        }
    
        if ($self->{MetricSummaries}) {
#	    $writer->start_property("MetricSumamries");
#	    $writer->start_object();
#	    $writer->start_property("MetricSummary");
#	    $writer->start_array();
	    foreach my $summaryName (keys(%{$self->{MetricSummaries}})) {
                my $summary = $self->{MetricSummaries}->{$summaryName};
#    	    
#		$writer->add_property("Type", $summaryName);
#    
                my $count = $summary->{Count};
#		$writer->add_property("Count", $count);
#    
#                if (defined $summary->{Sum}) {
#                    for my $sumElt (qw/Sum SumOfSquares Minimum Maximum/) {
#                        $writer->add_property($sumElt, $summary->{$sumElt});
#                    }
#    
                my $average = 0;
                if ($count != 0) {
                    $average = $summary->{Sum}/$count;
                }
		$summary->{Average} = sprintf("%.2f", $average);
#                    $writer->add_property("Average", (sprintf("%.2f", $average)));
#    
		my $denominator = $count * ($count - 1);
                my $square_of_sum = $summary->{Sum} * $summary->{Sum};
                my $standard_dev = 0;
                if ($denominator != 0) {
                    $standard_dev = sqrt(($summary->{SumOfSquares} * $count - $square_of_sum ) / $denominator);
                }
		$summary->{StandardDeviation} = (sprintf("%.2f", $standard_dev));
#                   $writer->add_property("StandardDeviation", (sprintf("%.2f", $standard_dev)));
	    }
	    $out->print(" \"MetricSummaries\" : ");
	    $out->print($self->{writer}->encode($self->{MetricSummaries}));

	}
    }
    return $self;
}




return 1;












