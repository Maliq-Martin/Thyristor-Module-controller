#include <Arduino.h>
#include <SoftwareSerial.h>
struct module
{
  char number;        // module number
  int voltage;        // Anode GND Voltage
  int code;           // fault code
  int temp;           // module temperature
  boolean Bdot_Fault; // High if b Dot fault detected
  boolean comms;      // High if comms link established and not in timeout
  int delay;          // delay
  int delay_case;     // swtich case number for calculting delay
  int fan;            // fan speed from tacho
};

// Code

// No COMMS | Gate Triggered | Gate Overvoltage | Overtemp 1st warning | Ovetemp 2nd Warning | Bdot Fault
//    32          16                    8                     4                 2             1

module my_module;
SoftwareSerial IR(10, 11);

int Bdot_Stack = 12;
int Bdot_Fault = 2;
int Latch_RST = 3;
int Gate_Trig = 20;
int Gate_OV = 21;
int Bdot_Stack_LED = 18;
int Fault_LED = 4;
int Fibre_LED = 9;
int T_40C = 14;
int T_60C = 15;
int Fibre_RX = 5;
int x;
int V_div = A3;
int Temp_IC = A2;

void Fibre_RX_Event()
{
  digitalWrite(Fibre_LED, HIGH);
}

void Gate_Trig_Event()
{
    x = my_module.code;
    my_module.code = x | 16;
  }

void setup()
{
  Serial.begin(9600);
  IR.begin(4800);
  my_module.number = 'A';

  pinMode(Bdot_Stack, INPUT);      // Bdot Stack event
  pinMode(Bdot_Fault, INPUT);      // Bdot Fault event
  pinMode(Latch_RST, OUTPUT);      // Latch Reset
  digitalWrite(Latch_RST, HIGH);
  pinMode(Gate_OV, INPUT);         // Gate Overvoltage event
  pinMode(Fault_LED, OUTPUT);      // RED fault LED
  digitalWrite(Fault_LED, LOW);
  pinMode(Bdot_Stack_LED, OUTPUT); // Thryistor fired
  digitalWrite(Bdot_Stack_LED, LOW);
  pinMode(Fibre_LED, OUTPUT);      // Trigger received from Fibre
  digitalWrite(Fibre_LED, LOW);
  pinMode(T_40C, INPUT);           // T > 40C
  pinMode(T_60C, INPUT);           // T> 60C

  attachInterrupt(digitalPinToInterrupt(Fibre_RX), Fibre_RX_Event, RISING);// interrupt if Trigger event detected on FIBRE RX
  attachInterrupt(digitalPinToInterrupt(Gate_Trig), Gate_Trig_Event, RISING);// interrupt if Trigger event detected on FIBRE RX
  digitalWrite(Latch_RST,LOW);
}

void loop()
{
/*
  my_module.voltage = analogRead(V_div);
  Serial.print("V_div: ");
  Serial.println(my_module.voltage);
  my_module.temp = analogRead(Temp_IC);
  Serial.print("Temp: ");
  Serial.println(my_module.temp);
  Serial.print("Code: ");
  Serial.println(my_module.code, BIN);
  Serial.println("     Gate Triggered | Gate Overvoltage | Overtemp 1st warning | Ovetemp 2nd Warning | Bdot Fault");
  Serial.println("--------------------------------------------------------------------------------------------------");
  Serial.println();
  delay(2000);
  */

  my_module.voltage = analogRead(V_div);
  my_module.temp = ((analogRead(Temp_IC)*0.004887)-0.5)/0.01;
  // Codes are cleared after successful IR comms via latch reset
  if (digitalRead(T_40C) == LOW)// external pull up to 5V
  {
    x = my_module.code;
    my_module.code = x | 4;
  }
  if (digitalRead(T_60C) == LOW)// external pull up to 5V
  {
    x = my_module.code;
    my_module.code = x | 2;
    digitalWrite(Fault_LED, HIGH);
    digitalWrite(Fault_LED, HIGH);
  }
  if (digitalRead(Bdot_Fault) == HIGH)
  {
    x = my_module.code;
    my_module.code = x | 1;
    digitalWrite(Fault_LED, HIGH);
  }
  if (digitalRead(Bdot_Stack) == HIGH)
  {
    digitalWrite(Bdot_Stack_LED, HIGH);
  }
  if (digitalRead(Gate_OV) == HIGH)
  {
    x = my_module.code;
    my_module.code = x | 8;
  }
  if (IR.available())
  {
    // bytes in buffer
    Serial.print("Polling Request Received: ");
    while (IR.available() > 0)
    {
      char inByte = IR.read();
      if (inByte == my_module.number)
      {
        while (IR.available() > 0)
        {
          char dump = IR.read();
        }
        // correct module number
        Serial.println("For me");
        // send data via IR
        String data = String(my_module.number) + ' ' + String(my_module.number) + ' ' + String(my_module.voltage) + ' ' + String(my_module.temp) + ' ' + String(my_module.code);
        IR.println(data);
        IR.flush();
        Serial.println("Sent via IR: " + data);
        Serial.flush();
        digitalWrite(Fault_LED, LOW);
        digitalWrite(Fibre_LED, LOW);
        digitalWrite(Latch_RST, HIGH);
        delayMicroseconds(10);
        digitalWrite(Latch_RST, LOW);
        my_module.code=0;
      }
      else
      {
        Serial.println("Not for me");
        Serial.println(inByte);
      }
      IR.flush();
    }
  }
}
