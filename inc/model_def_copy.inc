********************************************************************************

$ontext



   Manage project



   GAMS file : MODEL_def.INC



   @purpose  : Paired version of list of model equations

               Pairing information will be removed with Python code

               to produce non-paired version



   @author   : Manage team

   @date     : 24.11.22

   @since    :

   @refDoc   :

   @seeAlso  :

   @calledBy : run.gms



$offtext

********************************************************************************



     e_px
     e_pp
     e_xap
     e_pap
     e_xfac
     e_pxfac
     e_pxfacInd
     e_lambdaxFac
     e_yieldLnd
     e_diffLabProd
     e_LandProd
     e_NatresProd
     e_capProd
     e_twist
     e_shrxFac
     e_x
     e_xp
     e_p
     e_xs


     e_facty
     e_kapy
     e_yh
     e_corrFac
     e_yhDsp
     e_gini
     e_deprate
     e_aps
     e_hhTr
     e_yfh
     e_savh
     e_pindh
     e_FundT
     e_FundG
     e_FundP
     e_FundF
     e_FundPrice
     e_FundPriceD
     e_interest
     e_ygov


     e_tFac
     e_rtFac
     e_tpsb
     e_tPrd
     e_tPhn
     e_tExp
     e_tImp
     e_tEmi
     e_rtEmi
     e_tPIT
     e_tCIT
     e_tVat
     e_tPaf
     e_ygEmi
     e_ygEmiXP
     e_ygPit
     e_ygCit
     e_ygPaf
     e_ygTotFacLand
     e_ygTotFacNatres
     e_ygTotFacLab
     e_ygTotFacCap
     e_ygTotpsb
     e_ygTotPrd
     e_ygTotPhn
     e_ygTotExp
     e_ygTotImp
     e_ygTotEmi
     e_ygTotEmiXP
     e_ygTotPit
     e_ygTotCit
     e_ygTotVatDom
     e_ygTotVatImp
     e_ygTotPaf
     e_ygTot


     e_gtax
     e_govTr
     e_primaryBalance
     e_fiscalBalance
     e_govBalance
     e_rgovBalance


     e_entrY
     e_savEntr
     e_entrTr


     e_dtransfers
     e_rowtransfers
     e_inv
*

*    --- foreign savings are usually fixed, and the invdrives some

*        other variable. Therefore not paired

*

     e_kstocke
     e_ror
     e_rorc
     e_rore
     e_devRoR
     e_grK
     e_savfGDP
     e_popAge
     e_Birth
     e_pop
     e_hpop
     e_consScale
     e_cdeInt
     e_xakh
     e_uh
     e_mmuh
     e_xakShr
     e_pfh
     e_xaoa
     e_xak
     e_xakf
     e_pxak


     e_pf
     e_xmarg
     e_xarmMarg
     e_pmarg
     e_pmargInd
     e_yf
     e_afInv
     e_gammaInv
     e_invPrice
     e_newInv


     e_pma
     e_pda
     e_xat
     e_xdd
     e_xm
     e_pa
     e_paf
     e_nrgInt
     e_elyInt
     e_ps
     e_xds
     e_eds
     e_pe
     e_pwe
     e_pwm
     e_pxfac_preTax
     e_atCap
     e_atCapVint
     e_trent
     e_trentInst
     e_pfmSupCap
     e_xfacFp
     e_xfacFpCap
     e_xfmSup
     e_xfmSupTot
     e_pfmSup
     e_pfmSupD
     e_unemp


     e_oldCap


     e_rrat1
     e_trentKaps
     e_trentAvg
     e_pk
     e_xpOldNoCap
     e_xpNew
     e_xpOld
     e_xpEnTech


     e_tdelst
     e_rtdelst
     e_pdelst
     e_totexp
     e_rtotexp
     e_pexp
     e_totimp
     e_rtotimp
     e_pimp
     e_gdpmp
     $$iftheni.capAcc "%capAccClosure%" == "Endogenous with exchange rate and GDP price index fixed"

       e_pgdpmp
     $$else.capAcc

       e_pgdpmp
       e_savf
     $$endif.capAcc

     e_rgdppc
     e_gdpfc
     e_rgdpfc
     e_pgdpfc
     e_rshrGDP
     e_fiscalRespDrv1
     e_fiscalRespDrv
     e_rshrGDPGov
     e_nshrGDP
     e_GdpCompShr
     e_GdpCompShrX
     e_GdpCompShrM
     e_cpi
     e_kaps
     e_depr
     e_prodInv
     e_kstock


     e_emiLUCF
     e_emiComm
     e_emiFact
     e_emiProd
     e_emiTot
     e_emiGHG
     e_emiInv
     e_emiInt
     e_emiIntSec
     e_emiIntHH
     e_emiSecA
     e_debtGDP
     e_dCarbon
     e_lCovArea
     e_builtupArea
     e_dmgAir
     e_ExposedAirPoll


     e_avgDebtInt
     e_DebtStk
     e_debtPay
     e_debtInterest
     e_debtPrincipal
     e_debtNew
     e_debtNewT
     e_DebtStkT
     e_pIND
     e_psIND
     e_paIND
     e_xfdInv
     e_walras
     e_bot
     $$iftheni.botGDP   "%GPDCompNetExports%"=="nominal"

         e_botGdp
     $$else.botGDP

         e_botGDP
     $$endif.botGDP

     e_ElecMix
     e_sectTot


     $$iftheni.baseline "%task%"=="Baseline"

        $$iftheni.diffLabProd "%DiffLabProd%"=="on"

           e_rgdpmp
        $$else.diffLabProd

           e_rgdpmp
        $$endif.diffLabProd

     $$else.baseline

*

*       --- simulation mode: rdgpmd endogenous

*

        e_rgdpmp
     $$endif.baseline



     e_ggdppc
     e_sectShr
     e_fiscalResp


     e_psExpRel
     e_delSt
     e_St
     e_psExp
     e_xsExp


     $$iftheni.endotech "%endotech%"=="on"

        e_xRD
        e_yfdRD
        e_pfdRD
        e_xfdRD
        e_einn:(einn | frontBGP)

     $$endif.endotech

     e_dummy





