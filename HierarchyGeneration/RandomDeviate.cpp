/****************************************************************************
File:        RandomDeviate.cpp
Description: Implementation file for a better rand generator.
             Followed by generation of random deviates.
Created:     12/31/2017
****************************************************************************/

#include <cstdlib>
#include <cmath>
#include "RandomDeviate.h"

/***************************************************************************/
const unsigned int CRandomDeviate::msc_iMaxRand = 0xffffffff; // 2^32 - 1
/***************************************************************************/

CRandomDeviate::CRandomDeviate()
{
    unsigned int iRandomValue;
    for (int index = 0; index < SHUFFLE_ARRAYSIZE; index++)
    {
        iRandomValue = (rand() & 0x7fff) << 17;
        iRandomValue |= (rand() & 0x7fff) << 2;
        iRandomValue |= rand() & 3;
        m_ShuffleArray[index] = iRandomValue;
    }
}

unsigned int CRandomDeviate::Get()
{
    int index = rand() % SHUFFLE_ARRAYSIZE;
    unsigned int iReturnValue = m_ShuffleArray[index];
    unsigned int iRandomValue = (rand() & 0x7fff) << 17;
    iRandomValue |= (rand() & 0x7fff) << 2;
    iRandomValue |= rand() & 3;
    m_ShuffleArray[index] = iRandomValue;
    return iReturnValue;
}

/***************************************************************************/

// CNormalDeviate::Get Function: Uses Box-Muller Transformation
double CNormalDeviate::Get()
{
    double dX, dY, dSqrDist, dFactor;

    if (m_bToggle)
    {
        m_bToggle = false;
        return m_dSaved;
    }

    m_bToggle = true;

    for (;;)
    {
        dX = CRandomDeviate::Get();
        dX /= CRandomDeviate::msc_iMaxRand;
        dX = 2 * dX - 1;

        dY = CRandomDeviate::Get();
        dY /= CRandomDeviate::msc_iMaxRand;
        dY = 2 * dY - 1;

        dSqrDist = dX * dX + dY * dY;
        if ((dSqrDist > 0.0) && (dSqrDist < 1.0)) break;
    }

    dFactor = sqrt( -2 * log( dSqrDist ) / dSqrDist );

    m_dSaved = dFactor * dX;
    return (dFactor * dY);
}

/***************************************************************************/

// CUniformDeviate::Get Function
double CUniformDeviate::Get()
{
    double dValue = CRandomDeviate::Get();
    dValue /= CRandomDeviate::msc_iMaxRand;
    return dValue;
}

/***************************************************************************/

// CPoissonDeviate::Get Function: Lambda is the expected value
unsigned int CPoissonDeviate::Get(const double dLambda)
{
    const double dLogMaxRand = log((double) CRandomDeviate::msc_iMaxRand);
    unsigned int iRandomValue = CRandomDeviate::Get();
    double dSum = (iRandomValue > 0) ? dLogMaxRand - log((double) iRandomValue) : 0;
    unsigned int nCount = 0;
    while (dSum < dLambda)
    {
        nCount++;
        if ((iRandomValue = CRandomDeviate::Get()) > 0) dSum += dLogMaxRand - log((double) iRandomValue);
    }
    return nCount;
}

/***************************************************************************/

// CRayleighDeviate::Get Function
double CRayleighDeviate::Get()
{
    const double dLogMaxRand = log((double) CRandomDeviate::msc_iMaxRand);
    unsigned int iRandomValue;
    double dValue;
    for (;;)
    {
        if ((iRandomValue = CRandomDeviate::Get()) > 0)
        {
            dValue = sqrt(2.0 * (dLogMaxRand - log((double) iRandomValue)));
            break;
        }
    }
    return dValue;
}

/***************************************************************************/

// CLaplaceDeviate::Get Function
double CLaplaceDeviate::Get()
{
    unsigned int iRandomValue = CRandomDeviate::Get();
    if (iRandomValue == 0) iRandomValue++;
    else if (iRandomValue == CRandomDeviate::msc_iMaxRand) iRandomValue--;

    double dValue = iRandomValue;
    dValue /= CRandomDeviate::msc_iMaxRand;
    dValue -= 0.5;
    return (dValue < 0) ? log(1.0 + 2.0 * dValue) : -log(1.0 - 2.0 * dValue);
}

/***************************************************************************/


