#pragma once

#include "gps/gps_enums.hpp"

namespace WarGrey::SCADA {
#define MESSAGE_TYPE(ch1, ch2, ch3) ((ch1 << 16) | (ch2 << 8) | ch3)

	unsigned int message_type(const unsigned char* pool, size_t index);

	bool scan_boolean(const unsigned char* pool, size_t* idx, size_t endp1, unsigned char T, unsigned char F);
	unsigned long long scan_natural(const unsigned char* pool, size_t* idx, size_t endp1);
	double scan_scalar(const unsigned char* pool, size_t* idx, size_t endp1);
	double scan_vector(const unsigned char* pool, size_t* idx, size_t endp1, unsigned char unit);
	double scan_vector(const unsigned char* pool, size_t* idx, size_t endp1, unsigned char positive_dir, unsigned char negative_dir);

	NMEA_PSMI scan_positioning_system_mode_indicator(const unsigned char* pool, size_t* idx, size_t endp1);
	NMEA_GQI scan_gps_quality_indicator(const unsigned char* pool, size_t* idx, size_t endp1);
}
