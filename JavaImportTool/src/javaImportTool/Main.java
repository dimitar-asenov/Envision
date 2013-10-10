package javaImportTool;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import javaImportTool.Node.OutputFormat;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.filefilter.SuffixFileFilter;
import org.apache.commons.io.filefilter.TrueFileFilter;
import org.eclipse.jdt.core.ToolFactory;
import org.eclipse.jdt.core.dom.AST;
import org.eclipse.jdt.core.dom.ASTParser;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.util.ClassFileBytesDisassembler;
import org.eclipse.jdt.core.util.ClassFormatException;

public class Main {
	
	public static void main(String[] args) {
		
		if (args == null
			|| (args.length < 3 || args.length > 4)
			|| (args.length == 4 && !args[0].equals("--classFiles")))
		{
			System.out.println(usageInfo);
			return;
		}
		
		boolean classFiles = args.length == 4 ;
		int argIndex = classFiles ? 1 : 0;
		
		String projectName = args[argIndex + 0];
		
		String inputDirectory = args[argIndex + 1];
		if (!inputDirectory.endsWith(File.separator)) inputDirectory += File.separator;
		
		String outputDirectory = args[argIndex + 2];
		if (!outputDirectory.endsWith(File.separator)) outputDirectory += File.separator;
		
		try
		{
			Node root = new Node(null, "Project", "0");
			root.setSymbol(projectName);
			
			// Add the default java.lang import
			Node nameImport = root.child("subDeclarations").add(new Node(null, "NameImport", "0"));
			nameImport.child("importedName").child("ref").setStringValue("____NULL____:lang");
			nameImport.child("importAll").setLongValue(1);
			Node prefix = nameImport.child("importedName").add(new Node(null, "ReferenceExpression", "prefix"));
			prefix.child("ref").setStringValue("____NULL____:java");

			File dir = new File(inputDirectory);
			ClassFileBytesDisassembler dis = ToolFactory.createDefaultClassFileBytesDisassembler();
			
			String suffix =  classFiles ? ".class" : ".java";
			for(File file : FileUtils.listFiles(dir, new SuffixFileFilter(suffix), TrueFileFilter.INSTANCE))
			{
				System.out.print("Processing file: " + file.getPath() + "...");
				ASTParser parser = ASTParser.newParser(AST.JLS4);
				
				String source;
				if (classFiles)
					source = dis.disassemble(FileUtils.readFileToByteArray(file), System.getProperty("line.separator"));
				else
					source = FileUtils.readFileToString(file);
				
				parser.setSource(source.toCharArray());
				CompilationUnit unit = (CompilationUnit) parser.createAST(null);
				ASTConverter tl = new ASTConverter(root, source);
				tl.visit(unit);
				System.out.println("Done");
			}
			
			String outputDir = outputDirectory + projectName + File.separator;
			System.out.print("Writing the result to " + outputDir + "...");
				
			root.renderRootTree(outputDir, projectName, OutputFormat.SIMPLE);

			System.out.println("Done\n\nFinished");
			
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ConversionException e) {
			e.printStackTrace();
		} catch (ClassFormatException e) {
			e.printStackTrace();
		}
	}
	
	private static String usageInfo =
			  "Usage: JavaImportTool project-name input-directory output-directory\n"
		  	+ "\n"
			+ "   [--classFiles]\n"
			+ "      Parse .class fiels instead of .java files.\n"
			+ "\n"
			+ "   project-name\n"
			+ "      This is the name of the project being converted. This will be used as a directory and file names\n"
			+ "      that are create inside output-directory.\n"
			+ "\n"
			+ "   input-directory\n"
			+ "      This is the directory that contains all Java sources that should be converted to Envision's XML\n"
			+ "      format. This directory is scanned recursively for all files with the .java extension and each\n"
			+ "       file is processed.\n"
			+ "\n"
			+ "   output-directory\n"
			+ "      This is the directory that will contain the converted result. In this directory a sub-directory\n"
			+ "      specified by the project-name is crated. It will contain the generated XML file to be imported\n"
			+ "      in Envision.\n";
}
