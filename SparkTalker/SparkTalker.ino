// Based on code from https://github.com/jrnelson90/tinderboxpedal 
//
// Adapted for M5Stack Core and uses the three buttons to select presets 1-3
//
//
#include "BluetoothSerial.h" // https://github.com/espressif/arduino-esp32
#include <M5Stack.h>

const String SPARK_BT_NAME = "Spark 40 Audio";

const int PRESET_CMD_SIZE = 27;

byte LOAD_TONE_PRESET_1_CMD[] = {
  0x01, 0xFE, 0x00, 0x00,
  0x53, 0xFE, 0x1A, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xF0, 0x01, 0x24, 0x00,
  0x01, 0x38, 0x00, 0x00,
  0x00, 0xF7
};

byte LOAD_TONE_PRESET_2_CMD[] = {
  0x01, 0xFE, 0x00, 0x00,
  0x53, 0xFE, 0x1A, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xF0, 0x01, 0x24, 0x00,
  0x01, 0x38, 0x00, 0x00,
  0x01, 0xF7
};

byte LOAD_TONE_PRESET_3_CMD[] = {
  0x01, 0xFE, 0x00, 0x00,
  0x53, 0xFE, 0x1A, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xF0, 0x01, 0x24, 0x00,
  0x01, 0x38, 0x00, 0x00,
  0x02, 0xF7
};

byte LOAD_TONE_PRESET_4_CMD[] = {
  0x01, 0xFE, 0x00, 0x00,
  0x53, 0xFE, 0x1A, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xF0, 0x01, 0x24, 0x00,
  0x01, 0x38, 0x00, 0x00,
  0x03, 0xF7
};

byte* LOAD_TONE_PRESET_LIST[] = {
  LOAD_TONE_PRESET_1_CMD, 
  LOAD_TONE_PRESET_2_CMD, 
  LOAD_TONE_PRESET_3_CMD, 
  LOAD_TONE_PRESET_4_CMD
};

BluetoothSerial SerialBT;

const String DEVICE_NAME = "SparkTalker";
bool connected, conn_failure;

void sendPreset(int preset) 
{
  SerialBT.write(LOAD_TONE_PRESET_LIST[preset-1], PRESET_CMD_SIZE);
}

void displayStartup() 
{
    M5.Lcd.setBrightness(100);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println("Spark Talker");
}

void displayStatus(String status_msg, int col)
{
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0,30);
    M5.Lcd.fillRect(0,30,M5.Lcd.width()-1,60,col);
    M5.Lcd.print(status_msg); 
}

void displayPreset(String status_msg)
{
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(0,100);
    M5.Lcd.fillRect(0,100,M5.Lcd.width()-1,60,BLUE);
    M5.Lcd.print("Preset - ");
    M5.Lcd.print(status_msg); 
}


void btEventCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  // On BT connection close
  if(event == ESP_SPP_CLOSE_EVT )
  {
    connected = false;
    conn_failure = true;
  }
}

void btInit() 
{
  SerialBT.register_callback(btEventCallback);
  if(!SerialBT.begin(DEVICE_NAME, true))
  { 
    displayStatus("Bluetooth init failed", RED); 
    while(true){};
  }
}

void connectToAmp() 
{
  while(!connected) 
  {
    if (!conn_failure) displayStatus("Connecting...", BLUE);
    connected = SerialBT.connect(SPARK_BT_NAME);

    if (connected && SerialBT.hasClient()) 
    {
      displayStatus("Connected!", GREEN);
      conn_failure = false;
    } 
    else 
    { 
      connected = false;
      conn_failure = false;
      displayStatus("Connection failed, retry", RED);      
      delay(3000);
    }
  }
}

void setup() 
{
  connected = false;
  conn_failure = false;
  
  M5.begin();
  M5.Power.begin();
  displayStartup();
  displayPreset("None");
  btInit();
}

void loop() {
  if(!connected) 
  {
    connectToAmp();
  } 
  else 
  { 
    M5.update();
    if(M5.BtnA.wasPressed()) 
    {
      displayPreset("1");
      sendPreset(1);    
    }
    if(M5.BtnB.wasPressed()) 
    {
      displayPreset("2");
      sendPreset(2);
    } 
    if(M5.BtnC.wasPressed()) 
    {
      displayPreset("3");    
      sendPreset(3);
    } 

    // Clear BT message buffer
    if (SerialBT.available()) {
      SerialBT.read();
    }
  }
}
