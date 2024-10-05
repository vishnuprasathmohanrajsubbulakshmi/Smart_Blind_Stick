#include <vl53l8cx.h>

#define PWR_MOT_1 3 
#define PWR_MOT_2 5
#define PWR_MOT_3 6  
#define GND_MOT 4

#ifdef ARDUINO_SAM_DUE
  #define DEV_I2C Wire1
#else
  #define DEV_I2C Wire
#endif
#define SerialPort Serial

#define LPN_PIN_1 A3
#define PWREN_PIN_1 11

#define LPN_PIN_2 A4
#define PWREN_PIN_2 12

#define LPN_PIN_3 A5
#define PWREN_PIN_3 13

void print_result(VL53L8CX_ResultsData *Result);
void clear_screen(void);
void handle_cmd(uint8_t cmd);
void display_commands_banner(void);

// Components.
VL53L8CX sensor_1(&DEV_I2C, LPN_PIN_1);
VL53L8CX sensor_2(&DEV_I2C, LPN_PIN_2);
VL53L8CX sensor_3(&DEV_I2C, LPN_PIN_3);

bool EnableAmbient = false;
bool EnableSignal = false;
uint8_t res = VL53L8CX_RESOLUTION_4X4;
char report[256];
uint8_t status;

void setup() {
    pinMode(PWR_MOT_1, OUTPUT);
    pinMode(PWR_MOT_2, OUTPUT);
    pinMode(PWR_MOT_3, OUTPUT);
    pinMode(GND_MOT, OUTPUT);
    digitalWrite(GND_MOT, LOW);
    SerialPort.begin(460800);
    SerialPort.println("Entered Setup");

    DEV_I2C.begin();

    if (PWREN_PIN_1 >= 0) {
    pinMode(PWREN_PIN_1, OUTPUT);
    digitalWrite(PWREN_PIN_1, HIGH);
    delay(10);
    }

    if (PWREN_PIN_2 >= 0) {
    pinMode(PWREN_PIN_2, OUTPUT);
    digitalWrite(PWREN_PIN_2, HIGH);
    delay(10);
  }
  // Enable PWREN right pin if present
  if (PWREN_PIN_3 >= 0) {
    pinMode(PWREN_PIN_3, OUTPUT);
    digitalWrite(PWREN_PIN_3, HIGH);
    delay(10);
  }
    
    // Initialize I2C and sensor setup
   

    sensor_1.begin();
    sensor_1.off();

    sensor_2.begin();
    sensor_2.off();

    sensor_3.begin();
    sensor_3.off();
    SerialPort.println("Begin Finished");

    sensor_1.on();
    sensor_1.set_i2c_address(0x10);
    sensor_1.init();
    SerialPort.println("sen_1 Init Finished");
    sensor_2.on();
    sensor_2.set_i2c_address(0x12);
    sensor_2.init();
    SerialPort.println("sen_2 Init Finished");
    sensor_3.on();
    sensor_3.set_i2c_address(0x14);
    sensor_3.init();
    SerialPort.println("sen_3 Init Finished");

    sensor_1.start_ranging();
    SerialPort.println("ranging 1 stared");
    sensor_2.start_ranging();
    SerialPort.println("ranging 2 stared");
    sensor_3.start_ranging();
    SerialPort.println("ranging 3 stared");
}

void loop() {
    VL53L8CX_ResultsData Results1, Results2, Results3;


    // Check data readiness and read data
    uint8_t NewDataReady = 0;
    do {
        sensor_1.check_data_ready(&NewDataReady);
    } while (!NewDataReady);

    sensor_1.get_ranging_data(&Results1);
    print_result(&Results1, 1);  // Print the results with sensor ID as 1


    // Repeat the above steps for sensors 2 and 3
    NewDataReady = 0;
    do {
        status = sensor_2.check_data_ready(&NewDataReady);
    } while (!NewDataReady);
    if ((!status) && (NewDataReady != 0)) {
        status = sensor_2.get_ranging_data(&Results2);
        print_result(&Results2, 2);
    }

    NewDataReady = 0;
    do {
        status = sensor_3.check_data_ready(&NewDataReady);
    } while (!NewDataReady);
    if ((!status) && (NewDataReady != 0)) {
        status = sensor_3.get_ranging_data(&Results3);
        print_result(&Results3, 3);
    }

    if (Serial.available() > 0) {
        handle_cmd(Serial.read());
    }
    delay(100);
    SerialPort.println("");
}

void print_result(VL53L8CX_ResultsData *Result, int sensor_id) {
  //SerialPort.println("Entered print");
    int8_t i, j;
    uint8_t zones_per_line;
    uint8_t number_of_zones = res;
    bool low = false, medium = false, high = false;
    bool noDetection = true;  // Assume no detection initially

    zones_per_line = (number_of_zones == 16) ? 4 : 8;

    for (j = 0; j < number_of_zones; j += zones_per_line) {
        for (i = 0; i < zones_per_line; i++) {
            int index = j + i;
            if (Result->nb_target_detected[index] > 0) {
                long distance = Result->distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * index];
                long status = Result->target_status[VL53L8CX_NB_TARGET_PER_ZONE * index];

                if (status == 4 || status == 5) {
                    if (distance > 1000) {
                        continue;
                    }
                    noDetection = false;
                    if (distance <= 10) {
                        high = true;
                    } else if (distance > 10 && distance <= 250) {
                        medium = true;
                    } else if (distance > 250 && distance <= 1000) {
                        low = true;
                    }
                }
            }
        }
    }

    // Print sensor ID
    SerialPort.print("Sensor ");
    SerialPort.print(sensor_id);
    SerialPort.print(": ");

    if (high) {
      SerialPort.println("Distance: Near | Mot_Speed: High");
      if (sensor_id == 1)
        {
          analogWrite(PWR_MOT_1, 255);
        }
        else if (sensor_id == 2)
        {
          analogWrite(PWR_MOT_2, 255);
        }
        else if (sensor_id == 3)
        {
          analogWrite(PWR_MOT_3, 255);
        } 
    } else if (medium) {
        SerialPort.println("Distance: Medium | Mot_Speed: Medium");
        if (sensor_id == 1)
        {
          analogWrite(PWR_MOT_1, 170);
        }
        else if (sensor_id == 2)
        {
          analogWrite(PWR_MOT_2, 170);
        }
        else if (sensor_id == 3)
        {
          analogWrite(PWR_MOT_3, 170);
        }
    } else if (low) {
        SerialPort.println("Distance: Far | Mot_Speed: Low");
        if (sensor_id == 1)
        {
          analogWrite(PWR_MOT_1, 85);
        }
        else if (sensor_id == 2)
        {
          analogWrite(PWR_MOT_2, 85);
        }
        else if (sensor_id == 3)
        {
          analogWrite(PWR_MOT_3, 85);
        }
    } else if (noDetection) {
        SerialPort.println("No object detected within range or object is too far");
        if (sensor_id == 1)
        {
          analogWrite(PWR_MOT_1, 0);
        }
        else if (sensor_id == 2)
        {
          analogWrite(PWR_MOT_2, 0);
        }
        else if (sensor_id == 3)
        {
          analogWrite(PWR_MOT_3, 0);
        }
    }
    
}



void toggle_resolution(void)
{
  status = sensor_1.stop_ranging();

  switch (res) {
    case VL53L8CX_RESOLUTION_4X4:
      res = VL53L8CX_RESOLUTION_8X8;
      break;

    case VL53L8CX_RESOLUTION_8X8:
      res = VL53L8CX_RESOLUTION_4X4;
      break;

    default:
      break;
  }
  status = sensor_1.set_resolution(res);
  status = sensor_1.start_ranging();
}

void toggle_signal_and_ambient(void)
{
  EnableAmbient = (EnableAmbient) ? false : true;
  EnableSignal = (EnableSignal) ? false : true;
}

void clear_screen(void)
{
  snprintf(report, sizeof(report), "%c[2J", 27); /* 27 is ESC command */
  SerialPort.print(report);
}

void display_commands_banner(void)
{
  snprintf(report, sizeof(report), "%c[2H", 27); /* 27 is ESC command */
  SerialPort.print(report);

  Serial.print("53L8A1 Simple Ranging demo application\n");
  Serial.print("--------------------------------------\n\n");

  Serial.print("Use the following keys to control application\n");
  Serial.print(" 'r' : change resolution\n");
  Serial.print(" 's' : enable signal and ambient\n");
  Serial.print(" 'c' : clear screen\n");
  Serial.print("\n");
}

void handle_cmd(uint8_t cmd)
{
  switch (cmd) {
    case 'r':
      toggle_resolution();
      clear_screen();
      break;

    case 's':
      toggle_signal_and_ambient();
      clear_screen();
      break;

    case 'c':
      clear_screen();
      break;

    default:
      break;
  }
}
