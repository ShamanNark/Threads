/**/


#include "stdafx.h"
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <stack>			  // std::deque
#include <map>				  // std::map
#include <conio.h>
#include <windows.h> 

std::mutex mtx, second_mtx;
std::condition_variable cv;
std::deque<int> aStack;
std::map<uint64_t, uint64_t> RezultThreads;
bool finish = false;


void search_NOK(int x, std::map<uint64_t, uint64_t> &mask)
{
	int i = 2;
	int summ = 0;
	while (x > 1)
	{

		if (x % i == 0)
		{
			summ += 1;
			x /= i;

			//add this value in map 
			if (mask.find(i) == mask.end())
			{
				// not found
				mask.insert(std::pair<uint64_t, uint64_t>(i, summ));
			}
			else
			{
				//found
				if (mask[i] < summ)	mask[i] = summ;
			}

		}
		else
		{
			i++;
			summ = 0;
		}
	}
}

void print_id(int id)
{
	std::unique_lock<std::mutex> lck(mtx);
	std::map<uint64_t, uint64_t> mask;
	int var = 0;
	while (!finish)
	{

		cv.wait(lck);
		if (!aStack.empty())
		{
			second_mtx.lock();
			var = aStack.front();
			aStack.pop_front();
			second_mtx.unlock();

			search_NOK(var, mask);
		}

	}

	second_mtx.lock();
	for (std::map<uint64_t, uint64_t>::iterator it = mask.begin(); it != mask.end(); ++it)
	{
		if (RezultThreads.find(it->first) == RezultThreads.end())
		{
			//not found 
			RezultThreads.insert(std::pair<uint64_t, uint64_t>(it->first, it->second));
		}
		else
		{
			// found 
			if (RezultThreads[it->first] < it->second)
			{
				RezultThreads[it->first] = it->second;
			}

		}
	}
	second_mtx.unlock();
	std::cout << "end thread number =  " << id << std::endl;
}

int main()
{
	std::thread threads[10];

	int var = 1;
	for (int i = 0; i < 10; ++i)
		threads[i] = std::thread(print_id, i);

	while (var>0)
	{

		std::cin >> var;
		if (var != 0)
		{
			second_mtx.lock();
			aStack.push_back(var);
			second_mtx.unlock();
		}
		else
		{
			finish = true;
			cv.notify_all();
			break;
		}
		if (!aStack.empty()) cv.notify_one();
	}

	for (auto& th : threads) th.join();

	uint64_t NOK = 1;
	for (auto& it = RezultThreads.begin(); it != RezultThreads.end(); ++it)
	{
		std::cout << it->first << " == > " << it->second << std::endl;
		for (int i = 1; i <= it->second; i++)
		{
			NOK *= it->first;
		}
	}

	std::cout << " Least common multiple == " << NOK << std::endl;
	std::cin >> var;
	return 0;
}