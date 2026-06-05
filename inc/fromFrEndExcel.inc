********************************************************************************
$ontext

   MANAGE project

   GAMS file : FromFrndExcel.inc

   @purpose  : Read exogenous variables from EXCEL sheet
   @author   : Wolfgang Britz
   @date     : 19.12.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
parameter input(blocks,*,endo_exo,*,*,*,*,t)                 "Potentially user modified values on output";


set unknownUel / nan /;
  $$onmulti
  set vars / exo /;
  $$offmulti

set check(*,*,*,*,*,*);
$ontext

*
* --- the following more elegant solution does not work
*
$onmulti
$onEmbeddedCode Python:
  from gams_connect.connectdatabase import ConnectDatabase
  blocks = gams.get('blocks',keyFormat=KeyFormat.FLAT)

  cdb = ConnectDatabase(gams._system_directory, gams)
  i = 0
  for block in blocks:
      i = i +1
      range = str(block)+'!A1'
      inputName = 'input' + str(i)
      checkName = 'check' + str(i)

      cdb.exec_task( {'PandasExcelReader':{'file':r'%odir%/%shk_file_xslx%.xlsx',
                                           'symbols':
                                              [{'name': checkName,'rowDimension':6,'columnDimension':0,'type':'set','range':range},
                                               {'name': inputName,'rowDimension':7,'columnDimension':1,'range':range}]
                                           }})
      cdb.exec_task( {'GAMSWriter':{'mergeType':'merge','symbols': [{'name':inputName,'newName':'input'},{'name':checkName,'newName':'check'}]}})

$offEmbeddedCode input check
$offmulti
$offtext

*
* --- read data from EXCEL into the GAMS symbol input defined above
*
$onEmbeddedCode Python:

  blocks = list(gams.get('blocks',keyFormat=KeyFormat.FLAT))
  with open(r'%savf%/readSheets.inc', 'w') as f:
       for block in blocks:
           f.write('$setglobal curSheet "'+str(block)+'"\n')
           f.write('$include inc/fromFrEndReadSheet.inc\n')
  f.closed

$offEmbeddedCode
$onmulti
$include '%savf%/readSheets.inc'
$offmulti
*
* --- throw check into GDX container and read from there the list of labels used
*     for the variable labels
*
$GDXOUT "%gams.scrdir%\check.gdx"
  $$unload check input
$GDXOUT
set dim1(*),dim2(*),dim3(*);
set foundBlocks,foundVars,foundEndo_exo,varNames;
$GDXIN  "%gams.scrdir%\check.gdx"
   $$load foundBlocks<check.dim1
   $$load foundVars<check.dim2
   $$load foundEndo_exo<check.dim3
   $$load dim1<check.dim4
   $$load dim2<check.dim5
   $$load dim3<check.dim6
   $$load varNames<input.dim7
$GDXIN

 set errorSet;
 errorSet(foundBlocks) = yes $ (not sum(sameas(foundBlocks,blocks),1));
 if (  card(errorSet), abort "Some block names in %odir%/%shk_file_xslx%.xlsx are not correct",errorSet);
 errorSet(foundVars) = yes $ (not sum(sameas(foundVars,vars),1));
 if (  card(errorSet), abort "Some variable names in %odir%/%shk_file_xslx%.xlsx are not correct",errorSet);
 errorSet(foundEndo_exo) = yes $ (not sum(sameas(foundEndo_exo,endo_exo),1));
 if (  card(errorSet), abort "Only endo and exo allowed in %odir%/%shk_file_xslx%.xlsx to set variable status",errorSet);

  parameter p_nexo(blocks,*);
*
* --- read # of exogenous instances in each block as generated when the EXCEL sheet was written
*
  p_nexo(blocks,"required")   = input(blocks,"exo","exo","-","-","-","-",t0);
*
* --- read # of exogenous instances as flagged by user
*
  p_nexo(blocks,"found")      = sum( (vars,dim1,dim2,dim3) $ (check(blocks,vars,"exo",dim1,dim2,dim3) $ (not sameas(vars,"exo"))),1);
*
* --- flag the correct instances where the counts match and remove them from nexo
*
  p_nexo(blocks,"correct")     $ (p_nexo(blocks,"required") eq p_nexo(blocks,"found")) = 1;
  p_nexo(blocks,"required")    $ p_nexo(blocks,"correct") = 0;
  p_nexo(blocks,"found")       $ p_nexo(blocks,"correct") = 0;
  p_nexo(blocks,"correct")     $ p_nexo(blocks,"correct") = 0;
*
* --- the leftover cases are an error intrduced by the user: abort and report the erroneous block(s)
*
  if( card(p_nexo), abort "Error in EXCEL input: at least one block where the number of exogenous variables is not correct ",p_nexo);
*
* --- generate include file where variables are either fixed or bounds are set to -/+ inf
*

$onEmbeddedCode Python:

  with open(r'%savf%/fromPhytonEndoExo.inc', 'w') as f:
     input = list(gams.get('input',keyFormat=KeyFormat.FLAT))


     for x in input:
#       --- skipped the lines which report the # of exogenous instances in each block
        if not x[1] == 'exo':

#       --- build record: block name,var name,exo/endo,dims,data
            if x[2] == 'exo':
                l = '.fx("'
            else:
                l = '.l("'

            if x[3] == '-':
               dim1 =''
            else:
               dim1 = x[3]+'","'

            if x[4] == '-':
               dim2 =''
            else:
               dim2 = x[4]+'","'

            if x[5] == '-':
               dim3 =''
            else:
               dim3 = x[5]+'","'

#
#           --- x[7] is the time point, x[8] is the actual variable level
#               x[6] is the variables long text, not used
#               x[0] is the block name, not used
#
            f.write(' '+ x[1]+l+dim1+dim2+dim3+x[7]+'") $ ts("'+x[7]+'") ='+str(x[8])+';'+'\n')
            if x[2] == 'endo':
                f.write(' '+ x[1]+'.lo("'+dim1+dim2+dim3+x[7]+'") $ ts("'+x[7]+'")=-inf;\n')
                f.write(' '+ x[1]+'.up("'+dim1+dim2+dim3+x[7]+'") $ ts("'+x[7]+'")= +inf;\n')

  f.closed

$offEmbeddedCode
$if not errorfree $abort Compilation error after file: %system.fn%
if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
