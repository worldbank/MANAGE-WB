********************************************************************************
$ontext

   MANAGE project

   GAMS file : INISTOCH.GMS

   @purpose  : Build ancestor structure for tree reduction
   @author   : W.Britz
   @date     : 27.02.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
*
  execseed = 1 + gmillisec(jnow);
*
* --- let in average on stochastic run characterise ~100 random draws
*
  $$evalGlobal rndDraws %nDraws%*%nStochDraws%

  set rndDraws / r1*r%rndDraws%/;

  set tCur(tt);
  tCur(tt) $ ((years(tt) le %lastSimYear%) $ (years(tt) ge %BaseYear%) $ (tt.pos gt 1))  = YES;

  $$evalglobal nt %lastSimYear% - %baseYear%+1
  $$evalGlobal nNodes card(rndDraws)*(%nt%-1) + 1

*
  set nodes "Linearized list of nodes (draw*years)" /n1*n%nNodes%/;
  set t_nodes(t,*) "Link betwen year and decision node";
  alias(nodes,nodes1);
  set anc(*,*)          "Is the second node the node before first one?";
  set isNodeBefore(*,*) "Is the second node before first one?";
  set sameScen(*,*)     "The two nodes belong to the same scenario";
  set leaves(*)         "Final nodes for different paths";
  parameter p_probN(*)      "Probalilities nodes (lineared draw*years)"
            p_prob(draws,t) "Probability for a draw and year to occur";

  set nCur(*) "Actual nodes left";
  alias(nCur,nCur1,nCur2,nCur3);

  parameter p_randVar(randVars,*);
  set draws_nodes(draws,*);

  $$if not "%iScen%"=="" $exit

  set nodesToDo(*);
  nodesToDo(nodes) = YES;

 $$onEmbeddedCode Python:

     anc      = []
     t_nodes  = []

     t_nodes.append(("%baseYear%","n1"));
     node = 2
     for i in range(1,%rndDraws%+1):
         for t in range((%baseYear%+1),%lastSimYear%+1):
            t_nodes.append((str(t),"n"+str(node)))
            if t == (%baseYear%+1):
               anc.append(("n"+str(node),"n1"))
            else:
               anc.append(("n"+str(node),"n"+str(node-1)))
            node = node + 1

     gams.set("anc",anc);
     gams.set("t_nodes",t_nodes);

 $$offEmbeddedCode anc,t_nodes
*
*  --- all outcomes are equally likely
*
   p_probN(nodes)   = 1/card(rndDraws);
*
*  --- and share the root node
*
   p_probN('n1') = 1;
*
*  --- results from SCENRED: reduced ancestor matrix and updated probabilities for the reduced
*      node set
*
   set ancRed(*,*)        "Reduced ancestor matrix, reflects the condensed node list";
   parameter p_probRed(*) "Reduced probabiliy set, reflects the condensed node list";

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
