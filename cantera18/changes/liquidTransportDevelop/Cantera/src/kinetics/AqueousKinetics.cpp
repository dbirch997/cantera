/**
 *  @file AqueousKinetics.cpp 
 *
 * Homogeneous kinetics in an aqueous phase, either condensed
 * or dilute in salts
 * 
 */
/*
 * Copywrite (2006) Sandia Corporation. Under the terms of
 * Contract DE-AC04-94AL85000 with Sandia Corporation, the
 * U.S. Government retains certain rights in this software.
 */
/*
 *  $Date$
 *  $Revision$
 */



#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include "AqueousKinetics.h"
#include "ReactionData.h"
#include "RateCoeffMgr.h"

#include <iostream>
using namespace std;

namespace Cantera {
  //====================================================================================================================
  AqueousKineticsData::AqueousKineticsData() :
    m_logp_ref(0.0),
    m_logc_ref(0.0),
    m_ROP_ok(false),
    m_temp(0.0)
  {
  }
  //====================================================================================================================
  AqueousKineticsData::~AqueousKineticsData()
  {
  }
 //====================================================================================================================
  AqueousKineticsData::AqueousKineticsData(const AqueousKineticsData &right) :
    m_logp_ref(0.0),
    m_logc_ref(0.0),
    m_ROP_ok(false),
    m_temp(0.0)
  {
    *this=right;
  }
  //====================================================================================================================

  AqueousKineticsData&  AqueousKineticsData::operator=(const AqueousKineticsData &right)
  {
    if (this != &right) {
      m_logp_ref = right.m_logp_ref;
      m_logc_ref = right.m_logc_ref;
      m_ropf = right.m_ropf;
      m_ropr = right.m_ropr;
      m_ropnet = right.m_ropnet;
      m_rfn_low = right.m_rfn_low;
      m_rfn_high = right.m_rfn_high;
      m_ROP_ok  = right.m_ROP_ok;
      m_temp = right.m_temp;
      m_rfn = right.m_rfn;
      m_rkcn = right.m_rkcn;
    }
    return *this;
  }
  //====================================================================================================================

 
  //====================================================================================================================
  /**
   * Construct an empty reaction mechanism.
   */    
  AqueousKinetics::AqueousKinetics(thermo_t* thermo) :
    Kinetics(),
    m_kk(0), 
    m_nfall(0), 
    m_nirrev(0), 
    m_nrev(0),
    m_finalized(false)
  {
    if (thermo != 0) addPhase(*thermo);
    m_kdata = new AqueousKineticsData;
    m_kdata->m_temp = 0.0;
    m_rxnstoich = new ReactionStoichMgr;
  }
 //====================================================================================================================
  AqueousKinetics::AqueousKinetics(const AqueousKinetics &right) :
    Kinetics(),
    m_kk(0), 
    m_nfall(0), 
    m_nirrev(0), 
    m_nrev(0),
    m_finalized(false)
  {
    *this = right;
  }
 //====================================================================================================================
  AqueousKinetics::~AqueousKinetics() {
    delete m_kdata; 
    delete m_rxnstoich;
  } 
  //====================================================================================================================
   AqueousKinetics& AqueousKinetics::operator=(const AqueousKinetics &right)
   {
   if (this == &right) return *this;

    Kinetics::operator=(right);
   
    m_kk = right.m_kk;
    m_nfall = right.m_nfall;
    m_rates = right.m_rates;
    m_index = right.m_index;
    m_irrev = right.m_irrev;

    *m_rxnstoich = *(right.m_rxnstoich);

    m_fwdOrder = right.m_fwdOrder;
    m_nirrev = right.m_nirrev;
    m_nrev = right.m_nrev;
    m_rgroups = right.m_rgroups;
    m_pgroups = right.m_pgroups;
    m_rxntype = right.m_rxntype;
    m_rrxn = right.m_rrxn;
    m_prxn = right.m_prxn;
    m_dn = right.m_dn;
    m_revindex = right.m_revindex;
    m_rxneqn = right.m_rxneqn;

    *m_kdata = *(right.m_kdata);

    m_conc = right.m_conc; 
    m_grt = right.m_grt;
    m_finalized = right.m_finalized;

    throw CanteraError("GasKinetics::operator=()",
		       "Unfinished implementation");

    return *this;

   }
  //====================================================================================================================
  Kinetics *AqueousKinetics::duplMyselfAsKinetics(const std::vector<thermo_t*> & tpVector) const
  {
    AqueousKinetics* gK = new AqueousKinetics(*this);
    gK->assignShallowPointers(tpVector);
    return dynamic_cast<Kinetics *>(gK);
  }
  
  //====================================================================================================================
  /**
   * Update temperature-dependent portions of reaction rates and
   * falloff functions.
   */
  void AqueousKinetics::
  update_T() {}

  void AqueousKinetics::
  update_C() {}

  void AqueousKinetics::_update_rates_T() {
    doublereal T = thermo().temperature();
    // m_kdata->m_logStandConc = log(thermo().standardConcentration()); 
    doublereal logT = log(T);
    m_rates.update(T, logT, &m_kdata->m_rfn[0]);
 

    m_kdata->m_temp = T;
    updateKc();
    m_kdata->m_ROP_ok = false;
   
  };


  /**
   * Update properties that depend on concentrations. Currently only
   * the enhanced collision partner concentrations are updated here.
   */         
  void AqueousKinetics::
  _update_rates_C() {
    thermo().getActivityConcentrations(&m_conc[0]);
   
    m_kdata->m_ROP_ok = false;
  }

  /**
   * Update the equilibrium constants in molar units.
   */
  void AqueousKinetics::updateKc() {
    int i, irxn;
    vector_fp& m_rkc = m_kdata->m_rkcn;
    doublereal rt = GasConstant*   m_kdata->m_temp;
        
    thermo().getStandardChemPotentials(&m_grt[0]);
    fill(m_rkc.begin(), m_rkc.end(), 0.0);
    int nsp = thermo().nSpecies();
    for (int k = 0; k < nsp; k++) {
      doublereal logStandConc_k = thermo().logStandardConc(k);
      m_grt[k] -= rt * logStandConc_k;
    }

    // compute Delta G^0 for all reversible reactions
    m_rxnstoich->getRevReactionDelta(m_ii, &m_grt[0], &m_rkc[0]);
 
    //doublereal logStandConc = m_kdata->m_logStandConc;
    doublereal rrt = 1.0/(GasConstant * thermo().temperature());
    for (i = 0; i < m_nrev; i++) {
      irxn = m_revindex[i];
      m_rkc[irxn] = exp(m_rkc[irxn]*rrt);
    }

    for(i = 0; i != m_nirrev; ++i) {
      m_rkc[ m_irrev[i] ] = 0.0;
    }
  }

  /**
   * Get the equilibrium constants of all reactions, whether
   * reversible or not.
   */
  void AqueousKinetics::getEquilibriumConstants(doublereal* kc) {
    int i;
    _update_rates_T();
    vector_fp& rkc = m_kdata->m_rkcn;
  
    thermo().getStandardChemPotentials(&m_grt[0]);
    fill(rkc.begin(), rkc.end(), 0.0);
    doublereal rt = GasConstant * m_kdata->m_temp;
    int nsp = thermo().nSpecies();
    for (int k = 0; k < nsp; k++) {
      doublereal logStandConc_k = thermo().logStandardConc(k);
      m_grt[k] -= rt * logStandConc_k;
    }
    
    // compute Delta G^0 for all reactions
    m_rxnstoich->getReactionDelta(m_ii, &m_grt[0], &rkc[0]);
 
    doublereal rrt = 1.0/(GasConstant * thermo().temperature());
    for (i = 0; i < m_ii; i++) {
      kc[i] = exp(-rkc[i]*rrt);
    }

    // force an update of T-dependent properties, so that m_rkcn will
    // be updated before it is used next.
    m_kdata->m_temp = 0.0;
  }

  /**
   *
   * getDeltaGibbs():
   *
   * Return the vector of values for the reaction gibbs free energy
   * change
   * These values depend upon the concentration
   * of the ideal gas.
   *
   *  units = J kmol-1
   */
  void AqueousKinetics::getDeltaGibbs(doublereal* deltaG) {
    /*
     * Get the chemical potentials of the species in the 
     * ideal gas solution.
     */
    thermo().getChemPotentials(&m_grt[0]);
    /*
     * Use the stoichiometric manager to find deltaG for each
     * reaction.
     */
    m_rxnstoich->getReactionDelta(m_ii, &m_grt[0], deltaG);
  }
    
  /**
   *
   * getDeltaEnthalpy():
   * 
   * Return the vector of values for the reactions change in
   * enthalpy.
   * These values depend upon the concentration
   * of the solution.
   *
   *  units = J kmol-1
   */
  void AqueousKinetics::getDeltaEnthalpy(doublereal* deltaH) {
    /*
     * Get the partial molar enthalpy of all species in the 
     * ideal gas.
     */
    thermo().getPartialMolarEnthalpies(&m_grt[0]);
    /*
     * Use the stoichiometric manager to find deltaG for each
     * reaction.
     */
    m_rxnstoich->getReactionDelta(m_ii, &m_grt[0], deltaH);
  }

  /*
   *
   * getDeltaEntropy():
   *
   * Return the vector of values for the reactions change in
   * entropy.
   * These values depend upon the concentration
   * of the solution.
   *
   *  units = J kmol-1 Kelvin-1
   */
  void AqueousKinetics::getDeltaEntropy( doublereal* deltaS) {
    /*
     * Get the partial molar entropy of all species in the
     * solid solution.
     */
    thermo().getPartialMolarEntropies(&m_grt[0]);
    /*
     * Use the stoichiometric manager to find deltaS for each
     * reaction.
     */
    m_rxnstoich->getReactionDelta(m_ii, &m_grt[0], deltaS);
  }

  /**
   *
   * getDeltaSSGibbs():
   *
   * Return the vector of values for the reaction 
   * standard state gibbs free energy change.
   * These values don't depend upon the concentration
   * of the solution.
   *
   *  units = J kmol-1
   */
  void AqueousKinetics::getDeltaSSGibbs(doublereal* deltaG) {
    /*
     *  Get the standard state chemical potentials of the species.
     *  This is the array of chemical potentials at unit activity 
     *  We define these here as the chemical potentials of the pure
     *  species at the temperature and pressure of the solution.
     */
    thermo().getStandardChemPotentials(&m_grt[0]);
    /*
     * Use the stoichiometric manager to find deltaG for each
     * reaction.
     */
    m_rxnstoich->getReactionDelta(m_ii, &m_grt[0], deltaG);
  }

  /**
   *
   * getDeltaSSEnthalpy():
   *
   * Return the vector of values for the change in the
   * standard state enthalpies of reaction.
   * These values don't depend upon the concentration
   * of the solution.
   *
   *  units = J kmol-1
   */
  void AqueousKinetics::getDeltaSSEnthalpy(doublereal* deltaH) {
    /*
     *  Get the standard state enthalpies of the species.
     *  This is the array of chemical potentials at unit activity 
     *  We define these here as the enthalpies of the pure
     *  species at the temperature and pressure of the solution.
     */
    thermo().getEnthalpy_RT(&m_grt[0]);
    doublereal RT = thermo().temperature() * GasConstant;
    for (int k = 0; k < m_kk; k++) {
      m_grt[k] *= RT;
    }
    /*
     * Use the stoichiometric manager to find deltaG for each
     * reaction.
     */
    m_rxnstoich->getReactionDelta(m_ii, &m_grt[0], deltaH);
  }

  /*
   *
   * getDeltaSSEntropy():
   *
   * Return the vector of values for the change in the
   * standard state entropies for each reaction.
   * These values don't depend upon the concentration
   * of the solution.
   *
   *  units = J kmol-1 Kelvin-1
   */
  void AqueousKinetics::getDeltaSSEntropy(doublereal* deltaS) {
    /*
     *  Get the standard state entropy of the species.
     *  We define these here as the entropies of the pure
     *  species at the temperature and pressure of the solution.
     */
    thermo().getEntropy_R(&m_grt[0]);
    doublereal R = GasConstant;
    for (int k = 0; k < m_kk; k++) {
      m_grt[k] *= R;
    }
    /*
     * Use the stoichiometric manager to find deltaS for each
     * reaction.
     */
    m_rxnstoich->getReactionDelta(m_ii, &m_grt[0], deltaS);
  }



  void AqueousKinetics::updateROP() {

    _update_rates_T();
    _update_rates_C();

    if (m_kdata->m_ROP_ok) return;

    const vector_fp& rf = m_kdata->m_rfn;
    const vector_fp& m_rkc = m_kdata->m_rkcn;
    array_fp& ropf = m_kdata->m_ropf;
    array_fp& ropr = m_kdata->m_ropr;
    array_fp& ropnet = m_kdata->m_ropnet;

    // copy rate coefficients into ropf
    copy(rf.begin(), rf.end(), ropf.begin());

    
    // multiply by perturbation factor
    multiply_each(ropf.begin(), ropf.end(), m_perturb.begin());
           
    // copy the forward rates to the reverse rates                
    copy(ropf.begin(), ropf.end(), ropr.begin());
        
    // for reverse rates computed from thermochemistry, multiply
    // the forward rates copied into m_ropr by the reciprocals of
    // the equilibrium constants
    multiply_each(ropr.begin(), ropr.end(), m_rkc.begin());

    // multiply ropf by concentration products
    m_rxnstoich->multiplyReactants(&m_conc[0], &ropf[0]); 
    //m_reactantStoich.multiply(m_conc.begin(), ropf.begin()); 

    // for reversible reactions, multiply ropr by concentration
    // products
    m_rxnstoich->multiplyRevProducts(&m_conc[0], &ropr[0]); 
    //m_revProductStoich.multiply(m_conc.begin(), ropr.begin());

    for (int j = 0; j != m_ii; ++j) {
      ropnet[j] = ropf[j] - ropr[j];
    }

    m_kdata->m_ROP_ok = true;
  }

  /**
   *
   * getFwdRateConstants():
   *
   * Update the rate of progress for the reactions.
   * This key routine makes sure that the rate of progress vectors
   * located in the solid kinetics data class are up to date.
   */
  void AqueousKinetics::
  getFwdRateConstants(doublereal *kfwd) {
    _update_rates_T();
    _update_rates_C();

    // copy rate coefficients into ropf
    const vector_fp& rf = m_kdata->m_rfn;
    array_fp& ropf = m_kdata->m_ropf;
    copy(rf.begin(), rf.end(), ropf.begin());

   

    // multiply by perturbation factor
    multiply_each(ropf.begin(), ropf.end(), m_perturb.begin());
       
    for (int i = 0; i < m_ii; i++) {
      kfwd[i] = ropf[i];
    }
  }

  /**
   *
   * getRevRateConstants():
   *
   * Return a vector of the reverse reaction rate constants
   *
   * Length is the number of reactions. units depends
   * on many issues. Note, this routine will return rate constants
   * for irreversible reactions if the default for
   * doIrreversible is overridden.
   */
  void AqueousKinetics::
  getRevRateConstants(doublereal *krev, bool doIrreversible) {
    /*
     * go get the forward rate constants. -> note, we don't
     * really care about speed or redundancy in these
     * informational routines.
     */
    getFwdRateConstants(krev);

    if (doIrreversible) {
      doublereal *tmpKc = &m_kdata->m_ropnet[0];
      getEquilibriumConstants(tmpKc);
      for (int i = 0; i < m_ii; i++) {
	krev[i] /=  tmpKc[i];
      }
    } else {
      /*
       * m_rkc[] is zero for irreversibly reactions
       */
      const vector_fp& m_rkc = m_kdata->m_rkcn;
      for (int i = 0; i < m_ii; i++) {
	krev[i] *= m_rkc[i];
      }
    }
  }

  void AqueousKinetics::addReaction(const ReactionData& r) {

    if (r.reactionType == ELEMENTARY_RXN)      addElementaryReaction(r);
 
    // operations common to all reaction types
    installReagents( r );
    installGroups(reactionNumber(), r.rgroups, r.pgroups);
    incrementRxnCount();
    m_rxneqn.push_back(r.equation);
  }




  void AqueousKinetics::addElementaryReaction(const ReactionData& r) {
    int iloc;

    // install rate coeff calculator
    iloc = m_rates.install( reactionNumber(),
			    r.rateCoeffType, r.rateCoeffParameters.size(), 
			    DATA_PTR(r.rateCoeffParameters) );

    // add constant term to rate coeff value vector
    m_kdata->m_rfn.push_back(r.rateCoeffParameters[0]);                

    // forward rxn order equals number of reactants
    m_fwdOrder.push_back(r.reactants.size());
    registerReaction( reactionNumber(), ELEMENTARY_RXN, iloc);
  }


  
        
  void AqueousKinetics::installReagents(const ReactionData& r) {
            
    m_kdata->m_ropf.push_back(0.0);     // extend by one for new rxn
    m_kdata->m_ropr.push_back(0.0);
    m_kdata->m_ropnet.push_back(0.0);
    int n, ns, m;
    doublereal nsFlt;
    doublereal reactantGlobalOrder = 0.0;
    doublereal productGlobalOrder  = 0.0;
    int rnum = reactionNumber();

    vector_int rk;
    int nr = r.reactants.size();
    for (n = 0; n < nr; n++) {
      nsFlt = r.rstoich[n];
      reactantGlobalOrder += nsFlt;
      ns = (int) nsFlt;
      if ((doublereal) ns != nsFlt) {
	if (ns < 1) {
	  ns = 1;
	}
      }
      if (r.rstoich[n] != 0.0) 
	m_rrxn[r.reactants[n]][rnum] += r.rstoich[n];
      for (m = 0; m < ns; m++) {
	rk.push_back(r.reactants[n]);
      }
    }
    m_reactants.push_back(rk);

    vector_int pk;
    int np = r.products.size();
    for (n = 0; n < np; n++) {
      nsFlt = r.pstoich[n];
      productGlobalOrder += nsFlt;
      ns = (int) nsFlt;
      if ((double) ns != nsFlt) {
	if (ns < 1) {
	  ns = 1;
	}
      }
      if (r.pstoich[n] != 0.0) 
	m_prxn[r.products[n]][rnum] += r.pstoich[n];
      for (m = 0; m < ns; m++) {
	pk.push_back(r.products[n]);
      }
    }
    m_products.push_back(pk);

    m_kdata->m_rkcn.push_back(0.0);

    m_rxnstoich->add(reactionNumber(), r);

    if (r.reversible) {
      m_dn.push_back(productGlobalOrder - reactantGlobalOrder);
      m_revindex.push_back(reactionNumber());
      m_nrev++;
    }
    else {
      m_dn.push_back(productGlobalOrder - reactantGlobalOrder);
      m_irrev.push_back( reactionNumber() );
      m_nirrev++;
    }        
  }


  void AqueousKinetics::installGroups(int irxn, 
				      const vector<grouplist_t>& r, 
				      const vector<grouplist_t>& p) {
    if (!r.empty()) {
      writelog("installing groups for reaction "+int2str(reactionNumber()));
      m_rgroups[reactionNumber()] = r;
      m_pgroups[reactionNumber()] = p;
    }
  }


  void AqueousKinetics::init() { 
    m_kk = thermo().nSpecies();
    m_rrxn.resize(m_kk);
    m_prxn.resize(m_kk);
    m_conc.resize(m_kk);
    m_grt.resize(m_kk);
    m_kdata->m_logp_ref = log(thermo().refPressure()) - log(GasConstant);
  }

  void AqueousKinetics::finalize() {
    if (!m_finalized) {
      m_finalized = true;
    }
  }

  bool AqueousKinetics::ready() const {
    return (m_finalized);
  }

}
