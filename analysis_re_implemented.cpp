//Garbellini Matteo
//Dept. of Physics
//University of Milan
//Project: Networks and Complex Structures
//Latest working build: June 27th 2019

//LOG
//June 27th		re-implementation: fixing memory-alloc errors
//June 28th		events in clusters, edges in cluster



#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include "higgs_analysis_tools.h"


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
	std::cerr<<"NOT MATCHING CLUSTERS: "<<not_matching<<std::endl;

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
		nodes[user1].active_in_event.push_back(&events[n_events]);
		n_events++;
		nodes[user1].is_active = 1;
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

	for(int i=1; i<n_nodes+1; i++)
	{
		for(int j=0; j<nodes[i].n_following; j++)
		{
			if(nodes[i].clusterID == nodes[i].following[j]->clusterID)
			{
				//edges within
				clusters[nodes[i].clusterID].n_edges[0]++; 
			}
			else
			{
				//edges outgoing
				clusters[nodes[i].clusterID].n_edges[2]++; 
				//edges ingoing
				clusters[nodes[i].following[j]->clusterID].n_edges[1]++; 
			}
		}
	}

	//COMPUTE ACTIVITY and INTERTIME
	for(int i=1; i<n_nodes+1; i++)
	{
		nodes[i].compute_activations();
		nodes[i].compute_intertime();
	}

	//COMPUTE ACTIVE USERS, INTERTIME AND AVERAGE ACTIVATION
	for(int i=0; i<n_clusters; i++)
	{
		clusters[i].find_active_users();
		clusters[i].compute_activity();
		clusters[i].compute_intertime();
	}

	for(int i=0; i<n_clusters; i++)
	{
		if(clusters[i].size > 10)
		{
			std::cout<<clusters[i].size <<" "<< clusters[i].average_intertime<<std::endl;;
		}
	}






















}





































