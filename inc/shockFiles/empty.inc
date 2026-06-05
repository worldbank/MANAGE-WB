********************************************************************************
$ontext

   MANAGE project

   GAMS file : empty.INC

   @purpose  : Dummy shock file
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

  $$endif.decl
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
