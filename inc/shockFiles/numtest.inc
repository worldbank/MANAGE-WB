********************************************************************************
$ontext

   MANAGE project

   GAMS file : numtest.inc

   @purpose  : Numeraire test with a 10% increase in the exchange rate. All
               values and prices should increase by 10% and quantities should
               not change.
   @author   : W.Britz
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

  $$iftheni.decl "%1"=="decl"



  $$else.decl

     DebtStk.fx(instfd,t0) $ (tt.pos eq 2) = DebtStk.l(instfd,t0)*1.1;
     DebtStkT.fx(t0)       $ (tt.pos eq 2) = DebtStkT.l(t0)*1.1;
     debtNew.fx(instfd,t0) $ (tt.pos eq 2) = debtNew.l(instfd,t0)*1.1;
     er.fx(ts(t)) = er.l(t0)*1.1;

  $$endif.decl

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
