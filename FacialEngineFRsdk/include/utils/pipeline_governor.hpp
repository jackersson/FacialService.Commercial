#ifndef PipelineGovernor_INCLUDED
#define PipelineGovernor_INCLUDED

#include <agents.h>
#include <iostream>

namespace Pipeline
{
	class PipelineGovernor
	{	
	public:
		explicit PipelineGovernor(int capacity) : capacity_(capacity), working_units_(0) {}

		virtual ~PipelineGovernor() {}

		void free_unit(){
			send(completed_items_, signal());
		}

		void wait_available_working_unit()	{
			if (working_units_ < capacity_)
				++working_units_;
			else
				receive(completed_items_);
		}

		void wait_until_empty()	{
			while (working_units_ > 0)
			{
				--working_units_;
				receive(completed_items_);
			}
		}

		virtual void resize(int new_size)	{
			if (new_size <= capacity_)
			{
				std::cout << "tried to resize pipeline capacity to smaller number" << std::endl;
				return;
			}
			capacity_ = new_size;
		}

	protected:
		int capacity_;
		int working_units_;
		
	private:
		PipelineGovernor(const PipelineGovernor&);
		PipelineGovernor const & operator=(PipelineGovernor const&);

		struct signal {};

		concurrency::unbounded_buffer<signal> completed_items_;

	};

}
#endif