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

package JSONToHash;
use strict;
use JSON::SL;
use Data::Dumper;

##########Initialize Reader##########
sub new
{
    my ($class, $source, $callbacks) = @_;
    my $hashref = {};
    my $self->{hashref} = $hashref;
#    $self->{lastElt} = "";
    $self->{source} = $source;
    die "no callbacks detected" if !(defined $callbacks);
    $self->{callbacks} = $callbacks;
    $self->{parser} = JSON::SL->new();

#    $self->{MetricSummary} = {};
#    $self->{BugSummary} = {};

    bless $self, $class;

}


##########Initiate parsing of file##########
sub parse
{
    my ( $self ) = @_;
    my $parser = $self->{parser};
    my $hash = {};
    my $lastElt = "";
    my $validStart = 0;
    my $validBody = 0;
#    my $bugPath = "/^/BugInstance/^/";
#    my $metricPath = "/^/Metric/^";
#    my $bugSumPath = "/^/BugSummary/BugCategory/^";
#    my $metricSumPath = "/^/MetricSummaries/^";
#    my $namePath = "/^/tool_name";
#    my $versionPath = "/^/tool_version";
#    my $uuidPath = "/^/uuid";
#    my @pointers = ();
#    push @pointers, $namePath;
#    push @pointers, $versionPath;
#    push @pointers, $uuidPath;
#    if (defined $self->{callbacks}->{BugCallback}) {
#	push @pointers, $bugPath;
#    }
#    if (defined $self->{callbacks}->{MetricCallback}) {
#	push @pointers, $metricPath;
#    }
#    if (defined $self->{callbacks}->{BugSummaryCallback}) {
#	push @pointers, $bugSumPath;
#    }
#    if (defined $self->{callbacks}->{MetricSummaryCallback}) {
#	push @pointers, $metricSumPath;
#    }
#    $parser->set_jsonpointer( [@pointers] );
    $parser->set_jsonpointer( [ ("/^/tool_name", "/^/tool_version", "/^/uuid", "/^/^/^") ] );
#    $parser->nopath(1);
    #possibly use xsdValidator to verify scarfi
    open( my $fh, "<", $self->{source} );
    local $/ = \5; #read only 4MB bytes at a time
    my $hash;

#    print Dumper(@pointers);
    while (my $buf = <$fh>) {
	print $buf;
	$parser->feed($buf); #parse what you can
	#fetch anything that completed the parse and matches the JSON Pointer
	while (my $obj = $parser->fetch) {
#	    print Dumper($obj);
	    my $tempHash = {};
	    if ( ! ( $validStart ) ) {
		if ( $obj->{JSONPointer} eq "/^/tool_name" ) { 
		    $hash->{tool_name} = $obj->{Value};
		} elsif ( $obj->{JSONPointer} eq "/^/tool_version" ) { 
		    $hash->{tool_version} = $obj->{Value};
		} elsif ( $obj->{JSONPointer} eq "/^/uuid" ) { 
		    $hash->{uuid} = $obj->{Value};
		} 
		if ( defined $hash->{tool_name} && defined $hash->{tool_version} && defined $hash->{uuid} ) {
		    $validStart = 1;
		    if ( defined $self->{callbacks}->{InitialCallback} ) {
			$self->{callbacks}->{InitialCallback}->($hash);
		    }
		}
	    } elsif ($obj->{Path} =~ /(\/AnalyzerReport\/BugInstances\/)([0-9]+)/) {
#		$tempHash = $obj->{Value}; 
#		$hash = {};
#		if ( defined $tempHash->{id} ) {
#		    $hash->{BugId} = $tempHash->{id};
#		}
#		for my $simpleElt ( qw/BugCode BugRank ClassName BugSeverity BugGroup BugMessage ResolutionSuggestion/ ) {
#		    if ( defined $tempHash->{$simpleElt} ) {
#			$hash->{$simpleElt} = $tempHash->{$simpleElt};
#		    }	
#		}
#		for my $bugTraceElt ( qw/AssessmentReportFile BuildId/ ) {
#		    if ( defined $tempHash->{BugTrace}->{$bugTraceElt} ) {
#                        $hash->{$bugTraceElt} = $tempHash->{BugTrace}->{$bugTraceElt};
#                    }    
#		}
#    	        if ( defined $tempHash->{Methods}) {
#    	            $hash->{Methods} = [];
#		    for my $method ( @{$tempHash->{Methods}} ) {
#			if ( $method->{primary} eq "true" ) {
#			    $method->{primary} = 1;
#			} else {
#			    $method->{primary} = 0;
#			}
#			push @{$hash->{Methods}}, $method;
#		    }
#    	        }
#    
#		if ( defined $tempHash->{BugLocations}->{Location} ) {
#    	            $hash->{BugLocations} = [];
#		    for my $location ( @{$tempHash->{BugLocations}->{Location}} ) {
#			if ( $location->{primary} eq "true" ) {
#			    $location->{primary} = 1;
#			} else {
#			    $location->{primary} = 0;
#			}
#			push @{$hash->{BugLocations}}, $location;
#		    }
#    	        }
#		
#		my $inst = $tempHash->{InstanceLocation};
#		if ( defined  $inst ) {
#		    $hash->{InstanceLocation} = {};
#		    if ( defined $inst->{Xpath} ) {
#			$hash->{InstanceLocation}->{Xpath} = $inst->{Xpath};
#			return $$hash;
#		    }
#		    my $lineNum = $inst->{LineNum};
#		    if ( defined  $lineNum) {
#			$hash->{InstanceLocation}->{LineNum} = {};
#			if ( defined $lineNum->{Start} ) {
#			   $hash->{InstanceLocation}->{LineNum}->{Start} = $lineNum->{Start};
#			}
#			if ( defined $lineNum->{End} ) {
#			   $hash->{InstanceLocation}->{LineNum}->{End} = $lineNum->{End};
#			}
#		    }
#		}
#		if ( defined $tempHash->{CweId} ) {
#		    $hash->{CweIds} = $tempHash->{CweId};
#		}
#		$self->{callbacks}->{BugCallback}->($hash);				
#	    } elsif ($obj->{Path} =~ /(\/AnalyzerReport\/Metrics\/)([0-9]+)/) {
#		$hash = {};
#		$tempHash = $obj->{Value};
#		if ( defined $tempHash->{id} ) {
#		    $hash->{MetricId} = $tempHash->{id};
#		}
#		for my $metricElm (qw/Class Method Type Value/){
#		    if ( defined $tempHash->{$metricElm} ) {
#			$hash->{$metricElm} = $tempHash->{$metricElm};
#		    }
#		}
#		if ( defined $tempHash->{Location} ) {
#		    if ( defined $tempHash->{Location}->{SourceFile} ) {
#			$hash->{SourceFile} = $tempHash->{Location}->{SourceFile};
#		    }
#		}
		$self->{callbacks}->{BugCallback}->($obj->{Value});#$hash);
		$self->{validBody} = 1;
	    } elsif ($obj->{Path} =~ /(\/AnalyzerReport\/Metrics\/)([0-9]+)/) {
		$self->{callbacks}->{MetricCallback}->($obj->{Value});#$hash);
		$self->{validBody} = 1;
	    } elsif ($obj->{JSONPointer} =~ /\/AnalyzerReport\/BugSummaries\//) {
	#	$hash = {};
	#	for my $category ($obj->{Value}) {
	#	    my $code = $category->{code};
	#	    my $data = { bytes => $category->{bytes}, count => $category->{count} };
	#	    if (! (defined $hash->{$code})) {
	#		$hash->{$code} = {};
	#	    }
	#	    $hash->{$code}->{$category->group} = $data;
	#	}
		$self->{callbacks}->{BugSummaryCallback}->($obj->{Value});#$hash);
	    } elsif ($obj->{JSONPointer} =~ /\/AnalyzerReport\/MetricSummaries\//) {
		$hash = $obj->{Value};
		$self->{callbacks}->{MetricSummaryCallback}->($hash);
	    }
	}	
    }
    if ( $self->{validBody} == 0 ) {
	print (" No BugInstances or Metrics found in file " );
    }
}






return 1;
