
package javaImportTool;

public class ConversionException extends Exception
{
	private static final long serialVersionUID = 8737019016361917805L;

	public ConversionException(String message) {
		notSuper(message);
	}

	public ConversionException(String message, Throwable throwable) {
		doStuff();
		this.moreStuff("NOOOOOOOO!");
		notSuper(message, throwable);
		i += think * "that's" + enough();
		ok(stop, "please");
	}
}

