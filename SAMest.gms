********************************************************************************
$ontext

   MANAGE project

   GAMS file : SAMEST.GMS

   @purpose  :
   @author   :
   @date     : 03.03.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy :

$offtext
********************************************************************************
$onglobal
$offListing
$include 'fromGuiToRun.gms'
$$setglobal pgmName SAMEST
$$include "inc/title_def.inc"
$$setGlobal regF     ./reg/%RegGtapName%
$$onEmbeddedCode Python:
     import os
     SAM = "%samXLS%"
     if "GTAP" in SAM:
        os.environ["GTAP_SAM"] = "on"
     else:
        os.environ["GTAP_SAM"] = "off"
$$offEmbeddedCode



SETS

* --- SAM Sets are assigned in Excel worksheet sets

 acIni                 "Accounts of SAM before disaggregation"
 ac                    "SAM accounts"
 c(ac)                 "Commodities"
 a(ac)                 "Activities"
 f(ac)                 "Factors"
 l(ac)                 "Labour Factors"
 h(ac)                 "Households"
 g(ac)                 "Government"
 gt(ac)                "tax accounts"
 ft(gt)                "factor tax accounts"
 map_f_ft(f,ft)        "factors tp factor tax accounts"
 gtind(ac)             "Indirect tax accounts"
 gtdir(ac)             "Direct tax accounts"
 e(ac)                 "Enterprises"
 i(ac)                 "Investment"
 w(ac)                 "Rest of the world"
 acm                   "Macro-sam accounts"
 fin(ac)               "Final demand"

 fIni(acIni)           "Factors"
 wIni(acIni)           "Rest of the world"
 iIni(acIni)           "Factors"
 cIni(acIni)           "Commodities"
 aIni(acIni)           "Activities"
 hIni(acIni)           "Housholds"
 gtIni(acIni)          "Taxes"
 gtindIni(acIni)       "Indirect tax accounts"
 gIni(acIni)           "Goverment"
 iIni(acIni)           "Investments"
 wIni(acIni)           "Rest of the world"
*
* --- relating to macro totals and macro SAM
*

 macro                 "potential macro control totals"
 macro2(macro)         "macro constraints actually imposed"

 acmnt(acm)            "Macro-Sam accounts except for total3"
 acmnt2(acm)           "Macro-Sam accounts except for total3 and fctr3"
 acm33(acm)            "all acm except total3 and other3"
 macmap3(acm,ac)       "mapping from ac to acm for aggregating sam to macsam"
 macmapIni(acm,acIni)  "mapping from acini to acm for aggregating sam to macsam"
 acIniMac(acIni)       "Accounts coming from macroSAM directly"
 macSet(acm,acm)       "mac-sam constraints to be imposed"
*
 rwCl                  "row and column constraints"  /row1, col1/
 rwCl1                 "row and column data"     / rowSum1, colSum1 /


* --- Sets assigned within the programme

 acbal(acini)              "accounts with row-column balance"
 acbalf(ac)                "accounts with row-column balance"
 acnt(ac)                  "all accounts except totals"
 acInInt(acIni)            "all accounts except totals"

 smlCell(acini,acini)      "SAM cells with abs value < cutoff are removed"
 iNeg(acini,acini)         "cells with negative values in data"
 iZero(acini,acini)        "cells with zero entry"
 nonZero(acini,acini)      "cells with nonzero entry"
 iCoeff(acIni,acini)       "cell coefficients to be estimated"
 iValue(acini,acini)       "cell values to be estimated"
 estimate(acini,acini)     "cells to be estimated and not fixed"
 iFixV(acini,acini)        "cell value fixed with no error"
 aCell(acini,acini)        "additive error for cells"
 lCell(acini,acini)        "logarithmic error for cells"

 icol(acini)               "columns included in estimation"
 icol2(acini)              "column sums to be constrained with or without error"
 icolNZ(acini)             "columns with nonzero sum"
 irow(acini)               "rows included in estimation"
 irow2(acini)              "rows to be constrained"

* --- Sets for error checks on constraints

 iCheck(acini,acini)       "Error check for cell constraints"
 iCheck2(acini)            "Check set for irow or icol but not in acnt"

* ---- maps

 map_acIni_ac(acIni,ac)       "Mapping of Initial SAM to SAM"
 map_acIni_acini(acIni,acini) "Mapping of Initial SAM to SAM"

;

alias(ac,acp,acpP,acpPP);
alias(acini,ACiniP,ACiniPP,ACiniPPP);
alias(acInInt,acInIntP,ACinintPP,ACinintPPP);
alias(acnt,acntP,acntPP,acntPPP);
alias(acBal,acBalP);

alias (acmnt,acmntp,acmntpp,acmntppp), (acm,acmp), (acmnt2, acmnt2p);

PARAMETER

* --- Parameters used for data entry ----------------------------------------

 samIni(acIni,acIni)            "Original sam as read from EXCEL"
 samscal                        "sam scaling factor"
 macscale                       "Scale choice for v_macroSam2"
 sam(*,*,*)                     "SAM, different states"
 splitShr(acIni,ac,acInip,acp)  "Split shares to calculate the protosam from initial sam"
 splitShrOri(acIni,ac,acInip,acp)  "Split shares, loaded from bridgeFile"
 sumSplit(acIni,acInip)         "Sum of split shares"
 macSam2(acm,acm)               "Macro-sam read in excel file"
 macSam(acm,acm,*)              "Macro SAM, different satetes"
 samBalChk3(acm)                "macro-sam row and column balance check"
 macmNz(acm,acm)                "non-zero entries of v_macroSam"

* --- macro aggregator
 macAgg(acIni,acIni,macro)      "Macro aggregator matrix (either 1 or -1)"

* --- Parameters used to control model flow

 smlCellcount                   "Number of small cells"
 nearcutoffcount                "Number of values between cutoff and 10 times cutoff value"
 sparsewarn(acini,acini)        "Cutoff cells in sparse rows or columns"
 epsilon                        "epsilon value for cross-entropy minimand"   /1e-6/
 countcol(acini)                "number of nonzero cells in columns"
 countrow(acini)                "number of nonzero cells in rows"
 cutoff                         "lower bound on absolute cell values"
 displ                          "control for DISPLAY statements"
 outgdx                         "control for GDX write out statements"

* --- Parameters used in estimation procedure

 coeffTarget(acini,acini)       "targets column coefficients"
 colSum0(acini)                 "initial column sum"
 rowSum0(acini)                 "initial row sum"
 sambalchk(*,*)                 "col sums minus row sums for acbal"
 sambalchkp(*,*)                "sambalchk as percent of column sum"

 p_wgt(acini,acini)             "Weights"


 colTarget(acini)               "target column sum"
 rowtarget(acini)               "target row sum"

 macTotal(macro,*)              "Macro control totals used in estimation"
 macroMAT(macro)                "Macro totals and prior standard errors from sam"

* --- Parameters used for descriptive statisitcs

 samStat
 fixes(*,*)
;
* ---------------------------------------------------------------------------
*
*   Read sam from XSL and convert to GDX, load sets
*
* ---------------------------------------------------------------------------

  $$batinclude "inc/title.inc" '" %regGtapName%: Input SAM, sets and maps from EXCEL"'

  $$CALL "GDXXRW i=reg/%regGtapName%/%samXLS%.xlsx o=reg/%regGtapName%/%samXLS%.gdx index=Layout!A4 MaxDupeErrors = 100"
  $$GDXIN "reg/%regGtapName%/%samXLS%.gdx"
*    --- Micro sam Sets
     $$LOADdc  ac acIni<samIni.dim1 acIniMac
     $$LOADDC  c a f h g i w
     $$LOADdc  gt gtind gtdir
     $$LOADdc  acm
     $$LOADdc  macsam2
     $$LOADdc  macscale
     $$LOADdc  fixes
     $$loaddc  macmap3 samIni map_acIni_ac  map_acIni_acini
   $$GDXIN



*   --- Define sets that exclude total and other elements

   acnt(ac)              = YES;
   acnt('total0')        = NO;
   acInInt(acIni)        = YES;
   acInInt('total-I')    = NO;

* --- Take over macro totals or remove depending on useMacroTotal

  $$iftheni.useMacTotal "%MacroControl%"=="Macro Totals"

     set

       k(ac)                 "Capital Factors"
       l(ac)                 "Labor factors"
       kIni(acIni)           "Capital factors"
       lIni(acIni)           "Labor factors"
       eIni(acIni)           "Enterprises"
       etax(ac)              "Export tax account"
       etaxIni(acIni)        "Export tax account"

      ;

     $$GDXIN "reg/%regGtapName%/%samXLS%.gdx"
        $$LOADDC  k l etax e
        $$LOADdc  macro macro2<macroMat.dim1
        $$LOADdc  macromat
     $$GDXIN

     macTotal(macro2,"in")  = macromat(macro2);
     abort $ (card(macro2) eq 0) "Use of macro totals is on, but all entries in macroMat are zero, file: %system.fn%, line: %system.incline%";
     option kill=macSam;
     option kill=macMapIni;
     set acm_acm(acm,acm);
     option kill=acm_acm;
     set acmDupl(acm);
     option kill=acmDupl;

     kIni(acIni)     $ sum(k,map_acIni_ac(acIni,k))       = YES;
     lIni(acIni)     $ sum(l,map_acIni_ac(acIni,l))       = YES;
     eIni(acIni)     $ sum(e,map_acIni_ac(acIni,e))       = YES;
     etaxIni(acIni)  $ sum(etax,map_acIni_ac(acIni,etax)) = YES;


  $$else.useMacTotal
*
*   --- assign macro SAM
*
     macSam(acm,acmp,"in") $ (abs(macSam2(acm,acmp)) gt 1.E-20) = macSam2(acm,acmp);
     abort $ (card(macSam) eq 0) "Use of macro SAM is on, but all entries in macSam2 are zero, file: %system.fn%, line: %system.incline%";
     option kill=macTotal;

*   --- Define sets that exclude total and other elements

     acmnt(acm)            = yes;
     acmnt("total3")       = no;
     acmnt2(acm)           = acmnt(acm);
     acm33(acmnt)          = yes;

*   --- Define the "other3" account to include any accounts not specified in one
*     of the aggregate accounts.

     macmap3("other3",acnt)$(NOT sum(acm33, macmap3(acm33,acnt))) = yes;
*
*   --- check for correct mapping of macro accounts
*
     Parameter macMapCount(*)  "Count of ac entries in macmap3";
     macMapCount(acnt) = sum(macmap3(acm,acnt), 1) +eps;

     if (sum(acnt $ (macMapCount(acnt) ne 1),1),
        macMapCount(acnt) $ (macMapCount(acnt) eq 1) = 0;
        abort "All macMapCount entries should be 1, file: %system.fn%, line: %system.incline%", macMapCount;
     );

     abort $ (sum(acnt, macMapCount(acnt)) ne card(acnt))
                  "Not all macro accounts are mapped to a account in the proto SAM, file: %system.fn%, line: %system.incline%",macMapCount;

     macMapIni(acm,acIni) $ sum(map_acIni_ac(acIni,ac),macmap3(acm,ac)) = YES;

     set acm_acm(acm,acm);
     acm_acm(acm,acmp) $ sum(macMapIni(acm,acIni) $ macMapIni(acmp,acini),1) = YES;

     set acmDupl(acm);
     acmDupl(acm) $ (sum(acm_acm(acm,acmp),1) gt 1) = YES;
     display $ card(acmDupl)
             "Warning: Some proto-SAM accounts are assigned to multiple macro-SAM accounts. Will be considered in split step", acmDupl;

     set acmEmpty(acm);
     acmEmpty(acm) $ (sum(acm_acm(acm,acmp),1) lt 1) = YES;
     acmEmpty("total3") = no;
     acmEmpty("other3") = no;
     abort $ card(acmEmpty)
             "Error: Some macro-SAM accounts are not assigned to any proto-SAM accounts", acmEmpty;


     macMapCount(acIni) = sum(macMapIni(acm,acIni), 1);



  $$endif.useMacTotal

 alias (cini,cinip),(aini,ainip),(hini,hinip);

 fIni(acIni)  $ sum(f,map_acIni_ac(acIni,f))    = YES;
 wIni(acIni)  $ sum(w,map_acIni_ac(acIni,w))    = YES;
 iIni(acIni)  $ sum(i,map_acIni_ac(acIni,i))    = YES;
 cIni(acIni)  $ sum(c,map_acIni_ac(acIni,c))    = YES;
 aIni(acIni)  $ sum(a,map_acIni_ac(acIni,a))    = YES;
 hIni(acIni)  $ sum(h,map_acIni_ac(acIni,h))    = YES;
 gtIni(acIni) $ sum(gt,map_acIni_ac(acIni,gt))  = YES;
 gtIndIni(acIni) $ sum(gtInd,map_acIni_ac(acIni,gtInd))  = YES;
 gIni(acIni)  $ sum(g,map_acIni_ac(acIni,g))    = YES;
 wIni(acIni)  $ sum(w,map_acIni_ac(acIni,w))    = YES;
 iIni(acIni)  $ sum(i,map_acIni_ac(acIni,i))    = YES;

 fin(h) = YES;
 fin(g) = YES;
 fin(i) = YES;
*
* --- read split shares
*
  $$GDXIN "reg/%regGtapName%/%samXLS%.gdx"
    $$LOADdc splitShr
    $$LOADdc splitShrOri=splitShr
  $$GDXIN



 $$batinclude "inc/title.inc" '" %regGtapName%: Aggregate to target SAM"'

 Parameter SamBal(*,*)       "Balance of sam";
*
* -- aggregate to desired proto SAM  from original one
*
  sam(acIni,acInip,"read") = samIni(acini,acinip);

  sam(acIni,acInip,"read")
    = sum((map_acIni_acIni(acInipp,acIni),acInippp) $ map_acIni_acIni(acInippp,acInip),samIni(acInipp,acInippp));


$macro balsam(sam,AC,TOTAL,pos) \
 sam('&TOTAL&',&AC&P,pos) = 0; \
 sam(ac,'&TOTAL&',pos)  = 0; \
 sam("&TOTAL&",&ac&p,pos)  = sum(ac,  sam(ac,&ac&p,pos)); \
 sam(ac,"&TOTAL&",pos)     = sum(ac&p, sam(ac,&ac&p,pos)); \
 sam&Bal(ac,"lvl") =  sam("&TOTAL&",ac,pos) - sam(&ac&,"&TOTAL&",pos);    \
 sam&Bal(ac,"prcCT") $ sam("&TOTAL&",ac,pos)   =  100*(sam("&TOTAL&",ac,pos) - sam(&ac&,"&TOTAL&",pos))/sam("&TOTAL&",ac,pos);    \
 sam&Bal(ac,"prcRT") $ sam(&ac&,"&TOTAL&",pos) =  100*(sam("&TOTAL&",ac,pos) - sam(&ac&,"&TOTAL&",pos))/sam(&ac&,"&TOTAL&",pos);    \
 sam&Bal(ac,"lvl")   $ (abs(sam&Bal(ac,"lvl"))   lt 1E-7) =  0;      \
 sam&Bal(ac,"prcCT") $ (abs(sam&Bal(ac,"prcCT")) lt 1E-7) =  0;     \
 sam&Bal(ac,"prcRT") $ (abs(sam&Bal(ac,"prcRT")) lt 1E-7) =  0;

 balsam(sam,acIni,Total-i,"read")

 samscal        = %samScale%;
 $$iftheni.useMacSAM "%MacroControl%"=="Macro SAM"

     $$batinclude "inc/title.inc" '" %regGtapName%: Update based on MACRO SAM"'

*    -----------------------------------------------------------------------------
*
*       Update (not yet dis-aggregated) SAM to macroSAM
*
*    -----------------------------------------------------------------------------

*    --- define macro SAM shares from SAM as read in

     parameter macShares(acIni,acIni);
    ;
    macSam(acm,acmp,"proto") = sum((macmapIni(acm,acInipp),acInippp) $ (macmapIni(acmp,acInippp) $ (abs(sam(acInipp,acInippp,"read")) gt 1.E-20)),
                                    sam(acInipp,acInippp,"read"));




*
*   --- check if some macro SAM non-empty cells are linked to empty ones in the proto
*
    parameter emptyMacSam;
    emptyMacSam(acm,acmp) $ ((not macSam(acm,acmp,"proto")) and macSam(acm,acmp,"in")) = macSam(acm,acmp,"in");
    abort $ card(emptyMacSam)
             "Some cells in the macro SAM are mapped to block of empty cells in the proto SAM, file: %system.fn%, line: %system.incline%",emptyMacSam;

    emptyMacSam(acm,acmp) $ (macSam(acm,acmp,"proto") and (Not macSam(acm,acmp,"in"))) = macSam(acm,acmp,"proto");
    abort $ card(emptyMacSam)
             "Some non-empty block of cells in the proto SAM are mapped to empty cells in the macro SAM, file: %system.fn%, line: %system.incline%",emptyMacSam;

    macShares(acInipp,acInippp)
         = sum((macmapIni(acm,acInipp),acmp) $ (macmapIni(acmp,acInippp) $ macSam(acm,acmp,"proto")),
             sam(acInipp,acInippp,"read")/ macSam(acm,acmp,"proto"));
*
*   --- accounts dirrectly from macro SAM
*
   alias (acIniMac,acIniMacp);
   macShares(acIniMac,acIniMacp) $ sum((macMapIni(acm,acIniMac),acmp) $ macMapIni(acmp,acIniMacp),1) = 1;

*  --- create the initial sam by applying shares dervied from read-in SAM to Macro SAM

    sam(acIni,acInip,"ini") = sum((macMapIni(acm,acIni),acmp) $ macmapIni(acmp,acInip), macSam(acm,acmp,"in") * macShares(acIni,acInip));
* --- assume that macSam amd macTotal is already scaled if macScale=1.
    macSam(acm,acmp,"scaled") $ (macScale eq 0)  = macSam(acm,acmp,"in")/samscal;
$else.useMACSam
    sam(acIni,acInip,"ini") = sam(acIni,acInip,"read");

* --- assume that macSam amd macTotal is already scaled if macScale=1.

    macTotal(macro,"scaled")  $ (macTotal(macro,"in") gt 1.E-10)  = macTotal(macro,"in")/samscal;


$endif.useMACSam


    balsam(sam,acIni,Total-i,"ini")


* ----------------------------------------------------------------------------
*
*    SAM scaling and removal of small cells
*
* ----------------------------------------------------------------------------

 $$batinclude "inc/title.inc" '" %regGtapName%: Scale and remove small cells"'

 cutoff         = 1.E%cutOff%;

*SR Default for target is 1 (average of column and row sums)
*   To use other options, must specify default3
 outgdx         = 2;

* --- scale SAM

 sam(acini,acinip,"scaled")       = sam(acini,acinip,"ini")/samscal;
 fixes(acini,acinip)              = fixes(acini,acinip)/samscal;


* --  Initialize row and column sums to zero

 sam('total0',acinip,"scaled") = 0;
 sam(acini,'total0',"scaled")  = 0;

* --- Remove tiny cells

 smlCell(acInInt,acInIntp) $ (sam(acInInt,acInIntp,"scaled") and (abs(sam(acInInt,acInIntp,"scaled")) lt cutoff)) = yes;

* --- The cells corresponding to elements in smlCell are set at zero.

 sam(acInInt,acInIntp,"cleansed") = sam(acInInt,acInIntp,"scaled");
 sam(acInInt,acInIntp,"cleansed") $ smlCell(acInInt,acInIntp) = 0;

* --- Generate column and row sums
 sam('total0',acIni,"cleansed") = 0;
 sam(ac,'total0',"cleansed")  = 0;
 sam("total0",acInip,"cleansed")  = sum(acIni,  sam(acIni,acInip,"cleansed"));
 sam(acIni,"total0","cleansed")   = sum(acIniP, sam(acIni,acInip,"cleansed"));

$ontext
 Warns if removing cells results in sparse rows or columns (with less than
 5 nonzero entries remaining). This could indicate that important information
 is being lost and could cause potentially serious problems with the
 estimation process.

 Interpretation of nonzero cells in sparsewarn:

  1 - row is sparse
  2 - column is sparse
  3 - both column and row are sparse

$offtext

 sparsewarn(acIniNt,acIniNtp)
          =   1 $ (smlCell(acIniNt,acIniNtp) and sum((acIniNtpp) $ sam(acIniNt,acIniNtpp,"cleansed"),1)  < 5)
            + 2 $ (smlCell(acIniNt,acIniNtp) and sum((acIniNtpp) $ sam(acIniNtpp,acIniNtp,"cleansed"),1) < 5);

* --- Small cells and near-small cells counted

 smlCellcount   =  sum(smlCell,1);

 nearcutoffcount  =  sum((acIniNt,acIniNtp) $ (     (abs(sam(acIniNt,acIniNtp,"cleansed"))>cutoff)
                                          and (abs(sam(acIniNt,acIniNtp,"cleansed"))<(10*cutoff))),1);

* --- icol irow Non zero columns and rows
*      For default, count nonzero elements in columns and rows and set icol and irow
*      to include all columns and rows with any nonzero elements.

 countcol(acIniNt)  = sum(acIniNtp $ sam(acIniNtp,acIniNt,"cleansed"),1);
 countrow(acIniNt)  = sum(acIniNtp $ sam(acIniNt,acIniNtp,"cleansed"),1);

$ontext

 Define column and row sets for all accounts with any nonzero elements in the
 corresponding row and column. A pure balancing sam account might have a row or
 column with all zeros. It is harmless to exclude such accounts from the
 balancing procedure.

 If one wants to estmate a subset of the sam, then icol and irow would need to
 be redefined to include only those accounts. The code has not been tested
 for estimating sub-matrices, but it should be feasible to do do. One must
 also check acBal to ensure that row and column sum equality is imposed only
 for those accounts for which it is valid.

$offtext

 icol(acIniNt) $ (countcol(acIniNt) or countRow(acIniNt))   = yes;
 irow(acIniNt) $ (countrow(acIniNt) or countCol(acIniNt))  = yes;

* --- Assign Values to macAgg and macTotal0 ------------------------------------

$ontext

 macAgg(ac,acp,macro) is a mapping of micro sam entries to target macro totals
 in macTotal0. A number of macro aggregates are pre-specified in
 macTotal_eg_agg.inc, and the set is specified in the excel file, sheet "macro".

 The controlling set on macTotal0 is macro, i.e., all the macro control
 totals that might POTENTIALLY be used in the programme. Those ACTUALLY
 imposed are a subset - macro2 - of macro.

 In addition, macro totals are defined in a standard macro-sam. Any elements of
 the macro-sam can be specified as a macro constraint, with its own prior
 standard error. This is done in the excel file, sheet "macro-sam". The set
 macSet(acmnt,acmntp) determines which are used.

 The set macSet is calculated in macTotal_eg_agg.inc if default2 = 1.

 If macro-toals are given, this will also scale the SAM to better
 match the totals

$offtext


$INCLUDE "inc/samest/macTotal_eg_agg.inc"

* --- Descriptive Statistics for the sam Used for Estimation ----------------

$ontext

 These descriptive statistics refer to the best estimate sam ACTUALLY used
 in the estimation procedure, i.e., after all scaling and adjustments.

 These statistics are useful if the programme is having difficulties; they
 provide information for setting the scaling and cutoff parameters.

 The same statistics are also calculated for the estimated sam - samCe.

$offtext

$batInclude "inc/samest/samStat.inc" cleansed acIniNt

$ontext
 Define acBal for common accounts in icol and irow
 Default (default0 = 1) assumes common rows and columns are to be balanced,
 which is standard for estimating sams.

 acBal need not include the whole matrix, and the code will work for
 rectangular matrices such as input-output tables rather than square sams.
 In this case, corresponding rows and columns may be equal, but may not.

 By default, acBal includes common set elements in irow and icol.
 If balancing only part of a sam, acBal needs to be only set to common rows and
 columns that include all elements and hence should balance.

 If default0 is 0 then acBal is empty and no row-column sum constraints
 are imposed.
$offtext

 acBal(acIniNt) $ (icol(acIniNt) and irow(acIniNt)) = yes;

option countcol:0, countrow:0;

* --- Assign Values for sam Balancing (irow,icol is a subset of the whole SAM)

 $$batinclude "inc/title.inc" '" %regGtapName%: Prepare for balancing"'

 sam(irow,icol,"target")        = sam(irow,icol,"cleansed");
 sam(irow,icol,"fixes")         = fixes(irow,icol);
 sam(irow,icol,"target")        $ fixes(irow,icol) = fixes(irow,icol);
 sam("total0",icol,"target")    = sum(irow, Sam(irow,icol,"target"));
 sam(irow,"total0","target")    = sum(icol, Sam(irow,icol,"target"));

 colSum0(icol)                  = Sam("total0",icol,"target");
 rowSum0(irow)                  = Sam(irow,"total0","target");

 samBalChk(acBal,"target")      = sam('total0',acBal,"target") - sam(acBal,'total0',"target");
 samBalChk(acBal,"target") $ (abs( samBalChk(acBal,"target"))<1E-6)    = 0;

 samBalChkP(acBal,"target") $ Sam(acBal,'total0',"target")   =
                100*(Sam('total0',acBal,"target") - sam(acBal,'total0',"target"))/sam(acBal,'total0',"target");

* --- Define columns and rows with nonzero sums. Nonzero is defined as an significant
*     absolute value so that column coefficients can be computed safely.

 icolNZ(icol) $ (abs(Sam("total0",icol,"target") gt 0.01)) = yes;

* --- Find zero, non-zero and negative ccell entries

 iZero(irow,icol)   $ (sam(irow,icol,"target") eq 0)                   = yes;
 nonZero(irow,icol) $ (not iZero(irow,icol))                           = yes;
 iNeg(irow,icol)    $ ((sam(irow,icol,"target") lt 0) and nonzero(irow,icol)) = yes;

* --- Set all valid column constraints

 icol2(icol)   = yes;
 icol2(icol) $ ((countcol(icol) le 2) and (countRow(iCol) le 2)) = no;

* --- Set row constraints
*
 irow2(irow) $ (not acBal(iRow)) = yes;
 irow2(irow) $ ((countcol(irow) le 2) and (countRow(iRow) le 2)) = no;

 p_wgt(irow,icol) = 1;

 option kill=iCoeff;


* --- Standard assumptions:
*
* (1) all activity related cost (intermediate, factors, taxes) are targeted
*      as cost share coefficients (not in absolute terms)
*
 iCoeff(irow,icol) $ ((not izero(irow,icol)) $ aIni(iCol))  = yes;
*
* (2) all final demands (government, households, investments)
*     are targeted as expenditure share coefficient (not in absolute terms)
*
 iCoeff(irow,icol) $ ((not izero(irow,icol)) $ hIni(iCol) $ cIni(irow))  = yes;
 iCoeff(irow,icol) $ ((not izero(irow,icol)) $ iIni(iCol) $ cIni(irow))  = yes;
 iCoeff(irow,icol) $ ((not izero(irow,icol)) $ gIni(iCol) $ cIni(irow))  = yes;

* --- compute column coefficients for non-empty columns

 coeffTarget(iCoeff(irow,icol))  = sam(irow,icol,"target")/Sam("total0",icol,"target");
 p_wgt(iCoeff(irow,icol)) = %wgtCoeff%;

* --- Set targets for cells not targeted as shares on row totals to absolute
 iValue(irow,icol) $ ((not izero(irow,icol)) $ (not iCoeff(irow,iCol)))  = yes;

* --- Set additive errors for all targets
 aCell(irow,icol)  $ (iValue(irow,icol) or iCoeff(irow,icol)) = yes;

* --- Define column and row target values ----------------------------------

$ontext

 Set colTarget to average of initial column and row sums. Column sum
 can also be zero or negative. Using an average of protosam column and row sums
 is arbitrary.

$offtext

$iftheni.sumTargets "%sumTargets%"=="on"

 colTarget(icol)  = colSum0(icol);
 colTarget(acbal) = 0.50*(colSum0(acbal) + rowSum0(acbal));

 rowTarget(irow)  = rowSum0(irow);
 rowTarget(acbal) = 0.50*(colSum0(acbal) + rowSum0(acbal));

$else.sumTargets

  option kill=colTarget;
  option kill=rowTarget;

$endif.sumTargets

* --- Force consistency for constraint and error specifications -----------
*
*   Cells which are not specified as values will be specified as coefficients.
*   Ensure that negative cells are specified as values and zero cells are omitted.
*

 iValue(irow,icol) $ ((not icolnz(icol)) $ (not iCoeff(irow,iCol)))    = yes;
 iValue(irow,icol) $ (ineg(irow,icol)    $ (not iCoeff(irow,iCol)))    = yes;
 iValue(irow,icol) $ izero(irow,icol)       = no;

 iCoeff(irow,icol) $ (not ivalue(irow,icol) and (not izero(irow,icol))) = yes;

 estimate(irow,icol) $ (ivalue(irow,icol) or icoeff(irow,icol)) = yes;

 iFixV(irow,icol) $ (not estimate(irow,icol))   = yes;

* --- Make sure that aCell is on for negative cells
 aCell(irow,icol) $ (ineg(irow,icol) and estimate(irow,icol))    = yes;

* --- All cells that are not aCell will be lCell, with multiplicative errors
 lCell(irow,icol) $ ((NOT aCell(irow,icol)) and estimate(irow,icol)) = yes;

* --- Check for errors in setting of constraints ------------------------
*
*     Cells cannot be constrained in both values and coefficients.
*     All cells to be estimated must have lcell or acell.

 iCheck(irow,icol) $ (ivalue(irow,icol) and icoeff(irow,icol)) = yes;
 abort $ card(iCheck)
   "Error check. Cells with both ivalue and icoeff, file: %system.fn%, line: %system.incline%", iCheck;

 iCheck(irow,icol) $ (estimate(irow,icol) and ((not acell(irow,icol)) and
                        not lcell(irow,icol))) = yes;
 abort $ card(iCheck)
   "Error check. Cells to be estimated, but with neither an acell nor lcell target, file: %system.fn%, line: %system.incline%", iCheck;

 iCheck(irow,icol) $ (estimate(irow,icol) and (acell(irow,icol) and
                        lcell(irow,icol))) = yes;
 abort $ card(iCheck)
   "Error check. Cells to be estimated, but with both a acell and lcell target, file: %system.fn%, line: %system.incline%", iCheck;

 iCheck(irow,icol) $ (estimate(irow,icol) and izero(irow,icol)) = yes;
 abort $ card(iCheck)
   "Error check. Cells to be estimated but initially zero, file: %system.fn%, line: %system.incline%", iCheck;

 iCheck2(acini) = ((not acIniNt(acini)) and (irow(acini) or icol(acini)));
 abort $ card(iCheck2)
   "Error check. Accounts in irow or icol but not in acIniNt,file: %system.fn%, line: %system.incline%", icheck2;

*------------------------------------------------------------------------
*
*   Variable declaration
*
*------------------------------------------------------------------------

 $$batinclude "inc/title.inc" '" %regGtapName%: HPD based balancing"'

VARIABLES

 v_tSam(acIni,acInip)        "Estimation sam values"
 v_coeff(acIni,acInip)       "Estimated sam coefficients (shares on row totals)"
 v_colSum(acIni)             "Column sums"
 v_rowSum(acIni)             "Row sums "
 v_errCRSum(acIni,RWCL)      "Error value on row and column sums"
 v_errCell(acIni,acInip)     "Error value for cell constraint"
 v_hpd                       "Highest postierior density penalty term"
 v_bot
 v_bob
;

*------------------------------------------------------------------------
*
* Initialize variables
*
*------------------------------------------------------------------------

 v_tSam.l(irow,icol)                 = Sam(irow,icol,"target");
 v_coeff.l(irow,icol) $ iCoeff(irow,iCol) = coeffTarget(irow,icol);
 v_colSum.l(icol)                    = colSum0(icol);
 v_rowSum.l(irow)                    = rowSum0(irow);
 v_errCRSum.l(icol,rwcl)             = 0;
 v_errCell.l(irow,icol)              = 0;

*------------------------------------------------------------------------
*
* Set bounds, including fixing of variables
*
*------------------------------------------------------------------------

* --- SAM cells are fixed based on user choice

 v_tSam.FX(irow,icol) $ iFixV(irow,icol)    = sam(irow,icol,"target");
 v_tSam.FX(irow,icol) $ fixes(irow,icol)    = fixes(irow,icol);
*
* --- zero entries stay zero
*
 v_tSam.FX(acIni,acInip)    $ (NOT sam(acIni,acInip,"target")) = 0;
 v_coeff.FX(irow,icol) $ izero(irow,icol)  = 0;
*
* --- sign changes are not allowed
*
 v_tSam.lO(acIni,acInip) $ (sam(acIni,acInip,"target") > 0) = 0;
 v_tSam.UP(acIni,acInip) $ (sam(acIni,acInip,"target") < 0) = 0;

 v_coeff.lO(acIni,acInip) $ (coeffTarget(acIni,acInip) > 0) = 0;
 v_coeff.UP(acIni,acInip) $ (coeffTarget(acIni,acInip) < 0) = 0;

* -- unload everything before solving the model

 $$iftheni.GTAP_SAM not "%sysEnv.GTAP_SAM%"=="on"
    Execute_Unload $ (outgdx eq 1 or 2) 'reg/%regGtapName%/set_para_ent.gdx';
 $$endif.GTAP_SAM

* ---------------------------------------------------------------------------
*
*    sam estimation framework
*
* ---------------------------------------------------------------------------

EQUATIONS

 e_samFlow1(acIni,acIni)        "sam flows with additive errors"
 e_samFlow2(acIni,acIni)        "sam flows with logarithmic errors"
 e_samCoeffA(acIni,acIni)       "sam coefficients with additive errors"
 e_samCoeffL(acIni,acIni)       "sam coefficients with logarithmic errors"
 e_colSum(acIni)                "column sums with additive errors"
 e_rowSum(acIni)                "row sums with additive errors"
 e_colSumDef(acIni)             "define column sums"
 e_rowSumDef(acIni)             "define row sums"
 e_coeffDef(acIni,acIni)        "define coefficients"
 e_sam(acIni)                   "row equal column sum constraint"
 e_macroSam(acm,acm)            "macro-sam definition"
 e_macro(macro)                 "macro aggregation constraints"
*e_export(c)                    "Exports cannot exceed production"
 e_hpd                          "Higher Posterior Density Estimator - basic setup"
 e_bot
 e_bop
;


*
*  --- define SAM cells and coefficients from target and endogenous errors
*      using either additive or multiplicate errors (but not both, see checks above)
*
 e_samFlow1(irow,icol) $ (iValue(irow,icol) and aCell(irow,icol)) ..
  v_tSam(irow,icol)    =E= sam(irow,icol,"target")  + v_errCell(irow,icol);

 e_samFlow2(irow,icol) $ (iValue(irow,icol) and lCell(irow,icol)) ..
  v_tSam(irow,icol)    =E= sam(irow,icol,"target")*EXP(v_errCell(irow,icol));

 e_samCoeffA(irow,icol) $ (iCoeff(irow,icol) and aCell(irow,icol)) ..
   v_coeff(irow,icol)  =E= (coeffTarget(irow,icol) + v_errCell(irow,icol));

 e_samCoeffL(irow,icol) $ (iCoeff(irow,icol) and lCell(irow,icol)) ..
   v_coeff(irow,icol)  =E= coeffTarget(irow,icol)*EXP(v_errCell(irow,icol));

* --- Column and row sum constraints (if activitated by user)

 e_colSum(icol2) $ colTarget(iCol2) ..
  v_colSum(icol2)      =E= colTarget(icol2) + v_errCRSum(icol2,"col1");

 e_rowSum(irow2) $ rowTarget(iRow2) ..
  v_rowSum(irow2)      =E= rowtarget(irow2) + v_errCRSum(irow2,"row1");

* --- Define column and row sums and coefficients

 e_colSumDef(icol) ..
   v_colSum(icol)      =E= sum(irow, v_tSam(irow,icol));

 e_rowSumDef(irow) ..
   v_rowSum(irow)      =E= sum(icol, v_tSam(irow,icol));

* --- Row and column sum equality constraint (= SAM balancing constraints)

 e_sam(acbal) ..
   v_colSum(acbal)     =E= v_rowSum(acbal);

* --- define SAM estimate from estiamted cost or expenditure share (v_coeff)
*     and column sum

 e_coeffDef(irow,icol ) $ ((not izero(irow,icol)) and icolnz(icol) and iCoeff(irow,icol))..
   v_coeff(irow,icol)*v_colSum(icol)  =E= v_tSam(irow,icol);

* --- Macro-sam constraints. Not that macSAM is a parameter and not a variable.
*     Ab unbalanced macro sam will imply that the problem will end infeasible

 $$iftheni.useMacSAM "%MacroControl%"=="Macro SAM"

 e_macroSam(acmnt,acmntp) $ macmNz(acmnt,acmntp) ..

   sum((acm_acm(acmnt,acm),acmp) $ acm_acm(acmntp,acmp),macSam(acm,acmp,"used"))

     =E= sum((acIniNt,acIniNtp) $ (macmapIni(acmnt,acIniNt) and macmapIni(acmntp,acIniNtp)),
          v_tSam(acIniNt,acIniNtp));

 $$else.useMacSam

* -- Macro constraints (different from v_macSam)
*    (this can be mutually incompatible as well and cause infeasibilities)

 e_macro(macro2) $ macTotal(macro2,"scaled") ..
   sum((irow,icol), macAgg(irow,icol,macro2)*v_tSam(irow,icol)) =E= macTotal(macro2,"scaled");

$$endif.useMacSam

 e_bot ..

     v_bot =E=  sum((cini,wini) $ (not izero(cini,wini)), v_tSam(cini,wini))
              - sum((wini,cini) $ (not izero(wini,cini)), v_tSam(wini,cini));
 e_bop ..

     v_bot =E= -sum((irow,wini) $ ((not izero(irow,wini)) and (not cini(irow))), v_tSam(irow,wini))
               +sum((wini,irow) $ ((not izero(wini,irow)) and (not cini(irow))), v_tSam(wini,irow));

e_hpd ..
*
*            --- the terms in brackets are introduced to scale the penalty function)
*
  v_hpd *    [    sum((iCol2,rwcl) $ (colTarget(iCol2) $ sameas(rwCl,"col1")),1)
                + sum((iRow2,rwcl) $ (rowTarget(iRow2) $ sameas(rwcl,"row1")),1)
                + sum((iRow,iCol) $ ( (sam(iRow,iCol,"target") $ iValue(irow,icol)) or (coeffTarget(iRow,iCol) $ iCoeff(irow,icol))), p_Wgt(iRow,iCol))
             ]  /100

        =E=
*
*             --- the first two terms target relative deviations from colum and row targets
*
              sum((iCol2,rwcl) $ (colTarget(iCol2) $ sameas(rwCl,"col1")),
                        sqr(v_errCRSum(iCol2,rwcl)/colTarget(iCol2)))

             + sum((iRow2,rwcl) $ (rowTarget(iRow2) $ sameas(rwcl,"row1")),
                        sqr(v_errCRSum(iRow2,rwcl)/rowTarget(iRow2)))
*
*             --- the third terms target errors for indidivual SAM cells or coefficients
*
             +   sum((iRow,iCol) $ ( (sam(iRow,iCol,"target") $ aCell(irow,icol) $ iValue(irow,icol)) or (coeffTarget(iRow,iCol) $ iCoeff(irow,icol))),
                              sqr(v_errCell(iRow,iCol)/(   sam(iRow,iCol,"target")   $ iValue(irow,icol)
                                                         + coeffTarget(iRow,iCol)    $ iCoeff(irow,icol))) * p_wgt(iRow,iCol))

             +   sum((iRow,iCol) $ ( (sam(iRow,iCol,"target") $ lCell(irow,icol) $ iValue(irow,icol)) or (coeffTarget(iRow,iCol) $ iCoeff(irow,icol))),
                              sqr(v_errCell(iRow,iCol)) * p_wgt(iRow,iCol));

 MODEL samHPD
 /
 e_samFlow1
 e_samFlow2
 e_samCoeffA
 e_samCoeffL
 e_colSum
 e_rowSum
 e_colSumDef
 e_rowSumDef
 e_coeffDef
 e_sam
 $$iftheni.useMacSAM "%MacroControl%"=="Macro SAM"
 e_macroSam
 $$else.useMacSam
 e_macro
$$endif.useMacSam
 e_hpd
 e_bot
 e_bop
 /;

* --- Solve statenment
  v_bot.l =  sum((cini,wini) $ (not izero(cini,wini)), v_tSam.l(cini,wini))
            -sum((wini,cini) $ (not izero(wini,cini)), v_tSam.l(wini,cini));

 OPTION NLP        = conopt4;
 samHPD.HOLDFIXED  = 1;
 samHPD.optfile    = 2;
 SOLVE samHPD using nlp minimizing v_hpd;

* -----------------------------------------------------------------------------
*
*    Reporting part
*
* -----------------------------------------------------------------------------

 $$batinclude "inc/title.inc" '" %regGtapName%: Post balancing reporting"'

* --------- Sets for reporting results

  SET pctchg  "Boundaries for percentage changes" /
     p1    "1   percent"
     p2    "2   percent"
     p3    "3   percent"
     p5    "5   percent"
     p10   "10  percent"
     p15   "15  percent"
     p25   "25  percent"
     p50   "50  percent"
     p100  "100 percent"
     /;
  SET levchg  "Boundaries for level changes" /
     l1       "1   percent of samCeMean plus 2 x samCeStdDev"
     l2       "2   percent of samCeMean plus 2 x samCeStdDev"
     l3       "3   percent of samCeMean plus 2 x samCeStdDev"
     l5       "5   percent of samCeMean plus 2 x samCeStdDev"
     l10      "10  percent of samCeMean plus 2 x samCeStdDev"
     l15      "15  percent of samCeMean plus 2 x samCeStdDev"
     l25      "25  percent of samCeMean plus 2 x samCeStdDev"
     l50      "50  percent of samCeMean plus 2 x samCeStdDev"
     l100     "100 percent of samCeMean plus 2 x samCeStdDev"
     /;

* ---------------- Parameters for reporting results

Parameters
 SEM                             "Standard deviation of coefficient changes"
 valDiff(ac,acp)                 "Differnece btw original sam and final sam in values"
 PERDIFF(ac,acp)                 "Differnce btw original sam and Final sam in Percent"
 bigDiffv(ac,acp)                "Cells with large value changes"
 bigDiffp(ac,acp)                "Cells with large percent change"
 bigDiffvp(ac,acp)               "Cells with large value and percent change"
 NormEntrop                      "Normalized Entropy a measure of total uncertainty"

* --- samCe descriptive statistics

 samCeStat                       "Varous statistics on final SAM"

* --- Big difference parameters

 nBiggdiff(ac,acp,pctchg,levchg) "Cells with large percent and value changes"
 biggdiffCnt(pctchg,levchg)      "Count of conforming cells in each configuration"
 chgIntPct(pctchg)               "Interval boundary values for pct changes"
 chgIntLev(levchg)               "Interval boundary values for lev changes"

;
*
* --- assign results
*
 sam(irow,icol,"ce")       = v_tSam.l(irow,icol);
 sam("total0",icol,"ce")   = sum(irow, sam(irow,icol,"ce"));
 sam(irow,"total0","ce")   = sum(icol, sam(irow,icol,"ce"));

 samBalChk(icol,"ce")        = sam('total0',icol,"ce") - sam(icol,'total0',"ce");
 samBalChk(icol,"ce") $ (abs( samBalChk(icol,"ce"))<1E-6)   = 0;
 if (sum(icol $ samBalChk(icol,"ce"),1), abort "Not balanced ,file: %system.fn%, line: %system.incline%",samBalChk);

* --- Compute basic descriptive stats

* They are interesting in themselves but also useful for determining big diffs
* below. Similar descriptive stats are calculated for the sam prior to
* estimation above, a comparison could be useful.

$batInclude "inc/samest/samStat.inc" target acIniP
$batInclude "inc/samest/samStat.inc" ce     acIniP

 SEM        = SQRT(Sum((irow,icol),
                 SQR(v_coeff.l(irow,icol) - coeffTarget(irow,icol)))/samStat("nonZcnt","ce"));
*
* --- differences in levels and in percentage terms
*
 valDiff(ac,acp)                 = sam(ac,acp,"ce") - sam(ac,acp,"target");
 perdiff(ac,acp) $ sam(ac,acp,"target") = 100*(sam(ac,acp,"ce")/sam(ac,acp,"target") - 1);

 bigDiffv(ac,acp) $  (abs(valDiff(ac,acp)) gt 10/samscal) = valDiff(ac,acp);
 bigDiffp(ac,acp) $  (abs(perdiff(ac,acp)) gt 10)         = perdiff(ac,acp);

 bigDiffvp(ac,acp) $ (bigDiffv(ac,acp) and bigDiffp(ac,acp)) =  bigDiffp(ac,acp);

* --- sam big level and percentage differences

* --- Assign boundary values
 chgIntPct('p1')    = 1;
 chgIntPct('p2')    = 2;
 chgIntPct('p3')    = 3;
 chgIntPct('p5')    = 5;
 chgIntPct('p10')   = 10;
 chgIntPct('p15')   = 15;
 chgIntPct('p25')   = 25;
 chgIntPct('p50')   = 50;
 chgIntPct('p100')  = 100;

* -- For levels, first assign plain values then adjust them so that they are
*    proportional to samCeMean + 2 x samCeStdDev

 chgIntLev('l1')    = 1;
 chgIntLev('l2')    = 2;
 chgIntLev('l3')    = 3;
 chgIntLev('l5')    = 5;
 chgIntLev('l10')   = 10;
 chgIntLev('l15')   = 15;
 chgIntLev('l25')   = 25;
 chgIntLev('l50')   = 50;
 chgIntLev('l100')  = 100;

 chgIntLev(levchg) = (chgIntLev(levchg)/100) * (samStat("Mean","ce")  + 2 * samStat("StdDev","ce"));

* --- Calculate matrix of level and percentage change counts

 nBiggDiff(ac,acp,pctchg,levchg) $ (     (abs(perdiff(ac,acp)) gt chgIntPct(pctchg))
                                     and (abs(valDiff(ac,acp)) gt chgIntLev(levchg)) )= 1;

* --- Count conforming values in each configuration

 biggDiffCnt(pctchg,levchg) =  sum((ac,acp) $ nbiggdiff(ac,acp,pctchg,levchg),1);

*
* --- store balanced proto SAM before splits into XLSX file in SAV directory
*
  parameter samout;
  samout(irow,icol) = sam(irow,icol,"ce");
  execute_unload "./sav/sam0_%regGtapName%.gdx", samout;
  execute 'gdxxrw ./sav/sam0_%regGtapName%.gdx output=reg/%regGtapName%/%BridgeFileToWrite%.xlsx par=samout rng=CE-SAM!A1'

* -----------------------------------------------------------------------------
*
*    Apply Split (first user supplied, then from GTAP)
*
* -----------------------------------------------------------------------------

*
* -- find one to one mappings
*
 set oneToOne(ac);
 oneToOne(ac) $ (sum(map_acIni_ac(acIni,acp) $ map_acIni_ac(acIni,ac),1) eq sum(map_acIni_ac(acIni,ac),1)) = YES;
 alias(oneToOne,oneToOne1);
*
* -- define the dis-aggregated cells subject to split
*
 set split(ac,acp);
 split(ac,acp) $ sum((acIni,acInip) $ (map_acIni_ac(acIni,ac) $ map_acIni_ac(acIniP,acP)),sam(acIni,acInip,"ce")) = YES;

 set notOneToOne(ac); notOneToOne(ac) = yes $ (not oneToOne(ac));

 scalar needSplit; needSplit = (card(ac) ne card(oneToOne));

 if ( needSplit,

    $$batinclude "inc/title.inc" '" %regGtapName%: Apply split shares from EXCEL file"'

*   --- missing user provided split shares are set to unity, 1:1 relation to SAM updated from Macro SAM

     sumSplit(acIni,acIniP) = sum((map_acIni_ac(acIni,ac),acp) $ (map_acIni_ac(acInip,acp) $ split(ac,acp)) ,splitShr(acIni,ac,acInip,acp));

     splitShr(acIni,ac,acIniP,acp) $ (  (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp))
                                      $ (not  sumSplit(acIni,acIniP)) $ split(ac,acp)) = 1;

     sumSplit(acIni,acIniP) $ ((not sumSplit(acIni,acIniP)))
        = sum((map_acIni_ac(acIni,ac),acp) $ map_acIni_ac(acInip,acp),splitShr(acIni,ac,acInip,acp));
     splitShr(acIni,ac,acIniP,acp) $ splitShr(acIni,ac,acIniP,acp) = splitShr(acIni,ac,acIniP,acp)/sumSplit(acIni,acIniP);

*   --- calculate protosam with disaggregations

     sam(ac,acp,"dis") = sum( (map_acIni_ac(acIni,ac),acInip) $ map_acIni_ac(acInip,acp),sam(acIni,acInip,"ce")*splitShr(acIni,ac,acInip,acp));
     sam(ac,acp,"dis") $ (sam(ac,acp,"dis") eq eps) = 0;
     balsam(sam,ac,Total0,"dis")

  else
*
*    --- no split: one to one mapping
*
     sam(ac,acp,"dis") = sum( (map_acIni_ac(acIni,ac),acInip) $ map_acIni_ac(acInip,acp),sam(acIni,acInip,"ce"));
  );

*
* --- read maps which link acitivities, products and factors to split to GTAP
*
  set asplit_gtap(a),csplit_gtap(c),fsplit_gtap(f);
  $$GDXIN "reg/%regGtapName%/%samXLS%.gdx"
    $$LOAD asplit_gtap,csplit_gtap,fsplit_gtap
  $$GDXIN
  $$onempty

  $$setglobal useGtapp off
  $$ife card(asplit_gtap)>0 $setglobal useGtap on
  $$ife card(csplit_gtap)>0 $setglobal useGtap on
  $$ife card(fsplit_gtap)>0 $setglobal useGtap on

  $$iftheni.useGtap "%useGtap%"=="on"

    $$GDXIN "reg/%regGtapName%/%samXLS%.gdx"
      $$LOADdc  ft<map_f_ft.dim2 map_f_ft
    $$GDXIN



   $$batinclude "inc/title.inc" '" %regGtapName%: Define split shares from GTAP Data base"'

    parameter sam_tax(ac,ac);
    sam_tax(ac,acp) = sam(ac,acp,"dis");

*   --- Split shares from GTAP

    Set
       aGC               "GTAP activities plus investment account cgds"
       aG(agc)           "GTAP activities"
       cG                "GTAP commodities"
       fG                "GTAP factors"
       rG                "GTAP regions"
       map_aG_ac(aG,ac)  "Mapping from GTAP acitivities to activities in SAM"
       map_cG_ac(cG,ac)  "Mapping from GTAP commodities to commoditie in SAM"
       map_fG_ac(fG,ac)  "Mapping from GTAP factors to factors in SAM"
       map_a_c(ac,ac)    "Mapping from acitivities to commodities, necessary for GTAP splits only"
    ;
*
*   --- Parameters used in split problem
*
    parameter
         gtapUse(ac,ac)          "Data for SAM accounts aggregated from GTAP data"
         gtapShare(ac,acIni,acp,acInip)
         splitShr2(acIni,ac,acIniP,acp)
     ;
     alias (asplit_gtap,aS,aSp),(csplit_gtap,cS,cSp),(fsplit_gtap,fS,fsp);
     SET acs(ac) "splitted accounts based on gtap";
*
*    --- read GTAP data
*
     set agLoad,fgLoad;
     $$gdxin "dat/%gtapDataFile%.gdx"
        $$load agLoad=ag cG fGLoad=fg
     $$gdxin
*
     set agc / set.agLoad,cgds /;
     set ag  / set.agLoad /;
     $$iftheni.GTAPSams "%gtapDataFile%"=="gtapSamsV11"
        set tg;
        $$gdxin "dat/%gtapDataFile%.gdx"
           $$load tg
        $$gdxin
        set fg / set.fgLoad,set.tg /;
     $$else.GTAPSams
        set fg / set.fgLoad /;
     $$endif.GTAPSams


*
     $$GDXIN "reg/%regGtapName%/%samXLS%.gdx"
        $$loaddc map_aG_ac map_cG_ac map_fG_ac map_a_c
     $$gdxin
*
*    --- read mapping from proto-SAM to GTAP
*
     set tests(*);
     tests(aG) $ (not sum(map_ag_ac(ag,ac),1)) = YES;
     tests("cgds") = no;
     if (card(tests), abort "Some GTAP activities are not mapped to any SAM activities, file: %system.fn%, line: %system.incline%",tests);
     tests(a) $ (not sum(map_ag_ac(ag,a),1)) = YES;
     if (card(tests), abort "Some SAM activities are not mapped to any GTAP activities, file: %system.fn%, line: %system.incline%",tests);

     tests(cG) $ (not sum(map_cg_ac(cg,c),1)) = YES;
     if (card(tests), abort "Some GTAP commodities are not mapped to any SAM activities, file: %system.fn%, line: %system.incline%",tests);
     tests(c) $ (not sum(map_cg_ac(cg,c),1)) = YES;
     if (card(tests), abort "Some SAM activities are not mapped to any GTAP activities, file: %system.fn%, line: %system.incline%",tests);

     tests(fG) $ (not sum(map_fg_ac(fg,ac),1)) = YES;
     if (card(tests), abort "Some GTAP factors are not mapped to any SAM factors, file: %system.fn%, line: %system.incline%",tests);

     tests(f)  $ (not sum(map_fg_ac(fg,f),1)) = YES;
     if (card(tests), abort "Some SAM factors are not mapped to any GTAP factors, file: %system.fn%, line: %system.incline%",tests);

     $$iftheni.GTAPSams "%gtapDataFile%"=="gtapSamsV11"

        set is,r;
        alias(is,js);
        parameter GtapSams(r,*,*);
        $$gdxin "dat/%gtapDataFile%.gdx"
            $$load is
            $$load r
            $$load gtapSams
        $$gdxin

     $$else.GTAPSams
*
*        --- GTAP Data parameters
*
         parameter
              vfm(fg,aG,rG)       "Endowments - firm purchases at market prices"
              vdfm(cG,aGc,rG)     "Intermediates - Firm s domestic purchases at market prices"
              vdpm(cG,rG)         "Private households - Domestic purchase at market prices"
              vdgm(cG,rG)         "Government - Domestic purchase at market s price"
              vifm(cG,aGc,rG)      "Intermediates - Firm s imports at market prices"
              vipm(cG,rG)         "Private households - Imports at market prices"
              vigm(cG,rG)         "Government - Imports at market price"
              vxmd(cG,rG,rG)      "Trade - Bilateral exports at market prices"

              vdfa(cG,aGc,rG)      "Intermediates - Firm s domestic purchases at agents prices"
              vdpa(cG,rG)         "Private households - Domestic purchase at agents prices"
              vdga(cG,rG)         "Government - Domestic purchase at agent s price"
              vifa(cG,aGc,rG)      "Intermediates - Firm imports at agents prices"
              vipa(cG,rG)         "Private households - Imports at agents prices"
              viga(cG,rG)         "Government - Imports at agent s price"

              ftrv(fG,aG,rG)      "Factor tax revenue"
              fbep(fG,aG,rG)      "Factor subidies"
              isep(cG,aG,rG)      "Input Subsidies"
              osep(cG,rG)         "Output Subsidies"
              tarifrev(cG,rG,rG)  "Tariffs"
              xtrev(cG,rG,rG)     "Export Tax"
              tvom(cG,aG,rG)      "Make matrix"
        ;
        set rg;
        $$gdxin "dat/%gtapDataFile%.gdx"
           $$load    rG
           $$loaddc  vfm  vdpm vdgm vipm vigm vxmd ftrv fbep
           $$loaddc  vdfm vifm vdfa vifa tvom
           $$loaddc  vdpa vdga  vipa viga
        $$gdxin
     $$endif.GTAPSams

     $$SETGLOBAL regCur %regGtapName%
     alias(a,ap);alias(c,cp);alias(f,fp);alias(ft,ftp);

     if ( needSplit,

        $$iftheni.GTAPSams not "%gtapDataFile%"=="gtapSamsV11"
*
*           --- traditional GTAP Data base
*
            vfm(fg,ag,rg) $ (not sameas(rg,"%regCur%")) = 0;
*           --- factor use at market prices (net of factor taxes and subsidies)
            gtapUse(f,a)  = sum((map_fG_ac(fG,f),map_aG_ac(aG,a)),vfm(fG,aG,"%regCur%"));

*           --- factor taxes minus subsidies
            gtapUse(ft,a)  = sum((map_fG_ac(fG,f),map_aG_ac(aG,a),map_f_ft(f,ft)),ftrv(fG,aG,"%regCur%")-fbep(fg,ag,"%regCur%"));

*           --- intermediate input use
            gtapUse(c,a) = sum((map_cG_ac(cG,c),map_aG_ac(aG,a)),vdfm(cG,aG,"%regCur%") + vifm(cG,aG,"%regCur%"));

*           --- use by aggregate household and government
            gtapUse(c,h) = sum(map_cG_ac(cG,c),vdpm(cG,"%regCur%") + vipm(cG,"%regCur%"));
            gtapUse(c,g) = sum(map_cG_ac(cG,c),vdgm(cG,"%regCur%") + vigm(cG,"%regCur%"));

*           --- factor earnings by households (aggregate factor use at market prices)
            gtapUse(h,f) = sum(a, gtapUse(f,a));

*           --- factor tax - subsidies earnings by government (aggregate factors taxes and subsidies over activities)
            gtapUse(g,ft) = sum(a, gtapUse(ft,a));

*           --- exports
            gtapUse(c,w) = sum((map_cG_ac(cG,c),rG),vxmd(cG,"%regCur%",rG));

*           --- imports
            gtapUse(w,c) = sum((map_cG_ac(cG,c),rG),vxmd(cG,rG,"%regCur%"));
*
*           --- make matrix
            gtapUse(a,c) $ map_a_c(a,c)  = sum((map_aG_ac(aG,a),sameas(cg,ag)),tvom(cG,aG,"%regCur%"));
*
*           --- remove tiny entries
*
            gtapUse(ac,acp) $ (abs(gtapUse(ac,acp)) < 1E-15)   = 0;

        $$else.GTAPSams

            gtapUse(f,a)   = sum((map_fG_ac(fG,f),map_aG_ac(aG,a)),gtapSams("%regCur%",fG,aG));
            gtapUse(c,a)   = sum((map_cg_ac(cg,c),map_aG_ac(aG,a)),gtapSams("%regCur%",cG,aG));
            gtapUse(a,c)   = sum((map_cg_ac(cg,c),map_aG_ac(aG,a)),gtapSams("%regCur%",ag,cG));
            gtapUse(c,h)   = sum(map_cG_ac(cG,c),gtapSams("%regCur%",cg,"h-hhold"));
            gtapUse(c,g)   = sum(map_cG_ac(cG,c),gtapSams("%regCur%",cg,"g-govt"));
            gtapUse(c,i)   = sum(map_cG_ac(cG,c),gtapSams("%regCur%",cg,"i-cgds"));
            gtapUse(c,w)   = sum(map_cG_ac(cG,c),gtapSams("%regCur%",cg,"w-ww_world"));
            gtapUse(w,c)   = sum(map_cG_ac(cG,c),gtapSams("%regCur%","w-ww_world",cg));
            gtapUse(h,f)   = sum(a, gtapUse(f,a));
            gtapUse(g,ft)  = sum(a, gtapUse(ft,a));

        $$endif.GTAPSams
*
*    ------------------------------------------------------------------------------
*
*       Define split shares from GTAP
*
*    ------------------------------------------------------------------------------
*
*
*       --- intermediate demand, for non-splitted commodities to splitted activities
*
        gtapShare(c,acIni,aS,aini) $ (map_acIni_ac(aini,as) and map_acIni_ac(acIni,c) and gtapUse(c,aS) )
                                       = gtapUse(c,aS)/sum(map_acIni_ac(aini,asp),gtapUse(c,asp));
*
*       --- intermediate demand, for splitted commodities to splitted and non-splitited activities
*
        gtapShare(cs,cini,a,aini)  $ (map_acIni_ac(aini,a) and map_acIni_ac(cini,cs) and  gtapUse(cs,a))
                                        = gtapUse(cs,a)/sum((map_acIni_ac(aini,ap),csp) $ map_acIni_ac(cini,csp),gtapUse(csp,ap));
*
*       --- make matrix
*
        gtapShare(ac,acIni,c,cini) $ (map_acIni_ac(cini,c) and map_acIni_ac(acIni,ac) and sum((map_acIni_ac(cini,cp),acp) $ map_acIni_ac(acIni,acp),gtapUse(acp,cp)))
                                      = gtapUse(ac,c)/sum((map_acIni_ac(cini,cp),acp) $ map_acIni_ac(acIni,acp),gtapUse(acp,cp));

*       --- splitted activities producing the splitted commodity

        gtapShare(as,aini,c,cini) $ ((not cs(c)) and map_acIni_ac(aini,as) and map_acIni_ac(cini,c) and sum((cs,asp)$map_acIni_ac(aini,asp),gtapUse(asp,cs)) and sam(aini,cini,"ini"))
                                      = sum(cs,gtapUse(aS,cs))/sum((cs,map_acIni_ac(aini,asp)),gtapUse(asp,cs));

*       --- non-splitted activities producing the splitted commodity

        gtapShare(a,aini,cs,cini) $ ((not as(a)) and  map_acIni_ac(aini,a) and map_acIni_ac(cini,cs) and sam(aini,cini,"ini"))
                                               = sum(as,gtapUse(aS,cs))/sum((map_acIni_ac(cini,csp),as),gtapUse(as,csp));
*
*       --- hosueholds, government use, exports, investments
*
        gtapShare(ac,acIni,h,hini) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(hini,h) and gtapUse(ac,h))
                                               = gtapUse(ac,h)/sum(map_acIni_ac(acIni,acpp),gtapUse(acpp,h));

        gtapShare(ac,acIni,g,gini) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(gini,g) and gtapUse(ac,g))
                                               = gtapUse(ac,g)/sum(map_acIni_ac(acIni,acpp),gtapUse(acpp,g));

        gtapShare(ac,acIni,w,wini) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(wini,w) and gtapUse(ac,w))
                                               = gtapUse(ac,w)/sum(map_acIni_ac(acIni,acpp),gtapUse(acpp,w));

        gtapShare(ac,acIni,i,iini) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(iini,i) and gtapUse(ac,i))
                                               = gtapUse(ac,i)/sum(map_acIni_ac(acIni,acpp),gtapUse(acpp,i));

*
*       --- factor use by activities
*
        gtapShare(f,fini,a,aIni)   $ (map_acIni_ac(aIni,a) and map_acIni_ac(fini,f) and gtapUse(f,a))
                                               = gtapUse(f,a)/sum((map_acIni_ac(aIni,ap),fp) $ map_acIni_ac(fini,fp),gtapUse(fp,ap));
*
*       --- factor taxes and subsidies by activies
*
        gtapShare(ft,gtini,a,aIni) $ (map_acIni_ac(aIni,a) and map_acIni_ac(gtini,ft) and gtapUse(ft,a))
                                               = gtapUse(ft,a)/sum((map_acIni_ac(aIni,ap),ftp) $ map_acIni_ac(gtIni,ftp), gtapUse(ftp,ap));
*
*       --- factor earnings by households
*
        gtapShare(ac,acini,f,fini) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(fini,f) and gtapUse(ac,f))
                                              = gtapUse(ac,f)/sum((map_acIni_ac(acIni,acpp),fp) $ map_acIni_ac(fini,fp),gtapUse(acpp,fp));
*
*       --- factor tax/subsidies revenues by government
*
        gtapShare(ac,acini,ft,gtini) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(gtini,ft) and gtapUse(ac,ft))
                                              = gtapUse(ac,ft)/sum((map_acIni_ac(acIni,acpp),ftp) $ map_acIni_ac(gtini,ftp),gtapUse(acpp,ftp));
*
*       --- imports
*
        gtapShare(w,wini,ac,acIni) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(wini,w) and sum(map_acIni_ac(acIni,acpp),gtapUse(w,acpp)))
                                               = gtapUse(w,ac)/sum(map_acIni_ac(acIni,acpp),gtapUse(w,acpp));
*
*       --- set missing split factors to eps
*
        gtapShare(c,cini,as,aini)   $ ((map_acIni_ac(cini,c)   and map_acIni_ac(aini,as)) $ (not gtapShare(c,cini,as,aini)))   = eps;
        gtapShare(f,fini,aS,aini)   $ ((map_acIni_ac(fini,f)   and map_acIni_ac(aini,as)) $ (not gtapShare(f,fini,aS,aini)))   = eps;
        gtapShare(gt,gtini,aS,aini) $ ((map_acIni_ac(gtini,gt) and map_acIni_ac(aini,as)) $ (not gtapShare(gt,gtini,aS,aini))) = eps;
        gtapShare(ft,gtini,a,aini)  $ ((map_acIni_ac(gtini,ft) and map_acIni_ac(aini,a))  $ (not gtapShare(ft,gtini,a,aini)))  = eps;
        gtapShare(fs,fini,a,aini)   $ ((map_acIni_ac(fini,fs)  and map_acIni_ac(aini,a))  $ (not gtapShare(fs,fini,a,aini)))   = eps;
        gtapShare(w,wini,aS,aini)   $ ((map_acIni_ac(wini,w)   and map_acIni_ac(aini,as)) $ (not gtapShare(w,wini,aS,aini)))   = eps;


        gtapShare(gt,gtini,cs,cini) $ ((map_acIni_ac(gtini,gt) and map_acIni_ac(cini,cs)) $(not gtapShare(gt,gtini,cs,cini))) = eps;
        gtapShare(w,wini,cs,cini)   $ ((map_acIni_ac(wini,w)   and map_acIni_ac(cini,cs)) $(not gtapShare(w,wini,cs,cini)))   = eps;
        gtapShare(as,aini,cs,cini)  $ ((map_acIni_ac(aini,as)  and map_acIni_ac(cini,cs)) $(not gtapShare(as,aini,cs,cini)))  = eps;
        gtapShare(fs,fini,g,gini)   $ ((map_acIni_ac(fini,fs)  and map_acIni_ac(gini,g))  $(not gtapShare(fs,fini,g,gini)))   = eps;
        gtapShare(fs,fini,h,hini)   $ ((map_acIni_ac(fini,fs)  and map_acIni_ac(hini,h))  $(not gtapShare(fs,fini,h,hini)))   = eps;

*       --- if splitted activity does not produce splitted commodity it cannot produce any other commodity

        gtapShare(as,aini,c,cini) $ ((map_acIni_ac(aini,as) and map_acIni_ac(cini,c)) $ (not sum(cs,gtapUse(as,cs))))   = eps;

        gtapShare(cs,cini,h,hini) $ ((map_acIni_ac(cini,cs) and map_acIni_ac(hini,h)) $ (not gtapShare(cs,cini,h,hini))) = eps;
        gtapShare(cs,cini,g,gini) $ ((map_acIni_ac(cini,cs) and map_acIni_ac(gini,g)) $ (not gtapShare(cs,cini,g,gini))) = eps;
        gtapShare(cs,cini,w,wini) $ ((map_acIni_ac(cini,cs) and map_acIni_ac(wini,w)) $ (not gtapShare(cs,cini,w,wini))) = eps;
        gtapShare(cs,cini,i,iini) $ ((map_acIni_ac(cini,cs) and map_acIni_ac(iini,i)) $ (not gtapShare(cs,cini,i,iini))) = eps;

        sumSplit(acIni,acInip) = sum((map_acIni_ac(acIni,ac),acp) $ map_acIni_ac(acInip,acp),gtapShare(ac,acIni,acp,acInip));
        gtapShare(ac,acIni,acp,acInip) $ (sumSplit(acIni,acInip) le eps) = 0;


        gtapShare(ac,acIni,acp,acInip) $ (gtapShare(ac,acIni,acp,acInip) $ (sumSplit(acIni,acInip) gt eps))
           = gtapShare(ac,acIni,acp,acInip)/sumSplit(acIni,acInip);

        option gtapShare:2:0:1;display gtapShare;
*
*       --- WB: Beware there are no split shares for some taxes generated!
*
        acs(ac) $ sum(acp, gtapUse(ac,acp))  =  YES;

*      --- calculate protosam with disaggregations based on GTAP

       splitShr(acIni,ac,acIniP,acp) $ ((map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)) $ (not splitShr(acIni,ac,acIniP,acp)) $ sumSplit(acIni,acIniP)) = 1;

       splitShr2(acIni,ac,acIniP,acp) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acIniP,acp) $ splitShrOri(acIni,ac,acIniP,acp))
         = splitShr(acIni,ac,acIniP,acp)*gtapShare(ac,acIni,acp,acInip);

       splitShr2(acIni,ac,acIniP,acp) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acIniP,acp) $ (not splitShrOri(acIni,ac,acIniP,acp)))
         = gtapShare(ac,acIni,acp,acInip);

       splitShr2(acIni,acS,acIniP,aS) $ ( (map_acIni_ac(acIni,acS) and map_acIni_ac(acIniP,aS)) $ (NOT splitShr2(acIni,acS,acIniP,aS)) $ sumSplit(acIni,acIniP)) = eps;

       splitShr(acIni,ac,acIniP,acp) $ splitShr2(acIni,ac,acIniP,acp) = splitShr2(acIni,ac,acIniP,acp);

        option splitShr:2:0:1;display splitShr;
    );

 $$endif.useGtap

 if ( needSplit,
    $$batinclude "inc/title.inc" '" %regGtapName%: Apply split shares"'

    sam(ac,acp,"dis") = sum((acIni,acInip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),sam(acIni,acInip,"ce")*splitShr(acIni,ac,acInip,acp));
    sam(ac,acp,"dis") $ (sam(ac,acp,"dis") eq eps) = 0;
*
*   --- call program for corrections / additions from regional folder if present
*
    $$ifi exist '%regf%/sam_corr.inc' $include '%regf%/sam_corr.inc'

    sam("total0",acnt,"dis")  = sum(acntp,  sam(acntp,acnt,"dis"));
    sam(acntp,"total0","dis") = sum(acnt,   sam(acntp,acnt,"dis"));
 );
*
* ------------------------------------------------------------------------------
*
*    Balance dis-aggregated SAM with fixed aggregate cells
*
* ------------------------------------------------------------------------------
*
* --- balancing framework
*
 variable v_splitCell(ac,acp);
 equation e_splitCell(acIni,acInip),e_balSplit(ac),e_macroSamSplit(acm,acm),e_hpdSplit;

 e_macroSamSplit(acmnt,acmntp) $ (macmNz(acmnt,acmntp) and (acmDupl(acmnt) or acmDupl(acmntp))) ..

     macSam(acmnt,acmntp,"used")

     =E= sum((ac,acp) $ (macmap3(acmnt,ac) and macmap3(acmntp,acp)),
          v_SplitCell(ac,acp));

  e_splitCell(acIni,acInip) $ sum((ac,acp) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),1) ..

      sam(acIni,acInip,"ce") =E= sum((ac,acp) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)), v_splitCell(ac,acp));

  e_balSplit(ac) $ sum(acp,split(ac,acp)+split(acp,ac)) ..

           sum((acIni,acp,acinip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)), v_splitCell(ac,acp))
       =E= sum((acini,acp,aciniP) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)), v_splitCell(acp,ac));

  e_hpdSplit ..

      v_hpd *  (sum((acIni,ac,acInip,acp) $ ((map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)) $ (v_splitCell.range(ac,acp) ne 0)),1)+1.E-10)

        =E= sum((acIni,ac,acInip,acp) $ ((map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)) $ (v_splitCell.range(ac,acp) ne 0)),
                  sqr( [v_splitCell(ac,acp)-sam(ac,acp,"dis")]/[sam(ac,acp,"dis")+1.E-6] )) + 1.E-10;

 model m_hpdSplit /  e_macroSamSplit,e_splitCell,e_balSplit,e_hpdSplit /;
*model m_hpdSplit /  e_splitCell,e_balSplit,e_hpdSplit /;
*
* --- find cases where no split was assigned, use columns/rows totals for dis-aggregated accounts to
*     define split share in this case
*
  set missing(acIni,acinip);
  parameter totals(acini,acinip);

  if ( needSplit,
*
*    --- user defined fixed targets for dis-aggregation
*
     sam(ac,acp,"dis") $ fixes(ac,acp) = fixes(ac,acp);
     display fixes;

     sam("total0",acnt,"dis")  = sum(acntp,  sam(acntp,acnt,"dis"));
     sam(acntp,"total0","dis") = sum(acnt,   sam(acntp,acnt,"dis"));

     missing(acIni,acinip)
      $ ((not sum((ac,acp) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),sam(ac,acp,"dis"))) $  sam(acIni,acInip,"ce")) = YES;


     totals(acini,acinip) $ missing(acIni,acinip)
       = sum((ac,acp) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)), abs(sam("total0",ac,"dis"))+abs(sam(acp,"total0","dis")));

     sam(ac,acp,"dis") $ sum((acini,acinip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),totals(acIni,acInip))
      = sum((acini,acinip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)), [abs(sam("total0",ac,"dis"))+abs(sam(acp,"total0","dis"))]
                                                                                     * sam(acIni,acInip,"ce")/abs(totals(acini,acinip)));
*    --- if still empty, use totals

     missing(acIni,acInip) $ totals(acIni,acInip) = NO;

     sam(ac,acp,"dis") $ sum((acini,acinip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),missing(acIni,acInip))
      = sum((acini,acinip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)), sam(acIni,acInip,"ce"));

*
*    --- start values are the dis-aggregated cells
*
     v_splitCell.l(ac,acp)  = sam(ac,acp,"dis");
*
*    --- fix empty dis-aggregated cells, don't allow for sign changes
*
     v_splitCell.fx(ac,acp) $ (not sam(ac,acp,"dis"))  = 0;
     v_splitCell.lo(ac,acp) $ (sam(ac,acp,"dis") > 0)  = 0;
     v_splitCell.up(ac,acp) $ (sam(ac,acp,"dis") < 0)  = 0;
*
*    --- reflect general bounds on spplit from interface
*
     v_splitCell.lo(a,c)     $ (sam(a,c,"dis") > 0)                                        = sam(a,c,"dis")*%lowBoundMakeMatrix%;
     v_splitCell.up(a,c)     $ ((sam(a,c,"dis") > 0)   $ (%UppBoundMakeMatrix% ne 100))    = sam(a,c,"dis")*%UppBoundMakeMatrix%;

     v_splitCell.lo(f,a)     $ (sam(f,a,"dis") > 0)                                        = sam(f,a,"dis")*%lowBoundFacDemand%;
     v_splitCell.up(f,a)     $ ((sam(f,a,"dis") > 0)   $ (%UppBoundFacDemand% ne 100))     = sam(f,a,"dis")*%UppBoundFacDemand%;

     v_splitCell.lo(c,a)     $ (sam(c,a,"dis") > 0)                                        = sam(c,a,"dis")*%lowBoundIntDemand%;
     v_splitCell.up(c,a)     $ ((sam(c,a,"dis") > 0)   $ (%UppBoundIntDemand% ne 100))     = sam(c,a,"dis")*%UppBoundIntDemand%;

     v_splitCell.lo(gt,a)    $ (sam(gt,a,"dis") > 0)                                       = sam(gt,a,"dis")*%lowBoundIndFirmTaxes%;
     v_splitCell.up(gt,a)    $ ((sam(gt,a,"dis") > 0)  $ (%UppBoundIndFirmTaxes% ne 100))  = sam(gt,a,"dis")*%UppBoundIndFirmTaxes%;

     v_splitCell.up(gt,a)    $ (sam(gt,a,"dis")  < 0)                                      = sam(gt,A,"dis")*%lowBoundIndFirmTaxes%;
     v_splitCell.lo(gt,a)    $ ((sam(gt,a,"dis") < 0)  $ (%UppBoundIndFirmTaxes% ne 100))  = sam(gt,A,"dis")*%UppBoundIndFirmTaxes%;

     v_splitCell.lo(c,fin)   $ (sam(c,fin,"dis") > 0)                                      = sam(c,fin,"dis")*%lowBoundFinDemand%;
     v_splitCell.up(c,fin)   $ ((sam(c,fin,"dis") > 0)   $ (%UppBoundFinDemand% ne 100))   = sam(c,fin,"dis")*%UppBoundFinDemand%;

     v_splitCell.lo(c,w)     $ (sam(c,w,"dis") > 0)                                        = sam(c,w,"dis")*%lowBoundTrade%;
     v_splitCell.up(c,w)     $ ((sam(c,w,"dis") > 0)   $ (%UppBoundTrade% ne 100))         = sam(c,w,"dis")*%UppBoundTrade%;

     v_splitCell.lo(w,c)     $ (sam(w,c,"dis") > 0)                                        = sam(w,c,"dis")*%lowBoundTrade%;
     v_splitCell.up(w,c)     $ ((sam(w,c,"dis") > 0)   $ (%UppBoundTrade% ne 100))         = sam(w,c,"dis")*%UppBoundTrade%;

     v_splitCell.lo(gt,c)    $ (sam(gt,c,"dis") > 0)                                       = sam(gt,c,"dis")*%lowBoundIndProdTaxes%;
     v_splitCell.up(gt,c)    $ ((sam(gt,c,"dis") > 0)  $ (%UppBoundIndProdTaxes% ne 100))  = sam(gt,c,"dis")*%UppBoundIndProdTaxes%;

     v_splitCell.up(gt,c)    $ (sam(gt,c,"dis")  < 0)                                      = sam(gt,c,"dis")*%lowBoundIndProdTaxes%;
     v_splitCell.lo(gt,c)    $ ((sam(gt,c,"dis") < 0)  $ (%UppBoundIndProdTaxes% ne 100))  = sam(gt,c,"dis")*%UppBoundIndProdTaxes%;
*
*    --- for one-to-one mapping, use the target value
*
     v_splitCell.fx(ac,acp) $ ((oneToOne(ac) and oneToOne(acp))
         $ sum((acIni,acInip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),sam(acIni,acInip,"ce")))
        = sum((acIni,acInip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),sam(acIni,acInip,"ce"));
*
*    --- fix dis-aggregated cells to zero if aggregate target is empty
*
     v_splitCell.fx(ac,acp)
         $ (not sum((acIni,acInip) $ (map_acIni_ac(acIni,ac) and map_acIni_ac(acInip,acp)),sam(acIni,acInip,"ce"))) = 0;

     v_splitCell.fx(ac,acp) $ fixes(ac,acp) = fixes(ac,acp);

     m_hpdSplit.optfile    = 2;
     m_hpdSplit.HOLDFIXED  = 1;
     m_hpdSplit.tolinfeas  = 1.E-5;
     option QCP=%QCPSolver%;
     solve m_hpdSplit using QCP minimizing v_hpd;
*
*    --- rescale dis-aggregated SAM targets (for reporting)
*
     sam(ac,acp,"dis")         = sam(ac,acp,"dis") * samscal;
     sam("total0",acnt,"dis")  = sum(acntp,  sam(acntp,acnt,"dis"));
     sam(acntp,"total0","dis") = sum(acnt,   sam(acntp,acnt,"dis"));
*
*    --- assign results and scale
*
     sam(ac,acp,"fin")         = v_splitCell.l(ac,acp) * samScal;
  else
*
*    --- no split needed
*
     sam(ac,acp,"fin")         = sam(ac,acP,"dis") * samScal

  );

  sam("total0",acnt,"fin")  = sum(acntp,  sam(acntp,acnt,"fin"));
  sam(acntp,"total0","fin") = sum(acnt,   sam(acntp,acnt,"fin"));
*
* --- check for balancing errors
*
  samBalChk(ac,"fin")        = sam('total0',ac,"fin") - sam(ac,'total0',"fin");
  samBalChk(ac,"fin") $ (abs( samBalChk(ac,"fin"))<1E-6)   = 0;
  if (sum(ac $ samBalChk(ac,"fin"),1), abort "Not balanced , file: %system.fn%, line: %system.incline%",samBalChk);
*
* --- stats for final SAM
*
  $$batInclude "inc/samest/samStat.inc" fin acNt
*
* --- Load and print the new estimated macro-sam, macroSamMac,
*     by aggregating the new solution micro-sam, samCe.

 macSam(acmnt,acmntp,"fin") =
   sum((ac,acp)$(macmap3(acmnt,ac) and macmap3(acmntp,acp)), sam(ac,acp,"fin"));

* --- Computing and checking totals

 macSam('total3',ACMNT,"fin") = sum(acMNT2P, macSam(acMNT2P,ACMNT,"fin"));
 macSam(acMNT,'total3',"fin") = sum(acMNT2P, macSam(acMNT,ACMNT2P,"fin"));
 samBalChk3(acMNT)       = macSam('total3',ACMNT,"fin") - macSam(acMNT,'total3',"fin");

 display sam,samBalChk,macSam,samBalchk3,acm,macSet,macTotal,samStat;
*
* --- if this is a GTAP derived SAM with the right number of sectors, update GTAP based bridge file
*
$iftheni.GTAP_SAM "%sysEnv.GTAP_SAM%"=="on"

   $$batinclude "inc/title.inc" '" %regGtapName%: Update bridge file"'

   parameter samFinal;
   samFinal(acnt,acntp) = sam(acnt,acntp,"fin");
   execute_unload "%gams.scrdir%samFinal.gdx" samFinal;

   execute 'GDXXRW.EXE input=%gams.scrdir%samFinal.gdx O=reg/%regGtapName%/%BridgeFileToWrite%.xlsx par=samFinal  rng=SAM!A1 rdim=1 cdim=1'

$else.GTAP_SAM

   Execute_Unload $ (outGdx eq 2)
    "reg/%regGtapName%/sam_eg_out_%regGtapName%.gdx", SEM,
                  sam,
                  samCeStat,
                  valDiff,
                  PERDIFF,
                  bigDiffv,
                  bigDiffp,
                  bigDiffvp,
                  nBiggdiff,
                  biggDiffCnt,
                  samBalChk,
                  samscal
                  macSam
 ;


$endif.GTAP_SAM

 $$iftheni.GTAP_SAM not "%sysEnv.GTAP_SAM%"=="on"
    Execute_Unload  'reg/%regGtapName%/post-sam.gdx', sam, samBal;
 $$endif.GTAP_SAM

 $$batinclude "inc/title.inc" '" %regGtapName%: Post-balancing statistics and output for back-end"'
*
* --- to reporting back-end
*
  parameter p_samReport;

  set versionIni / target,ce,fixes /;
  p_samReport(acini,acinip,versionIni,"sam")    = sam(acini,acinip,versionIni);
  p_samReport(acini,"colSum",versionIni,"Sums") = sam("total0",acini,versionIni);
  p_samReport(acini,"rowSum",versionIni,"Sums") = sam(acini,"total0",versionIni);

  set versionFin / dis,fin /;
  p_samReport(ac,acp,versionFin,"sam")    = sam(ac,acp,versionFin);
  p_samReport(ac,acp,"disOnly","sam")     = sam(ac,acp,"dis");
  p_samReport(ac,acp,"finOnly","sam")     = sam(ac,acp,"fin");
  p_samReport(ac,acp,"disOnly","sam") $  (oneToOne(ac) and oneToOne(acp))      = 0;
  p_samReport(ac,acp,"finOnly","sam") $  (oneToOne(ac) and oneToOne(acp))      = 0;

  p_samReport(ac,"colSum",versionFin,"Sums") = sam("total0",ac,versionFin);
  p_samReport(ac,"rowSum",versionFin,"Sums") = sam(ac,"total0",versionFin);

  set stats  / nonZcnt,mean,max,min,StdDev,1DevLoCnt,1DevHiCnt,2DevLoCnt,2DevHiCnt /;

  p_samReport(stats,"stat",versionIni,"samStat") = samStat(stats,versionIni);
  p_samReport(stats,"stat",versionFin,"samStat") = samStat(stats,versionFin);

  p_samReport(acMnt,acMnt2P,"ce","macSam")     = macSam(acMNT,ACMNT2P,"fin");
  p_samReport(acMnt,acMnt2P,"proto","macSam")  = macSam(acMNT,ACMNT2P,"proto");

  $$ifthen.newDir not dexist "res/%regGtapName%"
    $$call mkdir "res/%regGtapName%"
  $$endif.newDir
  $$ifthen.newDir not dexist "reg/%regGtapName%"
    $$call mkdir "reg/%regGtapName%"
  $$endif.newDir
  $$ifthen.newDir not dexist "reg/%regGtapName%/sim"
    $$call mkdir "reg/%regGtapName%/sim"
  $$endif.newDir
  $$ifthen.noShock not exist "reg/%regGtapName%/sim/none.inc"
    $$call cp    "inc/shockfiles/*.inc"   "reg/%regGtapName%/sim"
  $$endif.noShock

  execute_unload "res/%regGtapName%/sam_%regGtapName%.gdx", p_samReport;

$iftheni.GTAP_SAM not "%sysEnv.GTAP_SAM%"=="on"
  $$ifthen.writeSAM "%writeSAM%" == "ON"

      $$batinclude "inc/title.inc" '" %regGtapName%: Writing SAM to bridge file"'

      Parameter SAM0(ac,acp) ;

      SAM0(ac,acp) = sam(ac,acp,"fin") ;
      SAM0("total0",acp) = 0 ;
      SAM0(acp,"total0") = 0 ;

      execute_unload "./sav/sam0_%regGtapName%.gdx", sam0;
      execute 'gdxxrw ./sav/sam0_%regGtapName%.gdx output=reg/%regGtapName%/%BridgeFileToWrite%.xlsx par=sam0 rng=SAM!B4'

  $$endif.writeSAM
$endif.GTAP_SAM
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
