package org.continuousassurance.scarf.parser;

import org.continuousassurance.scarf.datastructures.BugInstance;
import org.continuousassurance.scarf.datastructures.BugSummary;
import org.continuousassurance.scarf.datastructures.InitialInfo;
import org.continuousassurance.scarf.datastructures.Metric;
import org.continuousassurance.scarf.datastructures.MetricSummary;

public interface ScarfInterface {
	
	public void initialCallback(InitialInfo initial);
	
	public void bugCallback(BugInstance bug);
	
	public void metricCallback(Metric metric);
	
	public void metricSummaryCallback(MetricSummary metricSum);
	
	public void bugSummaryCallback(BugSummary bugSum);
	
	public void finalCallback();

	/*
	default public void initialCallback(InitialInfo initial) {
		System.out.println(initial);
	}
	
	default public void bugCallback(BugInstance bug) {
		System.out.println(bug);
	}
	
	default public void metricCallback(Metric metric) {
		System.out.println(metric);
	}
	
	default public void metricSummaryCallback(MetricSummary metricSum) {
		System.out.println(metricSum);
	}
	
	default public void bugSummaryCallback(BugSummary bugSum) {
		System.out.println(bugSum);
	}
	
	default public void finalCallback() {
		System.out.println("Final callback");
	}
	*/	
	
}
