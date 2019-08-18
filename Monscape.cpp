#include "Monscape.h"


Monscape::Monscape(int relai, byte prtcl) {
  _Pin_Relai[0] = relai;
  _Num_Relais = 1;
  _Protocole = prtcl;
  Init();
};

Monscape::Monscape(int relai[2], byte prtcl) {
  _Pin_Relai[0] = relai[0];
  _Pin_Relai[1] = relai[1];
  _Num_Relais = 2;
  _Protocole = prtcl;
  Init();
};


bool Monscape::Init()
{

  for (int i = 0 ; i == _Num_Relais; i++) pinMode(_Pin_Relai[i], OUTPUT);

}

bool Monscape::Desc_Game(String Nom_sys, String Adresse, String Ver_G, String Ver_ms, String Desc_Game, String Win_Code)
{
  _Nom_sys = Nom_sys;
  _Ver_G = Ver_G;
  _Ver_ms = Ver_ms;
  _Desc_Game = Desc_Game;
  _Win_Code = Win_Code;
  _Adresse = Adresse;
}



void Monscape::set_Win(String Win_Code){
  _Win_Code = Win_Code;
}



bool Monscape::Communication(int Pinrx, int pintx , byte pinRS)
{
  int baud =74880;
  delay(500);
  switch (_Protocole) {

    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;
    case MSCape_RS485:
#if defined(ESP32)

#else
    _RS485Pin = pinRS;
    Serial.begin(9600);
    Serial.println("Communication OK");
    RS485_Start(Pinrx,pintx,baud);
      //mySerial->println("Communication RS485 OK");
    digitalWrite(pinRS, LOW);
    Serial.println("Done!");
#endif

    break;
    case MSCape_I2C:
      // statements
    break;
    default:
    Serial.begin(9600);
    Serial.println("Communication OK");
    break;
  }
  Init_Trame();
}

bool Monscape::RS485_Start(int Pinrx, int pintx,int baud = 74880) {
  pinMode(_RS485Pin, OUTPUT);
  digitalWrite(_RS485Pin, LOW);
  Serial.println("Communication RS485");
#if defined(ESP32)
  Serial2.begin(baud);
#else
  mySerial = new SoftwareSerial(Pinrx, pintx);
  mySerial->begin(baud);
#endif
  Serial.println("Envoie de Data");
  digitalWrite(_RS485Pin, HIGH);
}

bool Monscape::Log_Trame() {
  String buffers;
  doc.clear();
  buffers = "";
  doc["Nom"] = _Nom_sys;
  doc["Adr"] = _Adresse;
  doc["V_G"] = _Ver_G;
  doc["V_mon"] = _Ver_ms;
  doc["Desc"] = _Desc_Game;
  doc["Win"] = _Win_Code;
  doc["stat"] = _Trame.stat;
  doc["l_input"] = _Trame.last_input;
  doc["input"] = _Trame.input;

  //doc["last_modification"] = time();

  switch (_Protocole) {
    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;
    case MSCape_RS485:
    digitalWrite(_RS485Pin, HIGH);
    delay(50);
    serializeJson(doc, buffers);
#if defined(ESP32)
    Serial2.println(buffers);
#else
    mySerial->println(buffers);
#endif
    buffers = "";
    delay(50);
    digitalWrite(_RS485Pin, LOW);
    break;
    case MSCape_I2C:
      // statements
    break;
    default:
    serializeJson(doc, Serial);
    break;
  }


}

bool Monscape::Init_Trame() {
  String buffer;
  //_Nom_sys, _Ver_G, _Ver_ms, _Desc_Game;
  doc["Nom"] = _Nom_sys;
  doc["V_G"] = _Ver_G;
  doc["V_mon"] = "0.2";
  doc["Desc"] = _Desc_Game;
  doc["Win"] = _Win_Code;


  switch (_Protocole) {
    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;
    case MSCape_RS485:

    digitalWrite(_RS485Pin, HIGH);
    serializeJson(doc, buffer);

#if defined(ESP32)
    Serial2.println(buffers);
#else
    mySerial->println(buffer);
#endif
    serializeJson(doc, Serial);
    Serial.println(' ');
    digitalWrite(_RS485Pin, LOW);

    break;
    case MSCape_I2C:
      // statements
    break;
    default:
    serializeJson(doc, Serial);
    break;
  }


}

//  String last_input;
//  char stat;
//  char input;

void Monscape::set_Trame_Last_Input(char* value, int sizet) {
  String str(value);
  _Trame.last_input = str.substring(0, sizet);
}

void Monscape::set_Trame_Last_Input(int value) {
  String str(value);
  _Trame.last_input = str;
}

void Monscape::set_Trame_Last_Input(long value) {
  String str(value);
  _Trame.last_input = str;
}

void Monscape::set_Trame_Stat(char value) {
  _Trame.stat = value;
}

void Monscape::set_Trame_Input(char* value, int sizet) {
  String str(value);
  _Trame.input = str.substring(0, sizet);
}

void Monscape::set_Trame_Input(long value) {
  String str(value);
  _Trame.input = str;
}

void Monscape::set_Trame_Input(int value) {
  String str(value);
  _Trame.input = str;
}

#if defined(ESP32)
bool Monscape::Listenserv() {
  String inData;

  digitalWrite(_RS485Pin, LOW);
  if (Serial2.available() > 0)
  {
    //Serial.println("A");
    inData = Serial2.readStringUntil('\n');
    //Serial.println("B");
    //Serial.println("data: " + inData);
    deserializeJson(doc, inData);
    BasicCommand();

    //else Serial.print("not for me");
    inData = "";

  }
}

#else

bool Monscape::Listenserv() {
  String inData;

  digitalWrite(_RS485Pin, LOW);
  if (mySerial->available() > 0)
  {
    //Serial.println("A");
    inData = mySerial->readStringUntil('\n');
    //Serial.println("B");
    //Serial.println("data: " + inData);
    deserializeJson(doc, inData);
    BasicCommand();

    //else Serial.print("not for me");
    inData = "";
  }
}

#endif


bool Monscape::BasicCommand(){
  String Adresse = doc["A"];
    //Serial.println(Adresse);

  if (Adresse == _Adresse) {
      //Serial.println("commande passé");
    const char* Comm = doc["C"];
      //Serial.println(Comm);
    switch (Comm[0]) {
      case 'R':
      Serial.println("Reset");
      delay(100);
      resetard();
      break;
      case 'B':
      Win(true);
      break;
      case 'X':
          // statements
      break;
      case 'S':
          // statements
      break;
      case 'I':
      Log_Trame();
      break;
      default:

      special_command(doc);
      break;

    }
    doc.clear();
  }

}


bool Monscape::Win(bool bypass = false)
{

  for (int i = 0 ; i == _Num_Relais; i++) digitalWrite(_Pin_Relai[i], HIGH);
    if (bypass == true)  set_Trame_Stat(MSC_BYPASS);
  else set_Trame_Stat(MSC_WIN);
  Log_Trame();
}



void Monscape::resetard() {

  #if defined(ESP32)
ESP.restart();
#else
  asm volatile ("  jmp 0");  //declare reset function @ address 0
  #endif
}



