# What is Tuxrup, and how do you use it?
Tuxrup is (meant to be) a program that allows you to modify various elements of GTK programs while they run.

Tuxrup is delivered as shared library. For GTK2-3 programs, use `libtuxrup_gtk3.so`, for GTK4, use `libtuxrup_gtk4`. To use Tuxrup on, say, Inkscape, find the path for the inkscape executable (run `which inkscape`), then set the `LD_PRELOAD` environment variable when launching inkscape, like so:

`LD_PRELOAD="./libtuxrup_gtk3.so" /usr/bin/inkscape`

Currently, if you don't use it on our specific testing "Hello world" program in `gtk_program_src/hw`, Tuxrup does more or less nothing outside of printing a few console messages. But as it stands already, there's a very solid (i think, potentially) foundation built here to start implementing real features!


# So how does this LD_PRELOAD thing work?
Suppose there was some library that a lot of Linux programs used, maybe a library implementing a linked list or something, say `liblinkedlist`. If both `A.c` and `B.c` use this library, we could have them *both* just include it in their actual executable files when compiling (more or less just copy-pasting the code from `liblinkedlist` into `A.c` and `B.c` before compiling), but this would waste space. Then you might end up with `liblinkedlist` stored a hundred times on your PC, all with slightly different versions. 

What we do instead is that we *link* `A.c` and `B.c` to `liblinkedlist.so` when compiling. If `A.c` is in your working directory, and `liblinkedlist.so´`is in `/usr/local/lib`, you would compile with

`gcc A.c -o A -L/usr/local/lib -llinkedlist`

The syntax here is a bit silly and strange: `gcc` expects any shared library file to be prefixed with "lib", so if your shared library is named `libsome_stupid_name.so`, you should add `-lsome_stupid_name.so` to the command.

When we now launch the executable `A`, our program scans our pc for all the functions, variables and definitions it needs from `liblinkedlist.so`. Since we've linked to `/usr/local/lib`, it of course starts by scanning that folder. If `liblinkedlist.so` includes some function `void foo()`, then the launcher picks the first implementation of `foo` it can find in `/usr/local/lib` and other certain pre-set folders. This implementation will very most likely be in `liblinkedlist.so`, but there may be naming conflicts, and the `void foo()` from another library would be found first! The order that the files are looked at **matters**! And with `LD_PRELOAD`, we can force that order to pick a certain file as the very first one it scans! So if we wanna modify `A`, and `A` calls `foo`, we can make a shared library that implements `foo`, and when `A` calls out to `foo` from `liblinkedlist.so`, it'll actually just call *our* implementation of `foo` instead!

Let's see what options we have for a GTK application. Running `nm -D program` reads a program file and gets a list of all identifiers (variables, functions) that need to be found dynamically (hence the `-D`) in shared libraries. We compile our simple `hello_world.c` program out to the executable `hw`:

`gcc -g $(shell pkg-config --cflags gtk4) hello_world.c -o hw $(shell pkg-config --libs gtk4) -lelf -ldl`

We now run `nm -D hw` and get:

```bash
    w __cxa_finalize@GLIBC_2.2.5
    U g_application_get_type
    U g_application_run
    w __gmon_start__
    U g_object_unref
    U g_print
    U g_signal_connect_data
    U gtk_application_new
    U gtk_application_window_new
    U gtk_box_append
    U gtk_box_get_type
    U gtk_box_new
    U gtk_button_new_with_label
    U gtk_widget_set_halign
    U gtk_widget_set_name
    U gtk_widget_set_valign
    U gtk_window_get_type
    U gtk_window_present
    U gtk_window_set_child
    U gtk_window_set_default_size
    U gtk_window_set_title
    U g_type_check_instance_cast
    w _ITM_deregisterTMCloneTable
    w _ITM_registerTMCloneTable
    U __libc_start_main@GLIBC_2.34
```

A few options here stick out. We can that `gtk_application_new` more or less has to be called from the very start of the program:

```C hello_world.c:
int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    
    // ....
}
```

We check out the documentation of `gtk_application_new` and see that it has the signature

```C 
GtkApplication* gtk_application_new(const char *application_id, GApplicationFlags flags)
```

So we can make a new C file `oursharedlib.c`, put into it an implementation

```C oursharedlib.c:
#include <stdio.h>
#include <gtk/gtk.h>

GtkApplication* gtk_application_new(const char *application_id, GApplicationFlags flags){
    printf("hello from my shared library!\n");
    return NULL;
}
```
then compile it to a shared library with 

`gcc -shared -fPIC oursharedlib.c -o oursharedlib.so` 

and finally launch `hw` with this modified `gtk_application_new` by running

`LD_PRELOAD="./oursharedlib.so" ./hw`

What should happen now is that the console will print "hello from my shared library!", then a bunch of error messages. This is because we return null, and the original `gtk_application_new` is never called, so we never really create an application. This is a problem!

Luckily it is possible to tell the program launcher to dynamically, at runtime, look for the next implementation of *gtk_application_new* and give us it's function pointer:

```C oursharedlib.c:
#include <stdio.h>
#include <gtk/gtk.h>

// we need this library to look for the next implementation
#include <dlfcn.h> 

// give a name to the type of function pointer we will retrieve
typedef GtkApplication* (*gtk_application_new_t)(const char* application_id, GApplicationFlags flags); 

// define a variable we will use to store the function pointer
static gtk_application_new_t original_gtk_application_new = NULL;

GtkApplication* gtk_application_new(const char *application_id, GApplicationFlags flags){
    //check if we've already found it before
    if (!original_gtk_application_new) 
    {
        //retrieve the pointer with dlsym
        original_gtk_application_new = (gtk_application_new_t)dlsym(RTLD_NEXT, "gtk_application_new");

        //test whether or not you actually found it
        if (!original_gtk_application_new)
        {
            fprintf(stderr, "Error finding original gtk_application_new: %s\n", dlerror());
            return 0;
        }
    }
    
    // Call your own silly code here
    printf("hello from my shared library!\n");

    // Call the original function here and return it's result
    return original_gtk_application_new(application_id, flags);
}
```

Now compiling `oursharedlib.c` and running `./hw` again with `LD_PRELOAD` should work fine! You'll both see "hello from my shared library!" and the program will launch as usual.


# What GTK functions do we override?
For this, look inside `tuxrup/src/intercept_gtk_functions.c`. We override three functions:

```C
// The very first function GTK calls that we can actually override. We use this to call our own "init" / starting function
int 
g_application_run(
GApplication* application,
int argc,
char** argv);

// This is the last function called by the original program before the application is actually shown to the viewer. We can assume here that the DOM has been built, and will now be presented
void 
gtk_window_present(
GtkWindow *window);

// This is the really good one. This function is called any time the widget "instance" adds a callback on the action "detailed_signal" (for example, detailed_signal="clicked") to call the function at "c_handler" (so c_handler=function pointer to some function that will be called). So we can read and pick up every signal event on every signal widget by just adding them to some database of our own any time one is added!
gulong
g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags);
```

Currently, we've hardcoded a list of "types" of actions we'll store for potential modification in `globals.c`:

```C globals.c
const char *remapable_events[MAPPABLE_ACTIONS_LEN] = {"clicked"};
```

... so right now we only work with remapping "clicked" actions on widgets. Later we might add extra actions like "mouseenter", "mouseleave", right clicks, ect. We maintain a hashtable `widget_callback_table` that maps from (widget, callback_name) to the `callback_info` struct:

```C callback_map.c
typedef void (*callback_type)(GtkWidget*, gpointer);

typedef struct {
    gpointer widget;
    gchar* callback_name;
} callback_identifier;

typedef struct {
    // info about original function
    callback_type original_function_pointer;
    gchar* function_name;

    // info about function we've replaced it with
    void* dl_handle;
    void** identifier_pointers;
    int identifier_pointers_n;
} callback_info;
```

So if you have some `GtkWidget* my_button` and you want info about its clicked action, then look up `widget_callback_table` for `{my_button, "clicked"}`. We store the function pointer to it's callback function, the name of this function in the code (for example "button_A_callback" as it's named in `hello_world.c`) as well as some extra information i'll get to later. 


# Okay, that's great, but how do we override the functionality of a button?
It's quite simple actually! We just use the very same GTK functions used to remove and add callbacks! So to change the "clicked" callback on `button_A`, we´d call 

```C
g_signal_handlers_disconnect_matched(
        button_A, 
        G_SIGNAL_MATCH_ID, 
        g_signal_lookup("clicked", G_OBJECT_TYPE(w)), 
        0, 
        NULL, 
        NULL, 
        NULL);  
```

then we call `g_signal_connect_data()` with a pointer to our new modified function `foo`, whereever it may be. We don't need to do anything sketchy like jump instructions or similar. We really don't need to "hack" the application, we just use the completely safe GTK functions we've already been given!


# But hold up: Where do you get that reference to button_A?
We might get it from very many places in fact! Remmember, we hook onto every call initially made by the application to `g_signal_connect_data`, and this call gives us (among other things) pointers to the widgets that are being connected. Another approach is to use `gtk_window_present` which gives a pointer to the root node of the DOM. Then one could look through every single widget recursively and search for every button with a "click" action, for example. For now, the widgets themselves are stored as the keys of `widget_callback_table`, so if you want to access them all you can just iterate through this map. 

Right now, any time a "clicked" callback is added to some widget, we also give it a right click action which opens up the code-modification menu:

```C gtk_events.c
gulong
on_g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){

    /// ... some extra stuff here

    if(!g_hash_table_contains(widget_hashes, instance)){
        g_hash_table_insert(widget_hashes, instance, NULL);
        normal_g_signal_connect_data(instance, "notify::root", G_CALLBACK(on_added_to_dom), NULL, ((void*)0), (GConnectFlags)0);

        // if event name is one of the types of events we look at like "clicked"
        if(is_mappable_action(detailed_signal)){
            // then add right click callback to "on_right_click_gtk4"
            GtkGesture* gesture = gtk_gesture_click_new();
            gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3);
            gtk_widget_add_controller(GTK_WIDGET(instance), GTK_EVENT_CONTROLLER(gesture));
            normal_g_signal_connect_data(gesture, "pressed", G_CALLBACK(on_right_click_gtk4), instance, NULL, (GConnectFlags)0);
        }
    }

    return 0;
}
```


# So what functions do we replace these events with? How can the user generate those functions *on the fly* in a way where we can call them and get their function pointers?
That's where we see another used of shared libraries on linux: Dynamically loading code! If you have some shared library file `sharedlib.so`, you can load it in at any point in your code with just a path to the file like:

```C
void* shared_lib_handle = dlopen("path/to/sharedlib.so", RTLD_LAZY);
```

`dlopen` gives you a `void*` that serves as kind of an "entry adress" into that shared library. You can get a specific function `void foo(int num, char charachter)`  from this library by using `dlsym` just like before:

```C
void (*some_function_type)(int, char);
some_function_type some_function = (some_function_type) dlsym(shared_lib_handle, "foo");
//boom! now you can call it:
some_function(20, 'a');
```

So the overall process in Tuxrup is this: The user writes some code for a new callback functionality overwriting function `my_callback` for some widget `w`. Tuxrup takes this code and writes it to a C file it then compiles to a shared library `some_sharedlib.so`. We store the `dlopen` handle to this newly created shared library and the name of the function being overridden in `widget_callback_table`. We then remove the existing callback on `w`, and replace it with a callback to the so-called "function dispatcher":

```C modify_callback.c
// .... 
remove_callback(args->modified_widget, args->callback_name);
normal_g_signal_connect_data(
    args->modified_widget, //this is just the widget we're modifying

    args->callback_name, //callback_name is type of callback, like "clicked"

    G_CALLBACK(function_dispatcher), 

    args->callback_name, //this is the argument that will be sent to "data" parameter. We give it the callback_name for now, as this together with the widget pointer is all the function dispatcher needs to know for now.  

    NULL, (GConnectFlags)0 //ignore these two last parameters, aren´t important
    );
// ... 
```

```C callback_map.h
typedef void (*callback_type)(GtkWidget*, gpointer);
```

```C function_dispatcher.c
void function_dispatcher(GtkWidget* widget, gpointer data){
    char* callback_name = (char*)data

    // look up callback information for the callback of type "callback_name" on "widget" 
    callback_identifier cb_id = {widget, callback_name};
    callback_info* cb_info = (callback_info*)g_hash_table_lookup(widget_callback_table, &cb_id);
    if(cb_info == NULL){return;} //nothing was found in map, so ignore 

    // load and call the function that the user wrote
    callback_type their_foo = (callback_type) dlsym(cb_info->dl_handle, cb_info->function_name);
    their_foo(widget, cb_info->identifier_pointers); //don't worry about "identifier_pointers" for now, i'll explain later.
}
```

So yes, Tuxrup is a shared library that dynamically produces and loads other shared libraries! 


# Sure, i see how the user can write some code we can compile into a shared library and then load ... but how can they write code that references variables and functions in the original code?
This is where the trickery comes in and things start to get slightly complicated. Lets look at the function that `button_A` calls in `hello_world.c`:

```C hello_world.c
typedef int my_own_int;

int some_original_variable = 20;
void some_original_function(int var) {
  g_print("Hi! I am some_original_function and now i will print: %d\n", var);
}

static void
button_A_callback 
(GtkWidget *widget,
 gpointer   data)
{
    some_original_variable++;
    my_own_int local_var = (my_own_int)some_original_variable;
    some_original_function(local_var);
}
```

Now suppose the user wanted to turn this function into instead:

```C
    some_original_variable *= 2;
    my_own_int local_var = (my_own_int)some_original_variable;
    some_original_function(local_var);
```

We know that this code they've just written will at some point need to be compiled. As far as I know, interpreting C code isn't really a thing, and if it is, it's probably complicated to set it up so it works perfectly with a library like GTK. For this to be compiled properly, the typedef `my_own_int` must be recognized as a type alias for `int`, `some_original_variable` must be recognized as an integer variable, and `some_original_function` must be recognized as a function from int to void. 

But trickier yet, even if we can get this to compile, we want that this code really actually changes `some_original_variable`! Pushing this button should really have the effect of doubling `some_original_var`, so that when `button_B` prints it

```C hello_world.c
static void
button_B_callback 
(GtkWidget *widget,
 gpointer   data)
{
  some_original_function(some_original_variable);
}
```
- then it has been doubled! 

The point is: For this modified function in question, we will need a pointer to `some_original_variable` and a pointer to `some_original_function`. But in fact, knowing just the names of these two identifiers, we can read the debug symbols of the program to determine the pointer locations! In `pointer_name_conversion.c` we have two functions `get_pointer_from_identifier(char* identifier)` and `get_identifier_from_pointer(void* pointer)`. So if we know what things our modified function needs, we can just "retrieve" pointers to them and somehow send them to the modified function. 

What we do in practice is that we dynamically produce a C file like this:

```C
#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

typedef int my_own_int;
 
void button_A_callback(GtkWidget *widget, gpointer data){
    //PREFIX: 
    void** _data = (void**)data; 

    typedef void(*some_original_function_t)(int);
    some_original_function_t some_original_function = (some_original_function_t)(_data[1]);
    typedef int some_original_variable_t;
    some_original_variable_t some_original_variable = *((some_original_variable_t*)(_data[0]));

    //THEIR FUNCTION: 
    some_original_variable *= 2;
    my_own_int local_var = (my_own_int)some_original_variable;
    some_original_function(our_stupid_int+1);

    //POSTFIX: 
    *((some_original_variable_t*)(_data[0])) = some_original_variable;
}
```

Any callback function in GTK takes a generic `data` parameter that simply stores a pointer to "some data" - it's up to the function to safely and correctly know what exactly this data will be. Welcome to the wild unsafe west of C programming! Anyways, our function dispatcher looks up `widget_callback_map` and finds the pointers it knows this modified function will need. It then sends a pointer to an array of pointers for the `data` parameter, so thats why we do the cast `void** _data = (void**)data`. 

For every identifier (where "identifier" is a general term we'll use that encompasses both variables and functions) we make a typedef for its type, just for convenience. We then load these identifiers into local variables / function pointers with exactly the same names as the original ones. For the functions, just pointers will be fine, but the variables we need to actually dereference them and get their values. The original code the users write assume it's working with actual values, not pointers! Since we've dereferenced the variable `some_original_variable`, any changes we've made to it won't be applied to the original variable. That's what we need the postfix for, where we take the original variable, which is stored at `_data[0]`, we cast it to a `some_original_variable_t` pointer, we then use the `*pointer = value`-notation to set the value at this pointer to the value of `some_original_variable`. Boom. This actually works!


# But given just the pointer to the callback function, how do you retrieve the function source code?
Remmember: We have `get_pointer_from_identifier(char* identifier)` and `get_identifier_from_pointer(void* pointer)`. So we just call `get_identifier_from_pointer` to get the function name. Then, assuming we have the source code downloaded somewhere, and we know where this somewhere is (for now its just hardcoded into the program), we can use the program "ctags" to generate a "tags" file of the source code. This tags file lists every function and variable and the name of the file in which it is implemented. We "simply" get the name of the function, generate the ctags for the source code directory, then retrieve the document with the code that implements this function.


# You have the document that contains the function, but how do you get the *exact* place in this document that contains the implementation of the function in question? How do you get the types of the various identifiers you need, and all the typedefs and include statements you need, but *not* any other variables or functions you don't need in the document ...
Yeah ... This is no job for regax or simple string-matching/building algorithms. To be safe and proper and not reinvent the wheel, we need a C parser. `LibClang` to the rescue! `LibClang` is a C-library that can be used to parse C code. To use the proper terminology, it's a C language front-end, that parses the language into an AST, but doesn't further translate that AST.  

The document `document_parsing.c` contains various functions used to parse the C file with the function we'll be modifying. Some tasks we need to perform are:

- Using the function name, get the function type and header
- Using the function name, get the part of the document before the function, the actual code body of the function, and the part of the document after the function
- Scan through the document, but ignore every variable or function. This way we can build up a version of the document with all the include statements, macros, typedefs and structs we need, but nothing else
- Given the new modified function, figure out all the identifiers it will need (all the variables and functions it needs outside of it's own scope). Also figure out the types of these identifiers

Luckily, once you get the hang of how it works, LibClang is actually very simple and easy to use, so none of these are that particularily difficult to accomplish. 


# Please recap the whole pipeline/process for me as a whole, i'm feeling a bit lost!
When the program launches, we hook into its "application_new" method to initialize anything we need to initialize in Tuxrup (like our hash tables). When the program builts up all it's initial callbacks / signals, we hook into the "signal_new" method and add to our table all the callbacks and widgets we might want to change later. 

When the user wants to change some callback, we use the callback function pointer to retrieve the name of the function it calls, then use that name together with ctags to get the document that contains the function, then use `LibClang` to parse the document and get the part before, during, and after the function. We then let the user write their new function, and scan this again with `LibClang` to determine all the identifiers their function will need. We use this list of identifiers to add a prefix and postfix to the modified function that will load in pointers to these identifiers into variables. We then compile a C file with this modified function with the prefix and postfix, as well as all includes, typedefs, macros and structs from the original file (just to be on the safe side). It is compiled into a shared library. We load a hook into this shared library and store it in the `widget_callback_table`. We then remove the current callback on the widget we're modifying, and set its new callback to be the function dispatcher, which will load the hook from the table and call the function.



















