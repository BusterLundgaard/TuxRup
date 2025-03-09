# Distribution and source code
-[] Figure out how to get pointer locations / identifiers from external debug symbol file
-[] Figure out how well it works downloading debug symbols for various programs (try some of the programs in the list)
-[] Figure out how well it works to downloade the source code
-[] Start working on pipeline that can, given a name/executable:
    -Get the repository link to the source code
    -Download the source code and run ctags, then delete source code again
    -get the debug symbols
    -from a function name, use the tags file and the repository link to retrieve the individual file 

# Functionality of Tuxrup
-[] Revise the hashing system so that it is smarter and less dependent/variable on the path of the widget
-[] Add support for deletion of widgets
-[] Make this deletion permanent by reading a config file 
-[] Add a proper right-click button for what we have so far (deletion and changing source code)
-[] Add rudimentary support for editing properties of a widget, whatever that means or however it'd work
    -Can we use GObject to somehow get a list of all valid properties for this instance of this class?
-[] Add rudimentary support for adding new widgets (maybe just type the name of the type of widgets, then edit it's properties after?)
-[] Implement/integrate the CSS modification
-[] Polish up the source code editing and make it possible to select between different callbacks 
(then with all this done, i think we can sort of consider tuxrup "done" for now? at least for GTK applications written in C)
-[] Make all of these changes permanent!

# Report and going beyond GTK
-[] Figure out which percentage of software uses which UI libraries in which languages
-[] Analyze what function hooks are available in QT and WxWidgets (electron is too different for now)
-[] Analyze what options QT and WxWidgets give you in terms of getting and setting widget properties and CSS
-[] Analyze how the hashing/identification would work 
-[] Analyze (if time) what options just the accessability API would give in terms of overriding functions (what more could be done though, while still being agnostic?)
-[] Describe how a version of Tuxrup supporting multiple different UI Libraries would work: What kind of architechture and "pattern" should we use? What functions would one need to implement for each library to add support?





