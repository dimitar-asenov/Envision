Envision
========

This project aims to develop a next generation cross-platform IDE featuring a visual structured code editor.

Envision is an open-source project using a BSD license. The project is based on the Qt framework.

Development status
------------------

Envision is in its early development stage and can not be used yet for programming. The current version showcases some of the key visualization and interaction concepts for the IDE.

Envision currently runs on Linux and Windows. Development is done in Linux.

### Implemented features
* A core component that finds and loads plug-ins at run time
* A small test framework for testing plug-ins
* An extensible application model which currently supports a subset of Java (no generics, no interfaces)
* Flexible visualizations for elements of the model
* Interaction using a structured editor working directly with the visualizations

The implemented features should be considered a proof of concept only and are not stable at this point.

### Missing features required for basic usability
* Import/export to any programming language
* Support for external libraries

Try it out
----------

Envision is written in C++ and uses some features of the new C++11 standard. Make sure your C++ compiler is up-to-date. Gcc versions 4.6 and newer should be sufficient.

### Using Eclipse (recommended):
1. If you do not have the Qt development version installed, download and install it.
2. Download and install the Qt Integration plug-in for your version of eclipse (32/64 bit).
3. Inside Eclipse, configure Qt so that it knows where to find the development files.
4. Download the sources. The project is structured in separate directories where each one is an Eclipse project.
5. Load all projects into Eclipse. Eclipse/Qt might try to change some project files. Cancel all requested changes.
6. Compile all projects.
7. Choose a project you would like to test and launch it. The default launch configuration will run Envision and will initiate the built-in test for the selected project. Projects such as `OOVisualization`, `OOInteraction`, `ControlFlowVisualization` and `CustomMethodCall` visualize some existing code and are interesting to explore.

### Not using Eclipse:
1. If you do not have the Qt development version installed, download and install it.
2. Download the sources. The project is structured in separate directories and each one is a Qt project.
3. Run `qmake` in all directories.
4. Run `make debug debug-install` in all directories
5. Enter the `DebugBuild` directory and start Envision with one of :
   
    $./Envision -graphicssystem raster --test oointeraction
    $./Envision -graphicssystem raster --test oovisualization
    $./Envision -graphicssystem raster --test controlflowvisualization
    $./Envision -graphicssystem raster --test custommethodcall

### What to explore
Running the tests for the `OOVisualization`, `OOInteraction` projects will display some code in a few methods. You might want to explore the visualizations and try interacting with the various elements.

Launching the `ControlFlowVisualization` project displays two methods with some non-trivial control flow graph. Selecting a method and pressing `F2` will switch the visualization between the standard and the control flow representation.

Launching the `CustomMethodCall` project shows a very simple class that defines a few methods. On the right the methods are called and one can see that each method call is visualized in a non-standard way that is dependent on the called method.