//Garbellini Matteo
//Dept. of Physics
//University of Milan
//Project: Networks and Complex Structures
//Latest working build: June 27th 2019

//LOG
//June 27th		re-implementation: fixing memory-alloc errors

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>

class NODE
{
	public:
		NODE()
		{
			ID = 0;
			is_active = 0;
			follower = 0;
			following = 0;
			activations = 0;
			intertime = 0;
		}

		NODE(int node_ID)
		{
			ID = node_ID;
		}

		~NODE()
		{

		}

		int ID;
		int clusterID;
		int follower;
		int following;
		int is_active;
		int activations;
		float intertime;
		std::vector<NODE*> follower;
		std::vector<NODE*> following;	
} 

class EVENT
{
	public:
		EVENT()
		{

		}

		EVENT(int t, int time)
		{
			type = t;
			timestamp = time;
		}

		~EVENT()
		{
			
		}

		int type;
		int timestamp;
		int ID;

		NODE* active_user;
		NODE* passive_user;
		EVENT* previous;
		EVENT* next;
}

class CLUSTER
{
	CLUSTER()
	{

	}
	CLUSTER()
	{

	}
	~CLUSTER()
	{

	}
}


































