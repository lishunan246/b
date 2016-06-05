//
// Created by lishunan on 16-5-23.
//

#ifndef DESKTOP_REPEATWORK_H
#define DESKTOP_REPEATWORK_H

#include <atomic>
#include <functional>
#include <thread>
#include <type_traits>
#include <mutex>
#include <condition_variable>

namespace GkcHostSvc
{
	class RepeatWork//:std::enable_shared_from_this<RepeatWork>
	{
		std::atomic<bool> _active{false}, _close{false};
		std::atomic<int> _c{ 0 };
		std::mutex m;
		std::condition_variable cv;
		std::shared_ptr<std::thread> pthread;
		int count;
	public:
		template <class callable, class... arguments>
		RepeatWork(int afterMS, bool repeat, callable&& f, arguments&&... args) : count(0)
		{
			println("init RepeatWork");
			std::function<typename std::result_of<callable(arguments ...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

			if (repeat)
			{
				//auto self(shared_from_this());
				pthread=make_shared<std::thread>([afterMS,task, this]()
					{
						while (1)
						{
							
							if (_c!=0)
							{
								++this->count;
								cv.notify_one();
								break;
							}
								
							if (this->_active)
							{
								std::this_thread::sleep_for(std::chrono::milliseconds(afterMS));
								task();
								//cout << "running " << this->_active;
							}
						}
				});
			}
			else
			{
				std::thread([&]()
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(afterMS));
					task();
				});
				
			}
		}
		~RepeatWork()
		{
			println("end repeatWork");
		}

		void deactivate()
		{
			_active = false;
		}

		void activate()
		{
			_active = true;
		}

		void close()
		{
			
			this->_close.store(true);
			_c+=1;
			std::unique_lock<std::mutex> lk(this->m);
			while (count==0)
			{
				cv.wait(lk);
			}
			--count;
			pthread->join();
			
		}
	};
}

#endif //DESKTOP_REPEATWORK_H
