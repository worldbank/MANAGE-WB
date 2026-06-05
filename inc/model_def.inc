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

     e_px.px
     e_pp.pp
     e_xap.xa
     e_pap.pxFac
     e_xfac.xFac
     e_pxfac.pxFac
     e_pxfacInd.pxFacInd
     e_lambdaxFac.lambdaXfac
     e_yieldLnd.yieldLnd
     e_diffLabProd.chixFacFA
     e_LandProd.chixFacF
     e_NatresProd.chixFacF
     e_capProd.chixFacF
     e_twist.twist
     e_shrxFac.shrxfac
     e_x.x
     e_xp.xp
     e_p.p
     e_xs.xs

     e_facty.facty
     e_kapy.kapy
     e_yh.yh
     e_corrFac.corrFac
     e_yhDsp.yhDsp
     e_gini.gini
     e_deprate.deprate
     e_aps.aps
     e_hhTr.tottr
     e_yfh.xfd
     e_savh.savh
     e_pindh.pindh
     e_FundT.fundt
     e_FundG.FundG
     e_FundP.FundP
     e_FundF.FundF
     e_FundPrice.FundPrice
     e_FundPriceD.FundPriceD
     e_interest.interest
     e_ygov.ygov

     e_tFac.tfac
     e_rtFac.rtfac
     e_tpsb.tpsb
     e_tPrd.tprd
     e_tPhn.tphn
     e_tExp.texp
     e_tImp.timp
     e_tEmi.tEmi
     e_rtEmi.rtEmi
     e_tPIT.tPit
     e_tCIT.tCit
     e_tVat.tVat
     e_tPaf.tPaf
     e_ygEmi.ygEmi
     e_ygEmiXP.ygEmiXP
     e_ygPit.ygPit
     e_ygCit.ygCit
     e_ygPaf.ygPaf
     e_ygTotFacLand.yg
     e_ygTotFacNatres.yg
     e_ygTotFacLab.yg
     e_ygTotFacCap.yg
     e_ygTotpsb.yg
     e_ygTotPrd.yg
     e_ygTotPhn.yg
     e_ygTotExp.yg
     e_ygTotImp.yg
     e_ygTotEmi.yg
     e_ygTotEmiXP.yg
     e_ygTotPit.yg
     e_ygTotCit.yg
     e_ygTotVatDom.yg
     e_ygTotVatImp.yg
     e_ygTotPaf.yg
     e_ygTot.ygTot

     e_gtax.gtax
     e_govTr.totTr
     e_primaryBalance.primaryBalance
     e_fiscalBalance.fiscalBalance
     e_govBalance.govBalance
     e_rgovBalance.rgovBalance

     e_entrY.entrY
     e_savEntr.savEntr
     e_entrTr.totTr

     e_dtransfers.transfers
     e_rowtransfers.transfers
     e_inv.xfd
*
*    --- foreign savings are usually fixed, and the invdrives some
*        other variable. Therefore not paired
*
     e_kstocke.kstocke
     e_ror.ror
     e_rorc.rorc
     e_rore.rore
     e_devRoR.devRor
     e_grK.grK
     e_savfGDP.savfGDP
     e_popAge.popAge
     e_Birth.popAge
     e_pop.pop
     e_hpop.hpop
     e_consScale.consScale
     e_cdeInt.cdeInt
     e_xakh.xak
     e_uh.uh
     e_mmuh.mmuh
     e_xakShr.xakShr
     e_pfh.pf
     e_xaoa.xa
     e_xak.xak
     e_xakf.xak
     e_pxak.pxak

     e_pf.pf
     e_xmarg.xmarg
     e_xarmMarg.xa
     e_pmarg.pmarg
     e_pmargInd.pmargInd
     e_yf.yf
     e_afInv.lambdaOa
     e_gammaInv.gammaInv
     e_invPrice.invPrice
     e_newInv.newInv

     e_pma.pma
     e_pda.pda
     e_xat.xat
     e_xdd.xd
     e_xm.xm
     e_pa.pa
     e_paf.paf
     e_nrgInt.nrgInt
     e_elyInt.elyint
     e_ps.ps
     e_xds.pd
     e_eds.xe
     e_pe.pe
     e_pwe.pwe
     e_pwm.pwm
     e_pxfac_preTax.pxFac
     e_atCap.atFmSup
     e_atCapVint.xfmSup
     e_trent.trent
     e_trentInst.pfmSup
     e_pfmSupCap.pfmSup
     e_xfacFp.pxfac_pretax
     e_xfacFpCap.xfmSup
     e_xfmSup.xfmsup
     e_xfmSupTot.xfmsup
     e_pfmSup.pfmsup
     e_pfmSupD.pfmsupD
     e_unemp.unemp

     e_oldCap.oldCap

     e_rrat1.rrat1
     e_trentKaps.trent
     e_trentAvg.trentAvg
     e_pk.pxfac_preTax
     e_xpOldNoCap.xfac
     e_xpNew.xfac
     e_xpOld.xfac
     e_xpEnTech.xfac

     e_tdelst.tdelst
     e_rtdelst.rtdelst
     e_pdelst.pdelSt
     e_totexp.totexp
     e_rtotexp.rtotexp
     e_pexp.pexp
     e_totimp.totimp
     e_rtotimp.rtotimp
     e_pimp.pimp
     e_gdpmp.gdpmp
     $$iftheni.capAcc "%capAccClosure%" == "Endogenous with exchange rate and GDP price index fixed"
       e_pgdpmp.savf
     $$else.capAcc
       e_pgdpmp.pgdpmp
       e_savf.savf
     $$endif.capAcc
     e_rgdppc.rgdppc
     e_gdpfc.gdpfc
     e_rgdpfc.rgdpfc
     e_pgdpfc.pgdpfc
     e_rshrGDP.rshrGDP
     e_fiscalRespDrv1.fiscalRespDrv1
     e_fiscalRespDrv.fiscalRespDrv
     e_rshrGDPGov.xfd
     e_nshrGDP.nShrGDP
     e_GdpCompShr.gdpCompShr
     e_GdpCompShrX.gdpCompShr
     e_GdpCompShrM.gdpCompShr
     e_cpi.cpi,
     e_kaps.kaps
     e_depr.depr
     e_prodInv.prodinv
     e_kstock.kstock

     e_emiLUCF.emiLUCF
     e_emiComm.emiComm
     e_emiFact.emiFact
     e_emiProd.emiXP
     e_emiTot.emiTot
     e_emiGHG.emighg
     e_emiInv.emiInv
     e_emiInt.emiInt
     e_emiIntSec.emiIntSec
     e_emiIntHH.emiIntHH
     e_emiSecA.emiSecA
     e_debtGDP.debtGDP
     e_dCarbon.dCarbon
     e_lCovArea.lCovArea
     e_builtupArea.lCovArea
     e_dmgAir.dmgAir
     e_ExposedAirPoll.ExposedAirPoll

     e_avgDebtInt.avgDebtInt
     e_DebtStk.DebtStk
     e_debtPay.debtPay
     e_debtInterest.debtInterest
     e_debtPrincipal.debtPrincipal
     e_debtNew.debtNew
     e_debtNewT.debtNewT
     e_DebtStkT.DebtStkT
     e_pIND.pIND,
     e_psIND.psIND
     e_paIND.paIND
     e_xfdInv.xfd
     e_walras.walras
     e_bot.bot
     $$iftheni.botGDP   "%GPDCompNetExports%"=="nominal"
         e_botGdp.savfGdp
     $$else.botGDP
         e_botGDP.botGDP
     $$endif.botGDP
     e_ElecMix.elecmix
     e_sectTot.sectTot

     $$iftheni.baseline "%task%"=="Baseline"
        $$iftheni.diffLabProd "%DiffLabProd%"=="on"
           e_rgdpmp.labProd
        $$else.diffLabProd
           e_rgdpmp.ChixFacL
        $$endif.diffLabProd
     $$else.baseline
*
*       --- simulation mode: rdgpmd endogenous
*
        e_rgdpmp.rgdpmp
     $$endif.baseline

     e_ggdppc.ggdppc
     e_sectShr.sectShr
     e_fiscalResp.tmall

     e_psExpRel.psExpRel
     e_delSt.delSt
     e_St.St
     e_psExp.psExp
     e_xsExp.xsExp

     $$iftheni.endotech "%endotech%"=="on"
        e_xRD.xrd
        e_yfdRD.yfdRD
        e_pfdRD.pfdRD
        e_xfdRD.xfdRD
        e_einn:(einn | frontBGP)
     $$endif.endotech
     e_dummy


