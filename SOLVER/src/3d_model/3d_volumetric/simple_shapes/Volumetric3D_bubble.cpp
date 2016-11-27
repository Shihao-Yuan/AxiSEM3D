// Volumetric3D_bubble.cpp
// created by Kuangdai on 19-Oct-2016 
// a bubble-shaped heterogeneity

#include "Volumetric3D_bubble.h"
#include "XMath.h"
#include <sstream>

void Volumetric3D_bubble::initialize(const std::vector<double> &params) {
    // need at least 6 parameters to make a bubble
    if (params.size() < 7) throw std::runtime_error("Volumetric3D_bubble::initialize || "
        "Not enough parameters to initialize a Volumetric3D_bubble object.");
    
    // initialize location    
    mDepth = params[0] * 1e3;
    mLat = params[1];
    mLon = params[2];
    
    // initialize Gaussian parameters
    mRadius = params[3] * 1e3;
    mHWHM = params[4] * 1e3;
    mMax = params[5];
    
    // initialize reference type
    if (params[6] < 0.5) 
        mReferenceType = ReferenceTypes::Absolute;
    else if (params[6] < 1.5) 
        mReferenceType = ReferenceTypes::Reference1D;
    else if (params[6] < 2.5)
        mReferenceType = ReferenceTypes::ReferenceDiff;
    else 
        mReferenceType = ReferenceTypes::Reference3D;
        
    try {
        int ipar = 7;
        mChangeVp = (params.at(ipar++) > tinyDouble);
        mChangeVs = (params.at(ipar++) > tinyDouble);
        mChangeRho = (params.at(ipar++) > tinyDouble);
    } catch (std::out_of_range) {
        // nothing
    }    
}

bool Volumetric3D_bubble::get3dProperties(double r, double theta, double phi, double rElemCenter,
    double &dvpv, double &dvph, double &dvsv, double &dvsh, double &drho) const {
    
    // find the distance between bubble center and target point
    RDCol3 rtpBubble, rtpTarget;
    rtpBubble(0) = XMath::getROuter() - mDepth;
    rtpBubble(1) = XMath::lat2Theta(mLat, mDepth);
    rtpBubble(2) = XMath::lon2Phi(mLon);
    const RDCol3 &xyzBubble = XMath::toCartesian(rtpBubble);
    rtpTarget(0) = r;
    rtpTarget(1) = theta;
    rtpTarget(2) = phi;
    const RDCol3 &xyzTarget = XMath::toCartesian(rtpTarget);
    double distance = (xyzBubble - xyzTarget).norm();
    
    // zero results
    dvpv = dvph = dvsv = dvsh = drho = 0.;
    
    // treat as center if inside bubble
    distance -= mRadius; 
    if (distance < 0.) distance = 0.;
    
    // outside range
    if (distance > 4. * mHWHM) return false;
    
    // compute Gaussian
    double stddev = mHWHM / sqrt(2. * log(2.));
    double gaussian = mMax * exp(-distance * distance / (stddev * stddev * 2.));
    
    // set perturbations    
    if (mChangeVp) dvpv = dvph = gaussian;
    if (mChangeVs) dvsv = dvsh = gaussian;
    if (mChangeRho) drho = gaussian;
    
    return true;
}

std::string Volumetric3D_bubble::verbose() const {
    std::stringstream ss;
    ss << "\n======================= 3D Volumetric ======================" << std::endl;
    ss << "  Model Name          =   bubble" << std::endl;
    ss << "  Depth / km          =   " << mDepth / 1e3 << std::endl;
    ss << "  Lat / degree        =   " << mLat << std::endl;
    ss << "  Lon / degree        =   " << mLon << std::endl;
    ss << "  Bubble Radius / km  =   " << mRadius / 1e3 << std::endl;
    ss << "  HWHM / km           =   " << mHWHM / 1e3 << std::endl;
    ss << "  Maximum at Center   =   " << mMax << std::endl;
    ss << "  Reference Type      =   " << ReferenceTypesString[mReferenceType] << std::endl;
    ss << "  Affect VP           =   " << (mChangeVp ? "YES" : "NO") << std::endl;
    ss << "  Affect VS           =   " << (mChangeVs ? "YES" : "NO") << std::endl;
    ss << "  Affect Density      =   " << (mChangeRho ? "YES" : "NO") << std::endl;
    ss << "======================= 3D Volumetric ======================\n" << std::endl;
    return ss.str();
}

