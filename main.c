#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gap_buffer.h"
#include "document.h"
#include "window.h"
#include "terminal.h"
#include "log.h"
#include "status.h"


int main(int argc, char* argv[])
{
    Document* document = NULL;
    char* filename = NULL;
    if (argc > 1)
    {
        // Read filename if provided
        filename = argv[1];
        document = document_read(filename);
        Line* current = document->head;
    } else
    {
        // Otherwise create a new document with one empty line
        filename = "unnamed.txt";
        document = document_create();
        Line* line = line_create();
        document_insert_after(document, document->tail, line);
    }

    // Initialize the terminal screen
    terminal_init();
    // Get the width and height of the terminal so we know how much to display
    int width;
    int height;
    terminal_size(&width, &height);

    // Create a window which is a view of the document.
    // Reduce the height by 1 so that we can show the status bar at the bottom
    Window* window = window_create(document, width, height - 1);
    for (;;)
     {
        // Clear the screen
        terminal_clear();

        // Draw the contents of the window
        window_draw(window);
        // Draw the status bar message here
        status_message(0, window->height, "--Wee--\n");
        // ...

        // Get the current line that the cursor is on
        Line* currentline = document_get(document, window->current);
        // Move the cursor to the correct location on the screen
        terminal_move(currentline->gbuf->insert_position,
            window->current - window->first_row);
        // Show everything to the user all at once
        terminal_refresh();

        // Wait for user input
        int ch = terminal_read();
        // Handle user input here
        // if user enters backspace
        if(strcmp(terminal_keyname(ch), "^?") == 0 || ch == KEY_BACKSPACE)
        {
          // regular remove character
          if(currentline->gbuf->insert_position > 0)
          {
            gap_remove_char(currentline->gbuf);
          }
          // combine current line with the line before it:
          else if(currentline->previous != NULL && currentline->gbuf->insert_position == 0)
          {
            //int oldPos = currentline->gbuf->insert_position;
            char* toInsert = gap_to_string(currentline->gbuf);
            gap_insert_string(currentline->previous->gbuf, strlen(toInsert), toInsert);
            document_remove(document, currentline);
            free(toInsert);
            window->current--;
            //Line* cur = document_get(document, window->current);
            //gap_set_insert_position(cur->gbuf, oldPos);
          }
        }
        // if user enters enter
        else if(strcmp(terminal_keyname(ch), "^J") == 0)
        {
          // if the user hits enter at the end of a line:
          if(currentline->gbuf->insert_position == gap_length(currentline->gbuf))
          {
            Line* newL = line_create();
            document_insert_after(document,currentline,newL);
            window->current++;
          }
          // if the user hits enter at the start of a line:
          else if(currentline->gbuf->insert_position == 0)
          {
            Line* newL = line_create();
            document_insert_before(document, currentline, newL);
            window->current++;
          }
          // else if the user hits enter in the middle of a line:
          else
          {
            Line* newL = line_create();
            newL->gbuf = gap_break(currentline->gbuf);
            document_insert_after(document, currentline, newL);
            window->current++;
          }
        }
        // if the user enters Ctrl-Q
        else if(strcmp(terminal_keyname(ch), "^Q") == 0)
        {
          break;
        }
        // if the user enters up arrow key:
        else if(ch == KEY_UP)
        {
          if(currentline->previous != NULL)
          {
            if(window->current == window->first_row)
            {
              window->first_row--;
            }
            window->current--;
            terminal_move(currentline->previous->gbuf->insert_position,
            window->current - window->first_row);
          }
        }
        //if the user enters down array key:
        else if (ch == KEY_DOWN)
        {
          if(currentline->next != NULL) //&& window->current < window->height -1)
          {
            if(window->current == (window->height + window->first_row - 1))
            {
              window->first_row++;
            }
            window->current++;
            terminal_move(currentline->next->gbuf->insert_position,
            window->current - window->first_row);
          }
        }
        // if the user enters the right arrow key:
        else if(ch == KEY_RIGHT)
        {
          if((currentline->gbuf->insert_position) < gap_length(currentline->gbuf))
          {
            gap_set_insert_position(currentline->gbuf, currentline->gbuf->insert_position + 1);
          }
        }
        // if the user enters the left arrow key:
        else if(ch == KEY_LEFT)
        {
          if(currentline->gbuf->insert_position > 0)
          {
            gap_set_insert_position(currentline->gbuf, currentline->gbuf->insert_position - 1);
          }
        }
        // insert whatever else the user entered:
        else if(ch != -1)
        {
          gap_insert_char(currentline->gbuf, ch);
        }


    }

    terminal_end();

    // On quit, write the document to output file here
    //document_write(window->document, "output.txt");
    document_write(window->document, filename);
    window_free(window);
}
