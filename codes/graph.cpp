#include"graph.h"
#include"heap.h"
void createGraph(ALGraph& g, std::string filename)
{
	Info t;
	g.vertices = new AdjList[27000000];
	for (int i = 0; i < 27000000; i++)
	{
		g.vertices[i].data = i;
		g.vertices[i].firstarc = NULL;
	}
	int max_vex = 0, max_edge = 0;
	ifstream input_file(filename, ios::binary);
	if (input_file)
	{
		ArcNode* temp;
		while (input_file.read((char*)&t, sizeof(t)))
		{
			max_edge++;
			max_vex = max(max_vex, t.src);
			max_vex = max(max_vex, t.dst);
			if (t.src != t.dst)
			{
				ArcNode* next = new ArcNode;
				next->adjvex = t.dst;
				next->nextarc = NULL;
				next->value = t.dis;
				if (g.vertices[t.src].firstarc == NULL)
					g.vertices[t.src].firstarc = next;
				else
				{
					temp = g.vertices[t.src].firstarc;
					while (temp->nextarc)
						temp = temp->nextarc;
					temp->nextarc = next;
				}
			}
		}
	}
	g.vexnum = max_vex;
	g.edgenum = max_edge;
}
void dijkstra(ALGraph g, int src, int dst)
{
	bool* visit = new bool[g.vexnum + 1];
	int* dist = new int[g.vexnum + 1];
	std::string* pre = new std::string[g.vexnum + 1];
	for (int i = 1; i < g.vexnum; i++)
	{
		dist[i] = INT_MAX;
		visit[i] = false;
	}
	visit[src] = true;
	dist[src] = 0;
	pre[src] = to_string(src);
	int head = src;
	ArcNode* temp = new ArcNode;
	for (int j = 1; j <= g.vexnum; j++)
	{
		temp = g.vertices[head].firstarc;
		while (temp)
		{
			int to = temp->adjvex;
			if (dist[to] > dist[head] + temp->value)
			{
				dist[to] = dist[head] + temp->value;
				pre[to] = to_string(head);
			}
			temp = temp->nextarc;
		}
		int min = INT_MAX;
		int index = head;
		for (int i = 1; i <= g.vexnum; i++)
		{
			if (!visit[i] && dist[i] < min)
			{
				min = dist[i];
				index = i;
			}
		}
		visit[index] = true;
		head = index;
	}
	cout << dist[dst] << endl;
	findPath(g, pre, src, dst);
	delete[] visit;
	delete[] dist;
	delete[] pre;
	delete[] g.vertices;
}
void dijkstra_queue(ALGraph g, int src, int dst)
{
	std::priority_queue<p, vector<p>, greater<p> > heap;
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
	heap.push(make_pair(0, src));
	ArcNode* temp = new ArcNode;
	while (heap.size())
	{
		int head = heap.top().second;
		heap.pop();
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
				heap.push(make_pair(dist[to], to));
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
void findPath(ALGraph g, std::string* pre, int src, int dst)
{
	int index = dst;
	int pre_int;
	while (index != src)
	{
		pre_int = atoi(pre[index].c_str());
		cout << to_string(index) << "<-";
		index = pre_int;
	}
	cout << pre[src];
}
