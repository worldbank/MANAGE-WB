********************************************************************************
$ontext

   MANAGE project

   GAMS file : INIEmis.inc

   @purpose  : Initialize emissions from GTAP data base

   @author   : MANAGE team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms
                                                                                                                                                                  x
$offtext
********************************************************************************

  reEscale = 1;
  $$iftheni.useGTAPEmis "%skipGTAPEmis%"=="off"

     $$ifthen.existGTAPGHG exist "%datF%/%version%.gdx"

*       --- Load data and define sets/parameters to Calibrate emissions to reproduce GHG inventory

        set ghgReporter,ghgYear;
        $$gdxin "%datF%/GHG.gdx"
           $$load ghgInv
           $$load ghgReporter<p_ghgInv.dim1
           $$load ghgYear    <p_ghgInv.dim5
        $$gdxin

        Parameters
           GHGBase(ghgReporter,regGtap,ghginv,em,ghgYear)    "Emission inventory from CAIT (UNFCC), enrichwed with Climate Watch detail as found in CAIT"
           emiInventory(ghgInv,em)                      "Emission inventory for country and year"
           emiInventory_OFF(ghgInv,em)                  "Emission inventory from country authorities, to overwrite data from CAIT"
        ;

        $$gdxin %readFile%
           $$load   emiInventory_OFF
        $$gdxin %readFile%
        execute_load "%datf%/ghg.gdx" GHGBase=p_ghgInv;

        Sets
            em         "GTAP emission types"
            endwGTAP   "Endowment Commodities in GTAP"
         ;

        Parameters

           MDF(cGTAP,aGTAP,regGTAP)                 "Emissions from firms (intermediate) consumption of domestic products MTCO2e"
           MDG(cGTAP,regGTAP)                       "Emissions from government consumption of domestic products MTCO2e"
           MDP(cGTAP,regGTAP)                       "Emissions from private consumption of domestic products MTCO2e"
           MIF(cGTAP,aGTAP,regGTAP)                 "Emissions from firms (intermediate) consumption of import products MTCO2e"
           MIG(cGTAP,regGTAP)                       "Emissions from government consumption of import products MTCO2e"
           MIP(cGTAP,regGTAP)                       "Emissions from private consumption of import products MTCO2e"
           EIP(cGTAP,regGTAP)                       "Use of imported energy carries by households in MTOE"
           EDP(cGTAP,regGTAP)                       "Use of domestic energy carries by households in MTOE"
           EIF(cGTAP,aGTAP,regGTAP)                 "Use of imported energy carries by firms in MTOE"
           EDF(cGTAP,aGTAP,regGTAP)                 "Use of domestic energy carries by firms in MTOE"
           NC_ENDW_CEQ(em,endwGTAP,aGTAP,regGTAP)   "Non-co2 emissions from endowment use"
           NC_HH_CEQ(em,cGTAP,regGTAP)              "Non-co2 emissions from private consumption"
           NC_QO_CEQ(*,aGTAP,regGTAP)               "Non-co2 emissions from production"
           NC_TRAD_CEQ(em,cGTAP,aGTAP,regGTAP)      "Non-co2 emissions from input use"

           AirPE(em,endwGTAP,aGTAP,regGTAP)         "Air pollution from endowment use"
           AirPF(em,cGTAP,aGTAP,regGTAP)            "Air pollution from input use"
           AirPO(em,aGTAP,regGTAP)                  "Air pollution from production activity"
           AirPP(em,cGTAP,regGTAP)                  "Air pollution from consumption"

           mtoeGTAP(cGtap,*)                        "Energy carrier use in MTOE (million tons of oil equivalents), GTAP commodities and accounts"
           emiGTAPOri(em,*,*)                       "MtCo2e and air emissions per Million USD, in GTAP definitions"

           mtoe0(is0,js0)                           "Energy carrier use in MTOE (million tons of oil equivalents)"
           mtoe(is,js)                              "Energy carrier use in MTOE (million tons of oil equivalents)"
        ;

        set fgases;
        $$gdxin %datF%/%version%.gdx
           $$loaddc endwGTAP
           $$load   fgases<NC_QO_CEQ.dim1
           $$load   NC_TRAD_CEQ NC_ENDW_CEQ NC_HH_CEQ NC_QO_CEQ
           $$loaddc MDF MDG MDP MIF MIG MIP EDP EIP EDF EIF
           $$loaddc AirPE=APE AirPF=APF AirpO=APO AirPP=APP
        $$gdxin
*
*       --- define list of detailed f-gases
*
        fgases("Co2") = no;
        fgases("CH4") = no;
        fgases("N2O") = no;
*
*       --- aggregate single f-ages to f-gas total
*
        nc_qo_ceq("f-gas",aGtap,regGtap)  = sum(fgases,nc_qo_ceq(fgases,aGtap,regGtap));
        nc_qo_ceq(fgases,aGtap,regGtap) = 0;
*
*       --- GTAP non-CO2 and air emissions linked to product use
*
        emiGTAPOri(em,cGTAP,aGTAP)
                =   NC_TRAD_CEQ(em,cGTAP,aGTAP,"%RegGTAPName%")
                  + AirPF(em,cGTAP,aGTAP,"%RegGTAPName%");
*
*       --- GTAP non-CO2 and air emissions linked to household consuption
*
        emiGTAPOri(em,cGTAP,"h0")
                =   NC_HH_CEQ(em,cGTAP,"%RegGTAPName%")
                  + AirPP(em,cGTAP,"%RegGTAPName%");
*
*        --- emissions linked to production
*             (includeed land and capital in previous version, this is a double counting?)
*
        emiGTAPOri(em,"emixp",aGTAP)
                =    NC_QO_CEQ(em,aGTAP,"%RegGTAPName%")
                   + AirPO(em,aGTAP,"%RegGTAPName%");
*
*        --- emissions linked to land and capital use
*
        emiGTAPOri(em,"lnd0",aGTAP)
                  =   NC_ENDW_CEQ(em,"Land",aGTAP,"%RegGtapName%")
                    + AirPE(em,"Land",aGTAP,"%RegGtapName%");

        emiGTAPOri(em,"cap0",aGTAP)
                =   NC_ENDW_CEQ(em,"Capital",aGTAP,"%RegGtapName%")
                  + AirPE(em,"Capital",aGTAP,"%RegGtapName%");
*
*       --- emissions from fossil fuel use
*
        emiGTAPOri("co2",cGTAP,aGTAP)  = MDF(cGTAP,aGTAP,"%RegGTAPName%") + MIF(cGTAP,aGTAP,"%RegGTAPName%");
        emiGTAPOri("co2",cGTAP,"h0")   = MDP(cGTAP,"%RegGTAPName%")       + MIP(cGTAP,"%RegGTAPName%");
        emiGTAPOri("co2",cGTAP,"gov0") = MDG(cGTAP,"%RegGTAPName%")       + MIG(cGTAP,"%RegGTAPName%");
*
*       --- mapping sets from GTAP activities and products to final activities and products
*
        set mapaGtapF(aGtap,a); mapaGtapF(aGtap,a) = sum((mapis(a0,a),mapaGtap(aGtap,a0)),1);
        set mapcGtapF(cGtap,i); mapcGtapF(cGtap,i) = sum((mapis(i0,i),mapcGtap(cGtap,i0)),1);

        emiGtapOri(em,cGtap,aGtap)    $ (not sum((mapcGtapF(cGtap,i),mapaGtapF(aGtap,a)),sam(i,a))) = 0;
        emiGtapOri(em,cGtap,"h0")     $ (not sum((mapcGtapF(cGtap,i),h),sam(i,h))) = 0;
        emiGtapOri(em,cGtap,"gov0")   $ (not sum((mapcGtapF(cGtap,i),gov),sam(i,gov))) = 0;
        emiGtapOri(em,"lnd0",aGtap)   $ (not sum((mapaGtapF(aGtap,a),lnd),sam(lnd,a))) = 0;
        emiGtapOri(em,"cap0",aGtap)   $ (not sum((mapaGtapF(aGtap,a),cap),sam(cap,a))) = 0;
        emiGtapOri(em,"emiXp0",aGtap) $ (not sum((mapaGtapF(aGtap,a),is),sam(a,is))) = 0;


        parameter p_emiComp;
*
*       --- aggregate original GTAP emission by group of agents
*
        p_emiComp(em,"xp","GTAP")   = sum(aGTAP, emiGTAPOri(em,"emixp",aGTAP));
        p_emiComp(em,"h","GTAP")    = sum(cGTAP, emiGTAPOri(em,cGtap,"h0"));
        p_emiComp(em,"gov","GTAP")  = sum(cGTAP, emiGTAPOri(em,cGtap,"gov0"));
        p_emiComp(em,"int","GTAP")  = sum((cGTAP,aGtap), emiGTAPOri(em,cGtap,aGtap));
        p_emiComp(em,"lnd","GTAP")  = sum(aGtap, emiGTAPOri(em,"lnd0",aGtap));
        p_emiComp(em,"cap","GTAP")  = sum(aGtap, emiGTAPOri(em,"cap0",aGtap));
*
*       --- use of energy carriers in MTOE
*
        mtoeGTAP(cGtap,aGtap) = EDF(cGTAP,aGTAP,"%RegGTAPName%") + EIF(cGTAP,aGTAP,"%RegGTAPName%");
        mtoeGTAP(cGtap,"h0")  = EDP(cGTAP,"%RegGTAPName%")       + EIP(cGTAP,"%RegGTAPName%");

     $$else.existGTAPGHG

        set gtapDummy / h0,gov0,lnd0,cap0/;
        execute_load "%regf%/addData_%regGtapName%.gdx" emiGtapOri,mtoeGtap;

     $$else.existGTAPGHG

        $$abort "No data file found to read GTAP emissions from, file: %system.fn%, line: %system.incline%."

     $$endif.existGTAPGHG
*
*    --- assign CAIT inv to emiInventory with data from bridgefile if given
*
     set emiYear(ghgYear);
     alias(ghgYear,ghgYear1);
     emiYear(ghgYear) $ (abs(ghgYear.val-%baseYear%) eq smin(ghgYear1, abs(ghgYear1.val-%baseYear%))) = YES;

     emiInventory(ghgInv,em)                               = sum(emiYear,GHGBase("CAIT","%RegGTAPName%",ghginv,em,emiYear))/card(emiYear);
     emiInventory(ghgInv,em) $ emiInventory_OFF(ghgInv,em) = emiInventory_OFF(ghgInv,em);

     set usrGtap "GTAP accounts which emit GHGs"                   / set.aGtap,h0,gov0 /;
     set srcGtap "Emission source: product, land, capital, output" / set.cGtap,lnd0,cap0,emiXp /;

     positive variables
        v_ghgInvAlloc(ghgInv,em,srcGtap,usrGtap) "Allocation keys to distribute GTAP emissions to inventories"
     ;
     variable
        v_hpdGhgInv                      "Highest Posterior Density of inventory allocation"
     ;
     equations
        e_ghgInv(ghgInv,em)              "Recover given inventories by gas"
        e_hpdGhgInv                      "Penalty function"
     ;
*
*    --- The GTAP emission accounts are distributed with the endogenous allocation keys such that to
*        exactly exhaust the given inventories
*
     e_ghgInv(ghgInv,em) $ ((not (sameas(ghgInv,"LULUCF") and sameas(em,"CO2"))) $ emiInventory(ghgInv,em)) ..

        emiInventory(ghgInv,em) =E= sum( (srcGtap,usrGTap) $ v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap),
                                                             v_ghgInvAlloc(ghgInv,em,srcGtap,usrGtap) * emiGtapOri(em,srcGtap,usrGtap));
*
*    --- Penalty to minize: Relative deviation between a-priori allocation keys and final ones
*
     e_hpdGhgInv ..

       v_hpdGhgInv * sum( (ghginv,em,srcGtap,usrGTap) $ v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap),1)
          =E= sum( (ghgInv,em,srcGtap,usrGTap) $ v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap),
                       sqr[v_ghgInvAlloc(ghgInv,em,srcGtap,usrGtap)/v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap)-1])
*
*         --- try to keep original GTAP emissions: allocation keys should add up to unity
*
          + sum( (em,srcGtap,usrGTap) $ emiGtapOri(em,srcGtap,usrGtap),
                       sqr(sum(ghgInv $ v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap),
                                    v_ghgInvAlloc(ghgInv,em,srcGtap,usrGtap))-1))*10;

     model m_ghgInv / e_ghgInv,e_hpdGhgInv /;

     set allSec(usrGtap)   / set.aGtap/;
     set animUsr(usrGtap)  / set.GtapAnim /;
     set cropsUsr(usrGtap) / set.GtapCrops/;
     set agrGtap(usrGtap)  / set.GtapCrops,set.GtapAnim /;
     set fossil(srcGtap)   / coa,oil,gas /;
     set energy(srcGtap)   / set.fossil,gdt /;
     set fugiSect(usrGtap) / coa,gas,oil,gdt,p_c /;
     set process(srcGtap)  / lnd0,cap0,emiXP /;
     set manuGtap(usrGtap) / set.manuGtapA /;
     set elecGtap(usrGtap) / set.elecGtapA /;
     set servGtap(usrGtap);

     set tnd / tnd /;

     elecGtap(allSec)  $ sameas(allSec,"tnd") = YES;
     servGtap(allSec) = YES;
     servGtap(animUsr) = no;
     servGtap(agrGtap) = no;
*
*    --- Water and waste account is responsible for waste inventory
*
     v_ghgInvAlloc.m("waste",em,srcGtap,"wtr")   $ emiGtapOri(em,srcGtap,"wtr") = 1;
*
*    --- Agricultural emissions: non-CO2 from animals and crops
*
     v_ghgInvAlloc.m("agri",em,"lnd0",agrGtap)   $ emiGtapOri(em,"lnd0",agrGtap)   = 1;
     v_ghgInvAlloc.m("agri",em,"cap0",agrGtap)   $ emiGtapOri(em,"cap0",agrGtap)   = 1;
     v_ghgInvAlloc.m("agri",em,"emiXP",agrGtap)  $ emiGtapOri(em,"emiXP",agrGtap)  = 1;
*
     v_ghgInvAlloc.m("agri","co2",srcGtap,agrGtap) $ emiGtapOri("co2",srcGtap,agrGtap) = 0.10;
*
*    --- emissions from fertilizer use and forestry use (?)
*
     set chm1(srcGtap) /
        $$iftheni.GTAPVers "%sysEnv.GTAP9%"=="on"
           crp
        $$elseifi.GTAPVers "%sysEnv.GTAPCE%"=="on"
           kfr,nfr,pfr,xch,rbr,plp,pls,plr
        $$else.GTAPVers
           chm
        $$endif.GTAPVers
     /;

     v_ghgInvAlloc.m("lulucf",em,chm1,agrGtap)     $ emiGtapOri(em,chm1,agrGtap)   = 1;
     v_ghgInvAlloc.m("lulucf",em,"frs",usrGtap)    $ emiGtapOri(em,"frs",usrGtap)  = 1;
     v_ghgInvAlloc.m("lulucf",em,"lnd0",agrGtap)   $ emiGtapOri(em,"lnd0",agrGtap) = 0.01;
*
*    --- Unclear which sectors are responsible for industrial processes and input use
*        For non-Co2
*
     v_ghgInvAlloc.m("indu",em,"emiXP",allSec)   $ ((not sameas(em,"Co2")) and emiGtapOri(em,"emiXp",allSec)) = 1;
     v_ghgInvAlloc.m("indu",em,"emiXP",fugiSect)                                                              = 0;
     v_ghgInvAlloc.m("indu",em,"emiXP",agrGTAP)                                                               = 0;
     v_ghgInvAlloc.m("indu",em,"emiXP","wtr")                                                                 = 0;
     v_ghgInvAlloc.m("indu",em,chm1,usrGtap)    $ emiGtapOri(em,chm1,usrGtap)                               = 1;
     v_ghgInvAlloc.m("indu",em,chm1,agrGtap)    $ emiGtapOri(em,chm1,agrGtap)                               = 0;
     $$iftheni.GTAP9 not "%sysEnv.GTAP9%"=="on"
        v_ghgInvAlloc.m("indu",em,"bph",usrGtap)    $ emiGtapOri(em,"bph",usrGtap)                               = 1;
     $$endif.GTAP9
*
*    --- CO2 emissions under industrial processes are cement:
*
     $$iftheni.GTAPVers "%sysEnv.GTAPCE%"=="on"
        v_ghgInvAlloc.m("indu","CO2","emiXP","cem")    $ emiGtapOri("co2","emiXp","cem")                         = 1;
     $$else.GTAPVers
        v_ghgInvAlloc.m("indu","CO2","emiXP","nmm")    $ emiGtapOri("co2","emiXp","nmm")                         = 1;
     $$endif.GTAPVers
*
*    --- emissions from domestic transport: rest of air and water transport, all of other transport
*        (cars, lorries, busses, trains, pipelines etc.)
*
     v_ghgInvAlloc.m("trans",em,srcGtap,"atp")  $ emiGtapOri(em,srcGtap,"atp")  = 1.00;
     v_ghgInvAlloc.m("trans",em,srcGtap,"wtp")  $ emiGtapOri(em,srcGtap,"wtp")  = 1.00;
     v_ghgInvAlloc.m("trans",em,srcGtap,"otp")  $ emiGtapOri(em,srcGtap,"otp")  = 1.00;
     v_ghgInvAlloc.m("trans",em,"p_c",allSec)   $ emiGtapOri(em,"p_c",allSec)   = 0.5;
     v_ghgInvAlloc.m("trans",em,"p_c",manuGtap) $ emiGtapOri(em,"p_c",manuGtap) = 0.25;
*
*    --- 50% of petroleum coke (diesel, gasoline) of household/gov emissions to transport
*
     v_ghgInvAlloc.m("trans",em,"p_c","h0")    $ emiGtapOri(em,"p_c","h0")    = 0.50;
     v_ghgInvAlloc.m("trans",em,"p_c","gov0")  $ emiGtapOri(em,"p_c","gov0")  = 0.50;
*
*    --- 1% of gas production and distribution of household/gov emissions to transport
*        (liquid natural gas in cares and similar)
*
     v_ghgInvAlloc.m("trans",em,"gdt","h0")    $ emiGtapOri(em,"gdt","h0")    = 0.01;
     v_ghgInvAlloc.m("trans",em,"gdt","gov0")  $ emiGtapOri(em,"gdt","gov0")  = 0.01;
*
*    --- Rest goes to buildings (heating)
*
     v_ghgInvAlloc.m("build",em,"p_c","h0")    $ emiGtapOri(em,"p_c","h0")    = 0.50;
     v_ghgInvAlloc.m("build",em,"p_c","gov0")  $ emiGtapOri(em,"p_c","gov0")  = 0.50;
     v_ghgInvAlloc.m("build",em,"gdt","h0")    $ emiGtapOri(em,"gdt","h0")    = 0.99;
     v_ghgInvAlloc.m("build",em,"gdt","gov0")  $ emiGtapOri(em,"gdt","gov0")  = 0.99;
*
*    --- Coal, gas, oil use of services (non-manufacturing, no power generation) to
*        to building. 5% for manufacturing and power generation
*
     v_ghgInvAlloc.m("build",em,energy,servGtap) $ emiGtapOri(em,energy,servGtap)  = 0.9;
     v_ghgInvAlloc.m("build",em,energy,agrGtap)  $ emiGtapOri(em,energy,agrGtap)   = 0.1;
     v_ghgInvAlloc.m("build",em,energy,manuGtap) $ emiGtapOri(em,energy,manuGtap)  = 0.05;

     v_ghgInvAlloc.m("build",em,energy,elecGtap) $ emiGtapOri(em,energy,elecGtap) = 0;
*
*    --- fossil use households/government to buildings
*
     v_ghgInvAlloc.m("build",em,fossil,"h0")      $ emiGtapOri(em,fossil,"h0")    = 1.00;
     v_ghgInvAlloc.m("build",em,fossil,"gov0")    $ emiGtapOri(em,fossil,"gov0")  = 1.00;
*
*    --- allocate emissions of electricity/heat sectors to elecHeat
*
     v_ghgInvAlloc.m("elecHeat",em,srcGtap,elecGtap)  $ emiGtapOri(em,srcGtap,elecGtap) = 1.00;
*
*    --- Manufacturing and construction: mostly to this account
*
     v_ghgInvAlloc.m("manuCons",em,srcGtap,manuGtap)  $ emiGtapOri(em,srcGtap,manuGtap) = 0.93;
*
*    --- Fugitives are mostly stemming from oil and gas extraction and coal mining
*
     v_ghgInvAlloc.m("fugi",em,process,fugiSect)      $ emiGtapOri(em,process,fugiSect)  = 0.99;
     v_ghgInvAlloc.m("manuCons",em,process,fugiSect)  $ emiGtapOri(em,process,fugiSect)  = 0.00;
*
*    --- keep power generation clean
*
     v_ghgInvAlloc.m("manuCons",em,srcGtap,elecGtap)  $ emiGtapOri(em,srcGtap,elecGtap)   = 0.00;
     v_ghgInvAlloc.m("fugi",em,srcGtap,elecGtap)      $ emiGtapOri(em,srcGtap,elecGtap)   = 0.00;
*
*    --- check for missing account and abort if found
*
     parameter notAlloc(em,srcGtap,usrGtap);
     notAlloc(em,srcGtap,usrGtap) $ ((not sum(ghgInv, v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap))) $ emiGtapOri(em,srcGtap,usrGtap))
        =  emiGtapOri(em,srcGtap,usrGtap);
     if (card(notAlloc), display "Warning: Not allocated GTAP emissions",notAlloc);

     if (sum(em $ [sum(ghgInv,emiInventory(ghgInv,em)) and sum((srcGtap,usrGtap),notAlloc(em,srcGtap,usrGtap))],1),
         Abort "Not allocated GTAP GHG emissions",notAlloc;
     );

*
*    --- kill allocation keys for not existing inventories
*
     v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap) $ (not emiInventory(ghgInv,em)) = 0;
*
*    --- scale a-priori information to unity
*
     alias(ghgInv,ghgInv1);
     v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap) $ v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap)
      = v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap)/sum(ghgInv1, v_ghgInvAlloc.m(ghginv1,em,srcGtap,usrGtap));

     $$iftheni.GTAP9 "%sysEnv.GTAP9%"=="on"
        emiInventory(ghgInv,em) $ (not sum((srcGtap,usrGtap),v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap))) = 0;
     $$endif.GTAP9
*
*    --- a priori-allocation keys are used as starting point
*
     parameter p_compGhgInv(*,*,*);
     if ( card(emiInventory),

        v_ghgInvAlloc.l(ghginv,em,srcGtap,usrGtap) $ emiInventory(ghgInv,em) = v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap);

        v_ghgInvAlloc.up(ghginv,em,srcGtap,usrGtap) $ emiInventory(ghgInv,em) = v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap)*1000;

        p_compGhgInv(ghgInv,em,"GTAP") = sum( (srcGtap,usrGTap),v_ghgInvAlloc.l(ghgInv,em,srcGtap,usrGtap)*emiGtapOri(em,srcGtap,usrGtap));
        m_ghgInv.limRow = 0;
        m_ghgInv.limCol = 0;
        m_ghgInv.holdFixed = 1;
        m_ghgInv.optfile   = 1;
*       m_ghgInv.iterlim   = 0;
        option nlp=CONOPT4;

        v_hpdGhgInv.l
              = [ sum( (ghgInv,em,srcGtap,usrGTap) $ v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap),
                           sqr[v_ghgInvAlloc.l(ghgInv,em,srcGtap,usrGtap)/v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap)-1])

                + sum( (em,srcGtap,usrGTap) $ emiGtapOri(em,srcGtap,usrGtap),
                           sqr(sum(ghgInv $ v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap),
                                        v_ghgInvAlloc.l(ghgInv,em,srcGtap,usrGtap))-1))*10 ]
               / sum( (ghginv,em,srcGtap,usrGTap) $ v_ghgInvAlloc.m(ghgInv,em,srcGtap,usrGtap),1);
*
*       --- there are few cases where CAIT reports non-CO2 emissions from ElecHeat, but GTAP not.
*           delete the inventory
*
        emiInventory("elecHeat",em) $ (not p_compGhgInv("elecHeat",em,"GTAP")) = 0;

        solve m_ghgInv using NLP minimizing v_hpdGhgInv;
*
*       --- after model solve: re-scale GTAP emissions such that they match inventories (based on updated allocation keys)
*
        emiGtapOri(em,srcGtap,usrGtap) $ sum(ghgInv,emiInventory(ghgInv,em))
         = sum(ghgInv, v_ghgInvAlloc.l(ghginv,em,srcGtap,usrGtap)*emiGtapOri(em,srcGtap,usrGtap));
*
*       --- scale allocation keys to unity (scaling done by statement above)
*
        v_ghgInvAlloc.l(ghginv,em,srcGtap,usrGtap) $ v_ghgInvAlloc.l(ghginv,em,srcGtap,usrGtap)
         = v_ghgInvAlloc.l(ghginv,em,srcGtap,usrGtap)/sum(ghgInv1, v_ghgInvAlloc.l(ghginv1,em,srcGtap,usrGtap));
*
*       --- report updated inventories by emission and user type
*
        p_emiComp(em,"xp","CAIT")   = sum(aGTAP, emiGTAPOri(em,"emixp",aGTAP));
        p_emiComp(em,"h","CAIT")    = sum(cGTAP, emiGTAPOri(em,cGtap,"h0"));
        p_emiComp(em,"gov","CAIT")  = sum(cGTAP, emiGTAPOri(em,cGtap,"gov0"));
        p_emiComp(em,"int","CAIT")  = sum((cGTAP,aGtap), emiGTAPOri(em,cGtap,aGtap));
        p_emiComp(em,"lnd","CAIT")  = sum(aGtap, emiGTAPOri(em,"lnd0",aGtap));
        p_emiComp(em,"cap","CAIT")  = sum(aGtap, emiGTAPOri(em,"cap0",aGtap));

        option p_emiComp:7;
        display p_emiComp;

     else
*
*       --- there are a few cases such as for Hong-Kong where CAIT does not report an inventory. We construct the
*           inventories from the GTAP data base using the assummed allocation keys
*
        v_ghgInvAlloc.l(ghginv,em,srcGtap,usrGtap) $ emiInventory(ghgInv,em) = v_ghgInvAlloc.m(ghginv,em,srcGtap,usrGtap);
     );


     alias(a,a1);alias(i,i1);alias(gov,gov1);alias(cap,cap1);
*
     Parameter emiWeight(is,js)  "Weights to accounts for assigning multiple activities to multiple GTAP sectors"
               emiWeightXP(is)   "Weights to accounts for assigning multiple activities to multiple GTAP sectors"
     ;
*
*    --- this aggregates the intermediates demand weights for each activity, adding over all products and activities
*        mapped to the same gtap product and activity as i0 and a0. Similar for households, government and land/capital demands below
*
     emiWeight(i,a) $ sum((mapaGtapF(aGtap,a),mapcGtapF(cGtap,i)),1)
      = sum((mapcGtapF(cGtap,i1),mapaGtapF(aGtap,a1)) $ (mapcGtapF(cGtap,i) and mapaGtapF(aGtap,a)),
             sam(i1,a1))/sum((mapaGtapF(aGtap,a),mapcGtapF(cGtap,i)),1);

     emiWeight(i,h)   $ sum(mapcGtapF(cGtap,i),1)
      = sum((mapcGtapF(cGtap,i1),hp)   $ mapcGtapF(cGtap,i),sam(i1,hp))/sum(mapcGtapF(cGtap,i),1);
     emiWeight(i,gov) $ sum(mapcGtapF(cGtap,i),1)
      = sum((mapcGtapF(cGtap,i1),gov1) $ mapcGtapF(cGtap,i),sam(i1,gov1))/sum(mapcGtapF(cGtap,i),1);

     emiWeight(lnd,a) $ sum(mapaGtapF(aGtap,a),1)
      = sum((lndp,mapaGtapF(aGtap,a1)) $ mapaGtapF(aGtap,a),sam(lndp,a1))/sum(mapaGtapF(aGtap,a),1);
     emiWeight(cap,a) $ sum(mapaGtapF(aGtap,a),1)
      = sum((cap1,mapaGtapF(aGtap,a1)) $ mapaGtapF(aGtap,a),sam(cap1,a1))/sum(mapaGtapF(aGtap,a),1);
*
*    --- total production output for all activities mapped to the same GTAP activity
*
     emiWeightXP(a) $ sum(mapaGtapF(aGtap,a),1)
       = sum((is,mapaGtapF(aGtap,a1)) $ mapaGtapF(aGtap,a),sam(is,a1))/sum(mapaGtapF(aGtap,a),1);

*    --- aggregate over GTAP mapping and distribute with SAM weights if multiple sam element mapped to one GTAP element

     emiGTAP(em,i,a) $ emiWeight(i,a)
      = sum((mapcGtapF(cGtap,i),mapaGtapF(aGtap,a)),emiGTAPOri(em,cGTAP,aGtap)) * sam(i,a)/emiWeight(i,a);
*
     emiGTAP(em,i,h)   $ emiWeight(i,h)   = sum(mapcGtapF(cGtap,i),emiGTAPOri(em,cGTAP,"h0"))   * sam(i,h)  /emiWeight(i,h);
     emiGTAP(em,i,gov) $ emiWeight(i,gov) = sum(mapcGtapF(cGtap,i),emiGTAPOri(em,cGtap,"gov0")) * sam(i,gov)/emiWeight(i,gov);

     emiGTAP(em,lnd,a) $ emiWeight(lnd,a) = sum(mapaGtapF(aGtap,a),emiGTAPOri(em,"lnd0",aGtap)) * sam(lnd,a)/emiWeight(lnd,a);
     emiGTAP(em,cap,a) $ emiWeight(cap,a) = sum(mapaGtapF(aGtap,a),emiGTAPOri(em,"cap0",aGtap)) * sam(cap,a)/emiWeight(cap,a);

     emiXPGTAP(em,a)  $ emiWeightXP(a) = sum(mapaGtapF(aGtap,a),emiGTAPOri(em,"emixp",aGTAP)) * sum(is,sam(is,a))/emiWeightXP(a);
*
*    --- aggregate emissions by group of agents as currently assigned (level of SAM accounts)
*
     p_emiComp(em,"xp","SAM")  = sum(a,       emiXPGTAP(em,a));
     p_emiComp(em,"h","SAM")   = sum((i,h),   emiGTAP(em,i,h));
     p_emiComp(em,"gov","SAM") = sum((i,gov), emiGTAP(em,i,gov));
     p_emiComp(em,"int","SAM") = sum((i,a),   emiGTAP(em,i,a));
     p_emiComp(em,"lnd","SAM") = sum((lnd,a), emiGTAP(em,lnd,a));
     p_emiComp(em,"cap","SAM") = sum((cap,a), emiGTAP(em,cap,a));
*
*    --- aggregate to totals (for GTAP, CAIT and SAM, for comparison)
*
     set emiCompPos / xp,h,gov,int,lnd,cap,tot /;
     p_emiComp(em,"tot","GTAP") = sum(emiCompPos,p_emiComp(em,emiCompPos,"GTAP"));
     p_emiComp(em,"tot","CAIT") = sum(emiCompPos,p_emiComp(em,emiCompPos,"CAIT"));
     p_emiComp(em,"tot","SAM")  = sum(emiCompPos,p_emiComp(em,emiCompPos,"SAM"));
*
*    --- calculate relative error (differences can stem from assigning multiple products/activities to one GTAP account)
*
     p_emiComp(em,emiCompPos,"%") $ (p_emiComp(em,emiCompPos,"CAIT") $ p_emiComp(em,emiCompPos,"SAM"))
       = p_emiComp(em,emiCompPos,"CAIT")/p_emiComp(em,emiCompPos,"SAM");
*
*    --- correct emissions by correction mutiplier
*
     emiGTAP(em,i,a)   = emiGTAP(em,i,a)   * p_emiComp(em,"int","%");
     emiGTAP(em,i,h)   = emiGTAP(em,i,h)   * p_emiComp(em,"h","%");
     emiGTAP(em,i,gov) = emiGTAP(em,i,gov) * p_emiComp(em,"gov","%");
     emiGTAP(em,lnd,a) = emiGTAP(em,lnd,a) * p_emiComp(em,"lnd","%");
     emiGTAP(em,cap,a) = emiGTAP(em,cap,a) * p_emiComp(em,"cap","%");
     emiXPGTAP(em,a)   = emiXPGTAP(em,a)   * p_emiComp(em,"xp","%");
*
*    --- convert correction multiplier to percentage change for reporting
*
     p_emiComp(em,emiCompPos,"%") $ (p_emiComp(em,emiCompPos,"CAIT") $ p_emiComp(em,emiCompPos,"SAM"))
      = (p_emiComp(em,emiCompPos,"%")-1)*100;
*
*    --- aggregate corrected emission coefficients again by group of agents
*
     p_emiComp(em,"xp","COR")  = sum(a,       emiXPGTAP(em,a));
     p_emiComp(em,"h","COR")   = sum((i,h),   emiGTAP(em,i,h));
     p_emiComp(em,"gov","COR") = sum((i,gov), emiGTAP(em,i,gov));
     p_emiComp(em,"int","COR") = sum((i,a),   emiGTAP(em,i,a));
     p_emiComp(em,"lnd","COR") = sum((lnd,a), emiGTAP(em,lnd,a));
     p_emiComp(em,"cap","COR") = sum((cap,a), emiGTAP(em,cap,a));
     p_emiComp(em,"tot","COR")  = sum(emiCompPos,p_emiComp(em,emiCompPos,"COR"));

     p_emiComp(em,emiCompPos,"%,cor") $ (p_emiComp(em,emiCompPos,"CAIT") $ p_emiComp(em,emiCompPos,"COR"))
      = (p_emiComp(em,emiCompPos,"COR")/p_emiComp(em,emiCompPos,"CAIT")-1) * 100;

     p_emiComp(em,emiCompPos,"%,cor") $ (abs(p_emiComp(em,emiCompPos,"CAIT")-p_emiComp(em,emiCompPos,"Cor")) le 1.E-3) = 0;
*
*    --- if there are still differences, throw error and abot
*
     option p_emiComp:7;
     if( sum((em,emiCompPos) $ (abs(p_emiComp(em,emiCompPos,"%,COR")) gt 1.E-6),1),
        abort "Scaling of GTAP emissions failed",p_emiComp;
     else
        display p_emiComp;
     );
*
*    --- define allocation keys from individual emissions to inventories
*        (weighted agregation)
*
     p_ghgInvAlloc(ghgInv,em,i,h) $ emiGTAP(em,i,h)
       = sum((mapcGtapF(cGtap,i),sameas(cGtap,srcGtap)),v_ghgInvAlloc.l(ghgInv,em,srcGtap,"h0")*emiGTAPOri(em,cGTAP,"h0"))
       / sum((mapcGtapF(cGtap,i),sameas(cGtap,srcGtap)),                                        emiGTAPOri(em,cGTAP,"h0"));

     p_ghgInvAlloc(ghgInv,em,i,gov) $ emiGTAP(em,i,gov)
       = sum((mapcGtapF(cGtap,i),sameas(cGtap,srcGtap)),v_ghgInvAlloc.l(ghgInv,em,srcGtap,"gov0")*emiGTAPOri(em,cGTAP,"gov0"))
       / sum((mapcGtapF(cGtap,i),sameas(cGtap,srcGtap)),                                          emiGTAPOri(em,cGTAP,"gov0"));

     p_ghgInvAlloc(ghgInv,em,i,a) $ emiGTAP(em,i,a)
       = sum((mapcGtapF(cGtap,i),mapaGtapF(aGtap,a),sameas(cGtap,srcGtap),usrGtap) $ sameas(aGtap,usrGtap),
           v_ghgInvAlloc.l(ghgInv,em,srcGtap,usrGtap)*emiGTAPOri(em,cGTAP,usrGtap))
       / sum((mapcGtapF(cGtap,i),mapaGtapF(aGtap,a),sameas(cGtap,srcGtap),usrGtap) $ sameas(aGtap,usrGtap),
                                                      emiGTAPOri(em,cGTAP,usrGtap));
     p_ghgInvAlloc(ghgInv,em,lnd,a) $ emiGTAP(em,lnd,a)
       = sum((mapaGtapF(aGtap,a),sameas("lnd0",srcGtap),usrGtap) $ sameas(aGtap,usrGtap),
       v_ghgInvAlloc.l(ghgInv,em,srcGtap,usrGtap)*emiGTAPOri(em,"lnd0",usrGtap))
       / sum((mapaGtapF(aGtap,a),sameas("lnd0",srcGtap),usrGtap) $ sameas(aGtap,usrGtap),
                                                  emiGTAPOri(em,"lnd0",usrGtap));
     p_ghgInvAlloc(ghgInv,em,cap,a) $ emiGTAP(em,cap,a)
       = sum((mapaGtapF(aGtap,a),sameas("cap0",srcGtap),usrGtap) $ sameas(aGtap,usrGtap),
       v_ghgInvAlloc.l(ghgInv,em,srcGtap,usrGtap)*emiGTAPOri(em,"cap0",usrGtap))
       / sum((mapaGtapF(aGtap,a),sameas("cap0",srcGtap),usrGtap) $ sameas(aGtap,usrGtap),
                                                  emiGTAPOri(em,"cap0",usrGtap));

     p_ghgInvAlloc(ghgInv,em,"emiXp",a) $ emiXPGTAP(em,a)
       = sum((mapaGtapF(aGtap,a),sameas(aGtap,usrGtap)),v_ghgInvAlloc.l(ghgInv,em,"emiXP",usrGtap)*emiGTAPOri(em,"emiXP",usrGtap))
       / sum((mapaGtapF(aGtap,a),sameas(aGtap,usrGtap)),                                           emiGTAPOri(em,"emiXP",usrGtap));

     option kill=v_ghgInvAlloc;

*
*    --- check exhaustion of CAIT inventories
*
     p_compGhgInv(ghgInv,em,"CAIT") = emiInventory(ghgInv,em);
     p_compGhgInv(ghgInv,em,"SAM")
        =  sum( (js,aa),    p_ghgInvAlloc(ghgInv,em,js,aa)*emiGtap(em,js,aa))
         + sum(  a,         p_ghgInvAlloc(ghgInv,em,"emiXp",a)*emiXPGtap(em,a));
*
*    --- define scaling factor and apply, afterwards convert to percentage differences
*
     p_compGhgInv(ghgInv,em,"cor%") $ (p_compGhgInv(ghgInv,em,"CAIT") and p_compGhgInv(ghgInv,em,"SAM"))
       = p_compGhgInv(ghgInv,em,"CAIT")/p_compGhgInv(ghgInv,em,"SAM");

     p_ghgInvAlloc(ghgInv,em,js,aa)     = p_ghgInvAlloc(ghgInv,em,js,aa)     *  p_compGhgInv(ghgInv,em,"cor%");
     p_ghgInvAlloc(ghgInv,em,"emiXp",a) = p_ghgInvAlloc(ghgInv,em,"emiXp",a) *  p_compGhgInv(ghgInv,em,"cor%");
     p_compGhgInv(ghgInv,em,"cor%") $ p_compGhgInv(ghgInv,em,"cor%") = (p_compGhgInv(ghgInv,em,"cor%")-1)*100;
*
*    --- report final inventories and error to cait. If there are still discrepancies, throw error and abort
*
     p_compGhgInv(ghgInv,em,"FIN")
        =  sum( (js,aa),    p_ghgInvAlloc(ghgInv,em,js,aa)*emiGtap(em,js,aa))
         + sum(  a,         p_ghgInvAlloc(ghgInv,em,"emiXp",a)*emiXPGtap(em,a));

     p_compGhgInv(ghgInv,em,"test%") $ (p_compGhgInv(ghgInv,em,"CAIT") and p_compGhgInv(ghgInv,em,"FIN"))
       = (p_compGhgInv(ghgInv,em,"CAIT")/p_compGhgInv(ghgInv,em,"FIN")-1)*100;

     set toAddGhgInv / GTAP,SAM,CAIT,FIN /;
     p_compGhgInv("tot",em,toAddGhgInv)    = sum(ghgInv,p_compGhgInv(ghgInv,em,toAddGhgInv));
     p_compGhgInv("tot","ghg",toAddGhgInv) = sum(em,p_compGhgInv("tot",em,toAddGhgInv));

     option p_compGhgInv:2:2:1;
     if ( sum((ghginv,em), abs(p_compGhgInv(ghgInv,em,"test%")) gt 1.E-6),
         abort "Emissions not consistently allocated to inventories ",p_compGhgInv;
     else
         display p_compGhGInv;
     );
*
*    --- define MTOEs
*
     mtoe(i,h) $ emiWeight(i,h) = sum(mapcGtapF(cGtap,i),mtoeGTAP(cGtap,"h0"))                       * sam(i,h)/emiWeight(i,h);
     mtoe(i,a) $ emiWeight(i,a) = sum((mapcGtapF(cGtap,i),mapaGtapF(aGtap,a)),mtoeGTAP(cGtap,aGtap)) * sam(i,a)/emiWeight(i,a);
*
*    --- remove scaling factors for ema for reporting. Air emissions are not available from CAIT/OWID
*
     reEscale $ ((rescale ne 1) $ card(emiInventory)) = 1.E-3*1E+%autoscaleBase%
         /sum(em $ p_emiConv(em,"GHGs"), p_emiConv(em,"GHGs")*sum((is,aa),emiGTAP(em,is,aa)) + sum(a,emiXPGTAP(em,a))) * 1/eScale;
*
*    --- set flags where emission coefficients are present
*
     emSrc(i)             $ sum((em,aa),emiGTAP(em,i,aa))  = yes;
     emiCommFlag(em,i,aa) $ emiGTAP(em,i,aa)  = 1;

     option kill=GHGBAse
            kill=NC_TRAD_CEQ ,kill=NC_ENDW_CEQ,kill=NC_HH_CEQ,kill=NC_QO_CEQ,
            kill=MDF,kill=MDG,kill=MDP,kill=MIF,Kill=MIG,kill=MIP,kill=EDF,kill=EDP,kill=EIF,kill=EIP
            kill=AirPE,kill=AirPF,kill=AirpO,kill=AirPP;
*
*    --- define energy use in Millon tons of oil equivalents
*
     mtoe(js,is) = mtoe(js,is)*inscale*1.E+3;
*
*    --- redefine prices such that quantities will be defined in Million tons of oil equivalents
*        (this is important if using the volume preserving CET/CES for energy carriers)
*
     pa.l(i,ttCal) $ sum(is $ sam(i,is),mtoe(i,is)) = sum(is $ mtoe(i,is),sam(i,is))/sum(is $ sam(i,is),mtoe(i,is));

     pa.l(i,ttCal) $ sum(is $ sam(i,is),mtoe(i,is)) = min(10,max(0.01,pa.l(i,ttCal)));

     pwm.l(i,ttCal) $ sum(is $ sam(i,is),mtoe(i,is)) = pa.l(i,ttCal);
     ps.l(i,ttCal)  $ sum(is $ sam(i,is),mtoe(i,is)) = pa.l(i,ttCal);
     p.l(a,i,ttCal) $ sam(a,i) = ps.l(i,ttCal);

  $$else.useGTAPEmis

     option kill=emSrc;
     option kill=emiCommFlag;
     option kill=emiFlag;
     $$onempty
       set ghgInv/ElecHeat/;
     $$offempty
     option kill=emiGTAP;
     option kill=emiXPGTAP;
     option kill=p_ghgInvAlloc;

  $$endif.useGTAPEMis
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
