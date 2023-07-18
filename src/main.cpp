#include <Arduino.h>
#include <HardwareSerial.h>
#include <UnixTime.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

int YEAR, MONTH, DAY, HOUR, MINUTE, SECONDS;

UnixTime stamp(0);
HardwareSerial SerialPort(2); // use UART2
String latitude, longitude;
String response = "";
String IMEI = "";
bool checkSendCmd(const char *cmd, const char *resp)
{
  char SIMbuffer[100];
  String response;
  SerialPort.write(cmd);
  Serial.println(cmd);
  delay(100);
  if (SerialPort.available() > 0)
  {
    response = SerialPort.readString();
    Serial.println(response);

    SerialPort.flush();
  }
  response.toCharArray(SIMbuffer, response.length() + 1);
  if (NULL != strstr(SIMbuffer, resp))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup()
{
  pinMode(27, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(27, LOW);
  delay(500);
  digitalWrite(27, HIGH);
  Serial.begin(115200);
  SerialPort.begin(19200, SERIAL_8N1, 16, 17);
  Serial.println("Begin");

  // Test AT+CPIN: READY
  while (1)
  {
    char SIMbuffer[100];
    SerialPort.write("AT\r\n");
    delay(500);
    String response = SerialPort.readString();
    Serial.println(response);
    response.toCharArray(SIMbuffer, response.length() + 1);
    if (NULL != strstr(SIMbuffer, "+CPIN: READY"))
    {
      Serial.println("OK");
      delay(10);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(10);
    }
  }
  delay(200);

  // // Change Baud To 19200
  // while (1)
  // {
  //   if (checkSendCmd("AT+IPR=19200\r\n", "OK"))
  //   {
  //     Serial.println("Serial=19200");
  //     break;
  //   }
  //   else
  //   {
  //     Serial.println("ERROR");
  //     delay(2000);
  //   }
  // }

  // Set Functionality
  checkSendCmd("AT+CFUN=1\r\n", "0K");

  // Get IMEI
  while (1)
  {
    String data[8];
    SerialPort.write("AT+GSN\r\n");
    if (SerialPort.available() > 0)
    {
      IMEI = SerialPort.readString();
      data[0] = getValue(IMEI, '\n', 0);
      Serial.print("Data1: ");
      Serial.println(data[0]);
      data[1] = getValue(IMEI, '\n', 1);
      Serial.print("IMEI: ");
      Serial.println(data[1]);
      IMEI = data[1];
      Serial.println(IMEI);
      if (IMEI != "ERROR")
      {
        Serial.println("IMEI OK");
        delay(100);
        break;
      }
      else
      {
        Serial.println("ERROR");
        delay(100);
      }
    }
  }

  // Start Single IP Mode
  while (1)
  {
    if (checkSendCmd("AT+CIPMUX=0\r\n", "OK"))
    {
      Serial.println("CIPMUX OK");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  // Check SIM Card Status
  while (1)
  {
    if (checkSendCmd("AT+CPIN?\r\n", "READY"))
    {
      Serial.println("SIM OK");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  checkSendCmd("AT+CGNSPWR=0\r\n", "OK");
  delay(100);

  // Init GNSS
  while (1)
  {
    if (checkSendCmd("AT+CGNSPWR=1\r\n", "OK"))
    {
      Serial.println("GNSS OK");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  checkSendCmd("AT+CGNSPWR?\r\n", "1");

  // Init GNSS
  while (1)
  {
    if (checkSendCmd("AT+CGNSMOD=1,1,1,1\r\n", "OK"))
    {
      Serial.println("MOD OK");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  // Check Signal Quality
  checkSendCmd("AT+CSQ\r\n", "OK");

  // Set Mode
  // AT+CNMP=<mode>
  // 2 = Auto
  //   13 = GSM Only
  //   38 = LTE Only
  //   51 = GSM and LTE Only
  while (1)
  {
    if (checkSendCmd("AT+CNMP=13\r\n", "OK"))
    {
      Serial.println("Mode=GPRS");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  // Preferred Selection between CAT-M and NB-IoT

  while (1)
  {
    if (checkSendCmd("AT+CMNB=1\r\n", "OK"))
    {
      Serial.println("CAT-M");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  // Attach or Detach from GPRS Service
  while (1)
  {
    if (checkSendCmd("AT+CGATT=1\r\n", "OK"))
    {
      Serial.println("Attached");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  // Start Task and Set APN, USER NAME, PASS
  while (1)
  {
    char SIMbuffer[100];
    SerialPort.write("AT+CSTT=\"M2MAUTOTRONIC\"\r\n");
    delay(500);
    if (SerialPort.available() > 0)
    {
      String response = SerialPort.readString();
      Serial.println(response);
      response.toCharArray(SIMbuffer, response.length() + 1);
      if (NULL != strstr(SIMbuffer, "OK"))
      {
        Serial.println("APN OK");
        delay(100);
        break;
      }
      else
      {
        Serial.println("ERROR");
        delay(100);
      }
    }
  }

  // Bring Up Wireless Connection with GPRS
  while (1)
  {
    if (checkSendCmd("AT+CIICR\r\n", "OK"))
    {
      Serial.println("GPRS OK");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  // Get Local IP Address
  checkSendCmd("AT+CIFSR\r\n", "OK");
  delay(500);

  //  Bearer Settings for Applications Based on IP
  while (1)
  {
    if (checkSendCmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n", "OK"))
    {
      Serial.println("GPRS Conf Ok");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  while (1)
  {
    if (checkSendCmd("AT+SAPBR=3,1,\"APN\",\"M2MAUTOTRONIC\"\r\n", "OK"))
    {
      Serial.println("APN Conf Ok");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  while (1)
  {
    if (checkSendCmd("AT+SAPBR=1,1\r\n", "OK"))
    {
      Serial.println("Bearer Connected");
      delay(100);
      break;
    }
    else
    {
      Serial.println("ERROR");
      delay(100);
    }
  }

  checkSendCmd("AT+SAPBR=2,1\r\n", "OK");
  delay(100);
  int send = 0;

  while (1)
  {
    if (send == 0)
    {
      checkSendCmd("AT+HTTPINIT\r\n", "OK");
    }
    // Init HTTP
    int datalenght;
    String gnss = "";
    String data[22];
    String times = "";
    String buff;
    char bufferunix[11];
    // 1689420424
    char buffer[33];
    char SIMbuffer[100];
    SerialPort.write("AT+CGNSINF\r\n");
    send = 1;
    if (SerialPort.available() > 0)
    {
      gnss = SerialPort.readString();
    }
    delay(1000);

    gnss.toCharArray(SIMbuffer, gnss.length() + 1);
    if (NULL != strstr(SIMbuffer, "OK"))
    {
      Serial.println(gnss);
      data[0] = getValue(gnss, ',', 0);
      Serial.print("Init: ");
      Serial.println(data[0]);
      data[1] = getValue(gnss, ',', 1);
      Serial.print("Fix: ");
      Serial.println(data[1]);
      data[2] = getValue(gnss, ',', 2);
      Serial.print("Time: ");
      Serial.println(data[2]);
      data[3] = getValue(gnss, ',', 3);
      Serial.print("Latitude: ");
      Serial.println(data[3]);
      data[4] = getValue(gnss, ',', 4);
      Serial.print("Longitude: ");
      Serial.println(data[4]);
      data[5] = getValue(gnss, ',', 5);
      Serial.print("Altitude: ");
      Serial.println(data[5]);
      data[6] = getValue(gnss, ',', 6);
      Serial.print("Speed: ");
      Serial.println(data[6]);
      data[7] = getValue(gnss, ',', 7);
      Serial.print("Angle: ");
      Serial.println(data[7]);
      data[8] = getValue(gnss, ',', 8);
      Serial.print("Fix Mode: ");
      Serial.println(data[8]);
      data[9] = getValue(gnss, ',', 9);
      Serial.print("Reserved1: ");
      Serial.println(data[9]);
      data[10] = getValue(gnss, ',', 10);
      Serial.print("HDOP: ");
      Serial.println(data[10]);
      data[11] = getValue(gnss, ',', 1);
      Serial.print("PDOP: ");
      Serial.println(data[11]);
      data[12] = getValue(gnss, ',', 12);
      Serial.print("VDOP: ");
      Serial.println(data[12]);
      data[13] = getValue(gnss, ',', 13);
      Serial.print("Reserved2: ");
      Serial.println(data[13]);
      data[14] = getValue(gnss, ',', 14);
      Serial.print("GNSS: ");
      Serial.println(data[14]);
      data[15] = getValue(gnss, ',', 15);
      Serial.print("GPS: ");
      Serial.println(data[15]);
      data[16] = getValue(gnss, ',', 16);
      Serial.print("GLONASS: ");
      Serial.println(data[16]);
      data[17] = getValue(gnss, ',', 17);
      Serial.print("Reserved3: ");
      Serial.println(data[17]);
      data[18] = getValue(gnss, ',', 18);
      Serial.print("CNO max: ");
      Serial.println(data[18]);
      data[19] = getValue(gnss, ',', 19);
      Serial.print("HPA: ");
      Serial.println(data[19]);
      data[20] = getValue(gnss, ',', 20);
      Serial.print("VPA: ");
      Serial.println(data[20]);
    }
    if (data[1] == "1")
    {
      buff = String(data[2].charAt(0)) + String(data[2].charAt(1)) + String(data[2].charAt(2)) + String(data[2].charAt(3));
      int year;
      buff.toCharArray(buffer, buff.length() + 1);
      year = atoi(buffer);
      Serial.println(buff);
      buff = String(data[2].charAt(4)) + String(data[2].charAt(5));
      int month;
      buff.toCharArray(buffer, buff.length() + 1);
      month = atoi(buffer);
      Serial.println(month);
      buff = String(data[2].charAt(6)) + String(data[2].charAt(7));
      int day;
      buff.toCharArray(buffer, buff.length() + 1);
      day = atoi(buffer);
      Serial.println(day);
      buff = String(data[2].charAt(8)) + String(data[2].charAt(9));
      int hour;
      buff.toCharArray(buffer, buff.length() + 1);
      hour = atoi(buffer);
      Serial.println(hour);
      buff = String(data[2].charAt(10)) + String(data[2].charAt(11));
      int minute;
      buff.toCharArray(buffer, buff.length() + 1);
      minute = atoi(buffer);
      Serial.println(minute);
      buff = String(data[2].charAt(12)) + String(data[2].charAt(13));
      int sec;
      buff.toCharArray(buffer, buff.length() + 1);
      sec = atoi(buffer);
      Serial.println(sec);
      stamp.setDateTime(year, month, day, hour, minute, sec);
      uint32_t unix = stamp.getUnix();
      Serial.print("Unix: ");
      Serial.println(unix);
      sprintf(bufferunix, "%d", unix);

      if (data[1] == "0")
      {
        send = 2;
      }
      delay(500);
      if (data[1] == "1")
      {
        digitalWrite(2, HIGH);
        // String sending = "AT+HTTPPARA=\"URL\",\"http://194.233.80.108:6055/?id=865234033676973&lat=-7.060677&lon=110.447651&timestamp=1689403987\""
        String sending = "AT+HTTPPARA=\"URL\",\"http://194.233.80.108:6055/?id=865234033676973&lat=" + String(data[3]) + "&lon=" + String(data[4]) + "&timestamp=" + String(bufferunix) + "&Altitude=" + String(data[5]) + "&Speed=" + String(data[6]) + "&Angle=" + String(data[7]) + "&GNSS=" + String(data[14]) + "&HDOP=" + String(data[10]) + '"' + "\r\n";
        char sendit[300];
        sending.toCharArray(sendit, sending.length() + 1);
        Serial.println(sendit);
        delay(500);

        SerialPort.write(sendit);
        response = SerialPort.readString();
        Serial.println(response);
        delay(500);

        int count = 200;
        while (1)
        {
          SerialPort.write("AT+HTTPACTION=0\r\n");
          response = SerialPort.readString();
          Serial.println(response);
          response.toCharArray(SIMbuffer, response.length() + 1);
          if (NULL != strstr(SIMbuffer, "200"))
          {
            Serial.println("SEND");
            SerialPort.write("AT+HTTPREAD\r\n");
            delay(1000);
            digitalWrite(2, LOW);
            // Stop TCP or UDP Connection
            checkSendCmd("AT+HTTPTERM\r\n", "OK");
            send = 0;
            break;
          }

          else if (count <= 0 || (NULL != strstr(SIMbuffer, "604")))
          {
            Serial.println("ERROR");
            delay(10);
            send = 0;
            break;
          }
          else
          {
            count--;
          }
        }
        Serial.print("Next");
        delay(2000);
        // }
        // else
        // {
        //   delay(2000);
        // }
      }
    }
  }
}
void loop()
{
}
