********************************************************************************
$ontext

   Manage project

   GAMS file : MODEL.INC

   @purpose  : Declarations of model variables, equations and parameters use
               in MANAGE Model and definition of equations and model

   @author   : Manage team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************

  $$ifi not "%SolveType%"=="MCP" $setglobal paired off
  $$ifi not "%gams.docfile%"=="" $setglobal scaleModel off
  $$ifi not setglobal scaleModel $setglobal scaleModel on
*
* ---- set use in the model equations
*      (additional sets used during benchmarking and reporting in inc/base.inc)
*
  sets

     ts(t)                      "Simulation period"

     is                         "SAM accounts"
     aa(is)                     "Armington agents"
     aaIn(aa)                   "Armington agents in actual model instance"

*    ---- final demanders

     oa(aa)                     "Other Armington agents besides activities"
     h(oa)                      "Households"
     hs(h)                      "Households in current CDE calibration problem (dynamic set)"
     f(oa)                      "Other final demand accounts"
     adpinv(oa)                 "Adaptation investments"
     map_adpInv(f,adpinv)       "Mapping of adapative investment to private and government investments"
*
*    --- activities and derived sets
*
     a(aa)                      "Activities"
     aact(a)                    "Active activities"
     aElec(a)                   "Electricity activities"
     agr(a)                     "Agricultural activities"
     man(a)                     "Manufacturing activities"
     srv(a)                     "Services"
     crops(a)                   "Crops"
     anim(a)                    "Animals"
     frs(a)                     "Forestry"

     $$iftheni.vintages "%vintages%"=="on"
        v                          "Vintages production"          / Old, New /
     $$else.vintages
        v                          "Dummy vintage production"     / "" /
     $$endif.vintages
     $$iftheni.labVintages "%labVintages%"=="on"
        vf                         "Vintages for factors"        / Oldf, Newf /
     $$else.labVintages
        vf                         "Dummy single vintage for factors"  / "" /
     $$endif.labVintages
*
*    --- products and derived sets
*
     i(aa)                      "Commodities"
     iagr(i)                    "Agricultural commodities"
     iElec(i)                   "Electricity commodities, shares are defined in model"
     addCEST(i)                 "Use additive CES and CET for product i"
     e(i)                       "Energy commodities"
     iact(i)                    "Active domestic commodities"
     iarm(i)                    "Active Armington commodities"
     iimp(i)                    "Active imports"
     idom(i)                    "Active domestic supply"
     iexp(i)                    "Active export supply"
     ist(i)                     "Active stock changes"
     emSrc(i)                   "Emission source commodities"
     mgEncl(i)                  "Commodities that require margins"
     xaact(i,aa)                "Active Armington demand"
*
*    --- land use
*
     lCov                       "Land cover types, for carbon accounting"
     lCovUnMng(lCov)            "Unmanaged land cover types, for carbon accounting"
     lCovMng(lCov)              "Managed land cover types, for carbon accounting"
     lCovDrv(lCov)              "Land cover types which drive changes in unmanaged land"
*
*    --- demand nesting
*
     kNest                      "Final demand nests"
     topKNest(kNest)            "Final demand nests in top-level demand function"
     mapk(kNest,i)              "Mapping between consumed and produced goods"
     map_i_knest(i,knest)       "Mapping between products and demand nests"
     consCont(oa,kNest)         "Control for final demand nesting"
     akfact(oa,kNest)           "Active final demand cells"
     xakact(oa,knest)

*    ---- sets for flexible tech nesting and endowments

     fpNest(is)                 "Factors & aggregates & commodities"
     actFpNest(fpNest)          "Factors & aggregates & commodities, actually in model"

     fp(fpNest)                 "Primary factors of production (land, labor, capital)"
     actFp(fp)                  "Endowments actually found in SAM"
     l(fpnest)                  "Labor"
     lFpNest(fpNest,a)          "Nest wich comprise labor factor, only"
     cap(fpnest)                "Capital"
     lnd(fpnest)                "Land ressources"
     actLnd(lnd)                "Land and water ressources found in SAM"
     actHa(lnd)                 "Land ressources in SAM with coverage data in Mio ha"
     natres(fpnest)             "Natural ressources"
     nrgFac(fpNest)             "Energy carriers"

     fpchl(fpNest)              "Factors and commodities as child in production nest"
     fpchlTwist(fpChl)          "Child nests which drive the twist"
     fpParTc(fpNest)            "Parent with twists"
     fpChlTc(fpChl,a)           "Child nests with twists"
     fppar(fpNest)              "Factors and commodities as parent in production nest"
     fpNat(fpNest)              "Natural factors"
     fpIdent(fpNest,a)          "Nests where substitution elasticities are identical"
     fpNestI(fpnest)            "Intermediate inputs as child nests in production function"
     isAdditiveCes(fpNest)      "True if the additive CES is used for this parent nest"
     flagXfac(fpnest,a,v)       "Flag for factor and intermediate uses (used to control FOCs)"
*
*    --- factor supply nesting
*
     fmAllfp(is)                "All parent nests plus activities, as subset-of is"
     fmAll(fmAllfp)             "All nests as own subset"
     fmNest(fmAll)              "Parent and child nests, factor, but not activities"
     fmA(fmAll)                 "Activities as part of the fmNest structure"
     fmP(fmAll)                 "Primary factors as part of the fmnest structure"
     actFmp(fmp)                "Primary factor as part of the fmNest structure in use"
*
     fmChild(fmAll)             "Child nests, including activities"
     fmPar(fmAll)               "Parents nests, including primary factors"
     fmParBot(fmAll,fmp)        "Parents nests, including primary factors, aggregate of individual activities"
     fmNesting(fmAll,fmAll,fmP) "Factor supply nesting: child, parent, related factor market"
     isAdditiveCet(fmAll)       "True if the additive CET is use for this parent nest"
     fmNestFmp(fmAll,fmp)
*
*    --- institutions
*
     instTot(is)                "Institutions plus sum of institutions"
     inst(instTot)              "Institutions"
     dinst(inst)                "Subset of domestic institutions"
     instd(inst)                "Domestic creditors"
     instf(inst)                "Foreign creditors"
     instfd(inst)               "Foreign + domestic creditors"
     mapInst(is,inst)           "Mapping for institutions from is to inst"
     entr(is)                   "Enterprises"
     inv(f)                     "Investment"
     ginv(f)                    "Government Investment"
     stb(is)                    "Stock Changes"
*
*    --- demography
*
     cohorts                    "Population cohorts"
     age                        "Ages"
     mapage(age,cohorts)        "Mapping from ages to cohorts"
*
*     --- taxes
*
     dtx(is)                    "Direct taxes"
     stx(is)                    "Sales Taxes"
     ctx(is)                    "Carbon taxes"
     ssb(is)                    "Subsidies"
     ftx(is)                    "Factor taxes"
     mapftax(fp,is)             "Mapping between factors and factor taxes"

     emiAa                      "Sectors in emission reporting"

     aagr                       "Aggregate activities target for structural change"
     mapAagr(aagr,is)           "Mapping between three sectors to activities (for structural change)"
     chnl                       "Climate change damange channels, need to be defined by user"
  ;

  set kNest / top /;

  Singleton sets
     debt(is)      "Debt Accounts"
     ptx(is)       "production taxes"
     psb(is)       "Production subsidies"
     mtx(is)       "Tariffs"
     etx(is)       "Export taxes"
     vat(is)       "Other taxes (VAT)"
     gov(f)        "Government"
     row(is)       "Rest of the world"
     fpTop(fpNest) "Top nest in production tree"
     $$iftheni.vintages "%vintages%"=="on"
        Old(v)        "Old vintage index"          / Old /
        New(v)        "New vintage index"          / New /
     $$else.vintages
        Old(v)        "Old vintage index"          / "" /
        $$onempty
          new(v) //
        $$offempty
     $$endif.vintages
     $$iftheni.labVintages "%labVintages%"=="on"
       oldf(vf)      "Old factor vintage index"          / oldF /
       newf(vf)      "Old factor vintage index"          / newF /
     $$else.labVintages
       oldf(vf)      "Old factor vintage index"          / "" /
     $$endif.labVintages
  ;
*
* --- The labels on set g are explicitly used in model.inc and cannot
*     defined later in inc/base.inc. Same for transport nodes
*
  set g "Government tax accounts"
  /
     FacLand   "Land Factor Tax"
     FacNatres "Natural resource Factor Tax"
     FacLab    "Labor Factor Tax"
     FacCap    "Capital Factor Tax"
     psb       "Production Subsidies"
     Prd       "Production Taxes"
     Phn       "Phantom Tax to calibrate activity shares"
     Exp       "Export Taxes"
     Imp       "Import Taxes"
     Emi       "Emission Taxes on commodity use"
     EmiXP     "Emisison taxes on production"
     Pit       "Personal income tax"
     Cit       "Corporate income tax"
     VatDom    "VAT on domestic goods"
     VatImp    "VAT on imported goods"
     Paf       "Agent specific sales tax"
  /;

  set n "Transport nodes"
  /
     d  "Domestic"
     x  "Export"
     m  "Import"
   /;

  sets
*
* --- The following six sets are extended with additional elements in inc/base.inc. Here only labels
*     used in code of model.inc are included in these sets to allow for
*     domain checking.

      cohorts                 "Age cohorts"                         / P1564,pTotl /
      is                      "SAM accounts"                        / sumInst,GHGs,PMTF /
      instTot(is)             "Institution as supplier of factors"  / sumInst /
      pollType(is)            "Pollution types"                     / GHGs,PMTF /
      lCov                    "Land cover classses"                 / BuiltUpLand /
      nipa(nipa)              "GDP component set nipa"              / expo,impo /

      mapn(oa,nipa)           "Link between final demands and GDP component set nipa"
      ghgInv                  "GHG inventory positions"
      em                      "Emission types (Green house gases, air pollutants)"
      ema(em)                 "Air pollutants"
      emiCommFlag(em,i,aa)    "Indicator set of emission type em is linked to product i and Armington agent aa"
      emiFlag(em,aa)          "Indicator set if Armington agent aa emits emission type em"
  ;

  Alias
   (is, js), (i, j), (inst, instp), (l,lp,lpp), (h,hp,hpp), (e,ep),
   (aelec,aelecp), (lnd,lndp), (fpchl,fpchlp),(fppar,fpparp),(fpNest,fpNestp,fpNestpp)
   (a,a1),(instfd,instfdp), (v,vp),(instd,instdp),(instf,instfp),
   (topkNest,topKnestp,topKNestk),(instp,iinst),(h,hp),(fd,oa),(aagr,aagrp),(is,js),
   (kNest,kNest1),(vf,vf1);

  Variables

*   --- Production variables

    xfac(fpNest,a,v,t)                 "Factor Demand"
    pxfac(fpNest,a,v,t)                "Factor Price including taxes"
    pxfacInd(fpNest,a,v,t)             "Factor Price including taxes, exhaustion price for additive CES"
    pxfac_preTax(fpNest,a,v,t)         "Pre-Tax Factor Price"
    lambdaxfac(fpNest,a,v,t)           "Factor augmenting productivity change (Hicks neutral)"
    twist(fpNest,fpNest,a,v,t)         "Cost neutral change in share parameter, additive"
    shrxfac(fpnest,fpnestP,a,v,t)      "Actual cost shares, needed for cost neutral twists"
    yield(fp,a,t)                      "Factor Yield"
    yieldLnd(a,t)                      "Yield per unit of land [t or 1000 USD/ha]"
    chitwFA(fpNest,a,t)                "CES share twist parameter shifter for all a v and t"
    chitwF(fpNest,t)                   "CES share twist parameter shifter for all a v and t"
    px(a,t)                            "Average unit cost"
    pp(a,t)                            "Producer price"
    xp(a,t)                            "Gross output"
    x(a,i,t)                           "Domestic production of good i produced by activity a"
    p(a,i,t)                           "Price of good i produced by activity a"
    pIND(i,t)                          "Price index of good i produced by multiple activities"
    xs(i,t)                            "Supply of good i produced domestically"
    ps(i,t)                            "Supply price of good i"
    psExp(i,t)                         "Expected supply price of good i"
    xsExp(i,t)                         "Expected supply price of good i"
    psExpRel(i,t)                      "Delta expected minus current price over current price"
    psIND(i,t)                         "Price index of good i for exports and domestic supply ACET"
    st(i,t)                            "Domestic stock of products"
    delst(i,t)                         "Stock changes assumed from domestic production"
    mdelst(i,t)                        "Imported stocks"
    ElecMix(a,i,t)                     "Electricity mix"

*   --- Income variables

    facty(fpNest,t)                    "Factor income before direct taxes"
    kapy(t)                            "Gross profits"
    yh(h,t)                            "Total household income"
    corrFac(fpnest,t)                  "Correction factor to recover exhaustion in factor markets"
    yhDsp(h,t)                         "Total disposable household income"
    gini(t)                            "Gini coefficient"
    aps(h,t)                           "Average propensity to save"
    chims(t)                           "Shifter on share of savings in disposible household income and in enterprise income"
    chiFundG(t)                        "Shifter on share of savings allocated to domestic gov debt"
    pindh(h,t)                         "Price index between pf and fundPrice for each household"
    savh(h,t)                          "Nominal household saving"
    FundT(t)                           "Total Funds available"
    FundG(t)                           "Funds available for public debt from domestic lenders"
    FundF(t)                           "Funds available for public debt from foreign  lenders"
    FundP(t)                           "Funds available for private debt"
    FundPrice(t)                       "Average returns to savings in %"
    FundPriceD(t)                      "Average returns to savings in %, dual"
    interest(t)                        "Interest on government debt"
    totTr(inst,t)                      "Total value of transfers paid out by institution"
    YTaxRedist(t)                      "Ammount of tax revenues redistribued as transfers to domestic institutions"
    chiTotTr(inst,t)                   "Total transfer share of incomes of domestic institutions"
    avgDebtInt(inst,t)                 "Average interest on domestic debt stock over past years"
    debtStk(inst,t)                    "Domestic Debt stock"
    debtStkT(t)                        "Total Debt stock"
    debtGDP(t)                         "Debt to GDP ratio"
    debtPay(inst,t)                    "Total Debt payment to inst"
    debtInterest(inst,t)               "Debt interest payment to inst"
    debtPrincipal(inst,t)              "Debt Principal payment to inst"
    debtNew(inst,t)                    "Debt New from inst"
    debtNewT(t)                        "Total Debt New"
    rDebtStk(inst,t)                   "Repayment ratio for debt to inst"
    rDebtStkT(t)                       "repayment ratio for debt to all institutions"
    riskPrem(inst,t)                   "Risk premium on debt from inst = interest on foreign debt - interest on  domestic debt"
    chiSavGDebt(t)                     "Debt Shifter to equalize govBalance to new debt"

*   --- Foreign capital variables

    ror(t)                             "Net aggregate rate of return to capital"
    rorc(t)                            "Cost adjusted rate of return to capital"
    rore(t)                            "Expected rate of return to capital"
    devRoR(t)                          "Change in rate of return"
    grK(t)                             "Anticipated growth of the capital stock"
    rord(t)                            "Country risk premium"
    rorg(t)                            "Average expected global rate of return to capital"
    chigrK(t)                          "Elasticity parameter in growth of capital"
    eta1(t)                            "Efficiency param. returns to capital"
    eta2(t)                            "Efficiency param. GDP p.c. growth"
    eta1a(t)                           "Efficiency param. returns to capital"
    eta2a(t)                           "Efficiency param. GDP p.c. growth"
    epsRoR(t)                          "Sensitivity of rate-of-return expectation"
    epsRoRE(t)                         "Sensitivity of rate-of-return expectation"
    invtilde(t)                        "Investment interim value or potential investment"
    savfGDP(t)                         "Foreign savings GDP ration"

    yg(g,t)                            "Vector of government tax revenues"
    ygTot(t)                           "Total government tax revenues"
    ygov(t)                            "Total government revenues"
    gtax(t)                            "Government direct taxes"
    dgovtax(t)                         "Direct tax rate on government"

    ygPhn(a,t)                         "Phantom Taxes"
    ygEmi(aa,t)                        "Emission Taxes on commodity use"
    ygEmiXP(a,t)                       "Emisison taxes on production"
    ygPit(h,t)                         "Personal income tax"
    ygCit(entr,t)                      "Corporate income tax"
    ygVatDom(i,t)                      "VAT on domestic goods"
    ygVatImp(i,t)                      "VAT on imported goods"
    ygPaf(i,t)                         "Agent specific sales tax"

    fiscalBalance(t)                   "Total Deficit = primary + interest payment"
    primaryBalance(t)                  "Primary Deficit = revenue - current and capital spending"
    govBalance(t)                      "Public Budget Balance = primary deficit + debt - Principal payment"
    rgovBalance(t)                     "Real government deficit"

    entrY(entr,t)                      "Enterprise income"
    entrTax(entr,t)                    "Enterprise direct taxes"
    savEntr(entr,t)                    "Enterprise savings"
    apsEntr(entr,t)                    "Enterprise savings rate"

    savf(t)                            "Foreign savings in foreign currency terms (finance new foreign debt and foreign investments)"
    bot(t)                             "BOT"
    botGDP(t)                          "B.O.T. to GDP ratio"

    transfers(inst,instp,t)            "Transfers across institutions"
    chiTr(inst,instp,t)                "Transfer shares"
    chiTaxRedist(inst,t)               "Share of receiving inst in taxes redistributed as transfers"
    rowtransfers(inst,t)               "Exogenous transfers from ROW in foreign currency units"

*   --- Aggregate expenditure variables

    yf(oa,t)                           "Aggregate nominal expenditures on goods and services"
    xfd(oa,t)                          "Aggregate real expenditurs on goods and services"
    pf(oa,t)                           "Aggregate expenditure price index"

*   --- CDE variables

    cdeInt(h,kNest,t)                  "Scaling factor in household demand equation"
    consScale(h,t)                     "Scaling factor in household demand equation"
    uh(h,t)                            "Household utility function"
    mmuh(h,t)                          "Household money metric utility function"
    hpop(h,t)                          "Household population"

    eh(h,kNest,t)                      "CDE expansion parameters"
    bh(h,kNest,t)                      "CDE substitution parameters"

    incelas(h,kNest,t)                 "Income elasticity"
    prcElas(h,kNest,kNest,t)           "Uncompensated demand elasticity"

*   --- Household demand variables

    xakShr(oa,kNest,t)                 "Budget shares"

*   --- Other demand variables

    xak(oa,kNest,t)                    "Demand for demand nest"
    pxak(oa,kNest,t)                   "Price for demand nest"

    xmarg(j,t)                         "Demand for margin good j"
    pmargInd(j,t)                      "Price of margin good j, price index under additive CES"
    pmarg(j,t)                         "Price of marigin good j"

    gammaInv(i,a,t)                    "Activity specific share of investments into good i"
    newInv(t)                          "Total new investments"
    invPrice(a,t)                      "Investment price index for activity a"

*   --- Trade variables

    xa(i,aa,t)                         "Armington demand"
    xat(i,t)                           "Aggregate Armington demand"
    pa(i,t)                            "Armington price"
    paIND(i,t)                         "Price index of i for import and domestic demand ACET"
    paf(i,aa,t)                        "Tax inclusive Armington price"

    xd(i,t)                            "Domestic supply of domestically produced goods"
    pd(i,t)                            "Producer price of domestic supply"
    pda(i,t)                           "Agents price of domestic supply"
    xe(i,t)                            "Domestic supply of exported goods"
    pe(i,t)                            "Producer price of exported goods"
    pwe(i,t)                           "World price of domestic exports"

    xm(i,t)                            "Imports"
    pwm(i,t)                           "World price of domestic imports"
    pma(i,t)                           "Agents price of imports"

*   --- Factor supplies

    xfmSup(fmAll,fmp,instTot,vf,t)     "Factor supply of factor nest fmPar linked to factor p"
    pfmSup(fmAll,fmp,instTot,vf,t)     "Factor supply price of factor nest fmPar linked to factor p, exhaustion or default CET case"
    pfmSupD(fmAll,fmp,instTot,vf,t)    "Factor supply price of factor nest fmPar linked to factor p, additive dual case"
    pDfmSup(fmNest,inst,fmp,t)         "Dual factor supply price of factor nest fmPar linked to factor p, for additive CET"
    atFmSup(fmAll,instTot,vf,t)        "Aggregate factor market supply parameter (also for shifts)"
    xfmSupUpdateFac(fmAll,instTot,t)    "Update factor for factor supply, for instance to consider differences in saving rates"

    rrat(a,t)                           "Rate of return of old capital relative to new capital"
    rrat1(a,t)                          "Rate of return of old capital relative to new capital"
    trent(t)                            "Economy wide rate of return to new capital"
    trentAvg(t)                         "Economy wide rate of return to new capital"
    oldCap(a,t)                         "Previous year capital stock"

    kaps(t)                             "Aggregate capital stock normalized"
    prodInv(t)                          "Productive investments"
    kstock(t)                           "Aggregate capital stock in level"
    kstocke(t)                          "Anticipated capital stock"
    depr(t)                             "Economy wide depreciation rate"
    deprA(a,t)                          "Activity specific depreciation rate"

*   --- Demographics

    pop(cohorts,t)                      "Population"

    unemp(t)                            "Unemployed population"
    deprate(cohorts,t)                  "Dependency ratio by age cohort"
    popAge(age,t)                       "Poplulation at each age"
    brateU(t)                           "Birth rate"
    drate(age,t)                        "Death rate"

*  --- R&D variables #ENDOTECH

    xRD(a,t)                            "R&D intensity by activity"
    xfdRD(is,t)                         "Total R&D effort (factors of production and products)"
    einn(a,t)                           "Expected number of innovations available next period"
    pfdrd(t)                            "Price index for R&D inputs"
    frontBGP(a,t)                       "Progress of tech frontier"
    yfdRD(t)                            "R&D expenses"

*   --- Emission variables

    emiComm(em,i,aa,t)                  "Emissions for each cell of the SAM"
    emiFact(em,fpnest,a,t)              "Emissions for each cell of the SAM"
    emiXP(em, a, t)                     "Emissions for overall activity levels"
    emiLUCF(em,a,t)                     "Emissions from land use change"
    emiTot(em,t)                        "Total emissions"
    emiOth(em,t)                        "Exogenous other emissions"
    emiGHG(polltype,tf)                 "Greenhouse gas emissions"
    emiInv(ghgInv,em,t)                 "Emissions by user categories in emission inventories"
    emiInt(polltype,t)                  "Emission intensity of GDP"
    emiIntSec(a,t)                      "Air emission intensity of production in sector a"
    emiIntHH(h,t)                       "Emission intensity of consumption by HH type h"
    emical(t)                           "Emission Calibration factor"
    emiSecA(em,aa,t)                    "Emissions by Sector"

    chiEmi(em,i,aa,t)                   "Shifters on emissions per unit of demand"
    chiEmiFac(em,fpnest,aa,t)           "Shifters on emissions per unit of demand"
    chiEmiXP(em,a,t)                    "Shifters on emissions per unit of activity"
    chiEmiAA(aa,t)                      "Shifters on emissions per unit of demand for all commodities and emission types"
    elyInt(t)                           "Electricity intensity of GDP"
    nrgInt(t)                           "Energy intensity of GDP"
    ExposedAirpoll(t)                   "Exposed air pollution"

    dCarbon(fpNest,lcov,t)              "Carbon stock changes by land cover type"
    lCovArea(fpNest,lcov,t)             "Land cover type areas"

*   --- Macro variables

    tdelst(t)                           "Aggregate nominal change in stocks"
    rtdelst(t)                          "Aggregate real change in stocks"
    pdelst(t)                           "Aggregate change in stocks price deflator"

    totExp(t)                           "Aggregate nominal exports"
    rTotExp(t)                          "Aggregate real exports"
    pexp(t)                             "Afgregate export price deflator"
    totimp(t)                           "Aggregate nominal imports"
    rtotimp(t)                          "Aggregate real imports"
    pimp(t)                             "Aggregate import price deflator"

    pgdpmp(t)                           "GDP at market price deflator"
    gdpmp(t)                            "Nominal GDP at market price"
    rgdpmp(t)                           "Real GDP at market price"

    rgdppc(t)                           "Real GDP per capita"
    ggdppc(t)                           "Per capita GDP growth"

    pgdpfc(t)                           "GDP at factor cost excl indirect taxes deflator"
    gdpfc(t)                            "GDP at factor cost excl indirect taxes"
    rgdpfc(t)                           "Real GDP at factor cost excl indirect taxes"

    rshrGDP(oa,t)                       "Real share of GDP"
    nshrGDP(oa,t)                       "Nominal share of GDP"
    GdpCompShr(nipa,t)                  "Share of NIPA elements in GDP"
    fiscalRespDrv1(t)                   "Driver for fiscal responability adjustment (based sigmoid function and fudged max), upper limit"
    fiscalRespDrv(t)                    "Driver for fiscal responability adjustment (based sigmoid function and fudged min), lower limit"
    cpi(t)                              "Choice of domestic deflator"

    walras                              "Walras law"

*   --- Productivity variables

    lambdaOa(oa,i,kNest,t)              "Efficiency in demand"
    lambdaNRG(t)                        "Energy efficiency for all agents"


    chixFacF(fpNest,t)                  "Product. shifter, nest fpNest, all activities and vintages"
    chixFacA(a,t)                       "Product. shifter, activity a for all nests and vintages"
    chixFacV(v,t)                       "Product. shifter, vintage v for all activities and nests"
    chixFacFA(fpnest,a,t)               "Product. shifter, activity a and nest fpnest for all vintages"
    chixFacL(t)                         "Product. shifter, labor in all sectors"
    chixFacK(t)                         "Product. shifter, capital in all sectors"
    chixFacKL(t)                        "Product. shifter, labor and capital in all sectors"
    chixFacFP(t)                        "Product. shifter, all natural factors of production in all sectors"
    chixFacNRG(t)                       "Product. shifter, energy carriers"
    chixFacAgr(aagr,fpNest,t)           "Product. shifter, all activities under sector group aagr"
    labProd(t)                          "Yearly change in labor productivity, such as 0.1 (not a factor such as 1.05)"

*   --- Policy variables

    tFac(a,fpnest,t)                    "Factor tax"
    rtFac(a,fpnest,t)                   "Real Factor tax"
    tFac0(a,fpnest,t)                   "Factor tax base level"
    tFacD0(t)                           "Factor tax additive adjuster for all dimensions"
    tFacD1(a,t)                         "Factor tax additive adjuster for activity a"
    tFacD2(fpnest,t)                    "Factor tax additive adjuster for factor fp"
    tFacM0(t)                           "Factor tax multiplicative adjuster for all dimensions"
    tFacM1(a,t)                         "Factor tax multiplicative adjuster for activity a"
    tFacM2(fpnest,t)                    "Factor tax multiplicative adjuster for factor fp"
    tfacMLab(t)                         "Factor tax multiplicative adjuster for labor"
    tfacMLabA(a,t)                      "Factor tax multiplicative adjuster for labor"
    tfacMCap(t)                         "Factor tax multiplicative adjuster for capital"
    tfacMlnd(t)                         "Factor tax multiplicative adjuster for land"

    tpsb(a,t)                           "Production Subsidy"
    tpsb0(a,t)                          "Production Subsidy base level"
    tpsbD0(t)                           "Production Subsidy additive adjuster for all dimensions"
    tpsbD1(a,t)                         "Production Subsidy additive adjuster for activity a"
    tpsbM0(t)                           "Production Subsidy multiplicative adjuster for all dimensions"
    tpsbM1(a,t)                         "Production Subsidy multiplicative adjuster for activity a"

    tPrd(a,t)                           "Production Tax"
    tPrd0(a,t)                          "Production Tax base level"
    tPrdD0(t)                           "Production Tax additive adjuster for all dimensions"
    tPrdD1(a,t)                         "Production Tax additive adjuster for activity a"
    tPrdM0(t)                           "Production Tax multiplicative adjuster for all dimensions"
    tPrdM1(a,t)                         "Production Tax multiplicative adjuster for activity a"

    tPhn(a,t)                           "Phantom Tax"
    tPhn0(a,t)                          "Phantom Tax base level"
    tPhnD0(t)                           "Phantom Tax additive adjuster for all dimensions"
    tPhnD1(a,t)                         "Phantom Tax additive adjuster for activity a"
    tPhnD2(aagr,t)                      "Phantom Tax additive adjuster for activity group aagr"

    tExp(i,t)                           "Export tax"
    tExp0(i,t)                          "Export tax base level"
    tExpD0(t)                           "Export tax additive adjuster for all dimensions"
    tExpD1(i,t)                         "Export tax additive adjuster for activity a"
    TExpM0(t)                           "Export tax multiplicative adjuster for all dimensions"
    tExpM1(i,t)                         "Export tax multiplicative adjuster for commodity i"

    tVat(i,n,t)                         "Value added tax"
    tVat0(i,n,t)                        "Value added tax base level"
    tVatD0(t)                           "Value added tax additive adjuster for all dimensionsnsion"
    tVatD1(i,t)                         "Value added tax additive adjuster for commodity i"
    tVatD2(n,t)                         "Value added tax additive adjuster for source n"
    tVatM0(t)                           "Value added tax multiplicative adjuster for all dim"
    tVatM1(i,t)                         "Value added tax multiplicative adjuster for commodity i"
    tVatM2(n,t)                         "Value added tax multiplicative adjuster for source n"

    tImp(i,t)                           "Import tax"
    tImp0(i,t)                          "Import tax base level"
    tImpD0(t)                           "Import tax additive adjuster for all dimensions"
    tImpD1(i,t)                         "Import tax additive adjuster for commodity i"
    tImpM0(t)                           "Import tax multiplicative adjuster for all dimensions"
    tImpM1(i,t)                         "Import tax multiplicative adjuster for commodity i"

    tEmi(em,aa,t)                       "Agent specific emission tax"
    rtEmi(em,aa,t)                      "Agent specific emission tax"
    tEmi0(em,aa,t)                      "Agent specific emission tax base level"
    tEmiD0(t)                           "Emission tax additive adjuster for all dimensions"
    tEmiD1(em,t)                        "Emission tax additive adjuster for emissions em"
    tEmiD2(aa,t)                        "Emission tax additive adjuster for agent aa"
    tEmiM0(t)                           "Agent specific emission tax multiplicative adjuster for all dim"
    tEmiM1(em,t)                        "Agent specific emission tax multiplicative adjuster for emission em"
    tEmiM2(aa,t)                        "Agent specific emission tax multiplicative adjuster for agent aa"

    tPIT(h,t)                           "Personal income tax"
    tPIT0(h,t)                          "Personal income tax base level"
    tPITD0(t)                           "Personal income tax additive adjuster for all dimensions"
    tPITD1(h,t)                         "Personal income tax additive adjuster for household h"
    tPITM0(t)                           "Personal income tax multiplicative adjuster for all dimensions"
    tPITM1(h,t)                         "Personal income tax multiplicative adjuster for household h"

    tCIT(entr,t)                        "Corporate income tax"
    tCIT0(entr,t)                       "Corporate income tax base level"
    tCITD0(t)                           "Corporate income tax additive adjuster for all dimensions"
    tCITD1(entr,t)                      "Corporate income tax additive adjuster for enterprise entr"
    tCITM0(t)                           "Corporate income tax multiplicative adjuster for all dimensions"
    tCITM1(entr,t)                      "Corporate income tax multiplicative adjuster for enterprise entr"

    tDr(t )                             "Personal and Corporate income tax multiplicative adjuster"

    tmAll(t)                            "Shifter for all taxes"

    tPaf(i,aa,t)                        "Agent specific commodity tax"
    tPaf0(i,aa,t)                       "Agent specific commodity tax base level"
    tPafD0(t)                           "Commodity tax additive adjuster for all dimensions"
    tPafD1(i,t)                         "Commodity tax additive adjuster for commodity i"
    tPafD2(aa,t)                        "Commodity tax additive adjuster for agent aa"
    tPafM0(t)                           "Commodity tax multiplicative adjuster for all dimensions"
    tPafM1(i,t)                         "Commodity tax multiplicative adjuster for commodity i"
    tPafM2(aa,t)                        "Commodity tax multiplicative adjuster for agent aa"

*   --- Exogenous variables

    tmg(i,n,t)                          "Trade and transport margins"
    pwmROW(i,t)                         "Global price of imports"
    pweROW(i,t)                         "Global export price"
    er(t)                               "Exchange rate"

*   --- CC damages

    dmgAir(fpnest,a,t)                   "Air pollution impact on labor productivity"
    dmgPrd(fpnest,a,chnl,t)              "CC Damage to Factor Productivity"
    dmgYld(fpnest,a,chnl,t)              "CC Damage to Factor Yield"
    dmgSup(fpnest,chnl,t)                "CC Damage to Factor Supply - factor type "
    dmgStk(*,chnl,t)                     "CC Damage to Factor Stock - pop by age, capital, land types"
    dmgDem(oa,i,knest,chnl,t)            "CC Damage to household demand"
    dmgTrd(i,chnl,t)                     "CC Damage to exports of commodity"
    dmgDepr(a,chnl,t)                    "CC Damage to depreciation rate of commodity"

*   --- other

    adpInvShr(oa,t)                     "Share of adaptation investment in public and private investment"
    adpInvShft(oa,t)                    "Shift of adaptation investment"
    chiP(a,i,t)                         "Supply by activity price normalizer"
    SectTot(t)                          "Total factor demand"
    SectShr(aagr,t)                     "Sector share in VA"
    dummy
  ;

  equations
    e_xfac(fpNest,a,v,t)                "Demand for factor fpnest under the nest fpnestp"
    e_pxfac(fpNest,a,v,t)               "Price index of nest fpNest"
    e_pxfacInd(fpNest,a,v,t)            "Price index of nest fpNest, exhaustion, for additive CES"
    e_pxfac_preTax(fpNest,a,v,t)        "Tax inclusive factor price"
    e_lambdaxfac(fpNest,a,v,t)          "Factor augmenting (Hicks Neutral) technological change"
    e_twist(fpchl,fpNest,a,v,t)         "Cost neutral twist in share parameters, additive"
    e_shrxFac(fpchl,fpNest,a,v,t)       "Actual cost shares"
    e_yield(fp,a,t)                     "Factor Yield"
    e_yieldLnd(a,t)                     "Yield per unit of land"
    e_diffLabProd(fpNest,a,t)           "Labor productivity differs across broad sectors"
    e_LandProd(fpNest,t)                "Land productivity changes depend on labor productivity changes"
    e_NatresProd(fpNest,t)              "Natural resource productivity changes depend on labor productivity changes"
    e_capProd(fpNest,t)                 "Capital productivity changes depend on labor productivity changes"

    e_px(a,t)                           "Average unit cost"
    e_pp(a,t)                           "Output price"

    e_xap(i,a,t)                        "Demand for commodities used as childs in production nests"
    e_pap(fpnest,a,v,t)                 "Price for intermediates as childs in production nests"

    e_x(a,i,t)                          "Supply of i from activity a"
    e_xp(a,t)                           "CET revenue function"

    e_p(a,i,t)                          "Demand for i from activity a"
    e_xs(i,t)                           "CES revenue function"
    e_pIND(i,t)                         "Value additivity constraint for supply of goods by multiple activities"

    e_facty(fpNest,t)                   "Factor income before direct taxes"
    e_kapy(t)                           "Gross profits"
    e_yh(h,t)                           "Gross household income"
    e_corrFac(fpNest,t)                 "Correction factor between factor incoe to flows to institutions"
    e_yhDsp(h,t)                        "Disposable household income"
    e_gini(t)                           "Gini coefficient"
    e_deprate(cohorts,t)                "Dependency ratio definition"
    e_aps(h,t)                          "Household saving rate"
    e_hhtr(inst,t)                      "Total household transfers"
    e_yfh(h,t)                          "Household expenditures on goods and services"
    e_pindh(h,t)                        "Price index between pf and fundPrice for each household"
    e_savh(h,t)                         "Household savings"
    e_avgDebtInt(inst,t)                "Average interest on domestic debt stock over past years"
    e_FundT(t)                          "Total Funds available"
    e_FundG(t)                          "Funds available for public debt"
    e_FundP(t)                          "Funds available for private debt"
    e_FundF(t)                          "Funds available for public debt from foreign lenders"
    e_FundPrice(t)                      "Exhaustion price of returns to capitan and interest, normalized"
    e_FundPriceD(t)                     "Composite price of returns to capital in % and interest rate in %"
    e_interest(t)                       "interest rate on government debt"
    e_tPit(h,t)                         "Household direct taxes"
    e_tFac(a,fp,t)                      "Factor tax definition"
    e_rtFac(a,fp,t)                     "Factor tax definition"
    e_tpsb(a,t)                         "Production subsidy definition"
    e_tPrd(a,t)                         "Production tax definition"
    e_tPhn(a,t)                         "Production tax definition"
    e_tExp(i,t)                         "Export tax definition"
    e_tImp(i,t)                         "Import tax definition"
    e_tEmi(em,aa,t)                     "Agent specific emission tax definition"
    e_rtEmi(em,aa,t)                    "Real emission Tax"
    e_tPIT(h,t)                         "Personal income tax definition"
    e_tCIT(entr,t)                      "Corporate income tax definition"
    e_tVat(i,n,t)                       "VAT tax definition"
    e_tPaf(i,aa,t)                      "Agent specific commodity tax definition"

    e_ygPhn(a,t)                        "Production Taxes"
    e_ygEmi(aa,t)                       "Emission Taxes on commodity use"
    e_ygEmiXP(a,t)                      "Emisison taxes on production"
    e_ygPit(h,t)                        "Personal income tax"
    e_ygCit(entr,t)                     "Corporate income tax"
    e_ygVatDom(i,t)                     "VAT on domestic goods"
    e_ygVatImp(i,t)                     "VAT on imported goods"
    e_ygPaf(i,t)                        "Agent specific sales tax"

    e_ygTotFacLand(g,t)                 "Land Factor Tax revenue"
    e_ygTotFacNatres(g,t)               "Natural resource Tax revenue"
    e_ygTotFacLab(g,t)                  "Labor Factor Tax revenue"
    e_ygTotFacCap(g,t)                  "Capital Factor Tax revenue"
    e_ygTotpsb(g,t)                     "Production Subsidies revenue"
    e_ygTotPrd(g,t)                     "Production Taxes revenue"
    e_ygTotPhn(g,t)                     "Production Taxes revenue"
    e_ygTotExp(g,t)                     "Export Taxes revenue"
    e_ygTotImp(g,t)                     "Import Taxes revenue"
    e_ygTotEmi(g,t)                     "Emission Taxes on commodity use revenue"
    e_ygTotEmiXP(g,t)                   "Emisison taxes on production revenue"
    e_ygTotPit(g,t)                     "Personal income tax revenue"
    e_ygTotCit(g,t)                     "Corporate income tax revenue"
    e_ygTotVatDom(g,t)                  "VAT on domestic goods revenue"
    e_ygTotVatImp(g,t)                  "VAT on imported goods revenue"
    e_ygTotPaf(g,t)                     "Agent specific sales tax revenue"
    e_ygTot(t)                          "Total Tax revenue"

    e_ygov(t)                           "Gross government revenues"
    e_gtax(t)                           "Government taxes"
    e_govTr(inst,t)                     "Total government transfers"
    e_primaryBalance(t)                 "Primary Deficit = revenue - current and capital spending"
    e_fiscalBalance(t)                  "Total Deficit = primary + interest payment"
    e_govBalance(t)                     "Public Budget Balance = primary deficit + debt - Principal payment"
    e_rgovBalance(t)                    "Real government deficit"

    e_entrY(entr,t)                     "Enterprise income"
    e_tCit(entr,t)                      "Enterprise taxes"
    e_savEntr(entr,t)                   "Enterprise savings"
    e_entrTr(inst,t)                    "Total enterprise transfers"

    e_dtransfers(inst,instp,t)          "Allocation of total domestic transfers across destinations"
    e_rowtransfers(inst,instp,t)        "Transfers from ROW"

    e_hpop(h,t)                         "Household population (currently constant base shares)"
    e_cdeInt(h,kNest,t)                 "Scaling factor in household demand equation"
    e_consScale(h,t)                    "Scaling factor used in CDE demand system"
    e_xakh(h,kNest,t)                   "Household demand for goods and services"
    e_uh(h,t)                           "CDE utility function"
    e_mmuh(h,t)                         "CDE moneymetric utility function"
    e_pfh(h,t)                          "Household expenditure deflator"

*   --- Used for calibrating CDE demand system


    e_xakShr(oa,kNest,t)                "Budget shares"
    e_pf(f,t)                           "Other final demand expenditure deflator"
    e_xakf(f,kNest,t)                   "Demand for bundles in other final demand"
    e_xaoa(i,oa,t)                      "Armington demands by household"
    e_xak(oa,kNest,t)                   "Demands for demand nests"
    e_pxak(oa,kNest,t)                  "Dual price aggregator for demand nests"
    e_xmarg(j,t)                        "Demand for margin good j"
    e_xarmMarg(i,j,t)                   "Armington demand for good i to deliver good j"
    e_pmarg(j,t)                        "Price of margin good j"
    e_pmargInd(j,t)                     "Price index for composite margin good j, dual price aggregator CES"
    e_yf(oa,t)                          "Volume and or price split for aggregate expenditures"

    e_afInv(inv,i,knest,t)              "Economy-wide investment share of product i"
    e_gammaInv(i,a,t)                   "Activity specific investment shares for products"
    e_invPrice(a,t)                     "Activity specific investment price index"
    e_newInv(t)                         "Sum of capital demands by new vintage"
    e_xfdInv(adpinv,t)                  "Add exogenous adaptation capital demand"

    e_pma(i,t)                          "Agents price of imports"
    e_pda(i,t)                          "Agents price of domestic goods"

    e_xat(i,t)                          "Aggregate Armington demand"
    e_xdd(i,t)                          "Demand for domestic production"
    e_xm(i,t)                           "Demand for imports"
    e_pa(i,t)                           "Armington price"
    e_paInd(i,t)                        "Value additivity constraint for demand from domestic markets and imports"
    e_paf(i,aa,t)                       "Armington price inclusive of all taxes"

    e_xds(i,t)                          "Domestic supply of production"
    e_eds(i,t)                          "Export supply of production"
    e_ps(i,t)                           "Total production"
    e_psind(i,t)                        "Supply side market clearing condition in quantities"
    e_pe(i,t)                           "Domestic price of exports"
    e_pwe(i,t)                          "Border price of exports"
    e_pwm(i,t)                          "Border price of imports"
*
*   --- flexible factor markets
*
    e_atCap(fmp,instTot,vf,t)           "Capital supply is equal to kaps, vinatges are off"
    e_atCapVint(fmAll,fmp,instTot,vf,t) "Capital supply is equal to kaps, vinagtes are on"
    e_trent(t)                          "Rental rate trent equal supply price of capital, if vintages are off"
    e_trentInst(fmp,fmp,instTot,vf,t)   "Rental rate trent equal supply price of capital, if vintages are off"
    e_pfmSupCap(fmAll,fmp,instTot,vf,t) "Rental rate trent equal supply price of capital"
    e_xfacFp(fp,a,v,t)                  "Factor demand equals factor supply for activities"
    e_xfacFpCap(fmAll,fmp,instTot,vf,t) "Factor demand equals factor supply for activities"
    e_xfmSup(fmAll,fmp,instTot,vf,t)    "Factor supply of factor nest fmPar linked to factor p"
    e_xfmSupTot(fmAll,fmp,instTot,vf,t) "Summing up over institution"
    e_pfmSup(fmAll,fmp,instTot,vf,t)    "Factor supply price of factor nest fmPar linked to factor p, exhaustion"
    e_pfmSupD(fmAll,fmp,instTot,vf,t)   "Factor supply price of factor nest fmPar linked to factor p, dual case, standard or additive"
    e_pDfmSup(fmNest,inst,fmp,vf,t)     "Dual factor supply price of factor nest fmPar linked to factor p, for additive CET"
    e_unemp(t)                          "Unemployed population equation"

*     --- Capital market equations in comparative static

    e_ksCS(cap,a,v,t)                   "Sectoral capital supply"
    e_trentCS(t)                        "Aggregate rent"

*     --- Capital market equations in dynamics

    e_oldCap(a,t)                       "Definition of old capital"
    e_rrat1(a,t)                        "Rate of return to old capital relative to new capital"
    e_rrat(a,t)                         "Rate of return to old capital relative to new capital"
    e_trentKaps(t)                      "Kaps equal to summed up capital, defines Economywide rate of return to new capital"
    e_trentAvg(t)                       "Economywide rate of return to new capital"
    e_pk(cap,a,v,t)                     "Sectoral rate of return to capital by vintage"
    e_xpOld(fpNest,a,v,t)               "Determination of output with installed capital"

*     --- Vintage or processes production constraint

    e_xpNew(fpNest,a,v,t)               "xp is the sum over vintages or equal old if no vintages are active"
    e_xpOldNoCap(fpNest,a,v,t)          "xp is the sum over vintages or equal old if no vintages are active"
    e_xpEnTech(fpnest,a,v,t)            "Demand for new and old processes #ENDOTECH"

*     --- stock and stock changes

    e_psExpRel(i,t)                     "Delta expected and current price over current price"
    e_psExp(i,t)                        "Definition of expected prices"
    e_xsExp(i,t)                        "Definition of expected output"
    e_st(i,t)                           "Stock flow balance"
    e_delst(i,t)                        "Change in stock from domestic production"

*     --- Macro identities
    e_tdelst(t)                         "Aggregate nominal change in stocks"
    e_rtdelst(t)                        "Aggregate real change in stocks"
    e_pdelst(t)                         "Change in stock deflator"
    e_totexp(t)                         "Aggregate nominal exports"
    e_rtotexp(t)                        "Aggregate real exports"
    e_pexp(t)                           "Aggregate export price deflator"
    e_totimp(t)                         "Aggregate nominal imports"
    e_rtotimp(t)                        "Aggregate real imports"
    e_pimp(t)                           "Aggregate import price deflator"

    e_gdpmp(t)                          "Nominal GDP at market price"
    e_rgdpmp(t)                         "Real GDP at market price"
    e_pgdpmp(t)                         "GDP at market price deflator"

    e_rgdppc(t)                         "Definition of real GDP per capita"
    e_ggdppc(t)                         "Growth of per capita GDP"

    e_gdpfc(t)                          "Nominal GDP at factor cost"
    e_rgdpfc(t)                         "Real GDP at factor cost"
    e_pgdpfc(t)                         "GDP at factor cost deflator"

    e_rshrGDP(oa,t)                     "Real share of GDP components"
    e_rshrGDPGov(f,t)                   "Real share of GDP for Government"
    e_fiscalRespDrv1(t)                 "Driver for fiscal responability adjustment (based sigmoid function and fudged max), upper limit"
    e_fiscalRespDrv(t)                  "Driver for fiscal responability adjustment (based sigmoid function and fudged max), lower limit"
    e_fiscalResp(t)                     "Government share on GDP at benchmark plus dampening/expanding if drivers kick in"
    e_nshrGDP(oa,t)                     "Nominal share of GDP"
    e_GdpCompShr(nipa,t)                "Share of NIPA elements in GDP"
    e_GdpCompShrX(*,t)                  "Export share in GDP"
    e_GdpCompShrM(*,t)                  "Import share in GDP"

    e_cpi(t)                            "Choice of domestic price deflator"
    e_walras(t)                         "Walras law"
    e_BOT(t)                            "Define BOT"
    e_BOTGdp(t)                         "Define BOT to GDP ratio"

*     --- R&D equations #ENDOTECH

    e_xfdRD(is,t)                       "Total R&D effort (factors of production and products)"
    e_xRD(a,t)                          "R&D intensity"
    e_yfdRD(t)                          "Totoal R&D expenses"
    e_pfdrd(t)                          "Price of R&D input"
    e_einn(a,t)                         "Expected number of innovations available next period"
    e_intot(a,t)                        "Total number of active innovations"

*     --- Emissions module

    e_ElecMix(a,i,t)                    "Electricity output"
    e_emiComm(em,i,aa,t)                "Emissions per unit of demand"
    e_emiFact(em,fp,a,t)                "Emissions per unit of factor use"
    e_emiLUCF(em,a,t)                   "Emissions from land use change"
    e_emiProd(em,a,t)                   "Emissions per unit of activity"
    e_emiTot(em,t)                      "Total emissions"
    e_emiGHG(polltype,tf)               "Total emissions of GHG in carbon equivalent"
    e_emiInv(ghgInv,em,t)               "emissions by user categories in emission inventories"
    e_emiInt(polltype,t)                "emission intensity of GDP"
    e_emiIntSec(a,t)                    "Air emission intensity of production in sector a"
    e_emiIntHH(h,t)                     "emission intensity of consumption by HH type h"
    e_emiSecA(em,aa,t)                  "Emissions by sector"
    e_elyInt(t)                         "Electricity intensity of GDP"
    e_nrgInt(t)                         "Energy intensity of GDP"
    e_ExposedAirpoll(t)                 "Exposed air pollution"
    e_dCarbon(fpNest,lcov,t)            "Carbon stock changes by land cover type"
    e_lCovArea(fpNest,lcov,t)           "Land cover type areas"
    e_builtUpArea(lnd,lcov,t)           "Land cover type areas"

*     --- Demographics

    e_popAge(age,t)                     "Population in each age group definition"
    e_Birth(*,t)                        "Number of births"
    e_pop(cohorts,t)                    "Population equation"

*     --- Dynamic equations

    e_kaps(t)                           "Capital normalization equation"

*     --- debt equations

    e_DebtStk(inst,t)                   "Domestic debt stock"
    e_DebtStkT(t)                       "Total debt stock"
    e_debtGDP(t)                        "Debt to GDP ratio"
    e_debtPay(inst,t)                   "Debt payments"
    e_debtInterest(inst,t)              "Debt payments"
    e_debtPrincipal(inst,t)             "Debt payments"
    e_debtNew(inst,t)                   "New domestic debt"
    e_debtNewT(t)                       "New total debt"
    e_inv(inv,t)                        "Investment equation"

*   -- Foreign saving closure equations

    e_kstocke(t)                        "Anticipated end-of-period capital stock"
    e_ror(t)                            "Net aggregate rate of return to capital"
    e_rorc(t)                           "Cost adjusted rate of return to capital"
    e_rore(t)                           "Expected rate of return to capital"
    e_devROR(t)                         "Change in rate of return"
    e_grK(t)                            "Anticipated growth of the capital stock"
    e_savf(t)                           "Capital account closure"
    e_savfGDP(t)                        "Relation between savf and GDP"

*     --- climate change damages

    e_dmgAir(l,a,t)                     "CC damage to human health"

*     --- other

    e_depr(t)                           "Define average economy-wide depreciation rate"
    e_prodInv(t)                        "Define productive investments"
    e_kstock(t)                         "Capital stock"
    e_SectTot(t)                        "Total demand by all sectors"
    e_SectShr(aagr,t)                   "Sector Shares in VA"
    e_dummy                             "Pseudo equation to maximize"
    ;

  parameters

*   --- Production parameters

    p_sigmaXFac(fpNest,a,v)               "CES Production Elasticity of nest fpNest"
    p_alphaXfac(fpnest,fpnest,a,v)        "CES share parameters"
    p_tw(fpParTc,a,v,t)                   "Used to change the cost neutral shift effect"
    p_ap(a,i,t)                           "CET share parameters for domestic make matrix"
    p_omegaP(a)                           "CET elasticity of domestic make matrix"
    p_as(a,i,t)                           "CES share parameters for domestic use table"
    p_sigmaS(i)                           "CES elasticity for domestic use table"
    p_gammaP(a,i,t)                       "Supply by activity price shifter"

*   --- Household demand

    p_akf(oa,kNest,t)                     "Share parameter for top level household demand"
    p_af(oa,i,KNest,t)                    "CES share parameter for second level final demand bundles"
    p_sigmaC(oa,kNest)                    "CES substitution elasticity between components of demand bundle"

    p_pfini(h)                            "Initial price of unit of utility"
    p_pafini(i,h)                         "Initial tax inclusive Armington price"
    p_afk(oa,knest,knest,t)                "Share parameter of demand nest in demand nest"

*   --- Other demand

    p_sigmaF(f)                           "CES elasticity of substitution for top level final demand bundle"
    p_amarg(i,j)                          "CES share parameters for margin demand"
    p_sigmaMg(j)                          "CES substitution for margin demand"
    p_sigmaInv(a)                         "Investment bundle substitution elasticity for each activity"

*   --- Armington CES function

    p_ad(i,t)                             "Domestic demand share parameter"
    p_am(i,t)                             "Import demand share parameter"
    p_sigmaM(i)                           "Armington CES elasticity"
    p_chiPDa(i)                           "Domestic price normalization factor"
    p_chiPMa(i)                           "Import price normalization factor"
    p_chiPA(i)                            "Armington price normalization factor"
    p_gammaMd(i,t)                        "Domestic price shifter"
    p_gammaMm(i,t)                        "Import price shifter"

*   --- Supply CET function

    p_gd(i,t)                             "Domestic supply share parameter"
    p_ge(i,t)                             "Export supply share parameter"
    p_omegaX(i)                           "Transformation elasticity between domestic sales and exports"
    p_chiPD(i)                            "Domestic price normalization factor"
    p_chiPE(i)                            "Export price normalization factor"
    p_chiPS(i)                            "Supply price normalization factor"
    p_gammaXd(i,t)                        "Domestic price shifter"
    p_gammaXe(i,t)                        "Export price shifter"

*   --- saving investment module

    p_omegaFund(t)                        "Transformation elasticity between investments and government bonds"
    p_gammaFundF(t)                       "Multiplicative shifter for new debt from foreign lenders"
    p_gammaInv0(i,a)                     "Benchmark share of investments into good i by activity a"
    p_lambdaInv0(i)                      "Benchmark line-up paramater for investment shares"

*   --- Export demand function

    p_chiE(i,t)                           "Export demand shifter"
    p_etaE(i)                             "Export demand elasticity"
    p_chiPWE(i)                           "Export price normalizer"
    p_gammaPWE(i,t)                       "Export price shifter"
    p_chiPWEx(i)                          "World export price normalizer"

*   --- Import supply function

    p_chiM(i,t)                           "Import supply shifter"
    p_omegaM(i)                           "Import supply elasticity"

*   --- flexible factor markets

    p_omegaFmp(fmAll,vf)                  "Transformation elasticity for factor supply nest"
    p_omegaFmpShort(fmAll,vf)             "Transformation elasticity for factor supply nest, short run"
    p_epsFmp(fmAll,vf)                    "Factor supply elasticity"
    p_epsFmpShort(fmAll,vf)               "Factor supply elasticity, short run"
    p_epsFmpIntens(fpNest,a)              "Factor supply elasticity, short run, at level of individual activities, intensive margin"
    p_epsFmA(fmAll,fmp,vf)                "Factor supply elasticity, single activities"
    p_gammaFmp(fmAll,fmp,instTot,vf,t)    "Share parameter for factor child nest fmNest and factor fmp"
    p_stickyWage(fmAll)                   "Sticky wage parameter"

*   --- Capital market parameters for vintage version of the model

    p_invElas(a)                          "Dis investment elasticity for declining sectors"

*   --- Emission coefficients

    p_rhoEmi(em,i,js)                     "Emissions per unit of demand"
    p_rhoEmiFac(em,fpnest,js)             "Emissions per unit of demand"
    p_rhoEmiXP(em, a)                     "Emissions per unit of activity"
    p_rhoEmiLnd(em,lnd,a)                 "Average carbon intensity by land use CO2 per hectare"
    p_phiEmi(em,i,aa)                     "Level of participation in emissions tax (0 = no 1 = full)"
    p_phiEmiFac(em,fpNest,a)              "Level of participation in emissions tax (0 = no 1 = full)"
    p_phiEmiXP(em,a,t)                    "Level of participation in emissions tax (0 = no 1 = full)"

*   --- R&D and endogenous technology parameters #ENDOTECH

    p_rdprof(a)                           "R&D profit for SAM recalibration"
    p_chiFdRD(is,t)                       "Share of inputs for R&D sector"
    p_phiRD(a,t)                          "Elasticity of knowledge production with respect to R&D intensity"
    p_lambdaEnCr(a,v,t)                   "Vertical (frontier bounded) productivity change corrected for vertical (frontier bounded) productivity change between xfac(top) and xp"
    p_sigmaXpvt                           "Elasticity of substitution between varieties"
    p_egrowth(a,t)                        "Expected growth of revenues of varieties"
    p_markup(a,t)                         "Markup charged by variety producers who own blueprints"
    p_variety(a,t)                        "Number of varieties in sector - drives horizontal (unbounded) endogenous growth"
    p_frontier(a,t)                       "Measure of advancement of global technological frontier"
    p_frontCr(a,t)                        "frontier for centering to zero vertical progress along BGP"
    p_varietyCr(a,t)                      "Factor centering to zero variety growth along Balanced Growth Path"
    p_inn(a,t)                            "Number of innovations that became available in the current period"
    p_xRD0(a)                             "R&D expentiture at time t0"
    p_obsol(a,t)                          "Obsolescence rate"
    p_interestRD(t)                       "Interest rate for R&D projects"
    p_rhoEn                               "Share of innovations leading to horizontal (unbounded) progress"
    p_lambdaEn(a,v,t)                     "Vertical (frontier bounded) productivity change between xfac(top) and xp"
    p_lambdaCr(a,v,t)                     "Factor centering to zero vertical productivity change along BGP"
    p_markupNonComp(fpNest,a,t)           "Markup charged by non-competitive firms"
    p_tolerance

*   --- Output parameters

    p_oscale
    p_samT(is,js,t)                       "Post-sim SAM"
    p_samTDiff(is,js,t)                   "Post-sim SAM"
    p_samTBal(*,is,t)                     "Post-sim SAM balance"

    p_work
*
*   --- others
*
    p_phiPcInv(inv,t)                     "Share of public investment that increases capital stock"
    p_chiRor(t)                           "Shifter for capital supply share of capital stock"
    p_nonProdInv(inv,t)                   "Non-productive investments (e.g. coal phase-out costs)"
    p_AirPollEff(l,a,t)                   "Elasticity of air pollution to labor productivity in sector a"
    p_yf0(oa,t)                           "Indicator for yf being observed"
    p_aSav(h,t)                           "Share parameter for saving equation"
    p_omegaSav(h)                         "Transformation Substitution between saving and consumption"
    p_aSavEntr(entr,t)                    "Share parameter for enterprise savings"
    p_omegaSavEntr(entr)                  "Elasticity of transformatin for enterprise savings"
    p_aFundG(t)                           "Share parameter for government funds"
    p_exptWgts(t)                         "Weights in forming expectations from future years"
*
*     --- relating to stock behavior
*
    p_alphaSt(i,t)                        "Targeted share of stock over production"
    p_rhoXDelSt(i)                        "Elasticity of stock changes in desired stock over output"
    p_rhoPDelSt(i)                        "Elasticity of stock changes to targeted stock over production"
*
    p_exolCovAreaChg(lnd,lCov,t)          "Exogenous change in unmanaged land in Mio ha"
    p_oldCapInvShare(a)                   "Share of old capital in activity specific investment demand"

    sam(is,is)                            "Final SAM, aggregated from base SAM"
    p_chiFac(is,fpnest,t)                 "income distribution parameter"
    p_debtGdpTgtUp(t)                     "Threshold above which gov consumption is dampended"
    p_debtGdpTgtLo(t)                     "Threshold below which gov consumption is expanded"
    p_relExpInc                           "Decline in expenditures relative to increase in revenues for fiscal resp"
    p_sigmaFint                           "Elasticity of foreign new public debt to interest rate"
    p_chiDebtPay(inst,t)                  "Share of creditor inst in total domestic debt payments"
    p_chiDebtNew(inst,t)                  "Share of creditor inst in total new domestic debt"
    years(tf)                             "Years, start from 2010"

*   --- Foreign capital account closure parameters

    p_grkMin(t)                     "Lower bound on investment growth"
    p_grkMax(t)                     "Upper bound on investment growth"
    p_grkTrend(t)                   "Long-term trend for investment growth"
    p_gammaSf(t)                    "efficiency param or shifter on foreign savings baseline value"
    p_gammaSf1(t)                   "efficiency param or shifter on foreign savings baseline value"
    p_gammaSf2(t)                   "efficiency param or shifter on foreign savings baseline value"
    p_retrow(t)                     "expected return rate in the row"
    p_ggdppcrow(t)                  "Expected per capita GDP growth rate in the row"
    p_omegaSavf(t)                  "Parameter indicating debt to gdp default threshhold"
    p_rhosavf(t)                    "Positive scaling parameter"
    p_emiConv(em,pollType)          "Conversion factors for different pollution indicators"
    p_pop0(cohorts)                 "Base year population"
    p_hpop0(h)                      "Base year population by HHs"
    p_ghgInvAlloc(ghgInv,em,*,aa)   "Allocation of emissions to inventories"
    lndTransMat(lnd,lCov,lCov)      "Land transmittion matrix (ha to ha), driving land type first"

    carbCnt(*,lcov)                 "Carbon contents in t per ha"
  ;

  scalar hasEmiT "Flag to check if emission taxes are present";
  scalar nMsg;
  set msg / msg1*msg99 /, amsg(msg);
  $$macro writeMessages(mymsg) loop(mymsg $ (myMsg.pos le nMsg), put_utility 'msg' / " ",mymsg.te(mymsg)); option clear=mymsg;

  set tfPPars / a,b,c /;
  parameter p_prodShift(tfpPars,a);

* ----------------------------------------------------------------------------
*
*  Macros and globals
*
* ----------------------------------------------------------------------------

  $$iftheni.scale "%scaleModel%"=="on"
*
*    --- Standard assumptions for scaling: no multiplier to variable level,
*        minimum scaling factor is 0.001
*
     $$setglobal scaleMult 1
     $$setglobal minScale  1.E-3
*
*    --- m_scale is the scaling macro: with scale one equations are scaled by the
*       indicated variables (usually the RHS). For negative variables, the
*       absolute value will be used. Values < minScale (currently 1.E-3) will
*       be set to 1.E-3
*
     $$macro     s(var,dims) /max(%minScale%,abs(var.l(&&dims&&))*%scaleMult%)
*
*    --- a few cases don't use a single variable, but expressions for scaling
*        this is handled by the global s1 (just empty for scaling)
*
     $$setglobal s1

  $$else.scale
*
*    --- without scaling, the m_scale macro is empty and the scaling expressions
*        will vanish from the equations (empty line)
*
     $$setglobal scaleMult eps
     $$setglobal minScale  1
     $$macro     s(var,dims)
*
*    --- a few cases don't use a single variable, but expressions for scaling
*        this is handled by the global s1 which turns the scaling lines
*        into comment lines with the global below
     $$setglobal s1 $$hidden

  $$endif.scale

* --- calculates cumulative damage from different channels by including only the non-zero variables

  $$macro m_dmgAgg(var,dims)  prod(chnl,1+var(&&dims&&,chnl,t) $ ((var.l(&&dims&&,chnl,t) ne 0) $ ifdmg))

* ----------------------------------------------------------------------------
*
*  Production equations
*
* ----------------------------------------------------------------------------


* --- (1) + (3) Factor demand (for intermediate inputs, factors, or CES sub-production functions = production nests)
*
*     The equation is introduced for active combinations of childs, activities and vintage and the years
*     in the current model instance. fpPar are the parent nests of the current child. Note that that one
*     child might be driven by multiple parents (energy demand, for instance, depend on overall demand
*     transport and process heat). It covers the standard CES and the volume preserving one.
*

  e_xfac(flagXFac(fpChl,aact(a),v),ts(t)) $ (sum(fpPar,p_alphaXfac(fpChl,fpPar,a,v)) $ actfPNest(fpChl)) ..

   xfac(fpChl,a,v,t)s(xfac,'fpChl,a,v,t-1')
   =e= [ sum(fpPar $ p_alphaXfac(fpChl,fpPar,a,v),
*
*           --- share parameters
*
            p_alphaXfac(fpChl,fpPar,a,v)

*           --- exogenous iceberg cost shifter, updates shares parameters
*                active in forward looking extension to capture changes in updates
*                of the share parameters from the vintage mechanism to recover baseline
*
*           --- another exogenous iceberg cost shifter, updates shares parameters with twist approach
*                 in a cost-neutral way
*
            * (1+twist(fpChl,fpPar,a,v,t) $ fpParTc(fpPar))
*
*           --- this the current demand quantity of activity a and vintage v for the parent nest
*               which drives the demands on the LHS
*
            * xfac(fpPar,a,v,t)
*
*           --- production shifter: first part for child nests, second for if parent is total output (= fptop)
*                                   Note that the impact depends on the functional form: the volume preserving
*                                   one (mostly used for energy carriers) is different from the standard CES
*
            * (lambdaXfac(fpChl,a,v,t) * (1 $ (not fpTop(fpPar)) + lambdaXfac(fpPar,a,v,t) $ fpTop(fpPar)))
                   **[ (p_sigmaXFac(fpPar,a,v)-1) $ (not isAdditiveCes(fpPar))
                      - p_sigmaXFac(fpPar,a,v)    $      isAdditiveCes(fpPar) ]
*
*           --- if substitution elasticities are identical (and thus the share parameters), use the idential composite price
*               for parent nest from the old vintage
*
            * [1+{([  (pxfac(fpPar,a,v,t)    $ (not fpIdent(fpPar,a)) + pxFac(fpPar,a,old,t)    $ fpIdent(fpPar,a)) $ (not isAdditiveCes(fpPar))
                    + (pxfacInd(fpPar,a,v,t) $ (not fpIdent(fpPar,a)) + pxFacInd(fpPar,a,old,t) $ fpIdent(fpPar,a)) $      isAdditiveCes(fpPar) ]
*
*             --- note that for primary factors and intermediates due to assumption on market mechanisms,
*                 the same price is used for the old and new vintage. See e_pap for intermediate prices.
*
              /[ pxfac(fpChl,a,v,t)   $ (not  fpIdent(fpChl,a))
               + pxfac(fpChl,a,old,t) $       fpIdent(fpChl,a)  ])**p_sigmaXFac(fpPar,a,v)-1} $ p_sigmaXFac(fpPar,a,v)]
         )

*
*         -- mark up demand (= non competitive part)
*
          + p_markUpNonComp(fpChl,a,t) * sum(fpTop,xfac(fptop,a,v,t))
        ]s(xfac,'fpChl,a,v,t-1');


* --- (2) + (4) Price of parent of a production nest (unit cost of nest branch). Defined for old, only, if parameters across vintages are identical
*     This is a dual price formulation

  e_pxfac(flagXFac(fpPar,aact(a),v),ts(t)) $ (((not fpIdent(fpPar,a)) or old(v)) $ actfPNest(fpPar)) ..

   [(pxfac(fpPar,a,v,t) $ (not isAdditiveCes(fpPar)) + pxFacInd(fpPar,a,v,t) $ isAdditiveCes(fpPar))
     * (1 $ (not fpTop(fpPar)) + lambdaXfac(fpPar,a,v,t) $ fpTop(fpPar))]s(pxfac,'fppar,a,v,t-1')
    =e=
*
*       --- First block for a non-zero substiution elasticity. Leontief case follows below.
*
        [{sum(fpChl $ p_alphaXfac(fpChl,fpPar,a,v),
*
*            --- share parameters, basic weights for childs in parent price index
*
             p_alphaXfac(fpchl,fpPar,a,v)


             *(1+twist(fpChl,fpPar,a,v,t) $ fpParTc(fpPar))
*
*            --- impact of productivity shifters for child. Note that the impact depends on the
*                functional form, the volume preserving for additive CES differs from the standard case
*
             * lambdaXfac(fpchl,a,v,t)**[  (p_sigmaXFac(fpPar,a,v)-1) $ (not isAdditiveCes(fpPar))
                                           -p_sigmaXFac(fpPar,a,v)    $      isAdditiveCes(fpPar)]
*
*            --- for some childs, there are differences between the old and new vintage, as indicated
*                by the indicator set fpIdent. For instance, after use tax prices for product
*                used as intermediate are identical for the old and new vintage. Equally, not again
*                that the substitution elasticity impact different the weight under the additive and
*                standard CES case.

             *([  pxfac(fpChl,a,v,t)   $  (not fpIdent(fpChl,a))
                + pxfac(fpChl,a,old,t) $       fpIdent(fpChl,a) ])**(1 $ (not isAdditiveCes(fpPar)) - p_sigmaXFac(fpPar,a,v))

           )**(1/(1 $ (not isAdditiveCes(fpPar)) - p_sigmaXFac(fpPar,a,v)))

          } $ p_sigmaXFac(fpPar,a,v)
*
*       --- this second block is introduced for computational efficiency, only. It is identical to the above
*           for the of a zero substitution elasticity = Leontief
*

        + sum(fpChl $ p_alphaXfac(fpchl,fpPar,a,v),p_alphaXfac(fpchl,fpPar,a,v)

*
                  *(1+twist(fpChl,fpPar,a,v,t) $ fpParTc(fpPar))

                  *([  pxfac(fpChl,a,v,t)   $  (not fpIdent(fpChl,a))
                     + pxfac(fpChl,a,old,t) $       fpIdent(fpChl,a) ]
                     /lambdaXfac(fpchl,a,v,t))
          ) $ (not p_sigmaXFac(fpPar,a,v))

        ]s(pxfac,'fppar,a,v,t-1');

* --- (5) Exhaustion price, used in case of additive (or volume preseving) CES (different from dual price index above)

  e_pxfacInd(flagXFac(fpPar,aact(a),v),ts(t)) $ ([(not fpIdent(fpPar,a)) or old(v)] $ isAdditiveCES(fpPar)  $ actFpNest(fpPar))   ..

   pxfac(fpPar,a,v,t)s(pxfac,'fppar,a,v,t-1')
   =e= [sum(fpChl $ p_alphaXfac(fpChl,fpPar,a,v),

            p_alphaXfac(fpChl,fpPar,a,v)


            * (1+twist(fpChl,fpPar,a,v,t) $ fpParTc(fpPar))

            * [   pxfac(fpChl,a,v,t)   $ (not fpIdent(fpChl,a))
                + pxfac(fpChl,a,old,t) $      fpIdent(fpChl,a)]**(1-p_sigmaXFac(fpPar,a,v))

            * lambdaXfac(fpchl,a,v,t)**(-p_sigmaXFac(fpPar,a,v))) * pxFacInd(fpPar,a,v,t)**p_sigmaXFac(fpPar,a,v)
       ]s(pxfac,'fppar,a,v,t-1');

* --- (6) Armington demand for intermediates i by an activity a equals its factor demand, added up over vintages v

  e_xap(i,a,ts(t)) $ sam(i,a) ..
   xa(i,a,t)s(xa,'i,a,t-1') =e= sum(flagXFac(fpNestI,a,v) $ sameas(i,fpNestI),xfac(fpNestI,a,v,t))s (xa,'i,a,t-1');

* --- (7) Armington price of intermediates (computed for old vintage only, identical by model structure to new one)
*     This also reflects the linear aggregation in e_xap above

  e_pap(flagXfac(fpNestI,a,old),ts(t)) ..
   pxfac(fpNestI,a,old,t)s(pxfac,'fpNestI,a,old,t-1')
    =e=  sum(sameas(fpNestI,i),p_chiPA(i)*paf(i,a,t))s(pxfac,'fpNestI,a,old,t-1');

* --- (8) CES cost share twists
*
*    --- Twists are cost-neutral updates of share parameters. The set fpParTc depicts the parent nests where
*        such twist are active. The childs under such a parent nests are split in two: one where the share parameter
*        exogenously changed (or driven by another equation), and the one where share paramters are updates
*        with endogenous variable twist, defined here
*
   e_twist(fpchl,fpParTC,a,v,ts(t)) $ (flagXfac(fpchl,a,v) $ p_alphaXfac(fpChl,fpParTc,a,v)
                                        $ ((chitwFA.range(fpParTC,a,t) ne 0) OR (chitwF.range(fpParTc,t) ne 0))
                                        $ p_tw(fpParTc,a,v,t)) ..
      twist(fpChl,fpParTC,a,v,t)
*           --- this will remove the impact of chitwFA for the non pfChlTC subNests
      =e=  [     (1 + sum(fpChlTC(fpChl,a), chitwFA(fpParTC,a,t)+chitwF(fpParTC,t)))
*           --- this term is always the same for all childs
              / (1 + (chitwFA(fpParTC,a,t)+chitwF(fpParTC,t))
                   * sum(fpChlTC(fpChlTwist,a) $  p_alphaXfac(fpChlTwist,fpParTc,a,old),shrxfac.l(fpChlTwist,fpParTC,a,old,t-1)))
             * (1+twist.l(fpChl,fpParTC,a,v,t-1)) -1
           ]
*          --- this allows to apply only a part of chitwFA to specific activities and vintages
           * p_tw(fpParTc,a,v,t);

*    --- (9) Define share parameter for nests with share twist, used in t+1 in e_twist

   e_shrxFac(fpChlTwist,flagxFac(fpParTC,aact(a),old),ts(t)) $ (p_alphaXfac(fpChlTwist,fpParTc,a,old)
                                                                  $ fpChlTC(fpChlTwist,a) $ p_tw(fpParTc,a,old,t)) ..

*      --- note that for all nests with at the bottom of tree where substitution elasticities and share parameters
*          are identical, only the (composite) price for the old vinate is used

      shrxFac(fpChlTwist,fpParTc,a,old,t) =E= sum(vp $ flagXFac(fpChlTwist,a,vp), xfac(fpChlTwist,a,vp,t)
                                                        * [ pxfac(fpChlTwist,a,vp,t)   $ (not  fpIdent(fpChlTwist,a))
                                                          + pxfac(fpChlTwist,a,old,t)  $       fpIdent(fpChlTwist,a) ])
                                              /sum(vp $ flagXFac(fpChlTwist,a,vp),xfac(fpParTC,a,vp,t)
                                                        * [ pxfac(fpParTc,a,vp,t)   $ (not  fpIdent(fpParTc,a))
                                                          + pxfac(fpParTc,a,old,t)  $ (     fpIdent(fpParTc,a))]);
*
* --- (10) Productivity factor shifter equation.
*     A .scale=999 allows to swtich the equation off and drive lambdaXfax by a closure swap

  e_lambdaXfac(flagxFac(actFpNest,a,v),ts(t)) $ (  (lambdaXfac.range(actFpNest,a,v,t) ne 0)
                                                 $ (lambdaXfac.scale(actFpNest,a,v,t) ne 999)) ..

   lambdaXfac(actFpNest,a,v,t)s(lambdaXfac,'actFpNest,a,v,t-1')

    =e= [ chixFacF(actFpNest,t) * chixFacA(a,t) * chixFacV(v,t)
                * chixFacFA(actFpNest,a,t)
*               --- air pollution impact on labor productivity
                * (1 + dmgAir(actFpNest,a,t) $ ifAirPollDmg $ l(actFpNest))
*               --- shifter for all labor types
                * (1 + (chixFacL(t)) $ l(actFpNest))
*               --- climate damages
                *  m_dmgAgg(dmgPrd,'actFpNest,a')
*               --- shifter for capital
                * (1 + chixFacK(t)   $  cap(actFpNest))
*               --- shifter for capital and labor
                * (1 + chixFacKL(t)  $ (cap(actFpNest) or l(actFpNest)))
*               --- shifter for primary factors (land,labor,capital etc.)
                * (1 + chixFacFP(t)  $ fp(actFpNest))
*               --- shifter for sector groups
                * prod(mapAagr(aagr,a),chixFacAgr(aagr,actFpNest,t))
*               --- shifter for energy carriers
                * (1 + chixFacNRG(t) $ nrgFac(actFpNest)) ]s(lambdaXfac,'actFpNest,a,v,t-1');

* --- (11) Aggregate unit cost of production (exhaustion) #ENDOTECH
*     This is unit cost = output times output cost price, augmented by potential mark ups

  e_px(aact(a),ts(t)) ..
   px(a,t)*xp(a,t)
    %s1% /max(%minScale%,xp.l(a,t-1)*px.l(a,t-1)*%scaleMult%)
    =e=[   sum(flagxFac(fptop,a,v), xfac(fptop,a,v,t)*(1+p_markup(a,t)$ifEnTech)*pxfac(fptop,a,v,t))
         + sum(flagxFac(actFp,a,v), p_markUpNonComp(actFp,a,t)*xfac(fptop,a,v,t)*pxfac(actFp,a,old,t))]
    %s1% /max(%minScale%,xp.l(a,t-1)*px.l(a,t-1)*%scaleMult%)
  ;

* --- (12) Unit cost prices of activities augmented by ad-valorem production taxes / subsidies
*     and emission taxes linked to output driven emissions

  e_pp(aact(a),ts(t))  ..
   pp(a,t)s(pp,'a,t-1')
    =e= [  px(a,t)*(1 + tPrd(a,t) + tpsb(a,t) + tphn(a,t))
          + sum(emiFlag(em,a) $ p_rhoEmiXP(em,a),tEmi(em,a,t)*emical(t)*p_phiEmiXP(em,a,t)*chiEmiXP(em,a,t)*p_rhoEmiXP(em,a))]s(pp,'a,t-1');

* --- (13) Factor yield (output/factor use)

  e_yield(actFp,a,ts(t)) $ (sum(v,flagxFac(actFp,a,v)) $ (yield.range(actFp,a,t) eq 0)) ..

   xp(a,t)s(xp,'a,t-1') =e= m_dmgAgg(dmgYld,'actFp,a') * yield(actFp,a,t) * sum(flagXFac(actFp,a,v),xfac(actFp,a,v,t))s(xp,'a,t-1');

* --- (13) Average output per unit of all types of land (= usual yield definition in agriculture)
*     The xs(i) summation is used to capture the case where physical production is used

  e_yieldLnd(a,ts(t)) $ sum((actHa,v),flagxFac(actHa,a,v)) ..

   sum(i $ p_as(a,i,t),xs(i,t))s(xp,'a,t-1') =e= yieldLnd(a,t) * sum(flagXFac(actha,a,v),xfac(actha,a,v,t))s(xp,'a,t-1');

* --- (14) Differentiated labor productivity for first, second and tertiary sector production activities
*     This equation is used in baseline mode, only. Yearly changes in real GDP determine the
*     the change in labor productivity

  e_diffLabProd(lFpNest(fpNest,a),ts(t)) $ (sum(flagXFac(fpNest,a,v),1) $ ifLabProdDiff) ..

  chixFacFA(fpNest,a,t)
    =E= -ncpVuPow[-labProd(t),-labProd(t)*{   p_prodShift("a",a)
                                            + p_prodShift("b",a) *    [rgdpmp(t)/rgdpmp.l(t-1) -1]
                                            + p_prodShift("c",a) * sqr[rgdpmp(t)/rgdpmp.l(t-1) -1] }] $ (p_prodShift("a",a)+ p_prodShift("b",a)+ p_prodShift("c",a))
       + chixFacFA.l(fpNest,a,t-1);

* --- (15) land productivity linked with a factor chosen on GUI to labor productivity
*     This equation is used in baseline mode, only

  e_landProd(actLnd,ts(t)) $ ifLandProdAsLabor ..

   chixFacF(actLnd,t)
    =e= 1 - %LandProdAsLabor%*ncpVuPow[0,-((labProd(t)  + labProd.l(t-1)) $ ifLabProdDiff
                                       + ChixFacL(t) $ (not ifLabProdDiff))];
* --- (16) Natural ressource productivity, linked with a factor chosen on GUI to labor productivity

*     This equation is used in baseline mode, only

  e_natResProd(natres,ts(t)) $ ifNatresProdAsLabor ..

   chixFacF(natRes,t)
    =E= 1 - %NatresProdAsLabor%*ncpVuPow[0,-((labProd(t)  + labProd.l(t-1)) $ ifLabProdDiff
                                       + ChixFacL(t) $ (not ifLabProdDiff))];

* --- (17) Capital productivity linked with a factor chosen on GUI to labor productivity
*     This equation is used in baseline mode, only

  e_capProd(cap,ts(t)) $ ifCapProdAsLabor ..

   chixFacF(cap,t)
    =e= 1 - %capProdAsLabor%*ncpVuPow[0,-((labProd(t)  + labProd.l(t-1)) $ ifLabProdDiff
                                       + ChixFacL(t) $ (not ifLabProdDiff))];

* --- (18) Joint-production equation (activity a a producing multiple outputs i), CET, determines output shares of products i
*     Two cases: (1) in the sluggish (or leontief) case, output x of product i by activity a is a share of total ouput xp
*                    and the relation between its price p and the activity ouput price pp (defined in e_pp above)
*                (2) infinite case: the price of x for all i must be identical to pp (defined in e_pp above)

  e_x(a,i,ts(t)) $ P_ap(a,i,t) ..
   0 =e= ( [x(a,i,t) - p_ap(a,i,t)*xp(a,t)*(chiP(a,i,t)*p(a,i,t)/pp(a,t))**p_omegaP(a)]s(x,'a,i,t-1')) $ (p_omegaP(a) ne inf)
       + ( [p(a,i,t)/p.l(a,i,t0) - pp(a,t)]s(p,'a,i,t-1')                                            ) $ (p_omegaP(a) eq inf);

* --- (19) Supply equilibrium condition (exhaustion of sales revenues, defines price for new vintage)

  e_xp(aact(a),ts(t)) ..

   0 =E=    [ (pp(a,t)
              - sum(i $ p_ap(a,i,t), p_ap(a,i,t) * (chiP(a,i,t)*p(a,i,t))**(1+p_omegaP(a))
                    )**(1/(1+p_omegaP(a))))s(pp,'a,t-1')                                   ] $ (p_omegaP(a) ne inf)
         +  [ (xp(a,t) - sum(i $ p_ap(a,i,t), x(a,i,t)*chiP(a,i,t)))s(pp,'a,t-1')          ] $ (p_omegaP(a) eq inf);

* --- (20)+(22) Determination of demand of product i from activity a in case that multiple activities determine supply quantity xs
*     for product i

  e_p(a,i,ts(t)) $ (p_as(a,i,t) ne 0)..
   0 =e= ([x(a,i,t) - p_as(a,i,t)*xs(i,t)
            * p_gammaP(a,i,t)**[ (p_sigmaS(i)-1) $ (not addCest(i)) -p_sigmaS(i) $ addCest(i)]
            *( (pInd(i,t)*p_chiPs(i) $ addCest(i) + ps(i,t)*p_chiPs(i) $ (not addCest(i)))
              /(p(a,i,t)*chiP(a,i,t))                                                    )**p_sigmaS(i)]s(x,'a,i,t-1')
         ) $ (p_sigmaS(i) ne inf)

      +  ([p(a,i,t)*chiP(a,i,t) - p_gammaP(a,i,t)*ps(i,t)*p_chiPs(i)]
            %s1% /max(%minScale%,chip.l(a,i,t-1)*p.l(a,i,t-1)*%scaleMult%)
         ) $ (p_sigmaS(i) eq inf) ;

* --- (21)+(23) Dual price index for supply price ps for additive and non-additive case  (ACES or standard)

  e_xs(i,ts(t)) $ iact(i) ..

   0 =e=     [ {ps(i,t)*p_chiPs(i) $ (not addCest(i)) + pInd(i,t)*p_chiPs(i) $ addCest(i)
                            - sum(a $ p_as(a,i,t),p_as(a,i,t)
                                    * p_gammaP(a,i,t)**[-p_sigmaS(i) $ addCest(i) + (p_sigmaS(i)-1) $ (not addCest(i))]
                                    * (p(a,i,t)*chiP(a,i,t))**[1 $ (not addCest(i)) -p_sigmaS(i)]
                              )**(1/(1 $ (not addCest(i)) -p_sigmaS(i)))
                 }s(ps,'i,t-1') ] $ (p_sigmaS(i) ne inf)
*
*             xs is a linear aggregate over x from multiple activities under inifinite subsitution
*
            + [(xs(i,t) - sum(a $ p_as(a,i,t), x(a,i,t)))s(xs,'i,t-1')] $(p_sigmaS(i) eq inf);

* --- (24) Exhaustion pices for ps in case of additive CES

  e_pInd(iact(i),ts(t)) $ ( (p_sigmaS(i) ne inf) and addCest(i))..

   ps(i,t)s(ps,'i,t-1')
    =E= sum[a $ p_as(a,i,t), p_as(a,i,t)*p(a,i,t)**(1-p_sigmaS(i))/p_gammaP(a,i,t)**p_sigmaS(i)]
      * pInd(i,t)**p_sigmaS(i)s(ps,'i,t-1');


* --- (25) Power mix targeting (shares of electricity activities might be fixed) #ENDOTECH

  e_elecMix(aElec,iElec,ts(t)) $ p_ap(aElec,iElec,t) ..
   elecMix(Aelec,iElec,t) =e= x(aelec,iElec,t)/sum(aelecp $ p_ap(aElecp,iElec,t),x(aelecp,iElec,t));

* ----------------------------------------------------------------------------
*
*  Income distribution equations
*
* ----------------------------------------------------------------------------

* --- (26) Total factor income by factor = pretax prices times factor demand, summed over activities and vintages

  e_facty(actFp,ts(t)) ..
   facty(actFp,t)s(facty,'actFp,t-1')
    =E= sum(flagXFac(actFp,a,v), pxfac_preTax(actFp,a,old,t)*xfac(actFp,a,v,t))s(facty,'actFp,t-1');

* --- (27) Gross capital income (current model set-up requires cap to be singleton)

  e_kapy(ts(t)) ..
   kapy(t)s(kapy,'t-1') =e=  sum(cap,facty(cap,t))s(kapy,'t-1');

* --- (28) Gross household income: share of factor incomes, transfers received, income from past lending

  e_yh(h,ts(t)) ..
   yh(h,t)s(yh,'h,t-1')
    =e= [
*        --- supply xfmSup by household h of factor actmp, mutiplied with price and an economy-wide correction factor
*            which is corrected with factor corrFac to ensure exhaustion of total factor income for each factor
             sum((sameas(actFp,actfmp),inst,vf) $ (mapInst(h,inst) $ xfmsup.l(actFmp,actFmp,inst,vf,t0) ),
                 xfmSupUpdateFac(actFmp,inst,t)*xfmSup(actFmp,actFmp,inst,vf,t)*pfmSup(actFmp,actFmp,inst,vf,t)*corrFac(actFp,t))

*        --- share p_chiFac of economy-wide factor incomes
         +   sum((actFp,mapInst(h,inst)), facty(actFp,t)*p_chiFac(inst,actFp,t))

*        --- transfers to households from institutions inst (gov, row, other households, enterprises)
         +   sum((mapinst(h,instp),inst), transfers(instp, inst, t))

*        --- interest and principal received
         +   sum(mapinst(h,inst),debtPay(inst,t))]s(yh,'h,t-1');


* --- (29) Household disposible income (after income taxes, minus transfers to other institutions, detmerined e_hhtr above)

  e_yhDsp(h,ts(t)) ..
   yhDsp(h,t)s(yhDsp,'h,t-1') =e= [ yh(h,t) *(1-tPIT(h,t) ) - sum(mapinst(h,inst), totTr(inst,t))]s(yhDsp,'h,t-1');

* --- (30) Household savings: additive CET between consumption value and savings,
*     share of disposable household yhDsp

  e_savh(h,ts(t)) ..
   savH(h,t)s(savH,'h,t-1')
    =e= [   (chims(t) $ (p_aSav(h,t) gt 0) + (1/chims(t)) $ (p_aSav(h,t) le 0)) * yhDsp(h,t) * p_aSav(h,t)
          * (fundPrice(t)*pgdpmp(t)/pindH(h,t))**(sign(p_aSav(h,t))*p_omegaSav(h))
        ]s(savH,'h,t-1');

* --- (31) Composite revenue price of returns to savings and consumption
*     Drives household savings jointly with fundprice

  e_pindH(h,ts(t)) ..
   pindH(h,t)s(pindh,'h,t-1')
    =e= [      p_aSav(h,t) * (chims(t) $ (p_aSav(h,t) gt 0) + (1/chims(t)) $ (p_aSav(h,t) le 0))
            * (fundPrice(t)*pgdpmp(t))**(sign(p_aSav(h,t))*p_omegaSav(h))

           + (1-p_aSav(h,t) * (chims(t) $ (p_aSav(h,t) gt 0) + (1/chims(t)) $ (p_aSav(h,t) le 0)))
             * pf(h,t)                ** (sign(p_aSav(h,t))*p_omegaSav(h))

        ]**(1/(sign(p_aSav(h,t))*p_omegaSav(h)))s(pindh,'h,t-1');


* --- (32) Average propensity to save: household savings over disposable income

  e_aps(h,ts(t)) ..
   aps(h,t) =e= savh(h,t)/yhDsp(h,t);

* --- (33) Total household transfer expenditure, share chiTotTr of household income before direct taxes

  e_hhtr(inst,ts(t)) $ sum(h,mapinst(h,inst)) ..
   totTr(inst,t)s(totTr,'inst,t-1') =e= sum(mapInst(h,inst),chiTotTr(inst,t)*yh(h,t))s(totTr,'inst,t-1');

* --- (34) Household expenditures on goods and services:
*     disposible income minus saving

  e_yfh(h,ts(t)) ..
   yf(h,t)s(yf,'h,t-1') =e= [yhDsp(h,t) - savh(h,t)]s(yf,'h,t-1');

* --- (35) Gini coefficient (HD: post-triangular: move to reporting)

  e_gini(ts(t)) $ (card(h) gt 1) ..
   gini(t) =e= 2 * sum(h,ord(h) * yh(h,t))/(card(h)*sum(h,yh(h,t))) - (card(h)+1)/card(h);

* --- (36) Dependency ratio: population in cohort, in relation to total population in working age
*                       (15-64 years old)

  e_deprate(cohorts,ts(t)) ..
   deprate(cohorts,t) =e= pop(cohorts,t)/pop("P1564",t);

* ----------------------------------------------------------------------------
*
*  Government revenue block
*
* ----------------------------------------------------------------------------

* --- Tax definitions
*     taxName0 variables are initial tax rates, such as tFac0
*     taxNameDX are different additive shifters, relating to dimensions of the tax (such as a and actFp in tFac)
*     taxNameMX are different multiplicative shifters, relating to dimensions of the tax
*     Other shifters are explained within the equation as needed

* --- (37) Total revenues generated by natural resource taxes

  e_ygTotFacNatRes("FacNatres",ts(t)) ..
   yg("FacNatres",t)s(yg,'"FacNatRes",t-1')
     =e= sum(flagXFac(natres,a,v), tFac(a,natres,t)* pxfac_preTax(natres,a,old,t)*xfac(natres,a,v,t))s(yg,'"FacNatRes",t-1');

* --- (37) Total revenues generated by labor taxes

  e_ygTotFacLab("Faclab",ts(t)) ..
   yg("FacLab",t)s(yg,'"FacLab",t-1')
    =e= sum(flagXFac(l,a,v),   tFac(a,l,t)  * pxfac_preTax(l,a,old,t)*xfac(l,a,v,t))s(yg,'"FacLab",t-1');

* --- (37) Total revenues generated by capital taxes

  e_ygTotFacCap("FacCap",ts(t)) ..
   yg("FacCap",t)s(yg,'"FacCap",t-1')
    =e= sum(flagXFac(cap,a,v), tFac(a,cap,t) * pxfac_preTax(cap,a,old,t)*xfac(cap,a,v,t))s(yg,'"FacCap",t-1');

* --- (37) Total revenues generated by land taxes

  e_ygTotFacLand("FacLand",ts(t)) ..
   yg("FacLand",t)s(yg,'"Facland",t-1')
    =e= sum(flagXFac(actLnd,a,v), tFac(a,actLnd,t)* pxfac_preTax(actLnd,a,old,t)*xfac(actLnd,a,v,t))s(yg,'"Facland",t-1');

* --- (38) Total government expenditure related to production subsidies

  e_ygTotpsb("psb",ts(t)) ..
   yg("psb",t)s(yg,'"psb",t-1') =e= sum(aact(a),tpsb(a,t)*px(a,t)*xp(a,t))s(yg,'"psb",t-1');

* --- (39) Total revenues generated by production taxes

  e_ygTotPrd("prd",ts(t)) ..
   yg("Prd",t)s(yg,'"prd",t-1') =e= sum(aact(a),tPrd(a,t)*px(a,t)*xp(a,t))s(yg,'"prd",t-1');

* --- (40) Total revenues generated by phantom taxes (should always be zero)

  e_ygTotPhn("phn",ts(t)) ..
   yg("Phn",t)s(yg,'"Phn",t-1') =e= sum(aact(a),tphn(a,t)*px(a,t)*xp(a,t))s(yg,'"Phn",t-1');

* --- (41) Total revenues generated by VAT on consumption of domestic goods

  e_ygTotVatDom("VatDom",ts(t)) ..
   yg("VatDom",t)s(yg,'"VatDom",t-1')
    =e= sum(iDom(i),(p_chiPD(i)*pd(i,t) + pmarg(i,t)*tmg(i,"d",t))*tVat(i,"d",t)*xd(i,t))s(yg,'"VatDom",t-1');

* --- (42) Total revenues generated by VAT on consumption of imported goods by commodity

  e_ygTotVatImp("Vatimp",ts(t)) ..
   yg("VatImp",t)s(yg,'"VatImp",t-1')
    =e= sum(iImp(i),(er(t)*pwm(i,t)*(1+tImp(i,t)) + pmarg(i,t)*tmg(i,"m",t))*tVat(i,"m",t)*xm(i,t))s(yg,'"VatImp",t-1');

* --- (43) Total revenues generated by export taxes

  e_ygTotExp("exp",ts(t)) ..
   yg("Exp",t)s(yg,'"Exp",t-1') =e= sum(iExp(i), xe(i,t)*p_chiPE(i)*pe(i,t)*tExp(i,t))s(yg,'"Exp",t-1');

* --- (44) Total revenues generated by import taxes

  e_ygTotImp("imp",ts(t)) ..
   yg("Imp",t)s(yg,'"Imp",t-1') =e= sum(iImp(i),(xm(i,t) + mdelst(i,t))*pwm(i,t)*er(t)*tImp(i,t))s(yg,'"Imp",t-1');

* --- (45) Revenues generated by agent specific sales taxes, by commodity

  e_ygPaf(iArm(i),ts(t)) ..
   ygPaf(i,t)s(ygPaf,'i,t-1') =e= sum(aaIn(aa) $ sam(i,aa),tPaf(i,aa,t)*p_chiPA(i)*pa(i,t)*xa(i,aa,t))s(ygPaf,'i,t-1');

* --- (45) Total revenues generated by agent specific sales taxes

  e_ygTotPaf("paf",ts(t)) ..
   yg("Paf",t)s(yg,'"Paf",t-1') =e= sum(iArm(i),ygPaf(i,t))s(yg,'"Paf",t-1');

*
* --- (46) Revenues generated by taxes on product use by agents which contributes to emissions,
*     such as combustion of fossil fuels, use of products emitting f-gases etc.
*     haEmit is a indicator which is 0 if emissions are not taxes
*     p_phiEmi is the share of taxed emissions

  e_ygEmi(aa,ts(t)) $ hasEmit ..
   ygEmi(aa,t)s(ygEmi,'aa,t-1')
    =e= sum((em,emSrc) $ (emiFlag(em,aa) $ p_rhoEmi(em,emSrc,aa)),
                                tEmi(em,aa,t)*emical(t)*p_phiEmi(em,emSrc,aa)*chiEmi(em,emSrc,aa,t)*p_rhoEmi(em,emSrc,aa)*xa(emSrc,aa,t))s(ygEmi,'aa,t-1');

* --- (46) Total revenues generated by taxes on emissions from product use

  e_ygTotEmi("emi",ts(t))  ..
   yg("Emi",t)s(yg,'"Emi",t-1') =e= sum(aa $ hasEmit,ygEmi(aa,t))s(yg,'"Emi",t-1');

* --- (47) Revenues generated by taxes on emissions related to output and factors use by firms
*     Usually called "process emissions"
*     p_phiEmi is the share of taxed emissions

  e_ygEmiXP(a,ts(t)) $ hasEmit ..
   ygEmiXP(a,t)s(ygEmiXp,'a,t-1')
    =e= [sum(emiFlag(em,a) $ p_rhoEmiXP(em,a),       tEmi(em,a,t)*emical(t)*p_phiEmiXP(em,a,t)     *chiEmiXP(em,a,t)       *p_rhoEmiXP(em,a)*xp(a,t))
        +sum((em,actFp)    $ p_rhoEmiFac(em,actFp,a),tEmi(em,a,t)*emical(t)*p_phiEmiFac(em,actFp,a)*chiEmiFac(em,actFp,a,t)*p_rhoEmiFac(em,actFp,a)
                                                  *sum(flagxFac(actFp,a,v), xfac(actFp,a,v,t)))]s(ygEmiXp,'a,t-1');

* --- (47) Total revenues generated by taxes on process emissions (output and factor use driven)

  e_ygTotEmiXP("Emixp",ts(t))  ..
   yg("EmiXP",t)s(yg,'"EmiXP",t-1') =e= sum(a $ hasEmit,ygEmiXP(a,t))s(yg,'"EmiXP",t-1');

* --- (48) Revenues generated by personal income taxes paid by each household

  e_ygPit(h,ts(t)) ..
   ygPit(h,t)s(ygPit,'h,t-1') =e= tPit(h,t) * yh(h,t)s(ygPit,'h,t-1');

* --- (48) Total revenues generated by personal income taxes (summation over households)

  e_ygTotPit("pit",ts(t)) ..
   yg("Pit",t)s(yg,'"Pit",t-1') =e= sum(h,ygPit(h,t))s(yg,'"Pit",t-1');

* --- (48) Revenues generated by corporate income taxes paid by each type of enterprise

  e_ygCit(entr,ts(t)) ..
   ygCit(entr,t)s(ygCit,'entr,t-1') =e= tCit(entr,t) * entrY(entr,t)s(ygCit,'entr,t-1');

* --- (48) Total revenues generated by corporate income taxes (summation over enterpricss)

  e_ygTotCit("cit",ts(t)) ..
   yg("Cit",t)s(yg,'"Cit",t-1') =e= sum(entr,ygCit(entr,t))s(yg,'"Cit",t-1');

* --- (49) Government revenues

  e_ygov(ts(t)) ..
   ygov(t)s(yGov,'t-1')
    =e= [
*           --- tax revenue and expenditures (from subsidies) flows
            sum(g, yg(g,t))
*           --- transfers from other agents to the government
          + sum((mapinst(gov,instp),inst), transfers(instp, inst, t))
*           --- factor income
          + sum((sameas(actFp,actfmp),inst,vf) $ (mapInst(gov,inst) $ xfmsup.l(actFmp,actFmp,inst,vf,t0)),
                      xfmSupUpdateFac(actFmp,inst,t)*xfmSup(actFmp,actFmp,inst,vf,t)*pfmSup(actFmp,actFmp,inst,vf,t)*corrFac(actFp,t))
          + sum((actFp,mapInst(gov,inst)), facty(actFp,t)*p_chiFac(inst,actFp,t))
        ]s(yGov,'t-1');

* --- (50) Total revenues generated by all taxes

  e_ygTot(t) $ (ts(t)) ..
   ygTot(t)s(ygTot,'t-1') =e= sum(g,yg(g,t))s(ygTot,'t-1');

* --- (51) Government direct taxes

  e_gtax(ts(t)) ..
   gtax(t)s(gTax,'t-1') =e= dgovTax(t)*ygov(t)s(gTax,'t-1');

* --- (52) Total government transfers to other insitutions, share of nominal GDP
*     updated with a factor if the fiscal responsability mechanism is active)

  e_govtr(inst,ts(t)) $ sum(mapinst(gov,inst),1) ..
   totTr(inst,t)s(totTr,'inst,t-1')
     =e= chiTotTr(inst,t)*(1 + p_relExpInc/100*fiscalRespDrv(t) $ p_relExpInc $ ifFiscalResp)*gdpmp(t)s(totTr,'inst,t-1');

* --- (53) Public Primary Balance: goss deficit plus interest and principal

  e_primaryBalance(ts(t)) ..
   primaryBalance(t)s(primaryBalance,'t-1')
    =e= [
*         --- government income (tax revenues, factor income, transfers)
          ygov(t)
*         --- minus taxes paid by the government

        - gtax(t)
*         --- minus transfer from government to other institutions
        - sum(mapinst(gov,inst), totTr(inst,t))
*         --- minus taxes redistributed
        - yTaxRedist(t)
*         --- minus government consumption
        - yf(gov,t)
*         --- minus government investment, contributing to capital stock
        - sum(ginv $ p_yf0(ginv,t),yf(ginv,t))
*         --- minus government adaptive investments, non-productice
        - sum(map_adpInv(gov,adpinv),yf(adpinv,t))
        ]s(primaryBalance,'t-1');

* --- (54) Public Fiscal Balance: Primary balance minus interest payments

  e_fiscalBalance(ts(t)) ..
   fiscalBalance(t)s(fiscalBalance,'t-1')
     =e= [primaryBalance(t) - sum(instfd,debtInterest(instfd,t))]s(fiscalBalance,'t-1');

* --- (55) Gross government balance: fiscal revenues - all expenditures inclusive interest and principal
*                               = equal the amount of new borrowing if negative
*                               = equal the amount government saving contributing to private investments
  e_govBalance(ts(t)) ..
   govBalance(t)s(govBalance,'t-1')
    =e= [fiscalBalance(t) - sum(instfd,debtPrincipal(instfd,t))]s(govBalance,'t-1');

* --- (56) Real government deficit, nomimal balance normalized with price index of GDP at market prices

  e_rgovBalance(ts(t)) ..
   rgovBalance(t)*pgdpmp(t)s(govBalance,'t-1') =e= govBalance(t)s(govBalance,'t-1');

* ----------------------------------------------------------------------------
*
*  Enterprice block
*
* ----------------------------------------------------------------------------

* --- (57) Enterprise income

  e_entrY(entr,ts(t)) ..
   entrY(entr,t)s(entry,'entr,t-1')
    =e= [
*             --- factor income
              sum((sameas(actFp,actfmp),inst,vf) $ (mapInst(entr,inst) $ xfmsup.l(actFmp,actFmp,inst,vf,t0)),
                     xfmSupUpdateFac(actFmp,inst,t)*xfmSup(actFmp,actFmp,inst,vf,t)*pfmSup(actFmp,actFmp,inst,vf,t)*corrFac(actFp,t))
           +  sum((actFp,mapInst(entr,inst)), factY(actFp,t)*p_chiFac(inst,actFp,t))
*             --- transfers received from other institution
           +  sum((mapinst(entr,instp),inst), transfers(instp, inst, t))
*             --- interest and principal received from domestic gov debt
           +  sum(mapinst(entr,inst),debtPay(inst,t))
*             --- markups (endotech extension)
           +  sum((fptop,v,a)$(aact(a)), p_markup(a,t)*xfac(fptop,a,v,t)*pxfac(fptop,a,v,t))
*             --- spendings on RD, financed from markups (must not balance perfectly with markup income)
           -  sum(f $ p_chiFdRD(f,t), xfdRD(f,t)*pf(f,t))
               ]s(entry,'entr,t-1');

* --- (58) Enterprise saving, driven by CET

  e_savEntr(entr,ts(t)) $ sum(mapinst(entr,inst),1) ..
   savEntr(entr,t)s(savEntr,'entr,t-1')
    =e= chims(t) * entrY(entr,t) * (1- tCit(entr,t)) * p_aSavEntr(entr,t) * fundPrice(t)**p_omegaSavEntr(entr)s(savEntr,'entr,t-1');

* --- (59) Total enterprise transfers, difference between after tax revenues
*     and savings

  e_entrtr(inst,ts(t)) $ sum(mapinst(entr,inst),1) ..
    totTr(inst,t)s(totTR,'inst,t-1')
    =e= [sum(mapinst(entr,inst),entrY(entr,t) * (1- tCit(entr,t)) - savEntr(entr,t))]s(totTR,'inst,t-1');

* ----------------------------------------------------------------------------
*
*  Transfers
*
* ----------------------------------------------------------------------------

* --- (60) Allocation of domestic transfers for the different agents: total transfer expenditures
*     are distributed with shares chiTr to the different institutions. Additional mechanism
*     to redistribute some tax income yTaxRedist (defined in equations not in template)
*     with shares chiTaxRedist

  e_dtransfers(inst,dinst,ts(t)) $ sam(inst,dinst) ..
   transfers(inst,dinst,t)s(transfers,'inst,dinst,t-1')
    =e= [chiTr(inst, dinst,t) * totTr(dinst,t) + chiTaxRedist(inst,t) * yTaxRedist(t) $ sum(mapInst(gov,dinst),1)]
        s(transfers,'inst,dinst,t-1');

* --- (61) Transfers from ROW to institutions inst

  e_rowtransfers(inst,instp,t) $ (ts(t) and sum(mapinst(row,instp), (sam(inst,instp) ne 0))) ..
   sum(mapinst(row,instp),transfers(inst,instp,t))s(rowtransfers,'inst,t-1')
     =e= er(t)*rowtransfers(inst,t)s(rowtransfers,'inst,t-1');

* ----------------------------------------------------------------------------
*
*  Saving-Investment block
*
* ----------------------------------------------------------------------------

* --- (62) Total domestic funds available from households and enterprises to finance
*     investments or new government debt
  e_fundT(ts(t)) ..
   fundT(t)s(fundT,'t-1') =e= [sum(h,savH(h,t))+ sum(entr,savEntr(entr,t))]s(fundT,'t-1');

* --- (63) Define implicitly fundP (= domestically financed private investments) as difference between total funds
*     and the domestic contribution to new government debt FundG, additivity guaranteed by function form

  e_fundP(ts(t)) ..
   fundP(t)s(fundP,'t-1') =e= (fundT(t) - fundG(t))s(fundP,'t-1');

* --- (64) Composite price index for funds (additive CET): average of  interest rate on gov. bonds
*                                                     and expected returns to capital (rore)
  e_fundPriceD(ts(t)) ..
   fundPriceD(t)s(fundPriceD,'t-1')
    =e=  (        p_aFundG(t) * chiFundG(t)  * (interest(t)*pgdpmp(t)/interest.l(t0))**(p_omegaFund(t))
           +   (1-p_aFundG(t) * chiFundG(t)) * (rore(t)    *pgdpmp(t)/rore.l(t0)    )**(p_omegaFund(t))
         )**(1/(p_omegaFund(t)))s(fundPriceD,'t-1');

* --- (65) Funds available to finance new government debt: share is updated by relative changes
*     in interest rate paid by government
*     and the composite price of interest and returns to capital
  e_fundG(ts(t)) ..
   fundG(t)s(fundG,'t-1')
    =e= [ FundT(t)
          * (-1.E-8+p_aFundG(t) * chiFundG(t)
          * (interest(t)*pgdpmp(t)/[fundPriceD(t)*interest.l(t0)])**p_omegaFund(t))
        ]s(fundG,'t-1');

* --- (66) Exhaustion condition for funds, which define prices fundPrice (different from composite price for additive CET)

  e_fundPrice(ts(t)) ..
   [fundPrice(t) * fundT(t)]s(fundT,'t-1')
    =e= [  fundG(t) * (interest(t)/(interest.l(t0))) + fundP(t) * (rore(t)/rore.l(t0))]s(fundT,'t-1');

* --- (67),(68) Expected rate of return to capital

  e_rore(ts(t))..
   rore(t)s(rore,'t-1')
    =e= [
        $$iftheni.forwardLooking "%ForwardLooking%"=="on"

           sum(tNext $ (years(tNext) ge years(t)), p_exptWgts(tNext)
                   *(rorc(tNext) *(kstocke(tNext)/kstock(tNext))**(epsRorE(t))) $ (not ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) )))
           /sum(tNext $ (years(tNext) ge years(t)), p_exptWgts(tNext))

         $$else.forwardLooking
                   (rorc(t)*(kstocke(t)/kstock(t))**(epsRorE(t)))       $ (not ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) ))
         $$endif.forwardLooking
                +  ((1+rorc(t))/(1+interest(t)/100) - 1)          $      ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) )
        ]s(rore,'t-1');

* --- (69) Anticipated end-of-period capital stock

  e_kstocke(ts(t))..
   kstocke(t)s(kstocke,'t-1')
     =e= [(1-depr(t))*kstock(t) + prodInv(t)]s(kstocke,'t-1');
*
* --- (70) determines productive investments
*
  e_prodInv(ts(t)) ..

    prodInv(t)s(prodInv,'t-1')
     =E=
          sum(inv,
            + xfd(inv,t)
*           --- only a share of government investment is increasing the capital stock
            +  p_phiPcInv(inv,t) * sum(ginv, xfd(ginv,t))
*           --- remove investment in R&D if endotech is active
            - xfdRD(inv,t) $ p_chiFdRD(inv,t)
*           --- remove other non-productive investments
            - p_nonProdInv(inv,t))s(prodInv,'t-1')
     ;


* --- (71) Cost adjusted rate of return to capital: capital trent over price of investments minus deprecition rate
*     the fudging function keeps rorc above zero

  e_rorc(ts(t))..
   rorc(t)s(rorc,'t-1') =e= -ncpVuPow(-1.E-10,-[trent(t)/sum(inv, pf(inv,t)) - depr(t)],1.E-7)s(rorc,'t-1');

* --- (72) depreciation rate as weighted average of activity specific depreciation rates

  e_depr(ts(t)) ..
   depr(t)
    =E= sum((sameas(a,fma),vf,cap,fmp) $ (sameas(cap,fmp) $ xfmSup.l(fma,fmp,"suminst",vf,t0)),
                  xfmSup(fma,fmp,"suminst",vf,t) * deprA(a,t)
*
*                 --- activity specific depreciation rate is updated with short-run fluctation
*                     of capital utilization rate
                   *(1  + p_epsFmpIntens(cap,a) * (pxfac_pretax(cap,a,old,t)/pxfac_pretax.scale(cap,a,old,t)-1) $ isShortRun )
                   *m_dmgAgg(dmgDepr,'a')
         )
         /sum((sameas(a,fma),vf,cap,fmp) $ (sameas(cap,fmp) $ xfmSup.l(fma,fmp,"suminst",vf,t0)),
                   xfmSup(fma,fmp,"suminst",vf,t));

* --- (73) Net aggregate rate of return to capital

  e_ror(ts(t))..
   ror(t)/ror.l(t0) =e=  p_chiRor(t)*([(trent(t)/(trent.l(t0)*pgdpmp(t)))**epsRor(t)] $ epsRor.l(t0) + 1 $ (not epsRor.l(t0)));

* --- (74) Capital normalization equation
*     (Converts stock into yearly capital use)

  e_kaps(ts(t)) ..
   kaps(t)s(kaps,'t-1')
    =e= prod(cap,m_dmgAgg(dmgSup,'cap')) * ror(t)*kstock(t)s(kaps,'t-1');

* ----------------------------------------------------------------------------
*
*  External funds, goverment debt and interest rate
*
* ----------------------------------------------------------------------------

* --- (75) Behavioral equation for R.o.W. contribution to new government debt
*     Increase with risk corrected interest on goverment debt according to elasticty p_sigmaFInt
*     and a sigmoid function with reduces the available fund with growing debt to GDP ratio

  e_fundF(ts(t)) ..
   fundF(t)s(fundF,'t-1')
    =E= [1.E-12+p_gammaFundF(t) * (interest(t)*sum(instf,riskPrem(instf,t)))**p_sigmaFInt
                             * pgdpmp(t)
      * ( 1 + ncpVuPow(0,-{sigmoid([debtGdp(t)-0.5]*4)-0.5}*2,1.E-7))* rgdpmp(t)/rgdpmp.l(t0)]s(fundF,'t-1');

* --- (76) Funds available for government borrowing equals to government deficit
*        (solves for interest rate on government bonds)
*         if govBalance<0 new debt equals to govBalance; if govBalance>0 new debt is zero.

  e_interest(ts(t)) ..
   debtNewT(t)
      %s1%    /max(%minScale%,debtNewT.l(t-1)*%scaleMult%,1.E-2*abs(ygov.l(t-1))*%scaleMult%)
    =e= -ncpVuPow(-1.E-6,govBalance(t),max(1.E-7,1.E-7*abs(ygov.l(t-1))))
      %s1%     /max(%minScale%,debtNewT.l(t-1)*%scaleMult%,1.E-2*abs(ygov.l(t-1))*%scaleMult%)
   ;

* --- (78)+(79) New borrowing by institutions

  e_debtNew(inst,t) $ (p_chiDebtNew(inst,t) and ts(t)) ..
   debtNew(inst,t)s(debtNew,'inst,t-1')
    =E= [   fundG(t)*p_chiDebtNew(inst,t) $ instD(inst)
          + fundF(t)                      $ instF(inst)
        ]s(debtNew,'inst,t-1');

* ----------------------------------------------------------------------------
*
*  Debt stocks and I=S
*
* ----------------------------------------------------------------------------

* --- (80) Debt stock: last year stock, minus principal, plus new debt

  e_debtStk(instfd,ts(t)) ..
   debtStk(instfd,t)s(debtStk,'instfd,t-1')
    =e= [{DebtStk.l(instfd,t-1) $ (not tNext(t-1)) + DebtStk(instfd,t-1) $ tNext(t-1)}
         - debtPrincipal(instfd,t) + debtNew(instfd,t)]s(debtStk,'instfd,t-1');

* --- (81) Total debt stock: sum of foreign and domestic, solely used to calculate variable DebtGDP

  e_debtStkT(ts(t)) ..
   debtStkT(t)s(debtStkT,'t-1') =e= sum(instfd,DebtStk(instfd,t))s(debtStkT,'t-1');

* --- (82) Debt to GDP ratio

  e_debtGDP(ts(t)) ..
   debtGDP(t) =e= DebtStkT(t) / gdpmp(t);

* --- (83) total debt payments (principal and interest)

  e_debtPay(instfd,ts(t)) $ (debtPay.range(instfd,t) ne 0) ..
   debtPay(instfd,t)s(debtPay,'instfd,t-1')
    =e= [debtInterest(instfd,t) + debtPrincipal(instfd,t)]s(debtPay,'instfd,t-1');

* --- (84) Interest payments on current debt stock using the average interest rate defined above

  e_debtInterest(instfd,ts(t)) $ (debtInterest.range(instfd,t) ne 0) ..
   debtInterest(instfd,t)s(debtInterest,'instfd,t-1')
    =e= ((DebtStk(instfd,t)-debtNew(instfd,t))*avgDebtInt(instfd,t)/100)s(debtInterest,'instfd,t-1');


* --- (85) Principal payments on existing debt, a fixed share calibrated in inical.inc

  e_debtPrincipal(instfd,ts(t)) $ (debtPrincipal.range(instfd,t) ne 0) ..
   debtPrincipal(instfd,t)s(debtPrincipal,'instfd,t-1')
    =e= [rDebtStkT(t) * rDebtStk(instfd,t) * DebtStk(instfd,t-1)]s(debtPrincipal,'instfd,t-1');

* --- (86) Average interest rate on past and current new debt

  e_avgDebtInt(instfd,ts(t)) $ sum(t1 $ [((t1.pos lt t.pos)) $ tt(t1)],
                                  max(0,(debtNew.l(instfd,t1)+1.E-10)*(1 - rDebtStk.l(instfd,t1)*(t.pos-t1.pos)))) ..
   avgDebtInt(instfd,t)s(avgDebtInt,'instfd,t-1')
    =E= [  sum(t1 $ [((t1.pos lt t.pos)) $ tt(t1)],
              max(0,(debtNew(instfd,t1)+1.E-10)*(1 - rDebtStk(instfd,t1)*(t.pos-t1.pos))) * interest(t1)*riskPrem(instfd,t1))
         / sum(t1 $ [((t1.pos lt t.pos)) $ tt(t1)],
              max(0,(debtNew(instfd,t1)+1.E-10)*(1 - rDebtStk(instfd,t1)*(t.pos-t1.pos))))
        ]s(avgDebtInt,'instfd,t-1');

* --- (87) New total borrowing, summed up over domeistic institutions and foreign savings

  e_debtNewT(ts(t)) ..

   debtNewT(t)
     %s1%   /max(%minScale%,debtNewT.l(t-1)*%scaleMult%,1.E-2*abs(ygov.l(t-1))*%scaleMult%)
     =E= sum(instfd $ p_chiDebtNew(instfd,t),debtNew(instfd,t))
    %s1%   /max(%minScale%,debtNewT.l(t-1)*%scaleMult%,1.E-2*abs(ygov.l(t-1))*%scaleMult%)
;

* --- (88) Investment savings balance, including stock change (domestic delst and imported mdelst),
*     minus CC related adaptation investments

  e_inv(inv,ts(t)) ..
   yf(inv,t)s(yf,'inv,t-1')
   =e= [
*          --- part of enterprise and household savings not used for new domestic debt
           fundP(t)
*          --- add a positive government deficit (smooth max), by assumption used for investments
           - ncpVuPow(0,-govBalance(t)-max(1.E-7,1.E-7*ygov.l(t-1)),max(1.E-7,1.E-7*abs(ygov.l(t-1))))
*
*          --- difference between total foreign savings in local currency and new foreign debt
           +  er(t)*savF(t) - fundF(t)
*          --- nominal total stock changes domestic and foreign
           - tdelst(t) $ ifDelSt
*          --- private adaptive investments
           -  sum(map_adpInv(inv,adpinv),yf(adpinv,t))
*          --- R&D investments
           + xfdRD(inv,t)*pf(inv,t) $ p_chiFdRD(inv,t)
         ]s(yf,'inv,t-1');

* ----------------------------------------------------------------------------
*
*  Demographic equations
*
* ----------------------------------------------------------------------------

* --- (89) Number of new borns is previous year's working age population times birth rate
*     Determine the population in age class 1

  e_birth(age,ts(t)) $ (ord(age) eq 1) ..
   popAge(age,t)s(popAge,'age,t-1') =e=  brateU(t) * pop.l("P1564",t-1)s(popAge,'age,t-1');

* --- (90) Number of persons of a certain age, is previous years population which was in next
*     younger age clase, times the survival rate

  e_popAge(age,ts(t)) $(ord(age)>1 and ord(t)>1) ..
   popAge(age,t)s(popAge,'age,t-1')
*       --- previous years population times the survical rate
    =e= [popAge.l(age-1,t-1)*drate(age,t)
*       --- times the reduction in population by age due to climate change damages aggregated over damage channels
         * m_dmgAgg(dmgStk,'age')]s(popAge,'age,t-1');

* --- (91) Population by cohorts (adding up over the ages)

  e_pop(cohorts,ts(t)) ..
   pop(cohorts,t)s(pop,'cohorts,t-1') =e= sum(mapAge(age,cohorts),popAge(age,t))s(pop,'cohorts,t-1');

* --- (92) Population for each household: base year population share time current year population
*     Currently, no demographic differences across households

  e_hpop(h,ts(t)) ..
   hpop(h,t)s(hpop,'h,t-1') =e=  p_hpop0(h)/p_pop0("ptotl")*pop("ptotl",t)s(hpop,'h,t-1');

* ----------------------------------------------------------------------------
*
*  Household demand equations
*
* ----------------------------------------------------------------------------

* --- (93) Intermediate expression used in CDE equations, k are product groups

  e_cdeInt(akFact(h,topKNest),ts(t))  ..
   cdeInt(h,topKNest,t)
       %s1%   /max(1.E-5,cdeInt.l(h,topKNest,t-1)*%scaleMult%)
    =E=   p_akf(h,topKNest,t)*bh(h,topKNest,t)
        * (uh(h,t)**(eh(h,topKNest,t)*bh(h,topKNest,t)))
        * (pxak(h,topKNest,t)**(bh(h,topKNest,t)))
        * ((yf(h,t)/hpop(h,t))**(-bh(h,topKNest,t)))
      %s1%    /max(1.E-5,cdeInt.l(h,topKNest,t-1)*%scaleMult%)
  ;
*
* ---(94) sum of the above over product groups k, division gives  budget share
*
  e_consScale(h,ts(t)) ..
   consScale(h,t)
     %s1% /max(1.E-5,consScale.l(h,t-1)*%scaleMult%)
    =e= sum(akfact(h,topKNest),cdeInt(h,topKNest,t))
     %s1% /max(1.E-5,consScale.l(h,t-1)*%scaleMult%)
  ;

* --- (95) Marshallian for household demands' for bundles in the top level
*     CDE demand system

  e_xakh(akFact(h,topKNest),ts(t)) ..
   xak(h,topKNest,t)s(xak,'h,topKNest,t-1')
    =e= (yf(h,t)/pxak(h,topKNest,t)*cdeInt(h,topKNest,t)/consScale(h,t))s(xak,'h,topKNest,t-1');

* --- (96) Household utility

  e_uh(h,ts(t))  ..
   1 =e= sum(akfact(h,topKNest),cdeInt(h,topKNest,t)/bh(h,topKNest,t));

* --- (97) Household money metric utility

  e_mmuh(h,ts(t)) ..
   1 =e= sum(akfact(h,topKNest), p_akf(h,topKNest,t)
       *  (uh(h,t)**(eh(h,topKNest,t)*bh(h,topKNest,t)))
       /  ((mmuh(h,t)/hpop(h,t))**(bh(h,topKNest,t))));

* --- (98) Household budget budget shares (in simulation mode only for information)

  e_xakShr(xakAct(oa,topKNest),ts(t)) ..
   xakShr(oa,topKNest,t) =e= pxak(oa,topKNest,t)*xak(oa,topKNest,t)/yf(oa,t);

* --- (99) Consumer price index

  e_pfh(h,ts(t)) ..
*      Original equation (approximation?):
*        pf(h,t) =e= sum(k, xkfshr(h,topKNest,t)*pkf(h,topKNest,t));
*      If we want to use rgdpmp as a measure of welfare:
*        pf(h,t) =e= sum(k, xkf(h,topKNest,t)*pkf(h,topKNest,t)) / uh(h,t) / pfini(h);
*      If we want to use rgdpmp as a measure of GDP, we could use Laspeyres price index or Paasche price index.
*       Paasche price index:
    pf(h,t)s(pf,'h,t-1')
    =e= sum(i $ sam(i,h), xa(i,h,t)*paf(i,h,t)) /  sum(i $ sam(i,h), xa(i,h,t)*p_pafini(i,h))s(pf,'h,t-1');

*
*   (100)  Laspeyres price index:
*   pf(h,t) =e= sum(i, xaini(i,h)*paf(i,h,t)) /  sum(i, xaini(i,h)*pafini(i,h));

* ----------------------------------------------------------------------------
*
*  Multiple layer flexible CES demand function
*
* ----------------------------------------------------------------------------

* --- (102) Aggregate demand for top level demand bundles, CES, for final demanders
*     (f = oa - h)

  e_xakf(f,topKNest,ts(t)) $ p_akf(f,topKNest,t) ..

   xak(f,topKNest,t)s(xak,'f,topKNest,t-1')
    =e= [   {p_akf(f,topKNest,t)*xfd(f,t)*(pf(f,t)/pxak(f,topKNest,t))**p_sigmaF(f)} $ p_sigmaF(f)
           + p_akf(f,topKNest,t)*xfd(f,t) $ (not p_sigmaF(f))
         ]s(xak,'f,topKNest,t-1');

* --- (103) Aggregate price deflator for agent f: non-linear weighted average of
*     price for top bundles

  e_pf(f,ts(t)) $ p_yf0(f,t) ..
   pf(f,t)s(pf,'f,t-1')
    =e= [  { sum(topKNest $ p_akf(f,topKNest,t), p_akf(f,topKNest,t)*pxak(f,topKNest,t)**(1-p_sigmaF(f)))**(1/(1-p_sigmaF(f))) } $ p_sigmaF(f)
          +  sum(topKnest $ p_akf(f,topKNest,t), p_akf(f,topKNest,t)*pxak(f,topKNest,t)) $ (not p_sigmaF(f))
        ]s(pf,'f,t-1');
*
* --- (104) Demand for child bundle KNest not part of the top level demand system
*     Indicator set consCont
*
  e_xak(oa,kNest,ts(t)) $ conscont(oa,kNest) ..
   xak(oa,kNest,t)s(xak,'oa,kNest,t-1')
    =e= [ sum(kNest1 $ p_afk(oa,kNest,kNest1,t),
*                 --- share parameter times parent nest demand kNest1 (can be driven by multiple ones)
                  p_afk(oa,kNest,kNest1,t)*xak(oa,kNest1,t)
*                 --- price impact
                 *[1 + {[(pxak(oa,kNest1,t)/pxak(oa,kNest,t))**p_sigmaC(oa,kNest1)-1] $  p_sigmaC(oa,kNest1)}])
        ]s(xak,'oa,kNest,t-1');

* --- (105) Price of demand bundle kNest, weighted average of price of components (= products i contributing to kNest,
*           plus prices of childs kNest1 comprised in the nest

  e_pxak(xakAct(oa,knest),ts(t)) ..
   pxak(oa,knest,t)s(pxak,'oa,kNest,t-1')
     =e= (   [  {    sum(i $ p_af(oa,i,knest,t),
                           p_af(oa,i,knest,t)*(p_chiPA(i)*paf(i,oa,t)
                                   /(m_dmgAgg(dmgDem,'oa,i,knest') * (lambdaOa(oa,i,knest,t)*(1+lambdaNRG(t)$e(i)))))**(1-p_sigmaC(oa,knest)))
                   + sum(knest1 $ p_afk(oa,knest1,knest,t), p_afk(oa,knest1,knest,t)*pxak(oa,knest1,t)**(1-p_sigmaC(oa,knest)))
                 }**(1/(1-p_sigmaC(oa,knest)))
              ] $ p_sigmaC(oa,knest)

            + [   sum(i $ p_af(oa,i,knest,t),
                      p_af(oa,i,knest,t)*(p_chiPA(i)*paf(i,oa,t)
                                     /(m_dmgAgg(dmgDem,'oa,i,knest') * (lambdaOa(oa,i,knest,t)*(1+lambdaNRG(t)$e(i))))))
                + sum(knest1 $ p_afk(oa,knest1,knest,t), p_afk(oa,knest1,knest,t)*pxak(oa,knest1,t))
              ] $ (not p_sigmaC(oa,knest))

          )s(pxak,'oa,kNest,t-1');
*
* --- (106) Armington demands for all final demanders, the flag xaact depicts
*     that good i is consumded by the demander
*
  e_xaoa(i,oa,ts(t)) $ xaact(i,oa)..
   xa(i,oa,t)s(xa,'i,oa,t-1')
    =e= [sum{map_i_knest(i,knest) $ (p_af(oa,i,knest,t) and p_sigmaC(oa,knest)),
*                  --- share of a demand bundle xak (not necessarily a 1:1 relation,
*                      one product can be par of multiple nests)
                   p_af(oa,i,knest,t)*xak(oa,knest,t)
*                  --- shifters: CCC damages, specific, for all energy demand
              *     [m_dmgAgg(dmgDem,'oa,i,knest')*lambdaOa(oa,i,knest,t)*(1+lambdaNRG(t)$e(i))]**(p_sigmaC(oa,knest)-1)
*                  --- price dependent part
              *     [ pxak(oa,knest,t)/(p_chiPA(i)*paf(i,oa,t))]**p_sigmaC(oa,knest)}

*
*        --- same as above for Leontief case, to reduce computational burden
*
      +  sum{map_i_knest(i,knest) $ (p_af(oa,i,knest,t) and (not p_sigmaC(oa,knest))),
                   p_af(oa,i,knest,t)*xak(oa,knest,t)
                   /[m_dmgAgg(dmgDem,'oa,i,knest')*lambdaOa(oa,i,knest,t)*(1+lambdaNRG(t)$e(i))]}
              ]s(xa,'i,oa,t-1');

* --- (107) ammount of new investment. Active under sector specific investments

  e_newInv(ts(t)) $ ifSectSpecInv ..
*
*     --- the economy-wide investments are equal to capital use of new vintage
*         plus some share (usually 2.5%) of old vintage capital use
*
   newInv(t)s(newInv,'t-1')
         =E= sum((cap,aact(a)) $ flagXFac(cap,aact,new), xfac(cap,a,new,t)+p_oldCapInvShare(a)*xfac(cap,a,old,t))s(newInv,'t-1');
*
* --- (108) shifter for top level investment demand by product
*
  e_afInv(inv,i,"top",ts(t)) $ (p_af(inv,i,"top",t) $ ifSectSpecInv $ (lambdaOa.range(inv,i,"top",t) ne 0)) ..

*     --- the economy-wide investment share (change, via lambdaOa) is a weighted average of the activity specific ones,
*         using the capital use as weight

   lambdaOa(inv,i,"top",t)*p_lambdaInv0(i)s(lambdaOa,'inv,i,"top",t-1')
    =E= [ sum((cap,aact(a)) $ gammaInv.l(i,a,t0),
*           --- total investment demand by sector
              (xfac(cap,a,new,t)+p_oldCapInvShare(a)*xfac(cap,a,old,t))
*           --- times endogenous share
               * gammaInv(i,a,t) )
*             --- divided by total new investment and the share at the benchmark
              /newInv(t)]/p_af(inv,i,"top",t0)s(lambdaOa,'inv,i,"top",t-1');

* --- (109) CES-based activity specific investment price

  e_invPrice(aact(a),ts(t)) $ ifSectSpecInv ..

   invPrice(a,t)  =E= [sum(i $ gammaInv.l(i,a,t0),
                                   gammaInv.l(i,a,t0) * sum(inv,p_chiPA(i)*paf(i,inv,t))**(1-p_sigmaInv(a)))**(1/(1-p_sigmaInv(a)))
                      ]  $ p_sigmaInv(a)
                    + [sum(i $ gammaInv.l(i,a,t0),
                                    gammaInv.l(i,a,t0) * sum(inv,p_chiPA(i)*paf(i,inv,t)))
                      ]  $ (not p_sigmaInv(a));

* --- (110) CES driven activity specific investment shares

  e_gammaInv(i,aact(a),ts(t)) $ (gammaInv.l(i,a,t0) $ ifSectSpecInv) ..

   gammaInv(i,a,t)/p_gammaInv0(i,a)
    =E=  [(invPrice(a,t)/sum(inv,p_chiPA(i)*paf(i,inv,t)))**p_sigmaInv(a) ] $ p_sigmaInv(a) + 1 $ (not p_sigmaInv(a));

* ----------------------------------------------------------------------------
*
*  Miscellaneous demand equations
*
* ----------------------------------------------------------------------------

* --- (111) Aggregate margins to deliver good i to each of the three nodes

  e_xmarg(j,ts(t)) $ ( mgencl(j)) ..
   xmarg(j,t)s(xmarg,'j,t-1')
    =e= [tmg(j,"d",t)*xd(j,t) $ iDom(j) + tmg(j,"m",t)*xm(j,t) $ iImp(j) + tmg(j,"x",t)*xe(j,t) $ iExp(j)]s(xmarg,'j,t-1');

* --- (112) Demand for margin commodities (Additive CES)

  e_xarmMarg(i,j,ts(t)) $ ( p_amarg(i,j) ne 0) ..
   xa(i,j,t)s(xa,'i,j,t-1')
     =e= (  [p_amarg(i,j)*xmarg(j,t)*(pmargInd(j,t)/(p_chiPA(i)*pa(i,t)))**p_sigmaMg(j)] $ p_sigmaMg(j)
          + [p_amarg(i,j)*xmarg(j,t)] $ (not p_sigmaMg(j)) )s(xa,'i,j,t-1');

* --- (113) Related price index (additive CES, beware, pmarg is at unity and not equal to the exhaustion
*                          price. This requires a correction factor by division with the term evaluated at 0)

  e_pmargInd(j,ts(t)) $ mgencl(j) ..
   pmargInd(j,t) =e=   [ sum(i $ p_amarg(i,j), p_amarg(i,j)*(p_chiPA(i)*pa(i,t))**(-p_sigmaMg(j)))**(1/(-p_sigmaMg(j)))
                        /sum(i $ p_amarg(i,j), p_amarg(i,j)*(p_chiPA(i)*pa.l(i,t0))**(-p_sigmaMg(j)))**(1/(-p_sigmaMg(j)))] $ p_sigmaMg(j)
                     + [sum(i $ p_amarg(i,j), p_amarg(i,j)*(p_chiPA(i)*pa(i,t)))] $ (not p_sigmaMg(j));


* --- (114) Exhaustion prices (= average prices) used in the remainder of the model. This is the dual form
*     which calculates the shares times the prices

  e_pmarg(j,ts(t)) $ mgencl(j) ..
   pmarg(j,t) =e=    [sum(i $ p_amarg(i,j), p_amarg(i,j)*(p_chiPA(i)*pa(i,t))**(1-p_sigmaMg(j))) *  pmargInd(j,t)**p_sigmaMg(j)] $ p_sigmaMg(j)
                   + pMargInd(j,t) $ (not p_sigmaMg(j));

* --- (115) Volume or value definition for total final demand for non-household final demanders oa

  e_yf(oa,ts(t)) $ p_yf0(oa,t) ..
   yf(oa,t)s(yf,'oa,t-1') =e= [pf(oa,t)*xfd(oa,t)]s(yf,'oa,t-1');

* ----------------------------------------------------------------------------
*
*  Stock changes and stocks
*
* ----------------------------------------------------------------------------

* --- (116) stock flow equation for commodity stock st, previous year stock plus stock changes delst

  e_st(iAct(i),ts(t)) $ p_alphaSt(i,t) ..
    st(i,t)s(st,'i,t-1')
     =E= [(st(i,t-1) $ tNext(t-1) + st.l(i,t-1) $ (not tNext(t-1))) + delst(i,t)]s(st,'i,t-1');
*
* --- (117) stock changes
*
  e_delst(iAct(i),ts(t)) $ p_alphaSt(i,t) ..
    delst(i,t)
     %s1% /max(%minScale%,0.01*p_alphaSt(i,t)*xsExp.l(i,t-1)*%scaleMult%)
      =E=
*
*             --- elasticity of stock releases (if expected price is higher than
*                 current period one, build up stocks)
*
       [     (st(i,t-1) $ tNext(t-1) + st.l(i,t-1) $ (not tNext(t-1)))
               * [ (-ncpVuPow(-1.E-6,-psExpRel(i,t),1.E-4))**p_rhoPDelSt(i) - (-ncpVuPow(-1.E-6,psExpRel(i,t),1.E-4))**p_rhoPDelSt(i) ]
                           $ sum(tNext, years(tNext) gt years(t))
*
*             --- elasticity of stock over production (= desired stock level)
*
         +   xsExp(i,t) * {(2*p_alphaSt(i,t)/((st(i,t)+st(i,t-1) $ tNext(t-1) + st.l(i,t-1) $ (not tNext(t-1)))/xsExp(i,t)))**p_rhoXDelSt(i) -1}
       ]
    %s1% /max(%minScale%,0.01*p_alphaSt(i,t)*xsExp.l(i,t-1)*%scaleMult%)
  ;

* --- (118) behavioral equation for stock changes
*     (reacts to changes in expected prices and a target stock level)

  e_psExpRel(iAct(i),ts(t)) $ p_alphaSt(i,t)  ..
    psExpRel(i,t) =E= [(psExp(i,t)-ps(i,t))/psExp(i,t)];

* --- (119) expected prices, weighted shares of future simulated prices

  e_psExp(iAct(i),ts(t)) $ p_alphaSt(i,t) ..
   psExp(i,t)s(psExp,'i,t-1')
     =E= sum(tNext $ (years(tNext) ge years(t)), p_exptWgts(tNext) * ps(i,tNext))
        /sum(tNext $ (years(tNext) ge years(t)), p_exptWgts(tNext))s(psExp,'i,t-1');

* --- (120) expected output (used to drive desired stock : output level)

  e_xsExp(iAct(i),ts(t)) $ p_alphaSt(i,t) ..
   xsExp(i,t)s(xsExp,'i,t-1')
    =E=[ sum(tNext $ (years(tNext) ge years(t)), p_exptWgts(tNext) * xs(i,tNext))
        /sum(tNext $ (years(tNext) ge years(t)), p_exptWgts(tNext))]s(xsExp,'i,t-1');

* ----------------------------------------------------------------------------
*
*  Armington decomposition
*
* ----------------------------------------------------------------------------

* --- (121) Agents' price of imports (after tariffs, import marings and VAT)

  e_pma(iImp(i),ts(t)) ..
   p_chiPMa(i)*pma(i,t)s(pma,'i,t-1')
    =e= (er(t)*pwm(i,t)*(1+tImp(i,t)) + pmarg(i,t)*tmg(i,"m",t)) *  (1+tVat(i,"m",t))s(pma,'i,t-1');

* --- (122) Agents' price of domestically produced goods (after margins and VAT)

  e_pda(iDom(i),ts(t))  ..
   p_chiPDa(i)*pda(i,t)s(pda,'i,t-1')
    =e= (p_chiPD(i)*pd(i,t) + pmarg(i,t)*tmg(i,"d",t)) *  (1+tVat(i,"d",t))s(pda,'i,t-1');

* --- (123) Aggregate Armington demand, summation of all Armington agents aa (sectors and final demanders)
*     The summation implies that each demander faces the same pre sales-tax price pa

  e_xat(iArm(i),ts(t)) ..
   xat(i,t)s(xat,'i,t-1') =e= sum(aaIn(aa) $ sam(i,aa), xa(i,aa,t))s(xat,'i,t-1');


* --- (124) + (126) Domestic demand for domestic goods (standard CES or additive one)
*     p_gammaMd is an exogenous shifter

  e_xdd(idom(i),ts(t))..
   xd(i,t)s(xd,'i,t-1')
    =e= p_ad(i,t)*p_gammaMd(i,t)**[ -p_sigmaM(i) $ addCest(i) + (p_sigmaM(i)-1) $ (not addCest(i))]
          * xat(i,t)*((paIND(i,t) $ addCest(i) + pa(i,t) $ (not addCest(i)))/pda(i,t))**p_sigmaM(i)s(xd,'i,t-1');

* --- (125) + (127) Domestic demand for imported goods (ACES)
*     p_gammaMm is an exogenous shifter

  e_xm(iimp(i),ts(t))..
   xm(i,t)s(xm,'i,t-1')
    =e= p_am(i,t)*p_gammaMm(i,t)**[ -p_sigmaM(i) $ addCest(i) + (p_sigmaM(i)-1) $ (not addCest(i))]
          * xat(i,t)*((paIND(i,t) $ addCest(i) + pa(i,t) $ (not addCest(i)))/pma(i,t))**p_sigmaM(i)s(xm,'i,t-1');

* --- (128) * (129) Armington price composite price, after margins and VAT, but before further sales and emission taxes
*     (defines paInd for additive CES and pa otherwise)

  e_pa(iArm(i),ts(t)) ..

   [   pa(i,t)    $ (not addCest(i))
     + paInd(i,t) $ (    addCest(i))]s(pa,'i,t-1')
    =e= ([  (p_ad(i,t)*(pda(i,t)/p_gammaMd(i,t))**(1 $ (not addCest(i)) - p_sigmaM(i))) $ p_ad(i,t)
         +  (p_am(i,t)*(pma(i,t)/p_gammaMm(i,t))**(1 $ (not addCest(i)) - p_sigmaM(i))) $ p_am(i,t)]**(1/(1 $ (not addCest(i)) - p_sigmaM(i))))s(pa,'i,t-1');

* --- (130) dual definitiion of exhaustion price pa in case of additive CES

  e_paIND(iact(i),ts(t)) $ addCest(i) ..

   pa(i,t)s(pa,'i,t-1')
    =E= [  (p_ad(i,t)*(pda(i,t)/p_gammaMd(i,t))**(1-p_sigmaM(i)) ) $ p_ad(i,t)
        +  (p_am(i,t)*(pma(i,t)/p_gammaMm(i,t))**(1-p_sigmaM(i)) ) $ p_am(i,t)] * paInd(i,t)**p_sigmaM(i)s(pa,'i,t-1');

* ----------------------------------------------------------------------------
*
*  Tax rate definitions from tax shifters
*
* ----------------------------------------------------------------------------

* --- (131) Factor Taxes (ad-valorem)

  e_tFac(aact(a),actFp,ts(t)) $ (sam(actFp,a) $ (tFac.range(a,actFp,t) ne 0)) ..
   tFac(a,actFp,t) =e= (tFac0(a,actFp,t) + (tFacD0(t) + tFacD1(a,t) + tFacD2(actFp,t)))
                   * tFacM0(t) * tFacM1(a,t) * tFacM2(actFp,t)
*                    --- multiplicative shifters for labor (tfacMLab for all activities, tfacMLabA activity specific)
                 * (    ( tfacMLab(t)*tfacMLabA(a,t))   $ l(actFp)
*                    --- multiplicative shifters for capital
                     + tfacMCap(t)      $ cap(actFp)
*                    --- multiplicative shifters for land or natural ressources

                     + tfacMlnd(t)      $ (lnd(actFp) or natres(actFp)))
*
*                    --- shifter for all taxes. Note the impact of the sign function: if the tax rate
*                        is negative (= subsidy), an increase in tMall (to raise gov revenues)
*                        will decrease the subsidy rate, if the tax rate is positive,
*                        tmMall will increase it instead
*
                      * (sign(tFac.l(a,actFp,t-1)) * (tMall(t)-1)+1);

* --- (132) Production subsidies (assumed to be negative, ad-valorem)

  e_tPsb(aact(a),ts(t)) $ (tPsb.range(a,t) ne 0)  ..
   tPsb(a,t) =e= (tPsb0(a,t) + (tPsbD0(t) + tPsbD1(a,t)))
                * tPsbM0(t) * tPsbM1(a,t) * (-sign(tPsb.l(a,t-1)) * (tMall(t)-1)+1);

* --- (133) Production taxes (ad-valorem)

  e_tPrd(aact(a),ts(t)) $ (tPrd.range(a,t) ne 0)  ..
   tPrd(a,t) =e= (tPrd0(a,t) + (tPrdD0(t) + tPrdD1(a,t)))
                * tPrdM0(t) * tPrdM1(a,t) * (sign(tPrd.l(a,t-1)) * (tMall(t)-1)+1);

* --- (134) Phantom taxes (usually an additional equation ensures that tax income
*                    added over the activities ammounts to zero)

  e_tPhn(aact(a),ts(t)) $ (tphn.range(a,t) ne 0)  ..
   tPhn(a,t) =e= tPhn0(a,t) + tPhnD0(t) + tPhnD1(a,t)
*               ----  agregate activity specific additive shifter
                + sum(mapAagr(aagr,a),tPhnD2(aagr,t));

* --- (135) Export taxes (ad-valorem)

  e_tExp(iExp(i),ts(t)) $ (tExp.range(i,t) ne 0) ..
   tExp(i,t) =e= (tExp0(i,t) + (tExpD0(t) + tExpD1(i,t)))
                * tExpM0(t) * tExpM1(i,t) * (sign(tExp.l(i,t-1)) * (tMall(t)-1)+1);

* --- (136) Import taxes (ad-valorem)

  e_tImp(iImp(i),ts(t)) $ (tImp.range(i,t) ne 0) ..
   tImp(i,t) =e= (tImp0(i,t) + (tImpD0(t) + tImpD1(i,t)))
                * tImpM0(t) * tImpM1(i,t) * (sign(tImp.l(i,t-1)) * (tMall(t)-1)+1);

* --- (137) Emission taxes. tEmid0 is only applied to emissions contributing to
*     global warmining. Exempt for general tax shifter tmAll

  e_tEmi(emiFlag(em,aaIn(aa)),ts(t)) $ ((tEmi.range(em,aa,t) ne 0) $ hasEmit) ..
   tEmi(em,aa,t)s(tEmi,'em,aa,t-1')
    =e= [(tEmi0(em,aa,t) + (tEmiD0(t) $ p_emiConv(em,"GHGs") + tEmiD1(em,t) + tEmiD2(aa,t)))
       * tEmiM0(t) * tEmiM1(em,t) * tEmiM2(aa,t)]s(tEmi,'em,aa,t-1') ;

* --- (138) Personal income taxes (ad-valorem)

  e_tPit(h,ts(t)) ..
   tPit(h,t) =e= (tPit0(h,t) + (tPitD0(t) + tPitD1(h,t)))
               * tDr(t) * tPitM0(t) * tPitM1(h,t) * (sign(tPit.l(h,t-1)) * (tMall(t)-1)+1);

* --- (139) Corporate income taxes (ad-valorem)

  e_tCit(entr,ts(t)) ..
   tCit(entr,t) =e= (tCit0(entr,t) + (tCitD0(t) + tCitD1(entr,t)))
                   * tDr(t) * tCitM0(t) * tCitM1(entr,t) * (sign(tCit.l(entr,t-1)) * (tMall(t)-1)+1);

* --- (140) VAT taxes

  e_tVat(iArm(i),n,ts(t)) $ (tVat.range(i,n,t)  ne 0) ..
   tVat(i,n,t) =e= (tVat0(i,n,t) + (tVatD0(t) + tVatD1(i,t) + tVatD2(n,t)))
                  * tVatM0(t) * tVatM1(i,t) * tVatM2(n,t) * (sign(tVat.l(i,n,t-1)) * (tMall(t)-1)+1);

* --- (141) Agent specific sales taxes

  e_tPaf(i,aaIn(aa),ts(t)) $ ((tPaf.range(i,aa,t) ne 0) $ sam(i,aa)) ..
   tPaf(i,aa,t) =e= (tPaf0(i,aa,t) + (tPafD0(t) + tPafD1(i,t) + tPafD2(aa,t)))
                   * tPafM0(t) * tPafM1(i,t) * tPafM2(aa,t) * (sign(tPaf.l(i,aa,t-1)) * (tMall(t)-1)+1);

* --- (142) End-user Armington price (add agent specific sales taxes tPaf and emission taxes)
*     Note that emission taxes are not ad-valorem. p_phiEmi is the participation share
*     in emission taxation

  e_paf(iArm(i),aaIn(aa),ts(t)) $ sam(i,aa)  ..
   paf(i,aa,t)s(paf,'i,aa,t-1')
    =e= [(1 + tPaf(i,aa,t)) * pa(i,t)
                       +   {sum(emiCommFlag(em,emSrc(i),aa),
                                     tEmi(em,aa,t) * emical(t)* chiEmi(em,emSrc,aa,t)
                                   * p_rhoEmi(em,emSrc,aa) * p_phiEmi(em,emSrc,aa))} $ hasEmiT ]s(paf,'i,aa,t-1');
* ----------------------------------------------------------------------------
*
*  Domestic sales and exports
*
* ----------------------------------------------------------------------------


* --- (143) + (145) Domestic sales of domestic goods (standard CET or additive one)
*     Distributes a part of total output xs minus stock changes delst to the domestic market

  e_xds(idom(i),ts(t))..
   0 =e= [(xd(i,t) - p_gd(i,t)*(xs(i,t) - delst(i,t))* p_gammaXd(i,t)**([-1-p_omegaX(i)] $ (not addCest(i)) + p_omegaX(i) $ addCest(i))
                  *(pd(i,t)/(psInd(i,t) $ addCest(i) + ps(i,t) $ (not addCest(i))))**p_omegax(i))s(xd,'i,t-1')
          ]  $ (p_omegaX(i) ne inf)
*         --- perfectly mobile case: sales prices in domestic market equal to unit cost of production

       +  [(p_gammaXd(i,t)*pd(i,t) - ps(i,t))s(pd,'i,t-1')
          ] $ (p_omegaX(i) eq inf);

* --- (144) + (146) Supply of export goods (standard CET or additive one)
*     Distributes a part of total output xs minus stock changes delst to the export market

  e_eds(iexp(i),ts(t))..
   0 =e= [(xe(i,t) - m_dmgAgg(dmgTrd,'i') * p_ge(i,t)*(xs(i,t) - delst(i,t)) * p_gammaXe(i,t)**([-1-p_omegaX(i)] $ (not addCest(i)) + p_omegaX(i) $ addCest(i))
                  *(pe(i,t)/(psInd(i,t) $ addCest(i) + ps(i,t) $ (not addCest(i))))**p_omegaX(i))s(xe,'i,t-1')
          ]$(p_omegaX(i) ne inf)
*         --- perfectly mobile case: sales prices in export market equal to unit cost of production
       +  [ (p_gammaXe(i,t)*pe(i,t) - ps(i,t))s(pe,'i,t-1')
          ] $ (p_omegaX(i) eq inf);

* --- (147) Composite price definition for ps / psInd from domestic sales and exports

  e_ps(iact(i),ts(t))..
   0 =e= [ {ps(i,t) $ (not addCest(i)) + psInd(i,t) $ addCest(i)
                   -  [   p_gd(i,t) *(pd(i,t)/p_gammaXd(i,t))**(1 $ (not addCest(i)) + p_omegaX(i))
                        + m_dmgAgg(dmgTrd,'i') * p_ge(i,t) *(pe(i,t)/p_gammaXe(i,t))**(1 $ (not addCest(i)) + p_omegaX(i)) ]**(1/(1 $ (not addCest(i)) + p_omegaX(i)))}
             s(ps,'i,t-1')
         ] $ (p_omegaX(i) ne inf)

       + [ (xs(i,t) - (xd(i,t) + xe(i,t) $ iExp(i) + delst(i,t)))s(xs,'i,t-1')
         ] $ (p_omegaX(i) eq inf);

* --- (148) Exhaustion of domestic sales and export revenues, defines ps in case of additive CET

  e_psIND(i,ts(t)) $ (idom(i) and iexp(i) $ addCest(i))..
   ps(i,t)s(ps,'i,t-1')
      =e= [   p_gd(i,t) * p_gammaXd(i,t)**p_omegaX(i)*pd(i,t)**(p_omegaX(i)+1)
            + p_ge(i,t) * p_gammaXe(i,t)**p_omegaX(i)*pe(i,t)**(p_omegaX(i)+1) ] * (1/psIND(i,t))**(p_omegaX(i))s(ps,'i,t-1');

* --- (149) Definition of export prices pe from given world market prices
*     ad-valorem taxes/subsidies tExp, plus export margines. Price received for exports pwe
*     converted to local currency based on exchange rate er

  e_pe(iExp(i),ts(t)) ..
   p_chiPE(i)*pe(i,t)*(1 + tExp(i,t)) + pmarg(i,t)*tmg(i,"x",t) =e= er(t)*p_chiPWE(i)*pwe(i,t);

* --- (150)) Determination of price received for export in foreign currency
*     Either equal to world market if export demand is fully elastic (p_etaE == inf)
*     for driven by is-elastic schedule

  e_pwe(iExp(i),ts(t)) ..
   0 =e=    (pwe(i,t) - p_gammaPWE(i,t)*pwerow(i,t)) $ (p_etaE(i) eq inf)
         +  ([xe(i,t)*pwe(i,t)**p_etaE(i) - p_chiE(i,t)*(p_gammaPWE(i,t)**p_etaE(i))*pwerow(i,t)**p_etaE(i)]s(xe,'i,t-1'))$ (p_etaE(i) ne inf);

* --- (151) World price of imports (either equal to world market price for imports
*     (Fully elastic, p_omegaM == inf) or driven by iso-elastic schedule

  e_pwm(iImp(i),ts(t)) ..
   0 =e=    (pwm(i,t) - pwmrow(i,t)) $ (p_omegaM(i) eq inf)
         +  ([(xm(i,t) +mdelst(i,t))*pwmROW(i,t)**p_omegaM(i) - p_chiM(i,t)*pwm(i,t)**p_omegaM(i)]s(xm,'i,t-1') ) $ (p_omegaM(i) ne inf);

* ----------------------------------------------------------------------------
*
*  Land and natural resource market equilibrium
*
* ----------------------------------------------------------------------------

* --- (152) this macro substitutes out the pfmsup for individual activities and ensures that the old and new
*     vintage face the same prices for factors (capital is differently treated)
*
  $$macro m_pfmsup(fmAll,actFmp,instTot,vf,t) [   pfmsup(fmAll,actFmp,instTot,vf,t) $ (not fma(fmAll)) \
                                             + sum((sameas(fmall,a),actfp) $ sameas(actFmp,actFp),pxfac_pretax(actFp,a,old,t)) $ fma(fmAll)]


* --- (153) Factor market equilibrium at activity level: factor demand added over vintages equal to supply of the factor to this activity

  e_xfacFp(flagXFac(actFp,a,old),ts(t)) $ sum(fmNestFmp(fma,fmp) $ (sameas(a,fma) $ sameas(actFp,fmp)),1) ..
   sum(vp $ flagXFac(actFp,a,vp),xfac(actFp,a,vp,t))
    %s1%    /max(%minScale%,sum(vp $ flagXFac(actFp,a,vp),xfac.l(actFp,a,vp,t-1)*%scaleMult%))
       =E= sum((sameas(a,fma),vf,fmp) $ (sameas(actFp,fmp) $ xfmSup.l(fma,fmp,"suminst",vf,t0)),xfmSup(fma,fmp,"suminst",vf,t)
*
*                     --- this term capture intensive margin change (labor hour per worker, changes in capital utilization rate
*                         at the level of individual activities in short run mode (= unexpected shocks)
*
              * (1  + p_epsFmpIntens(actFp,a) * (pxfac_pretax(actFp,a,old,t)/pxfac_pretax.scale(actFp,a,old,t)-1) $ isShortRun ) )
    %s1%                                                 /max(%minScale%,sum(vp $ flagXFac(actFp,a,vp),xfac.l(actFp,a,vp,t-1)*%scaleMult%))
  ;

* --- (154)-(159) Factor supply equation. ActFmp: active endowments (types of labor, capital, land, natural resources)
*     fmAll: set of factor supply nests, actFmp,actFmp depicts economy wide supply, fma depicts supply to single activities

  e_xfmSup(fmNestFmp(fmAll,actFmp),instTot,vf,ts(t)) $ (xfmSup.l(fmAll,actFmp,instTot,vf,t0)
      $$ifi "%shortRunShock%"=="on" and ( (xfmSup.range(fmAll,actFmp,instTot,vf,t) ne 0) or (not isShortRun))
                                    $ ((not ((not sum((inst,sameas(actFp,actFmp)),p_chiFac(inst,actFp,t))) and sameas(instTot,"suminst"))) or
                                        (sum((inst,sameas(actFp,actFmp)),p_chiFac(inst,actFp,t)) and sameas(instTot,"suminst"))))  ..

      0 =E= {[xfmSup(fmAll,actFmp,instTot,vf,t)
*
*           --- (154) This the top parent (fm-top), i.e. total factor supply by institutions, driven by elasticity epsFmp
*               sluggish or fully mobile: p_omegaFmp(actFmp,vf) ne 0. Supply depends on relation between
*               factor price and price index of GDP. atFmSup can be endogenous variable which shifts supply
*               at unchanged factor prices.
*
            - { atFmSup(fmAll,instTot,vf,t) * sum(sameas(actfmp,actFp),m_dmgAgg(dmgSup,actFp))
              * (pfmSup(fmAll,actFmp,instTot,vf,t)/pgdpmp(t))**p_epsFmp(actFmp,vf) }
                          $ (sameAs(fmAll,actFmp) $ (p_omegaFmp(actFmp,vf) ne 0))

*           --- (155) immobile case (p_omegaFmp(actFmp,vf) eq 0), activity specific factor supply elasticity epsFmA applied
*               to a share p_gammaFmp of the total stock atFmSup
*               (with special response under falling prices depicted by a sigmoid function where elasticity is much higher.

             - {(sum(sameas(actFmp,actFp)
                 %s1% $ p_gammaFmp(fmAll,actFmp,instTot,vf,t)
              ,
                     m_dmgAgg(dmgSup,actFp))*atFmSup(actFmp,instTot,vf,t)*p_gammaFmp(fmAll,actFmp,instTot,vf,t))
                   * ncpVuPow((sigmoid((m_pfmSup(fmAll,actFmp,instTot,vf,t)/pgdpmp(t)-0.75)*25)-1.E-4)*10,
                                       (m_pfmSup(fmAll,actFmp,instTot,vf,t)/pgdpmp(t))**p_epsFmA(fmAll,actFmp,vf),1.E-7)
               }                     $ (p_omegaFmp(actFmp,vf) eq 0)
*
*           --- (156) Immobile factors such as natural resources ( p_omegaFmp(actFmp,vf) eq 0):
*               The economy wide use of factor actFmp is defined by adding up the supply over the activities fma.
*               The total factor stock in use is actFmp,actFmp
*
              - sum(fmNesting(fma,actFmp,actFmp) $ fmNestFmp(fma,actFmp),xfmSup(fma,actFmp,instTot,vf,t))
                  $ ((p_omegaFmp(actFmp,vf) eq 0) $ sameas(fmAll,actFmp))
*
*           --- (157) Case where nest fmAll is a children of some other nest fmPar: supply is a price dependent share of the parent
*               this capture the case where fmAll is a single production activity
*
            -  sum(fmNesting(fmAll,fmPar,actFmp)
                 %s1% $ p_gammaFmp(fmAll,actFmp,instTot,vf,t)
            ,
*                                       --- share of child nest in total supply of factot nest
                                        p_gammaFmp(fmAll,actFmp,instTot,vf,t)*xfmSup(fmPar,actFmp,instTot,vf,t)
*
*                                       --- times price relation between child and parent. In case of volume preserving
*                                           CET, the dual price index pmfSupD is used
*
                                            * (m_pfmSup(fmAll,actFmp,instTot,vf,t)
                                             /(   pfmSupD(fmPar,actFmp,instTot,vf,t) $ isAdditiveCet(fmPar)
                                                + pfmSup(fmPar,actFmp,instTot,vf,t)  $ (not isAdditiveCet(fmPar))))**p_omegaFmp(fmPar,vf))
                  $ ((not sameAs(fmAll,actFmp)) $ (p_omegaFmp(actFmp,vf) ne 0))


            ]s(xfmSup,'fmAll,actFmp,instTot,vf,t-1')
            } $ (sum(fmNesting(fmAll,fmPar,actFmp),p_omegaFmp(fmPar,vf) ne inf) or (sameas(fmAll,actFmp) and (not p_stickyWage(fmAll))))
*
*        --- (158) sub-nest fmAll belongs to parent fmPar with infinite mobile (p_omegaFmp(fmPar,vf) eq in):
*            child has same price as parent, pfmSup(fmAll,... == pfmSup(fmPar, ...
*
         +  { ([m_pfmSup(fmAll,actFmp,instTot,vf,t)
               - sum(fmNesting(fmAll,fmPar,actFmp) $ p_gammaFmp(fmAll,actFmp,instTot,vf,t),
                     pfmSup(fmPar,actFmp,instTot,vf,t)
                    *pfmSup.l(fmAll,actFmp,instTot,vf,t0)/pfmSup.l(fmPar,actFmp,instTot,vf,t0))] $ pfmSup.l(fmAll,actFmp,instTot,vf,t0)
              )s(pfmSup,'fmAll,actFmp,instTot,vf,t-1')
             } $ (sum(fmNesting(fmAll,fmPar,actFmp),p_omegaFmp(fmPar,vf) eq inf) and (not sameas(fmAll,actFmp)))

*
*         --- (159) sticky wage mechanism, the price difference between t and t-1 is function of the utilization rate
*             (xfmSup over atFmSup) and the wage normlized with the price index of GTAP exponent an elasticity
*
          +  { (pfmSup(fmAll,actFmp,instTot,vf,t)
                - pfmSup(fmAll,actFmp,instTot,vf,t-1)   $ tNext(t-1)
                - pfmSup.l(fmAll,actFmp,instTot,vf,t-1) $ (not tNext(t-1))
                * (1 + p_stickyWage(fmAll) * (xfmSup(fmAll,actFmp,instTot,vf,t)
                         /[ atFmSup(fmAll,instTot,vf,t) * sum(sameas(actfmp,actFp),m_dmgAgg(dmgSup,actFp))
                             * (pfmSup(fmAll,actFmp,instTot,vf,t)/pgdpmp(t))**p_epsFmp(actFmp,vf)]-1))
               )s(pfmSup,'fmAll,actFmp,instTot,vf,t-1')
             } $ (sameAs(fmAll,actFmp) $ (p_omegaFmp(actFmp,vf) ne 0) $ p_stickyWage(fmAll));

* --- (156) of total supply in case of inifinite transformation, (160) parent price in case of immobile
*     markets, (161) dual price aggregator for sluggish case

  e_pfmSup(fmNestFmp(fmPar,actFmp),instTot,vf,ts(t)) $ (   pfmSup.l(fmPar,actFmp,instTot,vf,t0)
                   $ ((not sum((inst,sameas(actFp,actFmp)),p_chiFac(inst,actFp,t))) or sameas(instTot,"suminst")))  ..

   0 =E=
*
*              --- (156) Nest with infinite transformation (p_omegaFmp(fmPar,vf) eq inf) :
*                  physical exhaustion of parent supply  xfmSup(fmPar,actFmp,instTot,vf,t) by the supply to its
*                  childs xfmSup(fmAll,actFmp,instTot,vf,t)
*
          {
              [  xfmSup(fmPar,actFmp,instTot,vf,t)
               - sum(fmNesting(fmAll,fmPar,actFmp) $ (xfmSup.l(fmAll,actFmp,instTot,vf,t0) $ fmNestFmp(fmAll,actFmp)),
                                                      xfmSup(fmAll,actFmp,instTot,vf,t))
              ]s(xfmSup,'fmPar,actFmp,instTot,vf,t-1')
          } $ ((p_omegaFmp(fmPar,vf) eq inf))
*
*             --- (160) average parent price for immobile nests (p_omegaFmp(fmPar,vf) eq 0):
*                 value of childs (pfmsup * xfmSup) divived by parent supply xfmSup(fmPar,actFmp,instTot,vf,t)
*
         +  { [
                  pfmSup(fmPar,actFmp,instTot,vf,t)

               - sum(fmNesting(fmAll,fmPar,actFmp) $ (fmNestFmp(fmAll,actFmp) $ p_gammaFmp(fmAll,actFmp,instTot,vf,t)),
                     m_pfmSup(fmAll,actFmp,instTot,vf,t)*xfmSup(fmAll,actFmp,instTot,vf,t)
                     )/xfmSup(fmPar,actFmp,instTot,vf,t)
               ]s(pfmSup,'fmPar,actFmp,instTot,vf,t-1')
             } $ ((p_omegaFmp(fmPar,vf) eq 0))
*
*           --- (161)+(162) Finite but not zero transformation elasticity
*               dual CET price aggregator (different for additive and non-additive case)
*
         +  { [   pfmSupD(fmPar,actFmp,instTot,vf,t) $ isAdditiveCet(fmPar)
                + pfmSup(fmPar,actFmp,instTot,vf,t) $ (not isAdditiveCet(fmPar))

                - sum(fmNesting(fmAll,fmPar,actFmp) $ p_gammaFmp(fmAll,actFmp,instTot,vf,t),
                          p_gammaFmp(fmAll,actFmp,instTot,vf,t)
                        * m_pfmSup(fmAll,actFmp,instTot,vf,t)**(1 $ (not isAdditiveCet(fmPar)) + p_omegaFmp(fmPar,vf))
                  )**(1/(1 $ (not isAdditiveCet(fmPar)) + p_omegaFmp(fmPar,vf)))
               ]s(pfmSup,'fmPar,actFmp,instTot,vf,t-1')

             } $ ((p_omegaFmp(fmPar,vf) ne inf) and (p_omegaFmp(fmPar,vf) ne 0));

* --- (163) Definition of exhaustion price pfmSup for additive CET, based on dual formulation
*
  e_pfmSupD(fmNestFmp(fmPar,actFmp),instTot,vf,ts(t)) $ (isAdditiveCet(fmPar) $ pfmSupD.l(fmPar,actFmp,instTot,vf,t0)
                            $ ((not sum((inst,sameas(actFp,actFmp)),p_chiFac(inst,actFp,t))) or sameas(instTot,"suminst")))  ..
   pfmSup(fmPar,actFmp,instTot,vf,t)s(pfmSup,'fmPar,actFmp,instTot,vf,t-1')
     =E= sum(fmNesting(fmAll,fmPar,actFmp) $ p_gammaFmp(fmAll,actFmp,instTot,vf,t),
                 p_gammaFmp(fmAll,actFmp,instTot,vf,t)*m_pfmSup(fmAll,actFmp,instTot,vf,t)**(p_omegaFmp(fmPar,vf)+1))
               * pfmSupD(fmPar,actFmp,instTot,vf,t)**(-p_omegaFmp(fmPar,vf))s(pfmSup,'fmPar,actFmp,instTot,vf,t-1');

* --- (164) Factor supply to a factor nest (which can be a single production activity) summed up over institutions supplying
*     this factor.

  e_xfmsupTot(fmNestFmp(fmAll,actFmp),"sumInst",vf,ts(t)) $ (xfmSup.l(fmAll,actFmp,"suminst",vf,t0)
                                                             and (not sum((inst,sameas(actFp,actFmp)),p_chiFac(inst,actFp,t)))) ..
   xfmSup(fmAll,actFmp,"suminst",vf,t)s(xfmSup,'fmAll,actFmp,"sumInst",vf,t-1')
      =E= sum(inst $ xfmSup.l(fmAll,actfmp,inst,vf,t0),xfmSup(fmAll,actFmp,inst,vf,t))s(xfmSup,'fmAll,actFmp,"sumInst",vf,t-1');

* --- (165) Correction factor from non-linear CET aggregation to align total factor income with xfmSup*pfmsup

  e_corrFac(actfp,ts(t)) $ ((corrFac.range(actFp,t) ne 0) $ sum((sameas(actFp,actfmp),inst,vf), xfmSup.l(actFmp,actFmp,inst,vf,t0))) ..
   corrFac(actFp,t)
    =e= facty(actFp,t)
         /(  sum((sameas(actFp,actfmp),inst,vf) $ (xfmSup.l(actFmp,actFmp,inst,vf,t0) $ (not p_chiFac(inst,actFp,t))),
                       xfmSupUpdateFac(actfmp,inst,t)*xfmSup(actFmp,actFmp,inst,vf,t)*pfmSup(actFmp,actFmp,inst,vf,t)));

* --- (166) if the vintage mechanism is off, the flex factor equation also cover capital. Total supply equal to kaps, shift
*     constants of factor supply equation

  e_atCap(fmp,instTot,oldf,ts(t)) $ ((not ifVint) $ sum(sameas(fmp,cap) $ xfmSup.l(fmp,fmp,instTot,oldf,t0), atFmSup.l(fmp,instTot,oldf,t))) ..
   kaps(t)/kaps.l(t0)
     =E= sum(sameas(fmp,cap) $ xfmSup.l(fmp,fmp,instTot,oldf,t0),
            xfmSupUpdateFac(fmp,instTot,t)*atFmSup(fmp,instTot,oldf,t)/atFmSup.l(fmp,instTot,oldf,t0));

* --- (166) if the vintage mechanism is on: supply to households is equal to changes in kaps (this is the same
*           as the equation above, the difference in the paired variable

  e_atCapVint(fmp,fmp,instTot,oldf,ts(t)) $ (ifVint $ sum(sameas(fmp,cap),xfmSup.l(fmp,fmp,instTot,oldf,t-1))) ..
   kaps(t)/kaps.l(t0)
    =E= sum(sameas(fmp,cap),
              xfmSupUpdateFac(fmp,instTot,t)*xfmSup(fmp,fmp,instTot,oldf,t)/xfmSup.l(fmp,fmp,instTot,oldf,t0));

* --- (167) the average rent if equal to the uniform price of capital

  e_trent(ts(t)) $ (not ifVint) ..
   trent(t)s(trent,'t-1')
    =E= sum(sameas(cap,fmp),pFmSup(fmp,fmp,"sumInst",oldf,t))s(trent,'t-1');


* --- (168) Exhaustion price of capital

  e_trentInst(fmp,fmp,"sumInst",oldf,ts(t)) $ ((not ifVint) $ sum(sameas(cap,fmp),1) $ xFmSup.l(fmp,fmp,"sumInst",oldf,t0)
                                               $ (not sum((inst,cap),p_chiFac(inst,cap,t)))
                                            ) ..
   pFmSup(fmp,fmp,"sumInst",oldf,t)s(pfmSup,'fmp,fmp,"sumInst",oldf,t-1')
    =E= [ sum(inst $ xFmSup.l(fmp,fmp,inst,oldf,t0),pFmSup(fmp,fmp,inst,oldf,t)*xFmSup(fmp,fmp,inst,oldf,t))
          /xfmSup(fmp,fmp,"suminst",oldf,t)]s(pfmSup,'fmp,fmp,"sumInst",oldf,t-1');

* ----------------------------------------------------------------------------
*
*     Capital market equilibrium with vintage mechanism
*
* ----------------------------------------------------------------------------


* --- (169), Aggregate capital market equilibrium
*            determines 'new' rent, or trent

  e_trentKaps(ts(t)) $ ifVint..
*
*  --- normalized capital stock kaps must be exhausted by demand for capital,
*      summed over activities and their vintages
*
   kaps(t)s(kaps,'t-1') =e= sum(flagXfac(cap,a,v), xfac(cap,a,v,t))s(kaps,'t-1');

* --- (170) Calculate the old capital (this is relevant for the forward looking case where t-1 might be endogenous)

  e_oldCap(a,ts(t)) $ (sum(cap,flagXfac(cap,a,old)) $ ifVint $ (oldCap.range(a,t))) ..
   oldCap(a,t)s(oldCap,'a,t-1')
    =E= sum(flagXFac(cap,a,vp),   xfac.l(cap,a,vp,t-1) $ (not tNext(t-1))
                                + xfac(cap,a,vp,t-1)   $      tNext(t-1))*(1-deprA(a,t)*m_dmgAgg(dmgDepr,'a')
        )s(oldCap,'a,t-1');

* --- (171) This determines the capital use in the old vintage as the share of old capital in use

  e_xpOld(fptop,aact(a),old,ts(t)) $ (ifVint and (not ifEnTech) and sum(flagXFac(cap,aact,old),1) ) ..

   sum(cap,xfac(cap,a,old,t))
     %s1% /max(%minScale%,sum(cap,xfac.l(cap,a,old,t-1)*%scaleMult%))
    =e= oldCap(a,t) * rrat(a,t)
     %s1% /max(%minScale%,sum(cap,xfac.l(cap,a,old,t-1)*%scaleMult%))
   ;

* --- (172) Set the vintage rental rates
*     ( if rrat = 1, both vintages are non-zero and the old and new vintage have the returns
*                    equal to the economic-wide one.
*          rrat < 1, the new vintate is zero, and the old capital use rate is a function of returns to capital in the old vintage

  e_pk(flagXfac(cap,a,old),ts(t)) $ ifVint ..
    rrat(a,t) =e= (pxfac_preTax(cap,a,old,t) / trent(t))**p_InvElas(a);

* --- (173) rrat = min(1,sum(v,xfac(a,fpTop,v,t))/xfac(a,ftpTop,old,t), if both vintages are active,
*           full capital use

  e_rrat1(a,ts(t)) $ (sum(cap,flagXfac(cap,a,old)) $ ifVint) ..
   rrat1(a,t) =E= sum((cap,v),xFac(cap,a,v,t)+5.E-11)/(oldCap(a,t)+1.E-10);

* --- (174) Determine the mulitiplier to old capital use when new and old output would be based on the capital output
*     of the old vintage. The multiplier exceeds 1 if there is new vintage output

  e_rrat(a,ts(t)) $ (sum(cap,flagXfac(cap,a,old)) $ ifVint) ..

   $$iftheni.cns not "%SolveType%"=="MCP"
*     --- use smooth min function if solved as CNS
      rrat(a,t) =e= ncpVuPow(rrat1(a,t),1,1.E-7);
   $$else.cns
*     --- use MCP formulation if solved as MCP. Requires an upper bound on rrat, see inical.inc
      rrat(a,t) =L= sum((cap,v),xFac(cap,a,v,t))/oldCap(a,t);
   $$endif.cns

*  --- (175) This essentially determines the new vintage output with the capital vintages
*            or sets xp equal output of the old (and only vintage) without capital vintages

  e_xpNew(fpTop,aact(a),v,ts(t)) $ (( (new(v) $ sum(cap $ flagXFac(cap,aact,old),1) $ ifVint) or (old(v) $ (not ifVint)) ) $ (not ifEnTech) ) ..
*
*   --- total output of activity a is the sum of the output (= top nest of production function)
*       of old and new vintage. Note that due to differences in unit cost and selling prices,
*       the relation between the variable at the benchmark t0 is used for normalization
*
    xp(a,t)s(xp,'a,t-1')
      =e= sum(flagxFac(fpTop,a,vp), xfac(fptop,a,vp,t)/xfac.l(fptop,a,old,t0)*xp.l(a,t0))s(xp,'a,t-1');

  e_xpOldNoCap(fpTop,aact(a),v,ts(t)) $ (( (old(v) $ (not sum(cap $ flagXFac(cap,aact,old),1)) $ ifVint) $ (not ifEnTech) )) ..
*
*   --- total output of activity a is the sum of the output (= top nest of production function)
*       of old and new vintage. Note that due to differences in unit cost and selling prices,
*       the relation between the variable at the benchmark t0 is used for normalization
*
    xp(a,t)s(xp,'a,t-1')
      =e=  sum(flagxFac(fpTop,a,vp), xfac(fptop,a,old,t)/xfac.l(fptop,a,old,t0)*xp.l(a,t0))s(xp,'a,t-1');

* --- (176) Average capital rent: factor income from capital divided by total capital demand

  e_trentAvg(ts(t)) ..
   trentAvg(t) =e= sum(cap, facty(cap,t))/kaps(t);
*
* --- (177)
*
  e_pfmSupCap(fmp,fmp,instTot,oldf,ts(t)) $ (ifVint $ sum(sameas(cap,fmp),1) $ xfmsup.l(fmp,fmp,instTot,oldf,t0))  ..
   trentAvg(t)s(trent,'t-1')
    =E= sum(sameas(cap,fmp),pfmSup(fmp,fmp,instTot,oldf,t))s(trent,'t-1');


* --- (178) For individual activities, capital prices are idential across different suppliers inst (such as multiple households)
*           such that physical aggregation hold
*     fma = activities as demanders in factor markets, fmp = endowments, vf = vintages for endowments

  e_xfacFpCap(fma,fmp,"suminst",vf,ts(t)) $ ( ifVint $ sum((cap,sameas(a,fma),vp) $ sameas(cap,fmp),flagXFac(cap,a,vp))
                                               $ xfmSup.l(fma,fmp,"suminst",vf,t0)) ..
     sum((cap,sameas(a,fma),vp) $ flagXFac(cap,a,vp),xfac(cap,a,vp,t))s(xfmSup,'fma,fmp,"sumInst",vf,t-1')
       =E= xfmSup(fma,fmp,"suminst",vf,t)s(xfmSup,'fma,fmp,"sumInst",vf,t-1');

* ----------------------------------------------------------------------------
*
*     Endogenous technical progress
*
* ----------------------------------------------------------------------------

* --- (179) Number of new processes #ENDOTECH

  e_einn(aact(a),ts(t)) $ (ifEntech and p_markUp(a,t)) ..
   einn(a,t)s(einn,'a,t-1')
    =e= [ (    p_inn(a,t0)*xRD(a,t)**(p_phiRD(a,t))) $   P_phiRD(a,t)
             + p_inn(a,t0)                           $ (not p_phiRD(a,t))]s(einn,'a,t-1');

* --- (180) RD intensity #ENDOTECH

  e_xRD(aact(a),ts(t)) $ (ifEnTech $ p_markUp(a,t)) ..

      (xRD(a,t)**(1-p_phiRD(a,t)) * pfdrd(t) * p_xRD0(a))
     %s1%  /max(%minScale%,(xRd.l(a,t-1)**(1-p_phiRD(a,t)) * pfdrd.l(t-1) * p_xRD0(a))*%scaleMult%)
    =e=
*       --- case where endo tech is active (p_phiRd is non-zero)
          [{(p_markup(a,t) * sum(fptop, xfac(fptop,a,new,t) * pxfac(fptop,a,new,t)))
            /([(1+p_interestRD(t))/(1+p_egrowth(a,t))-(1-(1+2*p_rhoEn)*einn(a,t))]
               *frontBGP(a,t)*p_frontier(a,t)*p_inn(a,t0)/p_inn(a,t))} $ (p_inn(a,t) and p_phiRD(a,t))

*        --- case where no endo tech is active for an activity
          + {pfdrd(t) * p_xRD0(a)} $((not p_inn(a,t)) or (not p_phiRD(a,t)))
         ]
     %s1%  /max(%minScale%,(xRd.l(a,t-1)**(1-p_phiRD(a,t)) * pfdrd.l(t-1) * p_xRD0(a))*%scaleMult%)
   ;

* --- (181) Total R&D inputs #ENDOTECH

  e_xfdRD(is,ts(t)) $ (p_chiFdRD(is,t) and ifEnTech) ..
   xfdRD(is,t)s(xfdRd,'is,t-1')
    =e= p_chiFdRD(is,t) * sum(aact(a) $ p_markUp(a,t),(xRD(a,t)*p_xRD0(a))*frontBGP(a,t)*p_frontier(a,t))s(xfdRd,'is,t-1');

* --- (182) Total R&D expenditure #ENDOTECH

  e_yfdRD(ts(t)) $ ifEnTech ..
   yfdRD(t)s(yfdRd,'t-1') =e= sum(f $ p_chiFdRD(f,t), xfdRD(f,t)*pf(f,t))s(yfdRd,'t-1');

* --- (183) price index of R&D input #ENDOTECH

  e_pfdRD(ts(t)) $ ifEntech  ..
   pfdRD(t) =e= yfdRD(t) /sum(f $ p_chiFdRD(f,t), xfdRD(f,t));

* --- (184) Distribution of total output to the two vintages under the endotech extension
*
  e_xpEnTech(flagxFac(fptop,a,v),ts(t)) $ (aact(a) and ifEnTech)..
    xfac(fptop,a,v,t)*p_lambdaEnCr(a,v,t)s(xfac,'fptop,a,v,t-1')
      =e= (  [
*               --- share parameters plus shifter impact
                (p_inn(a,t)*(1+p_rhoEn)) $ new(v)
              + (1-p_obsol(a,t)        ) $ old(v)]

*               --- scaling between xfac and xp from benchmark (attention: here both new and old are set!)
                * sum(vp,xfac.l(fptop,a,vp,t0))/xp.l(a,t0)
*               --- account for change in variety and correct for variety change between t0 and t0+1
                * p_variety(a,t-1)/(1-p_rhoEn*p_inn(a,t0))
*               --- distribute total xp to vintages based on prices
                * xp(a,t)
                 * ( (px(a,t)/px.l(a,t0)*p_lambdaEnCr(a,v,t))
                            /( ((1+p_markup(a,t)) *pxfac(fptop,a,v,t))
                              /((1+p_markup(a,t0))*pxfac.l(fptop,a,v,t0))))**p_sigmaXpvt
                              )s(xfac,'fptop,a,v,t-1');

*
* Note: Equations (185)-(190) update parameters in endotech equations, they are found
*       in inc/iterloop.inc
*

* ----------------------------------------------------------------------------
*
*  Factor prices
*
* ----------------------------------------------------------------------------

* --- (191) Factor prices without factor tax

  e_pxfac_preTax(flagXFac(actFp,a,old),ts(t))  ..
   pxfac(actFp,a,old,t)s(pxFac,'actFp,a,old,t-1')
    =e=
      (
*       --- ad-valorem tax on top of market prices of factor
         pxfac_preTax(actFp,a,old,t) * (1 + tFac(a,actFp,t))
*       --- additive emission taxes, p_phiEmiFac is the share of emission taxes
           + sum(em $ p_rhoEmiFac(em,actFp,a),
              tEmi(em,a,t)*emical(t)*chiEmiFac(em,actFp,a,t)*p_rhoEmiFac(em,actFp,a)*p_phiEmiFac(em,actFp,a)) $ hasEmit
      )s(pxFac,'actFp,a,old,t-1');

* --- (192) Real factor tax

  e_rtFac(a,actFp,ts(t)) $ (rtFac.range(a,actFp,ts) eq 0) ..
   rtFac(a,actFp,t)*pgdpmp(t) =e= tFac(a,actFp,t);

* ----------------------------------------------------------------------------
*
*     Foreign  capital account
*
* ----------------------------------------------------------------------------
*
* --- (193) Foreign savings relative to GDP
*     fixes the relation savfGDP if savfExoRel is used for the current account closure

  e_savfGDP(ts(t)) $ (capAccClosure ne savfExoRel) ..
   savfGDP(t) =e= (savF(t)*er(t) )/gdpmp(t);

* --- (194),(195),(196) Capital account closure

  $$macro m_logasc(y,t) ((p_grKMax(t) - y(t))/(y(t) - p_grKMin(t)))

  e_savf(ts(t)) $ ((capAccClosure ne savfEndoERFix) $ (botGdp.range(t) ne 0)) ..
   0 =e=

*     --- (194) savf is fixed as a share of nominal GDP
        [(savF(t) - savfGDP(t)*gdpmp(t)/er(t))
         %s1% /max(%minScale%,(abs(totImp.l(t-1)+totExp.l(t-1))*0.5)**%scaleMult%)
         ] $ (capAccClosure eq savfExoRel)
*
*     --- (195) global and country specific expected return to capital are equal
*
      +  (rorg(t) - rore(t))                                                        $ (capAccClosure eq savfEndoGTAP)

*     --- (196) depends on debt-to-GDP relative to 50% threshhold and real exchange rate
*         well as relative rate of return (domestic/exogenous global or target ROR)
      +  (savF(t)/gdpmp(t)
                   - p_gammaSf2(t)*[ exp(-p_omegaSavf(t)*er(t)/pgdpmp(t)*debtGDP(t))
                                     *depr(t)*kstock(t)*exp(p_rhosavf(t)*(rore(t)-rorg(t)))])  $ (capAccClosure eq savfEndoGTEM)

*     --- (197) Flexible capital account based on user-defined determinants of foreign capital inflow
      +  (savF(t)/gdpmp(t) - p_gammaSf1(t) * [((1+trent(t))/(1+p_retrow(t))) ** eta1a(t)]
                                            *[((1+ggdppc(t))/(1+p_ggdppcrow(t)))** eta2a(t)])  $ (capAccClosure eq savfEndoResRegSavings)

*     --- (198) the equilibrium capital growth from which is derived private investment;
*         supply of investible funds determined by an inverse logistic function
*         smaller the chigrK closer the grK to grKTrend
      +  (grk(t) - (p_grKMax(t)*exp(chigrK(t)*devRoR(t)) + p_grKMin(t) * m_logasc(p_grKTrend,t))
                   /  (exp(chigrK(t)*devRoR(t)) + m_logasc(p_grKTrend,t)))                 $ (capAccClosure eq savfEndoUSAGE)

   ;


* --- (199) Change in rate of return

  e_devRoR(ts(t)) $ (capAccClosure eq savfEndoUSAGE)..
   devRoR(t) =e= rore(t) - rord(t) - rorg(t) ;

* --- (200) Anticipated growth of the capital stock

  e_grK(ts(t)) ..
    prodInv(t)s(prodInv,'t-1')
     =e= kstock(t)*(grK(t) + depr(t))s(prodInv,'t-1');

* --- (201) Walras law

  e_walras(ts(t)) ..
   (walras(t) - savf(t)*er(t))
     %s1%   /max(%minScale%,[abs(bot.l(t-1))+abs(savf.l(t-1)*er.l(t-1))]*%scaleMult%)
    =e= [     bot(t)
            + sum((inst,mapInst(row,instp)),transfers(inst,instp,t)- transfers(instp,inst,t))
            - sum(instf, debtPay(instf,t))
            - sum((actFp,mapInst(row,inst)), facty(actFp,t)*p_chiFac(inst,actFp,t))
            - sum((sameas(actFp,actfmp),mapInst(row,inst),vf) $ xfmsup.l(actFmp,actFmp,inst,vf,t0),
                    xfmSupUpdateFac(actFmp,inst,t)*xfmSup(actFmp,actFmp,inst,vf,t)*pfmSup(actFmp,actFmp,inst,vf,t)*corrFac(actFp,t))
           ]
     %s1%   /max(%minScale%,[abs(bot.l(t-1))+abs(savf.l(t-1)*er.l(t-1))]*%scaleMult%)
  ;

* ----------------------------------------------------------------------------
*
*  Macro identities
*
* ----------------------------------------------------------------------------

* --- (202) Nominal aggregate stock changes

  e_tdelst(ts(t)) $ ifDelSt ..
   tdelst(t)s(tdelSt,'t-1')
    =e= sum(ist(i), p_chiPS(i)*ps(i,t)*delst(i,t) $ iDom(i) + er(t)*pwm(i,t) $ iImp(i) *(1+tImp(i,t))*mdelst(i,t))s(tdelSt,'t-1');

* --- (203) Real aggregate stock changes

  e_rtdelst(ts(t)) $ ifDelSt ..
   rtdelst(t)s(rtdelSt,'t-1')
    =e= sum(ist(i), p_chiPS(i)*ps.l(i,t0)*delst(i,t) + er.l(t0)*pwm.l(i,t0)$iImp(i)*(1+tImp(i,t))*mdelst(i,t))s(rtdelSt,'t-1');

* --- (204) Stock changes aggregate price index

*
* WB: This can give absolute nonsense results as the two variables above can even have a different sign.
*
  e_pdelst(ts(t)) $ (ifDelSt $ (1 eq 2)) ..
   pdelst(t)*rtdelst(t)s(tdelSt,'t-1') =e= tdelst(t)s(tdelSt,'t-1');

* --- (205) Nominal aggregate exports

  e_totexp(ts(t)) ..
   totexp(t)s(totExp,'t-1') =e= er(t) * sum(iexp(i), p_chiPWE(i)*pwe(i,t) * xe(i,t))s(totExp,'t-1');

* --- (206) Real aggregate exports

  e_rtotexp(ts(t)) ..
   rtotexp(t)s(rtotExp,'t-1') =e= er.l(t0) * sum(iexp(i), p_chiPWE(i)*pwe.l(i,t0) * xe(i,t))s(rtotExp,'t-1');

* --- (207) Aggregate export price index

  e_pexp(ts(t)) ..
   rtotexp(t)*pexp(t)s(totExp,'t-1') =e= totexp(t)s(totExp,'t-1');

* --- (208) Nominal aggregate imports

  e_totimp(ts(t)) ..
   totimp(t)s(totImp,'t-1') =e= er(t) * sum(iImp(i), pwm(i,t) * (xm(i,t) + mdelst(i,t)))s(totImp,'t-1');

* --- (209) Real aggregate imports

  e_rtotimp(ts(t)) ..
   rtotimp(t)s(rtotImp,'t-1') =e= er.l(t0) * sum(iImp(i), pwm.l(i,t0) * (xm(i,t) + mdelst(i,t)))s(rtotImp,'t-1');

* --- (210) Aggregate import price index

  e_pimp(ts(t)) ..
   rtotimp(t)*pimp(t)s(totImp,'t-1') =e= totimp(t)s(totImp,'t-1');

* --- (211) Nominal GDP at market price

  e_gdpmp(ts(t)) ..
   gdpmp(t)s(gdpmp,'t-1') =e= [sum(fd $ p_yf0(fd,t), yf(fd,t)) + tdelst(t) $ ifDelSt + totexp(t) - totimp(t)]s(gdpmp,'t-1');

* --- (212) Real GDP at market price

  e_rgdpmp(ts(t)) ..
   rgdpmp(t)s(rgdpmp,'t-1') =e= [sum(fd $ p_yf0(fd,t), xfd(fd,t)) + rtdelst(t) $ ifDelSt + rtotexp(t) - rtotimp(t)]s(rgdpmp,'t-1');

* --- (213) GDP at market price deflator

  e_pgdpmp(ts(t)) ..
   pgdpmp(t) =e= gdpmp(t)/rgdpmp(t);

* --- (214) Real per capita GDP

  e_rgdppc(ts(t)) ..
   rgdppc(t)*pop("ptotl",t)s(rgdpmp,'t-1') =e= rgdpmp(t)s(rgdpmp,'t-1');

* --- (215) Growth in real per capita GDP

  e_ggdppc(ts(t))..
   rgdppc(t)s(rgdppc,'t-1') =e= rgdppc.l(t-1)*(1 + ggdppc(t))s(rgdppc,'t-1');

* --- (216) Nominal GDP at factor cost

  e_gdpfc(ts(t)) ..
   gdpfc(t)s(gdpfc,'t-1')
    =e= sum(flagXFac(actFp,a,v),pxfac(actFp,a,old,t)*xfac(actFp,a,v,t))s(gdpfc,'t-1');

* --- (217) Real GDP at factor cost

  e_rgdpfc(ts(t)) ..
   rgdpfc(t)s(rgdpfc,'t-1') =e=  sum(flagXFac(actFp,a,v),pxfac.l(actFp,a,old,t0)*xfac(actFp,a,v,t))s(rgdpfc,'t-1');

* --- (218) GDP at factor cost deflator

  e_pgdpfc(ts(t)) ..
   pgdpfc(t) =e= gdpfc(t)/rgdpfc(t);

* --- (219) Real GDP shares (gov and inv), with a term which decrease government spending share if a debt target is exceeded

  e_rshrGDP(oa,ts(t)) ..
   rshrGDP(oa,t) =e= xfd(oa,t)/rgdpmp(t);

* --- (220) Nominal GDP shares

  e_nshrGDP(oa,ts(t)) $ p_yf0(oa,t) ..
   nshrGDP(oa,t) =e= yf(oa,t)/gdpmp(t);
*
* --- (221)
*
  e_fiscalRespDrv1(ts(t)) $ ifFiscalResp ..
   fiscalRespDrv1(t) =E= ncpVuPow(0,-{sigmoid([{debtGdp(t)-p_debtGdpTgtUp(t)}/p_debtGdpTgtUp(t)]*0.5)-0.5}*2,1.E-4);

  e_fiscalRespDrv(ts(t)) $ ifFiscalResp ..
   fiscalRespDrv(t) =E= fiscalRespDrv1(t)
*
*                    --- fiscal expansion in case that debtGdp undercuts a target lower debtToGdp ratio
*
                       -ncpVuPow(0,-{sigmoid([{p_debtGdpTgtLo(t)-debtGdp(t)}/p_debtGdpTgtLo(t)]*0.5)-0.5}*10,1.E-4) $ p_debtGdpTgtLo(t);

*
* --- (222) Government consumption and investment as base year shares on real GDP
*           plus impact of fiscal resp. driver
*
  e_rshrGDPGov(f,ts(t)) $ ((ginv(f) or gov(f)) $ (xfd.range(f,t) ne 0) $ (rshrGDP.range(f,t) ne 0) $ (nshrGDP.range(f,t) ne 0)) ..
   xfd(f,t)s(xfd,'f,t-1') =e= [    xfd.l(f,t0)/rgdpmp.l(t0)
                                + (p_relExpInc/100*fiscalRespDrv(t)) $ p_relExpInc $ ifFiscalResp]*rgdpmp(t)s(xfd,'f,t-1');

* --- (223) Fiscal revenues fixed to GDP, with a term which increase the share if a debt target is exceeded

  e_fiscalResp(ts(t)) $ (ifFiscalResp $ (p_relExpInc ne 100)) ..
   tmAll(t) =e= tmAll.l(t0) - fiscalRespDrv(t);
*
* --- (224) Balance of trade
*
  e_bot(ts(t)) ..
   bot(t)s(bot,'t-1') =e= [ totexp(t) - totImp(t) ]s(bot,'t-1');
*
* --- (225) Balance of trade over GDP
*
  e_botGDP(ts(t)) ..
   botGdp(t) =e= bot(t)/gdpMp(t);

* --- (226) Real GDP component shares: nipa elements

  e_GdpCompShr(nipa,ts(t))$ sum(mapn(fd,nipa),1) ..
   GdpCompShr(nipa,t) =e=  sum(mapn(fd,nipa), xfd(fd,t))/rgdpmp(t);

* --- (227) GDP component shares: Exports

  e_GdpCompShrX("expo",ts(t)) ..
   GdpCompShr("Expo",t) =e=  rtotexp(t)/rgdpmp(t);

* --- (228) GDP component shares: Imports

  e_GdpCompShrM("impo",ts(t)) ..
   GdpCompShr("Impo",t) =e= -rtotimp(t)/rgdpmp(t);

* --- (229) Consumer price index (HD: post-triangular; not targeted)

  e_cpi(ts(t)) ..
   cpi(t) =e= sum((i,h) $ sam(i,h), xa(i,h,t)*paf(i,h,t)) /  sum((i,h) $ sam(i,h), xa(i,h,t)*p_pafini(i,h));

* ----------------------------------------------------------------------------
*
*  Emissions module
*
* ----------------------------------------------------------------------------

* --- (230) Emissions linked to the use of commodities

  e_emiComm(emiCommFlag(em,emSrc,aa),ts(t))   ..
   emiComm(em,emSrc,aa,t)s(emiComm,'em,emSrc,aa,t-1')
    =e= emical(t)*chiEmi(em,emSrc,aa,t)*p_rhoEmi(em,emSrc,aa)*xa(emSrc,aa,t)s(emiComm,'em,emSrc,aa,t-1');

* --- (231) Emisssions linked to the use of factors of production (land, capital, etc.)

  e_emiFact(em,actFp,a,ts(t)) $ p_rhoEmiFac(em,actFp,a) ..
   emiFact(em,actFp,a,t)s(emiFact,'em,actFp,a,t-1')
    =e= emical(t)*chiEmiFac(em,actFp,a,t)*p_rhoEmiFac(em,actFp,a)*sum(flagxFac(actFp,a,v), xfac(actFp,a,v,t))s(emiFact,'em,actFp,a,t-1');

* --- (232) Emissions linked to the volume of production (e.g. municipal waste)

  e_emiProd(em,a,ts(t)) $ p_rhoEmiXP(em,a) ..
   emiXP(em,a,t)s(emiXP,'em,a,t-1') =e= emical(t)*chiEmiXP(em,a,t)*p_rhoEmiXP(em,a)*xp(a,t)s(emiXP,'em,a,t-1');

* --- (233) Emisssions linked to land use change (only for CO2, all other emissions will be zero due to zero RHS)

  e_emiLUCF(em,a,ts(t)) $ (sameas(em,"co2") $ sum(actLnd,-p_rhoEmiLnd(em,actLnd,a))) ..
   emiLUCF(em,a,t)s(emiLUCF,'em,a,t-1')
    =e= sum(actLnd,-p_rhoEmiLnd(em,actLnd,a) * sum(v,xfac(actLnd,a,v,t)-xfac.l(actLnd,a,v,t-1)))s(emiLUCF,'em,a,t-1');

* --- (234) Total emissions by each Armington agent (including production activities a)

  e_emiSecA(emiFlag(em,aa),ts(t)) $ card(emiFlag) ..
   emiSecA(em,aa,t)s(emiSeca,'em,aa,t-1')
    =e= [    sum(emiCommFlag(em,emSrc,aa), emiComm(em,emSrc,aa,t))
          +  sum(sameas(aa,a),
                              sum(actFp $ p_rhoEmiFac(em,actFp,a), emiFact(em,actFp,a,t))
                           +  emiXP(em,a,t) $ p_rhoEmiXP(em,a)
                           +  emiLUCF(em,a,t) $ (sum(actLnd,-p_rhoEmiLnd(em,actLnd,a)) $ sameas(em,"co2")))
       ]s(emiSeca,'em,aa,t-1');

* --- (235) Emission inventory: emissions are distribution by allocation shares p_ghgInvAlloc to inventories

  e_emiInv(ghgInv,em,ts(t)) $ card(emiFlag)  ..
   emiInv(ghgInv,em,t)s(emiInv,'ghgInv,em,t-1')
    =e=
        [      sum(emiCommFlag(em,emSrc,aa) $ p_ghgInvAlloc(ghgInv,em,emSrc,aa),
                                              p_ghgInvAlloc(ghgInv,em,emSrc,aa)    * emiComm(em,emSrc,aa,t))
             + sum((actFp,a)                $ p_ghgInvAlloc(ghginv,em,actFp,a),
                                              p_ghgInvAlloc(ghginv,em,actFp,a)   * emiFact(em,actFp,a,t))
             + sum(a $ (p_rhoEmiXP(em,a)      $ p_ghgInvAlloc(ghginv,em,"emiXP",a)),
                                              p_ghgInvAlloc(ghginv,em,"emiXp",a) * emiXP(em,a,t)
                                            + emiLUCF(em,a,t) $ (sum(actLnd,-p_rhoEmiLnd(em,actLnd,a)) $ (sameas(em,"co2") and sameas(ghgInv,"LUCF"))))
             - sum((actHa,lCov) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ carbCnt(actHa,lCov)),
                                              dCarbon(actHa,lCov,t)*3.67) $ (sameas(em,"co2") and sameas(ghgInv,"LUCF"))
        ]s(emiInv,'ghgInv,em,t-1');

* --- (236) Real emission tax

  e_rtEmi(emiFlag(em,aa),ts(t)) $ ((not ema(em)) $ ( (tEmi.range(em,aa,t) ne 0) or (tEmi.lo(em,aa,t) ne 0))) ..
   rtEmi(em,aa,t) *pgdpmp(t)s(tEmi,'em,aa,t-1') =e= tEmi(em,aa,t)s(tEmi,'em,aa,t-1');

* --- (237) Total emissions by emission type

  e_emiTot(em,ts(t)) $ card(emiFlag) ..
   emiTot(em,t)s(EmiTot,'em,t-1')
    =e= [sum(emiFlag(em,aa),emiSecA(em,aa,t))
           $$ifi "%considerCarbon%"=="on"  - sum((actHa,lCov) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ carbCnt(actHa,lCov)), dCarbon(actHa,lCov,t)*3.67) $ sameas(em,"co2")
        ]s(EmiTot,'em,t-1');

* --- (238) carbon stock changes from land use change

  e_dCarbon(actHa,lCov,ts(t)) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ carbCnt(actHa,lCov))  ..
   dCarbon(actHa,lCov,t)s(dCarbon,'actHa,lCov,t-1')
       =E=
          carbCnt(actHa,lCov) * (lCovArea(actHa,lCov,t) - lCovArea.l(actHa,lCov,t-1) $ (not tNext(t-1))
                                                        - lCovArea(actHa,lCov,t-1)   $      tNext(t-1)
                                 )s(dCarbon,'actHa,lCov,t-1');

* --- (239) land cover definition (actHa can be one landstock, the list of AEZ or another dis-aggregation of total land)

  e_lCovArea(actHa,lCov,ts(t)) $ ((lCovArea.l(actHa,lCov,t0) gt eps) $ (lCovMng(lCov) or lCovUnMng(lCov))) ..
   lCovArea(actHa,lCov,t)s(lCovArea,'actHa,lCov,t-1')
     =E=
    [
*
*     -- previous year stock of land cover type
*
         lCovArea.l(actHa,lCov,t-1)  $ (not tNext(t-1))
       + lCovArea(actHa,lCov,t-1)    $      tNext(t-1)
*
*     --- changes in managed categories from changes in xFac (expressed in Mio ha)
*
       + sum((crops,vp) $ flagXFac(actHa,crops,vp),xfac(actHa,crops,vp,t)-xfac.l(actHa,crops,vp,t-1) $ (not tNext(t-1))
                                                                         -xfac(actHa,crops,vp,t-1)   $      tNext(t-1)
             ) $ sameas(lCov,"cropLand")
       + sum((anim,vp)  $ flagXFac(actHa,anim,vp), xfac(actHa,anim,vp,t) -xfac.l(actHa,anim,vp,t-1)  $ (not tNext(t-1))
                                                                         -xfac(actHa,anim,vp,t-1)    $      tNext(t-1)
             ) $ sameas(lCov,"pastureLand")
       + sum((frs,vp)   $ flagXFac(actHa,frs,vp),  xfac(actHa,frs,vp,t)  -xfac.l(actHa,frs,vp,t-1)   $ (not tNext(t-1))
                                                                         -xfac(actHa,frs,vp,t-1)     $      tNext(t-1)
             ) $ sameas(lCov,"forest")
*
*     --- add exogenous changes in current land cover
*
      + p_exolCovAreaChg(actHa,lCov,t)
*
*     --- changes in unmanaged categories driven by changes in other land categories
*         (managed land plus built-up land) times land transition matrix (one ha of lCovDrc required x ha of lCov)
*
    - sum(lCovDrv $ (lCovArea.l(actHa,lCovDrv,t0) gt eps),
         (lCovArea(actHa,lCovDrv,t) - lCovArea.l(actHa,lCovDrv,t-1) $ (not tNext(t-1))
                                    - lCovArea(actHa,lCovDrv,t-1)   $      tNext(t-1)

         )*lndTransMat(actHa,lCovDrv,lCov)
      ) $ lCovUnMng(lCov)
    ]s(lCovArea,'actHa,lCov,t-1');

* --- (240) built-up land cover, driven by GDP per capita, population and pop density

  e_builtupArea(actHa,"builtupLand",ts(t)) $ (lCovArea.l(actHa,"builtupLand",t0) gt eps) ..
   lCovArea(actHa,"builtupLand",t)/lCovArea.l(actHa,"builtupLand",t0)
        =E=
              (rgdpmp(t)/rgdpmp.l(t0) * pop.l("ptotl",t0)/pop("ptotl",t))**0.33503
             *(pop("ptotl",t)/pop.l("ptotl",t0))**(1-0.17634);

* --- (241) Total emissions by pollution type (GHG in co2eq according to conversion factor emiConv)

  e_emiGHG(polltype,ts(t)) $ card(emiFlag) ..
   emiGHG(polltype,t)s(emiGHG,'polltype,t-1') =e= sum(em, p_emiConv(em,polltype)*emiTot(em,t))s(emiGHG,'polltype,t-1');

* --- (242) Emission intensity of GDP

  e_emiInt(polltype,ts(t)) $ card(emiFlag) ..
   emiInt(polltype,t)s(emiInt,'pollType,t-1') =e= emiGHG(polltype,t)/rgdpmp(t)s(emiInt,'pollType,t-1');

* ----------------------------------------------------------------------------
*
*  (243) Adaptive investments (only included if CC damages are active)
*
* ----------------------------------------------------------------------------

  e_xfdInv(adpinv,ts(t)) $ ifDmg ..
   xfd(adpinv,t)s(xfd,'adpInv,t-1')
     =e= [adpInvShr(adpinv,t) * sum(map_adpInv(f,adpinv), xfd(f,t)) + adpInvShft(adpinv,t)]s(xfd,'adpInv,t-1');

* ----------------------------------------------------------------------------
*
*  Air Pollution
*
* ----------------------------------------------------------------------------

* --- (244) Air emission intensity of production

  e_emiIntSec(aact(a),ts(t)) $ (sum(emiFlag(ema,a),1) $ ifAirPollDmg)  ..
   emiIntSec(a,t)*xp(a,t)
    %s1% /max(%minScale%,sum(emiFlag(ema,a),emiSecA.l(ema,a,t-1))*%scaleMult%)
          =e= sum(emiFlag(ema,a),emiSecA(ema,a,t))
    %s1% /max(%minScale%,sum(emiFlag(ema,a),emiSecA.l(ema,a,t-1))*%scaleMult%)
  ;

* --- (245) Air emission intensity of household consumption

  e_emiIntHH(h,ts(t)) $ (sum(emiFlag(ema,h),1) $ ifAirPollDmg) ..
   emiIntHH(h,t)*yf(h,t)
     %s1% /max(%minScale%,sum(emiFlag(ema,h),emiSecA.l(ema,h,t-1))*%scaleMult%)
          =e= sum(emiFlag(ema,h),emiSecA(ema,h,t))
     %s1% /max(%minScale%,sum(emiFlag(ema,h),emiSecA.l(ema,h,t-1))*%scaleMult%)
  ;

* --- (246) Aggregate air pollution emission over last 5 years

  e_exposedAirpoll(ts(t)) $ ifAirPollDmg ..
   exposedAirPoll(t)s(exposedAirPoll,'t-1')
    =e=   sum(tf $ (     sameas(tf,ts)  and (years(tf) le years(t)) and (years(t)-years(tf)<5)),emiGhg("PMTF",tf)   )s(exposedAirPoll,'t-1')
        + sum(tf $ ((not sameas(tf,ts)) and (years(tf) le years(t)) and (years(t)-years(tf)<5)),emiGhg.l("PMTF",tf) )s(exposedAirPoll,'t-1');

* --- (247) Air pollution damages to labor stock (depend on aggregated emission of last 5 years)

  e_dmgAir(l,a,ts(t)) $ ifAirPollDmg ..
   dmgAir(l,a,t) =e= -p_AirPollEff(l,a,t) * log(ExposedAirPoll(t)/ExposedAirPoll.l(t0));

* ----------------------------------------------------------------------------
*
*  Dynamics and additional variables to target
*
* ----------------------------------------------------------------------------

*
* --- (248) Current year capital is last year stock minus depreciation plus productive
*           investments. Depreciation can include CC damages to the stock
*
  e_kstock(ts(t)) $ (kStock.range(t) ne 0) ..
   kstock(t)s(kStock,'t-1')
    =e=
*
*     --- previous year capital stock minus depreciation
*
         [
            $$iftheni.forwardLooking "%ForwardLooking%"=="on"
              prod(cap,m_dmgAgg(dmgStk,'cap')) * (  [ (1-depr.l(t-1))*kstock.l(t-1)] $ (not tNext(t-1))
                                                  + [ (1-depr(t-1))  *kstock(t-1)  ] $ tNext(t-1))
*              --- plus productive investments
              + prodInv.l(t-1) $ (not tNext(t-1)) + prodInv(t-1) $ tNext(t-1)
            $$else.forwardLooking
              prod(cap,m_dmgAgg(dmgStk,'cap')) * (1-depr.l(t-1))*kstock.l(t-1)
              + prodInv.l(t-1)
            $$endif.forwardLooking
         ]s(kStock,'t-1');

* --- (249) Unemployment

  e_unemp(ts(t)) ..

     unemp(t)
      %s1%  /max(%minScale%,sum((sameas(l,actfmp),vf),xfmSup.l(actfmp,actfmp,"suminst",vf,t-1))*%scaleMult%)
     =E=   (pop("P1564",t) - sum((sameas(l,actfmp),vf),xfmSup(actfmp,actfmp,"sumInst",vf,t)))
       %s1%       /max(%minScale%,sum((sameas(l,actfmp),vf),xfmSup.l(actfmp,actfmp,"suminst",vf,t-1))*%scaleMult%)
  ;

* --- (250) Total value of value added across sectors and vintages, based on added up pre-tax value of factor use
*     by activities

  e_sectTot(ts(t)) ..
   sectTot(t)s(sectTot,'t-1') =e= sum(flagXFac(actFp,a,vp),pxfac_preTax(actFp,a,old,t)*xfac(actFp,a,vp,t))s(sectTot,'t-1');

* --- (251) Sector share in calculated total VA above

  e_sectShr(aagr,ts(t)) $ sum(mapAagr(aagr,a),1) ..
   sectShr(aagr,t) =e=   sum(flagXFac(actFp,a,vp) $ mapAagr(aagr,a),pxfac_preTax(actFp,a,old,t)*xfac(actFp,a,vp,t))/SectTot(t);

* --- (252) Energy intensity of GDP (might be targeted)

  e_nrgInt(ts(t)) ..
   nrgInt(t) =e= sum(e,xat(e,t))/rgdpmp(t);

* --- (253) Electricity intensity of GDP (might be targeted)

  e_elyInt(ts(t)) ..
   elyInt(t) =e= sum(ielec,xat(ielec,t))/rgdpmp(t);
*
* --- dummy objective to solve model as a CNS
*
  e_dummy ..
   dummy =E= 0;

  $$ifi not set paired $setglobal paired off
  $$iftheni.paired "%paired%"=="on"
     model cge_ /
       $$include 'inc/model_def.inc'
       e_rrat.rrat
     /;
  $$else.paired

    $$iftheni.mother "%iScen%"==""

        $$onEmbeddedCode Python:
            f = open(r"inc/model_def_copy.inc","w")
            with open(r"inc/model_def.inc","r") as file:
               for line in file:
                  s = line
                  if " e_" in line:
                     pos = line.find(".")
                     if not pos == -1:
                        s = line[:pos]

                  f.write(s+"\n")

            f.close()
        $$offEmbeddedCode

    $$endif.mother
    model cge_ /
       $$include 'inc/model_def_copy.inc'
       e_rrat.rrat
    /;

  $$endif.paired

  model m_dummy / e_dummy /;
  m_dummy.solvelink=0;
  m_dummy.solprint=2;
  m_dummy.limrow=0;
  m_dummy.limCol=0;



  $$if not errorfree $abort Compilation error after file: %system.fn%
