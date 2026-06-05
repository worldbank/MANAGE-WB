********************************************************************************
$ontext

   GTAP8 in GAMS project

   GAMS file : LHS_TO_GDX.GMS

   @purpose  : Read output from SANDIA LHS utility to store in GDX
   @author   : Wolfgang Britz
   @date     : 28.05.15
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : sensitivity.gms

$offtext
********************************************************************************
  $$offlisting

  $$include %lhsout%

  set draws / d1*d%nDraws%/;
  parameter p_doe(draws,var) "Probability for each draw and variable from LHS";
  p_doe(draws,var) = sum(obs $ (obs.pos eq draws.pos), LHSample("1",var,obs));

  execute_unload "%gdxfile%" p_doe;

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
