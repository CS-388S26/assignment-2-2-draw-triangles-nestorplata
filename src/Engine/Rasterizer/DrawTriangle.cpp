#include <AEEngine.h>
#include "Rasterizer.h"


namespace Rasterizer
{
	EDrawTriangleMethod CurrentTriangleMethod;
	namespace TriangleHelperFunctions
	{
		float m = 0;
		float dx = 0, dy = 0;
		// screen space;  when initialized again as float, 
		//they'll be real space
		int x, y;
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

		float AEVec3dotProduct(AEVec3 const v0, AEVec3 const v1)
		{
			return float(v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);

		}

		AEVec3 AEVec3FromV2(AEVec2 const v0, float z)
		{
			return AEVec3(v0.x, v0.y, z);
		}

		AEVec2 AEVec2FromV3(AEVec3 const v0)
		{
			return AEVec2(v0.x, v0.y);
		}
		float CrossMag(float x0, float x1, AEVec2 c)
		{
			return CrossMag(x0, c.x, x1, c.y);
		}
		float CrossMag(float x0, float r0, float x1, float r1)
		{

			return x0 * r1 - x1 * r0;

		}
		AEVec2 CalNormalAandB(AEVec2 const v0, AEVec2 const v1, float const c0, float const c1)
		{

			return AEVec2(CrossMag(v0.x, c0, v1.x, c1), CrossMag(v0.y, c0, v1.y, c1));
		}

		AEVec3 CalNormal(AEVec2 const v0, AEVec2 const v1, float const c0, float const c1)
		{
			return AEVec3(v0.x, v0.y, c0).Cross(AEVec3(v1.x, v1.y, c1)).Normalize();
		}
		AEVec3 CalD(AEVec3 const n, AEVec2 const v, float c)
		{
			return -AEVec3dotProduct(AEVec3(v0.x, v0.y, c0).Cross(AEVec3(v1.x, v1.y, c1)).Normalize();
		}

		AEVec3()

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
			DrawTrianglePlaneNormal(v0, v2, v2);
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

	void DrawPlaneNormalNaive(const Vertex& v0, const Vertex& v1, const Vertex& v2)
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
		float mTopBot = CalSlopeInv(vBot->mPosition, vTop->mPosition);
		float mMidBot = CalSlopeInv(vBot->mPosition, vMid->mPosition);
		float xL, xR; xL = xR = Round(vTop->mPosition.x);

		// c == dotproduct is the crossmag of vTopBot and vTopMid;
		AEVec3 nR = AEVec3FromV2(CalNormalAandB(vTopBot.mPosition, vTopMid.mPosition, vTopBot.mColor.r, vTopMid.mColor.r),
							dotproduct); 

		nR = CalNormal(vTopBot.mPosition, vTopMid.mPosition, vTopBot.mColor.r, vTopMid.mColor.r);
		
		float dR = ;

		
		return
		
			, dG = ..., dB = ..., dA = ...;
		for (yT->yM) { // Traversal Loop
			for (xL->xR) {
				Color c;
				c.r = -(nR.x * x + nR.y * y + dR) / dotproduct;
				c.G = ....;
				c.B = ....;
				c.A = ....;
				SetPixel(x, y, c);
			}// end of scan line
			xL -= slopeLeft; xR -= slopeRight;
		}// end of triangle traversal
	}
	}

	}

	EDrawTriangleMethod GetDrawTriangleMethod() {
		return CurrentTriangleMethod;
	}

	void SetDrawTriangleMethod(EDrawTriangleMethod triangleMethod) {
		CurrentTriangleMethod = triangleMethod;
	}
}