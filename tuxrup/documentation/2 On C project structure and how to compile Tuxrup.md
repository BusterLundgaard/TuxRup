# Header files, and what we actually do when we #include something in C
So. You have a large project written in C, and you want to split this project into multiple smaller documents. You also want some notion of scoping, so that different documents can use different variables with the same name without naming conflicts occuring. What to do?

Suppose your file very roughly looks like this:

```C your_big_ass_file.c
// A: A BUNCH OF UTILITY/HELPER FUNCTIONS

// B: A BUNCH OF VARIABLE DEFINITIONS

// C: SOME METHODS USING THESE VARIABLES

// D: YOUR int main METHOD
```

You could now split it up like this:

```C util.c
// A BUNCH OF UTILITY/HELPER FUNCTIONS
```
```C variables.c
// A BUNCH OF VARIABLE DEFINITIONS
```
```C cool_methods.c
#include "util.c"
#include "variables.c"

// SOME METHODS USING THESE VARIABLES
```
```C main.c
#include "cool_methods.c"

int main(){
    // some main method here
}
```

Now you would just compile with `gcc main.c -o main`. This is the most basic kind of include you can do, as `#include "cool_methods.c"` litteraly just copy pastes the contents of `cool_methods.c` into `main.c` before compiling! And since `cool_methods.c` includes `util.c` and `variables.c` it also includes those! 

As you might imagine, this kind of "copy-pasting" solution is ... inflexible. It kind of forces you to structure things in a very linear fashion where everything only relies on the things before it. It also doesn't give us any kind of scoping options for variable/function names, since eventually everything will just be pasted into our main file! 

There is another way: `.o` files (object files). Suppose we had

```C some_file.c
int exponentiation(int x, int p){
    int res = 1;
    for(int i = 0; i < p; i++){
        res *= x;
    }
    return res 
}

int modulos(int x, int m){
    return x % m;
}

float pi = 3.14138584;

int main(){
    return modulos(exponentiation(20, 5), 10);
}
```

What you can do is that inside `some_file.c` you can *define* some functions and variables, even if you don't give them an implementation or initial value. You can bassicly say: "These are functions and variables that will be implemented in another file". You could split into two files:

```C main.c
int exponentiation(int x, int p);
int modulos(int x, int m);
float pi;

int main(){
    return modulos(exponentiation(20, 5), 10);
}
```
```C math.c
int exponentiation(int x, int p){
    int res = 1;
    for(int i = 0; i < p; i++){
        res *= x;
    }
    return res 
}

int modulos(int x, int m){
    return x % m;
}

float pi = 3.14138584;
```

Now you can compile your math file like `gcc -c math.c -o math.o` and your main file like `gcc -c main.c -o main.o`. Note that we use the `-c` flag, which tells GCC to not make an executable, but an object. You now have two objects, `math.o` and `main.o`. You can link these objects together into an executable with `gcc main.o math.o -o my_program`. 

What GCC does is that for each object file, it gets a list of all the variables or functions that need implementations from other files. It then runs through every file and gets those implementations. Finally, it makes sure one and only one file has a `int main()` method, and makes that the entry point of the application. Tada!

Note that what we're doing in `gcc main.o math.o -o my_program` no longer has anything to do specifically with C. `math.o` and `main.o` are assembly files! The could just as well have been produced by other languages. It is the job of the operating system "linker" to link these objects together into an executable.  The beauty of C is that there's no magic layer doing things for us, we're really just doing everything, including the project management here, in the most manual way you could imagine. It really is possible to sort of understand what's going on at a low level. 

Now let's go a little further. Doesn't it feel somewhat silly and cumbersome that `main.c` needs to include those signatures of math functions from `math.c`? It does yeah. Let's put them into another file `math_defs.c`:

```C math_defs.c
int exponentiation(int x, int p);
int modulos(int x, int m);
float pi;
```

Now we change `main.c` into:

```C main.c
#include "math_defs.c"

int main(){
    return modulos(exponentiation(20, 5), 10);
}
```

This of course works just the same as before, since `math_defs.c` is litteraly copy-pasted into `main.c`, so that when we compile it, it looks exactly the same as before. But IMO this isn't *that* elegant either: It's sort of confusing that `main.c` and `math.c` are files that are meant to be compiled into `.o` files, but `math_defs.c` distinctly isn't. It contains litteraly nothing and can't be compiled! To make it clear that these are two distinct types of files, we give it the file extension `.h` instead, so `math.h`. Our project is now:

my_project
-- main.c
-- math.c
-- math.h

So `.h` header files contain *definitions* but not implementations. `.c` files can contain both. `.c` files can be compiled into object files (or directly into an executable in one big command like `gcc main.c math.c -o my_program`), and `.h` files cannot, they are meant to be `#include`'d in `.c` files. 

By "definitions" we mean a lot of things:
- function signatures (name, return type, parameters)
- variable signatures (name, type)
- struct and union definitions
- typedefs
- macro definitions


# A quick example of a header file in Tuxrup:
Let's look at an example. In Tuxrup we have `callback_map.h`:

```C callback_map.h
typedef void (*callback_type)(GtkWidget*, gpointer);

typedef struct {
    gpointer widget;
    gchar* callback_name;
} callback_identifier;

typedef struct {
    callback_type original_function_pointer;
    gchar* function_name;
    void* dl_handle;
    void** identifier_pointers;
    int identifier_pointers_n;
} callback_info;

void add_callback_to_table(GtkWidget* widget, const gchar* detailed_signal, gpointer c_handler);

guint callback_key_hash(gconstpointer key);
gboolean callback_key_equal(gconstpointer a, gconstpointer b);

void callback_key_free(gpointer data);
void callback_value_free(void* data);
```

We have a callback, two structs, and six function signatures. `callback_map.c` implements those six functions:

```C callback_map.c
#include <dlfcn.h>
#include "util.h"
#include "callback_map.h"

guint callback_key_hash(gconstpointer key){
    const callback_identifier* k = key;
    guint hash = hash_string(k->callback_name);
    hash ^= g_direct_hash(k->widget);
    return hash;
}

gboolean callback_key_equal(gconstpointer a, gconstpointer b){
    const callback_identifier *ka = a;
    const callback_identifier *kb = b;
    return (ka->widget == kb->widget) && strcmp(ka->callback_name, kb->callback_name) == 0;
}

void callback_key_free(gpointer data){
    callback_identifier *k = data;
    g_free(k->callback_name);
    g_free((gpointer)k);
}

void callback_value_free(void* data){
    callback_info* cb = data;
    g_free(cb->function_name);
    g_free(cb->identifier_pointers);
    if(cb->dl_handle != NULL){
        dlclose(cb->dl_handle);
    }
    g_free(cb);
}

void add_callback_to_table(GtkWidget* widget, const gchar* detailed_signal, gpointer c_handler){
    callback_identifier* cb_key = malloc(sizeof(callback_identifier));
    cb_key->widget = widget;
    cb_key->callback_name = g_strdup(detailed_signal);

    callback_info* cb_info = malloc(sizeof(callback_info));
    cb_info->original_function_pointer = (callback_type)c_handler;
    cb_info->function_name = get_identifier_from_pointer(c_handler);
    cb_info->dl_handle = NULL;
    cb_info->identifier_pointers = NULL;
    cb_info->identifier_pointers_n = 0;

    g_hash_table_insert(widget_callback_table, cb_key, cb_info);
}
```

`callback_map.c` includes `callback_map.h` so that it can use the same typedefs / structs ect. If none of the function implementations use any of these definitions, then technically speaking `callback_map.c` doesn't need to include `callback_map.h` to compile into a `.o` file, but it's a good convention to follow to always include `.h` file you're implementing.


# Header files as *interfaces*
Let's look at a pretty different example. `fix_function_body.h` is very small:

```C fix_function_body.h
// Takes some code and a set (we make sets in C by using a hashtable, it's a slightly hacky solution) of type information about the undefined identifiers in that code (identifiers from the original document that the code needs)
// Produces a "fixed" function body with a prefix and a postfix where these undeclared indentifiers are declared by dereferencing pointers
char* create_fixed_function_body(char* modified_code, GHashTable* undefined_identifiers);
```

The actual `fix_function_body.c` is way bigger and has a lot more functions:

```C fix_function_body.c
#include "reference_type.h"
#include "fix_function_body.h"

void write_ref_typedef(char* identifier, reference_type* ref, GString* buffer){
    typedef int my_cooler_int;
    
    if(ref->is_function){
        g_string_append(buffer, g_strdup_printf("typedef %s(*%s_t)(%s);\n", ref->return_type, identifier, ref->args_types));
    } else {
        g_string_append(buffer, g_strdup_printf("typedef %s %s_t;\n", ref->return_type, identifier));     
    }
}

void write_ref_declaration(char* identifier, reference_type* ref, GString* buffer){
    if(ref->is_function){
        g_string_append(buffer, g_strdup_printf("%s_t %s = (%s_t)(_data[%d]);\n", identifier, identifier, identifier, ref->number));
    } else {
        g_string_append(buffer, g_strdup_printf("%s_t %s = *((%s_t*)(_data[%d]));\n", identifier, identifier, identifier, ref->number));
    }
    
}

void write_var_overwrite(char* identifier, reference_type* ref, GString* buffer){
    g_string_append(buffer, g_strdup_printf("*((%s_t*)(_data[%d])) = %s;\n", identifier, ref->number, identifier));
}

char* create_fixed_function_body(char* modified_code, GHashTable* undefined_identifiers){
    // PREFIX
    GString* buffer = g_string_new("//PREFIX: \n");
    g_string_append(buffer, "void** _data = (void**)data; \n");

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, undefined_identifiers);
    
    for(; g_hash_table_iter_next(&iter, &key, &value); ){
        char* undefined_identifier = (char*)key;
        reference_type* undefined_type = (reference_type*)value;
        
        g_string_append(buffer, "    ");
        write_ref_typedef(undefined_identifier, undefined_type, buffer);
        g_string_append(buffer, "    ");
        write_ref_declaration(undefined_identifier, undefined_type, buffer);
    }

    // BODY
    g_string_append(buffer, "\n //THEIR FUNCTION: \n");
    g_string_append(buffer, modified_code);
    g_string_append(buffer, "\n");

    // POSTFIX
    g_string_append(buffer, "//POSTFIX: \n");
    g_hash_table_iter_init(&iter, undefined_identifiers);
    for(; g_hash_table_iter_next(&iter, &key, &value); ){
        char* undefined_identifier = (char*)key;
        reference_type* undefined_type = (reference_type*)value;
        if(undefined_type->is_function){continue;}

        g_string_append(buffer, "    ");
        write_var_overwrite(undefined_identifier, undefined_type, buffer);
    }

    char* str = g_strdup(buffer->str);
    g_string_free(buffer, TRUE);
    return str;
}
```

Think about it: Other `.c` files will in 99.9% of cases only include the header files, not the actual `.c` files. So your header files define a "public interface" that tell other files what functions, variables, typedefs and macros they can use. Other files *only* need `create_fixed_function_body`, they don't need `write_var_overwrite`, so we don't make that public by including it in the header file. So this is kind of encapsulation as you know it from object oriented programming: Only make public what needs to be public, think about interfaces as defining what the function of a certain "tool" is, while the corresponding `.c` file actually creates/implements that tool. If you think of SVEA, then we can even (sort of) use the "strategy" pattern in C: we might have two different `.c` files that both implement `fix_function_body.h` but in two diffent ways, and then we can choose at link-time which one to use.

## Sidenote: Object oriented-ish programming in C
If you're a little creative with function pointers and structs, there's a lot of object oriented patterns you can recreate in just C, all the while being a lot closer to the hardware and having a lot more control. It's really only inheritance that's difficult to do in C, and inheritance is cringe anyways! 

Suppose you had a very simple class like this 

```C#
Class Person{
    int age;
    string name;
    void say_hello(){
        printf("Hello, my name is %s, and i'm %d years old\n", name, age);
    }
}
```

We could *sort of* do this in just C like:

```C person.h
typedef struct {
    int age;
    char* name;
    void(*say_hello)(Person* self);
} Person;

// Constructor:
Person Person(int age, char* name);
```
```C person.c
#include "person.h"

// Implement say_hello method:
void Person_say_hello(Person* self){
    printf("Hello, my name is %s, and i'm %d years old\n", self.name, self.age);
}

// Implement constructor:
Person Person(int age, char* name){
    return {.age = age, .name = name, .say_hello = Person_say_hello};
}
```
```C main.c
#include "person.h"

int main() {
    Person p = Person(20, "simon");
    p.say_hello(&p);
}
```

Of course this is significantly more verbose and less elegant than in C#. It's especially annoying having to pass the pointer to our object instance anytime we want to call one of it's method that uses it's properties. But really, you get a lot of the C# features (you can still have a lot of control over what is private and public based on what you put in your header file vs implementation file), with the disadvantage of shitty syntax but the advantage of speed and a real understanding of what is going on at the hardware level. 


# Multiple definition problems in header files, and conditional compilation guards
Suppose you have `utils.h` that both `algebra.h` and `geometry.h` needs (and such, both of them include it). Now you might have some file `main.c` that includes both `algebra.h` and `geometry.h`. But inclusion is just copy pasting! So it will include the contents of `utils.h` twice, first from copy-pasting the contents of `algebra.h`, then from copy-pasting the contents of `geometry.h`. This is fucking træls. But we have a solution! A commonly-used, sort of incredibly hacky and wack solution. The first time i learned about this, i genuinely could not belive this was the standard way of preventing this problem in C. You have to add these special little lines to the top and end of your `utils.h` file:

```C utils.h
#ifndef UTILS_H
#define UTILS_H

typedef {
    float value;
} my_cool_struct;
#endif
```

Yes, it's extremely ugly. To understand what's going on, we need to understand "compilation guards" in C. In C you can use kind of "meta-if-statements" that only include certain code if certain properties are met, like setting a certain compiler option when compiling, or a property of your PC like what program it's running. We actually have a good example of why this is useful in Tuxrup. GTK4 changed a lot of things from GTK3, and if you try to use Tuxrup on a GTK3 program, GTK will give an error message saying that GTK4 symbols are not compatible with GTK2/3 programs. So we need both a GTK3 version of Tuxrup and a GTK4 version. We *could* just copy paste our source code and make a completely different version of Tuxrup that uses GTK3 instead, but that would be a multiple maintenance hell. Only a few parts of our source code need to change between GTK3 and GTK4, so it would be lovely if we could write a kind of if-statement directly in the relevant parts of the code. 

This is what we do. In `modify_callback.c` we have at one point:

```C 
// Create a vertical box layout
#ifdef USE_GTK3
GtkAdjustment* v_adj = gtk_adjustment_new(0,0,0,0,0,0);
GtkAdjustment* h_adj = gtk_adjustment_new(0,0,0,0,0,0);
GtkWidget* scrolled_window = gtk_scrolled_window_new(v_adj, h_adj);

#else 

GtkWidget* scrolled_window = gtk_scrolled_window_new();
#endif
```

GTK3 and GTK4 differ in how a scrolled window is created, so we create a compilation-time if-statement with #ifdef, and put our GTK3 code in the true clause and the GTK4 code in the else clause. 

Of course, for GTK4 compilation we should include `<gtk/gtk-4.0.h>`, while for GTK3 compilation we should include `<gtk/gtk-3.0.h>` so we replace any previous occurence of simply `#include <gtk/gtk.h>` with

```C
#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif
```

Brilliant, init? Now when we compile we can choose whether or not we want to add a `-DUSE_GTK` option to GCC (yes we need the "D" in front for some reason!), in which case the `#ifdef USE_GTK3` is true, or not, in which case it will be false. A valid compilation command would be:

```bash
gcc \
$(pkg-config --cflags gtk4) $(pkg-config --libs gtk4) -lelf -ldl \
-DUSE_GTK3 \
intercept_gtk_functions.c \
callback_information.c \ 
callback_map.c \
compute_widget_hash.c \
document_parsing.c \
fix_function_body.c \
function_dispatcher.c \
globals.c \
gtk_events.c \
modify_callback.c \
pointer_name_conversion.c \
util.c \
-shared -fPIC -o hw
```

So if we want a `Makefile` we can use to compile both a GTK3 and GTK4 version, we could have it look like:

```Makefile
gtk3:
    gcc \
    $(pkg-config --cflags gtk4) $(pkg-config --libs gtk4) -lelf -ldl \
    -DUSE_GTK3 \
    intercept_gtk_functions.c \
    callback_information.c \ 
    callback_map.c \
    compute_widget_hash.c \
    document_parsing.c \
    fix_function_body.c \
    function_dispatcher.c \
    globals.c \
    gtk_events.c \
    modify_callback.c \
    pointer_name_conversion.c \
    util.c \
    -shared -fPIC -o hw

gtk4:
    gcc \
    $(pkg-config --cflags gtk4) $(pkg-config --libs gtk4) -lelf -ldl \
    intercept_gtk_functions.c \
    callback_information.c \ 
    callback_map.c \
    compute_widget_hash.c \
    document_parsing.c \
    fix_function_body.c \
    function_dispatcher.c \
    globals.c \
    gtk_events.c \
    modify_callback.c \
    pointer_name_conversion.c \
    util.c \
    -shared -fPIC -o hw
```

Very cool. What the `-DUSE_GTK3` option does in practice is to add to the very start of any document it compiles 
```C
#define USE_GTK3
```
What an `#ifdef` statement just does is to check if a certain macro has been defined yet. So in the following file

```C
#define MY_HUGE_ASS

int main() {
    #ifdef MY_HUGE_ASS
    printf("It's enourmous!\n");
    #else 
    printf("It's a little sad actually ... \n");
    #endif
}
```

the `printf("It's a little sad actually ... \n")` will never ever ever be compiled! The compiler will completely ignore it, because `#ifdef MY_HUGE_ASS` will always be true with `MY_HUGE_ASS` defined directly in the document. 

This is the trick we exploit with our header files. `#ifndef` is a negated `#ifdef` that checks if a macro has *not* been defined. So lets remmember the actual situation: `main.c` includes both `algebra.h` and `geometry.h`, which both include `utils.h`. Suppose our various files look like this:

```C utils.h
#ifndef UTILS_H
#define UTILS_H

typedef struct {
    float value;
} my_cool_struct;

#endif
```

```C algebra.h
#include "utils.h"
my_cool_struct exponent(int x, int p);
```

```C geometry.h
#include "utils.h"
my_cool_struct tan(float angle);
```

```C main.c
#include "algebra.h"
#include "geometry.h"

int main() {
    return exponent(20, 3).value + (int)(tan(30.5f)).value;
}
```

During compilation, `main.c` will first be expanded into

```C
#include "utils.h"
my_cool_struct exponent(int x, int p);

#include "utils.h"
my_cool_struct tan(float angle);

int main() {
    return exponent(20, 3).value + (int)(tan(30.5f)).value;
}
```

This will then be further expanded into

```C
#ifndef UTILS_H
#define UTILS_H

typedef struct {
    float value;
} my_cool_struct;

#endif
my_cool_struct exponent(int x, int p);

#ifndef UTILS_H
#define UTILS_H

typedef struct {
    float value;
} my_cool_struct;

#endif
my_cool_struct tan(float angle);

int main() {
    return exponent(20, 3).value + (int)(tan(30.5f)).value;
}
```

Now the compiler starts preprocessing this from top to bottom. In the start, `UTILS_H` has not been defined yet, so `#ifndef` is true. This means the contents between `#ifndef` and `#endif` are included, so we get the struct `my_cool_struct` *and*, crucially, we get the `#define UTILS_H`, so now it's defined. When we then reach the next `#ifndef`, it's contents won't be included because it isn't true anymore! It's ugly, but it's also an absolutely brilliant trick. 
