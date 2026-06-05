********************************************************************************
$ontext

   Manages project

   GAMS file : RSCCDAMAGES.INC

   @purpose  : Map damages from Roson and Sartori to damage shifter
   @author   : W.Britz
   @date     : 02.03.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
  $$ifi "%RSCCDamages%"=="off" $exit

  ifDmg = 1;


  $$ifthen.decl "%1"=="decl"
*
*    --- define active CC damage channel based on selected Roson and
*        Sartori impacts
*
     $$onmulti
       set chnl / tempChangeRS /;
     $$offmulti

     set RSdamag1,RSdamag2,RSDamag3,rsDamag4;
     $$GDXIN '%datF%/cc_Damages.gdx'
         $$load  RSdamag1<p_ccDamages.Dim1
         $$load  RSdamag2<p_ccDamages.Dim2
         $$load  RSdamag3<p_ccDamages.Dim3
         $$load  RSdamag4<p_ccDamages.Dim4
     $$GDXIN

     set iDemShocked(i,RSDamag3);

     set iEnerDummy / c-ely,c-gas,c-p_c /;


     parameter p_ccDamages(rsDamag1,rsDamag2,rsDamag3,rsDamag4);
     execute_load '%datF%/cc_Damages.gdx' p_ccDamages

     $$ife  %tempIncreaseBaseYear%>%tempIncreaseSimYear% $abort "Temperature delta base year of %tempIncreaseBaseYear% > %tempIncreaseSimYear% for sim year",

     parameter p_tempDelta(tt) "Temperature increases in each simulation year";
     p_tempDelta(tt) = %tempIncreaseBaseYear% + (%tempIncreaseSimYear%-%tempIncreaseBaseYear%) * (tt.pos-1)/(card(tt)-1);

     $$macro m_ccDamage(reg,l1,l2,tempDelta)  [  p_ccDamages(reg,l1,l2,"x0")    \
                                               + p_ccDamages(reg,l1,l2,"x1") * tempDelta   \
                                               + p_ccDamages(reg,l1,l2,"x2") * tempDelta**2   \
                                               + p_ccDamages(reg,l1,l2,"x3") * tempDelta**3 ]
  $$else.decl
*
     iDemShocked(ielec,"ely") = YES;
     iDemShocked(i,"gas") $ sum(mapis(i0,i),mapcGTAP("gas",i0)) = YES;
     iDemShocked(i,"p_c") $ sum(mapis(i0,i),mapcGTAP("p_c",i0)) = YES;
*
*    --- shifters from Roson and Sartori
*
     $$iftheni.labProd "%useRosonSartoriLabProd%"=="on"

          dmgPrd.fx(l,agr(a),"tempChangeRS",tt)  = m_ccDamage("%regGtapName%","dmgLambdaL","agr",p_tempDelta(tt))
                                                 - m_ccDamage("%regGtapName%","dmgLambdaL","agr",p_tempDelta("%baseYear%"));
     $$endif.labProd

     $$iftheni.landStock "%useRosonSartoriLandStock%"=="on"

          dmgSup.fx(lnd,"tempChangeRS",tt) = m_ccDamage("%regGtapName%","dmgAtLAnd","lnd",p_tempDelta(tt))
                                            - m_ccDamage("%regGtapName%","dmgAtLAnd","lnd",p_tempDelta("%baseYear%"));
     $$endif.landStock

     $$iftheni.energDemand "%useRosonSartoriEnergyDemand%"=="on"

          dmgDem.fx(h,i,kNest,"tempChangeRS",tt) $ p_af(h,i,knest,tt)
                                              = sum(iDemShocked(i,RSdamag3),
                                                    m_ccDamage("%regGtapName%","dmgLambdaEh",rsDamag3,p_tempDelta(tt))
                                                  - m_ccDamage("%regGtapName%","dmgLambdaEh",rsDamag3,p_tempDelta("%baseYear%")));

     $$endif.energDemand

     $$iftheni.tourism "%useRosonSartoriTourism%"=="on"

          dmgTrd.fx(ctour,"tempChangeRS",tt)
                                            = m_ccDamage("%regGtapName%","dmgChie","tou",p_tempDelta(tt))
                                            - m_ccDamage("%regGtapName%","dmgChie","tou",p_tempDelta("%baseYear%"));
     $$endif.tourism

     $$iftheni.cropYields "%useRosonSartoriCropYields%"=="on"

          dmgPrd.fx("f-top",agr(a),"tempChangeRS",tt)  = m_ccDamage("%regGtapName%","dmgTfpAct","agr",p_tempDelta(tt))
                                                       - m_ccDamage("%regGtapName%","dmgTfpAct","agr",p_tempDelta("%baseYear%"));
     $$endif.cropYields

     $$iftheni.health "%useRosonSartoriHealth%"=="on"
*
*        --- unclear, only a constant term?
*
     $$endif.health

  $$endif.decl

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
