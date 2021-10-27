// Simulated measurement intervall
unsigned long startMillisecond;  
const unsigned long measurePeriod = 2000;

HardwareSerial& num2Port = Serial1;
HardwareSerial& coPort = Serial2;

const byte numChars = 128;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

// variables to hold the parsed data
int num2;
int num2temp;
int num2hum;

boolean newData = false;

//============

void setup()

{
    Serial.begin(9600);
    num2Port.begin(9600);
    Serial.println("Collecting and parsing measurement string from a SPEC sensor.");
    Serial.println();
    
}

//============

void loop() {
  
  if (millis() - startMillisecond >= measurePeriod) {
    startMillisecond = millis();
    Serial.println("Read Num2 Sensor:");
    ReadSPECSensor(num2, num2temp, num2hum, num2Port);
    Serial.println("-----------------------------------");
  }
}

//============

void recvWithEndMarker(HardwareSerial& gasPort) {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
   
    while (gasPort.available() > 0 && newData == false) {
        rc = gasPort.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
            Serial.println("Newdata is collected ...");
        }
    }
}

//============

void parseData(int &sensorGas, int &sensorTemp, int &sensorHum) {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index
    
    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strtokIndx = strtok(NULL, ","); 
    sensorGas = atoi(strtokIndx);     // convert this part to an integer
    strtokIndx = strtok(NULL, ",");
    sensorTemp = atoi(strtokIndx);     // convert this part to an integer
    strtokIndx = strtok(NULL, ",");
    sensorHum = atoi(strtokIndx);     // convert this part to an integer

}

//============

void showParsedData(int gas, int temp, int hum) {
    Serial.print("Gasconc.: ");
    Serial.println(gas);
    Serial.print("Temp. : ");
    Serial.println(temp);
    Serial.print("Humid. : ");
    Serial.println(hum);
    if (gas > 10000 || temp > 1000 || hum > 1000) { // in the case received string was wrongly read (mixed up)
      Serial.println("Stop exexcution");
      stop();
    }

}

void showNewData() {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
}

void ReadSPECSensor(int &gas, int &temp, int &hum, HardwareSerial& gasPort) {
  gasPort.print('\r');  // print to gasport to get single measurement string 
  recvWithEndMarker(gasPort);
  if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            // because strtok() used in parseData() replaces the commas with \0
        showNewData();    
        parseData(gas, temp, hum);
        showParsedData(gas, temp, hum);
        newData = false;
  }
}


void stop() {
  while(1);
}
