#include "timezone.h"

static int32_t float_to_int(float input, float scale, uint8_t precision) {
  const float inputScaled = input / scale;
  return (int32_t)(inputScaled * (float)(1 << (precision - 1)));
}

static float int_to_float(int32_t input, float scale, uint8_t precision) {
  const float value = (float)input / (float)(1 << (precision - 1));
  return value * scale;
}



static int32_t next_value(File *file, u_int32_t *position) {
  int32_t value;

  char buf5[sizeof(uint8_t)];
  size_t bytesRead = file->readBytes(buf5, sizeof(buf5));
  uint8_t *marker = (uint8_t *)buf5;



  if (*marker != 0x80 && *marker != 0x7F) {  // Not the marker. Read again as 8 Bit value

    char buf6[sizeof(uint8_t)];
    bytesRead = file->readBytes(buf6, sizeof(buf6));
    uint8_t *bufvalue = (uint8_t *)buf6;
    *position += sizeof(uint8_t);
    file->seek(*position);



    //ESP_LOGI(TAG, "value 8 Bit: %x", value);
    value = (int8_t)*bufvalue;
    //ESP_LOGI(TAG, "value 8 Bit (int8_t): %d", value);

  } else if (*marker == 0x80) {  // marker for 16 bit value

    *position += sizeof(int8_t);  // Move to next position

    char buf7[sizeof(int16_t)];
    bytesRead = file->readBytes(buf7, sizeof(buf7));
    int16_t *bufvalue = (int16_t *)buf7;
    *position += sizeof(int16_t);
    file->seek(*position);


    value = (int16_t) *bufvalue;


  } else if (*marker == 0x7F) {  // marker for 32 Bit value

    *position += sizeof(int8_t);  // Move to next position

    char buf8[sizeof(int32_t)];
    bytesRead = file->readBytes(buf8, sizeof(buf8));
    int32_t *bufvalue = (int32_t *)buf8;
    *position += sizeof(int32_t);
    file->seek(*position);

    value = (int32_t) *bufvalue;

  } else {
    Serial.println("Error");
  }

  return value;
}


static bool check_inside_shape(File *file, u_int32_t shape_position, int32_t latitude_int, int32_t longitude_int) {

  file->seek(shape_position);

  char buf4[sizeof(tz_database_point_t)];
  size_t bytesRead = file->readBytes(buf4, sizeof(buf4));
  tz_database_point_t *start_point = (tz_database_point_t *)buf4;
  shape_position += sizeof(tz_database_point_t);
  file->seek(shape_position);


  char buff[sizeof(u_int32_t)];
  bytesRead = file->readBytes(buff, sizeof(buff));
  u_int32_t *deltas = (u_int32_t *)buff;
  shape_position += sizeof(u_int32_t);
  file->seek(shape_position);


  //ESP_LOGI(TAG, "deltas: %d", deltas);

  tz_database_point_t p1;
  tz_database_point_t p2;

  p1.latitude = start_point->latitude;
  p1.longitude = start_point->longitude;

  p2.latitude = start_point->latitude;
  p2.longitude = start_point->longitude;

  double x_inters;
  bool odd = false;

  for (int k = 0; k < *deltas; k++) {
    p2.latitude += next_value(file, &shape_position);
    p2.longitude += next_value(file, &shape_position);
    //ESP_LOGI(TAG, "                                  k P2: %d %f %f", k, int_to_float(p2.latitude, 90, 24), int_to_float(p2.longitude, 180, 24));
    //ESP_LOGI(TAG, "P1: %d %d   P2: %d %d", p1.latitude, p1.longitude, p2.latitude, p2.longitude);

    // y muss zwischen min und max der Linie sein
    if (latitude_int > min(p1.latitude, p2.latitude) && latitude_int <= max(p1.latitude, p2.latitude)) {
      // x muss kleiner gleich dem großeren x Wert der Linie sein
      if (longitude_int <= max(p1.longitude, p2.longitude)) {
        // Horizontale Linie wird ignoriert da schon beim Endpunkt einer anderen Linie gezählt
        if (p1.latitude != p2.latitude) {
          // Geradengleichung nach x aufgelöst  https://www.mathematik-oberstufe.de/analysis/lin/gerade2d-2punkte.html
          x_inters = (latitude_int - p1.latitude) * (p2.longitude - p1.longitude) / (p2.latitude - p1.latitude) + p1.longitude;
          if (longitude_int <= x_inters) {
            odd = !odd;
            Serial.printf("Crossed line P1: %f %f   P2: %f %f\n", int_to_float(p1.latitude, 90, 24), int_to_float(p1.longitude, 180, 24), int_to_float(p2.latitude, 90, 24), int_to_float(p2.longitude, 180, 24));
          }
        }
      }
    }
    p1 = p2;
  }

  return odd;
}

static bool check_inside_country(File *file, u_int32_t data_position, int32_t latitude_int, int32_t longitude_int) {

  
  file->seek(data_position);
  char buf1[sizeof(tz_database_boundingbox_t)];
  size_t bytesRead = file->readBytes(buf1, sizeof(buf1));
  tz_database_boundingbox_t *boundingbox = (tz_database_boundingbox_t *)buf1;
  // Serial.printf("chck cntry: %s", buf1);
  data_position += sizeof(tz_database_boundingbox_t);
  file->seek(data_position);
  //ESP_LOGI(TAG, "from_latitude: %d", boundingbox.from_latitude);
  //ESP_LOGI(TAG, "from_longitude: %d", boundingbox.from_longitude);
  //ESP_LOGI(TAG, "to_latitude: %d", boundingbox.to_latitude);
  //ESP_LOGI(TAG, "to_longitude: %d", boundingbox.to_longitude);

  if (latitude_int < boundingbox->from_latitude || latitude_int > boundingbox->to_latitude || longitude_int < boundingbox->from_longitude || longitude_int > boundingbox->to_longitude) {

    return false;
  } else {
    char buf2[sizeof(u_int32_t)];
    bytesRead = file->readBytes(buf2, sizeof(buf2));
    u_int32_t *shapes = (u_int32_t *)buf2;
    data_position += sizeof(u_int32_t);
    file->seek(data_position);

    Serial.println("Inside Bounding Box");
    return true;

    tz_database_shape_t *shape;
    for (int j = 0; j < *shapes; j++) {
      char buf3[sizeof(tz_database_shape_t)];
      size_t bytesRead = file->readBytes(buf3, sizeof(buf3));
      shape = (tz_database_shape_t *)buf3;
      data_position += sizeof(tz_database_shape_t);
      file->seek(data_position);

      u_int32_t shape_position = shape->position;

      bool is_inside = check_inside_shape(file, shape_position, latitude_int, longitude_int);

      if (is_inside) {
        return true;
      }
    }
    return false;
  }
}




String timezone_find(float latitude, float longitude, String db) {
  
  int pos = 0;


  File file = SD.open(db, FILE_READ);

  // char *buf = NULL;
  // buf = (char *)malloc (200);
  // memset(buf, 0, 32);


  char buf[sizeof(tz_database_header_t)];
  size_t bytesRead = file.readBytes(buf, sizeof(buf));

  tz_database_header_t *header = (tz_database_header_t *)buf;

  Serial.printf("Timezone Database info: Version: %d   Signature: %.4s   Precision: %d   Creation Date %.10s\n", header->version, header->signature, header->precision, header->creation_date);

  pos += sizeof(tz_database_header_t);

  file.seek(pos);

  int32_t latitude_int = float_to_int(latitude, 90, header->precision);
  int32_t longitude_int = float_to_int(longitude, 180, header->precision);

  char bufa[sizeof(u_int32_t)];
  bytesRead = file.readBytes(bufa, sizeof(bufa));


  u_int32_t *entries = (u_int32_t *)bufa;
  Serial.printf("Entries in TOC: %d\n", *entries);

  pos += sizeof(u_int32_t);
  file.seek(pos);


  for (int i = 0; i < *entries; i++) {
    char bufb[sizeof(tz_database_entry_t)];
    bytesRead = file.readBytes(bufb, sizeof(bufb));
    // Serial.printf("Byres read: %s\n", bufb);

    tz_database_entry_t *entry = (tz_database_entry_t *)bufb;
    pos += sizeof(tz_database_entry_t);
    file.seek(pos);
    // Serial.printf("POS: %i\n", pos);
    // Serial.printf("Name: %.64s\n", entry->tz_name);
    // Serial.printf("Value: %.64s\n", entry->tz_value);
    // Serial.printf("Postion: ")

    u_int32_t data_position = entry->position;
    // Serial.printf("datapos: %i\n", data_position);
    bool is_inside = check_inside_country(&file, data_position, latitude_int, longitude_int);
    file.seek(pos);

    if (is_inside) {
      Serial.printf("Inside timezone: %.64s\n", entry->tz_name);
      return (char *)&entry->tz_value;
    }
    // return;
  }
}