#include "FS.h"
#include "SD.h"
#include "SPI.h"

/**
 * @brief TZ Database header
 *
 */
typedef struct {
  uint8_t version;
  char signature[4];
  uint8_t precision;
  char creation_date[10];
  char filler[16];
} tz_database_header_t;

/**
 * @brief TZ Database entry
 *
 */
typedef struct {
  char tz_name[64];
  char tz_value[64];
  uint32_t position;
} tz_database_entry_t;

/**
 * @brief TZ Database bounding box
 *
 */
typedef struct {
  int32_t from_latitude;
  int32_t from_longitude;
  int32_t to_latitude;
  int32_t to_longitude;
} tz_database_boundingbox_t;

/**
 * @brief TZ Shape entry
 *
 */
typedef struct {
  uint32_t position;
} tz_database_shape_t;

/**
 * @brief TZ Database point
 *
 */
typedef struct {
  int32_t latitude;
  int32_t longitude;
} tz_database_point_t;


String timezone_find(float latitude = 39.22646073, float longitude = -76.94586193, String db = "/timezones.bin");