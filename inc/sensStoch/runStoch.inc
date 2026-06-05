********************************************************************************
$ontext

   MANAGE-WB project

   GAMS file : RUNSTOCH.GMS

   @purpose  : Run scenario tree reduction
   @author   :
   @date     : 27.02.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : scenario files using the stochastic extension

$offtext
********************************************************************************
*
* --- scenario tree construction from fan
*
  $$libinclude scenRed2
*
* --- information for SCENRED: option file and options from interface
*
  ScenredParms('sroption')       = 1;
* ScenredParms('visual_red')     = 1;

  $$setglobal sr2prefix test
  $$setglobal treeGen on

  execseed = 1 + gmillisec(jnow);

  set testOpt / order3*order6 /;

  file sr2opt / sr2Test.Opt /;
*
  execute_unload 'sr2%sr2Prefix%_in.gdx' anc,p_randVar,p_probN,t_nodes=tn,nodes=nCur;

  parameter p_mean;
  p_mean("ori") = sum(nodes, p_randVar("depr",nodes))/card(nodes);

  scalar bestOpt; bestOpt =  10*p_mean("ori");
  loop(testOpt,

      $$batinclude "inc/title.inc" '"%regGtapNameOri%: Scenario reduction tool for %SimName% scenario for "' testOpt.tl:0:0

      put sr2opt;

      if ( testOpt.pos lt card(testOpt),
         put 'order      ' testOpt.pos:0:0/;
      else
         put 'order      ' bestOpt/;
      );

      put 'metric_type transport' /;
      put 'p_norm 0' /;
      put 'section num_nodes' /;
      put '1  1'/;
      tCur("%baseYear%") = no;
      loop(tcur, put (tCur.pos+1):0:0,'    %nDraws%' /;);
      tCur("%baseYear%") = yes;
      put 'end' /;
      putclose;
*
*    --- run scenario tree reduction
*
     $$libinclude runScenRed2 %sr2Prefix% tree_con nodes anc p_probN ancRed p_probRed p_randVar

*
*    --- load information from ScenRed2
*
     execute_load 'sr2%sr2Prefix%_out.gdx' ancRed=red_ancestor,p_probRed=red_prob;

     option kill=nCur;
     nCur(nodes) $ p_probRed(nodes) = YES;
     p_mean(testOpt) = sum(nCur, p_randVar("depr",nCur))/card(nCur);
     if ( (abs(p_mean(testOpt) - p_mean("ori")) lt bestOpt),
        execute_unload 'sr2best_out.gdx' ancRed=red_ancestor,p_probRed=red_prob;
        bestOpt =  abs(p_mean(testOpt) - p_mean("ori"));
     );
  );

  execute_load 'sr2best_out.gdx' ancRed=red_ancestor,p_probRed=red_prob;

  execute 'rm -f sr2best_out.gdx';
  execute 'rm -f sr2%sr2Prefix%_out.gdx';
  execute 'rm -f sr2%sr2Prefix%_in.gdx';
  execute 'rm -f sr2%sr2Prefix%.opt';
  execute 'rm -f sr2%sr2Prefix%.log';

  $$batinclude "inc/title.inc" '"%regGtapNameOri%: Select best order and build sets and define random variables for representatives scenarios for %SimName%"'

  option kill=nCur;
  nCur(nodes) $ p_probRed(nodes) = YES;
  p_randVar(randVars,nodes) $ (not nCur(nodes)) = 0;
  t_nodes(t,nodes) $ (not nCur(nodes)) = 0;
  option kill=nodes,kill=p_prob,kill=anc;
  option kill=ts;
  $$include 'inc/solve/kill_equ.inc'

  p_mean("best") = sum(nCur, p_randVar("depr",nCur))/card(nCur);
  option p_mean:5;

  set leaves(*);
  leaves(nCur) $ sum(t_nodes("%lastSimYear%",nCur) $ p_probRed(nCur),1) = YES;


  isNodeBefore(nCur,nCur1) $ ancRed(nCur,nCur1) = YES;
  isNodeBefore(nCur,nCur)= YES;
  scalar it;
  for (it = %nt% downto 2 by 1,
      option kill=tCur;
      tCur(tt) $ (tt.pos eq it) = YES;
      loop(t_nodes(tCur,nCur),
         loop(ancRed(nCur1,nCur),
            isNodeBefore(nCur,nCur2) $ isNodeBefore(nCur1,nCur2) = YES;
          );
      );
  );

  sameScen(nCur,nCur1) $ (isNodeBefore(nCur,nCur1) or isNodeBefore(nCur1,nCur)) = YES;

  draws_nodes(draws,leaves) $ (draws.pos eq leaves.pos) = YES;

  draws_nodes(draws,nCur)   $ sum(samescen(ncur,leaves),draws_nodes(draws,leaves)) = YES;
  draws_nodes(draws,"n1") = YES;

  p_prob(draws,tt) = sum((t_nodes(tt,nCur),draws_nodes(draws,nCur)), p_probRed(nCur));
  p_prob(draws,"%baseYear%") = 1/card(draws);
  parameter p_testProb(tt); p_testProb(tt) = sum(draws, p_prob(draws,tt));
  parameter p_count(draws); p_count(draws) = sum(draws_nodes(draws,nCur),1);
  display p_count,p_testProb;

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
