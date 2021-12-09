#pragma once
#include<iostream>
#include<algorithm>
#include<cmath>
#include<string>
#include<stdlib.h>
#include<climits>
#include<queue>
#include<fstream>
#include<ostream>
#include<sstream>
#include<vector>

using namespace std;

typedef struct ArcNode
{
	int adjvex;
	struct ArcNode* nextarc;
	int value;
}ArCNode;
typedef struct VNode
{
	int data;
	ArcNode* firstarc;
}VNode, AdjList;
typedef struct Graph
{
	int vexnum;
	int edgenum;
	AdjList* vertices;
}ALGraph;
struct Info
{
	int src;
	int dst;
	int dis;
};
typedef pair<int, int> p;

void createGraph(ALGraph& g, std::string filname);
void dijkstra(ALGraph g, int src, int dst);
void dijkstra_queue(ALGraph g, int src, int dst);
//void dijkstra_heap(ALGraph g, int src, int dst);
void findPath(ALGraph g, std::string* pre, int src, int dst);
