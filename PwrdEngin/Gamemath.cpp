#include "stdafx.h"
#include "Gamemath.h"
#include <iomanip>
#include <assert.h>
Vector2 Vector2::operator/(float scalar) const
{
	return Vector2(x/scalar,y/scalar);
}

Vector2 Vector2::operator*(float scalar) const
{
	return Vector2(x*scalar,y*scalar);
}

float Vector2::operator*(const Vector2 &v) const
{
	return this->x*v.x+this->y*v.y;
}

Vector2 Vector2::operator-(const Vector2& v) const
{
	return Vector2(x-v.x,y-v.y);
}

Vector2 Vector2::operator+(const Vector2&v ) const
{
	return Vector2(x+v.x,y+v.y);
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x,-y);
}

Vector2 Vector2::operator+() const
{
	return *this;
}

Vector2& Vector2::operator/=(float scalar)
{
	x=x/scalar;
	y=y/scalar;
	return *this;
}

Vector2& Vector2::operator*=(float scalar)
{
	x=x*scalar;
	y=y*scalar;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	this->x=this->x-v.x;
	this->y=this->y-v.y;
	return *this;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	this->x=this->x+v.x;
	this->y=this->y+v.y;
	return *this;
}

bool Vector2::operator==(const Vector2&v) const
{
	return x==v.x && y==v.y;
}

bool Vector2::operator!=(const Vector2&v) const
{
	return !(x==v.x && y==v.y);
}

Vector2& Vector2::Normalize()
{
	float sqr=sqrtf(x*x+y*y);
	if(sqr==0.0f)
		return *this;
	x/=sqr;
	y/=sqr;
	return *this;
}

float Vector2::Dot(const Vector2 &v) const
{
	return this->x*v.x+this->y*v.y;
}

float Vector2::Dot(const Vector2 *p) const
{
	assert(p);
	return x*p->x+y*p->y;
}

std::ostream& operator<<(std::ostream & out,Vector2 &v)
{
	out<<v.x<<"   "<<v.y;
	return out;
}





Vector3::Vector3(const float *v)
{
	x=v[0];
	y=v[1];
	z=v[2];
}

Vector3::Vector3(float x, float y, float z)
{
	this->x=x;
	this->y=y;
	this->z=z;
}

Vector3& Vector3::operator+=(const Vector3&v)
{
	x+=v.x;
	y+=v.y;
	z+=v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3&v)
{
	x-=v.x;
	y-=v.y;
	z-=v.z;
	return *this;
}

Vector3& Vector3::operator*=(float scalar)
{
	x*=scalar;
	y*=scalar;
	z*=scalar;
	return *this;
}

Vector3& Vector3::operator/=(float scalar)
{
	x/=scalar;
	y/=scalar;
	z/=scalar;
	return *this;
}

Vector3 Vector3::operator+() const
{
	return *this;	
}

Vector3 Vector3::operator+(const Vector3& v) const
{
	return Vector3(x+v.x,y+v.y,z+v.z);
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x,-y,-z);
}

Vector3 Vector3::operator-(const Vector3& v) const
{
	return Vector3(x-v.x,y-v.y,z-v.z);
}

Vector3 Vector3::operator*(float scalar) const
{
	return Vector3(x*scalar,y*scalar,z*scalar);
}

float Vector3::operator*(const Vector3 &v) const
{
	return x*v.x+y*v.y+z*v.z;
}

Vector3 Vector3::operator/(float scalar) const
{
	return Vector3(x/scalar,y/scalar,z/scalar);
}

bool Vector3::operator==(const Vector3&v) const
{
	return x==v.x && y==v.y && z==v.z;
}

bool Vector3::operator!=(const Vector3&v) const
{
	return  !(x==v.x && y==v.y && z==v.z);
}

Vector3& Vector3::Normalize()
{
	float sqr=sqrtf(x*x+y*y+z*z);
	if(sqr==0.0f)
		return *this;
	x/=sqr;
	y/=sqr;
	z/=sqr;
	return *this;
}

Vector3 Vector3::operator^(const Vector3 &v) const
{
	return Vector3(
		this->y*v.z-this->z*v.y,
		this->z*v.x-this->x*v.z,
		this->x*v.y-this->y*v.x);
}

Vector3 Vector3::Cross(const Vector3 &v) const
{
	return Vector3(
		this->y*v.z-this->z*v.y,
		this->z*v.x-this->x*v.z,
		this->x*v.y-this->y*v.x);
}

Vector3 Vector3::Cross(const Vector3 *p) const
{
	assert(p);
	return Vector3(
		this->y*p->z-this->z*p->y,
		this->z*p->x-this->x*p->z,
		this->x*p->y-this->y*p->x);
}

float Vector3::Dot(const Vector3 &v) const
{
	return x*v.x+y*v.y+z*v.z;
}

float Vector3::Dot(const Vector3 *p) const
{
	assert(p);
	return x*p->x+y*p->y+z*p->z;
}

Vector3& Vector3::operator=(const Vector4&v)
{
	if(v.w==0.0f)
		return *this;
	x=v.x/v.w;
	y=v.y/v.w;
	z=v.z/v.w;
	return *this;
}

std::ostream& operator<<(std::ostream & out,Vector3&v)
{
	out<<v.x<<"   "<<v.y<<"   "<<v.z;
	return out;
}
Vector4::Vector4(const float*v)
{
	x=v[0];
	y=v[1];
	z=v[2];
	w=v[3];
}

Vector4::Vector4(float x, float y, float z, float w)
{
	this->x=x;
	this->y=y;
	this->z=z;
	this->w=w;
}

Vector4& Vector4::operator+=(const Vector4&v)
{
	x+=v.x;
	y+=v.y;
	z+=v.z;
	w+=v.w;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4&v)
{
	x-=v.x;
	y-=v.y;
	z-=v.z;
	w-=v.w;
	return *this;
}

Vector4& Vector4::operator*=(float scalar)
{
	x*=scalar;
	y*=scalar;
	z*=scalar;
	w*=scalar;
	return *this;
}

Vector4& Vector4::operator/=(float scalar)
{
	x/=scalar;
	y/=scalar;
	z/=scalar;
	w/=scalar;
	return *this;
}

Vector4 Vector4::operator+() const
{
	 return *this;
}

Vector4 Vector4::operator+(const Vector4&v) const
{
	return Vector4(x+v.x,y+v.y,z+v.z,w+v.w);
}

Vector4 Vector4::operator-() const
{
	return Vector4(-x,-y,-z,-w);
}

Vector4 Vector4::operator-(const Vector4& v) const
{
	return Vector4(x-v.x,y-v.y,z-v.z,w-v.w);
}

Vector4 Vector4::operator*(float scalar) const
{
	return Vector4(x*scalar,y*scalar,z*scalar,w*scalar);	
}

float Vector4::operator*(const Vector4 &v) const
{
	return x*v.x+y*v.y+z*v.z+w*v.w;
}

Vector4& Vector4::operator*=(const Matrix &m)
{
	Vector4 tmp(*this);
	x=tmp.x*m.m[0][0]+tmp.y*m.m[1][0]+tmp.z*m.m[2][0]+tmp.w*m.m[3][0];
	y=tmp.x*m.m[0][1]+tmp.y*m.m[1][1]+tmp.z*m.m[2][1]+tmp.w*m.m[3][1];
	z=tmp.x*m.m[0][2]+tmp.y*m.m[1][2]+tmp.z*m.m[2][2]+tmp.w*m.m[3][2];
	w=tmp.x*m.m[0][3]+tmp.y*m.m[1][3]+tmp.z*m.m[2][3]+tmp.w*m.m[3][3];
	return *this;
}

Vector4 Vector4::operator/(float scalar) const
{
	return Vector4(x/scalar,y/scalar,z/scalar,w/scalar);
}

bool Vector4::operator==(const Vector4&v) const
{
	return x==v.x && y==v.y && z==v.z && w==v.w;	
}

bool Vector4::operator!=(const Vector4& v) const
{
	return !(*this==v);
}

Vector4& Vector4::Normalize()
{
	float sqr=sqrtf(x*x+y*y+z*z+w*w);
	if(sqr==0.0f)
		return *this;
	x/=sqr;
	y/=sqr;
	z/=sqr;
	w/=sqr;
	return *this;
}

float Vector4::Dot(const Vector4 &v) const
{
	return x*v.x+y*v.y+z*v.z+w*v.w;
}

float Vector4::Dot(const Vector4 *p) const
{
	return x*p->x+y*p->y+z*p->z+w*p->w;
}

Vector4& Vector4::operator=(const Vector3&v)
{
	x=v.x;
	y=v.y;
	z=v.z;
	w=1.0f;
	return *this;
}

Vector4& Vector4::ProjectDivied()
{
	if(w==0.0f)
		return *this;
	x=x/w;
	y=y/w;
	z=z/w;
	w=1.0f;
	return *this;
}

std::ostream& operator<<(std::ostream & out,Vector4&v)
{
	out<<v.x<<"   "<<v.y<<"   "<<v.z<<"   "<<v.w;
	return out;
}

Matrix * MatrixMultiply(Matrix *out,const Matrix *a,const Matrix *b)
{
	if(out==nullptr)
		return nullptr;
	Matrix tmp_a(*a);
	Matrix tmp_b(*b);
	memset(out,0,sizeof(Matrix));
	
	for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
			for(int k=0;k<4;++k)
			{
				out->m[i][j]+=(tmp_a.m[i][k]*tmp_b.m[k][j]);
			}
	return out;
}

Matrix::Matrix(const float *farr)
{
	memcpy(m,farr,sizeof(m));
}

Matrix::Matrix(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
{
	this->_11 = _11; this->_12 = _12; this->_13 = _13; this->_14 = _14;
	this->_21 = _21; this->_22 = _22; this->_23 = _23; this->_24 = _24;
	this->_31 = _31; this->_32 = _32; this->_33 = _33; this->_34 = _34;
	this->_41 = _41; this->_42 = _42; this->_43 = _43; this->_44 = _44;
}

float& Matrix::operator()(UINT row, UINT col)
{
	return m[row][col];
}

float Matrix::operator()(UINT row, UINT col) const
{
	return m[row][col];
}

Matrix::operator float*()
{
	return (float*)&m[0];
}

Matrix::operator const float*() const
{
	return (const float*)&m[0];
}

Matrix& Matrix::operator*=(const Matrix& m)
{
	MatrixMultiply(this,this,&m);
	return *this;
}

Matrix& Matrix::operator*=(float f)
{
	float *fp=&m[0][0];
	for(int i=0;i<16;i++)
	{
		fp[i]*=f;
	}
	return *this;
}

Matrix& Matrix::operator+=(const Matrix&m)
{
	float *fp_des=&this->m[0][0];
	const float *fp_src=&m.m[0][0];
	for(int i=0;i<16;i++)
		fp_des[i]+=fp_src[i];
	return *this;
}

Matrix& Matrix::operator-=(const Matrix&m)
{
	float *fp_des=&this->m[0][0];
	const float *fp_src=&m.m[0][0];
	for(int i=0;i<16;i++)
		fp_des[i]-=fp_src[i];
	return *this;
}

Matrix& Matrix::operator/=(float f)
{
	float *fp=&m[0][0];
	for(int i=0;i<16;i++)
	{
		fp[i]/=f;
	}
	return *this;
}

Matrix Matrix::operator+() const
{
	return *this;
}

Matrix Matrix::operator+(const Matrix&m) const
{
	Matrix tmp(*this);
	tmp+=m;
	return tmp;
}

Matrix Matrix::operator-() const
{
	Matrix tmp(*this);
	float *fp=&tmp.m[0][0];
	for(int i=0;i<16;i++)
	{
		fp[i]=-fp[i];
	}
	return tmp;	
}

Matrix Matrix::operator*(const Matrix&m) const
{
	Matrix tmp;
	MatrixMultiply(&tmp,this,&m);
	return tmp;
}

Matrix Matrix::operator*(float f) const
{
	Matrix tmp(*this);
	tmp*=f;
	return tmp;
}

Matrix Matrix::operator-(const Matrix& m) const
{
	Matrix tmp(*this);
	tmp-=m;
	return tmp;
}

Matrix Matrix::operator/(float f) const
{
	Matrix tmp(*this);
	tmp/=f;
	return tmp;
}

bool Matrix::operator==(const Matrix& m) const
{
	if(memcmp(this->m,m.m,sizeof(Matrix))==0)
		return true;
	else 
		return false;
}

bool Matrix::operator!=(const Matrix&m) const
{
	if(memcmp(this->m,m.m,sizeof(Matrix))!=0)
		return true;
	else 
		return false;
}

std::ostream & operator<<(std::ostream & out,Matrix &m)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			out<<std::setw(15)<<m.m[i][j];
		}
		out<<std::endl;
	}
	return out;
}
inline float CalcDetermin3X3(const float &_0,const float &_1,
	const float &_2,const float &_3,const float &_4,const float &_5,const float &_6,
	const float &_7,const float &_8)
{
	return _0*_4*_8+_1*_5*_6+_2*_3*_7-_0*_5*_7-_1*_3*_8-_2*_4*_6;
}
float MatrixDetermint(const Matrix *m,float *f)
{
	float cal=0.0f;
	const float *pf=*m;
	cal+=m->m[0][0]*CalcDetermin3X3(pf[5],pf[6],pf[7],pf[9],pf[10],pf[11],pf[13],pf[14],pf[15]);
	cal+=(-1)*m->m[1][0]*CalcDetermin3X3(pf[1],pf[2],pf[3],pf[9],pf[10],pf[11],pf[13],pf[14],pf[15]);
	cal+=m->m[2][0]*CalcDetermin3X3(pf[1],pf[2],pf[3],pf[5],pf[6],pf[7],pf[13],pf[14],pf[15]);
	cal+=(-1)*m->m[3][0]*CalcDetermin3X3(pf[1],pf[2],pf[3],pf[5],pf[6],pf[7],pf[9],pf[10],pf[11]);
	if(f)
		*f=cal;
	return cal;
}

void MatrixIdentity(Matrix *in_out)
{
	if(in_out)
	{
		memset(in_out,0,sizeof(Matrix));
		in_out->_11=1.0f;
		in_out->_22=1.0f;
		in_out->_33=1.0f;
		in_out->_44=1.0f;
	}
}
//////////////////////////////////////////////////////////////////////////
/*
首先 把观察坐标移到原点
T=
    1		0		0		0
	0		1		0		0
	0		0		1		0
	-eye.x	-eye.y		-eye.z		1
然后 旋转
n = normal(At - Eye)  //Z轴
u= normal(cross(Up, n))//X轴
v = cross(n, u)  //Y轴
R=
	u.x		v.x		n.x		0  
	u.y		v.y		n.y		0
	u.z		v.z		n.z		0
	0		0		0		1
T*R=
u.x           v.x           n.x          0
u.y           v.y           n.y          0
u.z           v.z           n.z          0
-dot(u, eye)  -dot(v, eye)  -dot(n, eye)  1

*/
//////////////////////////////////////////////////////////////////////////
Matrix * MatrixLookAtLH(Matrix *out,const Vector3 *eye,const Vector3 *look_at,const Vector3 *up)
{
	Vector3 eye_tmp=*eye;
	Vector3 look_at_tmp=*look_at;
	Vector3 up_tmp=*up;
	Vector3 n=(look_at_tmp-eye_tmp).Normalize();
	Vector3 u=(up_tmp^n).Normalize();
	Vector3 v=n^u;
	out->m[0][0]=u.x; out->m[0][1]=v.x; out->m[0][2]=n.x;out->m[0][3]=0.0f;
	out->m[1][0]=u.y; out->m[1][1]=v.y; out->m[1][2]=n.y;out->m[1][3]=0.0f;
	out->m[2][0]=u.z; out->m[2][1]=v.z; out->m[2][2]=n.z;out->m[2][3]=0.0f;
	out->m[3][0]=-u*eye_tmp; out->m[3][1]=-v*eye_tmp; out->m[3][2]=-n*eye_tmp;out->m[3][3]=1.0f;
	return out;	
}

void Normalize(Vector4 *v)
{
	float squre=v->x*v->x+v->y*v->y+v->z*v->z+v->w*v->w;
	float sqr=sqrtf(squre);
	if(sqr==0.0f)
		return;
	v->x=v->x/sqr;
	v->y=v->y/sqr;
	v->z=v->z/sqr;
	v->w=v->w/sqr;
}

void Normalize(Vector3 *v)
{
	float squre=v->x*v->x+v->y*v->y+v->z*v->z;
	float sqr=sqrtf(squre);
	if(sqr==0.0f)
		return;
	v->x=v->x/sqr;
	v->y=v->y/sqr;
	v->z=v->z/sqr;
}

void Normalize(Vector2 *v)
{
	float squre=v->x*v->x+v->y*v->y;
	float sqr=sqrtf(squre);
	if(sqr==0.0f)
		return;
	v->x=v->x/sqr;
	v->y=v->y/sqr;
}

float Dot(const Vector2*v1,const Vector2*v2)
{
	return v1->x*v2->x+v1->y*v2->y;
}

float Dot(const Vector3*v1,const Vector3*v2)
{
	return v1->x*v2->x+v1->y*v2->y+v1->z*v2->z;	
}

float Dot(const Vector4*v1,const Vector4*v2)
{
	return v1->x*v2->x+v1->y*v2->y+v1->z*v2->z+v1->w*v2->w;
}

Vector3 Cross(const Vector3*a ,const Vector3*b)
{
	return Vector3(
		a->y*b->z-a->z*b->y,
		a->z*b->x-a->x*b->y,
		a->x*b->y-a->y*b->x);
}
//////////////////////////////////////////////////////////////////////////
/*

xScale     0          0               0
0        yScale       0               0
0          0       zf/(zf-zn)         1
0          0       -zn*zf/(zf-zn)     0
where:
yScale = cot(fovY/2)

xScale = yScale / aspect ratio
*/
//////////////////////////////////////////////////////////////////////////
Matrix * MatrixPerspectiveFOVLH(Matrix *out,float fov,float aspect,float zn,float zf)
{
	assert(out);
	float y=tan(PI/2-fov/2);
	float x=y/aspect;
	out->m[0][0]=x; out->m[0][1]=0.0f; out->m[0][2]=0.0f;out->m[0][3]=0.0f;
	out->m[1][0]=0.0f; out->m[1][1]=y; out->m[1][2]=0.0f;out->m[1][3]=0.0f;
	out->m[2][0]=0.0f; out->m[2][1]=0.0f; out->m[2][2]=zf/(zf-zn);out->m[2][3]=1.0f;
	out->m[3][0]=0.0f; out->m[3][1]=0.0f; out->m[3][2]=-zn*zf/(zf-zn);out->m[3][3]=0.0f;
	return out;
}

Matrix * MatrixViewPort(Matrix* out, int x,int y,int width,int height,float minZ/*=0.0f*/,float maxZ/*=1.0f*/)
{
	assert(out);
	out->m[0][0]=width/2; out->m[0][1]=0.0f; out->m[0][2]=0.0f;out->m[0][3]=0.0f;
	out->m[1][0]=0.0f; out->m[1][1]=-height/2; out->m[1][2]=0.0f;out->m[1][3]=0.0f;
	out->m[2][0]=0.0f; out->m[2][1]=0.0f; out->m[2][2]=maxZ-minZ;out->m[2][3]=0.0f;
	out->m[3][0]=x+width/2; out->m[3][1]=y+height/2; out->m[3][2]=minZ;out->m[3][3]=1.0f;
	return out;
}

Matrix * MatrixRotationX(Matrix *out,float angle)
{
	if(out)
	{
		memset(out,0,sizeof(Matrix));
		float c=cos(angle);
		float s=sin(angle);
		out->m[0][0]=1.0f;
		out->m[1][1]=c;
		out->m[1][2]=s;
		out->m[2][1]=-s;
		out->m[2][2]=c;
		out->m[3][3]=1.0f;
	}
	return out;
}

Matrix * MatrixRotationY(Matrix *out,float angle)
{
	if(out)
	{
		memset(out,0,sizeof(Matrix));
		float c=cos(angle);
		float s=sin(angle);
		out->m[0][0]=c;
		out->m[0][2]=-s;
		out->m[1][1]=1.0f;
		out->m[2][0]=s;
		out->m[2][2]=c;
		out->m[3][3]=1.0f;
	}
	return out;
}

Matrix * MatrixRotationZ(Matrix *out,float angle)
{
	if(out)
	{
		memset(out,0,sizeof(Matrix));
		float c=cos(angle);
		float s=sin(angle);
		out->m[0][0]=c;
		out->m[0][1]=s;
		out->m[1][0]=-s;
		out->m[1][1]=c;
		out->m[2][2]=1.0f;
		out->m[3][3]=1.0f;
	}
	return out;
}

Matrix * MatrixRotationQuaternion(Matrix *out,const Quaternion *q)
{
	assert(q);
	const float x=q->x;
	const float y=q->y;
	const float z=q->z;
	const float w=q->w;
	if(out)
	{
		memset(out,0,sizeof(Matrix));
		out->m[0][0]=1-2*y*y-2*z*z; out->m[0][1]=2*x*y+2*w*z; out->m[0][2]=2*x*z-2*w*y;
		out->m[1][0]=2*x*y-2*w*z;out->m[1][1]=1-2*x*x-2*z*z;out->m[1][2]=2*y*z+2*w*x;
		out->m[2][0]=2*x*z+2*w*y;out->m[2][1]=2*y*z-2*w*x;out->m[2][2]=1-2*x*x-2*y*y;
		out->m[3][3]=1.0f;
	}
	return out;
}
std::ostream& operator<<(std::ostream&out,const Quaternion &q)
{
	out<<q.x<<"	"<<q.y<<"		"<<q.z<<"		"<<q.w<<endl;
	return out;
}
Quaternion * QuaternionIdentity(Quaternion *q)
{
	if(q)
	{
		q->x=q->y=q->z=0.0f;
		q->w=1.0f;
	}
	return q;
}

Matrix * MatrixTranslation(Matrix *out,float x,float y,float z)
{
	if(out)
	{
		memset(out,0,sizeof(Matrix));
		out->m[0][0]=1.0f;
		out->m[1][1]=1.0f;
		out->m[2][2]=1.0f;
		out->m[3][3]=1.0f;
		out->m[3][0]=x;
		out->m[3][1]=y;
		out->m[3][2]=z;
	}
	return out;
}
float Matrix33(const float &_0,const float&_1,const float&_2,const float&_3,const float&_4,const float&_5,const float&_6,const float&_7,const float&_8)
{
	return _0*_4*_8+_1*_5*_6+_2*_3*_7-_0*_5*_7-_1*_3*_8-_2*_4*_6;
}

float MatrixYUZI(const Matrix* mat,int i,int j)
{
	int arr[9];
	int pos=0;
	for(int m=0;m<4;m++)
		for(int n=0;n<4;n++)
		{
			if(m!=i && n!=j)
				arr[pos++]=m*4+n;
		}
	float index=(i+j)%2==0?1.0f:-1.0f;
	const float* mat_f=(const float*)mat;
	return index*Matrix33(mat_f[arr[0]],mat_f[arr[1]],mat_f[arr[2]],mat_f[arr[3]],mat_f[arr[4]],mat_f[arr[5]],mat_f[arr[6]],mat_f[arr[7]],mat_f[arr[8]]);
}
Matrix * MatrixInverse(Matrix*out,float *determin,const Matrix *in)
{
	if(out==NULL||in==NULL)
		return out;
	float deter=MatrixDetermint(in);
	if(deter==0)
	{
		if(determin)
			*determin=0;
		return out;
	}
	Matrix result;
	memset(&result,0,sizeof(Matrix));
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			result.m[i][j]=MatrixYUZI(in,j,i)/deter;
	*out=result;	
	return out;
}


Quaternion Quaternion::operator*(const Quaternion & q)
{
	Quaternion p;
	p.w=w*q.w-x*q.x-y*q.y-z*q.z;
	p.x=w*q.x+x*q.w+y*q.z-z*q.y;
	p.y=w*q.y-x*q.z+y*q.w+z*q.x;
	p.z=w*q.z+x*q.y-y*q.x+z*q.w;
	return p;
}
