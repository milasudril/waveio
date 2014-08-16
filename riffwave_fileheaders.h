#ifdef __WAND__
target[name[riffwave_fileheaders.h] type[include]]
#endif

#ifndef WAVEIO_RIFFWAVE_FILEHEADERS_H
#define WAVEIO_RIFFWAVE_FILEHEADERS_H

#include <herbs/endian/endian.h>

namespace WaveIO
	{
	struct RiffwaveHeader
		{
		uint32_t id_riff;
		uint32_t size;
		uint32_t id_wave;
		};

	static constexpr uint32_t RIFFID_ENDIAN_BIG=0x52494646;
	static constexpr uint32_t RIFFID_ENDIAN_LITTLE=0x46464952;
	static constexpr uint32_t WAVEID_ENDIAN_BIG=0x57415645;
	static constexpr uint32_t WAVEID_ENDIAN_LITTLE=0x45564157;

	struct RiffwaveGUID
		{
		uint32_t data_0;
		uint16_t data_1;
		uint16_t data_2;
		uint8_t data_4[8];
		};

	inline RiffwaveGUID bswap(RiffwaveGUID guid)
		{
		guid.data_0=Herbs::bswap(guid.data_0);
		guid.data_1=Herbs::bswap(guid.data_1);
		guid.data_2=Herbs::bswap(guid.data_2);
		return guid;
		}

	struct RiffwaveFormat
		{
		uint16_t tag;            uint16_t channel_count; uint32_t sample_rate;
		uint32_t data_rate;      uint16_t size_block;    uint16_t bit_depth;
		uint16_t size_extension; uint16_t bits_valid;    uint32_t channel_mask;
		RiffwaveGUID format_guid;
		};

	struct RiffwaveFact
		{
		uint32_t sample_length;
		};

	static constexpr uint16_t RIFFWAVE_FORMAT_PCM=0x1;
	static constexpr uint16_t RIFFWAVE_FORMAT_IEEEFLOAT=0x3;
	static constexpr uint16_t RIFFWAVE_FORMAT_EXTENSIBLE=0xFFFE;

	static constexpr RiffwaveGUID RIFFWAVE_GUID_IEEEFLOAT
		{
		RIFFWAVE_FORMAT_IEEEFLOAT,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}
		};

	static constexpr RiffwaveGUID RIFFWAVE_GUID_PCM
		{
		RIFFWAVE_FORMAT_PCM,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}
		};
	}

#endif
