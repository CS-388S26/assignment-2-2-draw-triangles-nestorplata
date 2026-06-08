#include <AEEngine.h>
#include "Rasterizer.h"
namespace Rasterizer
{
	EDrawTriangleMethod CurrentTriangleMethod;
	namespace TriangleHelperFunctions
	{
		float m = 0, b = 0;
		unsigned int y = 0, x = 0;
		int dx = 0, dy = 0;
		unsigned int ep = 0;  //endpoint
		int step = 0;
		// calDelta based on  independent variables

		void CalStep(const int p1i, const int p2i)
		{
			step = (p2i - p1i) > 0 ? 1 : -1; // Step: going to the left or right
			ep = p2i + step;
		}

		//simple interpolate
		void Interpolate(unsigned int* p1i, const int p2i, const Color& c)
		{
			CalStep(*p1i, p2i);
			for (; *p1i != ep; *p1i += step)
			{
				FrameBuffer::SetPixel(x, y, c);
			}
		}
		float CalRateOfChange(const AEVec2 p1, const AEVec2 p2)
		{
			return m = p2.y - p1.y / p2.x - p1.x;
		}
		float CalSlopeInv(const AEVec2* p0, const AEVec2* p1)
		{
			float dy = (p0->y - p1->y);
			float dx = (p0->x - p1->x);
			if (Round(dy))
			{
				return  dx / dy;
			}
			else
			{
				return dy / dx;
			}
		}

		Color CalColorSlopeInv(const Color* c0, const Color* c1)
		{
			Color CStep = *c0 - *c1;
			float dy = (c0->y - c1->y);
			float dx = (c0->x - c1->x);
			if (Round(dy))
			{
				return  dx / dy;
			}
			else
			{
				return dy / dx;
			}
		}

		//calculate Deltas
		void CalDeltas(const AEVec2 p1, const AEVec2 p2)
		{
			dy = Round(p2.y - p1.y);
			dx = Round(p2.x - p1.x);
		}
		void setStartingPoint(const AEVec2& p)
		{
			x = Round(p.x);
			y = Round(p.y);
		}
	}	
	using namespace TriangleHelperFunctions;

	void FillTriangleNaive(const AEVec2& v0, const AEVec2& v1, const AEVec2& v2, const Color& c)
	{
		const AEVec2* vTop = &v0;
		const AEVec2* vMid = &v1;
		const AEVec2* vBot = &v2;

		if (vTop->y < vMid->y) std::swap(vTop, vMid);
		if (vMid->y < vBot->y) std::swap(vMid, vBot);
		if (vTop->y < vMid->y) std::swap(vTop, vMid);

		AEVec2 vTopBot = (*vTop) - (*vBot);
		AEVec2 vTopMid = (*vTop) - (*vMid);
		float dotproduct = vTopBot.CrossMag(vTopMid);
		bool midIsLeft = dotproduct < 0;

		float mTopMid = CalSlopeInv(vMid, vTop);
		float mTopBot = CalSlopeInv(vBot, vTop);
		float mMidBot = CalSlopeInv(vBot, vMid);


		float xL = Round(vTop->x);
		float xR = Round(vTop->x);

		int y = 0;
		int x = 0;
		for (y = Round(vTop->y); y >= Round(vMid->y); --y)
		{
			for (x = Round(xL); x <= Round(xR); ++x)
			{
				FrameBuffer::SetPixel(x, y, c);
			}
			xL -= midIsLeft ? mTopMid : mTopBot;
			xR -= midIsLeft ? mTopBot : mTopMid;


		}

		if (midIsLeft)
			xL = Round(vMid->x);
		else
			xR = Round(vMid->x);

		for (; y >= Round(vBot->y); --y)
		{
			for (x = Round(xL); x <= Round(xR); ++x)
			{
				FrameBuffer::SetPixel(x, y, c);
			}
			xL -= midIsLeft ? mMidBot : mTopBot;
			xR -= midIsLeft ? mTopBot : mMidBot;
		}
	}

	void FillTriangleTopLeft(const AEVec2& v0, const AEVec2& v1, const AEVec2& v2, const Color& c)
	{

			const AEVec2* vTop = &v0;
			const AEVec2* vMid = &v1;
			const AEVec2* vBot = &v2;

			if (vTop->y < vMid->y) std::swap(vTop, vMid);
			if (vMid->y < vBot->y) std::swap(vMid, vBot);
			if (vTop->y < vMid->y) std::swap(vTop, vMid);

			AEVec2 vTopBot = (*vTop) - (*vBot);
			AEVec2 vTopMid = (*vTop) - (*vMid);
			float dotproduct = vTopBot.CrossMag(vTopMid);
			bool midIsLeft = dotproduct < 0;

			float mTopMid =CalSlopeInv(vMid, vTop);
			float mTopBot =CalSlopeInv(vBot, vTop);
			float mMidBot = CalSlopeInv(vBot, vMid);

			float xL = Round(vTop->x);
			float xR = Round(vTop->x);

			int y = 0;
			int x = 0;
			for (y = Ceiling(vTop->y); y >= Ceiling(vMid->y) + 1; --y)
			{
				for (x = Round(xL); x <= Round(xR) - 1; ++x)
				{
					FrameBuffer::SetPixel(x, y, c);
				}

				xL -= midIsLeft ? mTopMid : mTopBot;
				xR -= midIsLeft ? mTopBot : mTopMid;
			}

			if (midIsLeft)
				xL = Round(vMid->x);
			else
				xR = Round(vMid->x);

			for (; y >= Ceiling(vBot->y) + 1; --y)
			{
				for (x = Round(xL); x <= Round(xR) - 1; ++x)
				{
					FrameBuffer::SetPixel(x, y, c);
				}
				xL -= midIsLeft ? mMidBot : mTopBot;
				xR -= midIsLeft ? mTopBot : mMidBot;

			}
	}

	/// -----------------------------------------------------------------------
	///	DRAW TRIANGLE ALGORITHM IMPLEMENTATIONS
	/// 
	/// 
	/// enum	EDrawLineMethod
	///	\brief	Specifies which method should be used when drawing a line. 

	void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		switch (CurrentTriangleMethod)
		{
		case EDrawTriangleMethod::eDT_BILINEAR:
			DrawTriangleBiLinear(v0, v1, v2);
			break;
		case EDrawTriangleMethod::eDT_PLANE_NORMAL:

			break;
		case EDrawTriangleMethod::eDT_BARYCENTRIC:
			break;
		default:
			break;
		}
	}

	void DrawTriangleBiLinear(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		Ve
		const AEVec2* vTop = &v0.mPosition;
		const AEVec2* vMid = &v1.mPosition;
		const AEVec2* vBot = &v2.mPosition;

		if (vTop->y < vMid->y) std::swap(v0, v1);
		if (vMid->y < vBot->y) std::swap(v1, v2);
		if (vTop->y < vMid->y) std::swap(v0, v1);

		AEVec2 vTopBot = (*vTop) - (*vBot);
		AEVec2 vTopMid = (*vTop) - (*vMid);
		float dotproduct = vTopBot.CrossMag(vTopMid);
		bool midIsLeft = dotproduct < 0;
		float mTopMid = CalSlopeInv(vMid, vTop);
		float mTopBot = CalSlopeInv(vBot, vTop);
		float mMidBot = CalSlopeInv(vBot, vMid);

		Color cStepmTopMid = (v1.mColor- v0.mColor);
		Color cStepmTopMid = CalSlopeInv(v1.mColor, v0.mColor);
		Color cStepmTopMid = CalSlopeInv(v1.mColor, v0.mColor);

		float colorstep = 
		float xL = Round(vTop->x);
		float xR = Round(vTop->x);

		int y = 0;
		int x = 0;
		for (y = Ceiling(vTop->y); y >= Ceiling(vMid->y) + 1; --y)
		{
			for (x = Round(xL); x <= Round(xR) - 1; ++x)
			{
				FrameBuffer::SetPixel(x, y, c);
			}

			xL -= midIsLeft ? mTopMid : mTopBot;
			xR -= midIsLeft ? mTopBot : mTopMid;
		}

		if (midIsLeft)
			xL = Round(vMid->x);
		else
			xR = Round(vMid->x);

		for (; y >= Ceiling(vBot->y) + 1; --y)
		{
			for (x = Round(xL); x <= Round(xR) - 1; ++x)
			{
				FrameBuffer::SetPixel(x, y, c);
			}
			xL -= midIsLeft ? mMidBot : mTopBot;
			xR -= midIsLeft ? mTopBot : mMidBot;

		}

	}

	EDrawTriangleMethod GetDrawTriangleMethod() {
		return CurrentTriangleMethod;
	}

	void SetDrawTriangleMethod(EDrawTriangleMethod triangleMethod) {
		CurrentTriangleMethod = triangleMethod;
	}
}