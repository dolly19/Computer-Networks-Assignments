#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd){

	for (int i = 0; i < nd.size(); i++) 
	  nd[i]->printTable();
	
}

void routingAlgo(vector<RoutingNode*> nd){

  int count;
  bool 	converge;
  vector<struct routingtbl> routingTable;
  count=nd.size();
  converge=false;
  
  while(1)
  {
  	converge=true;
  	routingTable.clear();

  	for(int i=0; i<count; i++)
  		routingTable.push_back((*nd[i]).getTable());
  
  	for(int i=0;i<count;i++)
  		(*nd[i]).sendMsg();
  		
  	for(int i=0;i<count;i++)
  	{	

		int tempSize = routingTable[i].tbl.size();
		int normSize = ((*nd[i]).getTable()).tbl.size();
  		if(normSize!=tempSize)
  		{
  			converge=false;
  			break;
  		}

  		for(int j=0; j<normSize; j++)
  		{	
			string tempHop = routingTable[i].tbl[j].nexthop;
			string normHop = ((*nd[i]).getTable()).tbl[j].nexthop;

			string tempInterface = routingTable[i].tbl[j].ip_interface;
			string normInterface = ((*nd[i]).getTable()).tbl[j].ip_interface;
  			if((normHop==tempHop) && (normInterface == tempInterface))
			  continue;
			else
  			{
  				converge=false;
  				break;
  			}
  		}
  	}
  	if(converge)
  		break;
  }
	/*Print routing table entries after routing algo converges */
  	printRT(nd);
}

void RoutingNode::recvMsg(RouteMsg *msg) {

  int num =0;
  int k=0;
  int j=0;
  bool flag = false;

  num = mytbl.tbl.size();
  k = (*(*msg).mytbl).tbl.size();
  
  while(j<k)
  {

  	mytbl.tbl.push_back((*(*msg).mytbl).tbl[j]);
  	mytbl.tbl[mytbl.tbl.size()-1].cost++;
  	mytbl.tbl[mytbl.tbl.size()-1].nexthop=(*msg).from;
  	mytbl.tbl[mytbl.tbl.size()-1].ip_interface=(*msg).recvip;

  	flag=false;

  	for(int i=0;i<num;i++){
		
		string tempDstip = (*(*msg).mytbl).tbl[j].dstip;
		string normDstip = mytbl.tbl[i].dstip;
  		if(normDstip == tempDstip)
  		{
  			flag=true;
			int newCost = (*(*msg).mytbl).tbl[j].cost+1;
			int origCost = mytbl.tbl[i].cost;
  			if(newCost < origCost)
  			{
  				if(isMyInterface((*(*msg).mytbl).tbl[j].nexthop))
  					break;
  				
  				else
  				{
  					mytbl.tbl[i].nexthop=(*msg).from;
  					mytbl.tbl[i].ip_interface=(*msg).recvip;
  					mytbl.tbl[i].cost=newCost;
  					break;
  				}
  			}
  		}
  	}
  	if(flag)
  		mytbl.tbl.pop_back();
  	
  	j++;
  }
}
  





