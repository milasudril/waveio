#ifdef __WAND__
target[name[riffwave_writer.o] type[object]]
#endif

#include "riffwave_writer.h"
#include "riffwave_fileheaders.h"
#include "streaminfo.h"
#include <herbs/streamout/streamout.h>
#include <herbs/exceptionmissing/exceptionmissing.h>

#include <algorithm>

WaveIO::RiffwaveWriter::RiffwaveWriter(Herbs::StreamOut& dest):
	length_written(0),m_dest(dest)
	{
	RiffwaveHeader header;
	header.id_riff=RIFFID_ENDIAN_LITTLE;
	header.size=0xffffffff;
	header.id_wave=WAVEID_ENDIAN_LITTLE;

	m_dest.write(&header,sizeof(header));
	}

void WaveIO::RiffwaveWriter::infoWrite(const Streaminfo& info)
	{
	RiffwaveFormat format;
	format.tag=RIFFWAVE_FORMAT_EXTENSIBLE;
	format.channel_count=info.channel_count;
	format.sample_rate=info.sample_rate;
	format.data_rate=format.sample_rate*format.channel_count*sizeof(sample_t);
	format.size_block=format.channel_count*sizeof(sample_t);
	format.bit_depth=8*sizeof(sample_t);
	format.size_extension=22;
	format.bits_valid=format.bit_depth;
	format.channel_mask=info.channel_mask;
	format.format_guid=RIFFWAVE_GUID_IEEEFLOAT;
	channel_count=info.channel_count;

	if(!chunkIDWrite("fmt "))
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
	if(!chunkSizeWrite(sizeof(format)))
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
	if(dataWrite(&format,sizeof(format))!=sizeof(format))
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}

	RiffwaveFact fact;
	fact.sample_length=0xffffffff;
	if(!chunkIDWrite("fact"))
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
	if(!chunkSizeWrite(sizeof(fact)))
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
	if(dataWrite(&fact,sizeof(fact))!=sizeof(fact))
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}

	chunkIDWrite("data");
	chunkSizeWrite(0xffffffff);
	}

size_t WaveIO::RiffwaveWriter::write(const sample_t* samples,size_t length)
	{
	auto ret=dataWrite(samples,length*sizeof(sample_t));
	length_written+=ret;
	return ret;
	}

WaveIO::RiffwaveWriter::~RiffwaveWriter()
	{
	uintmax_t size_headers=sizeof(RiffwaveHeader)
		+8+sizeof(RiffwaveFormat)
		+8+sizeof(RiffwaveFact)
		+4;

	if(length_written + size_headers + 4- 8 < 0xffffffff )
		{
		uintmax_t offset=size_headers;

		if(m_dest.seek(offset))
			{
			uint32_t l=length_written;
			m_dest.write(&l,sizeof(l));
			}
		else
			{return;}

		offset=sizeof(RiffwaveHeader)
			+8+sizeof(RiffwaveFormat)
			+8;
		m_dest.seek(offset);
		uint32_t sample_length=length_written/(channel_count*sizeof(sample_t));
		m_dest.write(&sample_length,sizeof(sample_length));

		offset=4;
		m_dest.seek(offset);
		uint32_t size_tot=length_written + size_headers + 4- 8;
 		m_dest.write(&size_tot,sizeof(size_tot));
		}
	}




bool WaveIO::RiffwaveWriter::chunkIDWrite(const char* header_name)
	{
	char buffer_out[4]={0,0,0,0};
	auto ptr_out=buffer_out;
	while(*header_name && (ptr_out-buffer_out)!=4)
		{
		*ptr_out=*header_name;
		++ptr_out;
		++header_name;
		}
	return m_dest.write(buffer_out,4)==4;
	}

bool WaveIO::RiffwaveWriter::chunkSizeWrite(uint32_t size)
	{
	header_size=size;
	return m_dest.write(&header_size,sizeof(header_size));
	}

size_t WaveIO::RiffwaveWriter::dataWrite(const void* buffer,size_t buffer_size)
	{
	auto n_bytes_write=buffer_size;
	if(header_size!=0xffffffff)
		{
		n_bytes_write=std::min(header_size,uint32_t(buffer_size));
		auto res=m_dest.write(buffer,n_bytes_write);
		header_size-=res;
		return res;
		}
	return m_dest.write(buffer,n_bytes_write);
	}
