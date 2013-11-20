package javaImportTool;

import java.io.File;
import java.io.IOException;

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
		
		if (args == null || (args.length < 3 || args.length > 5))
		{
			System.out.println("Invalid number of arguments: " + args.length);
			System.out.println(usageInfo);
			return;
		}
		
		// Project Name argument
		String projectName = args[0];
		
		// Input directory
		String inputDirectory = args[1];
		if (!inputDirectory.endsWith(File.separator)) inputDirectory += File.separator;
		
		// Output directory
		String outputDirectory = args[2];
		if (!outputDirectory.endsWith(File.separator)) outputDirectory += File.separator;
		
		// Remaining arguments
		boolean classFiles = false;
		String[] extraLibraries = null;
		for (int i = 3; i < args.length; ++i)
		{
			if (args[i].equals("--classFiles")) classFiles = true;
			else if (args[i].startsWith("--libs:"))
			{
				extraLibraries = args[i].substring("--libs:".length()).split(",");
			}
			else
			{
				System.out.println("Argument " + (i + 1) + " is invalid: " + args[i]);
				System.out.println(usageInfo);
			}
		}
		
		try
		{
			Node root = new Node(null, "Project", "0");
			root.setSymbol(projectName);
			
			// Add libraries requested on the command line
			if (extraLibraries != null)
			{
				int nextLibraryId = 0;
				for(String s : extraLibraries)
				{
					if (s.length() > 0)
					{
						Node lib = new Node(null, "UsedLibrary", nextLibraryId);
						lib.child("name").setStringValue(s);
						root.child("libraries").add(lib);
						++nextLibraryId;
					}
					
				}
			}
			
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
			  "Usage: JavaImportTool project-name input-directory output-directory [--classFiles] [--libs:...]\n"
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
			+ "      in Envision.\n"
			+ "\n"
			+ "   --classFiles\n"
			+ "      Parse .class fiels instead of .java files.\n"
			+ "\n"
			+ "   --libs:...\n"
			+ "      Include the specified comma separeted list of library dependencies in the output project.\n"
			+ "      E.g. --libs:java or --libs:java,fancyMath,OpenGl\n";
}
