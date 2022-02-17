#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
	for (int i = 0; i < nd.size(); i++) {
	  nd[i]->printTable();
	}

}

void routingAlgo(vector<RoutingNode*> nd){
  	
	int num=nd.size();
	while(num--){

		for(int i=0;i<nd.size();i++)
			nd[i]->sendMsg();

	}

	/*Print routing table entries after routing algo converges */
  	printRT(nd);
}

void RoutingNode::recvMsg(RouteMsg *msg) {

  for(int i=0;i<msg->mytbl->tbl.size();i++)
	{
		int cnt=0;
		int da;

		for(int j=0;j<mytbl.tbl.size();j++)
		{
			if(mytbl.tbl[j].dstip==msg->mytbl->tbl[i].dstip)
			{
				da =j;
				cnt++;
				break;
			}
		}
		if(cnt==0){

			RoutingEntry NewRoute;
			NewRoute.ip_interface= msg->recvip;
			NewRoute.nexthop=msg->from;
			NewRoute.dstip=msg->mytbl->tbl[i].dstip;

			if(msg->from==NewRoute.dstip)
				NewRoute.cost=1;
			
			else
				NewRoute.cost=1+msg->mytbl->tbl[i].cost;

			mytbl.tbl.push_back(NewRoute);
		}
		else 
		{
			if(mytbl.tbl[da].cost > msg->mytbl->tbl[i].cost+1)
				mytbl.tbl[da].cost=msg->mytbl->tbl[i].cost+1;
		
		}

	}
}

