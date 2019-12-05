#!/usr/bin/perl -w

package ScarfXmlReader;
use strict;
use FindBin;
use lib "$FindBin::Bin";
use XML::Parser;
use SwampXML;
use Data::Dumper;

######### HANDLERS ##########
my %InitialHandlers = (
    start => \&InitialStartHandler
);

my %AnalyzerReportHandlers = (
    start => \&AnalyzerReportStartHandler
);

my %BugInstanceHandlers = (
    start => \&BugInstanceStartHandler,
    end => \&BugInstanceEndHandler,
);

my %BugLocationsHandlers = (
    start => \&BugLocationsStartHandler
);

my %BugLocationsLocationHandlers = (
    start => \&BugLocationsLocationStartHandler
);

my %BugTraceHandlers = (
    start => \&BugTraceStartHandler
);

my %MethodsHandlers = (
    start => \&MethodsStartHandler
);

my %InstanceLocationHandlers = (
    start => \&InstanceLocationStartHandler
);

my %InstanceLocationLineNumHandlers = (
    start => \&InstanceLocationLineNumStartHandler
);

my %BugSummaryHandlers = (
    start => \&BugSummaryStartHandler,
    end => \&BugSummaryEndHandler
);

my %MetricHandlers = (
    start => \&MetricStartHandler,
    end => \&MetricEndHandler
);

my %MetricLocationHandlers = (
    start => \&MetricLocationStartHandler
);

my %MetricSummariesHandlers = (
    start => \&MetricSummariesStartHandler
);

my %MetricSummaryHandlers = (
    start => \&MetricSummaryStartHandler,
    end => \&MetricSummaryEndHandler
);

my %LeafHandlers = (
    char => \&SwampXML::LeafCharHandler
);
#############################


######## GLOBAL VARIABLES #######

my @hd = qw/handlers data/;

#################################


########### USER CALLBACKS #########

sub SetInitialCallback {
    my ($self, $callback) = @_;
    $self->{callbacks}{AnalyzerReport} = $callback;
}

sub SetBugCallback {
    my ($self, $callback) = @_;
    $self->{callbacks}{BugInstance} = $callback;
}

sub SetMetricCallback {
    my ($self, $callback) = @_;
    $self->{callbacks}{Metric} = $callback;
}

sub SetBugSummaryCallback {
    my ($self, $callback) = @_;
    $self->{callbacks}{BugSummary} = $callback;
}

sub SetMetricSummaryCallback {
    my ($self, $callback) = @_;
    $self->{callbacks}{MetricSummary} = $callback;
}

sub SetFinalCallback {
    my ($self, $callback) = @_;
    $self->{callbacks}{final} = $callback;
}

sub SetCallbackData {
    my ($self, $callbackData) = @_;
    $self->{callbacks}{callbackData} = $callbackData;
}

####################################

sub new {
    my ($class, $fileToParse) = @_;

    my $self = {};
    $self->{fileToParse} = $fileToParse;
    $self->{parser} = new XML::Parser(
        Handlers => {
            Start   => sub {
                SwampXML::StartHandler($self, @_);
            },
            End     => sub {
                SwampXML::EndHandler($self, @_);
            },
            Char    => sub {
                SwampXML::CharHandler($self, @_);
            },
            Default => sub {
                SwampXML::DefaultHandler($self, @_);
            }
        }
    );

    bless $self, $class;
}

sub Parse {
    my ($self) = @_;

    push @{$self->{stack}}, { handlers => \%InitialHandlers, data => {} };

    $self->{parser}->parsefile($self->{fileToParse});

    pop @{$self->{stack}};
}

sub CallCallback {
    my ($self, $data, $name) = @_;

    if ($self->{callbacks}{$name}) {
        $self->{callbacks}{$name}($data, $self->{callbacks}{callbackData});
    }
}

sub InitialStartHandler {
    my ($self, $data, $name, $atts) = @_;
    
    my %validElements = (
        AnalyzerReport => { handlers => \%AnalyzerReportHandlers, data => {} }
    );

    CallCallback($self, $atts, $name);
    return SwampXML::FindElementNoData($data, \%validElements, $name);
}

sub AnalyzerReportStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        BugInstance => { handlers => \%BugInstanceHandlers, data => {} },
        BugSummary => { handlers => \%BugSummaryHandlers, data => {} },
        Metric => { handlers => \%MetricHandlers, data => {} },
        MetricSummaries => { handlers => \%MetricSummariesHandlers, data => {} }
    );

    if ($name eq 'BugInstance' and defined $atts->{id}) {
        $validElements{$name}->{data}{BugId} = $atts->{id};
    } elsif ($name eq 'Metric' and defined $atts->{id}) {
        $validElements{$name}->{data}{MetricId} = $atts->{id};
    }

    return SwampXML::FindElementNoData($data, \%validElements, $name);
}

sub BugInstanceStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        BugGroup => { handlers => \%LeafHandlers, data => '' },
        BugCode => { handlers => \%LeafHandlers, data => '' },
        BugMessage => { handlers => \%LeafHandlers, data => '' },
        BugRank => { handlers => \%LeafHandlers, data => '' },
        BugSeverity => { handlers => \%LeafHandlers, data => '' },
        ResolutionSuggestion => { handlers => \%LeafHandlers, data => '' },
        BugLocations => { handlers => \%BugLocationsHandlers, data => [] },
        BugTrace => { handlers => \%BugTraceHandlers, data => {} },
        Methods => { handlers => \%MethodsHandlers, data => [] },
        ClassName => { handlers => \%LeafHandlers, data => '' },
        CweId => { handlers => \%LeafHandlers, data => '' }
    );

    if ($name eq 'CweId') {        
        my ($newHandlers, $newData) = @{$validElements{$name}}{@hd};
        push @{$data->{CweIds}}, $newData;
        $newData = \$data->{CweIds}[-1];
        return ($newHandlers, $newData);
    }

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub BugLocationsStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        Location => { handlers => \%BugLocationsLocationHandlers, data => {} }
    );

    if ($name eq 'Location') {
        $validElements{Location}->{data}{LocationId} = $atts->{id} if defined $atts->{id};
        $validElements{Location}->{data}{primary} = $atts->{primary} if defined $atts->{primary};
    }

    if ($validElements{$name}) {
        my ($newHandlers, $newData) = @{$validElements{$name}}{@hd};
        push @{$data}, $newData;
        return ($newHandlers, $newData);
    }
}

sub BugLocationsLocationStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        SourceFile => { handlers => \%LeafHandlers, data => '' },
        StartLine => { handlers => \%LeafHandlers, data => '' },
        EndLine => { handlers => \%LeafHandlers, data => '' },
        StartColumn => { handlers => \%LeafHandlers, data => '' },
        EndColumn => { handlers => \%LeafHandlers, data => '' },
        Explanation => { handlers => \%LeafHandlers, data => '' }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub BugTraceStartHandler {
    my ($self, $data, $name, $atts) = @_;
    
    my %validElements = (
        BuildId => { handlers => \%LeafHandlers, data => '' },
        AssessmentReportFile => { handlers => \%LeafHandlers, data => '' },
        InstanceLocation => { handlers => \%InstanceLocationHandlers, data => {} }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub MethodsStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        Method => { handlers => \%LeafHandlers, data => {} }
    );

    if ($name eq 'Method') {
        $validElements{$name}->{data}{MethodId} = $atts->{id} if defined $atts->{id};
        $validElements{$name}->{data}{primary} = $atts->{primary} if defined $atts->{primary};
        $validElements{$name}->{data}{name} = '';

        my ($newHandlers, $newData) = @{$validElements{$name}}{@hd};
        push @{$data}, $newData;
        $newData = \$validElements{$name}->{data}{name};
        return ($newHandlers, $newData);
    }
}

sub InstanceLocationStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        Xpath => { handlers => \%LeafHandlers, data => '' },
        LineNum => { handlers => \%InstanceLocationLineNumHandlers, data => {} }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub InstanceLocationLineNumStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        Start => { handlers => \%LeafHandlers, data => '' },
        End => { handlers => \%LeafHandlers, data => '' }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub BugSummaryStartHandler {
    my ($self, $data, $name, $atts) = @_;

    $data->{$atts->{group}}{$atts->{code}} = {
        bytes => $atts->{bytes},
        count => $atts->{count}
    };

    return (undef, undef);
}

sub MetricStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        Type => { handlers => \%LeafHandlers, data => '' },
        Value => { handlers => \%LeafHandlers, data => '' },
        Location => { handlers => \%MetricLocationHandlers, data => {} }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name)
}

sub MetricLocationStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        SourceFile => { handlers => \%LeafHandlers, data => '' }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub MetricSummariesStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        MetricSummary => { handlers => \%MetricSummaryHandlers, data => {} }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub MetricSummaryStartHandler {
    my ($self, $data, $name, $atts) = @_;

    my %validElements = (
        Type => { handlers => \%LeafHandlers, data => '' },
        Count => { handlers => \%LeafHandlers, data => '' },
        Sum => { handlers => \%LeafHandlers, data => '' },
        SumOfSquares => { handlers => \%LeafHandlers, data => '' },
        Minimum => { handlers => \%LeafHandlers, data => '' },
        Maximum => { handlers => \%LeafHandlers, data => '' },
        Average => { handlers => \%LeafHandlers, data => '' },
        StandardDeviation => { handlers => \%LeafHandlers, data => '' }
    );

    return SwampXML::FindElementAndData($data, \%validElements, $name);
}

sub MetricSummaryEndHandler {
    my ($self, $data, $name) = @_;

    CallCallback($self, $data, $name);
}

sub MetricEndHandler {
    my ($self, $data, $name) = @_;

    if ($data->{Location}) {
        @{$data}{keys %{$data->{Location}}} = values %{$data->{Location}};
        delete $data->{Location};
    }

    CallCallback($self, $data, $name);
}

sub BugSummaryEndHandler {
    my ($self, $data, $name) = @_;

    CallCallback($self, $data, $name);
}

sub BugInstanceEndHandler {
    my ($self, $data, $name) = @_;

    if ($data->{BugTrace}) {
        @{$data}{keys %{$data->{BugTrace}}} = values %{$data->{BugTrace}};
        delete $data->{BugTrace};
    }

    CallCallback($self, $data, $name);
}

1;
