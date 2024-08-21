#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "document.h"
#include "log.h"
//#include "gap_buffer.c"

// Create a line node with an empty gap buffer
Line* line_create()
{
    GapBuffer* gbuf = gap_create(DEFAULT_GAP_SIZE);
    return line_create_gap(gbuf);
}

Line* line_create_gap(GapBuffer* gbuf)
{
    Line* line = (Line*) malloc(sizeof(Line));
    line->gbuf = gbuf;
    line->next = line->previous = NULL;
    return line;
}

// Make a new document
Document* document_create()
{
  Document* doc = (Document*)malloc(sizeof(Document));
  doc->head = NULL;
  doc->tail = NULL;
  doc->num_lines = 0;

  return doc;
}

Line* document_get(Document* document, int row)
{
  if(document->num_lines > row)
  {
    Line* crawler = document->head;
    for(int i = 0; i < row; i++)
    {
      crawler = crawler->next;
    }
  return crawler;
  }
  else
  {
    return NULL;
  }
}

void document_insert_after(Document* document, Line* insertAfter, Line* newline)
{
  // if the list is empty:
  if(insertAfter == NULL && document->head == NULL && document->tail == NULL)
  {
    document->head = newline;
    document->tail = newline;
    newline->previous = NULL;
    newline->next = NULL;
    document->num_lines++;
  }
  else
  {
    newline->previous = insertAfter;
    newline->next = insertAfter->next;
    // if there is more than 1 line in the list:
    if(insertAfter->next != NULL)
    {
      insertAfter->next->previous = newline;
    }
    // in the case where there is only one line in the list (prior to insertion):
    else
    {
      document->tail = newline;
    }
    insertAfter->next = newline;
    document->num_lines++;
  }
}

void document_insert_before(Document* document, Line* insertBefore, Line* newline)
{
  // if the list is empty:
  if(insertBefore == NULL && document->head == NULL && document->tail == NULL)
  {
    document->head = newline;
    document->tail = newline;
    newline->previous = NULL;
    newline->next = NULL;
    document->num_lines++;
  }
  // if the list isn't empty:
  else
  {
    newline->next = insertBefore;
    newline->previous = insertBefore->previous;
    // if there is more than 1 line in the list:
    if(insertBefore->previous != NULL)
    {
      insertBefore->previous->next = newline;
    }
    // in the case where there is only one line in the list(prior to insertion):
    else
    {
      document->head = newline;
    }
    insertBefore->previous = newline;
    document->num_lines++;
  }
}

void document_remove(Document* document, Line* line)
{
  // if line is the only line in the list:
  if(line->previous == NULL && line->next == NULL)
  {
    // free the head & tail pointers and set them to NULL:
    document->head = NULL;
    document->tail = NULL;
    free(line);
  }
  // else:
  else
  {
    // if line is the head:
    if(line->previous == NULL)
    {
      line->next->previous = NULL;
      document->head = line->next;
      free(line);
    }
    // if line is the tail:
    else if(line->next == NULL)
    {
      // free the line and update tail pointer:
      document->tail = line->previous;;
      document->tail->next = NULL;
      free(line);
    }
    // if the line to be removed isn't the head or the tail:
    else
    {
      line->previous->next = line->next;
      line->next->previous = line->previous;
      free(line);
    }
  }
  document->num_lines--;
}

Document* document_read(const char* filename)
{
  Document* doc = document_create();
  FILE* fp = fopen(filename, "r");
  char buf[500];

    while(fgets(buf, 500, fp) != NULL)
    {
      // find the index of the \n character in the buf and set buf[idx] = \0:
      int idx = strcspn(buf, "\n");
      buf[idx] = '\0';
      Line* newLine = line_create();
      gap_insert_string(newLine->gbuf, strlen(buf), buf);
      document_insert_after(doc, doc->tail, newLine);
    }

  fclose(fp);
  return doc;
}

bool document_write(Document* document, const char* filename)
{
  FILE* fp = fopen(filename, "w+");
  Line* curLine = document->head;
  // use gap to string
  for(int i = 0; i < document->num_lines; i++)
  {
    char* line = gap_to_string(curLine->gbuf);
    int length = strlen(line);
    fwrite(line, length, 1, fp);
    fwrite("\n", 1, 1, fp);
    curLine = curLine->next;
    free(line);
  }
  fclose(fp);
  return false;
}

void document_print(Document* document)
{
    printf("Document %p lines=%d [\n", document, document->num_lines);
    for (Line* line = document->head; line != NULL; line = line->next)
    {
        printf("\t");
        gap_print(line->gbuf);
    }
    printf("]\n");
}

void document_free(Document* document)
{
  Line* crawler = document->head;
  for(int i = 0; i < document->num_lines; i++)
  {
    gap_free(crawler->gbuf);
    Line* temp = crawler;
    crawler = crawler->next;
    free(temp);
  }
  free(document);
}
