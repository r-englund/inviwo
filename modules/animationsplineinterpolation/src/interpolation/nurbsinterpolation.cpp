#include "../../ext/tinynurbs/tinynurbs/tinynurbs.h"
#include "inviwo/animationsplineinterpolation/interpolation/nurbsinterpolation.h"
#include "glm/gtc/constants.hpp"
#include "Eigen/Dense"
#include <cmath>
#include <iostream>

using namespace Eigen;

///Based on The NURBS Book by Les Piegl and Wayne Tiller, Springer 1997, Chapter 9.2.1, Page 364

void InterpolateCurveGlobalNoDeriv(const std::vector<glm::vec3> &InPoints, tinynurbs::Curve<3, float> &ResCurve) {
    //Let us use the classic variables, so that our math conforms to the book
    const int n = (int)InPoints.size() - 1;
    const int DesiredDegree = 3; //We choose a piecewise cubic b-spline.

    //Lower the degree when you have less points.
    const int degree = (n < DesiredDegree) ? n : DesiredDegree;
    const int p = degree;

    //Set up the resulting curve
    ResCurve.degree = degree;

    //We get as many de Boor points as input points with this method
    ResCurve.control_points.resize(InPoints.size());

    //Curve Parameter u: chord length parameterization according to formulae 9.4 and 9.5
    // - get chord length of input points (interpreted as a polyline)
    float d(0);
    for(int i(1);i<=n;i++)
    {
        d += glm::length(InPoints[i] - InPoints[i-1]);
    }
    std::cout << "Chord Length is " << d << std::endl;
    // - get parameters defining where we find our input points along the new interpolated curve
    std::vector<float> CurveParameter(n+1);
    CurveParameter[0] = 0;
    CurveParameter[n] = 1;
    for(int i(1);i<n;i++)
    {
        CurveParameter[i] = CurveParameter[i-1] + glm::length(InPoints[i] - InPoints[i-1]) / d;
    }
    // - shorthand
    const std::vector<float>& u = CurveParameter;

    //Knot Vector U using the method of averaging according to formula 9.8
    const int m = n+p+1;
    std::vector<float> Knots(m+1);
    for(int i(0);i<=p;i++)
    {
        Knots[i] = 0;
        Knots[m-i] = 1;
    }
    for(int j(1);j<=n-p;j++)
    {
        float& ThisKnot = Knots[j+p];
        ThisKnot = 0;

        for(int i(j);i<=j+p-1;i++)
        {
            ThisKnot += u[i];
        }

        ThisKnot /= p; //p == degree
    }
    // - shorthand
    const std::vector<float>& U = Knots;

    //Coefficient matrix with n+1 equations
    MatrixXf CoeffMat(n+1, n+1);
    CoeffMat.setZero();
    for(int i(0);i<=n;i++)
    {
        const int idxSpan = tinynurbs::findSpan<float>(degree, U, u[i]);
        auto Basis = tinynurbs::bsplineBasis<float>(degree, idxSpan, U, u[i]);
        for(int c(0);c<=degree;c++)
        {
            CoeffMat(i, idxSpan - degree + c) = Basis[c];
        }
    }

    std::cout << CoeffMat << std::endl;

    //Solve in each dimension
    VectorXf Solution[3];
    for(int d(0);d<3;d++)
    {
        //A vector with the input points' coordinates in that dimension
        VectorXf RightSide(n+1);
        for(int i(0);i<=n;i++)
        {
            RightSide(i) = InPoints[i][d];
        }

        Solution[d] = CoeffMat.colPivHouseholderQr().solve(RightSide);
        std::cout << Solution[d] << std::endl;
    }


    //Set up the actual curve
    ResCurve.knots = U;
    ResCurve.degree = degree;
    for(int i(0);i<=n;i++)
    {
        ResCurve.control_points[i].x = Solution[0][i];
        ResCurve.control_points[i].y = Solution[1][i];
        ResCurve.control_points[i].z = Solution[2][i];
    }

/*
    if (!tinynurbs::curveIsValid(ResCurve))
    {
        // check if degree, knots and control points are configured as per
        // #knots == #control points + degree + 1
        printf("Not Valid\n\t#knots = %zd  ==  #ctrlpts = %zd  +  degree = %d  +  1\n\n",
               ResCurve.knots.size(),
               ResCurve.control_points.size(),
               ResCurve.degree);
    }
    else
    {
        printf("Valid\n");
    }
    */
}


void InterpolateCurve(const std::vector<glm::vec3> &InPoints, tinynurbs::Curve<3, float> &ResCurve) {
    //Let us use the classic variables, so that our math conforms to the lecture slides
    const int n = (int)InPoints.size() - 1;
    const int degree = 3; //We choose a piecewise cubic b-spline
    const int k = degree + 1;
    const int order = k;

    //Set up the resulting curve
    ResCurve.degree = degree;

    //We get n+3 de Boor points when interpolating our input points
    // - actually, the 3 refers to the cubic degree
    ResCurve.control_points.resize(n + 3);

    //Let us assume a simple knot vector first, maybe something more special later.
    std::vector<float> InKnots(n+1);
    for(int i(0);i<(int)InKnots.size();i++)
    {
        InKnots[i] = (float)i;
    }
    // - shorthand
    const std::vector<float>& s = InKnots;

    //For our output knot vector, we choose an interpolating one
    {
        ResCurve.knots.resize(n+7);
        int i = 0;
        int j = 0;
        for(int i(0),j(0);i<ResCurve.knots.size();i++)
        {
            if (i >= degree + 1 && i <= ResCurve.knots.size() - degree - 1)
            {
                j++;
            }
            ResCurve.knots[i] = InKnots[j];
        }
    }
    // - shorthand
    const std::vector<float>& t = ResCurve.knots;

    //A matrix with n+3 equations
    MatrixXf TridiagonalMatrix(n+3, n+3);
    TridiagonalMatrix.setZero();
    // - first and last row
    TridiagonalMatrix(0, 0) = 1;
    TridiagonalMatrix(n+2, n+2) = 1;
    // - alpha_zero, beta_zero, gamma_zero
    TridiagonalMatrix(1, 0) = s[2] - s[0];
    TridiagonalMatrix(1, 1) = -(s[2] - s[0]) - (s[1] - s[0]);
    TridiagonalMatrix(1, 2) = s[1] - s[0];
    // - alpha_n, beta_n, gamma_n
    TridiagonalMatrix(n+1, n) = s[n] - s[n-1];
    TridiagonalMatrix(n+1, n+1) = -(s[n] - s[n-1]) - (s[n] - s[n-2]);
    TridiagonalMatrix(n+1, n+2) = s[n] - s[n-2];
    // - all rows inbetween for the alpha_i, beta_i, gamma_i from i = 1 .. n-1
    // - those are the rows r = 2 .. n
    for(int r(2),i(1);r<=n;r++,i++)
    {
        //CoeffMat(r, i) = tinynurbs::bsplineOneBasis<float>(i, degree, s, s[i]);
        //CoeffMat(r, i+1) = tinynurbs::bsplineOneBasis<float>(i+1, degree, s, s[i]);
        //CoeffMat(r, i+2) = tinynurbs::bsplineOneBasis<float>(i+2, degree, s, s[i]);
        TridiagonalMatrix(r, i  ) = tinynurbs::bsplineBasis<float>(degree, tinynurbs::findSpan<float>(degree, s, s[i]), s, s[i])[0];
        TridiagonalMatrix(r, i+1) = tinynurbs::bsplineBasis<float>(degree, tinynurbs::findSpan<float>(degree, s, s[i+1]), s, s[i+1])[0];
        if (i+2 < s.size())
        {
            TridiagonalMatrix(r, i+2) = tinynurbs::bsplineBasis<float>(degree, tinynurbs::findSpan<float>(degree, s, s[i+2]), s, s[i+2])[0];
        }
        else
        {
            TridiagonalMatrix(r, i+2) = 0;
        }
    }

    std::cout << TridiagonalMatrix << std::endl;

    //Compute proper control points that actually interpolate the given input points
    //Solve in each dimension
    VectorXf Solution[3];
    for(int d(0);d<3;d++)
    {
        //A vector with the input points and some extras
        VectorXf RightSide(n+3);
        //RightSide.ro
        RightSide(0) = InPoints[0][d];
        RightSide(1) = 0;
        RightSide(n+1) = 0;
        RightSide(n+2) = InPoints[n][d];
        for(int r(2),i(1);r<=n;r++,i++)
        {
            RightSide(r) = InPoints[i][d];
        }

        Solution[d] = TridiagonalMatrix.colPivHouseholderQr().solve(RightSide);
        std::cout << Solution[d] << std::endl;
    }

    for(int i(0);i<n+3;i++)
    {
        ResCurve.control_points[i].x = Solution[0][i];
        ResCurve.control_points[i].y = Solution[1][i];
        ResCurve.control_points[i].z = Solution[2][i];
    }

/*
    if (!tinynurbs::curveIsValid(ResCurve))
    {
        // check if degree, knots and control points are configured as per
        // #knots == #control points + degree + 1
        printf("Not Valid\n\t#knots = %zd  ==  #ctrlpts = %zd  +  degree = %d  +  1\n\n",
               ResCurve.knots.size(),
               ResCurve.control_points.size(),
               ResCurve.degree);
    }
    else
    {
        printf("Valid\n");
    }*/
}



void ApproximateCurve(const std::vector<glm::vec3> &InPoints, tinynurbs::Curve<3, float> &ResCurve) {
    ResCurve.control_points = InPoints;
    ResCurve.degree = 3;
    //ResCurve.knots = {0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 6, 6, 6};
    ResCurve.knots.resize(ResCurve.control_points.size() + ResCurve.degree + 1);
    int i = 0;
    int k = 0;
    for(int i(0),k(0);i<ResCurve.knots.size();i++)
    {
        if (i >= (int)ResCurve.degree + 1 && i <= (int)ResCurve.knots.size() - (int)ResCurve.degree - 1)
        {
            k++;
        }
        ResCurve.knots[i] = (float)k;
    }
/*
    if (!tinynurbs::curveIsValid(ResCurve))
    {
        // check if degree, knots and control points are configured as per
        // #knots == #control points + degree + 1
        printf("Not Valid\n\t#knots = %zd  ==  #ctrlpts = %zd  +  degree = %d  +  1\n\n",
               ResCurve.knots.size(),
               ResCurve.control_points.size(),
               ResCurve.degree);
    }
    else
    {
        printf("Valid\n");
    }
    */
}

//
//void SaveCurve(tinynurbs::Curve<3, float> &Curve, const char *FileName, const int NumPoints) {
//    FILE* f = fopen(FileName, "wt");
//    const float MinKnot = Curve.knots.front();
//    const float MaxKnot = Curve.knots.back();
//    for(int i=(0);i<NumPoints;i++)
//    {
//        const float t = float(i) / float(NumPoints - 1);
//        float InterpolKnot = (1-t) * MinKnot + t * MaxKnot;
//        if (InterpolKnot == MaxKnot) InterpolKnot -= 1e-5f; //tinynurbs is buggy and does not work with a knot value == MaxKnot
//        glm::vec3 P = tinynurbs::curvePoint(Curve, InterpolKnot);
//        fprintf(f, "v %g %g %g\n", P.x, P.y, P.z);
//    }
//
//    fprintf(f, "l");
//    for(int i=(0);i<NumPoints;i++)
//    {
//        fprintf(f, " %d", i+1);
//    }
//
//    fclose(f);
//}
//
//
//void SavePolyline(std::vector<glm::vec3> &Curve, const char *FileName) {
//    FILE* f = fopen(FileName, "wt");
//    for(int i=(0);i<Curve.size();i++)
//    {
//        fprintf(f, "v %g %g %g\n", Curve[i].x, Curve[i].y, Curve[i].z);
//    }
//
//    fprintf(f, "l");
//    for(int i=(0);i<Curve.size();i++)
//    {
//        fprintf(f, " %d", i+1);
//    }
//
//    fclose(f);
//}

