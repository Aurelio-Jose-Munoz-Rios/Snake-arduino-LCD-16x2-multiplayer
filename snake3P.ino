/*
   Juego Snake para dos o tres jugadores en Arduino

   Libraries :
    - LiquidCrystal.h - https://docs.arduino.cc/libraries/liquidcrystal/

   Aurelio M. - v1.1 - 09.2024
   If you like this project, please add a star! Thank you!
   https://github.com/Aurelio-Jose-Munoz-Rios/Snake-arduino-LCD-16x2-multiplayer
 */

#include <LiquidCrystal.h>

// ======================================================================
// ==================== DEFINICIONES DE MÚSICA ==========================
// ======================================================================

// Pin donde está conectado el zumbador
#define SPEAKER_PIN 9

// Definición de las frecuencias de las notas (en Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760

// --- ETAPA 1: INTRODUCCIÓN (Menú) ---
int introMelody[] = {
  NOTE_C5, NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_G5, NOTE_E5,
  NOTE_A4, NOTE_E4, NOTE_A4, NOTE_C5, NOTE_F5, NOTE_A5, NOTE_F5, NOTE_C5,
  NOTE_G4, NOTE_D4, NOTE_G4, NOTE_B4, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_B4,
  NOTE_F4, NOTE_C4, NOTE_F4, NOTE_A4, NOTE_D5, NOTE_F5, NOTE_D5, NOTE_A4
};
int introDurations[] = {
  150, 150, 150, 200, 200, 300, 200, 200,
  150, 150, 150, 200, 200, 300, 200, 200,
  150, 150, 150, 200, 200, 300, 200, 200,
  150, 150, 150, 200, 200, 300, 200, 200
};

// --- ETAPA 3: PERSECUCIÓN (Juego Principal) ---
int chaseMelody[] = {
  NOTE_C5, NOTE_C6, NOTE_C5, NOTE_C6, NOTE_G5, NOTE_C6, NOTE_G5, NOTE_C6,
  NOTE_D5, NOTE_D6, NOTE_D5, NOTE_D6, NOTE_A5, NOTE_D6, NOTE_A5, NOTE_D6,
  NOTE_E5, NOTE_E6, NOTE_E5, NOTE_E6, NOTE_B5, NOTE_E6, NOTE_B5, NOTE_E6,
  NOTE_F5, NOTE_F6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G5
};
int chaseDurations[] = {
  100, 120, 100, 120, 100, 120, 100, 120,
  100, 120, 100, 120, 100, 120, 100, 120,
  100, 120, 100, 120, 100, 120, 100, 120,
  150, 150, 150, 150, 150, 150, 150, 150
};

// --- ETAPA 5: GAME OVER ---
int gameOverMelody[] = {
  NOTE_G5, 0, NOTE_E5, 0, NOTE_C5, 0, NOTE_G4,
  NOTE_A4, 0, NOTE_F4, 0, NOTE_D4, 0, NOTE_C4,
  NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4
};
int gameOverDurations[] = {
  400, 100, 400, 100, 400, 100, 800,
  400, 100, 400, 100, 400, 100, 800,
  600, 600, 600, 1200
};

// --- VARIABLES PARA MÚSICA NO BLOQUEANTE ---
const int* currentMelody = nullptr;
const int* currentDurations = nullptr;
int totalNotes = 0;
int currentNoteIndex = 0;
unsigned long noteStartTime = 0;
boolean loopMelody = false;
boolean musicPlaying = false;


// --- PINES PARA JOYSTICKS ---
#define JOYSTICK1_X_PIN  A5
#define JOYSTICK1_Y_PIN  A4
#define JOYSTICK1_SW_PIN 8   // Pin para el botón del Joystick 1

#define JOYSTICK2_X_PIN A1
#define JOYSTICK2_Y_PIN A0

#define JOYSTICK3_X_PIN A3 // Pin para el Joystick 3 (Eje X)
#define JOYSTICK3_Y_PIN A2 // Pin para el Joystick 3 (Eje Y)

// --- DEFINICIONES DEL JUEGO ---
#define JOYSTICK_RELEASED 0
#define JOYSTICK_UP_      1
#define JOYSTICK_DOWN_    2
#define JOYSTICK_LEFT_    3
#define JOYSTICK_RIGHT_   4

#define ROWS 2
#define MAX_SNAKE_LENGTH 50

// --- CONFIGURACIÓN DE LA PANTALLA LCD ---
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// --- VARIABLES GLOBALES DEL JUEGO ---
unsigned long time1, time2, time3;
int gameSpeed1 = 10;
int gameSpeed2 = 10;
int gameSpeed3 = 10;
boolean gameOver = false;
int numPlayers = 2; // Inicia con 2 jugadores por defecto

byte key1 = 0, oldkey1 = 0;
byte key2 = 0, oldkey2 = 0;
byte key3 = 0, oldkey3 = 0;

byte Field[8 * ROWS * 16];

// --- ESTRUCTURA DE LA SERPIENTE (LISTA ENLAZADA) ---
struct partdef {
  int row, column, dir;
  struct partdef *next;
};
typedef partdef part;

// --- VARIABLES PARA SERPIENTE 1 ---
part *head1, *tail1;
int collected1;

// --- VARIABLES PARA SERPIENTE 2 ---
part *head2, *tail2;
int collected2;

// --- VARIABLES PARA SERPIENTE 3 ---
part *head3, *tail3;
int collected3;

// --- VARIABLES PARA LA COMIDA ---
int foodRow, foodCol;

// --- DECLARACIÓN DE FUNCIONES ---
void ChangeDot(unsigned int RowVal, unsigned int ColVal, boolean NewVal);
byte ScanJoyStick(int xPin, int yPin);
boolean moveSnake(part *&head, part *&tail, part *head_other1, part *tail_other1, part *head_other2, part *tail_other2, int &collected);
void playStage(int melody[], int durations[], int notes);
void startMelody(const int melody[], const int durations[], int notes, boolean shouldLoop);
void stopMusic();
void updateMusic();


// ======================================================================
// ========= DIBUJA EL ESTADO ACTUAL DEL JUEGO EN LA PANTALLA ===========
// ======================================================================
void drawMatrix() {
  byte myChar[8];
  boolean special;
  int cc = 0;

  if (gameOver) return;

  // Dibuja la comida
  ChangeDot(foodRow, foodCol, true);

  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < 16; c++) {
      special = false;
      for (int i = 0; i < 8; i++) {
        byte b = B00000;
        if ((Field[16 * (r * 8 + i) + (c * 5 + 0) / 5] & (1 << ((c * 5 + 0) % 5)))) { b += B10000; special = true; }
        if ((Field[16 * (r * 8 + i) + (c * 5 + 1) / 5] & (1 << ((c * 5 + 1) % 5)))) { b += B01000; special = true; }
        if ((Field[16 * (r * 8 + i) + (c * 5 + 2) / 5] & (1 << ((c * 5 + 2) % 5)))) { b += B00100; special = true; }
        if ((Field[16 * (r * 8 + i) + (c * 5 + 3) / 5] & (1 << ((c * 5 + 3) % 5)))) { b += B00010; special = true; }
        if ((Field[16 * (r * 8 + i) + (c * 5 + 4) / 5] & (1 << ((c * 5 + 4) % 5)))) { b += B00001; special = true; }
        myChar[i] = b;
      }

      if (special) {
        lcd.createChar(cc, myChar);
        lcd.setCursor(c, r);
        lcd.write(byte(cc));
        cc++;
      } else {
        lcd.setCursor(c, r);
        lcd.write(128);
      }
    }
  }
}

// ======================================================================
// ======== LIBERA LA MEMORIA USADA POR UNA SERPIENTE ===================
// ======================================================================
void freeList(part *&head, part *&tail) {
  part *p, *q;
  p = tail;
  while (p != NULL) {
    q = p;
    p = p->next;
    free(q);
  }
  head = tail = NULL;
}

// ======================================================================
// ============= MUESTRA LA PANTALLA DE GAME OVER =======================
// ======================================================================
void gameOverFunction() {
  stopMusic(); // Detiene la música del juego
  playStage(gameOverMelody, gameOverDurations, sizeof(gameOverMelody) / sizeof(int)); // Reproduce melodía final

  lcd.clear();
  freeList(head1, tail1);
  freeList(head2, tail2);
  if (numPlayers == 3) {
    freeList(head3, tail3);
  }

  lcd.setCursor(3, 0);
  lcd.print("Game Over!");
  lcd.setCursor(1, 1);

  if (numPlayers == 2) {
    if (collected1 > collected2) {
      lcd.print("Gana Jugador 1!");
    } else if (collected2 > collected1) {
      lcd.print("Gana Jugador 2!");
    } else {
      lcd.print("      Empate!      ");
    }
  } else {
    if (collected1 > collected2 && collected1 > collected3) {
      lcd.print("Gana Jugador 1!");
    } else if (collected2 > collected1 && collected2 > collected3) {
      lcd.print("Gana Jugador 2!");
    } else if (collected3 > collected1 && collected3 > collected2) {
      lcd.print("Gana Jugador 3!");
    } else {
      lcd.print("      Empate!      ");
    }
  }
  
  while(true) {
    // Espera a que se reinicie el Arduino
  }
}

// ======================================================================
// ===== AÑADE UN NUEVO SEGMENTO A LA COLA DE LA SERPIENTE ==============
// ======================================================================
void growSnake(part *&tail) {
  part *p = (part*)malloc(sizeof(part));
  p->row = tail->row;
  p->column = tail->column;
  p->dir = tail->dir;
  p->next = tail;
  tail = p;
}

// ======================================================================
// =========== GENERA UNA NUEVA POSICIÓN PARA LA COMIDA =================
// ======================================================================
void newFood() {
  boolean positionOk = false;
  while (!positionOk) {
    foodRow = random(8 * ROWS);
    foodCol = random(80);
    positionOk = true;
    part *p = tail1;
    while (p != NULL) {
      if (p->row == foodRow && p->column == foodCol) {
        positionOk = false;
        break;
      }
      p = p->next;
    }
    if (!positionOk) continue;
    p = tail2;
    while (p != NULL) {
      if (p->row == foodRow && p->column == foodCol) {
        positionOk = false;
        break;
      }
      p = p->next;
    }
    if (!positionOk) continue;
    if (numPlayers == 3) {
      p = tail3;
      while (p != NULL) {
        if (p->row == foodRow && p->column == foodCol) {
          positionOk = false;
          break;
        }
        p = p->next;
      }
    }
  }
}

// ======================================================================
// MUEVE LA SERPIENTE, VERIFICA COLISIONES Y DEVUELVE TRUE SI COMIÓ
// ======================================================================
boolean moveSnake(part *&head, part *&tail, part *head_other1, part *tail_other1, part *head_other2, part *tail_other2, int &collected) {
  part *p = tail;
  while (p->next != NULL) {
    p->row = p->next->row;
    p->column = p->next->column;
    p->dir = p->next->dir;
    p = p->next;
  }
  switch (head->dir) {
    case JOYSTICK_UP_:    head->row--; break;
    case JOYSTICK_DOWN_:  head->row++; break;
    case JOYSTICK_RIGHT_: head->column++; break;
    case JOYSTICK_LEFT_:  head->column--; break;
  }
  if (head->column >= 80) head->column = 0;
  if (head->column < 0) head->column = 79;
  if (head->row >= (8 * ROWS)) head->row = 0;
  if (head->row < 0) head->row = (8 * ROWS - 1);
  p = tail;
  while (p != head) {
    if (p->row == head->row && p->column == head->column) {
      gameOver = true;
      return false;
    }
    p = p->next;
  }
  if (head_other1 != NULL) {
    p = tail_other1;
    while (p != NULL) {
      if (p->row == head->row && p->column == head->column) {
        gameOver = true;
        return false;
      }
      p = p->next;
    }
  }
  if (head_other2 != NULL) {
    p = tail_other2;
    while (p != NULL) {
      if (p->row == head->row && p->column == head->column) {
        gameOver = true;
        return false;
      }
      p = p->next;
    }
  }
  if (head->row == foodRow && head->column == foodCol) {
    collected++;
    if (collected < MAX_SNAKE_LENGTH) {
      growSnake(tail);
    }
    newFood();
    return true;
  }
  return false;
}

// ======================================================================
// ================ CREA UNA SERPIENTE INICIAL ==========================
// ======================================================================
void createSnake(part *&head, part *&tail, int start_row, int start_col, int dir) {
  tail = (part*)malloc(sizeof(part));
  tail->row = start_row;
  tail->column = start_col + 1;
  tail->dir = dir;
  ChangeDot(tail->row, tail->column, true);
  part *p = (part*)malloc(sizeof(part));
  p->row = start_row;
  p->column = start_col;
  p->dir = dir;
  p->next = NULL;
  ChangeDot(p->row, p->column, true);
  tail->next = p;
  head = p;
}

// ======================================================================
// =================== MENÚ DE SELECCIÓN DE JUGADORES ===================
// ======================================================================
void playerSelectionMenu() {
  boolean selectionMade = false;
  lcd.clear();

  // Inicia la música del menú en bucle
  startMelody(introMelody, introDurations, sizeof(introMelody) / sizeof(int), true);

  while (!selectionMade) {
    updateMusic(); // Actualiza la música en cada ciclo para que no se detenga

    lcd.setCursor(0, 0);
    lcd.print("Elegir Jugadores");
    lcd.setCursor(0, 1);
    lcd.print("Jugadores: ");
    lcd.print(numPlayers);
    lcd.print("   ");

    if (digitalRead(JOYSTICK1_SW_PIN) == LOW) {
      numPlayers = (numPlayers == 2) ? 3 : 2;
      delay(200);
    }

    byte move = ScanJoyStick(JOYSTICK1_X_PIN, JOYSTICK1_Y_PIN);
    if (move == JOYSTICK_RIGHT_) {
      selectionMade = true;
    }
  }
  stopMusic(); // Detiene la música del menú al salir
}

// ======================================================================
// ================ CONFIGURACIÓN INICIAL DEL JUEGO =====================
// ======================================================================
void setup() {
  lcd.begin(16, ROWS);
  randomSeed(analogRead(A2));
  
  pinMode(SPEAKER_PIN, OUTPUT); // Configura el pin del zumbador
  pinMode(JOYSTICK1_SW_PIN, INPUT_PULLUP);

  playerSelectionMenu();

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(String(numPlayers) + "P Snake Game");
  lcd.setCursor(4, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();

  for (unsigned int i = 0; i < (8 * ROWS * 16); i++) {
    Field[i] = 0;
  }
  
  head1 = tail1 = head2 = tail2 = head3 = tail3 = NULL;
  createSnake(head1, tail1, 4, 20, JOYSTICK_LEFT_);
  createSnake(head2, tail2, 12, 60, JOYSTICK_LEFT_);
  collected1 = 0;
  collected2 = 0;
  if (numPlayers == 3) {
    createSnake(head3, tail3, 4, 60, JOYSTICK_LEFT_);
    collected3 = 0;
  }
  newFood();
  
  time1 = millis();
  time2 = millis();
  if (numPlayers == 3) {
    time3 = millis();
  }
  
  // Inicia la música del juego principal
  startMelody(chaseMelody, chaseDurations, sizeof(chaseMelody) / sizeof(int), true);
}

// ======================================================================
// ===================== BUCLE PRINCIPAL DEL JUEGO ======================
// ======================================================================
void loop() {
  updateMusic(); // La música del juego se actualiza constantemente

  if (gameOver) {
    gameOverFunction();
  }

  key1 = ScanJoyStick(JOYSTICK1_X_PIN, JOYSTICK1_Y_PIN);
  if (key1 != oldkey1) {
    delay(50);
    key1 = ScanJoyStick(JOYSTICK1_X_PIN, JOYSTICK1_Y_PIN);
    if (key1 != oldkey1) {
      oldkey1 = key1;
      if (key1 == JOYSTICK_RIGHT_ && head1->dir != JOYSTICK_LEFT_) head1->dir = key1;
      else if (key1 == JOYSTICK_LEFT_ && head1->dir != JOYSTICK_RIGHT_) head1->dir = key1;
      else if (key1 == JOYSTICK_UP_ && head1->dir != JOYSTICK_DOWN_) head1->dir = key1;
      else if (key1 == JOYSTICK_DOWN_ && head1->dir != JOYSTICK_UP_) head1->dir = key1;
    }
  }
  
  key2 = ScanJoyStick(JOYSTICK2_X_PIN, JOYSTICK2_Y_PIN);
  if (key2 != oldkey2) {
    delay(50);
    key2 = ScanJoyStick(JOYSTICK2_X_PIN, JOYSTICK2_Y_PIN);
    if (key2 != oldkey2) {
      oldkey2 = key2;
      if (key2 == JOYSTICK_RIGHT_ && head2->dir != JOYSTICK_LEFT_) head2->dir = key2;
      else if (key2 == JOYSTICK_LEFT_ && head2->dir != JOYSTICK_RIGHT_) head2->dir = key2;
      else if (key2 == JOYSTICK_UP_ && head2->dir != JOYSTICK_DOWN_) head2->dir = key2;
      else if (key2 == JOYSTICK_DOWN_ && head2->dir != JOYSTICK_UP_) head2->dir = key2;
    }
  }
  
  if (numPlayers == 3) {
    key3 = ScanJoyStick(JOYSTICK3_X_PIN, JOYSTICK3_Y_PIN);
    if (key3 != oldkey3) {
      delay(50);
      key3 = ScanJoyStick(JOYSTICK3_X_PIN, JOYSTICK3_Y_PIN);
      if (key3 != oldkey3) {
        oldkey3 = key3;
        if (key3 == JOYSTICK_RIGHT_ && head3->dir != JOYSTICK_RIGHT_) head3->dir = JOYSTICK_LEFT_;
        else if (key3 == JOYSTICK_LEFT_ && head3->dir != JOYSTICK_LEFT_) head3->dir = JOYSTICK_RIGHT_;
        else if (key3 == JOYSTICK_UP_ && head3->dir != JOYSTICK_UP_) head3->dir = JOYSTICK_DOWN_;
        else if (key3 == JOYSTICK_DOWN_ && head3->dir != JOYSTICK_DOWN_) head3->dir = JOYSTICK_UP_;
      }
    }
  }

  unsigned long timeNow = millis();
  if (timeNow - time1 > 1000 / gameSpeed1) {
    int oldTailRow = tail1->row;
    int oldTailCol = tail1->column;
    boolean ateFood = moveSnake(head1, tail1, head2, tail2, head3, tail3, collected1);
    if (!gameOver && !ateFood) {
      ChangeDot(oldTailRow, oldTailCol, false);
    }
    time1 = timeNow;
  }
  
  if (timeNow - time2 > 1000 / gameSpeed2) {
    int oldTailRow = tail2->row;
    int oldTailCol = tail2->column;
    boolean ateFood = moveSnake(head2, tail2, head1, tail1, head3, tail3, collected2);
    if (!gameOver && !ateFood) {
      ChangeDot(oldTailRow, oldTailCol, false);
    }
    time2 = timeNow;
  }

  if (numPlayers == 3) {
    if (timeNow - time3 > 1000 / gameSpeed3) {
      int oldTailRow = tail3->row;
      int oldTailCol = tail3->column;
      boolean ateFood = moveSnake(head3, tail3, head1, tail1, head2, tail2, collected3);
      if (!gameOver && !ateFood) {
        ChangeDot(oldTailRow, oldTailCol, false);
      }
      time3 = timeNow;
    }
  }

  if (!gameOver) {
     ChangeDot(head1->row, head1->column, true);
     ChangeDot(head2->row, head2->column, true);
     if (numPlayers == 3) {
       ChangeDot(head3->row, head3->column, true);
     }
     drawMatrix();
  }
}

// ======================================================================
// ================== LEE LA POSICIÓN DEL JOYSTICK ======================
// ======================================================================
byte ScanJoyStick(int xPin, int yPin) {
  byte Direction = JOYSTICK_RELEASED;
  unsigned int X_Val = analogRead(xPin);
  unsigned int Y_Val = analogRead(yPin);
  if (Y_Val >= 900) Direction = JOYSTICK_UP_;
  else if (Y_Val <= 100) Direction = JOYSTICK_DOWN_;
  else if (X_Val >= 900) Direction = JOYSTICK_RIGHT_;
  else if (X_Val <= 100) Direction = JOYSTICK_LEFT_;
  return Direction;
}

// ======================================================================
// ============ MODIFICA UN PÍXEL EN EL CAMPO DE JUEGO ==================
// ======================================================================
void ChangeDot(unsigned int RowVal, unsigned int ColVal, boolean NewVal) {
  Field[16 * RowVal + ColVal / 5] &= ~(1 << (ColVal % 5));
  if (NewVal) {
    Field[16 * RowVal + ColVal / 5] |= 1 << (ColVal % 5);
  }
}

// ======================================================================
// =================== FUNCIONES PARA LA MÚSICA =========================
// ======================================================================

/**
 * @brief Reproduce una secuencia de notas (método bloqueante con delay).
 * Usado solo para la pantalla de Game Over.
 */
void playStage(int melody[], int durations[], int notes) {
  for (int thisNote = 0; thisNote < notes; thisNote++) {
    int noteDuration = durations[thisNote];
    tone(SPEAKER_PIN, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(SPEAKER_PIN);
  }
}

/**
 * @brief Inicia la reproducción de una melodía para el sistema no bloqueante.
 */
void startMelody(const int melody[], const int durations[], int notes, boolean shouldLoop) {
  currentMelody = melody;
  currentDurations = durations;
  totalNotes = notes;
  currentNoteIndex = -1; // Se incrementará a 0 en la primera llamada de update
  noteStartTime = 0;
  loopMelody = shouldLoop;
  musicPlaying = true;
}

/**
 * @brief Detiene la reproducción de la música no bloqueante.
 */
void stopMusic() {
  noTone(SPEAKER_PIN);
  musicPlaying = false;
}

/**
 * @brief Actualiza el estado de la música no bloqueante. Debe llamarse en cada ciclo del loop.
 */
void updateMusic() {
  if (!musicPlaying) {
    return;
  }
  
  unsigned long currentTime = millis();
  
  // Es hora de reproducir la siguiente nota
  if (currentTime >= noteStartTime) {
    currentNoteIndex++;

    if (currentNoteIndex >= totalNotes) {
      if (loopMelody) {
        currentNoteIndex = 0; // Reinicia para el bucle
      } else {
        stopMusic(); // Se detiene si no es bucle
        return;
      }
    }

    int noteDuration = currentDurations[currentNoteIndex];
    if (currentMelody[currentNoteIndex] != 0) {
      tone(SPEAKER_PIN, currentMelody[currentNoteIndex], noteDuration);
    }
    
    // Calcula cuándo debe empezar la siguiente nota
    noteStartTime = currentTime + (noteDuration * 1.30);
  }
}