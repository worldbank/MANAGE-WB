********************************************************************************
$ontext

   MANAGE-WB project

   GAMS file : BASE.INC

   @purpose  : Declare core sets used in the model and in the base-bridge file,
               load country specific base information
   @author   : MANAGE Team
   @date     : 29.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
*
* --- sets used in base.inc and inical.inc, but not in model.inc
*
  sets
    is0                             "Base (not aggregated) SAM accounts"
    i0(is0)                         "Product in base SAM"
    a0(is0)                         "Activities in base SAM"
    h0(is0)                         "Households in base SAM"
    fp0(is0)                        "Factors in base SAM"
    mapis(is0,is)                   "Mapping from base SAM to final (aggregated) SAM"
    aAll                            "Activities plus label all, used on pnest"
    cTour(i)                        "Tourism commodities, used in Roson&Sartori damages"
    iMan(i)                         "Manufacturing commodities"
    iSrv(i)                         "Services commodities"
    fpChlUpdate(fpnest,a)           "Nests where share parameter of old vintage are updated"
    fpParUpdate(fpnest,a)           "Nests where share parameter of old vintage are updated"
    fpNestUpdate(fpnest,a)          "Nests where share parameter of old vintage are updated"
    leontief(a,fpNest)
    ;

  alias (is0,js0);
*
*    --- parameters read from the base-bridge file and other data file
*           used in base.inc, inical.inc, iterloop.inc, etc. but not in model.inc
*
  Parameters
    sam(is,is)                      "Final SAM, aggregated from base SAM"
    GDPComp(*,*,tf)                 "GDP and VA composition to target"
    mapkNestShr(*,kNest)            "Composition of final demand nests"
*
*    --- derived parameters used in inical.inc
*
    p_transMat(i,kNest,oa)          "Transformation matrix, used to define nested demand structure (also in rebench.inc)"
    p_popT(cohorts,t)               "Population by cohorts"
    p_ggdppcT(t)                    "Per capita growth assumptions"

    p_labor(fpnest,a)               "Labor quantities at benchmark"
    p_land(lnd,a)                   "Land quantities at benchmark"
    p_rshrGDPT(f,t)                 "Share of real macro variables in real GDP"
    optRec(t)                       "Stores successful opt files from baseline for later use in simulations"

    pNest1(aAll,fpNest,fpNest)      "Production Nest Shares (activity,child,parent nest), with label aAll for any activity"
    pNest(a,fpNest,fpNest)          "Production Nest Shares (activity,child,parent nest)"

    p_GDP(tf)                       "GDP"
    p_upDateFac(*,*)                "Update factors for quantities and volumes from year to year"
    ;

  years(tf) = 2010 + ord(tf);

  scalars
    usd                             "Exchange rate to USD"
    RoR0                            "Average rate of return on capital"
    inScale                         "Scale factor for SAM, might be updated by automatic scaling"
    samScale                        "Scale factor for SAM, as inputted"
    pScale                          "Scale factor for population"
    eScale                          "Scale factor for emissions"
    reScale                         "Rescaling factor to get from scaled variables back to units of SAM"
    reEscale                        "Rescaling factor to get from scaled emissions back to units of original emission inventory (in eScale units)"
    iniVint
    upDateLoop
    reScale / 1 /
    ;

  $$onembeddedCode Python:
    import os
    db = gams.ws.add_database_from_gdx(r'%readFile%')
    try:
        db.get_symbol('nutcont')
        os.environ["nutcont"] = "on"
    except:
        os.environ["nutcont"] = "off"

    try:
        db.get_symbol('macc')
        os.environ["macc_found"] = "on"
    except:
        os.environ["macc_found"] = "off"

    try:
        db.get_symbol('GHGMitigation')
        os.environ["GHGMitigation"] = "on"
    except:
        os.environ["GHGMitigation"] = "off"

    try:
        db.get_symbol('EnergyProj')
        os.environ["EnergyProj"] = "on"
    except:
        os.environ["EnergyProj"] = "off"

    try:
        db.get_symbol('PowerMix')
        os.environ["PowerMix"] = "on"
    except:
        os.environ["PowerMix"] = "off"

    try:
        db.get_symbol('aez')
        os.environ["aez"] = "on"
    except:
        os.environ["aez"] = "off"

    try:
        ss =  db.get_set('gtapVersion')
        rec = str(ss.first_record())
        gams.printLog(rec)
        first  = rec.find("(")
        second = rec.find(")")
        os.environ["GTAPVersion"] = rec[first+1:second]
    except:
        os.environ["GTAPVersion"] = "gtapData11c"

  $$offEmbeddedCode

  $$setglobal version      %sysEnv.GTAPVersion%
  $$setglobal nutcont      %sysEnv.nutCont%
  $$setglobal macc_found   %sysEnv.macc_found%
  $$setglobal aez          %sysEnv.aez%

  $$ifi "%aez%"=="on"      set aez(*), raez(*), raez_aez(*,*);
  $$ifi "%nutCont%"=="on"  set nutr; parameter nutCont; set sumDummy /sum/;
*
*
*
  $$iftheni.task "%task%"=="baseline"
*
*    ----------------------------------------------------------------------------------
*
*        Baseline mode:
*        Define various sets and parameters
*        (Loaded from BaU file in simulations)
*
*    ----------------------------------------------------------------------------------

*
*    --- load list of labels for some nests, and
*        then use to define domain checked ones
*
     set s_inv, s_ginv, s_adpinv, s_fpChl, s_fpPar, s_row;

     $$gdxin %readFile%
        $$onmulti
        $$loaddc is is0<mapis.dim1 mapis
        $$offmulti
        $$loaddc s_inv=inv s_ginv=ginv s_adpinv=adpinv s_row=row
        $$load   s_fpChl<pnest.dim2
        $$load   s_fpPar<pnest.dim3
     $$gdxin
*

*    --- use Phython code to build list of products, activities, households, factors
*        in base and disaggregated SAM. This assumes that prefixes as shown below
*        strictly used. Failing to do so will lead to unpredicable behavior.
*
     set s_i(*), s_a(*), s_h(*),s_fp(*), s_entr(*), s_gov(*), s_ft(*), s_fp_ft(*,*),s_fpParP(*);

     $$onEmbeddedCode Python:

          i0 = []
          a0 = []
          h0 = []
          fp0 = []
          labels = gams.get('is0')
          for s in labels:
              if s.startswith("c-"):
                 i0.append(s)
              if s.startswith("a-"):
                 a0.append(s)
              if s.startswith("h-"):
                 h0.append(s)
              if s.startswith("f-"):
                 fp0.append(s)

          gams.set("i0",i0)
          gams.set("a0",a0)
          gams.set("h0",h0)
          gams.set("fp0",fp0)

          i  = []
          a  = []
          h  = []
          e  = []
          f  = []
          fpParP  = []
          ft = []
          f_ft = []
          g  = []
          labels = gams.get('is')
          for s in labels:
              if s.startswith("c-"):
                 i.append(s)
              if s.startswith("a-"):
                 a.append(s)
              if s.startswith("h-"):
                 h.append(s)
              if s.startswith("e-"):
                 e.append(s)
              if s.startswith("g-"):
                 g.append(s)
              if s.startswith("f-"):
                 if s != "f-top":
                    f.append(s)
                    ft.append("t-"+s[2:])
                    f_ft.append((s,"t-"+s[2:]))
          labels = gams.get('s_fpPar')
          for s in labels:
              if not s in f:
                fpParP.append(s)

          gams.set("s_i",i)
          gams.set("s_a",a)
          gams.set("s_h",h)
          gams.set("s_gov",g)
          gams.set("s_entr",e)
          gams.set("s_fp",f)
          gams.set("s_fpParP",fpParP)
          gams.set("s_ft",ft)
          gams.set("s_fp_ft",f_ft)
     $$offEmbeddedCode i0 a0 h0 fp0 s_i s_a s_h s_entr s_gov s_fp s_fpParP s_ft s_fp_ft
     $$onmulti
        set is / set.s_fpParP,sumInst,co2EqCarbTax /;
     $$offmulti
*
*    ---- read nutrient content (cal,prot,fat) per USD
*
     $$iftheni.nutCont "%nutCont%"=="on"


        $$GDXIN %readFile%
           $$load nutr<nutCont.dim2
           $$load nutCont
        $$GDXIN
     $$endif.nutCont
*
*    --- these labels are used below but might not be found in the brigde file
*
     set dummylabels /psb0,psbm0,omegaz0,hPop0,physProd,additive,depr,old,a-macc,c-ctlRais,omegaCetShort,
                      intensMargElas,capital,labor,factSupElasShort,markupNonComp /;

     alias (aagr,aagrp);
     alias(is,js);alias(is0,js0);

     if ( execError, abort " Execerror encountered after reading core sets from base-bridge file: %system.fn%, line: %system.incline%");
*
*    -- the full list of sets comprises f-top (=total output)
*       which is not part of the pNest mapping set in the bridgefile
*

     set fpNest(is)    "CES nests in production function" / set.s_fpChl,f-top /;
     set fpChl(fpNest) "CES childs in production function, include also products and endowments" / set.s_fpChl /;
     set fpPar(fpNest) "CES nests which are parents"      / set.s_fpPar /;
     if ( not card(s_entr), Abort "No enterprise, please use e- as a prefix in is / mapis / SAM0 for the enterprise, file: %system.fn%, line: %system.incline%");
     if ( not card(s_h),    Abort "No households, please use h- as a prefix in is / mapis / SAM0 for the household, file: %system.fn%, line: %system.incline%");
     if ( not card(s_gov),  Abort "No government, please use g- as a prefix in is / mapis / SAM0 for the government, file: %system.fn%, line: %system.incline%");
     if ( not card(s_fp),   Abort "No factors, please use f- as a prefix in is / mapis / SAM0 for the factors, file: %system.fn%, line: %system.incline%");
     if ( not card(s_i),    Abort "No products, accounts, please use c- as a prefix in is / mapis / SAM0 for the products, file: %system.fn%, line: %system.incline%");
     if ( not card(s_a),    Abort "No activities, accounts, please use a- as a prefix in is / mapis / SAM0 for the activities, file: %system.fn%, line: %system.incline%");
*
*    --- defines sets used in the model (definition see above)
*        by inserting the sets defined in Phyton or read from
*        base-bridge GDX
*
     set aa(is)           "Armington agents"                                                   / set.s_a,set.s_i,set.s_h,set.s_gov,set.s_inv,set.s_adpinv,set.s_ginv /;
     set oa(aa)           "Final demanders"                                                    / set.s_h,set.s_gov,set.s_inv,set.s_ginv,set.s_adpinv/;
     set a(aa)            "Production activities"                                              / set.s_a /;
     set i(aa)            "Products as demander of agents"                                     / set.s_i /;
     set h(oa)            "Households"                                                         / set.s_h /;
     set f(oa)            "Set of final demanders without housholds"                           / set.s_gov,set.s_inv,set.s_ginv,set.s_adpinv/;
     set fp(fpNest)       "Endowments"                                                         / set.s_fp /;
     set mapTax(*,is)     "Mapping from standardized tax instrument names to SAM rows";

     $$onmulti
     set instTot(is)      "Institutions, plus a label for sum over all institutions 'sumInst'" / set.h,set.s_entr,set.s_gov,set.s_row,sumInst /;
     $$offmulti
     set inst(instTot)    "Institutions (household,enterprises,government,rest-of-the-world)"  / set.h,set.s_entr,set.s_gov,set.s_row /;
     set entr(is)         "Enterprise account"                                                 / set.s_entr /;
     singleton set gov(f) "Government account"                                                 / set.s_gov /;
     set aAll                                                                                  / set.a, all /;

     addCest(i) = no;

     $$onmulti
        set pollType(is)  "Pollution types" / GHGs, Acid, TOFP, PMTn, PMTF, AirT /;
     $$offmulti
*
*     --- load various parameters and sets from the base-bridge file after
*         sets related to are defined
*
     $$gdxin %readFile%
        $$onmulti
        $$loaddc cohorts
        $$offmulti
        $$loaddc aagr<mapaagr.dim1 mapaagr
        $$loaddc e
        $$loaddc pNest1=pNest
        $$loaddc inv ginv adpinv stb row debt nrgfac
        $$loaddc maptax
        $$loaddc l cap lnd natres
        $$Ifthene card(adpinv)<>0
           $$loaddc map_adpInv
        $$else
          option kill=map_adpInv;
        $$endif
     $$gdxin
*
*    --- define mapping from factor to factor taxes based on set
*        constructed in Python
*
     mapFtax(fp,is) $ s_fp_ft(fp,is) = yes;
*
*    ---  Dynamic set assignments: non-energy products as complement of energy products in

*
*    --- tax names used in the model are defined as dynamic sets
*        based on the mapTax set from the brigdefile
*        Exemption: factor tax names are derived from factor names
*
     dtx(is)   $ (mapTax("dtx",is) $ (Not sameas(is,"na"))) = yes;
     ptx(is)   $ (mapTax("ptx",is) $ (Not sameas(is,"na"))) = yes;
     stx(is)   $ (mapTax("stx",is) $ (Not sameas(is,"na"))) = yes;
     mtx(is)   $ (mapTax("mtx",is) $ (Not sameas(is,"na"))) = yes;
     ftx(is)   $ s_ft(is)                                   = yes;
     vat(is)   $ (mapTax("vat",is) $ (Not sameas(is,"na"))) = yes;
     psb(is)   $ (mapTax("psb",is) $ (Not sameas(is,"na"))) = yes;
     etx(is)   $ (mapTax("etx",is) $ (Not sameas(is,"na"))) = yes;
*    envTx(is) $ mapTax("envtx",is) = yes;
     ssb(is)   $ mapTax("ssb",is)                           = yes;
     ctx(is)   $ (mapTax("ctx",is) $ (Not sameas(is,"na"))) = yes;
     ctx("co2EqCarbTax")                                    = yes;
*
     $$onmulti
     set nipa "GDP components" / cons,govt,invt,ginv,expo,impo /;
     $$offmulti
*
*    --- nipa component mapping
*
     mapn(h,    "cons") = yes;
     mapn(gov,  "govt") = yes;
     mapn(inv,  "invt") = yes;
     mapn(ginv, "ginv") = yes;
*
*    --- initalize child-parent relation for individual activities where given
*
     pnest(a,fpChl,fpPar) $ sum(sameas(a,aAll),pNest1(aAll,fpChl,fpPar))
                          = sum(sameas(a,aAll) $ pNest1(aAll,fpChl,fpPar),pNest1(aAll,fpChl,fpPar));
*
*    --- otherwise, use standard mapping for all activities stored in "all"
*
     pnest(a,fpChl,fpPar) $ (not pNest(a,fpChl,fpPar)) = pNest1("all",fpChl,fpPar);

     set testAagr(a);
     testAagr(a) $ (sum(mapaagr(aagr,a),1) ne 1) = yes;
     if (card(testAagr),
        abort "The map mapAagr does not map activities or assigns them twice, file: %system.fn%, line: %system.incline%",
              testAagr,mapAagr;
     );

     $$gdxin %readFile%
     $$onmulti
     $$loadDc kNest<mapKNestShr.dim2
     $$offmulti
     $$loadDc mapkNestShr
     $$gdxin

     if ( execError, abort " ExecError encountered after reading data from the base-bridge file: %system.fn%, line: %system.incline%");
*
*    --- mapping set from SAM rows to institutions.
*        Needed to allow for domain checked expressions
*        the institutions
*
     mapInst(is,inst)  $ sameas(is,inst)    = yes;
*
*    --- define link between "pseudo nests" and the
*        products. Must have the same name
*

     set unName                        "Name of region in UN statistics" / %regGtapName% /
*
*    --- parameters needed for benchmarking, but not in model
*
     Parameters
       SAM0(is0,is0)                   "Disaggregated base SAM"
       Land0(is0,a0)                   "Land quantities for original SAM"
       LCov0(*,lcov)                   "Land cover data for original SAM"
       EnTPar(*)                       "Vector of parameters for endogenous tech change"
       phiRDtoes(a)                    "Stepping on toes effect"
       Supfgrowth(a)                   "Supernormal frontier growth"
       p_fgrowth(a)                    "Frontier growth"
       p_fcrgrowth(a,t)                "Frontier growth correction for exogenous growth"
       p_chigrK0                       "Elasticity parameter in growth of capital"
       p_rord0                         "Country risk premium"
       p_psb0(i)                       "Basic supply price of domestically produced products"
       p_psbm0(i)                      "Basic supply price of imports"
       unPop(UnName,age,t)             "Un population statistics"
       Labor0(is0,a0)                  "Labor quantities for original SAM"
       defPar(*)                       "Default parameter values"
       actPar(a,*,*)                   "Exceptions to default activity parameters"
       subsElas(*,fpNest)              "Substitution parameters for production nests, can be overwritten by actpar"
*      --- this set is defined here as we need to check for identical subs elasticities to define nest fpIdent
       p_sigmaxfac0(fpNest,a,v)        "Factor substitution elasticities"
       compar(i,*)                     "Exceptions to default commodity parameters"
       hohpar(h,*,*)                   "Exceptions to default household parameters"
       sigmacNest0(kNest)              "Substitution elasticity inside demand nests"
       othPar(*,*)                     "Exceptions to default household parameters"
       p_GDPProj(tf)                   "GDP projection from bridge-file, scenario '%GDPCompScen%'"
       p_GDPpcProj(tf)                 "GDP per capita growth"
       p_GDPpc(tf)                     "GDP per capita"
       p_popAge0(tf,age)               "Base year population"
       p_ggdppcT(t)                    "Per capita growth assumptions"
     ;

     scalars
        totLab
     ;

     $$gdxin %readFile%
        $$loaddc defPar actPar comPar hohPar othPar subsElas labor0
        $$loaddc SAM0
        $$loadDc sigmaCNest0
        $$iftheni.endoTech "%endoTech%"=="on"
           $$loaddc EnTPar PhiRDtoes Supfgrowth
        $$endif.endoTech
        $$load GDPComp
     $$gdxin

*    ------------------------------------------------------------------------------------------
*
*       Run checks for correct aggregation
*
*    ------------------------------------------------------------------------------------------

     set nonEmpty(is0) "Non empty SAM columns, excluding the summation column";
     alias(is0,js0,ks0);
     sam0(is0,js0) $ (abs(sam0(is0,js0)) le 1.E-20) = 0;
     nonEmpty(is0) $ sum(js0 $ sam0(is0,js0),sam0(is0,js0))  = yes;
*
*    -- exclude the column which is the largest in each row (this must be the summing up column)
*
     parameter samMax(is0);
     samMax(js0) = smax(ks0,abs(sam0(js0,ks0)));
     nonEmpty(is0) $ (sum(js0 $ (samMax(js0) eq abs(sam0(js0,is0))),1) ge card(js0)-1) = no;
*
*    --- are all original SAM rows is0 mapped to some new SAM row IS?
*
     set errorAgr(*);
     errorAgr(is0) $ ((not sum(mapis(is0,is),1)) $ nonEmpty(is0)) = yes;
     if (card(errorAgr), abort "Some original SAM rows / columns are not mapped, check mapis, file: %system.fn%, line: %system.incline%", errorAgr);

     errorAgr(is)  $ sum(mapis(is0,js) $ (mapis(is0,is) and (not sameas(is,js))),1) = yes;
     if (card(errorAgr), abort "Some original SAM rows / columns are mapped twice, check mapis, file: %system.fn%, line: %system.incline%", errorAgr);
*
*    --- check additionally that products are linked to products, acitivities to activtities
*        households to houseolds and factors to factors from the old to new SAM
*
     errorAgr(i0)  $ ((not sum(mapis(i0,i),1)) $ nonEmpty(i0)) = yes;
     if (card(errorAgr), abort "Some original SAM products are not mapped, check mapis, file: %system.fn%, line: %system.incline%", errorAgr);

     errorAgr(a0)  $ ((not sum(mapis(a0,a),1)) $ nonEmpty(a0)) = yes;
     if (card(errorAgr), abort "Some original SAM activities are not mapped, check mapis, file: %system.fn%, line: %system.incline%", errorAgr);

     errorAgr(h0)  $ ((not sum(mapis(h0,h),1)) $ nonEmpty(h0)) =  yes;
     if (card(errorAgr), abort "Some original SAM households are not mapped, check mapis, file: %system.fn%, line: %system.incline%", errorAgr);

     errorAgr(fp0) $ ((not sum(mapis(fp0,fp),1)) $ nonEmpty(fp0)) =  yes;
     if (card(errorAgr), abort "Some original SAM factors are not mapped, check mapis, file: %system.fn%, line: %system.incline%", errorAgr);
*
*    --- build list of remaining SAM rows (institutions, taxes) and do a similar check
*
     set restOfIs0(is0);
     restOfIs0(is0) $ (not (i0(is0) or a0(is0) or h0(is0) or fp0(is0))) = yes;

     set restOfIs(is);
     restOfIs(is) = yes;
     restOfIs(i)  = no;
     restOfIs(a)  = no;
     restOfIs(h)  = no;
     restOfIs(fp) = no;

     errorAgr(restOfIs0) $ ((not sum(mapis(restOfIs0,restOfIs),1)) $ nonEmpty(restOfIs0) ) =  yes;
     if (card(errorAgr), abort "Some original SAM row (not products, activities, factors, households) are not mapped, check mapis, file: %system.fn%, line: %system.incline%", errorAgr);

*    ------------------------------------------------------------------------------------------
*
*       Some checks for factors
*
*    ------------------------------------------------------------------------------------------

     abort $ (card(cap) ne 1)            "Cap (capital) must be singleton, file: %system.fn%, line: %system.incline%"
     abort $ (not sum(sameas(l,fp),1))   "L (labor) must be comprised in the set fp, file: %system.fn%, line: %system.incline%";
     abort $ (not sum(sameas(cap,fp),1)) "Cap (capital) must be comprised in the set fp, file: %system.fn%, line: %system.incline%";

     abort $ (card(l) + card(cap) + card(lnd) + card(natres) ne card(fp))
                                         "Some factors are not in l,cap,lnd or natres, file: %system.fn%, line: %system.incline%";

*    ------------------------------------------------------------------------------------------
*
*       Some checks for flex factor nesting
*
*    ------------------------------------------------------------------------------------------

     set errorNest(a,fpNest);

     errorNest(a,fpChl) = (sum(fpPar,pNest(a,fpChl,fpPar)) ne 1) and (sum(fpPar,pNest(a,fpChl,fpPar)) ne 0);
     if (card(errorNest),
         abort "Shares of some child tech nests don't add up to zero or are not empty, file: %system.fn%, line: %system.incline%",
         errorNest;
     );
*
*    --- scaling factors for SAM and other transactions
*        for population/labor and emissions, exchnage rate
*
     inScale  = defpar('inscale');
     samScale = defpar('inscale');
     pScale   = defpar('pscale');
     eScale   = defpar('escale');
     usd      = defpar('usd');

     scalar reScale;
     $$iftheni.autoScale "%scaleSAM%"=="on"
*
*        --- automatic scaling of SAM based on largest entry on demand
*            implies that all quantity and value variable in the model listing
*            are scaled compared to SAM.
*
        scalar autScale;
        autScale = defpar('gdp');
        inScale  = 1E+%autoscaleBase%/autScale;
        reScale  = defPar("inscale")/inScale;
     $$else.autoscale
        $$setglobal autoscaleBase 6
        rescale = 1;
     $$endif.autoscale
*
*    --- from proto to final SAM
*
     sam(is,js) = inScale*sum((is0,js0) $ (mapis(is0,is) and mapis(js0,js)), sam0(is0,js0));
*
*    --- scale physical output
*
     actPar(a,"physProd","") = actPar(a,"physProd","")*inScale;
*
*    -------------------------------------------------------------------------------------
*
*       Fix in case that debtnew is counted twice in gov account, both as income flow and as
*       negative cost by a negative entry for private savings
*
*    -------------------------------------------------------------------------------------
*
     scalar p_toCorr;
     if ( sum(inv,sam(inv,gov)) < 0,
        p_toCorr        = sum(inv,sam(inv,gov));
        sam(gov,entr)   = sam(gov,entr) - p_toCorr;
        sam(inv,entr)   = sam(inv,entr) + p_toCorr;
        sam(inv,gov)    = 0;
     );
*
*    --- check for balanced SAM
*
     parameter samBal(is),colSum(is),rowSum(is);
     colSum(is)                           = sum(js,sam(is,js));
     rowSum(is)                           = sum(js,sam(js,is));
     samBal(is)                           = colSum(is) - rowSum(is);
     colSum(is) $ (abs(samBal(is))<1.E-5) = 0;
     rowSum(is) $ (abs(samBal(is))<1.E-5) = 0;
     samBal(is) $ (abs(samBal(is))<1.E-5) = 0;
     if (card(samBal), abort "SAM not balanced at 1.E-5, file: %system.fn%, line: %system.incline%", samBal,colSum,rowSum);

*     --- assign creditors (domestic,foreign,combined), uses observed debt related flows in SAM

     instD(inst) $ (sam(debt,inst) and not (sum(gov,mapinst(gov,inst) or sum(row,mapinst(row,inst))))) = yes;
     instD(inst) $ ((card(instD) eq 0) $ sum(sameas(inst,h),1))                                        = yes;
     instF(inst) $ (sum(mapinst(row,inst),sam(debt,row)) and not sum(gov,mapinst(gov,inst)))           = yes;
     instFD(inst)                                                                                      = instD(inst) + instF(inst);
     dinst(inst) $ (not mapinst(row,inst))                                                             = yes;

*    ------------------------------------------------------------------------------------------
*
*       Some checks for demand aggregation
*       (Note that mapAgr can comprise definition of aggregates which are used
*        to calculate others. The tests for the demand nests can therefore not be applied)
*
*    ------------------------------------------------------------------------------------------
*
     parameter errorK(*);
     errorK(i) $ (not sum(kNest $ mapkNestShr(i,knest),1)) = yes;
     if (card(errorK), abort "Some products i are not mapped to any demand nest in mapkNestShr, file: %system.fn%, line: %system.incline%",errorK;);

     errorK(i) $ (sum(kNest $ mapkNestShr(i,knest),mapkNestShr(i,knest)) ne 1) = sum(kNest, mapkNestShr(i,knest));
     if (card(errorK), abort "For some products i, shares in nest don't add up to unity, file: %system.fn%, line: %system.incline%",errorK;);

     map_i_knest(i,knest) $ mapkNestShr(i,knest) = yes;
     loop((i,knest,h) $ map_i_knest(i,knest),
     );
*
*    -- top-level nests topKNest have no parent
*
     topkNest(kNest) $ (not sum(kNest1,mapkNestShr(kNest,kNest1)))                                = yes;

     errorK(kNest)   $ (sum(i,mapkNestShr(i,knest)) + sum(knest1,mapkNestShr(knest1,kNest)) eq 0) = yes;
     errorK("top")                                                                                = no;
     if (card(errorK), abort "Empty demand nests, file: %system.fn%, line: %system.incline%",errorK;);

     errorK(kNest)   $ ((sum(kNest1,mapkNestShr(kNest,knest1)) ne 1) $ (not topkNest(kNest)))     = yes;
     if (card(errorK), abort "Share for some demand nests in other nests don't add up to unity, file: %system.fn%, line: %system.incline%",errorK;);

     p_GDPProj(tf) = gdpComp("%GDPCompScen%","GDP",tf);

*    ------------------------------------------------------------------------------------------
*
*      Assign parameter in baseline mode from sheets in bridge file
*
*    ------------------------------------------------------------------------------------------
*
*    ---- flag for additive CES, only used if nest is not Leontief
*
     isAdditiveCES(fpNest)                          = yes                          $ (subsElas("additive",fpNest) gt eps);
*
*    --- Substitution elasticities and labor productivity impacts
*
     p_sigmaxfac0(fpNest,a,v)                       = subsElas("subsElas",fpNest)  $ (subsElas("subsElas",fpNest) gt eps);
     p_sigmaxfac0(fpNest,a,v) $ actPar(a,fpNest,v)  = actPar(a,fpNest,v)           $ (actPar(a,fpNest,v) gt eps);

     $$iftheni.vintages "%vintages%"=="off"
*
*       --- if the vintages are switched off and a substitution elasticity for the old vintage
*           is given, use for the activity (the new vintage is a small part of production, only)
*
        p_sigmaxfac0(fpNest,a,v) $ ((not actPar(a,fpNest,v)) and actPar(a,fpNest,"old"))
          = actPar(a,fpNest,"old") $ (actPar(a,fpNest,"old") gt eps);
     $$endif.vintages
*
*    ---- manual parameters to test USAGE (????)
*
     p_grKMin(t)        = -0.1;
     p_grKTrend(t)      = 0.025;
     p_grKMax(t)        = 0.10;
*    grKMax(t)          = grKTrend(t)+0.06;
     p_chiGrk0          = 10;
*
*    --- emission sets
*
     set em "Emission types"
     /
      co2               "CO2 emissions"
      n2o               "N2O emissions"
      ch4               "Methane emissions"
      f-Gas             "Emissions of fluoridated gases"
      BC                "Black carbon"
      CO                "Carbon monoxide"
      NH3               "Ammonia"
      NMVOC             "Non-methane volatile organic compounds"
      noX               "Nitrogen oxides"
      OC                "Organic carbon"
      PM10              "Particulate matter 10"
      PM2_5             "Particulate matter 2.5"
      SO2               "Sulfur dioxide"
     /;
*
     set ema(em) "Air pollutants"           / BC, CO, NH3, NMVOC, noX, OC, PM10, PM2_5, SO2 /
         ghg(em) "Greenhouse gas emissions" / co2, n2o, ch4, f-Gas /;

     parameter  emiGTAP(em,is,js) "MtCo2e and air emissions per Million USD, in the definitions of the SAM"
                emiXPGTAP(em,is)  "MtCo2e and air emissions per Million USD, for produciton activities"
                gwp(em)           "Global warming potential of gasses to convert to CO2 equivalents"
     ;
     gwp(ghg) = 1;

     Table p_emiConv(em,pollType) "Conversion factors for different pollution indicators"

               GHGs   PMTF
      BC         0
      CO         0
      NH3        0
      NMVOC      0
      noX        0
      OC         0
      PM10       0
      PM2_5      0     1
      SO2        0
      CH4        1
      CO2        1
      N2O        1
     ;
*
*    --- Population data from UN
*        change in load_base if using a different source
*
     $$gdxin %datF%/unPop.gdx
       $$load age<unPop.dim2 unPop
     $$gdxin

     SET mapage(age,cohorts) "mapping from ages to cohorts"
      /
       PLT15.PLT15
       P1564.P1564
       P65UP.P65UP
       PLT15.PTOTL
       P1564.PTOTL
       P65UP.PTOTL
     /;

     p_popAge0(t,age) = unPop("%RegGTAPName%",age,t)*pScale/reScale;
     abort $ ( not card(p_popage0)) "No UN population projections for %RegGtapName%,file: %system.fn%, line: %system.incline%";
     option kill=unPop;

     p_pop0(cohorts) = sum(age $ mapage(age,cohorts),p_popAge0(t0,age));
     labor0(fp0,a0)  = labor0(fp0,a0)/reScale;

     if (card(labor0) le 1,
        labor0(fp0,a0) $ sum(mapis(fp0,l),1) = sam0(fp0,a0);
        totLab                               = sum((fp0,a0),labor0(fp0,a0));
        totLab         $ totLab              = p_pop0("p1564") * 2/3 * 1/totLab;
        labor0(fp0,a0)                       = labor0(fp0,a0) * totLab/pScale;
        totLab                               = sum((fp0,a0),labor0(fp0,a0));
     );
*
*    --- assign intermediate inputs to quasi-factors in production nest table

     pNest(a,fpChl,fpPar) $ (    (not sum(fpParp,pNest(a,fpChl,fpParp)))
                             and (not sum(fpChlp,pNest(a,fpChlp,fpPar)))
                             and sum(i,sameas(fpChl,i))) = 1;

     set         fpNat(fpNest)                   "Natural factors and commodities"
                 map_fp_fpParA(a,fpNest,fpNestp) "Mapping between parent and all child commodities and factors (no composites), used in inical.inc";
     parameter   map_fp_fpPar(a,fpNest,fpNestp)  "Mapping between parent and all child commodities and factors (no composites), used in inical.inc";

*    --- Initialize the map

     map_fp_fpparA(a,fpNest,fpNestp) = pNest(a,fpNest,fpNestp);
*
*    --- complete the full nest table recursively
*         safety net for infinite loops:
     option forlim = 10;
*
*    --- pseudo factors (intermediate products and services i)

     fpNestI(fpNest) $ sum(i,sameas(fpNest,i)) = yes;

*    --- find natural factors (those which have a value in SAM)

     fpNat(fpNest) $ (fp(fpNest) OR fpNestI(fpNest)) = yes;

*    --- find the top nest

     fpTop(fpPar) $ (not fpChl(fpPar)) = yes;

     set toDo(a,fpNest),cur(a,fpNest),curParNest(fpNest),curChlNest(fpNest);
     alias(toDo,toDo1);
     toDo(a,fpNest) = yes;
     toDo(a,fpNat)  = no;
     map_fp_fppar(a,fpNat,fpPar) = pNest(a,fpNat,fpPar);
     while(card(toDo),
        option kill=cur;
        cur(toDo(a,fpPar)) $ ((not sum(toDo1(a,fpNest), pNest(a,fpNest,fpPar)))
                               and (    sum((fpNest,fpNat) $ pNest(a,fpNest,fpPar),map_fp_fppar(a,fpNat,fpNest))
                                     or sum(fpNat,map_fp_fppar(a,fpNat,fpPar))))
                                = yes;

        map_fp_fppar(a,fpNat,fpPar) $ cur(a,fpPar) =   sum(fpNest,pNest(a,fpNest,fpPar)*map_fp_fppar(a,fpNat,fpNest))
                                                     + map_fp_fppar(a,fpNat,fpPar);
        toDo(cur) = no;
     );
     map_fp_fppar(a,fpNat,fpPar) $ (map_fp_fppar(a,fpNat,fpPar) eq eps) = 0;

     lFpNest(l,a) = yes;
     lFpNest(fpPar,a) $ ((sum(l,map_fp_fpPar(a,l,fpPar)) eq sum(fpNat,map_fp_fpPar(a,fpNat,fpPar))) $ sum(l,pNest(a,l,fpPar)))  = yes;
     lFpNest(l,a)     $ sum(fppar $ lFpNest(fpPar,a),pNest(a,l,fppar)) = no;

     set errorTop(a,fpNest);
     errorTop(a,fpNat) $ (map_fp_fppar(a,fpNat,"f-top") ne 1) = yes;
     if ( card(errorTop), abort "Error in production nesting: some intermediates or primary factors are not fully mapped to top-level composite, file: %system.fn%, line: %system.incline%",errorTop);

*
*    ------------------------------------------------------------------------------
*
*        Flexible factor markets
*
*    ------------------------------------------------------------------------------
*
     set fmNest1 "Columns labels of fmNest symbol in bridgefile, should be parent nests including primary factors";
     $$gdxin %readFile%
         $$load fmNest1<fmNestPar.dim2
     $$gdxin
     $$offmulti
*
     set testFmNest(*) "Temporary set for tests, should be empty after each test";
*
*    --- test for presence of factors in parents
*
     testFmNest(fp) $ (not sum(sameas(fmNest1,fp),1)) = yes;
     if (card (testFmNest), abort "The factor nests need to comprise the factors in the parents, file: %system.fn%, line: %system.incline%",testFmNest);
*
*    --- add the parent nests in the factor market nesting to the set is

     set fmNest1P(*);
     $$onEmbeddedCode Python:
          fmNest1P = []

          f = gams.get("s_fp")
          labels = gams.get("fmNest1")
          for s in labels:
              if not s in f:
                fmNest1P.append(s)
          gams.set("fmNest1P",fmNest1P)
     $$offEmbeddedCode fmNest1P


     $$onmulti
        set is / set.fmNest1P /;
     $$offmulti

     set
        fmAllfp(is)    "All parent nests plus activities, as subset-of is"  / set.fmNest1,set.a /
        fmAll(fmAllfp) "All nests as own subset"                            / set.fmNest1,set.a /
        fmNest(fmAll)  "Parent and child nests, factor, but not activities" / set.fmNest1 /
        fmA(fmAll)     "Activities as part of the fmNest structure"         / set.a /
        fmP(fmAll)     "Primary factors as part of the fmnest structure"    / set.fp /
     ;
*
*    --- read the parameters related to the factro nests, including the transformation and factor supply elasticities
*        and the information about a nest being additive or not.

     set fmNestParLabels  / additive,omegaCET,omegaCETShort,factSupElas,factSupElasShort /;
     parameter fmNestPar(fmNestParLabels,fmNest)
               xfmSupDistrInst(inst,fmp,fma)    "Distribution of factor income by institions and activity, optional"
     execute_load "%readfile%" fmNestPar,xfmSupDistrInst;
*
*    --- test for non-empty omegaCET
*
     testFmNest(fmNest) $ (not fmNestPar("omegaCet",fmNest)) = yes;
     if (card (testFmNEst), abort "No transformation elasticties for some factor nests. Use eps for zero, file: %system.fn%, line: %system.incline%",testFmNest);
*
*    --- all parent nests, with the exemption of primary factors, and all acticities are a child of some parent
*
     fmChild(fmNest) = yes;
     fmChild(fmP)    = no;
     fmChild(fmA)    = Yes;
*
*    --- all fmNest in the column of the fmNest table are parent nests
*
     fmPar(fmNest)   = yes;
*
*    --- read original nesting table as set (all non-zero entries are shown as true)
*        The lines with the elasticities are skipped
*
     set fmNestingOri(fmAll,fmAllFp);
     parameter pfmNest(fmAll,fmAllFp);
     execute_load "%readfile%" fmNestingOri=fmNest,pfmNest=fmNest;
*
*    --- special case: a primary factor fmp has not children. Map all activities
*
     fmNesting(fmA,fmp,fmP) $ (not sum(fmNestingOri(fmChild,fmP),1)) = yes;
*
*    --- map children to factor markets
*
     alias(fmPar,fmPar1);
     set fmpMap(fmAll,fmp);
*
*    --- map childs to primary factors if already mapped to the primary factor
*        (either by a manual entry in the EXCEl sheet or the special case statement above)
*
     fmpMap(fmChild,fmp) $ fmNestingOri(fmChild,fmp) = yes;
*
*    --- primary factors are mapped to themselves
*
     fmpMap(fmp,fmp)                                 = yes;
*
*    --- derive the factor market to which childs belong recursiveley
*
     loop(fmPar1,
        fmpMap(fmChild,fmP) $ sum(fmNestingOri(fmChild,fmPar) $ fmNestingOri(fmPar,fmp),1) = yes;
     );
*
*    --- assign given nestings to three dimensional map (children,parent,related factor market)
*
*    ... for activities
     fmNesting(fmA,fmAll,fmP)     $ (fmNestingOri(fmA,fmAll)     $ fmpMap(fmAll,fmp))   = yes;
*    ... for children nests
     fmNesting(fmChild,fmPar,fmP) $ (fmNestingOri(fmChild,fmPar) $ fmpMap(fmPar,fmp))   = yes;
*    ... for parents nests
     fmNesting(fmPar,fmp,fmP)     $ fmNestingOri(fmPar,fmp)                             = yes;
*
*    --- special case: empty children nest. Assign activities not in other nests
*
     fmNesting(fma,fmPar,fmp)     $ (fmNestingOri(fmPar,fmp) $ (not sum(fmChild,fmNesting(fmChild,fmPar,fmp)))
                                                             $ (not sum(fmPar1,fmNesting(fma,fmPar1,fmp)))) = yes;
*
*    --- check: Are some children in multiple parent nests?
*
     set testFmNesting(fmAll,*);
     testFmNesting(fmChild,fmp) $ (sum(fmNesting(fmChild,fmPar,fmp),1) gt 1) = yes;
     if (card(testFmNesting),
        fmNesting(fmChild,fmPar,fmp) $ (not testFmNesting(fmChild,fmp))      = no;
        abort "Some factor child nests (inluding activities) are assigned to multiple parents, file: %system.fn%, line: %system.incline%",testFmNesting,fmNesting;
     );
*
*    --- check: Are all children in some parent nest?
*
     testFmNesting(fmPar,"") $ (not sum(fmNesting(fmChild,fmPar,fmp),1)) = yes;
     if (card(testFmNesting),
        fmNesting(fmChild,fmPar,fmp) $ (not testFmNesting(fmChild,""))   = no;
        abort "Some factor child nests (inluding activities) are not assigned to any parent, file: %system.fn%, line: %system.incline%",testFmNesting,fmNesting;
     );
*
*    --- set the flag for the additive CET functional form, information is on p_fmNest
*        parameter which is a clone of fmNest with the additional lines
*
     isAdditiveCet(fmNest)  $ fmNestPar("additive",fmNest)        = yes;
*
*    --- map to speed up some statement: mappings from nests to primary factor markets
*
     fmNestFmp(fmPar,fmp)   $ sum(fmNesting(fmChild,fmPar,fmp),1) = yes;
     fmNestFmp(fmChild,fmp) $ sum(fmNesting(fmChild,fmPar,fmp),1) = yes;
*
*    --- Find nests where defining composite prices over vintages is possible
*
     fpIdent(fpChl,a) $ (p_sigmaxfac0(fpChl,a,old) eq p_sigmaxfac0(fpChl,a,new)) = yes;
*
*    --- parent nests where child share parameters are updated
*
     fpParUpdate(fpPar,a)   $ (p_sigmaxfac0(fppar,a,old) ne p_sigmaxfac0(fppar,a,new))                      = yes;
     fpChlUpdate(fpnest,a)  $ sum((fpChl,fpPar) $ ((pNest(a,fpChl,fpPar) gt eps) $ fpParUpdate(fpPar,a)),1) = yes;
     fpNestUpdate(fpnest,a) $ (fpChlUpdate(fpnest,a) or fpParUpdate(fpNest,a))                              = yes;

     if ( [card(fpIdent) eq card(fpChl)*card(a)] $ ifVint, abort "All substitution elasticities are equal. De-activate vintage mechanism, file: %system.fn%, line: %system.incline%.");

     set nestsToDo(fpNest),curNest(fpNest);
     alias(nestsToDo,nestsToDo1);
     nestsToDo(fpNest) = yes;

     while(card(nestsToDo),
        option kill=curNest;
        curNest(nestsToDo) $ (not sum((a,nestsToDO1),pNest(a,nestsToDo1,nestsToDo))) = yes;
        fpIdent(curNest,a) $ fpIdent(curNest,a) = Yes $ (sum(fpChl $ ((pNest(a,fpChl,curNest) gt eps) $ fpIdent(fpChl,a)),1) eq sum(fpChl $ (pNest(a,fpChl,curNest) gt eps),1));
        nestsToDo(curNest) = No;
     );
     option kill=fpChlUpdate,kill=fpNestUpdate;
     fpNestUpdate(fpChl,a) $ sum(fpPar $ fpParUpdate(fpPar,a), (pNest(a,fpChl,fpPar) gt eps)) = yes;

     fpNestUpdate(fpParUpdate(fpPar,a)) = yes;

*    ------------------------------------------------------------------------------
*
*        GTAP bridge
*
*    ------------------------------------------------------------------------------

*
*     --- determine the GTAP version in use (check if there are labels
*         for GTAP-CE and GTAP Version 9
*
      set cGTAPinBridge(*);
      $$gdxin %readFile%
        $$load cGTAPinBridge<mapcGTAP.dim1
      $$gdxin

     $$onEmbeddedCode Python:
        import os
        labels = gams.get('cGTAPinBridge')
        os.environ["GTAPCE"] = "off"
        for s in labels:
            if s.lower() == 'cem':
              os.environ["GTAPCE"] = "on"
        os.environ["GTAP9"] = "off"
        for s in labels:
            if s.lower() == 'crp':
              os.environ["GTAP9"] = "on"
     $$offEmbeddedCode
*
*     --- load mapping between gtap sets and model sets

      Sets
         aGTAP                "GTAP activities with ely aggregated"
         cGTAP                "GTAP Commodities"
         regGTAP              "GTAP regions"
         mapaGtap(aGtap,is0)  "mapping between GTAP activities and SAM0 activities"
         mapcGtap(cGtap,i0)   "mapping between GTAP commodities and SAM0 commodities"
     ;

     $$gdxin %datF%/%version%.gdx
        $$load aGTAP=ag cGTAP=cg regGTAP=rg
        $$onmulti
        $$load lcov
        $$offmulti
     $$gdxin
*
*    --- test if the current project region is found in the list of GTAP
*        regions. If not, skip the calculation of emissions and reading
*        the AEZ data
*
     $$onEmbeddedCode Python:
        import os

        found  = "false"
        os.environ["skipGtapEmis"] = "off"
        labels = gams.get('regGTAP')
        for s in labels:
            if s == "%regGtapName%":
               found  = "true"

        if found == "false":
           os.environ["skipGtapEmis"] = "on"
     $$offEmbeddedCode

     $$iftheni.skipGtapEmis not "%skipGtapEmis%"=="on"

        $$iftheni.noGTAPReg "%sysEnv.skipGtapEmis%"=="on" $setglobal skipGtapEmis on
            $$Log   "Warning: Current region is not in the GTAP Data base, no emission and land cover accounting"
            display "Waning:  Current region is not in the GTAP Data base, no emission and land cover accounting"
        $$endif.noGtapReg
     $$endif.skipGtapEmis

     $$gdxin %readFile%
     $$loaddc mapaGtap mapcGtap
     $$gdxin
     if ( execError, abort " ExecError encountered after reading mapping to GTAP from the base-bridge file: %system.fn%, line: %system.incline%");

     set gtapCrops(aGtap) "Crop activities in GTAP"   / pdr,wht,gro,v_f,osd,c_b,pfb,ocr /
         gtapAnim(aGtap)  "Animal activities in GTAP" / oap,ctl,rmk,wol /;
*
*    --- define crops, animals and forestry activities, used for the
*        land cover change part of the model
*
     crops(a) = sum(mapaGtap(gtapCrops,a0),mapis(a0,a));
     anim(a)  = sum(mapaGtap(gtapAnim,a0),mapis(a0,a));
     frs(a)   = sum(mapaGtap("frs",a0),mapis(a0,a));

     set agrGtapI(cGtap) "Agricultural, forestry, fishery products in GTAP" /
        set.gtapCrops
        set.gtapAnim
        frs
        fsh
     /;

     set agrGtapA(aGtap) "Agricultural, forestry, fishery activities in GTAP" /
        set.gtapCrops
        set.gtapAnim
        frs
        fsh
     /;
*
*    --- define sets for some GTAP products/sectors which are further
*        dis-aggregated in V11 and V11-CE compared to V9
*
     $$iftheni.GTAPCE "%sysEnv.GTAPCE%"=="on"
        $$setglobal oxt nmn,mio,mao,mcu,moo
        $$setglobal chm kfr,nfr,pfr,xch,rbr,plp,pls,plr,bph
        $$setglobal nmm cem,nmx
        $$setglobal i_s isp,iss,ris,isc
        $$setglobal nfm app,aps,ral,cpp,cps,rcp,mpp,mps,rom,nfc
     $$elseifi.GTAPCE "%sysEnv.GTAP9%"=="on"
        $$setglobal oxt omn
        $$setglobal chm crp
        $$setglobal nmm nmm
        $$setglobal i_s i_s
        $$setglobal nfm nfm
     $$else.GTAPCE
        $$setglobal oxt oxt
        $$setglobal chm chm,rpp,bph
        $$setglobal nmm nmm
        $$setglobal i_s i_s
        $$setglobal nfm nfm
     $$endif.GTAPCE

     set manuGtapA(aGtap) "Manufacturing sectors in GTAP" /
        coa,oil,gas,%oxt%,cmt,omt,vol,mil,pcr,sgr,ofd,b_t
        tex,wap,lea,lum,ppp,p_c,%chm%,%nmm%,%i_s%,%nfm%
        fmp,ele,ome,mvh,otn,omf,
        $$iftheni.GTAP9 "%sysEnv.GTAP9%"=="on"
          ely
        $$else.GTAP9
          eeq,TnD,NuclearBL,CoalBL,GasBL,WindBL,HydroBL
          OilBL,OtherBL,GasP,HydroP,OilP,SolarP
        $$endif.GTAP9
        gdt,wtr,cns
     /;

     set manuGtapI(cGtap) "Manufacturing commodities in GTAP" / set.manuGtapA /;

     set elecGtapA(aGtap) "Power generation types as sectors in GTAP" /
        $$iftheni.GTAP9 "%sysEnv.GTAP9%"=="on"
           ely
        $$else.GTAP9
           NuclearBL,CoalBL,GasBL,WindBL,HydroBL
           OilBL,OtherBL,GasP,HydroP,OilP,SolarP
        $$endif.GTAP9
     /;

     set elecGtapI(cGtap) "Power generation types as products in GTAP" /
         set.elecGtapA
     /;

     set ctourGtapI(cGtap) "Tourism commodity in GTAP" /
        $$iftheni.GTAP9 "%sysEnv.GTAP9%"=="on"
           trd
        $$else.GTAP9
           afs
        $$endif.GTAP9
     /;
*
*    --- define electricity, agricultural, manufacturing, service an tourism
*        activities an products from mapping of GTAP activities/products
*        to proto SAM activities and products, and from mapping from
*        proto to final SAM
*
     aelec(a) $ sum(mapaGtap(elecGtapA,a0) $ mapis(a0,a),1)  = yes;
     ielec(i) $ sum(mapcGtap(elecGtapI,i0) $ mapis(i0,i),1)  = yes;

     agr(a)   $ sum(mapaGtap(agrGtapA,a0)  $ mapis(a0,a),1)  = yes;
     iagr(i)  $ sum(mapcGtap(agrGtapI,i0)  $ mapis(i0,i),1)  = yes;

     man(a)   $ sum(mapaGtap(manuGtapA,a0) $ mapis(a0,a),1)  = yes;
     iman(i)  $ sum(mapcGtap(manuGtapI,i0) $ mapis(i0,i),1)  = yes;

     srv(a)   $ (not ( man(a) or  agr(a)))                   = yes;
     iSrv(i)  $ (not (iman(i) or iagr(i)))                   = yes;

     ctour(i) $ sum(mapcGtap(ctourGtapI,i0) $ mapis(i0,i),1) = yes;
*
*    --- land data can be used if the region is int GTAP and are assigned a few lines below.
*        Otherwise, they are zero.
*
     $$iftheni.useGTAPEmis "%skipGTAPEmis%"=="off"

*       ------------------------------------------------------
*
*        load land use from GTAP data
*
*       ------------------------------------------------------

        set
            aez                      "Agro-Ecological Zones" / aez1*aez18 /,
            faez(*)                  "List of agro-ecological zones if f- to indicate factor use"
            faez_aez(*,aez)          "Mapping between f-aez# and aez#"
            raez(*)                  "List of regioname-aez#"
            raez_aez(*,*)            "Link between regioname-aez# and f-aez# set"
        ;

        parameter
            p_landGTAP(regGtap,*,*)  "Land cover data from GTAP AEZ"
            p_carb(regGtap,*,lcov)   "Carbon content by land cover from GTAP"
        ;

        $$onEmbeddedCode Python:

            faez     = []
            raez     = []
            raez_aez = []
            faez_aez = []
            labels = gams.get('aez')
            for s in labels:
               faez.append("f-"+s)
               raez.append("%regGtapname%-"+s[3:])
               raez_aez.append(("%regGtapname%-"+s[3:],"f-"+s))
               faez_aez.append(("f-"+s+"_0",s))

            gams.set("faez",faez)
            gams.set("raez",raez)
            gams.set("faez_aez",faez_aez)
            gams.set("raez_aez",raez_aez)

        $$offEmbeddedCode faez faez_aez raez raez_aez

        alias(fp0,fp0p);

        faez_aez(fp0,aez) $ (sum(mapis(fp0,lnd),1) and (not sum(faez_aez(fp0p,aez),1))) = yes;

        $$ifthen.existGTAPData exist "%regf%/addData_%regGtapName%.gdx"
*
*           --- possibility to provide data on land use, land cover and carbon contents
*               in separate file in reg folder instead of using the GTAP data base
*
           execute_load "%regf%/addData_%regGtapName%.gdx" land0, lCov0, carbCnt;

        $$elseif.existGTAPData exist "%datF%\%version%.gdx"

           execute_load "%datF%/%version%.gdx" p_landGtap=p_land, p_carb;
*
*          --- attention: the following is not working correctly if multiple activities
*              are mapped to the same GTAP activity
*
           alias(a0,a0p);

           land0(fp0,a0) $ (sum(mapis(fp0,lnd),1) $ sam0(fp0,a0) $ sum((mapaGtap(aGtap,a0p),faez_aez(fp0,aez)) $ mapaGtap(aGtap,a0),1))
              = sum((mapaGtap(aGtap,a0),faez_aez(fp0,aez)),p_landGTAP("%regGtapName%",aez,aGtap));

           lCov0(fp0,lCov) = sum(faez_aez(fp0,aez),p_landGTAP("%regGtapName%",aez,lCov));
*
*          --- calculate average and scale to Mio ha
*
           carbCnt(fp0,lcov) $ sum(faez_aez(fp0,aez),p_landGTAP("%regGtapName%",aez,lCov))
              = sum(faez_aez(fp0,aez),p_carb("%regGtapName%",aez,lCov)*p_landGTAP("%regGtapName%",aez,lCov))
                                                   / sum(faez_aez(fp0,aez),p_landGTAP("%regGtapName%",aez,lCov)) * 1000;
           option kill=p_land,kill=p_carb;

        $$endif.existGTAPData

     $$else.useGTAPEmis
*
*       --  This is done to make sure skipGTAPemi works properly
*           Otherwise it complains of empty sets and parameters
*           should be overwritten if skipGTAPEmi is ON

        option kill=land0,kill=lCov0,kill=carbCnt;

     $$endif.useGTAPEMis


     $$iftheni.BG "%BGCheck%"=="on"

*       -----------------------------------------------------------------------------------------------
*
*          Redefinitions of parameters on defpar and hohpar for balanced growth check
*
*       -----------------------------------------------------------------------------------------------

        scalar p_gdp0  "Benchmark GDP";
        p_gdp0 = defpar('gdp');
        if ( p_gdp0 eq 0, abort "Balanced growth test requires the field GDP in defPar, file: %system.fn%, line: %system.incline%");
*
*       (1) CD-demand function: income elasticities to unity, own price elasticities to minus unity,
*                               to avoid that consumption composition changes if income inceases
*
        hohpar(h,"incElas0",topKNest) =  1;
        hohpar(h,"prcElas0",topKNest) = -1;
*
*       (2) Population growth of 1
*
        loop(tt $ (tt.pos gt 1),
          p_popAge0(tt,age) = p_popage0(tt-1,age) * (1 + 0.01);
        );
*
*       (3) Define the growth rates of the capital stock in the first year, used to define debt stock and interest rate
*
        scalar
            p_capStockChange  "Capital stock change derived from GDP, depreciation and investment at benchmark"
            p_govDefObserved  "New government debt at benchmark"
            p_debtStockTgt    "Targeted debt from new debt and change in capital stock"
            p_debtStockOld    "Reported debt stock, domestic and foreign"
            p_intPayment      "Interest payment, derived"
            p_interestRate    "Derived interest rate"
            p_capoutRate      "Capital stock GDP ratio"
        ;

        p_capoutRate     = defPar('capoutRate0');
        p_capStockChange = [   p_capoutRate * p_gdp0*inScale * (1-defPar('depr0'))
                             + sum((inv,is),sam(inv,is))
                           ]/[p_capoutRate * p_gdp0*inscale] - 1;

        p_GDPProj(tf) = p_capStockChange*100;
*
*       --- given new government debt, used to define t0 debt stock levels
*
        p_govDefObserved = sum( (gov,debt),sam(gov,debt));
*
*       --- the debt stock must grow with GDP = capital stock change. The given new
*           debt therefore define the given debt stock at given captial stock change
*
        p_debtStockTgt = p_govDefObserved * (1 + p_capStockChange)/p_capStockChange;
*
*       --- read debt stock levels and interest rates from bridge file
*
        parameters
           p_debtStkF0      "Foreign debt stock"
           p_debtStkD0      "Domestic debt stock"
           p_debtIntF0      "Interest on foreign debt"
           p_debtIntD0      "Interest on domestic debt"
        ;

        p_debtStkF0         = defPar('DebtStkF0')*inScale;
        p_debtStkD0         = defPar('DebtStkD0')*inScale;
        p_debtIntF0         = defPar('DebtIntF0');
        p_debtIntD0         = defPar('DebtIntD0');
*
*       --- update domestic and foreign debt stocks with same correction factor
*
        p_debtStockOld      = p_DebtStkD0 + p_DebtStkF0;
        p_DebtStkD0         = p_DebtStkD0 * p_debtStockTgt/p_debtStockOld;
        p_DebtStkF0         = p_DebtStkF0 * p_debtStockTgt/p_debtStockOld;
*
*       --- remove principal (see inc/balGrowth.inc) and
*           define one common interest rate for domestic and
*           foreign debt from the given interest payments and updated debt stocks
*
        p_intPayment        = sum((instFD,debt),sam(instFD,debt))*(1 + p_capStockChange);
        p_interestRate      = p_intPayment / (p_DebtStkD0 + p_DebtStkF0) * 100;
        p_DebtIntD0         = p_interestRate;
        p_DebtIntF0         = p_interestRate;
*
*       --- remap
*
        defpar('DebtStkF0') = p_debtStkF0/inscale;
        defpar('DebtStkD0') = p_debtStkD0/inscale;
        defpar('DebtIntF0') = p_debtIntF0;
        defpar('DebtIntD0') = p_debtIntD0;
*
*       (4) as wages are changing according to labor productivity growth, we need
*           to make sure that the labor stock is not reacting to wage changes

        fmNestPar("factSupElas",fmNest) $ sum(sameas(l,fmNest),1) = 0;

     $$endif.BG

  $$else.task

*    ----------------------------------------------------------------------------------
*
*        Simulation mode:
*        Load sets for domain checking from BaU file
*        Additional sets not used for domain checking and all parameters
*        loaded in inc/inical.inc
*
*    ----------------------------------------------------------------------------------

     set fmNest1 "Parent nest in the flexible factor supply CET nesting structure"
         fmChild "Childs in the flexible factor supply CET nesting structure";

     set lCov, crops, anim, frs, lCovMng, lCovUnMng, lCovDrv

     $$onmulti
     $$gdxin %readFile%
       $$load is,aa,oa,f,h,gov,inv,ginv,adpInv,entr,row,debt,a,aAll,i,instTot,inst,fpNest,fp,fpChl,fpPar,l,cap,lnd,natres,aagr,mapAagr
       $$load stx,vat,ftx,mtx,etx,stb,ctx,ptx,dtx,psb,mapfTax,is0,a0,i0,nipa,mapn,cohorts,map_adpInv,e,addCest,lCov,pollType,kNest,nrgFac
       $$load fmNest1=fmNest
       $$ifi "%aez%"=="on"     $$LOAD raez,raez_aez
       $$ifi "%nutCont%"=="on" $$LOAD nutr,nutCont
       $$load lCov,crops,anim,frs,lCovMng,lCovUnMng,lCovDrv,em,ema,age,instfd,dinst,instf,instd,mapinst,ghgInv,mapis
       $$load GDPComp
     $$gdxin
     $$offmulti

     set fmAllfp(is)     / set.fmNest1,set.a /;
     set fmAll(fmAllfp)  / set.fmNest1,set.a /;
     set fmNest(fmAll)   / set.fmNest1 /;
     set fmA(fmAll)           "Activities as childs in the factor supply nesting structure" / set.a /;
     set fmP(fmAll)      / set.fp /
         actFmp(fmp)          "Primary factors as part of the fmNest structure in use";

     set fmChild(fmAll)       "Childs in factor supply CET nesting structure, including individual production activities"
         fmPar(fmAll)         "Parent nests"
         fmParBot(fmAll,fmp)
         fmNesting(fmAll,fmAll,fmP);
     set isAdditiveCet(fmAll) "Indicator set for factor supply nests using the additive CET function form";
     set fmNestFmp(fmAll,fmp) "Indicator set, which factor supply nests is used for a specific factor fmp";
*
*    ---- declare the GTAP sets as empty, these sets used in reportDecl.inc to avoid that the code tries
*         to aggregate non-existing data for these position over households in baseline mode
*
     $$onempty
        set cGtap(*)//,aGtap(*)//,regGtap(*)//;
     $$offempty

     if (execError, abort " ExecError encountered after reading sets from %readFile% in: %system.fn%, line: %system.incline%");

  $$endif.task
*
* --- read GDP comp, points on MACC curves, energy projection and GHG mitigation
*     The LOAD statements will make sure that all labels used on the symbols
*     added to the UELs, different from execute_load used in inc/cal/inical.inc
*     Additionally, the symbols might not have been used in baseline construction
*     but might be needed in counterfactuals. Loading them here will make sure
*     they are available in the BaU file
*
  $$onmulti
  $$gdxin %readFile%

  $$iftheni.macc "%macc_found%"=="on"
     set maccPoints;
     parameter macc(*,maccPoints);
     $$load maccPoints<macc.dim2
     $$load macc
  $$endif.macc

  $$iftheni.energyProj "%sysEnv.energyProj%"=="on"
     parameter energyProj(*,*,*,t) "Projections of changes in demand for energy services and energy use";
     $$load energyProj
  $$endif.energyProj

  $$iftheni.powerMix "%sysEnv.powermix%"=="on"
    parameter powerMix(*,*,t) "Power mix and power price projections from basebridge";
    set enerScen;
    $$load enerScen<powerMix.dim1
    $$load powerMix
  $$endif.powerMix

  $$iftheni.GHGMitigation "%sysEnv.GHGMitigation%"=="on"
     parameter GHGMitigation(*,*,t) "Relative and absolute emission ceilings and carbon taxes";
     $$load GHGMitigation
  $$endif.GHGMitigation
  $$gdxin
  $$offmulti
*
* --- add additional sets, parameters etc. from project specific
*     file load_base.inc in project folder. The user can switch
*     this off by setting loadBase to off. If the global
*     is not set, the file will be included if existing
*
  $$ifi not setglobal loadBase $setglobal loadBase on
  $$ifi not exist '%regf%/load_base.inc'  $setglobal loadBase off
  $$ifi "%loadBase%"=="on" $include '%regf%/load_base.inc'
*
* --- if no climate damage channels are defined, set as empty
*
  $$ifi not defined chnl set chnl / "" /;
*
* --- if the twist mechnism is off, remove related sets
*     the same for energy factor nrgFac
*
  $$onempty
  $$ifi not defined  fpParTC set fpParTc(fpNest)//;
  $$ifi not defined  fpChlTC set fpChlTc(fpChl,a) //;
  $$ifi not defined  nrgFac option kill=nrgFac;
  $$offempty

  fpChlTwist(fpChl) $ sum(fpChlTC(fpChl,a),1) = yes;
*
* --- The twist equations will be introduced in the model, only,
*     if twist parents are found
*
  $$setglobal hasTwist off
  $$ife card(fpParTC)>0 $$setglobal hasTwist on

  $$iftheni.draws not "%1"=="draws"

     $$onembeddedCode Python:
        with open(r'%gams.scrdir%equs.gms', 'w') as f:
           for s in gams.db:
             if (type(s) == GamsEquation) :
                f.write('  kill='+ s.name +'\n')
        f.closed
     $$offembeddedCode

  $$endif.draws
*
* --- MCP uses an inequality condition for rrat
*     and therefore needs an upper limit
*
  $$iftheni.cns not "%SolveType%"=="MCP"
     rrat.up(a,ttCal) = inf;
  $$else.cns
     rrat.up(a,ttCal) = 1;
  $$endif.cns
*
* --- anbort end of base.inc if errors occured
*
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
