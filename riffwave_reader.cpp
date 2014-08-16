#ifdef __WAND__
target[name[riffwave_reader.o] type[object]]
#endif

#include "riffwave_reader.h"
#include "riffwave_fileheaders.h"
#include "streaminfo.h"
#include <herbs/exceptionmissing/exceptionmissing.h>
#include <herbs/streamin/streamin.h>

#include <cstring>

#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif

#ifdef FLOAT
#undef FLOAT
#endif

namespace
	{
	static constexpr uint32_t ENDIAN_BIG=0x1;
	static constexpr uint32_t FLOAT=0x2;

	template<class To,class From>
	To convert(From from);

	template<>
	float convert<float,double>(double x)
		{return x;}

	template<>
	float convert<float,uint8_t>(uint8_t x)
		{return 2.0f*(x/255.0f - 0.5f);}

	template<>
	float convert<float,int16_t>(int16_t x)
		{return (x + 0.5f)/32767.5f;}

	template<>
	float convert<float,int32_t>(int32_t x)
		{return float( (x + 0.5)/( 0x7fffffff + 0.5));}

	template<class From,class To>
	void convert_impl(const From* buffer_src,To* buffer_dest,size_t n)
		{
		while(n)
			{
			*buffer_dest=convert<To>(*buffer_src);
			++buffer_dest;
			++buffer_src;
			--n;
			}
		}

	template<class T>
	void convert_impl(const T* buffer_src,T* buffer_dest,size_t n)
		{
		memcpy(buffer_dest,buffer_src,n*sizeof(T));
		}

	template<class From,class To>
	void convert(const void* buffer_src,void* buffer_dest,size_t n)
		{
		convert_impl((From*)buffer_src,(To*)buffer_dest,n);
		}
	}



WaveIO::RiffwaveReader::RiffwaveReader(Herbs::StreamIn& source):m_source(source)
	{
	RiffwaveHeader header;
	m_buffer_size=4096;
	m_buffer.reset(new uint8_t[m_buffer_size]);
	m_source.modeBufferedOn();
	auto offset_0=m_source.offsetGet();
	m_source.read(&header,sizeof(header));
	flags=0;

	if(header.id_riff==RIFFID_ENDIAN_LITTLE && header.id_wave==WAVEID_ENDIAN_LITTLE)
		{
	//	Nothing to do here
		}
	else
	if(header.id_riff==RIFFID_ENDIAN_BIG && header.id_wave==WAVEID_ENDIAN_BIG)
		{flags|=ENDIAN_BIG;}
	else
		{
		m_source.offsetRestore(offset_0);
		m_source.modeBufferedOff();
		throw Herbs::ExceptionMissing(___FILE__,__LINE__);
		}

	if(header.size==0xffffffff)
		{
		if(m_source.lengthKnown())
			{size_tot=source.lengthGet()-8;}
		else
			{
			m_source.offsetRestore(offset_0);
			m_source.modeBufferedOff();
			throw Herbs::ExceptionMissing(___FILE__,__LINE__);
			}
		}
	else
		{
		if(flags&ENDIAN_BIG)
			{size_tot=Herbs::bswap(header.size);}
		else
			{size_tot=header.size;}
		}
	m_source.modeBufferedOff();
	size_tot-=4;
	}

bool WaveIO::RiffwaveReader::headerRead(Herbs::Stringbase<char>& name)
	{
	name.clear();
	m_source.modeBufferedOn();
	size_t k=4;
	while(k!=0 && !m_source.eof())
		{
		name.append(m_source.byteGet());
		--k;
		}
	name.terminate();
	if(m_source.eof())
		{
		m_source.modeBufferedOff();
		return 0;
		}

	uint32_t size;
	if(m_source.read(&size,sizeof(size))!=sizeof(size))
		{
		m_source.modeBufferedOff();
		return 0;
		}
	size_tot-=8;
	if(size==0xffffffff)
		{current_chunk_size=size_tot;}
	else
		{
		if(flags&ENDIAN_BIG)
			{current_chunk_size=Herbs::bswap(size);}
		else
			{current_chunk_size=size;}
		}

	m_source.modeBufferedOff();
	return 1;
	}


void WaveIO::RiffwaveReader::infoRead(Streaminfo& info)
	{
	Herbs::Stringbase<char> header_name(4);
	while(headerRead(header_name))
		{
		if(header_name=="data")
			{
			length=8LL*uintmax_t(current_chunk_size)/uintmax_t(bit_depth);
			return;
			}

		if(header_name=="fmt ")
			{
			RiffwaveFormat format;
			memset(&format,0,sizeof(format));
			dataRead(&format,sizeof(format));

			if(flags&ENDIAN_BIG)
				{
				format.tag=Herbs::bswap(format.tag);
				format.channel_count=Herbs::bswap(format.channel_count);
				format.sample_rate=Herbs::bswap(format.sample_rate);
				format.bit_depth=Herbs::bswap(format.bit_depth);
				format.size_extension=Herbs::bswap(format.size_extension);
				format.channel_mask=Herbs::bswap(format.channel_count);
				format.format_guid=bswap(format.format_guid);
				}

			bit_depth=format.bit_depth;

			switch(format.tag)
				{
				case RIFFWAVE_FORMAT_PCM:
					break;

				case RIFFWAVE_FORMAT_IEEEFLOAT:
					flags|=FLOAT;
					break;

				case RIFFWAVE_FORMAT_EXTENSIBLE:
					if(format.size_extension!=22)
						{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}

					if(memcmp(&format.format_guid,&RIFFWAVE_GUID_IEEEFLOAT
						,sizeof(RiffwaveGUID))==0)
						{flags|=FLOAT;}
					else
					if(memcmp(&format.format_guid,&RIFFWAVE_GUID_PCM
						,sizeof(RiffwaveGUID))==0)
						{
					//	Nothing to do here
						}
					else
						{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
					break;

				default:
					throw Herbs::ExceptionMissing(___FILE__,__LINE__);
				}

			if(format.size_extension==0)
				{
				switch(format.channel_count)
					{
					case 1:
						format.channel_mask=SPEAKER_FRONT_CENTER;
						break;

					case 2:
						format.channel_mask=SPEAKER_STEREO_LEFT
							|SPEAKER_STEREO_RIGHT;
						break;

					case 3:
						format.channel_mask=SPEAKER_FRONT_LEFT
							|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER;
						break;

					case 4:
						format.channel_mask=SPEAKER_FRONT_LEFT
							|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER
							|SPEAKER_BACK_CENTER;
						break;

					case 5:
						format.channel_mask=SPEAKER_FRONT_LEFT
							|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER
							|SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT;
						break;

					case 6:
						format.channel_mask=SPEAKER_FRONT_LEFT
							|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER
							|SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT
							|SPEAKER_LOW_FREQUENCY;
						break;

					default:
						format.channel_mask=0xffffffff;
					}
				}

			info.sample_rate=format.sample_rate;
			info.channel_count=format.channel_count;
			info.channel_mask=format.channel_mask;

			if(flags&ENDIAN_BIG)
				{
			//	TODO
				throw Herbs::ExceptionMissing(___FILE__,__LINE__);
				}
			else
				{
				if(flags&FLOAT)
					{
					switch(bit_depth)
						{
						case 32:
							converter=convert<float,sample_t>;
							break;
						case 64:
							converter=convert<double,sample_t>;
							break;
						default:
							throw Herbs::ExceptionMissing(___FILE__,__LINE__);
						}
					}
				else
					{
					switch(bit_depth)
						{
						case 8:
							converter=convert<uint8_t,sample_t>;
							break;
						case 16:
							converter=convert<int16_t,sample_t>;
							break;
						case 32:
							converter=convert<int32_t,sample_t>;
							break;
						default:
							throw Herbs::ExceptionMissing(___FILE__,__LINE__);
						}
					}
				}
			m_buffer_size=1000*sizeof(sample_t)*info.channel_count*(info.sample_rate/1000);
			m_buffer.reset(new uint8_t[m_buffer_size]);
			}
		skip();
		}
	}




size_t WaveIO::RiffwaveReader::dataRead(void* buffer,size_t buffer_size)
	{
	auto ret=m_source.read(buffer,std::min(buffer_size,current_chunk_size));
	current_chunk_size-=ret;
	return ret;
	}

void WaveIO::RiffwaveReader::skip()
	{
	while(dataRead(m_buffer.get(),m_buffer_size)==m_buffer_size);
	}

size_t WaveIO::RiffwaveReader::read(sample_t* samples,size_t n_samples)
	{
	auto n_samples_0=n_samples;
	while(n_samples)
		{
		auto samples_to_read=std::min(m_buffer_size/sizeof(sample_t),n_samples);
		auto n_valid=dataRead(m_buffer.get(),samples_to_read*sizeof(sample_t))
			/sizeof(sample_t);
		converter(m_buffer.get(),samples,n_valid);
		if(n_valid!=samples_to_read)
			{break;}
		n_samples-=n_valid;
		samples+=n_valid;
		}
	return n_samples_0 - n_samples;
	}
