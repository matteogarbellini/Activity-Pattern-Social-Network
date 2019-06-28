//M. Garbellini
//University of Milan, Dept. of Physics
//Networks and Complex Structures Project

#ifndef TOOLS_FOR_ACTIVITY_PATTERN_ANALYSIS
#define TOOLS_FOR_ACTIVITY_PATTERN_ANALYSIS



#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>

struct EVENT;

struct NODE
{
	public:

	NODE()
	{
		ID = 0;
		is_active = 0;
		n_follower = 0;
		n_following = 0;
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

	void compute_intertime();
	void compute_activations();

	int ID;
	int clusterID;
	int n_follower;
	int n_following;
	int is_active;
	int activations;
	float intertime;
	std::vector<NODE*> follower;
	std::vector<NODE*> following;
	std::vector<EVENT*> active_in_event;	
};





struct EVENT
{
	public:

	EVENT()
	{
		type = 0;
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
};

struct CLUSTER
{
	public: 

	CLUSTER(int cluster)
	{
		ID = cluster;
		n_active_users = 0;
		average_intertime = 0;
		average_activity = 0;
		n_total_events = 0;

		for(int i=0; i<3; i++)
		{
			n_events_within[i] = 0;
			n_events_ingoing[i] = 0;
			n_events_outgoing[i] = 0;
			n_total_events_by_type[i] = 0;
			n_edges[i] = 0;
		}
	}
	
	~CLUSTER()
	{

	}

	//Functions declaration
	void find_active_users();
	void compute_intertime();
	void compute_activity();

	int ID;
	int size;

	int n_active_users;
	int n_events_within[3];
	int n_events_ingoing[3];
	int n_events_outgoing[3];
	int n_total_events_by_type[3];
	int n_total_events;
	int n_edges[3]; //0 - within, 1 - ingoing, 2 - outgoing

	float average_intertime;
	float average_activity;

	std::vector<NODE*> nodes_in_cluster;
	std::vector<EVENT*> events_within;
	std::vector<EVENT*> events_ingoing;
	std::vector<EVENT*> events_outgoing;

};


void CLUSTER::find_active_users()
{
	for(int i=0; i<size; i++)
	{
		if(nodes_in_cluster[i]->is_active)
		{
			n_active_users++;
		}
	}
}

void CLUSTER::compute_activity()
{
	int n_intertime=0;
	for(int i=0; i<size; i++)
	{
		if(nodes_in_cluster[i]->is_active)
		{
			average_activity+= nodes_in_cluster[i]->activations;
		}
	}
	average_activity = (float) average_activity / size;
}

void CLUSTER::compute_intertime()
{
	for(int i=0; i<size; i++)
	{
		if(nodes_in_cluster[i]->activations > 1)
		{
			average_intertime+= nodes_in_cluster[i]->intertime;
		}
	}
	average_intertime = (float) average_intertime / size;
}

void NODE::compute_intertime()
{
	if(activations>1)
	{
		for(int i=0; i< active_in_event.size()-1; i++)
		{
			intertime += active_in_event[i+1]->timestamp - active_in_event[i]->timestamp; 
		}

		intertime = (float) intertime/(active_in_event.size()-1);
	}
}

void NODE::compute_activations()
{
	activations = active_in_event.size();
}


#endif //TOOLS_FOR_ACTIVITY_PATTERN_ANALYSIS
