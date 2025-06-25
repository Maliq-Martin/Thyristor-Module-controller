#include <Arduino.h>
#include <SoftwareSerial.h>

struct module
{
  char number;    // module number
  int voltage;    // Anode Cathode Voltage
  int fault;      // fault code recieved from moduule via IR
  int temp;       // module temperature
  boolean BDot;   // High if b Dot fault detected
  boolean comms;  // High if comms link established and not in timeout
  int delay;      // delay
  int delay_case; // swtich case number for calculting delay
  int fan;        //  fan speed from tacho
  boolean mux_60_A;
  boolean mux_60_B;
  boolean mux_60_C;
  boolean mux_200_A;
  boolean mux_200_B;
  boolean mux_200_C;
  boolean inhibit;
};

module my_module;

SoftwareSerial IR(10, 11);

void setup()
{
  Serial.begin(9600);
  IR.begin(9600);
}

void loop()
{
  my_module.number = 'A';
  my_module.voltage = random(2000, 4000);
  my_module.temp = random(20, 100);
  my_module.comms = 1;
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
        String data = String(my_module.number) + ' ' + String(my_module.voltage) + ' ' + String(my_module.fault) + ' ' + String(my_module.temp) + ' ' + String(my_module.BDot) + ' ' + String(my_module.comms);
        IR.println(data);
        IR.flush();
        Serial.println("Sent via IR: " + data);
        Serial.flush();
      }
      else
      {
        Serial.println("Not for me");
      }
      IR.flush();
    }
  }
  // put your main code here, to run repeatedly:
}
