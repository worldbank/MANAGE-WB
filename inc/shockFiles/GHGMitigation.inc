********************************************************************************
$ontext

   MANAGE-WB project

   GAMS file : GHGMitigation.inc

   @purpose  : Impose an emission ceiling or carbon tax, tax recycling
   @author   : W.Britz
   @date     : 20.10.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : ccdr.inc

$offtext
********************************************************************************
  $$iftheni.decl "%1"=="decl"

    $$ifi not "%shk_file%"=="ccdr" $$Abort "GHGMitigation.inc is part of the ccdr shock file and cannot be used in standalone mode"
*
*    --- store some benchmark results used in equations
*
     parameter p_ghgs(t)       "GHG emissions from benchmark"
               p_taxes(g,t)    "Tax revenues from benchmark"
               p_yTaxRedist(t) "Redistributed taxes from benchmark";

     p_ghgs(t)       = emiGHG.l("GHGs",t);
     p_taxes(g,t)    = yg.l(g,t);
     p_yTaxRedist(t) = yTaxRedist.l(t);
*
*     --- Check if GHG emission policy instruments are available from bridge-file
*
      $$ifi not defined GHGMitigation $$abort "Bridge-file missing GHGMigitation data, in file: %system.fn%, line: %system.incline%"
*
*     --- define equations/variables
*
      equations
       e_reyclSalesTaxes(t)        "Define tax shifter for sales taxes by settings level of recycled sales taxes"
       e_reyclFactorTaxes(t)       "Define tax shifter for factor taxes by settings level of recycled factor taxes"
       e_reyclDirectTaxes(t)       "Define tax shifter for direct taxes by settings level of recycled direct taxes"
       e_reyclAllTaxes(t)          "Define tax shifter for all taxes by settings level of recycled sum of taxes"
       e_reyclTransfers(t)         "Define ammount of additional taxes redistributed as transfers"
       e_redEmisa(a,t)             "Define relative reduction in process emissions"
       e_chiEmiFac(em,fpNest,a,t)  "Define emission shifter: 1 minus relative change in emsisions due to MACC"
       e_chiEmiXP(em,a,t)          "Define emission shifter: 1 minus relative change in emsisions due to MACC"
       e_emisa                     "Define process emissions in CO2eq"
       e_macDemand1(a,t)           "Define physical MAC demand per unit of emission"
       e_macDemand(fpNest,a,v,t)   "Value of MAC services in total for an activity"
       e_pxMac(a,t)                "Define unit cost including MAC cost"
      ;

      variables
         macDemand1(a,t)           "Physical MAC demand per unit of emission"
         redEmisa(a,t)             "Relative reduction in process emissions, based on integratal under points on MACC curve"
         emisa(a,v,t)              "Ammount of process emissions in CO2eq"
      ;

      set factG(g) "Factor tax flows" /
       FacLand      "Land Factor Tax"
       FacNatres    "Natural resource Factor Tax"
       FacLab       "Labor Factor Tax"
       FacCap       "Capital Factor Tax"
      /;

      set allTax(g) "All tax flows with the exemption of carbon taxes";
      allTax(g) = yes; allTax("emi") = no; allTax("emiXP") = no;

      scalar p_salesTaxRecl  "Share of carbon taxes recycled as sales taxes"  / %salesTaxRecl%   /
             p_factTaxRecl   "Share of carbon taxes recycled as factor taxes" / %factTaxRecl%    /
             p_dirTaxRecl    "Share of carbon taxes recycled as direct taxes" / %dirTaxRecl%     /
             p_allTaxRecl    "Share of carbon taxes recycled as sum of taxes" / %allTaxRecl%     /
             p_transfersRecl "Share of carbon taxes recycled as transfers"    / %transfersRecl%  /
      ;

      scalar hasGhgPolicy "Indicator for carbon taxation / emission ceiling" /0/;
*
*     --- recycling of carbon tax to reduce factor taxes
*
      e_reyclFactorTaxes(ts(t)) $ ((tFacM0.range(t) ne 0) $ sum(factG,p_taxes(factG,t))) ..


           sum(factG $ p_taxes(factG,t), yg(factG,t))/max(%minScale%,sum(factG, abs(yg.l(factG,t-1)))*%scaleMult%)

             =E=
          [
          $$iftheni.baseline "%task%"=="Baseline"
*
*            --- baseline: let taxes increase with GDP at market prices, minusa share of carbon tax revenues
*
             sum(factG, yg.l(factG,t-1))*gdpmp(t)/gdpmp.l(t-1)
               - (yg("emi",t) + yg("emiXp",t))-(yg.l("emi",t-1) + yg.l("emiXp",t-1))*p_factTaxRecl
          $$else.baseline
*
*            --- counterfactual: decrease factor tax revenues from baseline by carbon tax revenue
*
             sum(factG,p_taxes(factG,t))
          +  (p_taxes("emi",t)+p_taxes("emiXp",t) - yg("emi",t) - yg("emiXp",t))*p_factTaxRecl

          $$endif.baseline
          ]/max(%minScale%,sum(factG, abs(yg.l(factG,t-1)))*%scaleMult%);
*
*     --- recycling of carbon tax to reduce sales taxes
*
      e_reyclSalesTaxes(ts(t)) $ ((tPafM0.range(t) ne 0) $ p_taxes("Paf",t)) ..


           yg("Paf",t)/max(%minScale%,abs(yg.l("Paf",t-1))*%scaleMult%)

             =E=
          [
          $$iftheni.baseline "%task%"=="Baseline"
*
*            --- baseline: let taxes increase with GDP at market prices minus a share of carbon tax revenues
*
                yg.l("Paf",t-1)*gdpmp(t)/gdpmp.l(t-1)
                  + (yg("emi",t) + yg("emiXp",t))-(yg.l("emi",t-1) + yg.l("emiXp",t-1))*p_salesTaxRecl
          $$else.baseline
*
*            --- counterfactual: decrease sales tax revenues from baseline by carbon tax revenue
*
             p_taxes("Paf",t)
          +  (p_taxes("emi",t)+p_taxes("emiXp",t) - yg("emi",t) - yg("emiXp",t))*p_salesTaxRecl

          $$endif.baseline
          ]/max(%minScale%,abs(yg.l("Paf",t-1))*%scaleMult%);

*
*     --- recycling of carbon tax to reduce direct taxes
*
      e_reyclDirectTaxes(ts(t)) $ ((tDr.range(t) ne 0) $ (p_taxes("pit",t)+p_taxes("cit",t))) ..

          (yg("cit",t) + yg("pit",t))/max(%minScale%,(abs(yg.l("cit",t-1)) + abs(yg.l("pit",t-1)))*%scaleMult%)

             =E=
          [
          $$iftheni.baseline "%task%"=="Baseline"
*
*            --- baseline: let taxes increase with GDP at market prices minus a share of carbon tax revenues
*
                (yg.l("cit",t-1) + yg.l("pit",t-1))*gdpmp(t)/gdpmp.l(t-1)
              - (yg("emi",t) + yg("emiXp",t))-(yg.l("emi",t-1) + yg.l("emiXp",t-1))*p_dirTaxRecl
          $$else.baseline
*
*            --- counterfactual: decrease direct tax revenues from baseline by carbon tax revenue
*
              p_taxes("pit",t)+p_taxes("cit",t)
          +  (p_taxes("emi",t)+p_taxes("emiXp",t) - yg("emi",t) - yg("emiXp",t))*p_dirTaxRecl

          $$endif.baseline
          ]/max(%minScale%,(abs(yg.l("cit",t-1)) + abs(yg.l("pit",t-1)))*%scaleMult%);

*
*     --- targeting total tax income with recycling
*
      e_reyclAllTaxes(ts(t)) $ (tMall.range(t) ne 0) ..

          sum(allTax, yg(allTax,t)) /max(%minScale%,sum(allTax, abs(yg.l(allTax,t-1)))*%scaleMult%)

             =E=
          [
          $$iftheni.baseline "%task%"=="Baseline"
*
*            --- baseline: let total tax revenues increase with GDP at market prices minus carbon tax revenues
*
             sum(allTax, yg.l(allTax,t-1))*gdpmp(t)/gdpmp.l(t-1)
              + (yg("emi",t) + yg("emiXp",t))-(yg.l("emi",t-1) + yg.l("emiXp",t-1))*p_allTaxRecl
          $$else.baseline
*
*            --- counterfactual: keep total tax income (incl. carbon taxes) from baseline
*
             sum(allTax,p_taxes(allTax,t))
            +  (p_taxes("emi",t)+p_taxes("emiXp",t) - yg("emi",t) - yg("emiXp",t))*p_allTaxRecl

          $$endif.baseline
          ]/max(%minScale%,sum(allTax, abs(yg.l(allTax,t-1)))*%scaleMult%);
*
*     --- targeting total tax income with recycling
*
      e_reyclTransfers(ts(t)) $ (yTaxRedist.range(t) ne 0) ..

          yTaxRedist(t)/max(%minScale%,abs(yTaxRedist.l(t-1))*%scaleMult%)

             =E=
          [
          $$iftheni.baseline "%task%"=="Baseline"
*
*            --- baseline: let transfers increase with GDP at market prices (incl. carbon tax revenue)
*
             yTaxRedist.l(t-1)*gdpmp(t)/gdpmp.l(t-1)
              + (yg("emi",t) + yg("emiXp",t))-(yg.l("emi",t-1) + yg.l("emiXp",t-1))*p_transfersRecl
          $$else.baseline
*
*            --- counterfactual: keep total tax income from baseline
*
             p_YTaxRedist(t)
          -  (p_taxes("emi",t)+p_taxes("emiXp",t) - yg("emi",t) - yg("emiXp",t))*p_transfersRecl

          $$endif.baseline
          ]/max(%minScale%,abs(yTaxRedist.l(t-1))*%scaleMult%);


      $$iftheni.maccs "%maccs%"=="on"
*
*        --- equations for MACC mechanism
*
         $$ifi not defined MACC $$abort "Bridge-file missing MACC data, in file: %system.fn%, line: %system.incline%"

         parameter p_maccPoints(maccPoints);
         alias(maccPoints,maccPoints1,maccPoints2);
*
*        --- the points for the tax rates where a certain abatement level is reach are equal to the labels (.val)
*
         p_maccPoints(maccPoints) = maccPoints.val;
*
*        --- the first point (zero abatement) is set to 1.E-10 (no effect)
*
         p_maccPoints("0")        = 1.E-10;
*
*        --- for activities (with the exemption of the MACC activity itself) with no info no MACCs, take "rest"
*
         macc(aact(a),"0") $ sum(maccPoints1, macc(a,maccPoints1)) = 1.E-10;

         macc(aact(a),maccPoints) $ (not sum(maccPoints1, macc(a,maccPoints1))) = macc("rest",maccPoints);
         macc("a-macc",maccPoints) = 0;

         macc(aact(a),maccPoints) = macc(a,maccPoints);
*
*        --- maccs are only applied to process emissions. Remove if no process emissions coefficients are found
*
         macc(a,maccPoints) $ (not (    sum(em      $ p_emiConv(em,"GHGs"), p_rhoEmiXP(em,a))
                                     or sum((em,fp) $ p_emiConv(em,"GHGs"), p_rhoEmiFac(em,fp,a)))) = 0;
*
*        --- marginal abatement
*
         e_redEmisa(aact(a),ts(t)) $ macc(a,"0") ..

            redEmisa(a,t)/max(%minScale%,redEmisa.l(a,t-1)*%scaleMult%)  =E=

               sum[(maccPoints,maccPoints1) $ (   macc(a,maccPoints) $ macc(a,maccPoints1)
                                                  $ (maccPoints.pos gt maccPoints1.pos)
                                                  $ (macc(a,maccPoints1)
                                                     eq smax(maccPoints2 $ (maccPoints.pos gt maccPoints2.pos), macc(a,maccPoints2)))),

               {-ncpVuPow(0,
                  -[ncpVuPow(tEmiD0(t)*rescale/reEscale,p_maccPoints(maccPoints),max(1.E-4,1.E-4*p_maccPoints(maccPoints)))
                         - p_maccPoints(maccPoints1)],max(1.E-4,1.E-4*macc(a,maccPoints)))}

                     * ( macc(a,maccPoints)-macc(a,maccPoints1))
                     / (p_maccPoints(maccPoints)-p_maccPoints(maccPoints1))
                     ]/max(%minScale%,redEmisa.l(a,t-1)*%scaleMult%) ;
*
*        --- define current total taxed process emission (before abatement) for activity
*
         e_emisA(aact(a),v,ts(t)) $  macc(a,"0")  ..

            emisa(a,v,t)/max(%minScale%,emisa.l(a,v,t-1)*%scaleMult%)
               =E= [
*
*                  --- process emissions link to output
*
                        sum(em       $ (p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs")),
                           emical(t) *  p_phiEmiXP(em,a,t)  * p_rhoEmiXP(em,a) * xp(a,t))
*
*                  --- process emisssionslink to factor use
*
                      +  sum((em,actFp) $ (p_rhoEmiFac(em,actFp,a) and p_emiConv(em,"GHGs")),
                              emical(t) * p_phiEmiFac(em,actFp,a) * p_rhoEmiFac(em,actFp,a) * sum(flagxFac(actFp,a,v), xfac(actFp,a,v,t)))
                    ]/max(%minScale%,emisa.l(a,v,t-1)*%scaleMult%);

*
*        --- demand for Maccs (integral under MACC curve = average abatement cost per unit) times total emissions
*
         e_macDemand1(aact(a),ts(t)) $ macc(a,"0")  ..

          macDemand1(a,t)/max(%minScale%,macDemand1.l(a,t)*%scaleMult%)
            =E= 1000 *

               sum[(maccPoints,maccPoints1) $ (   macc(a,maccPoints) $ macc(a,maccPoints1)
                                                   $ (maccPoints.pos gt maccPoints1.pos)
                                                   $ (macc(a,maccPoints1)
                                                      eq smax(maccPoints2 $ (maccPoints.pos gt maccPoints2.pos), macc(a,maccPoints2)))),

                {-ncpVuPow(0,
                  -[ncpVuPow(tEmiD0(t)*rescale/reEscale,p_maccPoints(maccPoints),max(1.E-4,1.E-4*p_maccPoints(maccPoints)))
                         - p_maccPoints(maccPoints1)],max(1.E-4,1.E-4*macc(a,maccPoints)))}

                 * [ 0.5 * (macc(a,maccPoints)-macc(a,maccPoints1))
                          + macc(a,maccPoints1)
                   ]]/max(%minScale%,macDemand1.l(a,t)*%scaleMult%);

         e_macDemand("c-macc",aact(a),v,ts(t)) $ (flagXFac("c-macc",a,v) $ macc(a,"0"))  ..

           xfac("c-macc",a,v,t)*ps("c-macc",t)/max(%minScale%,ps.l("c-macc",t-1)*xfac.l("c-macc",a,v,t-1)*%scaleMult%)
            =E= macDemand1(a,t)*0.001 * emisa(a,v,ts)/max(%minScale%,ps.l("c-macc",t-1)*xfac.l("c-macc",a,v,t-1)*%scaleMult%);

*
*         --- apply relative reduction for activity to shifter variables for emissions
*
         e_chiEmiFac(em,fp,aact(a),ts(t)) $ ((p_rhoEmiFac(em,fp,a) and p_emiConv(em,"GHGs")) $  macc(a,"0")) ..

                chiEmiFac(em,fp,a,t) =E= 1. - redEmisa(a,t);

         e_chiEmiXp(em,aact(a),ts(t))      $ ((p_rhoEmiXP(em,a)   and p_emiConv(em,"GHGs"))  $  macc(a,"0")) ..

                chiEmiXp(em,a,t)     =E= 1. - redEmisa(a,t);

*         ---Aggregate unit cost of production (exhaustion)

         e_pxMac(aact(a),ts(t)) ..
           px(a,t)*xp(a,t)/max(%minScale%,xp.l(a,t-1)*px.l(a,t-1)*%scaleMult%)
              =e= [sum(flagxFac(fptop,a,v),                   xfac(fptop,a,v,t)*pxfac(fptop,a,v,t)*(1+p_markup(a,t)$ifEnTech))
                  + sum(flagXFac("c-macc",a,v) $ macc(a,"0"), xfac("c-macc",a,v,t)*ps("c-macc",t))]
                    /max(%minScale%,xp.l(a,t-1)*px.l(a,t-1)*%scaleMult%);

         option kill=redEmisa;
         option kill=macDemand1;

         emisa.l(a,v,tt)
               =  [      sum(em      $ (p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs")),
                              emical.l(tt) *  p_phiEmiXP(em,a,tt)     * p_rhoEmiXP(em,a)     * xp.l(a,tt))

                      +  sum((em,actFp) $ (p_rhoEmiFac(em,actFp,a) and p_emiConv(em,"GHGs")),
                              emical.l(tt) * p_phiEmiFac(em,actFp,a) * p_rhoEmiFac(em,actFp,a) * sum(flagxFac(actFp,a,v), xfac.l(actFp,a,v,tt)))
                    ];

      $$endif.maccs

      model m_ghgMitigation "Sub-model for carbon tax recycling and MACC mechanism"/

         $$ife %salesTaxRecl%>0   e_reyclSalesTaxes.tPafM0
         $$ife %factTaxRecl%>0    e_reyclFactorTaxes.tFacM0
         $$ife %dirTaxRecl%>0     e_reyclDirectTaxes.tDr
         $$ife %allTaxRecl%>0     e_reyclallTaxes.tMAll
         $$ife %transfersRecl%>0  e_reyclTransfers.yTaxRedist
         $$ifi "%maccs%"=="on"    e_redEmisa.redEmisa,e_emisa.emisa,e_macDemand1.macDemand1,e_chiEmiFac.chiEmiFac,e_chiEmiXp.chiEmiXp,e_macDemand.xfac -e_px +e_pxMac
      /;

     set ceilings / ceilingAbsolute /;

  $$else.decl

     $$iftheni.baseline "%task%"=="Baseline"
       p_ghgs(tt)       = emiGHG.l("GHGs",tt-1);
       p_taxes(g,tt)    = yg.l(g,tt-1);
       p_yTaxRedist(tt) = yTaxRedist.l(tt-1);

     $$endif.baseline

*
*    --- tax recycling
*
     $$ifthene.recyclDirTax %dirTaxRecl%>0
*
*        --- define a multiplicative hifter such that carbon tax revenues are recycled
*
         tDr.lo(tt) = -inf;
         tDr.up(tt) = +inf;
     $$endif.recyclDirTax

     $$ifthene.recyclSalesTax %SalesTaxRecl%>0

         tPafM0.lo(tt) = -inf;
         tPafM0.up(tt) = +inf;
     $$endif.recyclSalesTax

     $$ifthene.recyclFactTax %factTaxRecl%>0

         tFacM0.lo(tt) = -inf;
         tFacM0.up(tt) = +inf;
     $$endif.recyclFactTax

     $$ifthene.recyclAllTax %allTaxRecl%>0

         tMall.lo(tt) = -inf;
         tMall.up(tt) = +inf;

     $$endif.recyclAllTax

     $$ifthene.recyclTransfers %transfersRecl%>0

         yTaxRedist.lo(tt) = -inf;
         yTaxRedist.up(tt) = +inf;
     $$endif.recyclTransfers
*
*    ---- Simulation mode, code is called for each year
*
     $$iftheni.polInstrument "%Policy_instrument%"=="carbon tax"
*
*       --- exogenous carbon tax
*
        tEmiD0.fx(%tt%) = GHGMitigation("%GHGMitigationScenario%","carbonTax",%tt%)*reEscale*1/rescale;
        hasGhgPolicy $ sum(t,GHGMitigation("%GHGMitigationScenario%","carbonTax",t)) = 1;

     $$else.polInstrument

        $$iftheni.baseline "%task%_%Policy_instrument%"=="Baseline_Ceiling to baseline"

          $$abort "Ceiling to baseline cannot be used in baseline mode."

        $$endif.baseline
*
*       --- emission ceiling (= emissions fixed), carbon tax is endogenous
*
        $$iftheni.ceiling "%Policy_instrument%"=="Ceiling to baseline"
             emiGHG.fx("GHGs",%tt%) $ GHGMitigation("%GHGMitigationScenario%","ceilingBaseline",%tt%)
              = p_ghgs(%tt%) * GHGMitigation("%GHGMitigationScenario%","ceilingBaseline",%tt%);
             hasGhgPolicy $ sum(t,GHGMitigation("%GHGMitigationScenario%","ceilingBaseline",t)) = 1;
        $$elseifi.ceiling  "%Policy_instrument%"=="Ceiling to benchmark"
             emiGHG.fx("GHGs",%tt%) $ GHGMitigation("%GHGMitigationScenario%","ceilingBench",%tt%)
               = p_ghgs(t0)   * GHGMitigation("%GHGMitigationScenario%","ceilingBench",%tt%);
             hasGhgPolicy $ sum(t,GHGMitigation("%GHGMitigationScenario%","ceilingBench",t)) = 1;
        $$else.ceiling
             emiGHG.fx("GHGs",%tt%) $ GHGMitigation("%GHGMitigationScenario%","ceilingAbsolute",%tt%)
              = GHGMitigation("%GHGMitigationScenario%","ceilingAbsolute",%tt%);
             hasGhgPolicy $ sum(t,GHGMitigation("%GHGMitigationScenario%","ceilingAbsolute",t)) = 1;
        $$endif.ceiling

        if ( emiGHG.range("GHGs",tt) eq 0,
           tEmiD0.lo(ts(t)) = -inf;
           tEmiD0.up(ts(t)) = +inf;
           tEmiD0.l(%tt%)   = tEmiD0.l(%tt1%);
           tEmiD0.l(ts(t)) $ (Not tEmiD0.l(t-1)) = 0.01;
        );
     $$endif.polInstrument
*
*    --- define start value for emission tax and tax revenues
*
     $$iftheni.maccs "%maccs%"=="on"
*
*       --- remove maccs if neither a carbon tax nor ceiling is active
*
        macc(a,"0") $ (not hasGhgPolicy) = 0;
*
*       --- allow driving xfac demand by other equation
*
        p_alphaXfac(fpNat,fpPar,a,v) $ (sameas("c-macc",fpNat) $  macc(a,"0")) = 0;
*
*        --- release bounds for emission shifters
*
        chiEmiFac.lo(em,fp,a,%tt%) $ (macc(a,"0") $ (p_rhoEmiFac(em,fp,a) and p_emiConv(em,"GHGs"))) = -inf;
        chiEmiFac.up(em,fp,a,%tt%) $ (macc(a,"0") $ (p_rhoEmiFac(em,fp,a) and p_emiConv(em,"GHGs"))) = +inf;

        chiEmiXP.lo(em,a,%tt%)     $ (macc(a,"0") $ (p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs"))) = -inf;
        chiEmiXP.up(em,a,%tt%)     $ (macc(a,"0") $ (p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs"))) = +inf;

        redEmisa.fx(a,%tt%)        $ ( not macc(a,"0") ) = 0;

        $$iftheni.iniAddVars "%iniAddVars%"=="on"
*
*          --- recursively define start values for variables in GHG/MACC equations
*
           tEmi.l(emiFlag(em,aa),ts(t))
             = (tEmi0.l(em,aa,t) + (tEmiD0.l(t) $ p_emiConv(em,"GHGs") + tEmiD1.l(em,t) + tEmiD2.l(aa,t)))
                                  * tEmiM0.l(t) * tEmiM1.l(em,t) * tEmiM2.l(aa,t);

           rtEmi.l(emiFlag(em,aa),ts(t)) = tEmi.l(em,aa,t)/pgdpmp.l(t);

           if ( hasGhgPolicy,

              tEmiD0.l(tt) = tEmiD0.l(tt-1);
              tEmiD0.l(tt) $ tEmiD0.l(tt-2) = tEmiD0.l(tt-1) * (1 + (tEmiD0.l(tt-1)/tEmiD0.l(tt-2)-1) * 0.25);

              tEmi.l(emiFlag(em,aa),tt)  $ p_emiConv(em,"GHGs") = tEmi.l(em,aa,tt-1) + (tEmiD0.l(tt) - tEmiD0.l(tt-1));

              rtEmi.l(emiFlag(em,aa),tt) $ p_emiConv(em,"GHGs") = tEmi.l(em,aa,tt)/pgdpmp.l(tt);

              ygEmi.l(aa,tt)
               = sum((em,emSrc) $ (emiFlag(em,aa) $ p_rhoEmi(em,emSrc,aa)),
                                     tEmi.l(em,aa,tt)*emical.l(tt)*chiEmi.l(em,emSrc,aa,tt)*p_rhoEmi(em,emSrc,aa)*p_phiEmi(em,emSrc,aa)*xa.l(emSrc,aa,tt));
              $$iftheni.maccs "%maccs%"=="on"

                  emisa.l(a,v,%tt%)  $ macc(a,"0")
                      =  [      sum(em      $ (p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs")),
                                     emical.l(%tt%) * p_phiEmiXP(em,a,%tt%)*p_rhoEmiXP(em,a)*xp.l(a,%tt%))

                             +  sum((em,actfp) $ (p_rhoEmiFac(em,actfp,a) and p_emiConv(em,"GHGs")),
                                     emical.l(%tt%) * p_phiEmiFac(em,actFp,a) * p_rhoEmiFac(em,actFp,a)*sum(flagxFac(actFp,a,v), xfac.l(actFp,a,v,%tt%)))
                           ];

                  redEmisa.l(a,%tt%) $ macc(a,"0")

                     = sum[(maccPoints,maccPoints1) $ (   macc(a,maccPoints) $ macc(a,maccPoints1)
                                                         $ (maccPoints.pos gt maccPoints1.pos)
                                                         $ (macc(a,maccPoints1)
                                                            eq smax(maccPoints2 $ (maccPoints.pos gt maccPoints2.pos), macc(a,maccPoints2)))),

                      {-ncpVuPow(0,
                         -[ncpVuPow(tEmiD0.l(%tt%)*1000*rescale/reEscale,p_maccPoints(maccPoints),max(1.E-4,1.E-4*p_maccPoints(maccPoints)))
                                - p_maccPoints(maccPoints1)],max(1.E-4,1.E-4*macc(a,maccPoints)))}

                            * ( macc(a,maccPoints)-macc(a,maccPoints1))
                            / (p_maccPoints(maccPoints)-p_maccPoints(maccPoints1))
                            ];

                  chiEmiFac.l(em,fp,a,tt)      $ ((p_rhoEmiFac(em,fp,a) and p_emiConv(em,"GHGs")) $  macc(a,"0")) = (1. - redEmisa.l(a,tt));
                  chiEmiXp.l(emiFlag(em,a),tt) $ ((p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs")) $  macc(a,"0")) = (1. - redEmisa.l(a,tt));

                  ygEmiXP.l(a,tt)
                    =  sum(emiFlag(em,a) $ p_rhoEmiXP(em,a),       tEmi.l(em,a,tt)*emical.l(tt)*chiEmiXP.l(em,a,tt)       *p_rhoEmiXP(em,a)       *xp.l(a,tt)*p_phiEmiXP(em,a,tt))
                      +sum((em,actFp)    $ p_rhoEmiFac(em,actFp,a),tEmi.l(em,a,tt)*emical.l(tt)*chiEmiFac.l(em,actFp,a,tt)*p_rhoEmiFac(em,actFp,a)*p_phiEmiFac(em,actFp,a)
                                                                *sum(flagxFac(actFp,a,v), xfac.l(actFp,a,v,tt)));

                  macDemand1.l(aact(a),%tt%) $ macc(a,"0")
                   = sum[(maccPoints,maccPoints1) $ (   macc(a,maccPoints) $ macc(a,maccPoints1)
                                                          $ (maccPoints.pos gt maccPoints1.pos)
                                                          $ (macc(a,maccPoints1)
                                                             eq smax(maccPoints2 $ (maccPoints.pos gt maccPoints2.pos), macc(a,maccPoints2)))),

                       {-ncpVuPow(0,
                         -[ncpVuPow(tEmiD0.l(%tt%)*1000*rescale/reEscale,p_maccPoints(maccPoints),max(1.E-4,1.E-4*p_maccPoints(maccPoints)))
                                - p_maccPoints(maccPoints1)],max(1.E-4,1.E-4*macc(a,maccPoints)))}

                        * [ 0.5 * (macc(a,maccPoints)-macc(a,maccPoints1))
                                 + macc(a,maccPoints1)
                          ]];

                  xfac.l("c-macc",a,v,%tt%) $ (flagXFac("c-macc",a,v) $ macc(a,"0"))
                   = max(1.E-10, macDemand1.l(a,%tt%)*(0.001/rescale) * emisa.l(a,v,%tt%)*reEscale/ps.l("c-macc",%tt%));

                  xa.l("c-macc",a,%tt%) $ sum(flagXFac("c-macc",a,v) $ macc(a,"0"),1) = sum(flagXFac("c-macc",a,v),xfac.l("c-macc",a,v,%tt%));
                  xat.l("c-macc",%tt%) = sum(aa $ sam("c-macc",aa), xa.l("c-macc",aa,%tt%));
                  xd.l("c-macc",%tt%)  =  xd.l("c-macc",%tt1%)* xat.l("c-macc",%tt%)/xat.l("c-macc",%tt1%);
                  xfac.l(flagXfac(fpNest,"a-macc",v),%tt%) $ xfac.l(fpNest,"a-macc",v,%tt1%)
                    = xfac.l(fpNest,"a-macc",v,%tt1%)  * xat.l("c-macc",%tt%)/xat.l("c-macc",%tt1%);

              $$endif.maccs
           );
        $$else.iniAddVars
*
*          --- use t-1 resutls as start values
*
           chiEmiFac.l(em,fp,a,%tt%) $ (macc(a,"0") $ (p_rhoEmiFac(em,fp,a) and p_emiConv(em,"GHGs"))) = chiEmiFac.l(em,fp,a,%tt1%);
           chiEmiXP.l(em,a,%tt%)     $ (macc(a,"0") $ (p_rhoEmiXP(em,a)     and p_emiConv(em,"GHGs"))) = chiEmiXP.l(em,a,%tt1%);
           emisa.l(a,v,%tt%)  $ macc(a,"0") = emisa.l(a,v,%tt1%);
           redEmisa.l(a,%tt%) $ macc(a,"0") = redEmisa.l(a,%tt%-1);
           macDemand1.l(a,%tt%) $ macc(a,"0") = macDemand1.l(a,%tt%-1);

        $$endif.iniAddVars
     $$endif.maccs

  $$endif.decl
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
