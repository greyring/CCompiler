/*
 * flowgraph.h - Function prototypes to represent control flow graphs.
 */
#include "graph.h"
#include "temp.h"


Temp_tempList FG_def(G_node n);
Temp_tempList FG_use(G_node n);
bool FG_isMove(G_node n);

void preproc(T_instList il);//预处理
G_graph FG_AssemFlowGraph(T_instList il);
