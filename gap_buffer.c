#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gap_buffer.h"


GapBuffer* gap_create()
{
    GapBuffer* gap = (GapBuffer*)malloc(sizeof(GapBuffer));
    gap->data = (char*)malloc(sizeof(char) * DEFAULT_GAP_SIZE);
    gap->size = DEFAULT_GAP_SIZE;
    gap->insert_position = 0;
    gap->second_position = gap->size;
    return gap;
}

void gap_set_insert_position(GapBuffer* gbuf, int new_position)
{
    if(new_position < gbuf->insert_position)
    {
      for(int difference = gbuf->insert_position - new_position; difference > 0; difference--)
      {
        gbuf->second_position--;
        gbuf->insert_position--;
        char temp = gbuf->data[gbuf->insert_position];
        gbuf->data[gbuf->second_position] = temp;
      }
    }
    else if(new_position > gbuf->insert_position)
    {
      for(int difference = new_position - gbuf->insert_position; difference > 0; difference--)
      {
        gbuf->insert_position++;
        gbuf->second_position++;
        char temp = gbuf->data[gbuf->second_position-1];
        gbuf->data[gbuf->insert_position -1] = temp;
      }
    }
}

void gap_insert_char(GapBuffer* gbuf, char c)
{
    // if gap needs to grow
    // grow buffer
    if(gbuf->insert_position == gbuf->second_position)
    {
      char* temp = (char*)malloc(sizeof(char) * gbuf->size);

      // copy whats in gbuf->data into temp:
      for(int i = 0; i < gbuf->size; i++)
      {
        temp[i] = gbuf->data[i];
      }

      free(gbuf->data);

      gbuf->data = (char*)malloc((gbuf->size + DEFAULT_GAP_SIZE) * sizeof(char));
      // copy what's in temp back into data:
      for(int i = 0; i < gbuf->size; i++)
      {
        gbuf->data[i] = temp[i];
      }
      free(temp);

      if(gbuf->second_position != gbuf->size)
      {
      // shift everything after second position in data[] over 1 to the right
      // x amount of times where x is the amount the gap just grew

        int numCharsToShift = (gbuf->size) - gbuf->second_position;
        int l = numCharsToShift;
        for(int j = 0; j < numCharsToShift; j++)
        {
          int idx = (gbuf->second_position -1) + l;
          gbuf->data[idx + 4] = gbuf->data[idx];
          l--;
        }
      }
      gbuf->second_position += DEFAULT_GAP_SIZE;
      gbuf->size += DEFAULT_GAP_SIZE;
    }
    // insert the new character:
      gbuf->data[gbuf->insert_position] = c;
      gbuf->insert_position++;

}

void gap_insert_string(GapBuffer* gbuf, int count, const char* s)
{
    for(int i = 0; i < count; i++)
    {
      gap_insert_char(gbuf, s[i]);
    }
}

void gap_remove_char(GapBuffer* gbuf)
{
  gbuf->insert_position--;
}

char* gap_to_string(GapBuffer* gbuf)
{
    int sizeBuffer = (gbuf->second_position - gbuf->insert_position);
    int sLength = gbuf->size - sizeBuffer + 1;
    char* string = (char*)malloc(sizeof(char) * sLength);
    int strIdx = 0;
    for(int i = 0; i < gbuf->size; i++)
    {
      if(i < gbuf->insert_position)
      {
        string[strIdx] = gbuf->data[i];
        strIdx++;
      }
      else if (i >= gbuf->second_position)
      {
        string[strIdx] = gbuf->data[i];
        strIdx++;
      }
    }
    string[sLength -1] = '\0';
    return string;
}

int gap_length(GapBuffer* gbuf)
{
    char* s = gap_to_string(gbuf);
    int length = strlen(s);
    free(s);
    return length;
}

GapBuffer* gap_break(GapBuffer* gbuf)
{
  // calculate the length of the string following the gap buffer,
  // then initialize a string of that length, and fill it with the
  // contents of gbuf->data after gbuf->second_position:
    int slength = gbuf->size - gbuf->second_position;
    char newLineContents[slength];
    for(int i = 0; i < slength; i++)
    {
      newLineContents[i] = gbuf->data[gbuf->second_position + i];
    }
    gbuf->second_position = gbuf->size;
    GapBuffer* newGbuf = gap_create();
    gap_insert_string(newGbuf, slength, newLineContents);
    gap_set_insert_position(newGbuf, 0);
    return newGbuf;
}

void gap_print(GapBuffer* gbuf)
{
    printf("%p|", gbuf);
    for (int i = 0; i < gbuf->insert_position; i++)
    {
        printf("%c", gbuf->data[i]);
    }
    printf("[");
    for (int i = gbuf->insert_position; i < gbuf->second_position; i++)
    {

    }
    printf("]");
    for (int i = gbuf->second_position; i < gbuf->size; i++)
    {
        printf("%c", gbuf->data[i]);
    }
    printf("|\n");
}

void gap_free(GapBuffer* gbuf)
{
  free(gbuf->data);
  free(gbuf);
}
