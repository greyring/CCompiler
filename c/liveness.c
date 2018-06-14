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

//把一个templist复制一份，交给另一个指针
static Temp_tempList assignList(Temp_tempList a){
    Temp_tempList p1=a, p2;
    Temp_tempList res=p2;
    int first = 1;

    while(p1!=NULL){
        p2 = (Temp_tempList)checked_malloc(sizeof(Temp_tempList_));
        p2->head = (Temp_temp)checked_malloc(sizeof(Temp_temp_));
        *(p2->head) = *(p1->head);
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


//这部分估计会有很多bug，能不能改对不知道
//从控制流图，构建活跃性图
struct Live_graph Live_liveness(G_graph flow){
    G_nodeList current_node = flow->mynodes;
    int flag; // flag代表是否所有节点的in和out集合都不再变
    
    Temp_tempList in_;/* = (Temp_tempList)checked_malloc(sizeof(*in_));*/
    Temp_tempList out_;/* = (Temp_tempList)checked_malloc(sizeof(*out_));*/
    
    do{
        flag=0;
        Temp_tempList p;
        G_nodeList p2;

        for(current_node = flow->mynodes;
            current_node!=NULL;
            current_node = current_node->tail){

            //保存当前的in&out集合
            in_=assignList(current_node->head->in);
            out_=assignList(current_node->head->out);

            //处理in集合
            current_node->head->in = assignList(current_node->head->use);//把use赋值给in
            for(p=current_node->head->out;p!=NULL;p=p->tail){
                if(!isInList(current_node->head->def, p->head)){
                    //将out中不在def里的元素加入in
                    current_node->head->in=addTemp(p->head, current_node->head->in);
                }
            }

            //处理out集合
            current_node->head->out = NULL;
            for(p2=current_node->tail;p2!=NULL;p2=p2->tail){
                for(p=p2->head->in;p!=NULL;p=p->tail){
                    //把后继节点的in元素加入out集合
                    if(!isInList(current_node->head->out,p->head)){
                        addTemp(p->head, current_node->head->out);
                    }
                }
            }

            //判断in&out集合是否发生改变
            if(!isTheSame(in_, current_node->in)){
                flag=1;//发生改变，继续循环
            }
            if(!isTheSame(out_, current_node->out)){
                flag=1;//发生改变，继续循环
            }

        }
        


    }while(flag==1);

    struct Live_graph livegraph;
    livegraph.graph=flow;
    livegraph.moves=NULL;
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

        p=p->tail
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
    inter.n=-1;
    inter.templist=NULL;
    for(i=0;i<50;i++){
        for(j=0;j<50;j++){
            inter.matrix[i][j]=0;
        }
    }

    G_graph graph = livegraph.graph;

    G_nodeList nodelist = graph->mynodes;

    while(nodelist != NULL){
        inst = (T_inst)G_nodeInfo(nodelist->head);

        if(inst.kind == I_INST ||inst.kind == W_INST ||inst.kind == M_INST||inst.kind == C_INST){
            switch(inst->kind){
                case I_INST:{
                    if(!isInList(inter.templist,nodelist->head->def->head)){
                        addTemp(nodelist->head->def->head, inter.templist);
                    }
                    index1 = getIndex(inter.templist,nodelist->head->def->head);

                    outlist = nodelist->head->out;
                    while(outlist!=NULL){
                        if(!isInList(inter.templist,outlist->head)){
                            addTemp(outlist->head, inter.templist);
                        }
                        index2 = getIndex(inter.templist,outlist->head);

                        //更新邻接矩阵
                        inter.matrix[index1][index2]=1;
                        inter.matrix[index2][index1]=1;

                        outlist=outlist->tail;
                    }
                    break;
                }

                case W_INST:{
                    //不处理SW指令，只处理LW指令
                    if(inst->u.w.type == W_SW) break;

                    if(!isInList(inter.templist,nodelist->head->def->head)){
                        addTemp(nodelist->head->def->head, inter.templist);
                    }
                    index1 = getIndex(inter.templist,nodelist->head->def->head);

                    outlist = nodelist->head->out;
                    while(outlist!=NULL){
                        if(!isInList(inter.templist,outlist->head)){
                            addTemp(outlist->head, inter.templist);
                        }
                        index2 = getIndex(inter.templist,outlist->head);

                        //更新邻接矩阵
                        inter.matrix[index1][index2]=1;
                        inter.matrix[index2][index1]=1;

                        outlist=outlist->tail;
                    }
                    break;
                }

                case M_INST:{
                    if(!isInList(inter.templist,nodelist->head->def->head)){
                        addTemp(nodelist->head->def->head, inter.templist);
                    }
                    index1 = getIndex(inter.templist,nodelist->head->def->head);

                    outlist = nodelist->head->out;
                    while(outlist!=NULL){
                        if(nodelist->head->use->head->num == outlist->head->num){
                            outlist=outlist->tail;
                            continue;
                        }
                        if(!isInList(inter.templist,outlist->head)){
                            addTemp(outlist->head, inter.templist);
                        }
                        index2 = getIndex(inter.templist,outlist->head);

                        //更新邻接矩阵
                        
                        inter.matrix[index1][index2]=1;
                        inter.matrix[index2][index1]=1;

                        outlist=outlist->tail;
                    }
                    break;

                }

                case C_INST:{
                    if(!isInList(inter.templist,nodelist->head->def->head)){
                        addTemp(nodelist->head->def->head, inter.templist);
                    }
                    index1 = getIndex(inter.templist,nodelist->head->def->head);

                    outlist = nodelist->head->out;
                    while(outlist!=NULL){
                        if(nodelist->head->use->head->num == outlist->head->num){
                            outlist=outlist->tail;
                            continue;
                        }
                        if(!isInList(inter.templist,outlist->head)){
                            addTemp(outlist->head, inter.templist);
                        }
                        index2 = getIndex(inter.templist,outlist->head);

                        //更新邻接矩阵
                        
                        inter.matrix[index1][index2]=1;
                        inter.matrix[index2][index1]=1;

                        outlist=outlist->tail;
                    }
                    break;
                }

                default:{
                    break;
                }
            }
        }

        nodelist=nodelist->tail;
    }

    return inter;
}