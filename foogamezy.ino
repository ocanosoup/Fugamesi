#include <Wire.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "rgb_lcd.h"

#define MIC A0
#define BUT 2
#define TOUCH 5

//global variables
rgb_lcd lcd;
int leng = 0;
char *in;

struct beatInput{
  long sum;
  long average;
  int *average_list;
  int count;
  unsigned long currentMillis;
};
typedef struct beatInput beatInput;

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
  beatInput *input;
  song *songs;
  note **output;
};
typedef struct game game;

beatInput *createInput(){
  beatInput *newIn = (beatInput*)malloc(sizeof(beatInput));
  newIn->sum = 0;
  newIn->average = 0;
  newIn->average_list = (int*)calloc(20, sizeof(int));
  newIn->count = 0;
  newIn->currentMillis = millis();
  return newIn;
}

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


// n, m, o
rhythm *fromBeatmap(rhythm *emptyRhythm, char *input, int size) {
  int i = 0, len = leng;
  note *last, *head;
  last = head = emptyRhythm->head;
  for (i = 0; i < len; i++) {
    if (input[i] == 'n'){
      makeBeat(head);
     //head->next->left_nearby = head;
      //last->right_nearby = head;
      //last = head;
      head = head->next;
    }
    else {
      head = head->next;
    }
  }
  return emptyRhythm;
}
rhythm *randomRhythm(rhythm *emptyRhythm) {
  int i, size;
  note *last, *head;
  size = emptyRhythm->size;
  head = last = emptyRhythm->head;
  
  srand(time(NULL));
  for (i = 0; i < size; i++) {
    if (rand()%2) {
    
    }
  }
  return emptyRhythm;
}

song *createSong(int rhythms, int bpm) {
  song *new_song;
  new_song = (song*)malloc(sizeof(song));
  new_song->rhythms = (rhythm**)malloc((sizeof(rhythm*)*rhythms));
  new_song->rhythms[0] = fromBeatmap(createEmptyRhythm(leng), in, leng);
  new_song->num_rhythms = 1;
  new_song->bpm = bpm;
  return new_song;
}

game *createGame(int rhythms, int bpm) {
  int i;
  game *new_game = (game*)malloc(sizeof(game));
  new_game->songs = createSong(1, bpm);
  new_game->output = (note**)malloc(sizeof(note*)*leng);
    for (i = 0; i < 16; i++)
      new_game->output[i] = createNote();
  new_game->input = createInput();
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
    hit( current_game, 1, current_game->songs->bpm);
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
    hit( current_game, 1, current_game->songs->bpm);
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
  int pressed = 0, i;
  unsigned long int current_millis;
  beatInput *in = current_game->input;
  
  current_millis = millis();
  while(millis() - current_millis < span) {
    in->sum = 0;
    for (i = 0; i < 32; i++)
      in->sum += analogRead(MIC);
    in->sum >>= 5;
    in->average = in->average - in->average_list[in->count %3]/3 + in->sum/3;
    in->average_list[in->count%3] = in->sum;
    in->count += 1;
    if (!pressed && in->count > 3 && in->sum > 500) {
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

char *listening() {
  long average = 0;
  int cnt = 0;
  int avg[20];
  boolean listening = 1;
  unsigned long currentMillis = 1;
  unsigned long* times = (unsigned long*)(malloc(sizeof(unsigned long)*200));
  char* beatmap = (char*)(malloc(sizeof(char)*200));
  int curr = 0;
  int step;
  leng = 0;
  listening = 1;
  
  for (int i = 0; i<20; i++)
  {
    avg[i] = 0;
  }
  step = 350;

  while(1){
  if ((digitalRead(TOUCH) && listening))
  {
    listening = 0;
    //delay(1000);
  }
  long sum = 0;
  for(int i=0; i<32; i++)
  {
    sum += analogRead(MIC);
  }
  sum >>= 5;

  average = average-avg[cnt %20]/20 + sum/20;
  avg[cnt % 20] = sum;
  cnt = cnt + 1;
  if(cnt > 20 && listening)
  {
    if(sum > 750)
    {
      if(currentMillis != 0) {
        times[curr] = millis()-currentMillis;
        currentMillis = millis();
        curr = curr + 1;
      }
      else {
        currentMillis = millis();
      }
    } 
    }
    else if(cnt >20) {
    //Start decoding this nonsense
    int maxtimes = curr;
    curr = 0;
    for(curr = 0; curr < maxtimes; curr++) {
      while(times[curr] > 1.5*step) {
        strcat(beatmap,"m");     
        leng = leng+1;
        times[curr] = times[curr] - step;
      }
      if(.5*step <= times[curr] && times[curr]<= 1.5*step) {
        leng = leng+2;
        strcat(beatmap,"on");
      }
      else if(0 <= times[curr] && times[curr] < .5*step) {
        leng = leng+1;
        strcat(beatmap,"n");
      }
    }
    free(times);
    return beatmap;
    
    }
  }
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
  in = listening();
  game *test = createGame(1, 350);
  lcd.setCursor(0,0);
  startCycle(test);
  cycleRhythm(test, 100);
  exit(1);
}
