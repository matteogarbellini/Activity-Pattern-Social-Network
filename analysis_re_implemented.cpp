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

	int ID;
	int clusterID;
	int n_follower;
	int n_following;
	int is_active;
	int activations;
	float intertime;
	std::vector<NODE*> follower;
	std::vector<NODE*> following;	
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
		n_total_events =0;

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
	int n_edges[3]; //1 - within, 2 - ingoing, 3 - outgoing

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




int main(int argc, char** argv)
{
	std::ifstream input;
	std::ofstream output;


	std::vector<NODE> nodes;
	std::vector<EVENT> events;
	std::vector<CLUSTER> clusters;


	//AUXILIARY VARIABLES
	int id, n, iter;
	int user1, user2;
	int time, t_events;
	int n_nodes = 0;
	int n_clusters = 0;
	int n_events = 0;


	//READ USERS;
	input.open("users.txt");
	NODE aux;
	nodes.push_back(aux);
	while(input.good())
	{
		input >> id;
		aux.ID = id;
		nodes.push_back(aux);
		n_nodes++;
	}
	input.close();
	
	//READ FOLLOWING
	input.open("following.txt");
	while(input.good())
	{
		input >> user1 >> user2;
		nodes[user1].following.push_back(&nodes[user2]);
		nodes[user2].follower.push_back(&nodes[user1]);

	}
	input.close();

	//READ CLUSTERS
	input.open("clusters.txt");
	CLUSTER cluster(0);
	while(input.good())
	{
		input >> id;
		cluster.ID = id;
		clusters.push_back(cluster);
		n_clusters++;
	}
	input.close();

	//DEBUG CLUSTERS ID
	int not_matching=0;
	for(int i=0; i<n_clusters; i++)
	{
		if(clusters[i].ID != i)
		{
			std::cerr<<"Errors: clusters IDs not matching"<<std::endl;
			not_matching++;
		}
	}
	std::cout<<"NOT MATCHING CLUSTERS: "<<not_matching<<std::endl;

	//READ NODE in CLUSTER
	input.open("node_clusters.txt");
	NODE* node_to_push;
	while(input.good())
	{
		input >> id >> n;
		nodes[id].clusterID = n;
		node_to_push = &nodes[id];
		clusters[n].nodes_in_cluster.push_back(node_to_push);
	}

	for(int i=0; i<n_clusters; i++)
	{
		clusters[i].size = clusters[i].nodes_in_cluster.size();
	}
	input.close();

	//READ EVENTS
	input.open("interactions.txt");
	EVENT interaction;
	while(input.good())
	{
		input >> user1 >> user2 >> time >> t_events;
		interaction.type = t_events;
		interaction.timestamp = time;
		interaction.active_user = &nodes[user1];
		interaction.passive_user = &nodes[user2];
		events.push_back(interaction);
		n_events++;
	}

	for(int i=0; i<n_events; i++)
	{
		if(i==0)
		{
			events[i].next = &events[i+1];
			events[i].previous = NULL;
		}
		else if(i==n_events-1)
		{
			events[i].previous = &events[i-1];
			events[i].next = &events[i+1];
		}
		else
		{
			events[i].previous = &events[i-1];
			events[i].next = NULL;
		}
	}

	//EVENTS WITHIN/INGOING/OUTGOING CLUSTER
	for(int i =0; i<n_events; i++)
	{
		if(events[i].active_user->clusterID == events[i].passive_user->clusterID)
		{
			clusters[events[i].active_user->clusterID].events_within.push_back(&events[i]);
			switch(events[i].type)
			{
				case 1:
					clusters[events[i].active_user->clusterID].n_events_within[0]++;
					break;
				case 2:
					clusters[events[i].active_user->clusterID].n_events_within[1]++;
					break;
				case 3:
					clusters[events[i].active_user->clusterID].n_events_within[2]++;
					break;
				default:
					std::cerr<<"Error: event is of unknown type"<<std::endl;
					break; 
			}
		}
		else
		{
			clusters[events[i].active_user->clusterID].events_outgoing.push_back(&events[i]);
			clusters[events[i].passive_user->clusterID].events_ingoing.push_back(&events[i]);
			switch(events[i].type)
			{
				case 1:
					clusters[events[i].active_user->clusterID].n_events_outgoing[0]++;
					clusters[events[i].passive_user->clusterID].n_events_ingoing[0]++;
					break;
				case 2:
					clusters[events[i].active_user->clusterID].n_events_outgoing[1]++;
					clusters[events[i].passive_user->clusterID].n_events_ingoing[1]++;
					break;
				case 3:
					clusters[events[i].active_user->clusterID].n_events_outgoing[2]++;
					clusters[events[i].passive_user->clusterID].n_events_ingoing[2]++;
					break;
				default:
					std::cerr<<"Error: event is of unknown type"<<std::endl;
					break; 
			}
		}
	}





















}





































