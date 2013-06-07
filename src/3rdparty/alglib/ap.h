/********************************************************************
AP Library version 1.3

Copyright (c) 2003-2007, Sergey Bochkanov (ALGLIB project).
See www.alglib.net or alglib.sources.ru for details.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef AP_H
#define AP_H

#include <stdlib.h>
#include <string>
#include <math.h>

#ifdef __BORLANDC__
#include <list.h>
#include <vector.h>
#else
#include <list>
#include <vector>
#endif

/********************************************************************
Array bounds check
********************************************************************/
#define AP_ASSERT

#ifndef AP_ASSERT     //
#define NO_AP_ASSERT  // This code avoids definition of the
#endif                // both AP_ASSERT and NO_AP_ASSERT symbols
#ifdef NO_AP_ASSERT   //
#ifdef AP_ASSERT      //
#undef NO_AP_ASSERT   //
#endif                //
#endif                //


/********************************************************************
Current environment.
********************************************************************/
#ifndef AP_WIN32
#ifndef AP_UNKNOWN
#define AP_UNKNOWN
#endif
#endif
#ifdef AP_WIN32
#ifdef AP_UNKNOWN
#error Multiple environments are declared!
#endif
#endif

/********************************************************************
This symbol is used for debugging. Do not define it and do not remove
comments.
********************************************************************/
//#define UNSAFE_MEM_COPY


/********************************************************************
Namespace of a standard library AlgoPascal.
********************************************************************/
namespace ap
{

/********************************************************************
Service routines:
    amalloc - allocates an aligned block of size bytes
    afree - frees block allocated by amalloc
    vlen - just alias for n2-n1+1
********************************************************************/
void* amalloc(size_t size, size_t alignment);
void afree(void *block);
int vlen(int n1, int n2);

/********************************************************************
Exception class.
********************************************************************/
class ap_error
{
public:
    ap_error(){};
    ap_error(const char *s){ msg = s; };

    std::string msg;

    static void make_assertion(bool bClause)
        { if(!bClause) throw ap_error(); };
    static void make_assertion(bool bClause, const char *msg)
        { if(!bClause) throw ap_error(msg); };
private:
};

/********************************************************************
Class defining a complex number with double precision.
********************************************************************/
class complex;

class complex
{
public:
    complex():x(0.0),y(0.0){};
    complex(const double &_x):x(_x),y(0.0){};
    complex(const double &_x, const double &_y):x(_x),y(_y){};
    complex(const complex &z):x(z.x),y(z.y){};

    complex& operator= (const double& v){ x  = v; y = 0.0; return *this; };
    complex& operator+=(const double& v){ x += v;          return *this; };
    complex& operator-=(const double& v){ x -= v;          return *this; };
    complex& operator*=(const double& v){ x *= v; y *= v;  return *this; };
    complex& operator/=(const double& v){ x /= v; y /= v;  return *this; };

    complex& operator= (const complex& z){ x  = z.x; y  = z.y; return *this; };
    complex& operator+=(const complex& z){ x += z.x; y += z.y; return *this; };
    complex& operator-=(const complex& z){ x -= z.x; y -= z.y; return *this; };
    complex& operator*=(const complex& z){ double t = x*z.x-y*z.y; y = x*z.y+y*z.x; x = t; return *this; };
    complex& operator/=(const complex& z)
    {
        ap::complex result;
        double e;
        double f;
        if( fabs(z.y)<fabs(z.x) )
        {
            e = z.y/z.x;
            f = z.x+z.y*e;
            result.x = (z.x+z.y*e)/f;
            result.y = (z.y-z.x*e)/f;
        }
        else
        {
            e = z.x/z.y;
            f = z.y+z.x*e;
            result.x = (z.y+z.x*e)/f;
            result.y = (-z.x+z.y*e)/f;
        }
        *this = result;
        return *this;
    };

    double x, y;
};

const complex operator/(const complex& lhs, const complex& rhs);
const bool operator==(const complex& lhs, const complex& rhs);
const bool operator!=(const complex& lhs, const complex& rhs);
const complex operator+(const complex& lhs);
const complex operator-(const complex& lhs);
const complex operator+(const complex& lhs, const complex& rhs);
const complex operator+(const complex& lhs, const double& rhs);
const complex operator+(const double& lhs, const complex& rhs);
const complex operator-(const complex& lhs, const complex& rhs);
const complex operator-(const complex& lhs, const double& rhs);
const complex operator-(const double& lhs, const complex& rhs);
const complex operator*(const complex& lhs, const complex& rhs);
const complex operator*(const complex& lhs, const double& rhs);
const complex operator*(const double& lhs, const complex& rhs);
const complex operator/(const complex& lhs, const complex& rhs);
const complex operator/(const double& lhs, const complex& rhs);
const complex operator/(const complex& lhs, const double& rhs);
const double abscomplex(const complex &z);
const complex conj(const complex &z);
const complex csqr(const complex &z);


/********************************************************************
Templates for vector operations
********************************************************************/
#include "apvt.h"

/********************************************************************
BLAS functions
********************************************************************/
double vdotproduct(const double *v1, const double *v2, int N);
complex vdotproduct(const complex *v1, const complex *v2, int N);

void vmove(double *vdst, const double* vsrc, int N);
void vmove(complex *vdst, const complex* vsrc, int N);

void vmoveneg(double *vdst, const double *vsrc, int N);
void vmoveneg(complex *vdst, const complex *vsrc, int N);

void vmove(double *vdst, const double *vsrc, int N, double alpha);
void vmove(complex *vdst, const complex *vsrc, int N, double alpha);
void vmove(complex *vdst, const complex *vsrc, int N, complex alpha);

void vadd(double *vdst, const double *vsrc, int N);
void vadd(complex *vdst, const complex *vsrc, int N);

void vadd(double *vdst, const double *vsrc, int N, double alpha);
void vadd(complex *vdst, const complex *vsrc, int N, double alpha);
void vadd(complex *vdst, const complex *vsrc, int N, complex alpha);

void vsub(double *vdst, const double *vsrc, int N);
void vsub(complex *vdst, const complex *vsrc, int N);

void vsub(double *vdst, const double *vsrc, int N, double alpha);
void vsub(complex *vdst, const complex *vsrc, int N, double alpha);
void vsub(complex *vdst, const complex *vsrc, int N, complex alpha);

void vmul(double *vdst, int N, double alpha);
void vmul(complex *vdst, int N, double alpha);
void vmul(complex *vdst, int N, complex alpha);


/********************************************************************
Template of a dynamical one-dimensional array
********************************************************************/
template<class T, bool Aligned = false>
class template_1d_array
{
public:
    template_1d_array()
    {
        m_Vec=0;
        m_iVecSize = 0;
        m_iLow = 0;
        m_iHigh = -1;
    };

    ~template_1d_array()
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
    };

    template_1d_array(const template_1d_array &rhs)
    {
        m_Vec=0;
        m_iVecSize = 0;
        m_iLow = 0;
        m_iHigh = -1;
        if( rhs.m_iVecSize!=0 )
            setcontent(rhs.m_iLow, rhs.m_iHigh, rhs.getcontent());
    };


    const template_1d_array& operator=(const template_1d_array &rhs)
    {
        if( this==&rhs )
            return *this;

        if( rhs.m_iVecSize!=0 )
            setcontent(rhs.m_iLow, rhs.m_iHigh, rhs.getcontent());
        else
        {
            m_Vec=0;
            m_iVecSize = 0;
            m_iLow = 0;
            m_iHigh = -1;
        }
        return *this;
    };


    const T& operator()(int i) const
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i>=m_iLow && i<=m_iHigh);
        #endif
        return m_Vec[ i-m_iLow ];
    };


    T& operator()(int i)
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i>=m_iLow && i<=m_iHigh);
        #endif
        return m_Vec[ i-m_iLow ];
    };


    void setbounds( int iLow, int iHigh )
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
        m_iLow = iLow;
        m_iHigh = iHigh;
        m_iVecSize = iHigh-iLow+1;
        if( Aligned )
            m_Vec = (T*)ap::amalloc(m_iVecSize*sizeof(T), 16);
        else
            m_Vec = new T[m_iVecSize];
    };


    void setlength(int iLen)
    {
        setbounds(0, iLen-1);
    }


    void setcontent( int iLow, int iHigh, const T *pContent )
    {
        setbounds(iLow, iHigh);
        for(int i=0; i<m_iVecSize; i++)
            m_Vec[i] = pContent[i];
    };


    T* getcontent()
    {
        return m_Vec;
    };

    const T* getcontent() const
    {
        return m_Vec;
    };


    int getlowbound(int iBoundNum = 0) const
    {
        return m_iLow;
    };


    int gethighbound(int iBoundNum = 0) const
    {
        return m_iHigh;
    };

    raw_vector<T> getvector(int iStart, int iEnd)
    {
        if( iStart>iEnd || wrongIdx(iStart) || wrongIdx(iEnd) )
            return raw_vector<T>(0, 0, 1);
        else
            return raw_vector<T>(m_Vec+iStart-m_iLow, iEnd-iStart+1, 1);
    };


    const_raw_vector<T> getvector(int iStart, int iEnd) const
    {
        if( iStart>iEnd || wrongIdx(iStart) || wrongIdx(iEnd) )
            return const_raw_vector<T>(0, 0, 1);
        else
            return const_raw_vector<T>(m_Vec+iStart-m_iLow, iEnd-iStart+1, 1);
    };
private:
    bool wrongIdx(int i) const { return i<m_iLow || i>m_iHigh; };

    T         *m_Vec;
    long      m_iVecSize;
    long      m_iLow, m_iHigh;
};



/********************************************************************
Template of a dynamical two-dimensional array
********************************************************************/
template<class T, bool Aligned = false>
class template_2d_array
{
public:
    template_2d_array()
    {
        m_Vec=0;
        m_iVecSize=0;
        m_iLow1 = 0;
        m_iHigh1 = -1;
        m_iLow2 = 0;
        m_iHigh2 = -1;
    };

    ~template_2d_array()
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
    };

    template_2d_array(const template_2d_array &rhs)
    {
        m_Vec=0;
        m_iVecSize=0;
        m_iLow1 = 0;
        m_iHigh1 = -1;
        m_iLow2 = 0;
        m_iHigh2 = -1;
        if( rhs.m_iVecSize!=0 )
        {
            setbounds(rhs.m_iLow1, rhs.m_iHigh1, rhs.m_iLow2, rhs.m_iHigh2);
            for(int i=m_iLow1; i<=m_iHigh1; i++)
                for(int j=m_iLow2; j<=m_iHigh2; j++)
                    operator()(i,j) = rhs(i,j);
                //vmove(&(operator()(i,m_iLow2)), &(rhs(i,m_iLow2)), m_iHigh2-m_iLow2+1);
        }
    };
    const template_2d_array& operator=(const template_2d_array &rhs)
    {
        if( this==&rhs )
            return *this;

        if( rhs.m_iVecSize!=0 )
        {
            setbounds(rhs.m_iLow1, rhs.m_iHigh1, rhs.m_iLow2, rhs.m_iHigh2);
            for(int i=m_iLow1; i<=m_iHigh1; i++)
                for(int j=m_iLow2; j<=m_iHigh2; j++)
                    operator()(i,j) = rhs(i,j);
                //vmove(&(operator()(i,m_iLow2)), &(rhs(i,m_iLow2)), m_iHigh2-m_iLow2+1);
        }
        else
        {
            if(m_Vec)
            {
                if( Aligned )
                    ap::afree(m_Vec);
                else
                    delete[] m_Vec;
            }
            m_Vec=0;
            m_iVecSize=0;
            m_iLow1 = 0;
            m_iHigh1 = -1;
            m_iLow2 = 0;
            m_iHigh2 = -1;
        }
        return *this;
    };

    const T& operator()(int i1, int i2) const
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i1>=m_iLow1 && i1<=m_iHigh1);
        ap_error::make_assertion(i2>=m_iLow2 && i2<=m_iHigh2);
        #endif
        return m_Vec[ m_iConstOffset + i2 +i1*m_iLinearMember];
    };

    T& operator()(int i1, int i2)
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i1>=m_iLow1 && i1<=m_iHigh1);
        ap_error::make_assertion(i2>=m_iLow2 && i2<=m_iHigh2);
        #endif
        return m_Vec[ m_iConstOffset + i2 +i1*m_iLinearMember];
    };

    void setbounds( int iLow1, int iHigh1, int iLow2, int iHigh2 )
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
        int n1 = iHigh1-iLow1+1;
        int n2 = iHigh2-iLow2+1;
        m_iVecSize = n1*n2;
        if( Aligned )
        {
            //if( n2%2!=0 )
            while( (n2*sizeof(T))%16!=0 )
            {
                n2++;
                m_iVecSize += n1;
            }
            m_Vec = (T*)ap::amalloc(m_iVecSize*sizeof(T), 16);
        }
        else
            m_Vec = new T[m_iVecSize];
        m_iLow1  = iLow1;
        m_iHigh1 = iHigh1;
        m_iLow2  = iLow2;
        m_iHigh2 = iHigh2;
        m_iConstOffset = -m_iLow2-m_iLow1*n2;
        m_iLinearMember = n2;
    };

    void setlength(int iLen1, int iLen2)
    {
        setbounds(0, iLen1-1, 0, iLen2-1);
    }

    void setcontent( int iLow1, int iHigh1, int iLow2, int iHigh2, const T *pContent )
    {
        setbounds(iLow1, iHigh1, iLow2, iHigh2);
        for(int i=m_iLow1; i<=m_iHigh1; i++, pContent += m_iHigh2-m_iLow2+1)
            for(int j=m_iLow2; j<=m_iHigh2; j++)
                operator()(i,j) = pContent[j-m_iLow2];
            //vmove(&(operator()(i,m_iLow2)), pContent, m_iHigh2-m_iLow2+1);
    };

    int getlowbound(int iBoundNum) const
    {
        return iBoundNum==1 ? m_iLow1 : m_iLow2;
    };

    int gethighbound(int iBoundNum) const
    {
        return iBoundNum==1 ? m_iHigh1 : m_iHigh2;
    };

    raw_vector<T> getcolumn(int iColumn, int iRowStart, int iRowEnd)
    {
        if( (iRowStart>iRowEnd) || wrongColumn(iColumn) || wrongRow(iRowStart) ||wrongRow(iRowEnd) )
            return raw_vector<T>(0, 0, 1);
        else
            return raw_vector<T>(&((*this)(iRowStart, iColumn)), iRowEnd-iRowStart+1, m_iLinearMember);
    };

    raw_vector<T> getrow(int iRow, int iColumnStart, int iColumnEnd)
    {
        if( (iColumnStart>iColumnEnd) || wrongRow(iRow) || wrongColumn(iColumnStart) || wrongColumn(iColumnEnd))
            return raw_vector<T>(0, 0, 1);
        else
            return raw_vector<T>(&((*this)(iRow, iColumnStart)), iColumnEnd-iColumnStart+1, 1);
    };

    const_raw_vector<T> getcolumn(int iColumn, int iRowStart, int iRowEnd) const
    {
        if( (iRowStart>iRowEnd) || wrongColumn(iColumn) || wrongRow(iRowStart) ||wrongRow(iRowEnd) )
            return const_raw_vector<T>(0, 0, 1);
        else
            return const_raw_vector<T>(&((*this)(iRowStart, iColumn)), iRowEnd-iRowStart+1, m_iLinearMember);
    };

    const_raw_vector<T> getrow(int iRow, int iColumnStart, int iColumnEnd) const
    {
        if( (iColumnStart>iColumnEnd) || wrongRow(iRow) || wrongColumn(iColumnStart) || wrongColumn(iColumnEnd))
            return const_raw_vector<T>(0, 0, 1);
        else
            return const_raw_vector<T>(&((*this)(iRow, iColumnStart)), iColumnEnd-iColumnStart+1, 1);
    };
private:
    bool wrongRow(int i) const { return i<m_iLow1 || i>m_iHigh1; };
    bool wrongColumn(int j) const { return j<m_iLow2 || j>m_iHigh2; };

    T           *m_Vec;
    long        m_iVecSize;
    long        m_iLow1, m_iLow2, m_iHigh1, m_iHigh2;
    long        m_iConstOffset, m_iLinearMember;
};


typedef template_1d_array<int>          integer_1d_array;
typedef template_1d_array<double,true>  real_1d_array;
typedef template_1d_array<complex>      complex_1d_array;
typedef template_1d_array<bool>         boolean_1d_array;

typedef template_2d_array<int>          integer_2d_array;
typedef template_2d_array<double,true>  real_2d_array;
typedef template_2d_array<complex>      complex_2d_array;
typedef template_2d_array<bool>         boolean_2d_array;


/********************************************************************
dataset functions.
will be finished in AP 1.4
********************************************************************/
/*class dataset
{
public:
    dataset():nin(0), nout(0), nclasses(0), iscls(false), isreg(false), trnsize(0), valsize(0), tstsize(0), size(0){};

    int nin, nout, nclasses;
    bool iscls, isreg;

    int trnsize;
    int valsize;
    int tstsize;
    int size;

    ap::real_2d_array trn;
    ap::real_2d_array val;
    ap::real_2d_array tst;
    ap::real_2d_array all;
};*/

//bool opendataset(std::string file, dataset *pdataset);

//
// internal functions
//
bool readstrings(std::string file, std::list<std::string> *pOutput);
bool readstrings(std::string file, std::list<std::string> *pOutput, std::string comment);
void explodestring(std::string s, char sep, std::vector<std::string> *pOutput);

/********************************************************************
reverse communication state
********************************************************************/
struct rcommstate
{
    int stage;
    ap::integer_1d_array ia;
    ap::boolean_1d_array ba;
    ap::real_1d_array ra;
    ap::complex_1d_array ca;
};


/********************************************************************
Constants and functions introduced for compatibility with AlgoPascal
********************************************************************/
extern const double machineepsilon;
extern const double maxrealnumber;
extern const double minrealnumber;

int sign(double x);
double randomreal();
int randominteger(int maxv);
int round(double x);
int trunc(double x);
int ifloor(double x);
int iceil(double x);
double pi();
double sqr(double x);
int maxint(int m1, int m2);
int minint(int m1, int m2);
double maxreal(double m1, double m2);
double minreal(double m1, double m2);

};//namespace ap


#endif
