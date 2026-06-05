********************************************************************************
$ontext

   MAANGE project

   GAMS file : MACTOTAL_EG_AGG.INC

   @purpose  : Define macro SAM and macto total
   @author   :
   @date     : 07.03.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : SAMEST

$offtext
********************************************************************************
$ontext
*SR 04/06

This file defines macro aggregate controls through:

(1) A macro-sam defined as an aggregation of the micro-sam. The accounts in
    the macro-sam are defined in the set acm in the excel file. The
    aggregation set, MACMAP3, is set in MACROSAM3.INC based on the
    data from the excel file on aggregations.

(2) Other macro aggregates from the micro-sam are defined by the set
    MACRO and the aggregator set macAgg(ac,ac,macro). The set MACRO is
    specified in the excel file and macAgg is set in MACROSAM3.INC. The control
    totals actually used are given in the set MACRO2(MACRO), also defined
    in the excel file, or through aggregation of the proto-sam.

The procedure for defining the various macro control totals is:

(1) Define a macro-sam, MACSAM3(acm,acm). The accounts are  given by acm
    which is specified in the excel input file. The aggregation
    set for MACSAM3 is MACMAP3(acm,ac), which is determined given the
    aggregates provided in the excel input file.

(2) The macro-sam control totals, macSam(acm,acm), are either read in
    separately (MACSAM2) from the Excel input file or aggregated from the
    proto-sam (MACSAM3). The matrix MACSAM2 can be a partial aggregate
    macro-sam, if desired, consisting only of elements that are known.
    The missing elements will then be taken from aggregating the proto-sam.
    MACSAM2 and/or the proto-sam (SAM and MACSAM3) do not need to be balanced,
    since they represent prior estimates.

(3) The other macro control totals are given in the set MACRO defined in
    the excel input file, and the aggregation set macAgg(ac,ac,macro).

(4) The macro control totals, macTotal, are either read directly from the
    excel file (macTotal0) or computed from the proto-sam (macTotal2).
$offtext

 $$iftheni.useMacTotal "%MacroControl%"=="Macro Totals"

     $$iftheni.MFMOD "%useMFMod%"=="on"

          set MfModYear / "2020" /;
          set MfModReg  / "%RegGtapName%" /;
          set MfModMacro  / NyGdpFcstKN,NyGdpMktPKN,NeConPrvTKN,NeConGovTKN,NEGDIFTOTKN /;

          parameter MFMOD(mfModYear,mfModReg,*);

          execute_load "dat/mfModMacro.gdx" mfMod;


          macTotal("tgdpfc2","mfMod")  =  MFMOD("2020","%RegGtapName%","NyGdpFcstKN");
          macTotal("tgdpmp2","mfMod")  =  MFMOD("2020","%RegGtapName%","NyGdpMktpKN");
*         macTotal("tindtax2","mfMod") =  MFMOD("2020","%RegGtapName%","NyGdpMktpKN");
          macTotal("tfidemand2","mfMod") =  MFMOD("2020","%RegGtapName%","NeConPrvtKN")+MFMOD("2020","%RegGtapName%","NEGDIFTOTKN");

          macro2(macro) $ macTotal(macro,"mfMod") = YES;
          macTotal(macro2,"scaled") $ macTotal(macro2,"mfMod") = macTotal(macro2,"mfMod");

     $$endif.MFMOD

*   --- Define macro aggregation set macAgg for aggregates in set MACRO
*       These must be defined individually. The list includes a lot of detailed
*       accounts that might be imposed. The macro aggregates actually imposed are
*       determined by the set MACRO2(MACRO) in the excel file.

*   --- Commodity accounts
     macAgg(cini,iini,"tfidemand2")               = 1;


*   --- Factor accounts
*       GDP at factor cost equals all payments to factors from activities. If there
*       are any payments from the world (w) to factors, they should be seen as part
*       of GNP at factor cost, not GDP.
     macAgg(fini,aini,"tgdpfc2")                  = 1;

*   --- Note that GDP at market prices includes GDPFC plus all indirect taxes.
*       If there are indirect tax accounts, gtind, they will be used. If there
*       are payments from activities and commodities to government, they are assumed
*       to be indirect taxes.
     macAgg(fini,aini,"tgdpmp2")                  = 1;
     macAgg(gtindIni,aini,"tgdpmp2")              = 1;
     macAgg(gtindIni,cini,"tgdpmp2")              = 1;
     macAgg(gini,aini,"tgdpmp2")                  = 1;
     macAgg(gini,cini,"tgdpmp2")                  = 1;

     macAgg(kIni,wIni,"tgosyrow2")                = 1;
     macAgg(lIni,wIni,"tlabyrow2")                = 1;

*   --- Enterprise Accounts
     macAgg(eIni,fIni,"teyfact2")                 = 1;
     macAgg(eIni,gIni,"teygov2")                  = 1;

*   --- Government Accounts
     macAgg(gIni,cIni,"tcomtax2")                 = 1;
*    macAgg("Imptax",c,"timptax2")          = 1;
*    macAgg("Vatax",c,"tvattax2")           = 1;
*    macAgg("Vatax",a,"tvattax2")           = 1;
     macAgg(etaxIni,cIni,"texctax2")              = 1;
     macAgg(gIni,aIni,"tacttax2")                 = 1;
     macAgg(gtindIni,aIni,"tindtax2")             = 1;
     macAgg(gIni,eIni,"tdetax2")                  = 1;

*   --- Capital Account
     macAgg(iIni,kIni,"tdeprec2")                 = 1;
     macAgg(iIni,eIni,"tesav2")                   = 1;

*   --- RoW Account
     macAgg(wIni,eIni,"trowyent2")                 =  1;
     macAgg(fIni,wIni,"tnetfacrow2")               =  1;
     macAgg(wIni,fIni,"tnetfacrow2")               = -1;

*   --- Compute macTotal2 from initial micro proto-sam, SAM

     macTotal(macro,"cleansed") = SUM((irow,icol), SAM(irow,icol,"cleansed")*macAgg(irow,icol,macro));

     parameter p_scaleToMacro;
     p_scaleToMacro $ card(macro2)
      = sum(macro2 $ macTotal(macro2,"cleansed") , (macTotal(macro2,"scaled")/macTotal(macro2,"cleansed"))*macTotal(macro2,"scaled"))
       /sum(macro2 $ macTotal(macro2,"cleansed") , macTotal(macro2,"scaled"));

     macTotal(macro2,"scaled") = macTotal(macro2,"scaled")/p_ScaleToMacro;
     SAM(irow,icol,"cleansed") = SAM(irow,icol,"cleansed") * p_scaleToMacro;

     option kill=macSet;
     option kill=acmnt;
     option kill=acMNT2P;
     option kill=macmnz;

$else.useMacTotal

*    ---------------------------------------------------------------------------
*
*     Define macro-sam aggregation set and calculate MACSAM3 from proto-sam, SAM
*
*    ---------------------------------------------------------------------------

*    --- Include file macrosam3.inc defines:
*
*    (1) MACMAP3(acmac3,ac) for creating macro-sam from the micro-sam,
*    (2) aggregated MACSAM3 from the proto-sam, SAM, and
*    (3) set of non-zero macro-sam elements in MACSAM3, MSAMNZ(acmnt,acmntp).

     $$include "inc/samest/macrosam3.inc"

*   --- Define macro-sam control totals for estimation. Values from MACSAM2
*       in excel file are used, if available. If not, elements are taken from
*       macro-sam aggregated from proto-sam, MACSAM3.

     macSam(acm,acmp,"used")                                  = macSam(acm,acmp,"scaled");
     macSam(acm,acmp,"used") $ (macSam(acm,acmp,"used") eq 0) = macSam(acm,acmp,"fromProto");
     macmnz(acm,acmp)        $ macSam(acm,acmp,"used")        = yes;


*   --- Define macro-sam constraints actually imposed, macset(acm,acm). Initially
*       read from excel file (stderrs4), but correct it here if some macsam
*        elements are always zero.

     macset(acm,acmp)                            = yes;
     macset(acm,acmp)$(macmnz(acm,acmp) eq 0)    = no;

     option kill=macTotal;
     option kill=macAgg;

$$endif.useMacTotal
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
