package javaSCARF;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;
import javax.xml.stream.events.XMLEvent;

import dataStructures.BugInstance;
import dataStructures.BugSummary;
import dataStructures.BugTrace;
import dataStructures.BugCategory;
import dataStructures.InitialInfo;
import dataStructures.InstanceLocation;
import dataStructures.Location;
import dataStructures.Method;
import dataStructures.Metric;
import dataStructures.MetricSummary;

public class ScarfXmlReader {
	private XMLStreamReader reader;
	private ScarfInterface scarfCallbacks;
	
	public ScarfXmlReader(ScarfInterface s) {
		scarfCallbacks = s;
	}

	private void parse() {
		try { 
			int eventType = reader.getEventType();
			System.out.println("Event type: " + eventType);
			while (reader.hasNext()) {
				if (reader.next() == XMLEvent.START_ELEMENT) {
		        	String elementName = reader.getLocalName();
		        	handleElement(elementName);
		        }
			}
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			System.err.println("Error: XMLStream error");
			e.printStackTrace();
		}
		try {
			reader.close();
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void handleElement(String elementName) {
		System.out.println("Name: " + elementName);
		
		switch(elementName) {
		case Constants.ANALYZER_REPORT:
			InitialInfo info = handleAnalyzerReport();
			System.out.println(info);
			// TODO: pass it to initialCallback
			scarfCallbacks.initialCallback(info);
			break;
		case Constants.BUG_INSTANCE:
			BugInstance bug = handleBugInstance();
			System.out.println(bug);
			// TODO: pass it to bugCallback
			scarfCallbacks.bugCallback(bug);
			break;
		case Constants.METRIC:
			Metric metric = handleMetric();
			System.out.println(metric);
			// TODO: pass it to metricCallback
			scarfCallbacks.metricCallback(metric);
			break;
		case Constants.BUG_SUMMARY:
			BugSummary summary = handleBugSummary();
			System.out.println(summary);
			// TODO: pass it to bugSummaryCallback
			scarfCallbacks.bugSummaryCallback(summary);
			break;
		case Constants.METRIC_SUMMARIES:
			List<MetricSummary> summaries = handleMetricSummaries();
			for (MetricSummary m : summaries) {
				System.out.println(m);
				// TODO: pass it to metricSummaryCallback
				scarfCallbacks.metricSummaryCallback(m);
			}
			break;
		}
	}
	
	private InitialInfo handleAnalyzerReport() {
		String namespace = reader.getNamespaceURI();
		String name = reader.getAttributeValue(namespace, Constants.ANALYZER_REPORT_ATTR_TOOL_NAME);
		String version = reader.getAttributeValue(namespace, Constants.ANALYZER_REPORT_ATTR_TOOL_VERSION);
		String uuid = reader.getAttributeValue(namespace, Constants.ANALYZER_REPORT_ATTR_UUID);
		return new InitialInfo(name, version, uuid);
	}
	
	private String getChars(String endEvent) {
		try {
			int eventType = reader.next();
			while (reader.hasNext()) {
				if (isEndElement(endEvent)) {
					return "";
				}
				if (eventType == XMLEvent.CHARACTERS) {
					return reader.getText();
				}
			}
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return "";
	}
	
	private List<MetricSummary> handleMetricSummaries() {
		List<MetricSummary> list = new ArrayList<>();
		try {
			while (reader.hasNext()) {
				if (reader.next() == XMLEvent.START_ELEMENT && reader.getLocalName().equals(Constants.METRIC_SUMMARY)) {
					list.add(handleMetricSummary());
				}
				else {
					if (isEndElement(Constants.METRIC_SUMMARIES)) {
						break;
					}
				}
			}
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return list;
	}
	
	private MetricSummary handleMetricSummary() {
		MetricSummary ms = new MetricSummary();
		try {
			while (reader.hasNext()) {
				int eventType = reader.next();
				if (eventType == XMLEvent.START_ELEMENT) {
					switch (reader.getLocalName()) {
					case Constants.METRIC_SUMMARY_COUNT:
						ms.setCount(Integer.parseInt(getChars(Constants.METRIC_SUMMARY_COUNT)));
						break;
					case Constants.METRIC_SUMMARY_TYPE:
						ms.setMetricType(getChars(Constants.METRIC_SUMMARY_TYPE));
						break;
					case Constants.METRIC_SUMMARY_SUM:
						ms.setSum(Double.parseDouble(getChars(Constants.METRIC_SUMMARY_SUM)));
						break;
					case Constants.METRIC_SUMMARY_SUM_OF_SQUARES:
						ms.setSumOfSquares(Double.parseDouble(getChars(Constants.METRIC_SUMMARY_SUM_OF_SQUARES)));
						break;
					case Constants.METRIC_SUMMARY_MINIMUM:
						ms.setMin(Double.parseDouble(getChars(Constants.METRIC_SUMMARY_MINIMUM)));
						break;
					case Constants.METRIC_SUMMARY_MAXIMUM:
						ms.setMax(Double.parseDouble(getChars(Constants.METRIC_SUMMARY_MAXIMUM)));
						break;
					case Constants.METRIC_SUMMARY_AVERAGE:
						ms.setAverage(Double.parseDouble(getChars(Constants.METRIC_SUMMARY_AVERAGE)));
						break;
					case Constants.METRIC_SUMMARY_STANDARD_DEVIATION:
						ms.setStdDev(Double.parseDouble(getChars(Constants.METRIC_SUMMARY_STANDARD_DEVIATION)));
						break;
					}
				}
				else {
					if (isEndElement(Constants.METRIC_SUMMARY)) {
						break;
					}
				}
			}
		} catch (NumberFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return ms;
	}
	
	private BugInstance handleBugInstance() {
		String namespace = reader.getNamespaceURI();
		String id = reader.getAttributeValue(namespace, Constants.ID);
		System.out.println("ID: " + id);
		BugInstance bug = new BugInstance(Integer.parseInt(id));
		try {
			while (reader.hasNext()) {
				int eventType = reader.next();
				if (eventType == XMLEvent.START_ELEMENT) {
					String elementName = reader.getLocalName();
					switch (elementName) {
					case Constants.BUG_INSTANCE_CLASS_NAME:
						bug.setClassName(getChars(Constants.BUG_INSTANCE_CLASS_NAME));
						break;
					case Constants.BUG_INSTANCE_METHODS:
						bug.addMethods(handleMethods());
						break;
					case Constants.BUG_INSTANCE_BUG_LOCATIONS:
						bug.addLocations(handleLocations());
						break;
					case Constants.BUG_INSTANCE_BUG_GROUP:
						bug.setBugGroup(getChars(Constants.BUG_INSTANCE_BUG_GROUP));
						break;
					case Constants.BUG_INSTANCE_BUG_RANK:
						bug.setBugRank(getChars(Constants.BUG_INSTANCE_BUG_RANK));
						break;
					case Constants.BUG_INSTANCE_BUG_SEVERITY:
						bug.setBugSeverity(getChars(Constants.BUG_INSTANCE_BUG_SEVERITY));
						break;
					case Constants.BUG_INSTANCE_CWE_ID:
						bug.addCWE(Integer.parseInt(getChars(Constants.BUG_INSTANCE_CWE_ID)));
						break;
					case Constants.BUG_INSTANCE_BUG_MESSAGE:
						bug.setBugMessage(getChars(Constants.BUG_INSTANCE_BUG_MESSAGE));
						break;
					case Constants.BUG_INSTANCE_RESOLUTION_SUGGESTION:
						bug.setResolutionSuggestion(getChars(Constants.BUG_INSTANCE_RESOLUTION_SUGGESTION));
						break;
					case Constants.BUG_INSTANCE_BUG_TRACE:
						bug.setBugTrace(handleBugTrace());
						break;
					case Constants.BUG_INSTANCE_BUG_CODE:
						bug.setBugCode(getChars(Constants.BUG_INSTANCE_BUG_CODE));
						break;
					}
				}
				else {
					if (isEndElement(Constants.BUG_INSTANCE)) {
						break;
					}
				}
			}
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return bug;
	}
	
	private BugTrace handleBugTrace() {
		BugTrace bt = new BugTrace();
		try {
			while (reader.hasNext()) {
				if (reader.next() == XMLEvent.START_ELEMENT) {
					switch (reader.getLocalName()) {
						case Constants.BUG_TRACE_BUILD_ID:
							bt.setBuildID(getChars(Constants.BUG_TRACE_BUILD_ID));
							break;
						case Constants.BUG_TRACE_ASSESSMENT_REPORT_FILE:
							bt.setAssessmentReportFile(Constants.BUG_TRACE_ASSESSMENT_REPORT_FILE);
							break;
						case Constants.BUG_TRACE_INSTANCE_LOCATION:
							bt.setInstanceLocation(handleInstanceLocation());
							break;
					}
				}
				else {
					if (isEndElement(Constants.BUG_INSTANCE_BUG_TRACE)) {
						break;
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return bt;
	}
	
	private InstanceLocation handleInstanceLocation() {
		InstanceLocation il = new InstanceLocation();
		try {
			while (reader.hasNext()) {
				if (reader.next() == XMLEvent.START_ELEMENT) {
					switch (reader.getLocalName()) {
					case Constants.INSTANCE_LOCATION_XPATH:
						il.setXPath(getChars(Constants.INSTANCE_LOCATION_XPATH));
						break;
					case Constants.INSTANCE_LOCATION_LINE_NUM:
						while (reader.hasNext()) {
							if (reader.next() == XMLEvent.START_ELEMENT){
								switch (reader.getLocalName()) {
								case Constants.LINE_NUM_START:
									il.setStartLine(Integer.parseInt(getChars(Constants.INSTANCE_LOCATION_LINE_NUM)));
									break;
								case Constants.LINE_NUM_END:
									il.setEndLine(Integer.parseInt(getChars(Constants.INSTANCE_LOCATION_LINE_NUM)));
									break;
								}
							}
							else {
								if (isEndElement(Constants.INSTANCE_LOCATION_LINE_NUM)) {
									break;
								}
							}
						}
						break;
					}
				}
				else {
					if (isEndElement(Constants.BUG_TRACE_INSTANCE_LOCATION)) {
						break;
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return il;
	}
	
	private BugSummary handleBugSummary() {
		BugSummary summary = new BugSummary();
		try {
			while (reader.hasNext()) {
				if (reader.next() == XMLEvent.START_ELEMENT) {
					if (reader.getLocalName().equals(Constants.BUG_CATEGORY)) {
						summary.add(handleBugCategory());
					}
				}
				else {
					if (isEndElement(Constants.BUG_SUMMARY)) {
						break;
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return summary;
	}
	
	private BugCategory handleBugCategory() {
		String namespace = reader.getNamespaceURI();
		String count = reader.getAttributeValue(namespace, Constants.BUG_CATEGORY_COUNT);
		String byteCount = reader.getAttributeValue(namespace, Constants.BUG_CATEGORY_BYTES);
		String code = reader.getAttributeValue(namespace, Constants.BUG_CATEGORY_CODE);
		String group = reader.getAttributeValue(namespace, Constants.BUG_CATEGORY_GROUP);
		return new BugCategory(Integer.parseInt(count), Integer.parseInt(byteCount), code, group);
	}
	
	private List<Method> handleMethods() {
		List<Method> list = new ArrayList<>();
		try {
			while (reader.hasNext()) {
				int eventType = reader.next();
				if (eventType == XMLEvent.START_ELEMENT && reader.getLocalName().equals(Constants.METHOD)) {
					String namespace = reader.getNamespaceURI();
					String id = reader.getAttributeValue(namespace, Constants.ID);
					String primary = reader.getAttributeValue(namespace, Constants.PRIMARY);
					String name = reader.getElementText();
					Method m = new Method(id, name, Boolean.parseBoolean(primary));
					list.add(m);
				}
				else {
					if (isEndElement(Constants.BUG_INSTANCE_METHODS)) {
						break;
					}
				}
			}
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return list;
	}
	
	private List<Location> handleLocations() {
		List<Location> list = new ArrayList<>();
		try {
			while (reader.hasNext()) {
				int eventType = reader.next();
				if (eventType == XMLEvent.START_ELEMENT && reader.getLocalName().equals(Constants.LOCATION)) {
					Location l = handleLocation();
					list.add(l);
				}
				else {
					if (isEndElement(Constants.BUG_INSTANCE_BUG_LOCATIONS)) {
						break;
					}
				}
			}
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return list;
	}
	
	private Location handleLocation() {
		String namespace = reader.getNamespaceURI();
		String id = reader.getAttributeValue(namespace, Constants.ID);
		String primary = reader.getAttributeValue(namespace, Constants.PRIMARY);
		Location location = new Location(id, Boolean.parseBoolean(primary));
		try {
			while (reader.hasNext()) {
				int eventType = reader.next();
				if (eventType == XMLEvent.START_ELEMENT) {
					switch (reader.getLocalName()) {
					case Constants.LOCATION_SOURCE_FILE:
						location.setSourceFile(getChars(Constants.LOCATION_SOURCE_FILE));
						break;
					case Constants.LOCATION_START_LINE:
						location.setStartLine(Integer.parseInt(getChars(Constants.LOCATION_START_LINE)));
						break;
					case Constants.LOCATION_END_LINE:
						location.setEndLine(Integer.parseInt(getChars(Constants.LOCATION_END_LINE)));
						break;
					case Constants.LOCATION_START_COLUMN:
						location.setStartColumn(Integer.parseInt(getChars(Constants.LOCATION_START_COLUMN)));
						break;
					case Constants.LOCATION_END_COLUMN:
						location.setEndColumn(Integer.parseInt(getChars(Constants.LOCATION_END_COLUMN)));
						break;
					case Constants.LOCATION_EXPLANATION:
						location.setExplanation(getChars(Constants.LOCATION_EXPLANATION));
						break;
					}
				}
				else {
					if (isEndElement(Constants.LOCATION)) {
						break;
					}
				}
			}
		} catch (NumberFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return location;
	}
		
	private Metric handleMetric() {
		Metric metric = new Metric();
		try {
			while (reader.hasNext()) {
				int eventType = reader.next();
				if (eventType == XMLEvent.START_ELEMENT) {
					switch (reader.getLocalName()) {
					case Constants.LOCATION:
						metric.setLocation(handleLocation());
						break;
					case Constants.METRIC_CLASS:
						metric.setClassName(getChars(Constants.METRIC_CLASS));
						break;
					case Constants.METRIC_METHOD:
						metric.setMethodName(getChars(Constants.METRIC_METHOD));
						break;
					case Constants.METRIC_TYPE:
						metric.setMetricType(getChars(Constants.METRIC_TYPE));
						break;
					case Constants.METRIC_VALUE:
						metric.setMetricType(getChars(Constants.METRIC_VALUE));
						break;
					}
				}
				else {
					if (isEndElement(Constants.METRIC)) {
						break;
					}
				}
			}
		} catch (NumberFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return metric;
	}
	private boolean isEndElement(String name) {
		return (reader.getEventType() == XMLEvent.END_ELEMENT) && (reader.getLocalName().equals(name));
	}
	
	public void parseFromFile(File f) {
		try {
			XMLInputFactory factory = XMLInputFactory.newInstance();
			InputStream stream = new FileInputStream(f);
			reader = factory.createXMLStreamReader(stream);
		} catch (Exception e) {
			System.err.println("Error: Unable to open XML stream in specified file");
		}
		parse();
	}
	
	public void parseFromFilepath(String filepath) {
		File f = new File(filepath);
		if (!f.exists()) {
			System.err.println("Error: Invalid filepath");
		}
		else {
			parseFromFile(f);
		}
	}
	
	/*
	public static void main(String[] args) {
		System.out.println("1st arg: " + args[0]);
		ScarfXmlReader r = new ScarfXmlReader();
		r.parseFromFilepath(args[0]);
	}
	*/
	
}
