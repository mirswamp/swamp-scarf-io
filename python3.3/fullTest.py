#!/usr/bin/python

from HashToScarf import HashToScarf
from ScarfToHash import ScarfToHash

def summary(test):
    print(test)


outputFile = "/p/swamp/home/bklein/scarf/python/testresults.xml" 
inputFile =  "/p/swamp/home/bklein/perl/parsed_results.xml"
#inputFile = "/p/swamp/home/bklein/metrictest.xml"
#inputFile = "/p/swamp/home/bklein/out.xml"

test_writer = HashToScarf(outputFile, 2)

callbacks = {"InitialCallback":test_writer.addStartTag, "MetricCallback":test_writer.addMetric, "BugCallback":test_writer.addBugInstance, "SummaryCallback":summary}

test_reader = ScarfToHash(inputFile, callbacks)

test_reader.parse()

test_writer.addSummary()

test_writer.addEndTag()


