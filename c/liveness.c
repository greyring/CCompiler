#include "liveness.h"
#include "graph.h"
#include "temp.h"
#include "inst.h"


static G_table liveMap;

//全局表，记录在某个节点n，哪些寄存器是出口活跃的
static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps){
    G_enter(t, flowNode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flownode){
    return (Temp_tempList)G_look(t, flownode);
}

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail){
    Live_moveList a = (Live_moveList)checked_malloc(sizeof(*a));

    a->dst = dst;
    a->src = src;
    a->tail = tail;

    return a;
}

Temp_temp Live_gtemp(G_node n){
    return (Temp_temp)(n->info);
}

//判断temp是否在一个templist里
static bool isInList(Temp_tempList a, Temp_temp temp){
    Temp_tempList p=a;
    bool flag=false;

    while(p!=NULL){
        if(temp->num == p->head->num){
            flag=true;
        }
        p=p->tail;
    }

    return flag;
}


// 判断两个templist内容是否相同
static bool isTheSame(Temp_tempList a, Temp_tempList b){
    bool flag=true;
    
    Temp_tempList p=a;
    while(p!=NULL){
        if(isInList(b, p->head)==false){
            flag=false;
        }
        p=p->tail;
    }

    p=b;
    while(p!=NULL){
        if(isInList(a, p->head)==false){
            flag=false;
        }
        p=p->tail;
    }

    return flag;
}


//把一个templist复制一份，交给另一个指针
static Temp_tempList assignList(Temp_tempList a){
    Temp_tempList p1=a, p2;
    Temp_tempList res=p2;
    int first = 1;

    while(p1->head!=NULL){
        p2 = (Temp_tempList)checked_malloc(sizeof(Temp_tempList_));
        p2->head = (Temp_temp)checked_malloc(sizeof(Temp_temp_));
        // *(p2->head) = *(p1->head);
        p2->head->num = p1->head->num;
        p2->tail=NULL;
        if(first==1){
            res=p2;//给res赋值，作为返回值
            first=0;
        }
        p1=p1->tail;
        p2=p2->tail;
    }

    return res;
}

//把temp加入一个templist尾部
static Temp_tempList addTemp(Temp_temp temp, Temp_tempList templist){
    Temp_tempList p = templist;
    while(p!=NULL){
        p=p->tail;
    }
    p=Temp_TempList(temp, NULL);
    return templist;
}

//打印一个templist
static void print_templist(Temp_tempList templist) {
	printf("变量表：\n");

	Temp_tempList list = templist;
	while (list != NULL) {
		printf("%d\n", list->head->num);
		list = list->tail;
	}
}

//这部分估计会有很多bug，能不能改对不知道
//从控制流图，构建活跃性图
struct Live_graph Live_liveness(G_graph flow){
    G_nodeList current_node = flow->mynodes;
	int flag; // flag代表是否所有节点的in和out集合都不再变

	Temp_tempList in_;/* = (Temp_tempList)checked_malloc(sizeof(*in_));*/
	Temp_tempList out_;/* = (Temp_tempList)checked_malloc(sizeof(*out_));*/

	int test = 0;
	T_inst now;

	do {
		flag = 0;
		Temp_tempList p;
		G_nodeList p2;

		//test
		test++;
		printf("test: %d\n", test);

		for (current_node = flow->mynodes;
			current_node != NULL;
			current_node = current_node->tail) {

			//test
			now = (T_inst)current_node->head->info;
			printf("kind: %d\n", now->kind);

			//保存当前的in&out集合
			in_ = assignList(current_node->head->in);
			out_ = assignList(current_node->head->out);

			//test
			printf("保存集合成功！\n");

			//处理in集合
			current_node->head->in = assignList(current_node->head->use);//把use赋值给in
			if (current_node->head->in != NULL) printf("here:%d\n", current_node->head->in->head->num);
			//if (current_node->head->in->tail != NULL) printf("here:%d\n", current_node->head->in->tail->head->num);
			for (p = current_node->head->out; p != NULL; p = p->tail) {
				if (!isInList(current_node->head->def, p->head)) {
					//将out中不在def里的元素加入in
					if (!isInList(current_node->head->in, p->head)) {
						//同时这个元素不能在in集合里重复存在
						current_node->head->in = addTemp(p->head, current_node->head->in);
					}
					
					printf("加入in\n");
				}
			}

			//test
			printf("处理in集合成功！\n");

			//处理out集合,只需要将succs集合元素的in集合加进来就可以
			current_node->head->out = NULL;
			
			for (p2 = current_node->head->succs; p2 != NULL; p2 = p2->tail) {
				for (p = p2->head->in; p != NULL; p = p->tail) { 
					//把后继节点的in元素加入out集合
					if (!isInList(current_node->head->out, p->head)) {
						printf("加入out\n");
						current_node->head->out=addTemp(p->head, current_node->head->out);
					}
				}
			}

			//test
			printf("处理out集合成功！\n");


			//判断in&out集合是否发生改变
			if (!isTheSame(in_, current_node->head->in)) {
				printf("in集合不同\n");
				flag = 1;//发生改变，继续循环
			}
			if (!isTheSame(out_, current_node->head->out)) {
				printf("out集合不同\n");
				flag = 1;//发生改变，继续循环
			}

		}



	} while (flag == 1);

	printf("生成活跃性图成功！\n");
	struct Live_graph livegraph;
	livegraph.graph = flow;
	livegraph.moves = NULL;

/*
	print_templist(flow->mynodes->head->in);
	print_templist(flow->mynodes->head->out);
	print_templist(flow->mynodes->tail->head->in);
	print_templist(flow->mynodes->tail->head->out);
	print_templist(flow->mynodes->tail->tail->head->in);
	print_templist(flow->mynodes->tail->tail->head->out);*/
	return livegraph;
}

//返回一个temp在templist中排第几个
static int getIndex(Temp_tempList templist, Temp_temp temp){
    Temp_tempList p=templist;
    int res=-1;

    while(p!=NULL){
        res++;
        if(p->head->num == temp->num){
            break;
        }

        p=p->tail;
    }

    return res;
}

//从活跃性图，构建冲突图
struct interference interference_graph(struct Live_graph livegraph){
    int i, j;
	T_inst inst;
	int index1, index2;
	Temp_tempList outlist;
	Temp_temp out;

	struct interference inter;//构建一个新的冲突图，并初始化
	inter.n = 0;
	inter.templist = NULL;
	for (i = 0; i<50; i++) {
		for (j = 0; j<50; j++) {
			inter.matrix[i][j] = 0;
		}
	}

	G_graph graph = livegraph.graph;

	G_nodeList nodelist = graph->mynodes;
	const int I_INST = inst->I_INST;
	const int W_INST = inst->W_INST;
	const int M_INST = inst->M_INST;
	const int C_INST = inst->C_INST;

	printf("初始化冲突图成功！\n");
	while (nodelist != NULL) {
		inst = (T_inst)G_nodeInfo(nodelist->head);

		if (inst->kind == inst->I_INST || inst->kind == inst->W_INST || inst->kind == inst->M_INST || inst->kind == inst->C_INST) {
			switch (inst->kind) {
			case I_INST: {
				printf("kind:%d\n", I_INST);
				if (!isInList(inter.templist, nodelist->head->def->head)) {
					inter.templist = addTemp(nodelist->head->def->head, inter.templist);
					inter.n++;
				}
				index1 = getIndex(inter.templist, nodelist->head->def->head);
				printf("index1: %d\n", index1);

				outlist = nodelist->head->out;
				while (outlist != NULL) {
					if (!isInList(inter.templist, outlist->head)) {
						inter.templist=addTemp(outlist->head, inter.templist);
						inter.n++;
					}
					index2 = getIndex(inter.templist, outlist->head);
					printf("index2: %d\n", index2);

					//更新邻接矩阵
					inter.matrix[index1][index2] = 1;
					inter.matrix[index2][index1] = 1;

					outlist = outlist->tail;
				}
				break;
			}

			case W_INST: {
				//不处理SW指令，只处理LW指令
				printf("kind:%d\n", W_INST);
				if (inst->u.w.type == W_SW) break;

				if (!isInList(inter.templist, nodelist->head->def->head)) {
					inter.templist = addTemp(nodelist->head->def->head, inter.templist);
					inter.n++;
				}
				index1 = getIndex(inter.templist, nodelist->head->def->head);

				outlist = nodelist->head->out;
				while (outlist != NULL) {
					if (!isInList(inter.templist, outlist->head)) {
						inter.templist=addTemp(outlist->head, inter.templist);
						inter.n++;
					}
					index2 = getIndex(inter.templist, outlist->head);

					//更新邻接矩阵
					inter.matrix[index1][index2] = 1;
					inter.matrix[index2][index1] = 1;

					outlist = outlist->tail;
				}
				break;
			}

			case M_INST: {
				printf("kind:%d\n", M_INST);

				if (!isInList(inter.templist, nodelist->head->def->head)) {
					inter.templist = addTemp(nodelist->head->def->head, inter.templist);
					inter.n++;
				}
				index1 = getIndex(inter.templist, nodelist->head->def->head);
				printf("index1: %d\n", index1);

				outlist = nodelist->head->out;
				while (outlist != NULL) {
					if (nodelist->head->use->head->num == outlist->head->num) {
						outlist = outlist->tail;
						continue;
					}
					if (!isInList(inter.templist, outlist->head)) {
						inter.templist=addTemp(outlist->head, inter.templist);
						inter.n++;
					}
					index2 = getIndex(inter.templist, outlist->head);
					printf("index2: %d\n", index2);

					//更新邻接矩阵

					inter.matrix[index1][index2] = 1;
					inter.matrix[index2][index1] = 1;

					outlist = outlist->tail;
				}
				break;

			}

			case C_INST: {
				printf("kind:%d\n", C_INST);

				if (nodelist->head->def != NULL) {
					if (!isInList(inter.templist, nodelist->head->def->head)) {
						printf("冲突图加入一个变量\n");
						inter.templist=addTemp(nodelist->head->def->head, inter.templist);
						inter.n++;
					}
				}
				
				index1 = getIndex(inter.templist, nodelist->head->def->head);
				if (inter.templist == NULL) printf("yes\n");
				printf("index1: %d\n", index1);

				outlist = nodelist->head->out;
				while (outlist != NULL) {
					/*
					if (nodelist->head->use->head->num == outlist->head->num) {
						outlist = outlist->tail;
						continue;
					}*/
					if (!isInList(inter.templist, outlist->head)) {
						inter.templist=addTemp(outlist->head, inter.templist);
						inter.n++;
					}
					index2 = getIndex(inter.templist, outlist->head);
					printf("index2: %d\n", index2);

					//更新邻接矩阵

					inter.matrix[index1][index2] = 1;
					inter.matrix[index2][index1] = 1;

					outlist = outlist->tail;
				}
				break;
			}

			default: {
				break;
			}
			}
		}

		nodelist = nodelist->tail;
	}

	return inter;
}