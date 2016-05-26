#include <memory>

class Unc
{
public:
	Unc() {}

private:
	Unc(const Unc&);
	Unc const & operator=(Unc const&);
};

template<typename T>
class UncT : public Unc
{
public:
	UncT() {}

private:
	UncT(const UncT&);
	UncT const & operator=(UncT const&);
};

class Uncc : public UncT<int>
{
public:
	Uncc() {}

private:
	Uncc(const Uncc&);
	Uncc const & operator=(Uncc const&);
};

typedef std::shared_ptr<Uncc> UnccPtr;

void ccc(UnccPtr cc_uncc)
{
	
}




int main()
{
	auto uncc = std::make_shared<Uncc>();
	ccc(uncc);
	return 0;
}