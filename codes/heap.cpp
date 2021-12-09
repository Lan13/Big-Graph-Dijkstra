#include"heap.h"

void push(Heap& heap, int dis, int to, int& size)
{
	size++;
	heap[size].dis = dis;
	heap[size].to = to;
	int index = size;
	while (index)
	{
		int next = index >> 1;
		if (heap[next].dis > heap[index].dis)
			swap(heap[next], heap[index]);
		else break;
		index = next;
	}
}

void pop(Heap& heap, int& size)
{
	swap(heap[size], heap[1]);
	size--;
	int index = 1;
	while ((index << 1) <= size)
	{
		int next = index << 1;
		if (next + 1 <= size && heap[next + 1].dis < heap[next].dis)
			next++;				//选择左右结点的较小结点
		if (heap[next].dis < heap[index].dis)
			swap(heap[index], heap[next]);
		else
			break;
		index = next;
	}
}

Node top(Heap& heap)
{
	return heap[1];
}

void dijkstra_heap(ALGraph g, int src, int dst)
{
	int size = 0;
	Heap heap = new Node[g.vexnum + 1];
	bool* visit = new bool[g.vexnum + 1];
	int* dist = new int[g.vexnum + 1];
	std::string* pre = new std::string[g.vexnum + 1];
	for (int i = 1; i <= g.vexnum; i++)
	{
		dist[i] = INT_MAX;
		visit[i] = false;
	}
	dist[src] = 0;
	pre[src] = to_string(src);
	push(heap, dist[src], src, size);
	ArcNode* temp = new ArcNode;
	Node topnode;
	while (size)
	{
		topnode = top(heap);
		pop(heap, size);
		int head = topnode.to;
		if (visit[head])
			continue;
		if (head == dst)
			break;
		visit[head] = true;
		temp = g.vertices[head].firstarc;
		while (temp)
		{
			int to = temp->adjvex;
			if (dist[to] > dist[head] + temp->value)
			{
				dist[to] = dist[head] + temp->value;
				pre[to] = to_string(head);
				push(heap, dist[to], to, size);
			}
			temp = temp->nextarc;
		}
	}
	cout << dist[dst] << endl;
	findPath(g, pre, src, dst);
	delete[] visit;
	delete[] dist;
	delete[] pre;
	delete[] g.vertices;
}

void dijkstra_heap_opt(ALGraph g, int src, int dst)
{
	int size = 0;
	Heap heap = new Node[g.vexnum + 1];
	int* pos = new int[g.vexnum + 1];
	int* dist = new int[g.vexnum + 1];
	std::string* pre = new std::string[g.vexnum + 1];
	for (int i = 1; i <= g.vexnum; i++)
	{
		dist[i] = INT_MAX;
		pos[i] = 0;
	}
	dist[src] = 0;
	pre[src] = to_string(src);
	push_opt(heap, dist[src], src, size, pos);
	ArcNode* temp = new ArcNode;
	Node topnode;
	while (size)
	{
		topnode = top(heap);
		pop_opt(heap, size, pos);
		int head = topnode.to;
		if (head == dst)
			break;
		temp = g.vertices[head].firstarc;
		while (temp)
		{
			int to = temp->adjvex;
			if (dist[to] > dist[head] + temp->value)
			{
				dist[to] = dist[head] + temp->value;
				pre[to] = to_string(head);
				push_opt(heap, dist[to], to, size, pos);
			}
			temp = temp->nextarc;
		}
	}
	cout << dist[dst] << endl;
	findPath(g, pre, src, dst);
	delete[] pos;
	delete[] dist;
	delete[] pre;
	delete[] g.vertices;
}

void push_opt(Heap& heap, int dis, int to, int& size, int*& pos)
{
	if (pos[to] != 0)
	{
		heap[pos[to]].dis = dis;
		int index = pos[to];
		while (index)
		{
			int next = index >> 1;
			if (heap[next].dis > heap[index].dis)
			{
				swap(heap[next], heap[index]);
				swap(pos[heap[next].to], pos[heap[index].to]);
			}
			else break;
			index = next;
		}
		return;
	}
	size++;
	heap[size].dis = dis;
	heap[size].to = to;
	pos[to] = size;
	int index = size;
	while (index)
	{
		int next = index >> 1;
		if (heap[next].dis > heap[index].dis)
		{
			swap(heap[next], heap[index]);
			swap(pos[heap[next].to], pos[heap[index].to]);
		}
		else break;
		index = next;
	}
}

void pop_opt(Heap& heap, int& size, int*& pos)
{
	swap(heap[size], heap[1]);
	swap(pos[heap[size].to], pos[heap[1].to]);
//	pos[heap[size].to] = 0;
	size--;
	int index = 1;
	while ((index << 1) <= size)
	{
		int next = index << 1;
		if (next + 1 <= size && heap[next + 1].dis < heap[next].dis)
			next++;				//选择左右结点的较小结点
		if (heap[next].dis < heap[index].dis)
		{
			swap(heap[index], heap[next]);
			swap(pos[heap[index].to], pos[heap[next].to]);
		}
		else
			break;
		index = next;
	}
}