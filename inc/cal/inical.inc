********************************************************************************
$ontext

   MANAGE-WB project

   GAMS file : INICAL.INC

   @purpose  : Initialize variables and parameters for the model in the Baseline
               mode and load them from the baseline results for counterfactual simulations
   @author   : MANAGE-WB team
   @date     : 24.11.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms
                                                                                                                                                                  x
$offtext
********************************************************************************

  $$iftheni.task "%task%"=="Baseline"

     $$batinclude "inc/title.inc" '"Initialization of variables"'

     ifcal  = 1;

*    ----------------------------------------------------------------------------
*
*     Recalibration to account for markups
*
*    ----------------------------------------------------------------------------

     $$iftheni.endotech "%endotech%"=="on"
*
*       --- production value for each activity, net of production taxes and subsidies
*
        xp.l(a,t)     = sum(is,sam(is,a))-sum(ptx,sam(ptx,a))-sum(psb,sam(psb,a));
*
*       --- GDP at factor cost
*
        gdpfc.l(t)    = sum((fp,a),sam(fp,a));
*
*       --- average markup is share RD on GDP at factor costs
*           divided by production value
*
        p_markup(a,t) = EnTPar("prvRD")*gdpfc.l(t)/sum(a1,xp.l(a1,t));
*
*       --- if markups don't exceed 50% of capital cost, assign as R&D profits
*           otherwise, activity is not subject to R&D mechanism
*
        p_rdProf(a)     $ (sum(cap,sam(cap,a)) gt p_markup(a,t0)/(1+p_markup(a,t0))*xp.l(a,t0)*2)
                      = p_markup(a,t0)/(1+p_markup(a,t0))*xp.l(a,t0);
*
*       --- rebook expenditures for R&D from capital
*           to enterprise account
*
        sam(cap,a)    = sam(cap,a)    - p_rdProf(a);
        sam(entr,cap) = sam(entr,cap) - sum(a,p_rdProf(a));

        p_variety(a,t) = 1;

     $$else.endotech

        option kill=xrd;
        option kill=p_rdProf;
        option kill=p_markup;
        option kill=p_variety;

     $$endif.endotech

*    ----------------------------------------------------------------------------
*
*     Initialize prices
*
*    ----------------------------------------------------------------------------

     p_psb0(i)     $ comPar(i,"psb0")        = comPar(i,"psb0");
     p_psb0(i)     $ (not comPar(i,"psb0"))  = 1;
*
*    --- if physical producition is given, derive producer
*        price from there (output value per unit of production)
*
     p_psb0(i) $ sum(a $ sam(a,i), actPar(a,"physProd",""))
       = sum(a $ sam(a,i),sam(a,i)/actPar(a,"physProd",""));

     ps.l(i,t0) = p_psb0(i);
*
*    --- import prices. As above: replace in case of phys. production
*
     p_psbm0(i) $ comPar(i,"psbm0")                          = comPar(i,"psbm0");
     p_psbm0(i) $ (not comPar(i,"psbm0"))                    = 1;
     p_psbm0(i) $ sum(a $ sam(a,i), actPar(a,"physProd","")) = p_psb0(i);

*    --- What is this? Energy product treated differently?

     ps.l(e,t0)  $ (sum(a,sam(a,e)) eq 0) = 1;
     ps.l(e,t0)  $ (sum(a,sam(a,e)) ne 0) = p_psb0(e);

     p.l(a,i,t0) $ sam(a,i)               = ps.l(i,t0);
     pa.l(i,t0)                           = 1;

*    ----------------------------------------------------------------------------
*
*     Initialize emission variables
*
*    ----------------------------------------------------------------------------

     $$include 'inc/cal/iniEmis.inc'

*    ----------------------------------------------------------------------------
*
*     Initialize Armington demands
*
*    ----------------------------------------------------------------------------

     pp.l(a,t0)    = 1;
     xa.l(i,aa,t0) = sam(i,aa)/pa.l(i,t0);

*    ----------------------------------------------------------------------------
*
*     Calibrate production equations
*
*    ----------------------------------------------------------------------------

     if (ifVintUpdate,
         p_sigmaxfac(fppar,a,v) = p_sigmaxfac0(fppar,a,old) $ (not ifVint) + p_sigmaxfac0(fppar,a,v) $ ifVint;
     else
*       --- attention: this code is not correct. It is left for an intermediate time until all projects
*                      can switch to the correct version above.
        p_sigmaxfac(fppar,a,v)  = p_sigmaxfac0(fppar,a,old) $ ifVint + p_sigmaxfac0(fppar,a,v) $ (not ifVint);
     );

     p_omegaP(a) = defPar('omegap0');

*
*    --- use defaults from defPar and overwrite with comPar sheet if given
*        an eps in the comPar sheets results in a zero elasticity
*
     p_sigmaS(i)                          = defPar('sigmas0')    $ (defPar('sigmas0')    gt eps);
     p_sigmaM(i)                          = defPar('sigmam0')    $ (defPar('sigmam0')    gt eps);
     p_sigmaMg(i)                         = defPar('sigmamg0')   $ (defPar('sigmamg0')   gt eps);
     p_sigmaS(i)   $ comPar(i,"sigmas0")  = comPar(i,"sigmas0")  $ (comPar(i,"sigmas0")  gt eps);
     p_sigmaM(i)   $ comPar(i,"sigmam0")  = comPar(i,"sigmam0")  $ (comPar(i,"sigmam0")  gt eps);
     p_sigmaMg(i)  $ comPar(i,"sigmamg0") = comPar(i,"sigmamg0") $ (comPar(i,"sigmamg0") gt eps);

*
*    --- production subsidies and related shifers (note that the base is the net-of-tax production cost)`
*
     tPsb0.fx(a,ttCal)     $ (sum(is,sam(is,a)) ne 0) = sam(psb,a) / (sum(is,sam(is,a)) - sam(psb,a) - sam(ptx,a));
     tPsbD0.fx(ttCal)       = 0;
     tPsbD1.fx(a,ttCal)     = 0;
     tPsbM0.fx(ttCal)       = 1;
     tPsbM1.fx(a,ttCal)     = 1;
     tPsb.l(a,ttCal)        = (tPsb0.l(a,ttCal) + (tPsbD0.l(ttCal) + tPsbD1.l(a,ttCal))) * tPsbM0.l(ttCal) * tPsbM1.l(a,ttCal);
*
*    --- production taxes and related shifters (note that the base is the net-of-tax production cost)
*
     tPrd0.fx(a,ttCal)     $ (sum(is,sam(is,a)) ne 0) = sam(ptx,a) / (sum(is,sam(is,a)) - sam(ptx,a) - sam(psb,a));
     tPrdD0.fx(ttCal)       = 0;
     tPrdD1.fx(a,ttCal)     = 0;
     tPrdM0.fx(ttCal)       = 1;
     tPrdM1.fx(a,ttCal)     = 1;
     tPrd.l(a,ttCal)        = (tPrd0.l(a,ttCal) + (tPrdD0.l(ttCal) + tPrdD1.l(a,ttCal))) * tPrdM0.l(ttCal) * tPrdM1.l(a,ttCal);
*
*    --- phantom taxes
*
     tPhn.l(a,ttCal)        = 0;
     tPhn0.fx(a,ttCal)      = 0;
     tPhnD0.fx(ttCal)       = 0;
     tPhnD1.fx(a,ttCal)     = 0;
     tPhnD2.fx(aagr,ttCal)  = 0;
*
*    --- tax and subsidy exclusive price of output (pp is at unity)
*
     px.l(a,t0) $ (not ifEnTech) = pp.l(a,t0)/(1 + tPrd.l(a,t0) + tPsb.l(a,t0));
*
*    --- markup over production costs: RD spendings, correct production cost price px
*
     p_markup(a,ttCal) $ (ifEnTech $ sum(is,sam(is,a))) = p_rdProf(a) / sum(is,sam(is,a));
     px.l(a,t0)     $ ifEnTech                       = 1 + p_markup(a,t0);
*
*    --- selling price equal production cost plus taxes and subsidies (later should be negative)
*
     pp.l(a,t0) $ ifEnTech = px.l(a,t0)*(1 + tPrd.l(a,t0) + tPsb.l(a,t0) );
*
*    --- production value inclusive R&D cost
*
     xp.l(a,t0)  = (sum(is,sam(is,a)) + p_rdProf(a)) / pp.l(a,t0);
*
*    --- ouput of product i by activity a. Note that p.l is not necessarily unity
*        due to psb0 or use of MTOE
*
     x.l(a,i,t0) $ sam(a,i) = sam(a,i) / p.l(a,i,t0);
*
*    --- total domestis sales, aggregated over activities
*
     xs.l(i,t0)             = sum(a, p.l(a,i,t0) * x.l(a,i,t0)) / ps.l(i,t0);
*
*    --- stock changes
*
     delst.fx(i,ttCal)         = sum(stb,sam(i,stb)) / ps.l(i,t0);

     aact(a) = yes $ (xp.l(a,t0) ne 0);
     iact(i) = yes $ (xs.l(i,t0) ne 0);

*    --- If domestic stock changes delst > xs, assign to stock changes of imports
*         Must re-value mdelst once we have tariffs

     mdelst.fx(i,ttCal) $ (delst.l(i,ttCal) gt xs.l(i,t0)) = delst.l(i,ttCal);
     delst.fx(i,ttCal)  $ (delst.l(i,ttCal) gt xs.l(i,t0)) = 0;
*
*    --- indicator set for products with stock changes
*
     ist(i) $ (mdelst.l(i,t0) or delst.l(i,t0)) = yes;

     if ( execError, abort " ExecError encountered after initialization of production in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Initialize debt variables
*
*    ----------------------------------------------------------------------------

*     --- average interest on domestic and external debt

     chiSavGDebt.l(ttCal)      = 1;

     avgDebtInt.l(instD,ttCal) = defPar('DebtIntD0');
     avgDebtInt.l(instF,ttCal) = defPar('DebtIntF0');

*    ---- shares of institutions in debt payments

     p_chiDebtPay(instF,ttCal) $ sum(instFp,sam(instFp,debt))  =  sam(instF,debt)/sum(instFp,sam(instFp,debt));
     p_chiDebtPay(instD,ttCal) $ sum(instDp,sam(instDp,debt))  =  sam(instD,debt)/sum(instDp,sam(instDp,debt));

*    --- debt stocks (not from SAM, additiional accounts)

     DebtStk.l(instD,ttCal) = p_chiDebtPay(instD,ttCal)*defPar('DebtStkD0')*inScale;
     DebtStk.l(instF,ttCal) = defPar('DebtStkF0')*inScale;
     DebtStkT.l(ttCal)      = sum(instFD,DebtStk.l(instFD,ttCal));

*    --- Interest rates, drives saving behavior, use domestic one

     interest.l(t0)             = defPar('DebtIntD0');
     riskPrem.fx(instfd,ttCal) = 1;
*
*    --- Define risk premium as relation between foreign and domestic interest rate
*        This can be low if there is a lot of subsidized debt, such as from IFM
*
     riskPrem.fx(instf,ttCal) = defPar('DebtIntF0') / defPar('DebtIntD0');

*    --- debt payments (prinicpal and interest)

     debtPay.l(instFD,t0)  = sam(instFD,debt);

*    --- interest payments

     debtInterest.l(instFD,t0) = DebtStk.l(instFD,t0) * avgDebtInt.l(instFD,t0) / 100;

*    --- debt repayment ratio total debt paymnent (interest and principal) from SAM
*        minus interest rate

     rDebtStk.fx(instFD,ttCal) $ (not DebtStk.l(instFD,ttCal)) = 0;
     rDebtStk.fx(instFD,ttCal) $ DebtStk.l(instFD,ttCal)= sam(instFD,debt) / DebtStk.l(instFD,t0)
                                                        - (interest.l(t0) * riskPrem.l(instFD,t0))/100;
     rDebtStkT.fx(ttCal) = 1;

*    --- Principal payments on existing debt, based on variables defined above

     debtPrincipal.l(instFD,t0) = rDebtStkT.l(t0) * rDebtStk.l(instFD,t0) * DebtStk.l(instFD,t0);

     debtNew.l(instFD,ttCal)                                   = sam(debt,instFD);
     debtNew.fx(instFD,ttCal) $ (not  debtNew.l(instFD,ttCal)) = 0;
     debtNewT.l(t0)                                           = sum(instFD,sam(debt,instFD));

*    --- Fiscal responsibility mechanism

     p_relExpInc          = %relExpInc%;
     p_debtGdpTgtLo(t)    = %debtGdpTgtLo%/100;

     $$ifthene %debtGdpTgtUp%>0
        p_debtGdpTgtUp(t) = %debtGdpTgtUp%/100;
     $$else
        p_debtGdpTgtUp(t) = defPar('debtGdpTgt0');
     $$endif

     if ( execError, abort " ExecError encountered after initialiation of debt related variables in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Initialize income variables
*
*    ----------------------------------------------------------------------------

*
*    --- expenditures by final demanders oa (based on Armington quantity and price)
*
     yf.l(oa,t0)    = sum(i,pa.l(i,t0) * xa.l(i,oa,t0));
     p_yf0(oa,ttCal)   = yf.l(oa,t0);
*
*    --- primary factor and transfer income
*
     facty.l(fp,t0) = sum(a,sam(fp,a));
*
*    --- set of primary and land factors in use
*
     actFp(fp)   $ facty.l(fp,t0)  = yes;
     actLnd(lnd) $ facty.l(lnd,t0) = yes;
*
*    --- total captial income, transfers, tax redistribution vaiables
*
     kapy.l(t0)                 = sum(cap, facty.l(cap,t0));
     transfers.l(inst,instp,t0) = sam(inst,instp);
     yTaxRedist.fx(ttCal)          = 0;
*
*    --- household income net of taxes (factor income, transfers recevied, principal received
*
     yh.l(h,t0)                 = sum(actFp, sam(h,actFp))
                                     + sum((instp,inst) $ mapinst(h,instp),transfers.l(instp,inst,t0))
                                     + sum(instD        $ mapinst(h,instD),debtPay.l(instD,t0));

     gini.l(t0) $ (card(h) gt 1)  = 2 * sum(h,ord(h) * yh.l(h,t0))/(card(h) * sum(h,yh.l(h,t0))) - (card(h)+1) / card(h);

     savH.l(h,t0)                 = sum(inv, sam(inv,h)) + sum(sameas(h,instFD),debtNew.l(instFD,t0));

     tDr.fx(t)                       = 1;
*
*    --- private income tax rate and related shifters
*
     tPit0.fx(h,ttCal)     = sum(dtx, sam(dtx,h))/yh.l(h,t0);
     tPitD0.fx(ttCal)      = 0;
     tPitD1.fx(h,ttCal)    = 0;
     tPitM0.fx(ttCal)      = 1;
     tPitM1.fx(h,ttCal)    = 1;
     tPit.l(h,ttCal)       = (tPit0.l(h,ttCal) + (tPitD0.l(ttCal) + tPitD1.l(h,ttCal))) * tDr.l(ttCal) * tPitM0.l(ttCal) * tPitM1.l(h,ttCal);

     totTr.l(instp,t0)  = sum(inst,transfers.l(inst,instp,t0));

     loop(mapinst(h,instp),
*       --- share of income used for transfers
        chiTotTr.fx(instp,ttCal) $ yh.l(h,t0)        = totTr.l(instp,t0) / (yh.l(h,t0));
        chiTotTr.fx(instp,ttCal) $ (not yh.l(h,t0))  = 0;
     );
*
*    --- disposable income for consumption and saving: income after direct taxation,
*        minus spending on transfers
*
     yhDsp.l(h,t0)      = (yh.l(h,t0) * (1 - tPIT.l(h,t0)) - sum(mapinst(h,inst),totTr.l(inst,t0)));
*
*    --- marginal prospensity to save and related shifter
*
     aps.l(h,ttCal)        = savH.l(h,t0) / yhDsp.l(h,t0);
     chims.fx(ttCal)       = 1;
*
*    --- total enterprise income (tax inclusive) and direct taxes paid by enterprises
*
     entrY.l(entr,t0)   = sum(is,sam(entr,is));
     entrTax.l(entr,t0) = sum(dtx,sam(dtx,entr));
*
*    --- cooperate income tax and related shifters
*
     tCit0.fx(entr,ttCal)  = (sum(dtx, sam(dtx,entr)) / entrY.l(entr,t0)) $ entry.l(entr,t0);
     tCitD0.fx(ttCal)      = 0;
     tCitD1.fx(entr,ttCal) = 0;
     tCitM0.fx(ttCal)      = 1;
     tCitM1.fx(entr,ttCal) = 1;
     tCit.l(entr,ttCal)    = (tCit0.l(entr,ttCal) + (tCitD0.l(ttCal) + tCitD1.l(entr,ttCal))) * tDr.l(ttCal) * tCitM0.l(ttCal) * tCitM1.l(entr,ttCal);
*
*    --- productive enterprise savings (contributing to investment):
*        deduct R&D cost stored on rdProf, and new debt financed by enterprices
*
     savEntr.l(entr,t0) = sum(inv, sam(inv,entr)) + sum(ginv, sam(ginv,entr)) + sam(debt,entr) - sum(a,p_rdProf(a));
*
*    --- saving rate of enterprieses: first calculate base (after tax income minus contribution to new gov. debt)
*
     apsEntr.fx(entr,ttCal) = entrY.l(entr,t0) - entrTax.l(entr,t0) - sum(mapinst(entr,inst),debtNew.l(inst,t0));
*
*    --- saving rate is savings over base
*
     apsEntr.fx(entr,ttCal)$(apsEntr.l(entr,ttCal) ne 0) = savEntr.l(entr,t0) / apsEntr.l(entr,t0);

     loop(mapinst(entr,instp),
*       --- share of income used for transfers (currently not used in model)
        chiTotTr.fx(instp,ttCal) $ entrY.l(entr,t0)
          = totTr.l(instp,t0) / (entrY.l(entr,t0) - entrTax.l(entr,t0) - savEntr.l(entr,t0) - debtNew.l(instp,t0));
        chiTotTr.fx(instp,t0) $ (not entrY.l(entr,t0)) = 0;
     );

     loop(mapinst(row, instp),
        rowtransfers.fx(inst,ttCal) = transfers.l(inst,instp,t0);
     );
*
*    --- allocation shares on total transfer spending by institutions
*
     chiTr.fx(inst,instp,ttCal) $ totTr.l(instp,t0) = transfers.l(inst,instp,t0)/totTr.l(instp,t0);
     if ( execError, abort " ExecError encountered after initialiation of income related variables in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Initialize final demand variables
*
*    ----------------------------------------------------------------------------

     pf.l(oa,t0)  = 1;
     xfd.l(oa,t0) = yf.l(oa,t0) / pf.l(oa,t0);
*
*    --- share of adaptive investments on physical demand
*
     adpInvShr.fx(adpinv,ttCal) $ sum(map_adpInv(f,adpinv),xfd.l(f,t0))         = xfd.l(adpinv,t0) / sum(map_adpInv(f,adpinv),xfd.l(f,t0));
     adpInvShr.fx(adpinv,ttCal) $ (not sum(f$map_adpInv(f,adpinv),xfd.l(f,t0))) = 0;
     adpInvShft.fx(adpinv,ttCal)                                                = 0;
*
*    --- initialize demand nests
*
     set  kNestsToDo(kNest),curNests(kNest);
     kNestsToDo(kNest) = yes;
     alias(kNestsToDo,kNestsToDo1);

     pxak.l(oa,knest,t0) = 1;
     option kill=xak.l;

     while(card(kNestsToDo),
*
*        --- nests in current loop: all which do not comprise shares of nests not yet
*            calculate. In first round, only nests which comprise single products will
*            be defined.
*
         option kill=curNests;
         curNests(kNestsToDo) $ (not sum(kNestsToDo1,mapkNestShr(kNestsToDo1,kNestsToDo))) = yes;

         xak.l(oa,curNests,t0) =  sum(i,     mapkNestShr(i,curNests) * sam(i,oa))
                                + sum(kNest, mapkNestShr(kNest,curNests) * xak.l(oa,kNest,t0));
*
*        --- nests which have been calculated won't be considered again
*
         kNestsToDo(curNests) = no;

     );
*
*    --- shares of nests in total demand expenditures
*
     xakshr.l(oa,topKNest,ttCal) $ xak.l(oa,topKNest,t0) = pxak.l(oa,topKNest,t0) * xak.l(oa,topKNest,t0) / yf.l(oa,t0);
*
*    --- preference shifters for product demand by final demanders
*
     lambdaOa.fx(oa,i,kNest,ttCal) $ (mapKNestShr(i,kNest) $ xa.l(i,oa,t0)) = 1;

     if ( execError, abort " ExecError encountered after initialiation of income final demand in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Initialize imports
*
*    ----------------------------------------------------------------------------

*
*    --- exchange rate shifter (all transactions/prices in model are defined in local currency)
*
     er.l(ttCal) = 1;
*
*    --- list products which are imported
*
     iImp(i) $ (sam(row,i) ne 0) = yes;
*
*    --- import prices (can be set different from zero with psbm0 on comPar sheet of bridge-file
*
     pwm.l(i,t0) $ iImp(i)          = p_psbm0(i) / er.l(t0);
     pwm.l(i,t0) $ (not iImp(i))    = pa.l(i,t0);
     pwmROW.fx(i,ttCal)             = pwm.l(i,t0);
*
*    --- import Taxes and related shifters
*
     tImp0.fx(i,ttCal) $ iImp(i)       = sam(mtx,i) / sam(row,i);
     tImp0.fx(i,ttCal) $ (not iImp(i)) = 0;
     tImpD0.fx(ttCal)                  = 0;
     tImpD1.fx(i,ttCal)                = 0;
     tImpM0.fx(ttCal)                  = 1;
     tImpM1.fx(i,ttCal)                = 1;
     tImp.l(i,ttCal)                   = (tImp0.l(i,ttCal) + (tImpD0.l(ttCal) + tImpD1.l(i,ttCal))) * tImpM0.l(ttCal) * tImpM1.l(i,ttCal);

*    --- Re-base imported inventory on cif+tariff

     mdelst.fx(i,ttCal) = mdelst.l(i,t0) / (pwm.l(i,t0) * er.l(t0) * (1+tImp.l(i,t0)));

*    --- Calculate import volumes (based on c.i.f. prices)

     xm.l(i,t0)      = sam(row,i) / (er.l(t0) * pwm.l(i,t0)) - mdelst.l(i,t0);

*    ----------------------------------------------------------------------------
*
*     Initialize exports
*
*    ----------------------------------------------------------------------------

*    --- Assume trade and transport margins are uniform across all nodes, then

     pmarg.l(i,t0)    = 1;
     pmargInd.l(i,t0) = pmarg.l(i,t0);

*    --- calculate a uniform margin for all nodes on position "d" in case that either
*        domestic supply or imports are given

     tmg.fx(j,"d",ttCal)  $ (pmarg.l(j,t0) * (xs.l(j,t0) - delst.l(j,t0) + xm.l(j,t0)))
        = sum(i,sam(i,j)) / (pmarg.l(j,t0) * (xs.l(j,t0) - delst.l(j,t0) + xm.l(j,t0)));

     aaIn(aa)                        = yes;
     aaIn(j) $ (not sum(i,sam(i,j))) = no;
*
*    -- copy "d" to "m" (imports) and "x" (exports) if these are present
*
     tmg.fx(j,"m",ttCal)  $ (xm.l(j,t0) ne 0) = tmg.l(j,"d",t0);
     iExp(i)              $ (sam(i,row) ne 0)    = yes;
     tmg.fx(j,"x",ttCal)  $  iExp(j)             = tmg.l(j,"d",t0);
*
*    -- delete on "d" (domestic) if soley imports
*
     tmg.fx(j,"d",ttCal)  $ (xs.l(j,t0) eq 0) = 0;
*
*    --- export price is equal to sales price
*
     pe.l(i,t0) $ iExp(i)                     = ps.l(i,t0);
     pe.l(i,t0) $ (not iExp(i))               = 1;
     pwe.l(i,t0)                              = 1;
*
*    --- export quantities: value in sam minus export taxes minus values of margins
*
     xe.l(i,t0)     $ iExp(i)   = (sam(i,row) - sam(etx,i)) / (pe.l(i,t0) + pmarg.l(i,t0)*tmg.l(i,"x",t0));
*
*    --- export taxes and related shifters
*
     tExp0.fx(i,ttCal) $ iExp(i)       = sam(etx,i) / (pe.l(i,t0) * xe.l(i,t0));
     tExp0.fx(i,ttCal) $ (not iExp(i)) = 0;
     tExpD0.fx(ttCal)                  = 0;
     tExpD1.fx(i,ttCal)                = 0;
     TExpM0.fx(ttCal)                  = 1;
     tExpM1.fx(i,ttCal)                = 1;
     tExp.l(i,t0)                      = (tExp0.l(i,t0) + (tExpD0.l(t0) + tExpD1.l(i,t0))) * tExpM0.l(t0) * tExpM1.l(i,t0);
     pwe.l(i,t0)    $iExp(i)           = (pe.l(i,t0) * (1 + texp.l(i,t0)) + pmarg.l(i,t0) * tmg.l(i,"x",t0)) / er.l(t0);

     pweROW.fx(i,ttCal)                = pwe.l(i,ttCal);
*
*    --- calculate deomstic sales residually
*
     xd.l(i,t0) = xs.l(i,t0) - xe.l(i,t0) - delst.l(i,t0);

     abort $ sum(i $ (xd.l(i,t0) < 0),1) 'Exports + stock changes > Production; check xd, file: %system.fn%, line: %system.incline%';

     if ( execError, abort " ExecError encountered after initialiation of imports and exports in file: %system.fn%, line: %system.incline%");

*    ---------------------------------------------------------------------------------
*
*     Initialize price for domestic and imported origin, and for Armingont composite
*
*    ---------------------------------------------------------------------------------

     set ax(aa); ax(aa) = yes; ax(i) = no;
*
*    --- domestically sold products
*
     iDom(i) $ (xd.l(i,t0) ne 0)   = yes;

     pd.l(i,t0) $ iDom(i)       = ps.l(i,t0);
     pd.l(i,t0) $ (not iDom(i)) = 1;

*    --- Calculate value added tax rate, asssume uniformity between domestic and import

     tVat0.l(i,n,t0)     $ ((pd.l(i,t0) + pmarg.l(i,t0) * tmg.l(i,"d",t0)) * xd.l(i,t0)
                         + (pwm.l(i,t0) * er.l(t0) * (1 + timp.l(i,t0)) + pmarg.l(i,t0) * tmg.l(i,"m",t0)) * xm.l(i,t0))
                         = sum(vat,sam(vat,i))
                         / ((pd.l(i,t0) + pmarg.l(i,t0) * tmg.l(i,"d",t0)) * xd.l(i,t0)
                         + (pwm.l(i,t0) * er.l(t0) * (1 + timp.l(i,t0)) + pmarg.l(i,t0) * tmg.l(i,"m",t0)) * xm.l(i,t0));

     tVat0.fx(i,n,ttCal)                    = tVat0.l(i,n,t0);
     tVat0.fx(i,n,ttCal) $ (not sam(vat,i)) = 0;
     tVatD0.fx(ttCal)                       = 0;
     tVatD1.fx(i,ttCal)                     = 0;
     tVatD2.fx(n,ttCal)                     = 0;
     tVatM0.fx(ttCal)                       = 1;
     tVatM1.fx(i,ttCal)                     = 1;
     tVatM2.fx(n,ttCal)                     = 1;
     tVat.l(i,n,t0)                      = (tVat0.l(i,n,t0) + (tVatD0.l(t0) + tVatD1.l(i,t0) + tVatD2.l(n,t0))) * tVatM0.l(t0) * tVatM1.l(i,t0) * tVatM2.l(n,t0);

*    --- Calculate user prices of domestic and imported goods (after margins and VAT tax)

     pda.l(i,t0) = (pd.l(i,t0) + pmarg.l(i,t0) * tmg.l(i,"d",t0)) * (1 + tVat.l(i,"d",t0));
     pma.l(i,t0) = (pwm.l(i,t0) * er.l(t0) * (1 + tImp.l(i,t0)) + pmarg.l(i,t0) * tmg.l(i,"m",t0)) * (1 + tVat.l(i,"m",t0));

*    --- Armington prices and demands

     pa.l(i,t0)  $ (xd.l(i,t0) + xm.l(i,t0)) = (pda.l(i,t0) * xd.l(i,t0) + pma.l(i,t0) * xm.l(i,t0)) / (xd.l(i,t0) + xm.l(i,t0));

     xat.l(i,t0) $ pa.l(i,t0)               = (pda.l(i,t0) * xd.l(i,t0) + pma.l(i,t0) * xm.l(i,t0)) / pa.l(i,t0);

     iArm(i)    $ (xat.l(i,t0) ne 0)      = yes;

*    --- Trade and transport margins are not taxed, define demand accordingly

     xa.l(i,j,t0)                      = sam(i,j) / pa.l(i,t0);
*
*    --- flag for products demanded for margins
*
     mgencl(j)  $ (sum(i, sam(i,j)) ne 0) = yes;

*    --- Calculate end user demand taxes -- assume uniformity across Armington agents

     tmAll.fx(ttCal) = 1;

     abort $ (sum(ssb,1) = 0) "SSB is empty. Please add at least a dummy element. Error in file: %system.fn%, line: %system.incline%";

     tPaf0.fx(i,ax,t0)    = ((sum(ssb,sam(ssb,i)) + sum(stx,sam(stx,i)))
                          / (pa.l(i,t0) * (xat.l(i,t0) - sum(j,xa.l(i,j,t0))))) $ (xat.l(i,t0) - sum(j,xa.l(i,j,t0)));

     tPaf0.fx(i,ax,ttCal)            = tPaf0.l(i,ax,t0);
     tPafD0.fx(ttCal)                = 0;;
     tPafD1.fx(i,ttCal)              = 0;;
     tPafD2.fx(aa,ttCal)             = 0;;
     tPafM0.fx(ttCal)                = 1;;
     tPafM1.fx(i,ttCal)              = 1;;
     tPafM2.fx(aa,ttCal)             = 1;;
     tPaf.l(i,aa,t0)    $ sam(i,aa)  = (tPaf0.l(i,aa,t0) + (tPafD0.l(t0) + tPafD1.l(i,t0) + tPafD2.l(aa,t0)))
                                                          * tPafM0.l(t0) * tPafM1.l(i,t0) * tPafM2.l(aa,t0) * tmAll.l(t0);

*    --- Margins don't have user taxes

     tpaf0.fx(i,j,ttCal) $ sam(i,j)  = 0;
     xa.l(i,aa,t0)    $ ax(aa)    = sam(i,aa) / ((1+ tpaf.l(i,aa,t0)) * pa.l(i,t0));
     if ( execError, abort " ExecError encountered after initialiation of imports and exports in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*      Initialize factor demands and production nests
*
*    ----------------------------------------------------------------------------

*
*   --- Labor quantities: use pyhsical quantities from bridge file
*
     p_labor(l,a) = sum((fp0,a0) $ (mapis(fp0,l) and mapis(a0,a)),labor0(fp0,a0)) * pScale;

     parameter p_testLabor(l,a);
     p_testLabor(l,a) $ (sam(l,a) and (not p_labor(l,a))) = sam(l,a);
     if (card(p_testLabor), abort "Labor data not consistent, not all SAM entries show physical use",p_testLabor);

*
*    --- Land use quantities: Use physical quantities defined in 1000 ha from GTAP AEZ where available
*
     p_land(actLnd,a)  $ sam(actLnd,a)       = sum((fp0,mapis(a0,a)) $ mapis(fp0,actLnd),land0(fp0,a0)) * 0.001 * inScale*usd;
*
*    --- carbon constent per ha
*
     carbCnt(actLnd,lCov)                    = sum(mapis(fp0,actLnd),carbCnt(fp0,lCov))*0.001/(inScale*usd) * reEScale;
*
*    --- remove physical land demand if no land rents found in SAM
*
     p_land(actLnd,a) $ (not sam(actLnd,a))  = 0;
*
*    --- actHa is the set of acreages, land could also store irrigation water
*
     actHa(actLnd) $ sum(a,p_land(actlnd,a)) = yes;
*
*    ---  assign factor values from SAM as quantities (change if factor quantities are available)
*         Note: Labor0 should be in the same scale with population (e.g. thousand people) for
*               this to work.

     xfac.l(actFp,a,old,t0)                                    = sam(actFp,a);
     xfac.l(l,a,old,t0) $ (xfac.l(l,a,old,t0) $ card(p_labor)) = p_labor(l,a);
*
*    --- remove land use information if not given for all land using activities
*
     parameter p_missingLnd(Lnd,a);
     p_missingLnd(actHa,a) $ (xfac.l(actHa,a,old,t0) and (not p_land(actHa,a))) = xfac.l(actHa,a,old,t0);
     alias(crops,crops1);
     alias(anim,anim1);
*
*    --- assign missing acreage from crops and animals based on total cropland or pastureland,
*        and share of land rent of such crops/animals on total land rents for crops/animals
*
     p_land(actHa,crops) $ p_missingLnd(actHa,crops)
                           = sum(mapis(fp0,actHa),lCov0(fp0,"cropLand") * 0.001) * inscale * usd * sam(actHa,crops) / sum(crops1,sam(actHa,crops1));
     p_land(actHa,anim)  $ p_missingLnd(actHa,anim)
                           = sum(mapis(fp0,actHa),lCov0(fp0,"pastureLand") * 0.001) * inscale * usd * sam(actHa,anim) / sum(anim1,sam(actHa,anim1));
*
*    --- if still missing: take SAM value
*
     p_missingLnd(actHa,a)  $ (xfac.l(actHa,a,old,t0) and (not p_land(actHa,a))) = xfac.l(actHa,a,old,t0);

     p_land(actHa,a)        $ sum(a1,p_missingLnd(actHa,a1))                     = 0;
     xfac.l(actHa,a,old,t0) $ (p_land(actHa,a) $ xfac.l(actHa,a,old,t0))         = p_land(actHa,a);

*    --- initialize the pre-Tax prices of natural factors

     pxfac_preTax.l(actFp,a,old,t0) $ xfac.l(actFp,a,old,t0)                     = sam(actFp,a) / xfac.l(actFp,a,old,t0);

*    --- avoid crazy land returns. With sigma at 7.5 across the AEZ, high land price easily provoke math trap errors

     pxfac_preTax.l(actHa,a,old,t0) $ (xfac.l(actHa,a,old,t0) $ card(p_land))    = max(0.001,min(5000,pxfac_preTax.l(actHa,a,old,t0)));
     xfac.l(actHa,a,old,t0)         $ (xfac.l(actHa,a,old,t0) $ card(p_land))    = sam(actHa,a) / pxfac_pretax.l(actHa,a,old,t0);

*    --- initialize factor taxes

     tFac0.fx(a,actFp,ttCal) $ sam(actFp,a)           = 0;
     tFac0.fx(a,actFp,ttCal) $ xfac.l(actFp,a,old,t0) = sum(mapftax(actFp,is),sam(is,a))/(xfac.l(actFp,a,old,t0)*pxfac_pretax.l(actFp,a,old,t0));

     if ( execError, abort " ExecError encountered after initialiation of factor demands in file: %system.fn%, line: %system.incline%",tFac0.l,xfac.l,pxFac_pretax.l;);

     tFac0.fx(a,actFp,ttCal) $ ((not tfac0.l(a,actFp,t0)) $ xfac.l(actFp,a,old,t0)) = 0;

     tFacD0.fx(ttCal)              = 0;
     tFacD1.fx(a,ttCal)            = 0;
     tFacD2.fx(actFp,ttCal)        = 0;
     tFacM0.fx(ttCal)              = 1;
     tFacM1.fx(a,ttCal)            = 1;
     tFacM2.fx(actFp,ttCal)        = 1;
     tfacMLab.fx(ttCal)            = 1;
     tfacMLabA.fx(a,ttCal)         = 1;
     tfacMCap.fx(ttCal)            = 1;
     tfacMlnd.fx(ttCal)            = 1;

     tFac.l(a,actFp,t0) $ xfac.l(actFp,a,old,t0)
              = (tFac0.l(a,actFp,t0) + (tFacD0.l(t0) + tFacD1.l(a,t0) + tFacD2.l(actFp,t0)))
               * tFacM0.l(t0) * tFacM1.l(a,t0) * tFacM2.l(actFp,t0);

     rtfac.l(a,actFp,t0) $ xfac.l(actFp,a,old,t0) = tfac.l(a,actFp,t0);

*    --- initialize post tax factor prices

     pxfac.l(actFp,a,old,t0) $ xfac.l(actFp,a,old,t0)   = pxfac_preTax.l(actFp,a,old,t0) * (1 + tfac.l(a,actFp,t0));

*    ---  assign intermediate use as quasi-factor quantities

     xfac.l(fpNestI,a,old,t0) = sum(sameas(fpNestI,i),xa.l(i,a,t0));

*    ---- initialize pseudo-factor prices for intermediate inputs

     pxfac.l(fpNestI,a,old,t0) $ xfac.l(fpNestI,a,old,t0)
         = sum(sameas(fpNestI,i),sam(i,a)) / xfac.l(fpNestI,a,old,t0);

*    ---- aggregate to parent nests and define related prices

     xfac.l(fppar,a,old,t0)
        = sum(fpNat $ map_fp_fppar(a,fpNat,fppar),xfac.l(fpNat,a,old,t0) * map_fp_fppar(a,fpNat,fppar));

     pxfac.l(fppar,a,old,t0) $ xfac.l(fpPar,a,old,t0)
        = sum(fpNat $ map_fp_fppar(a,fpNat,fppar),xfac.l(fpNat,a,old,t0) * map_fp_fppar(a,fpNat,fppar) * pxfac.l(fpNat,a,old,t0)) / xfac.l(fpPar,a,old,t0);

*    ---  flag the factor and intermediate uses (used to control FOCs)

     xfac.l(fppar,a,old,t0) $ (not pxfac.l(fpPar,a,old,t0)) = 0;
     flagXFac(fpNest,a,v)   $ xfac.l(fpNest,a,old,t0)       = 1;
     actFpNest(fpNest)      $ sum(flagXFac(fpNest,a,v),1)   = yes;
*
*    --- copy factor prices and quantities from t0 and old vintage to all years and vinates
*
     pxfacInd.l(flagXFac(fpPar,a,v),t0) $ isAdditiveCes(fpPar) = pxfac.l(fpPar,a,old,t0);
     pxfac_preTax.l(flagXFac(actFp,a,v),t0)                    = pxfac_pretax.l(actFp,a,old,t0);
     pxfac.l(flagXFac(actFpNest,a,v),t0)                       = pxfac.l(actFpNest,a,old,t0);

*    --- initialize productivity shifters

     chixFacFA.fx(actFpNest,a,ttCal) $ xfac.l(actFpNest,a,old,t0) = 1;
     chixFacF.fx(actFpNest,ttCal)         = 1;
     chixFacA.fx(a,ttCal)                 = 1;
     chixFacV.fx(v,ttCal)                 = 1;
     chixFacL.fx(ttCal)                   = 0;
     chixFacK.fx(ttCal)                   = 0;
     chixFacK.fx(ttCal)                   = 0;
     chixFacKL.fx(ttCal)                  = 0;
     chixFacFP.fx(ttCal)                  = 0;
     chixFacNRG.fx(ttCal)                 = 0;
     chixFacAgr.fx(aagr,actFpNest,ttCal)  = 1;

     lambdaxfac.l(actFpNest,a,v,t0)   $ xFac.l(actFpNest,a,v,t0)  = 1;
     lambdaxfac.l(actFpNest,a,new,t0) $ flagXFac(actFpNest,a,old) = lambdaxfac.l(actFpNest,a,old,t0);

*    ---- initialize climate damage shifters to zero

     option kill=dmgPrd,kill=dmgYld,kill=dmgSup,kill=dmgStk,kill=dmgDem,kill=dmgTrd,kill=dmgDepr;

*    --- initialize air pollution damage parameters to zero

     dmgAir.l(actFpNest,a,t0)  = 0;

     p_AirpollEff(l,a,ttCal)                            = defPar('AirpolEff0');
     p_AirpollEff(l,a,ttCal) $ actPar(a,"AirpolEff0",l) = actPar(a,"AirpolEff0",l);

*   --- set flags for top nest and remove flags for new vintages if the vintages are not active

     flagXFac("f-top",a,v)     $ xfac.l("f-top",a,old,t0) = 1;
     flagXFac(actFpNest,a,new) $ (not ifVint)             = 0;

*   ---  set the share parameters inside the nests

     p_markUpNonComp(actFp,a,ttCal) = actPar(a,actFp,"markUpNonComp");

     p_alphaXfac(actFpNest,fppar,a,v) $ (pNest(a,actFpNest,fpPar) $ xfac.l(fpPar,a,old,t0)  $ xfac.l(actFpNest,a,old,t0))
             =     (xfac.l(actFpNest,a,old,t0)*pNest(a,actFpNest,fpPar)/xfac.l(fpPar,a,old,t0))
                 * (1 - p_markUpNonComp(actFpNest,a,t0))
                 * (pxfac.l(actFpNest,a,old,t0)/(lambdaxfac.l(actFpNest,a,old,t0)*pxfac.l(fpPar,a,old,t0)))**p_sigmaxfac(fppar,a,v);

     if (execError, abort " ExecError encountered after setting share parameters in production nests in file: %system.fn%, line: %system.incline%",p_alphaXfac,xfac.l,pxfac.l);


     p_markUpNonComp(actFp,a,ttCal) $ xfac.l(fptop,a,old,t0)
          = [ xfac.l(actFp,a,old,t0)*p_markUpNonComp(actFp,a,t0)]
            / xfac.l(fptop,a,old,t0);

     $$iftheni.twist "%hasTwist%"=="on"

        p_tw(flagxFac(fpparTc,a,v),ttCal)                                  = 1;
        chitwFA.fx(fpparTC,a,ttCal)                                        = 0;
        chitwF.fx(fpparTC,ttCal)                                           = 0;
        twist.fx(fpChl,fpParTC,a,v,ttCal) $ p_alphaXfac(fpChl,fpParTC,a,v) = 0;

        shrxfac.l(fpchl,fpparTC,a,v,t0) $ (xfac.l(fpparTC,a,v,t0) and p_alphaXfac(fpChl,fpparTC,a,v) and fpChlTC(fpChl,a))
            = (pxfac.l(fpchl,a,v,t0)*xfac.l(fpchl,a,v,t0))/(pxfac.l(fpparTC,a,v,t0)*xfac.l(fpparTC,a,v,t0));
     $$else.twist
        option kill=p_tw,kill=chitwFA,kill=chitwF,kill=twist,kill=shrxFac;
     $$endif.twist

     yield.l(actFp,a,t0) $ sum(flagXFac(actFp,a,v),xfac.l(actFp,a,v,t0))
              = prod(chnl,1+dmgYld.l(actFp,a,chnl,t0)) * xp.l(a,t0) /sum(flagXFac(actFp,a,v),xfac.l(actFp,a,v,t0));

     if ( execError, abort " ExecError encountered after initialiation of production nests in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Initialize margin equations
*
*    ----------------------------------------------------------------------------

     xmarg.l(j,t0) = sum(i,sam(i,j)) / pmarg.l(j,t0);

*    ----------------------------------------------------------------------------
*
*     Initialize macro equations
*
*    ----------------------------------------------------------------------------

*
*    --- total stock changes and related prices
*
     tdelst.l(t0)                           = sum(i,ps.l(i,t0) * delst.l(i,t0) + er.l(t0) * pwm.l(i,t0) * (1+tImp.l(i,t0)) * mdelst.l(i,t0));
     rtdelst.l(t0)                          = tdelst.l(t0);
     pdelst.l(t0)                           = 1;
     pdelst.l(t0) $ (rtdelst.l(t0) ne 0) = tdelst.l(t0) / rtdelst.l(t0);
     ifDelSt         $ (tdelst.l(t0)     eq 0) = 0;
     ifDelSt         $ (tdelst.l(t0)     ne 0) = 1;
*
*    --- total exports and related prices
*
     totexp.l(t0)         = er.l(t0) * sum(i, pwe.l(i,t0) * xe.l(i,t0));
     rtotexp.l(t0)        = totexp.l(t0);
     pexp.l(t0)           = totexp.l(t0) / rtotexp.l(t0);
*
*    --- total imports and related prices
*
     totimp.l(t0)         = er.l(t0) * sum(i, pwm.l(i,t0) * (xm.l(i,t0) + mdelst.l(i,t0)));
     rtotimp.l(t0)        = totimp.l(t0);
     pimp.l(t0)           = totimp.l(t0) / rtotimp.l(t0);
*
*    --- GDP at market prices and related price index
*
     gdpmp.l(t0)          = sum(fd, yf.l(fd,t0)) + tdelst.l(t0) + totexp.l(t0) - totimp.l(t0);
     rgdpmp.l(t0)         = gdpmp.l(t0);
     pgdpmp.l(t0)         = gdpmp.l(t0) / rgdpmp.l(t0);
*
*    --- debt to GDP ratio and related driver for fiscal responsability mechanism
*
     debtGDP.l(t0)        = DebtStkT.l(t0) / gdpmp.l(t0) + 1.E-6;
     fiscalRespDrv1.l(t0) = ncpvusin(0,-{sigmoid([debtGdp.l(t0) - p_debtGdpTgtUp(t0)]*0.5)-0.5}*2,1.E-4);
     fiscalRespDrv.l(t0)      = fiscalRespDrv1.l(t0) - ncpVuPow(0,-{sigmoid([{p_debtGdpTgtLo(t0) - debtGdp.l(t0)} / p_debtGdpTgtLo(t0)]*0.5)-0.5}*10,1.E-4) $ p_debtGdpTgtLo(t0);

*    ----------------------------------------------------------------------------
*
*     GDP and Population
*
*    ----------------------------------------------------------------------------

*
*    --- GDP at benchmark from SAM
*
     p_GDP(t0)                    =   sum((i,fd),sam(i,fd)) - sum((i,fd),sam(fd,i))
                                    + sum((i,stb),sam(i,stb))
                                    + sum((i,row),sam(i,row))
                                    - sum((i,row),sam(row,i));
*
*    --- apply yearly growth rate to define targeted GDP in each year
*
     loop(t $ (not t0(t)),
      p_GDP(t)                    = (1 + p_gdpProj(t) / 100) * p_GDP(t-1);
     );
*
*    --- check that first year of sim is not smaller than first year of GDP or population projections
*
     abort $ (not sum(age,p_popage0("%baseYear%",age)) AND not p_GDPProj("%baseYear%"))
              "Check base year; both GDP growth and population are zero, in file: %system.fn%, line: %system.incline%";
*
*    --- GDP per capita in each year and its growth rate
*
     p_GDPpc(ttCal)                       = p_GDP(ttCal) / sum(age,p_popAge0(ttCal,age));
     p_GDPpcProj(ttCal) $ (not t0(ttCal)) = p_GDPpc(ttCal) / p_GDPpc(ttCal-1) * 100 - 100;
*
*    ----  Initialize population scenario
*
     p_popT(cohorts,ttCal)         = sum(mapage(age,cohorts),p_popAge0(tTcal,age));
*
*    --- set population cohorts
*
     pop.l(cohorts,ttCal)         = p_popT(cohorts,ttCal);
*
*    --- Per capita growth rates
*
     p_ggdppcT(ttCal) $ (not t0(ttCal))   = p_GDPpcProj(ttCal) * 0.01;
*
*    --- GDP per capita
*
     rgdppc.l(t0)              = rgdpmp.l(t0) / pop.l("ptotl",t0);
*
*    --- GDP growth rate
*
     ggdppc.l(t0)              = p_ggdppcT(t0);
*
*    --- GDP at factor cost and related price index
*
     gdpfc.l(t0)               = sum(flagXFac(actFp,a,v),pxfac.l(actFp,a,v,t0) * xfac.l(actFp,a,v,t0));

     rgdpfc.l(t0)              = gdpfc.l(t0);
     pgdpfc.l(t0)              = gdpfc.l(t0) / rgdpfc.l(t0);
*
*    --- GDP composition: share of aggregated final demand on real GDP
*
     rshrGDP.l(oa,t0)          = xfd.l(oa,t0) / rgdpmp.l(t0);
     nshrGDP.l(oa,t0)          = yf.l(oa,t0) / gdpmp.l(t0);
*
*    --- share of nipa components (= aggregated over final demanders fd)
*
     GdpCompShr.l(nipa,t0)     =  sum(mapn(fd,nipa), xfd.l(fd,t0))/rgdpmp.l(t0);
     GdpCompShr.l("Expo",t0)   =  rtotexp.l(t0)/rgdpmp.l(t0);
     GdpCompShr.l("Impo",t0)   = -rtotimp.l(t0)/rgdpmp.l(t0);

     cpi.l(t0)                 = pgdpmp.l(t0);
     walras.l(t0)              = 0;

     if ( execError, abort " ExecError encountered after initialiation of macro variables in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Tax flows and govenment balances
*
*    ----------------------------------------------------------------------------

     gtax.l(t0)           = sum((dtx, gov),sam(dtx,gov));
*
*    --- total government income with the exemption of new debt
*
     ygov.l(t0)           = sum((gov,is), sam(gov,is)) -  sum((gov,debt), sam(gov,debt));
     dgovtax.fx(ttCal)    = gtax.l(t0) / ygov.l(t0);
*
*    --- primary balances: government revenues (with the exemption of new debt)
*                          minus spendings not related to debt
*
     primaryBalance.l(t0) = ygov.l(t0) - gtax.l(t0)
                              - sum((inst,gov,instp) $ mapinst(gov,instp),transfers.l(inst, instp,t0))
                              - sum(gov,yf.l(gov,t0))
                              - sum(ginv,yf.l(ginv,t0))
                              - sum(map_adpInv(gov,adpinv),yf.l(adpinv,t0));
*
*    --- fiscal balance: add interest on debt, goverment balance: add principal
*
     fiscalBalance.l(t0)   = primaryBalance.l(t0) - sum(inst,debtInterest.l(inst,t0));
     govBalance.l(t0)      = fiscalBalance.l(t0)  - sum(instFD,debtPrincipal.l(instFD,t0));

     rgovBalance.l(t0)     = govBalance.l(t0)/pgdpmp.l(t0);

     loop(mapinst(gov,instp),
*       --- share of gov transfers spent on GDP
        chiTotTr.fx(instp,ttCal) $ ygov.l(t0)               = totTr.l(instp,t0) / gdpmp.l(t0);
        chiTotTr.fx(instp,ttCal) $ (not ygov.l(t0))         = 0;
     );

     chiTotTr.fx(instp,ttCal) $ (not chiTotTr.l(instp,ttCal))  = 0;
*
*    --- total foreign saving: contributions to new investments and to new government debt
*
     savF.l(t0) = (sum((inv,row),sam(inv,row)) + sam(debt,row)) / er.l(t0);
*
*    --- share of domestic institution on new government debt from all domestic institutions
*
     p_chiDebtNew(instD,ttCal) $ sam(debt,instD) = sam(debt,instD) / (sum(inst $ instD(inst),sam(debt,inst)));
*
*    --- only one foreign lender
*
     p_chiDebtNew(instF,ttCal) = 1;
*
*    --- total capital use and average returns to capital use
*
     kaps.l(t0)  = sum((cap,a),xfac.l(cap,a,old,t0));
     trent.l(t0) = sum((cap,a),pxfac_preTax.l(cap,a,old,t0) * xfac.l(cap,a,old,t0)) / kaps.l(t0);
*
*    --- distribution shares of factor income
*
     p_chiFac(inst,actFp,t0) $ sum(a,xfac.l(actFp,a,old,t0) * pxfac_pretax.l(actFp,a,old,t0))
           = sam(inst,actFp) / sum(a,xfac.l(actFp,a,old,t0) * pxfac_pretax.l(actFp,a,old,t0));
*
*    --- assign factor demand by activities
*
     actFmp(fmp) $ sum(sameas(fmp,actFp),1) = yes;
*
*    --- remove uniform distribution factor across activities derived from SAM
*        if distribution keys xfmSupDistrInst are given via the bridgefile
*
     p_chiFac(inst,actFp,t0) $ sum((instp,actfmp,fma) $ sameas(actFp,actFmp),xfmSupDistrInst(instp,actFmp,fma)) = 0;
     p_chiFac(inst,actFp,ttCal)                                                                                 = p_chiFac(inst,actFp,t0);
*
*    --- targets for the distribution of factor supply by institutions, either from xfmSupDistrInst or chiFac
*
     xfmSup.m(fmNestFmp(fma,actFmp),inst,oldf,t0) $ ((not sum(sameas(actFp,actFmp),p_chiFac(inst,actFp,t0))) $ sum(sameas(actFp,actFmp),sam(inst,actFp)))
      = sum((sameas(fma,a),actFp) $ sameas(actFp,actFmp),xfac.l(actFp,a,old,t0) * xfmSupDistrInst(inst,actFmp,fma));

     xfmSup.m(fmNestFmp(fma,actFmp),inst,oldf,t0) $ sum(sameas(actFp,actFmp),p_chiFac(inst,actFp,t0))
      = sum((sameas(fma,a),actFp) $ sameas(actFp,actFmp),xfac.l(actFp,a,old,t0) * p_chiFac(inst,actFp,t0));

     equation e_keepIncome(inst,fpNest)    "Make sure that institution receive the income by sector from the SAM"
              e_keepFactorCost(fpNest,a)   "Make suare that sectors face the costs from the sam"
              e_hpdXfmsup;
     variable hpdXfmsup;

     e_keepIncome(inst,actfp) $ sam(inst,actFp) ..
      1 =e= sum(fmNestFmp(fma,actFmp) $ (sameas(actFmp,actfp) $ xfmSup.m(fma,actFmp,inst,oldf,t0)),
                               xfmSup(fma,actFmp,inst,oldf,t0)*sum(sameas(fma,a),pxfac_pretax.l(actFp,a,old,t0)))/sam(inst,actFp);

     e_keepFactorCost(actfp,a) $ xfac.l(actFp,a,old,t0) ..
      1 =e= sum((sameas(fma,a),actFmp,inst) $ (sameas(actFp,actFmp) $ xfmSup.m(fma,actFmp,inst,oldf,t0)),
                                        xfmSup(fma,actFmp,inst,oldf,t0))/xfac.l(actFp,a,old,t0);

     e_hpdXfmSup ..
       hpdXfmsup * sum((fmNestFmp(fma,actFmp),inst) $ xfmSup.m(fma,actFmp,inst,oldf,t0),0.1)
               =e= sum((fmNestFmp(fma,actFmp),inst) $ xfmSup.l(fma,actFmp,inst,oldf,t0),
               sqr((xfmSup(fma,actFmp,inst,oldf,t0) - xfmSup.m(fma,actFmp,inst,oldf,t0))
                             / xfmSup.m(fma,actFmp,inst,oldf,t0)));
*
*    --- calculate income estimate from given allocation of factor demand to institutions
*
     parameter xfmSupSum(fmp,inst);
     xfmSupSum(actFmp,inst) = sum(fma,xfmsup.m(fma,actFmp,inst,oldf,t0)
                            * sum((actFp,sameas(fma,a)) $ sameas(actFmp,actfp),pxfac_pretax.l(actFp,a,old,t0)));
*
*    --- improve starting point by scaling factor supply institution and activity to match income flow from SAM
*
     xfmSup.l(fmNestFmp(fma,actFmp),inst,oldf,t0) $ xfmSup.m(fma,actFmp,inst,oldf,t0)
      = xfmSup.m(fma,actFmp,inst,oldf,t0) * sum(sameas(actFmp,actFp),sam(inst,actFp)) / xfmSupSum(actFmp,inst);

     xfmSup.lo(fmNestFmp(fma,actFmp),inst,oldf,t0) $ xfmSup.m(fma,actFmp,inst,oldf,t0) = 0;
     option nlp=conopt4;
     model m_balanceXfmsup / e_keepIncome, e_keepFactorCost, e_hpdXfmSup /;
     m_balanceXfmsup.solvelink=5;
     m_balanceXfmsup.iterlim  = 1000;
     solve m_balanceXfmsup using NLP minimizing hpdXfmsup;
*
*    --- calculate totals factor supply and assign prices
*
     xfmSup.l(fmNestFmp(fma,actFmp),"sumInst",oldf,ttCal)  = sum(inst,xfmSup.l(fma,actFmp,inst,oldf,t0));
     pfmSup.l(fmNestFmp(fma,actFmp),instTot,oldf,t0) $ xfmSup.l(fma,actFmp,instTot,oldf,t0)
      = sum((sameas(fma,a),actFp) $ sameas(actFp,actFmp), pxfac_preTax.l(actFp,a,old,t0));

     option kill=xfmSup.m;
*
*    --- remove xfmsup and pfmsup for individual institutions if chiFac is used
*
     xfmSup.l(fmNestFmp(fma,actFmp),inst,oldf,t0) $ sum(sameas(actFp,actFmp),p_chiFac(inst,actFp,t0)) = 0;
     pfmSup.l(fmNestFmp(fma,actFmp),inst,oldf,t0) $ sum(sameas(actFp,actFmp),p_chiFac(inst,actFp,t0)) = 0;
*
*    --- optional distribution of labor factor supply to old and new vintages
*
     $$iftheni.labVintages "%labVintages%"=="on"
*
*       --- initialization of new labor factor stock, assume average years in
*           working population of 35 years; 1/35 is the yearly expected share
*           of new entrates
*
        xfmSup.l(fmNestFmp(fma,actFmp),instTot,newf,t0) $ sum(sameas(l,actFmp),1)
          = xfmSup.l(fma,actFmp,instTot,oldf,t0) * 1/35;
*
*       --- the remainder is old stock already present in t-1
*
        xfmSup.l(fmNestFmp(fma,actFmp),instTot,oldf,t0) $ sum(sameas(l,actFmp),1)
          = xfmSup.l(fma,actFmp,instTot,oldf,t0) * 34/35;
*
*       --- same start prices
*
        pfmSup.l(fmNestFmp(fma,actFmp),instTot,newf,t0) $ sum(sameas(l,actFmp),1)
          = pfmSup.l(fma,actFmp,instTot,oldf,t0);

     $$endif.labVintages
*
*    --- aggregate factor supply over CET nesting structure
*
     loop(fmPar1,
         xfmSup.l(fmNestFmp(fmpar,actFmp),instTot,vf,t0) = sum(fmNesting(fmChild,fmPar,actFmp),xfmSup.l(fmChild,actFmp,instTot,vf,t0));
         pfmSup.l(fmNestFmp(fmpar,actFmp),instTot,vf,t0) $ xfmSup.l(fmpar,actFmp,instTot,vf,t0)
          = sum(fmNesting(fmChild,fmPar,actFmp),xfmSup.l(fmChild,actFmp,instTot,vf,t0) * pfmSup.l(fmChild,actFmp,instTot,vf,t0))
            / xfmSup.l(fmpar,actFmp,instTot,vf,t0);
     );
     pfmSup.l(fmNestFmp(fmpar,actFmp),instTot,vf,ttCal) = pfmSup.l(fmpar,actFmp,instTot,vf,t0);

     xfmSupUpdateFac.fx(actFmp,instTot,ttCal) $ sum(vf,xfmSup.l(actfmp,actFmp,instTot,vf,t0)) = 1;
*
*    --- assign land cover (actHa are part of the land accounts, excludes water)
*
     lCovArea.l(actHa,"cropLand",t0)     = sum(crops,xfac.l(actHa,crops,old,t0)) + eps;
     lCovArea.l(actHa,"pastureLand",t0)  = sum(anim,xfac.l(actHa,anim,old,t0))   + eps;
     lCovArea.l(actHa,"forest",t0)       = sum(frs,xfac.l(actHa,frs,old,t0))     + eps;

     lCovMng(lCov) $ sum(actHa,lCovArea.l(actHa,lcov,t0)) = yes;
     lCovDrv(lCov) $ sum(actHa,lCovArea.l(actHa,lcov,t0)) = yes;
*
*    -- stock of unmanaged land from given data
*
     lCovUnMng(lCov) $ ((not sum(actHa,lCovArea.l(actHa,lcov,t0))) $ card(land0)) = yes;
     lCovArea.l(actHa,lcovUnMng,ttCal) = sum(mapis(fp0,actHa),lCov0(fp0,lCovUnMng))*0.001 * inscale*usd;

     lCovDrv("builtUpLand")   = yes;
     lCovUnMng("builtUpLand") = no;
     option kill=dCarbon;

     $$iftheni.landTransMat not defined lndTransMat
*
*       --- if no land transititon factors are found in bridgefile, use proportionality assumptions
*
        if (card(land0),
           lndTransMat(actHa,lCovDrv,lCovUnMng) $ (not sameas(lCovUnMng,"otherLand"))
            = lCovArea.l(actHa,lCovUnMng,t0);
           lndTransMat(actHa,lCovDrv,lCovUnMng) $ sum(lCov,lndTransMat(actHa,lCovDrv,lCov))
            = lndTransMat(actHa,lCovDrv,lCovUnMng) / sum(lCov,lndTransMat(actHa,lCovDrv,lCov));
           );
     $$endif.landTransMat
*
*    --- copy of price for volume preserving nests
*
     pfmSupD.l(fmNestFmp(fmPar,actFmp),instTot,vf,t0) $ isAdditiveCet(fmPar) = pfmSup.l(fmPar,actFmp,instTot,vf,t0);

     fmNestFmp(fma,actFmp) $ (not sum(instTot,xfmSup.l(fma,actFmp,instTot,oldf,t0))) = no;

     fmNestFmp(fmall,actFmp) $ (ifVint and sum(sameas(actFmp,cap),1)) = no;
     fmParBot(fmPar,actFmp)  $ sum(fmNesting(fma,fmPar,actFmp),1)     = yes;
*
*    --- transformation elasticities for factor nests, including short-run version
*
     p_omegaFmp(fmNest,vf)      = fmNestPar("omegaCET",fmNest);
     p_omegaFmpShort(fmNest,vf) = fmNestPar("omegaCETShort",fmNest);
*
     $$iftheni.labVintages "%labVintages%"=="on"
*
*          --- at least CNS does not work properly if the labor vintage nests have a cet-elastistic of inf
*
         p_omegaFmp(fmNest,vf)  $ (sum((fmNestFmp(fmNest,actFmp),sameas(actFmp,l)),1) $ (p_omegaFmp(fmNest,vf) eq inf)) = 5;
         p_omegaFmp(fmpar,newf) $  sum((fmNestFmp(fmPar,actFmp), sameas(actFmp,l)),1)                                   = p_omegaFmp(fmpar,oldf)*2;

     $$endif.labVintages
*
     p_omegaFmp(fmPar,vf)   $ ((sum((fmNesting(fmChild,fmPar,actFmp),instTot) $ xfmSup.l(fmChild,actFmp,instTot,vf,t0),1) eq 1)
                                  $ (not isAdditiveCet(fmPar)))   = inf;

     p_omegaFmp(fmAll,vf)   $ (p_omegaFmp(fmAll,vf) eq eps) = 0;
*
*    --- share parameters for CET nests
*
     p_gammaFmp(fmNestFmp(fmChild,actFmp),instTot,vf,ttCal) $ (sum(fmNesting(fmChild,fmPar,actFmp), p_omegaFmp(fmPar,vf) eq inf)
                                                                                $ xfmSup.l(fmChild,actFmp,instTot,vf,t0)) = 1;
*
     p_gammaFmp(fmNestFmp(fmChild,actFmp),instTot,vf,ttCal) $ (sum(fmNesting(fmChild,fmPar,actFmp), p_omegaFmp(fmPar,vf) ne inf)
                                                                                $ xfmSup.l(fmChild,actFmp,instTot,vf,t0))
      = sum(fmNesting(fmChild,fmPar,actFmp),xfmSup.l(fmChild,actFmp,instTot,vf,t0) / xfmSup.l(fmPar,actFmp,instTot,vf,t0)
                                         * (pfmSup.l(fmpar,actFmp,instTot,vf,t0) / pfmSup.l(fmChild,actFmp,instTot,vf,t0))**p_omegaFmp(fmPar,vf));
*
*    --- factor which line up total income by factor with results of CET allocation system
*
     corrFac.fx(actFp,ttCal) = 1;
     corrFac.lo(actFp,ttCal) $ sum((fmNesting(fmAll,fmPar,actFmp),vf) $ sameas(actFmp,actfp),
        [(p_omegaFmp(fmPar,vf) eq inf) and (not sameas(fmAll,actFmp))]) = -inf;
     corrFac.up(actFp,ttCal) $ sum((fmNesting(fmAll,fmPar,actFmp),vf) $ sameas(actFmp,actfp),
        [(p_omegaFmp(fmPar,vf) eq inf) and (not sameas(fmAll,actFmp))]) = +inf;
*
*    --- factor supply elasticities
*
     p_epsFmp(fmNest,vf)                                         = fmNestPar("factSupElas",fmNest);
     p_epsFmpShort(fmNest,vf)                                    = fmNestPar("factSupElasShort",fmNest);
     p_epsFmA(fma,actFmp,vf)  $ (p_omegaFmp(actFmp,vf)   eq eps) = pfmNest(fma,actFmp);
*
     p_epsFmA(fma,actFmp,vf)  $ (p_epsFmA(fma,actFmp,vf) eq eps) = 0;
     p_epsFmp(fmAll,vf)       $ (p_epsFmp(fmAll,vf)      eq eps) = 0;
     p_epsFmpShort(fmAll,vf)  $ (p_epsFmpShort(fmAll,vf) eq eps) = 0;
*
*    --- constant terms in double-log factor supply equations
*
     atFmSup.fx(actFmp,instTot,vf,ttCal) $ xfmSup.l(actFmp,actFmp,instTot,vf,t0)
         = xfmSup.l(actFmp,actFmp,instTot,vf,t0) / (pfmSup.l(actFmp,actFmp,instTot,vf,t0) / pgdpmp.l(t0))**p_epsFmp(actFmp,vf);

     if (not ifVint,
        atFmSup.lo(fmp,instTot,oldf,ttCal) $ sum(sameas(fmp,cap),1) = -inf;
        atFmSup.up(fmp,instTot,oldf,ttCal) $ sum(sameas(fmp,cap),1) = +inf;
     );
*
*    --- short run elasticities impact factor use intensity
*
     p_epsFmpIntens(fpNest,a)                                = actPar(a,"intensMargElas",fpNest);
     p_epsFmpIntens(l,a)      $ (not p_epsFmpIntens(l,a))    = actPar(a,"intensMargElas","labor");
     p_epsFmpIntens(cap,a)    $ (not p_epsFmpIntens(cap,a))  = actPar(a,"intensMargElas","capital");
*
*    --- sticky wage parameter
*
     p_stickyWage(actFmp)                           = 0;
     p_stickyWage(actFmp) $ sum(sameas(actFmp,l),1) = %stickyWage%;
*
*    --- unemployment
*
     unemp.l(t0) = (pop.l("P1564",t0) - sum((sameas(l,actFmp),vf),xfmSup.l(actFmp,actFmp,"suminst",vf,t0)));
*
*    --- average rental rate
*
     trentAvg.l(t0) = trent.l(t0);
*
*    --- demographics: assign population projections by age
*
     popAge.l(age,ttCal) = p_popAge0(ttCal,age);
     popAge.fx(age,t0)   = popAge.l(age,t0);
*
*    --- birth rate: population at lowest age relative to 15-64 cohort in previous year
*
     brateU.fx(ttCal) $ (years(ttCal) > years(t0))  = sum(age $ (ord(age) eq 1),popAge.l(age,ttCal)) / pop.l("P1564",ttCal-1);
*
*    --- survival rate
*
     drate.fx(age,ttCal)   $ (ord(ttCal)>1 and ord(age)>1) = popAge.l(age,ttCal) / popAge.l(age-1,ttCal-1);
*
*    --- Investment efficiency
*
     p_PhiPcInv(inv,ttCal)                                 = defPar('PhiPcInv0');
     p_PhiPcInv(inv,ttcal) $ othPar(inv,"PhiPcInv0")       = othPar(inv,"PhiPcInv0");
     p_nonProdInv(inv,ttCal)                               = 0;
*
*    --- Capital stock derived from real GDP and factor
*
     kstock.l(t0) = defPar('capoutRate0') * rgdpmp.l(t0);
*
*    --- Return to capital stock rate: total capital earnings over stock
*
     ror.l(t0) = kaps.l(t0) / kstock.l(t0);
*
*    --- depreciation rate by sector
*
     deprA.fx(a,ttCal)                                 = defPar('depr0');
     deprA.fx(a,ttCal) $ (actPar(a,"depr","") gt eps)  = actPar(a,"depr","");
*
*    --- average depreciation rate, using captial use by sector as weight
*
     depr.l(t0) = sum((cap,a), sam(cap,a)*defPar('depr0')) / sum((cap,a),sam(cap,a));
*
*    --- cost corrected return to captial rate: capital rent corrected by price of investments
*        minus depreciation
*
     rorc.l(t0) = trent.l(t0)/sum(inv,pf.l(inv,t0)) - depr.l(t0);

     if ( execError, abort " ExecError encountered after initialiation of factor market variables in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Initialize vintage related variables
*
*    ----------------------------------------------------------------------------

*
*    --- the ondotl is needed for marco m_dmgAgg which comprises variables
*
     $$ondotl
     oldCap.l(aact(a),ttCal(t0)) = sum(cap,xfac.l(cap,a,old,t0)) * (1-depr.l(t0) * sum(sameas(t,t0),m_dmgAgg(dmgDepr,'a')));
     $$offdotl

     rrat1.l(a,t0) = 1;
     rrat.l(a,t0)  = 1;
     trent.l(t0)   = 1;

*    ----------------------------------------------------------------------------
*
*     Forward looking behavior
*
*    ----------------------------------------------------------------------------

     $$iftheni.forwardLooking "%ForwardLooking%"=="on"

       p_alphaSt(i,ttCal)  $ (defPar('alphaSt0') gt eps)   = defPar('alphaSt0');
       p_alphaSt(i,ttCal)  $ comPar(i,'alphaSt0')          = comPar(i,'alphaSt0') $ (comPar(i,'alphaSt0') gt eps);

       ifDelSt             $ sum(i,p_alphaSt(i,t0))        = 1;

       ist(i)              $ (iDom(i) and p_alphaSt(i,t0)) = yes;
       p_alphaSt(i,ttCal)  $ (not ist(i))                  = 0;

       p_rhoXDelSt(ist(i))                                 = defPar('rhoXDelSt0');
       p_rhoXDelSt(ist(i)) $ comPar(i,'rhoXDelSt0')        = comPar(i,'rhoXDelSt0');

       p_rhoPDelSt(ist(i))                                 = max(0.5,defPar('rhoPDelSt0'));
       p_rhoPDelSt(ist(i)) $ comPar(i,'rhoPDelSt0')        = max(0.5,comPar(i,'rhoPDelSt0'));

       psExp.l(ist(i),t0)   $ p_alphaSt(i,t0)               = ps.l(i,t0);
       delst.l(ist(i),t0)                                   = 0;
       st.l(ist(i),t0)                                      = p_alphaSt(i,t0) * xs.l(i,t0);

       delst.lo(ist(i),ttCal)  $ p_alphaSt(i,ttCal)                = -inf;
       delst.up(ist(i),ttCal)  $ p_alphaSt(i,ttCal)                = +inf;

     $$else.forwardLooking

        option kill=p_alphaSt,kill=p_rhoXDelSt,kill=p_rhoPDelSt,kill=psExp,kill=xsExp;

     $$endif.forwardLooking

*    ----------------------------------------------------------------------------
*
*     Emissions and emission inventotiry
*
*    ----------------------------------------------------------------------------
*
*    --- aggregate product use related emisions to level of SAM accounts
*
     emiComm.l(em,i,aa,t0)    $ emiCommFlag(em,i,aa)  = reEscale * emiGTAP(em,i,aa) * 1/1000;
*
*    --- aggregate primary factor use related emisions to level of SAM accounts
*
     emiFact.l(em,actFp,a,t0)             = reEscale * emiGTAP(em,actFp,a) * 1/1000;
*
*    --- aggregate emisions related to outout to level of SAM accounts
*
     emiXP.l(em,a,t0)                     = reEscale * emiXPGtap(em,a) * 1/1000;
     emiOth.fx(em,ttCal)                  = 0;
     option kill=emiLucf.l;

     reEscale                             = 1/reEscale;

*    ---- define emissions by armington agent

     emiSecA.l(em,aa,t0)                  = sum(emiCommFlag(em,i,aa),emiComm.l(em,i,aa,t0))
                                          + sum((actFp,a)$sameas(aa,a),emiFact.l(em,actFp,a,t0))
                                          + sum(sameas(aa,a),emiXP.l(em,a,t0))
                                          + sum(sameas(aa,a),emiLUCF.l(em,a,t0));

     emiFlag(em,aa) $ emiSecA.l(em,aa,t0) = yes;
*
*    --- emission totals
*
     emiTot.l(em,t0)                   = sum(aa,emiSecA.l(em,aa,t0));
     emiGHG.l(polltype,t0)             = sum(em,p_emiConv(em,polltype) * emiTot.l(em,t0)) + sum(em,p_emiConv(em,polltype) * emiOth.l(em,t0));

*    --- define detailed emission inventory

     emiInv.l(ghgInv,em,t0)
           =     sum(emiCommFlag(em,i,aa) $ p_ghgInvAlloc(ghgInv,em,i,aa),     p_ghgInvAlloc(ghgInv,em,i,aa)      * emiComm.l(em,i,aa,t0))
               + sum((actFp,a)            $ p_ghgInvAlloc(ghgInv,em,actFp,a),  p_ghgInvAlloc(ghgInv,em,actFp,a)   * emiFact.l(em,actFp,a,t0))
               + sum(a                    $ p_ghgInvAlloc(ghgInv,em,"emiXP",a),p_ghgInvAlloc(ghgInv,em,"emiXP",a) * [emiXP.l(em,a,t0) + emiLUCF.l(em,a,t0)]);

     emiInt.l(polltype,t0)             = emiGHG.l(polltype,t0) / rgdpmp.l(t0);
*
*    --- emission intensity for each activity, and for households
*
     emiIntSec.l(a,t0) $ xp.l(a,t0) = sum(emiFlag(ema,a),emiSecA.l(ema,a,t0)) / xp.l(a,t0);
     emiIntHH.l(h,t0)               = sum(emiFlag(ema,h),emiSecA.l(ema,h,t0)) / yf.l(h,t0);
*
*    --- energy and electricity intensity per real GDP
*
     nrgInt.l(t0)                      = sum(e,xat.l(e,t0))/rgdpmp.l(t0);
     elyInt.l(t0)                      = sum(iElec,xat.l(iElec,t0))/rgdpmp.l(t0);
*
*    --- initialize emission taxes to zero
*
     tEmi0.fx(emiFlag(em,aa),ttCal)       = 0;
     tEmiD0.fx(ttCal)    $ card(emiFlag)  = 0;
     tEmiD1.fx(em,ttCal) $ card(emiFlag)  = 0;
     tEmiD2.fx(aa,ttCal) $ card(emiFlag)  = 0;
     tEmiM0.fx(ttCal)    $ card(emiFlag)  = 1;
     tEmiM1.fx(em,ttCal) $ card(emiFlag)  = 1;
     tEmiM2.fx(aa,ttCal) $ card(emiFlag)  = 1;
     tEmi.l(em,aa,t0) $ card(emiFlag)  = (tEmi0.l(em,aa,t0) + (tEmiD0.l(t0) + tEmiD1.l(em,t0) + tEmiD2.l(aa,t0)))
                                                            * tEmiM0.l(t0) * tEmiM1.l(em,t0) * tEmiM2.l(aa,t0);
     rtEmi.l(em,aa,t0)                 = tEmi.l(em,aa,t0);

     emical.fx(ttCal)                     = 1;

     emiGhg.l("PMTF",tf) $ (not emiGhg.l("PMTF",tf)) = emiGhg.l("PMTF",t0);
     exposedAirpoll.l(ttCal)                         = sum(tf $ ((years(tf) le years(ttCal)) and (years(ttCal) - years(tf) < 5)),emiGhg.l("PMTF",tf));

     if ( execError, abort " Execerror encountered after initialiation of emission variables in file: %system.fn%, line: %system.incline%");

*    ----------------------------------------------------------------------------
*
*     Calibrate equations distributing production output
*
*    ----------------------------------------------------------------------------

     lambdaNRG.fx(ttCal) = 0;

     pInd.l(i,t0)  = ps.l(i,t0);
*
*    --- share factor for allocation of multiple products from one activity
*
     p_ap(a,i,ttCal) $ (x.l(a,i,t0) ne 0) = (  (x.l(a,i,t0)/xp.l(a,t0)) * (pp.l(a,t0) / p.l(a,i,t0))**p_omegaP(a))   $ (p_omegaP(a) ne inf)
                                             + (p.l(a,i,t0)*x.l(a,i,t0) / (pp.l(a,t0) * xp.l(a,t0)))                  $ (p_omegaP(a) eq inf);
*
*    --- share factor for domestic sourcing of same product from multiple activities
*
     p_as(a,i,ttCal) $ (x.l(a,i,t0) ne 0) = (  (x.l(a,i,t0)/xs.l(i,t0)) * (p.l(a,i,t0) / pInd.l(i,t0))**p_sigmaS(i)) $ (p_sigmaS(i) ne inf)
                                             + (p.l(a,i,t0)*x.l(a,i,t0) / (ps.l(i,t0) * xs.l(i,t0)))                  $ (p_sigmaS(i) eq inf);

     p_sigmaS(i)     $ (sum(a $ p_as(a,i,t0),1) eq 1) = inf;
*
*    --- average yield per ha of land (needed for AEZ and similar split-up of total land). Excludes water
*
     yieldLnd.l(a,t0) $ sum(flagXFac(actHa,a,v),xfac.l(actHa,a,v,t0))
      = sum(i $ p_as(a,i,t0),xs.l(i,t0)) / sum(flagXFac(actHa,a,v),xfac.l(actHa,a,v,t0));
*
*    --- electricity mix (make matric related)
*
     elecMix.l(aElec,iElec,t0) $ xs.l(iElec,t0) = x.l(aElec,iElec,t0) / sum(aElecp,x.l(aElecp,iElec,t0));
*
*    --- if only one output of an activity, set the transformation elasticity to inifinite (= phyiscal balance)
*
     p_omegaP(a) $ (sum(i $ p_ap(a,i,t0),1) eq 1 ) = inf;
*
*    --- if only one activity is producting an product, set substitution elasticity to infinite (= physical balance)
*

*    ----------------------------------------------------------------------------
*
*     Calibrate final demand equations
*
*    ----------------------------------------------------------------------------

*    --- Calibrate demand nests for households

     p_sigmaC(oa,kNest)                                          = sigmacNest0(kNest) $ (sigmacNest0(kNest) gt eps);
     p_sigmaC(oa,kNest) $ othPar(oa,kNest)                       = othPar(oa,kNest)   $ (othPar(oa,kNest)   gt eps);
     p_sigmaC(oa,kNest) $ (p_sigmaC(oa,kNest) eq 1)              = 1.01;

     akfAct(oa,topKNest)       $ (xakshr.l(oa,topKNest,t0) ne 0) = yes;
     xaact(i,aa)               $ (xa.l(i,aa,t0)            ne 0) = yes;
     xakact(oa,knest)          $ (xak.l(oa,knest,t0)       ne 0) = yes;
*
*    --- demand for single product in nests
*
     p_transmat(i,knest,oa) = SAM(i,oa)*mapkNestShr(i,knest) ;
     p_af(oa,i,knest,ttCal)$( (xak.l(oa,knest,t0) ne 0) $ mapkNestShr(i,kNest))
       = ((p_transmat(i,kNest,oa)/((1+tpaf.l(i,oa,t0))*pa.l(i,t0)))/xak.l(oa,knest,t0))*(((1+tpaf.l(i,oa,t0))*pa.l(i,t0))/pxak.l(oa,knest,t0))**p_sigmaC(oa,knest);
*
*    --- demand for nests in nests
*
     p_afk(oa,kNest,kNest1,ttCal)$((xak.l(oa,kNest,t0) ne 0) and mapkNestShr(kNest,kNest1))
       = (xak.l(oa,kNest,t0)/xak.l(oa,kNest1,t0))*(pxak.l(oa,kNest,t0)/pxak.l(oa,kNest1,t0))**p_sigmaC(oa,kNest1);
*
*    --- set related flags
*
     akfAct(oa,topKNest)       $ (xakshr.l(oa,topKNest,t0)                                     ne 0) = yes;
     xaact(i,aa)               $ (xa.l(i,aa,t0)                                                ne 0) = yes;
     xakact(oa,knest)          $ (xak.l(oa,knest,t0)                                           ne 0) = yes;
     conscont(oa,kNest)        $ (sum(kNest1,mapkNestShr(kNest,kNest1)) and xak.l(oa,kNest,t0) ne 0) = yes;
*
*    ---- Other final demand, demand for bundles

     p_sigmaF(f)                       = defPar('sigmaf0')    $ (defPar('sigmaf0')    gt eps);
     p_sigmaF(f) $ othPar(f,"sigmaf0") = othPar(f,"sigmaf0")  $ (othPar(f,"sigmaf0")  gt eps);

     p_sigmaF(f) $ (p_sigmaF(f) eq 1)  = 1.01;

     p_akf(f,topKNest,ttCal) $ (xak.l(f,topKNest,t0) ne 0)
      = (xak.l(f,topKNest,t0) / xfd.l(f,t0)) * (pxak.l(f,topKNest,t0) / pf.l(f,t0))**p_sigmaF(f);

     p_akf(adpinv,topKNest,ttCal) $ (xfd.l(adpinv,t0) ne 0) = sum(inv,p_akf(inv,topKNest,t0));

     xaact(i,aa) $ (xa.l(i,aa,t0) ne 0) = yes;

*    --- HD: this is not exact but works for now
     mmuh.l(h,t0)        = yf.l(h,t0);
*
*    --- scale given data on population by household to match total populaation
*
     p_hpop0(h) $ hohpar(h,"hpop0","") = hohpar(h,"hpop0","");
     p_hpop0(h) $ p_hPop0(h)           = p_hpop0(h) * pop.l("ptotl",t0)/sum(hp,p_hpop0(hp));
*
*    --- estimate population by household from labor stocks, if not given
*
     p_hpop0(h) $ (not card(p_hPop0)) = pop.l("ptotl",t0)*sum(sameas(l,actFmp) $ sam(h,l),sam(h,l) / pfmSup.l(actFmp,actFmp,"sumInst",oldf,t0))
                                                        / sum((sameas(l,actFmp),hp) $ sam(hp,actFmp),sam(hp,actFmp) / pfmSup.l(actFmp,actFmp,"suminst",oldf,t0));
*
*    --- tax redistribution parameter (for yTaxRedist): default assumtion is same absolute ammount per capita
*
     chiTaxRedist.fx(inst,ttCal) = sum(mapInst(h,inst),p_hpop0(h) / sum(hp,p_hpop0(hp)));

     hpop.l(h,t0)  = p_hpop0(h);
*
*    --- scale nutrient content of products to match in average the FAO data
*
     $$iftheni.nutrients defined nutCont

        parameter p_nutrMatch;
*
*       --- total nutrient supply to all households and government per capita
*
        p_nutrMatch(nutr,"MNG")     = sum(i,[sum(h,xa.l(i,h,t0))+sum(gov,xa.l(i,gov,t0))]*nutCont(i,nutr))/sum(h,hpop.l(h,t0));
*
*       --- reported total nutrient supply by FAO for 2017
*
        p_nutrMatch(nutr,"FAO")     = nutCont("sum",nutr);
*
*       --- calculate scaling factor
*
        p_nutrMatch(nutr,"scale") = 1;
        p_nutrMatch(nutr,"scale") $ (p_nutrMatch(nutr,"MNG") and p_nutrMatch(nutr,"FAO"))
            = p_nutrMatch(nutr,"FAO")/p_nutrMatch(nutr,"MNG");
*
*       --- scale all nutrient content to match
*
        nutCont(i,nutr) = nutCont(i,nutr)*p_nutrMatch(nutr,"scale");
     $$endif.nutrients
*
*    --- dependency rates: population of each cohort over population in working age
*
     deprate.l(cohorts,t0)    = pop.l(cohorts,t0)/pop.l("P1564",t0);
     deprate.fx(cohorts,t0)   = deprate.l(cohorts,t0);

*    ---------------------------------------------------------------------
*
*        calibration of CDE demand system
*
*    ---------------------------------------------------------------------

     $$include 'inc/cal/calCDE.inc'

*    ----------------------------------------------------------------------------
*
*     Calibrate trade equations
*
*    ----------------------------------------------------------------------------

*
*    --- CES subsititution between imports and domestic sales (Armington)
*
*
*    --- price shfiters
*
     p_chiPDa(i) = 1;
     p_chiPMa(i) = 1;
     p_chiPA(i)  = 1;

     paIND.l(i,t0) = pa.l(i,t0);

*    --- share parameters for domestic and imported origin

     p_ad(iDom(i),ttCal) = (xd.l(i,t0)/xat.l(i,t0))*(pda.l(i,t0)/paInd.l(i,t0))**p_sigmaM(i);
     p_am(iImp(i),ttCal) = (xm.l(i,t0)/xat.l(i,t0))*(pma.l(i,t0)/paInd.l(i,t0))**p_sigmaM(i);
*
*    --- preference shifters
*
     p_gammamd(i,ttCal) = 1;
     p_gammamm(i,ttCal) = 1;

*    --- Re-normalize prices

     paf.l(i,aa,t0) $ sam(i,aa)  = (1 + tpaf.l(i,aa,t0))*pa.l(i,t0);
     psInd.l(i,t0) = ps.l(i,t0);
*
*    --- CET allocation domestic sales and exports
*
     p_omegaX(i)  = defPar('omegax0');
     p_omegaX(i)  $ comPar(i,"omegax0") = comPar(i,"omegax0") $ (comPar(i,"omegax0") gt eps);

*    --- share parameters for domestic sales and exports

     p_gd(iAct(i),ttCal) = ((xd.l(i,t0)/(xs.l(i,t0) - delst.l(i,t0)))*(ps.l(i,t0)/pd.l(i,t0))**p_omegaX(i)) $ (p_omegaX(i) ne inf)
                          + (pd.l(i,t0)*xd.l(i,t0)/(ps.l(i,t0)*(xs.l(i,t0) - delst.l(i,t0))))               $ (p_omegaX(i) eq inf);

     p_ge(iAct(i),ttCal) = ((xe.l(i,t0)/(xs.l(i,t0) - delst.l(i,t0)))*(ps.l(i,t0)/pe.l(i,t0))**p_omegaX(i)) $ (p_omegaX(i) ne inf)
                          + (pe.l(i,t0)*xe.l(i,t0)/(ps.l(i,t0)*(xs.l(i,t0) - delst.l(i,t0))))               $ (p_omegaX(i) eq inf);

     p_omegaX(i) $ ((p_gd(i,t0) eq 0) or (p_ge(i,t0) eq 0)) = inf;
*
*    --- shiifters for CET equations
*
     p_gammaxd(i,ttCal) = 1;
     p_gammaxe(i,ttCal) = 1;

*    --- Re-normalize prices

     p_chiPD(i)    = pd.l(i,t0);
     pd.l(i,t0)    = 1;
     p_chiPE(i)    = pe.l(i,t0);
     pe.l(i,t0)    = 1;

*    --- Re-normalize demand side of make matrix

     chiP.fx(a,i,ttCal) $ p_ap(a,i,ttCal) = p.l(a,i,t0);
     p.l(a,i,ttCal)     $ p_ap(a,i,ttCal) = 1;

     p_gammap(a,i,ttCal) $ x.l(a,i,t0) = ps.l(i,t0) / (p.l(a,i,t0)*chiP.l(a,i,t0));

     p_chiPS(i)     = ps.l(i,t0);
     ps.l(i,t0)     = 1;
     psInd.l(i,t0)  = 1;
     $$iftheni.forwardLooking "%ForwardLooking%"=="on"
       psExp.l(ist(i),ttCal) $ p_alphaSt(i,ttCal)  = ps.l(i,t0);
       xsExp.l(ist(i),ttCal) $ p_alphaSt(i,ttCal)  = xs.l(i,t0);
     $$endif.forwardLooking
*
*    --- export demand function
*
     p_etae(iExp(i))                     = defPar('etae0');
     p_etae(iExp(i))$ comPar(i,"etae0")  = comPar(i,"etae0");
     p_chie(iExp(i),ttCal) $ (p_etae(i) ne inf) = xe.l(i,t0)*(pwe.l(i,t0)/pweROW.l(i,t0))**p_etae(i);

     p_chiPWE(i)  = pwe.l(i,t0);
     p_chiPWEx(i) = pweROW.l(i,t0);
     p_gammaPWE(i,ttCal) = p_chiPWEx(i)/p_chiPWE(i);

     pwe.l(i,t0)        = 1;
     pweROW.fx(i,ttCal) = 1;
*
*    --- import supply function
*
     p_omegam(i)                         = defPar('omegam0');
     p_omegam(i)  $ comPar(i,"omegam0")  = comPar(i,"omegam0") $ (comPar(i,"omegam0") gt eps);

     p_chim(iImp(i),ttCal) $ (p_omegam(i) ne inf) = xm.l(i,t0)*(pwmROW.l(i,t0)/pwm.l(i,t0))**p_omegam(i);

*    --- Re-normalize energy share coefficients in production

     p_alphaXfac(fpNestI,fppar,aact(a),v) $ (sum(sameas(fpNestI,e),1) $ xfac.l(fppar,a,old,t0))
       = p_alphaXfac(fpNestI,fppar,a,v) * sum(sameas(fpNestI,e),p_chiPA(e))**(-p_sigmaxfac(fppar,a,v));

     leontief(a,fpChl) = (not sum(fpPar $ (pNest(a,fpChl,fpPar) and sum(v,p_sigmaxfac(fpPar,a,v) gt eps)),1));

     bot.l(t0) = sum(i, p_chiPWE(i)*pwe.l(i,t0)*xe.l(i,t0) $ iExp(i) - pwm.l(i,t0)$ iImp(i)*(xm.l(i,t0)  +mdelst.l(i,t0)));
     botGdp.l(t0) = bot.l(t0)/gdpMp.l(t0);

*    ----------------------------------------------------------------------------
*
*     Calibrate margin equations
*
*    ----------------------------------------------------------------------------

     p_amarg(i,j) $ (sam(i,j) ne 0) = (xa.l(i,j,t0)/xmarg.l(j,t0))
                                    * (p_chiPA(i)*pa.l(i,t0)/pmarg.l(j,t0))**p_sigmaMg(j);

     xmarg.fx(j,ttCal) $ (xmarg.l(j,t0) eq 0) = 0;
     pmarg.fx(j,ttCal) $ (xmarg.l(j,t0) eq 0) = 1;
     xa.l(i,j,t0)      $ (p_amarg(i,j)  eq 0) = 0;

*    ----------------------------------------------------------------------------
*
*     Calibrate factor market equations
*
*    ----------------------------------------------------------------------------

     p_invElas(a) = defPar('invElas0');

*    ----------------------------------------------------------------------------
*
*     Calibrate emission equations
*
*    ----------------------------------------------------------------------------

*    ---- Assume full participation by all agents in carbon taxation
*         and define emission factors accordingly

     p_rhoEmi(em,i,aa)        $ (emiComm.l(em,i,aa,t0)     ne 0) = emiComm.l(em,i,aa,t0)    /xa.l(i,aa,t0);
     p_rhoEmiFac(em,cap,a)    $ (emiFact.l(em,cap,a,t0)    ne 0) = emiFact.l(em,cap,a,t0)   /xfac.l(cap,a,old,t0);
     p_rhoEmiFac(em,actLnd,a) $ (emiFact.l(em,actLnd,a,t0) ne 0) = emiFact.l(em,actLnd,a,t0)/xfac.l(actLnd,a,old,t0);
     p_rhoEmiXP(em,a)         $ (xp.l(a,t0) ne 0)                = emiXP.l(em,a,t0)         / xp.l(a,t0);
*
*    --- emission factor shifters, fix to unity
*
     chiEmi.fx(em,i,aa,ttCal)        $ emiCommFlag(em,i,aa)     = 1;
     chiEmiFac.fx(em,actFp,aa,ttCal) $ p_rhoEmiFac(em,actFp,aa) = 1;
     chiEmiXP.fx(em,a,ttCal)         $ emiFlag(em,a)            = 1;
     chiEmiAA.fx(aa,ttCal)                                      = 1;

     p_phiEmi(em,i,aa)       $ ((not ema(em)) $ emiCommFlag(em,i,aa))    = 1;
     p_phiEmiXP(em,a,ttCal)  $ ((not ema(em)) $ emiFlag(em,a))           = 1;
     p_phiEmiFac(em,actFp,a) $ ((not ema(em)) $ p_rhoEmiFac(em,actFp,a)) = 1;
*
*    --- default assumption is that we are not tracking carbon stock changes.
*        equation is active, using requires to define the factors in shock file
*        (use e.g. CAIT and past land use change in ha to calibrate the emission factor
*         in SAM units. emiFlag might need to be activated as well)
*
     p_rhoEmiLnd("CO2",actLnd,a) = 0;

*    ----------------------------------------------------------------------------
*
*     Tax revenue flows
*
*    ----------------------------------------------------------------------------

     ygPaf.l(i,t0)        = sum(aa,tPaf.l(i,aa,t0)*p_chiPA(i)*pa.l(i,t0)*xa.l(i,aa,t0));
     ygEmi.l(aa,t0)       = sum((em,i), tEmi.l(em,aa,t0)*emical.l(t0)*chiEmi.l(em,i,aa,t0)*p_rhoEmi(em,i,aa)*p_phiEmi(em,i,aa)*xa.l(i,aa,t0));
     ygEmiXP.l(a,t0)      = sum((em),tEmi.l(em,a,t0)*p_phiEmiXP(em,a,t0)*chiEmiXP.l(em,a,t0)*p_rhoEmiXP(em,a)*xp.l(a,t0));
     ygPit.l(h,t0)        = tPit.l(h,t0) * yh.l(h,t0);
     ygCit.l(entr,t0)     = tCit.l(entr,t0) * entrY.l(entr,t0);

     yg.l("FacLand",t0)   = sum( (a,actLnd),tFac.l(a,actLnd,t0) * pxfac_pretax.l(actLnd,a,old,t0)*xfac.l(actLnd,a,old,t0));
     yg.l("FacNatres",t0) = sum( (a,natres),tFac.l(a,natres,t0) * pxfac_pretax.l(natres,a,old,t0)*xfac.l(natres,a,old,t0));
     yg.l("FacLab",t0)    = sum( (a,l),     tFac.l(a,l,t0)      * pxfac_pretax.l(l,a,old,t0)*xfac.l(l,a,old,t0));
     yg.l("FacCap",t0)    = sum( (a,cap),tFac.l(a,cap,t0) * pxfac_pretax.l(cap,a,old,t0)*xfac.l(cap,a,old,t0));
     yg.l("psb",t0)       = sum(a,tpsb.l(a,t0)*px.l(a,t0)*xp.l(a,t0));
     yg.l("Prd",t0)       = sum(a,tPrd.l(a,t0)*px.l(a,t0)*xp.l(a,t0));

     yg.l("Exp",t0)       = sum(i,xe.l(i,t0)*p_chiPE(i)*pe.l(i,t0)*tExp.l(i,t0));
     yg.l("Imp",t0)       = sum(i,(xm.l(i,t0) + mdelst.l(i,t0))*pwm.l(i,t0)*er.l(t0)*tImp.l(i,t0));

     yg.l("Emi",t0)       = sum(aa,ygEmi.l(aa,t0));
     yg.l("EmiXP",t0)     = sum(a,ygEmiXP.l(a,t0));
     yg.l("Pit",t0)       = sum(h,ygPit.l(h,t0));
     yg.l("Cit",t0)       = sum(entr,ygCit.l(entr,t0));
     yg.l("VatDom",t0)    = sum(i,(p_chiPD(i)*pd.l(i,t0) + pmarg.l(i,t0)*tmg.l(i,"d",t0))*tVat.l(i,"d",t0)*xd.l(i,t0));
     yg.l("VatImp",t0)    = sum(i,(er.l(t0)*pwm.l(i,t0)*(1+tImp.l(i,t0)) + pmarg.l(i,t0)*tmg.l(i,"m",t0))*tVat.l(i,"m",t0)*xm.l(i,t0));

     yg.l("Paf",t0)       = sum(i,ygPaf.l(i,t0));
     ygTot.l(t0)          = sum(g,yg.l(g,t0));

*    ----------------------------------------------------------------------------
*
*      Savings
*
*    ----------------------------------------------------------------------------

     p_pfini(h) = sum(topKNest, xak.l(h,topKNest,t0)*pxak.l(h,topKNest,t0)) / uh.l(h,t0);

     p_pafini(i,h) = paf.l(i,h,t0);

     p_omegaSav(h)                             = defPar('omegaSav0');
     p_omegaSav(h)  $ hohpar(h,"omegaSav0","") = hohpar(h,"omegaSav0","") $ (hohpar(h,"omegaSav0","") gt eps);

     p_omegaSavEntr(entr)                              = defPar('omegaSavEntr0');
     p_omegaSavEntr(entr)   $ othPar(entr,"omegaSav0") = othPar(entr,"omegaSav0") $ (othPar(entr,"omegaSav0") gt eps);

     p_omegaFund(ttCal)   = defPar('omegaFund0');

*
*    --- total domestic funds available for private investments and government bonds
*
     FundT.l(t0)    =  sum(h,savH.l(h,t0))+ sum(entr,saventr.l(entr,t0));
     FundF.l(t0)    =  sum(instf,debtnew.l(instf,t0));

     p_sigmaFInt       = defPar('sigmaFInt0') $ (defPar('sigmaFInt0') gt eps);

     p_gammaFundF(t0) $ FundF.l(t0)
       = fundF.l(t0)/[ (interest.l(t0)*sum(instf,riskPrem.l(instf,t0)))**p_sigmaFInt
                          * ( 1 + ncpvusin(0,-{sigmoid([debtGdp.l(t0)-0.5]*4)-0.5}*2))];

     FundG.l(t0)    =  sum(instd,debtnew.l(instd,t0));
     FundP.l(t0)    =  FundT.l(t0) - fundG.l(t0);

     fundPrice.l(t0)  =  1;
     fundPriceD.l(t0) =  1;

     p_aFundG(ttCal) = FundG.l(t0)/FundT.l(t0) * (fundPrice.l(t0)*interest.l(t0)/interest.l(t0))**p_omegaFund(t0) + 1.E-8;

     chiFundG.fx(ttCal)  = 1;

     SectTot.l(t0) =  sum(flagXFac(actFp,a,vp), pxfac_pretax.l(actFp,a,vp,t0)*xfac.l(actFp,a,vp,t0));

     SectShr.l(aagr,t0)
                 =  sum(flagXFac(actFp,a,vp) $ mapaagr(aagr,a),pxfac_pretax.l(actFp,a,vp,t0)*xfac.l(actFp,a,vp,t0))/sectTot.l(t0);

     pindH.l(h,t0) = 1;
     p_asav(h,ttCal)  = savH.l(h,t0)/yhdsp.l(h,t0) * (fundPrice.l(t0)/pindh.l(h,t0))**(sign(savH.l(h,t0))*p_omegaSav(h));

     p_aSavEntr(entr,ttCal) = savEntr.l(entr,t0)/(entrY.l(entr,t0) * (1- tCit.l(entr,t0)) *(fundPrice.l(t0)/pgdpmp.l(t0))**p_omegaSavEntr(entr));

*    --------------------------------------------------------------------------------------------------
*
*     Initialization of foreign saving module
*
*    --------------------------------------------------------------------------------------------------

     eta1.fx(ttCal)    = defPar('eta10');

     eta2.fx(ttCal)    = defPar('eta20');
     eta1a.fx(ttCal)   = defPar('eta1a0');
     eta2a.fx(ttCal)   = defPar('eta2a0');
     chigrK.fx(ttCal)  = p_chigrK0;

     savfGDP.l(t0)     = (savf.l(t0)*er.l(t0) )/gdpmp.l(t0);

     kstocke.l(t0) = (1-depr.l(t0))*kstock.l(t0) + sum(inv, xfd.l(inv,t0) + p_PhiPcInv(inv,t0) * sum(ginv, xfd.l(ginv,t0)));


     ror.l(t0) = kapY.l(t0)/kstock.l(t0);
     rorc.l(t0) =  trent.l(t0)/sum(inv, pf.l(inv,t0)) - depr.l(t0);

     epsror.fx(ttCal)       = defPar('epsRor0')   $ (defPar('epsRor0') gt eps);
     epsrorE.fx(ttCal)      = defPar('epsRorE0');
     p_chiror(ttCal)        = 1;
     rore.l(t0) $ (not ((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) ))
                  = rorc.l(t0)*(kstocke.l(t0)/kstock.l(t0))**(epsRorE.l(t0));

     rorg.fx(ttCal) = interest.l(t0)/100;

     p_omegasavf(ttCal) = defPar('omegasavf0');
     p_rhosavf(ttCal)   = defPar('rhosavf0');

     p_gammasf2(ttCal)  = [savf.l(t0)/gdpmp.l(t0)]
                         /[exp(-p_omegasavf(t0)*er.l(t0)/pgdpmp.l(t0)*debtGDP.l(t0))
                         *depr.l(t0)*kstock.l(t0)*exp(p_rhosavf(t0)*(rore.l(t0)-rorg.l(t0)))];

     rore.l(t0) $ (((capAccClosure eq savfEndoUSAGE) or (capAccClosure eq savfEndoResRegSavings) ))
                  = ((1+rorc.l(t0))/(1+interest.l(t0)/100) - 1);

     p_retrow(ttCal)        = defPar('retrow0');
     p_ggdppcrow(ttCal)     = defPar('ggdppcrow0');

     p_gammasf1(ttCal) $ (capAccClosure eq savfEndoResRegSavings)
      = [savf.l(t0)/gdpmp.l(t0)]
        /(([(1+trent.l(t0))/(1+p_retrow(t0))]**eta1a.l(t0))*([(1+ggdppc.l(t0))/(1+p_ggdppcrow(t0))]**eta2a.l(t0)));


     $$iftheni.useGTAPEmis "%skipGTAPEmis%"=="off"

      execute_unload "%regf%/addData_%regGtapName%.gdx" land0,LCov0,carbCnt,emiGtapOri,mtoeGtap;
      option kill=EmiGTAPOri;

     $$endif.useGTAPEmis
*
*    --- sets and parameters for labor productivity differentiated by broad sectors
*
     set globSec / agr,ind,srv/;
     set a_globsec(a,globsec);

     a_globSec(agr,"AGR") = YES;
     a_globSec(man,"IND") = YES;
     a_globSec(srv,"SRV") = YES;

     Table p_diffProdShifters(tfpPars,globSec)

                 AGR      IND         SRV
        a        0.925391 2.893917      1
        b        11.99205 -94.8599
        c        291.8147 1680.554
     ;

     p_prodShift(tfpPars,a) = sum( a_globSec(a,globSec),p_diffProdShifters(tfpPars,globSec));
     $$if uelExist a-care p_prodShift("a",a) $ sameas("a-care",a) = eps;
     abort $ sum(a $ (not p_prodShift("a",a)),1) "Missing productivity shifter for at least one sector, in file: %system.fn%, line: %system.incline%",p_prodShift;
     $$if uelExist a-care p_prodShift("a",a) $ sameas("a-care",a) = 0;
     option kill=labProd;

     $$iftheni.IncomeDepCDE "%IncomeDepCDE%"=="on"

        parameter regParIncomeCDE;
        $$gdxin %readFile%
           $$load regParIncomeCDE
           $$if not errorFree $$abort "Compilation error after trying to load regParIncomeCDE in 'inical.inc', ... you might need to switch 'Income dependent CDE parameters' off."
        $$gdxin

     $$endif.IncomeDepCDE

     $$ifi not setglobal oldCapInvShare $setglobal oldCapInvShare 0
     p_oldCapInvShare(aact(a)) = %oldCapInvShare%;
     $$iftheni.sectSpecInv "%sectSpecInv%"=="on"

        $$onmulti
        set kNest / top /;
        $$offmulti

        if ( card(inv) gt 1, abort "Sector specific investment demand requires one single investment account, in file: %system.fn%, line: %system.incline%",inv);
        if ( not ifVint,     abort "Sector specific investment demand requires captial vintages, in file: %system.fn%, line: %system.incline%");
*
*       --- remove deamnd nesting for investments
*
        p_akf(inv,topKNest,ttCal)      = 0;
        akfAct(inv,topKNest)           = 0;
        p_akf(inv,"top",ttCal)         = 1;
        akFact(inv,kNest)  = 0;
        xakact(inv,kNest) $ (not sameas(kNest,"top")) = 0;
        p_af(inv,i,kNest,ttCal) = 0;
        conscont(inv,kNest) = 0;
*
*       --- instead, one Leontief top nest
*
        p_sigmaC(inv,"top")  = 0;
        map_i_knest(i,"top") = yes;
        xakact(inv,"top")  = 1;
        akFact(inv,"top")  = 1;
*
*       --- top nest demand equal total investment demand, same price
*
        xak.l(inv,"top",t0)  = xfd.l(inv,t0);
        pxak.l(inv,"top",t0) = pf.l(inv,t0);
*
*       --- calculate share parameter
*
        p_af(inv,i,"top",ttCal) $ pxak.l(inv,"top",t0)
          = (xa.l(i,inv,t0)/xak.l(inv,"top",t0));
*
*       --- render activity specific investment shares consistent with benchmark structure of investment demand
*
        set nonDiagInv(i) "Product for which investment demand is likely not related to stock changes";

        $$iftheni.GTAPCE "%sysEnv.GTAPCE%"=="on"
          $$setglobal nmm cem,nmx
        $$else.GTAPCE
          $$setglobal nmm nmm
        $$endif.GTAPCE

        set gtapNonDiagInv(CGtap)         / lum,%nmm%,cns,mvh,otn,omf,ele,
        $$ifi not "%sysEnv.GTAP9%"=="on"    eeq,rsa,
                                            obs,cmn /;
        nonDiagInv(i) $ sum(mapcGtap(gtapNonDiagInv,i0) $ mapis(i0,i),1) = YES;
*
*       --- standard assumption of some minimal investment demand everywhere
*
        gammaInv.l(i,aact(a),t0) $ (not nonDiagInv(i))     = sum(inv,p_af(inv,i,"top",t0))*0.01;
*
*       --- wharehousing might store all manufacturing outputs
*
        $$iftheni.GTAP9 not "%sysEnv.GTAP9%"=="on"
*
*           --- wharehousing might store all manufacturing and agr. outputs
*
           gammaInv.l(iMan,a,t0) $ ((not nonDiagInv(iMan)) $ sum(mapaGtap("whs",a0),mapis(a0,a))) = sum(inv,p_af(inv,iMan,"top",t0))*0.2;
           gammaInv.l(iMan,a,t0) $ (     nonDiagInv(iMan)  $ sum(mapaGtap("whs",a0),mapis(a0,a))) = sum(inv,p_af(inv,iMan,"top",t0))*0.05;
           gammaInv.l(iAgr,a,t0) $ sum(mapaGtap("whs",a0),mapis(a0,a))                            = sum(inv,p_af(inv,iAgr,"top",t0))*0.2;

           p_oldCapInvShare(aact(a)) $ sum(mapaGtap("whs",a0),mapis(a0,a)) = max(0.5,p_oldCapInvShare(a));
*
*          --- trade might store all manufacturing and agr. outputs
*
           gammaInv.l(iMan,a,t0) $ ((not nonDiagInv(iMan)) $ sum(mapaGtap("trd",a0),mapis(a0,a)))   = sum(inv,p_af(inv,iMan,"top",t0))*0.1;
           gammaInv.l(iMan,a,t0) $ (     nonDiagInv(iMan)  $ sum(mapaGtap("whs",a0),mapis(a0,a)))   = sum(inv,p_af(inv,iMan,"top",t0))*0.025;
           gammaInv.l(iAgr,a,t0) $ sum(mapaGtap("trd",a0),mapis(a0,a))                              = sum(inv,p_af(inv,iAgr,"top",t0))*0.1;

           p_oldCapInvShare(aact(a)) $ sum(mapaGtap("trd",a0),mapis(a0,a)) = max(0.5,p_oldCapInvShare(a));
        $$else.GTAP9
*
*          --- trade/retail etc. might store all manufacturing and agr. outputs
*
           gammaInv.l(iMan,a,t0) $ ((not nonDiagInv(iMan)) $ sum(mapaGtap("trd",a0),mapis(a0,a)))   = sum(inv,p_af(inv,iMan,"top",t0))*0.15;
           gammaInv.l(iMan,a,t0) $ (     nonDiagInv(iMan)  $ sum(mapaGtap("trd",a0),mapis(a0,a)))   = sum(inv,p_af(inv,iMan,"top",t0))*0.0375;
           gammaInv.l(iAgr,a,t0) $ sum(mapaGtap("trd",a0),mapis(a0,a))                              = sum(inv,p_af(inv,iAgr,"top",t0))*0.15;

           p_oldCapInvShare(aact(a)) $ sum(mapaGtap("trd",a0),mapis(a0,a)) = max(0.5,p_oldCapInvShare(a));
        $$endif.GTAP9
*
*       --- food industry might store crops
*
        gammaInv.l(i,a,t0) $ (sum(crops,sam(crops,i)) $ sum(mapaGtap("ofd",a0),mapis(a0,a)))     = sum(inv,p_af(inv,i,"top",t0))*0.1;
*
*       --- crops should not invest into other crops
*
        alias(crops,crops1);
        gammaInv.fx(i,crops1,t0) $ (sum(crops,sam(crops,i)) and (not sam(crops1,i))) = 0;
*
*       --- rice processing industry might store paddy
*
        gammaInv.l(i,a,t0) $ (sum(mapcGtap("pdr",i0),mapis(i0,i)) and sum(mapaGtap("pcr",a0),mapis(a0,a)))
          = sum(inv,p_af(inv,i,"top",t0))*0.5;
*
*       --- cattle to cattle
*
        gammaInv.l(i,a,t0) $ (sum(mapaGtap("ctl",a0),mapis(a0,a)) $ sameas(i,"c-ctlRais")) = sum(inv,p_af(inv,i,"top",t0))*0.2;
        gammaInv.l(i,a,t0) $ (sum(mapaGtap("rmk",a0),mapis(a0,a)) $ sameas(i,"c-ctlRais")) = sum(inv,p_af(inv,i,"top",t0))*0.2;

        gammaInv.l(i,aact(a),t0)  $ (sam(a,i) $ (not nonDiagInv(i)))  = sum(inv,p_af(inv,i,"top",t0))*10;

        gammaInv.l(nonDiagInv(i),aact(a),t0)                          = sum(inv,p_af(inv,i,"top",t0))*0.9;

        alias(i,ip);
        gammaInv.l(nonDiagInv(i),aact(a),t0) $ (not sum((inv,ip) $ sam(a,ip),p_af(inv,ip,"top",t0))) = sum(inv,p_af(inv,i,"top",t0))*2.0;

        equations
             e_addUpGammaInv(a,t)     "Investment demand shares for each activity must add up to unity"
             e_exhaustXfdInv(i,inv,t) "Investment demand by activities must exhaust total investment demand"
             e_keepBelow(i,a,t)       "Make sure that a single investment demand does not exceed a share of total investment demand"
             e_hdpGammaInv            "Penalty function"

        ;
        variable hdpGammaInv;

        e_addUpGammaInv(aact(a),t0) $ sum(cap,xfac.l(cap,a,new,t0))  ..

            sum(i $ gammaInv.l(i,a,t0),gammaInv(i,a,t0)) =E= 1;

        e_exhaustXfdInv(i,inv,t0) $ xa.l(i,inv,t0) ..

            xa.l(i,inv,t0)*p_chiPA(i)*paf.l(i,inv,t0)
              =E= sum((cap,aact(a)) $ gammaInv.l(i,a,t0), xfac.l(cap,a,new,t0) * gammaInv(i,a,t0));

        e_hdpGammaInv ..

            hdpGammaInv * sum( (i,aact(a)) $ gammaInv.l(i,a,t0),1)
             =E= sum( (i,aact(a)) $ gammaInv.l(i,a,t0),  sqr(gammaInv.l(i,a,t0)-gammaInv(i,a,t0)));


        set dummyCns / c-cns /;

        e_keepBelow(i,aact(a),t0) $ gammaInv.l(i,a,t0) ..

             sum(cap, xfac.l(cap,a,new,t0) * gammaInv(i,a,t0))
                =L=  sum(inv,xa.l(i,inv,t0)*p_chiPA(i)*paf.l(i,inv,t0)) * (0.25 + 0.10 $ sameas(i,"c-cns")
                                                                            + sum(cap, xfac.l(cap,a,new,t0))/sum((cap,a1),xfac.l(cap,a1,new,t0)))
                   + 1.E-8;

        model m_hdpGammaInv / e_addUpGammaInv,e_exhaustXfdInv,e_keepBelow,e_hdpGammaInv/;

        alias(a,aAlias);
        xfac.scale(cap,a,new,t0) = xfac.l(cap,a,new,t0);

        newInv.l(t0)    = sum((i,inv),xa.l(i,inv,t0)*p_chiPA(i)*paf.l(i,inv,t0));

        xfac.l(cap,a,new,t0) = xfac.l(cap,a,old,t0)*(deprA.l(a,t0)+p_oldCapInvShare(a))
           *  newInv.l(t0)/sum(aAlias,xfac.l(cap,aAlias,old,t0)*(deprA.l(aAlias,t0)+p_oldCapInvShare(aAlias)));

        gammaInv.l(i,aact(a),t0) $ (not sum(cap,xfac.l(cap,a,new,t0))) = 0;

        gammaInv.fx(i,aact(a),t0) $ (not gammaInv.l(i,a,t0)) = 0;
        gammaInv.lo(i,aact(a),t0) $ gammaInv.l(i,a,t0)       = 0;
        m_hdpGammaInv.iterlim   = 10000;
        m_hdpGammaInv.solveLink = 5;
        m_hdpGammaInv.optfile   = 1;
        m_hdpGammaInv.tolinfeas = 1.E-7;
        solve m_hdpGammaInv minimizing hdpGammaInv using NLP;
        gammaInv.l(i,a,t0) $ (gammaInv.l(i,a,t0) lt sum(inv,p_af(inv,i,"top",t0))*0.01) = 0;
        solve m_hdpGammaInv minimizing hdpGammaInv using NLP;
        gammaInv.fx(i,aact(a),t0) $ (not gammaInv.l(i,a,t0))                         = 0;
*
*       --- assign calibrated investment demand shares for each activity
*
        p_sigmaInv(aact(a)) = sum(inv,p_sigmaF(inv));
        p_sigmaInv(a) $ (sum(i $ gammaInv.l(i,a,t0),1) le 1) = 0;
*
*       --- sector specific investments: make investment nest Leontief
*
        p_sigmaf(inv)       = 0;
        p_sigmac(inv,"top") = 0;
*
*       --- calculate price index
*
        invPrice.l(aact(a),t0) = sum(i $ gammaInv.l(i,a,t0),gammaInv.l(i,a,t0)*sum(inv,p_chiPA(i)*paf.l(i,inv,t0))**(1-p_sigmaInv(a)))**(1/(1-p_sigmaInv(a)));
        invPrice.l(aact(a),ttCal) = invPrice.l(a,t0);

        option kill=gammaInv.lo,kill=gammaInv.up;
        gammaInv.fx(i,a,t0)  $ gammaInv.l(i,a,t0) = gammaInv.l(i,a,t0);
        p_gammaInv0(i,a) $ gammaInv.l(i,a,t0)
           = gammaInv.l(i,a,t0)/{[(invPrice.l(a,t0)/sum(inv,p_chiPA(i)*paf.l(i,inv,t0)))**p_sigmaInv(a) ] $ p_sigmaInv(a) + 1 $ (not p_sigmaInv(a))};

        lambdaOa.l(inv,i,topKNest,ttCal)      = 0;
        lambdaOa.lo(inv,i,topKNest,ttCal)     = -inf;
        lambdaOa.up(inv,i,topKNest,ttCal)     = +inf;

        lambdaOa.l(inv,i,"top",ttCal) = 1;
        lambdaOa.fx(inv,i,"top",ttCal) $ (xa.l(i,inv,t0) and (not sum(a,gammaInv.l(i,a,t0)))) = lambdaOa.l(inv,i,"top",t0);

        p_lambdaInv0(i) $ sum(inv,p_af(inv,i,"top",t0))
             = [sum((cap,aact(a)) $ gammaInv.l(i,a,t0), xfac.l(cap,a,new,t0)*gammaInv.l(i,a,t0))/newInv.l(t0)]/sum(inv,p_af(inv,i,"top",t0));

        newInv.l(t0)    = sum((cap,a),xfac.l(cap,a,old,t0)*(deprA.l(a,t0)+p_oldCapInvShare(a)));

        xfac.l(cap,a,new,t0) = xfac.scale(cap,a,new,t0);
        xfac.scale(cap,a,new,t0) = 1;

     $$else.sectSpecInv

        option kill=p_gammaInv0,kill=p_lambdaInv0,kill=p_sigmaInv,kill=gammaInv,kill=newInv,kill=invPrice;

     $$endif.sectSpecInv

     chixFacFA.lo(lFpNest(fpNest,a),ttCal)    $ (sum(flagXFac(fpNest,a,v),1) $ ifLabProdDiff) = -inf;
     chixFacFA.up(lFpNest(fpNest,a),ttCal)    $ (sum(flagXFac(fpNest,a,v),1) $ ifLabProdDiff) = +inf;

     chixFacF.lo(natRes,ttCal)  $ ifNatResProdAsLabor = -inf;
     chixFacF.up(natRes,ttCal)  $ ifNatResProdAsLabor = +inf;

     chixFacF.lo(actLnd,ttCal)  $ ifLandProdAsLabor   = -inf;
     chixFacF.up(actLnd,ttCal)  $ ifLandProdAsLabor   = +inf;

     chixFacF.lo(cap,ttCal)     $ ifCapProdAsLabor    = -inf;
     chixFacF.up(cap,ttCal)     $ ifCapProdAsLabor    = +inf;

     chixFacL.up(ttCal)         $ (not ifLabProdDiff) = +inf;
     chixFacL.lo(ttCal)         $ (not ifLabProdDiff) = -inf;

     option kill=p_exolCovAreaChg;
     pfmsup.l(fmNestFmp(fma,fmp),instTot,vf,ttCal) $ (sum(sameas(fmp,actfp),1) $ (not t0(ttCal))) = 0;

     $$iftheni.endoTech "%endotech%"=="on"

        p_rhoEn = EnTPar("rhoEn");

        frontBGP.fx(a,ttCal) = 1;
*
*       --- if not set via bridge file, assume that the whole R&D mechanism is not used
*           for this activity and phiRD becomes zero which takes out equations and vars
*
        phiRDtoes(a) $ (not phiRDtoes(a)) = 1.0;
        p_phiRD(a,ttCal) = 1 - phiRDtoes(a);

        p_sigmaxpvt = EnTPar("sigmaxpvt");

*       ---- !careful! obsol must be bigger than egrowth.

        p_obsol(a,ttCal)  = EnTPar("obsol");
        p_inn(a,ttCal)    = p_obsol(a,t0)/(1+2*p_rhoEn);
        einn.l(a,ttCal)   = p_inn(a,t0);
        p_lambdaEn(a,v,ttCal) = 1;

*       --- growth correction for sectors that should not grow faster than average

        p_fcrgrowth(a,t)  = 0;
        p_fgrowth(a)      = EnTPar("fgrowthDef") + Supfgrowth(a);
        p_fcrgrowth(a,t)  = EnTPar("fgrowthDef");
*
*       --- this sets the sector specific maximum frontier for tfp growth (exogenous one might still be added
*           via lambda shifters)
*
        p_frontier(a,t0)    = (p_obsol(a,t0)+ p_fgrowth(a))/(1+p_fgrowth(a))*1/p_inn(a,t0);
        p_frontier(a,ttCal) = prod(tf $ ((years(tf) ge years(t0)) and (years(tf) le years(ttCal))),
                                        (1+p_fgrowth(a)))/(1+p_fgrowth(a))*p_frontier(a,t0);
*
*       --- exogenous ???
*
        p_frontCr(a,ttCal)   = prod(tf $((years(tf) ge years(t0)) and (years(tf) le years(ttCal))),
                                   (1+p_fcrgrowth(a,ttCal)))/(1+p_fcrgrowth(a,ttCal))*p_frontier(a,t0);

        p_VarietyCr(a,ttCal) = prod(tf $((years(tf) ge years(t0)) and (years(tf) le years(ttCal))),
                                   (1+einn.l(a,t0)*p_RhoEn));

        p_egrowth(a,ttCal)   = (1-p_sigmaxpvt)*p_fgrowth(a);
*
        p_lambdaEn(a,new,ttCal) = p_frontier(a,ttCal);
        p_lambdaEn(a,old,ttCal) = 1/(1+p_fgrowth(a));
        p_LambdaEn(a,new,ttCal) = 1;
        p_LambdaEn(a,v,ttCal)   = 1;
        p_LambdaCr(a,v,ttCal)   = 1;
        p_LambdaEnCr(a,v,ttCal) = 1;
*
*       jan: I presume we would need more changes in the model if we would target products or factors?
*            Should we keep the quite generic equations?
*
        p_ChifdRD(is,ttCal)  = 0;
        p_ChifdRD(inv,ttCal) = 1;

        p_ChifdRD(is,ttCal) = p_ChifdRD(is,ttCal)/sum(js,p_ChifdRD(js,ttCal));

        pfdrd.l(t0) = sum(f, p_ChifdRD(f,t0)*pf.l(f,t0));

        p_interestRD(ttCal) = EnTPar("interestRD");
*
*       --- share of new vintage is equal to share of new innovations
*
        xfac.l(fptop,a,new,t0)    = p_inn(a,t0)*(1+p_rhoEn)/(1-p_rhoEn*p_inn(a,t0))*xfac.l(fptop,a,old,t0);
*
*       --- apply same distribution factor to all tech nests (old currently store the total)
*
        xfac.l(flagxfac(actFpNest,aact,new),t0)
          = xfac.l(actFpNest,aact,old,t0) * xfac.l(fptop,aact,new,t0)/xfac.l(fptop,aact,old,t0);
*
*       --- old vintage is the residual
*
        xfac.l(flagxfac(actFpNest,aact,old),t0) =  xfac.l(actFpNest,aact,old,t0) - xfac.l(actFpNest,aact,new,t0);

        p_xRD0(a) = p_markup(a,t0) * sum(fptop, xfac.l(fptop,a,new,t0) * pxfac.l(fptop,a,new,t0))
                   /((1+p_interestRD(t0))/(1+p_egrowth(a,t0))-(1-p_inn(a,t0))) / (pfdrd.l(t0)*p_frontier(a,t0));

        xRD.l(a,t0) = 1;

        xfdRD.l(is,t0) = p_chifdRD(is,t0) * sum(aact(a),xRD.l(a,t0)*p_xRD0(a)*p_frontier(a,t0));
        yfdRD.l(t0)    = sum(f, xfdRD.l(f,t0)*pf.l(f,t0));

     $$else.endotech
*
*       --- peuso-initialization of symbols used in endotech equations
*           to avoid that GAMS compiles about non-defined symbols
*
        option kill=p_phiRD;
        option kill=p_inn;
        option kill=einn;
        option kill=p_rhoEn;
        p_sigmaxpvt=0;
        option kill=p_interestRd;
        option kill=p_lambdaEnCr
        option kill=p_egrowth;
        option kill=p_frontier;
        option kill=p_xrD0;
        option kill=p_chifdRD;
        option kill=p_obsol;
        option kill=xfdRd;
        option kill=yfdRd;
        option kill=pfdRd;

     $$endif.endotech

     prodInv.l(t0)
      = sum(inv,
            + xfd.l(inv,t0)
            + p_phiPcInv(inv,t0) * sum(ginv, xfd.l(ginv,t0))
            - xfdRD.l(inv,t0) $ p_chifdRD(inv,t0)
            - p_nonProdInv(inv,t0))
      ;

     grK.l(t0)    = prodInv.l(t0)/kstock.l(t0) - depr.l(t0);

     $$ifi not defined p_rord0 p_rord0 = 0.05
     if(capAccClosure eq savfEndoUSAGE,

        grK.l(t0)       = p_grkTrend(t0);
        rord.fx(ttCal)  = p_rord0;
        rorg.fx(ttCal)  = 0;
        devRoR.l(t0)    = rore.l(t0) - rord.l(t0) - rorg.l(t0);
     );

  $$else.task
*
*    --- simulation mode
*
     $$iftheni.draws not "%1"=="draws"
*
*       --- gets list of variables and parameters via Phyton and store in
*          temp. file

        $$onembeddedCode Python:
          import os

          exemptions = ['p_prob','p_tempDelta','p_ccDamages','p_investShocks']

          with open(r'%gams.scrdir%varPars.gms', 'w') as f:
             for s in gams.db:
               if (s.name not in exemptions) :
                  if (type(s) == GamsVariable) or (type(s) == GamsParameter) :
                     f.write('  '+ s.name +'\n')
          f.closed

          ifDmg = list(gams.get("ifDmg"))
          os.environ["CCDamages"] = "off"
          if ifDmg[0] > 0:
           os.environ["CCDamages"] = "on"

        $$offembeddedCode
        $$ifi not "%CCDamages%"=="on" $$setglobal CCDamages %sysEnv.CCDamages%
        set ax(aa);

     $$endif.draws

*
*    --- load the variables and parameters plus flag sets defined above
*
     set mapCGtap(*,*);

     execute_load "%odir%/%BaUFile%"
*
*       --- load list of parameters and variables generated above
*
        $$include %gams.scrdir%varPars.gms
*
*          --- load sets
*
           flagXfac,ax,aact,iact,akfact,iImp,iDom,iarm,iexp,ist,aain,xaact,mgEncl,
           fpNestI,fpIdent,fpNestUpdate,fpChlUpdate,fpParUpdate,isAdditiveCes,fpTop,fpNat,lfpNest,leontief
           iAgr,iMan,iSrv,agr,man,srv,aElec,iElec,cTour,mapk,mapCGtap,mapAge
           map_i_knest,consCont,xakact,topKNest
           fmPar,fmParBot,fmNesting,fmNestFmp,isAdditiveCet,actFp,actFpNest,actFmp,actLnd,actHa,emsrc
           emiFlag,emiCommFlag,fmChild
           $$ifi "%aez%"=="on" aez,raez,raez_aez
     ;

     p_alphaXfac(actFpNest,fppar,a,v) $ (pNest(a,actFpNest,fpPar) $ xfac.l(fpPar,a,old,t0)  $ xfac.l(actFpNest,a,old,t0))
        =     (xfac.l(actFpNest,a,old,t0)*pNest(a,actFpNest,fpPar)/xfac.l(fpPar,a,old,t0))
            * (1 - p_markUpNonComp(actFpNest,a,t0))
            * (pxfac.l(actFpNest,a,old,t0)/(lambdaxfac.l(actFpNest,a,old,t0)*pxfac.l(fpPar,a,old,t0)))**p_sigmaxfac(fppar,a,v);

     $$iftheni.forwardLooking "%forwardLooking%"=="on"

       psExp.l(ist(i),t)    $ p_alphaSt(i,t)  = ps.l(i,t0);
       xsExp.l(ist(i),t)    $ p_alphaSt(i,t)  = xs.l(i,t0);
       psExpRel.l(ist(i),t) $ p_alphaSt(i,t)  = 0;
       rore.l(t0)                              = rore.l(t0);
       kstock.l(t0)                            = kStock.l(t0);
     $$endif.forwardLooking

     if ( (rescale ne 1) or (reeScale ne 1) or (samScale ne 1),
         $$setglobal scaleSam on
         reScale   = 1/reScale;
         reeScale  = 1/reeScale;
         samScale  = 1/samScale;
         usd       = 1/usd;
         $$include 'inc/cal/rescale.inc';
         reScale   = 1/reScale;
         reEScale  = 1/reeScale;
         samScale  = 1/samScale;
         usd       = 1/usd;
     );
     lCovArea.l(actHa,lCov,tScale) =  lCovArea.l(actHa,lCov,tScale)   * inscale * usd;

     $$ifi "%CCDamages%"=="on"    ifDmg = 1;
     $$ifi "%FiscalResp%"=="on"   ifFiscalResp=1;
     $$ifi "%FiscalResp%"=="off"  ifFiscalResp=0;

     $$ifi "%ifAirPollDmg%"=="on"      ifAirPollDmg = 1;
     $$ifi "%ifAirPollDmg%"=="off"     ifAirPollDmg = 0;

     ifLabProdDiff       = 0;
     ifCapProdAsLabor    = 0;
     ifLandProdAsLabor   = 0;
     ifNatResProdAsLabor = 0;

     if ( execError, abort " ExecError encountered after reading %odir%/%BaUFile% in file: %system.fn%, line: %system.incline%");
*
*    --- undo the fixing of variables for past years from iterloop.inc in run.gms
*        during baseline run
*
     debtpay.lo(inst,ttCal)=-inf;       debtpay.up(inst,ttCal) = +inf;
     debtNew.lo(inst,ttCal)=-inf;       debtNew.up(inst,ttCal) = +inf;
     debtInterest.lo(inst,ttCal)=-inf;  debtInterest.up(inst,ttCal) = +inf;
     debtPrincipal.lo(inst,ttCal)=-inf; debtPrincipal.up(inst,ttCal) = +inf;

     option
       kill=lCovArea.lo,kill=lCovArea.up
       kill=xfd.lo,kill=xfd.up
       kill=xfac.lo,kill=xfac.up
       kill=pxfac.lo,kill=pxfac.up
       kill=kaps.lo,kill=kaps.up
       kill=kstock.lo,kill=kstock.up
       kill=shrXFac.lo,kill=shrXFac.up
       kill=transfers.lo,kill=transfers.up
       kill=xfac.lo,kill=xfac.up
       kill=x.lo,kill=x.up
       kill=xa.lo,kill=xa.up
       kill=deprate.lo,kill=deprate.up
       kill=pop.lo,kill=pop.up
       kill=aps.lo,kill=aps.up
       kill=timp.lo,kill=timp.up
       kill=emiGhg.lo,kill=emiGhg.up
       kill=xfd.lo,kill=xfd.up
       kill=rgdpmp.lo,kill=rgdpmp.up
       kill=rgdppc.lo,kill=rgdppc.up
       kill=debtStk.lo,kill=debtStk.up
       kill=DebtStkT.lo,kill=DebtStkT.up
       kill=debtGDP.lo ,kill=debtGDP.up
       kill=rGDPpc.lo,kill=rGDPPc.up
       kill=pop.lo,kill=pop.up
       kill=popAge.lo,kill=popAge.up
       kill=hpop.lo,kill=hpop.up
       kill=elecMix.lo,kill=elecMix.up
       kill=sectShr.lo,kill=sectShr.up
       kill=yg.lo,kill=yg.up
       kill=elyInt.lo,kill=elyInt.up
       kill=exposedAirPoll.lo,kill=exposedAirPoll.up
       kill=debtNew.lo,kill=debtNew.up
       kill=lambdaxfac.lo,kill=lambdaxfac.up
       kill=interest.lo,kill=interest.up
*
*      --- remove bounds on taxes which might have been fixed
*          in iterloop.inc in fixTaxesLambda mode
*
       kill=tPrd.lo, kill=tPrd.up
       kill=tPsb.lo, kill=tPsb.up
       kill=tPhn.lo, kill=tPhn.up
       kill=tExp.lo, kill=tExp.up
       kill=tImp.lo, kill=tImp.up
       kill=tVat.lo, kill=tVat.up
       kill=tEmi.lo, kill=tEmi.up
       kill=tPaf.lo, kill=tPaf.up
       kill=tFac.lo, kill=tFac.up
*
*      --- remove lower bounds from energy ficxng
*
       kill=xat.lo, kill=xat.up
     ;

*
*    --- tax redistribution
*
     yTaxRedist.fx(ttCal) = yTaxRedist.l(ttCal);
*
*    --- fix demographics for base year
*
     deprate.fx(cohorts,t0) = deprate.l(cohorts,t0);
     popAge.fx(age,t0)      = popAge.l(age,t0);
*
*    --- this should be true for all scenarios
*
     ifcal  = 0;
*
*    --- fix interest and debt for years before the base year
*
     interest.fx(t)       $ (years(t) le %baseYear%) =  interest.l(t0);
     debtNew.fx(inst,t)   $ (years(t) le %baseYear%) =  debtNew.l(inst,t);
*
*    --- fix productivity, cost share and other shifters
*       which might be endogenous in baseline mode
*
     chixFacL.fx(ttCal)                                                = chixFacL.l(ttCal);
     ChixFacF.fx(actFpNest,ttCal)                                      = chixFacF.l(actFpNest,ttCal);
     ChixFacA.fx(a,ttCal)                                              = chixFacA.l(a,ttCal);
     ChixFacFA.fx(actFpNest,a,ttCal) $ sum(flagXfac(actFpNest,a,v),1)  = chixFacFA.l(actFpNest,a,ttCal);
     ChixFacAgr.fx(aagr,actFpNest,ttCal)                               = chixFacAgr.l(aagr,actFpNest,ttCal);
     lambdaxfac.fx(flagXFac(actFpNest,a,v),t0)                         = lambdaxfac.l(actFpNest,a,v,t0);

     lambdaxfac.fx(flagXFac(actFpNest,a,v),ttCal) $ (lambdaxFac.scale(actFpNest,a,v,ttCal) eq 999) = lambdaxfac.l(actFpNest,a,v,ttCal);

     tPhnD2.fx(aagr,ttCal)                                             = tPhnD2.l(aagr,ttCal);

     $$iftheni.twist "%hasTwist%"=="on"
        chitwFA.fx(fpParTC,a,ttCal)                                       = chitwFA.l(fpParTC,a,ttCal);
        chitwF.fx(fpParTC,ttCal)                                          = chitwF.l(fpParTC,ttCal);
        twist.fx(fpChl,fpParTC,a,v,ttCal)  $ alphaXfac(fpChl,fpParTC,a,v) = twist.l(fpChl,fpParTC,a,v,ttCal);
     $$endif.twist

  $$endif.task

  $$iftheni.shortRun "%shortRunShock%"=="on"
     parameter p_omegaFmpOri(fmAll,vf),p_epsFmpOri(fmAll,vf);
     alias(fmPar,fmPar1);
     p_omegaFmpOri(fmAll,vf) = p_omegaFmp(fmAll,vf);
     p_epsFmpOri(fmAll,vf)   = p_epsFmp(fmAll,vf);
  $$endif.shortRun

  $$ifi not defined psExpRel       option kill=psExpRel;
*
* --- abort at end of inical if errors occured
*
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( ExecError, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
