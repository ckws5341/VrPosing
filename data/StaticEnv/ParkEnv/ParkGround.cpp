/*
This file was produced by Deep Exploration Plugin: CPP Export filter.

Copyright (C) 1999-2009 Right Hemisphere
Mail support@righthemisphere.com for support.
Visit http://www.righthemisphere.com/dexp.htm for updates.
*/
#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include <math.h>

namespace ParkGround
{

struct sample_MATERIAL
{
	GLfloat ambient[3];
	GLfloat diffuse[3];
	GLfloat specular[3];
	GLfloat emission[3];
	GLfloat alpha;
	GLfloat phExp;
	int texture;
};
struct sample_TEXTURE
{
	char * name;
	GLint id;
};

static sample_MATERIAL materials [1] = {
 {{0.117647f,0.117647f,0.117647f},	{0.356863f,0.364706f,0.223529f},	{0.248471f,0.248471f,0.248471f},	{0.0f,0.0f,0.0f},	1.0f,8.0f,0} //Standard_11
};

static sample_TEXTURE texture_maps [1] = {
{"data/StaticEnv/ParkEnv/ParkGround._0.bmp",0}
};

// 4 Verticies
// 4 Texture Coordinates
// 1 Normals
// 2 Triangles

static BYTE face_indicies[2][9] = {
// object #0
	{0,1,2 ,0,0,0 ,0,1,2 }, {0,3,1 ,0,0,0 ,0,3,1 }
};
static GLfloat vertices [4][3] = {
{360.052f,-411.923f,0.198915f},{-392.595f,353.236f,0.198915f},{-392.63f,-411.923f,0.198915f},
{360.041f,354.768f,0.198915f}
};
static GLfloat normals [1][3] = {
{0.0f,0.0f,1.0f}
};
static GLfloat textures [4][2] = {
{0.998616f,0.0005f},{0.00146f,0.997474f},{-0.000385f,0.0005f},
{0.99854f,0.9995f}
};
/*Material indicies*/
/*{material index,face count}*/
static int material_ref [1][2] = {
{0,2}
};
struct DIB2D
{
	BITMAPINFOHEADER * Info;
	RGBQUAD * palette;
	BYTE * bits;
};

struct GLTXTLOAD
{
	GLint format;
	GLint perpixel;
	GLint Width;
	GLint Height;
	BYTE * bits;
};

/*
BOOL LoadDIB(char*file,DIB2D*dib)
Only trueColor and 256 color ucompressed bitmaps supported
*/
BOOL LoadDIB(char * file, DIB2D * dib)
{
	BOOL result=FALSE;
	HANDLE hfile= CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING ,FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile != INVALID_HANDLE_VALUE)
	{
		DWORD readed;
		int size = GetFileSize(hfile, 0);
		if (size > sizeof(BITMAPFILEHEADER) )
		{
			BITMAPFILEHEADER bmfh;
			ReadFile(hfile, &bmfh, sizeof(BITMAPFILEHEADER), &readed, 0);
			if ( (readed == sizeof(BITMAPFILEHEADER) ) && (bmfh.bfType == 0x4d42) )
			{
				dib->Info = (BITMAPINFOHEADER *)(new BYTE[size - sizeof(BITMAPFILEHEADER) ] );
				ReadFile(hfile, dib->Info, size - sizeof(BITMAPFILEHEADER), &readed, 0);
				dib->bits = (BYTE *)(dib->Info +1);
				if (dib->Info->biBitCount == 8)
				{
					dib->palette = (RGBQUAD *)dib->bits;
					if (dib->Info->biClrUsed)
						dib->bits += dib->Info->biClrUsed *4;
					else
						dib->bits += 1024;
				}
				else
				{
					dib->palette = NULL;
				}
				result=TRUE;
			}
		}
		CloseHandle(hfile);
	}
	return result;
};

long ScanBytes(int pixWidth, int bitsPixel)
{
  return ( ( (long)pixWidth * bitsPixel +31) / 32) * 4;
}

BOOL ScaleImage(DIB2D & dib, GLTXTLOAD & p)
{
	GLint glMaxTexDim;     // OpenGL maximum texture dimension
	GLint XDMaxTexDim = 512; // user maximum texture dimension
	GLint minsize = 2;
	double xPow2, yPow2;
	int ixPow2, iyPow2;
	int xSize2, ySize2;
	GLint m_iWidth = dib.Info->biWidth;
	GLint m_iHeight = dib.Info->biHeight;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);
	glMaxTexDim = min(XDMaxTexDim, glMaxTexDim);

	if (m_iWidth <= glMaxTexDim)
		xPow2 = log( (double)m_iWidth) / log(2.0);
	else
		xPow2 = log( (double)glMaxTexDim) / log(2.0);
	if (m_iHeight <= glMaxTexDim)
		yPow2 = log( (double)m_iHeight) / log(2.0);
	else
		yPow2 = log( (double)glMaxTexDim) / log(2.0);
	ixPow2 = (int)xPow2;
	iyPow2 = (int)yPow2;

	if (xPow2 != (double)ixPow2)
		ixPow2 ++;
	if (yPow2 != (double)iyPow2)
		iyPow2 ++;

	xSize2 = 1 << ixPow2;
	ySize2 = 1 << iyPow2;
	if (xSize2 < minsize)
		xSize2 = minsize;
	if (ySize2 < minsize)
		ySize2 = minsize;

	if ( ( (xSize2 == m_iWidth) && (ySize2 == m_iHeight) ) )
	{
		if (dib.Info->biBitCount == 24)
		{
			p.format = GL_BGR_EXT;
			p.perpixel = 3;
			return FALSE;
		}
		if (dib.Info->biBitCount == 32)
		{
			p.format = GL_BGRA_EXT;
			p.perpixel = 4;
			return FALSE;
		}
	}

	BYTE * bits = (BYTE *)dib.bits;
	if (dib.Info->biBitCount == 8)
	{
// convert to TRUECOLOR
		int _perline = ScanBytes(8, m_iWidth);
		int perline = ScanBytes(24, m_iWidth);
		bits = new BYTE[perline * m_iHeight * sizeof(BYTE)];
		for (int y = 0; y < m_iHeight; y ++)
		{
			BYTE * _b = ( (BYTE *)dib.bits) + y * _perline;
			BYTE * b = bits + y * perline;
			for (int x = 0; x < m_iWidth; x ++)
			{
				RGBQUAD _p = dib.palette[*_b];
				_b ++;
				*b = _p.rgbBlue; b ++;
				*b = _p.rgbGreen; b ++;
				*b = _p.rgbRed; b ++;
			}
		}
	}
	BOOL isAlpha = (dib.Info->biBitCount == 32);
	int _mem_size = xSize2 * ySize2 * sizeof(BYTE);
	if (isAlpha)
	{
		_mem_size *= 4;
		p.perpixel = 4;
		p.format = GL_BGRA_EXT;
	}
	else
	{
		_mem_size *= 3;
		p.perpixel = 3;
		p.format = GL_BGR_EXT;
	}
	BYTE * pData = (BYTE *)new BYTE[_mem_size];
	if (!pData)
		return FALSE;
	if (isAlpha)
	{
		gluScaleImage(GL_BGRA_EXT, m_iWidth, m_iHeight,
			GL_UNSIGNED_BYTE, bits,
			xSize2, ySize2, GL_UNSIGNED_BYTE, pData);
	}
	else
		gluScaleImage(GL_RGB, m_iWidth, m_iHeight,
			GL_UNSIGNED_BYTE, bits,
			xSize2, ySize2, GL_UNSIGNED_BYTE, pData);

	if (bits != dib.bits)
		delete bits;
//	m_pBits = pData;
	m_iWidth = xSize2;
	m_iHeight = ySize2;
	p.Width = m_iWidth;
	p.Height = m_iHeight;
	p.bits = pData;
	return TRUE;
}

void LoadTexture(char * filename)
{
	DIB2D dib;
	GLTXTLOAD load;
	if (LoadDIB(filename, &dib) )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (ScaleImage(dib, load) )
		{
			glTexImage2D(GL_TEXTURE_2D, 0, load.perpixel,
				load.Width, load.Height, 0,
				load.format, GL_UNSIGNED_BYTE,
				load.bits);
			delete load.bits;
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, load.perpixel,
				dib.Info->biWidth, dib.Info->biHeight,
				0, load.format, GL_UNSIGNED_BYTE,
				dib.bits);
		}
		delete dib.Info;
	}
//	DeleteObject(hbitmap);
};
void MyMaterial(GLenum mode, GLfloat * f, GLfloat alpha)
{
	GLfloat d[4];
	d[0] = f[0];
	d[1] = f[1];
	d[2] = f[2];
	d[3] = alpha;
	glMaterialfv(GL_FRONT_AND_BACK, mode, d);
}

/*
 *  SelectMaterial uses OpenGL commands to define facet colors.
 *
 *  Returns:
 *    Nothing
 */

void SelectMaterial(int i)
{
//
// Define the reflective properties of the 3D Object faces.
//
	glEnd();
	GLfloat alpha = materials[i].alpha;
	MyMaterial(GL_AMBIENT, materials[i].ambient, alpha);
	MyMaterial(GL_DIFFUSE, materials[i].diffuse, alpha);
	MyMaterial(GL_SPECULAR, materials[i].specular, alpha);
	MyMaterial(GL_EMISSION, materials[i].emission, alpha);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materials[i].phExp);
	glEnd();
	if (materials[i].texture > -1)
	{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_maps[materials[i].texture].id);
	}
	else
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);

	glBegin(GL_TRIANGLES);

};

GLint Gen3DObjectList(int list_id)
{
	int i;
	int j;

	GLuint texture_name;
	glGenTextures(1,&texture_name);
	texture_maps[0].id=texture_name;
	glBindTexture(GL_TEXTURE_2D,texture_name);
	LoadTexture(texture_maps[0].name);

	GLint lid;
	if ( list_id == -1 ) lid =glGenLists(1);
	else lid = list_id;

	int mcount=0;
	int mindex=0;
	glNewList(lid, GL_COMPILE);

	glBegin (GL_TRIANGLES);
	for(i=0;i<sizeof(face_indicies)/sizeof(face_indicies[0]);i++)
	{
		if (!mcount)
		{
			SelectMaterial(material_ref[mindex][0]);
			mcount = material_ref[mindex][1];
			mindex ++;
		}
		mcount --;
		for(j=0;j<3;j++)
		{
			int vi=face_indicies[i][j];
			int ni=face_indicies[i][j+3];//Normal index
			int ti=face_indicies[i][j+6];//Texture index
			glNormal3f (normals[ni][0],normals[ni][1],normals[ni][2]);
			glTexCoord2f(textures[ti][0],textures[ti][1]);
			glVertex3f (vertices[vi][0],vertices[vi][1],vertices[vi][2]);
		}
	}
	glEnd ();

	glEndList();
	return lid;
};

};