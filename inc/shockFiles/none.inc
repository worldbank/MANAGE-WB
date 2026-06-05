********************************************************************************
$ontext

   MANAGE project

   GAMS file : SHK_NONE.INC

   @purpose  : Test for a baseline replication with no shock
               Example of defining a model variant
   @author   : W.Britz
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

  $$iftheni.decl "%1"=="decl"

     $$iftheni.baseline "%task%"=="Baseline"

        $$setglobal modelVariant "test"

        Model cge_%modelVariant%
        /
           cge_
        /

     $$endif.baseline


  $$else.decl

  $$endif.decl
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
