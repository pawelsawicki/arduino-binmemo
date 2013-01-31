#define MAX_LEVEL     8
#define PUZZLE_DELAY  500

#define NUM_OF_LEDS  4
#define NUM_OF_NUMBERS 4
#define NUM_OF_BUTTONS 5

#define BUTTON_OK  11  // OK
#define BUTTON_0   10  // 2^0
#define BUTTON_1   9   // 2^1
#define BUTTON_2   8   // 2^2
#define BUTTON_3   7   // 2^3

#define BOUNCE_DELAY  1000

int ledPins[NUM_OF_LEDS] = { 2, 3, 4, 5 }; // MSB ... LSB
int buzzerPin = 6;
int buttons[NUM_OF_BUTTONS] = { BUTTON_3, BUTTON_2, BUTTON_1, BUTTON_0, BUTTON_OK }; // MSB ... LSB, OK

int input;

int puzzle[MAX_LEVEL];

int answerLevel = 0;
int puzzleLevel = 0;

int lastButtonPressed = 0;
int lastButtonPressedOn = 0;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);

  for (int ii = 0; ii < NUM_OF_LEDS; ii++)
    pinMode(ledPins[ii], OUTPUT);

  for (int ii = 0; ii < NUM_OF_BUTTONS; ii++) {
    pinMode(buttons[ii], INPUT);
    digitalWrite(buttons[ii], HIGH);
  }

  pinMode(buzzerPin, OUTPUT);

  lastButtonPressedOn = millis();
  lastButtonPressed = BUTTON_OK;
  
  // generate the puzzle
  generatePuzzle();
  
  // start the game with level 0
  showPuzzle();
}

void loop() {
  for (int i = 0; i < NUM_OF_BUTTONS; i ++)
    if (
        digitalRead(buttons[i]) == LOW
      &&
      (
          lastButtonPressed != buttons[i]
        ||
          millis() - lastButtonPressedOn >  BOUNCE_DELAY
      )
      ) {
      switch(buttons[i]) {
      case BUTTON_OK:
        clearDisplay();
        if (puzzle[answerLevel] == input) {
            if (answerLevel == puzzleLevel) {
              levelNext();
            } else {
              answerGood();
            } // if (answerLevel == puzzleLevel) {
        } else {
          answerBad();
        } // if (puzzle[answerLevel] == input)) {

        input = 0;
        break;
      default:
        input ^= B00000001 << i;
        break;
      }

      lastButtonPressed = buttons[i];
      lastButtonPressedOn = millis();
      break;
    }

  displayNumber(input);
  Serial.println(input);
}

void clearDisplay(void) {
  for (int ii = 0; ii < NUM_OF_LEDS; ii++)
    digitalWrite(ledPins[ii], LOW);
}

void displayNumber(int number) {
  clearDisplay();
  for (int ii = 0; ii < NUM_OF_LEDS; ii++)
    digitalWrite(ledPins[ii], (number & B00000001 << ii) ? HIGH : LOW);
}

void generatePuzzle(void) {
  for (int i = 0; i < MAX_LEVEL; i++)
    puzzle[i] = random(1, 15);
}

void showPuzzle() {
  if (puzzleLevel == 0) {
    for (int i = 0; i < 3; i++) {
      playNote('c', 100);
      delay(100);
    }
  }
  
  for (int i = 0; i <= puzzleLevel; i++) {
    clearDisplay();
    displayNumber(puzzle[i]);
    delay(PUZZLE_DELAY);
    clearDisplay();
    delay(PUZZLE_DELAY/2);
  }
}

void playTone(int tone, int duration) {
  for (long ii = 0; ii < duration * 1000L; ii += tone * 2) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'   };
  int tones[] = { 
    1915, 1700, 1519, 1432, 1275, 1136, 1014, 956   };

  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
      return;
    }
  }
}

void answerGood(void) {
  answerLevel++;

  playNote('C', 100);
}

void answerBad(void) {
  // play a note
  playNote('c', 500);
  delay(250);
  
  // generate a new puzzle
  generatePuzzle();
  
  // set the current level of both the answer and puzzle to 0 (beginning)
  answerLevel = 0;
  puzzleLevel = 0;
  
  // show the puzzle
  showPuzzle();
}

void levelNext(void) {
  puzzleLevel++;
  answerLevel = 0;

  playNote('c', 100);
  delay(100);
  playNote('a', 100);
  delay(100);
  playNote('C', 100);
  
  showPuzzle();
}
