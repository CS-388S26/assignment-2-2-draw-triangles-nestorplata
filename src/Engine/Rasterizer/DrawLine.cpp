#include <AEEngine.h>
#include "Rasterizer.h"


namespace Rasterizer
{
	/// -----------------------------------------------------------------------
	///	LINE ALGORITHM IMPLEMENTATIONS
	/// 
	/// 
	/// enum	EDrawLineMethod
	///	\brief	Specifies which method should be used when drawing a line. 
	EDrawLineMethod currentDrawLine = eDL_NAIVE;

	//General method
	namespace HelperFunctions
	{
		float m = 0, b = 0;
		unsigned int y = 0, x = 0;
		int dx = 0, dy = 0;
		unsigned int sp= 0, ep = 0;  //starting, endpoint
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
	using namespace HelperFunctions;

	//calculate rate of change

	void DrawHorizontalLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		setStartingPoint(p1);
		Interpolate(&x, Round(p2.x), c);
	}

	void DrawVerticalLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		setStartingPoint(p1);
		Interpolate(&y, Round(p2.y), c);

	}


	//Based off "drawing a line from any slop" section from the book:
	//Computer Graphics from Scratch: A Programmer's Introduction to 3D Rendering by Gabriel Gambetta
	void DrawDiagonalLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		setStartingPoint(p1);
		CalStep(x, Round(p2.x));
		for (; x != ep; x += step)
		{
			FrameBuffer::SetPixel(x, y, c);
			y++;
		}
	}

	void DrawLine(const AEVec2& p1, const Color& c1, const AEVec2& p2, const Color& c2)
	{
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		m = dy / dx;
		float mInv = 1 / m;
		Color cStep, c;
		if (abs(m) < 1)
		{
			step = dx > 0 ? 1 : -1;
			m *= step;
			ep = Round(p2.x) + step;
			cStep = (c2 - c1) / abs(dx);
			float y = p1.y;
			for (x = Round(p1.x); x != ep; x += step)
			{
				FrameBuffer::SetPixel(x, Round(y + 0.5), c);
				c += cStep;
				y += m;
			}
		}
		else // abs(m)>1
		{
			step = dy > 0 ? 1 : -1;
			mInv *= step;
			ep = Round(p2.y) + step;
			cStep = (c2 - c1) / abs(dy);

			float x = p1.x;
			for (y = Round(p1.y); y != ep; y += step)
			{
				FrameBuffer::SetPixel(Round(x + 0.5), y, c);
				c += cStep;
				x += mInv;
			}
		}

	}

	//starting from buttonleft, the function draws 2 lines to the oposite corner
	//It would then draw the remaining lines
	AEVec2 p0, p1, VecY, VecX;
	void DrawRect(const AEVec2& r, const AEVec2& size, const Color& c) {

		p0 = AEVec2(r.x - size.x / 2, r.y - size.y / 2);
		p1 = p0 + size;
		VecY = AEVec2(0, size.y);
		VecX = AEVec2(size.x, 0);
		for (int i = 0; i < 2; i++)
		{
			DrawHorizontalLine(p0 + VecY * i, p1, c);
			DrawVerticalLine(p0 + VecX * i, p1, c);
		}
	}


	//You will have to implement the three methods.DrawLine should call the correspondent line scan
	//	conversion function depending on the method selected(DDA or Naïve) and the type of line(horizontal,
	//		vertical, diagonal or general).
	void DrawLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {

		CalDeltas(p1, p2);
		if (dy == 0)
		{
			DrawHorizontalLine(p1, p2, c);
		}
		else if (dx == 0)
		{
			DrawVerticalLine(p1, p2, c);
		}
		else if (dx == dy)
		{
			DrawDiagonalLine(p1, p2, c);
		}
		else
		{
			switch (GetDrawLineMethod())
			{
			case eDL_NAIVE:
				DrawLineNaive(p1, p2, c);
				break;
			case eDL_DDA:
				DrawLineDDA(p1, p2, c);
				break;
			case eDL_BRESENHAM:
				DrawLineBresenham(p1, p2, c);
				break;
			}
		}
	}

	void DrawLineNaive(const AEVec2& p1, const AEVec2& p2, const Color& c) {

		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		m = dy / dx;
		b = -m * p1.x + p1.y;

		setStartingPoint(p1);
		if (abs(m) < 1)
		{
			step = dx > 0 ? 1 : -1; // Step: going to the left or right
			ep = Round(p2.x) + step;

			for (; x != ep; x += step)
			{
				FrameBuffer::SetPixel(x, y, c);
				y = Round(m * x + b + 0.5);

			}
		}
		else
		{
			step = dy > 0 ? 1 : -1; // Step: going to the left or right
			ep = Round(p2.y) + step;


			for (; y != ep; y += step)
			{
				FrameBuffer::SetPixel(x, y, c);
				x = Round((y - b) / m + 0.5);
			}
		}
	}

	void DrawLineDDA(const AEVec2& p1, const AEVec2& p2, const Color& c)
	{
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		m = dy / dx;
		float mInv = 1 / m;
		if (abs(m) < 1)
		{
			step = dx > 0 ? 1 : -1;
			m *= step;
			ep = Round(p2.x) + step;
			float y = p1.y;
			for (x = Round(p1.x); x != ep; x += step)
			{
				FrameBuffer::SetPixel(x, Round(y + 0.5), c);
				y += m;
			}
		}
		else // abs(m)>1
		{
			step = dy > 0 ? 1 : -1;
			mInv *= step;

			ep = Round(p2.y) + step;
			float x = p1.x;
			for (y = Round(p1.y); y != ep; y += step)
			{
				FrameBuffer::SetPixel(Round(x + 0.5), y, c);
				x += mInv;
			}
		}
	}

	void DrawLineBresenham(const AEVec2& p1, const AEVec2& p2, const Color& c)
	{
		dx = Round(p2.x - p1.x);
		dy = Round(p2.y - p1.y);
		int stepX = dx > 0 ? 1 : -1;
		int stepY = dy > 0 ? 1 : -1;

		dx = abs(dx);
		dy = abs(dy);


		x = Round(p1.x);
		y = Round(p1.y);

		if (dx > dy)
		{
			int dp = 2 * dy - dx;

			ep = Round(p2.x) + stepX;

			for (; x != ep;)
			{
				FrameBuffer::SetPixel(x, y, c);
				if (dp > 0)
				{
					y += stepY; x += stepX;
					dp += 2 * dy - 2 * dx;
				}
				else
				{
					x += stepX;
					dp += 2 * dy;
				}
			}
		}
		else
		{
			int dp = 2 * dx - dy;

			ep = Round(p2.y) + stepY;

			for (; y != ep;)
			{
				FrameBuffer::SetPixel(x, y, c);
				if (dp > 0)
				{
					y += stepY; x += stepX;
					dp += 2 * dx - 2 * dy;
				}
				else
				{
					y += stepY;
					dp += 2 * dx;
				}
			}
		}

	}

	/// @TODO
	// ------------------------------------------------------------------------
	/// \fn	GetDrawLineMethod
	/// \brief	Return the current draw line method.
	EDrawLineMethod GetDrawLineMethod() {
		return currentDrawLine;
	}

	/// @TODO
	// ------------------------------------------------------------------------
	/// \fn	GetDrawLineMethod
	/// \brief	Set the current draw line method to that given as input.
	void SetDrawLineMethod(EDrawLineMethod lineMethod) {
		currentDrawLine = lineMethod;
	}
}