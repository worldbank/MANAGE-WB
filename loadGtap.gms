********************************************************************************
$ontext

   MANAGE project

   GAMS file : LOADGTAP.GMS

   @purpose  : Load GTAP data from zip container and convert to
               GTAPDatax.gdx and GTAPGhgx.gdx
   @author   : W.Britz
   @date     : 13.10.24
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : -

$offtext
********************************************************************************
 $$SETENV GDXCOMPRESS 1
 $$setglobal pgmname LoadGtap
 $$offlisting
 $$onglobal
 $$include 'loadGtap_inc.gms'
 $$include 'inc/title_def.inc'
 $$if not setglobal scrdir $setglobal scrdir %gams.scrdir%
 $$if "%scrdir%"==""       $setglobal scrdir %gams.scrdir%

 $$setglobal sl %system.dirsep%
 $$setglobal baseDir .
 $$if setglobal curDir $setglobal baseDir %curDir%
 $$setGlobal savF  "%baseDir%%system.dirsep%sav"
 $$setGlobal datF  "%baseDir%%system.dirsep%dat"
*
* -- unzip GTAP-Data base into sav directory
*
  $$setglobal sl %system.dirsep%
  $$call gmsunzip  -j -o "%GTAPZip%"  -d "%savF%"
  $$call mv -f "%savF%%sl%GSDFDAT.gdx"  "%savF%%sl%GSDFDAT_GTAP.gdx"
  $$call mv -f "%savF%%sl%GSDFNCO2.gdx" "%savF%%sl%GSDFNCO2_GTAP.gdx"

  $$call gmsunzip  -j -o "%AEZZip%"   -d "%savF%"
  $$call mv -f "%savF%%sl%GSDFDAT.gdx" "%savF%%sl%GSDFDAT_AEZ.gdx"
*
* --- build GTAP data in same format as for GTAP 10
*
  parameter tvom,vdfa,vdfm,vdga,vdgm,vdpa,vdpm,vfm,vifa,vifm,viga,vigm,vipa,vipm,
             vxmd,viws,vxwd,evoa,ptax,ftrv,fbep,vst,vtwr,vims,vdep,evos,evoa,osep,vdip,vdib,vmip,vmib;
  set ag,cg,fg,rg;
  alias(ag,ag1);
  $$GDXIN "%savF%%sl%GSDFDAT_GTAP.gdx"
    $$LOAD tvom=makb,vdfa=vdfp,vdfm=vdfb,vdga=vdgp,vdgm=vdgb,vdpa=vdpp,vdpm=vdpb,vfm=evfb,vifa=vmfp,vifm=vmfb,viga=vmgp,vigm=vmgb,vipa=vmpp,vipm=vmpb,vdip,vdib,vmip,vmib
    $$LOAD           vxmd=vxsb,vxwd=vfob,viws=vcif,evos,ptax,ftrv,fbep,vst,vims=vmsb,vdep
    $$LOAD ag=acts,cg=comm,fg=endw,rg=reg
  $$GDXIN
*
* --- determine GTAP Version
*
  $$setglobal postFix 11c
  $$ife card(cg)<65 $$setglobal postFix 9
  $$ife card(cg)>80 $$setglobal postFix 11CE

  evoa(fg,rg) = sum(cg, evos(fg,cg,rg));
  osep(ag,rg) $ card(ptax) = -sum(cg,ptax(cg,ag,rg));

  vifa(cg,"cgds",rg) = vmip(cg,rg);
  vifm(cg,"cgds",rg) = vmib(cg,rg);
  vdfa(cg,"cgds",rg) = vdip(cg,rg);
  vdfm(cg,"cgds",rg) = vdib(cg,rg);


  alias(rg,rg1);
  vims(cg,rg,rg1) $ (vims(cg,rg,rg1)  eq viws(cg,rg,rg1)) = 0;
  vxmd(cg,rg,rg1) $ (vxmd(cg,rg,rg1)  eq vxwd(cg,rg,rg1)) = 0;
*
* --- construct land data base at national level from AEZ
*
  set aez_set,lcov;


  $$GDXIN "%savF%%sl%GSDFDAT_AEZ.gdx"
    $$load aez_set=aezs
    $$load lcov=covs
  $$GDXIN
  $$onmulti
  set lcov / unManagedForest,forest /;

  set fg / set.aez_set /;
  set aez(fg) / set.aez_set /;
  alias(aez,aez1);
  $$offmulti

  set carbon /"carbon"/;
*
* --- aggregate hectares for crops over AEZ
*
  parameter sgha(aez,ag,rg)          "Harvested hectares"
            sglc(aez,lcov,rg)         "Land cover data in ha"
            aVfm(fg,ag,rg)            "Land returns at factor prices at aez level"
            aEVFa(fg,ag,rg)            "Land returns at factor prices at aez level"
            aFtrv(fg,ag,rg)
            aFBep(fg,ag,rg)
            p_land(rg,*,*)            "Land use parameter for MANAGE-WB"
            p_carbon(rg,aez,lCov,*)   "Per ha carbon stocks"
  ;
  $$ifthene.V9 card(cg)<65
     execute_load "%savF%%sl%GSDFDAT_AEZ.gdx" sgha=area,sglc=lcov,avfm=evfb,aEvfa=evfp;
  $$else.V9
     execute_load "%savF%%sl%GSDFDAT_AEZ.gdx" sgha=area,sglc=lcov,avfm=evfb,aFtrv=ftrv,aFBep=fbep;
  $$endif.V9

  sglc(aez,"forest",rg)     =  sglc(aez,"forestLand",rg);
  sglc(aez,"forestLand",rg) =  0;

  execute_load "%datF%%sl%carbon.gdx" p_carbon;

  p_carbon(rg,aez,"pastureLand","carbon") = min(250,p_carbon(rg,aez,"pastureLand","carbon"));
  p_carbon(rg,aez,"pastureLand","carbon") $ p_carbon(rg,aez,"forest","carbon") = min(p_carbon(rg,aez,"forest","carbon")*0.75,p_carbon(rg,aez,"pastureLand","carbon"));

  p_carbon(rg,aez,"savnGrasLand","carbon") = p_carbon(rg,aez,"pastureLand","carbon");
  p_carbon(rg,aez,"shrubLand","carbon")    = p_carbon(rg,aez,"pastureLand","carbon");
*
* --- assign area if a return is given
*
  p_land(rg,aez,ag) $ avfm(aez,ag,rg) = sgha(aez,ag,rg);
*
* --- remove cropsland if no returns from animals
*
  set cropAg(ag) /
      pdr
      wht
      gro
      v_f
      osd
      c_b
      pfb
      ocr
  /;
  alias(cropAg,cropAg1);


*
* --- remove crop land cover if no returns to crops
*
  sglc(aez,"cropLand",rg)   $ (not sum(cropAg, avfm(aez,cropAg,rg))) = 0;
*
* --- remove forest cover if no forest returns
*
  sglc(aez,"forest",rg)     $ (not avfm(aez,"frs",rg)) = 0;
*
* --- remove pastureland land if no returns from animals
*
  set animAg(ag) / ctl,rmk,wol,oap /;
  alias(animAg,animAg1);
*
  set lndAg(ag) / set.cropAg,set.animAg,frs /;
*
* --- assign land ocer data
*
  p_land(rg,aez,lcov)  = sglc(aez,lcov,rg);
*
* --- calculate unmanaged forest residually
*
  set tot /"tot"/;
  parameter p_area(rg,*);
  execute_load "%datF%%sl%aezArea_v11.gdx" p_area;
*
* --- calculate average carbon contents
*
  parameter p_carb(*,*,lcov);
  option kill=p_carb;
*
* --- assign forest cover to frs
*
  p_land(rg,aez,"frs") $ avfm(aez,"frs",rg) = p_land(rg,aez,"forest");
*
* --- overwrite parameters relating to factors return for land-use activities
*     with AEZ daa
*
  $$ifthene.V9 card(cg)<65
     set fCopy(fg); fCopy(fg) $ sum( (lndAg,rg), avfm(fg,lndAg,rg)) = YES;
     vfm(fg,lndAg,rg)  $ fCopy(fg)  = avfm(fg,lndAg,rg);
     ftrv(fg,lndAg,rg) $ fCopy(fg)  = aEvfa(fg,lndAg,rg)-avfm(fg,lndAg,rg);
     fbep(fg,lndAg,rg) $ fCopy(fg)  = 0;
  $$else.V9
     vfm(fg,lndAg,rg)  = avfm(fg,lndAg,rg);
     ftrv(fg,lndAg,rg) = aftrv(fg,lndAg,rg);
     fbep(fg,lndAg,rg) = afbep(fg,lndAg,rg);
  $$endif.V9

  vfm("land",lndAg,rg)  = sum(aez,vfm(aez,lndAg,rg));
  ftrv("land",lndAg,rg) = sum(aez,ftrv(aez,lndAg,rg));
  fbep("land",lndAg,rg) = sum(aez,fbep(aez,lndAg,rg));
*
* --- next use share on VFM to distribute pastureland
*
  p_land(rg,aez,animAg) $  avfm(aez,animAg,rg)
   = p_land(rg,aez,"pastureLand") * avfm(aez,animAg,rg)/sum(animAg1,avfm(aez,animAg1,rg));
*
* --- if avfm is given, but not p_land for a crops, construct, assuming the same land rents as the average of the crops
*
  p_land(rg,aez,cropAg) $ ((not p_land(rg,aez,cropAg)) and avfm(aez,cropAg,rg)
                            $ sum(cropAg1 $ (p_land(rg,aez,cropAg1) and avfm(aez,cropAg1,rg)),avfm(aez,cropAg1,rg)))
         =  avfm(aez,cropAg,rg) * sum(cropAg1 $ (p_land(rg,aez,cropAg1) and avfm(aez,cropAg1,rg)),p_land(rg,aez,cropAg1))
                             /sum(cropAg1 $ (p_land(rg,aez,cropAg1) and avfm(aez,cropAg1,rg)),avfm(aez,cropAg1,rg));
*
* --- scale areas to exhaust given crop land cover
*
  p_land(rg,aez,cropAg) $ p_land(rg,aez,cropAg)
    = p_land(rg,aez,cropAg) * p_land(rg,aez,"cropLand")/sum(cropAg1, p_land(rg,aez,cropAg1));
*
* --- for other activities, use average land rent over all activities if no land cover information,
*     but returns
*
  p_land(rg,aez,ag) $ ((not p_land(rg,aez,ag)) and avfm(aez,ag,rg)
                            $ sum(ag1 $ (p_land(rg,aez,ag1) and avfm(aez,ag1,rg)),avfm(aez,ag1,rg)))
         =  avfm(aez,ag,rg) * sum(ag1 $ (p_land(rg,aez,ag1) and avfm(aez,ag1,rg)),p_land(rg,aez,ag1))
                             /sum(ag1 $ (p_land(rg,aez,ag1) and avfm(aez,ag1,rg)),avfm(aez,ag1,rg));


  p_land(rg,aez,"forest") = p_land(rg,aez,"frs");
*
* --- residual is unmanaged forest
*
  p_land(rg,aez,"unManagedForest")  = max(0,p_area(rg,aez)*0.1       - sum(lcov,p_land(rg,aez,lcov)));
  sglc(aez,"unManagedForest",rg)    = p_land(rg,aez,"unManagedForest");

  p_land(rg,"",ag)    = sum(aez, p_land(rg,"",ag));
  p_land(rg,"",lCov)  = sum(aez, p_land(rg,"",lCov));

  p_carb("wor",aez,lCov) $ sum(rg $ p_carbon(rg,aez,lcov,"carbon"),p_land(rg,aez,lcov))
   = sum(rg,p_land(rg,aez,lcov)*p_carbon(rg,aez,lcov,"carbon"))/sum(rg $ p_carbon(rg,aez,lcov,"carbon"),p_land(rg,aez,lcov));

  p_carbon(rg,aez,lCov,"carbon") $ ((not p_carbon(rg,aez,lcov,"carbon")) $ p_land(rg,aez,lcov)) = p_carb("wor",aez,lCov);

  p_carb(rg,aez,lcov) $ p_land(rg,aez,lcov) = p_carbon(rg,aez,lcov,"carbon");
  p_carb(rg,"",lcov)  $ p_land(rg,"",lcov)  = sum(aez,sglc(aez,lcov,rg)*p_carbon(rg,aez,lcov,"carbon"))/p_land(rg,"",lcov);
*
* --- build GTAP emissions data for 11a in same format as gtapGHG.gdx
*
  parameter mdf,mdg,mdp,mif,mig,mip;
  $$GDXIN "%savF%%sl%GSDFEmiss.gdx"
    $$LOAD mdf,mdg,mdp,mif=mmf,mig=mmg,mip=mmp
  $$GDXIN

  set NCO2,ars,ema;
  $$gdxin "%savF%%sl%gsdfnco2_GTAP.gdx"
     $$iftheni.version "%PostFix%"=="9"
        $$load nCO2=EM
        set ars / AR4 /;
        set ema /""/;
     $$else.version
        $$load nCO2=GHG
        $$load ema=NGHG
        $$load ars=ar
     $$endif.version
  $$gdxin

  parameter   emi_IOP(nco2,cg,ag,rg)      "GHG process related emissions from combustion"
              GWP(NCO2,rg,ARS)            "GWP potentials of different gases"
              nc_endw_ceq(nCo2,fg,ag,rg)  "Non-CO2 GHG emissions related to factor use"
              nc_qo_ceq(nCo2,ag,rg)       "Non-CO2 GHG emissions related to output"
              nc_hh_ceq(nCo2,cg,rg)       "Non-CO2 GHG emissions related to private demand"
              nc_trad_ceq(nCo2,cg,ag,rg)  "Non-CO2 GHG emissions related to intermediate use"

              ape(ema,fg,ag,rg)           "Air pollutant emissions related to factor use"
              apo(ema,ag,rg)              "Air pollutant emissions related to output"
              app(ema,cg,rg)              "Air pollutant emissions related to private demand"
              apf(ema,cg,ag,rg)           "Air pollutant emissions related to intermediate use"
    ;

  parameter nc_endw_ceq,nc_hh_ceq,nc_qo_ceq,nc_trad_ceq;
  singleton set AR(Ars) / AR4 /;
  $$iftheni.version "%postfix%"=="9"
         execute_load "%savF%%sl%gsdfnco2_GTAP.gdx"  nc_endw_ceq=emi_endw,nc_hh_ceq=emi_hh,nc_qo_ceq=emi_qo,nc_trad_ceq=emi_io;
         gwp(nco2,rg,ar) = 1;
         option kill=emi_iop;
  $$else.version
         execute_load "%savF%%sl%gsdfnco2_GTAP.gdx"  nc_endw_ceq=emi_endw,nc_hh_ceq=emi_hh,nc_qo_ceq=emi_qo,nc_trad_ceq=emi_io,emi_iop,gwp;
  $$endif.version

*
* --- convert to CO2 equivalents (and add process emisisons from combustion)
*
  nc_trad_ceq(nCo2,cg,ag,rg) = nc_trad_ceq(nCo2,cg,ag,rg) * gwp(nco2,rg,ar) + emi_iop(nco2,cg,ag,rg) * gwp(nco2,rg,ar);
  nc_qo_ceq(nCo2,ag,rg)      = nc_qo_ceq(nCo2,ag,rg)      * gwp(nco2,rg,ar);
  nc_endw_ceq(nCo2,fg,ag,rg) = nc_endw_ceq(nCo2,fg,ag,rg) * gwp(nco2,rg,ar);
  nc_hh_ceq(nCo2,cg,rg)      = nc_hh_ceq(nCo2,cg,rg)      * gwp(nco2,rg,ar);
*
* --- Air emissions (comprised in non-CO2 emissions)
*
  execute_load "%savF%%sl%gsdfnco2_GTAP.gdx"  ape=emi_endw,app=emi_hh,apo=emi_qo,apf=emi_io;
*
* --- CO2 emissions from combustion
*
  parameter edf,edp,eif,eip;
  $$GDXIN "%savF%%sl%GSDFVole.gdx"
    $$LOAD edf,edp,eif=emf,eip=emp
  $$GDXIN

*
* ---- build nutrients data base
*
  set newGtap11 / CAF,TCD,COG,COD,GNQ,GAB,COM,SDN,DZA,SWZ,IRQ,LBN,PSE,SYR,MLI,NER,HTI,UZB,TJK,AFG,SRB/
  set oldGtap10 / XAC,XCF,XNF,XEC,XWS,XCB,XSU,XSC,XWF,XER,XSA/;
  set G11_10(newGtap11,oldGtap10)/ HTI.XCB
                                    SRB.XER
                                    AFG.XSA
                                    (UZB,TJK).XSU
                                    (CAF,TCD,COG,GNQ,GAB).XCF
                                    COD.XAC
                                    (MLI,NER).XWF
                                    (COM,SDN).XEC
                                    DZA.XNF
                                    SWZ.XSC
                                    (IRQ,LBN,PSE,SYR).XWS/;


  set nutr / cal,prot,fat /;
  parameter p_calFab(*,*,nutr);
  execute_load "%datF%%sl%calories.gdx" p_calFab;

  p_calFab(newGtap11,ag,nutr) = sum(G11_10(newGtap11,oldGtap10),p_calFab(oldGtap10,ag,nutr));
  p_calFab(oldGtap10,ag,nutr) $ (not sum(sameas(oldGtap10,rg),1)) = 0;

  set nutlong / calorie,protein /;
  parameter p_nutrients(rg,*);
  execute_load "%datF%%sl%nutrients_iso.gdx" p_nutrients;

  p_calFab(rg,"sum","fat")  = p_nutrients(rg,"fat");
  p_calFab(rg,"sum","cal")  = p_nutrients(rg,"calorie");
  p_calFab(rg,"sum","prot") = p_nutrients(rg,"protein");

  alias(rg,regGtap);
  alias(ag,aGtap);
  alias(cg,cGtap);
  alias(fg,endwGtap);


  execute_unload "%datf%%sl%gtapData%GtapDataSuffix%.gdx"
     ag,cg,fg,rg,
     tvom,vdfa,vdfm,vdga,vdgm,vdpa,vdpm,vfm,vifa,vifm,viga,vigm,vipa,vipm,
     vxmd,viws,vxwd,evoa,osep,ftrv,fbep,vst,vims,vdep,
     p_land,lcov,p_carb,aez,
     mdf,mdg,mdp,mif,mig,mip,
     ape,apf,apo,app,ema
     edf,edp,eif,eip
     nc_endw_ceq,nc_hh_ceq,nc_qo_ceq,nc_trad_ceq,
     aGtap,cGTAP,regGTap,endwGtap
     p_calFab=p_nutrients;

