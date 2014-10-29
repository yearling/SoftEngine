#ifndef __GAME_MATH_H__
#define __GAME_MATH_H__
#pragma once
#include "stdafx.h"
#include <math.h>
#include <iostream>
#define PI    ((FLOAT)  3.141592654f)
#define ToRadian( degree ) ((degree) * (PI / 180.0f))
#define ToDegree( radian ) ((radian) * (180.0f / PI)

class Vector2
{
public:
	Vector2():x(float(0)),y(float(0)) {};
	Vector2( float x_, float y_ ){x=x_;y=y_; }
	Vector2& operator += ( const Vector2& );
	Vector2& operator -= ( const Vector2& );
	Vector2& operator *= ( float );
	Vector2& operator /= ( float );
	Vector2 operator + () const;
	Vector2 operator - () const;
	Vector2 operator + ( const Vector2& ) const;
	Vector2 operator - ( const Vector2& ) const;
	Vector2 operator * ( float ) const;
	Vector2 operator / ( float ) const;
	friend Vector2 operator * ( float, const Vector2& );
	bool operator == ( const Vector2& ) const;
	bool operator != ( const Vector2& ) const;
	friend std::ostream& operator<<(std::ostream & out,Vector2 &);
public:
	float x, y;
};
class Vector3 
{
public:
	Vector3():x(0.0f),y(0.0f),z(0.0f){};
	Vector3( const float * );
	Vector3( float x, float y, float z );
	Vector3& operator += ( const Vector3& );
	Vector3& operator -= ( const Vector3& );
	Vector3& operator *= ( float );
	Vector3& operator /= ( float );
	Vector3 operator + () const;
	Vector3 operator - () const;
	Vector3 operator + ( const Vector3& ) const;
	Vector3 operator - ( const Vector3& ) const;
	Vector3 operator * ( float ) const;
	Vector3 operator / ( float ) const;
	bool operator == ( const Vector3& ) const;
	bool operator != ( const Vector3& ) const;
	friend std::ostream& operator<<(std::ostream &,Vector3&);
public:
	float x,y,z;
};
class Vector4
{
public:
	Vector4():x(0),y(0),z(0),w(1) {};
	Vector4( const float* );
	Vector4( float x, float y, float z, float w );
	Vector4& operator += ( const Vector4& );
	Vector4& operator -= ( const Vector4& );
	Vector4& operator *= ( float );
	Vector4& operator /= ( float );
	Vector4 operator + () const;
	Vector4 operator - () const;
	Vector4 operator + ( const Vector4& ) const;
	Vector4 operator - ( const Vector4& ) const;
	Vector4 operator * ( float ) const;
	Vector4 operator / ( float ) const;
	bool operator == ( const Vector4& ) const;
	bool operator != ( const Vector4& ) const;
	friend std::ostream& operator<<(std::ostream &,Vector4&);
public:
	float x, y, z, w;
}; 
class Matrix 
{
public:
	Matrix() {};
	Matrix( const float * );
	Matrix( float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44 );

	float& operator () ( UINT row, UINT col );
	float  operator () ( UINT row, UINT col ) const;

	operator float* ();
	operator const float* () const;

	Matrix& operator *= ( const Matrix& );
	Matrix& operator += ( const Matrix& );
	Matrix& operator -= ( const Matrix& );
	Matrix& operator *= ( float );
	Matrix& operator /= ( float );

	Matrix operator + () const;
	Matrix operator - () const;

	Matrix operator * ( const Matrix& ) const;
	Matrix operator + ( const Matrix& ) const;
	Matrix operator - ( const Matrix& ) const;
	Matrix operator * ( float ) const;
	Matrix operator / ( float ) const;
	bool operator == ( const Matrix& ) const;
	bool operator != ( const Matrix& ) const;
	friend std::ostream & operator<<(std::ostream & out,Matrix &);	
	union {
			struct {
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;

			};
			float m[4][4];
		};

};
Matrix *MatrixMultiply(Matrix *out,const Matrix *pm1,const Matrix *pm2);
float MatrixDetermint(const Matrix *in,float *f=nullptr);
#endif