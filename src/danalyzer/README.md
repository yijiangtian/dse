# DANALYZER

The Danalyzer is a //dynamic analysis// tool that instruments the bytecode in a jar file for providing capture and display of symbolic expressions as the instrumented code is run dynamically (in real-time). Running the code dynamically reduces greatly the amount of time required for symbolic analysis, since the code is running only slightly (hopefully) slower than in its normal usage. However, there is currently no path steering performed to try to get the code to execute an area of interest. Instead, it will output path constraint solutions at every branch point (comparison opcodes) that can currently be used to provide changes that the user can attempt to make to steer execution to the desired direction. At a future time this information can be fed to a smart fuzzer to guide it in providing this mechanism in a more automated manner.

## INSTRUMENTATION PHASE

To instrument a jar file, run the following command:

  `java -jar danalyzer.jar TESTFILE.jar`

This will create a new instrumented jar file having the name TESTFILE-dan-ed.jar. The file will, of course, be larger due to the instrumentation, which adds an average of around 3 bytecode instructions per instruction read from the original code, plus additional required libebries (such as the Z3 solver, jConstraint ans ASM) are also added.

If you wish to replace the output of specific uninstrumented methods with symbolic expressions, create a file containing the names of these methods (one per line) and supply this as an additional argument to the above command line as such:

  `java -jar danalyzer.jar TESTFILE.jar symbolic_methods.txt`

In the above example, the contents of the file symbolic_methods.txt can be set to BufferedImage.getRGB to generate a seperate symbolic expression for each of the 2 input values X and Y.
Currently the only supported method is //BufferedImage.getRGB//.

## EXECUTION PHASE

To execute the instrumented jar file, run it as follows:

  `java -Xverify:none -cp lib/commons-io-2.5.jar:lib/asm-all-5.2.jar:lib/com.microsoft.z3.jar:lib/jgraphx.jar:TESTFILE-dan-ed.jar MAINCLASS ARGLIST`

Where:
  **MAINCLASS** is the Main Class of the program under test (TESTFILE.jar)
  **ARGLIST**   is the list of arguments that you normally pass to TESTFILE.jar when you run it.

To define the parameters that are to be symbolized you have 2 methods in which to accomplish this:

### Running with a danfig file

  - This requires defining the list of symbolic expressions in a file named **_danfig_** and located in the directory danalyzer is being executed from.
  - The file must have the following string as the first line of the file: **#! DANALYZER SYMBOLIC EXPRESSION LIST**
  - Entries in this file are defined as **<command>: <value>** with the following commands defined:
  - **Symbolic**: - To define a parameter to be symbolized. Each entry is composed of the following space-separated fields:
    - *ID* is a unique name to identify the symbolic parameter with.
    - *METHOD_NAME* is the full name (Class.Method) plus the argument type list and return type. For instance: collab/EventResultSet.class.add(J)V. Note that the class path uses '/' separators but the method is seperated by a '.'. 
    - *SLOT* is an integer representing the argument index for the method, starting at 0. Passed parameters are counted first, so if a method has 3 arguments passed to it named 'x', 'y', and 'z' and defines 2 int values 'a' and 'b' within it, if you wanted to symbolize the 'y' argument and the 'b' parameter, the INDEX for the first entry would be 1 (for 'y') and the second would be 4 (for 'b').
    - *START* (optional) is the start opcode index range in the method for which the param is valid
    - *END* (optional) is the end opcode index range in the method for which the param is valid
    - *TYPE* the data type for the parameter (I for int, J for long, F for float, D for double)
  - **Constraint**: *FIELDS* - To define a constraint for a symbolic parameter.  Each entry is composed of the following space-separated fields:
    - *ID* is the ID name of the symbolic parameter this entry is associated with.
    - *COMPARE_TYPE* is the type of numeric comparison to make. The allowed values are: *EQ, NE, GT, GE, LT, LE*.
    - *DATA_VALUE* is the value being compared to. 
    - *Example:* The symbolic parameter named 'xprime' list defined is a 32-bit integer and we want it to be constrained to values less than 100. Our entry should then be: //Constraint: xprime LT 100//
  - **DebugFlags**: *FLAG_LIST* - To specify the debug information to be output. The *FLAG_LIST* consists of a comma-separated list of the following values:
    - *WARN*   - Warnings (note that errors will always be enabled)
    - *INFO*   - additional info during opcode command execution
    - *THREAD* - changes in the thread selection
    - *AGENT*  - callbacks from the agent
    - *COMMAND* - the opcode commands executed
    - *SOLVE*  - the output of the constraints solver
    - *CALLS*  - method call and return info
    - *STACK*  - stack pushes and pops for each method
    - *LOCAL*  - local parameters and writes to object and array maps for each method
    - *BRANCH* - branching info
    - *ALL*    - enables all of the above messages
  - **DebugMode**: *DEST* - To specify where the debug information is to be output. The *DEST* consists of one of the following values:
    - *STDOUT*  - (default) output to the terminal (if the program uses stdin for user input, makes it difficult to read)
    - *GUI*     - a text panel os opened up and output is directed to it   (slows the program execution significantly)
    - *UDPPORT* - output is sent out in UDP packets to localhost port 5000 - such as *dandebug* program. (some loss of messages may occur)
    - *TCPPORT* - output is sent out in TCP packets to localhost port 5000 - such as *dandebug* program. (no loss of messages)
  - **TriggerOnCall**:   0 or 1 - Set to trigger debug output when the method specified by *TriggerClass* and *TriggerMethod* occur.
  - **TriggerOnReturn**: 0 or 1 - Set to trigger debug output when returning from the selected method. (Note: *TriggerOnCall* must also be set to 1)
  - **TriggerOnError**:  0 or 1 - Set to trigger debug output when an ERROR message has occurred.
  - **TriggerOnException**: 0 or 1 - Set to trigger debug output when an Exception has occurred.
  - **TriggerAuto**:     0 or 1 - Set to re-trigger on each occurrance of the trigger condition once met. (Note: only applicable if *TriggerRange* is set to 0)
  - **TriggerAnyMeth**:  0 or 1 - Set to trigger on any method of the selected *TriggerClass*.
  - **TriggerClass**: *CLASS_NAME* - The package class name of the method to trigger on for *TriggerOnCall* and *TriggerOnReturn*.
  - **TriggerMethod**: *METHOD_NAME* - The name and signature of the method to trigger on for *TriggerOnCall* and *TriggerOnReturn*. (Ignored if *TriggerAnyMeth* set to 1)
  - **TriggerCount**: *COUNT* - The number of times the specified method must be called prior to triggering. (e.g. 1 indicates the 1st time, 2 means skip the 1st and trigger on the 2nd, etc.)
  - **TriggerRange**: *RANGE* - The number of debug messages to output when the trigger condition occurs. 0 indicates no limit an makes triggering a one-shot process, effectively disabling *TriggerAuto*.

**Note:** The CONSTRAINT section must follow the SYMBOLIC section since it uses the index of the symbolic parameter in that list for the index value in these entries)*

**Note:** Multiple *TriggerOn---* conditions can be set to enable triggering on multiple events. Also, if none of the *TriggerOn---* settings are enabled, the debug output will run immediately.

* These require an external program (such as *dandebug*) to capture the packets and display the results.

### Running from the GUI interface

If the file '*danfig*' is not present in the executed directory, the a GUI interface will be presented to specify symbolics, the thread to run, and debug output conditions.
The initial settings for this panel are read from the *.danalyzer/site.properties* file in the current directory and any changes in the settings will be saved to this file upon closing it. If the file is not found, default values will be used and the file will be created upon exit.

#### MAIN PANEL

This panel provides the ability to select the method parameters you wish to symbolize in the program. It also allows specifying where debug information should be sent.

**Symbolic Parameter Selections section:**
  - *Class* - select the class of the method you wish to symbolize (only instrumented classes are listed).
  - *Method* - select the method you wish to symbolize.
  - *Parameter* specify the index value for the  parameter to symbolize (0 = 'this', 1 = 1st parameter to method, etc).
  - *Add* - press this button to add the above selection to the list.
  - *Clear* - press this button to remove all entries from the list.

**User-added constraints section:**
  - constraint is applied to the selection made in the Symbolic Parameter list.
  - *Comparison* - select the type of comparison the constraint is defining.
  - *Value* - the value of the constraint being applied.
  - *Add* - press this button to add the above selection to the list.
  - *Clear* - press this button to remove all entries from the list.

**Other:**
  - *Save danfig* - allows you to save the current setup to a danfig file to run without a gui interface.
  - *Debug* - this opens another panel for setting up the debug information to capture. (see **DEBUG PANEL** section below)
  - *Run* - closes the gui interface and begins execution of the program.

#### DEBUG PANEL

This panel allows specifying what debug information will be output and allows triggering capabilities to reduce the amount of debug information until specific events have occurred.
 
**Debug Flags section**
  - *Warnings* - warnings (note that errors will always be enabled)
  - *Info*     - additional info during opcode command execution
  - *Commands* - the opcode commands executed
  - *Agent*    - callbacks from the agent
  - *Solver*   - the output of the constraints solver
  - *Call/Return* - method call and return info
  - *Thread*   - changes in the thread selection
  - *Stack*    - stack pushes and pops for each method
  - *Locals*   - local parameters and writes to object and array maps for each method

**Debug Output section**
  - *Debug to stdout* - this specifies that debug information is directed to the console.
  - *Debug to gui* - this specifies that a scrollable text panel be created for receiving the debug information.
  - *Debug to UDP port* - this specifies that the debug information be sent in UDP packets to localhost port 5000.
  - *Debug to TCP port* - this specifies that the debug information be sent in TCP packets to localhost port 5000.

**Triggering section**
  - *on Call* - allows you to specify a class/method selection (instrumented methods only) that will hold off outputting debug information until that method is executed.
  - *on Return* - will also trigger when a return from a method occurs.
  - *on Error* - to hold off debug output until an ERROR message has occurred.
  - *on Exception* - to hold off debug output until an Exception has occurred.
  - *Auto Reset* - will cause the debug output to trigger every time the specified method event occurs (only useful if *Limit* is set to a non-zero value)
  - *Count* - indicates the number of times the specified event must occur before debug messages are enabled. (e.g. 1 indicates the 1st time, 2 means skip the 1st and trigger on the 2nd, etc.)
  - *Limit* - indicates the maximum number of messages that will be output before disabling the messages. This provides the ability to just capture a limited number of messages so the trigger can be re-armed and the event can be captured on each call to a method. Setting the value to 0 causes the output to run continuously once triggered.

**Triggering Method Selection section**
  - *Class* - the class of the method you wish to trigger on.
  - *Method* - the method you wish to trigger on.
  - *Any Method* - will wait until any instrumented method of the specified class is executed.

  - *Exit* - to save settings then close this panel and return to the main panel.
