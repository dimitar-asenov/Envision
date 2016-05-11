package envision.java.importtool;

import java.util.LinkedList;
import java.util.List;

/**
 * This class has methods which estimate the size of various nodes, when they are rendered. The size is the width and
 * height in pixels and is used to compute the initial arrangement of methods/classes/modules/projects within their
 * parent.
 *
 */
public class SizeEstimator {
	public static final float TEXT_SIZE_IN_PIXELS = 15;
	public static final float HEIGHT_TO_WIDTH_RATIO = 3.0f / 2.0f;
	public static final float TEXT_HEIGHT = TEXT_SIZE_IN_PIXELS;
	public static final float TEXT_WIDTH = TEXT_HEIGHT/HEIGHT_TO_WIDTH_RATIO;
	
	public static final float MIN_WIDTH = TEXT_WIDTH;
	public static final float MIN_HEIGHT = TEXT_HEIGHT;
	
	public static final Size ICON_SIZE = new Size(24.0f, 24.0f);
	
	public static final String POSITION_ATTRIBUTE_X = "_ext_Position_x";
	public static final String POSITION_ATTRIBUTE_Y = "_ext_Position_y";
	
	public static class Size
	{
		public float width = MIN_WIDTH;
		public float height = MIN_HEIGHT;
		
		public Size(float width, float height) {this.width = width; this.height = height;}
		public Size(Size other) {this.width = other.width; this.height = other.height;}
		public Size(){}
		
		public void addHorizontally(Size size) throws ConversionException
		{
			width += size.width;
			if (size.height > height) height = size.height;
		}
		
		public void addVertically(Size size) throws ConversionException
		{
			height += size.height;
			if (size.width > width) width = size.width;
		}
		
		public void addHorizontally(Node node) throws ConversionException
		{
			if (node == null) return;
			addHorizontally(node.estimatedSize());
		}
		
		public void addVertically(Node node) throws ConversionException
		{
			if (node == null) return;
			addVertically(node.estimatedSize());
		}
	}
	
	public static Size estimateSize(Node node) throws ConversionException
	{
		if (node.tag().equals("OOReference"))
			return new Size((node.valueAsText().length() - "S_____NULL____:".length())*TEXT_WIDTH, TEXT_HEIGHT);
			
		if (node.hasValue()) return new Size((node.valueAsText().length()-"X_".length())*TEXT_WIDTH, TEXT_HEIGHT);
		
		if (node.tag().equals("NullLiteral")
				|| node.tag().equals("BreakStatement")
				|| node.tag().equals("ContinueStatement"))
			 return new Size(10*TEXT_WIDTH, TEXT_HEIGHT);
		
		if (node.tag().equals("EmptyExpression"))
			 return new Size(TEXT_WIDTH, TEXT_HEIGHT);
		
		if (node.tag().endsWith("Literal")) return node.child("value").estimatedSize();
		
		if (node.tag().equals("LoopStatement")
				|| node.tag().equals("ForEachStatement")
				|| node.tag().equals("SwitchStatement")
				|| node.tag().equals("CaseStatement"))
			return arrangeWithBody(node, "body");
		
		if (node.tag().equals("IfStatement")) return arrangeIf(node);
				
		if (node.tag().endsWith("Expression")
				|| node.tag().endsWith("ArrayInitializer")
				|| node.tag().endsWith("Operation")
				|| node.tag().equals("FormalResult"))
			return arrange(Direction.Horizontally, 0, node.children());
		if (node.tag().endsWith("CatchClause")
				|| node.tag().equals("StatementItemList")
				|| node.tag().equals("TypedListOfCatchClause")
				|| (node.tag().endsWith("Statement") && !node.tag().startsWith("TypedList"))
				|| node.tag().equals("FormalArgument")
				|| node.tag().equals("FormalTypeArgument"))
			return arrange(Direction.Vertically, 1, node.children());
		if (node.tag().equals("NameImport")
				|| node.tag().equals("Field")
				|| node.tag().equals("VariableDeclaration"))
			return arrange(Direction.Horizontally, 0, node.children());
		if (node.tag().equals("Method")) return arrangeMethod(node);
		if (node.tag().equals("Class")) return arrangeClass(node);
		if (node.tag().equals("Module")) return arrangeModule(node);
		if (node.tag().equals("Project")) return arrangeProject(node);
		
		return arrange(Direction.Horizontally, 0, node.children());
	}
	
	public enum Direction {Horizontally, Vertically}
	public static Size arrange(Direction d, int spaceBetween, List<Node> nodes) throws ConversionException
	{
		Size size = new Size(0, 0);
		boolean passTheFirstNode = false;
		for(Node n : nodes)
		{
			if (d == Direction.Horizontally)
			{	
				size.addHorizontally( n );
				if (passTheFirstNode) size.width += spaceBetween; 
			}
			else
			{
				size.addVertically( n );
				if (passTheFirstNode) size.height += spaceBetween; 
			}
				
			passTheFirstNode = true;
		}
		
		return size;
	}
	
	public static Size arrangeWithBody(Node nodeWithBody, String bodyName) throws ConversionException
	{
		Size size = new Size(ICON_SIZE);
		for(Node child : nodeWithBody.children())
			if (!child.name().equals(bodyName))
				size.addHorizontally(child);

		size.addVertically(nodeWithBody.child(bodyName));
		
		size.height += 20; // Compensation for empty space in header/footer
		return size;
	}
	
	public static Size arrangeIf(Node ifs) throws ConversionException
	{
		Size size = new Size(ICON_SIZE);
		size.addHorizontally(ifs.childOrNull("condition"));
		size.addVertically(ifs.childOrNull("thenBranch"));
		size.addVertically(ifs.childOrNull("elseBranch"));
		
		size.height += 20; // Compensation for empty space in header/footer
		return size;
	}
	
	public static Size arrangeMethod(Node me) throws ConversionException
	{
		Size size = new Size(ICON_SIZE);
		size.addHorizontally(me.childOrNull("name"));
		size.addHorizontally(me.childOrNull("typeArguments"));
		size.addHorizontally(me.childOrNull("arguments"));
		size.addHorizontally(me.childOrNull("results"));

		size.addVertically(me.childOrNull("annotations"));
		size.addVertically(me.childOrNull("subDeclarations"));
		size.addVertically(me.childOrNull("memberInitializers"));
		size.addVertically(me.childOrNull("items"));
		
		size.height += 35; // Compensation for empty space, lines etc.
		return size;
	}
	
	public static Size arrangeClass(Node cl) throws ConversionException
	{
		Size size = new Size(ICON_SIZE);
		size.addHorizontally(cl.childOrNull("name"));
		size.addHorizontally(cl.childOrNull("baseClasses"));
		size.addHorizontally(cl.childOrNull("typeArguments"));
		
		size.addVertically(cl.childOrNull("annotations"));
		size.addVertically(cl.childOrNull("subDeclarations"));
		size.addVertically(cl.childOrNull("enumerators"));
		size.addVertically(cl.childOrNull("friends"));
		
		// Arrange body items
		List<Node> bodyNodes = new LinkedList<Node>();
		Node bodyElement = cl.childOrNull("classes");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		bodyElement = cl.childOrNull("methods");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		
		Size bodySize = arrangeNodesInGrid(bodyNodes, Major.ColumnMajor);
		size.addVertically(bodySize);
	
		size.height += 35; // Compensation for empty space, lines etc.
		return size;
	}
	
	public static Size arrangeModule(Node mo) throws ConversionException
	{
		Size size = new Size(ICON_SIZE);
		size.addHorizontally(mo.childOrNull("name"));		
		size.addVertically(mo.childOrNull("annotations"));
		size.addVertically(mo.childOrNull("subDeclarations"));
		
		// Arrange body items
		List<Node> bodyNodes = new LinkedList<Node>();
		Node bodyElement = mo.childOrNull("modules");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		bodyElement = mo.childOrNull("classes");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		bodyElement = mo.childOrNull("methods");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		
		Size bodySize = arrangeNodesInGrid(bodyNodes, Major.ColumnMajor);
		size.addVertically(bodySize);
		
		size.height += 35; // Compensation for empty space, lines etc.
		return size;
	}
	
	public static Size arrangeProject(Node pr) throws ConversionException
	{
		Size size = new Size(ICON_SIZE);
		size.addHorizontally(pr.childOrNull("name"));	
		size.addVertically(pr.childOrNull("annotations"));
		size.addVertically(pr.childOrNull("subDeclarations"));
		
		// Arrange body items
		List<Node> bodyNodes = new LinkedList<Node>();
		Node bodyElement = pr.childOrNull("projects");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		bodyElement = pr.childOrNull("modules");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		bodyElement = pr.childOrNull("classes");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());
		bodyElement = pr.childOrNull("methods");
		if (bodyElement != null) bodyNodes.addAll(bodyElement.children());

		Size bodySize = arrangeNodesInGrid(bodyNodes, Major.ColumnMajor);
		size.addVertically(bodySize);
		
		size.height += 35; // Compensation for empty space, lines etc.
		return size;
	}
	
	public enum Major {ColumnMajor, RowMajor}
	public static Size arrangeNodesInGrid(List<Node> nodes, Major major) throws ConversionException
	{
		// Get average width/height
		float averageWidth = 0;
		float averageHeight = 0;
		for (Node n : nodes)
		{			
			averageWidth += n.estimatedSize().width;
			averageHeight += n.estimatedSize().height;
		}
		averageWidth /= nodes.size();
		averageHeight /= nodes.size();
		
		// Get 'optimal' number of major/minor Elements to achieve a square
		float prevRatio = 0;
		int minorElements = 1;
		for (int majorElements = 1; majorElements<=nodes.size(); ++majorElements)
		{
			minorElements = (nodes.size()/majorElements) + ((nodes.size() % majorElements) > 0 ? 1:0);
			
			float ratio;
			if (major == Major.ColumnMajor)
				ratio = (averageWidth*majorElements) / (averageHeight*minorElements);
			else
				ratio = (averageWidth*minorElements) / (averageHeight*majorElements);

			if (ratio > 1) ratio = 1/ratio;

			if (ratio > prevRatio) prevRatio = ratio;
			else
			{
				if (majorElements > 1) --majorElements;
				minorElements = (nodes.size()/majorElements) + ((nodes.size() % majorElements) > 0 ? 1:0);
				break;
			}
		}
		
		float minorLengthLimit = minorElements*(major == Major.ColumnMajor ? averageHeight : averageWidth);
		
		int x = 0;
		int y = 0;
		Size finalSize = new Size(0,0);
		float maxMajor = 0;
		float lengthSoFar = 0;
		
		for (Node child : nodes)
		{
			// Set the position of the item
			Node posX = new Node(child, "Integer", POSITION_ATTRIBUTE_X);
			posX.setLongValue(x);
			Node posY = new Node(child, "Integer", POSITION_ATTRIBUTE_Y);
			posY.setLongValue(y);
			child.add(posX);
			child.add(posY);
			
			Size childSize = child.estimatedSize();
			if (major == Major.ColumnMajor )
			{
				lengthSoFar += childSize.height;
				if (childSize.width > maxMajor) maxMajor = childSize.width;
				
				if (lengthSoFar < minorLengthLimit) ++y;
				else
				{
					finalSize.width += maxMajor;
					maxMajor = 0;
					if (finalSize.height < lengthSoFar) finalSize.height = lengthSoFar;
					lengthSoFar = 0;
					
					y = 0;
					++x;
				}
			}
			else
			{
				lengthSoFar += childSize.width;
				if (childSize.height > maxMajor) maxMajor = childSize.height;
				
				if (lengthSoFar < minorLengthLimit) ++x;
				else
				{
					finalSize.height += maxMajor;
					maxMajor = 0;
					if (finalSize.width < lengthSoFar) finalSize.width = lengthSoFar;
					lengthSoFar = 0;
					
					x = 0;
					++y;
				}
			}
		}
		
		return finalSize;
	}
}
