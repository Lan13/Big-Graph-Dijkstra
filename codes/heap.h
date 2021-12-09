#pragma once

#include"graph.h"
using namespace std;

typedef struct Node
{
	int dis;
	int to;
}Node, * Heap;
void push(Heap& heap, int dis, int to, int& size);
void push_opt(Heap& heap, int dis, int to, int& size,int* &pos);
void pop(Heap& heap, int& size);
void pop_opt(Heap& heap, int& size, int*& pos);
Node top(Heap& heap);
void dijkstra_heap(ALGraph g, int src, int dst);
void dijkstra_heap_opt(ALGraph g, int src, int dst);