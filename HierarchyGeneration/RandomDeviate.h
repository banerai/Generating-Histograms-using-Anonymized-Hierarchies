/****************************************************************************
File:        RandomDeviate.h
Description: Include file for a better rand generator.
             Followed by generation of some random deviates.
Created:     12/31/2017
****************************************************************************/

#ifndef RANDOM_DEVIATE_H
#define RANDOM_DEVIATE_H

#define SHUFFLE_ARRAYSIZE 257
/***************************************************************************
CRandomDeviate
    Returns a uniformly distributed random integer
    in the range 0 to msc_iMaxRand = 0xffffffff 
****************************************************************************/
class CRandomDeviate
{
public:
    CRandomDeviate();

    // Get Function
    unsigned int Get();

    static const unsigned int msc_iMaxRand;

private:
    unsigned int m_ShuffleArray[SHUFFLE_ARRAYSIZE];
};

/***************************************************************************
CNormalDeviate
    Returns a Gaussian distributed random number
    with zero mean and unit sigma.
****************************************************************************/
class CNormalDeviate : public CRandomDeviate
{
public:
    CNormalDeviate()
        : m_bToggle(false)
        , m_dSaved(0)
        , CRandomDeviate()
    {};

    // Get Function
    double Get();

private:
    bool           m_bToggle;
    double         m_dSaved;
};

/***************************************************************************
CUniformDeviate
    Returns a uniformly distributed random number
    between zero and one. 
****************************************************************************/
class CUniformDeviate : public CRandomDeviate
{
public:
    CUniformDeviate()
        : CRandomDeviate()
    {};

    // Get Function
    double Get();
};

/***************************************************************************
CPoissonDeviate
    Returns a Poisson distributed random number
****************************************************************************/
class CPoissonDeviate : public CRandomDeviate
{
public:
    CPoissonDeviate()
        : CRandomDeviate()
    {};

    // Get Function: Lambda is the expected value
    unsigned int Get(const double dLambda);
};

/***************************************************************************
CRayleighDeviate
    Returns a Rayleigh distributed random number
    with unit mode
****************************************************************************/
class CRayleighDeviate : public CRandomDeviate
{
public:
    CRayleighDeviate()
        : CRandomDeviate()
    {};

    // Get Function
    double Get();
};

/***************************************************************************
CLaplaceDeviate
    Returns a Laplacian distributed random number
    with zero mean and unit epsilon.
****************************************************************************/
class CLaplaceDeviate : public CRandomDeviate
{
public:
    CLaplaceDeviate()
        : CRandomDeviate()
    {};

    // Get Function
    double Get();
};

#endif // RANDOM_DEVIATE_H


