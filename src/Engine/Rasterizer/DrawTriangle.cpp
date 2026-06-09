#include <AEEngine.h>
#include "Rasterizer.h"
namespace Rasterizer
{
	EDrawTriangleMethod CurrentTriangleMethod;
	namespace TriangleHelperFunctions
	{
		float m = 0;
		unsigned int y = 0, x = 0;
		float dx = 0, dy = 0;
		unsigned int sP = 0 , eP = 0;  //startingPoint, endPoint
		int step = 0;
		float CalSlopeInv(const AEVec2  p0, const AEVec2 p1)
		{
			dy = (p0.y - p1.y);
			dx = (p0.x - p1.x);
			if (Round(dy))
			{
				return  dx / dy;
			}
			else
			{
				return dy / dx;
			}
		}
		float CalSlopeInv(const AEVec2* p0, const AEVec2* p1)
		{
			return CalSlopeInv(*p0, *p1);
		}



		Color CalColorStep(const Color c0, const Color c1)
		{
			if (Round(dy)) { return (c0 - c1) / abs(dy); }
			return c0;

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
	Vertex operator - (Vertex own, const Vertex& other) 
	{
		Vertex result;  
		result.mColor = own.mColor - other.mColor;
		result.mPosition = own.mPosition - other.mPosition;
		return result;
	}
	void DrawTriangleBiLinear(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		const Vertex* vTop = &v0;
		const Vertex* vMid = &v1;
		const Vertex* vBot = &v2;



		if (vTop->mPosition.y < vMid->mPosition.y) std::swap(vTop, vMid);
		if (vMid->mPosition.y < vBot->mPosition.y) std::swap(vMid, vBot);
		if (vTop->mPosition.y < vMid->mPosition.y) std::swap(vTop, vMid);

		Vertex vTopBot = (*vTop) - (*vBot);
		Vertex vTopMid = (*vTop) - (*vMid);
		float dotproduct = vTopBot.mPosition.CrossMag(vTopMid.mPosition);
		bool midIsLeft = dotproduct < 0;

		
		float mTopMid = CalSlopeInv(vMid->mPosition, vTop->mPosition);
		Color cTopMidStep =  CalColorStep(vMid->mColor, vTop->mColor);

		float mTopBot = CalSlopeInv(vBot->mPosition, vTop->mPosition);
		Color cTopBotStep =  CalColorStep(vBot->mColor, vTop->mColor);
		
		float mMidBot = CalSlopeInv(vBot->mPosition, vMid->mPosition);
		Color cMidBotStep = CalColorStep(vBot->mColor, vMid->mColor);

		float xL, xR;xL =xR = Round(vTop->mPosition.x);
		Color cL, cR, cStep, c; cL = cR = vTop->mColor;

		for (y = Ceiling(vTop->mPosition.y); y >= Ceiling(vMid->mPosition.y) + 1; --y)
		{
			c = cL;
			cStep = (cR - cL) / abs(xR - xL);
			for (x = Round(xL); x <= Round(xR) - 1; ++x)
			{
				FrameBuffer::SetPixel(x, y, c);
				c += cStep;
			}
			cL += midIsLeft ? cTopMidStep : cTopBotStep;
			cR += midIsLeft ? cTopBotStep : cTopMidStep;

			xL -= midIsLeft ? mTopMid : mTopBot;
			xR -= midIsLeft ? mTopBot : mTopMid;
		}

		if (midIsLeft)
		{
			xL = Round(vMid->mPosition.x);
			cL = vMid->mColor;
		}
		else
		{
			xR = Round(vMid->mPosition.x);
			cR = vMid->mColor;
		}

		for (; y >= Round(vBot->mPosition.y) ; --y)
		{
			c = cL;
			cStep = (cR - cL) / abs(xR - xL);
			for (x = Round(xL); x <= Round(xR); ++x)
			{
				FrameBuffer::SetPixel(x, y, c);
				c += cStep;

			}
			cL += midIsLeft ? cMidBotStep : cTopBotStep;
			cR += midIsLeft ? cTopBotStep : cMidBotStep;
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