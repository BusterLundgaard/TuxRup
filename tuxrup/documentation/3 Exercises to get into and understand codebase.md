# Exercise 1:
Retrieving an element from the `widget_callback_table` right now is a little bit inconvenient and unsafe: You have to create a struct that stores the widget and the callback name, you have to get a pointer from the widget_callback_table for where the element *might* exist, and you have to check if that pointer is `NULL` and cast is to the appropriate type `cb_info` (which you just need to know and remmember!). Add a method in `callback_table.h` and `callback_table.c` that helps us do this more easily.

# Exercise 2:
Suppose we've just figured out that GTK supports a kind of callback/action we didn't actually know existed yet, say a widget "swipe" event if you've swiped on a widget (i don't know if this is an actual thing, just a hypothetical example!). Have a look in `globals.h` and `globals.c`. What should you add to support this new callback type?

# Exercise 3:
`fix_function_body.c` is responsible for generating the modified C file that we compile on the fly. It's job (as described in the note about Tuxrup general principles) is to add a prefix and a postfix to the user's modified function, so that any identifiers they need are given as pointers. 

When i wrote this function original, i was testing out and debugging the actual file it generated, and i wanted it to look nice for educational purposes, so i added a bunch of `"    "` spaces so that the spacing was proper, like
"void my_callback(){
    int hello = 30; 
}"
instead of 
"void my_callback(){
int hello = 30; 
}"
wow, huge difference, i know! Anyways, adding all these spaces made `fix_function_body.c` way uglier. Like you have for example:
```C
g_string_append(buffer, "    ");
write_ref_typedef(undefined_identifier, undefined_type, buffer);
g_string_append(buffer, "    ");
write_ref_declaration(undefined_identifier, undefined_type, buffer);
```
Anyways, your job is to patch `fix_function_body.c` so that we no longer worry about this spacing. Just attempt to make it a bit cleaner and more readable overall if you can. 

# Exercise 4:
So, as you may know by now, we manage memory ourselves in C. This includes strings, and strings are really one of the biggest paint points because it's so easy to forget to free these strings from memory you don't need anymore. Have a look at `callback_information.c` to see what i mean. We have a whole "free" function that just frees all the memory we allocated in `get_callback_code_information`. 

I am 100% there's *A LOT* of string memory leaks in our current Tuxrup code. I think specifically we might have some in `document_parsing.c`, since we have a lot of code here that works with and builds up strings. Try to find some places in `document_parsing.c` where we allocate memory we forget to deallocate, and fix it. 

# Exercise 5
The current window that opens up to edit source code is pretty ugly. Like, i dont know man, it just looks and feels like shite. Figure out where the code that generates this window is, and make it look smexier! I'm thinking, for example, that the code before and after should take up less space, like, it should be scrollable on it's own, not just "one big scroll" for the whole document like we have right now. 

# Exercise 6
Consult with ChatGpt and find a way to make a function in `pointer_name_conversion.c` that allows us to somehow check if our current executable has debug symbols (so returns true or false). 

# Exercise 7
This is a significantly bigger task than the other ones, and now you'll start making some real significant changes to Tuxrup. 

I've changed the previously very simple testing `hello_world.c` program so that it now has a right-click context menu on one of the buttons. Making context menus in GTK is really badly documented and changed a lot from GTK3 to GTK4, so it was a huge pain in the ass to figure out, but you should be able to figure out how to do it from my code and comments. More or less: The two important kind of items you need are first of all a `GMenu` which you make with `g_menu_new()`, and a `popover` which you make with `gtk_popover_menu_new_from_model` (which expects an argument of type `GMenuModel`, where you're just supposed to know that apparantly a `GMenu*` can safely be converted to a `GMenuModel` with a macro `G_MENU_MODEL()`, it's very stupid). You attach the context menu to a specific widget by using `gtk_widget_set_parent(widget)`. Now you can start populating your `GMenu` with three types of items: Elements, Sections, or Submenus, which you add with `g_menu_append_item`, `g_menu_append_section` and `g_menu_append_submenu` respectively. 

Anyways, what i want you to do is to add a right-click context menu to any widget in Tuxrup. The context menu should look like this roughly:
- Delete widget
- Edit widget properties
- Edit widget css
- Edit widget callbacks

"Delete widget" should actually delete the widget! For the other buttons, they should just each open a window that we can start implementing for real later. So just call some `open_widget_properties_window()`, some `open_widget_css_menu` and some `open_edit_callbacks_menu()`, and then have each just open an empty window with nothing in it for now. 

