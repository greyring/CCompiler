/*
 * canon.c - Functions to convert the IR trees into basic blocks and traces.
 *
 */
#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "canon.h"

/*在tree.h中有定义，不重复定义
typedef struct T_expList_ *T_expList;
struct T_expList_ {T_exp *head; T_expList tail;};
*/

/* local function prototypes */
static T_stm do_stm(T_stm stm);
static struct stmExp do_exp(T_exp exp);
static C_stmListList mkBlocks(T_stmList stms, Temp_label done);
static T_stmList getNext(void);

static T_expList T_expList(T_exp *head, T_expList tail)
{T_expList p = (T_expList) checked_malloc (sizeof *p);
 p->head=head; p->tail=tail;
 return p;
}

//是否为const类型的T_exp
static bool isNop(T_stm x) 
{  return x->kind == T_EXP && x->u.EXP->kind == T_CONST;
 }

//返回一个T_seq
static T_stm seq(T_stm x, T_stm y)
{
 if (isNop(x)) return y;
 if (isNop(y)) return x;
 return T_Seq(x,y);
}

//T_stm和T_exp是否可交换，默认name和const可以交换
static bool commute(T_stm x, T_exp y)
{
 if (isNop(x)) return TRUE;
 if (y->kind == T_NAME || y->kind == T_CONST) return TRUE;
 return FALSE;
}

//一个stm加一个exp
struct stmExp {T_stm s; T_exp e;};

static T_stm reorder(T_expList rlist) {
   if (!rlist) return T_Exp(T_Const(0)); /* nop */
   //如果顶层是call，替换掉call，再次调用reorder
   else if ((*rlist->head)->kind==T_CALL) {
      Temp_temp t = Temp_newtemp();
      *rlist->head = T_Eseq(T_Move(T_Temp(t),*rlist->head),T_Temp(t));
      return reorder(rlist);
    }
    //顶层不是call，对顶层做do_exp
   else {
      struct stmExp hd = do_exp(*rlist->head);
      //对tail做reorder
      T_stm s = reorder(rlist->tail);
      if (commute(s,hd.e)) {
	       *rlist->head=hd.e;
         return seq(hd.s,s);
      } else {
        Temp_temp t = Temp_newtemp();
        *rlist->head=T_Temp(t);
        return seq(hd.s, seq(T_Move(T_Temp(t),hd.e), s));
      }
    }
 }

static T_expList get_call_rlist(T_exp exp)
{T_expList rlist, curr;
 T_expList args = exp->u.CALL.args;
 curr = rlist = T_expList(&exp->u.CALL.fun, NULL);
 for (;args; args=args->tail) {
   curr = curr->tail = T_expList(&args->head, NULL);
 }
 return rlist;
}

//返回一个stm加一个exp
static struct stmExp StmExp(T_stm stm, T_exp exp) {
  struct stmExp x;
  x.s = stm;
  x.e = exp;
  return x;
}

//把exp拆解成一系列stm+一个exp
static struct stmExp do_exp(T_exp exp)
{
  switch(exp->kind) {
  case T_BINOP: 
    return StmExp(reorder(T_expList(&exp->u.BINOP.left, 
		       T_expList(&exp->u.BINOP.right, NULL))),
		  exp);
  case T_MEM: 
    return StmExp(reorder(T_expList(&exp->u.MEM, NULL)), exp);
  case T_ESEQ:
    {struct stmExp x = do_exp(exp->u.ESEQ.exp);
     return StmExp(seq(do_stm(exp->u.ESEQ.stm), x.s), x.e);
    }
  case T_CALL:    
      return StmExp(reorder(get_call_rlist(exp)), exp);
  default:
    return StmExp(reorder(NULL), exp);
  }
}

/* processes stm so that it contains no ESEQ nodes */
//把stm拆解成顶层的seq，和底层的嵌套seq
static T_stm do_stm(T_stm stm)
{
  switch (stm->kind) {
  case T_SEQ: 
    return seq(do_stm(stm->u.SEQ.left), do_stm(stm->u.SEQ.right));
  case T_JUMP:
    return seq(reorder(T_expList(&stm->u.JUMP.exp, NULL)), stm);
  case T_CJUMP:
    return seq(reorder(T_expList(&stm->u.CJUMP.left, 
				  T_expList(&stm->u.CJUMP.right,NULL))), stm);
  case T_MOVE:
    if (stm->u.MOVE.dst->kind == T_TEMP && stm->u.MOVE.src->kind == T_CALL)
      return seq(reorder(get_call_rlist(stm->u.MOVE.src)), stm);
    else if (stm->u.MOVE.dst->kind == T_TEMP)
      return seq(reorder(T_expList(&stm->u.MOVE.src, NULL)), stm);
    else if (stm->u.MOVE.dst->kind == T_MEM)
      return seq(reorder(T_expList(&stm->u.MOVE.dst->u.MEM, 
			 T_expList(&stm->u.MOVE.src, NULL))), stm);
    else if (stm->u.MOVE.dst->kind == T_ESEQ) {
      T_stm s = stm->u.MOVE.dst->u.ESEQ.stm;
      stm->u.MOVE.dst = stm->u.MOVE.dst->u.ESEQ.exp;
      return do_stm(T_Seq(s, stm));
    }
    assert(0); /* dst should be temp or mem only */
  case T_EXP:
    if (stm->u.EXP->kind == T_CALL)
         return seq(reorder(get_call_rlist(stm->u.EXP)), stm);
    else return seq(reorder(T_expList(&stm->u.EXP, NULL)), stm);
 default:
    return stm;
 }
}

/* linear gets rid of the top-level SEQ's, producing a list */
static T_stmList linear(T_stm stm, T_stmList right)
{
 if (stm->kind == T_SEQ) 
   return linear(stm->u.SEQ.left,linear(stm->u.SEQ.right,right));
 else return T_StmList(stm, right);
}

/* From an arbitrary Tree statement, produce a list of cleaned trees
   satisfying the following properties:
      1.  No SEQ's or ESEQ's
      2.  The parent of every CALL is an EXP(..) or a MOVE(TEMP t,..) */
T_stmList C_linearize(T_stm stm)
{
    return linear(do_stm(stm), NULL);
}

static C_stmListList StmListList(T_stmList head, C_stmListList tail)
{C_stmListList p = (C_stmListList) checked_malloc (sizeof *p);
 p->head=head; p->tail=tail;
 return p;
}
 
/* Go down a list looking for end of basic block */
//找到基本块末尾的jump或cjump
static C_stmListList next(T_stmList prevstms, T_stmList stms, Temp_label done)
{
  if (!stms) 
    return next(prevstms, 
		T_StmList(T_Jump(T_Name(done), Temp_LabelList(done, NULL)), 
			  NULL), done);
  if (stms->head->kind == T_JUMP || stms->head->kind == T_CJUMP) {
    C_stmListList stmLists;
    prevstms->tail = stms; 
    stmLists = mkBlocks(stms->tail, done);
    stms->tail = NULL;
    return stmLists;
  } 
  else if (stms->head->kind == T_LABEL) {
    Temp_label lab = stms->head->u.LABEL;
    return next(prevstms, T_StmList(T_Jump(T_Name(lab), Temp_LabelList(lab, NULL)), 
			     stms), done);
  }
  else {
    prevstms->tail = stms;
    return next(stms, stms->tail, done);
  }
}

/* Create the beginning of a basic block */
static C_stmListList mkBlocks(T_stmList stms, Temp_label done)
{
  if (!stms) { 
    return NULL;
  }
  if (stms->head->kind != T_LABEL) {
    return mkBlocks(T_StmList(T_Label(Temp_newlabel()), stms), done);
  }
  /* else there already is a label */
  return StmListList(stms, next(stms, stms->tail, done));
}

        /* basicBlocks : Tree.stm list -> (Tree.stm list list * Tree.label)
	       From a list of cleaned trees, produce a list of
	 basic blocks satisfying the following properties:
	      1. and 2. as above;
	      3.  Every block begins with a LABEL;
              4.  A LABEL appears only at the beginning of a block;
              5.  Any JUMP or CJUMP is the last stm in a block;
              6.  Every block ends with a JUMP or CJUMP;
           Also produce the "label" to which control will be passed
           upon exit.
        */
struct C_block C_basicBlocks(T_stmList stmList)
{
  struct C_block b;
  b.label = Temp_newlabel(); 
  b.stmLists = mkBlocks(stmList, b.label); 

  return b;
}

static S_table block_env;
static struct C_block global_block;

//找到一个stmList中的倒数第二个stm
static T_stmList getLast(T_stmList list)
{
  T_stmList last = list;
  while (last->tail->tail) last = last->tail;
  return last;
}

static void trace(T_stmList list)
{
  T_stmList last = getLast(list);
  T_stm lab = list->head;
  T_stm s = last->tail->head;
  S_enter(block_env, lab->u.LABEL, NULL);
  if (s->kind == T_JUMP) {
    T_stmList target = (T_stmList) S_look(block_env, s->u.JUMP.jumps->head);
    if (!s->u.JUMP.jumps->tail && target) {
      last->tail = target; /* merge the 2 lists removing JUMP stm */
      trace(target);
    }
    else last->tail->tail = getNext(); /* merge and keep JUMP stm */
  }
  /* we want false label to follow CJUMP */
  else if (s->kind == T_CJUMP) {
    T_stmList true =  (T_stmList) S_look(block_env, s->u.CJUMP.true);
    T_stmList false =  (T_stmList) S_look(block_env, s->u.CJUMP.false);
    if (false) {
      last->tail->tail = false;
      trace(false);
    }
    else if (true) { /* convert so that existing label is a false label */
      last->tail->head = T_Cjump(T_notRel(s->u.CJUMP.op), s->u.CJUMP.left,
				 s->u.CJUMP.right, s->u.CJUMP.false, 
				 s->u.CJUMP.true);
      last->tail->tail = true;
      trace(true);
    }
    else {
      Temp_label false = Temp_newlabel();
      last->tail->head = T_Cjump(s->u.CJUMP.op, s->u.CJUMP.left,
				 s->u.CJUMP.right, s->u.CJUMP.true, false);
      last->tail->tail = T_StmList(T_Label(false), getNext());
    }
  }
  else assert(0);
}

/* get the next block from the list of stmLists, using only those that have
 * not been traced yet */
//找到第一个在块环境中存在的块，开始trace()
static T_stmList getNext()
{
  if (!global_block.stmLists)
    return T_StmList(T_Label(global_block.label), NULL);
  else {
    T_stmList s = global_block.stmLists->head;
    if (S_look(block_env, s->head->u.LABEL)) {/* label exists in the table */
      trace(s);
      return s;
    }
    else {
      global_block.stmLists = global_block.stmLists->tail;
      return getNext();
    }
  }
}
         /* traceSchedule : Tree.stm list list * Tree.label -> Tree.stm list
            From a list of basic blocks satisfying properties 1-6,
            along with an "exit" label,
	    produce a list of stms such that:
	      1. and 2. as above;
              7. Every CJUMP(_,t,f) is immediately followed by LABEL f.
            The blocks are reordered to satisfy property 7; also
	    in this reordering as many JUMP(T.NAME(lab)) statements
            as possible are eliminated by falling through into T.LABEL(lab).
         */
//把cjump后面跟上false标号
//对其他模块的接口
T_stmList C_traceSchedule(struct C_block b)
{ C_stmListList sList;
  block_env = S_empty();
  global_block = b;

  for (sList=global_block.stmLists; sList; sList=sList->tail) {
    S_enter(block_env, sList->head->head->u.LABEL, sList->head);
  }

  return getNext();
}

