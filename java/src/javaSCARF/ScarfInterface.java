package javaSCARF;

import dataStructures.BugInstance;
import dataStructures.BugSummary;
import dataStructures.InitialInfo;
import dataStructures.Metric;
import dataStructures.MetricSummary;

public interface ScarfInterface {
	
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
	
	
}