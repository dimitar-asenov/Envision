
package javaImportTool;

public class ConversionException extends Exception
{
	private static final long serialVersionUID = 8737019016361917805L;

	public ConversionException(String message) {
	}

	public ConversionException(String message, Throwable throwable) {
		notSuper(message, throwable);
	}
}

