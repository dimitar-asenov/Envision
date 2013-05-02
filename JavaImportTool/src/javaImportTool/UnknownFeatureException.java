package javaImportTool;

public class UnknownFeatureException extends ConversionException {

	private static final long serialVersionUID = -1487628539179326028L;

	public UnknownFeatureException(String message) {
		super(message);
	}

	public UnknownFeatureException(String message, Throwable throwable) {
		super(message, throwable);
	}
}
