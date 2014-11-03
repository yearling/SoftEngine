#ifndef __LIGHTEFFECT_H__
#define __LIGHTEFFECT_H__
#include "stdafx.h"
#include "device.h"
#include "Parameter.h"
namespace SoftEngine
{

	class LightEffect :public IEffect
	{
	public:
		LightEffect(void);
		virtual ~LightEffect(void);
		virtual void Draw(int base_vertex_index,UINT start_index,UINT primitiveCount);
		virtual void SetData(void *p);
		SeftData* GetData() {return m_pSeftData;}
	protected:
		void LightEffect::FillPipline(int base_vertex_index,UINT start_index,UINT primitiveCount);
		void VSandPS(RenderVertex &v0,RenderVertex &v1,RenderVertex &v2);
		void VSandPS(RenderVertex* v[3]);
	private:
		std::vector<RenderVertex> m_vecRenderBuffer;
		std::vector<UINT> m_vecIndexBuffer;
		SeftData *m_pSeftData;
	};

}
#endif