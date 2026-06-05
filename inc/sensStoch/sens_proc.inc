********************************************************************************
$ontext

   MANAGE-WB project

   GAMS file : SENS_PROC.INC

   @purpose  : Generate cross-sets between LHS factors, changes parameters and their
               indices
   @author   : Wolfgang Britz
   @date     : 03.03.19
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : sensitivity.gms

$offtext
********************************************************************************
  option kill=s1,kill=s2,kill=s3;
  option kill=l1,kill=l2,kill=l3;
  option kill=l1_s1,kill=l2_s2,kill=l3_s3;
  l1("0") = YES;
  l2("0") = YES;
  l3("0") = YES;

  $$onembeddedCode Python:
    import os

    s = "%2"
    gams.printLog(" 2 "+s)
    if s.startswith("("):
       os.environ["arg2"] = '"'+s[1:-1]+'"'
    elif not s:
       os.environ["arg2"] = ""
    else:
       os.environ["arg2"] = s

    s = "%3"
    gams.printLog(" 3 "+s)
    if s.startswith("("):
       os.environ["arg3"] = '"'+s[1:-1]+'"'
    elif not s:
       os.environ["arg3"] = " "
    else:
       os.environ["arg3"] = s

    s = "%4"
    gams.printLog(" 4 "+s)
    if s.startswith("("):
       gams.printLog(s)
       os.environ["arg4"] = '"'+s[1:-1]+'"'
    elif s == "":
       os.environ["arg4"] = " "
    else:
       os.environ["arg4"] = s

  $$offembeddedCode

  $$set arg2 %sysEnv.arg2%
  $$set arg3 %sysEnv.arg3%
  $$set arg4 %sysEnv.arg4%

  $$iftheni.set1 not "%2"==""
*
*    --- the second agrument is the first driving set of the parameter
*

*
*      --- "same" implies that all elements on the driving set will reveive the same random shock,
*          link to empty element
*
     $$ifi "%5" == "same"  s1(%sysEnv.arg2%)     = yes; l1_s1(l1,s1) = YES;
*
*
*      --- "draws" implies that each element on the driving set will receive its own random shock
*          link draws to set elements
*
     $$ifi "%5" == "draws" s1(%sysEnv.arg2%)     = yes; l1(%sysEnv.arg2%)       = YES; l1("0") = no; l1_s1(l1,s1) $ (l1.pos eq s1.pos) = YES;

  $$else.set1
*
*    --- the driving set is empty: link to empy element (same random shock)
*
     s1("") = YES; l1_s1(l1,s1) = YES;
  $$endif.set1

  $$iftheni.set2 not  "%3"==""
     $$ifi "%6" == "same"  s2(%sysEnv.arg3%)   = yes; l2_s2(l2,s2) = YES;
     $$ifi "%6" == "draws" s2(%sysEnv.arg3%)   = yes; l2(%sysEnv.arg3%) = YES; l2("0") = no; l2_s2(l2,s2) $ (l2.pos eq s2.pos) = YES;
  $$else.set2
     s2("") = YES; l2_s2(l2,s2) = YES;
  $$endif.set2

  $$iftheni.set3 not "%4"==""
     $$ifi "%7" == "same"  s3(%sysEnv.arg4%)    = yes; l3_s3(l3,s3) = YES;
     $$ifi "%7" == "draws" s3(%sysEnv.arg4%)    = yes; l3(%sysEnv.arg4%) = YES; l3("0") = no; l3_s3(l3,s3) $ (l3.pos eq s3.pos) = YES;
  $$else.set3
     s3("") = YES; l3_s3(l3,s3) = YES;
  $$endif.set3

  s1All(s1) = YES;
  s2All(s2) = YES;
  s3All(s3) = YES;

  set lastl1,lastl2,lastl3;
  lastl1(l1) =yes;
  lastl2(l2) =yes;
  lastl3(l3) =yes;
  scalar change;

  nFactor = nFactor + 1;
  scalar start;start=nFactor;
  option kill=curRFactor;
  loop( (l1,l2,l3),
*
*    ---- the current linearized factor is the one which position in the set matches the current count
*
     curRFactor(rFactors) = yes $ (rfactors.pos eq nFactor);
*
*    --- link between current linearized factor, the parameter to shock ("%1") and the three
*        potential elements s1,s2 and s3 on that parameter
*
     factor_par_s1_s2_s3(curRFactor,"%1",s1,s2,s3) $ (l1_s1(l1,s1) $ l2_s2(l2,s2) $ l3_s3(l3,s3))  = YES;
     factor_par(curRFactor,"%1") = YES;
*
*    --- if one of the driving set elements changed, switch to next linearized factor
*
     if ( sum(lastL1 $ (not sameas(l1,lastl1)),1), change = 1; option kill=lastL1; lastL1(l1) = yes);
     if ( sum(lastL2 $ (not sameas(l2,lastl2)),1), change = 1; option kill=lastl2; lastl2(l2) = yes);
     if ( sum(lastL3 $ (not sameas(l3,lastl3)),1), change = 1; option kill=lastl3; lastl3(l3) = yes);

     if ( change, nFactor = nFactor+1);
  );

  nFactor $ (nFactor gt start) = nFactor -1;
  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
