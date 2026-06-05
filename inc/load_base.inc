********************************************************************************
$ontext

   CAPRI project

   GAMS file : LOAD_BASE.INC

   @purpose  : Default assumptions for GTAP-Power where the add ces is used

   @author   :
   @date     : 30.04.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
  addCest("c-oil") = YES;
  addCest("c-gas") = YES;
  addCest("c-coa") = YES;
  addCest("c-ely") = YES;
  addCest("c-chm") = YES;
  addCest("c-nmm") = YES;
  addCest("c-i_s") = YES;
  addCest("c-crp") = YES;
  addCest("c-lvs") = YES;
  addCest("c-otp") = YES;
  addCest("c-atp") = YES;
  addCest("c-wtp") = YES;
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
