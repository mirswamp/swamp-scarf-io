#!/usr/bin/perl -w

package ScarfToHash;
use strict;
use XML::Parser;
use Data::Dumper;

sub new
{
    my ($class, $source, $callbacks) = @_;
    my $hashref = {};
    my $self->{hashref} = $hashref;
    $self->{lastElt} = "";
    $self->{source} = $source;
    die "no callbacks detected" if !(defined $callbacks);
    $self->{callbacks} = $callbacks; 
    $self->{parser} = new XML::Parser ();

    $self->{MetricSummary} = {};
    $self->{BugSummary} = {};

    bless $self, $class;

}

sub parse
{
    my ( $self ) = @_;
    my $hash = {};
    my $lastElt = "";
    $self->{parser}->setHandlers(
				    "Start", sub { $hash = startHandler( \$hash, \$lastElt, @_ ) },
				    "End", sub { $hash = endHandler( \$hash, \$lastElt, 
					    $self->{callbacks}->{BugCallback},  $self->{callbacks}->{MetricCallback},
					    $self->{callbacks}->{InitialCallback}, 
					    $self->{callbacks}->{BugSummaryCallback}, 
					    $self->{callbacks}->{MetricSummaryCallback},@_ ) },
				    "Char", sub { $hash = charHandler( \$hash, \$lastElt, @_ ) },
				    "Default" ,\&defaultHandler
				);
    #possibly use xsdValidator to verify scarf
    $self->{parser}->parsefile($self->{source});
}

sub startHandler
{
    my ( $hash, $lastElt, $parser, $elt, %atts ) = @_;
    if ( $elt eq "BugInstance" ) {
	$$hash->{BugId} = $atts{id};
    } elsif ( $elt eq "Metric" ) {
	$$hash = {#Value => undef, Method => undef, Class => undef, 
			    SourceFile => undef, MetricId => $atts{id}, Type => undef
			    };  
    } elsif ( $elt eq "AnalyzerReport" ) {
	$$hash = { tool_name => $atts{tool_name}, tool_version => $atts{tool_version}, uuid => $atts{uuid} };
    } elsif ( $elt eq "BugCategory" ) {
	$$hash->{$atts{code}}->{$atts{group}} = { bytes => $atts{bytes}, count => $atts{count} };
    } elsif ( $elt eq "MetricSummaries" ) {
	my @metricSum = ();
	$$hash->{MetricSummaries} = [@metricSum];
    } elsif ( $elt eq "MetricSummary" ) {
	my $sum = {};
	push @{$$hash->{MetricSummaries}}, $sum;
    } elsif ( $elt eq "Methods" ) {
	my @methods = ();
	$$hash->{Methods} = [@methods];
    } elsif ( $elt eq "Method" && defined $$hash->{Methods} ) {
	my $primary;
	if ( $atts{primary} eq "true" ) {
	    $primary = 1;
	} else {
	 $primary = 0;
	}
	my $method = {MethodId => $atts{id}, primary => $primary};
	push @{$$hash->{Methods}}, $method;
    } elsif ( $elt eq "BugLocations" ) {
	my @locs = ();
	$$hash->{BugLocations} = [@locs];
    } elsif ( $elt eq "Location" && defined $$hash->{BugLocations} ) {
	my $primary;
	if ( $atts{primary} eq "true" ) {
	    $primary = 1;
	} else {
	 $primary = 0;
	}
	my $loc = {LocationId => $atts{id}, primary => $primary};
	push @{$$hash->{BugLocations}}, $loc;
    } elsif ( $elt eq "InstanceLocation" ) {
	$$hash->{InstanceLocation} = {};
    } elsif ( $elt eq "LineNum" && defined $$hash->{InstanceLocation} ) {
	$$hash->{InstanceLocation}->{LineNum} = {};
    }
    $$lastElt = $elt;
    return $$hash;
}

sub endHandler
{
    my ( $hash, $lastElt, $bugcallback, $metriccallback, $initialcallback, $bugsumcallback, $metricsumcallback, 
	    $parser, $elt ) = @_;
    if ( $elt eq "BugInstance" && defined $bugcallback ) {
	$bugcallback->( $$hash );
	$$hash = {};
    } elsif ( $elt eq "Metric" && defined $metriccallback ) {
	$metriccallback->( $$hash );
        $$hash = {};
    } elsif ( $elt eq "AnalyzerReport" && defined $initialcallback ) {
	$initialcallback->( $$hash );
        $$hash = {};
    } elsif ( $elt eq "BugSummary" && defined $bugsumcallback ) {
    	$bugsumcallback->( $$hash ); 
        $$hash = {};
    } elsif ( $elt eq "MetricSummaries" && defined $metricsumcallback ) {
	$metricsumcallback->( $$hash );
        $$hash = {};
    }
    $$lastElt = "";
    return $$hash;
}

sub charHandler
{
    my ( $hash, $lastElt, $parser, $chars ) = @_;
    my $elt = $$lastElt;
    for my $simpleElt ( qw/AssessmentReportFile BuildId BugCode BugId BugRank ClassName BugSeverity BugGroup
    BugMessage ResolutionSuggestion Class Method Value/ ) {
	if ( $elt eq $simpleElt ) {
	    $$hash->{$elt} = $chars;
	    return $$hash;
	}
    }
    for my $metricSumElt ( qw/Type Count Sum SumOfSquares Minimum Maximum Average StandardDeviation/ ) {
	if ( $elt eq $metricSumElt && defined $$hash->{MetricSummaries} ) {
	    @{$$hash->{MetricSummaries}}[-1]->{$metricSumElt} = $chars;
	    return $$hash;
	}
    }

    if ( $elt eq "SourceFile" && exists $$hash->{SourceFile} ) {
        $$hash->{$elt} = $chars;
        return $$hash;
    }
    if ( $elt eq "Type" && exists $$hash->{SourceFile} ) {
        $$hash->{$elt} = $chars;
        return $$hash;
    }

    if ( $elt eq "CweID" ) {
	if ( exists $$hash->{CweIds} ) {
	    push @{$$hash->{CweIds}}, $chars;
	} else {
	    $$hash->{CweIds} = ($chars);
	}
	return $$hash;
    }

    if ( $elt eq "Method" && defined $$hash->{Methods}) {
	@{$$hash->{Methods}}[-1]->{name} = $chars;
	return $$hash;
    }

    for my $locElt ( qw/EndColumn EndLine Explanation StartLine SourceFile StartColumn/ ) {
	if ( $elt eq $locElt && defined $$hash->{BugLocations}) {
	    @{$$hash->{BugLocations}}[-1]->{$elt} = $chars;
	    return $$hash;
	}
    }	

    
    if ( $elt eq "Xpath" && defined $$hash->{InstanceLocation} ) {
        $$hash->{InstanceLocation}->{Xpath} = $chars;
        return $$hash;
    }
    if ( $elt eq "Start" && exists $$hash->{InstanceLocation}->{LineNum} ) {
        $$hash->{InstanceLocation}->{LineNum}->{$elt} = $chars;
        return $$hash;
    }
    if ( $elt eq "End" && exists $$hash->{InstanceLocation}->{LineNum} ) {
        $$hash->{InstanceLocation}->{LineNum}->{$elt} = $chars;
        return $$hash;
    }
 
    return $$hash;
}

sub defaultHandler
{
    #throw everything away
}



1;






