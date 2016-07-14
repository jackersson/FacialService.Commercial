#ifndef RawImage_INCLUDED
#define RawImage_INCLUDED

namespace BioContracts
{
	enum ImageFileType
	{
		JPG,      // joint photographic experts group - .jpeg or .jpg
		PNG,      // portable network graphics
		GIF,      // graphics interchange format 
		TIFF,     // tagged image file format
		BMP,      // Microsoft bitmap format
		WEBP,     // Google WebP format, a type of .riff file
		ICO,      // Microsoft icon format
		INVALID,  // unidentified image types.
	};

	class RawImage
	{
	public:
		RawImage(const std::string& bytes, long id) : pair_(bytes, id)		{
			pixel_format_ = check_pixel_format(bytes.c_str(), bytes.size());			
			//std::cout << "image_format = " << pixel_format();
		}
		RawImage(const RawImage& raw_image) : pair_(raw_image.pair_)
			                                  , pixel_format_(raw_image.pixel_format_){}

		long id() const { return pair_.second; }

		const std::string& bytes() const { return pair_.first; }

		std::string pixel_format() const {		
			return get_pixel_fomat_name(pixel_format_);
		}	

		size_t size() const { return pair_.first.size(); }

	private:

		static std::string get_pixel_fomat_name(ImageFileType pixel_format)
		{
			switch (pixel_format)
			{
			case	JPG:  
				return "jpg";
			case	PNG:
				return "png";
			case	GIF:
				return "gif";
			case	TIFF:
				return "tiff";
			case	BMP:
				return "bmp";
			case	WEBP:
				return "webp";
			case	ICO:
				return "ico";

			default:
				return "inavlid file format";
			}
		}

		static ImageFileType check_pixel_format(const char* data, size_t len)
		{
			if (len < 16) return INVALID;

			// .jpg:  FF D8 FF
			// .png:  89 50 4E 47 0D 0A 1A 0A
			// .gif:  GIF87a      
			//        GIF89a
			// .tiff: 49 49 2A 00
			//        4D 4D 00 2A
			// .bmp:  BM 
			// .webp: RIFF ???? WEBP 
			// .ico   00 00 01 00
			//        00 00 02 00 ( cursor files )

			switch (data[0])
			{
			case '\xFF':
				return (!strncmp(data, "\xFF\xD8\xFF", 3)) ?
					JPG : INVALID;

			case '\x89':
				return (!strncmp(static_cast<const char*>(data),
					"\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8)) ?
					PNG : INVALID;

			case 'G':
				return (!strncmp(static_cast<const char*>(data), "GIF87a", 6) ||
					!strncmp(static_cast<const char*>(data), "GIF89a", 6)) ?
					GIF : INVALID;

			case 'I':
				return (!strncmp(static_cast<const char*>(data), "\x49\x49\x2A\x00", 4)) ?
					TIFF : INVALID;

			case 'M':
				return (!strncmp(static_cast<const char*>(data), "\x4D\x4D\x00\x2A", 4)) ?
					TIFF : INVALID;

			case 'B':
				return ((data[1] == 'M')) ?
					BMP : INVALID;

			case 'R':
				if (strncmp(static_cast<const char*>(data), "RIFF", 4))
					return INVALID;
				if (strncmp(static_cast<const char*>(data + 8), "WEBP", 4))
					return INVALID;
				return WEBP;

			case '\0':
				if (!strncmp(static_cast<const char*>(data), "\x00\x00\x01\x00", 4))
					return ICO;
				if (!strncmp(static_cast<const char*>(data), "\x00\x00\x02\x00", 4))
					return ICO;
				return INVALID;

			default:
				return INVALID;
			}
		}

		std::pair<std::string, long> pair_;
		ImageFileType pixel_format_;

	};
	
}

#endif