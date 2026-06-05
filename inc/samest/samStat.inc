********************************************************************************
$ontext

   Manage project

   GAMS file : SAMSTAT.INC

   @purpose  : calculate SAM statistics
   @author   :
   @date     : 07.03.23
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : SAMEst

$offtext
********************************************************************************

  SamStat("nonZcnt","%1")  = sum((%2,%2p) $ Sam(%2,%2p,"%1"),1);

  SamStat("Mean","%1")   = sum((%2,%2p), Sam(%2,%2p,"%1"))/SamStat("nonZcnt","%1");

  SamStat("Max","%1")    = SMAX((%2,%2p), Sam(%2,%2p,"%1"));

  SamStat("min","%1")    = SMIN((%2,%2p), Sam(%2,%2p,"%1"));

  SamStat("StdDev","%1") = SQRT(sum((%2,%2p) $ Sam(%2,%2p,"%1"),
                              sqr((Sam(%2,%2p,"%1")-SamStat("Mean","%1"))))/ SamStat("nonZcnt","%1"));

  SamStat("1DevLoCnt","%1")  = sum((%2,%2p) $ (Sam(%2,%2p,"%1") and Sam(%2,%2p,"%1")
                            < (SamStat("Mean","%1")  - SamStat("StdDev","%1"))), 1);

  SamStat("1DevHiCnt","%1")  = sum((%2,%2p) $ (Sam(%2,%2p,"%1") and Sam(%2,%2p,"%1")
                            > (SamStat("Mean","%1")  + SamStat("StdDev","%1"))), 1);

  SamStat("2DevLoCnt","%1")  = sum((%2,%2p) $ (sam(%2,%2p,"%1")  and Sam(%2,%2p,"%1")
                            < (SamStat("Mean","%1")  - (2 * SamStat("StdDev","%1")))), 1);

  SamStat("2DevHiCnt","%1")  = sum((%2,%2p)$(sam(%2,%2p,"%1") and Sam(%2,%2p,"%1")
                            > (SamStat("Mean","%1") + (2 * SamStat("StdDev","%1")))), 1);

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
