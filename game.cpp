/* game.cpp --- 
 * 
 * Filename: game.cpp
 * Description: 
 * Author: Bryce
 * Maintainer: Adeel Bhutta
 * Created: Tue Sep  6 11:08:59 2016
 * Last-Updated: 01-10-2021
 *           By: Adeel Bhutta
 *     Update #: 20
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

#define _GLIBCXX_USE_CXX11_ABI 0

#include <unistd.h> 
#include <ncurses.h>
#include <ctime>
#include <cstdlib>
#include "game.hpp"
#include "well.hpp"
#include "tetris.hpp"
#include "tetromino.hpp"
#include "key.hpp"
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <time.h>
#include <ctime>

#define YELLOW_BLOCK 1
#define BLUE_BLOCK 2
#define CYAN_BLOCK 3
#define RED_BLOCK 4
#define GREEN_BLOCK 5
#define MAGENTA_BLOCK 6

using namespace std;

bool pauseGame = false;          
int lives = 4;
string in_name;
char * name;
char * lives_cstr;
int dropChance;
int dropRate;
bool spaceKey = false;
tetromino* temp_next_ptr = (tetromino*) malloc(sizeof(tetromino_t));
int score = 0;
int level = EASY;
string levelResp;
vector<int> highScores;
vector<string> names;
time_t start;
time_t now;

void init_game(void) {
  int x,y;
}

void displaySideText(tetromino *temp_next_ptr, well *w, int next_color_pair) {
  mvprintw(w->upper_left_y+2, w->upper_left_x+(w->width/2)+20, "next piece:");
  move_tet(temp_next_ptr, w->upper_left_x+(w->width/2)+20, w->upper_left_y + 4);
  attron(COLOR_PAIR(next_color_pair));
  display_tetromino(temp_next_ptr); 
  attroff(COLOR_PAIR(next_color_pair));
  
  stringstream ss;
  ss << lives;
  string livesString = "";
  ss >> livesString;

  stringstream ss2;
  ss2 << score;
  string scoreString = "";
  ss2 >> scoreString;
   
  now = (time (NULL) - start) % 60;
  ss << now;
  string timeString = "";
  ss>> timeString;

  string time_str("time: " + timeString);
  char* time_cstr = new char[time_str.length()+1];
  strcpy(time_cstr, time_str.c_str());

  string lives_str ("lives: " + livesString);
  lives_cstr = new char [lives_str.length()+1];
  strcpy (lives_cstr, lives_str.c_str());

  string score_str ("score: " + scoreString);
  char* score_cstr = new char [score_str.length()+1];
  strcpy (score_cstr, score_str.c_str());

  mvprintw(w->upper_left_y+2, w->upper_left_x+(w->width/2)-30, name); 
  mvprintw(w->upper_left_y+4, w->upper_left_x+(w->width/2)-30, lives_cstr); 
  mvprintw(w->upper_left_y+10, w->upper_left_x+(w->width/2)-30, score_cstr); 
}

void displayTime(well* w) {
 now = (time(NULL) - start);
 mvprintw(w->upper_left_y+6, w->upper_left_x+(w->width/2)-40, "%+04d:%+03d", now / 60 , now % 60);
}

void undisplaySideText(tetromino *temp_next_ptr, well *w) {
  undisplay_tetromino(temp_next_ptr); 
}

void initSideText() {
  cout << "Please enter your name:" << "\n";
  cin>>in_name;
  cout << "Please enter a level of difficulty" << "\n"; // ADD exception handeling
  cin >> levelResp;
  if (levelResp == "easy") {
    level = EASY;
  }
  else if (levelResp == "medium") {
    level = MED;
  }
  else if (levelResp == "hard") {
    level = HARD;
  }
  else if (levelResp == "testing") {
    level = TESTING;
  }
  else {
    cout << "Wrong input, level will default to easy" << "\n";
    sleep(1);
    level = EASY;
  }
  string str ("name: " + in_name);
  name = new char [str.length()+1];
  strcpy (name, str.c_str());
  stringstream ss;
  ss << lives;
  string livesString = "";
  ss >> livesString;


  string lives_str ("lives: " + livesString);
  lives_cstr = new char [lives_str.length()+1];
  strcpy (lives_cstr, lives_str.c_str());
}

void toBottom() {
  dropRate = 0;
}

void printHigh() {
  for (int i = 0; i < 5; i++) {
    cout << highScores.at(i) << ", " << names.at(i) << "\n";
  }
}

int checkPrune(well_t * well) {
  if (prune_well(well)) {
    return 1 + checkPrune(well);
  }
  else {
    return 0;
  }
}

int compute_score(int previous_score, int lines_cleared) {
  return previous_score + pow(lines_cleared, 2);
}

void checkHigh() {
  ifstream infile;
  infile.open("high.txt");
  string line;
  bool replaced = false;
  string line_num = "";
  int index = 0;

  while(infile) {
    getline(infile, line);
    index = 0;
    for (int i = 0; i < line.size(); i++) {
      char c = line[i];
      if (c == ',') {
        break;
      }
      index++;
    }
    if (line.size() > 0) {
      string elt_string = line.substr(0, index);
      int elt;
      stringstream ss;
      ss << elt_string;
      ss >> elt;
      highScores.push_back(elt);
      names.push_back(line.substr(index+1, 999));
    }
  }

  for (int i = 0; i < 5; i++) {
    if (score > highScores.at(i) && !replaced) {
      highScores.insert(highScores.begin()+i, score);
      names.insert(names.begin()+i, in_name);
      replaced = true;
    }
  }

  fstream outfile;
  outfile.open("high.txt", ios_base::out);
  string out = "";
  for (int i = 0; i < 5; i++) {
    int tempScore = highScores.at(i);
    string stringScore;
    stringstream ss;
    ss << tempScore;
    ss >> stringScore;

    out = out + stringScore + ", " + names.at(i) + "\n";
  }
  outfile.write(out.data(), out.size());

  outfile.close();
  infile.close();
}

void initialize_colors() {
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_YELLOW);
  init_pair(2, COLOR_BLUE, COLOR_BLUE);
  init_pair(3, COLOR_CYAN, COLOR_CYAN);
  init_pair(4, COLOR_RED, COLOR_RED);
  init_pair(5, COLOR_GREEN, COLOR_GREEN);
  init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA);
}

void endScreen() {
  cout << "GAME OVER" << endl;
  cout << "Final Score: " << score << endl;
  checkHigh();
  
  printHigh();
  sleep(5);
}

int game(void) {
  srand(time(0));
  static int state = INIT;
  well_t *w;
  int x,y;
  int c;
  int arrow;
  struct timespec tim = {0,1000000};  // Each execution of while(1) is approximately 1mS
  struct timespec tim_ret;
  int move_counter = 0;
  int move_timeout = BASE_FALL_RATE;
  int current_color_pair;
  int next_color_pair;

  initSideText();
  start = time(NULL);
  tetromino_t *next = create_tetromino((w->upper_left_x+(w->width/2)), w->upper_left_y, level);
  tetromino_t *current = create_tetromino((w->upper_left_x+(w->width/2)), w->upper_left_y, level);
  while(1) {
    switch(state) {
    case INIT:               // Initialize the game, only run one time 
      initscr();
      curs_set(0);
      nodelay(stdscr,TRUE);  // Do not wait for characters using getch.  
      noecho();              // Do not echo input characters 
      if (has_colors() == FALSE) {
       printf("Your terminal does not support color\n");
       state = GAME_OVER;
      }
      else {
        start_color();
        initialize_colors();
      }
      getmaxyx(stdscr,y,x);  // Get the screen dimensions 

      w = init_well(((x/2)-(WELL_WIDTH/2)),1,WELL_WIDTH,WELL_HEIGHT);
      draw_well(w);
      srand(time(NULL));     // Seed the random number generator with the time. Used in create tet. 
      state = ADD_PIECE;
      break;
    case ADD_PIECE:          // Add a new piece to the game
      if (next) {
	      current = next;
        current_color_pair = next_color_pair;
        next_color_pair = rand() % 6 + 1;
	      next = create_tetromino ((w->upper_left_x+(w->width/2)), w->upper_left_y, level);
      }
      else {
	      current = create_tetromino ((w->upper_left_x+(w->width/2)), w->upper_left_y, level);
	      next = create_tetromino ((w->upper_left_x+(w->width/2)), w->upper_left_y, level);
        current_color_pair = rand() % 6 + 1;
        next_color_pair = rand() % 6 + 1;
      }
      attron(COLOR_PAIR(current_color_pair));
      display_tetromino(current);
      attroff(COLOR_PAIR(current_color_pair));
      undisplaySideText(temp_next_ptr, w);
      memcpy(temp_next_ptr, next, sizeof(tetromino_t));
      spaceKey = false;
      undisplaySideText(temp_next_ptr, w);
      memcpy(temp_next_ptr, next, sizeof(tetromino_t));
      spaceKey = false;
      displaySideText(temp_next_ptr, w, next_color_pair);
      state = MOVE_PIECE;
      switch(level) {
        case EASY:
          dropChance = rand() % 2; // 0 - 2
        case MED:
          dropChance = 1+ (rand() % 4); // 1 - 4
        case HARD:
          dropChance = 2 + (rand() % 4); // 2 - 5
        case TESTING:
          dropChance = 0;
      }
      switch(dropChance) {
          case 0:
            dropRate = 1000;
            break;
          case 1:
            dropRate = 750;
            break;
          case 2:
            dropRate = 500;
            break;
          case 3:
            dropRate = 400;
            break;
          case 4:
            dropRate = 300;
            break;
          case 5:
            dropRate = 200;
            break;
      }
      break;
     
    case MOVE_PIECE:         // Move the current piece
      displayTime(w); 
      if(move_counter % dropRate == 0 || spaceKey) {
        undisplay_tetromino(current);
        move_tet(current, current->upper_left_x, (current->upper_left_y)+1); 
        int testCase = move_tet(current, current->upper_left_x, (current->upper_left_y)+1);
        attron(COLOR_PAIR(current_color_pair));
        display_tetromino(current);
        attroff(COLOR_PAIR(current_color_pair));
        if (testCase == COLLIDE) {
          if (current->upper_left_y <= 1) {
            state = GAME_OVER;
          }
          else {
            int change = checkPrune(w);
            if (change > 0) {
              score = compute_score(score, change);
            }
            state = ADD_PIECE;
          }
        }
      }
    
      if ((arrow = read_escape(&c)) != NOCHAR) {
        switch (arrow) {
          case UP:
            mvprintw(10,10,"UP            ");
            undisplay_tetromino(current);
            rotate_cw(current);
            attron(COLOR_PAIR(current_color_pair));
            display_tetromino(current);
            attroff(COLOR_PAIR(current_color_pair));
            break;
          case DOWN:
            mvprintw(10,10,"DOWN          ");
            undisplay_tetromino(current);
            rotate_ccw(current);
            attron(COLOR_PAIR(current_color_pair));
            display_tetromino(current);
            attroff(COLOR_PAIR(current_color_pair));
            break;
          case LEFT:
            mvprintw(10,10,"LEFT          ");
            undisplay_tetromino(current);
            move_tet(current, (current->upper_left_x)-1, current->upper_left_y);
            attron(COLOR_PAIR(current_color_pair));
            display_tetromino(current);
            attroff(COLOR_PAIR(current_color_pair));
            break;
          case RIGHT:
            mvprintw(10,10,"RIGHT         ");
            undisplay_tetromino(current);
            move_tet(current, (current->upper_left_x)+1, current->upper_left_y);
            attron(COLOR_PAIR(current_color_pair));
            display_tetromino(current);
            attroff(COLOR_PAIR(current_color_pair));
            break;
          case REGCHAR: 
            mvprintw(10,10,"REGCHAR 0x%02x",c);
            if (c == 'q' || c == 'x') {
              state = GAME_OVER;
            }
            else if (c == ' ') {
              spaceKey = true;
            }
            else if (c == 'p') {
              pauseGame = !pauseGame;
            }
            else if (c == 'r') {
            }
        }
      } 
      if (move_counter++ >= move_timeout) {
	      move_counter = 0;
      }
      break;
  case GAME_OVER:
    lives--;
    if (lives > 0) {
      clear_well(w);
      state = INIT;
    }
    else {
      state = EXIT;
    } 
    break;
  case EXIT:
      endwin();
      system("clear");
      endScreen();
      return(0);
      break;
    }
    while(pauseGame) {
      if ((arrow = read_escape(&c)) != NOCHAR) {
      switch (arrow) {
      case REGCHAR:
      if (c == 'p') {
            pauseGame = !pauseGame;
          }
        } 
      }
    }
    refresh();
    nanosleep(&tim,&tim_ret);
  }
}


/* game.cpp ends here */
