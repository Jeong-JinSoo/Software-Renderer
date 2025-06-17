#include "RenderObject.h"
#include "Camera.h"

RenderObject::RenderObject()
	:pcam(nullptr)
{

}

RenderObject::~RenderObject()
{

}

void RenderObject::Initialize()
{
	int vert_num = vertecies.size();
	prevClipW.resize(vert_num);
	finalVertex.resize(vert_num);

	// 라인 렌더링의 경우엔 이 과정을 패스한다.
	if (type == RenderType::Face)
	{
		// 노말 계산
		// 각 삼ㄱ가형 마다 면 노말을 계산해서 3개의 정점에 더해주기
		for (int i = 0; i < indices.size(); i += 3)
		{
			// 인덱스로 부터 삼각형의 정점(의 인덱스) 추출
			int i0 = indices[i + 0];
			int i1 = indices[i + 1];
			int i2 = indices[i + 2];

			// 정점의 인덱스로부터 정점 추출
			Point A = vertecies[i0].vPoint;
			Point B = vertecies[i1].vPoint;
			Point C = vertecies[i2].vPoint;

			//두 변의 벡터를 구한다.
			Point e1 = B - A;
			Point e2 = C - A;

			// 외적으로 노말 벡터를 구한다 -> 단위 벡터로 정규화
			// 플렛 셰이딩용 노말을 얻을 수 있다.
			Point faceN = Point::Normalize(Point::Cross(e1, e2));

			vertecies[i0].vNormal += faceN;
			vertecies[i1].vNormal += faceN;
			vertecies[i2].vNormal += faceN;
		}

		// 모든 정점을 노말 단위 벡터로 정규화
		for (int i = 0; i < vert_num; i++)
		{
			vertecies[i].vNormal = Point::Normalize(vertecies[i].vNormal);
		}
	}

	// 트랜스폼 초기화
	transform.SetPosition(0, 0, 0);
	transform.SetRotation(0, 0, 0);
	transform.SetScale(1, 1, 1);
}

void RenderObject::Finalize()
{
}

void RenderObject::Update()
{
	Matrix camView = pcam->GetView();
	Matrix camProj = pcam->GetProj();
	Matrix viewport = pcam->GetViewPort();

	Matrix result;

	transform.Update();
	finalTransform = transform.GetTransformMatrix() * camView * camProj;

	// 노말 연산
	if (type == RenderType::Face)
	{

		// 모델과 뷰 합성
		Matrix modelView = transform.GetTransformMatrix() * camView;
		// 모델 뷰 역행 구하기
		modelView.Inverse();

		normalMatrix = modelView.Transposed();
	}

	int vertindex = vertecies.size();
	for (int i = 0 ; i < vertindex ; i++ )
	{
		finalVertex[i] = UpdateVertex(i);
	}
}

float RenderObject::GetPrevClipW(int index) const
{
	return prevClipW[index];
}

std::vector<float> RenderObject::GetPrevClipW() const
{
	return prevClipW;
}

Vertex RenderObject::UpdateVertex(int index)
{
	Vertex result;
	Point ver_result;
	Point nor_result = vertecies[index].vNormal;

	result.color = vertecies[index].color;

	/// 정점변환
	if (index == -1)
	{
		// Wt * Vt * Pt 연산 결과
		ver_result = ver_result * finalTransform;
	}

	else
	{
		ver_result = vertecies[index].vPoint * finalTransform;
		prevClipW[index] = ver_result.w;
	}

	// 투영나누기 (Perspective Divide)
	ver_result.x /= ver_result.w;
	ver_result.y /= ver_result.w;
	ver_result.z /= ver_result.w;
	ver_result.w /= ver_result.w;

	// 뷰포트 곱하기
	ver_result = ver_result * pcam->GetViewPort();

	/// 노말 변환
	// (inverse - tranpose -> 정규화)
	//normalMatrix = transpose(inverse(worldviewMatrix))
	nor_result = nor_result * normalMatrix;
	nor_result.Normalize(nor_result);

	result.vPoint = ver_result;
	result.vNormal = nor_result;

	return result;
}

int RenderObject::ReturnIndex(int i) const
{
	return indices[i];
}

int RenderObject::GetTriangleCount() const
{
	int result = indices.size() / 3;
	return result;
}
