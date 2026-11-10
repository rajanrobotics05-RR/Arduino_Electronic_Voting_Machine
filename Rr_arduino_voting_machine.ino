/*
Created by Balaji for Rajan Robotics 
Rajan Robotics 
  */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Candidate buttons
const int candidateButtons[] = {2, 3, 4, 5};

// Admin buttons
const int nextVoterButton = 6;
const int resultButton = 7;

// Output pins
const int greenLED = 8;
const int redLED = 9;
const int buzzer = 10;

// Candidate names
const char *candidateNames[] = {
  "Candidate A",
  "Candidate B",
  "Candidate C",
  "Candidate D"
};

// Vote counters
int votes[4] = {0, 0, 0, 0};

// System states
bool votingEnabled = true;
bool resultMode = false;


// --------------------------------
// BUTTON READING WITH DEBOUNCING
// --------------------------------

unsigned long readButton(int pin) {

  if (digitalRead(pin) == LOW) {

    delay(30);

    if (digitalRead(pin) == LOW) {

      unsigned long startTime = millis();

      while (digitalRead(pin) == LOW) {
        delay(5);
      }

      return millis() - startTime;
    }
  }

  return 0;
}


// --------------------------------
// WELCOME SCREEN
// --------------------------------

void welcomeScreen() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("RAJAN ROBOTICS");

  lcd.setCursor(0, 1);
  lcd.print("Voting Machine");

  delay(2000);

  lcd.clear();

  lcd.print("Welcome Voter");

  lcd.setCursor(0, 1);
  lcd.print("Select Candidate");
}


// --------------------------------
// VOTE CONFIRMATION
// --------------------------------

void confirmVote(int candidate) {

  votes[candidate]++;

  votingEnabled = false;

  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);

  tone(buzzer, 2000, 200);

  lcd.clear();

  lcd.print("Vote Recorded!");

  lcd.setCursor(0, 1);
  lcd.print(candidateNames[candidate]);

  delay(2000);

  digitalWrite(greenLED, LOW);

  lcd.clear();

  lcd.print("Voting Locked");

  lcd.setCursor(0, 1);
  lcd.print("Wait for Admin");
}


// --------------------------------
// ENABLE NEXT VOTER
// --------------------------------

void enableNextVoter() {

  votingEnabled = true;

  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);

  lcd.clear();

  lcd.print("Next Voter");

  lcd.setCursor(0, 1);
  lcd.print("Please Vote");

  tone(buzzer, 1500, 150);

  delay(1500);

  digitalWrite(redLED, LOW);

  lcd.clear();

  lcd.print("Select Candidate");
}


// --------------------------------
// DISPLAY VOTING RESULTS
// --------------------------------

void showResults() {

  resultMode = true;
  votingEnabled = false;

  lcd.clear();

  lcd.print("VOTING RESULTS");

  delay(1500);

  // Candidate A and B
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("A:");
  lcd.print(votes[0]);

  lcd.setCursor(8, 0);
  lcd.print("B:");
  lcd.print(votes[1]);

  lcd.setCursor(0, 1);
  lcd.print("Total:");
  lcd.print(votes[0] + votes[1] +
            votes[2] + votes[3]);

  delay(2500);

  // Candidate C and D
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("C:");
  lcd.print(votes[2]);

  lcd.setCursor(8, 0);
  lcd.print("D:");
  lcd.print(votes[3]);

  lcd.setCursor(0, 1);
  lcd.print("Total:");
  lcd.print(votes[0] + votes[1] +
            votes[2] + votes[3]);

  delay(2500);

  // Find maximum votes
  int maxVotes = votes[0];

  for (int i = 1; i < 4; i++) {

    if (votes[i] > maxVotes) {
      maxVotes = votes[i];
    }
  }

  // Check winner and tie
  int winnerCount = 0;
  int winner = -1;

  for (int i = 0; i < 4; i++) {

    if (votes[i] == maxVotes) {
      winnerCount++;
      winner = i;
    }
  }

  lcd.clear();

  if (maxVotes == 0) {

    lcd.print("No Votes Cast");

  }
  else if (winnerCount > 1) {

    lcd.print("Election Tie!");

  }
  else {

    lcd.print("Winner:");

    lcd.setCursor(0, 1);
    lcd.print(candidateNames[winner]);
  }

  tone(buzzer, 2000, 500);

  delay(3000);

  lcd.clear();

  lcd.print("Hold Result");

  lcd.setCursor(0, 1);
  lcd.print("3 sec to Reset");
}


// --------------------------------
// RESET VOTING MACHINE
// --------------------------------

void resetVoting() {

  for (int i = 0; i < 4; i++) {
    votes[i] = 0;
  }

  votingEnabled = true;
  resultMode = false;

  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);

  tone(buzzer, 1000, 300);

  lcd.clear();

  lcd.print("Votes Reset!");

  lcd.setCursor(0, 1);
  lcd.print("New Election");

  delay(2000);

  welcomeScreen();
}


// --------------------------------
// SETUP
// --------------------------------

void setup() {

  // Candidate buttons
  for (int i = 0; i < 4; i++) {

    pinMode(candidateButtons[i], INPUT_PULLUP);
  }

  // Admin buttons
  pinMode(nextVoterButton, INPUT_PULLUP);
  pinMode(resultButton, INPUT_PULLUP);

  // Outputs
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);

  // LCD initialization
  lcd.init();
  lcd.backlight();

  welcomeScreen();
}


// --------------------------------
// MAIN LOOP
// --------------------------------

void loop() {

  // Result button
  unsigned long resultPress =
      readButton(resultButton);

  // Long press: Reset
  if (resultPress >= 3000) {

    resetVoting();
    return;
  }

  // Short press: Show results
  if (resultPress > 0 && !resultMode) {

    showResults();
    return;
  }

  // Ignore voting after results
  if (resultMode) {
    return;
  }

  // Voting locked: Wait for admin
  if (!votingEnabled) {

    if (readButton(nextVoterButton) > 0) {

      enableNextVoter();
    }

    return;
  }

  // Read candidate buttons
  for (int i = 0; i < 4; i++) {

    if (readButton(candidateButtons[i]) > 0) {

      confirmVote(i);
      return;
    }
  }
}
