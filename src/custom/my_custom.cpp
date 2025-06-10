/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

// USAGE: - Copy this file and rename it to my_custom.cpp
//        - Change false to true on line 9

#include "hasplib.h"

#if defined(HASP_USE_CUSTOM) && HASP_USE_CUSTOM > 0 && true // <-- set this to true in your code

#include "my_custom.h"
#include "hasp_debug.h"

#define ST_CP 17
#define SH_CP 16
#define SHFT_DS 15
#define HR1 7
#define HR2 6
#define MIN1 5
#define MIN2 4
#define SEC 18
#define MUX_SPEED 1
unsigned long prev_mux = 0;

#define GPS_RX 40
#define GPS_BAUD 9600
EspSoftwareSerial::UART gpsSerial;

// cron_job* jobs[];
CronId id;
String alarmHour   = "*";
String alarmMinute = "*";

TinyGPSPlus gps;
// HardwareSerial Serial1(GPS_RX, GPS_TX);

int nums[] = {0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001,
              0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000};

bool gpsUpdate = false;

void smartDelay(int ms)
{
    int prev_ms = millis();
    while(millis() - prev_mux < ms) {
        1 + 1;
        continue;
    }
    return;
}

int GPSTime()
{
    // LOG_VERBOSE(TAG_CUSTOM, "init try");
    unsigned long start = millis();
    time_t t_of_day;
    struct tm t;
    // while(!gpsSerial.available()) {
    // }
    // Serial.print("trying GPS");
    while(millis() - start < 1000) {

        while(gpsSerial.available() > 0) {
            // Serial.print("\n");

            gps.encode(char(gpsSerial.read()));
            // LOG_VERBOSE(TAG_CUSTOM, char(gpsSerial.read()));
            // Serial.print(char(gpsSerial.read()));
            // Serial.print("\n");
        }
        if(gps.date.isUpdated()) {
            // LOG_VERBOSE(TAG_CUSTOM, "yay");
            t.tm_year = gps.date.year() - 1900;
            t.tm_mon  = gps.date.month() - 1; // Month, 0 - jan
            t.tm_mday = gps.date.day();       // Day of the month
            t.tm_hour = gps.time.hour();
            t.tm_min  = gps.time.minute();
            t.tm_sec  = gps.time.second();
            t_of_day  = mktime(&t);
            Serial.print(String(gps.date.year()));
            // LOG_VERBOSE(TAG_CUSTOM, (std::string)String(gps.date.year()));
            // String(gps.date.year()) + "/" + String(gps.date.month()) + "/" +
            //                         String(gps.date.day()) + "," + String(gps.time.hour()) + ":" +
            //                         String(gps.time.minute()) + ":" + String(gps.time.second()));
            // gpsSerial.end();
            return t_of_day;
        }
    }
    // LOG_ERROR(TAG_CUSTOM, "init fail");
    return 0;
}

// void test_cron_job_sample_callback(cron_job* job)
// {
//     /* DO YOUR WORK IN HERE */
//     return;
// }

void crontest()
{
    LOG_VERBOSE(TAG_CUSTOM, "this is alarming");
}

void custom_setup()
{
    // Initialization code here
    randomSeed(millis());
    // gpsSerial.begin(GPS_BAUD, EspSoftwareSerial::SWSERIAL_8N1, GPS_RX, -1, false);
    // time_t now = 1748439452;

    // struct tm timeinfo;

    // timeinfo.tm_hour = 12;
    // timeinfo.tm_min  = 48;
    // timeinfo.tm_year = 2018 - 1900;
    // timeinfo.tm_mon  = 10;
    // timeinfo.tm_mday = 15;
    // // timeinfo.tm_hour = 14;
    // // timeinfo.tm_min = 10;
    // timeinfo.tm_sec = 10;
    // time_t t        = mktime(&timeinfo);
    // printf("Setting time: %s", asctime(&timeinfo));
    // struct timeval nowTmp = {.tv_sec = t};

    // settimeofday(&nowTmp, NULL);

    // clock init
    pinMode(ST_CP, OUTPUT);
    pinMode(SH_CP, OUTPUT);
    pinMode(SHFT_DS, OUTPUT);
    pinMode(HR1, OUTPUT);
    pinMode(HR2, OUTPUT);
    pinMode(MIN1, OUTPUT);
    pinMode(MIN2, OUTPUT);
    pinMode(SEC, OUTPUT);

    // int epoch_time     = GPSTime();
    // timeval epoch      = {epoch_time, 0};
    // const timeval* tv  = &epoch;
    // timezone utc       = {0, 0};
    // const timezone* tz = &utc;
    // settimeofday(tv, tz);

    // gpsSerial.begin(9600, GPS_RX, -1);
    // Serial.print(gpsSerial.read());

    // jobs[0] = cron_job_create("* * * * * *", test_cron_job_sample_callback, (void*)0);
    // jobs[1] = cron_job_create("*/5 * * * * *", test_cron_job_sample_callback, (void*)10000);
    // cron_start();
    Cron.create("0 * * * * *", crontest, false);
}

// int i = 0;

void custom_loop()
{
    Cron.delay();

    // 7seg time
    

    // for(int i = 0; i < 4; i++) {
    //     // take the latchPin low so
    //     // the LEDs don't change while you're sending in bits:
    //     if(i == 0) {
    //         digitalWrite(HR1, HIGH);
    //     } else {
    //         digitalWrite(HR1, LOW);
    //     }
    //     if(i == 1) {
    //         digitalWrite(HR2, HIGH);
    //     } else {
    //         digitalWrite(HR2, LOW);
    //     }
    //     if(i == 2) {
    //         digitalWrite(MIN1, HIGH);
    //     } else {
    //         digitalWrite(MIN1, LOW);
    //     }
    //     if(i == 3) {
    //         digitalWrite(MIN2, HIGH);
    //     } else {
    //         digitalWrite(MIN2, LOW);
    //     }
    //     digitalWrite(ST_CP, LOW);
    //     // shift out the bits:

    //     int numToWrite = 0;

    //     if(i == 0) {
    //         numToWrite = (timeinfo->tm_hour / 10) % 10;
    //     }
    //     if(i == 1) {
    //         numToWrite = (timeinfo->tm_hour) % 10;
    //     }
    //     if(i == 2) {
    //         numToWrite = (timeinfo->tm_min / 10) % 10;
    //     }
    //     if(i == 3) {
    //         numToWrite = (timeinfo->tm_min) % 10;
    //     }

    //     shiftOut(SHFT_DS, SH_CP, MSBFIRST, nums[numToWrite]);
    //     // take the latch pin high so the LEDs will light up:
    //     digitalWrite(ST_CP, HIGH);
    //     // char buff[3];
    //     // sprintf(buff, "NUM: %i", numToWrite);
    //     // LOG_VERBOSE(TAG_CUSTOM, buff);
    //     // prev_mux = millis();
    //     delay(MUX_SPEED);
    // }
    // }
}

int i = 0;

void custom_clock_loop()
{
    time_t rawtime;
    time(&rawtime);
    
    struct tm* timeinfo;
    timeinfo = localtime(&rawtime);
    // for(int i = 0; i < 4; i++) {
    if (!(millis() % 100)) {
        digitalWrite(SEC, timeinfo->tm_sec % 2);
    }
    if(i == 0) {
        digitalWrite(HR1, LOW);
        digitalWrite(HR2, LOW);
        digitalWrite(MIN1, HIGH);
        digitalWrite(MIN2, LOW);
        digitalWrite(ST_CP, LOW);
        shiftOut(SHFT_DS, SH_CP, MSBFIRST, nums[(timeinfo->tm_min / 10) % 10]);
        digitalWrite(ST_CP, HIGH);
        i++;
    } else if(i == 1) {
        digitalWrite(HR1, HIGH);
        digitalWrite(HR2, LOW);
        digitalWrite(MIN1, LOW);
        digitalWrite(MIN2, LOW);
        digitalWrite(ST_CP, LOW);
        shiftOut(SHFT_DS, SH_CP, MSBFIRST, nums[(timeinfo->tm_hour / 10) % 10]);
        digitalWrite(ST_CP, HIGH);
        i++;
    } else if(i == 2) {
        digitalWrite(HR1, LOW);
        digitalWrite(HR2, HIGH);
        digitalWrite(MIN1, LOW);
        digitalWrite(MIN2, LOW);
        digitalWrite(ST_CP, LOW);
        shiftOut(SHFT_DS, SH_CP, MSBFIRST, nums[(timeinfo->tm_hour) % 10]);
        digitalWrite(ST_CP, HIGH);
        i++;
    } else if(i == 3) {
        digitalWrite(HR1, LOW);
        digitalWrite(HR2, LOW);
        digitalWrite(MIN1, LOW);
        digitalWrite(MIN2, HIGH);
        digitalWrite(ST_CP, LOW);
        shiftOut(SHFT_DS, SH_CP, MSBFIRST, nums[(timeinfo->tm_min) % 10]);
        digitalWrite(ST_CP, HIGH);
        i = 0;
    }
    // Cron.delay(1);
    // }
    // }
}

void custom_every_second()
{
    // Serial.print("#");
}

void custom_every_5seconds()
{
    //     LOG_VERBOSE(TAG_CUSTOM, "5 seconds have passsed...");
    //     // dispatch_state_subtopic("my_sensor", "{\"test\":123}");
}

bool custom_pin_in_use(uint8_t pin)
{
    switch(pin) {
        case ST_CP:
        case SH_CP:
        case SHFT_DS:
        case HR1:
        case HR2:
        case MIN1:
        case MIN2:
        case SEC:
        case GPS_RX:
            return true;
        default:
            return false;
    }
}

void custom_get_sensors(JsonDocument& doc)
{
    /* Sensor Name */
    // JsonObject sensor = doc.createNestedObject(F("Custom"));

    /* Key-Value pair of the sensor value */
    // sensor[F("Random")] = HASP_RANDOM(256);
}

void custom_topic_payload(const char* topic, const char* payload, uint8_t source)
{
    char buffer[500];
    sprintf(buffer, "T: %s | PL: %s", topic, payload);
    LOG_VERBOSE(TAG_CUSTOM, buffer);
}

void custom_state_subtopic(const char* subtopic, const char* payload)
{
    char buffer[500];
    sprintf(buffer, "ST: %s | PL: %s", subtopic, payload);
    LOG_VERBOSE(TAG_CUSTOM, buffer);
    // if(strcmp("p1b11", subtopic)) return;
    // StaticJsonDocument<128> json;
    // DeserializationError jsonError = deserializeJson(json, payload);

    // if(jsonError) { // Couldn't parse incoming JSON command
    //     dispatch_json_error(TAG_MSGR, jsonError);
    // } else {
    //     JsonVariant event = json[F("event")];
    //     JsonVariant value = json[F("text")];

    //     if(!strcmp("changed", event.as<const char*>())) {
    //         LOG_VERBOSE(TAG_CUSTOM, "CHANGING");
    //         if(!strcmp("p1b11", subtopic)) {
    //             alarmHour = value.as<String>();
    //         }
    //         if(!strcmp("p1b2", subtopic)) {
    //             alarmMinute = value.as<String>();
    //         }
    //     }
    // }
}

void custom_alarm_set()
{
    char buf[27];
    sprintf(buf, "0 %s %s * * %s", alarmMinute, alarmHour, "*");
    Cron.create(buf, crontest, false);
    LOG_VERBOSE(TAG_CUSTOM, buf);
    // jobs. = cron_job_create("* * * * * *", test_cron_job_sample_callback, (void*)0);
}

#endif // HASP_USE_CUSTOM