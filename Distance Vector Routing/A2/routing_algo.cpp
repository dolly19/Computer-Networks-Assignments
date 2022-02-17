#include "node.h"
#include <iostream>

using namespace std;
bool link_btw_BC = false;

void printRT(vector<RoutingNode*> nd){

	for (int i = 0; i < nd.size(); i++) 
	  nd[i]->printTable();
	
}

void routingHelper(vector<RoutingNode*> nd){

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

			int tempcost = routingTable[i].tbl[j].cost;
			int normcost = ((*nd[i]).getTable()).tbl[j].cost;

  			if((normHop==tempHop) && (normInterface == tempInterface) && (normcost == tempcost))
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

	if(link_btw_BC)
		printRT(nd);

  }
  

}

void routingAlgo(vector<RoutingNode*> nd){

  	routingHelper(nd);
	printRT(nd);
  	(*nd[1]).updateTblEntry("10.0.1.3");
	(*nd[2]).updateTblEntry("10.0.1.23");
	printRT(nd);
	link_btw_BC = true;
	routingHelper(nd);
	printRT(nd);

}

void RoutingNode::recvMsg(RouteMsg *msg) {

  if(link_btw_BC && (((*msg).from) == "10.0.1.3" || ((*msg).from) == "10.0.1.23"))
	  return;


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

			if(isMyInterface(mytbl.tbl[i].nexthop))
				break;
				
  			if(newCost < origCost || mytbl.tbl[i].nexthop == (*msg).from )
  			{
  				
				mytbl.tbl[i].nexthop=(*msg).from;
				mytbl.tbl[i].cost=min(16,newCost);
				mytbl.tbl[i].ip_interface=(*msg).recvip;  
				break;
				
  			}
  		}
  	}
  	if(flag)
  		mytbl.tbl.pop_back();
  	
  	j++;
  }
}
  





