#include <memory>

namespace test1
{
	class Unc
	{
	public:
		Unc() {}

		virtual ~Unc() {}

		virtual void test() = 0;

	private:
		Unc(const Unc&);
		Unc const & operator=(Unc const&);
	};
}
namespace test2
{
	class Uncc : public test1::Unc
	{
	public:
		Uncc() {}

		void test() override{}

	private:
		Uncc(const Uncc&);
		Uncc const & operator=(Uncc const&);
	};
}

typedef std::shared_ptr<test1::Unc>  UncPtr;
typedef std::shared_ptr<test2::Uncc> UnccPtr;

void ccc(UncPtr cc_uncc)
{
	
}




int main()
{
	auto uncc = std::make_shared<test2::Uncc>();
	ccc(uncc);
	return 0;
}