package dataStructures;

public class MetricSummary {
	private String metricType;
	private int count;
	private double sum;
	private double sumOfSquares;
	private double min;
	private double max;
	private double average;
	private double stdDev;
	
	public MetricSummary() {
		metricType = "";
		count = 0;
		sum = 0.0;
		sumOfSquares = 0.0;
		min = 0.0;
		max = 0.0;
		average = 0.0;
		stdDev = 0.0;
	}
	
	public void setMetricType(String type) {
		metricType = type;
	}
	
	public String getMetricType() {
		return metricType;
	}
	
	public void setCount(int cnt) {
		count = cnt;
	}
	
	public int getCount() {
		return count;
	}
	
	public void setSum(double s) {
		sum = s;
	}
	
	public double getSum() {
		return sum;
	}
	
	public void setSumOfSquares(double ss) {
		sumOfSquares = ss;
	}
	
	public double getSumOfSquares() {
		return sumOfSquares;
	}
	
	public void setMin(double m) {
		min = m;
	}
	
	public double getMin() {
		return min;
	}
	
	public void setMax(double m) {
		max = m;
	}
	
	public double getMax() {
		return max;
	}
	
	public void setAverage(double a) {
		average = a;
	}
	
	public double getAverage() {
		return average;
	}
	
	public void setStdDev(double s) {
		stdDev = s;
	}
	
	public double getStdDev() {
		return stdDev;
	}
	
	@Override
	public String toString() {
		String result = "Metric type: " + metricType + "\n";
		result += "Count: " + count + "\n";
		result += "Sum: " + sum + "\n";
		result += "Sum of Squares: " + sumOfSquares + "\n";
		result += "Min: " + min + "\n";
		result += "Max: " + max + "\n";
		result += "Average: " + average + "\n";
		result += "Standard Deviation: " + stdDev + "\n";
		return result;
	}
	
}
