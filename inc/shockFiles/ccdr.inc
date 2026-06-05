********************************************************************************
$ontext

   MANAGE project

   GAMS file : CCDR.INC

   @purpose  : Framework for CCDR related work
   @author   : W.Britz
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
$iftheni.decl "%1"=="decl"

   $$iftheni.baseline "%task%"=="Baseline"
*
*     --- in baseline mode, make sure that all symbols are loaded for subsequent
*         use in counterfactuals
*
      parameter GHGMitigation(*,*,t) "Relative emission ceiligns and carbon taxes"
                powerMix(*,*,t) "Power mix and power price projections from basebridge"
                energyProj(*,*,*,t) "Projections of changes in demand for energy services and energy use";
      set aEner(a),shockedFpNest(fpNest),shockedI(i);
      $$gdxin %readFile%
        $$load   aEner<energyProj.dim2
        $$load   shockedFpNest<energyProj.dim3
        $$load   shockedI<energyProj.dim2
      $$gdxin

   $$endif.baseline
*
*  --- add declaration part (including definitions of additional variables and equations)
*      for subshocks
*
   $$onmulti
   $$ifi "%GHGMitigation%"=="on" $batinclude '%simf%/GHGMitigation.inc' decl

   $$ifi "%fixPowerMix%"=="on"  $batinclude '%simf%/fixPowerMix.inc' decl

   $$ifi "%CCEnergy%"=="on"  $batinclude '%simf%/fixEnergy.inc' decl
   $$offmulti

   $$ifi "%ccDamagesIEC%"=="on" $batinclude '%simf%/ccDmgIEC.inc' decl

   $$batinclude  '%simf%/%add_shk_file%.inc' decl

   $$setglobal modelVariant "ccdr"

   Model cge_%modelVariant%
   /
      cge_

*
*     --- add additional model defined by the user (redirections are similar)
*
      $$ifi not set addModel $set addModel SomeUnknownModel
      $$ifi defined %addModel% %addModel%
*
*     ---  remove the pairing for the power mix and the fixing of the electriticy price
*
      $$ifi "%fixPowerMix%"=="on" -e_elecMix -e_ps +e_elecMix +e_ps

      $$ifi "%CCEnergy%"=="on" -e_xat +e_xat -e_xap +e_xap -e_xFac +e_xFac e_xFacAgr.chixFacAgr + e_lambdaOa.lambdaOa

      $$iftheni.ghgMitigation "%GHGMitigation%"=="on"
          m_ghgMitigation -e_emiGHG + e_emiGHG
          $$ifi "%maccs%"=="on"  -e_px + e_pxMac
      $$endif.ghgMitigation
*
      $$iftheni.baseline "%task%"=="Baseline"
*
*        ---   implement BaU closure by changing pairing
*
         -e_rgdpmp
         $$iftheni.diffLabProd "%diffLabProd%"=="on"
            e_rgdpmp.labProd
         $$else.diffLabProd
            e_rgdpmp.chixFacL
         $$endif.diffLabProd

      $$endif.baseline

   /;

$else.decl
*
*  --- add user supplied additional shock file (can be empty)
*
   $$batinclude  '%simf%/%add_shk_file%.inc' %1
*
*  --- run specific sub-shock files
*
   $$ifi "%fixPowerMix%"=="on"   $batinclude '%simf%/fixPowerMix.inc' %1

   $$ifi "%CCEnergy%"=="on"      $batinclude '%simf%/fixEnergy.inc' %1

   $$ifi "%ccDamagesIEC%"=="on"  $batinclude '%simf%/ccDmgIEC.inc' %1

   $$ifi "%GHGMitigation%"=="on" $batinclude '%simf%/GHGMitigation.inc' %1

$endif.decl
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
