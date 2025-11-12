#include <TMCStepper.h>
#include <SPI.h>

//  Pins Arduino 
#define EN_PIN    4
#define DIR_PIN   3
#define STEP_PIN  2
#define CS_PIN    10
#define CRASH_PIN 8       
#define R_SENSE   0.11f

TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE);

//  Paramètres 
const int stepDelay = 600; // µs entre pas
const int DEADBAND  = 5;

// Limites
const long POS_MIN = 0;
const long POS_MAX = 4500;

// État
long position = 0;
long targetPosition = 0;

bool running = false;
bool motorActive = false;
bool InitialisationDone = false;

// Prototypes
void stepMotor(bool direction);
void Initialisation();

void setup() {
  Serial.begin(115200);
  SPI.begin();

  pinMode(EN_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(CRASH_PIN, INPUT_PULLUP);

  digitalWrite(EN_PIN, HIGH);

  driver.begin();
  driver.toff(5);
  driver.rms_current(700);
  driver.microsteps(8);
  driver.intpol(true);

  driver.en_pwm_mode(true);
  driver.pwm_autoscale(true);
  driver.pwm_freq(1);

  digitalWrite(EN_PIN, LOW);

  Serial.println(" INITIALISATION ");
  Serial.println("Moteur inactif. Envoyez 'run' pour lancer l'initialisation et activer la commande moteur.");
  Serial.println("Commandes : run, resume, stop, open, close, gXXXX, p+, p-");
}

void loop() {

  // Commandes série 
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');

    // RUN 
    if (cmd == "run") {
      if (!InitialisationDone) {
        Serial.println(" Lancement Initialisation ");
        Initialisation();
        InitialisationDone = true;
        motorActive = true;
        running = false;
        Serial.println("Initialisation terminé, moteur prêt.");
      } else {
        motorActive = true;
      }
    }

    // RESUME après un STOP 
    else if (cmd == "resume" && motorActive) {
      running = true;
    }

    // STOP 
    else if (cmd == "stop" && motorActive) {
      running = false;
    }

    // Commandes mouvement 
    else if (motorActive && InitialisationDone) {

      // Aller à position
      if (cmd.startsWith("g")) {
        long val = cmd.substring(1).toInt();
        targetPosition = constrain(val, POS_MIN, POS_MAX);
        running = true;
      }

      //Ouverture max
      else if (cmd == "open") {
        targetPosition = POS_MAX;
        running = true;
      }

      //Fermeture max
      else if (cmd == "close") {
        targetPosition = POS_MIN;
        running = true;
      }

      // Incrément +100 pas
      else if (cmd == "p+") {
        targetPosition = constrain(position + 100, POS_MIN, POS_MAX);
        running = true;
      }

      //Décrément -50 pas
      else if (cmd == "p-") {
        targetPosition = constrain(position - 100, POS_MIN, POS_MAX);
        running = true;
      }
    }
  }

  //  Mouvement 
  if (running && motorActive && InitialisationDone) {
    bool dir = (targetPosition < position);

    if ((position <= POS_MIN && dir) || (position >= POS_MAX && !dir))
      running = false;

    if (running) stepMotor(dir);

    if (abs(position - targetPosition) <= DEADBAND)
      running = false;
  }

  //  Affichage
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) {

    if (running) {
      Serial.print("Position: ");
      Serial.print(position);
      Serial.print(", Cible: ");
      Serial.print(targetPosition);
      Serial.print(", Mode: ");
      Serial.println(targetPosition < position ? "fermeture" : "ouverture");
    }

    lastPrint = millis();
  }
}

//Initialisation
void Initialisation() {
  while (digitalRead(CRASH_PIN) == HIGH) {
    stepMotor(true);  // Ouvrir jusqu'au capteur
  }
  position = POS_MIN;
}

//PAS MOTEUR 
void stepMotor(bool direction) {
  digitalWrite(DIR_PIN, direction ? HIGH : LOW);
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(stepDelay);
  position += direction ? -1 : 1;
}
