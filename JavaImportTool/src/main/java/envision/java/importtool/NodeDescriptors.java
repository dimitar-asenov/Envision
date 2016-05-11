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
package envision.java.importtool;

import java.util.HashMap;
import java.util.Map;

/**
 * The NodeDescriptors class contains a description of all Envision nodes and their possible child nodes.
 * 
 * This class is used to create all children of a node when that node is newly created. The initializers should not
 * include attributes which are not required.
 */
public class NodeDescriptors {
	
	public static void initialize(Node node, String initializeAsType)
	{
		NodeInitializer initializer = initializerMap.get(initializeAsType);
		if (initializer != null) initializer.initialize(node);
	}
	
	public static boolean isPersistenceUnit(Node node)
	{
		NodeInitializer ni = initializerMap.get(node.tag());
		return ni != null ? ni.isPersistenceUnit() : false;
	}

	private static Map<String, NodeInitializer> initializerMap = new HashMap<String, NodeInitializer> ();
	
	private static void add(NodeInitializer ni)
	{
		initializerMap.put(ni.nodeType(), ni);
	}
	
	static {
		
		// Declarations
		add( new NodeInitializer("Declaration", new String[][] {
				{"NameText","name"},
				{"Modifier","modifiers"},
				{"StatementItemList","annotations"},
				{"TypedListOfDeclaration","subDeclarations"}
		}));
		add( new NodeInitializer("-class-level-container", "Declaration", new String[][] {
				{"TypedListOfClass","classes"},
				{"TypedListOfMethod","methods"},
				{"TypedListOfField","fields"},
		}));
		add( new NodeInitializer("Project", true, "-class-level-container", new String[][]{
				{"TypedListOfProject","projects"},
				{"TypedListOfModule","modules"},
				{"TypedListOfUsedLibrary","libraries"}
		}));
		
		add( new NodeInitializer("Module", true, "-class-level-container", new String[][]{
				{"TypedListOfModule","modules"},
				{"TypedListOfUsedLibrary","libraries"},
				{"Integer","modKind"}
		}));
		add( new NodeInitializer("Class", true, "-class-level-container", new String[][]{
				{"TypedListOfExpression","baseClasses"},
				{"TypedListOfEnumerator","enumerators"},
				{"TypedListOfFormalTypeArgument","typeArguments"},
				{"TypedListOfDeclaration","friends"},
				{"Integer","cKind"}
		}));
		add( new NodeInitializer("Method", true, "Declaration", new String[][]{
				{"StatementItemList","items"},
				{"TypedListOfFormalTypeArgument","typeArguments"},
				{"TypedListOfFormalArgument","arguments"},
				{"TypedListOfFormalResult","results"},
				{"TypedListOfMemberInitializer","memberInitializers"},
				{"TypedListOfExpression","throws"},
				{"Integer","mthKind"}
		}));
		add( new NodeInitializer("VariableDeclaration", "Declaration", new String[][]{
				{"Expression","typeExpression"},
				{"Integer", "initKind"}
		}));
		add( new NodeInitializer("Field", "VariableDeclaration", null));
		add( new NodeInitializer("NameImport", "Declaration", new String[][]{
				{"ReferenceExpression","importedName"},
				{"Boolean","importAll"}
		}));
		
		// Elements
		add( new NodeInitializer("FormalTypeArgument", new String[][]{
				{"NameText","name"}
		}));
		add( new NodeInitializer("FormalArgument", "VariableDeclaration", new String[][]{
				{"Integer","directionInt"}
		}));
		add( new NodeInitializer("FormalResult", new String[][]{
				{"NameText","name"},
				{"Expression","typeExpression"}
		}));
		add( new NodeInitializer("Modifier", "Integer", null));
		add( new NodeInitializer("Enumerator", new String[][]{
				{"NameText","name"}
		}));
		
		// Statements
		add( new NodeInitializer("Block", new String[][]{
				{"StatementItemList","items"}
		}));
		add( new NodeInitializer("ExpressionStatement", new String[][]{
				{"Expression","expression"}
		}));
		add( new NodeInitializer("LoopStatement", new String[][]{
				{"StatementItemList","body"},
				{"Integer","lpKind"}
		}));
		add( new NodeInitializer("ForEachStatement", new String[][]{
				{"NameText","varName"},
				{"Expression","collection"},
				{"StatementItemList","body"}
		}));
		add( new NodeInitializer("IfStatement", new String[][]{
				{"Expression","condition"},
				{"StatementItemList","thenBranch"},
				{"StatementItemList","elseBranch"}
		}));
		add( new NodeInitializer("ReturnStatement", new String[][]{
				{"TypedListOfExpression","values"}
		}));
		add( new NodeInitializer("CaseStatement", new String[][]{
				{"StatementItemList","body"}
		}));
		add( new NodeInitializer("SwitchStatement", new String[][]{
				{"Expression","switchExpression"},
				{"StatementItemList","body"}
		}));
		add( new NodeInitializer("TryCatchFinallyStatement", new String[][]{
				{"StatementItemList","tryBody"},
				{"TypedListOfCatchClause","catchClauses"},
				{"StatementItemList","finallyBody"}
		}));
		add( new NodeInitializer("CatchClause", new String[][]{
				{"StatementItemList","body"}
		}));
		add( new NodeInitializer("AssertStatement", new String[][]{
				{"Expression","expression"},
				{"Integer","aKind"}
		}));
		add( new NodeInitializer("SynchronizedStatement", new String[][]{
				{"Expression","expression"},
				{"StatementItemList","body"}
		}));
		
		// Expressions
		
		add( new NodeInitializer("ReferenceExpression", new String[][]{
				{"OOReference","ref"},
				{"TypedListOfExpression","typeArguments"},
				{"Integer","memKind"}
		}));
		add( new NodeInitializer("CommaExpression", new String[][]{
				{"Expression","left"},
				{"Expression","right"}
		}));
		add( new NodeInitializer("ThrowExpression", new String[][]{
				{"Expression","expr"}
		}));
		add( new NodeInitializer("VariableDeclarationExpression", new String[][]{
				{"VariableDeclaration","decl"}
		}));
		add( new NodeInitializer("BinaryOperation", new String[][]{
				{"Expression","left"},
				{"Expression","right"},
				{"Integer","opr"}
		}));
		add( new NodeInitializer("UnaryOperation", new String[][]{
				{"Expression","operand"},
				{"Integer","opr"}
		}));
		add( new NodeInitializer("NewExpression", new String[][]{
				{"Expression","newType"},
				{"TypedListOfExpression","dimensions"}
		}));
		add( new NodeInitializer("ArrayInitializer", new String[][]{
				{"TypedListOfExpression","values"}
		}));
		add( new NodeInitializer("AssignmentExpression", new String[][]{
				{"Expression","left"},
				{"Expression","right"},
				{"Integer","opr"}
		}));
		add( new NodeInitializer("CastExpression", new String[][]{
				{"Expression","castType"},
				{"Expression","expr"},
				{"Integer","cKind"}
		}));
		add( new NodeInitializer("InstanceOfExpression", new String[][]{
				{"Expression","expr"},
				{"Expression","typeExpression"}
		}));
		add( new NodeInitializer("MethodCallExpression", new String[][]{
				{"Integer","cKind"},
				{"Expression","callee"},
				{"TypedListOfExpression","arguments"}
		}));
		add( new NodeInitializer("ConditionalExpression", new String[][]{
				{"Expression","condition"},
				{"Expression","trueExpression"},
				{"Expression","falseExpression"}
		}));
		add( new NodeInitializer("LambdaExpression", new String[][]{
				{"TypedListOfFormalArgument","arguments"},
				{"TypedListOfFormalResult","results"},
				{"TypedListOfExpression","captures"},
				{"StatementItemList","body"},
				{"Integer","dCaptureType"}
		}));

		// Literals
		add( new NodeInitializer("BooleanLiteral", new String[][]{
				{"Boolean","value"}
		}));
		add( new NodeInitializer("CharacterLiteral", new String[][]{
				{"Text","value"}
		}));
		add( new NodeInitializer("IntegerLiteral", new String[][]{
				{"Text","value"}
		}));
		add( new NodeInitializer("FloatLiteral", new String[][]{
				{"Text","value"}
		}));
		add( new NodeInitializer("StringLiteral", new String[][]{
				{"Text","value"}
		}));
		
		// Type expressions
		add( new NodeInitializer("PrimitiveTypeExpression", new String[][]{
				{"Integer","val"}
		}));
		add( new NodeInitializer("ClassTypeExpression", new String[][]{
				{"ReferenceExpression","typeExpression"}
		}));
		add( new NodeInitializer("ArrayTypeExpression", new String[][]{
				{"Expression","typeExpression"}
		}));
		
		// Primitive types
		add( new NodeInitializer("Boolean", 0));
		add( new NodeInitializer("Character", "x"));
		add( new NodeInitializer("Integer", 0));
		add( new NodeInitializer("Float", 0.0));
		add( new NodeInitializer("Text", ""));
		add( new NodeInitializer("NameText", ""));
		
		// Misc
		add( new NodeInitializer("UsedLibrary", new String[][]{
				{"Text","name"}
		}));
		
		// Comments
		add( new NodeInitializer("CommentStatementItem", new String[][]{
				{"CommentNode","commentNode"}
		}));
		add( new NodeInitializer("CommentNode", new String[][]{
				{"TypedListOfText","lines"},
				{"TypedListOfCommentDiagram","diagrams"},
				{"TypedListOfCommentFreeNode","codes"},
				{"TypedListOfCommentTable","tables"}
		}));
	}
}