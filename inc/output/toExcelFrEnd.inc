********************************************************************************
$ontext

   MANAGE project

   GAMS file : toExcelFrEnd.INC

   @purpose  : Output selected variables as time series to an EXCEL sheet
   @author   : W.Britz
   @date     : 19.12.22
   @since    :
   @refDoc   :
   @seeAlso  :
   @calledBy : run.gms

$offtext
********************************************************************************
  $$iftheni.decl "%1"=="decl"

     $$onembeddedCode Python:

       import os
       f = r"%odir%/%regGtapName%_%simName%.xlsx"

       if os.path.exists(r"%odir%/%regGtapName%_%simName%.xlsx"):
          try:
              os.rename(r"%odir%/%regGtapName%_%simName%.xlsx", r"%odir%/%regGtapName%_%simName%.xlsx")
              os.environ["xlsx_can_write"] = "on"
          except OSError as e:
              os.environ["xlsx_can_write"] = "off"

     $$offembeddedCode
     $$ifi "%sysEnv.xlsx_can_write%"=="off" $abort "%odir%/%regGtapName%_%simName%.xlsx is blocked, please close."
*
*    --- Define the variables which should be outputted to excel. This likeley also
*        defines the order which in they are show inside each block
*
     set vars   "Variables covered by mechanism" / rgdpmp,chixFacl
                                                   sectShr,chixFacAgr,tPhnD2
                                                   elecMix,chixFacFA
                                                   elyInt,chixFacF
                                                   popAge
                                                   pwmrow,pwerow
                                                   emiGhg,tEmiD0
                                                   tFacM2,tPafD0,tPITD0,tCITD0,ygTot
                                                 /;
*
*    --- The blocks help the user to understand where closure swaps can be easily
*        introduced. Variables in the same block are shown following each other.
*
     set blocks "Closure blocks" / "Macro","Population",
                                   "Sector shares","Electricity mix","Electricity intensity",
                                   "World market prices","GHG emissions","Tax reform" /;

*
*    --- this shows the standard closure for a simulation run
*
     set endo_exo / endo,exo /;

     set block_vars(blocks,vars,endo_exo) "Closure groups"
                                 / "Macro".(rgdpmp.endo,chixFacl.exo)
*                                   "Labour markets".(lst.endo,(chils.exo))
                                   "Sector shares".(sectShr.endo,chixFacAgr.exo,tPhnD2.exo)
                                   "Electricity mix".(elecMix.endo,chixFacFA.exo)
                                   "Electricity intensity".(elyInt.endo,chixFacF.exo)
*
*    --- note that some variables are always exogenous
*
                                   "Population".((popage.exo))
                                   "World market prices".(pwmRow.exo,pwerow.exo)
                                   "GHG emissions".(emiGhg.endo,tEmiD0.exo)
                                   "Tax reform".(tFacM2.exo,tPafD0.exo,tPITD0.exo,tCITD0.exo,ygTot.endo)
                                 /;
     set empty_dim / '-' /;

*
*    --- currently, the interface supports up to four dimensions, where dim4
*        is always time and shown as the columns in the EXCEL sheet
*
     set dims / dim1*dim3 /;
*
*    --- this allows to drop instances of variables from the sheet which are not used
*        for the closure swaps (one could extend this to make it block specific)
*
     set restr_dims(vars,dims,*) / chixFacAgr.dim2."f-top"
                                   chixFacFA.dim1."va"
                                   chixFacF.dim1."c-ely"
                                   emiGhg.dim1."ghgs"
                                   tFacM2.dim1.(set.fp)
                                 /;
     restr_dims("chixFacFA","dim2",aElec) = YES;


    $$if not errorFree $$abort "Compilation error after end of declaration part in 'toExcelFrEnd.inc', check input data."

  $$else.decl

*    ------------------------------------------------------------------
*
*      Define steering blocks for Python based export
*      of some variables which I used might want to use in
*      closures swap in counterfactuals
*
*    ------------------------------------------------------------------

*
*    --- define variable long-texts to export as additional UELS in GAMS
*
     set newLabels(*);
     $$onembeddedCode Python:

        # --- get all mapped block-vars combinations from cross-set block-vars

         block_vars = list(gams.get('block_vars',keyFormat=KeyFormat.FLAT))
         labels = []

        #  --- get variable name x[1] and retrieve registered long-text from data base

         for x in block_vars:
              labels.append(gams.db[x[1]].text)
         gams.set('newLabels',labels)
     $$offembeddedCode newLabels

     $$if not errorfree $abort Compilation error in file: %system.fn%, line: %system.incline%

     if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
*
*    ---- number of years output, used to calculate # of exogenous ariables in each block
*
     scalar nTrun; nTrun = Card(t);
*
*    ---- define output block for EXCEL
*
     parameter p_excelOutput(*,*,*,*,*,*,*,*) "Output to excel: block,varname,vartext,endo_exo,dim1...3,data";
     embeddedCode Python:
       nTrun      = list(gams.get('ntRun',keyFormat=KeyFormat.FLAT))[0]
       t0         = list(gams.get('tt',keyFormat=KeyFormat.FLAT))[0]
       block_vars = list(gams.get('block_vars',keyFormat=KeyFormat.FLAT))
       restr_dims = list(gams.get('restr_dims',keyFormat=KeyFormat.FLAT))
     pauseEmbeddedCode

     $$ifthene.GAMSVersion %system.gamsrelease%>42
       continueEmbeddedCode:
          from gams.connect import ConnectDatabase
       pauseEmbeddedCode
     $$else.GAMSVersion
       continueEmbeddedCode:
          from gams_connect.connectdatabase import ConnectDatabase
       pauseEmbeddedCode
     $$endif.GAMSVersion
*
     singleton Set curBlock(blocks);

     loop(blocks,

        curBlock(blocks) =yes;
        option kill=p_excelOutput;
        $$batinclude "inc/title.inc" '"Export "' blocks.tl '" to EXCEL "'

        continueEmbeddedCode:

#          --- GAMS record for output symbol exported to EXCEL
           records = []

#          --- get blocks (to count exogenous vars in each block)
           curBlock = list(gams.get('curBlock',keyFormat=KeyFormat.FLAT))

#          --- reset counter for exogenous time series in block
           nExo = 0

#          --- get all block-var combinations

           for x in block_vars:

#              --- work only for the ones where the block name matches

               if x[0] == curBlock[0]:

#                --- build record: block name,var name,exo/endo,longtext,years,data

                 blockName    = x[0]
                 varName      = x[1]
                 XE           = x[2]
                 varText      = gams.db[varName].text

                 check = False
                 for z in restr_dims:
                     if z[0] == x[1]:
                        check = True

#                --- get the records for the variables (time series)
                 var_records = list(gams.get(varName,keyFormat=KeyFormat.FLAT))

                 count   = 0
                 nCurExo = 0
                 for y in var_records:
                     count = count + 1
                     nDim   = len(y) - 1;

#                    --- Set year label
                     dim4 = y[nDim-1]

#                    --- depending on actual dimension in use by GAMS for the
#                        variable, set labels in each dimension

                     if nDim > 1:
                        dim1 = y[0]
                     else:
                        dim1 = '-'

                     if nDim > 2:
                        dim2 = y[1]
                     else:
                        dim2 = '-'

                     if nDim == 4:
                        dim3 = y[2]
                     else:
                        dim3 = '-'

#                    --- variable level
                     val  = y[nDim][0] + 1.E-20

#                    --- check if we have a case where only some cases should be considered
#                        drop1 = "yes" means that the instance should be dropped as the label in dimension 1
#                                      is not in the list of instances to keep. "na" means that dimension 1
#                                      is not filtered. "no" means that the element is to be kept.
#
                     if check:
                        drop1 = "na"
                        drop2 = "na"
                        drop3 = "na"
                        for z in restr_dims:
#                           --- The variable name matches the current one
                            if z[0] == x[1]:
                               if  z[1] == 'dim1':
                                  if dim1 == z[2]:
                                     drop1 = "no"
                                  elif drop1 == "na":
                                       drop1 = "yes"
                               elif  z[1] == 'dim2':
                                  if dim2 == z[2]:
                                     drop2 = "no"
                                  elif drop2 == "na":
                                       drop2 = "yes"
                               elif  z[1] == 'dim3':
                                  if dim3 == z[2]:
                                     drop3 = "no"
                                  elif drop3 == "na":
                                       drop3 = "yes"

                        drop = "yes"
                        if drop1 != "yes" and drop2 != "yes" and drop3 != "yes": drop = "no"
                     else:
                        drop = "no"

#                    --- store current entry

                     if drop == "no":
                        records.append( (blockName,varName,XE,dim1,dim2,dim3,varText,dim4,val) )

#                       --- add up exogenous instances
                        if XE == 'exo':
                           nCurExo = nCurExo + 1

#                --- divide # of fixed observations by # obs

                 if count:
                    nExo = nExo + nCurExo / nTrun
                 else:
                    nExo = 0

           if nExo == 0:
              nExo = -1

#          --- pseudo record at end of block which reports the # of exogenous instances
           records.append( (blockName,'exo','exo','-','-','-','-',str(t0),nExo) )
           gams.set('p_excelOutput',records)

           range = str(curBlock[0])+'!A1'

           cdb = ConnectDatabase(gams._system_directory, ecdb=gams)
           cdb.execute( {'GAMSReader':{'symbols': [{'name': 'p_excelOutput'}]}})

           cdb.execute( {'ExcelWriter':
                             {
                               'file':r"%odir%/%regGtapName%_%simName%.xlsx",
                               'symbols': [{'name': 'p_excelOutput','range':range}]
                              }
                           })

     pauseEmbeddedCode

     );
*
*    -- avoid any hiccups in follow programs if something goes wrong with the EXCEL export
*
     if ( execError, execError = 0;);
  $$endif.decl

  $$if not errorfree $abort Compilation error after file: %system.fn%
  if ( execerror, abort "Run-Time error in file: %system.fn%, line: %system.incline%");
