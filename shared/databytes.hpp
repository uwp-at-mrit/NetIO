#pragma once

#define READ_BYTES(mbin, dest, count) \
    do { \
        for (uint16 i = 0; i < count; i++) { \
            dest[i] = mbin->ReadByte(); \
        } \
    } while(0)

#define READ_WORDS(mbin, dest, count) \
    do { \
        for (uint16 i = 0; i < count; i++) { \
            dest[i] = mbin->ReadUInt16(); \
        } \
    } while(0)

#define WRITE_BYTES(mbout, src, count) \
    do { \
        for (uint16 i = 0; i < count; i++) { \
            mbout->WriteByte(src[i]); \
        } \
    } while(0)

#define WORD_HIGH_BYTE(data) (((data) >> 8) & 0xFF)
#define WORD_LOW_BYTE(data) ((data) & 0xFF)

#define GET_INT64_FROM_INT16(tab_int16, index) \
    (((int64_t)tab_int16[(index)    ] << 48) + \
     ((int64_t)tab_int16[(index) + 1] << 32) + \
     ((int64_t)tab_int16[(index) + 2] << 16) + \
      (int64_t)tab_int16[(index) + 3])

#define GET_INT32_FROM_INT16(tab_int16, index) ((tab_int16[(index)] << 16) + tab_int16[(index) + 1])
#define GET_INT16_FROM_INT8(tab_int8, index) ((tab_int8[(index)] << 8) + tab_int8[(index) + 1])

#define SET_INT16_TO_INT8(tab_int8, index, value) \
    do { \
        tab_int8[(index)] = (value) >> 8;  \
        tab_int8[(index) + 1] = (value) & 0xFF; \
    } while (0)

#define SET_INT32_TO_INT16(tab_int16, index, value) \
    do { \
        tab_int16[(index)    ] = (value) >> 16; \
        tab_int16[(index) + 1] = (value); \
    } while (0)

#define SET_INT64_TO_INT16(tab_int16, index, value) \
    do { \
        tab_int16[(index)    ] = (value) >> 48; \
        tab_int16[(index) + 1] = ((value) >> 32) & 0xFFFF; \
        tab_int16[(index) + 2] = ((value) >> 16) & 0xFFFF; \
        tab_int16[(index) + 3] = (value) & 0xFFFF; \
    } while (0)

unsigned int discard_dirty_bytes(Windows::Storage::Streams::DataReader^ din);

/*************************************************************************************************/
// These APIs come from libmodbus
int read_bits(uint8 *src, uint16 address, uint16 quantity, uint8 *dest);
int read_words(uint16 *src, uint16 address, uint16 quantity, uint8 *dest);
void write_words(uint16 *dest, uint16 address, uint16 quantity, uint8 *src);

void set_bits_from_byte(uint8 *dest, uint16 idx, uint8 src);
void set_bits_from_bytes(uint8 *dest, uint16 idx, uint16 count, const uint8 *src);
uint8 get_byte_from_bits(const uint8 *src, uint16 idx, uint16 count);

/*************************************************************************************************/
// These APIs are designed for MRIT
bool quantity_bit_ref(const uint8* src, size_t idx, uint8 bit_idx);
void quantity_bit_set(uint8* src, size_t idx, uint8 bit_idx);

uint8 bigendian_uint8_ref(const uint8* src, size_t idx);
void bigendian_uint8_set(uint8* dest, size_t idx, uint8 x);

uint16 bigendian_uint16_ref(const uint8* src, size_t idx);
void bigendian_uint16_set(uint8* dest, size_t idx, uint16 x);

uint32 bigendian_uint32_ref(const uint8* src, size_t idx);
void bigendian_uint32_set(uint8* dest, size_t idx, uint32 x);

uint64 bigendian_uint64_ref(const uint8* src, size_t idx);
void bigendian_uint64_set(uint8* dest, size_t idx, uint64 x);

float bigendian_float_ref(const uint8* src, size_t idx);
void bigendian_float_set(uint8* dest, size_t idx, float x);

float bigendian_flword_ref(const uint8* src, size_t idx, float scale = 1.0F);
void bigendian_flword_set(uint8* dest, size_t idx, float x, float scale = 1.0F);

void read_bigendian_floats(uint8* src, size_t address, size_t quantity, float* dest);
