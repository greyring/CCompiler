#ifndef LIVENESS_H
#define LIVENESS_H

#include "graph.h"
#include "temp.h"
#include "flowgraph.h"

typedef struct Live_moveList_ *Live_moveList;

struct Live_moveList_{
    G_node src, dst;
    Live_moveList tail;
};

//构建出来的活跃性图
struct Live_graph{
    G_graph graph;
    Live_moveList moves;
};

//构建出来的冲突图
struct interference{
    int n; // 变量个数，max=50
    int matrix[50][50];
    Temp_tempList templist; //按顺序记录matrix代表的变量
};

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail);

//给定冲突图的节点n，返回它表示的临时变量，每个节点的info域指向一个Temp_temp
Temp_temp Live_gtemp(G_node n);

//构建活跃性图
struct Live_graph Live_liveness(G_graph flow);

//构建冲突图
struct interference interference_graph(struct Live_graph livegraph);


#endif