#ifndef __GAME_MATH_H__
#define __GAME_MATH_H__
#pragma once
#include "stdafx.h"
#include <math.h>
//#include <iostream>
#define PI    ((FLOAT)  3.141592654f)
#define ToRadian( degree ) ((degree) * (PI / 180.0f))
#define ToDegree( radian ) ((radian) * (180.0f / PI)
inline int Round( float f) {return int(f+0.5);}
class Vector2;
class Vector3;
class Vector4;
class Matrix;
class Matrix3x3;
class Quaternion;
class Color;
class Vector2
{
public:
	Vector2():x(float(0)),y(float(0)) {};
	Vector2(const float*p):x(p[0]),y(p[1]){};
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
	Vector2& Normalize();
	float Dot(const Vector2 &) const;
	float Dot(const Vector2 *) const;
	float operator*(const Vector2 &)const;
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
	Vector3& operator=(const Vector4&);
	Vector3 operator + () const;
	Vector3 operator - () const;
	Vector3 operator + ( const Vector3& ) const;
	Vector3 operator - ( const Vector3& ) const;
	Vector3 operator * ( float ) const;
	Vector3 operator *(const  Matrix3x3 &) const;
	Vector3 operator / ( float ) const;
	bool operator == ( const Vector3& ) const;
	bool operator != ( const Vector3& ) const;
	friend std::ostream& operator<<(std::ostream &,Vector3&);
	Vector3& Normalize();
	Vector3 operator^(const Vector3 &) const;
	Vector3 Cross(const Vector3 &) const;
	Vector3 Cross(const Vector3 *)const;
	float Dot(const Vector3 &) const;
	float Dot(const Vector3 *) const;
	float operator*(const Vector3 &)const;
	float GetLength() const;
public:
	float x,y,z;
};
class Vector4
{
public:
	Vector4():x(0),y(0),z(0),w(1) {};
	Vector4( const float* );
	Vector4( float x, float y, float z, float w );
	operator Color();
	Vector4& operator=(const Vector3&);
	Vector4& operator += ( const Vector4& );
	Vector4& operator -= ( const Vector4& );
	Vector4& operator *= ( float );
	Vector4& operator /= ( float );
	Vector4& operator*=(const Matrix &);
	Vector4 operator *(const Matrix &);
	Vector4 operator + () const;
	Vector4 operator - () const;
	Vector4 operator + ( const Vector4& ) const;
	Vector4 operator - ( const Vector4& ) const;
	Vector4 operator * ( float ) const;
	Vector4 operator / ( float ) const;
	bool operator == ( const Vector4& ) const;
	bool operator != ( const Vector4& ) const;
	friend std::ostream& operator<<(std::ostream &,Vector4&);
	Vector4& Normalize();
	Vector4& ProjectDivied();
	float Dot(const Vector4 &) const;
	float Dot(const Vector4 *) const;
	float operator*(const Vector4 &)const;
	int ToColor() const;
public:
	float x, y, z, w;
}; 
class Color:public Vector4
{
public:
	Color(){};
	Color(float r,float g,float b,float a);
	explicit Color(const Vector4 &);
	explicit Color(int );
	Color operator *(float ) const;
	Color operator *(const Color &) const;
	Color operator +(const Color &) const;
	Color &Sature();
	operator int();
};
class Matrix3x3
{
public:
	Matrix3x3() {};
	Matrix3x3(const float *);
	explicit Matrix3x3(const Matrix &);
	Matrix3x3( float _11, float _12, float _13, 
		float _21, float _22, float _23,
		float _31, float _32, float _33 
		);
	Matrix3x3(const Vector3 &,const Vector3 &,const Vector3 &);
	union {
		struct {
			float        _11, _12, _13;
			float        _21, _22, _23;
			float        _31, _32, _33;
		};
		float m[3][3];
		struct  
		{
			Vector3 u;
			Vector3 v;
			Vector3 n;
		};
	};
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
class Quaternion {
public:
	Quaternion(){};
	Quaternion(float x_,float y_,float z_,float w_):x(x_),y(y_),z(z_),w(w_){}
	Quaternion operator*(const Quaternion & q);
	friend std::ostream& operator<<(std::ostream&out,const Quaternion &q);
	float x;
	float y;
	float z;
	float w;
};
class Plane:public Vector4
{
public:
	Plane(){};
	Plane(float a,float b,float c,float d):Vector4(a,b,c,d){};
	float operator*(const Vector3& v) const ;
	float operator*(const Vector4& v) const; 
};
Matrix *MatrixMultiply(Matrix *out,const Matrix *pm1,const Matrix *pm2);
float MatrixDetermint(const Matrix *in,float *f=nullptr);
void MatrixIdentity(Matrix *in_out);
Matrix *MatrixLookAtLH(Matrix *out,const Vector3 *eye,const Vector3 *look_at,const Vector3 *up);
Matrix *MatrixPerspectiveFOVLH(Matrix *out,float fov,float aspect,float zn,float zf);
Matrix *MatrixViewPort(Matrix* out, int x,int y,int width,int height,float minZ=0.0f,float maxZ=1.0f);
Matrix *MatrixRotationX(Matrix *out,float angle);
Matrix *MatrixRotationY(Matrix *out,float angle);
Matrix *MatrixRotationZ(Matrix *out,float angle);
Matrix *MatrixTranslation(Matrix *out,float x,float y,float z);
Matrix *MatrixInverse(Matrix*out,float *determin,const Matrix *in);
Matrix *MatrixTranspose(Matrix*out,const Matrix *in);
Matrix *MatrixRotationQuaternion(Matrix *out,const Quaternion *q);
Quaternion * QuaternionIdentity(Quaternion *q);
void Normalize(Vector4 *v);
void Normalize(Vector3 *v);
void Normalize(Vector2 *v);
Vector3* Normalize(Vector3 *vout,const Vector3 *vin);
float Dot(const Vector2*v1,const Vector2*v2);
float Dot(const Vector3*v1,const Vector3*v2);
float Dot(const Vector4*v1,const Vector4*v2);
Vector3 Cross(const Vector3*v1 ,const Vector3*v2);
Vector3 *Vec3TransformCoord(Vector3*out,const Vector3*v,const Matrix* mat);
inline Vector3 ToVector3(const unsigned char* base_ptr,UINT pos,UINT data_size,UINT offset)
{
	const unsigned char* p=base_ptr+(pos*data_size+offset);
	return Vector3(reinterpret_cast<const float*>(p));
}
inline Vector4 ToVector4(const unsigned char* base_ptr,UINT pos,UINT data_size,UINT offset)
{
	const unsigned char* p=base_ptr+(pos*data_size+offset);
	return Vector4(reinterpret_cast<const float*>(p));
}
inline Vector2 ToVector2(const unsigned char* base_ptr,UINT pos,UINT data_size,UINT offset)
{
	const unsigned char* p=base_ptr+(pos*data_size+offset);
	return Vector2(reinterpret_cast<const float*>(p));
}
Vector2 Lerp(const Vector2 &v0,const Vector2&v1,float f);
Vector3 Lerp(const Vector3 &v0,const Vector3&v1,float f);
Vector4 Lerp(const Vector4 &v0,const Vector4&v1,float f);

int ToColor(const Vector4 &v);
float GetAngle(const Vector3 &,const Vector3 &);
#endif