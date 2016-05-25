#ifndef WorkUnitGovernor_INCLUDED
#define WorkUnitGovernor_INCLUDED

#include <agents.h>

namespace Pipeline
{
	class WorkUnitGovernor
	{
	private:
		struct signal {};

		int m_capacity  ;
		int working_units;
		concurrency::unbounded_buffer<signal> m_completedItems;

	public:
		WorkUnitGovernor(int capacity) : working_units(0), m_capacity(capacity) {}

		void FreeUnit(){
			send(m_completedItems, signal());
		}

		void AddWorkingUnit()	{
			if (working_units < m_capacity)
				++working_units;
			else
				receive(m_completedItems);
		}

		void Wait()	{
			while (working_units > 0)
			{
				--working_units;
				receive(m_completedItems);
			}
		}

	private:
		WorkUnitGovernor(const WorkUnitGovernor&);
		WorkUnitGovernor const & operator=(WorkUnitGovernor const&);
	};

}
#endif