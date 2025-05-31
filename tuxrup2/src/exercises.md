# exercise 1:
It seems that when we launch tuxrup right now, the Tuxrup window appears completely blank. Find out where we initialize the window, and fix the bug

# exercise 2:
When we try to change/inspect widget properties right now, nothing happens, we can see the properties, but changing them doesnt seem to ever do anything (it doesnt actually modify anything about the selected widget).
Fix the bug

# exercise 3:
When we dont have any widget selected right now and try to change its CSS (we click done), the program seems to crash.
Make the code safer to prevent that from happening.

# exercise 4:
Very chill: Just move the function read_file() currently in callbacks.c into util.c instead. Make sure that your code actually compiles without error!

# exercise 5:
This current version of Tuxrup doesn't support selecting and editing GtkLabels. Add support for that.


# use this to make it able to build:
unset GTK_PATH

# single line command for building test app and tuxrup, then running it.
cd ../../tests/test1 && make && cd ../../tuxrup2/build && make build && make