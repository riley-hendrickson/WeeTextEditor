#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "terminal.h"
#include "log.h"
#include <string.h>



Window* window_create(Document* document, int width, int height)
{
    Window* window = (Window*)malloc(sizeof(Window));
    window->document = document;
    window->first_row = 0;
    window->first_col = 0;
    window->current = 0;
    window->width = width;
    window->height = height;

    return window;
}

void window_draw(Window* window)
{
    //draw the window into the document
    int first_row = window->first_row;
    for(int i = 0; i < window->height && i < window->document->num_lines; i++)
    {
      Line* current = document_get(window->document, first_row);
      char* s = gap_to_string(current->gbuf);
      terminal_prints(0, i, s);
      int endOfString = strlen(s);
      free(s);
      terminal_printc(endOfString, i, '\n');
      first_row++;
    }
}

void window_free(Window* window)
{
  document_free(window->document);
  free(window);
}
