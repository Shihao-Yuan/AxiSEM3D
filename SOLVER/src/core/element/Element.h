// Element.h
// created by Kuangdai on 27-Mar-2016 
// base class of AxiSEM3D spectral elements

#pragma once

class Point;
class Gradient;
class PRT;

#include "eigenc.h"
#include "eigenp.h"

class Element {
public:    
    Element(Gradient *grad, PRT *prt, const std::array<Point *, nPntElem> &points);
    virtual ~Element();
    
    // compute stiffness term
    virtual void computeStiff() const = 0;
    
    // measure cost
    virtual double measure(int count) const = 0;
    
    // test stiffness 
    virtual void test() const = 0;
    
    // compute Real displacement, used by receiver
    virtual void computeGroundMotion(Real phi, const RMatPP &weights, RRow3 &u_spz) const = 0; 
    
    // verbose
    virtual std::string verbose() const = 0;
    
    // get point ptr
    const Point *getPoint(int index) const {return mPoints[index];};
    
    // source 
    void addSourceTerm(const arPP_CMatX3 &source) const;
    
    // get nr 
    int getMaxNr() const {return mMaxNr;};
    
    // signature for cost measurement
    std::string costSignature() const;
    
    // axial
    bool axial() const;
    
    // form theta for TIso
    RDMatPP formThetaMat() const;
    
protected:
    int mMaxNu;
    int mMaxNr;
    std::array<Point *, nPntElem> mPoints;
    Gradient *mGradient;
    PRT *mPRT;
    
    // flags
    bool mHasPRT;
    
public:
    // domain tag, mainly for debug
    void setDomainTag(int tag) {mDomainTag = tag;};
    int getDomainTag() const {return mDomainTag;};
    
private:
    int mDomainTag;
};

