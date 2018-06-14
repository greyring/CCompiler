
#include "flowgraph.h"

static G_table table = G_empty();

//返回一个G_node的def集合templist
Temp_tempList FG_def(G_node n){
    //建一个templist作为返回值
    /*
    Temp_tempList templist = (Temp_tempList)checked_malloc (*templist);
    templist->head=NULL;
    templist->tail=NULL;
    */

    T_inst inst = (T_inst)info;//读取node中的指令
    switch(inst->kind){
        case I_INST:{
            //返回I型指令的def集合
            Temp_temp temp = inst->u.i.rd;
            Temp_tempList templist = Temp_TempList(temp, NULL);
            return templist;
        }
        case T_INST:{
            //返回T型指令的def集合
            return NULL;
        }
        case O_INST:{
            //返回O型指令的def集合
            return NULL;
        }
        case J_INST:{
            //返回J型指令的def集合
            return NULL;
        }
        case W_INST:{
            //返回W型指令的def集合
            if(inst->u.w.type==W_LW){
                Temp_temp temp = inst->u.w.rt;
                Temp_tempList templist = Temp_TempList(temp, NULL);
                return templist;
            }
            else{
                //SW指令不需要def
                return NULL;
            }
        }
        case M_INST:{
            Temp_temp temp = inst->u.m.rt;
            Temp_tempList templist = Temp_TempList(temp, NULL);
            return templist;
        }
        case C_INST:{
            Temp_temp temp = inst->u.c.rt;
            Temp_tempList templist = Temp_TempList(temp, NULL);
            return templist;
        }
        case L_INST:{
            //label不需要def集合
            return NULL;
        }
        default:{
            return NULL;
        }
    }

}

//返回一个G_node的use集合templist
Temp_tempList FG_use(G_node n){
    T_inst inst = (T_inst)info;//读取node中的指令
    switch(inst->kind){
        case I_INST:{
            //I型指令的use集合
            Temp_temp temp1=inst->u.i.rs;
            Temp_temp temp2=inst->u.i.rt;
            Temp_tempList templist1 = Temp_TempList(temp1, NULL);
            Temp_tempList templist2 = Temp_TempList(temp2,templist1);
            return templist2;
        }
        case T_INST:{
            //branch指令的use集合
            Temp_temp temp1=inst->u.t.rs;
            Temp_temp temp2=inst->u.t.rt;
            Temp_tempList templist1 = Temp_TempList(temp1, NULL);
            Temp_tempList templist2 = Temp_TempList(temp2,templist1);
            return templist2;
        }
        case O_INST:{
            Temp_temp temp=inst->u.o.rs;
            Temp_tempList templist = Temp_TempList(temp, NULL);
            return templist;
        }
        case J_INST:{
            //jump指令不需要use集合
            return NULL;
        }
        case W_INST:{
            if(inst->u.w.type==W_LW){
                //LW指令的use集合
                Temp_temp temp=inst->u.w.rs;
                Temp_tempList templist = Temp_TempList(temp, NULL);
                return templist;
            }
            else if(inst->u.w.type==W_SW){
                //SW指令的use集合
                Temp_temp temp1=inst->u.w.rs;
                Temp_temp temp2=inst->u.w.rt;
                Temp_tempList templist1 = Temp_TempList(temp1, NULL);
                Temp_tempList templist2 = Temp_TempList(temp2,templist1);
                return templist2;
            }
            else return NULL;
        }
        case M_INST:{
            //move指令的use集合
            Temp_temp temp=inst->u.m.rs;
            Temp_tempList templist = Temp_TempList(temp, NULL);
            return templist;
        }
        case C_INST:{
            //move const指令不需要use集合
            return NULL;
        }
        case L_INST:{
            //label不需要use集合
            return NULL;
        }
        default: return NULL;
    }

}

bool FG_isMove(G_node n){
    T_inst inst = (T_inst)info;//读取node中的指令
    if(inst->kind==M_INST){
        //如果是move指令，返回true，否则返回false
        return true;
    }
    else return false;
}

//提前遍历一遍指令集合，将所有label包装成nodelist，放入table待查
void preproc(T_instList il){
    T_inst inst;
    while(il != NULL){
        inst = il->head; //读取一条指令
        if(inst->kind==L_INST){
            //如果是label指令
            G_node node = G_Node(NULL, inst);
            node->def = FG_def(node);
            node->use = FG_use(node);
            node->in = NULL;
            node->out = NULL;
            G_nodeList nodelist = G_NodeList(node, NULL);
            G_enter(table, il->u.l.label, nodelist);//放入table
        }
        il = il->tail;
    }
}


G_graph FG_AssemFlowGraph(T_instList il){
    //先预处理，把所有label存进table
    preproc(il);


    //生成一个空图
    G_graph G = G_Graph(void);

    //逐条分析指令，一条指令添加一个node和若干条边。
    while(il != NULL){
        G_nodeList nodelist = NULL;
        G_node node = NULL;
        if(il->head->kind != L_INST){
            //label指令需要特殊处理，查表获得，不能新建
            node = G_Node(G, il->head);
            nodelist = G_NodeList(node, NULL);
            node->def = FG_def(node);
            node->use = FG_use(node);
        }
        

        switch(il->head->kind){
            case I_INST:{
                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    //不是空图
                    G->nodecount++;

                    //更新mynodes
                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;
                    
                    //当前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }
                    

                    //更新mylast
                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }
            }
            case T_INST:{
                //先从表中获得label对应的node
                G_nodeList labelnodelist = G_look(table, il->head->u.t.label);
                G_node labelnode = nodelist->head;

                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    G->nodecount++;

                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;

                    //当前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }

                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }
                G_addEdge(node, labelnode);//增加branch指向label的一条边

            }
            case O_INST:{
                //先从表中获得label对应的node
                G_nodeList labelnodelist = G_look(table, il->head->u.o.label);
                G_node labelnode = nodelist->head;
                
                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    G->nodecount++;

                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;

                    //当前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }

                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }
                G_addEdge(node, labelnode);//增加branch指向label的一条边

            }
            case J_INST:{
                //先从表中获得label对应的node
                G_nodeList labelnodelist = G_look(table, il->head->u.j.label);
                G_node labelnode = nodelist->head;
                
                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    G->nodecount++;

                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;

                    //如果前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }

                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }
                G_addEdge(node, labelnode);//增加jump指向label的一条边

            }
            case W_INST:{
                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    //不是空图
                    G->nodecount++;

                    //更新mynodes
                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;
                    
                    //如果前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }
                    

                    //更新mylast
                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }
            }
            case M_INST:{
                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    //不是空图
                    G->nodecount++;

                    //更新mynodes
                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;
                    
                    //如果前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }
                    

                    //更新mylast
                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }
            }
            case C_INST:{
                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    //不是空图
                    G->nodecount++;

                    //更新mynodes
                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;
                    
                    //如果前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }
                    

                    //更新mylast
                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }
            }
            case L_INST:{
                //获得已经创建好的label对应的node
                nodelist = G_look(table, il->head.u.l.label);
                node = nodelist->head;

                if(G->mynodes == NULL){
                    //空图，添加到第一个节点，不添加任何边
                    G->nodecount++;
                    G->mynodes = G->mylast = nodelist;
                }
                else{
                    G->nodecount++;

                    G_nodeList p = G->mynodes;
                    while(p->tail!=NULL) p=p->tail;
                    p->tail=nodelist;

                    //如果前一条指令不是jump，添加一条边
                    T_inst inst = (T_inst)G_nodeInfo(G->mylast->head);
                    if(inst->kind != J_INST){
                        G_addEdge(G->mylast->head, node);
                    }

                    G->mylast->tail = nodelist;
                    G->mylast = G->mylast->tail;
                }

            }
        }

        il = il->tail;
    }

    return G;

}