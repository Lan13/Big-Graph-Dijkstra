#include"graph.h"
#include"heap.h"
int main()
{
	ALGraph g;
	createGraph(g, "D:\\Tencent Files\\1910992493\\lab3DS2021\\output.txt");
	//createGraph(g, "D:\\vs2022 code\\navigate\\navigate\\newout.txt");
	//dijkstra(g, 54321, 123456);
	//dijkstra_queue(g, 54321, 123456);
	//dijkstra_queue(g, 1000000, 2000000);
	//dijkstra_queue(g, 3141592, 2718281);
	//dijkstra_heap(g, 54321, 123456);
	//dijkstra_heap(g, 1000000, 2000000);
	//dijkstra_heap(g, 3141592, 2718281);
	//dijkstra_heap_opt(g, 54321, 123456);
	//dijkstra_heap_opt(g, 1000000, 2000000);
	dijkstra_heap_opt(g, 3141592, 2718281);
	return 0;
}