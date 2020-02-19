#ifndef MATRIX_H_
#define MATRIX_H_

class Matrix4x4
{	
public:
	union
	{	float m[16];
		struct
		{	float m00, m10, m20, m30;
			float m01, m11, m21, m31;
			float m02, m12, m22, m32;
			float m03, m13, m23, m33;									
		};		
    };    
    
	Matrix4x4(	float M00, 	float M10, 	float M20, 	float M30,
				float M01, 	float M11, 	float M21, 	float M31,
				float M02, 	float M12, 	float M22, 	float M32,
				float M03, 	float M13, 	float M23, 	float M33 )	:
					m00(M00), 	m10(M10), 	m20(M20), 	m30(M30),
					m01(M01), 	m11(M11), 	m21(M21), 	m31(M31),
					m02(M02), 	m12(M12), 	m22(M22), 	m32(M32),
					m03(M03), 	m13(M13), 	m23(M23), 	m33(M33) 	{}
			
    operator float*()			{ return m; 	}
	float& operator[](int i) 	{ return m[i]; 	}
};

#endif /* MATRIX_H_ */
