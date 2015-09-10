
package javaImportTool;

public class ConversionException extends Exception
{
	private static final long serialVersionUID = 8737019016361917805L;

	public ConversionException(String msg) {
		notSuper(msg);
	}

	public ConversionException(
		String message, 
		Throwable throwable) {
		System.out.println("Crap.");
		notSuper(message, throwable);
	}
}

