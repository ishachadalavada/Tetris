/* well.cpp --- 
 * 
 * Filename: well.cpp
 * Description: 
 * Author: Bryce
 * Maintainer: Adeel Bhutta
 * Created: Tue Sep  6 11:08:59 2016
 * Last-Updated: 01-10-2021
 *           By: Adeel Bhutta
 *     Update #: 0
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 * 
 * 
 */

/* Copyright (c) 2016 IUB 
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

/* Code: */

#include <cstdlib>
#include <ncurses.h>
#include "well.hpp"
#include <unistd.h>

well_t *init_well(int upper_left_x, int upper_left_y, int width, int height) {
  well_t *w;
  w = (well_t*) malloc(sizeof(well_t));
  w->upper_left_x = upper_left_x;
  w->upper_left_y = upper_left_y;
  w->width = width;
  w->height = height;
  w->draw_char = '#';
  w->color[0] = 0;
  w->color[1] = 0;
  w->color[2] = 0;
  return (w);
}

void draw_well(well_t *w) {
  int row_counter, column_counter;
  // Draw left side of well
  for (column_counter=w->upper_left_y;column_counter<=(w->upper_left_y + w->height);column_counter++) {
    mvprintw(column_counter,w->upper_left_x,"%c",w->draw_char);
  }

  // Draw right side of well
  for (column_counter=w->upper_left_y;column_counter<=(w->upper_left_y + w->height);column_counter++) {
    mvprintw(column_counter,(w->upper_left_x + w->width),"%c",w->draw_char);
  }
  
  // Draw Bottom of well 
  for (row_counter=w->upper_left_x;row_counter<=(w->upper_left_x + w->width);row_counter++) {
    mvprintw(w->upper_left_y + w->height,row_counter,"%c",w->draw_char);
  }
}

well_t *changeWellSize(int upper_left_x, int upper_left_y, int width, int height, well_t *w) {

  w->upper_left_x = upper_left_x;
  w->upper_left_y = upper_left_y;
  if(width < 10)
     w->width = 10;
  else
     w->width = width;

  w->height = height;
  
  return (w);
}

void undraw_well(well_t *w) {
  int row_counter, column_counter;
  // Undraw left side of well
  for (column_counter=w->upper_left_y;column_counter<=(w->upper_left_y + w->height);column_counter++) {
    mvprintw(column_counter,w->upper_left_x," ",w->draw_char);
  }

  // Undraw right side of well
  for (column_counter=w->upper_left_y;column_counter<=(w->upper_left_y + w->height);column_counter++) {
    mvprintw(column_counter,(w->upper_left_x + w->width)," ",w->draw_char);
  }
  
  // Undraw Bottom of well 
  for (row_counter=w->upper_left_x;row_counter<=(w->upper_left_x + w->width);row_counter++) {
    mvprintw(w->upper_left_y + w->height,row_counter," ",w->draw_char);
  }
}

void redraw(well_t * well, int row) {
  chtype row_buf[25];
  chtype curr;
  for (int r = row; r > -1; r--) {
    int num_chars = mvinchnstr(well->upper_left_y+r, well->upper_left_x+1, row_buf, 24);
    for (int x = 1; x < 25; x++) {
      if (r == 0) {
        mvprintw(well->upper_left_y+r, well->upper_left_x+x, " ", " ");
      }
      else {
        if (r != row) {
          if(row_buf[x-1] == '%') { 
            mvprintw(well->upper_left_y+r+1, well->upper_left_x+x, "%", "%");
          }
          else if(row_buf[x-1] == ' ') {
            mvprintw(well->upper_left_y+r+1, well->upper_left_x+x, " ", " ");
          }
          else {
            mvprintw(well->upper_left_y+r+1, well->upper_left_x+x, " ", " ");
          }
        }
      }
    }
  }
}

bool prune_well(well_t * well) {
  chtype row_buf[25];
  bool del;
  for (int r = 21; r > - 1; r--) {
    int num_chars = mvinchnstr(well->upper_left_y+r, well->upper_left_x+1, row_buf, 24);
    del = true;
    for (int x = 1; x < 25; x++) {
      if(row_buf[x-1] == ' ') {
        del = false;
      }
    }
    if (del) {
      redraw(well, r);
      usleep(100000);
      return true;
    }
  }  
  return false;
}

void clear_well(well_t * well) {
  for (int r = 0; r < well->height; r++) {
    for (int c = 1; c < well->width; c++) {
      mvprintw(well->upper_left_y+r, well->upper_left_x+c, " ", " ");
    }
  }
}

/* well.cpp ends here */