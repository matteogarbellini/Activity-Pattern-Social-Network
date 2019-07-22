//Garbellini Matteo
//Dept. of Physics
//University of Milan
//Project: Networks and Complex Structures
//Latest working build: June 29th 2019

//LOG
//June 27th		re-implementation: fixing memory-alloc errors
//June 28th		events in clusters, edges in cluster
//June 29th 	implemented print_n_events() function (see tools.h)
//July 2nd		intertime by type within cluster, regardless of user


#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include "tools.h"


int main(int argc, char** argv)
{
	std::ifstream input;
	std::ofstream output, output_2, output_3;


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
	input.open("./include/users.txt");
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
	input.open("./include/following.txt");
	while(input.good())
	{
		input >> user1 >> user2;
		nodes[user1].following.push_back(&nodes[user2]);
		nodes[user2].follower.push_back(&nodes[user1]);

	}
	input.close();

	//READ CLUSTERS
	input.open("./include/clusters.txt");
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
			not_matching++;
			std::cerr<<"Errors: clusters IDs not matching (total not matching: "<< not_matching<<")"<<std::endl;
			
		}
	}
	//READ NODE in CLUSTER
	input.open("./include/node_clusters.txt");
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
	input.open("./include/interactions.txt");
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
			events[i].next = NULL;
		}
		else
		{
			events[i].previous = &events[i-1];
			events[i].next = &events[i+1];
			
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
		clusters[i].compute_fraction_active();
		clusters[i].compute_activity();
		clusters[i].compute_intertime();
		if(clusters[i].size > 24)
		{
			clusters[i].compute_event_intertime(2);
			clusters[i].compute_time_dependent_fraction_active();
		}
	}

	/*
		output.open("intertime_re_within.txt");
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size > 24)
			{
				output<<clusters[i].size << " " << clusters[i].cluster_within_intertime <<std::endl;
			}
		}
		output.close();

		output.open("intertime_re_outgoing.txt");
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size > 24)
			{
				output<<clusters[i].size << " " << clusters[i].cluster_outgoing_intertime <<std::endl;
			}
		}
		output.close();

		output.open("intertime_re_ingoing.txt");
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size > 24)
			{
				output<<clusters[i].size << " " << clusters[i].cluster_ingoing_intertime <<std::endl;
			}
		}
		output.close();
		*/
		/*
		//PRINT 0-within, 1-ingoing, 2-outgoing, 0-retweet, 1-mentions, 2-replies
		//WHITIN
		output.open("RT_within_normalized.txt");
		print_n_events(0, 0, clusters, 1, output);
		output.close();

		output.open("MT_within_normalized.txt");
		print_n_events(0, 1, clusters, 1, output);
		output.close();

		output.open("RE_within_normalized.txt");
		print_n_events(0, 2, clusters, 1, output);
		output.close();

		//INGOING
		output.open("RT_ingoing_normalized.txt");
		print_n_events(1, 0, clusters, 1, output);
		output.close();

		output.open("MT_ingoing_normalized.txt");
		print_n_events(1, 1, clusters, 1, output);
		output.close();

		output.open("RE_ingoing_normalized.txt");
		print_n_events(1, 2, clusters, 1, output);
		output.close();

		//OUTGOING
		output.open("RT_ougoing_normalized.txt");
		print_n_events(2, 0, clusters, 1, output);
		output.close();

		output.open("MT_ougoing_normalized.txt");
		print_n_events(2, 1, clusters, 1, output);
		output.close();

		output.open("RE_ougoing_normalized.txt");
		print_n_events(2, 2, clusters, 1, output);
		output.close();
		*/


		/*
		output.open("intertime_within.txt");
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size >24)
			{
				output<< clusters[i].size << " " << clusters[i].cluster_within_intertime<<std::endl;
			} 
		}
		output.close();

		output.open("intertime_outgoing.txt");
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size >24)
			{
				output<< clusters[i].size << " " << clusters[i].cluster_outgoing_intertime<<std::endl;
			} 
		}
		output.close();

		output.open("intertime_node_average.txt");
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size >24)
			{
				output<< clusters[i].size << " " << clusters[i].average_intertime<<std::endl;
			} 
		}
		output.close();
		*/



		/*
		output.open("burst_intertime.txt");
		for(int i=0; i<n_clusters; i++)
		{
			clusters[i].compute_burst_index();
			for(int j=0; j<clusters[i].events_within.size(); j++)
			{
				if(clusters[i].events_within[j]->burst_index != 666)
				{
					output << clusters[i].events_within[j]->timestamp << " " << clusters[i].events_within[j]->burst_index << std::endl;
				}
			}
			for(int j=0; j<clusters[i].events_outgoing.size(); j++)
			{
				if(clusters[i].events_outgoing[j]->burst_index != 666)
				{
					output << clusters[i].events_outgoing[j]->timestamp << " " << clusters[i].events_outgoing[j]->burst_index << std::endl;
				}
			}
		}

		output.close();
		*/
		/*
		compute_burst_index_from_event_list(events, n_events);
		output.open("burst_index_s100-1000.txt");
		
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size > 100 && clusters[i].size < 1000)
			{
				for(int j=0; j<clusters[i].events_within.size(); j++)
				{
					output << clusters[i].events_within[j]->timestamp << " " <<clusters[i].events_within[j]->burst_index<<std::endl;
				}
			}
		}

		output.close();
		*/
		/*
		output.open("fraction_activated_25_100.txt");
		for(int i=0; i<n_clusters; i++)
		{
			if(clusters[i].size > 24 && clusters[i].size < 100)
			{
				for(int i=0; i<clusters[i].fraction_active_users.size(); i++)
				{
					output << clusters[i].fraction_active_timestamp[i] << " " << clusters[i].fraction_active_users[i] <<std::endl;
				}
			}
		}
		output.close();
		*/

	//TOTAL ACTIVITY
	output.open("total_activity.txt");
	for(int i=0; i<n_clusters; i++)
	{
		if(clusters[i].size > 24)
		{
			output << clusters[i].size << " " << (double)(clusters[i].events_within.size() + clusters[i].events_outgoing.size() + clusters[i].events_ingoing.size())/clusters[i].n_active_users <<std::endl;
		}
	}
	output.close();

/*
	//TOTAL ACTIVITY BY TYPE
	output.open("total_activity_within.txt");
	output_2.open("total_activity_outgoing.txt");
	output_3.open("total_activity_ingoing.txt");

	for(int i=0; i<n_clusters; i++)
	{
		if(clusters[i].size > 24)
		{
			output << clusters[i].size << " " << (double)clusters[i].events_within.size()/clusters[i].n_active_users <<std::endl;
			output_2 << clusters[i].size << " " << (double)clusters[i].events_outgoing.size()/clusters[i].n_active_users <<std::endl;
			output_3 << clusters[i].size << " " << (double)clusters[i].events_ingoing.size()/clusters[i].n_active_users <<std::endl;
		}
	}

	output.close();
	output_2.close();
	output_3.close();
*/

/*	//FRACTION ACTIVATED NODES
	output.open("fraction_activated_nodes.txt");
	for(int i=0; i<n_clusters; i++)
	{
		if(clusters[i].size > 24)
		{
			output << clusters[i].size << " " << clusters[i].fraction_active <<std::endl;
		}
	}
	output.close();

	//AVERAGE NODE ACTIVATION
	output.open("average_node_activation.txt");
	for(int i=0; i<n_clusters; i++)
	{
		if(clusters[i].size > 24)
		{
			output << clusters[i].size << " " << clusters[i].average_activity <<std::endl;
		}
	}
	output.close();
*/





	return 0;













}





































