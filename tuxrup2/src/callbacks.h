#ifndef CALLBACKS_H
#define CALLBACKS_H
void on_callback_edit(GtkWidget* widget, GtkTextBuffer* text);
void on_callback_done(GtkWidget* widget, GtkTextBuffer* text);
void callbacks_reset(GtkTextBuffer* buffer);
char* get_document_path(char* function_name);
#endif
