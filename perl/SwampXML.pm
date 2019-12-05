#!/usr/bin/perl -w

package SwampXML;
use strict;
use Data::Dumper;

my @hd = qw/handlers data/;

sub StartHandler {
    my ($self, $expat, $name, %atts) = @_;

    my ($handlers, $data) = @{$self->{stack}[-1]}{@hd};
    if (defined $handlers->{start}) {
        my ($newHandlers, $newData) = $handlers->{start}($self, $data, $name, \%atts);
        push @{$self->{stack}}, { handlers => $newHandlers, data => $newData };
    }
}

sub EndHandler {
    my ($self, $expat, $name) = @_;

    my ($handlers, $data) = @{$self->{stack}[-1]}{@hd};
    if (defined $handlers->{end}) {
        $handlers->{end}($self, $data, $name);
    }

    pop @{$self->{stack}};
}

sub CharHandler {
    my ($self, $expat, $string) = @_;

    if (@{$self->{stack}}) {
        my ($handlers, $data) = @{$self->{stack}[-1]}{@hd};
        if (defined $handlers->{char}) {
            $handlers->{char}($data, $string);
        }
    }
}

sub DefaultHandler {
    # Throw everything away
}

sub FindElementAndData {
    my ($data, $validElements, $name) = @_;

    if ($validElements->{$name}) {
        my ($newHandlers, $newData) = @{$validElements->{$name}}{@hd};
        $data->{$name} = $newData;
        $newData = \$data->{$name} if ref($newData) eq '';
        return ($newHandlers, $newData);
    }
}

sub FindElementNoData {
    my ($data, $validElements, $name) = @_;

    if ($validElements->{$name}) {
        my ($newHandlers, $newData) = @{$validElements->{$name}}{@hd};
        return ($newHandlers, $newData);
    }
}

sub LeafCharHandler {
    my ($data, $s) = @_;
    $$data .= $s;
}

1;