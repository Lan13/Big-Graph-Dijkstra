# 导航实验报告

## 实验要求

### 问题描述

在所给的数据集上建立图结果（邻接矩阵或邻接表），并在建立的图结构上自行实现Dijkstra算法求解任意两点之间的最短路径。实验目的为帮助大家熟悉并掌握图的建立算法和Dijkstra求图上最短路径算法，了解Dijkstra算法的改进方法，掌握时间复杂度的分析方法并且去分析对比验证不同时间复杂度的Dijkstra算法的时间开销，了解稀疏的 图结构的压缩存储方法。

### 基本要求

1. 正确实现朴素的Dijkstra算法（时间复杂度为$O(V^2)$)且正确预处理数据为二进制文件并读入，并且通过助教验收时给出的测试样例。（75%的总分）
2. 在1.的基础上，成功将Dijkstra算法的时间复杂度降到$O(ElogV)$--- [参考平均运行时间：100s之内]（4%的总分）
3. 在1.的基础上，成功将预处理的二进制文件的大小压缩为$O(2E+V)$大小（5%的总分）
4. 在2.的基础上略作改动，可以将Dijkstra算法的时间复杂度又降低一个常数因子---[参考平均运行时间：25s之内]（3%的总分）
5. 自由发散部分（上限3%的总分）：本实验鼓励你对Dijkstra算法做一些额外的探索与尝试，不设严格的限制，可以根据自己的兴趣和思考取自行选择方向和内容，不论做到什么程度，助教给综合根据思路、难度、工作量来打分。实验鼓励做大胆的、创新的尝试。
6. 实验报告（10%的总分）

### 测试数据

数据集为txt文档，包含若干行数据，每行数据形式如下：

**src dst distance**

其中src表示源点，dst表示目标点，distance表示源点到目标点有直接连接的路径且路径长度为distance

输出要求：

1. 最短路径的长度：distance
2. src到dst的一条最短路径，例如src->p1->p2->p3->...->dst（逆序输出路径也对）

## 设计思路

首先是对预处理的设计思路：已知每行有且只有三个整数，所以我们用`struck.pack`对其进行封装再写入。

```python
bytes = struct.pack('iii',src,dst,dis)
file.write(bytes)
```

其次是对预处理文件的压缩处理：由于本实验数据集中的图算得上是稀疏图，对于稀疏图我们采取压缩稀疏行(compressed spare row, CSR)的方法进行压缩。采用CSR的存储格式，可以将储存大小从$O(3E)$改到$O(2E+V)$。具体的设计为：

CSR中存在列索引和行偏移。CSR将稀疏邻接矩阵的行进行压缩，依此将每一行中的非零元素对应的列的索引号存储到列索引当中，即列索引中是按照顶点序号的顺序存储以各个顶点为起点的每条有向边的终点序号，而行偏移中存储每一行的第一个非零元素在列索引中的位置偏移，即每个顶点第一个外邻顶点在列索引中的位置偏移。

示例：

![](C:\Users\蓝\Desktop\电梯图片\CSR.png)

算法包括朴素的Dijkstra和堆优化的Dijkstra以及一些小优化。这个放在代码讲解中说明。

## 关键代码讲解

下面是创建图的代码讲解：

```c++
void createGraph(ALGraph& g, std::string filename)
{
	Info t;		//采取结构体的方式，来一次读入12位
	g.vertices = new AdjList[26000000];
	for (int i = 0; i < 26000000; i++)
	{
		g.vertices[i].data = i;
		g.vertices[i].firstarc = NULL;
	}
    //用最大顶点序号默认为顶点总数，减少内存分配
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
			if (t.src != t.dst)		//去除自环
			{
				ArcNode* next = new ArcNode;
				next->adjvex = t.dst;
				next->nextarc = NULL;
				next->value = t.dis;
				if (g.vertices[t.src].firstarc == NULL)	//如果当前src结点没有邻接点
					g.vertices[t.src].firstarc = next;
				else	//否则插入到邻接表的最后面
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
```

朴素的Dijkstra算法：在图论书中，Dijkstra算法是这样定义的

1. 任给$u,v\in V(G)$，若$uv\notin E(G)$，令$\omega(uv)=\infty$
2. 令$d(u_0)=0,l(u_0)=u_0$；任给$u\in V(G), u\neq u_0, d(u)=\infty,l(u)=*; S_0=\{u_0\};i=0$
3. 对任给$u\in V(G)-S_i$，若$d(u_i)+\omega(u_iu)<d(u)$，则令$d(u)\leftarrow d(u_i)+\omega(u_iu)$，且令$l(u)=u_i$
4. 选出$u_{i+1}\in V(G)-S_i$，使得$d(u_{i+1})=min_{u\in V(G)-S_i}d(u)$，令$S_{i+1}=S_i\cup\{u_{i+1}\}$
5. 若$i=\nu(G)-1$，算法停止；否则，令$i\leftarrow i+1$，转3.

其中，$l(u)$代表$u$的前继顶点，而$S_i$记录算法到第$i$次迭代时，已经求出的最终的最短路径的顶点集合。

将其转化为代码实现，就是如下：

```c++
void dijkstra(ALGraph g, int src, int dst)
{
	bool* visit = new bool[g.vexnum + 1];	//判断顶点是否已经找到最短路，即看作finish
	int* dist = new int[g.vexnum + 1];		//记录顶点到src的最短路的权值
	std::string* pre = new std::string[g.vexnum + 1];	//记录每个顶点的前继顶点
	for (int i = 1; i < g.vexnum; i++)	//初始化
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
		temp = g.vertices[head].firstarc;	//遍历当前head结点所有的相邻的边，进行松弛操作
		while (temp)
		{
			int to = temp->adjvex;
			if (dist[to] > dist[head] + temp->value)	//松弛
			{
				dist[to] = dist[head] + temp->value;
				pre[to] = to_string(head);			//在松弛的过程更新前继顶点
			}
			temp = temp->nextarc;
		}
		int min = INT_MAX;
		int index = head;
		for (int i = 1; i <= g.vexnum; i++)
		{
			if (!visit[i] && dist[i] < min)	//从未找到最短路径的顶点中挑选出最短路径，并将其归入到已找到最短路径的顶点集
			{
				min = dist[i];
				index = i;
			}
		}
		visit[index] = true;
		head = index;		//对新并入顶点的顶点集进行新一轮的松弛和搜索
	}
	cout << dist[dst] << endl;
	findPath(g, pre, src, dst);
	delete[] visit;
	delete[] dist;
	delete[] pre;
	delete[] g.vertices;
}
```

可以发现，每次从未找到最短路的顶点集中寻找最短路的顶点并入已找到最短路的顶点集所花费的时间为$O(V)$。最终其时间复杂度就为$O(V^2)$。我们可以对其进行优化：方法是采用小根堆的方式进行存储，这样可以将花费时间优化到$O(logV)$。

- 小根堆的插入：

  假设有一个堆是这样：

  ![](C:\Users\蓝\Desktop\电梯图片\堆1.png)

  这时候我要插入一个元素为0的结点，那么我先将其插入到堆底：

  ![](C:\Users\蓝\Desktop\电梯图片\堆2.png)

  发现不太符合小根堆的性质，那么我们将其向上交换，直到满足小根堆的性质：

  <center class="half">
      <img src="C:\Users\蓝\Desktop\电梯图片\堆换1.png" width="300"/>
  	<img src="C:\Users\蓝\Desktop\电梯图片\堆换2.png" width="300"/>
      <img src="C:\Users\蓝\Desktop\电梯图片\堆换3.png" width="300"/>
  </center>

  事实上堆的插入就是把新的元素放到堆底，然后检查它是否符合堆的性质，如果符合就结束操作，如果不符合，那就和它的父亲交换一下，一直交换交换交换，直到符合堆的性质，那么插入就完成了。

- 小根堆的删除

  还是以上面这个为例子，假设我现在想要删除元素为0这个结点，首先我先将它和最后一个结点进行交换，将其删除：

  ![](C:\Users\蓝\Desktop\电梯图片\删1.png)

  然后发现不满足小根堆性质，将元素5这个结点，在与其两个孩子中关键字较小的进行交换，直到满足小根堆性质：

  <center class="half">
      <img src="C:\Users\蓝\Desktop\电梯图片\删2.png" width="300"/>
  	<img src="C:\Users\蓝\Desktop\电梯图片\删3.png" width="300"/>
  </center>

那么，小根堆的插入和删除操作的代码实现如下：

```c++
void push(Heap& heap, int dis, int to, int& size)
{
	size++;
	heap[size].dis = dis;		//插入到最后面
	heap[size].to = to;
	int index = size;
	while (index)
	{
		int next = index >> 1;
		if (heap[next].dis > heap[index].dis)	//往上交换
			swap(heap[next], heap[index]);		
		else break;				//已经满足小根堆性质
		index = next;
	}
}

void pop(Heap& heap, int& size)
{
	swap(heap[size], heap[1]);		//先交换堆顶元素和最后一个元素
	size--;
	int index = 1;
	while ((index << 1) <= size)	//当其存在孩子时
	{
		int next = index << 1;
		if (next + 1 <= size && heap[next + 1].dis < heap[next].dis)
			next++;				//选择左右结点中关键字较小的结点
		if (heap[next].dis < heap[index].dis)	//向下交换
			swap(heap[index], heap[next]);		
		else
			break;		//已经满足小根堆性质
		index = next;
	}
}
```

小根堆在Dijkstra算法中优化的方式如下：

```c++
void dijkstra_heap(ALGraph g, int src, int dst)
{
	int size = 0;		//堆的元素个数
	Heap heap = new Node[g.vexnum + 1];
	int* dist = new int[g.vexnum + 1];
	std::string* pre = new std::string[g.vexnum + 1];
	for (int i = 1; i <= g.vexnum; i++)
	{
		dist[i] = INT_MAX;
	}
	dist[src] = 0;
	pre[src] = to_string(src);
	push(heap, dist[src], src, size);
	ArcNode* temp = new ArcNode;
	Node topnode;
	while (size)
	{
		topnode = top(heap);	//获取最小最短路的这个函数实际上只需要花费O(1)
		pop(heap, size);		//主要是删除和插入时，花费需要O(logV)
		int head = topnode.to;
		if (head == dst)
			break;
		temp = g.vertices[head].firstarc;
		while (temp)
		{
			int to = temp->adjvex;
			if (dist[to] > dist[head] + temp->value)	//松弛
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
	delete[] dist;
	delete[] pre;
	delete[] g.vertices;
}
```

由于会出现这样的情况：每次判断是否需要进行松弛操作时，都能满足需要进行松弛操作，那么，在这个堆中，就会存储至多$O(E)$的信息，加上小根堆的操作为$O(logV)$，所以这个优化的方法是$O(ElogV)$的。

接下来，对其再进行一些小优化，可以将$O(ElogV)$降到$O(VlogV)$。

下面就只讲思路，具体实现可以看代码，由于大体框架和上述程序一样，所以只把不同的地方讲解一下：

- 采用一个` bool visit[]`数组，判断从最小堆获取的顶点是否已经在前面完成操作了，如果是，则跳过。

  ```c++
  while(size)
  {
      topnode = top(heap);
  	pop(heap, size);
  	int head = topnode.to;
  	if (visit[head])
  		continue;
      ......
  }
  ```

  为什么这样可以从$O(ElogV)$降到$O(VlogV)$呢？是因为，原来的堆里面有$O(E)$个元素，但是其中有很多个重复结点的元素。除了第一个最小距离的元素是有用的，其他的都是在之前操作插入的结点，如果将它出堆的话，对其进行松弛操作是没有任何意义的，原因是因为新的距离不可能会更小。可以简单看个例子：

  ![](C:\Users\蓝\Desktop\电梯图片\图1.png)

  如果A先对C进行入堆，再从B到C进行入堆，则我们可以认为现在堆的排序如下`[(20,B),(60,C),(100,C)]`，当`(60,C)`出堆后，将其置为已访问过的并进行松弛操作，而当`(100,C)`出堆时，发现这个松弛操作已经在前面实现完毕了，那么这时再进行就只是浪费时间，所以我们将其跳过。即这样就是相当于可以把堆里面重复的顶点信息删除，只取每个元素的最短距离做处理。这样虽然堆里面是$O(E)$个顶点，但实际上操作时只动了$O(V)$个顶点。

- 另一种思路就是从源头上处理：即直接维护堆为没有重复顶点信息的。这里我们使用一个索引，判断每个顶点在堆里面的位置，如果不在堆里面的话，则索引为0。当索引不为0的时候，入堆时直接修改相应索引位置的最短距离，然后进行上移并更改索引。这样就可以维护堆的大小为$O(V)$。

  ```c++
  int* pos = new int[g.vexnum + 1];	//每个顶点在堆里面的位置
  while (size)
  	{
  		......
  		while (temp)
  		{
  			int to = temp->adjvex;
  			if (dist[to] > dist[head] + temp->value)
  			{
  				dist[to] = dist[head] + temp->value;
  				pre[to] = to_string(head);
  				push_opt(heap, dist[to], to, size, pos);	//这里直接把pos作为一个参数
  			}
  			temp = temp->nextarc;
  		}
  	}
  ```
  
  其中堆的操作改为：
  
  ```c++
  void push_opt(Heap& heap, int dis, int to, int& size, int*& pos)
  {
  	if (pos[to] != 0)	//如果堆里面已经存在该顶点的信息
  	{
  		heap[pos[to]].dis = dis;	//直接更改最短路径，并且进行上移
  		int index = pos[to];
  		while (index)
  		{
  			int next = index >> 1;
  			if (heap[next].dis > heap[index].dis)
  			{
  				swap(heap[next], heap[index]);
  				swap(pos[heap[next].to], pos[heap[index].to]);	//记得交换索引位置
  			}
  			else break;
  			index = next;
  		}
  		return;
  	}
  	......	//同上
  }
  ```

CSR压缩算法：

前面给出了设计思路，所以这里我直接讲解代码：

```c++
	int* beg_pos = new int[max_vex + 1];
	int* csr = new int[max_edge + 1];
	int* dis = new int[max_edge + 1];
	for (int i = 1; i <= max_vex + 1; i++)
		beg_pos[i] = 0;
	beg_pos[1] = 1;		//从1开始作为索引
	int now_edge = 1;
	ArcNode* temp = new ArcNode;
	for (int i = 1; i <= g.vexnum; i++)
	{
		temp = g.vertices[i].firstarc;	//这里采取的邻接表的方式进行CSR压缩，本质和邻接矩阵是一样的
		while (temp)
		{
			int to = temp->adjvex;
			beg_pos[i + 1]++;		//通过计数的方式，来确定偏移量
			csr[now_edge] = to;
			dis[now_edge] = temp->value;
			now_edge++;
			temp = temp->nextarc;
		}
		beg_pos[i + 1] += beg_pos[i];	//这一行的偏移量=本行边的个数+上一行的偏移量
	}
	ofstream out_file(outpath, ios::binary);
	if (out_file)
	{
		out_file.write((char*)&max_vex, sizeof(max_vex));		//把顶点数和边数先写入，便于分配空间大小
		out_file.write((char*)&max_edge, sizeof(max_edge));
		for (int i = 1; i <= max_vex + 1; i++)
			out_file.write((char*)&beg_pos[i], sizeof(beg_pos[i]));
		for (int i = 1; i <= max_edge; i++)
			out_file.write((char*)&csr[i], sizeof(csr[i]));
		for (int i = 1; i <= max_edge; i++)
			out_file.write((char*)&dis[i], sizeof(dis[i]));
	}
	out_file.close();
```

用CSR压缩的图信息来构建邻接表：

```c++
		ArcNode* temp;		//本质上和原始的构建邻接表方式是一样的
		for (int i = 1; i <= g.vexnum; i++)
		{
			for (int j = beg_pos[i]; j <= beg_pos[i + 1] - 1; j++)	//通过行偏移量来确定哪些顶点是当前顶点的邻点
			{
				ArcNode* next = new ArcNode;
				next->adjvex = csr[j];
				next->value = dis[j];
				next->nextarc = NULL;
				if (g.vertices[i].firstarc == NULL)
					g.vertices[i].firstarc = next;
				else
				{
					temp = g.vertices[i].firstarc;
					while (temp->nextarc)
						temp = temp->nextarc;
					temp->nextarc = next;
				}
			}
		}
```

## 调试分析

实验中遇见的问题有：

- 采取std::string path 存储每一个顶点的最短路径会导致内存占满，这个占用的空间资源太大。所以我改为了存储当前顶点最短路径的前继顶点，这样可以大大减少空间资源占用。
- 前继顶点的修改方式错误。原来我在每一次入栈的时候修改，最后我通过图论书上的指引，在每一次松弛的过程中修改前继顶点，这样的操作才是正确的。
- 找std::string path的时候，采用递归的方法，会导致函数栈满，同样会占用大量的空间资源，导致内存不够使用。最后我改为使用遍历的方法寻找。

## 代码测试



## 实验总结



## 附录

`main.py`是用来做预处理的文件，由于和助教说过用python做处理很简单，以为可以使用…。便使用了python进行预处理。

