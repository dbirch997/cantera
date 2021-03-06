/**
 *  @file SpeciesThermoMgr.h
 *  This file contains descriptions of templated subclasses of
 *  the virtual base class, SpeciesThermo, which includes SpeciesThermoDuo
 *  (see \ref mgrsrefcalc and class
 *   \link Cantera::SpeciesThermoDuo SpeciesThermoDuo\endlink)
 */

// Copyright 2001  California Institute of Technology
#ifndef CT_SPECIESTHERMO_MGR_H
#define CT_SPECIESTHERMO_MGR_H

#include "cantera/base/ct_defs.h"
#include "cantera/base/ctexceptions.h"
#include "cantera/base/stringUtils.h"
#include "SpeciesThermo.h"
#include <map>

namespace Cantera {
///////////////////////  Exceptions //////////////////////////////

//! Unknown species thermo manager string error
/*!
 * @ingroup mgrsrefcalc
 */
class UnknownSpeciesThermo: public CanteraError
{
public:

    //! constructor
    /*!
     * @param proc   name of the procecdure
     * @param type   unknown type
     */
    UnknownSpeciesThermo(const std::string& proc, int type) :
            CanteraError(proc, "Specified species parameterization type (" + int2str(type) + ") does not match any known type.")
    {
    }

    //! Alternate constructor
    /*!
     * @param proc   name of the procecdure
     * @param stype   String name for the unknown type
     */
    UnknownSpeciesThermo(const std::string& proc, const std::string& stype) :
            CanteraError(proc, "Specified species parameterization type (" + stype + ") does not match any known type.")
    {
    }
    //! destructor
    virtual ~UnknownSpeciesThermo() throw ()
    {
    }
};

/**
 *  This species thermo manager requires that all species have one
 *  of two parameterizations.
 *
 * Note this seems to be a slow way to do things, and it may be on its way out.
 *
 * @ingroup mgrsrefcalc
 */
template<typename ValAndDerivType, template<typename > class T1, template<typename > class T2>
class SpeciesThermoDuo: public SpeciesThermo<ValAndDerivType>
{

public:
    //! Constructor
    SpeciesThermoDuo() :
            SpeciesThermo<ValAndDerivType>(),
            m_p0(OneAtm)
    {
    }

    //! Destructor
    virtual ~SpeciesThermoDuo()
    {
    }

    //! copy constructor
    /*!
     * @param right Object to be copied
     */
    template<typename ValAndDerivType2>
    SpeciesThermoDuo(const SpeciesThermoDuo<ValAndDerivType2, T1, T2>& right)
    {
        operator=(right);
    }

    //! Assignment operator
    /*!
     * @param right Object to be copied
     */
    template<typename ValAndDerivType2>
    SpeciesThermoDuo<ValAndDerivType, T1, T2>& operator=(const SpeciesThermoDuo<ValAndDerivType2, T1, T2>& right);

    //! Duplication routine for objects which inherit from
    //! %SpeciesThermo87

    /*!
     *  This virtual routine can be used to duplicate %SpeciesThermo  objects
     *  inherited from %SpeciesThermo even if the application only has
     *  a pointer to %SpeciesThermo to work with.
     */
    virtual SpeciesThermo<ValAndDerivType>* duplMyselfAsSpeciesThermo() const;

    //! Duplication routine for objects which inherit from
    //! %SpeciesThermo
    /*!
     *  This virtual routine can be used to duplicate %SpeciesThermo  objects
     *  inherited from %SpeciesThermo even if the application only has
     *  a pointer to %SpeciesThermo to work with.
     *  ->commented out because we first need to add copy constructors
     *   and assignment operators to all of the derived classes.
     */
    virtual SpeciesThermo<doublereal>* duplMyselfAsSpeciesThermoDouble() const;

    /**
     * install a new species thermodynamic property
     * parameterization for one species.
     *
     * @param name      Name of the species
     * @param sp        The 'update' method will update the property
     *                  values for this species
     *                  at position i index in the property arrays.
     * @param type      int flag specifying the type of parameterization to be
     *                 installed.
     * @param c        vector of coefficients for the parameterization.
     *                 This vector is simply passed through to the
     *                 parameterization constructor.
     * @param minTemp  minimum temperature for which this parameterization
     *                 is valid.
     * @param maxTemp  maximum temperature for which this parameterization
     *                 is valid.
     * @param refPressure standard-state pressure for this
     *                    parameterization.
     * @see speciesThermoTypes.h
     */
    virtual void install(const std::string& name, size_t sp, int type, const doublereal* c, doublereal minTemp, doublereal maxTemp,
                         doublereal refPressure);

    //! Install a new species thermodynamic property
    //! parameterization for one species.
    /*!
     * @param stit_ptr Pointer to the SpeciesThermoInterpType object
     *          This will set up the thermo for one species
     */
    virtual void install_STIT(SpeciesThermoInterpType<ValAndDerivType>* stit_ptr)
    {
        throw CanteraError("install_STIT", "not implemented");
    }

    //! Compute the reference-state properties for all species.
    /*!
     * Given temperature T in K, this method updates the values of
     * the non-dimensional heat capacity at constant pressure,
     * enthalpy, and entropy, at the reference pressure, Pref
     * of each of the standard states.
     *
     * @param t       Temperature (Kelvin)
     * @param cp_R    Vector of Dimensionless heat capacities.
     *                (length m_kk).85L85
     *
     * @param h_RT    Vector of Dimensionless enthalpies.
     *                (length m_kk).
     * @param s_R     Vector of Dimensionless entropies.
     *                (length m_kk).
     */
    virtual void update(doublereal t, ValAndDerivType* cp_R, ValAndDerivType* h_RT, ValAndDerivType* s_R) const;

    //! Minimum temperature.
    /*!
     * If no argument is supplied, this
     * method returns the minimum temperature for which \e all
     * parameterizations are valid. If an integer index k is
     * supplied, then the value returned is the minimum
     * temperature for species k in the phase.
     *
     * @param k    Species index
     */
    virtual doublereal minTemp(size_t k = npos) const
    {
        return std::max(m_thermo1.minTemp(), m_thermo2.minTemp());
    }

    //! Maximum temperature.
    /*!
     * If no argument is supplied, this
     * method returns the maximum temperature for which \e all
     * parameterizations are valid. If an integer index k is
     * supplied, then the value returned is the maximum
     * temperature for parameterization k.
     *
     * @param k index for parameterization k
     */
    virtual doublereal maxTemp(size_t k = npos) const
    {
        return std::min(m_thermo1.maxTemp(), m_thermo2.maxTemp());
    }

    /**
     * The reference-state pressure for species k.
     *
     * returns the reference state pressure in Pascals for
     * species k. If k is left out of the argument list,
     * it returns the reference state pressure for the first
     * species.
     * Note that some SpeciesThermo implementations, such
     * as those for ideal gases, require that all species
     * in the same phase have the same reference state pressures.
     *
     * @param k index for parameterization k
     */
    virtual doublereal refPressure(size_t k = npos) const
    {
        return m_p0;
    }

    //! This utility function reports the type of parameterization
    //! used for the species with index number index.
    /*!
     *
     * @param k  Species index
     */
    virtual int reportType(size_t k) const;

    /*!
     * This utility function reports back the type of
     * parameterization and all of the parameters for the
     * species, index.
     *
     * @param index     Species index
     * @param type      Integer type of the standard type
     * @param c         Vector of coefficients used to set the
     *                  parameters for the standard state.
     * @param minTemp   output - Minimum temperature
     * @param maxTemp   output - Maximum temperature
     * @param refPressure output - reference pressure (Pa).
     *
     */
    virtual void reportParams(size_t index, int& type, doublereal* const c, doublereal& minTemp, doublereal& maxTemp,
                              doublereal& refPressure) const;

#ifdef H298MODIFY_CAPABILITY

    virtual doublereal reportOneHf298(size_t k) const
    {
        throw CanteraError("reportHF298", "unimplemented");
    }

    virtual void modifyOneHf298(const size_t k, const doublereal Hf298New)
    {
        throw CanteraError("reportHF298", "unimplemented");
    }

#endif

private:

    //! Thermo Type 1
    T1<ValAndDerivType> m_thermo1;
    //! Thermo Type 2
    T2<ValAndDerivType> m_thermo2;
    //! Reference pressure
    doublereal m_p0;
    //! map from species to type
    std::map<size_t, int> speciesToType;

    friend class SpeciesThermoDuo<doubleFAD, T1, T2> ;
    friend class SpeciesThermoDuo<doublereal, T1, T2> ;
};

// ------------------------- cpp part of file -------------------------------------

// Definitions for the SpeciesThermoDuo<T1,T2> templated class

template<typename ValAndDerivType, template<typename > class T1, template<typename > class T2>
SpeciesThermo<ValAndDerivType> *
SpeciesThermoDuo<ValAndDerivType, T1, T2>::duplMyselfAsSpeciesThermo() const
{
    return new SpeciesThermoDuo<ValAndDerivType, T1, T2>(*this);
}

template<typename ValAndDerivType, template<typename > class T1, template<typename > class T2>
void SpeciesThermoDuo<ValAndDerivType, T1, T2>::install(const std::string& name, size_t sp, int type, const doublereal* c,
                                                        doublereal minTemp, doublereal maxTemp, doublereal refPressure)
{
    m_p0 = refPressure;
    if (type == m_thermo1.ID) {
        m_thermo1.install(name, sp, 0, c, minTemp, maxTemp, refPressure);
        speciesToType[sp] = m_thermo1.ID;
    } else if (type == m_thermo2.ID) {
        m_thermo2.install(name, sp, 0, c, minTemp, maxTemp, refPressure);
        speciesToType[sp] = m_thermo2.ID;
    } else {
        throw UnknownSpeciesThermo("SpeciesThermoDuo:install", type);
    }
}

template<typename ValAndDerivType, template<typename > class T1, template<typename > class T2>
void SpeciesThermoDuo<ValAndDerivType, T1, T2>::update(doublereal t, ValAndDerivType* cp_R, ValAndDerivType* h_RT,
                                                       ValAndDerivType* s_R) const
{
    m_thermo1.update(t, cp_R, h_RT, s_R);
    m_thermo2.update(t, cp_R, h_RT, s_R);
}

template<typename ValAndDerivType, template<typename > class T1, template<typename > class T2>
int SpeciesThermoDuo<ValAndDerivType, T1, T2>::reportType(size_t k) const
{
    std::map<size_t, int>::const_iterator p = speciesToType.find(k);
    if (p != speciesToType.end()) {
        return p->second;
    }
    return -1;
}

template<typename ValAndDerivType, template<typename > class T1, template<typename > class T2>
void SpeciesThermoDuo<ValAndDerivType, T1, T2>::reportParams(size_t index, int& type, doublereal* const c, doublereal& minTemp,
                                                             doublereal& maxTemp, doublereal& refPressure) const
{
    int ctype = reportType(index);
    if (ctype == m_thermo1.ID) {
        m_thermo1.reportParams(index, type, c, minTemp, maxTemp, refPressure);
    } else if (ctype == m_thermo2.ID) {
        m_thermo2.reportParams(index, type, c, minTemp, maxTemp, refPressure);
    } else {
        throw CanteraError("  ", "confused");
    }
}

}
#endif
