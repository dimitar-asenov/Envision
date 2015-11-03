
if type(target) is IfStatement:
    parent = target.parent
    parent.beginModification("If to while")
    
    condition = target.condition
    thenBranch = target.thenBranch
    
    target.replaceChild(condition, Node.createNewNode("Expression", None))
    target.replaceChild(thenBranch, Node.createNewNode("StatementItemList", None))

    whileNode = Node.createNewNode("LoopStatement", None)
    whileNode.condition = condition
    whileNode.body = thenBranch
    
    parent.replaceChild(target, whileNode)
    parent.endModification()
    
else:
    print("Command only works on if statments")
