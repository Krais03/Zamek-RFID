#include &lt;SPI.h&gt;
#include &lt;MFRC522.h&gt;
#include &lt;EEPROM.h&gt;

#define SS_PIN 10
#define RST_PIN 9
#define LED_GREEN 6 // LED zielona - dostęp OK
#define LED_RED 7 // LED czerwona - dostęp zabroniony
#define BUZZER_PIN 4 // NOWOŚĆ: Pin buzzera (podłącz + buzzera tutaj, - do
GND)
#define RELAY_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

bool programMode = false;
unsigned long programModeTime = 0;

byte masterCard[4]; // UID karty master
byte readCard[4]; // Odczytana karta

void setup() {
Serial.begin(9600);
SPI.begin();
rfid.PCD_Init();

pinMode(LED_GREEN, OUTPUT);
pinMode(LED_RED, OUTPUT);

pinMode(BUZZER_PIN, OUTPUT); // NOWOŚĆ: Konfiguracja pinu buzzera
pinMode(RELAY_PIN, OUTPUT);
digitalWrite(RELAY_PIN, HIGH);

// Sprawdź czy karta master jest zdefiniowana
if (EEPROM.read(1) != 143) {
Serial.println(&quot;=== PIERWSZE URUCHOMIENIE ===&quot;);
Serial.println(&quot;Zdefiniuj kartę MASTER - przyłóż kartę...&quot;);

while (!getID()) {
delay(200);
}

// Zapisz kartę master w EEPROM
for (byte i = 0; i &lt; 4; i++) {
EEPROM.write(2 + i, readCard[i]);
}
EEPROM.write(1, 143); // Flaga że master jest zdefiniowany
EEPROM.write(0, 0); // Licznik zapisanych kart = 0

Serial.println(&quot;Karta MASTER zapisana!&quot;);
// NOWOŚĆ: Dźwięk potwierdzenia zapisu Mastera
tone(BUZZER_PIN, 2000, 500);
delay(2000);
}

// Wczytaj kartę master z EEPROM
for (byte i = 0; i &lt; 4; i++) {
masterCard[i] = EEPROM.read(2 + i);
}

Serial.println(&quot;=== SYSTEM GOTOWY ===&quot;);
Serial.print(&quot;Zapisanych kart: &quot;);
Serial.println(EEPROM.read(0));
printUID(masterCard);
Serial.println(&quot; &lt;- Karta MASTER&quot;);

// Krótki dźwięk na start
tone(BUZZER_PIN, 1000, 200);
}

void loop() {
// Sprawdź timeout trybu programowania
if (programMode &amp;&amp; (millis() - programModeTime &gt; 10000)) {
programMode = false;
Serial.println(&quot;Tryb programowania wyłączony&quot;);
digitalWrite(LED_RED, LOW);
digitalWrite(LED_GREEN, LOW);
tone(BUZZER_PIN, 500, 200); // Dźwięk wyjścia z trybu
}

// Odczytaj kartę
if (!getID()) {
return;
}

// Sprawdź czy to karta master
if (isMaster(readCard)) {
programMode = true;
programModeTime = millis();

Serial.println(&quot;\n=== TRYB PROGRAMOWANIA - 10 sekund ===&quot;);
Serial.println(&quot;Przyłóż kartę aby dodać/usunąć&quot;);

// Migaj LEDami z dźwiękiem
for (int i = 0; i &lt; 3; i++) {
digitalWrite(LED_GREEN, HIGH);
tone(BUZZER_PIN, 1500, 100); // NOWOŚĆ: Krótki pisk
delay(100);
digitalWrite(LED_GREEN, LOW);

digitalWrite(LED_RED, HIGH);
delay(100);
digitalWrite(LED_RED, LOW);
}
return;
}

// Tryb programowania - dodaj/usuń kartę
if (programMode) {
if (findCard(readCard)) {
// Karta już istnieje - usuń ją
deleteCard(readCard);
Serial.println(&quot;KARTA USUNIĘTA!&quot;);

// Sygnalizacja usunięcia (niski ton)
digitalWrite(LED_RED, HIGH);
tone(BUZZER_PIN, 500, 500); // NOWOŚĆ: Niski dźwięk 500Hz przez 0.5s
delay(1000);
digitalWrite(LED_RED, LOW);
} else {

// Nowa karta - dodaj ją
addCard(readCard);
Serial.println(&quot;KARTA DODANA!&quot;);

// Sygnalizacja dodania (wysoki ton)
digitalWrite(LED_GREEN, HIGH);
tone(BUZZER_PIN, 2000, 500); // NOWOŚĆ: Wysoki dźwięk 2000Hz przez
0.5s
delay(1000);
digitalWrite(LED_GREEN, LOW);
}

// Resetuj timer trybu programowania po każdej akcji
programModeTime = millis();

Serial.print(&quot;Zapisanych kart: &quot;);
Serial.println(EEPROM.read(0));
return;
}

// Normalny tryb - sprawdź autoryzację
if (findCard(readCard)) {
Serial.println(&quot;DOSTĘP PRZYZNANY!&quot;);
digitalWrite(RELAY_PIN, LOW); // Włącz przekaźnik (LOW dla większości
modułów)
digitalWrite(LED_GREEN, HIGH);
tone(BUZZER_PIN, 2000, 300);

delay(3000); // Czas na wejście (3 sekundy)

// Zamknij zamek

digitalWrite(RELAY_PIN, HIGH); // Wyłącz przekaźnik
digitalWrite(LED_GREEN, LOW);
} else {
Serial.println(&quot;DOSTĘP ZABRONIONY!&quot;);

// Sygnalizacja błędu
digitalWrite(LED_RED, HIGH);
tone(BUZZER_PIN, 300, 1000); // NOWOŚĆ: Długi, niski sygnał błędu
delay(2000);
digitalWrite(LED_RED, LOW);
}
}

// Odczytaj kartę
bool getID() {
if (!rfid.PICC_IsNewCardPresent()) return false;
if (!rfid.PICC_ReadCardSerial()) return false;

Serial.print(&quot;UID: &quot;);
for (byte i = 0; i &lt; 4; i++) {
readCard[i] = rfid.uid.uidByte[i];
Serial.print(readCard[i] &lt; 0x10 ? &quot; 0&quot; : &quot; &quot;);
Serial.print(readCard[i], HEX);
}
Serial.println();

rfid.PICC_HaltA();
return true;
}

// Sprawdź czy karta jest master
bool isMaster(byte card[]) {
for (byte i = 0; i &lt; 4; i++) {
if (card[i] != masterCard[i]) return false;
}
return true;
}

// Znajdź kartę w EEPROM
bool findCard(byte card[]) {
byte count = EEPROM.read(0);
byte storedCard[4];

for (byte i = 1; i &lt;= count; i++) {
// Odczytaj kartę z EEPROM
for (byte j = 0; j &lt; 4; j++) {
storedCard[j] = EEPROM.read(6 + (i-1)*4 + j);
}

// Porównaj
bool match = true;
for (byte j = 0; j &lt; 4; j++) {
if (card[j] != storedCard[j]) {
match = false;
break;
}
}
if (match) return true;
}
return false;

}

// Dodaj kartę do EEPROM
void addCard(byte card[]) {
byte count = EEPROM.read(0);
count++;
EEPROM.write(0, count);

for (byte i = 0; i &lt; 4; i++) {
EEPROM.write(6 + (count-1)*4 + i, card[i]);
}
}

// Usuń kartę z EEPROM
void deleteCard(byte card[]) {
byte count = EEPROM.read(0);
byte storedCard[4];

for (byte i = 1; i &lt;= count; i++) {
// Odczytaj kartę
for (byte j = 0; j &lt; 4; j++) {
storedCard[j] = EEPROM.read(6 + (i-1)*4 + j);
}

// Sprawdź czy to ta karta
bool match = true;
for (byte j = 0; j &lt; 4; j++) {
if (card[j] != storedCard[j]) {
match = false;
break;

}
}

if (match) {
// Przesuń wszystkie następne karty w dół
for (byte k = i; k &lt; count; k++) {
for (byte j = 0; j &lt; 4; j++) {
byte nextCard = EEPROM.read(6 + k*4 + j);
EEPROM.write(6 + (k-1)*4 + j, nextCard);
}
}
count--;
EEPROM.write(0, count);
return;
}
}
}

// Wyświetl UID
void printUID(byte card[]) {
for (byte i = 0; i &lt; 4; i++) {
Serial.print(card[i] &lt; 0x10 ? &quot; 0&quot; : &quot; &quot;);
Serial.print(card[i], HEX);
}
}
