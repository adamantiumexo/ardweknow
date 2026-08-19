// ============================================================
//             DEFUSE PROTOCOL A
//       4 PHASE ARDUINO UNO GAME
// ============================================================
//
// PHASE 1: Button memory
// PHASE 2: Wire sequence
// PHASE 3: Harder button memory
// PHASE 4: Final button hold
//
// TIME LIMIT: 3 MINUTES
//
// ============================================================


// ============================================================
// PINS
// ============================================================

// Buttons
const int LEFT_BUTTON  = 2;
const int RIGHT_BUTTON = 3;

// LEDs
const int RED_LED    = 4;
const int YELLOW_LED = 5;
const int BLUE_LED   = 6;
const int GREEN_LED  = 7;

// Buzzer
const int BUZZER = A1;

// Removable wires
const int RED_WIRE    = 10;
const int YELLOW_WIRE = 11;
const int GREEN_WIRE  = 12;
const int BLUE_WIRE   = 13;


// ============================================================
// GAME SETTINGS
// ============================================================

const unsigned long GAME_TIME = 180000UL;  // 3 minutes

unsigned long gameStart = 0;
unsigned long lastTimerPrint = 0;

bool gameRunning = false;
bool gameFinished = false;

int currentStage = 0;
int penalties = 0;


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(9600);

  // Buttons
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);

  // LEDs
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Buzzer
  pinMode(BUZZER, OUTPUT);

  // Wires
  pinMode(RED_WIRE, INPUT_PULLUP);
  pinMode(YELLOW_WIRE, INPUT_PULLUP);
  pinMode(GREEN_WIRE, INPUT_PULLUP);
  pinMode(BLUE_WIRE, INPUT_PULLUP);

  allLEDsOff();
  noTone(BUZZER);

  // Standby light
  digitalWrite(RED_LED, HIGH);

  Serial.println();
  Serial.println("================================");
  Serial.println("       DEFUSE PROTOCOL A");
  Serial.println("================================");
  Serial.println("SYSTEM READY");
  Serial.println();
  Serial.println("Press BOTH black buttons");
  Serial.println("to begin.");
  Serial.println();
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // WAITING TO START
  // ----------------------------------------------------------

  if (!gameRunning && !gameFinished) {

    if (bothButtonsPressed()) {

      delay(100);

      if (bothButtonsPressed()) {

        startGame();

        waitForBothRelease();
      }
    }

    return;
  }


  // ----------------------------------------------------------
  // GAME RUNNING
  // ----------------------------------------------------------

  if (gameRunning) {

    updateTimer();

    if (!gameRunning) {
      return;
    }

    runGame();
  }


  // ----------------------------------------------------------
  // GAME FINISHED
  // ----------------------------------------------------------

  if (gameFinished) {

    // Hold both buttons for 3 seconds
    // to reset the game.

    if (bothButtonsPressed()) {

      unsigned long resetStart = millis();

      Serial.println("RESETTING...");

      while (bothButtonsPressed()) {

        if (millis() - resetStart >= 3000) {

          waitForBothRelease();

          resetGame();

          return;
        }

        delay(10);
      }
    }
  }
}


// ============================================================
// START GAME
// ============================================================

void startGame() {

  gameRunning = true;
  gameFinished = false;

  currentStage = 1;
  penalties = 0;

  gameStart = millis();
  lastTimerPrint = millis();

  allLEDsOff();

  Serial.println();
  Serial.println("================================");
  Serial.println("          GAME START");
  Serial.println("================================");

  Serial.println("TIME LIMIT: 03:00");
  Serial.println();

  beep(1000, 150);

  delay(100);

  beep(1500, 150);

  delay(500);

  Serial.println("PHASE 1 STARTING...");
}


// ============================================================
// RUN GAME
// ============================================================

void runGame() {

  switch (currentStage) {

    case 1:
      phase1();
      break;

    case 2:
      phase2();
      break;

    case 3:
      phase3();
      break;

    case 4:
      phase4();
      break;
  }
}


// ============================================================
// PHASE 1
// BUTTON MEMORY
// ============================================================

void phase1() {

  currentStage = 1;

  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("           PHASE 1");
  Serial.println("        BUTTON MEMORY");
  Serial.println("--------------------------------");

  Serial.println("BLUE  = LEFT BUTTON");
  Serial.println("YELLOW = RIGHT BUTTON");
  Serial.println();

  delay(1000);


  // 0 = LEFT
  // 1 = RIGHT

  const byte sequence[8] = {

    0,
    1,
    1,
    0,
    1,
    0,
    0,
    1

  };


  Serial.println("WATCH THE SEQUENCE...");
  Serial.println();


  // ----------------------------------------------------------
  // SHOW SEQUENCE
  // ----------------------------------------------------------

  for (int i = 0; i < 8; i++) {

    if (!gameRunning) {
      return;
    }

    if (sequence[i] == 0) {

      Serial.print(i + 1);
      Serial.println(": BLUE / LEFT");

      digitalWrite(BLUE_LED, HIGH);

      tone(BUZZER, 800);

      delay(500);

      noTone(BUZZER);

      digitalWrite(BLUE_LED, LOW);

    }

    else {

      Serial.print(i + 1);
      Serial.println(": YELLOW / RIGHT");

      digitalWrite(YELLOW_LED, HIGH);

      tone(BUZZER, 1400);

      delay(500);

      noTone(BUZZER);

      digitalWrite(YELLOW_LED, LOW);
    }

    delay(400);
  }


  Serial.println();
  Serial.println("NOW REPEAT THE SEQUENCE!");
  Serial.println();


  // ----------------------------------------------------------
  // PLAYER REPEATS
  // ----------------------------------------------------------

  for (int i = 0; i < 8; i++) {

    int answer = waitForButton();

    if (answer == -1) {
      return;
    }


    if (answer == sequence[i]) {

      Serial.print("CORRECT ");
      Serial.print(i + 1);
      Serial.println("/8");

      beep(1500, 80);
    }

    else {

      Serial.println("WRONG BUTTON!");

      penalties++;

      Serial.print("PENALTY: ");
      Serial.println(penalties);

      wrongButton();

      Serial.println("RESTARTING PHASE 1...");

      delay(500);

      phase1();

      return;
    }
  }


  Serial.println();
  Serial.println("PHASE 1 SUCCESS!");

  successBeep();

  delay(500);

  currentStage = 2;

  phase2();
}


// ============================================================
// PHASE 2
// WIRE DEFUSAL
// ============================================================

void phase2() {

  currentStage = 2;

  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("           PHASE 2");
  Serial.println("         WIRE DEFUSAL");
  Serial.println("--------------------------------");

  Serial.println();
  Serial.println("REMOVE THE WIRES IN THIS ORDER:");
  Serial.println();
  Serial.println("BLUE -> RED -> YELLOW -> GREEN");
  Serial.println();
  Serial.println("WARNING:");
  Serial.println("Removing the wrong wire = FAIL");
  Serial.println();


  // ----------------------------------------------------------
  // BLUE
  // ----------------------------------------------------------

  Serial.println("WAITING FOR: BLUE");

  if (!waitForWire(BLUE_WIRE, "BLUE")) {
    return;
  }


  // ----------------------------------------------------------
  // RED
  // ----------------------------------------------------------

  Serial.println("WAITING FOR: RED");

  if (!waitForWire(RED_WIRE, "RED")) {
    return;
  }


  // ----------------------------------------------------------
  // YELLOW
  // ----------------------------------------------------------

  Serial.println("WAITING FOR: YELLOW");

  if (!waitForWire(YELLOW_WIRE, "YELLOW")) {
    return;
  }


  // ----------------------------------------------------------
  // GREEN
  // ----------------------------------------------------------

  Serial.println("WAITING FOR: GREEN");

  if (!waitForWire(GREEN_WIRE, "GREEN")) {
    return;
  }


  // ----------------------------------------------------------
  // COMPLETE
  // ----------------------------------------------------------

  Serial.println();
  Serial.println("PHASE 2 SUCCESS!");

  successBeep();

  delay(700);

  currentStage = 3;

  phase3();
}


// ============================================================
// WAIT FOR WIRE
// ============================================================

bool waitForWire(int correctWire, const char* correctColor) {
  bool blueWasHigh = digitalRead(BLUE_WIRE) == HIGH;
  bool redWasHigh = digitalRead(RED_WIRE) == HIGH;
  bool yellowWasHigh = digitalRead(YELLOW_WIRE) == HIGH;
  bool greenWasHigh = digitalRead(GREEN_WIRE) == HIGH;

  while (gameRunning) {

    updateTimer();

    if (!gameRunning) {
      return false;
    }


    // BLUE

    if (!blueWasHigh && digitalRead(BLUE_WIRE) == HIGH) {

      if (correctWire == BLUE_WIRE) {

        correctWireRemoved("BLUE");

        return true;

      } else {

        wrongWire("BLUE");

        return false;
      }
    }


    // RED

    if (!redWasHigh && digitalRead(RED_WIRE) == HIGH) {

      if (correctWire == RED_WIRE) {

        correctWireRemoved("RED");

        return true;

      } else {

        wrongWire("RED");

        return false;
      }
    }


    // YELLOW

    if (!yellowWasHigh && digitalRead(YELLOW_WIRE) == HIGH) {

      if (correctWire == YELLOW_WIRE) {

        correctWireRemoved("YELLOW");

        return true;

      } else {

        wrongWire("YELLOW");

        return false;
      }
    }


    // GREEN

    if (!greenWasHigh && digitalRead(GREEN_WIRE) == HIGH) {

      if (correctWire == GREEN_WIRE) {

        correctWireRemoved("GREEN");

        return true;

      } else {

        wrongWire("GREEN");

        return false;
      }
    }


    delay(10);
  }

  return false;
}


// ============================================================
// CORRECT WIRE
// ============================================================

void correctWireRemoved(const char* color) {

  Serial.print("CORRECT WIRE: ");
  Serial.println(color);

  beep(1600, 120);

  delay(400);
}


// ============================================================
// WRONG WIRE
// ============================================================

void wrongWire(const char* color) {

  gameRunning = false;
  gameFinished = true;

  Serial.println();
  Serial.println("================================");
  Serial.println("             FAIL");
  Serial.println("================================");

  Serial.print("WRONG WIRE: ");
  Serial.println(color);

  Serial.println("DEVICE DETONATED!");

  Serial.println();

  allLEDsOff();

  digitalWrite(RED_LED, HIGH);

  tone(BUZZER, 300);

  delay(3000);

  noTone(BUZZER);
}


// ============================================================
// PHASE 3
// HARDER BUTTON MEMORY
// ============================================================

void phase3() {

  currentStage = 3;

  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("           PHASE 3");
  Serial.println("       SECURITY OVERRIDE");
  Serial.println("--------------------------------");

  Serial.println("BLUE  = LEFT");
  Serial.println("YELLOW = RIGHT");
  Serial.println();

  delay(1000);


  // 12-step sequence

  const byte sequence[12] = {

    1,
    0,
    1,
    1,
    0,
    1,
    0,
    0,
    1,
    0,
    1,
    0

  };


  Serial.println("WATCH CAREFULLY...");
  Serial.println();


  // ----------------------------------------------------------
  // SHOW
  // ----------------------------------------------------------

  for (int i = 0; i < 12; i++) {

    if (!gameRunning) {
      return;
    }


    if (sequence[i] == 0) {

      Serial.print(i + 1);
      Serial.println(": BLUE / LEFT");

      digitalWrite(BLUE_LED, HIGH);

      tone(BUZZER, 800);

      delay(400);

      noTone(BUZZER);

      digitalWrite(BLUE_LED, LOW);

    }

    else {

      Serial.print(i + 1);
      Serial.println(": YELLOW / RIGHT");

      digitalWrite(YELLOW_LED, HIGH);

      tone(BUZZER, 1400);

      delay(400);

      noTone(BUZZER);

      digitalWrite(YELLOW_LED, LOW);
    }


    delay(300);
  }


  Serial.println();
  Serial.println("REPEAT THE SEQUENCE!");
  Serial.println();


  // ----------------------------------------------------------
  // REPEAT
  // ----------------------------------------------------------

  for (int i = 0; i < 12; i++) {

    int answer = waitForButton();

    if (answer == -1) {
      return;
    }


    if (answer == sequence[i]) {

      Serial.print("CORRECT ");
      Serial.print(i + 1);
      Serial.println("/12");

      beep(1500, 80);

    }

    else {

      Serial.println("WRONG BUTTON!");

      penalties++;

      Serial.print("PENALTY: ");
      Serial.println(penalties);

      wrongButton();

      Serial.println("RESTARTING PHASE 3...");

      delay(500);

      phase3();

      return;
    }
  }


  Serial.println();
  Serial.println("PHASE 3 SUCCESS!");

  successBeep();

  delay(700);

  currentStage = 4;

  phase4();
}


// ============================================================
// PHASE 4
// FINAL DEFUSAL
// ============================================================

void phase4() {

  currentStage = 4;

  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("           PHASE 4");
  Serial.println("        FINAL DEFUSAL");
  Serial.println("--------------------------------");

  Serial.println();
  Serial.println("PRESS BOTH BUTTONS");
  Serial.println("AND HOLD FOR 5 SECONDS.");
  Serial.println();


  allLEDsOff();

  delay(500);


  unsigned long holdStart = 0;

  bool holding = false;


  while (gameRunning) {

    updateTimer();

    if (!gameRunning) {
      return;
    }


    bool leftPressed =
      digitalRead(LEFT_BUTTON) == LOW;

    bool rightPressed =
      digitalRead(RIGHT_BUTTON) == LOW;


    // --------------------------------------------------------
    // BOTH PRESSED
    // --------------------------------------------------------

    if (leftPressed && rightPressed) {

      if (!holding) {

        holding = true;

        holdStart = millis();

        Serial.println("FINAL SEQUENCE ACTIVE!");

        digitalWrite(GREEN_LED, HIGH);
      }


      unsigned long heldTime =
        millis() - holdStart;


      // Print progress every second

      static int lastSecond = -1;

      int currentSecond =
        heldTime / 1000;


      if (currentSecond != lastSecond) {

        lastSecond = currentSecond;

        if (currentSecond < 5) {

          Serial.print("HOLD: ");
          Serial.print(currentSecond);
          Serial.println("/5");
        }
      }


      // Beeping

      if ((millis() / 300) % 2 == 0) {

        tone(BUZZER, 900);

      } else {

        noTone(BUZZER);
      }


      // 5 seconds reached

      if (heldTime >= 5000) {

        noTone(BUZZER);

        winGame();

        return;
      }
    }


    // --------------------------------------------------------
    // RELEASED
    // --------------------------------------------------------

    else {

      if (holding) {

        Serial.println("FINAL SEQUENCE INTERRUPTED!");

      }

      holding = false;

      digitalWrite(GREEN_LED, LOW);

      noTone(BUZZER);
    }


    delay(20);
  }
}


// ============================================================
// WAIT FOR BUTTON
// ============================================================

int waitForButton() {

  while (gameRunning) {

    updateTimer();

    if (!gameRunning) {
      return -1;
    }


    // LEFT

    if (digitalRead(LEFT_BUTTON) == LOW) {

      delay(40);

      if (digitalRead(LEFT_BUTTON) == LOW) {

        while (digitalRead(LEFT_BUTTON) == LOW) {
          delay(10);
        }

        Serial.println("INPUT: LEFT");

        return 0;
      }
    }


    // RIGHT

    if (digitalRead(RIGHT_BUTTON) == LOW) {

      delay(40);

      if (digitalRead(RIGHT_BUTTON) == LOW) {

        while (digitalRead(RIGHT_BUTTON) == LOW) {
          delay(10);
        }

        Serial.println("INPUT: RIGHT");

        return 1;
      }
    }


    delay(5);
  }

  return -1;
}


// ============================================================
// WRONG BUTTON
// ============================================================

void wrongButton() {

  Serial.println("5 SECOND PENALTY!");

  digitalWrite(RED_LED, HIGH);

  tone(BUZZER, 300);

  delay(600);

  noTone(BUZZER);

  digitalWrite(RED_LED, LOW);

  delay(400);
}


// ============================================================
// TIMER
// ============================================================

void updateTimer() {

  unsigned long elapsed =
    millis() - gameStart;


  // TIME EXPIRED

  if (elapsed >= GAME_TIME) {

    failGame();

    return;
  }


  // Print once per second

  if (millis() - lastTimerPrint >= 1000) {

    lastTimerPrint = millis();


    unsigned long remaining =
      GAME_TIME - elapsed;


    int totalSeconds =
      remaining / 1000;


    int minutes =
      totalSeconds / 60;


    int seconds =
      totalSeconds % 60;


    Serial.print("TIME: ");


    if (minutes < 10)
      Serial.print("0");

    Serial.print(minutes);

    Serial.print(":");


    if (seconds < 10)
      Serial.print("0");

    Serial.print(seconds);


    Serial.print(" | PHASE: ");

    Serial.print(currentStage);


    Serial.print(" | PENALTIES: ");

    Serial.println(penalties);
  }
}


// ============================================================
// TIME FAIL
// ============================================================

void failGame() {

  if (!gameRunning) {
    return;
  }


  gameRunning = false;
  gameFinished = true;


  unsigned long finalTime =
    millis() - gameStart;


  int totalSeconds =
    finalTime / 1000;


  Serial.println();
  Serial.println("================================");
  Serial.println("              FAIL");
  Serial.println("================================");

  Serial.println("REASON: TIME EXPIRED");

  Serial.print("TIME: ");

  Serial.print(totalSeconds / 60);

  Serial.print(":");


  if (totalSeconds % 60 < 10)
    Serial.print("0");

  Serial.println(totalSeconds % 60);


  Serial.print("FINAL PHASE: ");

  Serial.println(currentStage);


  Serial.print("PENALTIES: ");

  Serial.println(penalties);


  Serial.println("DEVICE DETONATED!");

  Serial.println("================================");


  allLEDsOff();

  digitalWrite(RED_LED, HIGH);

  tone(BUZZER, 300);

  delay(3000);

  noTone(BUZZER);
}


// ============================================================
// WIN
// ============================================================

void winGame() {

  gameRunning = false;
  gameFinished = true;


  unsigned long finalTime =
    millis() - gameStart;


  int totalSeconds =
    finalTime / 1000;


  Serial.println();
  Serial.println("================================");
  Serial.println("             *** PASS ***");
  Serial.println("================================");


  Serial.print("COMPLETION TIME: ");

  Serial.print(totalSeconds / 60);

  Serial.print(":");


  if (totalSeconds % 60 < 10)
    Serial.print("0");

  Serial.println(totalSeconds % 60);


  Serial.print("PENALTIES: ");

  Serial.println(penalties);


  Serial.println("DEVICE SUCCESSFULLY DEFUSED!");

  Serial.println("================================");


  allLEDsOff();

  digitalWrite(GREEN_LED, HIGH);


  // Victory melody

  beep(1000, 150);

  delay(50);

  beep(1500, 150);

  delay(50);

  beep(2000, 150);

  delay(50);

  beep(2500, 400);
}


// ============================================================
// RESET
// ============================================================

void resetGame() {

  gameRunning = false;
  gameFinished = false;

  currentStage = 0;
  penalties = 0;

  allLEDsOff();

  noTone(BUZZER);


  // Standby

  digitalWrite(RED_LED, HIGH);


  Serial.println();
  Serial.println("================================");
  Serial.println("          SYSTEM RESET");
  Serial.println("================================");

  Serial.println("Make sure all 4 wires are");
  Serial.println("reconnected before starting.");

  Serial.println();
  Serial.println("READY!");
  Serial.println("Press BOTH buttons to start.");
  Serial.println();
}


// ============================================================
// BOTH BUTTONS PRESSED
// ============================================================

bool bothButtonsPressed() {

  return
    digitalRead(LEFT_BUTTON) == LOW &&
    digitalRead(RIGHT_BUTTON) == LOW;
}


// ============================================================
// WAIT FOR BOTH BUTTONS RELEASE
// ============================================================

void waitForBothRelease() {

  while (bothButtonsPressed()) {

    delay(10);
  }
}


// ============================================================
// BEEP
// ============================================================

void beep(int frequency, int duration) {

  tone(BUZZER, frequency);

  delay(duration);

  noTone(BUZZER);

  delay(50);
}


// ============================================================
// SUCCESS BEEP
// ============================================================

void successBeep() {

  beep(1200, 100);

  delay(50);

  beep(1600, 100);

  delay(50);

  beep(2000, 200);
}


// ============================================================
// ALL LEDS OFF
// ============================================================

void allLEDsOff() {

  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}