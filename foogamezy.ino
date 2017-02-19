#include <Wire.h>
#include <time.h>
#include "rgb_lcd.h"

#define BUT 2

rgb_lcd lcd;

// Note struct, node of a linked list that also points to nearby beats
struct note {
  struct note *next;
  struct note *left_nearby;
  struct note *right_nearby;
  char shape;
  
};
typedef struct note note;

// This is a linked list struct
struct rhythm {
  note *head;
  int size;
};
typedef struct rhythm rhythm;

// Song struct, holds rhythms and bpm
struct song {
  struct rhythm **rhythms;
  int num_rhythms;
  int bpm;
};
typedef struct song song;

// Game struct, holds most everything needed fgame logic
struct game {
  int score;
  int combo;
  int hits;
  int offtime;
  int misses;
  song *songs;
  note **output;
};
typedef struct game game;


note *createNote() {
  
  note *head;
  if ((head = (note *)malloc(sizeof(note))) == NULL)
    exit(0);
  head->next = NULL;
  head->left_nearby = NULL;
  head->right_nearby = NULL;
  head->shape = 20;
  return head;
}

note *makeBeat(note *beat) {
  beat->left_nearby = beat;
  beat->right_nearby = beat;
  beat->shape = 220;
  return beat;
}


rhythm *createEmptyRhythm(int size) {
  int i;
  rhythm *new_rhythm;
  note *head, *temp;
  new_rhythm = (rhythm*)malloc(sizeof(rhythm));
  new_rhythm->size = size;
  head = createNote();
  temp = head;
  for (i = 1; i < size; i++) {
    temp->next = createNote();
    temp = temp->next;
  }
  new_rhythm->head = head;
  temp->next = head;
  return new_rhythm;
}

rhythm *alternatingRhythm(rhythm *emptyRhythm) {
  int i = 0, size = emptyRhythm->size;
  note *last, *head = emptyRhythm->head;
  last = head;
  for (i = 0; i < size; i++) {
    if (i%2) {
      head->left_nearby = last;
      head->right_nearby = head->next;
    }
    else {
      head = makeBeat(head);
      last = head;
    }
    head = head->next;
  }
  head = head->next;
  return emptyRhythm;
}

rhythm *randomRhythm(rhythm *emptyRhythm) {
  
}

song *createSong(int rhythms, int bpm) {
  song *new_song;
  new_song = (song*)malloc(sizeof(song));
  new_song->rhythms = (rhythm**)malloc((sizeof(rhythm*)*rhythms));
  new_song->rhythms[0] = alternatingRhythm(createEmptyRhythm(16));
  new_song->num_rhythms = 1;
  new_song->bpm = bpm;
  return new_song;
}

game *createGame(int rhythms, int bpm) {
  int i;
  game *new_game = (game*)malloc(sizeof(game));
  new_game->songs = createSong(1, 300);
  new_game->output = (note**)malloc(sizeof(note*)*16);
    for (i = 0; i < 16; i++)
      new_game->output[i] = createNote();
  new_game->score = 0;
  new_game->combo = 0;
  new_game->hits = 0;
  new_game->offtime = 0;
  new_game->misses = 0;
  return new_game;
}

void startCycle(game *current_game){
  int i, k;
  i = 15;
  note *head = current_game->songs->rhythms[0]->head;
  lcd.setCursor(0,0);
  current_game->output[i] = head;
  for (k = 15; k >= 0; k--) {
    for (i = k; i < 15; i++) {
      current_game->output[i] = current_game->output[i+1];
    }
    current_game->output[i] = current_game->output[i]->next;
    drawOutput(current_game);
    hit( current_game, 1, 400);
  }
}

void cycleRhythm(game *current_game, int cycles) {
  int i = 15;
  note *head = current_game->songs->rhythms[0]->head;
  current_game->output[i] = head;
  while(cycles){
    for (i = 0; i < 15; i++) {
      current_game->output[i] = current_game->output[i+1];
    }
    current_game->output[i] = head->next;
    head = head->next;
    drawOutput(current_game);
    hit( current_game, 1, 300);
    cycles--;
  }
}

void drawOutput(game* current_game) {
  int i;
  lcd.setCursor(1,0);
  lcd.print('v');
  lcd.setCursor(5, 0);
  lcd.print(current_game->score);
  lcd.setCursor(14, 0);
  lcd.print(current_game->combo);
  lcd.print("  ");
  for (i = 15; i >= 0; i--) {
    lcd.setCursor(i, 1);
    lcd.print((current_game->output[i] == NULL) ? ' ' : current_game->output[i]->shape);
  }
  lcd.setRGB(220, 220, 230);
}

void hit(game *current_game, int index, int span) {
  int pressed = 0;
  unsigned long int current_millis;
  current_millis = millis();
  while(millis() - current_millis < span) {
    if (!pressed && digitalRead(BUT)) {
      pressed = 1;
      lcd.setCursor(index, 0);
      if (current_game->output[index]->right_nearby == current_game->output[index]) {
        current_game->combo++;
        lcd.setRGB(20, 255, 60);
        current_game->score += (current_game->combo) ? 3*current_game->combo : 3;
      }
      else {
        current_game->combo = 0;
        lcd.setRGB(255, 10, 10);
        lcd.print('?');
      }
    }
  }
}

void printNote(note *printer) {
  lcd.setCursor(0,0);
  lcd.print(printer->shape);
}

void nextNote(game *current_game) {
  
}

void setup() {
  pinMode(BUT, INPUT);
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("S: ");
  lcd.setCursor(12,0);
  lcd.print("C: ");
  delay(100);
  lcd.setRGB(220, 220, 230);

}

void loop() {
  game *test = createGame(1, 300);
  lcd.setCursor(0,0);
  startCycle(test);
  cycleRhythm(test, 100);
  exit(1);
}
