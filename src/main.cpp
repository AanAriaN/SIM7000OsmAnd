#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial SerialPort(2); // use UART2

String response = "";
void setup()
{

  Serial.begin(115200);
  SerialPort.begin(19200, SERIAL_8N1, 16, 17);
  Serial.println("Begin");

  //Test AT
  SerialPort.write("AT");
  response = SerialPort.readString();
  Serial.println(response);
  if (response = "OK") {
    delay(1000);
  }
  else
  {
    Serial.println("ERROR");
    delay(1000);
  }

  //Change Baud To 19200
  while (1) {
    SerialPort.write("AT+IPR=19200\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      Serial.println("Serial=19200");
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Check SIM Card Status
  while (1) {
    SerialPort.write("AT+CPIN?\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "READY") {
      Serial.println("SIM OK");
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Check Signal Quality
  SerialPort.write("AT+CSQ\r\n");
  response = SerialPort.readString();
  Serial.println(response);

  //Set Mode
  //AT+CNMP=<mode>
  //2 = Auto
  //  13 = GSM Only
  //  38 = LTE Only
  //  51 = GSM and LTE Only
  while (1) {
    SerialPort.write("AT+CNMP=13\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      Serial.println("Mode=GPRS");
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Preferred Selection between CAT-M and NB-IoT


  while (1) {
    SerialPort.write("AT+CMNB=1\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      Serial.println("CAT-M");
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Attach or Detach from GPRS Service
  while (1) {
    SerialPort.write("AT+CGATT=1\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      Serial.println("Attached");
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Start Task and Set APN, USER NAME, PASS
  while (1) {
    SerialPort.write("AT+CSTT=\"M2MAUTOTRONIC\"\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Bring Up Wireless Connection with GPRS
  while (1) {
    SerialPort.write("AT+CIICR\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Get Local IP Address
  while (1) {
    SerialPort.write("AT+CIFSR\r\n");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  delay(2000);

  //Start Up TCP or UDP Connection
  while (1) {
    SerialPort.write("AT+CIPSTART=\"TCP\",\"156.67.216.137\",4000");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "OK") {
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }
  Serial.println(response);



  delay(1000);

  //Send Data
  while (1) {
    SerialPort.write("AT+CIPSEND=12");
    SerialPort.write("HELLO WORLD");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "SEND OK") {
      Serial.println("Send");
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }

  //Stop TCP or UDP Connection
  while (1) {
    SerialPort.write("AT+CIPSHUT");
    response = SerialPort.readString();
    Serial.println(response);
    if (response = "SHUT OK") {
      Serial.println("Disconnected");
      delay(1000);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(1000);
    }
  }
}

void loop()
{

}