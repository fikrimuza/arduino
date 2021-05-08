#include <Password.h>
#include <Keypad.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h> 
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2,3); //membuat serial baru tx-3,rx-2 di arduino
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); // finger print di pasang di pin 2 &3

Servo motor;
const int tombol=4;
int buttonState=0;
int buz=A2;
int led_p=A4;
int led_f=A5;
int i;

Password password = Password( "2569" );
const byte baris = 4;
const byte kolom = 4;

char keys[baris][kolom] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte barisPin[baris] = {12,11,10,9};
byte kolomPin[kolom]= {8,7,6,5};

Keypad keypad = Keypad( makeKeymap(keys), barisPin, kolomPin, baris, kolom ); //membuat alamat keypad

void setup() {
  Serial.begin(9600);
  delay(200);
  motor.attach(13);
  motor.write(180);
  
  pinMode(tombol,INPUT_PULLUP); //open = HIGH
  pinMode(buz,OUTPUT);
  pinMode(led_p,OUTPUT);
  pinMode(led_f,OUTPUT);
  
  keypad.addEventListener(keypadEvent); //menjalankan void keypadevent
  
  for(int xx=0;xx<3;xx++) {
    digitalWrite(buz,HIGH);  
    digitalWrite(led_p,HIGH);  
    digitalWrite(led_f,HIGH);
    delay(100);
    digitalWrite(buz,0);  digitalWrite(led_p,0);  digitalWrite(led_f,0);
    delay(100);
  }

  finger.begin(57600); //datasheet fingerprint 57600 bps
  if (finger.verifyPassword()) {
    Serial.println("Sensor sidik jari ditemuakan!");
  } 
  else {
    Serial.println("Sensor sidik jari tidak ditemukan :(");
    while (1) { delay(1); }
  }
  Serial.println("Mnunggu jari yang sesuai...");
}

bool openstate=0;
void loop() {
  getFingerprintID();
  delay(50);
  keypad.getKey();
  Serial.print(digitalRead(led_p));
  Serial.print("-");
  Serial.println(digitalRead(led_f));
  if(digitalRead(led_p) && digitalRead(led_f)) openstate=1;
  buttonState=digitalRead(tombol);
  if(!buttonState||openstate) {
    digitalWrite(led_p,0);
    digitalWrite(led_f,0);
    openstate=0;
    for(i=180;i>=50;i--) {
      motor.write(i);
      delay(5);
    }
    delay(10000);
    for(i=50;i<=180;i++) {
      motor.write(i);
      delay(5);
    }
  }
}

void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    case PRESSED:
    if (PRESSED) {
      digitalWrite(buz,1);
      delay(20);
      digitalWrite(buz,0);
    }
  
    Serial.print("Enter: ");
    Serial.println(eKey);
    delay(10);
  
    Serial.write(254);
  
    switch (eKey) {
      case'A':
        Login();
        delay(1);
        break;
      case'B':
        password.reset();
        delay(1);
        break;
    
      default: 
        password.append(eKey);
        delay(1);
    }
  }
}

void Login() { 
  if (password.evaluate()) {
    digitalWrite(led_p,1);
  }
  else {
    Serial.println("Akses Ditolak!");
    digitalWrite(buz,1);
    delay(2000);
    digitalWrite(buz,0);
  }
}

uint8_t getFingerprintID() { //un integral 8 bit
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Gambar diambil");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Komunikasi Gagal");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Pengambilan gambar gagal");
      return p;
    default:
      Serial.println("Kegagalan tidak di ketahui");
      return p;
  }

  // OK success!

  p = finger.image2Tz(); 
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Gambar di proses");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Gambar gagal di proses");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Komunikasi gagal");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Tidak dapat menemukan fitur sidik jari");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Tidak dapat menemukan fitur sidik jari");
      return p;
    default:
      Serial.println("Kegagalan tidak di ketahui");
      return p;
  }
  
  // OK converted!
  
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Menemukan kecocokan!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Komunikasi gagal");
    return p;
  } 
  else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Tidak menemukan kecocokan");
    digitalWrite(buz,1);
    delay(2000);
    digitalWrite(buz,0);
    return p;
  }
  else {
    Serial.println("Kegagalan tidak di ketahui");
    return p;
  }   
  
  // found a match!
  
  Serial.print("Menemukan ID #");
  Serial.print(finger.fingerID); 
  Serial.print("Menjalankan instruksi");
  Serial.println(finger.confidence); 
  if(Serial.println(finger.confidence))
    digitalWrite(led_f,1);
}
