********************************************************************************
$ontext

   Manage project

   GAMS file : CheckInfes.inc

   @purpose  : Use python to print infeasible equations and vars at bounds
   @author   :
   @date     : 10.12.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
   nMsg = 0;

   embeddedCode Python:

      nMsg = [0]
      gams.set('nMsg',nMsg)
      amsg = []
      gams.set('amsg',amsg)

      def addMsg(l,s):
        i = len(l)
        l.append(('msg'+str(i+1),s))

      def atBound(db):

        gams.printLog("\nInfeasibilities: variables and equations with non-zero marginals:\n")
      #
      # ---- get all variables from the GAMS data base
      #
        for s in db:
           if (type(s) == GamsVariable):
              for r in s:
                  if abs(r.marginal)> 1.E-12:
                     #
                     # --- variables must be close to bounds
                     #
                     if  abs(r.level-r.upper)<1.E-8 or abs(r.level-r.lower)<1.E-8:

                        if len(amsg) < 98:
                           gams.printLog("\n"+str(r))
                           addMsg(amsg,str(r))
                        else:
                           return 0

           if (type(s) == GamsEquation):
              for r in s:
                  if abs(r.level-r.upper)>1.E-7:
                     #
                     # --- report only if infes > 1.E-7
                     #
                     if len(amsg) < 98:
                        gams.printLog("\n"+str(r)+" infes="+str(abs(r.level-r.upper)))
                        addMsg(amsg,str(r)+" infes="+str(abs(r.level-r.upper)))
                     else:
                        return 0

        return 0

      import os
      if os.path.isfile(r'%gams.wdir%cge_%modelVariant%_p.gdx'):
         rc = atBound(gams.ws.add_database_from_gdx(r'%gams.wdir%cge_%modelVariant%_p.gdx'))

      gams.printLog("\n")
      gams.set('amsg',amsg)
      nMsg = [len(amsg)]
      gams.set('nMsg',nMsg)

   pauseEmbeddedCode amsg nMsg

   if ( nMsg,
      display "Infeasibilities: Variables with non-zeros marginals (at bounds), infeasible equations:";
      writeMessages(amsg);
      display " ";
   );

   $$if not errorfree $abort Compilation error after file: %system.fn%
   if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
