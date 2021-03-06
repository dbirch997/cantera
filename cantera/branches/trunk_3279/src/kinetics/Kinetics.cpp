/**
 *  @file Kinetics.cpp Declarations for the base class for kinetics managers
 *      (see \ref  kineticsmgr and class \link Cantera::Kinetics  Kinetics \endlink).
 *
 *  Kinetics managers calculate rates of progress of species due to
 *  homogeneous or heterogeneous kinetics.
 */
// Copyright 2001-2004  California Institute of Technology

#include "cantera/kinetics/Kinetics.h"
#include "cantera/kinetics/ReactionData.h"
#include "cantera/base/stringUtils.h"

using namespace std;

namespace Cantera
{
Kinetics::Kinetics() :
    m_ii(0),
    m_kk(0),
    m_thermo(0),
    m_surfphase(npos),
    m_rxnphase(npos),
    m_mindim(4)
{
}

Kinetics::~Kinetics() {}

Kinetics::Kinetics(const Kinetics& right)
{
    /*
     * Call the assignment operator
     */
    *this = right;
}

Kinetics& Kinetics::operator=(const Kinetics& right)
{
    /*
     * Check for self assignment.
     */
    if (this == &right) {
        return *this;
    }

    m_reactantStoich = right.m_reactantStoich;
    m_revProductStoich = right.m_revProductStoich;
    m_irrevProductStoich = right.m_irrevProductStoich;
    m_ii                = right.m_ii;
    m_kk                = right.m_kk;
    m_perturb           = right.m_perturb;
    m_reactants         = right.m_reactants;
    m_products          = right.m_products;
    m_rrxn = right.m_rrxn;
    m_prxn = right.m_prxn;
    m_rxntype = right.m_rxntype;

    m_thermo            = right.m_thermo; //  DANGER -> shallow pointer copy

    m_start             = right.m_start;
    m_phaseindex        = right.m_phaseindex;
    m_surfphase         = right.m_surfphase;
    m_rxnphase          = right.m_rxnphase;
    m_mindim            = right.m_mindim;
    m_rxneqn            = right.m_rxneqn;
    m_reactantStrings   = right.m_reactantStrings;
    m_productStrings    = right.m_productStrings;
    m_rgroups = right.m_rgroups;
    m_pgroups = right.m_pgroups;
    m_rfn = right.m_rfn;
    m_rkcn = right.m_rkcn;
    m_ropf = right.m_ropf;
    m_ropr = right.m_ropr;
    m_ropnet = right.m_ropnet;

    return *this;
}

Kinetics* Kinetics::duplMyselfAsKinetics(const std::vector<thermo_t*> & tpVector) const
{
    Kinetics* ko = new Kinetics(*this);

    ko->assignShallowPointers(tpVector);
    return ko;
}

int Kinetics::type() const
{
    return 0;
}

void Kinetics::checkReactionIndex(size_t i) const
{
    if (i >= m_ii) {
        throw IndexError("checkReactionIndex", "reactions", i, m_ii-1);
    }
}

void Kinetics::checkReactionArraySize(size_t ii) const
{
    if (m_ii > ii) {
        throw ArraySizeError("checkReactionArraySize", ii, m_ii);
    }
}

void Kinetics::checkPhaseIndex(size_t m) const
{
    if (m >= nPhases()) {
        throw IndexError("checkPhaseIndex", "phase", m, nPhases()-1);
    }
}

void Kinetics::checkPhaseArraySize(size_t mm) const
{
    if (nPhases() > mm) {
        throw ArraySizeError("checkPhaseArraySize", mm, nPhases());
    }
}

void Kinetics::checkSpeciesIndex(size_t k) const
{
    if (k >= m_kk) {
        throw IndexError("checkSpeciesIndex", "species", k, m_kk-1);
    }
}

void Kinetics::checkSpeciesArraySize(size_t kk) const
{
    if (m_kk > kk) {
        throw ArraySizeError("checkSpeciesArraySize", kk, m_kk);
    }
}

void Kinetics::assignShallowPointers(const std::vector<thermo_t*> & tpVector)
{
    size_t ns = tpVector.size();
    if (ns != m_thermo.size()) {
        throw CanteraError(" Kinetics::assignShallowPointers",
                           " Number of ThermoPhase objects arent't the same");
    }
    for (size_t i = 0; i < ns; i++) {
        ThermoPhase* ntp = tpVector[i];
        ThermoPhase* otp = m_thermo[i];
        if (ntp->id() != otp->id()) {
            throw CanteraError(" Kinetics::assignShallowPointers",
                               " id() of the ThermoPhase objects isn't the same");
        }
        if (ntp->eosType() != otp->eosType()) {
            throw CanteraError(" Kinetics::assignShallowPointers",
                               " eosType() of the ThermoPhase objects isn't the same");
        }
        if (ntp->nSpecies() != otp->nSpecies()) {
            throw CanteraError(" Kinetics::assignShallowPointers",
                               " Number of ThermoPhase objects isn't the same");
        }
        m_thermo[i] = tpVector[i];
    }


}

void Kinetics::selectPhase(const doublereal* data, const thermo_t* phase,
                           doublereal* phase_data)
{
    for (size_t n = 0; n < nPhases(); n++) {
        if (phase == m_thermo[n]) {
            size_t nsp = phase->nSpecies();
            copy(data + m_start[n],
                 data + m_start[n] + nsp, phase_data);
            return;
        }
    }
    throw CanteraError("Kinetics::selectPhase", "Phase not found.");
}

string Kinetics::kineticsSpeciesName(size_t k) const
{
    for (size_t n = m_start.size()-1; n != npos; n--) {
        if (k >= m_start[n]) {
            return thermo(n).speciesName(k - m_start[n]);
        }
    }
    return "<unknown>";
}

size_t Kinetics::kineticsSpeciesIndex(const std::string& nm) const
{
    for (size_t n = 0; n < m_thermo.size(); n++) {
        string id = thermo(n).id();
        // Check the ThermoPhase object for a match
        size_t k = thermo(n).speciesIndex(nm);
        if (k != npos) {
            return k + m_start[n];
        }
    }
    return npos;
}

size_t Kinetics::kineticsSpeciesIndex(const std::string& nm,
                                      const std::string& ph) const
{
    if (ph == "<any>") {
        return kineticsSpeciesIndex(nm);
    }

    for (size_t n = 0; n < m_thermo.size(); n++) {
        string id = thermo(n).id();
        if (ph == id) {
            size_t k = thermo(n).speciesIndex(nm);
            if (k == npos) {
                return npos;
            }
            return k + m_start[n];
        }
    }
    return npos;
}

thermo_t& Kinetics::speciesPhase(const std::string& nm)
{
    size_t np = m_thermo.size();
    size_t k;
    string id;
    for (size_t n = 0; n < np; n++) {
        k = thermo(n).speciesIndex(nm);
        if (k != npos) {
            return thermo(n);
        }
    }
    throw CanteraError("speciesPhase", "unknown species "+nm);
    return thermo(0);
}

size_t Kinetics::speciesPhaseIndex(size_t k)
{
    for (size_t n = m_start.size()-1; n != npos; n--) {
        if (k >= m_start[n]) {
            return n;
        }
    }
    throw CanteraError("speciesPhaseIndex", "illegal species index: "+int2str(k));
    return npos;
}

double Kinetics::reactantStoichCoeff(size_t kSpec, size_t irxn) const
{
    return getValue(m_rrxn[kSpec], irxn, 0.0);
}

double Kinetics::productStoichCoeff(size_t kSpec, size_t irxn) const
{
    return getValue(m_prxn[kSpec], irxn, 0.0);
}

void Kinetics::getFwdRatesOfProgress(doublereal* fwdROP)
{
    updateROP();
    std::copy(m_ropf.begin(), m_ropf.end(), fwdROP);
}

void Kinetics::getRevRatesOfProgress(doublereal* revROP)
{
    updateROP();
    std::copy(m_ropr.begin(), m_ropr.end(), revROP);
}

void Kinetics::getNetRatesOfProgress(doublereal* netROP)
{
    updateROP();
    std::copy(m_ropnet.begin(), m_ropnet.end(), netROP);
}

void Kinetics::getReactionDelta(const double* prop, double* deltaProp)
{
    fill(deltaProp, deltaProp + m_ii, 0.0);
    // products add
    m_revProductStoich.incrementReactions(prop, deltaProp);
    m_irrevProductStoich.incrementReactions(prop, deltaProp);
    // reactants subtract
    m_reactantStoich.decrementReactions(prop, deltaProp);
}

void Kinetics::getRevReactionDelta(const double* prop, double* deltaProp)
{
    fill(deltaProp, deltaProp + m_ii, 0.0);
    // products add
    m_revProductStoich.incrementReactions(prop, deltaProp);
    // reactants subtract
    // This is broken for irreversible reactions, because it will write into the index.
    m_reactantStoich.decrementReactions(prop, deltaProp);
}

void Kinetics::getCreationRates(double* cdot)
{
    updateROP();

    // zero out the output array
    fill(cdot, cdot + m_kk, 0.0);

    // the forward direction creates product species
    m_revProductStoich.incrementSpecies(&m_ropf[0], cdot);
    m_irrevProductStoich.incrementSpecies(&m_ropf[0], cdot);

    // the reverse direction creates reactant species
    m_reactantStoich.incrementSpecies(&m_ropr[0], cdot);
}

void Kinetics::getDestructionRates(doublereal* ddot)
{
    updateROP();

    fill(ddot, ddot + m_kk, 0.0);
    // the reverse direction destroys products in reversible reactions
    m_revProductStoich.incrementSpecies(&m_ropr[0], ddot);
    // the forward direction destroys reactants
    m_reactantStoich.incrementSpecies(&m_ropf[0], ddot);
}

void Kinetics::getNetProductionRates(doublereal* net)
{
    updateROP();

    fill(net, net + m_kk, 0.0);
    // products are created for positive net rate of progress
    m_revProductStoich.incrementSpecies(&m_ropnet[0], net);
    m_irrevProductStoich.incrementSpecies(&m_ropnet[0], net);
    // reactants are destroyed for positive net rate of progress
    m_reactantStoich.decrementSpecies(&m_ropnet[0], net);
}

void Kinetics::addPhase(thermo_t& thermo)
{
    // if not the first thermo object, set the start position
    // to that of the last object added + the number of its species
    if (m_thermo.size() > 0) {
        m_start.push_back(m_start.back()
                          + m_thermo.back()->nSpecies());
    }
    // otherwise start at 0
    else {
        m_start.push_back(0);
    }

    // the phase with lowest dimensionality is assumed to be the
    // phase/interface at which reactions take place
    if (thermo.nDim() <= m_mindim) {
        m_mindim = thermo.nDim();
        m_rxnphase = nPhases();
    }

    // there should only be one surface phase
    int ptype = -100;
    if (type() == cEdgeKinetics) {
        ptype = cEdge;
    } else if (type() == cInterfaceKinetics) {
        ptype = cSurf;
    }
    if (thermo.eosType() == ptype) {
        m_surfphase = nPhases();
        m_rxnphase = nPhases();
    }
    m_thermo.push_back(&thermo);
    m_phaseindex[m_thermo.back()->id()] = nPhases();
}

void Kinetics::finalize()
{
    m_kk = 0;
    for (size_t n = 0; n < nPhases(); n++) {
        size_t nsp = m_thermo[n]->nSpecies();
        m_kk += nsp;
    }
}

void Kinetics::addReaction(ReactionData& r) {
    //
    // vectors rk and pk are lists of species numbers, with repeated entries
    // for species with stoichiometric coefficients > 1. This allows the
    // reaction to be defined with unity reaction order for each reactant, and
    // so the faster method 'multiply' can be used to compute the rate of
    // progress instead of 'power'.
    //
    std::vector<size_t> rk;
    for (size_t n = 0; n < r.reactants.size(); n++) {
        double nsFlt = r.rstoich[n];
        size_t ns = (size_t) nsFlt;
        // If we are dealing with a noninteger stoichiometry make sure that ns is at least 1
        if ((double) ns != nsFlt) {
            ns = std::max<size_t>(ns, 1);
        }
        size_t kKin = r.reactants[n]; 
        if (r.rstoich[n] != 0.0) {
            m_rrxn[kKin][m_ii] += r.rstoich[n];
        }
        for (size_t m = 0; m < ns; m++) {
            rk.push_back(kKin);
        }
    }
    //  
    //  Push back the rk vector so that m_reactants has an entry for each reaction, m_ii
    //
    m_reactants.push_back(rk);

    //
    //   Do the same for the product side
    //
    std::vector<size_t> pk;
    for (size_t n = 0; n < r.products.size(); n++) {
        double nsFlt = r.pstoich[n];
        size_t ns = (size_t) nsFlt;
        if ((double) ns != nsFlt) {
            ns = std::max<size_t>(ns, 1);
        }
        if (r.pstoich[n] != 0.0) {
            m_prxn[r.products[n]][m_ii] += r.pstoich[n];
        }
        for (size_t m = 0; m < ns; m++) {
            pk.push_back(r.products[n]);
        }
    }
    m_products.push_back(pk);

    std::vector<size_t> extReactants = r.reactants;
    vector_fp extRStoich = r.rstoich;
    vector_fp extROrder = r.rorder;

    // If the reaction order involves reactant species with zero stoichiometries, add extra terms to
    // the reactants with zero stoichiometry so that the stoichiometry manager
    // can be used to compute the global forward reaction rate.
    //
    if (r.forwardFullOrder_.size() > 0) {
        size_t nsp = r.forwardFullOrder_.size();

        // Set up a signal vector to indicate whether the species has been added
        // into the input vectors for the stoich manager
        //
        vector_int kHandled(nsp, 0);
        //
        // Loop over the list of reactants which are also nonzero stoichioemtric entries
        // making sure the forwardFullOrder_ entries take precedence over rorder entries
        //
        for (size_t kk = 0; kk < r.reactants.size(); kk++) {
            size_t k = r.reactants[kk];
            double oo = r.rorder[kk];
            double of = r.forwardFullOrder_[k];
            if (of != oo) {
                extROrder[kk] = of;
            }
            kHandled[k] = 1;
        }
        for (size_t k = 0; k < nsp; k++) {
            double of = r.forwardFullOrder_[k];
            if (of != 0.0) {
                if (kHandled[k] == 0) {
                    //
                    // Add extra entries to reactant inputs. Set their reactant stoichiometric entries to zero.
                    //
                    extReactants.push_back(k);
                    extROrder.push_back(of);
                    extRStoich.push_back(0.0);
                }
            }
        }
    }


    size_t irxn = nReactions();
    
    m_reactantStoich.add(irxn, extReactants, extROrder, extRStoich);
    if (r.reversible) {
        m_revProductStoich.add(irxn, r.products, r.porder, r.pstoich);
    } else {
        m_irrevProductStoich.add(irxn, r.products, r.porder, r.pstoich);
    }

    installGroups(nReactions(), r.rgroups, r.pgroups);
    /*
     * Increment the reaction number, m_ii
     */
    incrementRxnCount();
    //
    //  Store a string representation of the reaction
    //
    m_rxneqn.push_back(r.equation);
    m_reactantStrings.push_back(r.reactantString);
    m_productStrings.push_back(r.productString);
    m_rxntype.push_back(r.reactionType);
    /*
     * Extend the reaction rate constant vectors and rates of progress vectors by one, as we are
     * adding a new reaction. 
     */
    m_rfn.push_back(0.0);
    m_rkcn.push_back(0.0);
    m_ropf.push_back(0.0);
    m_ropr.push_back(0.0);
    m_ropnet.push_back(0.0);
}
//==================================================================================================================
void Kinetics::installGroups(size_t irxn, const vector<grouplist_t>& r,
                             const vector<grouplist_t>& p)
{
    if (!r.empty()) {
        writelog("installing groups for reaction "+int2str(irxn));
        m_rgroups[irxn] = r;
        m_pgroups[irxn] = p;
    }
}
//===================================================================================================================
}
