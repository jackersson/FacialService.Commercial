#ifndef RawImage_INCLUDED
#define RawImage_INCLUDED

#include <string>

namespace BioContracts
{
	class RawImage
	{
	public:
		RawImage(const std::string& bytes, size_t size) : p(bytes, size) {}
		RawImage(const RawImage& raw_image) : p(raw_image.p) {}

		const std::string& bytes() const { return p.first; }

		size_t size() const { return p.second; }
	private:
		std::pair<std::string, size_t> p;
	};
	
}

#endif