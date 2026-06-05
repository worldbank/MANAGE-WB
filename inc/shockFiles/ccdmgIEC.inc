********************************************************************************
$ontext

   MANAGE project

   GAMS file : ccDmgIEC.INC

   @purpose  : Shock file for CC damages based on IEC data
               This selects scens without adapation

   @author   : W.Britz, starting from older code snippets from other projects
   @date     : 28.08.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
$iftheni.decl "%1"=="decl"

   $$ifi not "%shk_file%"=="ccdr" $$Abort "ccdmgIEC.inc is part of the ccdr shock file and cannot be used in standalone mode"

   set aHydro(a);

*
*  --- read the labels from the IEC shock matrix from the GDX
*
   set clim,clim_type,scen,chnl,ccShock;
   $$gdxin '%simf%/ccdmg_shocks_IEC.gdx'
     $$load  clim      < p_shocks.dim2
     $$load  clim_type < p_shocks.dim3
     $$load  scen      < p_shocks.dim4
     $$onmulti
     $$load  chnl      < p_shocks.dim5
     $$offmulti
     $$load  ccShock   < p_shocks.dim6
     parameter p_shocks(t,clim,clim_type,scen,chnl,ccShock) "IEC Shock matrix";
     $$load  p_shocks
   $$gdxin
   $$onmulti
   set clim /SSP370/;
   $$offmulti


*
*  --- the set of active channels is inputted via a scen driver field, the same for the
*      climate type (such as ensemble-mean) and the socio-economic scenario (IECScen)
*

   $$iftheni.chnl "%chnls%"=="all"
       set curChnls(chnl) / set.chnl /;
   $$else.chnl
     set curChnls(chnl) / set.IECChnls /;
   $$endif.chnl


   $$iftheni.mapaGtap not defined mapaGTAP

        set mapaGtap(aGtap,is0)  "mapping between GTAP activities and SAM0 activities"
            aGTAP                "GTAP activities with ely aggregated";

        $$onmulti
        $$gdxin "%datF%/gtapData11a.gdx"
            $$load aGTAP=ag
         $$gdxin

        $$gdxin %readFile%
          $$loaddc mapaGtap
        $$gdxin
   $$endif.mapaGtap

   aHydro(a) $ sum(mapaGtap("hydroBL",a0),mapis(a0,a))  = yes;
   aHydro(a) $ sum(mapaGtap("hydroP",a0),mapis(a0,a))   = yes;

*
*  -- define the output name such that GDX files have distinct names
*
   $$setglobal simNamePostFix %IECchnls%_%IECclim%_%IECclimType%_%IECScen%
*
*  --- some of the channels don't have a ensemble mean for SS3-7.0, construct in code
*
   $$iftheni.hydroScen "%IECClim%"=="SSP3-7.0 mean"
        set climHydroRoadBridge(clim) / "SSP3-7.0 INM-CM4-8","SSP3-7.0 INM-CM5-0","SSP3-7.0 CNRM-ESM2-1" /;
   $$else.hydroScen
        set climHydroRoadBridge(clim) / "%IECClim%"  /;
   $$endif.hydroScen

*
*  --- substitute scenarios for climate types to SSPs scenarios for inland flooding
*
   $$iftheni.inlandFlood "%IECClim%"=="Wet/Warm mean"
        singleton set climInlandFlood(clim) / "ssp245" /;
   $$elseifi.inlandFlood "%IECClim%"=="Dry/Hot mean"
        singleton set climInlandFlood(clim) / "ssp3-7.0 mean" /;
   $$elseifi.inlandFlood "%IECClim%"=="Hot/Dry mean"
        singleton set climInlandFlood(clim) / "ssp370" /;
   $$elseifi.inlandFlood "%IECClim%"=="SSP1-1.9 mean"
        singleton set climInlandFlood(clim) / "ssp119" /;
   $$elseifi.inlandFlood "%IECClim%"=="SSP3-7.0 mean"
        singleton set climInlandFlood(clim) / "ssp370" /;
   $$endif.inlandFlood

$else.decl

*
*  --- turn damages on
*
   ifDmg = 1;

$$iftheni.wash "%iecChnlsWash%"=="ON"
*
*  --- improved labour productivity from WASH under SR scenario
*      (empty for CG)
*
   dmgSup.fx(l,"lab",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","health","pct_change_supply");

$$endif.wash

$$iftheni.health "%iecChnlsHealth%"=="ON"
*
*  --- health damage to labor stock
*
   dmgSup.fx(l,"health",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","health","");

$$endif.health

$$iftheni.lab "%iecChnlsLab%"=="ON"
*
*  --- damage to labor productivity, by aggregate sector
*
   dmgPrd.fx(l,agr,"lab",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","lab","agr");
   dmgPrd.fx(l,man,"lab",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","lab","ind");
   dmgPrd.fx(l,srv,"lab",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","lab","ser");

$$endif.lab

$$iftheni.hydro "%iecChnlsHydro%"=="ON"
*
*  --- damage to hydro output, defined as relative change against historical climate
*      (attention: no data for SSP1, for SSP3-7.0, aggregate here)
*
   dmgPrd.fx("f-top",aHydro,"hydro",ttt) $ sum(climHydroRoadBridge,p_shocks(ttt,climHydroRoadBridge,"","%IECscen%","hydro","pow"))
       = sum(climHydroRoadBridge,(p_shocks(ttt,climHydroRoadBridge,"","%IECscen%","hydro","pow")- p_shocks(ttt,"Historical","","%IECscen%","hydro","pow"))
          /p_shocks(ttt,"Historical","","%IECscen%","hydro","pow"))/card(climHydroRoadBridge);

$$endif.hydro

$$iftheni.liveStock "%iecChnlsLiveStock%"=="ON"
*
*  --- damage to livestock productivity. Only available for CG
*
   dmgPrd.fx("f-top",anim,"livestock",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","livestock","");
$$endif.liveStock

$$iftheni.crops "%iecChnlsCrops%"=="ON"
*
*  --- damage to crops productivity
*
   dmgPrd.fx("f-top",crops,"crops",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","crops","");
$$endif.Crops

$$iftheni.erosion "%iecChnlsErosion%"=="ON"
*
*  --- damage from erosion. Modelled as change in land productivity (??)
*
   dmgPrd.fx(lnd,crops,"erosion",tt) = p_shocks(tt,"%IECClim%","%IECClimType%","%IECscen%","erosion","");
$$endif.erosion

$$iftheni.urban_flood "%iecChnlsUrban_flood%"=="ON"
*
*  --- damage from urban flooding (uses SSP1-2.6 mean type labels, climType is empty; no results for Dry/hot and Wet/Warm, also
*      sub-scenarios to aggregate to wet/warm or dry/hot are missing)
*
   dmgStk.fx(cap,"urban_flood",tt) = p_shocks(tt,"%IECClim%","","%IECscen%","urban_flood","cost_pctCap");

$$endif.urban_flood

$$iftheni.roadBridge "%iecChnlsRoadBridge"=="ON"
*
*  --- damage to road and bridges
*
   $$ifi "%IECScen%"=="Hot/Dry" $$setglobal "%IECScen" "Dry/Hot"
   dmgStk.fx(cap,"roadBridge",tt) $ (curChnls("roadBridge") $ sum(climHydroRoadBridge,p_shocks(tt,"%IECClim%","","%IECscen%","roadBridge","dmg_Cost_pctCap")))
    = sum(climHydroRoadBridge,  p_shocks(tt,"%IECClim%","","%IECscen%","roadBridge","dmg_Cost_pctCap"))
     /sum(climHydroRoadBridge $ p_shocks(tt,"%IECClim%","","%IECscen%","roadBridge","dmg_Cost_pctCap"),1);

$$endif.roadBridge

$$iftheni.inland_flood "%iecChnlsInland_flood%"=="ON"
*
*  --- damage from inland flooding (uses ssp1,ssp245 and ssp370 as labels, climType is empty)
*      No possibility to map to dry/hot or wet/warm directyl
*
   dmgStk.fx(cap,"inland_flood",tt) = p_shocks(tt,climInlandFlood,"%IECClimType%","%IECscen%","inland_flood","");

$$endif.inland_flood

$endif.decl
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
