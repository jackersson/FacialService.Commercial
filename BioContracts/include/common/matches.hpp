#ifndef Matches_INCLUDED
#define Matches_INCLUDED

#include <iostream>
#include <vector>

namespace BioContracts
{

	class Match
	{
	public:
		Match(long first_id, long second_id, float match) : p(first_id, second_id), match_(match) {
			std::cout << " match find : "<< first_id << " " << second_id << " " << match << std::endl;
		}
		Match(const Match& match) : p(match.p), match_(match.match_) {}

		long targetFaceId()     const {	return p.first; }
		long comparisonFaceId() const { return p.second; }

		float match() const { return match_; }
	private:
		std::pair<long, long> p;
		float match_;
	};

	class Matches
	{
	public:			
		void Add(const Match& match)
		{
			Match m(match);
			matches_.push_back(m);
		}

		const Match& operator[](size_t index) const
		{
			if (index >= size())
				throw std::invalid_argument("Index out of range");

			return matches_[index];
		}



		std::vector<Match>::const_iterator cbegin() { return matches_.cbegin(); }
		std::vector<Match>::const_iterator cend  () { return matches_.cend();   }

		void clear() { matches_.clear(); }
		size_t size() const { return matches_.size(); }
	private:
		std::vector<Match> matches_;
	};

	
}

#endif