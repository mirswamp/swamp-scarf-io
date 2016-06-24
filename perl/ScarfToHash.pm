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

package ScarfToHash;
use strict;
use XML::Twig;
use Data::Dumper;

sub new
{
    my ($class, $source, $callback) = @_;
    
    my $start_tags = {};
    my $roots = {};
    if (defined $callback->{MetricCallback}) {
	$roots->{'AnalyzerReport/Metric'} = sub { metricHandler(@_, $callback->{MetricCallback}) };
    }
    if (defined $callback->{BugCallback}) {
	$roots->{'AnalyzerReport/BugInstance'} = sub { bugHandler(@_,$callback->{BugCallback}) };    
    }
    if (defined $callback->{SummaryCallback}) {
	$roots->{'AnalyzerReport/BugSummary'} = sub { summaryHandler(@_,$callback->{SummaryCallback}) };
	$roots->{'AnalyzerReport/MetricSummaries'} = sub { summaryHandler(@_,$callback->{SummaryCallback}) };
    }

    my $self->{twig} = XML::Twig->new(twig_roots=>$roots);

    if (defined $callback->{InitialCallback}) {
	$self->{twig}->setStartTagHandlers({AnalyzerReport=>sub { initHandler(@_,$callback->{InitialCallback}) }});
    }

    $self->{source} = $source;
    bless $self, $class;
    return $self;
}


sub parse
{
    my ($self) = @_;
    #possibly use xsdValidator to verify scarf
    $self->{twig}->parsefile($self->{source});
}

sub setFile
{
    my ($self,$source) = @_;
    $self->{source} = $source;
    return $self;
}


sub initHandler
{
    my ($twig, $elt, $callback) = @_;
    my $initial_details = {};

    for my $attr (qw/tool_name tool_version uuid/) {
	$initial_details->{$attr} = $elt->{att}->{$attr};
    }

    $callback->($initial_details);
}


sub bugHandler
{
    my ($twig, $elt, $callback) = @_;
    #Given a fully parsed bug instance convert to hash of data
    
    my $bug = {};

    my $methods_elt = $elt->first_child("Methods");
    my @methods;
    if (defined $methods_elt) {
	foreach my $method_child ($methods_elt->children("Method")) {
	    my $method = {};
	    if ($method_child->{'att'}->{'primary'} eq "true") {
		$method->{primary} = 1;
	    } else {
		$method->{primary} = 0;
	    }
	    $method->{name} = $method_child->text;
	    $method->{MethodId} = $method_child->{att}->{id};
	    push @methods, $method;
	}
    }

    my @cweids_elts = $elt->children("CweId");
    my @cweids = ();
    foreach my $cwe_child (@cweids_elts) {
	push @cweids, $cwe_child->text;
    }

    my $bug_trace_elt = $elt->first_child("BugTrace");
    my $instloc_elt = $bug_trace_elt->first_child("InstanceLocation");
    my $instloc;
    if (defined $instloc_elt) {
	if (defined $instloc_elt->first_child("Xpath")) {
	    $instloc->{Xpath} = $instloc_elt->first_child_text("Xpath");
	}

	if (defined $instloc_elt->first_child("LineNum")) {
	    my $line_num = {};
	    $line_num->{Start} = $instloc_elt->first_child("LineNum")->first_child_text("Start");
	    $line_num->{End} = $instloc_elt->first_child("LineNum")->first_child_text("End");
	    $instloc->{LineNum} = $line_num;
	}
    }

    my @bug_locations = ();
    my $bug_locs_elt = $elt->first_child("BugLocations");
    foreach my $bug_loc_child ($bug_locs_elt->children("Location")) {
	my $bug_location= {};
	if ($bug_loc_child->{'att'}->{'primary'} eq "true") { 
	    $bug_location->{primary} = 1;
	} else {
	    $bug_location->{primary} = 0;
	}
	for my $reqLocElt (qw/SourceFile/) {
	    $bug_location->{$reqLocElt} = $bug_loc_child->first_child_text($reqLocElt);
	}
	for my $optLocElt (qw/StartColumn EndColumn Explanation StartLine EndLine/) {
	    if (defined $bug_loc_child->first_child($optLocElt)) {
		$bug_location->{$optLocElt} = $bug_loc_child->first_child_text($optLocElt);
	    }
	}
	$bug_location->{LocationId} = $bug_loc_child->{att}->{id};

	push @bug_locations, $bug_location;
    }

    $bug->{BugId} = $elt->{att}->{id};

    for my $reqElt (qw/BugMessage/) {
	$bug->{$reqElt} = $elt->first_child_text($reqElt);
    }

    for my $reqTraceElt (qw/BuildId AssessmentReportFile/) {
	$bug->{$reqTraceElt} = $elt->first_child("BugTrace")->first_child_text($reqTraceElt);
    }


    for my $optElt (qw/BugCode ClassName BugGroup BugRank BugSeverity ResolutionSuggestion/) {
	if (defined $elt->first_child($optElt)) {
	   $bug->{$optElt} = $elt->first_child_text($optElt);
	}
    }
    
    $bug->{BugLocations} = [@bug_locations];

    if (scalar @methods != 0) {
	$bug->{Methods} = [@methods];
    } 

    if (scalar @cweids != 0) {
	$bug->{CweIds} = [@cweids];
    }
    
    if(defined $instloc) {
	$bug->{InstanceLocation} = $instloc;
    }

    $callback->($bug); 

    $elt->purge;
}


sub metricHandler
{
    my ($twig, $elt, $callback) = @_;

    #Given a metric elmnt that is fully parsed convert to a hash of data.
    my $metric = {};

    $metric->{MetricId} = $elt->{att}->{id};
    $metric->{SourceFile} = $elt->first_child("Location")->first_child_text("SourceFile");
    
    for my $optMetr (qw/Class Method/) {
	if (defined $elt->first_child($optMetr)) {
	    $metric->{$optMetr} = $elt->first_child_text($optMetr);
	}
    }
    for my $reqMetr (qw/Type Value/) {
	$metric->{$reqMetr} = $elt->first_child_text($reqMetr);
    }

    $callback->($metric);

    $elt->purge;
}


sub summaryHandler
{
    my ($twig, $elt, $callback) = @_;
    my $summary = {};
    if ($elt->name eq "BugSummary") {
	foreach my $category ($elt->children('BugCategory')) {
#	    my $tag = "$category->{att}->{code}~#~$category->{att}->{group}";
	    my $sum_hash = {};
	    for my $attr (qw/count bytes/) {
		$sum_hash->{$attr} = $category->{att}->{$attr};
	    }
	    $summary->{$category->{att}->{code}}->{$category->{att}->{group}} = $sum_hash;
	}
    }

    if ($elt->name eq 'MetricSummaries') {
	foreach my $metric ($elt->children('MetricSummary')) {
	    my $type = $metric->first_child_text("Type");
	    my $sum_hash = {};
	    $sum_hash->{Count} = $metric->first_child_text("Count");
	    if (defined $metric->first_child("Sum")) {
		for my $sumElt (qw/Sum SumOfSquares Minimum Maximum Average StandardDeviation/) {
		    $sum_hash->{$sumElt} = $metric->first_child_text($sumElt);
		}
	    }
	    $summary->{$type} = $sum_hash;	
	}
    }
        
    $callback->($summary);

    $elt->purge;
}

1;
