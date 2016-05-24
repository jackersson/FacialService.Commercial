#include "utils\utils.hpp"
#include <chrono>

namespace BioFacialEngine
{
	long Utils::getTicks()
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

		auto   value = now_ms.time_since_epoch();
		return (long)value.count();
	}	
}

/*

class FaceItem
{
public:
	FaceItem( int photo_id
		      , int i
					, long configuration
		      , const std::string& image) 
		      : photo_id_(photo_id)
					, id_(i)
					, image_(image) 
					, cofiguration_(configuration)
	{}

	int id() const {
		return id_;
	}

	int photoId() const {
		return photo_id_;
	}

	const std::string& image() const 	{
		return image_;
	}

	void facefind() { std::cout << id_ << " face find" << std::endl; }
	void enroll() { std::cout << id_ << " enroll" << std::endl; }

	void isocompliance() { std::cout << id_ << " isocompliance" << std::endl; }
	void portrait() { std::cout << id_ << " portrait" << std::endl; }
	void extractor() { std::cout << id_ << " extractor" << std::endl; }

	long configuration() const {
		return cofiguration_;
	}

	bool hasStage(PipelineStage stage)	{
		return (cofiguration_ & stage) == stage;
	}

private:
	int id_;
	long cofiguration_;
	int photo_id_;
	std::string image_;
};


class FaceItemHolder
{
public:
	FaceItemHolder() : counter_(0) {}

	void Add(const FaceItem& item)
	{
		concurrency::parallel_for_each(container_.cbegin(), container_.cend(),
			[&](const FaceItem& iter_item)
		{

			if (item.photoId() == iter_item.photoId())
				return;

			++counter_;
			//std::cout << "compare " << item.image() << " vs " << iter_item.image()  << std::endl;
			bool score = item.image() == iter_item.image();
			float match = score ? 1.0f : 0.0f;

			if (score)
			{
				BioContracts::Match match_s(item.id(), iter_item.id(), match);
				matches_.Add(match_s);
			}
		});

		container_.push_back(item);
	}

	void clear()	{
		container_.clear();
		matches_.clear();
	}

	BioContracts::Matches matches() const	{
		return matches_;
	}

	int counter() const
	{
		return counter_;
	}

	size_t size() const	{
		return matches_.size();
	}

private:
	std::list<FaceItem> container_;
	BioContracts::Matches matches_;

	int counter_;
};

int main()
{

	long config = FaceFind | Portrait | Extract | Iso | Enroll;

	long acquire = FaceFind | Portrait | Iso | Extract | Verify;

	long enroll = acquire | Enroll | Verify;

	FaceItemHolder verificator;

	transformer<FaceItem, FaceItem> facefinder([](FaceItem n) {
		n.facefind();
		return n;
	});

	//unbounded_buffer<int> faces;	
	
	call<FaceItem> isocompliance([](FaceItem n) {
		n.isocompliance();
	});


	call<FaceItem> verification([&](FaceItem n) {
		verificator.Add(n);
	});

	call<FaceItem> portrait([&](FaceItem n) {
		n.portrait();

		if (n.hasStage(PipelineStage::Iso))
			asend(isocompliance, n);

		if (n.hasStage(PipelineStage::Verify) && !n.hasStage(PipelineStage::Enroll))
			asend(verification, n);
	});

	call<FaceItem> enrollment([&](FaceItem n) {

		n.enroll();
		if (n.hasStage(PipelineStage::Verify))
			asend(verification, n);
	});


	call<FaceItem> extractor([&](FaceItem n) {
		n.extractor();

		if (n.hasStage(PipelineStage::Enroll))
			asend(enrollment, n);
	});


	call<FaceItem> faceholder([&](FaceItem n) {
		if (n.hasStage(PipelineStage::Portrait))
			asend(portrait, n);

		if (n.hasStage(PipelineStage::Extract))
			asend(extractor, n);
	});

	
	facefinder.link_target(&faceholder);


	FaceItem item1(1, 1, acquire, "Tolik1");
	FaceItem item2(1, 2, acquire, "Sasha1");

	FaceItem item11(2, 1, enroll, "Taras1");
	FaceItem item21(2, 2, enroll, "Sasha1");

	asend(facefinder, item1 );
	asend(facefinder, item11);
	asend(facefinder, item2 );
	asend(facefinder, item21);

	std::cin.get();

	std::cout << "verification counter : " << verificator.counter() << std::endl;

	std::cin.get();
}

*/