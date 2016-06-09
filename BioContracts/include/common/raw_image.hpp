#ifndef RawImage_INCLUDED
#define RawImage_INCLUDED

namespace BioContracts
{
	class RawImage
	{
	public:
		RawImage(const std::string& bytes, long id) : p(bytes, id) {}
		RawImage(const RawImage& raw_image) : p(raw_image.p) {}

		long id() const { return p.second; }

		const std::string& bytes() const { return p.first; }

		size_t size() const { return p.first.size(); }
	private:
		std::pair<std::string, long> p;
	};
	
}

#endif