#ifndef Matches_INCLUDED
#define Matches_INCLUDED

#include <iostream>
#include <list>
#include <map>
#include <memory>

namespace BioContracts
{

	class Match
	{
	public:
		Match(long face_id, float match) : p(face_id, match) {
			std::cout << " Match  -> face : " << face_id << " score: " << match << std::endl;
		}
		Match(const Match& match) : p(match.p) {}

		long face_id()     const {	return p.first; }
		float match() const { return p.second; }
	private:
		std::pair<long, float> p;	
	};

	typedef std::list<Match> MatchSet;
	typedef std::shared_ptr<MatchSet> MatchSetPtr;

	class Matches
	{
	public:			
		explicit Matches(long population_id) : population_id_(population_id)	{}

		void add( long face_id, const Match& match)
		{
			auto it = matches_.find(face_id);
			if (it != matches_.end())
				it->second.push_back(match);
			else
			{
				std::list<Match> matches = { match };
				matches_.insert(std::pair<long, std::list<Match>>(face_id, matches));
			}			
		}

		void add(long face_id, const std::list<Match>& matches)
		{
			auto it = matches_.find(face_id);
			if (it != matches_.end())
			{
				for (auto iter = matches.cbegin(); iter != matches.cend(); ++iter)
					it->second.push_back(*iter);
			}
			else				
				matches_.insert(std::pair<long, std::list<Match>>(face_id, matches));			
		}

		void add( long face_id, const Matches& matches)
		{
			for (auto it = matches.cbegin(); it != matches.cend(); ++it)			
				add(face_id, it->second);			
		}

		const std::list<Match>& operator[](size_t index) const
		{
			if (index >= size())
				throw std::invalid_argument("Index out of range");

			auto it = matches_.cbegin();
			std::advance(it, index);
			return it->second;
		}

		std::map<long, std::list<Match>>::const_iterator cbegin() const { return matches_.cbegin(); }
		std::map<long, std::list<Match>>::const_iterator cend  () const { return matches_.cend()  ; }

		void clear() { matches_.clear(); }
		size_t size() const { return matches_.size(); }
	private:
		std::map<long, std::list<Match>> matches_;
		long population_id_;
	};

	typedef std::shared_ptr<Matches> MatchesPtr;

	
}

#endif