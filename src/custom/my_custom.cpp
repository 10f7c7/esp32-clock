/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

// USAGE: - Copy this file and rename it to my_custom.cpp
//        - Change false to true on line 9

#include "hasplib.h"

#if defined(HASP_USE_CUSTOM) && HASP_USE_CUSTOM > 0 && true // <-- set this to true in your code

#include "my_custom.h"
#include "hasp_debug.h"
// #include "zonedetect/zonedetect.h"
// #include "SPI.h"
// #include "SD.h"
#define HASP_ALARM_LOG_CMD                                                                                             \
    "p1b11.val\np1b12.val\np1b21.val\np1b22.val\np1b23.val\np1b24.val\np1b25.val\np1b26.val\np1b27.val\n"
#define HASP_ALARM_SET_CMD                                                                                             \
    "run /alarm_log.cmd\nalarm_set\njsonl {\"page\":0,\"id\":239,\"obj\":\"msgbox\",\"text\":\"Alarm "                 \
    "Set\",\"auto_close\":2000}"
#define HASP_BOOT_CMD "config/hasp {'theme':5}"

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
float latitude;
float longitude;

#define SD_MISO 13
#define SD_SCLK 12
#define SD_MOSI 11
#define SD_CS 10

bool clockTaskFinished = false;

// cron_job* jobs[];
std::map<CronId, cust_cron_expr> alarms;
std::vector<String> alarmBtns  = {"p1b11", "p1b12", "p1b21", "p1b22", "p1b23", "p1b24", "p1b25", "p1b26", "p1b27"};
uint8_t alarmHour              = 0;
uint8_t alarmMinute            = 0;
bool alarmDays[7]               = {};
std::vector<String> daysString = {"Sun ", "Mon ", "Tue ", "Wed ", "Thu ", "Fri ", "Sat "};

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

time_t GPSTime()
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
        if(gps.location.isUpdated()) {
            latitude  = gps.location.lat();
            longitude = gps.location.lng();
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
            Serial.print("\n");
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

void custom_clock_loop(void*)
{
    int i = 0;

    while(1) {
        time_t rawtime;
        time(&rawtime);

        struct tm* timeinfo;
        // setenv("TZ", "EST+5EDT,M3.2.0/2,M11.1.0/2", 1);
        // tzset();
        timeinfo = localtime(&rawtime);
        // for(int i = 0; i < 4; i++) {
        // if(!(millis() % 100)) {
        //     digitalWrite(SEC, timeinfo->tm_sec % 2);
        // }
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
        delay(2);
        // Cron.delay(1);
        // }
    }
}

void listDir(fs::FS& fs, const char* dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file) {
        if(file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

// void onError(int errZD, int errNative)
// {
//     fprintf(stderr, "ZD error: %s (0x%08X)\n", ZDGetErrorString(errZD), (unsigned)errNative);
// }

static void filesystem_write_file(const char* filename, const char* data, size_t len)
{
    if(HASP_FS.exists(filename)) return;

    LOG_TRACE(TAG_CUSTOM, F(D_FILE_SAVING), filename);
    File file = HASP_FS.open(filename, "w");

    if(file) {
        file.write((const uint8_t*)data, len);
        file.close();
        LOG_INFO(TAG_CUSTOM, F(D_FILE_SAVED), filename);
    } else {
        LOG_ERROR(TAG_CUSTOM, D_FILE_SAVE_FAILED, filename);
    }
}

void custom_setup()
{
    // Initialization code here
    randomSeed(millis());

    SPIClass spi = SPIClass(HSPI);
    spi.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);

    // SPI.begin()

    if(!SD.begin(SD_CS, spi)) {
        Serial.println("Card Mount Failed");
        // return;
    }

    gpsSerial.begin(GPS_BAUD, EspSoftwareSerial::SWSERIAL_8N1, GPS_RX, -1, false);

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

    time_t epoch_time     = GPSTime();
    struct timeval nowTmp = {.tv_sec = epoch_time};
    settimeofday(&nowTmp, NULL);

    alarmDays[2] = 1;
    custom_alarm_set();
    alarmDays[5] = 1;
    custom_alarm_set();

    // File alarmsFile = HASP_FS.open("/alarms.bin", "wr");
    File alarmsFile = SD.open("/alarms.bin", FILE_WRITE);
    LOG_TRACE(TAG_CUSTOM, F(D_FILE_SAVING), "/alarms.bin");

    // alarmsFile.seek(0);
    const uint8_t alarmSize = alarms.size();
    alarmsFile.write(&alarmSize, sizeof(uint8_t));

    alarmsFile.seek(sizeof(uint8_t));

    for(auto it = alarms.begin(); it != alarms.end(); ++it) {
        alarmsFile.write(reinterpret_cast<unsigned char*>(&it->second), sizeof(cust_cron_expr));
        alarmsFile.seek(sizeof(cust_cron_expr), SeekCur);
    }

    alarmsFile.close();

    File alarmsFile2 = SD.open("/alarms.bin");

    alarmsFile2.seek(0);

    char buf[20];
    size_t bytesRead   = alarmsFile2.readBytes(buf, sizeof(buf));
    uint8_t* readCount = (uint8_t*)buf;

    char realcount[20];
    sprintf(realcount, "ALARM COUNT: %i\n", *readCount);
    LOG_VERBOSE(TAG_CUSTOM, realcount);

    alarmsFile2.close();

    if(latitude && longitude) {
        LOG_INFO(TAG_CUSTOM, "using gps coords");
        setenv("TZ", timezone_find(latitude, longitude).c_str(), 1);
    } else {
        setenv("TZ", "EST+5EDT,M3.2.0/2,M11.1.0/2", 1);
    }
    tzset();

    // setenv("TZ", "America/New_York", 1);
    // tzset();

    // timeval epoch      = {epoch_time, 0};
    // const timeval* tv  = &epoch;
    // timezone utc       = {0, 0};
    // const timezone* tz = &utc;
    // settimeofday(tv, tz);

    // SET UP COMMANDS
    filesystem_write_file("/alarm_log.cmd", HASP_ALARM_LOG_CMD, strlen(HASP_ALARM_LOG_CMD));
    filesystem_write_file("/alarm_set.cmd", HASP_ALARM_SET_CMD, strlen(HASP_ALARM_SET_CMD));
    filesystem_write_file("/boot.cmd", HASP_BOOT_CMD, strlen(HASP_BOOT_CMD));

    xTaskCreatePinnedToCore(custom_clock_loop, "clockFace", 6000, NULL, 0, NULL, 1);

    // SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    // SD.begin();
    // listDir(SD, "/", 0);

    // ZDSetErrorHandler(onError);

    // ZoneDetect* const cd = ZDOpenDatabase("/timezone21.bin");

    // // File file = HASP_FS.open("/timezone21.bin", "r");

    // // Serial.print(file.size());
    // // Serial.print("\n");

    // // ZoneDetect* const cd = ZDOpenDatabaseFromMemory(file.read(), file.size());
    // if(!cd) {
    //     Serial.print("awwman");
    // }

    // const float lat = 39.22643550;
    // const float lon = -76.94594725;

    // float safezone = 0;
    // ZoneDetectResult *results = ZDLookup(cd, lat, lon, &safezone);
    // // printResults(cd, results, safezone);

    // Serial.printf("The simple string is [%s]\n", ZDHelperSimpleLookupString(cd, lat, lon));

    // ZDCloseDatabase(cd);
}

// int i = 0;

void custom_loop()
{
    Cron.delay();
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
        case SD_MISO:
        case SD_SCLK:
        case SD_MOSI:
        case SD_CS:
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

    if(std::find(alarmBtns.begin(), alarmBtns.end(), subtopic) != alarmBtns.end()) {
        StaticJsonDocument<128> json;
        DeserializationError jsonError = deserializeJson(json, payload);
        if(jsonError) { // Couldn't parse incoming JSON command
            dispatch_json_error(TAG_MSGR, jsonError);
        } else {
            // JsonVariant value = json[F("val")];
            const char* val = ("val");
            if(!json[val].isNull()) {
                int btn = std::stoi(subtopic + 3);
                if(btn == 11) {
                    alarmHour = json[val].as<uint8_t>();
                    // Serial.printf("ihateterm: %i\n", tmp);
                    // strncpy(alarmHour, tmp, sizeof(alarmHour) - 1);
                } else if(btn == 12) {
                    alarmMinute = json[val].as<uint8_t>();
                    // strncpy(alarmMinute, tmp, sizeof(alarmMinute) - 1);
                } else if((btn > 20) && (btn < 28)) {
                    alarmDays[btn - 21] = json[val].as<bool>();
                }
            }
        }
    } else {
        return;
    }

    // if(strcmp("p1b11", subtopic) && strcmp("p1b12", subtopic)) return;
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
    //         if(!strcmp("p1b12", subtopic)) {
    //             alarmMinute = value.as<String>();
    //         }
    //     }
    // }
}

void custom_alarm_set()
{
    int days            = 0;
    String alarmDaysStr = "";
    for(int i = 0; i < 7; i++) {
        if(alarmDays[i]) {
            alarmDaysStr += String(i);
            alarmDaysStr += ",";
            days++;
        }
    }
    if(days) {
        alarmDaysStr.remove(alarmDaysStr.length() - 1);
    } else {
        alarmDaysStr = "*";
    }
    cust_cron_expr expr;
    expr.minutes = alarmMinute;
    // strncpy(expr.minutes, alarmMinute, sizeof(expr.minutes) - 1);
    expr.hours = alarmHour;
    // strncpy(expr.hours, alarmHour, sizeof(expr.hours) - 1);
    memcpy(expr.days, alarmDays, sizeof(alarmDays));
    char buf[27];
    sprintf(buf, "0 %i %i * * %s", alarmMinute, alarmHour, alarmDaysStr);
    CronId id = Cron.create(buf, crontest, false);
    LOG_VERBOSE(TAG_CUSTOM, buf);
    alarms.insert(std::make_pair(id, expr));

    // {"page":2,"id":11,"parentid":10,"obj":"obj","x":5,"y":5,"w":220,"h":56,"click":0,"bg_color":"Gray","bg_grad_dir":0,"border_side":0}
    // {"page":2,"id":110,"parentid":11,"obj":"label","x":10,"y":0,"w":150,"h":50,"text":"06:00","text_font":32}
    // {"page":2,"id":111,"parentid":11,"obj":"label","x":10,"y":32,"w":150,"h":50,"text":"Sun Mon Fri"}
    // {"page":2,"id":112,"parentid":11,"obj":"switch","x":130,"y":13,"w":70,"h":30}

    int boxId = 11 + id;

    String alarmDaysStrAbbr = "";

    for(int i = 0; i < 7; i++) {
        if(alarmDays[i]) {
            alarmDaysStrAbbr += daysString[i];
        }
    }
    if(!days) {
        alarmDaysStrAbbr = "every day";
    }

    char box[200];
    sprintf(box,
            "{\"page\":2,\"id\":%i,\"parentid\":10,\"obj\":\"obj\",\"x\":5,\"y\":%i,\"w\":220,\"h\":56,\"click\":0,"
            "\"bg_color\":\"Gray\",\"bg_grad_dir\":0,\"border_side\":0}",
            boxId, (5 + (60 * id)));

    // if(alarmHour.length() < 2) alarmHour = "0" + alarmHour;
    // if(alarmMinute.length() < 2) alarmMinute = "0" + alarmMinute;

    char time[200];
    sprintf(time,
            "{\"page\":2,\"id\":%i0,\"parentid\":%i,\"obj\":\"label\",\"x\":10,\"y\":0,\"w\":150,\"h\":50,\"text\":\"%0*d:%0*d\",\"text_font\":32}",
            boxId, boxId,2, alarmHour,2, alarmMinute);

    char day[200];
    sprintf(day,
            "{\"page\":2,\"id\":%i1,\"parentid\":%i,\"obj\":\"label\",\"x\":10,\"y\":32,\"w\":150,\"h\":50,\"text\":\"%"
            "s\"}",
            boxId, boxId, alarmDaysStrAbbr);

    char en[200];
    sprintf(en,
            "{\"page\":2,\"id\":%i2,\"parentid\":%i,\"obj\":\"switch\",\"x\":130,\"y\":13,\"w\":70,\"h\":30,\"val\":1}",
            boxId, boxId);

    uint8_t pagenum = haspPages.get();

    StaticJsonDocument<200> boxJson;
    DeserializationError boxError = deserializeJson(boxJson, box);
    if(boxError == DeserializationError::Ok) {
        hasp_new_object(boxJson.as<JsonObject>(), pagenum);
    } else {
        LOG_ERROR(TAG_CUSTOM, "failed to create list item");
    }
    StaticJsonDocument<200> timeJson;
    DeserializationError timeError = deserializeJson(timeJson, time);
    if(timeError == DeserializationError::Ok) {
        hasp_new_object(timeJson.as<JsonObject>(), pagenum);
    } else {
        LOG_ERROR(TAG_CUSTOM, time);
        LOG_ERROR(TAG_CUSTOM, "failed to create list item");
    }
    StaticJsonDocument<200> dayJson;
    DeserializationError dayError = deserializeJson(dayJson, day);
    if(dayError == DeserializationError::Ok) {
        hasp_new_object(dayJson.as<JsonObject>(), pagenum);
    } else {
        LOG_ERROR(TAG_CUSTOM, "failed to create list item");
    }
    StaticJsonDocument<200> enJson;
    DeserializationError enError = deserializeJson(enJson, en);
    if(enError == DeserializationError::Ok) {
        hasp_new_object(enJson.as<JsonObject>(), pagenum);
    } else {
        LOG_ERROR(TAG_CUSTOM, "failed to create list item");
    }
}

#endif // HASP_USE_CUSTOM