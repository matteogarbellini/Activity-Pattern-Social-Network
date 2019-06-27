//Garbellini Matteo
//Dept. of Physics
//University of Milan
//Project: Networks and Complex Structures
//Latest working build: May 30th 2019

//LOG
//May 29th		first implementation
//May 30th 		implemented: events within cluster, edges within cluster, edges ingoing/outgoing cluster
//Jun 13th		fixing segmentation fault error in structs
//Jun 15th 		spike trains: local variation implementation



#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>

struct EVENT;
struct NODE
{

	NODE(int ID)
	{
		node_ID = ID;
		is_active=0;
		intertime=0;
		n_activations=0;
	}

	int node_ID;
	int n_followers;
	std::vector<NODE*> follower;
	std::vector<NODE*> following;
	int n_following;
	int cluster_ID;
	int is_active;
	float intertime;
	int n_activations;
	
};

struct EVENT
{
	EVENT(int t, int time)
	{
		type = t;
		timestamp = time;
	}

	int type;
	int timestamp;
	int event_ID;
	NODE* active_user;
	NODE* passive_user;
};

struct CLUSTER
{
	CLUSTER(int cluster)
	{
		cluster_ID = cluster;
		n_edges_within=0;
		n_edges_ingoing=0;
		n_edges_outgoing=0;
		n_events_total=0;
		n_active_users = 0;
		average_activity=0;
		for(int i=1; i<4; i++)
		{
			n_events_within[i] = 0;
			n_ingoing_events[i] = 0;
			n_outgoing_events[i] = 0;
		}
	}

	int cluster_ID;
	int cluster_size;
	std::vector<NODE*> nodes_in_cluster;
	
	int n_events_within[4];
	int n_ingoing_events[4];
	int n_outgoing_events[4];
	int n_events_total;
	
	int n_edges_within;
	int n_edges_ingoing;
	int n_edges_outgoing;

	float average_intertime;
	float average_activity;

	int n_active_users; //active users include users that have associated a passive event (e.g. being retweeted)
};

struct INTERTIME
{
	INTERTIME(int)
	{
		av_intertime=0;
		n_activity=0;
		is_active=0;
		is_spike_train=0;
		local_variation=0;
	}
	int node_ID;
	int is_active;
	int is_spike_train;
	NODE* link_to_node;
	float av_intertime;
	int n_activity;
	std::vector<EVENT*> interactions;

	float local_variation;

};


int main(int argc, char** argv)
{

//the flow follows the follwing scheme
//	1. read higgs-social-network and fill a std::vector<NODE> higgs-nodes //should be in ascending order, with none missing
//	2. read higgs-activity-time and fill std::vector<INTERACTION> higgs-activity
//  3. read clusterID_nodeID and fill std::vector<CLUSTER> higgs-clusters
//	4. at this point all information are loaded and calculations can be made


	std::ifstream input;
	std::ofstream output;

	std::vector<NODE> higgs_nodes;
	std::vector<EVENT> higgs_event;
	std::vector<CLUSTER> higgs_cluster;
	std::vector<INTERTIME> node_intertime;


	// // // READ USERS
	input.open("users.txt");
		int IDS;
		int n_nodes=0;
		NODE aux(0);
		higgs_nodes.push_back(aux);
		while(input.good())
		{
			//read nodes
			n_nodes++;
			input >> IDS;
			aux.node_ID = IDS;
			higgs_nodes.push_back(aux);		
		}
		input.close();	
	// // // End of READ USERS

	// // // READ FOLLOWING/FOLLOWERS
	input.open("following.txt");
		int user1, user2;
		while(input.good())
		{
			input >> user1 >> user2;
			higgs_nodes[user1].following.push_back(&higgs_nodes[user2]);	//following
			higgs_nodes[user2].follower.push_back(&higgs_nodes[user1]); 	//followers
		}
		input.close();


		for(int i=1; i<=n_nodes; i++)
		{
			higgs_nodes[i].n_following= higgs_nodes[i].following.size();
			higgs_nodes[i].n_followers= higgs_nodes[i].follower.size();
		}
	// // // End of FOLLOWING/FOLLOWERS

	// // // READ CLUSTER
	input.open("clusters.txt");
		int c;
		int n_clusters = 0;
		CLUSTER cluster(0);
		while(input.good())
		{
			input >> c;
			cluster.cluster_ID = c;
			higgs_cluster.push_back(cluster);
			n_clusters++;
		}
		input.close();
	// // // End of READ CLUSTER

	// // // NODE in CLUSTER
	input.open("node_clusters.txt");
		int n;
		NODE* node_to_push;
		while(input.good())
		{
			input >> n >> c;
			higgs_nodes[n].cluster_ID = c;
			node_to_push = &higgs_nodes[n];
			higgs_cluster[c].nodes_in_cluster.push_back(node_to_push);
		}

		for(int i=0; i<n_clusters; i++)
		{
			higgs_cluster[i].cluster_size = higgs_cluster[i].nodes_in_cluster.size();
		}
		input.close();
	// // // End of NODE in CLUSTER

	// // // READ INTERACTIONS
	input.open("interactions.txt");
		int time, event, n_events=0;
		EVENT interaction(0,0);
		while(input.good())
		{
			input >> user1 >> user2 >> time >> event;
			interaction.type = event;
			interaction.event_ID = n_events;
			interaction.timestamp = time;
			interaction.active_user = &higgs_nodes[user1];
			interaction.passive_user = &higgs_nodes[user2];
			higgs_event.push_back(interaction);
			
			n_events++;

		}
		input.close();
	// // // End of READ INTERACTIONS


	//EVENTS IN/OUT-GOING - CLUSTER
		for(int i=0; i<n_events; i++)
		{
			higgs_cluster[higgs_event[i].active_user->cluster_ID].n_outgoing_events[higgs_event[i].type]++;
			higgs_cluster[higgs_event[i].passive_user->cluster_ID].n_ingoing_events[higgs_event[i].type]++;
		}

	//EVENTS WITHIN CLUSTER
		for(int i=0; i<n_events; i++)
		{
			if(higgs_event[i].active_user->cluster_ID == higgs_event[i].passive_user->cluster_ID)
			{
				higgs_cluster[higgs_event[i].active_user->cluster_ID].n_events_within[higgs_event[i].type]++;
			}
		}

	//EDGES INGOING/OUTGOING CLUSTER
		input.open("following.txt");
		
		while(input.good())
		{
			input >> user1 >> user2;
			if(higgs_nodes[user1].cluster_ID != higgs_nodes[user2].cluster_ID)
			{
				higgs_cluster[higgs_nodes[user1].cluster_ID].n_edges_outgoing++;
				higgs_cluster[higgs_nodes[user2].cluster_ID].n_edges_ingoing++;
			}
			if(higgs_nodes[user1].cluster_ID == higgs_nodes[user2].cluster_ID)
			{
				higgs_cluster[higgs_nodes[user1].cluster_ID].n_edges_within++;
			}

		}
		input.close();

	//TOTAL EVENTS
		for(int i=0; i<n_clusters; i++)
		{
			for(int j=1; j<4; j++)
			{
				higgs_cluster[i].n_events_total =  	higgs_cluster[i].n_events_total + 
													higgs_cluster[i].n_events_within[j] + 
													higgs_cluster[i].n_outgoing_events[j];
			}
			
			if(higgs_cluster[i].n_events_total != 0)
			{
				//std::cout<<higgs_cluster[i].cluster_size<<" "<<higgs_cluster[i].n_events_total<<std::endl;
			}
		}

	//ACTIVE USERS
		int debug_active_users=0;
		input.open("interactions.txt");
		while(input.good())
		{
			input>> user1 >> user2;
			if(higgs_nodes[user1].is_active!=1)
			{
				higgs_nodes[user1].is_active=1;
				debug_active_users++;
				higgs_cluster[higgs_nodes[user1].cluster_ID].n_active_users++;
			}
			if(higgs_nodes[user2].is_active!=1)
			{
				higgs_nodes[user2].is_active=1;
				higgs_cluster[higgs_nodes[user2].cluster_ID].n_active_users++;
			}
		}

		input.close();
		//std::cout<<"ACTIVE USERS: "<<debug_active_users<<std::endl;

		INTERTIME inter_time(0);
		node_intertime.push_back(inter_time);
		for(int i=1; i<=n_nodes; i++)
		{	
				inter_time.node_ID = higgs_nodes[i].node_ID;
				node_intertime.push_back(inter_time);
		}

		for(int i=0; i<n_events; i++)
		{

			node_intertime[higgs_event[i].active_user->node_ID].interactions.push_back(&higgs_event[i]);
			node_intertime[higgs_event[i].active_user->node_ID].is_active = 1;
		}

	//INTERTIME
		int flag = 0;
		int previous;
		int timestamp1, timestamp2;
		for(int i=1; i<=n_nodes; i++)
		{
			if(node_intertime[i].is_active && node_intertime[i].interactions.size() > 1)
			{
				for(int j=0; j<node_intertime[i].interactions.size(); j++)
				{
					if(flag==0 && j==0)
					{
						timestamp1 = node_intertime[i].interactions[j]->timestamp;
						flag=1;
					}
					else if(!flag)
					{
						timestamp1 = previous;
						flag = 1;
					}
					if(flag)
					{
						timestamp2 = node_intertime[i].interactions[j]->timestamp;
						node_intertime[i].av_intertime += timestamp2 - timestamp1;
						previous = timestamp2;
						flag = 0;
					}
				}
				if(!node_intertime[i].av_intertime)
				{
					node_intertime[i].av_intertime = (double) node_intertime[i].av_intertime / (node_intertime[i].interactions.size()-1);
				}
				higgs_nodes[i].intertime = node_intertime[i].av_intertime;
				//std::cout<<higgs_nodes[i].intertime<<std::endl;
			}
		}

		//OUTPUT INTERTIME
		output.open("intertime_dist.txt");
		for(int i=1; i<=n_nodes; i++)
		{
			if(node_intertime[i].is_active && node_intertime[i].interactions.size()>1)
			{
				output<< node_intertime[i].av_intertime <<std::endl; 
			}
		}
		output.close();


		int intertime_active_users = 0;
		for(int i=0; i<n_clusters; i++)
		{
			for(int j=0; j<higgs_cluster[i].cluster_size; j++)
			{
				if(higgs_cluster[i].nodes_in_cluster[j]->is_active && node_intertime[higgs_cluster[i].nodes_in_cluster[j]->node_ID].interactions.size() > 1)
				{
					higgs_cluster[i].average_intertime += higgs_cluster[i].nodes_in_cluster[j]->intertime;
					intertime_active_users++;
				}
			}
			higgs_cluster[i].average_intertime = higgs_cluster[i].average_intertime / intertime_active_users;
			std::cout<<higgs_cluster[i].cluster_size<<" "<<higgs_cluster[i].average_intertime<<std::endl;;
			intertime_active_users = 0;

		}


	//LOCAL VARIATION
	//is spike trains?
		for (int i = 1; i <= n_nodes; ++i)
		{
			if(node_intertime[i].interactions.size()>2)
			{
				node_intertime[i].is_spike_train=1;
			}
		}
	
	//local variation
	float positive_variation, negative_variation;
	int skipped=0;
	int spike_train_nodes=0;
	float average_local_variation=0;

	for(int i=1; i<=n_nodes; i++)
	{
		if(node_intertime[i].is_spike_train)
		{
			for(int j=1; j<node_intertime[i].interactions.size()-1; j++)
			{
				negative_variation = node_intertime[i].interactions[j+1]->timestamp - 2* node_intertime[i].interactions[j]->timestamp + node_intertime[i].interactions[j-1]->timestamp;
				positive_variation  = node_intertime[i].interactions[j+1]->timestamp - node_intertime[i].interactions[j-1]->timestamp;
				if(negative_variation==0)
				{
					continue;
				}	
				else
				{
					node_intertime[i].local_variation +=  ((double)negative_variation/positive_variation)*((double)negative_variation/positive_variation);
				}
				
			}
			
			node_intertime[i].local_variation = (double) node_intertime[i].local_variation*3/(node_intertime[i].interactions.size() - 2);
			average_local_variation += node_intertime[i].local_variation;
			spike_train_nodes++;

			//DEBUG
			//std::cout<<node_intertime[i].local_variation<<std::endl;

		}
	}

	//std::cout<<"AVERAGE LOCAL VARIATION: "<<(double)average_local_variation/spike_train_nodes<<std::endl;
	//std::cout<<"SPIKE USERS: "<<spike_train_nodes<<std::endl;

	/*
	//OUTPUT by cluster
	for(int i=0; i<n_clusters; i++)
	{
		if(higgs_cluster[i].cluster_size >= 100 && higgs_cluster[i].cluster_size <1000)
		{
			for(int j=0; j<higgs_cluster[i].cluster_size; j++)
			{
				if(node_intertime[higgs_cluster[i].nodes_in_cluster[j]->node_ID].is_spike_train)
				{
					std::cout<< std::setprecision(2)<< node_intertime[higgs_cluster[i].nodes_in_cluster[j]->node_ID].local_variation<<std::endl;
				}
			}
		}
	}
	*/
	
	//OUTPUT OVERALL
	output.open("local_variation_all_nodes.txt");
	for(int i=1; i<=n_nodes; i++)
	{
		if(node_intertime[i].is_spike_train)
		{
			output<<std::setprecision(2)<<node_intertime[i].local_variation<<std::endl;
		} 
	}
	output.close();
	



return 0;

}