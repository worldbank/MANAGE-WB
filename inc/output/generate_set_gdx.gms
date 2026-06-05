********************************************************************************
$ontext

   MANAGE project

   GAMS file : GENERATE_SET_GDX.GMS

   @purpose  : Generate a GDX in the exp-ref-dir with all sets and their
               elements
   @author   : W.Britz
   @date     : 05.11.24
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run_.gms

$offtext
********************************************************************************

  $$onEmbeddedCode Python:
      f = open(r"%gams.scrdir%sets.txt","w")
      for s in gams.db:
          iCnt = 0
          if (type(s) == GamsSet) :
             if s.name.lower() == 'diag':
                gams.printLog(s.name)
                continue
             if s.name.lower() == 'sameas':
                continue
             f.write(s.name)
             f.write(" ")
             iCnt += 1
             if iCnt == 10:
                f.write("\n")
                iCnt = 0
      f.close()
  $$offEmbeddedCode
  execute_unload "inc/%gamsDocName%.gdx"
  $$include '%gams.scrdir%sets.txt'
  ;

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
