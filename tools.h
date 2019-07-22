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
	float burst_index;

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
		cluster_within_intertime = 0;
		cluster_outgoing_intertime = 0;
		cluster_ingoing_intertime = 0;
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
	void compute_event_intertime(int type);
	void compute_burst_index();
	void compute_fraction_active();
	void compute_time_dependent_fraction_active();			//NEEDS BETTER IMPLEMENTATION

	void print_intertime();

	//output function

	int ID;
	int size;

	int n_active_users;
	float fraction_active;
	int n_events_within[3];
	int n_events_ingoing[3];
	int n_events_outgoing[3];
	int n_total_events_by_type[3];
	int n_total_events;
	int n_edges[3]; //0 - within, 1 - ingoing, 2 - outgoing

	float average_intertime;
	float average_activity;
	float cluster_within_intertime;
	float cluster_outgoing_intertime;
	float cluster_ingoing_intertime;

	std::vector<NODE*> nodes_in_cluster;
	std::vector<EVENT*> events_within;
	std::vector<EVENT*> events_ingoing;
	std::vector<EVENT*> events_outgoing;

	std::vector<double> fraction_active_users;
	std::vector<float> fraction_active_timestamp;


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

void CLUSTER::compute_fraction_active()
{	
	if(size>24 && n_active_users > 1)
	{
		fraction_active = (float) n_active_users / size;
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

void CLUSTER::compute_time_dependent_fraction_active()                                                                   //<----- NEED TO BE IMPLEMENTED
{
	int bin_size = 600;
	int iter = 0;
	double fraction = 0;
	for(int i=0; i<1000; i++)
	{
		for(int i=0; i<events_within.size(); i++)
		{
			if(events_within[i]->timestamp > i*bin_size && events_within[i]->timestamp < (i+1)*bin_size)
			{
				fraction++;
			}
		}
		for(int i=0; i<events_outgoing.size(); i++)
		{
			if(events_outgoing[i]->timestamp > i*bin_size && events_outgoing[i]->timestamp < (i+1)*bin_size)
			{
				fraction++;
			}
		}

		fraction_active_users.push_back((double)fraction/n_active_users);
		fraction_active_timestamp.push_back((i+1)*bin_size);
	}
}

void CLUSTER::compute_event_intertime(int type) 
{
	//type: 0-retweet, 1-mentions, 2-replies, 3-no-type-selected
	int events_by_type_within = 0;
	int events_by_type_outgoing = 0;
	int events_by_type_ingoing = 0;
	if(size > 24)
	{
		//within
		for(int i=1; i<events_within.size(); i++)
		{
			if(type == 3)
			{
				cluster_within_intertime += events_within[i]->timestamp - events_within[i-1]->timestamp;
			}
			else if(events_within[i]->type == type)
			{
				cluster_within_intertime += events_within[i]->timestamp - events_within[i-1]->timestamp;
				events_by_type_within++;
			}
				
		}

		//outgoing
		for(int i=1; i<events_outgoing.size(); i++)
		{
			if(type==3)
			{
				cluster_outgoing_intertime += events_outgoing[i]->timestamp - events_outgoing[i-1]->timestamp;
			}
			else if(events_outgoing[i]->type == type)
			{
				cluster_outgoing_intertime += events_outgoing[i]->timestamp - events_outgoing[i-1]->timestamp;
				events_by_type_outgoing++;
			}
		}

		//ingoing
		for(int i=1; i<events_ingoing.size(); i++)
		{
			if(type==3)
			{
				cluster_ingoing_intertime += events_ingoing[i]->timestamp - events_ingoing[i-1]->timestamp;
			}
			else if(events_ingoing[i]->type == type)
			{
				cluster_ingoing_intertime += events_ingoing[i]->timestamp - events_ingoing[i-1]->timestamp;
				events_by_type_ingoing++;
			}
		}

		if(type == 3)
		{
			cluster_within_intertime = (float) n_active_users*cluster_within_intertime/(events_within.size()-1);
			cluster_outgoing_intertime = (float) n_active_users*cluster_outgoing_intertime/(events_outgoing.size()-1);
			cluster_ingoing_intertime = (float) n_active_users*cluster_ingoing_intertime/(events_ingoing.size()-1);
		}
		else
		{
			cluster_within_intertime = (float) n_active_users*cluster_within_intertime/(events_by_type_within);
			cluster_outgoing_intertime = (float) n_active_users*cluster_outgoing_intertime/(events_by_type_outgoing);
			cluster_ingoing_intertime = (float) n_active_users*cluster_ingoing_intertime/(events_by_type_ingoing);
		}
			
	}
}

void CLUSTER::compute_burst_index()
{
	for(int i=0; i<events_within.size(); i++)
	{
		if(events_within[i]->previous != NULL && events_within[i]->next != NULL)
		{	
			if(events_within[i]->next->timestamp == events_within[i]->previous->timestamp)
			{
				events_within[i]->burst_index = 666;
			}
			else
			{
				events_within[i]->burst_index = (float) 2/(events_within[i]->next->timestamp - events_within[i]->previous->timestamp);
			}
			
		}
		else
		{
			events_within[i]->burst_index = 666;
		}
	}

	for(int i=0; i<events_outgoing.size(); i++)
	{
		if(events_outgoing[i]->previous != NULL && events_outgoing[i]->next != NULL)
		{	
			if(events_outgoing[i]->next->timestamp == events_outgoing[i]->previous->timestamp)
			{
				events_outgoing[i]->burst_index = 666;
			}
			else
			{
				events_outgoing[i]->burst_index = (float) 2/(events_outgoing[i]->next->timestamp - events_outgoing[i]->previous->timestamp);
			}
			
		}
		else
		{
			events_outgoing[i]->burst_index = 666;
		}
	}
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

inline int normalize(int normalized, int active_nodes)
{
	if(normalized)
	{
		return active_nodes;
	}
	else
	{
		return 1;
	}
}

void print_n_events(int type_1, int type_2, std::vector<CLUSTER>& clusters, int normalized, std::ofstream& output)
{
	if(type_1 == 0) //within
	{
		for(int i=0; i<clusters.size(); i++)
		{
			if(clusters[i].size > 24)
			{
				output<< clusters[i].size << " " << (double) clusters[i].n_events_within[type_2]/normalize(normalized, clusters[i].n_active_users) <<std::endl;
			}
		}
	}
	else if(type_1 == 1) //ingoing
	{
		for(int i=0; i<clusters.size(); i++)
		{
			if(clusters[i].size > 24)
			{
				output<< clusters[i].size << " " << (double) clusters[i].n_events_ingoing[type_2]/normalize(normalized, clusters[i].n_active_users) <<std::endl;
			}		
		}
	}
	else if(type_1 == 2) //outgoing
	{
		for(int i=0; i<clusters.size(); i++)
		{
			if(clusters[i].size > 24)
			{
				output<< clusters[i].size << " " << (double) clusters[i].n_events_outgoing[type_2]/normalize(normalized, clusters[i].n_active_users) <<std::endl;
			}
		}
	}
	else
	{
		std::cerr<<"Error: non-existent event type"<<std::endl;
	}
}

void compute_burst_index_from_event_list(std::vector<EVENT>& events, int n_events)
{
	float order1;
	float order2;
	for(int i=2; i<n_events-2; i++)
	{
		order1 = (events[i+1].timestamp - events[i-1].timestamp)/2;
		order2 = (events[i+2].timestamp - events[i-2].timestamp)/4;
		events[i].burst_index = (float) order1 + order2;
	}
}







#endif //TOOLS_FOR_ACTIVITY_PATTERN_ANALYSIS