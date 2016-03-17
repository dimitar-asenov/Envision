/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/
package javaImportTool;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Map;

import javaImportTool.Node.OutputFormat;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.filefilter.SuffixFileFilter;
import org.apache.commons.io.filefilter.TrueFileFilter;
import org.eclipse.jdt.core.JavaCore;
import org.eclipse.jdt.core.ToolFactory;
import org.eclipse.jdt.core.dom.AST;
import org.eclipse.jdt.core.dom.ASTParser;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.Message;
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
		boolean oneFile = false;
		// Input directory
		String inputDirectory = args[1];
		if (inputDirectory.endsWith(".java"))
			oneFile = true;
		else if (!inputDirectory.endsWith(File.separator))
			inputDirectory += File.separator;
		
		// Output directory
		String outputDirectory = args[2];
		if (!outputDirectory.endsWith(File.separator)) outputDirectory += File.separator;
		
		// Remaining arguments
		boolean classFiles = false;
		String[] extraLibraries = null;
		for (int i = 3; i < args.length; ++i)
		{
			if (args[i].equals("-classFiles")) classFiles = true;
			else if (args[i].startsWith("-libs:"))
			{
				extraLibraries = args[i].substring("-libs:".length()).split(",");
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
			
			// Add implicit objects
			root.add(new Node(null , "ReferenceExpression", "implicitBaseType")).child("ref")
				.setStringValue("____NULL____:Object");
			root.add(new Node(null , "ReferenceExpression", "implicitEnumType")).child("ref")
				.setStringValue("____NULL____:Enum");

			File dir = new File(inputDirectory);
			ClassFileBytesDisassembler dis = ToolFactory.createDefaultClassFileBytesDisassembler();
			
			String suffix =  classFiles ? ".class" : ".java";
			
			Collection<File> files;
			if (oneFile)
			{
				files = new ArrayList<File>();
				files.add(dir);
			}
			else
				files = FileUtils.listFiles(dir, new SuffixFileFilter(suffix), TrueFileFilter.INSTANCE);
			
			for(File file : files)
			{
				if (!PRINT_METHODS) System.out.print("Processing file: " + file.getPath() + "...");
				ASTParser parser = ASTParser.newParser(AST.JLS8);
				
				String source;
				if (classFiles)
					source = dis.disassemble(FileUtils.readFileToByteArray(file), System.getProperty("line.separator"));
				else
					source = FileUtils.readFileToString(file);
				
				parser.setSource(source.toCharArray());
				
				// Ensure compliance with the latest java
				 Map options = JavaCore.getOptions();
				 JavaCore.setComplianceOptions(JavaCore.VERSION_1_7, options);
				 parser.setCompilerOptions(options);
				 
				CompilationUnit unit = (CompilationUnit) parser.createAST(null);
				
				if ( unit.getMessages().length > 0)
				{
					if (!PRINT_METHODS) System.err.println("Errors encountered while parsing.");
					for(Message m : unit.getMessages()) System.err.println(m.getMessage());
				}
				
				ASTConverter tl = new ASTConverter(root, source);
				tl.visit(unit);
				if (!PRINT_METHODS) System.out.println("Done");
			}
			
			System.out.print("Estimating size and arrangement...");
			root.estimatedSize();
			System.out.println("Done");
			
			String outputDir = outputDirectory + projectName + File.separator;
			System.out.print("Writing the result to " + outputDir + "...");
			
			// Create directory if it doesn't already exist
			(new File(outputDir)).mkdirs();
			  
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
	
	public static final boolean PRINT_METHODS = false;
	
	private static String usageInfo =
			  "Usage: JavaImportTool project-name (input-file | input-directory) output-directory [-classFiles] [-libs:...]\n"
		  	+ "\n"
			+ "   project-name\n"
			+ "      This is the name of the project being converted. This will be used as a directory and file names\n"
			+ "      that are created inside output-directory.\n"
			+ "\n"
			+ "   input-file, input-directory\n"
			+ "      This is the single Java source or the directory that contains all Java sources that should be\n"
			+ "	     converted to Envision's own format. If given a directory, it is scanned recursively for all files\n"
			+ "      with the .java extension and each file is processed.\n"
			+ "\n"
			+ "   output-directory\n"
			+ "      This is the directory that will contain the converted result. In this directory a sub-directory\n"
			+ "      specified by the project-name is crated. It will contain the generated text files to be imported\n"
			+ "      in Envision.\n"
			+ "\n"
			+ "   -classFiles\n"
			+ "      Parse .class files instead of .java files.\n"
			+ "\n"
			+ "   -libs:...\n"
			+ "      Include the specified comma separeted list of library dependencies in the output project.\n"
			+ "      E.g. -libs:java or -libs:java,fancyMath,OpenGl\n";
}
