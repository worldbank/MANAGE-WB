********************************************************************************
$ontext

   MANAGE project

   GAMS file : fixPowerInc.INC

   @purpose  : Code to fix power mix and power price as part of shock
   @author   : W.Britz
   @date     : 19.10.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
  $$iftheni.decl "%1"=="decl"

     $$ifi not "%shk_file%"=="ccdr" $$Abort "FixPowerMix.inc is part of the ccdr shock file and cannot be used in standalone mode"

     $$ifi not defined PowerMix $$abort "Bridge-file missing powerMix projections, in file: %system.fn%, line: %system.incline%"
*
*    --- largest electricity activity in t0 (higher share on powermix)
*
     set aElecFst(a);
     alias(aElec,aElec1);
     aElecFst(aElec) $ (powerMix("%powerScen%",aElec,t0) eq smax(aElec1, powerMix("%powerScen%",aElec1,t0))) = YES;
     set elecPrice / elecPrice /;

  $$else.decl
*
*    --- fix n-1 power mix shares
*
     elecmix.fx(aElec,iElec,tt) $ (not aElecFst(aElec)) = powerMix("%powerScen%",aElec,tt);
*
*    --- release prod shifters for n-1 power activities
*
     chixFacFa.lo("f-top",aElec,tt) $ (powerMix("%powerScen%",aElec,tt) and (not aElecFst(aElec))) = -inf;
     chixFacFa.up("f-top",aElec,tt) $ (powerMix("%powerScen%",aElec,tt) and (not aElecFst(aElec))) = +inf;
*
*    --- fix electricity price (implicitly defines the productivity shift for the n-1 share)
*
     $$iftheni.fixElecPrice "%fixElecPrice%"=="on"
        ps.fx(iElec,tt) $ powerMix("%powerScen%","elecPrice",tt) = ps.l(iElec,t0) * powerMix("%powerScen%","elecPrice",tt);
        chixFacFa.lo("f-top",aElecFst,tt) $ powerMix("%powerScen%","elecPrice",tt) = -inf;
        chixFacFa.up("f-top",aElecFst,tt) $ powerMix("%powerScen%","elecPrice",tt) = +inf;
     $$endif.fixElecPrice

     chixFacFa.l ("f-top",aElec,tt)    $ (chixFacFa.range("f-top",aElec,tt) ne 0) = chixFacFa.l("f-top",aElec,tt-1);
     lambdaxfac.l("f-top",aElec,v,tt)  $ (chixFacFa.range("f-top",aElec,tt) ne 0) = lambdaxfac.l("f-top",aElec,v,tt-1);
  $$endif.decl
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
