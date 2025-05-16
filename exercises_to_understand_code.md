The fundamental difference between Tuxrup1 and Tuxrup2 (this new overhaul):
In the previous version of Tuxrup, we tried to use function hooking to track a bunch of information about the application we were editing. We tracked which widgets had been added, as well as callback information for these widgets
Since we stored all this data, it was pretty beneficial from a performance standpoint, but it also meant we constantly had to make sure that our version of the application was what the actual application looked like
In a sense, using hashmaps, we kind of made a "model" of the application we were editing
The issue is that this model could easily get out of sync!

In Tuxrup2, we no longer store much information globally at all. Instead, we just recursively look through *everything* (every widget in every window) when we need to. This sounds really bad, but its actually fine, because 99% of the time the user won't be making changes. Besides, the DOM is rarely *that* large, the performance cost is frankly fine.
If there is some information we can't get directly from the DOM, like information about widget callbacks, then we *attach it to the widgets themselves!*. Turns out you can use
g_object_set_data() 
and 
g_object_get_data()
to get and set arbitrary data to a widget! So:
Tuxrup1: Trying to make a toy "model" of the application in question
Tuxrup2: Trying to "augment" the application in question with extra data gathered from function hooking. 

# Exercise 1:
Add and test tuxrup_test3() which makes sure there exists some element with the label "Bangladesh"
Remmember: You'll have to go into tests/test1/test1.c to make sure your test is actually called!

# Exercise 2:
not because it's super important, but more just as an exercise to understand the code, try to identify and fix any memory leaks if you can.
Remmember: Anything dealing with strings will probably have a chance to have some memory leaks.

# Exercise 3:
Look up documentation for the methods g_object_set_data() and g_object_get_data().
Use these (in combination with hooking into g_signal_connect_data) to attach data to any widget with a "click" or "activate" callback (we look only at those for now for simplicity). 
Attach as data the function pointer this widget calls as its callback, and the user_data that was attached (the argument that will be given the function called by the widget).


